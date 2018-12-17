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
#include "../../lib/vgl/texture_ext.h"
#include "../../lib/vgl/shader.h"
#include "../../lib/vgl/camera.h"
#include "../../lib/vgl/buffer.h"
#include "../../lib/vgl/uniform.h"
#include "../../lib/vgl/uniform_ext.h"
#include "../../lib/vgl/vertex_array.h"


const float MAX_SCALE = 96.0f;
const int DIRTY_REGION_CHUNK_SIZE = 2048;

class RenderingProcess {
public:
	virtual void render(const vgl::Camera &camera) = 0;
};

class RayCastProcess : public RenderingProcess {
	struct Data : public vgl::UniformData {
		UNIFORM(glm::mat4, u_ViewProj)
		UNIFORM(glm::mat4, u_InvViewProj)
		UNIFORM(glm::vec4, u_CamPos)
		UNIFORM(glm::vec4, u_TextureScale)
		UNIFORM(glm::vec4, u_ScreenSize)
	};

	struct Vertex{
		glm::vec2 pos;
		glm::vec2 uv;
	};

	Data data;
	std::shared_ptr<vgl::Shader> shader;
	std::shared_ptr<vgl::VertexArray<Vertex, GLuint>> vertexArray;
	glm::vec4 textureScale;

public:

	RayCastProcess(float maxScale,
					const std::shared_ptr<vgl::Texture2DArray> &heightMapTexture,
                   const std::shared_ptr<vgl::Texture2DArray> &colorTexture,
                   const std::shared_ptr<vgl::Texture2DArray> &metaTexture,
                   const std::shared_ptr<vgl::Texture1D> &paletteTexture,
	                       const std::string &shadersPath);


	void render(const vgl::Camera &camera);
	float scale;
	float maxScale;
};


class BilinearFilteringProcess : public RenderingProcess{
	struct Vertex{
		glm::vec2 pos;
		glm::vec2 uv;
	};

	struct Data : public vgl::UniformData {
		UNIFORM(glm::mat4, u_ViewProj)
		UNIFORM(glm::vec4, u_TextureScale)
		UNIFORM(glm::vec4, u_ScreenSize)
	};


	std::shared_ptr<vgl::Shader> shader;
	std::shared_ptr<vgl::VertexArray<Vertex, GLuint>> vertexArray;
	glm::vec4 textureScale;
	Data data;

public:
	BilinearFilteringProcess(
		const std::shared_ptr<vgl::Texture1D>& paletteTexture,
		const std::shared_ptr<vgl::Texture2DArray>& colorTexture,
		const std::string &shadersPath
	);
	void render(const vgl::Camera &camera);
};

class VMapRenderer {
private:
	int sizeX;
	int sizeY;
	int currentUpdateBuffer = 0;
	uint8_t* buffer;
    std::vector<bool> dirtyRegions;

	std::shared_ptr<RenderingProcess> process;
	std::shared_ptr<vgl::Camera> camera;
	std::shared_ptr<vgl::Texture1D> paletteTexture;
	std::shared_ptr<vgl::Texture2DArray> colorTexture;
	std::shared_ptr<vgl::Texture2DArray> heightTexture;
	std::shared_ptr<vgl::PixelUnpackBuffer> updateBuffer;
public:
	enum ShaderType {
		RayCast, BilinearFiltering
	};

	ShaderType shaderType;

	VMapRenderer(ShaderType shaderType, int sizeX, int sizeY) :
			sizeX(sizeX),sizeY(sizeY), shaderType(shaderType){}
	void init(const std::shared_ptr<vgl::Texture2DArray> &heightMapTexture,
	          const std::shared_ptr<vgl::Texture2DArray> &colorTexture,
	          const std::shared_ptr<vgl::Texture2DArray> &metaTexture,
	          const std::shared_ptr<vgl::Texture1D> &paletteTexture);
	void deinit();
	void setPalette(SDL_Palette *sdlPalette, SDL_PixelFormat *format);
	void render(int viewPortWidth, int viewPortHeight, int x, int y, int z, float turn, float slope, float focus);
	void resetDirty();
	void setDirty(int y);
	void setDirty(int yStart, int yEnd);
	void updateColor(uint8_t **color, int lineUp, int lineDown);
};


#endif //VANGERS_VMAPRENDERER_H
