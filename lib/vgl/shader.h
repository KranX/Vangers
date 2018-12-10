//
// Created by nikita on 2018-12-03.
//

#ifndef UPLOADBENCH_SHADER_H
#define UPLOADBENCH_SHADER_H

#include <string>
#include <vector>

#include "uniform.h"
#include "util.h"
#include "base.h"
#include "vertex_array.h"
#include "itexture.h"

namespace vgl {
	class _EmptyTexture: public ITexture{
		void bind(){}
		void free(){}
	};

	class TextureAttribute: public NamedObject<GLint>{
	private:
		TextureAttribute(const TextureAttribute& textureAttribute) = delete;
	public:
		ITexture& texture;

		TextureAttribute(TextureAttribute&& other) noexcept :
				NamedObject(other.objectId),
			texture(other.texture) {}

		TextureAttribute(GLint objectId, ITexture& texture):
			NamedObject(objectId),
			texture(texture){}
	};

	const GLuint GlTextures[] = {
		GL_TEXTURE0,
		GL_TEXTURE1,
		GL_TEXTURE2,
		GL_TEXTURE3
	};

	class Shader : public NamedObject<GLuint> {
		std::vector<TextureAttribute> textureAttributes;
	public:
		void bindUniformAttribs(UniformData &data);

		void addTexture(vgl::ITexture& texture, const std::string &name);

		Shader(GLuint objectId):NamedObject(objectId){}

		Shader(Shader&& other) noexcept : NamedObject(other.objectId){}

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		Shader& operator=(Shader&& other) noexcept{
			objectId = other.objectId;
			return *this;
		}


		GLint getAttribute(const std::string& name) const;

		void use() const {
			glUseProgram(objectId);
			vgl::checkErrorAndThrow("glUseProgram");
		}

		static std::shared_ptr<Shader> create(const std::string &vertexShaderCode,
		                     const std::string &fragmentShaderCode);

		static std::shared_ptr<Shader> createFromPath(const std::string &vertexShaderPath,
		                                      const std::string &fragmentShaderPath);

		template<typename VertexType, typename ElementType>
		void render(UniformData &data, const std::shared_ptr<VertexArray<VertexType, ElementType>>& vertexArray){
			use();

			for(auto* member: data.members){
				member->assignData();
			}

			for(int i = 0; i < textureAttributes.size();i++){
				glActiveTexture(GlTextures[i]);
				textureAttributes[i].texture.bind();
				glUniform1i(textureAttributes[i].getObjectId(), i);
			}

			vertexArray->enable();
			glDrawElements(GL_TRIANGLES, vertexArray->getNumElements(), GL_UNSIGNED_INT, 0);
			vgl::checkErrorAndThrow("glDrawElements");
			vertexArray->disable();
		}

		void free(){
			glDeleteProgram(objectId);
		}
	};
}
#endif //UPLOADBENCH_SHADER_H
