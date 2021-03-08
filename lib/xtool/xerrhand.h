#ifndef _ERRH_H
#define _ERRH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#ifndef NULL
#define NULL	0L
#endif

#define X_WINNT 		0x0001
#define X_WIN32S		0x0002
#define X_WIN95 		0x0003

#define XERR_ALL		0xFFFF
#define XERR_NONE		0x0000

#define XERR_USER		0x0001
#define XERR_CTRLBRK	0x0004
#define XERR_MATH		0x0008
#define XERR_FLOAT		0x0010
#define XERR_CRITICAL	0x0020
#define XERR_SIGNAL		0x0040

#define XERR_ACCESSVIOLATION	0x0080
#define XERR_DEBUG		0x0100
#define XERR_UNKNOWN	0x0200


struct XErrorHandler
{
	enum class MessageSeverity
	{
        Unknown = XERR_NONE,
		Debug = XERR_DEBUG,
	};

	std::string log_name;

	std::fstream log_file;
	XErrorHandler();
	~XErrorHandler();

	void	 Abort(const char* message, int code = XERR_USER, int val = -1, const char* subj = NULL);
	void	 Log(const char* message);
	void logMessage(MessageSeverity messageSeverity, const std::string &context, const std::string &message);
	void	 Exit(void);
	void	 RTC(const char *file,unsigned int line, const char *expr);

    static std::string to_string(XErrorHandler::MessageSeverity severity)
    {
        switch (severity)
        {
        case XErrorHandler::MessageSeverity::Debug:
            return "DEBUG";
        default:
            return "UNKNOWN";
        }
    }
};

extern XErrorHandler ErrH;

// Use this macro for after any operation for errors diagnostic
#define XAssert(expr) ErrH.RTC(__FILE__,__LINE__,expr)

struct ErrorHandlerStreamWrapper
{
    const std::string context;
    const XErrorHandler::MessageSeverity messageSeverity;

    std::stringstream stream;

    ErrorHandlerStreamWrapper(XErrorHandler::MessageSeverity messageSeverity, const std::string &context)
        : messageSeverity(messageSeverity)
        , context(context)
    {

    }

    ~ErrorHandlerStreamWrapper()
    {
        ::ErrH.logMessage(messageSeverity, context, stream.str());
    }

    ErrorHandlerStreamWrapper &operator<<(std::ostream&(*value)(std::ostream&)) // NOTE std::endl workaround
    {
        value(stream);

        return *this;
    }

    template <typename T>
    ErrorHandlerStreamWrapper &operator<<(T value)
    {
        stream << value;

        return *this;
    }
};

#define VNG_DEBUG_CTX(context) ErrorHandlerStreamWrapper(XErrorHandler::MessageSeverity::Debug, context)
#define VNG_DEBUG() VNG_DEBUG_CTX(std::string(__FUNCTION__) + "@" + std::to_string(__LINE__))

//#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
//#endif

#endif /* _ERRH_H */
