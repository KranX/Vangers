#ifndef LIB_RENDERER_SRC_RENDERER_EXCEPTION
#define LIB_RENDERER_SRC_RENDERER_EXCEPTION

#include <string>
#include <sstream>
#include <utility>

#include "common.h"

namespace renderer {
class RendererException: public std::exception{
	public:
		explicit RendererException(std::string message)
			: _message(std::move(message)){}

		const char * what () const noexcept final {
			return _message.c_str();
		}
	private:
		std::string _message;
	};

    template<typename TResourceId>
    class ResourceDoesNotExistsException: public RendererException{
	public:
		explicit ResourceDoesNotExistsException(TResourceId rid)
		    : RendererException("RID does not exit: " + std::to_string(rid.id))
			, _rid(rid)
		{}

		TResourceId rid() const {
			return _rid;
		}
	private:
		TResourceId _rid;
};

}

#endif /* LIB_RENDERER_SRC_RENDERER_EXCEPTION */
