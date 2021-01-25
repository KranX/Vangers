/*
		  Simple-Quick Interface by K-Division::KranK
			  All Rights Reserved (C)1995
*/

#ifndef _SQINT_
#define _SQINT_

#ifdef _SURMAP_
#define _SELFDRAW_
#endif

/* ------------------------- Changeable parameters -------------------------- */

/* ----------------------------- Useful macros ------------------------------ */

#ifndef MIN
#define MIN(a,b)	(((a) < (b))?(a):(b))
#define MAX(a,b)	(((a) > (b))?(a):(b))

#define CLOCK()		(clock()*18/CLOCKS_PER_SEC)

#endif

/* ---------------------------- Internal consts ----------------------------- */
#define E_REGION	0
#define E_COMMON	1

#define E_REFRESH	10

#define E_LBMPRESS	11
#define E_RBMPRESS	12
#define E_LBMUNPRESS	13
#define E_RBMUNPRESS	14
#define E_LBMMOV	15
#define E_RBMMOV	16
#define E_LBMDOUBLE	17
#define E_RBMDOUBLE	18

#define M_NEXTOBJ	1
#define M_PREVOBJ	2
#define M_SETOBJ	3
#define M_DEACTIVATE	4
#define M_CHOICE	5
#define M_SETOPTION	6
#define M_INSERT	7
#define M_DELETE	8
#define M_CHANGEOPTION	9
#define M_SETALL	10
#define M_UNSETALL	11
#define M_ACCEPT	12
#define M_CANCEL	13

#define T_STRING	1
#define T_NUMERIC	2
#define T_FIELD    0x0100

/* -------------------------------- Structs --------------------------------- */
extern int SQ_SYSCOLOR;
const int SYSCOLOR = 159;

struct sqFont {
	short num;
	unsigned char first,last;
	short sx,sy;
	void** data;

	void init(void* d);
	void draw(int x,int y,unsigned char* s,int fore = SQ_SYSCOLOR,int back = SQ_SYSCOLOR + 5);
	void draw(int x,int y,char* s,int fore = SYSCOLOR,int back = -1){ draw(x,y,(unsigned char*)s,fore,back); }
	void drawtext(int x,int y,char* s,int fore = SQ_SYSCOLOR,int back = SQ_SYSCOLOR + 5);
	void drawchar(int x,int y,int ch,int fore = SQ_SYSCOLOR,int back = SQ_SYSCOLOR + 5);
	};

struct sqEvent {
	int code;
	int type;
	int x,y;
	void* pointer;
	int value;
	};

struct sqEventQueue {
	sqEvent* queue;
	int max;
	int top,bottom;

		sqEventQueue(int _max){ queue = new sqEvent[max = _max]; top = bottom = 0; }

	void put(int _code);
	void put(int _code,int _type,int _x,int _y);
	void put(int _code,int _type,int _x,int _y,void* _pointer,int _value);
	sqEvent* get(void);
	};

struct sqElem {
	sqElem* owner;
	sqElem* tail;
	sqElem* next;
	sqElem* prev;
	int dxl,dyl,dxr,dyr;
	int xl,yl,xr,yr;
	int height;

		sqElem(int _height = 2,int _dxl = -1,int _dyl = -1,int _dxr = 1,int _dyr = 1);
		virtual ~sqElem(void){}

	sqElem& operator+ (sqElem* e);
	sqElem& operator- (sqElem* e);
	void reject(void);

	virtual void draw(int self = 1);
	virtual int* getXY(void);
	virtual void handler(sqEvent* e);
	virtual void accept(void);
	virtual void restore(int _x,int _y,int _sx,int _sy);
	virtual void remove(void);
	virtual int process(sqEvent* event);
	virtual void keytrap(int key);
	virtual void quant(void);
	virtual void message(int code,sqElem* object = NULL);
	virtual void activate(int force = 1);
	virtual void deactivate(void);
	virtual void flush(void);

	void set(int _height = 2);
	int ishere(int x,int y);
	int getX(void){ if(owner) return owner -> getX() + owner -> xl - owner -> dxl; return 0; }
	int getY(void){ if(owner) return owner -> getY() + owner -> yl - owner -> dyl; return 0; }
	int getRX(void){ if(owner) return owner -> getX() + owner -> xr + owner -> dxr; return 0; }
	int getRY(void){ if(owner) return owner -> getY() + owner -> yr + owner -> dyr; return 0; }
	};

struct sqScreen : sqElem {
	int x,y,sx,sy;

		sqScreen(int _x,int _y,int _sx,int _sy);

	virtual int* getXY(void){ return &x; }
	virtual void keytrap(int key);
	virtual void handler(sqEvent* e);
	virtual void draw(int self = 1);
	};

struct sqBox : sqElem {
	int x,y,sx,sy;

		sqBox(sqElem* _owner,int _x,int _y,int _sx,int _sy)
			{ x = _x; y = _y; sx = _sx; sy = _sy; set(); }

	virtual int* getXY(void){ return &x; }
	virtual void draw(int self = 1);
	virtual void restore(int _x,int _y,int _sx,int _sy);
	};

struct sqButton : sqElem {
	int x,y,sx,sy;
	int pressed;
	int shift;

		sqButton(sqElem* _owner,int _x,int _y,int _sx,int _sy,int _shift = 1,int _pressed = 0);

	virtual int* getXY(void){ return &x; }
	virtual void draw(int self = 1);
	virtual void handler(sqEvent* e);
	};

struct sqTextButton : sqElem {
	int x,y,sx,sy;
	char* text;
	sqFont* font;
	int offset;

	sqTextButton(sqElem* _owner,int _x,int _y,const char* _text,sqFont* _font,int _sx = 0);
	~sqTextButton();

	virtual int* getXY(void){ return &x; }
	virtual void draw(int self = 1);
	virtual void handler(sqEvent* e);
	virtual void keytrap(int key);
	virtual void activate(int force = 1);
	virtual void deactivate(void);
	virtual void remove(void);
	};

struct sqInputString : sqElem {
	int x,y;
	int size,sy;
	sqFont* font;
	int len,clen;
	int offset;
	unsigned char* str,*savestr;
	int index;
	int blink;
	int fstatus;
	int type,dec;

	static int insert,rus;

		sqInputString(sqElem* _owner,int _x,int _y,int _size,sqFont* _font,unsigned char* _str = NULL,int _len = 0,int _type = T_STRING,int _dec = 0);
		virtual ~sqInputString(void){ delete str; delete savestr; }

	void delcursor(void);
	void setstr(unsigned char* s);
	char* getstr(void);

	virtual int* getXY(void){ return &x; }
	virtual void draw(int self = 1);
	virtual void keytrap(int key);
	virtual void quant(void);
	virtual void handler(sqEvent* e);
	virtual void activate(int force = 1);
	virtual void deactivate(void);
	};

struct sqField : sqInputString {
	unsigned char* prompt;
	int x0,y0;

		sqField(sqElem* _owner,const char* _prompt,int _x,int _y,int _size,sqFont* _font,unsigned char* _str = NULL,int _len = 0,int _type = T_STRING,int _dec = 0);
		~sqField(void){ free(prompt); }

	virtual void draw(int self = 1);
	};

struct sqPopupMenu;
struct sqMenuBar {
	unsigned char* data;
	unsigned char* original_data;
	sqMenuBar* next,*prev;
	int status,value,rec;

		sqMenuBar(const unsigned char* s,sqPopupMenu* owner,int _status = 0,int _value = 0,int _rec = -1);
		~sqMenuBar(void){ delete[] data; free(original_data); }

	void replace(sqPopupMenu* owner,unsigned char* s);
	};

struct sqPopupMenu : sqElem {
	int x,y,sx,sy;
	int visibars,maxbars;
	sqMenuBar* first;
	sqMenuBar* pointer;
	sqMenuBar* insert;
	int margin,len,len0;
	int topbar,nbar;
	sqFont* font;
	unsigned char* seekbuf;
	int seeklen;
	unsigned int seekcounter;

		sqPopupMenu(sqElem* _owner,int _x,int _y,int _visibars,sqFont* _font,int _len0 = 0,int _margin = 1);
		virtual ~sqPopupMenu(void){ free(); delete[] seekbuf; }

	sqPopupMenu& operator* (sqMenuBar* obj);
	void disconnect(sqMenuBar* obj,sqMenuBar** del = NULL);
	void setlen(int l);
	sqMenuBar* getbar(int n);
	int getptr(sqMenuBar* b);
	void seek(sqMenuBar* p0,int force = 1);
	void setpointer(sqMenuBar* p,int force = 1);
	void free(void);

	virtual int* getXY(void){ return &x; }
	virtual void draw(int self = 1);
	virtual void keytrap(int key);
	virtual void handler(sqEvent* e);
	virtual void activate(int force = 1);
	virtual void deactivate(void);
	};

struct sqMultiMenu : sqElem {
	int x,y,sx,sy;
	sqInputString* edit;
	sqPopupMenu* list;
	sqElem* obj;
	int maxlen;
	sqMenuBar* deleted;

		sqMultiMenu(sqElem* _owner,int _x,int _y,int _visibars,sqFont* _font,int _maxlen);

	virtual int* getXY(void){ return &x; }
	virtual void keytrap(int key);
	virtual void restore(int _x,int _y,int _sx,int _sy);
	virtual void activate(int force = 1);
	virtual void deactivate(void);
	virtual void message(int code,sqElem* object = NULL);
	};

struct sqFiler : sqElem {
	int x,y,sx,sy;
	sqInputString* skel;
	sqPopupMenu* list;
	unsigned int origindrive,max;
	char* origindir;
	unsigned int curdrive;
	char* curdir;
	sqElem* obj;
	char* dirskel;

		sqFiler(sqElem* _owner,int _x,int _y,int _visibars,sqFont* _font);
		virtual ~sqFiler(void);

	void build(void);
	void setdirskel(void);
	void choice(void);

	virtual int* getXY(void){ return &x; }
	virtual void keytrap(int key);
	virtual void restore(int _x,int _y,int _sx,int _sy);
	virtual void activate(int force = 1);
	virtual void deactivate(void);
	virtual void message(int code,sqElem* object = NULL);
	};

struct sqInputBox : sqBox {
	sqElem* obj = nullptr;
	char* title;
	sqFont* font;

		sqInputBox(sqElem* _owner,int _x,int _y,int _sx,int _sy,sqFont* _font,const char* _title = NULL);
		~sqInputBox();

	void close(void);

	virtual void draw(int self = 1);
	virtual void restore(int _x,int _y,int _sx,int _sy);
	virtual void keytrap(int key);
	virtual void message(int code,sqElem* object = NULL);
	};

struct sqString {
	unsigned char* data;
	sqString* next,*prev;

		sqString(unsigned char* s,int len);
		~sqString(void){ if(data) delete data; }
	};

struct sqText : sqElem {
	int x,y,sx,sy;
	int visistr;
	sqString* first;
	sqString* top;
	sqString* last;
	int len;
	sqFont* font;

		sqText(sqElem* _owner,int _x,int _y,int _visistr,sqFont* _font,int _len,unsigned char* buf,int maxbuf);
		virtual ~sqText(void){ free(); }

	sqText& operator* (sqString* obj);
	void free(void);
	void build(unsigned char* buf,int max);
	int getptr(sqString* b);
	sqString* getstr(int n);

	virtual int* getXY(void){ return &x; }
	virtual void draw(int self = 1);
	virtual void keytrap(int key);
	virtual void activate(int force = 1);
	virtual void deactivate(void);
	virtual void handler(sqEvent* e);
	};

/* -------------------------- Prototypes & Externs -------------------------- */
extern int page;
extern sqEventQueue* sqE;
extern sqEventQueue* sqKey;
extern sqScreen* sqScr;
extern sqElem* QuantObj;
extern sqElem* KeyTrapObj;

void sqInit(int col0);
void sqInitMouse(void);
void sqRestoreCursor(void);
void sqQuant(void);

void sqSetPalette(char* pal);
char* sqstrNcpy(char* dest,char* src,int len);

#endif

