#include "xglobal.h"

#include <fstream>

static const char *sizeMSG = "FILE SIZE CALCULATION ERROR";

long XStream::seek(long offset, int dir)
{
	/* Full stream debug*/
	/*std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
	if (debug.is_open())
		debug<<"SEEK START "<<fname<<" tellg:"<<((std::fstream *)handler)->tellg()
		<<" addr:"<<handler<<" curo:"<<offset<<std::endl;
	debug.close();*/

	long ret=0;
	if(extSize != -1){
		switch(dir){
			case XS_BEG:
				//ret = SetFilePointer(handler,extPos + offset,0,dir) - extPos;
				if (((std::fstream *)handler)->flags() & std::ios::out) {
					ret = ((std::fstream *)handler)->tellp() - (std::streamoff)extPos;
					((std::fstream *)handler)->seekp(extPos + offset, std::ios_base::beg);
				}
				else {
					ret = ((std::fstream *)handler)->tellg() - (std::streamoff)extPos;
					((std::fstream *)handler)->seekg(extPos + offset, std::ios_base::beg);
				}
				break;
			case XS_END:
				//ret = SetFilePointer(handler,extPos + extSize - offset - 1,0,XS_BEG) - extPos;
				if (((std::fstream *)handler)->flags() & std::ios::out) {
					ret = ((std::fstream *)handler)->tellp() - (std::streamoff)extPos;
					((std::fstream *)handler)->seekp(extPos + extSize - offset - 1, std::ios_base::beg);
				} else {
					ret = ((std::fstream *)handler)->tellg() - (std::streamoff)extPos;
					((std::fstream *)handler)->seekg(extPos + extSize - offset - 1, std::ios_base::beg);
				}
				break;
			case XS_CUR:
				//ret = SetFilePointer(handler,extPos + pos + offset,0,XS_BEG) - extPos;
				//((std::fstream *)handler)->clear();
				if (((std::fstream *)handler)->flags() & std::ios::out) {
					ret = ((std::fstream *)handler)->tellp() - (std::streamoff)extPos;
					((std::fstream *)handler)->seekp(extPos + pos + offset, std::ios_base::beg);
				} else {
					ret = ((std::fstream *)handler)->tellg() - (std::streamoff)extPos;
					((std::fstream *)handler)->seekg(extPos + pos + offset, std::ios_base::beg);
				}
				break;
		}
	}
	else
	{
		//ret = SetFilePointer(handler,offset,0,dir);
		//std::cout<<"SEEK:"<<fname<<std::endl;
		switch(dir){
			case XS_BEG:
				if (((std::fstream *)handler)->flags() & std::ios::out)
					((std::fstream *)handler)->seekp(offset, std::ios_base::beg);
				else
					((std::fstream *)handler)->seekg(offset, std::ios_base::beg);
				break;
			case XS_END:
				if (((std::fstream *)handler)->flags() & std::ios::out)
					((std::fstream *)handler)->seekp(offset, std::ios_base::end);
				else
					((std::fstream *)handler)->seekg(offset, std::ios_base::end);
				break;
			case XS_CUR:
				if (((std::fstream *)handler)->flags() & std::ios::out)
					((std::fstream *)handler)->seekp(offset, std::ios_base::cur);
				else
					((std::fstream *)handler)->seekg(offset, std::ios_base::cur);
				break;
		}
		if (((std::fstream *)handler)->flags() & std::ios::out)
			ret = ((std::fstream *)handler)->tellp();
		else
			ret = ((std::fstream *)handler)->tellg();

	}
	/* Full stream debug*/
	/*debug.open("openfile.txt", std::ios::out|std::ios::app);
	if (debug.is_open())
		debug<<"SEEK "<<fname<<" offset:"<<offset<<" dir:"<<dir
		<<" extSize:"<<extSize<<" pos:"<<pos<<" extPos:"<<extPos
		<<" ret:"<<ret<<std::endl;
	debug.close();*/
	if (((std::fstream *)handler)->fail()) {
		std::cout<<"Warning: Bad seek in file."<<std::endl;
		}
	if (ret == -1L)
		{
		std::cout<<"Warning: XStream::seek applies when the end of the file reached. "<<fname<<std::endl;
		return -1L;
		}

	if (ret >= size() - 1) eofFlag = 1;  else eofFlag = 0;




	return pos = ret;
}

void XStream::flush()
{
	/* Full stream debug
	std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
	if (debug.is_open())
		debug<<"FLUSH "<<fname<<std::endl;
	debug.close();
	*/
	//if (!FlushFileBuffers(handler) && ErrHUsed)
	//	ErrH.Abort(flushMSG,XERR_USER,GetLastError(),fname);
	((std::fstream *)handler)->flush();
}

long XStream::size()
{
//std::cout<<"XStream::size()"<<std::endl;
	long tmp = extSize;
	long int tmp2;
	if(tmp == -1){
		//tmp=GetFileSize(handler,0);
		if (((std::fstream *)handler)->flags() & std::ios::in) {
			tmp2 = ((std::fstream *)handler)->tellg();
			((std::fstream *)handler)->seekp(0, std::ios_base::end);
			tmp = ((std::fstream *)handler)->tellg();
			((std::fstream *)handler)->seekp(tmp2);
		} else {
			tmp2 = ((std::fstream *)handler)->tellp();
			((std::fstream *)handler)->seekg(0, std::ios_base::end);
			tmp = ((std::fstream *)handler)->tellp();
			((std::fstream *)handler)->seekg(tmp2);
		}
		if (tmp == -1L) {
			if (ErrHUsed)
				ErrH.Abort(sizeMSG,XERR_USER,0,fname.c_str());
			else
				return -1;
		}
	}
	/* Full stream debug
	std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
	if (debug.is_open())
		debug<<"SIZE "<<fname<<" size:"<<tmp<<std::endl;
	debug.close();
	*/
	return tmp;
}

