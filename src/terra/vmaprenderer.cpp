//
// Created by nikita on 2/13/18.
//

#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <ios>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <hdf5_hl.h>
#include "vmaprenderer.h"


void VMapRenderer::init(const std::shared_ptr<vgl::Texture2DArray> &heightMapTexture,
                        const std::shared_ptr<vgl::Texture2DArray> &colorTexture,
                        const std::shared_ptr<vgl::Texture2DArray> &metaTexture,
                        const std::shared_ptr<vgl::Texture1D> &paletteTexture){
	resetDirty();

	camera = std::make_shared<vgl::Camera>();
	switch (shaderType){
		case RayCast:
			process = std::make_shared<RayCastProcess>(
									  MAX_SCALE,
									  heightMapTexture,
									  colorTexture,
									  metaTexture,
									  paletteTexture,
									  "shaders/3dmap");
			break;
		case BilinearFiltering:
			process = std::make_shared<BilinearFilteringProcess>(
					paletteTexture,
					colorTexture,
					"shaders/heightmap"
			);
			break;
	}

	this->colorTexture = colorTexture;
	this->paletteTexture = paletteTexture;
	this->heightTexture = heightMapTexture;

	int bufferSize = DIRTY_REGION_CHUNK_SIZE * sizeX;
	updateBuffers[0] = vgl::PixelUnpackBuffer::create(bufferSize, vgl::BufferUsage::StreamDraw);
	updateBuffers[0]->unbind();

	updateBuffers[1] = vgl::PixelUnpackBuffer::create(bufferSize, vgl::BufferUsage::StreamDraw);
	updateBuffers[1]->unbind();

	buffer = new uint8_t[bufferSize];
}

void
VMapRenderer::render(int viewPortWidth, int viewPortHeight, int x, int y, int z, float turn, float slope, float focus) {
	camera->focus = focus;
	camera->viewport = glm::vec2(viewPortWidth, viewPortHeight);
	camera->from_player(
			x, y, z, turn, slope
	);
	process->render(*camera);
	resetDirty();
}

void VMapRenderer::setPalette(SDL_Palette *sdlPalette, SDL_PixelFormat *format) {
	uint32_t data[256];

	for(int i = 0; i < 256; i++){
		auto color = sdlPalette->colors[i];
		data[i] = SDL_MapRGBA(format, color.r, color.g, color.b, 255);
	}
	paletteTexture->subImage(0, 256, vgl::TextureFormat::RGBA, vgl::TextureDataType::UnsignedInt8888, (GLvoid*)data);
}

void VMapRenderer::updateColor(uint8_t **color, int lineUp, int lineDown) {
	uint8_t* b = new uint8_t[sizeX * DIRTY_REGION_CHUNK_SIZE];

	for(int nRegion = 0; nRegion < dirtyRegions.size(); nRegion++){
		if(dirtyRegions[nRegion]){
//			std::cout<<"dirtyRegion: "<<nRegion<<std::endl;
			int yStart = nRegion * DIRTY_REGION_CHUNK_SIZE;
			int yEnd = (nRegion + 1) * DIRTY_REGION_CHUNK_SIZE;

			for(int i = yStart; i < yEnd; i++){
				uint8_t* lineBuffer = b + ((i - yStart) * sizeX);
				if(color[i]){
					memcpy(lineBuffer, color[i], sizeX * sizeof(uint8_t));
				}else{
					memset(lineBuffer, 0, sizeX * sizeof(uint8_t));
				}
			}
			int nLayer = yStart / DIRTY_REGION_CHUNK_SIZE;
			int y = yStart % DIRTY_REGION_CHUNK_SIZE;

//			currentUpdateBuffer = (currentUpdateBuffer + 1) % 2;
//			auto nextIndex = (currentUpdateBuffer + 1) % 2;
//
//			updateBuffers[currentUpdateBuffer]->bind();
//
//			colorTexture->subImage(
//					{0, y, nLayer},
//					{sizeX, DIRTY_REGION_CHUNK_SIZE, 1},
//					vgl::TextureFormat::RedInteger,
//					vgl::TextureDataType::UnsignedByte,
//					*(updateBuffers[currentUpdateBuffer])
//					);
//			updateBuffers[currentUpdateBuffer]->unbind();
//
//			updateBuffers[nextIndex]->bind();
//			updateBuffers[nextIndex]->update(b);
//			updateBuffers[nextIndex]->unbind();

//			currentUpdateBuffer = (currentUpdateBuffer + 1) % 2;
//			auto nextIndex = (currentUpdateBuffer + 1) % 2;

			updateBuffers[0]->bind();
			updateBuffers[0]->update(b);
			colorTexture->subImage(
					{0, y, nLayer},
					{sizeX, DIRTY_REGION_CHUNK_SIZE, 1},
					vgl::TextureFormat::RedInteger,
					vgl::TextureDataType::UnsignedByte,
					*(updateBuffers[currentUpdateBuffer])
			);
			updateBuffers[0]->unbind();
		}
	}
	delete[] b;
}

void VMapRenderer::resetDirty() {
	int numRegions = sizeY / DIRTY_REGION_CHUNK_SIZE;
	dirtyRegions = std::vector<bool>(numRegions, false);
}

void VMapRenderer::setDirty(int y) {
	int nRegion = y / DIRTY_REGION_CHUNK_SIZE;
	dirtyRegions[nRegion] = true;
}

void VMapRenderer::setDirty(int yStart, int yEnd) {
	int nRegionStart = yStart / DIRTY_REGION_CHUNK_SIZE;
	int nRegionEnd = yEnd / DIRTY_REGION_CHUNK_SIZE;
	for(int nRegion = nRegionStart; nRegion <= nRegionEnd; nRegion++){
		dirtyRegions[nRegion] = true;
	}
}

void VMapRenderer::deinit() {
	colorTexture->free();
	paletteTexture->free();
	updateBuffers[0]->free();
	updateBuffers[1]->free();
	delete[] buffer;
}

RayCastProcess::RayCastProcess(float maxScale,
                              const std::shared_ptr<vgl::Texture2DArray> &heightMapTexture,
                              const std::shared_ptr<vgl::Texture2DArray> &colorTexture,
                              const std::shared_ptr<vgl::Texture2DArray> &metaTexture,
                              const std::shared_ptr<vgl::Texture1D> &paletteTexture,
                              const std::string &shadersPath) :
                              scale(1.0f), maxScale(maxScale) {
	int heightMultiplier = colorTexture->getDimensions().z == 0 ? 1 : colorTexture->getDimensions().z;

	float w = colorTexture->getDimensions().x;
	float h = colorTexture->getDimensions().y * heightMultiplier;

	float numLayers = colorTexture->getDimensions().z;
	float width = colorTexture->getDimensions().x;
	float height = colorTexture->getDimensions().y * numLayers;
	textureScale = glm::vec4(width, height, 0.0f, numLayers);

	std::vector<Vertex> vertices = {
			{{-w,  -h}, {-1.0f, -1.0f}}, // Top-left
			{{2 * w,  -h}, {2.0f, -1.0f}}, // Top-right
			{{2 * w, 2 * h},  {2.0f, 2.0f}}, // Bottom-right
			{{-w, 2 * h},  {-1.0f, 2.0f}}  // Bottom-left
	};

	std::vector<GLuint> elements = {
			0, 1, 2,
			3, 2, 0,
	};

	vertexArray = vgl::VertexArray<Vertex, GLuint>::create(vertices, elements);
	vertexArray->addAttrib(0, glm::vec2::length(), offsetof(Vertex, pos));
	vertexArray->addAttrib(1, glm::vec2::length(), offsetof(Vertex, uv));

	shader = vgl::Shader::createFromPath(
			shadersPath+".vert",
			shadersPath+".frag"
	);
	shader->bindUniformAttribs(data);
	shader->addTexture(*heightMapTexture, "t_Height");
	shader->addTexture(*colorTexture, "t_Color");
	shader->addTexture(*paletteTexture, "t_Palette");
	shader->addTexture(*metaTexture, "t_Meta");
}

void RayCastProcess::render(const vgl::Camera &camera) {
	auto mvp = camera.mvp();
	auto invMvp = glm::inverse(mvp);

	data.u_ViewProj = mvp;
	data.u_InvViewProj = invMvp;
	data.u_CamPos = camera.position;
	data.u_TextureScale = textureScale;
	data.u_ScreenSize = glm::vec4(camera.viewport.x, camera.viewport.y, 0.0f, 0.0f);
	shader->render(data, vertexArray);
}


BilinearFilteringProcess::BilinearFilteringProcess(
		const std::shared_ptr<vgl::Texture1D>& paletteTexture,
		const std::shared_ptr<vgl::Texture2DArray>& colorTexture,
		const std::string &shadersPath
){
    int heightMultiplier = colorTexture->getDimensions().z == 0 ? 1 : colorTexture->getDimensions().z;

    float w = colorTexture->getDimensions().x;
    float h = colorTexture->getDimensions().y * heightMultiplier;

	float numLayers = colorTexture->getDimensions().z;
	float width = colorTexture->getDimensions().x;
	float height = colorTexture->getDimensions().y * numLayers;
	textureScale = glm::vec4(width, height, 0.0f, numLayers);

	std::vector<Vertex> vertices = {
		{{-w,  -h}, {2.0f, -1.0f}}, // Top-left
		{{2 * w,  -h}, {-1.0f, -1.0f}}, // Top-right
		{{2 * w, 2 * h},  {-1.0f, 2.0f}}, // Bottom-right
		{{-w, 2 * h},  {2.0f, 2.0f}}  // Bottom-left
	};

	std::vector<GLuint> elements = {
		0, 1, 2,
		3, 2, 0,
	};

	vertexArray = vgl::VertexArray<Vertex, GLuint>::create(vertices, elements);
	vertexArray->addAttrib(0, glm::vec2::length(), offsetof(Vertex, pos));
	vertexArray->addAttrib(1, glm::vec2::length(), offsetof(Vertex, uv));

	shader = vgl::Shader::createFromPath(
			shadersPath+".vert",
			shadersPath+".frag"
			);
	shader->bindUniformAttribs(data);
	shader->addTexture(*colorTexture, "t_Color");
	shader->addTexture(*paletteTexture, "t_Palette");
}

void BilinearFilteringProcess::render(const vgl::Camera &camera) {
	data.u_ViewProj = camera.mvp();
	data.u_TextureScale = textureScale;
	data.u_ScreenSize = glm::vec4(camera.viewport.x, camera.viewport.y, 0.0f, 0.0f);
	shader->render(data, vertexArray);
}