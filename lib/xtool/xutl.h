#ifndef __XUTIL_H
#define __XUTIL_H

void* xalloc(unsigned sz);
void* xrealloc(void* p,unsigned sz);
void  xfree(void* p);

#define XALLOC(a)	xalloc(a)
#define XREALLOC(a,b)	xrealloc(a,b)
#define XFREE(a)	xfree(a)

//unsigned int XRnd(unsigned int m);
void XRndSet(unsigned int m);
unsigned int XRndGet();

char* XFindNext(void);
char* XFindFirst(char* mask);

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifndef __ROUND__
#define __ROUND__

// TODO(amdmi3): both old and new code depends on current rounding mode;
// should use lrint family functions actually
/*#ifdef _WIN32
#include <cmath>

inline int round(double x)
{
	return std::floor(x + 0.5);
}
#endif*/

/*
__forceinline int round(double x)
{
	return (int)(x);
}

__forceinline int round(float x)
{
	return (int)(x);
}
*/
template <class T> 
inline T sqr(const T& x){ return x*x; }

template <class T> 
inline int SIGN(const T& x) { return x ? (x > 0 ? 1 : -1 ) : 0; }

#endif //__ROUND__

/*// (amdmi3): returns number of first bit set in abs(x)
inline int BitSR(int x)
{
	int a = abs(x);
	for (int i = 31; i >= 0; i--)
		if (a & (1 << i))
			return 31-i;

	// TODO(amdmi3): in case of x = 0 result of `bsr' instruction is undefined.
	// if code relies on specific behavior when x==0 implement it
	return 0;
}*/

inline bool IsPowerOf2(int x)
{
	return (x) && !(x & (x - 1));
}

// TODO(amdmi3): very inefficient; rewrite
inline int BitSR(int x) {
	unsigned int a = abs(x);
	for (int i = 31; i > 0; i--)
		if (a & (1 << i))
			return i;

	return 0;
}

void initclock(void);
int clocki(void);
double clockf(void);

void xtDeleteFile(char* fname);

#endif
