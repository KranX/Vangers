#include "xerrhand.h"
#include "port.h"
#include <stdlib.h>

#include <SDL.h>
#include <iostream>
#include <ctime>

XErrorHandler ErrH;

XErrorHandler::XErrorHandler(void)
{
#ifndef __HAIKU__
	log_name = "logfile.txt";
#else
	log_name = SDL_GetPrefPath("KranX Productions", "Vangers");
	log_name += "/logfile.txt";
#endif
}

XErrorHandler::~XErrorHandler(void)
{
	if(log_file.is_open())
		log_file.close();
}

void XErrorHandler::SetPrefix(const char* s)
{
	prefix = s;
}

void XErrorHandler::SetPostfix(const char* s)
{
	postfix = s;
}

void XErrorHandler::RTC(const char *file, unsigned int line, const char *expr)
{
	char msg[256], msg_tmp[10];
	strcpy(msg, file);
	strcat(msg,", LINE: ");
	strcat(msg, port_itoa(line,msg_tmp,10));
	strcat(msg,"\r\n");
	strcat(msg,expr);
	Abort("RTC",XERR_USER,-1,msg);
}

void XErrorHandler::Abort(const char* message, int code, int val, const char* subj)
{
	std::ostringstream stream;
	time_t now;
    time(&now);
    char time_buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(time_buf, sizeof time_buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

	stream<<time_buf<<" "<<std::endl;
		stream<<GIT_BRANCH<<" "<<GIT_COMMIT_HASH<<std::endl<<std::endl;

stream<<"Abort: "<<message<<" code:"<<code<<" val:"<<val<<std::endl;
	std::cout<<"Abort: "<<message<<" code:"<<code<<" val:"<<val<<std::endl;
	if (subj)
		stream<<"Subj:"<<subj<<std::endl<<std::endl;
	stream<<"Please send:" << std::endl <<
	" - this message," << std::endl <<
	" - logfile from " << SDL_GetBasePath() << log_name.c_str() << "," << std::endl <<
	" - your savegame" << std::endl <<
	"to https://t.me/vangers or https://github.com/KranX/Vangers";
	std::string str =  stream.str();
  std::cout<<str;
	log_file.open(log_name.c_str(),std::ios::out|std::ios::app);
  log_file<<str;
	log_file.close();
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
		"Vangers error",
		str.c_str(),
		NULL);
	//MessageBox(NULL,outmsg,prefix,attr | MB_TOPMOST | MB_SYSTEMMODAL);
	exit(code);
}

void XErrorHandler::Log(const char* message)
{
	log_file.open(log_name.c_str(),std::ios::out|std::ios::app);
	log_file<<message<<std::endl;
	log_file.close();
}
void XErrorHandler::Exit(void)
{
	exit(0);
}

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>

#include <stdio.h>
#include <stdlib.h>

  void windows_print_stacktrace(CONTEXT* context, std::ostringstream &error_string)
  {
    SymInitialize(GetCurrentProcess(), 0, true);

    STACKFRAME frame = { 0 };

    /* setup initial stack frame */
	#ifdef _M_AMD64
		frame.AddrPC.Offset         = context->Rip;
		frame.AddrPC.Mode           = AddrModeFlat;
		frame.AddrStack.Offset      = context->Rsp;
		frame.AddrStack.Mode        = AddrModeFlat;
		frame.AddrFrame.Offset      = context->Rbp;
		frame.AddrFrame.Mode        = AddrModeFlat;
	#else
		frame.AddrPC.Offset         = context->Eip;
		frame.AddrPC.Mode           = AddrModeFlat;
		frame.AddrStack.Offset      = context->Esp;
		frame.AddrStack.Mode        = AddrModeFlat;
		frame.AddrFrame.Offset      = context->Ebp;
		frame.AddrFrame.Mode        = AddrModeFlat;
	#endif

    while (StackWalk(
					#ifdef _M_AMD64
						IMAGE_FILE_MACHINE_AMD64
					#else
						IMAGE_FILE_MACHINE_I386
					#endif
					,
                     GetCurrentProcess(),
                     GetCurrentThread(),
                     &frame,
                     context,
                     0,
                     SymFunctionTableAccess,
                     SymGetModuleBase,
                     0 ) )
    {
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
		DWORD offset_from_symbol=0;
    	IMAGEHLP_LINE64 line = {0};
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;
		
		DWORD64 displacement = 0;
		HANDLE process = GetCurrentProcess();
		if (SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol)) {
			error_string << "[" << 0 << "] " << symbol->Name;
			if (SymGetLineFromAddr64( process, frame.AddrPC.Offset, &offset_from_symbol, &line ) )  {
				error_string << "\t" << line.FileName << "(" << line.LineNumber << ")";
			}
			error_string << "\n";
		} else {
			error_string << "[" << 0 << "???\n";
		}
    }

	ErrH.Abort(error_string.str().c_str());

    SymCleanup( GetCurrentProcess() );
  }

  LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS * ExceptionInfo)
  {
	std::ostringstream error_string;
    switch(ExceptionInfo->ExceptionRecord->ExceptionCode)
    {
      case EXCEPTION_ACCESS_VIOLATION:
        error_string << "Error: EXCEPTION_ACCESS_VIOLATION\n";
        break;
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        error_string << "Error: EXCEPTION_ARRAY_BOUNDS_EXCEEDED\n";
        break;
      case EXCEPTION_BREAKPOINT:
        error_string << "Error: EXCEPTION_BREAKPOINT\n";
        break;
      case EXCEPTION_DATATYPE_MISALIGNMENT:
        error_string << "Error: EXCEPTION_DATATYPE_MISALIGNMENT\n";
        break;
      case EXCEPTION_FLT_DENORMAL_OPERAND:
        error_string << "Error: EXCEPTION_FLT_DENORMAL_OPERAND\n";
        break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        error_string << "Error: EXCEPTION_FLT_DIVIDE_BY_ZERO\n";
        break;
      case EXCEPTION_FLT_INEXACT_RESULT:
        error_string << "Error: EXCEPTION_FLT_INEXACT_RESULT\n";
        break;
      case EXCEPTION_FLT_INVALID_OPERATION:
        error_string << "Error: EXCEPTION_FLT_INVALID_OPERATION\n";
        break;
      case EXCEPTION_FLT_OVERFLOW:
        error_string << "Error: EXCEPTION_FLT_OVERFLOW\n";
        break;
      case EXCEPTION_FLT_STACK_CHECK:
        error_string << "Error: EXCEPTION_FLT_STACK_CHECK\n";
        break;
      case EXCEPTION_FLT_UNDERFLOW:
        error_string << "Error: EXCEPTION_FLT_UNDERFLOW\n";
        break;
      case EXCEPTION_ILLEGAL_INSTRUCTION:
        error_string << "Error: EXCEPTION_ILLEGAL_INSTRUCTION\n";
        break;
      case EXCEPTION_IN_PAGE_ERROR:
        error_string << "Error: EXCEPTION_IN_PAGE_ERROR\n";
        break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
        error_string << "Error: EXCEPTION_INT_DIVIDE_BY_ZERO\n";
        break;
      case EXCEPTION_INT_OVERFLOW:
        error_string << "Error: EXCEPTION_INT_OVERFLOW\n";
        break;
      case EXCEPTION_INVALID_DISPOSITION:
        error_string << "Error: EXCEPTION_INVALID_DISPOSITION\n";
        break;
      case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        error_string << "Error: EXCEPTION_NONCONTINUABLE_EXCEPTION\n";
        break;
      case EXCEPTION_PRIV_INSTRUCTION:
        error_string << "Error: EXCEPTION_PRIV_INSTRUCTION\n";
        break;
      case EXCEPTION_SINGLE_STEP:
        error_string << "Error: EXCEPTION_SINGLE_STEP\n";
        break;
      case EXCEPTION_STACK_OVERFLOW:
        error_string << "Error: EXCEPTION_STACK_OVERFLOW\n";
        break;
      default:
        error_string << "Error: Unrecognized Exception\n";
        break;
    }
    /* If this is a stack overflow then we can't walk the stack, so just show
      where the error happened */
    windows_print_stacktrace(ExceptionInfo->ContextRecord, error_string);

    return EXCEPTION_EXECUTE_HANDLER;
  }

  void set_signal_handler()
  {
    SetUnhandledExceptionFilter(windows_exception_handler);
  }
#else
	void set_signal_handler();
#endif