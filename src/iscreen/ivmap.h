#ifndef __IVMAP_H__
#define __IVMAP_H__



const uint H_POWER = MAP_POWER_X;
const uint H_SIZE = 1 << H_POWER;
const uint H2_SIZE = 2*H_SIZE;
const int MAX_LINE = 604;

struct ivrtNode { ivrtNode* next; };
struct ivrtMap {
	uchar** lineT;
	uchar** lineTcolor;

	int upLine,downLine;

	uchar* dHeap;
	int freeMax,freeTail;
	ivrtNode* freeNodes;

	uchar* dHeap_c;
	int freeMax_c,freeTail_c;
	ivrtNode* freeNodes_c;

	XStream fmap;
	int foffset,offset;
	char* iniName;
	char* fileName;
	char* paletteName;
	char* fname;

	int isCompressed;
	int* st_table;
	short* sz_table;
	uchar* inbuf;

		ivrtMap(void);

	void init(void);
	void finit(void);
	void load(const char* name);
	void accept(int up,int down);

	inline uchar* use(void);
	inline uchar* use_c(void);

	void analyzeINI(const char* name);
	void fileLoad(void);
	};

extern ivrtMap* ivMap;
#endif
