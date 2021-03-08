#include "xerrhand.h"
#include "port.h"
#include <stdlib.h>

#include <algorithm>

#include <SDL.h>

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
	log_file<<"Abort: "<<message<<" code:"<<code<<" val:"<<val<<std::endl;
	if (subj)
		log_file<<"Subj:"<<subj<<std::endl;
	log_file.close();
	std::ostringstream stream;
	stream << "Error: "<< message << " code:" << code << " val:" << val << std::endl;
	if (subj)
		log_file<<"Subj:" << subj << std::endl;
	log_file<<"Please send:" << std::endl <<
	" - this message," << std::endl <<
	" - logfile from " << SDL_GetBasePath() << log_name.c_str() << "," << std::endl <<
	" - your savegame" << std::endl <<
	"to https://t.me/vangers or https://github.com/KranX/Vangers";
	std::string str =  stream.str();
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

void XErrorHandler::logMessage(MessageSeverity messageSeverity, const std::string &context, const std::string &message)
{
    std::stringstream stream;
    std::string line(message);

    line.erase(std::find_if(line.rbegin(), line.rend(), [](char symbol) // NOTE trim EoLs and spaces at end of the line
    {
        return !(std::iscntrl(symbol) || std::isspace(symbol));
    }).base(), line.end());

    stream << "[" << to_string(messageSeverity) << "] (" << context << ") " << line;

    Log(stream.str().c_str());
}
