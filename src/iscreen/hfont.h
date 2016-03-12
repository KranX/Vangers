#ifndef __HFONT_H__
#define __HFONT_H__

const int 	HFONT_DELTA		= 20;
const int 	HFONT_NULL_LEVEL	= 128;

const int 	NULL_HCHAR		= 0x01;

const int 	FONT_LOADED		= 0x01;
const int 	FONT_COLORS		= 0x02;

struct HChar
{
	short SizeX;
	short SizeY;
	int Size;
	int Flags;

	short RightOffs;
	short LeftOffs;

	short MinHeight;
	short MaxHeight;

	unsigned char* HeightMap;

	void calcHLimits(void);
	void calcOffsets(void);

	void put(int x,int y);
	int null_check(void);

	void init(int sx,int sy,int fl);

	HChar(int sx,int sy,int fl);
	~HChar(void);
};

struct HFont
{
	short SizeX;
	short SizeY;

	int Flags;

	short StartChar;
	short EndChar;
	int NumChars;

	HChar** data;

	int dataHeapSize;
	char* data_heap;

	int memHeapSize;
	char* mem_heap;

	void alloc_mem(void);
	void alloc_char_mem(void);
	void free_mem(void);

	void save(char* fname);
	void load(char* fname);

	void scan_chars(char* fname);

	void put_char(int x,int y,int ch);

	HFont(void){ Flags = 0; };
	HFont(int sx,int sy,int sc,int ec);
	~HFont(void);
};

/*
void prepare_hfont(void);
void init_hfont(void);
void get_hfont(void);
void put_hfont(void);
void bml2fnt(void);
*/

extern HFont* hFnt;
extern char* HFntName;
extern char* HBmlName;

#endif