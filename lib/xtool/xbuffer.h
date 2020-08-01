#ifndef __XBUFFER_H
#define __XBUFFER_H

#ifndef NULL
#define NULL	0L
#endif

#define XB_DEFSIZE	256

#define XB_BEG		0
#define XB_CUR		1
#define XB_END		2

#define XB_FORWARD	0
#define XB_BACKWARD	1
#define XB_GLOBAL	2

#define XB_CASEON	0
#define XB_CASEOFF	1

#define XB_DEFRADIX	10
#define XB_DEFDIGITS	8

struct XBuffer
{
	char* buf;
	unsigned int size;
	unsigned int offset;
	int radix;
	int digits;
	int MakeFree;
	int automatic_realloc;

	//Diffrent with _xtool.h in Source
	//XBuffer(unsigned int sz=XB_DEFSIZE, int automatic_realloc_=0)
	XBuffer(unsigned int sz=XB_DEFSIZE)
		{ 
		alloc(sz);
		radix=XB_DEFRADIX;
		digits=XB_DEFDIGITS;
		//Diffrent with _xtool.h in Source
		//automatic_realloc=automatic_realloc_; 
		}
	XBuffer(void* p,int sz);
	~XBuffer(void){ free(); }

	void SetRadix(int r) { radix = r; }
	void SetDigits(int d) { digits = d; }

	void alloc(unsigned int sz);
	void free(void);
	void fill(char fc = '\0');
	void set(int off,int mode = XB_BEG);
	void init(void){ offset = 0; *buf = 0; }
	int search(char* what,int mode = XB_FORWARD,int cs = XB_CASEON);
	int search(const char* what,int mode = XB_FORWARD,int cs = XB_CASEON);
	int end() const { return (offset > size); }

	unsigned int tell(void) const { return offset; }
	unsigned int length(void) const { return size; }
	char* address(){ return buf; }

	unsigned int read(void* s, unsigned int len);
	unsigned int write(const void* s, unsigned int len, int bin_flag = 1);
	void handleOutOfSize();
	
	XBuffer& operator< (const char* v);
	XBuffer& operator< (char v) { return write(v); }
	XBuffer& operator< (unsigned char v) { return write(v); }
	XBuffer& operator< (short v) { return write(v); }
	XBuffer& operator< (unsigned short v) { return write(v); }
	XBuffer& operator< (int v ) { return write(v); }
	XBuffer& operator< (unsigned int v) { return write(v); }
	XBuffer& operator< (long v) { return write(v); }
	XBuffer& operator< (unsigned long v) { return write(v); }
	XBuffer& operator< (float v) { return write(v); }
	XBuffer& operator< (double v) { return write(v); }
	XBuffer& operator< (long double v) { return write(v); }

	XBuffer& operator> (char* v);
	XBuffer& operator> (char& v) { return read(v); }
	XBuffer& operator> (unsigned char& v) { return read(v); }
	XBuffer& operator> (short& v) { return read(v); }
	XBuffer& operator> (unsigned short& v) { return read(v); }
	XBuffer& operator> (int& v) { return read(v); }
	XBuffer& operator> (unsigned int& v) { return read(v); }
	XBuffer& operator> (long& v) { return read(v); }
	XBuffer& operator> (unsigned long& v) { return read(v); }
	XBuffer& operator> (float& v) { return read(v); }
	XBuffer& operator> (double& v) { return read(v); }
	XBuffer& operator> (long double& v) { return read(v); }

	XBuffer& operator<= (char);
	XBuffer& operator<= (unsigned char);
	XBuffer& operator<= (short);
	XBuffer& operator<= (unsigned short);
	XBuffer& operator<= (int);
	XBuffer& operator<= (unsigned int);
	XBuffer& operator<= (long);
	XBuffer& operator<= (unsigned long);
	XBuffer& operator<= (long long int);
	XBuffer& operator<= (float);
	XBuffer& operator<= (double);
	XBuffer& operator<= (long double);

	XBuffer& operator>= (char&);
	XBuffer& operator>= (unsigned char&);
	XBuffer& operator>= (short&);
	XBuffer& operator>= (unsigned short&);
	XBuffer& operator>= (int&);
	XBuffer& operator>= (unsigned int&);
	XBuffer& operator>= (long&);
	XBuffer& operator>= (unsigned long&);
	XBuffer& operator>= (float&);
	XBuffer& operator>= (double&);
	XBuffer& operator>= (long double&);

	//Diffrent with _xtool.h
	//operator char* (){ return buf; }
	char* GetBuf() { return buf; }
	char* operator ()(int offs){ return buf + offs; }
	char* operator ()(unsigned int offs){ return buf + offs; }
	XBuffer& operator++(){ offset++; return *this; }
	XBuffer& operator--(){ offset--; return *this; }
	XBuffer& operator+= (int v){ offset+=v; return *this; }
	XBuffer& operator-= (int v){ offset-=v; return *this; }
	XBuffer& operator+= (unsigned int v){ offset += v; return *this; }
	XBuffer& operator-= (unsigned int v){ offset -= v; return *this; }

	char operator[](int ind) const { return buf[ind]; }
	char operator()() const { return buf[offset]; }

	char& operator[](int ind){ return buf[ind]; }
	char& operator()(){ return buf[offset]; }

	template<class T> XBuffer& write(const T& v){ while(offset + sizeof(T) >= size) handleOutOfSize(); (T&)buf[offset] = v; offset += sizeof(T); return *this; }
	template<class T> XBuffer& read(T& v) {
		v = (T&)buf[offset];
		offset += sizeof(T);
		return *this;
	}
};

#endif /* __XBUFFER_H */

