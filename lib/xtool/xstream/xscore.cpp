#include "xglobal.h"

XStream::XStream(int err)
{
	ErrHUsed = err;
	handler  = NULL;
	eofFlag  = 1;
	radix	 = XS_DEFRADIX;
	digits	 = XS_DEFDIGITS;
	extSize  = -1;
	extPos	 = 0;
}

XStream::~XStream()
{
	close();
}

/*
XStream::XStream(const char* name, unsigned flags,int err)
{
	ErrHUsed = err;
	handler  = INVALID_HANDLE_VALUE;
	eofFlag  = 1;
	radix	 = XS_DEFRADIX;
	digits	 = XS_DEFDIGITS;
	extSize  = -1;
	extPos	 = 0;
	open(name,flags);
}
*/

