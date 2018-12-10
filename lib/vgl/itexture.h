//
// Created by nikita on 2018-12-06.
//

#ifndef UPLOADBENCH_ITEXTURE_H
#define UPLOADBENCH_ITEXTURE_H
namespace vgl {
	class ITexture{
	public:
		virtual void bind() = 0;
		virtual void free() = 0;
	};
}
#endif //UPLOADBENCH_ITEXTURE_H
