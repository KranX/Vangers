/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"
#include "../common.h"

#include "ikeys.h"

#include "hfont.h"
#include "iscreen.h"
#include "i_mem.h"

#include "../actint/item_api.h"
#include "../actint/a_consts.h"
#include "../actint/actint.h"
#include "../actint/mlstruct.h"

#include "../sound/hsound.h"

/* ----------------------------- EXTERN SECTION ----------------------------- */

#ifdef _DEBUG
extern int iBoundsLog;
#endif

extern int iMouseX;
extern int iMouseY;

extern int actIntLog;
extern actIntDispatcher* aScrDisp;
extern unsigned char* aciCurColorScheme;

extern int iRussian;
extern char* AVInotFound;

extern int iFrameFlag;

extern iScreenOption** iScrOpt;

extern XBuffer* iResBuf;
extern const char* iVideoPath;
extern const char* iVideoPathDefault;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void aci_setMatrixBorder(void);
void aci_setScMatrixBorder(void);
void aciChangeLineID(void);
int aciCheckCredits(void);
int sdlEventToCode(SDL_Event *event);

void put_level(int x,int y,int sx,int sy,int lev);
void put_buf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode);
void put_tbuf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode,int terr);
void add_buf(int x,int y,int sx,int sy,unsigned char* buf,int lev);
void put_buf2col(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,int n_mode = 0);
void put_frame(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,int n_mode = 0);
void put_sframe(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,char* shape,int n_mode = 0);
void put_frame2scr(int x,int y,int sx,int sy,unsigned char* buf);

void put_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put_terr_fon(int x,int y,int sx,int sy,unsigned char* buf);
void get_fon(int x,int y,int sx,int sy,unsigned char* buf);
void get_terr_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put2fon(int x,int y,int sx,int sy,unsigned char* buf,int lev = 0);

void change_terrain(int x,int y,int sx,int sy,int terr1,int terr2);
void put_map(int x,int y,int sx,int sy);
void get_buf(int x,int y,int sx,int sy,unsigned char* buf);
void scale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1);
void dscale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1,int dx,int dy,int lev);
void put_buf2buf(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* buf,unsigned char* buf_to);

void smooth_buf(int sx,int sy,unsigned char* buf,int steps);
void s_smooth_buf(int sx,int sy,unsigned char* buf,int steps,int start_col);
void smooth_shape(int sx,int sy,unsigned char* buf);

void init_hfonts(void);
void free_hfonts(void);

void iregRender(int x0,int y0,int x1,int y1);

int iStrLen(unsigned char* p,int f,int space);
void iPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode);
void i_terrPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode,int terr);
void iPutStr2buf(int x,int y,int fnt,int bsx,int bsy,unsigned char* str,unsigned char* buf_to,int mode,int scale,int space);

void i_getpal(unsigned char* p);
void render_border(iScreenObject* p);

void i_pal_quant(unsigned char* pal_buf,int lev,int n_lev);

void aci_SendEvent(int cd,int dt = 0);

#ifdef _DEBUG
void map_rectangle(int x,int y,int sx,int sy,int col);
#endif

/* --------------------------- DEFINITION SECTION --------------------------- */

#define iEMPTY_AVI		"empty.avi"

#define iAVI_NULL_LEV		12

iScreenDispatcher* iScrDisp;
iScreenEventCommand* CurEvComm;

char* iMemHeap;
unsigned iHeapOffs = 0;
unsigned iHeapSize = 0;

char* BackupStr;

int iScreenOffs = 0;
int iScreenOffsDelta = 0;
int iScreenTargetOffs = 0;
int iScreenLastInput = 0;

int aciCurCredits06 = 0;

int EventFlag = 0;

int iEvLineID = 0;

iListElement::iListElement(void)
{
	//ID_ptr = new char[256];
	//ID_ptr = NULL;
	//name_len = 256;
}

iListElement::~iListElement(void)
{
	//if(ID_ptr) delete[] ID_ptr;
}

void iList::dconnect(iListElement* p)
{
	iListElement* pPtr,*nPtr;

	Size --;

	if(Size > 0){
		pPtr = p -> prev;
		nPtr = p -> next;

		if(pPtr) pPtr -> next = nPtr;
		nPtr -> prev = pPtr;

		if(p == first) first = nPtr;
		if(p == last) last = pPtr;

		last -> next = first;
		first -> prev = NULL;
	}
	else
		init_list();
}

void iList::swap(iListElement* p,iListElement* p1)
{
	if(p1 != p -> prev)
		ErrH.Abort("actInt swap error...");

	if(p1 == first){
		first = p;
	}
	if(p == last)
		last = p1;

	p -> prev = p1 -> prev;
	p1 -> prev = p;

	p1 -> next = p -> next;
	p -> next = p1;

	if(p1 -> next -> prev) p1 -> next -> prev = p1;
	if(p -> prev)
		p -> prev -> next = p;
	else
		last -> next = p;
}

void iListElement::init_id(const char* p)
{
	ID_ptr = p;
	/*int sz = strlen(p) + 1;
	std::cout<<"iListElement::init_id sz:"<<sz<<std::endl;
	if(!name_len || sz > name_len){
		if(name_len) {
			delete[] ID_ptr;
		}
		std::cout<<"iListElement::init_id name_len:"<<name_len<<std::endl;
		ID_ptr = new char[sz];
		std::cout<<"iListElement::init_id A"<<std::endl;
		name_len = sz;
	}
	strcpy(ID_ptr, p);*/
}

iScreenDispatcher::iScreenDispatcher(void)
{
	init_list();
	drwObjHeap = new iList;
	ObjHeap = new iList;
	ScreenIDs = new iList;
	ExtScreens = new iList;
	texts = new iList;
	ActiveEv = NULL;
	flags = 0;

	BackupStr = new char[256];

	curPal = new unsigned char[768];
	trgPal = new unsigned char[768];

	memset(curPal,0,768);
	memset(trgPal,0,768);
}

iScreen::iScreen(void)
{
	flags = 0;

	objList = new iList;
	EventList = new iList;
	TerrainList = new iList;
	ObjIDs = new iList;

	pal_name = NULL;
	palette_buf = NULL;

	ScreenOffs = 0;
	promptData = new XGR_MousePromptScreen;
}

iScreen::~iScreen(void)
{
	iObjID* id,*id1;
	iScreenObject* obj,*obj1;
	iScreenEvent* ev,*ev1;
	obj = (iScreenObject*)objList -> last;
	while(obj){
		obj1 = (iScreenObject*)obj -> prev;
		delete obj;
		obj = obj1;
	}
	obj = (iScreenObject*)TerrainList -> last;
	while(obj){
		obj1 = (iScreenObject*)obj -> prev;
		delete obj;
		obj = obj1;
	}
	ev = (iScreenEvent*)EventList -> last;
	while(ev){
		ev1 = (iScreenEvent*)ev -> prev;
		delete ev;
		ev = ev1;
	}
	id = (iObjID*)ObjIDs -> last;
	while(id){
		id1 = (iObjID*)id -> prev;
		delete id;
		id = id1;
	}
	delete objList;
	delete EventList;
	delete TerrainList;
	delete ObjIDs;
	delete promptData;
}

iScreenObject::iScreenObject(void)
{
	curHeightScale = 256;
	EventList = new iList;
	ElementList = new iList;
	SizeX = SizeY = 0;
	flags = 0;
	SmoothLevel = 0;
	align_x = align_y = 0;
	align_x_offs = align_y_offs = 0;

	ShadowSize = I_SHADOW_SIZE;

	FlushX = FlushY = -1;

	owner = NULL;
	promptID = 0;
	promptData = NULL;
}

iScreenObject::~iScreenObject(void)
{
	iScreenEvent* ev,*ev1;
	iScreenElement* el,*el1;
	iListElement** lst;
	el = (iScreenElement*)ElementList -> last;
	while(el){
		el1 = (iScreenElement*)el -> prev;
		delete el;
		el = el1;
	}
	ev = (iScreenEvent*)EventList -> last;
	while(ev){
		ev1 = (iScreenEvent*)ev -> prev;
		delete ev;
		ev = ev1;
	}
	delete ElementList;
	delete EventList;

	if(flags & OBJ_TRIGGER){
		lst = ((iTriggerObject*)this) -> StateElem;
		delete lst;
	}
	if(flags & OBJ_FON_DATA){
		flags ^= OBJ_FON_DATA;
		delete fonData;
		if(flags & OBJ_STORE_TERRAIN) delete terr_fonData;
	}
	if(promptData) delete promptData;
}

iScreenElement::iScreenElement(void)
{
	align_x = align_y = 0;
	align_x_offs = align_y_offs = 0;
	lX = lY = 0;
	flags = 0;
	null_level = I_LEVEL;
	SizeX = SizeY = 0;

	terrainNum = -1;

	scale = 256;
	scale_delta = 0;

	ID = 0;
}

iScreenElement::~iScreenElement(void)
{
	switch(type){
		case I_STRING_ELEM:
			((iStringElement*)this) -> free_mem();
			break;
		case I_BITMAP_ELEM:
			((iBitmapElement*)this) -> free_mem();
			break;
		case I_SCROLLER_ELEM:
			((iScrollerElement*)this) -> free_mem();
			break;
		case I_TERRAIN_ELEM:
			((iTerrainElement*)this) -> free_mem();
			break;
		case I_S_STRING_ELEM:
			((iS_StringElement*)this) -> free_mem();
			break;
		case I_AVI_ELEM:
			((iAVIElement*)this) -> free_mem();
			break;
		case I_AVI_BORDER_ELEM:
			((iAVIBorderElement*)this) -> free_mem();
			break;
	}
}

int iObjID::compare(const char* p)
{
	return !strcmp(p,ID);
}

iObjID::iObjID(void)
{
	ID = new char[256];
}

iObjID::~iObjID(void)
{
	delete[] ID;
}

void iObjID::init(const char* p,int num,iListElement* ptr)
{
	strcpy(ID,p);
	numID = num;
	obj = ptr;
}

void iScreenObject::copy(iScreenObject* dest)
{
	int i;
	iTriggerObject* p;
	iScreenEvent* dest_ev,*ev;
	iScreenElement* dest_el,*el;

	dest -> PosX = PosX;
	dest -> PosY = PosY;

	dest -> SizeX = SizeX;
	dest -> SizeY = SizeY;

	dest -> align_x = align_x;
	dest -> align_y = align_y;
	dest -> align_x_offs = align_x_offs;
	dest -> align_y_offs = align_y_offs;

	dest -> flags = flags;

	dest -> curHeightScale = curHeightScale;
	dest -> HeightScaleDelta = HeightScaleDelta;

	dest -> SmoothLevel = SmoothLevel;
	dest -> SmoothDelta = SmoothDelta;
	dest -> SmoothTime = SmoothTime;
	dest -> SmoothCount = SmoothCount;

	ev = (iScreenEvent*)EventList -> last;
	while(ev){
		dest_ev = new iScreenEvent;
		ev -> copy(dest_ev);
		dest -> EventList -> connect((iListElement*)dest_ev);
		ev = (iScreenEvent*)ev -> prev;
	}

	if(flags & OBJ_TRIGGER){
		p = (iTriggerObject*)this;

		((iTriggerObject*)dest) -> state = p -> state;
		((iTriggerObject*)dest) -> num_state = p -> num_state;

		((iTriggerObject*)dest) -> alloc_state();
		for(i = 0; i < p -> num_state; i ++){
			dest_el = iCreateScreenElement(((iScreenElement*)p -> StateElem[i]) -> type,((iScreenElement*)p->StateElem[i])->ID_ptr.c_str(),(iScreenElement*)p -> StateElem[i]);
			((iTriggerObject*)dest) -> StateElem[i] = dest_el;
			dest -> add_element((iListElement*)dest_el);
		}
	} else {
		el = (iScreenElement*)ElementList -> last;
		while(el){
			dest_el = iCreateScreenElement(el -> type,el->ID_ptr.c_str(),NULL);
			el -> copy(dest_el);
			dest -> add_element((iListElement*)dest_el);
			el = (iScreenElement*)el -> prev;
		}
	}
}

void iScreenElement::copy(iScreenElement* dest)
{
	int i,sz;
	iListElement* p,*pd;
	iList* l,*ld;

	dest -> type = type;

	dest -> lX = lX;
	dest -> lY = lY;

	dest -> SizeX = SizeX;
	dest -> SizeY = SizeY;

	dest -> scale = scale;
	dest -> scale_delta = scale_delta;

	dest -> align_x = align_x;
	dest -> align_y = align_y;
	dest -> align_x_offs = align_x_offs;
	dest -> align_y_offs = align_y_offs;

	dest -> flags = flags;

	dest -> null_level = null_level;

	switch(type){
		case I_STRING_ELEM:
			((iStringElement*)dest) -> font = ((iStringElement*)this) -> font;
			((iStringElement*)dest) -> space = ((iStringElement*)this) -> space;
			sz = strlen(((iStringElement*)this) -> string);
			strcpy(((iStringElement*)dest) -> string,((iStringElement*)this) -> string);
			break;
		case I_BITMAP_ELEM:
		case I_SCROLLER_ELEM:
			((iBitmapElement*)dest) -> bSizeX = ((iBitmapElement*)this) -> bSizeX;
			((iBitmapElement*)dest) -> bSizeY = ((iBitmapElement*)this) -> bSizeY;

			((iBitmapElement*)dest) -> bmp_null_level = ((iBitmapElement*)this) -> bmp_null_level;

			sz = strlen(((iBitmapElement*)this) -> fname);
			((iBitmapElement*)dest) -> fname = new char[sz + 1];
			strcpy(((iBitmapElement*)dest) -> fname,((iBitmapElement*)this) -> fname);

			l = ((iBitmapElement*)this) -> bmp_names;
			ld = ((iBitmapElement*)dest) -> bmp_names;
			p = l -> first;
			for(i = 0; i < l -> Size; i ++){
				pd = new iListElement;
				pd -> init_id(p->ID_ptr.c_str());
				ld -> connect(pd);
				p = p -> next;
			}

			l = ((iBitmapElement*)this) -> pal_names;
			ld = ((iBitmapElement*)dest) -> pal_names;
			p = l -> first;
			for(i = 0; i < l -> Size; i ++){
				pd = new iListElement;
				pd->init_id(p->ID_ptr.c_str());
				ld -> connect(pd);
				p = p -> next;
			}

			if(type == I_SCROLLER_ELEM){
				((iScrollerElement*)dest) -> dir = ((iScrollerElement*)this) -> dir;
				((iScrollerElement*)dest) -> Value = ((iScrollerElement*)this) -> Value;
				((iScrollerElement*)dest) -> maxValue = ((iScrollerElement*)this) -> maxValue;
				((iScrollerElement*)dest) -> prevValue = ((iScrollerElement*)this) -> prevValue;
				((iScrollerElement*)dest) -> scale = ((iScrollerElement*)this) -> scale;
				((iScrollerElement*)dest) -> scale_delta = ((iScrollerElement*)this) -> scale_delta;
				((iScrollerElement*)dest) -> space = ((iScrollerElement*)this) -> space;
			}
			break;
		case I_TERRAIN_ELEM:
			((iTerrainElement*)dest) -> scale_strength = ((iTerrainElement*)this) -> scale_strength;
			((iTerrainElement*)dest) -> def_null_level = ((iTerrainElement*)this) -> def_null_level;
			((iTerrainElement*)dest) -> level_delta = ((iTerrainElement*)this) -> level_delta;

			((iTerrainElement*)dest) -> pX = ((iTerrainElement*)this) -> pX;
			((iTerrainElement*)dest) -> pY = ((iTerrainElement*)this) -> pY;
			break;
		case I_S_STRING_ELEM:
			((iS_StringElement*)dest) -> font = ((iS_StringElement*)this) -> font;
			((iS_StringElement*)dest) -> space = ((iS_StringElement*)this) -> space;
//			  sz = strlen(((iS_StringElement*)this) -> string);
//			  ((iS_StringElement*)dest) -> string = new char[sz + 1];
			strcpy(((iS_StringElement*)dest) -> string,((iS_StringElement*)this) -> string);
			break;
		case I_AVI_ELEM:
			((iAVIElement*)dest) -> border_type = ((iAVIElement*)this) -> border_type;
			((iAVIElement*)dest) -> border_level = ((iAVIElement*)this) -> border_level;
			((iAVIElement*)dest) -> border_null_level = ((iAVIElement*)this) -> border_null_level;
			((iAVIElement*)dest) -> border_size = ((iAVIElement*)this) -> border_size;

			if(((iAVIElement*)dest) -> border_type == AVI_SHAPED_BORDER){
				sz = strlen(((iAVIElement*)this) -> border_shape_file);
				((iAVIElement*)dest) -> border_shape_file = new char[sz + 1];
				strcpy(((iAVIElement*)dest) -> border_shape_file,((iAVIElement*)this) -> border_shape_file);
			}

			l = ((iAVIElement*)this) -> avi_ids;
			ld = ((iAVIElement*)dest) -> avi_ids;
			p = l -> first;
			for(i = 0; i < l -> Size; i ++){
				pd = new iListElement;
				pd -> init_id(p->ID_ptr.c_str());
				ld -> connect(pd);
				p = p -> next;
			}

			strcpy(((iAVIElement*)dest) -> avi_name,((iAVIElement*)this) -> avi_name);
			break;
		case I_AVI_BORDER_ELEM:
			((iAVIBorderElement*)dest) -> border_type = ((iAVIBorderElement*)this) -> border_type;
			((iAVIBorderElement*)dest) -> border_level = ((iAVIBorderElement*)this) -> border_level;
			((iAVIBorderElement*)dest) -> border_null_level = ((iAVIBorderElement*)this) -> border_null_level;
			((iAVIBorderElement*)dest) -> border_size = ((iAVIBorderElement*)this) -> border_size;
			((iAVIBorderElement*)dest) -> dir = ((iAVIBorderElement*)this) -> dir;

			if(((iAVIBorderElement*)dest) -> border_type == AVI_SHAPED_BORDER){
				sz = strlen(((iAVIBorderElement*)this) -> border_shape_file);
				((iAVIBorderElement*)dest) -> border_shape_file = new char[sz + 1];
				strcpy(((iAVIBorderElement*)dest) -> border_shape_file,((iAVIBorderElement*)this) -> border_shape_file);
			}
			break;
	}
}

iStringElement::iStringElement(void)
{
	font = 0;
	space = I_DEFAULT_SPACE;
	type = I_STRING_ELEM;
	string = new char[iSTR_LEN];
	memset (string, '\0', iSTR_LEN);
}

iS_StringElement::iS_StringElement(void)
{
	font = 0;
	space = I_DEFAULT_SPACE;
	type = I_S_STRING_ELEM;
	string = new char[iS_STR_LEN];
	memset (string, '\0', iS_STR_LEN);
}

iBitmapElement::iBitmapElement(void)
{
	bmp_null_level = HFONT_NULL_LEVEL;

	border_shape_file = NULL;

	fname = new char[256];
	palname = new char[256];
	memset(palname,0,256);

	bmp_names = new iList;
	pal_names = new iList;

	ibs = NULL;

	type = I_BITMAP_ELEM;
}

iScrollerElement::iScrollerElement(void)
{
	Value = maxValue = dir = 0;
	space = I_DEFAULT_SPACE;
	bmp_null_level = I_LEVEL;

	type = I_SCROLLER_ELEM;
}

iAVIElement::iAVIElement(void)
{
	border_type = AVI_FLAT_BORDER;
	border_size = AVI_DEF_BORDER_SIZE;
	border_level = 0;
	border_null_level = 0;

	avi_ids = new iList;
	avi_name = new char[256];
	memset(avi_name, 0, sizeof(char));

	border_shape_file = NULL;
	palBuf = NULL;

	ibs = NULL;

	type = I_AVI_ELEM;
}

iAVIBorderElement::iAVIBorderElement(void)
{
	type = I_AVI_BORDER_ELEM;
}

void iStringElement::free_mem(void)
{
	if(string)
		delete[] string;
}

void iS_StringElement::free_mem(void)
{
	if(string)
		delete[] string;
}

void iAVIElement::free_mem(void)
{
	iListElement* p,*p1;
	p = avi_ids -> last;
	while(p){
		p1 = p -> prev;
		delete p;
		p = p1;
	}
	delete avi_ids;
	delete[] avi_name;

	if(ibs)
		delete ibs;

	free();
	if(border_shape_file)
		delete border_shape_file;
	//if(palBuf)
	//	delete palBuf;
	free_shape();
}

void iAVIElement::free_shape(void)
{
	if(flags & EL_BORDER_INIT){
		if(border_shape)
			delete border_shape;
		if(ibs)
			ibs -> free();
		flags ^= EL_BORDER_INIT;
	}
}

void iBitmapElement::free_shape(void)
{
	if(flags & EL_BORDER_INIT){
		if(border_shape)
			delete border_shape;
		if(ibs)
			ibs -> free();
		flags ^= EL_BORDER_INIT;
	}
}

void iAVIBorderElement::free_mem(void)
{
	if(border_shape_file){
		delete border_shape_file;
	}
}

void iAVIElement::free(void)
{
	if(flags & EL_DATA_LOADED){
		AVIstop(data);
		AVIclose(data);

		//delete palBuf;
		palBuf = NULL;

		flags ^= EL_DATA_LOADED;
	}
}

void iAVIElement::load(void)
{
	if(!(flags & EL_DATA_LOADED)){
		flags &= ~AVI_STOPPED;
		//palBuf = new unsigned char[768];

		iResBuf -> init();
		*iResBuf < iVideoPathDefault < avi_name;

		if(!AVIopen(iResBuf -> address(),AVI_NOTIMER | AVI_NODRAW | AVI_LOOPING | AVI_NOPALETTE,0,&data)){
			iResBuf -> init();
			*iResBuf < iVideoPath < avi_name;
			if(!AVIopen(iResBuf -> address(),AVI_NOTIMER | AVI_NODRAW | AVI_LOOPING | AVI_NOPALETTE,0,&data)){
				iResBuf -> init();
				*iResBuf < iVideoPath < iEMPTY_AVI;
				if(!AVIopen(iResBuf -> address(),AVI_NOTIMER | AVI_NODRAW | AVI_LOOPING | AVI_NOPALETTE,0,&data))
					ErrH.Abort(AVInotFound);
			}
		}
		AVIplay(data,0,0);

		//pal_tmp = (char*)AVIGetPalette(data);
		palBuf=(unsigned char*)AVIGetPalette(data);
		//memcpy(palBuf,pal_tmp,768);

		//std::cout<<"iAVIElement::load"<<std::endl;
		flags |= EL_DATA_LOADED;
	}
}

void iAVIElement::load_shape(void)
{
	XStream fh;
	if(!(flags & EL_BORDER_INIT)){
		if(border_shape_file){
			fh.open(border_shape_file);
			fh > ShSizeX > ShSizeY;
			if(ShSizeX != SizeX || ShSizeY != SizeY)
				ErrH.Abort("Bad AVI border shape file...",XERR_USER,0,ID_ptr.c_str());
			border_shape = new char[ShSizeX * ShSizeY];
			fh.read(border_shape,ShSizeX * ShSizeY);
			fh.close();

			smooth_shape(ShSizeX,ShSizeY,(unsigned char*)border_shape);
		}
		else {
			ShSizeX = 0;
			ShSizeY = 0;
			border_shape = NULL;
			if(ibs){
				ibs -> load();
			}
		}
		flags |= EL_BORDER_INIT;
	}
}

void iBitmapElement::load_shape(void)
{
	XStream fh;
	if(!(flags & EL_BORDER_INIT)){
		if(border_shape_file){
			fh.open(border_shape_file);
			fh > ShSizeX > ShSizeY;
			if(ShSizeX != SizeX || ShSizeY != SizeY)
				ErrH.Abort("Bad BMP border shape file...",XERR_USER,0,ID_ptr.c_str());
			border_shape = new char[ShSizeX * ShSizeY];
			fh.read(border_shape,ShSizeX * ShSizeY);
			fh.close();

			smooth_shape(ShSizeX,ShSizeY,(unsigned char*)border_shape);
		}
		else {
			border_shape = NULL;
			if(ibs){
				ibs -> load();
			}
		}
		flags |= EL_BORDER_INIT;
	}
}

void iAVIElement::init_size(void)
{
	if(flags & EL_DATA_LOADED){
		SizeX = AVIwidth(data);
		SizeY = AVIheight(data);
	}
}

void iAVIElement::change_avi(char* fname)
{
	free();
	if (avi_name!=fname) {
		strcpy(avi_name,fname);
	}
	load();
}

void iTriggerObject::change_state(int time,int st)
{
	iScreenElement* p;

	p = (iScreenElement*)StateElem[state];
	p -> scale_delta = -p -> scale / time;
	p -> flags |= EL_HIDE;

	prev_state = state;

	if(st == -1){
		state ++;
	}
	else {
		if(st == -2)
			state --;
		else
			state = st;
	}

	if(state >= num_state)
		state = 0;
	if(state < 0)
		state = num_state - 1;

	p = (iScreenElement*)StateElem[state];
	p -> flags &= ~EL_NO_REDRAW;
	p -> scale = 0;
	p -> scale_delta = (256 - p -> scale) / time;
	p -> flags |= EL_HIDE;
	
	if (callback) {
		callback(state);
	}
}

iTriggerObject::iTriggerObject(void)
{
	num_state = 2;
	state = 0;
	StateElem = NULL;
	callback = NULL;
}

void iScreenEvent::add_scancode(int cd)
{
	iScanCode* p = new iScanCode;
	p -> code = cd;
	codes -> connect((iListElement*)p);
}

void iTriggerObject::alloc_state(void)
{
	int i;
	StateElem = new iListElement*[num_state];

	for(i = 0; i < num_state; i ++)
		StateElem[i] = NULL;

	flags |= TRG_MEM_ALLOC;
}

void iScrollerElement::change_quant(void)
{
	scale += scale_delta;

	if(scale < -256) scale = -256;
	if(scale > 256) scale = 256;
}

void iScrollerElement::change_val(int x,int y)
{
	x -= lX + ((iScreenObject*)owner) -> PosX;
	y -= lY + ((iScreenObject*)owner) -> PosY;

	prevValue = Value;
	switch(dir){
		case I_LEFT:
			Value = (SizeX - x - bSizeX - space) / (bSizeX + space);
			break;
		case I_UP:
			Value = (SizeY - y - bSizeY - space) / (bSizeY + space);
			break;
		case I_RIGHT:
			Value = x / (bSizeX + space);
			break;
		case I_DOWN:
			Value = y / (bSizeY + space);
			break;
	}
	if(Value < 0) Value = 0;
	if(Value > maxValue) Value = maxValue;
	scale = (Value > prevValue) ? -256 : 256;
}

void iScrollerElement::scroller_init(void)
{
	prevValue = Value;
}

iTerrainElement::iTerrainElement(void)
{
	scale_strength = 0;
	null_level = def_null_level = 0;
	level_delta = 0;
	pX = pY = 0;

	type = I_TERRAIN_ELEM;
}

void iTerrainElement::free_mem(void)
{
}

iScreenEventCommand::iScreenEventCommand(void)
{
	time = 1;
	start_time = 0;
	value = 0;
	flags = 0;

	data0 = data1 = 0;

	LineID = 0;

	objType = I_ABSENT;

	pvalue = NULL;
	tmp_objID = NULL;
}

iScreenEventCommand::~iScreenEventCommand(void)
{
	iList* list;
	iListElement* p,*p1;
	if(pvalue){
		if(!(flags & EC_LIST))
			delete pvalue;
		else {
			list = (iList*)pvalue;
			p = list -> last;
			while(p){
				p1 = p -> prev;
				delete p;
				p = p1;
			}
			delete list;
		}
	}
	if(tmp_objID)
		delete tmp_objID;
}

void iScreenEventCommand::init_objID(const char* ptr)
{
	int sz = strlen(ptr);
	tmp_objID = new char[sz + 1];
	strcpy(tmp_objID,ptr);
}

void iScreenEventCommand::init_pvalue(char* ptr)
{
	int sz = strlen(ptr);
	pvalue = new char[sz + 1];
	strcpy(pvalue,ptr);
}

iScreenEvent::iScreenEvent(void)
{
	CommSeq = new iList;
	time = 1;
	flags = 0;
	codes = new iList;
}

iScreenEvent::~iScreenEvent(void)
{
	iScreenEventCommand* cm,*cm1;
	iScanCode* cd,*cd1;
	cd = (iScanCode*)codes -> last;
	while(cd){
		cd1 = (iScanCode*)cd -> prev;
		delete cd;
		cd = cd1;
	}
	cm = (iScreenEventCommand*)CommSeq -> last;
	while(cd){
		cm1 = (iScreenEventCommand*)cm -> prev;
		delete cm;
		cm = cm1;
	}
	delete CommSeq;
	delete codes;
}

iList::iList(void)
{
	init_list();
}

void iList::connect(iListElement* p)
{
	if(last){
		last -> next = p;
		p -> prev = last;
		p -> next = first;
		last  = p;
	}
	else{
		last = p;
		first = p;
		p -> next = p;
		p -> prev = 0;
	}
	Size ++;
}

void iScreenEventCommand::copy(iScreenEventCommand* dest)
{
	int sz;
	dest -> objID = objID;
	if(tmp_objID){
		sz = strlen(tmp_objID);
		if(dest -> tmp_objID) delete dest -> tmp_objID;
		dest -> tmp_objID = new char[sz + 1];
		strcpy(dest -> tmp_objID,tmp_objID);
	}
	dest -> objType = objType;

	dest -> EvType = EvType;
	dest -> flags = flags;
	dest -> value = value;

	if(pvalue){
		sz = strlen(pvalue);
		if(dest -> pvalue) delete dest -> pvalue;
		dest -> pvalue = new char[sz + 1];
		strcpy(dest -> pvalue,pvalue);
	}

	dest -> time = time;
	dest -> start_time = start_time;
}

void iScreenEvent::copy(iScreenEvent* dest)
{
	iScanCode* cd,*dest_cd;
	iScreenEventCommand* cm,*dest_cm;

	cd = (iScanCode*)codes -> last;
	while(cd){
		dest_cd = new iScanCode;
		dest_cd -> code = cd -> code;
		dest -> codes -> connect((iListElement*)dest_cd);
		cd = (iScanCode*)cd -> prev;
	}

	cm = (iScreenEventCommand*)CommSeq -> last;
	while(cm){
		dest_cm = new iScreenEventCommand;
		cm -> copy(dest_cm);
		dest -> CommSeq -> connect((iListElement*)dest_cm);
		cm = (iScreenEventCommand*)cm -> prev;
	}

	dest -> flags = flags;
	dest -> time = time;
	dest -> cur_time = cur_time;
}

void iScreenObject::getfon(int del)
{
	if(flags & OBJ_NO_FON) return;

	if(del){
		if(flags & OBJ_FON_DATA){
			delete[] fonData;
			if(flags & OBJ_STORE_TERRAIN)
				delete[] terr_fonData;
		}
		fonData = new unsigned char[SizeX * SizeY];
	}

	get_fon(PosX,PosY,SizeX,SizeY,fonData);
	if(flags & OBJ_STORE_TERRAIN){
		if(del)
			terr_fonData = new unsigned char[SizeX * SizeY];
		get_terr_fon(PosX,PosY,SizeX,SizeY,terr_fonData);
	}
	flags |= OBJ_FON_DATA;
}

void iScreenElement::change_terrainNum(int terr)
{
	int x,y;
	iScreenObject* p;
	if(terrainNum != -1 && terr != -1){
		p = (iScreenObject*)owner;
		x = p -> PosX + lX;
		y = p -> PosY + lY;

		change_terrain(x,y,SizeX,SizeY,terrainNum,terr);
	}
	terrainNum = terr;
}

void iScreenObject::free_fon(void)
{
	if(flags & OBJ_FON_DATA){
		flags ^= OBJ_FON_DATA;
		delete[] fonData;
		if(flags & OBJ_STORE_TERRAIN)
			delete[] terr_fonData;
	}
}

void iScreenObject::putfon(void)
{
	if(flags & OBJ_NO_FON) return;

	if(flags & OBJ_FON_DATA){
		put_fon(PosX,PosY,SizeX,SizeY,fonData);
		if(flags & OBJ_STORE_TERRAIN)
			put_terr_fon(PosX,PosY,SizeX,SizeY,terr_fonData);
	}
}

void iScreenObject::add_event(iListElement* p)
{
	EventList -> connect(p);
}

void iScreen::add_event(iListElement* p)
{
	EventList -> connect(p);
}

void iScreenObject::add_element(iListElement* p)
{
	ElementList -> connect(p);
	((iScreenElement*)p) -> owner = (iListElement*)this;
}

void iScreenObject::remove_element(iListElement* p)
{
	ElementList -> dconnect(p);
	((iScreenElement*)p) -> owner = NULL;
}

void iScreenElement::load_data(void)
{
	switch(type){
		case I_BITMAP_ELEM:
			((iBitmapElement*)this) -> load();
			((iBitmapElement*)this) -> load_shape();
			break;
		case I_SCROLLER_ELEM:
			((iScrollerElement*)this) -> load();
			break;
		case I_TERRAIN_ELEM:
			break;
		case I_AVI_ELEM:
			((iAVIElement*)this) -> load();
			((iAVIElement*)this) -> load_shape();
			((iAVIElement*)this) -> init_size();
			((iAVIElement*)this) -> init_align();
			break;
	}
}

int iScreenElement::check_xy(int x,int y)
{
	x -= lX + ((iScreenObject*)owner) -> PosX;
	y -= lY + ((iScreenObject*)owner) -> PosY;

	if(x >= 0 && y >= 0 && x < SizeX && y < SizeY)
		return 1;
	return 0;
}

void iBitmapElement::load(void)
{
	int i,t,sz;
	XStream fh;
	if(!(flags & EL_DATA_LOADED)){
		if(flags & EL_NONE_BMP){
			sz = bSizeX * bSizeY;
			fdata = new char[sz];
			memset(fdata,0,sz);
		}
		else {
			fh.open(fname,XS_IN);
			fh > bSizeX > bSizeY;

			sz = bSizeX * bSizeY;
			fdata = new char[sz];
			fh.read(fdata,sz);
			fh.close();

			if(!(flags & EL_PICTURE)){
				for(i = 0; i < sz; i ++){
					t = ((unsigned char*)fdata)[i];
					if(t)
						t += bmp_null_level;
					else
						t = HFONT_NULL_LEVEL;

					fdata[i] = t;
				}
			}
			else {
				if(flags & EL_HIDDEN_FON)
					memset(fdata,0,sz);
			}
			set_pal();
		}
	}
	flags |= EL_DATA_LOADED;
}

void iBitmapElement::alloc_mem(void)
{
	int sz;
	if(!(flags & EL_DATA_LOADED)){
		sz = bSizeX * bSizeY;
		fdata = new char[sz];
	}
	flags |= EL_DATA_LOADED;
}

void iBitmapElement::init_size(void)
{
	XStream fh;
	if(!(flags & EL_NONE_BMP)){
		fh.open(fname,XS_IN);
		fh > bSizeX > bSizeY;
		fh.close();
		SizeX = bSizeX;
		SizeY = bSizeY;
	}
	else {
		bSizeX = SizeX;
		bSizeY = SizeY;
	}
}

void iScreenElement::free_data(void)
{
	switch(type){
		case I_BITMAP_ELEM:
			((iBitmapElement*)this) -> free();
			((iBitmapElement*)this) -> free_shape();
			break;
		case I_SCROLLER_ELEM:
			((iScrollerElement*)this) -> free();
			break;
		case I_TERRAIN_ELEM:
			break;
		case I_AVI_ELEM:
			((iAVIElement*)this) -> free();
			((iAVIElement*)this) -> free_shape();
			break;
	}
}

void iBitmapElement::free_mem(void)
{
	iListElement* p,*p1;

	free();
	delete[] fname;
	delete[] palname;

	if(ibs)
		delete ibs;

	if(border_shape_file)
		delete border_shape_file;

	p = bmp_names -> last;
	while(p){
		p1 = p -> prev;
		delete p;
		p = p1;
	}
	delete bmp_names;

	p = pal_names -> last;
	while(p){
		p1 = p -> prev;
		delete p;
		p = p1;
	}
	delete pal_names;
}

void iBitmapElement::free(void)
{
	if((flags & EL_DATA_LOADED)){
		flags ^= EL_DATA_LOADED;
		delete[] fdata;
	}
}

#define iS_STR_COL_START	144
#define iS_STR_COL_SIZE 	1

#define iS_STR_COL0_START	0
#define iS_STR_COL0_SIZE	1

#define S_STRING_DELTA		3
void iScreenElement::redraw(int x,int y,int sc,int sm,int hide_mode)
{
	int i,v = 0,maxv = 0,bsx = 0,bsy = 0,bs = 0,sp = 0,pv = 0,offs,lev = 0,col = iS_STR_COL_START,col_sz = iS_STR_COL_SIZE,dx1,dx2,dy1,dy2;
	int null_lev = null_level;
	unsigned char* p,*p1,*data = NULL;

	if(flags & EL_NO_SCALE && scale != 256)
		return;

//	  if(flags & EL_NO_SCALE)
//		  sc = 256;

	if(flags & EL_HIDE)
		hide_mode = 1;

	if(flags & EL_SCALE_NULL){
		null_lev = (null_level * sc) >> 8;
	}
//std::cout<<"iScreenElement::redraw"<<std::endl;
	switch(type){
		case I_AVI_ELEM:
			//std::cout<<"I_AVI_ELEM"<<std::endl;
			if(flags & AVI_STOPPED) return;
			break;
		case I_AVI_BORDER_ELEM:
			//std::cout<<"I_AVI_BORDER_ELEM"<<std::endl;
			bsx = ((iAVIBorderElement*)this) -> ShapeX;
			bsy = ((iAVIBorderElement*)this) -> ShapeY;
			bs = ((iAVIBorderElement*)this) -> border_size;
			lev = ((iAVIBorderElement*)this) -> border_null_level;
			col = ((iAVIBorderElement*)this) -> border_level;

			lev = (lev * sc) >> 8;
			col = (col * sc) >> 8;
			break;
		case I_STRING_ELEM:
			//std::cout<<"I_STRING_ELEM"<<std::endl;
			data = (unsigned char*)(((iStringElement*)this) -> string);
			break;
		case I_S_STRING_ELEM:
			//std::cout<<"I_S_STRING_ELEM"<<std::endl;
			data = (unsigned char*)(((iS_StringElement*)this) -> string);
			if(((iScreenObject*)owner) -> flags & OBJ_SELECTABLE && !(((iScreenObject*)owner) -> flags & OBJ_SELECTED)){
				col = iS_STR_COL0_START;
				col_sz = iS_STR_COL0_SIZE;
			}
			break;
		case I_BITMAP_ELEM:
			//std::cout<<"I_BITMAP_ELEM"<<std::endl;
			data = (unsigned char*)(((iBitmapElement*)this) -> fdata);
			break;
		case I_SCROLLER_ELEM:
			//std::cout<<"I_SCROLLER_ELEM"<<std::endl;
			data = (unsigned char*)(((iScrollerElement*)this) -> fdata);
			v = ((iScrollerElement*)this) -> Value;
			pv = ((iScrollerElement*)this) -> prevValue;

			if(pv > v){
				pv = v;
				v = ((iScrollerElement*)this) -> prevValue;
			}
			maxv = ((iScrollerElement*)this) -> maxValue;
			bsx = ((iScrollerElement*)this) -> bSizeX;
			bsy = ((iScrollerElement*)this) -> bSizeY;
			sp = ((iScrollerElement*)this) -> space;
			break;
		case I_TERRAIN_ELEM:
			//std::cout<<"I_TERRAIN_ELEM"<<std::endl;
			data = ((iScreenObject*)owner) -> fonData;
			bsx = ((iScreenObject*)owner) -> SizeX;
			bsy = ((iScreenObject*)owner) -> SizeY;
			break;
	}

	if(scale != 256)
		sc = (sc * scale) >> 8;

	if(!sm){
		//std::cout<<"sm = 0"<<std::endl;
		switch(type){
			case I_STRING_ELEM:
				if(terrainNum == -1)
					iPutStr(x + lX,y + lY,((iStringElement*)this) -> font,data,0,sc,((iStringElement*)this) -> space,null_lev,hide_mode);
				else
					i_terrPutStr(x + lX,y + lY,((iStringElement*)this) -> font,data,0,sc,((iStringElement*)this) -> space,null_lev,hide_mode,terrainNum);
				break;
			case I_S_STRING_ELEM:
				dx1 = (x + lX < S_STRING_DELTA) ? (x + lX) : S_STRING_DELTA;
				dx2 = (x + lX + SizeX < I_MAP_RES_X - 1 - S_STRING_DELTA) ? S_STRING_DELTA : ((I_MAP_RES_X - 1) - (x + lX + SizeX));
				dy1 = (y + lY < S_STRING_DELTA) ? (y + lY) : S_STRING_DELTA;
				dy2 = (y + lY + SizeY < I_RES_Y - 1 - S_STRING_DELTA) ? S_STRING_DELTA : ((I_RES_Y - 1) - (y + lY + SizeY));
				bsx = SizeX + dx1 + dx2;
				bsy = SizeY + dy1 + dy2;
				_iALLOC_A_(unsigned char,p,bsx * bsy);

				memset(p,0,bsx * bsy);
				lev = (null_lev * abs(sc)) >> 8;

//				  iPutS_Str(dx1,dy1,((iS_StringElement*)this) -> font,col,data,bsx,bsy,p,((iStringElement*)this) -> space);
//				  s_smooth_buf(bsx,bsy,p,0,iS_STR_SEG_START_COL);

				aPutStr32(dx1,dy1,((iS_StringElement*)this) -> font,col,col_sz,data,bsx,p,((iStringElement*)this) -> space);
				put_buf2col(x + lX - dx1,y + lY - dy1,bsx,bsy,p,lev);

				_iFREE_A_(unsigned char,p,bsx * bsy);
				break;
			case I_BITMAP_ELEM:
				_iALLOC_A_(unsigned char,p,SizeX * SizeY);
				if(!(flags & EL_PICTURE)){
					scale_buf(SizeX,SizeY,sc,(unsigned char*)(((iBitmapElement*)this) -> fdata),p);
					if(terrainNum == -1)
						put_buf(x + lX,y + lY,SizeX,SizeY,p,data,null_lev,hide_mode);
					else
						put_tbuf(x + lX,y + lY,SizeX,SizeY,p,data,null_lev,hide_mode,terrainNum);
				}
				else {
					lev = (null_lev * abs(sc)) >> 8;
					if(lev){
						if(flags & EL_HIDDEN_FON)
							put_buf2col(x + lX,y + lY,SizeX,SizeY,data,lev,0);
						else
							put_buf2col(x + lX,y + lY,SizeX,SizeY,data,lev,1);
						if(((iBitmapElement*)this) -> ibs)
							((iBitmapElement*)this) -> ibs -> show();
					}
					else
						put_map(x + lX,y + lY,SizeX,SizeY);
				}
				_iFREE_A_(unsigned char,p,SizeX * SizeY);
				break;
			case I_SCROLLER_ELEM:
				if(((iScrollerElement*)this) -> dir == I_RIGHT){
					offs = sp + bsx;
					_iALLOC_A_(unsigned char,p,bsx * bsy);
					for(i = 0; i < pv; i ++){
						scale_buf(bsx,bsy,sc,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsx;
					}
					for(i = pv; i < v; i ++){
						scale_buf(bsx,bsy,((iScrollerElement*)this) -> scale,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsx;
					}
					for(i = v; i < maxv; i ++){
						scale_buf(bsx,bsy,-sc,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsx;
					}
					_iFREE_A_(unsigned char,p,bsx * bsy);
				}
				if(((iScrollerElement*)this) -> dir == I_UP){
					offs = sp + bsx;
					_iALLOC_A_(unsigned char,p,bsx * bsy);
					for(i = maxv - 1; i >= v; i --){
						scale_buf(bsx,bsy,-sc,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsy;
					}
					for(i = v - 1; i >= pv; i --){
						scale_buf(bsx,bsy,((iScrollerElement*)this) -> scale,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsy;
					}
					for(i = pv - 1; i >= 0; i --){
						scale_buf(bsx,bsy,sc,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsy;
					}
					_iFREE_A_(unsigned char,p,bsx * bsy);
				}
				if(((iScrollerElement*)this) -> dir == I_LEFT){
					offs = sp + bsx;
					_iALLOC_A_(unsigned char,p,bsx * bsy);
					for(i = maxv - 1; i >= v; i --){
						scale_buf(bsx,bsy,-sc,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsx;
					}
					for(i = v - 1; i >= pv; i --){
						scale_buf(bsx,bsy,((iScrollerElement*)this) -> scale,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsx;
					}
					for(i = pv - 1; i >= 0; i --){
						scale_buf(bsx,bsy,sc,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX + offs,y + lY,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsx;
					}
					_iFREE_A_(unsigned char,p,bsx * bsy);
				}
				if(((iScrollerElement*)this) -> dir == I_DOWN){
					offs = sp + bsx;
					_iALLOC_A_(unsigned char,p,bsx * bsy);
					for(i = 0; i < pv; i ++){
						scale_buf(bsx,bsy,sc,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsy;
					}
					for(i = pv; i < v; i ++){
						scale_buf(bsx,bsy,((iScrollerElement*)this) -> scale,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsy;
					}
					for(i = v; i < maxv; i ++){
						scale_buf(bsx,bsy,-sc,(unsigned char*)(((iScrollerElement*)this) -> fdata),p);
						if(terrainNum == -1)
							put_buf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode);
						else
							put_tbuf(x + lX,y + lY + offs,bsx,bsy,p,data,null_lev,hide_mode,terrainNum);
						offs += sp + bsy;
					}
					_iFREE_A_(unsigned char,p,bsx * bsy);
				}
				break;
			case I_TERRAIN_ELEM:
				_iALLOC_A_(unsigned char,p1,bsx * bsy);

				if(sc){
					sc = ((iTerrainElement*)this) -> scale_strength;

					if(sc > 0)
						dscale_buf(bsx,bsy,256 - sc,data,p1,lX,0,((iTerrainElement*)this) -> def_null_level);
					else
						dscale_buf(bsx,bsy,256 + sc,data,p1,lX,0,((iTerrainElement*)this) -> def_null_level);
				}
				else {
					memcpy(p1,data,bsx * bsy);
				}

				put_fon(x,y,bsx,bsy,p1);
				_iFREE_A_(unsigned char,p1,bsx * bsy);
				break;
			case I_AVI_ELEM://Draw video
				if(((iAVIElement*)this) -> check_visible()){
					_iALLOC_A_(unsigned char,p,SizeX * SizeY);
					memset(p,0,SizeX * SizeY);

					lev = (null_lev * abs(sc)) >> 8;
					AVIdraw(((iAVIElement*)this) -> data);
					AVIGetData(((iAVIElement*)this) -> data,p);

					if(lev != null_lev){
						i_pal_quant(((iAVIElement*)this) -> palBuf,lev,null_lev);
						flags |= EL_PAL_CHANGE;
					}
					else {
						if(flags & EL_PAL_CHANGE){
							i_pal_quant(((iAVIElement*)this) -> palBuf,lev,null_lev);
							flags ^= EL_PAL_CHANGE;
						}
					}

					if(lev){
						put_frame2scr(x + lX,y + lY,SizeX,SizeY,p);
						if(((iAVIElement*)this) -> ibs)
							((iAVIElement*)this) -> ibs -> show();
					}
					else {
						memset(p,0,SizeX * SizeY);
						put_buf2col(x + lX,y + lY,SizeX,SizeY,p,iAVI_NULL_LEV,1);
						put_map(x + lX,y + lY,SizeX,SizeY);
						if(((iAVIElement*)this) -> ibs)
							((iAVIElement*)this) -> ibs -> show();
					}

					_iFREE_A_(unsigned char,p,SizeX * SizeY);
				}
				else {
					_iALLOC_A_(unsigned char,p,SizeX * SizeY);
					memset(p,0,SizeX * SizeY);
					put_buf2col(x + lX,y + lY,SizeX,SizeY,p,iAVI_NULL_LEV,1);
					_iFREE_A_(unsigned char,p,SizeX * SizeY);
				}
				break;
			case I_AVI_BORDER_ELEM:
				if(((iAVIBorderElement*)this) -> border_type == AVI_FLAT_BORDER){
					_iALLOC_A_(unsigned char,p,SizeX * SizeY);
					memset(p,col,SizeX * SizeY);
					offs = (bs + 1) + (bs + 1) * SizeX;
					dx1 = SizeX - (bs * 2) - 1;
					dy1 = SizeY - (bs * 2) - 1;
					for(i = 0; i < dy1; i ++){
						memset(p + offs,0,dx1);
						offs += SizeX;
					}
					add_buf(x + lX,y + lY,SizeX,SizeY,p,lev);
					_iFREE_A_(unsigned char,p,SizeX * SizeY);
				}
				break;
		}
	}
	else {
		_iALLOC_A_(unsigned char,p,SizeX * SizeY);
		_iALLOC_A_(unsigned char,p1,SizeX * SizeY);
		switch(type){
			case I_STRING_ELEM:
				memset(p,HFONT_NULL_LEVEL,SizeX * SizeY);
				iPutStr2buf(0,0,((iStringElement*)this) -> font,SizeX,SizeY,data,p,0,sc,((iStringElement*)this) -> space);
				scale_buf(SizeX,SizeY,sc,p,p1);
				smooth_buf(SizeX,SizeY,p1,sm);
				if(terrainNum == -1)
					put_buf(x + lX,y + lY,SizeX,SizeY,p1,p1,null_lev,hide_mode);
				else
					put_tbuf(x + lX,y + lY,SizeX,SizeY,p1,p1,null_lev,hide_mode,terrainNum);
				break;
			case I_BITMAP_ELEM:
				scale_buf(SizeX,SizeY,sc,data,p);
				smooth_buf(SizeX,SizeY,p,sm);
				if(terrainNum == -1)
					put_buf(x + lX,y + lY,SizeX,SizeY,p,data,null_lev,hide_mode);
				else
					put_tbuf(x + lX,y + lY,SizeX,SizeY,p,data,null_lev,hide_mode,terrainNum);
				break;
		}
		_iFREE_A_(unsigned char,p1,SizeX * SizeY);
		_iFREE_A_(unsigned char,p,SizeX * SizeY);
	}
}

int iScreenObject::CheckXY(int x,int y)
{
	x -= PosX;
	y -= PosY;

	if(x >= 0 && y >= 0 && x <= SizeX && y <= SizeY)
		return 1;
	return 0;
}

void iScreenObject::init(void)
{
	iTriggerObject* trg;
	int i,sx = 0,sy = 0,dx = 0,dy = 0;
	iScreenElement* p = (iScreenElement*)ElementList -> last;
	while(p){
		if(p -> terrainNum != -1) flags |= OBJ_STORE_TERRAIN;
		if(p -> type == I_BITMAP_ELEM && p -> flags & EL_PICTURE){
			flags |= OBJ_PIC_PRESENT;
			((iBitmapElement*)p) -> bSizeX = p -> SizeX;
			((iBitmapElement*)p) -> bSizeY = p -> SizeY;
		}

		if(!p -> SizeX){
			if(p -> type == I_BITMAP_ELEM || p -> type == I_SCROLLER_ELEM){
				((iBitmapElement*)p) -> init_size();
			}
			if(p -> type == I_STRING_ELEM)
				p -> SizeX = iStrLen((unsigned char*)((iStringElement*)p) -> string,((iStringElement*)p) -> font,((iStringElement*)p) -> space);
			else {
				if(p -> type == I_SCROLLER_ELEM){
					switch(((iScrollerElement*)p) -> dir){
						case I_LEFT:
						case I_RIGHT:
							p -> SizeX = (((iScrollerElement*)p) -> space + ((iScrollerElement*)p) -> bSizeX) * (((iScrollerElement*)p) -> maxValue + 2);
							break;
						case I_UP:
						case I_DOWN:
							p -> SizeX = ((iScrollerElement*)p) -> bSizeX;
							break;
					}
				}
				else {
					if(p -> type == I_S_STRING_ELEM){
						p -> SizeX = aStrLen32(((iS_StringElement*)p) -> string,((iS_StringElement*)p) -> font,((iS_StringElement*)p) -> space);
//						  p -> SizeX = iS_StrLen((unsigned char*)((iS_StringElement*)p) -> string,((iS_StringElement*)p) -> font,((iS_StringElement*)p) -> space);
					}
				}
			}
		}
		if(!p -> SizeY){
			if(p -> type == I_STRING_ELEM)
				p -> SizeY = HFntTable[((iStringElement*)p) -> font] -> SizeY;
			else {
				if(p -> type == I_SCROLLER_ELEM){
					switch(((iScrollerElement*)p) -> dir){
						case I_LEFT:
						case I_RIGHT:
							p -> SizeY = ((iScrollerElement*)p) -> bSizeY;
							break;
						case I_UP:
						case I_DOWN:
							p -> SizeY = (((iScrollerElement*)p) -> space + ((iScrollerElement*)p) -> bSizeY) * (((iScrollerElement*)p) -> maxValue + 2);
							break;
					}
				}
				else {
					if(p -> type == I_S_STRING_ELEM)
						p -> SizeY = aStrHeight32(((iStringElement*)p) -> font);
//						  p -> SizeY = iScrFontTable[((iS_StringElement*)p) -> font] -> SizeY;
				}
			}
		}
		if(p -> type == I_AVI_ELEM){
			iResBuf -> init();
			*iResBuf < iVideoPathDefault < ((iAVIElement*)p) -> avi_name;
			if(!AVIopen(iResBuf->address(),AVI_NOTIMER | AVI_NODRAW | AVI_LOOPING | AVI_NOPALETTE,0,&(((iAVIElement*)p) -> data))){
				iResBuf -> init();
				*iResBuf < iVideoPath < ((iAVIElement*)p) -> avi_name;
				if(!AVIopen(iResBuf -> address(),AVI_NOTIMER | AVI_NODRAW | AVI_LOOPING | AVI_NOPALETTE,0,&(((iAVIElement*)p) -> data))){
					iResBuf -> init();
					*iResBuf < iVideoPath < iEMPTY_AVI;
					if(!AVIopen(iResBuf->address(),AVI_NOTIMER | AVI_NODRAW | AVI_LOOPING | AVI_NOPALETTE,0,&(((iAVIElement*)p) -> data)))
						ErrH.Abort(AVInotFound);
				}
			}
			if(!p -> SizeX) p -> SizeX = AVIwidth(((iAVIElement*)p) -> data);
			if(!p -> SizeY) p -> SizeY = AVIheight(((iAVIElement*)p) -> data);
			AVIclose(((iAVIElement*)p) -> data);

			flags |= OBJ_AVI_PRESENT;
		}
		if(p -> lX + p -> SizeX > sx) sx = p -> lX + p -> SizeX;
		if(p -> lY + p -> SizeY > sy) sy = p -> lY + p -> SizeY;
		if(p -> flags & EL_TEXT_STRING){
			p -> align_x = align_x;
		}
		p = (iScreenElement*)p -> prev;
	}
	if(!SizeX){
		SizeX = sx;
		flags |= OBJ_AUTO_SIZE;
	}
	if(!SizeY){
		SizeY = sy;
		flags |= OBJ_AUTO_SIZE;
	}

	if(!(flags & OBJ_ALIGN)){
		switch(align_x){
			case ALIGN_CENTER:
				PosX = (I_RES_X - SizeX)/2 + align_x_offs;
				break;
			case ALIGN_L:
				PosX = align_x_offs;
				break;
			case ALIGN_R:
				PosX = I_RES_X - SizeX - align_x_offs - 1;
				break;
		}
		switch(align_y){
			case ALIGN_CENTER:
				PosY = (I_RES_Y - SizeY)/2 + align_y_offs;
				break;
			case ALIGN_U:
				PosY = align_y_offs;
				break;
			case ALIGN_D:
				PosY = I_RES_Y - SizeY - align_y_offs - 1;
				break;
		}
		flags |= OBJ_ALIGN;
	}

	p = (iScreenElement*)ElementList -> last;
	while(p){
		switch(p -> align_x){
			case ALIGN_CENTER:
				p -> lX = (SizeX - p -> SizeX)/2 + p -> align_x_offs;
				break;
			case ALIGN_L:
				p -> lX = p -> align_x_offs;
				break;
			case ALIGN_R:
				p -> lX = SizeX - p -> SizeX - p -> align_x_offs - 1;
				break;
		}
		switch(p -> align_y){
			case ALIGN_CENTER:
				p -> lY = (SizeY - p -> SizeY)/2 + p -> align_y_offs;
				break;
			case ALIGN_U:
				p -> lY = p -> align_y_offs;
				break;
			case ALIGN_D:
				p -> lY = SizeY - p -> SizeY - p -> align_y_offs - 1;
				break;
		}
		p -> nlX = p -> lX;
		p -> nlY = p -> lY;
		if(p -> type == I_TERRAIN_ELEM){
			((iTerrainElement*)p) -> pX = PosX + p -> lX;
			((iTerrainElement*)p) -> pY = PosY + p -> lY;
			flags |= OBJ_TERRAIN;
		}

		p = (iScreenElement*)p -> prev;
	}
	if(flags & OBJ_AUTO_SIZE && !(flags & OBJ_SET_COORDS) && !(flags & OBJ_AVI_PRESENT) && !(flags & OBJ_PIC_PRESENT)){
		dx = (PosX > ShadowSize) ? ShadowSize : PosX;
		dy = (PosY + SizeY < I_RES_Y - ShadowSize - 1) ? ShadowSize : (I_RES_Y - (PosY + SizeY) - 1);

		nPosX = PosX;
		nPosY = PosY;

		PosX -= dx;
		SizeX += dx;

		p = (iScreenElement*)ElementList -> last;
		while(p){
			p -> lX += dx;
			p = (iScreenElement*)p -> prev;
		}

		if(PosX + SizeX < I_RES_X - 1)
			SizeX ++;
		if(PosY + SizeY < I_RES_Y - 1)
			SizeY ++;
	}

	if(flags & OBJ_TRIGGER){
		trg = (iTriggerObject*)this;
		for(i = 0; i < trg -> num_state; i ++)
			((iScreenElement*)trg -> StateElem[i]) -> flags |= EL_NO_REDRAW;

		((iScreenElement*)trg -> StateElem[trg -> state]) -> flags ^= EL_NO_REDRAW;
	}

	if(owner && !(flags & OBJ_SET_COORDS)){
		PosX += ((iScreen*)owner) -> ScreenOffs;
	}
	flags |= OBJ_SET_COORDS;

	if(PosX < 0 || PosX >= I_MAP_RES_X - SizeX){
		ErrH.Abort("Object PosX out of map...",XERR_USER,PosX,ID_ptr.c_str());
	}
	if(PosY < 0 || PosY >= I_RES_Y - SizeY){
		ErrH.Abort("Object PosY out of map...",XERR_USER,PosY,ID_ptr.c_str());
	}
}

void iScreenObject::redraw(int mode)
{
	int hide_mode = flags & OBJ_HIDE;
	iScreenElement* p = (iScreenElement*)ElementList -> last;

	if(flags & OBJ_PIC_PRESENT && flags & OBJ_CLEAR_FON){
		if(FlushX == -1){
			iregRender(PosX,PosY,PosX + SizeX,PosY + SizeY);
		}
		else {
			iregRender(FlushX,FlushY,FlushX + FlushSX,FlushY + FlushSY);
		}
		flags ^= OBJ_CLEAR_FON;
	}

	if(flags & OBJ_REINIT){
		FlushX = PosX;
		FlushY = PosY;
		FlushSX = SizeX;
		FlushSY = SizeY;

		if(flags & OBJ_AUTO_SIZE){
			SizeX = SizeY = 0;
			PosX = nPosX;
			PosY = nPosY;

			while(p){
				p -> lX = p -> nlX;
				p -> lY = p -> nlY;

				p -> SizeX = p -> SizeY = 0;

				p = (iScreenElement*)p -> prev;
			}

			flags &= ~OBJ_ALIGN;
			flags &= ~OBJ_SET_COORDS;
		}
		init();
		flags &= ~OBJ_REINIT;

		if(FlushX > PosX) FlushX = PosX;
		if(FlushX + FlushSX < PosX + SizeX) FlushSX = PosX + SizeX - FlushX;
		if(FlushY > PosY) FlushY = PosY;
		if(FlushY + FlushSY < PosY + SizeY) FlushSY = PosY + SizeY - FlushY;
		getfon();
	}

	p = (iScreenElement*)ElementList -> last;
	while(p){
		if(!(p -> flags & EL_NO_REDRAW) && p -> type != I_S_STRING_ELEM && p -> type != I_AVI_ELEM && !(p -> flags & EL_PICTURE))
			p -> redraw(PosX,PosY,curHeightScale,SmoothLevel,hide_mode);
		p = (iScreenElement*)p -> prev;
	}
	if(!(flags & OBJ_AVI_PRESENT) && !(flags & OBJ_AVI_BORDER) && !(flags & OBJ_PIC_PRESENT)){
		if(FlushX == -1){
			iregRender(PosX,PosY,PosX + SizeX,PosY + SizeY);
		}
		else {
			iregRender(FlushX,FlushY,FlushX + FlushSX,FlushY + FlushSY);
		}
	}

	p = (iScreenElement*)ElementList -> last;
	while(p){
		if(!(p -> flags & EL_NO_REDRAW) && (p -> flags & EL_PICTURE || p -> type == I_S_STRING_ELEM || p -> type == I_AVI_ELEM))
			p -> redraw(PosX,PosY,curHeightScale,SmoothLevel,hide_mode);
		p = (iScreenElement*)p -> prev;
	}
#ifdef _DEBUG
	if(iBoundsLog)
		map_rectangle(PosX + 1,PosY + 1,SizeX - 2,SizeY - 2,111);
#endif
	if(!(flags & OBJ_AVI_PRESENT) || flags & OBJ_AVI_STOPPED){
		if(FlushX == -1){
			put_map(PosX,PosY,SizeX,SizeY);
		}
		else {
			put_map(FlushX,FlushY,FlushSX,FlushSY);
		}
	}
	if(mode)
		flush();
}

void iScreenObject::flush(void)
{
	int x0,x1,sx;
	if(FlushX == -1 ){
		x0 = PosX - iScreenOffs;
		x1 = x0 + SizeX;

		if(x0 < 0) x0 = 0;
		if(x1 >= I_RES_X) x1 = I_RES_X - 1;

		sx = x1 - x0;

		if(sx > 0)
			XGR_Flush(x0,PosY,sx,SizeY);
	}
	else {
		x0 = FlushX - iScreenOffs;
		x1 = x0 + FlushSX;

		if(x0 < 0) x0 = 0;
		if(x1 >= I_RES_X) x1 = I_RES_X - 1;

		sx = x1 - x0;

		XGR_Flush(x0,FlushY,sx,FlushSY);
		FlushX = FlushY = -1;
	}
}

void iScreenElement::scale_quant(void)
{
	scale += scale_delta;
	if(scale > 256)
		scale = 256;
	if(scale < -256)
		scale = -256;
}

void iTerrainElement::level_quant(void)
{
	null_level += level_delta;
}

void iScreenObject::ScaleQuant(void)
{
	curHeightScale += HeightScaleDelta;
	if(curHeightScale < -256) curHeightScale = -256;
	if(curHeightScale > 256) curHeightScale = 256;

	if(flags & OBJ_TERRAIN)
		((iTerrainElement*)ElementList -> last) -> level_quant();
}

void iScreenObject::SmoothQuant(void)
{
	SmoothCount ++;
	if(SmoothCount >= SmoothTime){
		SmoothLevel += SmoothDelta;
		if(SmoothLevel < I_MIN_SMOOTH_INDEX)
			SmoothLevel = I_MIN_SMOOTH_INDEX;
		if(SmoothLevel > I_MAX_SMOOTH_INDEX)
			SmoothLevel = I_MAX_SMOOTH_INDEX;
		SmoothCount = 0;
	}
}

void iScreen::copy(iScreen* dest)
{
	iScreenEvent* dest_ev,*ev;
	iTriggerObject* trg;
	iScreenObject* tmp,*p;
	p = (iScreenObject*)objList -> last;
	while(p){
		if(p -> flags & OBJ_TRIGGER){
			trg = new iTriggerObject;
			tmp = (iScreenObject*)trg;
		}
		else
			tmp = new iScreenObject;

		tmp -> init_id(p->ID_ptr.c_str());
		p -> copy(tmp);
		dest -> add_object(tmp);
		p = (iScreenObject*)p -> prev;
	}
	p = (iScreenObject*)TerrainList -> last;
	while(p){
		tmp = new iScreenObject;
		tmp -> init_id(p->ID_ptr.c_str());
		p -> copy(tmp);
		dest -> add_terr_object(tmp);
		p = (iScreenObject*)p -> prev;
	}

	ev = (iScreenEvent*)EventList -> last;
	while(ev){
		dest_ev = new iScreenEvent;
		ev -> copy(dest_ev);
		dest -> EventList -> connect((iListElement*)dest_ev);
		ev = (iScreenEvent*)ev -> prev;
	}

	dest -> ScreenOffs = ScreenOffs;
}

iListElement* iScreen::get_object(const char* p)
{
	iObjID* id = (iObjID*)ObjIDs -> last;
	while(id){
		if(id -> compare(p))
			return id -> obj;
		id = (iObjID*)id -> prev;
	}
	return NULL;
}

iListElement* iScreenDispatcher::get_object(const char* p)
{
	iObjID* id = (iObjID*)ScreenIDs -> last;
	while(id){
		if(id -> compare(p))
			return id -> obj;
		id = (iObjID*)id -> prev;
	}
	return NULL;
}

void iScreen::add_object(iScreenObject* p)
{
	iObjID* id;
	iListElement* obj;
	if(iScrDisp -> check_id((char*)p->ID_ptr.c_str()))
		ErrH.Abort("Duplicate object ID...",XERR_USER,0,p->ID_ptr.c_str());

	if(check_id((char*)p->ID_ptr.c_str())){
		obj = get_object(p->ID_ptr.c_str());
		if(obj != (iListElement*)p){
			ErrH.Abort("Duplicate object ID...",XERR_USER,0,p->ID_ptr.c_str());
			return;
		} else {
			id = new iObjID;
			id -> init(p->ID_ptr.c_str(), p->ID);
			id -> obj = (iListElement*)p;
		}
	} else {
		id = new iObjID;
		id -> init(p->ID_ptr.c_str(), p -> ID);
		id -> obj = (iListElement*)p;
	}

	p -> owner = (iListElement*)this;

	ObjIDs -> connect((iListElement*)id);
	objList -> connect((iListElement*)p);
}

void iScreen::add_terr_object(iScreenObject* p)
{
	p -> owner = (iListElement*)this;
	TerrainList -> connect((iListElement*)p);
}

void iScreen::redraw(int mode)
{
	iScreenObject* p;
	p = (iScreenObject*)objList -> last;
	while(p){
		p -> putfon();
		p = (iScreenObject*)p -> prev;
	}
	p = (iScreenObject*)TerrainList -> last;
	while(p){
		p -> putfon();
		p = (iScreenObject*)p -> prev;
	}
	p = (iScreenObject*)TerrainList -> last;
	while(p){
		p -> redraw(mode);
		p = (iScreenObject*)p -> prev;
	}
	p = (iScreenObject*)objList -> last;
	while(p){
		p -> redraw(mode);
		p = (iScreenObject*)p -> prev;
	}
}

void iScreenDispatcher::redraw(int mode)
{
	iScreenObject* p;
	if(!actIntLog){
		p = (iScreenObject*)ObjHeap -> last;
		while(p){
			p -> redraw(mode);
			p = (iScreenObject*)p -> prev;
		}
	}
	curScr -> getfon();
	curScr -> redraw(mode);
}

void iScreen::HandleEvent(iScreenEvent* ev)
{
	iScreenEventCommand* p = (iScreenEventCommand*)ev -> CommSeq -> last;
	while(p){
		if(p -> EvType == EV_OBJECT_JUMP){
			if(p -> objID == curObj -> ID)
				return;
		}
		p = (iScreenEventCommand*)p -> prev;
	}
	if(!(ev -> flags & EV_ACTIVE)){
		ev -> flags |= EV_ACTIVE;
		ev -> cur_time = 0;
		iScrDisp -> ActiveEv = ev;
		SOUND_SELECT();
	}
}

void iScreen::CheckScanCode(int sc)
{
	int x = 0,y = 0,mouse_sc = 0;
	iScreenObject* obj;
	iListElement* cd;
	iScreenEvent* p;

	if(sc < 0){
		mouse_sc = 1;
		x = iMouseX + iScreenOffs;
		y = iMouseY;
	}

	obj = (iScreenObject*)objList -> last;
	while(obj){
		if(!iScrDisp -> ActiveEv){
			if(!mouse_sc || obj -> CheckXY(x,y)){
				p = (iScreenEvent*)obj -> EventList -> last;
				while(p){
					if (bool(obj->flags & OBJ_LOCKED) == bool(p->flags & EV_IF_LOCKED)) {
						cd = p -> codes -> last;
						while(cd){
							if(((iScanCode*)cd) -> code == sc && (!(p -> flags & EV_IF_SELECTED) || obj -> flags & OBJ_SELECTED))
								HandleEvent(p);
							cd = cd -> prev;
						}
					}
					p = (iScreenEvent*)p -> prev;
				}
			}
		}
		obj = (iScreenObject*)obj -> prev;
	}

	if(!iScrDisp -> ActiveEv){
		p = (iScreenEvent*)EventList -> last;
		while(p){
			cd = p -> codes -> last;
			while(cd){
				if(((iScanCode*)cd) -> code == sc)
					HandleEvent(p);
				cd = cd -> prev;
			}
			p = (iScreenEvent*)p -> prev;
		}
	}
}

void iScreen::prepare(void)
{
	iScreenObject* p = (iScreenObject*)objList -> last;
	while(p){
		p -> flags &= ~OBJ_HIDE;
		if(!(p -> flags & OBJ_NOT_UNLOCK)) p -> flags &= ~OBJ_LOCKED;
		if(p -> flags & OBJ_SELECTABLE){
			if(p -> flags & OBJ_SELECTED)
				p -> curHeightScale = 256;
			else
				p -> curHeightScale = I_UNSELECT_SCALE;
		}
		else
			p -> curHeightScale = 256;

		p -> SmoothLevel = 0;
		p = (iScreenObject*)p -> prev;
	}
	p = (iScreenObject*)TerrainList -> last;
	while(p){
		p -> flags &= ~OBJ_HIDE;
		p -> curHeightScale = ((iTerrainElement*)p -> ElementList -> last) -> scale_strength;
		p -> SmoothLevel = 0;
		p = (iScreenObject*)p -> prev;
	}
	XGR_MouseSetPromptData(promptData);
	XGR_MouseObj.PromptDeltaX = ScreenOffs;
	if(actIntLog)
		XGR_MouseObj.PromptColor = aciCurColorScheme[FM_SELECT_START] | (aciCurColorScheme[FM_SELECT_SIZE] << 16);
}

void iScreenDispatcher::HandleEvent(iScreenEvent* ev)
{
	iScreenEventCommand* p = (iScreenEventCommand*)ev -> CommSeq -> last;
	while(p){
		if(p -> EvType == EV_OBJECT_JUMP){
			if(p -> objID == curScr -> curObj -> ID)
				return;
		}
		p = (iScreenEventCommand*)p -> prev;
	}
	if(!(ev -> flags & EV_ACTIVE)){
		ev -> flags |= EV_ACTIVE;
		ev -> cur_time = 0;
		ActiveEv = ev;
		SOUND_SELECT();
	}
}

void iScreenDispatcher::CheckScanCode(int sc)
{
	int x = 0,y = 0,mouse_sc = 0;
	iScreenObject* obj;
	iListElement* cd;
	iScreenEvent* p;

	if(sc < 0){
		mouse_sc = 1;
		x = iMouseX + iScreenOffs;
		y = iMouseY;
	}

	if(!(curScr -> flags & iSCR_BLOCK_GLOBAL_OBJ)){
		obj = (iScreenObject*)ObjHeap -> last;
		while(obj){
			if(!ActiveEv){
				if(!mouse_sc || obj -> CheckXY(x,y)){
					p = (iScreenEvent*)obj -> EventList -> last;
					while(p){
						cd = p -> codes -> last;
						while(cd){
							if(((iScanCode*)cd) -> code == sc && (!(p -> flags & EV_IF_SELECTED) || obj -> flags & OBJ_SELECTED) && (!(p -> flags & EV_IF_NOT_SELECTED) || !(obj -> flags & OBJ_SELECTED)))
								HandleEvent(p);
							cd = cd -> prev;
						}
						p = (iScreenEvent*)p -> prev;
					}
				}
			}
			obj = (iScreenObject*)obj -> prev;
		}
	}
}

void iScreenDispatcher::prepare(void)
{
	iScreen* scr;
	iScreenObject* p = (iScreenObject*)ObjHeap -> last;
	while(p){
		if(p -> flags & OBJ_SELECTABLE){
			if(p -> flags & OBJ_SELECTED)
				p -> curHeightScale = 256;
			else
				p -> curHeightScale = I_UNSELECT_SCALE;
		}
		else
			p -> curHeightScale = 256;

		p = (iScreenObject*)p -> prev;
	}
	scr = (iScreen*)last;
	while(scr){
		if(scr != curScr){
			iScreenObject* p = (iScreenObject*)scr -> objList -> last;
			while(p){
				p -> curHeightScale = 0;
				p = (iScreenObject*)p -> prev;
			}
		}
		scr = (iScreen*)scr -> prev;
	}
}

void iScreenDispatcher::EventQuant(void)
{
	int hp_offs = iHeapOffs;
	iListElementPtr* tmp;
	iScreenObject* obj;
	if(flags & SD_INPUT_STRING){
		input_string_quant();
	}
	else {
		if(ActiveEv){
			ProcessEvent(ActiveEv);
		}
	}

	obj = (iScreenObject*)curScr -> objList -> last;
	while(obj){
		if(obj -> flags & OBJ_AVI_PRESENT)
			set_obj_redraw(obj);
		if(obj -> flags & OBJ_MUST_REDRAW){
			obj -> flags ^= OBJ_MUST_REDRAW;
			set_obj_redraw(obj);
		}
		obj = (iScreenObject*)obj -> prev;
	}

	redraw_quant();

	tmp = (iListElementPtr*)drwObjHeap -> last;
	while(tmp){
		obj = (iScreenObject*)tmp -> ptr;
		obj -> flush();
		obj -> flags &= ~OBJ_REDRAW;
		tmp = (iListElementPtr*)tmp -> prev;
	}
	drwObjHeap -> init_list();
	iHeapOffs = hp_offs;

	if(flags & SD_PAL_CHANGE){
		pal_quant();
	}

#ifdef SCREENSHOT
	shotMake(1);
#endif
}

void iScreenDispatcher::ProcessEvent(iScreenEvent* p)
{
	int i,sc;
	iScreenObject* obj;
	iScrollerElement* scl;
	iTriggerObject* trg;
	iScreenEventCommand* cm;
	iScreenElement* el;
	iAVIElement* avi_el;
	iList* list;
	iListElement* list_el;

	cm = (iScreenEventCommand*)p -> CommSeq -> last;
	while(cm){
		if(!(p -> flags & EV_MULTI_LINE) || !cm -> LineID || cm -> LineID == iEvLineID){
			if(cm -> time + cm -> start_time == p -> cur_time + 1){
				EventFlag = cm -> EvType;
				CurEvComm = cm;
			}
			if(cm -> start_time == p -> cur_time){
				EventFlag = cm -> EvType;
				CurEvComm = cm;
				i_preExtQuant();
				EventFlag = 0;
			}

			if(cm -> start_time == p -> cur_time){
				switch(cm -> EvType){
					case EV_LOCK_OBJ:
						obj = (iScreenObject*)cm -> obj;
						obj -> flags |= OBJ_LOCKED;
						break;
					case EV_UNLOCK_OBJ:
						obj = (iScreenObject*)cm -> obj;
						obj -> flags &= ~OBJ_LOCKED;
						break;
					case EV_EXT_EVENT:
						iHandleExtEvent(cm -> value,cm -> data0);
						break;
					case EV_RESET_TEXTS:
						curText = NULL;
						iEvLineID = iTEXT_NORMAL;
						break;
					case EV_NEXT_TEXT:
						if(copy_text_next((iScreen*)cm -> obj,cm -> value)) return;
						break;
					case EV_PREV_TEXT:
						if(copy_text_prev((iScreen*)cm -> obj,cm -> value)) return;
						break;
					case EV_STOP_AVI:
						avi_el = (iAVIElement*)cm -> obj;
						obj = (iScreenObject*)avi_el -> owner;
						avi_el -> flags |= AVI_STOPPED;
						obj -> flags |= OBJ_AVI_STOPPED;
						break;
					case EV_PLAY_AVI:
						avi_el = (iAVIElement*)cm -> obj;
						obj = (iScreenObject*)avi_el -> owner;
						avi_el -> flags &= ~AVI_STOPPED;
						obj -> flags &= ~OBJ_AVI_STOPPED;
						break;
					case EV_RISE_OBJECT:
						obj = (iScreenObject*)cm -> obj;
						if(!obj) obj = curScr -> curObj;
						obj -> HeightScaleDelta = (256 - obj -> curHeightScale) / cm -> time;
						break;
					case EV_HIDE_OBJECT:
						obj = (iScreenObject*)cm -> obj;
						if(!obj) obj = curScr -> curObj;
						obj -> HeightScaleDelta = -obj -> curHeightScale / cm -> time;
						obj -> flags |= OBJ_HIDE;
						break;
					case EV_LOW_OBJECT:
						obj = (iScreenObject*)cm -> obj;
						if(!obj) obj = curScr -> curObj;
						obj -> HeightScaleDelta = (I_UNSELECT_SCALE - obj -> curHeightScale) / cm -> time;
						break;
					case EV_START_DEMO:
//						startRecorder();
						break;
					case EV_SCALE_OBJECT:
						obj = (iScreenObject*)cm -> obj;
						if(!obj) obj = curScr -> curObj;
						obj -> HeightScaleDelta = (cm -> value - obj -> curHeightScale) / cm -> time;
						break;
					case EV_SMOOTH_OBJECT:
						obj = (iScreenObject*)cm -> obj;
						if(!obj) obj = curScr -> curObj;

						if(cm -> value == obj -> SmoothLevel){
							obj -> SmoothTime = 0;
							obj -> SmoothDelta = 0;
						}
						else {
							obj -> SmoothTime = cm -> time / (cm -> value - obj -> SmoothLevel);
							if(obj -> SmoothTime < 0){
								obj -> SmoothDelta = -1;
								obj -> SmoothTime = -obj -> SmoothTime;
							}
							else {
								obj -> SmoothDelta = 1;
							}
						}

						obj -> SmoothCount = obj -> SmoothTime >> 1;
						break;
					case EV_OBJECT_JUMP:
						curScr -> curObj -> flags ^= OBJ_SELECTED;
						curScr -> curObj = (iScreenObject*)cm -> obj;
						curScr -> curObj -> flags |= OBJ_SELECTED;
						break;
					case EV_SCREEN_JUMP:
						curScr -> free_data();
						curScr = (iScreen*)cm -> obj;
						curScr -> prepare();
						curScr -> load_data();
						break;
					case EV_EXT_SCREEN_JUMP:
						curScr -> free_data();
						curScr = iScrDisp -> get_ext_screen(cm -> pvalue);
						if(!curScr)
							ErrH.Abort("Can't find screen...",XERR_USER,0,cm -> pvalue);

						curScr -> prepare();
						curScr -> load_data();
						obj = (iScreenObject*)curScr -> TerrainList -> last;
						while(obj){
							obj -> flags |= OBJ_HIDE;
							obj -> curHeightScale = 0;
							obj = (iScreenObject*)obj -> prev;
						}
						obj = (iScreenObject*)curScr -> objList -> last;
						while(obj){
							obj -> flags |= OBJ_HIDE;
							obj -> curHeightScale = 0;
							obj = (iScreenObject*)obj -> prev;
						}
						break;
					case EV_RISE_SCREEN:
						flags |= SD_GETFON;
						obj = (iScreenObject*)curScr -> TerrainList -> last;
						while(obj){
							if(obj -> flags & OBJ_SELECTED || !(obj -> flags & OBJ_SELECTABLE))
								sc = ((iTerrainElement*)obj -> ElementList -> last) -> scale_strength;
							else
								sc = I_UNSELECT_SCALE;

							obj -> flags |= OBJ_HIDE;
							obj -> curHeightScale = 0;
							obj -> HeightScaleDelta = (sc - obj -> curHeightScale) / cm -> time;

							sc = ((iTerrainElement*)obj -> ElementList -> last) -> def_null_level;
							((iTerrainElement*)obj -> ElementList -> last) -> level_delta = (sc - ((iTerrainElement*)obj -> ElementList -> last) -> null_level) / cm -> time;

							obj = (iScreenObject*)obj -> prev;
						}
						obj = (iScreenObject*)curScr -> objList -> last;
						while(obj){
							if(obj -> flags & OBJ_SELECTED || !(obj -> flags & OBJ_SELECTABLE))
								sc = 256;
							else
								sc = I_UNSELECT_SCALE;

							obj -> flags |= OBJ_HIDE;
							obj -> curHeightScale = 0;
							obj -> HeightScaleDelta = (sc - obj -> curHeightScale) / cm -> time;
							obj = (iScreenObject*)obj -> prev;
						}
						break;
					case EV_HIDE_SCREEN:
						flags |= SD_GETFON;
						obj = (iScreenObject*)curScr -> objList -> last;
						while(obj){
							obj -> HeightScaleDelta = -obj -> curHeightScale / cm -> time;
							obj -> flags |= OBJ_HIDE;
							obj = (iScreenObject*)obj -> prev;
						}
						obj = (iScreenObject*)curScr -> TerrainList -> last;
						while(obj){
							obj -> HeightScaleDelta = -obj -> curHeightScale / cm -> time;
							obj -> flags |= OBJ_HIDE;

							((iTerrainElement*)obj -> ElementList -> last) -> level_delta = -((iTerrainElement*)obj -> ElementList -> last) -> null_level / cm -> time;

							obj = (iScreenObject*)obj -> prev;
						}
						break;
					case EV_CHANGE_SCROLLER:
						scl = (iScrollerElement*)cm -> obj;
						if(scl -> check_xy(iMouseX + iScreenOffs,iMouseY)){
							scl -> change_val(iMouseX + iScreenOffs,iMouseY);
							scl -> scale_delta = (scl -> Value > scl -> prevValue) ? 512 / cm -> time : -512 / cm -> time;
						}
						else {
							scl -> prevValue = scl -> Value;
							scl -> scale_delta = 0;
						}
						break;
					case EV_CHANGE_TRIGGER:
						trg = (iTriggerObject*)cm -> obj;
						trg -> change_state(cm -> time);
						break;
					case EV_PREV_TRIGGER:
						trg = (iTriggerObject*)cm -> obj;
						trg -> change_state(cm -> time,-2);
						break;
					case EV_SET_TRIGGER:
						trg = (iTriggerObject*)cm -> obj;
						if(trg -> state != cm -> value)
							trg -> change_state(cm -> time,cm -> value);
						else
							trg -> prev_state = trg -> state;
						break;
					case EV_CHANGE_AVI:
						avi_el = (iAVIElement*)cm -> obj;
						avi_el -> change_avi(cm -> pvalue);
						avi_el -> init_size();
						avi_el -> init_align();
						break;
					case EV_NEXT_AVI:
						avi_el = (iAVIElement*)cm -> obj;
						avi_el -> next_avi();
						avi_el -> init_size();
						avi_el -> init_align();
						break;
					case EV_PREV_AVI:
						avi_el = (iAVIElement*)cm -> obj;
						avi_el -> prev_avi();
						avi_el -> init_size();
						avi_el -> init_align();
						break;
					case EV_LIST_CHANGE_AVI:
						avi_el = (iAVIElement*)cm -> obj;
						list = (iList*)cm -> pvalue;
						list_el = list -> first;
						list -> index ++;
						if(list -> index >= list -> Size)
							list -> index = 0;
						for(i = 0; i < list -> index; i ++)
							list_el = list_el -> next;

						avi_el -> change_avi((char *)list_el->ID_ptr.c_str());
						avi_el -> init_size();
						avi_el -> init_align();
						break;
					case EV_INPUT_STRING:
						if(!(flags & SD_INPUT_STRING) && !(flags & SD_FINISH_INPUT)){
							el = (iScreenElement*)cm -> obj;
							init_input_string(el);
							cur_max_input = cm -> value;
						}
						break;
					case EV_MOVE_SCREEN:
						aci_move_screen(cm -> value,cm -> time);
						p -> cur_time = cm -> time + cm -> start_time - 1;
						break;
					case EV_MOVE2SCREEN:
						move2screen((iScreen*)cm -> obj,cm -> time);
						p -> cur_time = cm -> time + cm -> start_time - 1;
						break;
					case EV_SEND2ACTINT:
						aci_SendEvent(cm -> value + ACI_MAX_EVENT);
						break;
					case EV_SEND2ML_DISP:
						aciML_SendEvent(cm -> value);
						break;
					case EV_ML_EVENT:
						aciML_SendEvent(cm -> value,cm -> data0,cm -> data1);
						break;
					case EV_SEND_DATA2ACTINT:
						aci_SendEvent((cm -> value & 0xFFFF) + ACI_MAX_EVENT,(cm -> value >> 16) & 0xFFFF);
						break;
					case EV_SET_LINE_ID:
						iEvLineID = cm -> value;
						break;
					case EV_END_EVENT:
						p -> cur_time = p -> time;
						break;
					case EV_EXIT_EVENT:
						p -> cur_time = p -> time;
						aciChangeLineID();
						break;
					case EV_CHECK_CREDITS:
						if(!aciCheckCredits())
							p -> cur_time = p -> time;
						break;
				}
			}
			if(cm -> start_time <= p -> cur_time && cm -> time + cm -> start_time > p -> cur_time){
				switch(cm -> EvType){
					case EV_RISE_OBJECT:
					case EV_LOW_OBJECT:
					case EV_SCALE_OBJECT:
						obj = (iScreenObject*)cm -> obj;
						if(!obj) obj = curScr -> curObj;
						obj -> ScaleQuant();

						if(cm -> time + cm -> start_time == p -> cur_time + 1)
							 obj -> flags &= ~OBJ_HIDE;

						set_obj_redraw(obj);
						break;
					case EV_SMOOTH_OBJECT:
						obj = (iScreenObject*)cm -> obj;
						if(!obj) obj = curScr -> curObj;
						obj -> SmoothQuant();

						set_obj_redraw(obj);
						break;
					case EV_HIDE_OBJECT:
						obj = (iScreenObject*)cm -> obj;
						if(!obj) obj = curScr -> curObj;

						if(cm -> time + cm -> start_time == p -> cur_time + 1)
							obj -> curHeightScale = 0;
						else
							obj -> ScaleQuant();

						set_obj_redraw(obj);
						break;
					case EV_HIDE_SCREEN:
						if(cm -> time + cm -> start_time == p -> cur_time + 1)
							flags |= SD_END_GETFON;
						obj = (iScreenObject*)curScr -> objList -> last;
						while(obj){
							if(cm -> time + cm -> start_time == p -> cur_time + 1)
								obj -> curHeightScale = 0;
							else
								obj -> ScaleQuant();

							obj = (iScreenObject*)obj -> prev;
						}
						obj = (iScreenObject*)curScr -> TerrainList -> last;
						while(obj){
							if(cm -> time + cm -> start_time == p -> cur_time + 1){
								((iTerrainElement*)obj -> ElementList -> last) -> null_level = 0;
								obj -> curHeightScale = 0;
							}
							else
								obj -> ScaleQuant();

							obj = (iScreenObject*)obj -> prev;
						}
						set_scr_redraw(curScr);
						break;
					case EV_RISE_SCREEN:
						if(cm -> time + cm -> start_time == p -> cur_time + 1)
							flags |= SD_END_GETFON;
						obj = (iScreenObject*)curScr -> objList -> last;
						while(obj){
							obj -> ScaleQuant();
							if(cm -> time + cm -> start_time == p -> cur_time + 1){
								obj -> flags &= ~OBJ_HIDE;
							}
							obj = (iScreenObject*)obj -> prev;
						}
						obj = (iScreenObject*)curScr -> TerrainList -> last;
						while(obj){
							obj -> ScaleQuant();
							if(cm -> time + cm -> start_time == p -> cur_time + 1){
								((iTerrainElement*)obj -> ElementList -> last) -> null_level = ((iTerrainElement*)obj -> ElementList -> last) -> def_null_level;
								obj -> flags &= ~OBJ_HIDE;
							}
							obj = (iScreenObject*)obj -> prev;
						}
						set_scr_redraw(curScr);
						break;
					case EV_EXIT:
						if(!actIntLog || (aScrDisp -> curMatrix || !cm -> value)){
							iScrDisp -> flags |= SD_EXIT;
							iScrDisp -> ret_val = cm -> value;
						}
						break;
					case EV_CHANGE_SCROLLER:
						scl = (iScrollerElement*)cm -> obj;
						obj = (iScreenObject*)scl -> owner;
						if(cm -> time + cm -> start_time == p -> cur_time + 1){
							scl -> prevValue = scl -> Value;
						}
						else
							scl -> change_quant();

						set_obj_redraw(obj);
						break;
					case EV_CHANGE_TRIGGER:
					case EV_SET_TRIGGER:
					case EV_PREV_TRIGGER:
						trg = (iTriggerObject*)cm -> obj;
						if(trg -> state != trg -> prev_state){
							if(cm -> time + cm -> start_time == p -> cur_time + 1){
								el = (iScreenElement*)trg -> StateElem[trg -> prev_state];
								el -> scale = 0;
								el -> flags |= EL_NO_REDRAW;
								if(el -> type == I_AVI_ELEM) trg -> flags |= OBJ_AVI_STOPPED;

								el = (iScreenElement*)trg -> StateElem[trg -> state];
								el -> scale = 256;
								el -> flags &= ~EL_HIDE;
								if(el -> type == I_AVI_ELEM) trg -> flags &= ~OBJ_AVI_STOPPED;
							}
							else {
								el = (iScreenElement*)trg -> StateElem[trg -> prev_state];
								el -> scale_quant();

								el = (iScreenElement*)trg -> StateElem[trg -> state];
								el -> scale_quant();
								if(el -> type == I_AVI_ELEM) trg -> flags &= ~OBJ_AVI_STOPPED;
							}
							set_obj_redraw((iScreenObject*)trg);
						}
						break;
					case EV_INPUT_STRING:
						if(cm -> time + cm -> start_time == p -> cur_time + 1)
							flags &= ~(SD_INPUT_STRING | SD_FINISH_INPUT);
						break;
				}
			}
		}
		cm = (iScreenEventCommand*)cm -> prev;
	}
	p -> cur_time ++;
	if(p -> cur_time >= p -> time){
		p -> flags &= ~EV_ACTIVE;
		ActiveEv = NULL;
	}
}

void iScreenDispatcher::free_mem(void)
{
	iScreenObject* obj;

	if(!actIntLog){
		obj = (iScreenObject*)ObjHeap -> last;
		while(obj){
			obj -> free_data();
			obj = (iScreenObject*)obj -> prev;
		}
	}

	curScr -> free_data();

	_iFREE_HEAP_(I_MEM_HEAP_SIZE,char);
}

void iScreenDispatcher::alloc_mem(void)
{
	iScreenObject* obj;

	_iALLOC_HEAP_(I_MEM_HEAP_SIZE,char);

	if(!actIntLog){
		obj = (iScreenObject*)ObjHeap -> last;
		while(obj){
			obj -> load_data();
			obj -> getfon();
			obj = (iScreenObject*)obj -> prev;
		}
	}

	curScr -> load_data();
	ret_val = 0;
}

iScreenFont::iScreenFont(char* p,int sz)
{
	data = p;
	SizeX = 8;
	SizeY = sz / 256;

	int i,j,s,offs = 0;
	int align_left,align_right,fl,fr;

	LeftOffs = new char[256];
	RightOffs = new char[256];
	for(s = 0; s < 256; s ++){
		offs = SizeY * s;
		align_left = align_right = 3;
		for(i = 0; i < SizeY; i ++){
			fl = fr = 0;
			for(j = 0; j < SizeX; j ++){
				if(!fr && p[offs] & (1 << j) && j < align_right){
					align_right = j;
					fr = 1;
				}
				if(!fl && p[offs] & (1 << (7 - j)) && j < align_left){
					align_left = j;
					fl = 1;
				}
			}
			offs ++;
		}
		if(s != ' '){
			LeftOffs[s] = align_left;
			RightOffs[s] = align_right;
		}
		else {
			LeftOffs[s] = 0;
			RightOffs[s] = 0;
		}
	}
}

void iScreenElement::init_align(void)
{
	iScreenObject* own = (iScreenObject*)owner;
	switch(align_x){
		case ALIGN_CENTER:
			lX = (own -> SizeX - SizeX)/2 + align_x_offs;
			break;
		case ALIGN_L:
			lX = align_x_offs;
			break;
		case ALIGN_R:
			lX = own -> SizeX - SizeX - align_x_offs - 1;
			break;
	}
	switch(align_y){
		case ALIGN_CENTER:
			lY = (own -> SizeY - SizeY)/2 + align_y_offs;
			break;
		case ALIGN_U:
			lY = align_y_offs;
			break;
		case ALIGN_D:
			lY = own -> SizeY - SizeY - align_y_offs - 1;
			break;
	}
}

void iScreen::prepare_avi(void)
{
	iScreenElement* el;
	iScreenObject* p = (iScreenObject*)objList -> last;
	while(p){
		el = (iScreenElement*)p -> ElementList -> last;
		while(el){
			if(el -> type == I_AVI_ELEM){
				((iAVIElement*)el) -> load();
				((iAVIElement*)el) -> init_size();
				((iAVIElement*)el) -> init_align();
			}
			el = (iScreenElement*)el -> prev;
		}
		p = (iScreenObject*)p -> prev;
	}
}

void iScreen::close_avi(void)
{
	iScreenElement* el;
	iScreenObject* p = (iScreenObject*)objList -> last;
	while(p){
		el = (iScreenElement*)p -> ElementList -> last;
		while(el){
			if(el -> type == I_AVI_ELEM)
				((iAVIElement*)el) -> free();
			el = (iScreenElement*)el -> prev;
		}
		p = (iScreenObject*)p -> prev;
	}
}

int iScreen::check_id(char* p)
{
	iObjID* id = (iObjID*)ObjIDs -> last;
	while(id){
		if(id -> compare(p))
			return 1;
		id = (iObjID*)id -> prev;
	}
	return 0;
}

int iScreenDispatcher::check_id(char* p)
{
	iObjID* id = (iObjID*)ScreenIDs -> last;
	while(id){
		if(id -> compare(p))
			return 1;
		id = (iObjID*)id -> prev;
	}
	return 0;
}

void iScreen::init_ids(void)
{
	iObjID* id;
	iListElement* p;
	iScreenElement* el;
	iScreenObject* obj;
	obj = (iScreenObject*)objList -> last;
	while(obj){
		p = get_object(obj->ID_ptr.c_str());
		if(!p){
			id = new iObjID;
			id -> init(obj->ID_ptr.c_str(), obj -> ID,obj);
			ObjIDs -> connect((iListElement*)id);
		}
		else {
			if(p != (iListElement*)obj)
				ErrH.Abort("Duplicate object ID...",XERR_USER,0,obj->ID_ptr.c_str());
		}

		el = (iScreenElement*)obj -> ElementList -> last;
		while(el){
			if(el->ID_ptr.size()){
				p = get_object(el->ID_ptr.c_str());
				if(!p){
					id = new iObjID;
					id -> init(el->ID_ptr.c_str(),el -> ID,el);
					ObjIDs -> connect((iListElement*)id);
				}
				else {
					if(p != (iListElement*)el)
						ErrH.Abort("Duplicate object ID...",XERR_USER,0,el->ID_ptr.c_str());
				}
			}
			el = (iScreenElement*)el -> prev;
		}
		if(!obj -> owner) obj -> owner = (iListElement*)this;
		obj = (iScreenObject*)obj -> prev;
	}
}

void iScreen::init(void)
{
	iScreenEvent* ev;
	iScreenObject* obj;
	iScreenEventCommand* ec;
	iListElement* p;

	init_ids();
	ev = (iScreenEvent*)EventList -> last;
	while(ev){
		ec = (iScreenEventCommand*)ev -> CommSeq -> last;
		while(ec){
			if(ec -> objType != I_ABSENT){
				p = get_object(ec -> tmp_objID);
				if(!p)
					p = iScrDisp -> get_object(ec -> tmp_objID);
				if(!p)
					ErrH.Abort("Can't find object...",XERR_USER,0,ec -> tmp_objID);
				ec -> obj = p;
			}
			ec = (iScreenEventCommand*)ec -> prev;
		}
		ev = (iScreenEvent*)ev -> prev;
	}
	obj = (iScreenObject*)objList -> last;
	while(obj){
		ev = (iScreenEvent*)obj -> EventList -> last;
		while(ev){
			ec = (iScreenEventCommand*)ev -> CommSeq -> last;
			while(ec){
				if(ec -> objType != I_ABSENT){
					p = get_object(ec -> tmp_objID);
					if(!p)
						p = iScrDisp -> get_object(ec -> tmp_objID);
					if(!p)
						ErrH.Abort("Can't find object...",XERR_USER,0,ec -> tmp_objID);
					ec -> obj = p;
				}
				ec = (iScreenEventCommand*)ec -> prev;
			}
			ev = (iScreenEvent*)ev -> prev;
		}
		obj = (iScreenObject*)obj -> prev;
	}
}

void iScreen::init_prompt(void)
{
	iScreenObject* obj;
	XGR_MousePromptData* pr;
	obj = (iScreenObject*)objList -> last;
	while(obj){
		if(obj -> promptData){
			pr = new XGR_MousePromptData(obj -> PosX,obj -> PosY,obj -> SizeX,obj -> SizeY,0);
			pr -> set_text(obj -> promptData);
			if(obj -> promptID)
				pr -> ID = obj -> promptID;
			promptData -> AddElement((XListElement*)pr);
		}
		obj = (iScreenObject*)obj -> prev;
	}
}

void iScreen::init_avi_border(void)
{
	iScreenElement* el;
	iAVIBorderElement* bd;
	iAVIElement* avi;
	iScreenObject* obj,*b_obj;
	obj = (iScreenObject*)objList -> last;
	while(obj){
		obj -> init();
		if(obj -> flags & OBJ_AVI_PRESENT && !(obj -> flags & AVI_BORDER_INIT)){
			obj -> flags |= AVI_BORDER_INIT;
			el = (iScreenElement*)obj -> ElementList -> last;
			while(el && el -> type != I_AVI_ELEM)
				el = (iScreenElement*)el -> prev;
			if(!el)
				ErrH.Abort("Bad AVI object",XERR_USER,0,obj->ID_ptr.c_str());
			avi = (iAVIElement*)el;
				b_obj = new iScreenObject;
				b_obj -> ID = 0;
				b_obj -> init_id("BORDER_OBJ");

				bd = new iAVIBorderElement;
				bd -> type = I_AVI_BORDER_ELEM;
				bd -> border_type = avi -> border_type;
				bd -> border_level = avi -> border_level;
				bd -> border_null_level = avi -> border_null_level;
				bd -> border_size = avi -> border_size;

				bd -> SizeX = (bd -> border_size * 2) + avi -> SizeX;
				bd -> SizeY = (bd -> border_size * 2) + avi -> SizeY;

				b_obj -> add_element(bd);

				b_obj -> PosX = obj -> PosX + avi -> lX - bd -> border_size - 1;
				b_obj -> PosY = obj -> PosY + avi -> lY - bd -> border_size - 1;

				b_obj -> flags |= OBJ_AVI_BORDER;
				b_obj -> init();

				objList -> connect((iListElement*)b_obj);
		}
		obj = (iScreenObject*)obj -> prev;
	}
}

void iScreenDispatcher::add_ext_screen(iScreen* p)
{
	ExtScreens -> connect((iListElement*)p);
}

void iScreenDispatcher::remove_ext_screen(iScreen* p)
{
	ExtScreens -> dconnect((iListElement*)p);
}

iScreen* iScreenDispatcher::get_ext_screen(const char* p)
{
	iScreen* s = (iScreen*)ExtScreens -> last;
	while(s){
		if(s->ID_ptr!=p)
			return s;
		s = (iScreen*)s -> prev;
	}
	return NULL;
}

void iScreenObject::load_data(void)
{
	iScreenElement* el = (iScreenElement*)ElementList -> last;
	while(el){
		el -> load_data();
		el = (iScreenElement*)el -> prev;
	}
//	  getfon();
}

void iScreenObject::free_data(void)
{
	iScreenElement* el = (iScreenElement*)ElementList -> last;
	while(el){
		el -> free_data();
		el = (iScreenElement*)el -> prev;
	}
	free_fon();
}

void iScreen::load_data(void)
{
	iScreenObject* obj = (iScreenObject*)TerrainList -> last;
	while(obj){
		obj -> load_data();
		obj -> getfon();
		obj -> redraw();
		obj = (iScreenObject*)obj -> prev;
	}
	obj = (iScreenObject*)objList -> last;
	while(obj){
		obj -> load_data();
		obj = (iScreenObject*)obj -> prev;
	}
	if(pal_name) load_palette();
}

void iScreen::getfon(void)
{
	iScreenObject* obj;
	obj = (iScreenObject*)objList -> last;
	while(obj){
		obj -> getfon();
		obj = (iScreenObject*)obj -> prev;
	}
}

void iScreen::free_data(void)
{
	iScreenObject* obj = (iScreenObject*)objList -> last;
	while(obj){
		obj -> free_data();
		obj = (iScreenObject*)obj -> prev;
	}
	obj = (iScreenObject*)TerrainList -> last;
	while(obj){
		obj -> free_data();
		obj = (iScreenObject*)obj -> prev;
	}
	if(pal_name) free_palette();
}

void iScreen::load_palette(void)
{
	XStream fh;
	if(!pal_name) return;
	if(!palette_buf) palette_buf = new char[768];

	fh.open(pal_name,XS_IN);
	fh.read(palette_buf,768);
	fh.close();
}

void iScreen::free_palette(void)
{
	if(palette_buf){
		delete[] palette_buf;
		palette_buf = NULL;
	}
}

void render_border(iScreenObject* p)
{
	int bx,by,bs;
	iAVIBorderElement* b = (iAVIBorderElement*)p -> ElementList -> last;

	bx = p -> PosX + b -> lX;
	by = p -> PosY + b -> lY;
	bs = b -> border_size;

	int x1,x2,y1,y2,dx,dy;

	x1 = p -> PosX;
	x2 = p -> PosX + p -> SizeX;
	y1 = p -> PosY;
	y2 = p -> PosY + bs;
	dx = x2 - x1;
	dy = y2 - y1;

	iregRender(x1,y1,x2,y2);

	x1 = p -> PosX;
	x2 = bx + bs;
	y1 = p -> PosY;
	y2 = p -> PosY + p -> SizeY;
	dx = x2 - x1;
	dy = y2 - y1;

	iregRender(x1,y1,x2,y2);

	x1 = p -> PosX;
	x2 = p -> PosX + p -> SizeX;
	y1 = p -> PosY + b -> SizeY - bs;
	y2 = p -> PosY + p -> SizeY;
	dx = x2 - x1;
	dy = y2 - y1;

	iregRender(x1,y1,x2,y2);

	x1 = p -> PosX + b -> SizeX - bs;
	x2 = p -> PosX + p -> SizeX;
	y1 = p -> PosY;
	y2 = p -> PosY + p -> SizeY;
	dx = x2 - x1;
	dy = y2 - y1;

	iregRender(x1,y1,x2,y2);
}

void iBitmapElement::set_pal(void)
{
	char* p;

	if(!(flags & EL_PICTURE) || !strlen(palname)) return;

	XStream fh(palname,XS_IN);
	_iALLOC_A_(char,p,768);
	fh.read(p,768);
	fh.close();

	XGR_SetPal(p + I_PAL_OFFS,I_PAL_START,I_PAL_SIZE);
	_iFREE_A_(char,p,768);
}

void iBitmapElement::init_name(const char* ptr)
{
	iListElement* p = new iListElement;
	if(!strcmp(ptr,"NONE")) flags |= EL_NONE_BMP;
	p -> init_id(ptr);
	bmp_names -> connect(p);

	strcpy(fname,bmp_names->first->ID_ptr.c_str());
}

void iBitmapElement::init_palname(char* ptr)
{
	iListElement* p = new iListElement;
	p -> init_id(ptr);
	pal_names -> connect(p);

	strcpy(palname,pal_names->first->ID_ptr.c_str());
}

void iAVIElement::init_name(char* ptr)
{
	strcpy(avi_name,ptr);
}

void iAVIElement::add_name(const char* ptr)
{
	iListElement* p = new iListElement;
	p -> init_id(ptr);
	avi_ids -> connect(p);

	strcpy(avi_name,avi_ids->first->ID_ptr.c_str());
}

void iAVIElement::next_avi(void)
{
	int i;
	iListElement* p = avi_ids -> first;
	avi_ids -> index ++;
	if(avi_ids -> index >= avi_ids -> Size)
		avi_ids -> index = 0;
	for(i = 0; i < avi_ids -> index; i ++){
		p = p -> next;
	}
	change_avi((char *)p->ID_ptr.c_str());
}

void iAVIElement::prev_avi(void)
{
	int i;
	iListElement* p = avi_ids -> first;
	avi_ids -> index --;
	if(avi_ids -> index < 0)
		avi_ids -> index = avi_ids -> Size - 1;
	for(i = 0; i < avi_ids -> index; i ++){
		p = p -> next;
	}
	change_avi((char *)p->ID_ptr.c_str());
}

void iStringElement::init_string(const char* ptr)
{
	strcpy(string,ptr);
}

void iS_StringElement::init_string(char* ptr)
{
	strcpy(string,ptr);
}

void iScreenDispatcher::init_input_string(iScreenElement* p)
{
	int sz;
	iListElementPtr* tmp;
	iScreenObject* obj;
	unsigned char* ptr = NULL;
	switch(p -> type){
		case I_STRING_ELEM:
			ptr = (unsigned char*)(((iStringElement*)p) -> string);
			break;
		case I_S_STRING_ELEM:
			ptr = (unsigned char*)(((iS_StringElement*)p) -> string);
			break;
		default:
			ErrH.Abort("Bad input string type...");
			break;
	}
	strcpy(BackupStr,(char*)ptr);
	sz = strlen((char*)ptr);
	if(p -> flags & EL_KEY_NAME){
		sz = 0;
	}
	ptr[sz] = '_';
	ptr[sz + 1] = 0;
	obj = (iScreenObject*)p -> owner;
	obj -> flags |= OBJ_REINIT;
	if(!(obj -> flags & OBJ_REDRAW)){
		obj -> flags |= OBJ_REDRAW;

		_iALLOC_(iListElementPtr,tmp);
		tmp -> ptr = (iListElement*)obj;
		drwObjHeap -> connect((iListElement*)tmp);
	}
	ActiveEl = p;
	flags |= SD_INPUT_STRING;
}

void iScreenDispatcher::input_string_quant(void)
{
	int k,sz,init_flag = 0,redraw_flag = 0,code,shift_flag = 0;
	iListElementPtr* tmp;
	iScreenObject* obj;
	unsigned char* ptr = NULL;
	HFont* hfnt = NULL;

	const char* key_name = NULL;

	switch(ActiveEl -> type){
		case I_STRING_ELEM:
			ptr = (unsigned char*)(((iStringElement*)ActiveEl) -> string);
			hfnt = HFntTable[(((iStringElement*)ActiveEl) -> font)];
			break;
		case I_S_STRING_ELEM:
			ptr = (unsigned char*)(((iS_StringElement*)ActiveEl) -> string);
			break;
		default:
			ErrH.Abort("Bad input string type...");
			break;
	}

	while(KeyBuf -> size) {
		SDL_Event *event = KeyBuf->get();
		//TODO: UTF8 fix
		if (event->type != SDL_KEYDOWN)
			continue;
		k = sdlEventToCode(event);

		if(!(k & 0x1000)) {
			if(!(ActiveEl -> flags & EL_KEY_NAME)){
				switch(k){
					case SDL_SCANCODE_RETURN:
						sz = strlen((char*)ptr);
						ptr[sz - 1] = 0;
						flags ^= SD_INPUT_STRING;
						flags |= SD_FINISH_INPUT;
						init_flag = 1;
						redraw_flag = 1;
						break;
					case SDL_SCANCODE_ESCAPE:
						strcpy((char*)ptr,BackupStr);
						flags ^= SD_INPUT_STRING;
						flags |= SD_FINISH_INPUT;
						init_flag = 1;
						redraw_flag = 1;
						break;
					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_BACKSPACE:
						sz = strlen((char*)ptr);
						if(sz > 1){
							ptr[sz - 1] = 0;
							ptr[sz - 2] = '_';
							init_flag = 1;
							redraw_flag = 1;
						}
						break;
					default:
						if(!(ActiveEl -> flags & EL_NUMBER)){
							if(KeyBuf -> flag & SHIFT_PRESSED) shift_flag = 1;
							code = SDL_GetKeyFromScancode((SDL_Scancode)k);
							if(code && hfnt && code < hfnt->NumChars){
								if((hfnt -> data[code] -> Flags & NULL_HCHAR) && code != ' ')
									break;
								sz = strlen((char*)ptr);
								if(sz < cur_max_input){
									ptr[sz - 1] = code;
									ptr[sz] = '_';
									ptr[sz + 1] = 0;

									init_flag = 1;
									redraw_flag = 1;
								}
							}
						}
						else {
							if(KeyBuf -> flag & SHIFT_PRESSED) shift_flag = 1;
							code = SDL_GetKeyFromScancode((SDL_Scancode)k);
							if(code && code >= '0' && code <= '9'){
								if(hfnt && (hfnt -> data[code] -> Flags & NULL_HCHAR) && code != ' ')
									break;
								sz = strlen((char*)ptr);
								if(sz < cur_max_input){
									ptr[sz - 1] = code;
									ptr[sz] = '_';
									ptr[sz + 1] = 0;

									init_flag = 1;
									redraw_flag = 1;
								}
							}
						}
						break;
				}
			}
			else {
				switch(k){
					case SDL_SCANCODE_ESCAPE:
						strcpy((char*)ptr,BackupStr);
						flags ^= SD_INPUT_STRING;
						flags |= SD_FINISH_INPUT;
						init_flag = 1;
						redraw_flag = 1;
						iScreenLastInput = k;
						break;
					default:
						//NEED rewrite
						/*if(!(k & iJOYSTICK_MASK))
							key_name = iGetKeyNameText(k,iRussian);
						else
							key_name = iGetJoyBtnNameText(k,iRussian);
						*/
						key_name = iGetKeyNameText(k,iRussian);
						if(flags & SD_INPUT_STRING && key_name){
							if(!(ActiveEl -> flags & EL_JOYSTICK_KEY) || (k & iJOYSTICK_MASK)){
								strcpy((char*)ptr,key_name);
								flags ^= SD_INPUT_STRING;
								flags |= SD_FINISH_INPUT;
								init_flag = 1;
								redraw_flag = 1;
								iScreenLastInput = k;
							}
						}
						break;
				}
			}
		}
	}
	if(init_flag){
		obj = (iScreenObject*)ActiveEl -> owner;
		obj -> flags |= OBJ_REINIT;
	}
	else {
		if(iFrameFlag){
			sz = strlen((char*)ptr);
			if(ptr[sz - 1] == ' ')
				ptr[sz - 1] = '_';
			else
				ptr[sz - 1] = ' ';

			ptr[sz] = 0;

			redraw_flag = 1;
		}
	}
	if(redraw_flag){
		obj = (iScreenObject*)ActiveEl -> owner;
		if(!(obj -> flags & OBJ_REDRAW)){
			obj -> flags |= OBJ_REDRAW;

			_iALLOC_(iListElementPtr,tmp);
			tmp -> ptr = (iListElement*)obj;
			drwObjHeap -> connect((iListElement*)tmp);
		}
	}
	iFrameFlag = 0;
}

void iScreenDispatcher::move_screen(int val,int time)
{
	int i;

	iScreenOffsDelta = val / time;
	iScreenTargetOffs = iScreenOffs + val;

	XGR_MouseHide();

	for(i = 0; i < time; i ++){
		iScreenOffs += iScreenOffsDelta;
		if(i == time - 1){
			iScreenOffs = iScreenTargetOffs;
			iScreenOffsDelta = 0;
		}
		put_map(iScreenOffs,0,I_RES_X,I_RES_Y);
		//XGR_Flush(0,0,I_RES_X,I_RES_Y);
		xtClearMessageQueue();
	}
	XGR_MouseShow();
	XGR_MouseObj.PromptDeltaX = iScreenOffs;
}

void iScreenDispatcher::aci_move_screen(int val,int time)
{
	int i;

	iScreenOffsDelta = val / time;
	iScreenTargetOffs = iScreenOffs + val;

	XGR_MouseHide();
	if(actIntLog){
		aScrDisp -> flags &= ~AS_FULL_REDRAW;
		aScrDisp -> flags &= ~AS_FULL_FLUSH;
		aScrDisp -> i_redraw();
		redraw_quant();
		aScrDisp -> flags &= ~AS_FULL_REDRAW;
		aScrDisp -> flags &= ~AS_FULL_FLUSH;
		aScrDisp -> i_redraw();
		aScrDisp -> i_flush();
		curScr -> show_avi();
	}
	for(i = 0; i < time; i ++){
		iScreenOffs += iScreenOffsDelta;
		put_map(iScreenOffs,0,I_RES_X,I_RES_Y);
		if(i == time - 1){
			iScreenOffs = iScreenTargetOffs;
			iScreenOffsDelta = 0;
		}
		XGR_Flush(0,0,I_RES_X,I_RES_Y);
	}
	XGR_MouseShow();
	XGR_MouseObj.PromptDeltaX = iScreenOffs;
}

void iScreenDispatcher::hide_screen(iScreen* p)
{
	iScreenObject* obj = (iScreenObject*)p -> objList -> last;
	while(obj){
		obj -> curHeightScale = 0;
		obj -> flags |= OBJ_HIDE;
		obj = (iScreenObject*)obj -> prev;
	}
	obj = (iScreenObject*)p -> TerrainList -> last;
	while(obj){
		((iTerrainElement*)obj -> ElementList -> last) -> null_level = 0;
		obj -> curHeightScale = 0;
		obj -> flags |= OBJ_HIDE;
		obj = (iScreenObject*)obj -> prev;
	}
	set_scr_redraw(p);
}

void iScreenDispatcher::show_screen(iScreen* p)
{
	p -> prepare();
	set_scr_redraw(p);
}

void iScreenDispatcher::set_scr_redraw(iScreen* p)
{
	iScreenObject* obj;

	obj = (iScreenObject*)p -> objList -> last;
	while(obj){
		set_obj_redraw(obj);
		obj = (iScreenObject*)obj -> prev;
	}
	obj = (iScreenObject*)p -> TerrainList -> last;
	while(obj){
		set_obj_redraw(obj);
		obj = (iScreenObject*)obj -> prev;
	}
}

void iScreenDispatcher::set_obj_redraw(iScreenObject* p)
{
	iListElementPtr* tmp;
	if(!(p -> flags & OBJ_REDRAW)){
		p -> flags |= OBJ_REDRAW;

		_iALLOC_(iListElementPtr,tmp);
		tmp -> ptr = (iListElement*)p;
		drwObjHeap -> connect((iListElement*)tmp);
	}
}

void iScreenDispatcher::move2screen(iScreen* p,int time)
{
	iScreen* cp = curScr;
	int delta = p -> ScreenOffs - iScreenOffs;

	curScr = p;
	if(p != cp)
		p -> load_data();

	if(actIntLog){
		aci_setMatrixBorder();
		aci_setScMatrixBorder();
	}

	if(p -> pal_name)
		XGR_SetPal(p -> palette_buf,0,255);

	if(!delta){
		if(p != cp){
			hide_screen(cp);
			redraw_quant();

			flags |= (SD_GETFON | SD_END_GETFON);

			show_screen(p);
			redraw_quant();
			flush_quant();
		}
		else {
			show_screen(p);
			redraw_quant();
			flush_quant();
		}
	}
	else {
//		if(p != cp)
//			p -> load_data();

		show_screen(p);
		flags |= (SD_GETFON | SD_END_GETFON);
		redraw_quant();
		p -> show_avi();
		p -> hide_avi();

		if(actIntLog)
			aci_move_screen(delta,time);
		else
			move_screen(delta,time);
		drwObjHeap -> init_list();
		hide_screen(cp);
		redraw_quant();
		cp -> hide_avi_place();
	}
	drwObjHeap -> init_list();
	if(p != cp)
		cp -> free_data();

}

void iScreenDispatcher::redraw_quant(void)
{
	iListElementPtr* tmp;
	iScreenObject* obj;
	tmp = (iListElementPtr*)drwObjHeap -> last;
	while(tmp){
		obj = (iScreenObject*)tmp -> ptr;
		obj -> putfon();
		tmp = (iListElementPtr*)tmp -> prev;
	}
	tmp = (iListElementPtr*)drwObjHeap -> last;
	if(flags & SD_GETFON){
		while(tmp){
			obj = (iScreenObject*)tmp -> ptr;
			if(obj -> flags & OBJ_TERRAIN)
				obj -> redraw();
			obj -> flags &= ~OBJ_REDRAW;
			tmp = (iListElementPtr*)tmp -> prev;
		}
		tmp = (iListElementPtr*)drwObjHeap -> last;
		while(tmp){
			obj = (iScreenObject*)tmp -> ptr;
			if(!(obj -> flags & OBJ_TERRAIN))
				obj -> getfon();
			tmp = (iListElementPtr*)tmp -> prev;
		}
		tmp = (iListElementPtr*)drwObjHeap -> last;
		while(tmp){
			obj = (iScreenObject*)tmp -> ptr;
			if(!(obj -> flags & OBJ_TERRAIN))
				obj -> redraw();
			obj -> flags &= ~OBJ_REDRAW;
			tmp = (iListElementPtr*)tmp -> prev;
		}
	}
	else {
		while(tmp){
			obj = (iScreenObject*)tmp -> ptr;
			obj -> redraw();
			obj -> flags &= ~OBJ_REDRAW;
			tmp = (iListElementPtr*)tmp -> prev;
		}
	}

	if(flags & SD_END_GETFON){
		flags &= ~(SD_GETFON | SD_END_GETFON);
	}
}

void iScreenDispatcher::flush_quant(void)
{
	iListElementPtr* tmp;
	iScreenObject* obj;
	tmp = (iListElementPtr*)drwObjHeap -> last;
	while(tmp){
		obj = (iScreenObject*)tmp -> ptr;
		obj -> flush();
		obj -> flags &= ~OBJ_REDRAW;
		tmp = (iListElementPtr*)tmp -> prev;
	}
}

void iScreenDispatcher::pal_quant(void)
{
	int i,flag = 0;
	unsigned val,t_val;
	for(i = I_PAL_OFFS; i < 768; i ++){
		val = curPal[i];
		t_val = trgPal[i];
		if(val != t_val){
			flag = 1;
			val += SD_PAL_DELTA;
			if(val > t_val) val = t_val;
			curPal[i] = val;
		}
	}
	XGR_SetPal(curPal + I_PAL_OFFS,I_PAL_START,I_PAL_SIZE);
	if(!flag) flags &= ~SD_PAL_CHANGE;
}

void i_pal_quant(unsigned char* pal_buf,int lev,int n_lev)
{
	//int i,delta;
	int i;
	unsigned char* p;

	unsigned val,p_val;
	//delta = (lev << 6) / n_lev;
	_iALLOC_A_(unsigned char,p,768);
	memset(p,0,768);

	for(i = I_PAL_OFFS; i < 768; i ++){
		p_val = pal_buf[i];
		val = (lev * p_val) / n_lev;
		p[i] = val;
		//std::cout<<val<<" ";
	}
	XGR_SetPal(p + I_PAL_OFFS,I_PAL_START,I_PAL_SIZE);

}

int iAVIElement::check_visible(void)
{
	iScreen* scr;
	iScreenObject* obj;
	obj = (iScreenObject*)owner;
	scr = (iScreen*)obj -> owner;
	if(iScreenOffs != scr -> ScreenOffs) return 0;
	return 1;
}

void iScreen::show_avi(void)
{
	int sz;
	iAVIElement* avi;
	iScreenElement* el;
	iScreenObject* obj = (iScreenObject*)objList -> last;
	unsigned char* p;
	while(obj){
		if(obj -> flags & OBJ_AVI_PRESENT){
			el = (iScreenElement*)obj -> ElementList -> last;
			while(el){
				if(el -> type == I_AVI_ELEM){
					avi = (iAVIElement*)el;
					sz = avi -> SizeX * avi -> SizeY;
					_iALLOC_A_(unsigned char,p,sz);
					memset(p,0,sz);
					put_buf2col(obj -> PosX + avi -> lX,obj -> PosY + avi -> lY,avi -> SizeX,avi -> SizeY,p,iAVI_NULL_LEV,1);
					_iFREE_A_(unsigned char,p,sz);
				}
				el = (iScreenElement*)el -> prev;
			}
		}
		obj = (iScreenObject*)obj -> prev;
	}
}

void iScreen::hide_avi(void)
{
	iScreenObject* obj = (iScreenObject*)objList -> last;
	while(obj){
		if(obj -> flags & OBJ_AVI_PRESENT){
			obj -> curHeightScale = 0;
		}
		obj = (iScreenObject*)obj -> prev;
	}
}

void iScreen::hide_avi_place(void)
{
	iScreenObject* obj = (iScreenObject*)objList -> last;
	while(obj){
		if(obj -> flags & OBJ_AVI_PRESENT){
			obj -> curHeightScale = 0;
			iregRender(obj -> PosX,obj -> PosY,obj -> PosX + obj -> SizeX,obj -> PosY + obj -> SizeY);
		}
		obj = (iScreenObject*)obj -> prev;
	}
}

void iTriggerObject::init_state(void)
{
	if (StateElem==NULL) {
		return;
	}
	int i;
	iScreenElement* el;
	for(i = 0; i < num_state; i ++){
		el = (iScreenElement*)StateElem[i];
		el -> flags |= EL_NO_REDRAW;
		el -> scale = 0;
		if(el -> type == I_AVI_ELEM) flags |= OBJ_AVI_STOPPED;
	}

	el = (iScreenElement*)StateElem[state];
	el -> flags ^= EL_NO_REDRAW;
	el -> scale = 256;
	if(el -> type == I_AVI_ELEM) flags &= ~OBJ_AVI_STOPPED;
}

void iTriggerObject::trigger_init(void)
{
	init_state();
}

void iScreenDispatcher::lock_events(void)
{
	if(ActiveEv) ActiveEv = NULL;
	flags |= SD_LOCK_EVENTS;
}

void iScreenDispatcher::unlock_events(void)
{
	flags &= ~SD_LOCK_EVENTS;
}

void iScreenDispatcher::key_trap(int sc)
{
	if(!actIntLog)
		CheckScanCode(sc);
	curScr -> CheckScanCode(sc);
}

void iStringElement::init_size(void)
{
	SizeX = iStrLen((unsigned char*)string,font,space);
	SizeY = HFntTable[font] -> SizeY;
}

void iS_StringElement::init_size(void)
{
//	  SizeX = iS_StrLen((unsigned char*)string,font,space);
//	  SizeY = iScrFontTable[font] -> SizeY;
	SizeX = aStrLen32(string,font,space);
	SizeY = aStrHeight32(font);
}

iTextData::iTextData(void)
{
	ID = 0;
	flags = 0;

	numLines = curLine = 0;

	fname = NULL;
	data = NULL;
	mem_heap = NULL;
	objName = NULL;
}

iTextData::~iTextData(void)
{
	if(fname) delete[] fname;
	if(mem_heap) delete mem_heap;
	if(data) delete data;
	if(objName) delete objName;
}

void iTextData::load(void)
{
	XStream fh;
	int i,l = 0,num_len = 0;

	fh.open(fname,XS_IN);
	mem_heap = aciLoadPackedFile(fh,heap_size);

//	  heap_size = fh.size();
//	  mem_heap = new char[heap_size];
//	  fh.read(mem_heap,heap_size);

	fh.close();

	for(i = 0; i < heap_size; i ++){
		if(mem_heap[i] == '\n') num_len ++;
	}

	data = new char*[num_len];
	for(i = 0; i < num_len; i ++)
		data[i] = NULL;

	data[0] = mem_heap;

	numLines = num_len;

	for(i = 0; i < heap_size; i ++){
		if(mem_heap[i] == '\n'){
			l ++;
			mem_heap[i] = 0;
			if(l <= num_len - 1)
				data[l] = mem_heap + i + 1;
		}
	}

	for(i = 0; i < heap_size; i ++){
		if(mem_heap[i] == '\r') mem_heap[i] = 0;
	}
	reset();

	flags |= iTEXT_DATA_LOADED;
}

void iTextData::free(void)
{
	numLines = curLine = 0;

	if(mem_heap) {
		delete[] mem_heap;
	}
	if(data) {
		delete[] data;
	}

	data = NULL;
	mem_heap = NULL;

	flags &= ~iTEXT_DATA_LOADED;
}

void iTextData::copy(iScreenObject* p)
{
	int i;
	iScreenElement* el;

	char* ptr;

	el = (iScreenElement*)p -> ElementList -> first;
	for(i = 0; i < p -> ElementList -> Size; i ++){
		ptr = NULL;
		switch(el -> type){
			case I_STRING_ELEM:
				ptr = ((iStringElement*)el) -> string;
				break;
			case I_S_STRING_ELEM:
				ptr = ((iS_StringElement*)el) -> string;
				break;
		}
		if(ptr){
			if(!(flags & iTEXT_EOF))
				read_str(ptr);
			else
				strcpy(ptr," ");

			switch(el -> type){
				case I_STRING_ELEM:
					((iStringElement*)el) -> init_size();
					break;
				case I_S_STRING_ELEM:
					((iS_StringElement*)el) -> init_size();
					break;
			}
			el -> init_align();
		}
		el = (iScreenElement*)el -> next;
	}
}

void iTextData::read_str(char* p)
{
	if(flags & iTEXT_EOF) return;
	strcpy(p,data[curLine]);
	curLine ++;
	if(curLine >= numLines)
		flags |= iTEXT_EOF;
}

void iTextData::reset(void)
{
	curLine = 0;
	flags &= ~iTEXT_EOF;
}

void iScreenDispatcher::init_texts(void)
{
	iTextData* p = (iTextData*)texts -> last;
	while(p){
		p -> load();
		p = (iTextData*)p -> prev;
	}
	curText = NULL;
}

void iScreenDispatcher::finit_texts(void)
{
	iTextData* p = (iTextData*)texts -> last;
	while(p){
		p -> free();
		p = (iTextData*)p -> prev;
	}
}

void iScreenDispatcher::next_text(void)
{
	int id = 0;

	if(curText) id = curText -> ID + 1;
	if(!iRussian){
		if(!id || id > iTEXT_ENG_MAX)
			id = iTEXT_ENG1_ID;
	}
	else {
		if(!id || id > iTEXT_RUS_MAX)
			id = iTEXT_RUS1_ID;
	}
	curText = get_text(id);
	if(!curText) return;
	curText -> reset();
}

void iScreenDispatcher::prev_text(void)
{
	int id = 0;

	if(curText) id = curText -> ID - 1;
	if(!iRussian){
		if(id <= 0)
			return;
	}
	else {
		if(id <= iTEXT_RUS1_ID - 1)
			return;
	}
	curText = get_text(id);
	if(!curText) return;
	curText -> reset();
}

iTextData* iScreenDispatcher::get_text(int id)
{
	iTextData* p = (iTextData*)texts -> last;
	while(p){
		if(p -> ID == id) return p;
		p = (iTextData*)p -> prev;
	}
	return NULL;
}

int iScreenDispatcher::copy_text_next(iScreen* scr,int mode)
{
	int id,end_code,next_code;
	iScreenObject* obj = (iScreenObject*)scr -> get_object("Text00");
	iStringElement* title_obj = (iStringElement*)scr -> get_object("TextTitle00");
	if(obj){
		if(!curText){
			next_text();
		}
		else {
			if(curText -> eof()){
				id = curText -> ID + 1;
				ActiveEv -> flags &= ~EV_ACTIVE;
				ActiveEv = NULL;

				end_code = (mode) ? iTEXT_END_EVENT_CODE1 : iTEXT_END_EVENT_CODE0;
				next_code = (mode) ? iTEXT_NEXT_EVENT_CODE1 : iTEXT_NEXT_EVENT_CODE0;
				if(!iRussian){
					if(id > iTEXT_ENG_MAX){
						key_trap(end_code);
					}
					else {
						next_text();
						if(title_obj){
							strcpy(title_obj -> string,curText -> objName);
							title_obj -> init_size();
							title_obj -> init_align();
						}
						key_trap(next_code);
					}
				}
				else {
					if(id > iTEXT_RUS_MAX){
						key_trap(end_code);
					}
					else {
						next_text();
						if(title_obj){
							strcpy(title_obj -> string,curText -> objName);
							title_obj -> init_size();
							title_obj -> init_align();
						}
						key_trap(next_code);
					}
				}
				return 1;
			}
		}

		if(!curText) return 0;

		if(title_obj){
			strcpy(title_obj -> string,curText -> objName);
			title_obj -> init_size();
			title_obj -> init_align();
		}
		curText -> copy(obj);
	}
	return 0;
}

int iScreenDispatcher::copy_text_prev(iScreen* scr,int mode)
{
	int id,prev_code;
	iScreenObject* obj = (iScreenObject*)scr -> get_object("Text00");
	iStringElement* title_obj = (iStringElement*)scr -> get_object("TextTitle00");
	if(obj){
		if(!curText){
			next_text();
		}
		else {
			if(curText -> curLine <= obj -> ElementList -> Size){
				id = curText -> ID - 1;
				ActiveEv -> flags &= ~EV_ACTIVE;
				ActiveEv = NULL;

				prev_code = (mode) ? iTEXT_PREV_EVENT_CODE1 : iTEXT_PREV_EVENT_CODE0;
				if((!iRussian && id > 0) || id > iTEXT_RUS1_ID - 1){
					prev_text();
					if(title_obj){
						strcpy(title_obj -> string,curText -> objName);
						title_obj -> init_size();
						title_obj -> init_align();
					}
					curText -> curLine = curText -> numLines - (curText -> numLines % obj -> ElementList -> Size) + obj -> ElementList -> Size * 2;
					key_trap(prev_code);
				}
				return 1;
			}
			else {
				if(curText -> curLine >= obj -> ElementList -> Size * 2){
					if(curText -> curLine != curText -> numLines)
						curText -> curLine -= obj -> ElementList -> Size * 2;
					else
						curText -> curLine -= obj -> ElementList -> Size + (curText -> numLines % obj -> ElementList -> Size);
				}
				else {
					ActiveEv -> flags &= ~EV_ACTIVE;
					ActiveEv = NULL;
					return 1;
				}
			}
		}

		if(!curText) return 0;

		if(title_obj){
			strcpy(title_obj -> string,curText -> objName);
			title_obj -> init_size();
			title_obj -> init_align();
		}
		curText -> flags &= ~iTEXT_EOF;
		curText -> copy(obj);
	}
	return 0;
}

void iScreenDispatcher::save_data(XStream* fh)
{
	std::cout<<"iScreenDispatcher::save_data"<<std::endl;
	int i,num_opt = iMAX_OPTION_ID;
	*fh < num_opt;
	for(i = 0; i < iMAX_OPTION_ID; i ++){
		if(iScrOpt[i])
			iScrOpt[i] -> save(fh);
	}
}

void iScreenDispatcher::load_data(XStream* fh)
{
	int i,num_opt;
	*fh > num_opt;
	if(num_opt != iMAX_OPTION_ID) return;
	for(i = 0; i < iMAX_OPTION_ID; i ++){
		if(iScrOpt[i])
			iScrOpt[i] -> load(fh);
	}
}

void iScreenDispatcher::end_event(void)
{
	if(ActiveEv){
		ActiveEv -> flags &= ~EV_ACTIVE;
		ActiveEv = NULL;
	}
}

