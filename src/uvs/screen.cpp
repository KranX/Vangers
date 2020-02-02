#include "../global.h"

#include "../common.h"

#include "../units/uvsapi.h"
#include "univang.h"
#include "screen.h"

#undef COL1
#undef COL2
#undef COL3

#ifdef _ROAD_

#define TINY_MODE

#define COL1	111
#define COL2	175
#define COL3	235

#else

#define COL1	48
#define COL2	32
#define COL3	25

#endif

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern sqFont sysfont;
extern WorldScreen* scrTable[MAIN_WORLD_MAX];
extern uvsElement* ETail;
extern int c_All_Number;
extern int c_S_Number;
extern int c_R_Number;
extern int c_A_Number;
/* --------------------------- PROTOTYPE SECTION --------------------------- */
/* --------------------------- DEFINITION SECTION -------------------------- */
#ifdef TINY_MODE
const int ESCAVE_SIDE = 4;
const int SPOT_SIDE = 2;
const int PASSAGE_SIDE = 4;
#else
const int ESCAVE_SIDE = 9;
const int SPOT_SIDE = 5;
const int PASSAGE_SIDE = 9;
#endif

const int ESCAVE_SIZE = 2*ESCAVE_SIDE + 1;
const int SPOT_SIZE = 2*SPOT_SIDE + 1;
const int PASSAGE_SIZE = 2*PASSAGE_SIDE + 1;

const int ICON_SX = 7;
const int ICON_SY = 7;
static uchar ICON_TABU[ICON_SX*ICON_SY] = {
										SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,
										0,0,0,SYSCOLOR,0,0,0,
										0,0,0,SYSCOLOR,0,0,0,
										0,0,0,SYSCOLOR,0,0,0,
										0,0,0,SYSCOLOR,0,0,0,
										0,0,0,SYSCOLOR,0,0,0,
										0,0,0,SYSCOLOR,0,0,0
										};
static uchar ICON_SLAVE[ICON_SX*ICON_SY] = {
										0,0,0,0,0,0,0,
										0,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,
										0,SYSCOLOR,0,0,0,0,0,
										0,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,0,0,0,0,SYSCOLOR,0,
										0,SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,0,0,0,0,0,0
										};
static uchar ICON_RANGER[ICON_SX*ICON_SY] = {
										0,0,0,0,0,0,0,
										0,SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,SYSCOLOR,0,0,0,SYSCOLOR,0,
										0,SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,0,
										0,SYSCOLOR,0,0,SYSCOLOR,SYSCOLOR,0,
										0,0,0,0,0,0,0
										};
static uchar ICON_GAMER[ICON_SX*ICON_SY] = {
										0,0,0,0,0,0,0,
										0,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,SYSCOLOR,0,0,0,0,0,
										0,SYSCOLOR,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,
										0,SYSCOLOR,0,0,0,SYSCOLOR,0,
										0,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,0,0,0,0,0,0
										};
static uchar ICON_THIEF[ICON_SX*ICON_SY] = {
										0,0,0,0,0,0,0,
										0,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,SYSCOLOR,0,0,0,0,0,
										0,SYSCOLOR,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,SYSCOLOR,0,0,0,0,0,
										0,SYSCOLOR,0,0,0,0,0,
										0,0,0,0,0,0,0
										};
static uchar ICON_CARAVAN[ICON_SX*ICON_SY] = {
										0,0,0,0,0,0,0,
										0,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,SYSCOLOR,0,0,0,0,0,
										0,SYSCOLOR,0,0,0,0,0,
										0,SYSCOLOR,0,0,0,0,0,
										0,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0,
										0,0,0,0,0,0,0
										};
static uchar ICON_KILLER[ICON_SX*ICON_SY] = {
										SYSCOLOR,0,0,0,SYSCOLOR,0,0,
										SYSCOLOR,0,0,SYSCOLOR,0,0,0,
										SYSCOLOR,0,SYSCOLOR,0,0,0,0,
										SYSCOLOR,SYSCOLOR,0,0,0,0,0,
										SYSCOLOR,0,SYSCOLOR,0,0,0,0,
										SYSCOLOR,0,0,SYSCOLOR,0,0,0,
										SYSCOLOR,0,0,0,SYSCOLOR,0,0
										};
static uchar ICON_GSLAVE[ICON_SX*ICON_SY] = {
										SYSCOLOR,0,0,0,0,0,SYSCOLOR,
										0,SYSCOLOR,0,0,0,SYSCOLOR,0,
										0,0,SYSCOLOR,0,SYSCOLOR,0,0,
										0,0,0,SYSCOLOR,0,0,0,
										0,0,SYSCOLOR,0,SYSCOLOR,0,0,
										0,SYSCOLOR,0,0,0,SYSCOLOR,0,
										SYSCOLOR,0,0,0,0,0,SYSCOLOR
										};

static uchar ICON_FLY_FARMER[ICON_SX*ICON_SY] = {
										0,0,0,SYSCOLOR,0,0,0,
										0,0,0,SYSCOLOR,0,0,0,
										0,0,0,SYSCOLOR,0,0,0,
										0,0,SYSCOLOR,0,SYSCOLOR,0,0,
										0,SYSCOLOR,0,0,0,SYSCOLOR,0,
										0,SYSCOLOR,0,0,0,SYSCOLOR,0,
										0,0,SYSCOLOR,SYSCOLOR,SYSCOLOR,0,0
										};

WorldScreen* scrTable[MAIN_WORLD_MAX];

void StandScreenPrepare(void)
{
#ifdef TINY_MODE
	const int wsy = 64;
#else
	const int wsy = 145;
#endif
	for(int y = 80 + 16 + 2,i = 0;i < MAIN_WORLD_MAX;i++, y += wsy + 3) scrTable[i] = new WorldScreen(y,wsy,getWorld(i));
}

WorldScreen::WorldScreen(int _y,int _sy,uvsWorld* _Pworld)
{
	Pworld = _Pworld;
#ifdef TINY_MODE
	x = 100;
	sx = 320;
#else
	x = 0;
	sx = XGR_MAXX;
#endif
	y = _y; sy = _sy - (16 + 2);
	fx = 1; fy = y + sy + 2;
	x_size = Pworld -> x_size;
	y_size = Pworld -> y_size;
	placeTail = NULL;
}

void WorldScreen::Quant(void)
{
std::cout<<"WorldScreen::Quant"<<std::endl;
#ifdef TINY_MODE
	XGR_Rectangle(x,y,sx,sy,0,0,XGR_FILLED);
#else
	XGR_Rectangle(x,y,sx,sy,0,SYSCOLOR,XGR_OUTLINED);
#endif

	info.init();
	info < "                  " <  Pworld -> name;
	if (Pworld -> escTmax){
		info < " [Stage " <= Pworld -> escT[0] -> GetCurrentStage() < ": " < Pworld -> escT[0] -> GetCurrentStageName() < "]";
#ifndef TINY_MODE
	info < " cirtQ: "<= Pworld -> escT[0] -> Pbunch -> GetCirtQ();
	info < " vRes: "<= Pworld -> escT[0] -> Pbunch -> vngResource;
#endif
	}
	sysfont.draw(fx,fy,info.GetBuf());

	int i;
	int xx,yy,len;
	uvsEscave* pe;
	char* s;

	/*Place* p = placeTail;
	while(p){
		xx = x + GetX(p -> y);
		yy = y + GetY(p -> x);
		XGR_Rectangle(xx - 2,yy - 2,5,5,COL3,COL3,XGR_FILLED);
		p = (Place*)p -> next;
		}*/

	for(i = 0;i < Pworld -> escTmax;i++){
		pe = Pworld -> escT[i];
		xx = x + GetX(pe -> pos_y);
		yy = y + GetY(pe -> pos_x);
		XGR_Rectangle(xx - ESCAVE_SIDE,yy - ESCAVE_SIDE,ESCAVE_SIZE,ESCAVE_SIZE,COL1,COL1,XGR_FILLED);
		s = pe -> name;
		yy += ESCAVE_SIDE + 1;
		len = strlen(s)*8;
		xx -= len/2;
		if(xx < x + 2) xx = x + 2;
		else if(xx + len >= x + sx - 3) xx -= xx + len - (x + sx - 3);
#ifndef TINY_MODE
		sysfont.draw(xx,yy,s,COL1,-1);
#endif
		}

	uvsSpot* ps;
	for(i = 0;i < Pworld -> sptTmax;i++){
		ps = Pworld -> sptT[i];
		xx = x + GetX(ps -> pos_y);
		yy = y + GetY(ps -> pos_x);
		XGR_Rectangle(xx - SPOT_SIDE,yy - SPOT_SIDE,SPOT_SIZE,SPOT_SIZE,COL2,COL2,XGR_FILLED);
		s = ps -> name;
		yy += SPOT_SIDE + 1;
		len = strlen(s)*8;
		xx -= len/2;
		if(xx < x + 2) xx = x + 2;
		else if(xx + len >= x + sx - 3) xx -= xx + len - (x + sx - 3);
#ifndef TINY_MODE
		sysfont.draw(xx,yy,s,COL2,-1);
#endif
		}

	uvsPassage* pp;
	for(i = 0;i < Pworld -> pssTmax;i++){
		pp = Pworld -> pssT[i];															
		xx = x + GetX(pp -> pos_y);
		yy = y + GetY(pp -> pos_x);
		XGR_Rectangle(xx - PASSAGE_SIDE,yy - PASSAGE_SIDE,PASSAGE_SIZE,PASSAGE_SIZE,COL1,COL1,XGR_OUTLINED);
		info.init();
		info <	(char)0x10 < pp -> Poutput -> name;
		s = info.GetBuf();
		yy += PASSAGE_SIDE + 1;
		len = strlen(s)*8;
		xx -= len/2;
		if(xx < x + 2) xx = x + 2;
		else if(xx + len >= x + sx - 3) xx -= xx + len - (x + sx - 3);
#ifndef TINY_MODE
		sysfont.draw(xx,yy,s,COL1,-1);
#endif
		}

	uvsElement* ppe = Pworld -> Panymal;
	while(ppe){
			int xx = x + GetX(((uvsFlyFarmer*)ppe) -> pos_y);
			int yy = y + GetY(((uvsFlyFarmer*)ppe) -> pos_x);

			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_FLY_FARMER, XGR_HIDDEN_FON);

			ppe = ppe -> enext;
	}
}

void WorldScreen::DollyQuant(uvsDolly* pd)
{
	int xx = x + GetX(pd -> pos_y);
	int yy = y + GetY(pd -> pos_x);
	XGR_LineTo(xx - 3,yy,7,2,63);
	XGR_LineTo(xx,yy - 3,7,3,63);
	XGR_SetPixel(xx - 1,yy - 1,63); XGR_SetPixel(xx - 1,yy + 1,63);
	XGR_SetPixel(xx + 1,yy - 1,63); XGR_SetPixel(xx + 1,yy + 1,63);

	int i,j = pd -> tail_cInd,c;
	j = (j - 1 < 0) ? DOLLY_TAIL_LEN - 1 : j - 1;
	for(i = 0;i < DOLLY_TAIL_LEN;i++)
		if(pd -> tail_x[j] != -1){
			xx = x + GetX(pd -> tail_y[j]);
			yy = y + GetY(pd -> tail_x[j]);
			c = 63 - (i + 1)*60/DOLLY_TAIL_LEN;
			XGR_LineTo(xx - 2,yy,5,2,c);
			XGR_LineTo(xx,yy - 2,5,3,c);
			j = (j - 1 < 0) ? DOLLY_TAIL_LEN - 1 : j - 1;
			}
}

void WorldScreen::VangerQuant(uvsVanger* pv)
{
	int xx = x + GetX(pv -> pos_y);
	int yy = y + GetY(pv -> pos_x);

	switch(pv -> shape){
		case UVS_VANGER_SHAPE::TABUTASK:
			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_TABU, XGR_HIDDEN_FON);
			break;
		case UVS_VANGER_SHAPE::BUNCH_SLAVE:
			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_SLAVE, XGR_HIDDEN_FON);
			break;
		case UVS_VANGER_SHAPE::RANGER:
			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_RANGER,XGR_HIDDEN_FON);
			break;
		case UVS_VANGER_SHAPE::GAMER:
			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_GAMER, XGR_HIDDEN_FON);
			break;
		case UVS_VANGER_SHAPE::THIEF:
			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_THIEF, XGR_HIDDEN_FON);
			break;
		case UVS_VANGER_SHAPE::CARAVAN:
			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_CARAVAN, XGR_HIDDEN_FON);
			break;
		case UVS_VANGER_SHAPE::KILLER:
			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_KILLER, XGR_HIDDEN_FON);
			break;
		case UVS_VANGER_SHAPE::GAMER_SLAVE:
			XGR_PutSpr(xx - ICON_SX/2,yy - ICON_SY/2,ICON_SX,ICON_SY,ICON_GSLAVE, XGR_HIDDEN_FON);
			break;
		}
}

void ScreenQuant(void)
{
#ifdef TINY_MODE
	sysfont.draw(3,3,ConTimer.GetTime());
#else
	sysfont.draw(XGR_MAXX - 8*18 - 3,3,ConTimer.GetTime());
#endif

	for(int i = 0;i < MAIN_WORLD_MAX;i++)
		scrTable[i] -> Quant();

	static XBuffer buf;

#ifndef TINY_MODE
	buf.init();
	buf < "Total: " <= c_All_Number < " S: " <= c_S_Number < " R: " <= c_R_Number < " A: " <= c_A_Number;
	sysfont.draw(3,3,(char*)buf);
#endif

	uvsElement* p = ETail;
	int ind;
	while(p){
		switch(p -> type){
			case UVS_OBJECT::DOLLY:
				ind = ((uvsDolly*)p) -> Pworld -> gIndex;
				if(ind != -1) scrTable[ind] -> DollyQuant((uvsDolly*)p);
				break;
			case UVS_OBJECT::VANGER:
				ind = ((uvsVanger*)p) -> Pworld -> gIndex;
				if(ind != -1) scrTable[ind] -> VangerQuant((uvsVanger*)p);
				break;
			}
		p = p -> enext;
		}
}

void addPlace(int wInd,int x,int y)
{
	return;
	WorldScreen* w = scrTable[wInd];
	Place* p = new Place(w,x,y);
	p -> link(w -> placeTail);
}

void Place::link(Place*& tail)
{
	if(!tail) tail = this;
	else {
		tail -> prev -> next = this;
		prev = tail -> prev;
		}
	next = NULL;
	tail -> prev = this;
}

