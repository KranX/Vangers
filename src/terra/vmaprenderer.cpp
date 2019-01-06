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
// #include <hdf5_hl.h>
#include "vmaprenderer.h"

#include "vgl/vertexarray_ext.h"

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
	updateBuffer = vgl::PixelUnpackBuffer::create(bufferSize, vgl::BufferUsage::StreamDraw);
	updateBuffer->unbind();

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
	paletteTexture->subImage(vgl::TextureFormat::RGBA, vgl::TextureDataType::UnsignedInt8888, (GLvoid*)data);
}

void VMapRenderer::updateColor(uint8_t **color, int lineUp, int lineDown) {
	// uint8_t* b = new uint8_t[sizeX * DIRTY_REGION_CHUNK_SIZE];

	for(int nRegion = 0; nRegion < dirtyRegions.size(); nRegion++){
		if(dirtyRegions[nRegion]){
//			std::cout<<"dirtyRegion: "<<nRegion<<std::endl;
			int yStart = nRegion * DIRTY_REGION_CHUNK_SIZE;
			int yEnd = (nRegion + 1) * DIRTY_REGION_CHUNK_SIZE;

			for(int i = yStart; i < yEnd; i++){
				uint8_t* lineBuffer = buffer + ((i - yStart) * sizeX);
				if(color[i]){
					memcpy(lineBuffer, color[i], sizeX * sizeof(uint8_t));
				}else{
					memset(lineBuffer, 0, sizeX * sizeof(uint8_t));
				}
			}
			int nLayer = yStart / DIRTY_REGION_CHUNK_SIZE;
			int y = yStart % DIRTY_REGION_CHUNK_SIZE;

			updateBuffer->bind();
			updateBuffer->update(buffer);
			colorTexture->subImage(
					{0, y, nLayer},
					{sizeX, DIRTY_REGION_CHUNK_SIZE, 1},
					vgl::TextureFormat::RedInteger,
					vgl::TextureDataType::UnsignedByte,
					// buffer
					*(updateBuffer)
			);
			updateBuffer->unbind();
		}
	}
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
	updateBuffer->free();
	heightTexture->free();
	process->free();
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

	float numLayers = colorTexture->getDimensions().z;
	float width = colorTexture->getDimensions().x;
	float height = colorTexture->getDimensions().y * numLayers;
	textureScale = glm::vec4(width, height, scale, numLayers);


	pipeline = vgl::Pipeline::create(
			shadersPath,
			vgl::createQuad<vgl::Pos2Tex2Vertex>(),
			{
				{"t_Height", heightMapTexture},
				{"t_Color", colorTexture},
				{"t_Palette", paletteTexture},
				{"t_Meta", metaTexture},
			});
}

void RayCastProcess::render(const vgl::Camera &camera) {
	auto mvp = camera.mvp();
	auto invMvp = glm::inverse(mvp);

	scale = std::fminf(scale + 1, maxScale);
	textureScale.z = scale;
	pipeline->useShader();
	pipeline->setUniform<glm::mat4>("u_ViewProj", mvp);
	pipeline->setUniform<glm::mat4>("u_InvViewProj", invMvp);
	pipeline->setUniform<glm::vec4>("u_CamPos", glm::vec4(camera.position, 0));
	pipeline->setUniform<glm::vec4>("u_TextureScale", textureScale);
	pipeline->setUniform<glm::vec4>("u_ScreenSize", glm::vec4(camera.viewport.x, camera.viewport.y, 0.0f, 0.0f));
	pipeline->render();
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

	pipeline = vgl::Pipeline::create(
			shadersPath,
			vgl::createQuad<vgl::Pos2Tex2Vertex>(),
//			vertexArray,
			{
				{"t_Color", colorTexture},
				{"t_Palette", paletteTexture},
			});

}

void BilinearFilteringProcess::render(const vgl::Camera &camera) {
	pipeline->useShader();
	pipeline->setUniform("u_ViewProj", camera.mvp());
	pipeline->setUniform("u_TextureScale", textureScale);

	pipeline->render();
}

void BilinearFilteringProcess::free() {
	pipeline->free();
}

void RayCastProcess::free(){
	pipeline->free();
}