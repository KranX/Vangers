#include "CompositorException.h"

using namespace renderer::compositor;

CompositorException::CompositorException(const std::string& message)
	: _message(message)
{

}
