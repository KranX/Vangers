#include "xglobal.h"
#include <iostream>
#include <fstream>

static const char *openMSG	 = "CREATE/OPEN FAILURE";

std::fstream *open_file(const char* name, unsigned f)
{
	std::ios::openmode mode;
	mode = std::ios::binary;
	if (f & XS_IN)
		mode |= std::ios::in;
	if (f & XS_OUT)
		mode |= std::ios::out;
	if (f & XS_APPEND)
		mode |= std::ios::app;

	return new std::fstream(name, mode);
}

int XStream::open(const char* name, unsigned f)
{

	std::string smode;
	smode+="File name:";
	smode+=name;
	file_name = name;

#ifdef XSTREAM_DEBUG
	std::cerr << "DBG: XStream::open(\"" << name << "\", 0x" << std::hex << f << ")" << std::endl;
#endif

	std::fstream *file = open_file(name, f);
	handler = file;
	if (file->is_open()) {
		fname = name;
		pos = file->tellg();
		eofFlag = 0;
	} else {
		#ifdef XSTREAM_DEBUG
			std::cerr << "ERR: XStream::open(\"" << name << "\", 0x" << std::hex << f << ")" << std::endl;
		#endif
		if(ErrHUsed)
			ErrH.Abort(openMSG,XERR_USER,0,smode.c_str());
		else
			return 0;
	}
	return 1;
}

int XStream::open(XStream* owner,long s,long ext_sz)
{
	/* Full stream debug
	std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
	if (debug.is_open())
		debug<<"OPEN_XSTREAM "<<owner -> fname<<std::endl;
	debug.close();
	*/
	fname = owner -> fname;
	handler = owner -> handler;
	pos = 0;
	owner -> seek(s,XS_BEG);
	eofFlag = owner -> eof();
	extSize = ext_sz;
	extPos = s;
	return 1;
}

void XStream::close(void)
{
	
	if(handler == NULL)
		return;
	//std::cout<<"XStream::close: "<<fname<<std::endl;
	/* Full stream debug
	std::fstream debug("openfile.txt", std::ios::out|std::ios::app);

	if (debug.is_open()&&fname!=NULL)
		debug<<"CLOSE "<<fname<<std::endl;
	debug.close();
	*/
	//if(extSize == -1 && !CloseHandle(handler) && ErrHUsed)
	//	ErrH.Abort(closeMSG,XERR_USER,GetLastError(),fname);

	if (handler->is_open())
		handler->close();
	delete handler;
	handler = NULL;
	//fname = "";
	pos = 0L;
	eofFlag = 1;
	extSize = -1;
	extPos = 0;
}

