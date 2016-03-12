#define TABLE_TYPE short
#define FRACT_TYPE unsigned char
const int  FRACT_BITS = sizeof(FRACT_TYPE)*8;
const int  FRACT_AMPL = 1 << FRACT_BITS;
struct Mask {
	short mx,my;
	short bx,by;
	int n_frame;
	TABLE_TYPE* offset;
	unsigned char* buffer;
	char MSSfile[32];

	Mask(void);
	void load(const char*);
	void load_to_flame(char*);
	void deform(int fr,unsigned char* from,unsigned char* to);
	void deformBuff(int fr,int x,int y,unsigned char* buff);
	void refresh(int x,int y);
	void BackRestore(int c_x,int c_y,unsigned char* buff);
	void finit( void );
	};

#ifdef EXTERNAL_USE
struct DeformProcess : BackgroundElement{
#else
struct DeformProcess{
#endif
	int OrX,OrY;
	static Mask* DeformMask;
	unsigned char* buffer;
	int phase;
	void init();
	void activate();
	int deform(int d_phase,int x,int y);
	void BackRestore(void);
	};

