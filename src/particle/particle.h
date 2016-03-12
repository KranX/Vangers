const int  PARTICLE_PROCESS_COLOR_BITS = 4;
//const int  PARTICLE_PROCESS_COLOR_FIRST = 128;
const int  PARTICLE_PROCESS_COLOR_FIRST = 128;     // 64 - 32;
//255 - 64*2;


const int  PARTICLE_PROCESS_COLOR_MAX = 1 << PARTICLE_PROCESS_COLOR_BITS;

extern unsigned BogusRNDVAL;
inline unsigned BogusRND(unsigned m){
	BogusRNDVAL ^= BogusRNDVAL >> 3;
	BogusRNDVAL ^= BogusRNDVAL << 28;
	BogusRNDVAL &= 0x7FFFFFFF;

	if(!m) return 0;

	return BogusRNDVAL%m;
}

const int  PARTICLE_SHIFT = 2;
const int  PARTICLE_SIZE = 1<<PARTICLE_SHIFT;
struct ParticleProcess;

struct Particle{
	Particle* next,*prev;
	Particle* nextInCol,*prevInCol;
	int X,Y;
	Particle* LU,*U,*RU;
	Particle* L,	*R;
	Particle* LD,*D,*RD;
	unsigned char Value;
	int NewValue;
	int Protected;
	int Age;
	unsigned char BackGround[16];
	int Level;

	void ins(Particle* np);
	void del();
	void insInCol(Particle* np,ParticleProcess* proc);
	void delFromCol(ParticleProcess* proc);
	Particle* b_link(ParticleProcess*);
	void b_unlink();
	void process(ParticleProcess* proc);
	void purge(ParticleProcess* proc);
	void express(ParticleProcess* proc);

	void BackRestore(void);
	void GetBackGround(void);
};


struct ParticleProcess : BackgroundElement{
	Particle* heap;
	Particle* heap_end;
	int ActivN;
	int InActivN;
	int MaxParticle;
	int ListExhausted;

	uchar PaletteColor;
	int CurrNhotSpots;
	int NhotSpots;
	int HotCenterX;
	int HotCenterY;
	int AverageSpeed;
	int XHotSpotArea;
	int YHotSpotArea;
	//Mask Motion;
	int HotCenterXdelta;
	int HotCenterYdelta;
	int cosSpeed;
	int sinSpeed;
	Particle* NewParticles;
	Particle** ParticlesCol;
	Particle ActivList;
	Particle InActivList;

	int init(int m,int pal_col1,int n_h_s);
	void finit();
	void reset();
	void test(void);
//	void set_hot_center();
	void process();
	void express();
	void purge();
	Particle* AddRandom(int x,int y);
	Particle* AddNear(int x,int y,Particle* cr,int cr_dir);
	void quant1();
	void set_hot_spot(int,int,int = 255,int = 1);
	void quant2();

	void BackRestore(void);
	void GetBackGround(void);
};


//const int  _Xmax = 640;
//const int  _Ymax = 480;
extern uchar *ParticlePaletteTableDust;
extern uchar *ParticlePaletteTableSmog;
void ParticlePaletteTableInit( char unsigned *pal);
