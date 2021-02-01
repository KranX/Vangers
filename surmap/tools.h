extern int MosaicTypes[8];

struct BitMap {
	ushort sx,sy;
	int sz;
	uchar* data;
	uchar* palette;
	int force,mode,border,copt;
	int mosaic;
	int size,level;
	int xoffset,yoffset;

		BitMap(int _mosaic = 0);

	void load(char* name);
	void convert(void);
	void place(char* name,int _force,int _mode,int _border = 0,int _level = 0,int _size = 0);
	inline int getDelta(int x,int y,int delta){
		x += xoffset;
		y += yoffset;
		if(!mode) return delta*(64 - data[(y%sy)*sx + (x%sx)])/64;
		return delta + force*(64 - data[(y%sy)*sx + (x%sx)])/64;
		}
	inline int getType(int x,int y){ 
		x += xoffset;
		y += yoffset;
		return MosaicTypes[data[(y%sy)*sx + (x%sx)]%8]; 
		}
	};

extern BitMap placeBMP;
extern BitMap mosaicBMP;
extern int curBmpIndex;

