#include "../src/global.h"

#include "../src/common.h"

#include "../src/3d/3d_math.h"

#include "../src/terra/vmap.h"
#include "impass.h"
#include "../src/terra/world.h"
#include "../src/terra/render.h"

/* ----------------------------- EXTERN SECTION ---------------------------- */
/* --------------------------- PROTOTYPE SECTION --------------------------- */
/* --------------------------- DEFINITION SECTION -------------------------- */
const int PEAK_SIZE = 9;
static uchar PeakData[PEAK_SIZE*PEAK_SIZE] = {
		 10, 20, 50, 80, 80, 80, 50, 20, 10,
		 20, 50, 80,100,100,100, 80, 50, 20,
		 20, 50,100,200,200,200,100, 50, 20,
		 20, 50,200,200,255,200,200, 50, 20,
		 20, 50,200,255,255,255,200, 50, 20,
		 20, 50,200,200,255,200,200, 50, 20,
		 20, 50,100,200,200,200,100, 50, 20,
		 20, 50, 80,100,100,100, 80, 50, 20,
		 10, 20, 50, 80, 80, 80, 50, 20, 10
		};

ImpassType* ImpPattern[IMP_MAX];
#ifdef _SURMAP_
int curImpIndex;
const char* ImpItem[IMP_MAX] = {
				"SPHERE",
				"POLYGON",
				"PEAK"
				};
#endif

void ImpassPrepare(void)
{
#ifdef _SURMAP_
	XStream ff(0);
	if(!ff.open("impass.dat",XS_IN)){
		(ImpPattern[IMP_SPHERE]  = new ImpSphere(100,100)) -> init();
		(ImpPattern[IMP_POLYGON] = new ImpPolygon(100,100)) -> init();
		(ImpPattern[IMP_PEAK]	 = new ImpPeak(PEAK_SIZE,PEAK_SIZE)) -> init();
		}
	else {
		ImpPattern[IMP_SPHERE]	= new ImpSphere(100,100);
		ImpPattern[IMP_POLYGON] = new ImpPolygon(100,100);
		ImpPattern[IMP_PEAK]	= new ImpPeak(PEAK_SIZE,PEAK_SIZE);
		ff > curImpIndex;
		ImpPattern[IMP_SPHERE] -> read(ff);
		ImpPattern[IMP_POLYGON] -> read(ff);
		ImpPattern[IMP_PEAK] -> read(ff);
		ImpPattern[IMP_SPHERE] -> init();
		ImpPattern[IMP_POLYGON] -> init();
		ImpPattern[IMP_PEAK] -> init();
		ff.close();
		}
#else
	(ImpPattern[IMP_SPHERE]  = new ImpSphere(10,10,0,0,0,100,5)) -> init();
//	(ImpPattern[IMP_POLYGON] = new ImpPolygon(50,50,0,0,0,100,100)) -> init();
//	(ImpPattern[IMP_PEAK]	 = new ImpPeak(PEAK_SIZE,PEAK_SIZE,0,0,0,100,100)) -> init();
#endif
}

void ImpassSave(void)
{
#ifdef _SURMAP_
	XStream ff("impass.dat",XS_OUT);
	ff < curImpIndex;
	ImpPattern[IMP_SPHERE] -> write(ff);
	ImpPattern[IMP_POLYGON] -> write(ff);
	ImpPattern[IMP_PEAK] -> write(ff);
	ff.close();
#endif
}

ImpassType::ImpassType(int _sx,int _sy,int _force,int _mode,int _level,int _rad,int _h,int _n,int _phase,int _inverse,int _plain,int _noiseLevel,int _noiseAmp)
{
	map = NULL;
	sx = _sx; sy = _sy;
	level = _level;
	mode = _mode;
	force = _force;
	radInt = _rad;
	rad = (double)_rad/100.;
	h = _h;
	n = _n;
	phase = _phase;
	inverse = _inverse;
	plain = _plain;
	noiseLevel = _noiseLevel;
	noiseAmp = _noiseAmp;
}

void ImpassType::init(void)
{
	map = NULL;
	change(sx,sy,force,mode,level,radInt,h,n,phase,inverse,plain,noiseLevel,noiseAmp);
}

void ImpassType::read(XStream& ff)
{
	ff > sx > sy > level > force > mode > radInt > h > n > phase > inverse > plain > noiseLevel > noiseAmp;
}

void ImpassType::write(XStream& ff)
{
	ff < sx < sy < level < force < mode < radInt < h < n < phase < inverse < plain < noiseLevel < noiseAmp;
}

void ImpassType::change(int _sx,int _sy,int _force,int _mode,int _level,int _rad,int _h,int _n,int _phase,int _inverse,int _plain,int _noiseLevel,int _noiseAmp)
{
	if(!_sx) _sx = 16; if(!_sy) _sy = 16;
	sx = _sx; sy = _sy;
	size = sx*sy;
	level = _level;
	mode = _mode;
	force = _force;

	radInt = _rad;
	rad = (double)_rad/100.;
	h = _h;
	n = _n;
	phase = _phase;
	inverse = _inverse;
	plain = _plain;
	noiseLevel = _noiseLevel;
	noiseAmp = _noiseAmp;

	if(map) delete map;
	map = new short[size];

	register int i;
	short* p = map;
	for(i = 0;i < size;i++,p++) *p = level;

	build();
	ImpassSave();
}

#ifdef _SURMAP_
void ImpassType::put(int x,int y)
{
	x -= sx/2;
	y -= sy/2;

	register int i,j;
	int xx,yy,v,vv = 0;
	short* p = map;
	uchar** lt = vMap -> lineT;

	vMap -> increase(y,y + sy - 1);

	for(j = 0;j < sy;j++)
		for(i = 0;i < sx;i++,p++)
			if(force || *p != level){
				xx = XCYCL(x + i);
				yy = YCYCL(y + j);
				if(lt[yy]){
					v = *(lt[yy] + xx);
					switch(mode){
						case 0:
							vv = *p;
							break;
						case 1:
							vv = MAX(v,*p);
							if(vv != *p) continue;
							break;
						case 2:
							vv = MIN(v,*p);
							if(vv != *p) continue;
							break;
						case 3:
							vv = v + *p >> 1;
							break;
						case 4:
							vv = v + *p;
							break;
						}
					if(noiseLevel && noiseAmp)
						if((int)realRND(100) < noiseLevel) vv += noiseAmp - realRND((noiseAmp << 1) + 1);
					pixSet(xx,yy,vv - v);
					}
				}
	regRender(x,y,x + sx,y + sy);
}
#endif

static double plane(double x,double y, double x0,double y0,double z0,double x1,double y1,double z1,double x2,double y2,double z2)
{
	double a = (y1 - y0)*(z2 - z0) - (z1 - z0)*(y2 - y0);
	double b = (x1 - x0)*(z2 - z0) - (z1 - z0)*(x2 - x0);
	double c = (x1 - x0)*(y2 - y0) - (y1 - y0)*(x2 - x0);

	return ((y - y0)*b - (x - x0)*a)/c + z0;
}

static double minim(double* d,int n)
{
	double m = 1000.0;
	for(int i = 0;i < n;i++)
		if(d[i] >= 0 && d[i] < m) m = d[i];
	return m;
}

ImpSphere::ImpSphere(int _sx,int _sy,int _force,int _mode,int _level,int _rad,int _h,int _n,int _phase,int _inverse,int _plain,int _noiseLevel,int _noiseAmp)
: ImpassType(_sx,_sy,_force,_mode,_level,_rad,_h,_n,_phase,_inverse,_plain,_noiseLevel,_noiseAmp)
{ }

void ImpSphere::build(void)
{
	double dx = 2.0/(double)sx;
	double dy = 2.0/(double)sy;
	double xx,yy,zz;
	double r2 = rad*rad;

	register int x,y;
	short* p = map;
	for(yy = -1.0,y = 0;y < sy;yy += dy,y++)
		for(xx = -1.0,x = 0;x < sx;xx += dx,x++,p++){
			zz = r2 - (xx*xx + yy*yy);
			if(zz > 0.0){
				*p = (short)(sqrt(zz)*h);
				if(inverse) *p = -*p;
				if(plain) *p = level;
				else *p += level;
				}
			}
}

ImpPolygon::ImpPolygon(int _sx,int _sy,int _force,int _mode,int _level,int _rad,int _h,int _n,int _phase,int _inverse,int _plain,int _noiseLevel,int _noiseAmp)
: ImpassType(_sx,_sy,_force,_mode,_level,_rad,_h,_n,_phase,_inverse,_plain,_noiseLevel,_noiseAmp)
{ }

void ImpPolygon::build(void)
{
	double dx = 2.0/(double)sx;
	double dy = 2.0/(double)sy;
	double xx,yy,zz;
	double* zzz = new double[n];
	double phs = (double)phase/1000.;
	double angle = 2.*M_PI/(double)n;
	double ang;

	register int x,y,i;
	short* p = map;
	for(yy = 1.0,y = 0;y < sy;yy -= dy,y++)
		for(xx = -1.0,x = 0;x < sx;xx += dx,x++,p++){
//			  zzz[0] = plane(xx,yy, -rad,rad,0.0, rad,rad,0.0, 0.0,0.0,(double)h/256.);
//			  zzz[1] = plane(xx,yy, -rad,-rad,0.0, rad,-rad,0.0, 0.0,0.0,(double)h/256.);
//			  zzz[2] = plane(xx,yy, -rad,rad,0.0, -rad,-rad,0.0, 0.0,0.0,(double)h/256.);
//			  zzz[3] = plane(xx,yy, rad,rad,0.0, rad,-rad,0.0, 0.0,0.0,(double)h/256.);

			for(ang = phs,i = 0;i < n;i++,ang += angle){
				zzz[i] = plane(xx,yy,rad*sin(ang),rad*cos(ang),0.0, rad*sin(ang + angle),rad*cos(ang + angle),0.0, 0.0,0.0,(double)h/256.);
				if(zzz[i] < 0.0) zzz[i] = 0.0;
				}

			zz = minim(zzz,n);
			if(zz > 0){
				*p = (short)(zz*h);
				if(inverse) *p = -*p;
				if(plain) *p = level;
				else *p += level;
				}
			}
	delete[] zzz;
}

ImpPeak::ImpPeak(int _sx,int _sy,int _force,int _mode,int _level,int _rad,int _h,int _n,int _phase,int _inverse,int _plain,int _noiseLevel,int _noiseAmp)
: ImpassType(_sx,_sy,_force,_mode,_level,_rad,_h)
{ }

void ImpPeak::build(void)
{
	for(int i = 0;i < sx*sy;i++) map[i] = PeakData[i];
}

