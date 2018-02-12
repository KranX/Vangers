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
#include "../../lib/xgraph/gl.h"

struct RenderData {
	int width;
	int height;
//	GLuint colorTextureId;
//	GLuint heightMapTextureId;
//	GLuint paletteTextureId;
//	GLuint shaderProgramId;
//    GLuint vertexBuffer;
//    GLuint uvBuffer;
};

class Camera {
public:
	glm::vec2 viewport;
	glm::vec3 position;
	glm::quat rotation;
	void from_player(float x, float y, float height, float turn, float slope);
	glm::mat4 mvp();

};

class ShaderData {
public:
	std::shared_ptr<Texture> heightMapTexture;
	std::shared_ptr<Texture> paletteTexture;
	std::shared_ptr<Texture> colorTexture;
	std::shared_ptr<Texture> metaTexture;
	std::shared_ptr<Camera> camera;

	GLint heightMapTextureAttrId;
	GLint colorTextureAttrId;
	GLint paletteTextureAttrId;
	GLint metaTextureAttrId;

	GLint mvpAtrrId;
	GLint invMvpAttrId;
	GLint cameraPosAttrId;
	GLint positionAttrId;
	GLint texcoordAttrId;
	std::shared_ptr<BufferData> bufferData;
};

class VMapRendererException {

};

class VMapShader : public Shader<ShaderData> {

public:
	VMapShader(const std::shared_ptr<Camera> &camera,
	           const std::shared_ptr<Texture> &heightMapTexture,
	           const std::shared_ptr<Texture> &colorTexture,
		       const std::shared_ptr<Texture> &metaTexture,
	           const std::shared_ptr<Texture> &paletteTexture,
		       const std::string &shadersPath);
	void render_impl() override;
};

class VMapRenderer {
private:
	int sizeX;
	int sizeY;

	std::string shaderPath;


	std::shared_ptr<VMapShader> shader;
	std::shared_ptr<Camera> camera;

public:
	VMapRenderer(int sizeX, int sizeY, std::string shaderPath):
			sizeX(sizeX),sizeY(sizeY),shaderPath(std::move(shaderPath)){}
	void init(const std::shared_ptr<Texture> &heightMapTexture, const std::shared_ptr<Texture> &colorTexture,
		          const std::shared_ptr<Texture> &metaTexture, const std::shared_ptr<Texture> &paletteTexture);
	void setPalette(SDL_Palette *sdlPalette, SDL_PixelFormat *format);
	void render(int viewPortWidth, int viewPortHeight, int x, int y, int z, float turn, float slope);
	void updateColor(uint8_t **color, int lineUp, int lineDown);
};


#endif //VANGERS_VMAPRENDERER_H
