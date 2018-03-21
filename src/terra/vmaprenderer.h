//
// Created by nikita on 2/13/18.
//

#ifndef VANGERS_VMAPRENDERER_H
#define VANGERS_VMAPRENDERER_H


#include <GL/glew.h>
#include <memory>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SDL2/SDL.h>
#include "../../lib/xgraph/gl/Texture.h"
#include "../../lib/xgraph/gl/Camera.h"
#include "../../lib/xgraph/gl/Shader.h"
#include "../../lib/xgraph/gl/common.h"


class RayCastShaderData {
public:
	std::shared_ptr<gl::Texture> heightMapTexture;
	std::shared_ptr<gl::Texture> paletteTexture;
	std::shared_ptr<gl::Texture> colorTexture;
	std::shared_ptr<gl::Texture> metaTexture;
	std::shared_ptr<gl::Camera> camera;

	GLint heightMapTextureAttrId;
	GLint colorTextureAttrId;
	GLint paletteTextureAttrId;
	GLint metaTextureAttrId;

	GLint mvpAtrrId;
	GLint invMvpAttrId;
	GLint cameraPosAttrId;
	GLint positionAttrId;
	GLint texcoordAttrId;
	std::shared_ptr<gl::VertexBufferData> bufferData;
};

class VMapRendererException {

};


class RayCastShader: public gl::Shader{
public:
	RayCastShaderData data;
	RayCastShader(const std::shared_ptr<gl::Camera> &camera,
	                       const std::shared_ptr<gl::Texture> &heightMapTexture,
	                       const std::shared_ptr<gl::Texture> &colorTexture,
	                       const std::shared_ptr<gl::Texture> &metaTexture,
	                       const std::shared_ptr<gl::Texture> &paletteTexture,
	                       const std::string &shadersPath);


	void render_impl() override;
};

class BilinearShaderData {
public:
	std::shared_ptr<gl::Texture> paletteTexture;
	std::shared_ptr<gl::Texture> colorTexture;
	std::shared_ptr<gl::Camera> camera;

	GLint colorTextureAttrId;
	GLint paletteTextureAttrId;

	GLint mvpAtrrId;
	GLint positionAttrId;
	GLint texcoordAttrId;
	std::shared_ptr<gl::VertexBufferData> bufferData;
};

class BilinearFilteringShader: public gl::Shader{
	BilinearShaderData data;
public:
	BilinearFilteringShader(
		const std::shared_ptr<gl::Texture>& paletteTexture,
		const std::shared_ptr<gl::Texture>& colorTexture,
		const std::shared_ptr<gl::Camera>& camera,
		const std::string &shadersPath
	);
	void render_impl() override;
};

class VMapRenderer {
private:
	int sizeX;
	int sizeY;

	std::string shaderPath;


	std::shared_ptr<gl::Shader> shader;
	std::shared_ptr<gl::Camera> camera;
	std::shared_ptr<gl::Texture> paletteTexture;
	std::shared_ptr<gl::Texture> colorTexture;
public:
	VMapRenderer(int sizeX, int sizeY, std::string shaderPath):
			sizeX(sizeX),sizeY(sizeY),shaderPath(std::move(shaderPath)){}
	void init(const std::shared_ptr<gl::Texture> &heightMapTexture,
	          const std::shared_ptr<gl::Texture> &colorTexture,
	          const std::shared_ptr<gl::Texture> &metaTexture,
	          const std::shared_ptr<gl::Texture> &paletteTexture);
	void setPalette(SDL_Palette *sdlPalette, SDL_PixelFormat *format);
	void render(int viewPortWidth, int viewPortHeight, int x, int y, int z, float turn, float slope);
	void updateColor(uint8_t **color, int lineUp, int lineDown);
};


#endif //VANGERS_VMAPRENDERER_H
