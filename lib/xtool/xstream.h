#ifndef __XSTREAM_H
#define __XSTREAM_H

#include <fstream>

#define XS_IN		0x0001
#define XS_OUT		0x0002
#define XS_NOREPLACE	0x0004
#define XS_APPEND	0x0008
#define XS_NOBUFFERING	0x0010
#define XS_NOSHARING	0x0020

#define XS_SHAREREAD	0x0040
#define XS_SHAREWRITE	0x0080

#define XS_BEG		0
#define XS_CUR		1
#define XS_END		2

#define XS_DEFRADIX	10
#define XS_DEFDIGITS	8

struct XStream
{
	typedef std::fstream* XSHANDLE;

	XSHANDLE handler;
	long	pos;
	int	eofFlag;
	int	ErrHUsed;
	//const char* fname;
	std::string fname;
	int	radix, digits;
	long	extSize;
	long	extPos;
	std::string file_name;
	char _ConvertBuffer[_CONV_BUFFER_LEN + 1];

	XStream(int err = 1);
	XStream(const char* name, unsigned flags,int err=1) {
		ErrHUsed = err;
		handler  = NULL;//(XSHANDLE)-1;
		eofFlag  = 1;
		radix	 = XS_DEFRADIX;
		digits	 = XS_DEFDIGITS;
		extSize  = -1;
		extPos	 = 0;
		open(name,flags);
	}
	~XStream();

	int	open(const char* name, unsigned f = XS_IN);
	int	open(XStream* owner,long start,long ext_sz = -1);
	void	close();
	unsigned long read(void* buf, unsigned long len);
	unsigned long write(const void* buf, unsigned long len);
	long	seek(long offset, int dir);
	long	tell() const { return pos; }
	char*	getline(char* buf, unsigned len);
	int	eof(){ return eofFlag || pos >= size(); }
	long	size();

	XSHANDLE gethandler(){ return handler; }


	void	flush();
	const char*	GetFileName() const { return fname.c_str(); }
	void	SetRadix(int r){ radix=r; }
	void	SetDigits(int d){ digits=d; }

	XStream& operator< (const char*);
	XStream& operator< (char);
	XStream& operator< (unsigned char);
	XStream& operator< (short);
	XStream& operator< (unsigned short);
	XStream& operator< (int);
	XStream& operator< (unsigned int);
	XStream& operator< (long);
	XStream& operator< (unsigned long);
	XStream& operator< (float);
	XStream& operator< (double);
	XStream& operator< (long double);

	XStream& operator> (char*);
	XStream& operator> (char&);
	XStream& operator> (unsigned char&);
	XStream& operator> (short&);
	XStream& operator> (unsigned short&);
	XStream& operator> (int&);
	XStream& operator> (unsigned int&);
	XStream& operator> (long&);
	XStream& operator> (unsigned long&);
	XStream& operator> (float&);
	XStream& operator> (double&);
	XStream& operator> (long double&);

	XStream& operator<= (char);
	XStream& operator<= (unsigned char);
	XStream& operator<= (short);
	XStream& operator<= (unsigned short);
	XStream& operator<= (int);
	XStream& operator<= (unsigned int);
	XStream& operator<= (long);
	XStream& operator<= (unsigned long);
	XStream& operator<= (float);
	XStream& operator<= (double);
	XStream& operator<= (long double);

	XStream& operator>= (char&);
	XStream& operator>= (unsigned char&);
	XStream& operator>= (short&);
	XStream& operator>= (unsigned short&);
	XStream& operator>= (int&);
	XStream& operator>= (unsigned int&);
	XStream& operator>= (long&);
	XStream& operator>= (unsigned long&);
	XStream& operator>= (float&);
	XStream& operator>= (double&);
	XStream& operator>= (long double&);

	int operator! (){ if(handler != XSHANDLE(-1)) return 1; else return 0; }
	operator void* (){ if(handler != XSHANDLE(-1)) return 0; else return this; }

	template<class T> XStream& write(const T& v){ write(&v, sizeof(T)); return *this; }
	template<class T> XStream& read(T& v){ read(&v, sizeof(T)); return *this; }
};

#endif /* __XSTREAM_H */


