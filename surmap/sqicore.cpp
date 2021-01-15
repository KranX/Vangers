/*
		  Simple-Quick Interface by K-Division::KranK
			  All Rights Reserved (C)1995
				     (Core)
*/

#include "../src/global.h"

#include "sqint.h"

extern int TrackBuild;

/* ---------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION --------------------------- */
void LMousePress(int, int, int);
void LMouseUnpress(int, int, int);
void RMousePress(int, int, int);
void RMouseUnpress(int, int, int);
void LMouseMov(int, int, int);
void RMouseMov(int, int, int);
void LMouseDouble(int, int, int);
void RMouseDouble(int, int, int);
/* --------------------------- DEFINITION SECTION -------------------------- */
sqEventQueue* sqE;
sqEventQueue* sqKey;
sqScreen* sqScr;
sqElem* QuantObj;
sqElem* KeyTrapObj;
int SQ_SYSCOLOR;

sqScreen::sqScreen(int _x,int _y,int _sx,int _sy)
: sqElem(0,0,0,0,0)
{
	x = _x; y = _y; sx = _sx; sy = _sy;
	set(0);
//	XGR_MouseObj.SetClip(x + XGR_MouseObj.SizeX/2, y + XGR_MouseObj.SizeY/2,x + sx - XGR_MouseObj.SizeX/2, y + sy - XGR_MouseObj.SizeY/2);
}

static unsigned char XMouseMask[64] = {
		255,255,255,255,255,255,255,255,
		255,0,0,255,255,0,0,255,
		255,0,0,0,0,0,0,255,
		255,255,0,0,0,0,255,255,
		255,255,0,0,0,0,255,255,
		255,0,0,0,0,0,0,255,
		255,0,0,255,255,0,0,255,
		255,255,255,255,255,255,255,255
		};

void sqInitMouse(void)
{
	XGR_MouseInit(XGR_MAXX/2,XGR_MAXY/2,8,8,1,XMouseMask);
	XGR_MouseSetSpot(4,4);

	XGR_MouseSetPressHandler(XGM_LEFT_BUTTON,LMousePress);
	XGR_MouseSetUnPressHandler(XGM_LEFT_BUTTON,LMouseUnpress);
	XGR_MouseSetPressHandler(XGM_RIGHT_BUTTON,RMousePress);
	XGR_MouseSetUnPressHandler(XGM_RIGHT_BUTTON,RMouseUnpress);
	XGR_MouseSetDblHandler(XGM_LEFT_BUTTON,LMouseDouble);
	XGR_MouseSetDblHandler(XGM_RIGHT_BUTTON,RMouseDouble);

	XGR_MouseSetPos(XGR_MAXX/2,XGR_MAXY/2);
}

void sqRestoreCursor(void)
{
//	  XGR_MouseObj.SetCursor(XMouseMask,1,8,8);
}

void LMousePress(int, int x, int y)
{
	sqE -> put(E_LBMPRESS,E_REGION,x,y);
}

void LMouseUnpress(int, int x, int y)
{
	sqE -> put(E_LBMUNPRESS,E_REGION,x,y);
}

void RMousePress(int, int x, int y)
{
	sqE -> put(E_RBMPRESS,E_REGION,x,y);
}

void RMouseUnpress(int, int x, int y)
{
	sqE -> put(E_RBMUNPRESS,E_REGION,x,y);
}

void LMouseDouble(int, int x, int y)
{
	sqE -> put(E_LBMDOUBLE,E_REGION,x,y);
}

void RMouseDouble(int, int x, int y)
{
	sqE -> put(E_RBMDOUBLE,E_REGION,x,y);
}

void sqSetPalette(char* pal)
{
	for(int i = 0;i < 8;i++){
		pal[3*(SQ_SYSCOLOR + i) + 0] = 1 + i*8;
		pal[3*(SQ_SYSCOLOR + i) + 1] = 3 + i*8;
		pal[3*(SQ_SYSCOLOR + i) + 2] = 7 + i*8;
		}
}

void sqFont::init(void* d)
{
	char* p = (char*)d;
	memcpy(this,p,8);
	data = new void*[num];
	for(int i = 0;i < num;i++)
		data[i] = p + 8 + sy*i;
}

void sqFont::draw(int x,int y,unsigned char* s,int fore,int back)
{
	while(*s){
		drawchar(x,y,*s,fore,back);
		s++;
		x += sx;
		}
}

void sqFont::drawtext(int x,int y,char* s,int fore,int back)
{
	char c;
	int i = x;
	while((c = *s) != 0){
		switch(c){
			default:
				drawchar(i,y,*s,fore,back);
				i += sx;
				break;
			case '\n':
				y += sy;
				i = x;
				break;
			case '\t':
				i += 4*sx;
				break;
				}
		s++;
		}
}


void sqFont::drawchar(int x,int y,int ch,int fore,int back)
{
	if(x < 0 || y < 0 || x + sx >= XGR_MAXX || y + sy >= XGR_MAXY)
		return;
	unsigned char* p = (unsigned char*)data[ch];
	int i,j,m;
	for(j = 0;j < sy;j++)
		for(i = 0,m = 1 << sx;i < sx;i++,m >>= 1){
			if(p[j] & m) XGR_SetPixel(x + i,y + j,fore);
			else
				if(back != -1) XGR_SetPixel(x + i,y + j,back);
			}
}

void sqInit(int col0)
{
	sqInitMouse();
	SQ_SYSCOLOR = col0;

	sqE = new sqEventQueue(256);
	sqKey = new sqEventQueue(128);
	sqScr = new sqScreen(0,0,XGR_MAXX,XGR_MAXY);
}

#ifdef _SURMAP_
void sqQuant(void)
{
	int isFlush = 0;
	sqEvent* event = sqKey -> get();
	if(event){
//		  do {
			KeyTrapObj -> keytrap(event -> code);
			event = sqKey -> get();
//			  } while(event);
		isFlush = 1;
		}
	event = sqE -> get();
	if(event){
		do {
			sqScr -> process(event);
			event = sqE -> get();
			} while(event);
		isFlush = 1;
		}

	if(TrackBuild) isFlush = 1;

	if(isFlush && KeyTrapObj) KeyTrapObj -> draw();

	if(QuantObj) QuantObj -> quant();

	if(isFlush && KeyTrapObj){
		XGR_MouseHide();
		KeyTrapObj -> flush();
		XGR_MouseShow();
		}

}
#endif

#ifdef _ROAD_
void sqQuant(void)
{
	int isFlush = 0;
	sqEvent* event = sqKey -> get();
	if(event){
		if(isMouse) XGR_MouseObj.hide();
		do {
			KeyTrapObj -> keytrap(event -> code);
			event = sqKey -> get();
			} while(event);
		if(isMouse) XGR_MouseObj.show();
		isFlush = 1;
		}
	event = sqE -> get();
	if(event){
		if(isMouse) XGR_MouseObj.hide();
		do {
			sqScr -> process(event);
			event = sqE -> get();
			} while(event);
		if(isMouse) XGR_MouseObj.show();
		isFlush = 1;
		}
//	  sqScr -> quant();

//	  if(isFlush && KeyTrapObj) KeyTrapObj -> draw();

	if(QuantObj){
		isFlush = 1;
		QuantObj -> quant();
		}

//	if(isFlush && KeyTrapObj) KeyTrapObj -> flush();
}
#endif

#if !defined(_ROAD_) && !defined(_SURMAP_)
void sqScreen::keytrap(int key){}
void sqScreen::handler(sqEvent* e){}
void sqScreen::draw(int mode){}
#endif
