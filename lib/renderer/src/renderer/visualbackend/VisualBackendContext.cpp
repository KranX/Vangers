#include "VisualBackendContext.h"
using namespace renderer::visualbackend;

std::unique_ptr<VisualBackendContext> VisualBackendContext::_instance(nullptr);
bool VisualBackendContext::_renderer_enabled(true);
