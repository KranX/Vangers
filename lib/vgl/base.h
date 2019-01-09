//
// Created by nikita on 2018-12-03.
//

#ifndef UPLOADBENCH_BASE_H
#define UPLOADBENCH_BASE_H

#include <GL/glew.h>

namespace vgl {
	template <typename T>
	class NamedObject{
	protected:
		T objectId;
	public:
		explicit NamedObject(T objectId):objectId(objectId) {}

		T getObjectId() const {
			return objectId;
		}
	};

}

#endif //UPLOADBENCH_BASE_H
