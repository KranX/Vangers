#include <utility>

//
// Created by nikita on 2018-12-03.
//

#ifndef UPLOADBENCH_UNIFORM_H
#define UPLOADBENCH_UNIFORM_H

#define UNIFORM(type, name) vgl::Uniform<type> name{#name, this};

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "base.h"

namespace vgl {
	class UniformBase;
	class Shader;

	class UniformData {
		friend class Shader;
	private:
		std::vector<UniformBase*> members;
	public:
		void addUniform(UniformBase* uniform){
			members.push_back(uniform);
		}

		std::vector<UniformBase*> getMembers(){
			return members;
		}
	};

	template<typename T>
	std::vector<UniformBase*> _uniformCreator(){
		return T().getMembers();
	}

	class UniformBase {
	public:
		UniformBase(std::string& name, UniformData* data):attrId(0),name(std::move(name)){
			data->addUniform(this);
		}
		GLint attrId;
		std::string name;

		virtual void assignData() = 0;

	};

	template <class T>
	class Uniform : public UniformBase {
	public:
		Uniform(std::string name, UniformData* ownerData):UniformBase(name, ownerData){}
		T data;

		static void assign(GLint attrId, const T &data){
			std::cerr<<"Not implemented"<<std::endl;
		}

		void assignData() override{
			assign(attrId, data);
		}

		Uniform& operator=(const T& data){
			this->data = data;
			return *this;
		}

		T& operator->(){
			return data;
		}
	};


}
#endif //UPLOADBENCH_UNIFORM_H
