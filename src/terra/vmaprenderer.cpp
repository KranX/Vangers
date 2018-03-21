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
#include <sstream>
#include <vector>
#include <hdf5_hl.h>
#include "vmaprenderer.h"


void VMapRenderer::init(const std::shared_ptr<gl::Texture> &heightMapTexture,
		          const std::shared_ptr<gl::Texture> &colorTexture,
		          const std::shared_ptr<gl::Texture> &metaTexture,
		          const std::shared_ptr<gl::Texture> &paletteTexture){
	camera = std::make_shared<gl::Camera>();
	shader = std::make_unique<BilinearFilteringShader>(
			paletteTexture,
			colorTexture,
			camera,
			shaderPath
	);
//	shader = std::make_unique<RayCastShader>(camera,
//	                                      heightMapTexture,
//	                                      colorTexture,
//	                                      metaTexture,
//	                                      paletteTexture,
//	                                      shaderPath);
	this->colorTexture = colorTexture;
	this->paletteTexture = paletteTexture;
}

void VMapRenderer::render(int viewPortWidth, int viewPortHeight, int x, int y, int z, float turn, float slope) {
	camera->viewport = glm::vec2(viewPortWidth, viewPortHeight);
	camera->from_player(
			x, y, z, turn, slope
	);
	shader->render();
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
	if(lineUp <= lineDown){
		int bufferLength = lineDown - lineUp + 1;
		int bufferSize = bufferLength * sizeX;
		uint8_t* buffer = new uint8_t[bufferSize];
		for(int i = 0; i < bufferLength; i++){
			uint8_t* lineBuffer = buffer + (i * sizeX);
			memcpy(lineBuffer, color[i + lineUp], sizeX * sizeof(uint8_t));
		}
		colorTexture->update(0, lineUp, sizeX, bufferLength, buffer);
		delete[] buffer;
	}else{
		updateColor(color, 0, lineDown);
		updateColor(color, lineUp, sizeY - 1);
	}
}


void RayCastShader::render_impl() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data.heightMapTexture->textureId);
	glUniform1i(data.heightMapTextureAttrId, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, data.colorTexture->textureId);
	glUniform1i(data.colorTextureAttrId, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, data.paletteTexture->textureId);
	glUniform1i(data.paletteTextureAttrId, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, data.metaTexture->textureId);
	glUniform1i(data.metaTextureAttrId, 3);


	auto mvp = data.camera->mvp();
	auto invMvp = glm::inverse(mvp);
	glUniformMatrix4fv(data.mvpAtrrId, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(data.invMvpAttrId, 1, GL_FALSE, &invMvp[0][0]);
	glUniform4fv(data.cameraPosAttrId, 1, &data.camera->position[0]);

	glBindVertexArray(data.bufferData->vertexArrayObject);

	glEnableVertexAttribArray(data.positionAttrId);
	glBindBuffer(GL_ARRAY_BUFFER, data.bufferData->vertexBufferObject);
	glVertexAttribPointer(data.positionAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(data.texcoordAttrId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.bufferData->elementBufferObject);
	glVertexAttribPointer(data.texcoordAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

RayCastShader::RayCastShader(const std::shared_ptr<gl::Camera> &camera,
                             const std::shared_ptr<gl::Texture> &heightMapTexture,
                             const std::shared_ptr<gl::Texture> &colorTexture,
                             const std::shared_ptr<gl::Texture> &metaTexture,
                             const std::shared_ptr<gl::Texture> &paletteTexture,
                             const std::string &shadersPath) : Shader(shadersPath){
	float w = heightMapTexture->width;
	float h = heightMapTexture->height;

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

	data.positionAttrId = glGetAttribLocation(programId, "position");
	data.texcoordAttrId = glGetAttribLocation(programId, "texcoord");
}



void BilinearFilteringShader::render_impl() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data.colorTexture->textureId);
	glUniform1i(data.colorTextureAttrId, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, data.paletteTexture->textureId);
	glUniform1i(data.paletteTextureAttrId, 1);


	auto mvp = data.camera->mvp();
	glUniformMatrix4fv(data.mvpAtrrId, 1, GL_FALSE, &mvp[0][0]);

	glBindVertexArray(data.bufferData->vertexArrayObject);

	glEnableVertexAttribArray(data.positionAttrId);
	glBindBuffer(GL_ARRAY_BUFFER, data.bufferData->vertexBufferObject);
	glVertexAttribPointer(data.positionAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(data.texcoordAttrId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.bufferData->elementBufferObject);
	glVertexAttribPointer(data.texcoordAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

BilinearFilteringShader::BilinearFilteringShader(const std::shared_ptr<gl::Texture> &paletteTexture,
                                                 const std::shared_ptr<gl::Texture> &colorTexture,
                                                 const std::shared_ptr<gl::Camera> &camera,
                                                 const std::string &shadersPath): Shader(shadersPath) {
	float w = colorTexture->width;
	float h = colorTexture->height;

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
}
