
#define E_LBMREEV	23

#define E_MAINMAP	24
#define E_LAYERMENU	25
#define E_MAINMENU	26
#define E_PRMMENU	27
#define E_MUFLER	28

#define E_OPENFORM     100
#define E_SAVEFORM     101
#define E_IMPCHOICE    102
#define E_BMPCHOICE    103
#define E_IMPSPHERE    104
#define E_IMPPOLYGON   105
#define E_IMPPEAK      106
#define E_TRACKFORM    107
#define E_GOTOFORM     108
#define E_PLACEBMPFORM 109
#define E_IBMFORM 110
#define E_MOSAICFORM 111
#define E_FORM3D	112
#define E_MOBILOCMENU	113
#define E_MOBILOCFORM	114
#define E_MOBILOCSHOWCHOICE	115
#define E_MOBILOCDELETECHOICE	116
#define E_MOBILOCEDITCHOICE	117
#define E_MOBILOCCHOICE	118
#define E_WORLDCHOICE	119
#define E_VALOCMENU	120
#define E_VLTNTFORM	121
#define E_VLMLCFORM	122
#define E_VLSNSFORM	123
#define E_VLDNGFORM	124
#define E_MOBILOCFRAMECHOICE	125

#define PRM_ROTMAP	0x0001
#define PRM_INFO	0x0002

struct iGameMap : sqInputBox {
	int xc,yc;		// xmouse - xc + XC
	int xside,yside;
	int xsize,ysize;
	int _xI,_yI;

	int CX,CY;
	int DX,DY;
	int TurnAngle;
	int TurnD;
	int TurnSecX;
	int prmFlag;
	int FirstDraw;

	int isreEvent,reEvent;

	sqTextButton* knob3D;
	sqTextButton* knobAlt;
	sqTextButton* knobRender;
	sqTextButton* knobTrack;
	sqTextButton* knobSpline;
	sqTextButton* knobBuild;
	sqTextButton* knobBuildAll;
	sqTextButton* knobLinking;

		iGameMap(sqElem* _owner,int _x,int _y,int _xside,int _yside,char* _title);
		~iGameMap(void){ }

	virtual void message(int code,sqElem* object = NULL);
	virtual void keytrap(int key);
	virtual void draw(int self = 1);
	virtual void flush(void);
	virtual void quant(void);
	virtual void handler(sqEvent* e);

	void reset(void);
	void change(int Dx,int Dy);
	void gotoxy(int x,int y);
	void shift(int dx,int dy){ gotoxy(CX + dx,CY + dy); }

	void setFloodLvl(void);
	void restoreFloodLvl(void);
	};

extern iGameMap* curGMap;

struct iMainMenu : sqInputBox {
	sqPopupMenu* menu;
	int x0,y0;
	static int copt;

		iMainMenu(sqElem* _owner,int _x,int _y);

	virtual void message(int code,sqElem* object = NULL);
	virtual void flush(void);
	};

struct iPrmMenu : sqInputBox {
	sqPopupMenu* menu;
	int x0,y0;
	XBuffer* buf;
	static int copt;

		iPrmMenu(sqElem* _owner,int _x,int _y);
		~iPrmMenu(void){ delete buf; }

	virtual void message(int code,sqElem* object = NULL);
	virtual void flush(void);
	virtual void keytrap(int key);
	};

struct iLayerMenu : sqInputBox {
	sqPopupMenu* menu;
	int x0,y0;
	XBuffer* buf;
	static int copt;

		iLayerMenu(sqElem* _owner,int _x,int _y);
		~iLayerMenu(void){ delete buf; }

	virtual void message(int code,sqElem* object = NULL);
	virtual void flush(void);
	virtual void keytrap(int key);
	};

struct iInputForm : sqInputBox {
	sqField* name;
	int x0,y0;
	int mode;

	sqField* params[256];
	sqPopupMenu* menu;
	static int copt;

		iInputForm(sqElem* _owner,int _x,int _y,int _mode);

	virtual void message(int code,sqElem* object = NULL);
	virtual void flush(void);
	virtual void quant(void);
	};

void SetMCursor(int mode = 0);

struct ibmFile {
	int mapx_center,mapy_center;
	int x_center,y_center;
	int x_size,y_size;
	int x_side,y_side;
	int level;
	uchar* image;
	int counter,fcnt;
	int shoting;

	void save(int _mapx_center,int _mapy_center,int _x_center,int _y_center,int _x_size,int _y_size,int _x_side,int _y_side,int _level,int _counter);
	void quantSave(void);
	};

