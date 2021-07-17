#include "xglobal.h"

#include <fstream>

unsigned xsReadBytes = 0;
unsigned xsReadBytesDelta = 0;
unsigned xsWriteBytes = 0;
unsigned xsWriteBytesDelta = 0;

unsigned xsRdWrResolution = 500000U;

void (*xsReadHandler)(unsigned) = NULL;
void (*xsWriteHandler)(unsigned) = NULL;

void xsSetReadHandler(void (*fp)(unsigned),unsigned res)
{
	xsReadHandler = fp;
	xsRdWrResolution = res;
	xsReadBytes = 0;
}

void xsSetWriteHandler(void (*fp)(unsigned),unsigned res)
{
	xsWriteHandler = fp;
	xsRdWrResolution = res;
	xsWriteBytes = 0;
}

unsigned long XStream::read(void* buf, unsigned long len)
{
	unsigned long ret;
	/*if(!ReadFile(handler,buf,len,&ret,0))
		if(ErrHUsed) ErrH.Abort(readMSG,XERR_USER,GetLastError(),fname);
		else return 0U;
	if(ret < len) eofFlag = 1;*/
	((std::fstream *)handler)->read((char *)buf, len);
	len = ((std::fstream *)handler)->gcount();
	ret = len;
	pos += ret;
	if(extSize != -1 && pos >= extSize) eofFlag = 1;
	
	if(xsReadHandler){
		xsReadBytesDelta += ret;
		if(xsReadBytesDelta >= xsRdWrResolution){
			xsReadBytes += xsReadBytesDelta;
			xsReadBytesDelta = 0;
			(*xsReadHandler)(xsReadBytes);
		}
	}

	// Full stream debug
	/*std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
	if (debug.is_open())
		debug<<"READ "<<fname<<" len:"<<len<<std::endl;
		//debug<<"READ "<<fname<<" len:"<<len<<" buf:"<<(char *)buf<<std::endl;
	debug.close();*/
	return ret;
}

unsigned long XStream::write(const void* buf, unsigned long len)
{
	/* Full stream debug
	std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
	if (debug.is_open())
		debug<<"WRITE "<<fname<<" len:"<<len<<" buf:"<<(char*)buf<<std::endl;
	debug.close();
	*/
	unsigned long ret;
	/*VNG_DEBUG()<<"Start write to:"<<fname
			<<" len:"<<len
			<<" "<<((std::fstream *)handler)->fail()
			<<" "<<((std::fstream *)handler)->good()
			<<" "<<((std::fstream *)handler)->eof()
			<<" "<<((std::fstream *)handler)->bad()
			<<" open:"<<((std::fstream *)handler)->is_open()
			<<" tellp:"<<((std::fstream *)handler)->tellp()<<std::endl;*/
	((std::fstream *)handler)->write((char *)buf, len);
	/*
		VNG_DEBUG()<<"END write to:"<<fname
			<<" len:"<<len
			<<" "<<((std::fstream *)handler)->fail()
			<<" "<<((std::fstream *)handler)->good()
			<<" "<<((std::fstream *)handler)->eof()
			<<" "<<((std::fstream *)handler)->bad()
			<<" tellp:"<<((std::fstream *)handler)->tellp()<<std::endl;
		VNG_DEBUG()<<(char *)buf<<std::endl;*/
	
	ret = len;
	pos += ret;

	if(xsWriteHandler){
		xsWriteBytesDelta += ret;
		if(xsWriteBytesDelta >= xsRdWrResolution){
			xsWriteBytes += xsWriteBytesDelta;
			xsWriteBytesDelta = 0;
			(*xsWriteHandler)(xsWriteBytes);
		}
	}
	return ret;
}

