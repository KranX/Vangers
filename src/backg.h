#ifndef __BACKG_H__
#define __BACKG_H__

struct BackgroundElement
{
	virtual void BackRestore(void);
};

struct BackgroundDispatcher
{
	BackgroundElement** BkData;
	int point;
	int size;

	void init(int sz = 100);
	void free(void);
	void close(void);
	void put(BackgroundElement* p);
	void restore(void);
};

extern BackgroundDispatcher BackD;

#endif