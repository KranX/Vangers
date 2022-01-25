#ifndef COMPOSITOREXCEPTION_H
#define COMPOSITOREXCEPTION_H

#include <string>
#include <exception>

namespace renderer::compositor {
	class CompositorException : public std::exception
	{
	public:
		CompositorException(const std::string& message);

		inline const char* what() const noexcept {
			return _message.c_str();
		}
	private:
		std::string _message;
	};
}



#endif // COMPOSITOREXCEPTION_H
