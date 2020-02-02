#include "xerrhand.h"
#include "port.h"
#include <stdlib.h>

#ifdef __HAIKU__
#include <SDL.h>
#endif

XErrorHandler ErrH;

XErrorHandler::XErrorHandler(void)
{
#ifndef __HAIKU__
	log_name = "logfile.txt";
#else
	log_name = SDL_GetPrefPath("KranX Productions", "Vangers");
	log_name += "/logfile.txt";
#endif
	log_file.open(log_name.c_str(),std::ios::out|std::ios::trunc);
	log_file.close();

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
	
	log_file.open(log_name.c_str(),std::ios::out|std::ios::app);
	log_file<<"Abbort: "<<message<<" code:"<<code<<" val:"<<val
		<<std::endl<<"Subj:"<<subj<<std::endl;
	log_file.close();
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
