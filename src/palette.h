
struct PaletteTransform {
	uchar* from;
	uchar* to;
	char* delta;
	int n0,sz;
	int inProgress;
	int* event;

		PaletteTransform(void) {
			from = new uchar[768];
			to = new uchar[768];
			delta = new char[768];
		};
		~PaletteTransform(void){
			delete[] from;
			delete[] to;
			delete[] delta;
		}

	void set(uchar* _from, uchar* _to, int _n0, int _sz, int* _event = NULL);
	int quant(void);
};

void PalettePrepare(void);
void PalEvidence(char* tpal,char* pal);
void PalSlake(char* pal);

extern PaletteTransform* palTr;

void pal_iter(void);
void pal_iter_init(void);

extern int PAL_MAX;
extern int PAL_WAVE_TERRAIN;
extern int PAL_TERRAIN[TERRAIN_MAX];
extern int PAL_SPEED[TERRAIN_MAX];
extern int PAL_AMPL[TERRAIN_MAX];
extern int PAL_RED[TERRAIN_MAX];
extern int PAL_GREEN[TERRAIN_MAX];
extern int PAL_BLUE[TERRAIN_MAX];
