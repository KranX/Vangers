//
// Created by nikita on 2/13/18.
//

#ifndef VANGERS_VMAPRENDERER_H
#define VANGERS_VMAPRENDERER_H


#include <GL/glew.h>
#include <memory>
#include <vector>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SDL2/SDL.h>
#include "../../lib/xgraph/gl/Texture.h"
#include "../../lib/xgraph/gl/Camera.h"
#include "../../lib/xgraph/gl/Shader.h"
#include "../../lib/xgraph/gl/common.h"

const float MAX_SCALE = 96.0f;
const int DIRTY_REGION_CHUNK_SIZE = 2048;

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
	GLint textureScaleAttrId;
	GLint screenSizeAttrId;

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
	RayCastShader(float maxScale, const std::shared_ptr<gl::Camera> &camera,
	                       const std::shared_ptr<gl::Texture> &heightMapTexture,
	                       const std::shared_ptr<gl::Texture> &colorTexture,
	                       const std::shared_ptr<gl::Texture> &metaTexture,
	                       const std::shared_ptr<gl::Texture> &paletteTexture,
	                       const std::string &shadersPath);


	void render_impl() override;
	float scale;
	float maxScale;
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
    std::vector<bool> dirtyRegions;

	std::shared_ptr<gl::Shader> shader;
	std::shared_ptr<gl::Camera> camera;
	std::shared_ptr<gl::Texture> paletteTexture;
	std::shared_ptr<gl::Texture> colorTexture;
public:
	enum ShaderType {
		RayCast, BilinearFiltering
	};

	ShaderType shaderType;

	VMapRenderer(ShaderType shaderType, int sizeX, int sizeY) :
			sizeX(sizeX),sizeY(sizeY), shaderType(shaderType){}
	void init(const std::shared_ptr<gl::Texture> &heightMapTexture,
	          const std::shared_ptr<gl::Texture> &colorTexture,
	          const std::shared_ptr<gl::Texture> &metaTexture,
	          const std::shared_ptr<gl::Texture> &paletteTexture);
	void setPalette(SDL_Palette *sdlPalette, SDL_PixelFormat *format);
	void render(int viewPortWidth, int viewPortHeight, int x, int y, int z, float turn, float slope, float focus);
	void resetDirty();
	void setDirty(int y);
	void setDirty(int yStart, int yEnd);
	void updateColor(uint8_t **color, int lineUp, int lineDown);
};


#endif //VANGERS_VMAPRENDERER_H
