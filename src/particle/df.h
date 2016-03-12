const char DEFORM_WATER_ONLY = 0;
const char DEFORM_ALL = 1;

typedef short WAVE_TYPE;

struct WaveProcess
{
	short sx,sy;
	int NumFrame;

	WAVE_TYPE* offset;
	char* x_offset;
	char* y_offset;

	uchar* buff;
	int size,g_size;
	short bx,by;
	//int x_min,y_min,x_max,y_max;

	void Init(char* filename);
	void Free(void);
	void Make(void);
	int CheckOffset(int off);
	void Deform(int x,int y,int& off,char fl);
	void Show(int x,int y);
};

struct FireBallProcess
{
	int NumFrames;
	int x_size,y_size;
	int FrameSize,DataSize;
	uchar* Data;
	int cTable;

	char CheckOut(int& f);
	void Load(char* filename);
	void Free(void) {
		delete[] Data; 
	};
	void Activate(int& frame) { frame = 0; };
	void Show(int x,int y,int z,int scale,int& frame);
	void Show2(int x,int y,int& frame);
	void Save(char* filename);
	void Convert(char* filename);
};

void DrawHLine(int x0,int y0,int x1,uchar* c);
void DrawVLine(int x0,int y0,int y1,uchar* c);
void DrawHFLine(int x0,int y0,int x1,uchar* c,int h);
void ShowSigmaBmp(int x,int y,int sx,int sy,uchar* b,uchar* t);

/*void TeleportShowPrepare(void);
void TeleportShowFree(void);
void TeleportShowProcess(int x,int y,uchar* p);
void TeleportShowQuant(void);
void TeleportShowStart(void);*/


