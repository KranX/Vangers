#ifndef __TERRA__RENDER_H__
#define __TERRA__RENDER_H__

const int CLR_MAX_SIDE = 255;
const int CLR_MAX = 2*CLR_MAX_SIDE + 1;
const uchar H_CORRECTION = 1;
extern uchar* lightCLR[TERRAIN_MAX];
extern uchar palCLR[TERRAIN_MAX][2*256];

#define DX(x,d) 	(XCYCL((x) + (d)))
#define DY(y,d) 	(YCYCL((y) + (d)))

#define GET_TERRAIN_TYPE(a)	((a) & TERRAIN_MASK)
#define GET_TERRAIN(a)		(((a) & TERRAIN_MASK) >> TERRAIN_OFFSET)

#define IS_DOUBLE(a)	((a) & DOUBLE_LEVEL)

inline uchar GET_REAL_TERRAIN(uchar* type,int x)
{
	if(x & 1) return GET_TERRAIN(*type);
	else if(*type & DOUBLE_LEVEL) return GET_TERRAIN(*(type + 1));
	return GET_TERRAIN(*type);
}

inline uchar GET_REAL_TERRAIN_TYPE(uchar* type,int x)
{
	if(x & 1) return GET_TERRAIN_TYPE(*type);
	else if(*type & DOUBLE_LEVEL) return GET_TERRAIN_TYPE(*(type + 1));
	return GET_TERRAIN_TYPE(*type);
}

inline uchar GET_REAL_DOWNTERRAIN(uchar* type,int x)
{
	if(*type & DOUBLE_LEVEL) {
		if(!(x & 1)) {
			return GET_TERRAIN(*type);
		} else {
			return GET_TERRAIN(*(type - 1));
		}
	}
	return GET_TERRAIN(*type);
}

inline uchar GET_REAL_DOWNTERRAIN_TYPE(uchar* type,int x)
{
	if(*type & DOUBLE_LEVEL) {
		if(!(x & 1)) {
			return GET_TERRAIN_TYPE(*type);
		} else {
			return GET_TERRAIN_TYPE(*(type - 1));
		}
	}
	return GET_TERRAIN_TYPE(*type);
}

inline uchar GET_DOWN_ALT(uchar* type,int h,uchar* pa0,int x)
{
	if(IS_DOUBLE(*type)) if(x & 1) h = *(pa0 + x - 1);
	return h;
}

inline uchar GET_UP_ALT(uchar* type,int h,uchar* pa0,int x)
{
	if(IS_DOUBLE(*type)){
		if(x & 1)
			h = *(pa0 + x);
		else
			h = *(pa0 + x + 1);
		}
	return h;
}

inline void SET_UP_ALT(uchar* type,uchar h,uchar* pa0,int x)
{
	if(*type & DOUBLE_LEVEL){ if(x & 1) *(pa0 + x) = h; }
	else *(pa0 + x) = h;
}

inline void SET_DOWN_ALT(uchar* type,uchar h,uchar* pa0,int x)
{
	if(*type & DOUBLE_LEVEL){ if(!(x & 1)) *(pa0 + x) = h; }
	else *(pa0 + x) = h;
}

inline int SET_REAL_TERRAIN(uchar* type,uchar newtype,int x)
{
	if(*type & DOUBLE_LEVEL){
		if(x & 1){ SET_TERRAIN(*type,newtype); return 1; }
		}
	else {
		SET_TERRAIN(*type,newtype);
		return 1;
		}
	return 0;
}

inline int SET_REAL_DOWNTERRAIN(uchar* type,uchar newtype,int x)
{
	if(*type & DOUBLE_LEVEL){
		if(!(x & 1)){ SET_TERRAIN(*type,newtype); return 1; }
		}
	else {
		SET_TERRAIN(*type,newtype);
		return 1;
		}	
	return 0;
}

inline uchar GET_WIDTH(uchar* type, int x)
{
	if(*type & DOUBLE_LEVEL){
		if(x & 1)
			return (((GET_DELTA(*(type - 1)) << 2) + GET_DELTA(*type) + 1) << DELTA_SHIFT);
		else
			return (((GET_DELTA(*type) << 2) + GET_DELTA(*(type + 1)) + 1) << DELTA_SHIFT);
		}
	return 0;
}

void regRender(int x0,int y0,int x1,int y1,int changed = 1);
void regRenderSimple(int x0,int y0,int x1,int y1);
void regDownRender(int x0,int y0,int x1,int y1,int changed = 1);
void pixSet(int x,int y,int delta,int surf = 1);
void pixSetR(int x,int y,int delta,int surf = 1);
void pixDownSet(int x,int y,int delta,int surf = 1);
void regSet(int x0,int y0,int x1,int y1,int dry,int surf = 1);

#define GETUPALT(x,y)	GET_UP_ALT((vMap -> lineT[y] + H_SIZE + (x)),(*(vMap -> lineT[y] + (x))),vMap -> lineT[y],x)
#define GETDOWNALT(x,y)	GET_DOWN_ALT((vMap -> lineT[y] + H_SIZE + (x)),(*(vMap -> lineT[y] + (x))),vMap -> lineT[y],x)
#define GETWIDTH(x,y) GET_WIDTH(vMap -> lineT[y] + H_SIZE + (x), x)
#define GETCOL(x,y)	(*(vMap -> lineTcolor[y] + (x)))
#define GETTERRAIN(x,y) GET_REAL_TERRAIN((vMap -> lineT[y] + H_SIZE + (x)),x)
#define GETDOWNTERRAIN(x,y) GET_REAL_DOWNTERRAIN((vMap -> lineT[y] + H_SIZE + (x)),x)

#endif