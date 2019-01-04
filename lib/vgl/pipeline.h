//
// Created by nikita on 2018-12-17.
//

#ifndef VANGERS_PIPELINE_H
#define VANGERS_PIPELINE_H
#include <map>

#include "shader.h"

namespace vgl {
	class Pipeline {
	private:
		std::shared_ptr<vgl::Shader> shader;
		std::shared_ptr<vgl::IVertexArray> vertexArray;
		std::vector<vgl::TextureAttribute> textureAttributes;
	public:
		Pipeline(const std::shared_ptr<Shader> &shader,
				const std::shared_ptr<IVertexArray> &vertexArray,
				const std::vector<TextureAttribute> &textureAttributes);

		void render();
		void render(const UniformData& data);

		// TODO: refactor
		template<typename T>
		void setUniform(const std::string& name, const T& value){
			shader->setUniform(name, value);
		}

		void useShader();
		static std::shared_ptr<Pipeline>
		create(const std::shared_ptr<Shader> &shader,
				const std::shared_ptr<IVertexArray> &vertexArray,
		       const std::vector<TextureAttribute> &textureAttributes);

		static std::shared_ptr<Pipeline>
		create(const std::string& shader_base_path,
		       const std::shared_ptr<vgl::IVertexArray>& vertexArray,
		       const std::map<std::string, std::shared_ptr<vgl::ITexture>>& textureMapping
				);
		void free();
	};
}
#endif //VANGERS_PIPELINE_H
