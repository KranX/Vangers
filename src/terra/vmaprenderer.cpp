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


void VMapRenderer::init(const std::shared_ptr<gl::Texture> &heightMapTexture,
		          const std::shared_ptr<gl::Texture> &colorTexture,
		          const std::shared_ptr<gl::Texture> &metaTexture,
		          const std::shared_ptr<gl::Texture> &paletteTexture){
	resetDirty();

	camera = std::make_shared<gl::Camera>();
//	shader = std::make_unique<BilinearFilteringShader>(
//			paletteTexture,
//			colorTexture,
//			camera,
//			shaderPath
//	);
	shader = std::make_unique<RayCastShader>(camera,
	                                      heightMapTexture,
	                                      colorTexture,
	                                      metaTexture,
	                                      paletteTexture,
	                                      shaderPath);
	this->colorTexture = colorTexture;
	this->paletteTexture = paletteTexture;
	this->heightTexture = heightMapTexture;
	int bufferSize = DIRTY_REGION_CHUNK_SIZE * sizeX;
	buffer = new uint8_t[bufferSize];
}

void VMapRenderer::render(int viewPortWidth, int viewPortHeight, int x, int y, int z, float turn, float slope) {
	camera->viewport = glm::vec2(viewPortWidth, viewPortHeight);
	camera->from_player(
			x, y, z, turn, slope
	);
	shader->render();
	resetDirty();
}

void VMapRenderer::setPalette(SDL_Palette *sdlPalette, SDL_PixelFormat *format) {
	uint32_t data[256];

	for(int i = 0; i < 256; i++){
		auto color = sdlPalette->colors[i];
		data[i] = SDL_MapRGBA(format, color.r, color.g, color.b, 255);
	}
	paletteTexture->update(0, 0, 256, 0, data);
}

void VMapRenderer::updateColor(uint8_t **color, int lineUp, int lineDown) {
//	if(lineUp <= lineDown){
//		setDirty(lineUp, lineDown);
//	}else{
//		setDirty(0, lineDown);
//		setDirty(lineUp, sizeY - 1);
//	}

//	std::cout<<"VMapRenderer::updateColor"<<std::endl;
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

			colorTexture->update(0, yStart, sizeX, DIRTY_REGION_CHUNK_SIZE, b);
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
	colorTexture->release();
	paletteTexture->release();
	delete[] buffer;
}

void RayCastShader::render_impl() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, data.heightMapTexture->textureId);
	glUniform1i(data.heightMapTextureAttrId, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, data.colorTexture->textureId);
	glUniform1i(data.colorTextureAttrId, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, data.paletteTexture->textureId);
	glUniform1i(data.paletteTextureAttrId, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, data.metaTexture->textureId);
	glUniform1i(data.metaTextureAttrId, 3);


	auto mvp = data.camera->mvp();
	auto invMvp = glm::inverse(mvp);
	float numLayers = data.colorTexture->numLayers;
	float width = data.colorTexture->width;
	float height = data.colorTexture->height * numLayers;

	scale = fmin(scale + 2.0f, maxScale);


	glUniformMatrix4fv(data.mvpAtrrId, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(data.invMvpAttrId, 1, GL_FALSE, &invMvp[0][0]);
	glUniform4fv(data.cameraPosAttrId, 1, &data.camera->position[0]);

	auto textureScale = glm::vec4(width, height, scale, numLayers);

	auto screenSize = glm::vec4(data.camera->viewport.x, data.camera->viewport.y, 0.0f, 0.0f);
	glUniform4fv(data.textureScaleAttrId, 1, &textureScale[0]);
	glUniform4fv(data.screenSizeAttrId, 1, &screenSize[0]);

	glBindVertexArray(data.bufferData->vertexArrayObject);

	glEnableVertexAttribArray(data.positionAttrId);
	glBindBuffer(GL_ARRAY_BUFFER, data.bufferData->vertexBufferObject);
	glVertexAttribPointer(data.positionAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

//	glEnableVertexAttribArray(data.texcoordAttrId);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.bufferData->elementBufferObject);
//	glVertexAttribPointer(data.texcoordAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

RayCastShader::RayCastShader(float maxScale,
							 const std::shared_ptr<gl::Camera> &camera,
                             const std::shared_ptr<gl::Texture> &heightMapTexture,
                             const std::shared_ptr<gl::Texture> &colorTexture,
                             const std::shared_ptr<gl::Texture> &metaTexture,
                             const std::shared_ptr<gl::Texture> &paletteTexture,
                             const std::string &shadersPath) : scale(1.0f), maxScale(maxScale), Shader(shadersPath){
	int heightMultiplier = heightMapTexture->numLayers == 0 ? 1 : heightMapTexture->numLayers;

    float w = heightMapTexture->width;
	float h = heightMapTexture->height * heightMultiplier;

	GLfloat vertices[] = {
//			  Position   , Texcoords
			-w,  -h, -1.0f, -1.0f, // Top-left
			2 * w,  -h, 2.0f, -1.0f, // Top-right
			2 * w, 2 * h,  2.0f, 2.0f, // Bottom-right
			-w, 2 * h,  -1.0f, 2.0f  // Bottom-left
	};

	GLuint elements[] = {
			0, 1, 2,
			3, 2, 0,
	};

	auto buffer = gl::gen_buffer(vertices, elements, sizeof(vertices), sizeof(elements));
	glUseProgram(programId);

	data.bufferData = buffer;
	data.heightMapTexture = heightMapTexture;
	data.colorTexture = colorTexture;
	data.metaTexture = metaTexture;
	data.paletteTexture = paletteTexture;
	data.camera = camera;

	data.heightMapTextureAttrId = glGetUniformLocation(programId, "t_Height");
	data.colorTextureAttrId = glGetUniformLocation(programId, "t_Color");
	data.paletteTextureAttrId = glGetUniformLocation(programId, "t_Palette");
	data.metaTextureAttrId = glGetUniformLocation(programId, "t_Meta");
	data.mvpAtrrId = glGetUniformLocation(programId, "u_ViewProj");
	data.invMvpAttrId  = glGetUniformLocation(programId, "u_InvViewProj");
	data.cameraPosAttrId  = glGetUniformLocation(programId, "u_CamPos");
	data.textureScaleAttrId  = glGetUniformLocation(programId, "u_TextureScale");
	data.screenSizeAttrId  = glGetUniformLocation(programId, "u_ScreenSize");

	data.positionAttrId = glGetAttribLocation(programId, "position");
//	data.texcoordAttrId = glGetAttribLocation(programId, "texcoord");
}



void BilinearFilteringShader::render_impl() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, data.colorTexture->textureId);
	glUniform1i(data.colorTextureAttrId, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, data.paletteTexture->textureId);
	glUniform1i(data.paletteTextureAttrId, 1);


	auto mvp = data.camera->mvp();
	glUniformMatrix4fv(data.mvpAtrrId, 1, GL_FALSE, &mvp[0][0]);

	float numLayers = data.colorTexture->numLayers;
	float width = data.colorTexture->width;
	float height = data.colorTexture->height * numLayers;

	auto textureScale = glm::vec4(width, height, 0.0f, numLayers);
	auto screenSize = glm::vec4(data.camera->viewport.x, data.camera->viewport.y, 0.0f, 0.0f);
	glUniform4fv(data.textureScaleAttrId, 1, &textureScale[0]);
	glUniform4fv(data.screenSizeAttrId, 1, &screenSize[0]);

	glBindVertexArray(data.bufferData->vertexArrayObject);

	glEnableVertexAttribArray(data.positionAttrId);
	glBindBuffer(GL_ARRAY_BUFFER, data.bufferData->vertexBufferObject);
	glVertexAttribPointer(data.positionAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(data.texcoordAttrId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.bufferData->elementBufferObject);
	glVertexAttribPointer(data.texcoordAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	gl::check_GL_error("glDrawElements");
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

BilinearFilteringShader::BilinearFilteringShader(const std::shared_ptr<gl::Texture> &paletteTexture,
                                                 const std::shared_ptr<gl::Texture> &colorTexture,
                                                 const std::shared_ptr<gl::Camera> &camera,
                                                 const std::string &shadersPath): Shader(shadersPath) {
    int heightMultiplier = colorTexture->numLayers == 0 ? 1 : colorTexture->numLayers;

    float w = colorTexture->width;
    float h = colorTexture->height * heightMultiplier;

	GLfloat vertices[] = {
//			  Position   , Texcoords
			-w,  -h, 2.0f, -1.0f, // Top-left
			2 * w,  -h, -1.0f, -1.0f, // Top-right
			2 * w, 2 * h,  -1.0f, 2.0f, // Bottom-right
			-w, 2 * h,  2.0f, 2.0f  // Bottom-left
	};

	GLuint elements[] = {
			0, 1, 2,
			3, 2, 0,
	};

	auto buffer = gl::gen_buffer(vertices, elements, sizeof(vertices), sizeof(elements));
	glUseProgram(programId);

	data.bufferData = buffer;
	data.colorTexture = colorTexture;
	data.paletteTexture = paletteTexture;
	data.camera = camera;

	data.colorTextureAttrId = glGetUniformLocation(programId, "t_Color");
	data.paletteTextureAttrId = glGetUniformLocation(programId, "t_Palette");
	data.mvpAtrrId = glGetUniformLocation(programId, "u_ViewProj");

	data.positionAttrId = glGetAttribLocation(programId, "position");
	data.texcoordAttrId = glGetAttribLocation(programId, "texcoord");

	data.textureScaleAttrId  = glGetUniformLocation(programId, "u_TextureScale");
	data.screenSizeAttrId  = glGetUniformLocation(programId, "u_ScreenSize");

}
