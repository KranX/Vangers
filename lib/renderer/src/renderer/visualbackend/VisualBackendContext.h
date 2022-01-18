//
// Created by nikita on 26.11.2021.
//

#ifndef VANGERS_RENDERINGCONTEXT_H
#define VANGERS_RENDERINGCONTEXT_H

#include <memory>
#include <cassert>

#include "AbstractVisualBackend.h"


namespace renderer::visualbackend {
    class VisualBackendContext {
	public:
		static void create(std::unique_ptr<AbstractVisualBackend>&& renderer) {
			_instance = std::make_unique<VisualBackendContext>(std::move(renderer));
		}

		static bool has_renderer() {
			return (bool)_instance;
		}

		static void reset(){
			_instance.reset();
		}

//		static std::unique_ptr<VisualBackendContext>& instance() {
//			return _instance;
//		}

		static inline std::unique_ptr<AbstractVisualBackend>& backend() {
			assert(_instance && "VisualBackendContext instance is not created");

			return _instance->_renderer;
		}

		// TODO: private
		explicit VisualBackendContext(std::unique_ptr<AbstractVisualBackend>&& renderer){
			_renderer = std::move(renderer);
		}

	private:
		std::unique_ptr<AbstractVisualBackend> _renderer;
		static std::unique_ptr<VisualBackendContext> _instance;
	};
}




#endif //VANGERS_RENDERINGCONTEXT_H
