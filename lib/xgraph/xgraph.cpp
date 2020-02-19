//-#define FULLSCREEN_DEBUG

/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "xgraph.h"
#include <assert.h>

#ifdef __APPLE__
#include "ApplicationServices/ApplicationServices.h"
#endif

/* ----------------------------- STRUCT SECTION ----------------------------- */

struct XGR_ColorData
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	//unsigned char A;
};

/* ----------------------------- EXTERN SECTION ----------------------------- */

//extern HANDLE XAppHinst;
//extern HANDLE XGR_hWnd;
extern int XAppMode;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void XFNT_Prepare(void);

void xtRegisterSysMsgFnc(void (*fPtr)(SDL_Event*),int id);
void xtRegisterSysFinitFnc(void (*fPtr)(void),int id);
void xtDeactivateSysFinitFnc(int id);

void XGR_FinitFnc(void);
void XGR_MouseFnc(SDL_Event* p);

//char* ddError(HRESULT error);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define DD_STATE(a)		{ DDrawState = a; if(DDrawState != DD_OK ) ErrH.Abort("DirectDraw error...",XERR_USER,DDrawState,ddError(DDrawState)); }

//RECT XGR_Rect;

//LPDIRECTDRAWPALETTE XGR_DDPal = NULL;		// DirectDraw palette...
//HRESULT DDrawState;

XGR_Screen XGR_Obj;
XGR_Mouse XGR_MouseObj;

int xgrScreenSizeX = 0;
int xgrScreenSizeY = 0;

int XGR_InitFlag = 0;

int XGR_MouseOffsX = 0;
int XGR_MouseOffsY = 0;

XGR_OutTextHandler	XGR_TextOutFnc = NULL;
XGR_TextWidthHandler	XGR_TextXFnc = NULL;
XGR_TextHeightHandler	XGR_TextYFnc = NULL;

int XGR_MASK_R = 0;
int XGR_MASK_G = 0;
int XGR_MASK_B = 0;

int XGR_SHIFT_R = 0;
int XGR_SHIFT_G = 0;
int XGR_SHIFT_B = 0;

int XGR_COLOR_MASK_R = 0;
int XGR_COLOR_MASK_G = 0;
int XGR_COLOR_MASK_B = 0;

const char* XGraphWndID = "XGraphWindow";

int XGR_HighColorMode = 0;
int XGR_SysMsgFlag = 0;

bool XGR_FULL_SCREEN = false;

void XGR_FinitFnc(void)
{
	XGR_Finit();
}

Uint32 CursorAnim(Uint32 interval, void *param)
{
	int result = 0;
	result += XGR_MouseObj.NextFrame();
	
	return interval;
}


XGR_Screen::XGR_Screen(void)
{
	flags = 0;

	ClipMode = XGR_CLIP_PUTSPR;

	ScreenX = ScreenY = 0;
	yOffsTable = NULL;
	yStrOffs = 0;

	XGR_ScreenSurface = NULL;
	XGR32_ScreenSurface = NULL;
	sdlWindow = NULL;
	sdlRenderer = NULL;
	sdlTexture = NULL;
}

#ifdef WITH_OPENGL
GLuint XGR_Screen::SurfToTexture(SDL_Surface *surf)
{
	GLuint texture;			// This is a handle to our texture object
	GLenum texture_format;
	GLint  nOfColors;
	// Check that the image's width is a power of 2
	if ( (surf->w & (surf->w - 1)) != 0 ) {
		//std::cout<<"XGR_Screen::SurfToTexture warning surf width is not a power of 2"<<std::endl;
	}
	
	// Also check if the height is a power of 2
	if ( (surf->h & (surf->h - 1)) != 0 ) {
		//std::cout<<"XGR_Screen::SurfToTexture warning surf height is not a power of 2"<<std::endl;
	}
	        // get the number of channels in the SDL surface
        nOfColors = surf->format->BytesPerPixel;
        if (nOfColors == 4)     // contains an alpha channel
        {
                if (surf->format->Rmask == 0x000000ff)
                        texture_format = GL_RGBA;
                else
                        texture_format = GL_BGRA;
        } else if (nOfColors == 3)     // no alpha channel
        {
                if (surf->format->Rmask == 0x000000ff)
                        texture_format = GL_RGB;
                else
                        texture_format = GL_BGR;
        } else {
                std::cout<<"XGR_Screen::SurfToTexture warning: the image is not truecolor..  this will probably break"<<std::endl;
                // this error should not go unhandled
        }
        
	// Have OpenGL generate a texture object handle for us
	glGenTextures( 1, &texture );
 
	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, texture );
 
	// Set the texture's stretching properties
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
 
	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surf->w, surf->h, 0,
                      texture_format, GL_UNSIGNED_BYTE, surf->pixels );
	return texture;
}
#endif

int XGR_Screen::init(int x,int y,int flags_in)
{
	flags = flags_in;
	std::cout<<"XGR_Screen::init"<<std::endl;
	// Init SDL video
	if (XGR_ScreenSurface==NULL) {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
			ErrH.Abort(SDL_GetError(),XERR_USER, 0);
		}
		SDL_AddTimer(100, CursorAnim, NULL);
	} else {
		SDL_DestroyTexture(sdlTexture);

		SDL_DestroyRenderer(sdlRenderer);
		SDL_DestroyWindow(sdlWindow);
	}
	std::cout<<"SDL_CreateWindowAndRenderer"<<std::endl;
	if (XGR_FULL_SCREEN) {
		if (SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &sdlWindow, &sdlRenderer) < 0) {
			std::cout<<"ERROR1"<<std::endl;
			ErrH.Abort(SDL_GetError(),XERR_USER, 0);
		}
	} else {
		if (SDL_CreateWindowAndRenderer(x, y, 0, &sdlWindow, &sdlRenderer) < 0) {
			std::cout<<"ERROR2"<<std::endl;
			ErrH.Abort(SDL_GetError(),XERR_USER, 0);
		}
	}
	std::cout<<"SDL_SetWindowTitle"<<std::endl;
	SDL_SetWindowTitle(sdlWindow, "Vangers");
	
	std::cout<<"Load and set icon"<<std::endl;
#ifdef __APPLE__
	IconSurface = SDL_LoadBMP("vangers_mac.bmp");
#else
	IconSurface = SDL_LoadBMP("vangers.bmp");
#endif
	if (IconSurface) {
		SDL_SetWindowIcon(sdlWindow, IconSurface); 
		SDL_FreeSurface(IconSurface);
	} else {
		std::cout<<"Can't load icon vangers.bmp"<<std::endl;
	}
	std::cout<<"SDL_SetRenderDrawColor"<<std::endl;
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	std::cout<<"SDL_RenderClear"<<std::endl;
	SDL_RenderClear(sdlRenderer);
	std::cout<<"SDL_RenderPresent"<<std::endl;
	SDL_RenderPresent(sdlRenderer);
	
	std::cout<<"SDL_SetHint"<<std::endl;
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");  // "linear" make the scaled rendering look smoother.
	
	XGR_Palette = SDL_AllocPalette(256);
	
	std::cout<<"XGR_ScreenSurface = SDL_CreateRGBSurface"<<std::endl;
	XGR_ScreenSurface = SDL_CreateRGBSurface(0, x, y, 8, 0, 0, 0, 0);
	
	std::cout<<"XGR32_ScreenSurface = SDL_CreateRGBSurface"<<std::endl;
	XGR32_ScreenSurface = SDL_CreateRGBSurface(0, x, y, 32, 0, 0, 0, 0);
	
	std::cout<<"SDL_SetSurfacePalette"<<std::endl;
	SDL_SetSurfacePalette(XGR_ScreenSurface, XGR_Palette);
	
	
	std::cout<<"SDL_CreateTexture sdlTexture"<<std::endl;
	sdlTexture = SDL_CreateTexture(sdlRenderer,
		SDL_PIXELFORMAT_ARGB8888, //SDL_PIXELFORMAT_INDEX8,
		SDL_TEXTUREACCESS_STREAMING,
		x, y);
	
	SDL_GetWindowSize(sdlWindow, &RealX, &RealY);
	HDBackgroundSurface = SDL_LoadBMP("hd_background.bmp");
	SDL_SetSurfacePalette(HDBackgroundSurface, XGR_Palette);
	HDBackgroundTexture = SDL_CreateTextureFromSurface(sdlRenderer, HDBackgroundSurface);
	std::cout<<"SDL_ShowCursor"<<std::endl;
	//SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);
	
	if (XGR_FULL_SCREEN) {
		std::cout<<"SDL_SetWindowPosition"<<std::endl;
		SDL_SetWindowPosition(sdlWindow, 0, 0);
	}
	// TODO(amdmi3): assuming safe locking; otherwise, use additional surface + SDL_MapRGB
	std::cout<<"SDL_LockSurface"<<std::endl;
	if (SDL_LockSurface(XGR_ScreenSurface) < 0)
		ErrH.Abort(SDL_GetError(),XERR_USER, 0);

	ScreenBuf = (unsigned char*)XGR_ScreenSurface->pixels;

	// Other initializations
	ScreenX = xgrScreenSizeX = XGR_ScreenSurface->w;
	ScreenY = xgrScreenSizeY = XGR_ScreenSurface->h;

	if(yOffsTable) delete[] yOffsTable;
		yOffsTable = new int[y + 1];
	set_pitch(XGR_ScreenSurface->pitch);


	XFNT_Prepare();

	//Need SEE!!! TIMER
	//if(!(flags & XGR_REINIT)) SetTimer((HWND)XGR_hWnd,1,100,NULL);

	flags &= ~XGR_REINIT;

	//XRec.hWnd = XGR_hWnd;

	set_clip(0,0,x,y);
	set_clip_mode(XGR_CLIP_PUTSPR);

	if(XGR_MouseObj.flags & XGM_INIT){
		if(XGR_MouseObj.flags & XGM_AUTOCLIP){
			XGR_MouseObj.SetClipAuto();
			//XGR_MouseObj.AdjustPos();
		}
		XGR_MouseObj.flags &= ~XGM_PROMPT_ACTIVE;
	}

	xtClearMessageQueue();
	XGR_InitFlag = 1;
	flags |= XGR_INIT;

// 	if (old_surface_1!=NULL) {
// 		SDL_FreeSurface(old_surface_1);
// 		SDL_FreeSurface(old_surface_2);
// 	}
	
	return false;
}

void XGR_Screen::set_fullscreen(bool fullscreen) {
	if (fullscreen!=XGR_FULL_SCREEN) {
		SDL_SetWindowFullscreen(sdlWindow, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
		if (!fullscreen) {
			SDL_SetWindowSize(sdlWindow, 800, 600);
			SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			
		} else {
			SDL_SetWindowPosition(sdlWindow, 0, 0);
		}
		XGR_FULL_SCREEN = fullscreen;
#ifdef __APPLE__
		CGDisplayHideCursor(kCGDirectMainDisplay);
#endif
	} 
}

void XGR_Screen::setpixel(int x,int y,int col)
{
	assert(SDL_LockSurface(XGR_ScreenSurface) == 0);
	if(ClipMode == XGR_CLIP_ALL && (x < clipLeft || x >= clipRight || y < clipTop || y >= clipBottom)) return;
	ScreenBuf[yOffsTable[y] + x] = col;
	SDL_UnlockSurface(XGR_ScreenSurface);
}

void XGR_Screen::setpixel16(int x,int y,int col)
{
	assert(SDL_LockSurface(XGR_ScreenSurface) == 0);
	unsigned short* p;
	if(ClipMode == XGR_CLIP_ALL && (x < clipLeft || x >= clipRight || y < clipTop || y >= clipBottom)) return;

	p = (unsigned short*)(ScreenBuf + yOffsTable[y]);
	p[x] = col;
	SDL_UnlockSurface(XGR_ScreenSurface);
}

int XGR_Screen::getpixel(int x,int y)
{
	return (ScreenBuf[yOffsTable[y] + x]);
}

void XGR_Screen::set_pitch(int p)
{
	int i;
	if (flags & XGR_HICOLOR)
		p /= 2;

	for(i = 0; i <= ScreenY; i++)
		yOffsTable[i] = i * p;

	yStrOffs = p;
}

void XGR_Screen::set_clip(int left,int top,int right,int bottom)
{
	clipLeft = left;
	clipTop = top;
	clipRight = right;
	clipBottom = bottom;
}

void XGR_Screen::get_clip(int& left,int& top,int& right,int& bottom)
{
	left = clipLeft;
	top = clipTop;
	right = clipRight;
	bottom = clipBottom;
}

void XGR_Screen::putspr(int x,int y,int sx,int sy,void* p,int mode)
{
	//std::cout<<"XGR_Screen::putspr "<<x<<" "<<sx<<" "<<y<<" "<<sy<<std::endl;
	assert(SDL_LockSurface(XGR_ScreenSurface) == 0);
	int i,j,_x,_y,_x1,_y1,_sx,_sy,dx = 0,dy = 0;
	unsigned char* scrBuf,*memBuf;
	unsigned char* ptr = (unsigned char*)p;

	if(ClipMode == XGR_CLIP_ALL) mode |= XGR_CLIPPED;

	if(mode & XGR_CLIPPED && !(mode & XGR_NOCLIP)){
		_x = (x > clipLeft) ? x : clipLeft;
		_y = (y > clipTop) ? y : clipTop;

		dx = _x - x;
		dy = _y - y;

		_x1 = (x + sx <= clipRight) ? (x + sx) : clipRight;
		_y1 = (y + sy <= clipBottom) ? (y + sy) : clipBottom;

		_sx = _x1 - _x;
		_sy = _y1 - _y;
	}
	else {
		_x = x;
		_y = y;
		_sx = sx;
		_sy = sy;
	}
	if(_sx <= 0 || _sy <= 0) return;

	scrBuf = ScreenBuf + yOffsTable[_y] + _x;
	memBuf = ptr + dy * sx + dx;

	//Draw video and cursor
	if(mode & XGR_BLACK_FON){
		for(i = 0; i < _sy; i ++){
			memcpy(scrBuf,memBuf,_sx);
			scrBuf += yStrOffs;
			memBuf += sx;
		}
	} else {//Other
		for(i = 0; i < _sy; i ++){
			for(j = 0; j < _sx; j ++){
				if(memBuf[j]){
					scrBuf[j] = memBuf[j];
				}
			}
			scrBuf += yStrOffs;
			memBuf += sx;
		}
	}
	SDL_UnlockSurface(XGR_ScreenSurface);
}

void XGR_Screen::putspr16(int x,int y,int sx,int sy,void* p,int mode)
{
	int i,j,_x,_y,_x2,_x1,_y1,_sx,_sy,_sx2,sx2,dx = 0,dy = 0,yoffs;

	unsigned short* s_scrBuf,*s_pBuf;
	unsigned char* c_scrBuf,*c_pBuf;

	if(ClipMode == XGR_CLIP_ALL) mode |= XGR_CLIPPED;

	if(mode & XGR_CLIPPED && !(mode & XGR_NOCLIP)){
		_x = (x > clipLeft) ? x : clipLeft;
		_y = (y > clipTop) ? y : clipTop;

		dx = _x - x;
		dy = _y - y;

		_x1 = (x + sx < clipRight) ? (x + sx) : clipRight;
		_y1 = (y + sy < clipBottom) ? (y + sy) : clipBottom;

		_sx = _x1 - _x;
		_sy = _y1 - _y;
	}
	else {
		_x = x;
		_y = y;
		_sx = sx;
		_sy = sy;
	}
	if(_sx <= 0 || _sy <= 0) return;

	_x2 = _x << 1;
	_sx2 = _sx << 1;
	sx2 = sx << 1;

	if(mode & XGR_BLACK_FON){
		if(mode & XGR_BOTTOM_UP){
			c_scrBuf = ScreenBuf + yOffsTable[_y + _sy - 1] + _x2;
			c_pBuf = ((unsigned char*)p) + (dx << 1) + dy * sx2;
			for(i = 0; i < _sy; i ++){
				memcpy(c_scrBuf,c_pBuf,_sx2);
				c_scrBuf -= yStrOffs;
				c_pBuf += sx2;
			}
		}
		else {
			c_scrBuf = ScreenBuf + yOffsTable[_y] + _x2;
			c_pBuf = ((unsigned char*)p) + (dx << 1) + dy * sx2;
			for(i = 0; i < _sy; i ++){
				memcpy(c_scrBuf,c_pBuf,_sx2);
				c_scrBuf += yStrOffs;
				c_pBuf += sx2;
			}
		}
	}
	else {
		yoffs = yStrOffs >> 1;
		s_scrBuf = (unsigned short*)(ScreenBuf + yOffsTable[_y]) + _x;
		s_pBuf = ((unsigned short*)p) + dx + dy * sx;
		for(i = 0; i < _sy; i ++){
			for(j = 0; j < _sx; j ++){
				if(s_pBuf[j]){
					s_scrBuf[j] = s_pBuf[j];
				}
			}
			s_scrBuf += yoffs;
			s_pBuf += sx;
		}
	}
}

void XGR_Screen::putspr16a(int x,int y,int sx,int sy,void* p,int mode,int alpha)
{
	int i,j,_x,_y,_x1,_y1,_sx,_sy,dx = 0,dy = 0,yoffs,r,g,b;

	unsigned col0,col1;

	int alpha1 = 256 - alpha;

	unsigned short* s_scrBuf,*s_pBuf;

	if(ClipMode == XGR_CLIP_ALL) mode |= XGR_CLIPPED;

	if(mode & XGR_CLIPPED && !(mode & XGR_NOCLIP)){
		_x = (x > clipLeft) ? x : clipLeft;
		_y = (y > clipTop) ? y : clipTop;

		dx = _x - x;
		dy = _y - y;

		_x1 = (x + sx < clipRight) ? (x + sx) : clipRight;
		_y1 = (y + sy < clipBottom) ? (y + sy) : clipBottom;

		_sx = _x1 - _x;
		_sy = _y1 - _y;
	}
	else {
		_x = x;
		_y = y;
		_sx = sx;
		_sy = sy;
	}
	if(_sx <= 0 || _sy <= 0) return;

	yoffs = yStrOffs >> 1;
	s_scrBuf = (unsigned short*)(ScreenBuf + yOffsTable[_y]) + _x;
	s_pBuf = ((unsigned short*)p) + dx + dy * sx;
	for(i = 0; i < _sy; i ++){
		for(j = 0; j < _sx; j ++){
			if(s_pBuf[j]){
				col1 = s_pBuf[j];
				col0 = s_scrBuf[j];
				r = (((col0 & XGR_MASK_R) * alpha + (col1 & XGR_MASK_R) * alpha1)) & (XGR_MASK_R << 8);
				g = (((col0 & XGR_MASK_G) * alpha + (col1 & XGR_MASK_G) * alpha1)) & (XGR_MASK_G << 8);
				b = (((col0 & XGR_MASK_B) * alpha + (col1 & XGR_MASK_B) * alpha1)) & (XGR_MASK_B << 8);
				s_scrBuf[j] = (r + g + b) >> 8;
			}
		}
		s_scrBuf += yoffs;
		s_pBuf += sx;
	}
}

void XGR_Screen::putspr16ap(int x,int y,int sx,int sy,void* p,int mode,void* alpha_ptr)
{
	int i,j,_x,_y,_x1,_y1,_sx,_sy,dx = 0,dy = 0,yoffs,r,g,b;
	unsigned char* alpha_p = (unsigned char*)alpha_ptr;

	unsigned col0,col1;

	int alpha,alpha1,alpha_index = 0;

	unsigned short* s_scrBuf,*s_pBuf;

	if(ClipMode == XGR_CLIP_ALL) mode |= XGR_CLIPPED;

	if(mode & XGR_CLIPPED && !(mode & XGR_NOCLIP)){
		_x = (x > clipLeft) ? x : clipLeft;
		_y = (y > clipTop) ? y : clipTop;

		dx = _x - x;
		dy = _y - y;

		_x1 = (x + sx < clipRight) ? (x + sx) : clipRight;
		_y1 = (y + sy < clipBottom) ? (y + sy) : clipBottom;

		_sx = _x1 - _x;
		_sy = _y1 - _y;
	}
	else {
		_x = x;
		_y = y;
		_sx = sx;
		_sy = sy;
	}
	if(_sx <= 0 || _sy <= 0) return;


	alpha_index = dx + dy * sx;

	yoffs = yStrOffs >> 1;
	s_scrBuf = (unsigned short*)(ScreenBuf + yOffsTable[_y]) + _x;
	s_pBuf = ((unsigned short*)p) + dx + dy * sx;
	for(i = 0; i < _sy; i ++){
		for(j = 0; j < _sx; j ++){
			alpha1 = alpha_p[alpha_index + j];
			alpha = 256 - alpha1;
			if(alpha){
				col1 = s_pBuf[j];
				col0 = s_scrBuf[j];
				r = (((col0 & XGR_MASK_R) * alpha + (col1 & XGR_MASK_R) * alpha1)) & (XGR_MASK_R << 8);
				g = (((col0 & XGR_MASK_G) * alpha + (col1 & XGR_MASK_G) * alpha1)) & (XGR_MASK_G << 8);
				b = (((col0 & XGR_MASK_B) * alpha + (col1 & XGR_MASK_B) * alpha1)) & (XGR_MASK_B << 8);
				s_scrBuf[j] = (r + g + b) >> 8;
			}
		}
		s_scrBuf += yoffs;
		s_pBuf += sx;
		alpha_index += sx;
	}
}

void XGR_Screen::getspr(int x,int y,int sx,int sy,void* p)
{
	int i;
	unsigned char* scrBuf,*memBuf;
	unsigned char* ptr = (unsigned char*)p;

	if(sx <= 0 || sy <= 0) return;

	scrBuf = ScreenBuf + yOffsTable[y] + x;
	memBuf = ptr;

	for(i = 0; i < sy; i ++){
		memcpy(memBuf,scrBuf,sx);
		scrBuf += yStrOffs;
		memBuf += sx;
	}
}

void XGR_Screen::getspr16(int x,int y,int sx,int sy,void* p)
{
	int i,sx2;
	unsigned char* scrBuf,*memBuf;
	unsigned char* ptr = (unsigned char*)p;

	if(sx <= 0 || sy <= 0) return;

	sx2 = (sx << 1);
	scrBuf = ScreenBuf + yOffsTable[y] + (x << 1);
	memBuf = ptr;

	for(i = 0; i < sy; i ++){
		memcpy(memBuf,scrBuf,sx2);
		scrBuf += yStrOffs;
		memBuf += sx2;
	}
}

void XGR_Screen::erase(int x,int y,int sx,int sy,int col)
{
	assert(SDL_LockSurface(XGR_ScreenSurface) == 0);
	int i,_x,_y,_x1,_y1;
	unsigned char* ptr;

	if(flags & XGR_HICOLOR){
		erase16(x,y,sx,sy,col);
		return;
	}

	if(ClipMode == XGR_CLIP_ALL){
		_x = (x > clipLeft) ? x : clipLeft;
		_y = (y > clipTop) ? y : clipTop;

		_x1 = (x + sx < clipRight) ? (x + sx) : clipRight;
		_y1 = (y + sy < clipBottom) ? (y + sy) : clipBottom;

		sx = _x1 - _x;
		sy = _y1 - _y;

		if(sx <= 0 || sy <= 0) return;
	}
	else {
		_x = x;
		_y = y;
	}
	for(i = _y; i < _y + sy; i ++){
		ptr = ScreenBuf + yOffsTable[i] + _x;
		memset(ptr,col,sx);
	}
	SDL_UnlockSurface(XGR_ScreenSurface);
}

void XGR_Screen::erase16(int x,int y,int sx,int sy,int col)
{
	int i,j,_x,_y,_x1,_y1;
	unsigned short* ptr;
	if(ClipMode == XGR_CLIP_ALL){
		_x = (x > clipLeft) ? x : clipLeft;
		_y = (y > clipTop) ? y : clipTop;

		_x1 = (x + sx < clipRight) ? (x + sx) : clipRight;
		_y1 = (y + sy < clipBottom) ? (y + sy) : clipBottom;

		sx = _x1 - _x;
		sy = _y1 - _y;

		if(sx <= 0 || sy <= 0) return;
	}
	else {
		_x = x;
		_y = y;
	}
	for(i = _y; i < _y + sy; i ++){
		ptr = (unsigned short*)(ScreenBuf + yOffsTable[i] + (_x << 1));
		for(j = 0; j < sx; j ++){
			ptr[j] = col;
		}
	}
}

void XGR_Screen::finit(void)
{
	//char* ptr;
	if(flags & XGR_INIT){
		flags ^= XGR_INIT;

		xtDeactivateSysFinitFnc(XGRAPH_SYSOBJ_ID);

//		if(XGR_hWnd) KillTimer((HWND)XGR_hWnd,1);

		SDL_UnlockSurface(XGR_ScreenSurface);
		SDL_Quit();

		// TODO(AMDmi3): uncomment/rewrite more stuff to free used resources

		/*if(XGR_DIBptr){
			DeleteObject(XGR_DIB_Handle);
			ptr = (char*)XGR_DIBptr;
			delete ptr;

			XGR_DIB_Handle = NULL;
			XGR_DIBptr = NULL;
		}

		if(XGR_PrimSurf){
			DD_STATE(XGR_PrimSurf -> Release());
			XGR_PrimSurf = NULL;
		}
		if(XGR_BackSurf){
			DD_STATE(XGR_BackSurf -> Release());
			XGR_BackSurf = NULL;
		}

		if(XGR_DDPal){
			XGR_DDPal -> Release();
			XGR_DDPal = NULL;
		}

		if(XGR_DDObj){
			XGR_DDObj -> Release();
			XGR_DDObj = NULL;
		}

		DeleteObject(XGR_PalHandle);

		if(XGR_Pal){
			ptr = (char*)XGR_Pal;
			delete ptr;
		}*/

//		XGR_PalHandle = NULL;
//		XGR_Pal = NULL;
		XGR_hWnd = NULL;

		if(yOffsTable) delete[] yOffsTable;
		yOffsTable = NULL;
		XGR_InitFlag = 0;
	}
}

void XGR_Screen::close(void)
{
	//char* ptr;
	if(flags & XGR_INIT){
		flags ^= XGR_INIT;

		xtDeactivateSysFinitFnc(XGRAPH_SYSOBJ_ID);

		SDL_UnlockSurface(XGR_ScreenSurface);
		SDL_Quit();

		// TODO(AMDmi3): uncomment/rewrite more stuff to free used resources
		// TODO(AMDmi3): merge with finit

		/*if(XGR_DIBptr){
			DeleteObject(XGR_DIB_Handle);
			ptr = (char*)XGR_DIBptr;
			delete ptr;

			XGR_DIB_Handle = NULL;
			XGR_DIBptr = NULL;
		}

		if(XGR_PrimSurf){
			DD_STATE(XGR_PrimSurf -> Release());
			XGR_PrimSurf = NULL;
		}
		if(XGR_BackSurf){
			DD_STATE(XGR_BackSurf -> Release());
			XGR_BackSurf = NULL;
		}

		if(XGR_DDPal){
			XGR_DDPal -> Release();
			XGR_DDPal = NULL;
		}

		if(XGR_DDObj){
			XGR_DDObj -> Release();
			XGR_DDObj = NULL;
		}

		DeleteObject(XGR_PalHandle);

		if(XGR_Pal){
			ptr = (char*)XGR_Pal;
			delete ptr;
		}*/

//		XGR_PalHandle = NULL;
//		XGR_Pal = NULL;

		if(yOffsTable) delete[] yOffsTable;
		yOffsTable = NULL;
		XGR_InitFlag = 0;
	}
}

int UI_OR_GAME=1;

void XGR_Screen::blitScreen(uint32_t *dst, uint8_t *src) { 
	int x, y; 
	SDL_Color color;

	for (y = XGR_ScreenSurface->h; y > 0; y--) {
		for (x = XGR_ScreenSurface->w; x > 0; x--) {
			color = XGR_Palette->colors[*src];
			*(dst++) = SDL_MapRGBA(XGR32_ScreenSurface->format, color.r, color.g, color.b, color.a);
			src++;
		}
	}
} 

void XGR_Screen::set_render_buffer(SDL_Surface *buf) {
	ScreenBuf = (unsigned char*)buf->pixels;
}


void XGR_Screen::flip()
{
	if(flags & XGR_INIT){
		
		XGR_MouseObj.GetFon();
		XGR_MouseObj.PutFrame();
		//XGR_MouseObj.GetPromptFon();
		//XGR_MouseObj.PutPrompt();
		//std::cout<<"Flip"<<std::endl;
		assert(SDL_LockSurface(XGR_ScreenSurface) == 0);
#ifdef WITH_OPENGL
		//SDL_GL_SwapBuffers();
		SDL_RenderPresent(sdlRenderer);
		if (UI_OR_GAME) {
			//Texture realisation
			/*SDL_Surface *tmp_surf = SDL_ConvertSurface(XGR_ScreenSurface,XGR_ScreenSurface_Real->format, 0);
			GLuint screen_tex=SurfToTexture(tmp_surf);
			glBindTexture( GL_TEXTURE_2D, screen_tex );
 
			glBegin( GL_QUADS );
			//Top-left vertex (corner)
			glTexCoord2i( 0, 0 );
			glVertex3f( 0, 0, 0.0f );
	
			//Bottom-left vertex (corner)
			glTexCoord2i( 1, 0 );
			glVertex3f( tmp_surf->w, 0, 0.0f );
	
			//Bottom-right vertex (corner)
			glTexCoord2i( 1, 1 );
			glVertex3f( tmp_surf->w, tmp_surf->h, 0.0f );
	
			//Top-right vertex (corner)
			glTexCoord2i( 0, 1 );
			glVertex3f( 0, tmp_surf->h, 0.0f );
			glEnd();
		
		
			SDL_FreeSurface(tmp_surf);
			glDeleteTextures(1, &screen_tex);*/
		} else {
			
		}
#else
		SDL_LockTexture(sdlTexture, NULL, &XGR32_ScreenSurface->pixels, &XGR32_ScreenSurface->pitch);
		blitScreen((uint32_t *)XGR32_ScreenSurface->pixels, (uint8_t *)XGR_ScreenSurface->pixels);
		SDL_UnlockTexture(sdlTexture);
		
		SDL_RenderClear(sdlRenderer);
		
		if (XGR_FULL_SCREEN) {
			SDL_GetWindowSize(sdlWindow, &RealX, &RealY);
			SDL_RenderSetLogicalSize(sdlRenderer, RealX, RealY);
			SDL_SetTextureColorMod(HDBackgroundTexture, averageColorPalette.r, averageColorPalette.g, averageColorPalette.b);
			SDL_RenderCopy(sdlRenderer, HDBackgroundTexture, NULL, NULL);
		}
		SDL_RenderSetLogicalSize(sdlRenderer, XGR_ScreenSurface->w, XGR_ScreenSurface->h);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		
		SDL_RenderPresent(sdlRenderer);
#endif
		SDL_UnlockSurface(XGR_ScreenSurface);
		XGR_MouseObj.PutFon();
		//XGR_MouseObj.PutPromptFon();
	}
}

void XGR_Screen::flush(int x,int y,int sx,int sy)
{
//std::cout<<"flush: "<<x<<" "<<sx<<" "<<y<<" "<<sy<<std::endl;
//For fix trash nums...
//if (sx>800)
//	ScreenBuf[666666666666]++;

/*	int mouseFlag = 0;
	int pmouseFlag = 0;

	if(XGR_MouseObj.CheckRedraw(x,y,sx,sy)) mouseFlag = 1;
	if(XGR_MouseObj.CheckPromptRedraw(x,y,sx,sy)) pmouseFlag = 1;



	if(flags & XGR_INIT){
		if (sx <= 0 || sy <= 0)
			return;
		if(mouseFlag){
			XGR_MouseObj.GetFon();
			XGR_MouseObj.PutFrame();
		}
		if(pmouseFlag){
			XGR_MouseObj.GetPromptFon();
			XGR_MouseObj.PutPrompt();
		}

		ssert(SDL_LockSurface(XGR_ScreenSurface) == 0);
		std::cout<<"flush: "<<x<<" "<<sx<<" "<<y<<" "<<sy<<std::endl;
		if (sx >= 640 && sy >= 480)
			SDL_Flip(XGR_ScreenSurface);
		else
			SDL_UpdateRect(XGR_ScreenSurface, x, y, sx, sy);

		
		SDL_UnlockSurface(XGR_ScreenSurface);

		if(mouseFlag){
			XGR_MouseObj.PutFon();
		}
		if(pmouseFlag){
			XGR_MouseObj.PutPromptFon();
		}
		//if (sx >= 640 && sy >= 480)
		//	XGR_MouseObj.Redraw();
	}*/
}

void XGR_Screen::fill(int col)
{
	assert(SDL_LockSurface(XGR_ScreenSurface) == 0);
	//int i;
	//unsigned char* ptr = ScreenBuf;
	
	memset(ScreenBuf, col, ScreenX*ScreenY);
	
	/*for(i = 0; i < ScreenY; i ++){
		ptr = ScreenBuf + yOffsTable[i];
		memset(ptr,col,ScreenX);
	}*/
	SDL_UnlockSurface(XGR_ScreenSurface);
}

void XGR_Screen::fill16(int col)
{
	int i,j;
	unsigned short* ptr;
	for(i = 0; i < ScreenY; i ++){
		ptr = (unsigned short*)(ScreenBuf + yOffsTable[i]);
		for(j = 0; j < ScreenX; j ++)
			ptr[j] = col;
	}
}

void XGR_Screen::getpal(void* p)
{
	int i;
	unsigned char* ptr = (unsigned char*)p;
	for(i = 0; i < 256; i++){
		*ptr ++ = XGR_Palette->colors[i].r >> 2;
		*ptr ++ = XGR_Palette->colors[i].g >> 2;
		*ptr ++ = XGR_Palette->colors[i].b >> 2;
	}
}

void XGR_Screen::setpal(void* ptr,int start,int count)
{
	int i;

	if (!ptr)
		return;

	if (count < 0){
		count = -count;
	}
	//std::cout<<"stepal start:"<<start<<" count:"<<count<<std::endl;
	// ptr is XGR_ColorData*
	XGR_ColorData* pal = (XGR_ColorData*)ptr;
	for (i = 0; i < count; i++) {
		XGR_Palette->colors[i].r = pal[i].R << 2;
		XGR_Palette->colors[i].g = pal[i].G << 2;
		XGR_Palette->colors[i].b = pal[i].B << 2;
	}
	SDL_SetPaletteColors(XGR_Palette, XGR_Palette->colors, start, count);
  	averageColorPalette.r = XGR_Palette->colors[220].r;
  	averageColorPalette.g = XGR_Palette->colors[220].g;
  	averageColorPalette.b = XGR_Palette->colors[220].b;
	/*if (averageColorPalette.r>averageColorPalette.b&&averageColorPalette.r>averageColorPalette.g) {
		averageColorPalette.r = 255;
		if (averageColorPalette.b+averageColorPalette.g>200) {
			averageColorPalette.b = averageColorPalette.g = 50;
		}
	}
	if (averageColorPalette.g>averageColorPalette.b&&averageColorPalette.g>averageColorPalette.r) {
		averageColorPalette.g = 255;
		if (averageColorPalette.b+averageColorPalette.r>200) {
			averageColorPalette.b = averageColorPalette.r = 50;
		}
	}
	if (averageColorPalette.b>averageColorPalette.r&&averageColorPalette.b>averageColorPalette.r) {
		averageColorPalette.b = 255;
		if (averageColorPalette.r+averageColorPalette.g>200) {
			averageColorPalette.r = averageColorPalette.g = 50;
		}
	}*/
	//if (start+count > 128) {
//  		unsigned int r = 0, g = 0, b = 0, c = 1;
// 		SDL_Color *colors = XGR_Palette->colors;
// 		for (i = 0; i < 128; i++) {
// 			//if (colors[i].r>200||colors[i].g>200||colors[i].b>200) {
// 				r += colors[i].r;
// 				g += colors[i].g;
// 				b += colors[i].b;
// 				c++;
// 			//}
// 		}
// 		averageColorPalette.r = r/c;
// 		averageColorPalette.g = g/c;
// 		averageColorPalette.b = b/c;
		
// 		averageColorPalette.r += c;
// 		averageColorPalette.g += c;
// 		averageColorPalette.b += c;
		//std::cout<<"Set averageColorPalette r:"<<(int)averageColorPalette.r<<" g:"<<(int)averageColorPalette.g<<" b:"<<(int)averageColorPalette.b<<std::endl;
	//}
}

void XGR_Screen::capture_screen(char* bmp_name,char* pal_name)
{
	int i;
	unsigned char palBuf[768];

	unsigned char* ptr = ScreenBuf;

	XStream fh(bmp_name,XS_OUT);
	fh < (short)ScreenX < (short)ScreenY;

	for(i = 0; i < ScreenY; i ++){
		fh.write(ptr,ScreenX);
		ptr = ScreenBuf + yOffsTable[i];
	}
	fh.close();

	getpal(palBuf);

	fh.open(pal_name,XS_OUT);
	fh.write(palBuf,768);
	fh.close();
}

void XGR_Screen::lineto(int x,int y,int len,int dir,int col)
{
	assert(SDL_LockSurface(XGR_ScreenSurface) == 0);
	int i,v,_x,_x1;
	unsigned char* ptr;

	if(ClipMode == XGR_CLIP_ALL){
		switch(dir){
			case XGR_LEFT:
				v = x - len;
				if(y < clipTop || y >= clipBottom || x >= clipRight) return;
				_x = (x < clipRight) ? x : clipRight;
				_x1 = (v > clipLeft) ? v : clipLeft;
				ptr = ScreenBuf + yOffsTable[y] + _x;
				for(i = _x; i > _x1; i --){
					*ptr = col;
					ptr --;
				}
				break;
			case XGR_TOP:
				v = y - len;
				if(x < clipLeft || x >= clipRight || y < clipTop) return;
				_x = (y < clipBottom) ? y : clipBottom - 1;
				_x1 = (v > clipTop) ? v : clipTop;
				ptr = ScreenBuf + yOffsTable[_x] + x;
				for(i = _x; i > _x1; i --){
					*ptr = col;
					ptr -= yStrOffs;
				}
				break;
			case XGR_RIGHT:
				v = x + len;
				if(y < clipTop || y >= clipBottom || x >= clipRight) return;
				_x = (x > clipLeft) ? x : clipLeft;
				_x1 = (v < clipRight) ? v : clipRight;
				ptr = ScreenBuf + yOffsTable[y] + _x;
				for(i = _x; i < _x1; i ++){
					*ptr = col;
					ptr ++;
				}
				break;
			case XGR_BOTTOM:
				v = y + len;
				if(x < clipLeft || x >= clipRight || y >= clipBottom) return;
				_x = (y > clipTop) ? y : clipTop;
				_x1 = (v < clipBottom) ? v : clipBottom;
				ptr = ScreenBuf + yOffsTable[_x] + x;
				for(i = _x; i < _x1; i ++){
					*ptr = col;
					ptr += yStrOffs;
				}
				break;
		}
	}
	else {
		ptr = ScreenBuf + yOffsTable[y] + x;
		switch(dir){
			case XGR_LEFT:
				v = x - len;
				for(i = x; i > v; i --){
					*ptr = col;
					ptr --;
				}
				break;
			case XGR_TOP:
				v = y - len;
				for(i = y; i > v; i --){
					*ptr = col;
					ptr -= yStrOffs;
				}
				break;
			case XGR_RIGHT:
				v = x + len;
				for(i = x; i < v; i ++){
					*ptr = col;
					ptr ++;
				}
				break;
			case XGR_BOTTOM:
				v = y + len;
				for(i = y; i < v; i ++){
					*ptr = col;
					ptr += yStrOffs;
				}
				break;
		}
	}
	SDL_UnlockSurface(XGR_ScreenSurface);
}

void XGR_Screen::lineto16(int x,int y,int len,int dir,int col)
{
	int i,v,_x,_x1,yoffs;
	unsigned short* ptr;

	yoffs = yStrOffs >> 1;

	if(ClipMode == XGR_CLIP_ALL){
		switch(dir){
			case XGR_LEFT:
				v = x - len;
				if(y < clipTop || y >= clipBottom || x >= clipRight) return;
				_x = (x < clipRight) ? x : clipRight;
				_x1 = (v > clipLeft) ? v : clipLeft;
				ptr = (unsigned short*)(ScreenBuf + yOffsTable[y] + (_x << 1));
				for(i = _x; i > _x1; i --){
					*ptr = col;
					ptr --;
				}
				break;
			case XGR_TOP:
				v = y - len;
				if(x < clipLeft || x >= clipRight || y < clipTop) return;
				_x = (y < clipBottom) ? y : clipBottom - 1;
				_x1 = (v > clipTop) ? v : clipTop;
				ptr = (unsigned short*)(ScreenBuf + yOffsTable[_x] + (x << 1));
				for(i = _x; i > _x1; i --){
					*ptr = col;
					ptr -= yoffs;
				}
				break;
			case XGR_RIGHT:
				v = x + len;
				if(y < clipTop || y >= clipBottom || x >= clipRight) return;
				_x = (x > clipLeft) ? x : clipLeft;
				_x1 = (v < clipRight) ? v : clipRight;
				ptr = (unsigned short*)(ScreenBuf + yOffsTable[y] + (_x << 1));
				for(i = _x; i < _x1; i ++){
					*ptr = col;
					ptr ++;
				}
				break;
			case XGR_BOTTOM:
				v = y + len;
				if(x < clipLeft || x >= clipRight || y >= clipBottom) return;
				_x = (y > clipTop) ? y : clipTop;
				_x1 = (v < clipBottom) ? v : clipBottom;
				ptr = (unsigned short*)(ScreenBuf + yOffsTable[_x] + (x << 1));
				for(i = _x; i < _x1; i ++){
					*ptr = col;
					ptr += yoffs;
				}
				break;
		}
	}
	else {
		ptr = (unsigned short*)(ScreenBuf + yOffsTable[y] + (x << 1));
		switch(dir){
			case XGR_LEFT:
				v = x - len;
				for(i = x; i > v; i --){
					*ptr = col;
					ptr --;
				}
				break;
			case XGR_TOP:
				v = y - len;
				for(i = y; i > v; i --){
					*ptr = col;
					ptr -= yoffs;
				}
				break;
			case XGR_RIGHT:
				v = x + len;
				for(i = x; i < v; i ++){
					*ptr = col;
					ptr ++;
				}
				break;
			case XGR_BOTTOM:
				v = y + len;
				for(i = y; i < v; i ++){
					*ptr = col;
					ptr += yoffs;
				}
				break;
		}
	}
}

void XGR_Screen::line(int x1,int y1,int x2,int y2,int col)
{
	assert(SDL_LockSurface(XGR_ScreenSurface) == 0);
	unsigned char* ptr;
	int dx,dy,xinc,yinc,err,cnt,i_xinc,i_yinc;

	dx = x2 - x1;
	dy = y2 - y1;

	xinc = (dx > 0) ? 1 : (dx < 0) ? (-1) : 0;
	yinc = (dy > 0) ? 1 : (dy < 0) ? (-1) : 0;

	i_xinc = xinc;
	i_yinc = (dy > 0) ? yStrOffs : (dy < 0) ? (-yStrOffs) : 0;

	dx = abs(dx);
	dy = abs(dy);

	ptr = ScreenBuf + yOffsTable[y1] + x1;
	if(ClipMode == XGR_CLIP_ALL){
		if(dx > dy){
			err = dx/2;
			cnt = dx + 1;
			while(cnt--){
				if(x1 >= clipLeft && x1 < clipRight && y1 >= clipTop && y1 < clipBottom)
					*(ScreenBuf + yOffsTable[y1] + x1) = col;

				x1 += xinc;

				err += dy;
				if(err >= dx){
					err -= dx;
					y1 += yinc;
					ptr += i_yinc;
				}
			}
		}
		else {
			err = dy / 2;
			cnt = dy + 1;
			while(cnt--){
				if(x1 >= clipLeft && x1 < clipRight && y1 >= clipTop && y1 < clipBottom)
					*(ScreenBuf + yOffsTable[y1] + x1) = col;

				y1 += yinc;

				err += dx;
				if(err >= dy){
					err -= dy;
					x1 += xinc;
					ptr += i_xinc;
				}
			}
		}
	}
	else {
		if(dx > dy){
			err = dx/2;
			cnt = dx + 1;
			while(cnt--){
				*ptr = col;
				x1 += xinc;
				ptr += i_xinc;

				err += dy;
				if(err >= dx){
					err -= dx;
					y1 += yinc;
					ptr += i_yinc;
				}
			}
		}
		else {
			err = dy / 2;
			cnt = dy + 1;
			while(cnt--){
				*ptr = col;
				y1 += yinc;
				ptr += i_yinc;
				err += dx;
				if(err >= dy){
					err -= dy;
					x1 += xinc;
					ptr += i_xinc;
				}
			}
		}
	}
	SDL_UnlockSurface(XGR_ScreenSurface);
}

void XGR_Screen::line16(int x1,int y1,int x2,int y2,int col)
{
	unsigned short* ptr;
	int dx,dy,xinc,yinc,err,cnt,i_xinc,i_yinc,yoffs;

	dx = x2 - x1;
	dy = y2 - y1;

	yoffs = yStrOffs >> 1;

	xinc = (dx > 0) ? 1 : (dx < 0) ? (-1) : 0;
	yinc = (dy > 0) ? 1 : (dy < 0) ? (-1) : 0;

	i_xinc = xinc;
	i_yinc = (dy > 0) ? yoffs : (dy < 0) ? (-yoffs) : 0;

	dx = abs(dx);
	dy = abs(dy);

	ptr = (unsigned short*)(ScreenBuf + yOffsTable[y1] + (x1 << 1));
	if(ClipMode == XGR_CLIP_ALL){
		if(dx > dy){
			err = dx/2;
			cnt = dx + 1;
			while(cnt--){
				if(x1 >= clipLeft && x1 < clipRight && y1 >= clipTop && y1 < clipBottom)
					*((unsigned short*)(ScreenBuf + yOffsTable[y1] + (x1 << 1))) = col;
				x1 += xinc;

				err += dy;
				if(err >= dx){
					err -= dx;
					y1 += yinc;
					ptr += i_yinc;
				}
			}
		}
		else {
			err = dy / 2;
			cnt = dy + 1;
			while(cnt--){
				if(x1 >= clipLeft && x1 < clipRight && y1 >= clipTop && y1 < clipBottom)
					*((unsigned short*)(ScreenBuf + yOffsTable[y1] + (x1 << 1))) = col;

				y1 += yinc;
				err += dx;
				if(err >= dy){
					err -= dy;
					x1 += xinc;
					ptr += i_xinc;
				}
			}
		}
	}
	else {
		if(dx > dy){
			err = dx/2;
			cnt = dx + 1;
			while(cnt--){
				*ptr = col;
				x1 += xinc;
				ptr += i_xinc;

				err += dy;
				if(err >= dx){
					err -= dx;
					y1 += yinc;
					ptr += i_yinc;
				}
			}
		}
		else {
			err = dy / 2;
			cnt = dy + 1;
			while(cnt--){
				*ptr = col;
				y1 += yinc;
				ptr += i_yinc;
				err += dx;
				if(err >= dy){
					err -= dy;
					x1 += xinc;
					ptr += i_xinc;
				}
			}
		}
	}
}

void XGR_Screen::rectangle(int x,int y,int sx,int sy,int outcol,int incol,int mode)
{
	if(!sx || !sy) return;

	lineto(x,y,sx,XGR_RIGHT,outcol);
	lineto(x,y,sy,XGR_BOTTOM,outcol);
	lineto(x + sx - 1,y,sy,XGR_BOTTOM,outcol);
	lineto(x,y + sy - 1,sx,XGR_RIGHT,outcol);

	if(mode == XGR_FILLED){
		if(sx < 3) return;
		erase(x + 1,y + 1,sx - 2,sy - 2,incol);
	}
}

void XGR_Screen::rectangle16(int x,int y,int sx,int sy,int outcol,int incol,int mode)
{
	if(!sx || !sy) return;

	lineto16(x,y,sx,XGR_RIGHT,outcol);
	lineto16(x,y,sy,XGR_BOTTOM,outcol);
	lineto16(x + sx - 1,y,sy,XGR_BOTTOM,outcol);
	lineto16(x,y + sy - 1,sx,XGR_RIGHT,outcol);

	if(mode == XGR_FILLED){
		if(sx < 3) return;
		erase16(x + 1,y + 1,sx - 2,sy - 2,incol);
	}
}


XGR_Mouse::XGR_Mouse(void)
{
	flags = XGM_AUTOCLIP;

	BackBuf = FrameBuf = NULL;
	CurFrame = 0;
	CurAlpha = 0;

	BegSeqFrame = EndSeqFrame = SeqDelta = 0;
	SeqMode = XGM_PLAY_ONCE;

	BegSeqAlpha = EndSeqAlpha = AlphaSeqDelta = 0;
	AlphaSeqMode = XGM_NONE;

	MovementX = MovementY = 0;

	SizeX = SizeY = 0;
	PosX = PosY = 0;
	PosZ = LastPosZ = MovementZ = 0;

	PromptColor = 255;
	PromptX = PromptY = 0;
	PromptDeltaX = PromptDeltaY = 0;
	PromptSizeX = PromptSizeY = 0;
	PromptFon = NULL;
	promptData = NULL;
	AlphaData = NULL;

}

void XGR_Mouse::Hide(void)
{
	if(flags & XGM_VISIBLE){
		flags &= ~XGM_VISIBLE;
		Redraw();
	}
	if(flags & XGM_PROMPT_ACTIVE){
		flags &= ~XGM_PROMPT_ACTIVE;
		XGR_Flush(PromptX,PromptY,PromptSizeX,PromptSizeY);
	}
}

void XGR_Mouse::Show(void)
{
	if(!(flags & XGM_VISIBLE)){
		flags |= XGM_VISIBLE;
		Redraw();
	}
}

#define XGR_MOUSE_DEFSIZE_X		12
#define XGR_MOUSE_DEFSIZE_Y		20
static unsigned char XGR_MouseDefFrame[240] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned short XGR_MouseDefFrameHC[240] =
{
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0xFF0, 0x000, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0xFF0, 0x000, 0x000, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0xFF0, 0x000, 0x000, 0x000, 0x000, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0xFF0, 0xFF0, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0xFF0, 0xFF0, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0xFF0, 0xFF0, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0xFF0, 0xFF0, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0xFF0, 0xFF0, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};

void XGR_Mouse::Init(int x,int y,int sx,int sy,int num,void* p)
{
	PosX = x;
	PosY = y;
	SizeX = sx;
	SizeY = sy;
	PosZ = LastPosZ = MovementZ = 0;

	OffsX = XGR_MouseOffsX;
	OffsY = XGR_MouseOffsY;

	if(XGR_Obj.flags & XGR_HICOLOR)
		flags |= XGM_HICOLOR;
	else
		flags &= ~XGM_HICOLOR;

	if(!p){
		SizeX = sx = XGR_MOUSE_DEFSIZE_X;
		SizeY = sy = XGR_MOUSE_DEFSIZE_Y;
		num = 1;
		p = XGR_MouseDefFrame;
		if(flags & XGM_HICOLOR)
			p = XGR_MouseDefFrameHC;
	}

	if(flags & XGM_AUTOCLIP){
		SetClipAuto();
	}

	SetPos(x,y);
	SetCursor(sx,sy,num,p);

	if(!XGR_SysMsgFlag){
		xtRegisterSysMsgFnc(XGR_MouseFnc,0);
		XGR_SysMsgFlag = 1;
	}

	flags |= (XGM_VISIBLE | XGM_INIT);
}

void XGR_Mouse::SetClipAuto(void)
{
	ClipCoords[XGR_LEFT] = ClipCoords[XGR_TOP] = 0;
//	ClipCoords[XGR_RIGHT] = XGR_MAXX - 2;
//	ClipCoords[XGR_BOTTOM] = XGR_MAXY - 2;

	ClipCoords[XGR_RIGHT] = XGR_MAXX - SizeX + 1;
	ClipCoords[XGR_BOTTOM] = XGR_MAXY - SizeY + 1;
}

void XGR_Mouse::SetCursor(int sx,int sy,int num,void* p)
{
	//std::cout<<"XGR_Mouse::SetCursor sx:"<<sx<<" sy:"<<sy<<std::endl;
	int dx,dy;
	if(BackBuf) {
		delete[] BackBuf;
	}

	if(flags & XGM_HICOLOR)
		BackBuf = new unsigned char[sx * sy * 2];
	else
		BackBuf = new unsigned char[sx * sy];

	FrameBuf = (unsigned char*)p;

	LastPosX = PosX;
	LastPosY = PosY;

	LastSizeX = SizeX;
	LastSizeY = SizeY;

	dx = (SizeX - sx)/2;
	dy = (SizeY - sy)/2;

	SizeX = sx;
	SizeY = sy;
	NumFrames = num;

	//PosX += dx;
	//PosY += dy;

	if(flags & XGM_AUTOCLIP){
		SetClipAuto();
	}

	CurFrame = 0;
	SeqMode = XGM_NONE;
	SetSpot(sx / 2,sy / 2);
	//AdjustPos();
//	SetCursorPos(PosX + OffsX,PosY + OffsY);
}

void XGR_Mouse::PutFrame(void)
{
//std::cout<<"PutFrame:"<<CurFrame<<std::endl;
	int cl,ct,cr,cb;
	if(PosX + SizeX < XGR_MAXX && PosY + SizeY < XGR_MAXY){
		if(flags & XGM_HICOLOR){
			if(AlphaData){
				XGR_PutSpr16ap(PosX,PosY,SizeX,SizeY,FrameBuf + SizeX * SizeY * CurFrame * 2,XGR_HIDDEN_FON | XGR_NOCLIP,AlphaData + SizeX * SizeY * CurFrame);
			}
			else {
				if(!CurAlpha)
					XGR_PutSpr16(PosX,PosY,SizeX,SizeY,FrameBuf + SizeX * SizeY * CurFrame * 2,XGR_HIDDEN_FON | XGR_NOCLIP);
				else
					XGR_PutSpr16a(PosX,PosY,SizeX,SizeY,FrameBuf + SizeX * SizeY * CurFrame * 2,XGR_HIDDEN_FON | XGR_NOCLIP,CurAlpha);
			}
		}
		else {
			XGR_PutSpr(PosX,PosY,SizeX,SizeY,FrameBuf + SizeX * SizeY * CurFrame,XGR_HIDDEN_FON | XGR_NOCLIP);
		}
	}
	else {
		XGR_GetClip(cl,ct,cr,cb);
		XGR_SetClip(0,0,XGR_MAXX,XGR_MAXY);

		if(flags & XGM_HICOLOR){
			if(AlphaData){
				XGR_PutSpr16ap(PosX,PosY,SizeX,SizeY,FrameBuf + SizeX * SizeY * CurFrame * 2,XGR_HIDDEN_FON | XGR_CLIPPED,AlphaData + SizeX * SizeY * CurFrame);
			}
			else {
				if(!CurAlpha)
					XGR_PutSpr16(PosX,PosY,SizeX,SizeY,FrameBuf + SizeX * SizeY * CurFrame * 2,XGR_HIDDEN_FON | XGR_CLIPPED);
				else
					XGR_PutSpr16a(PosX,PosY,SizeX,SizeY,FrameBuf + SizeX * SizeY * CurFrame * 2,XGR_HIDDEN_FON | XGR_CLIPPED,CurAlpha);
			}
		}
		else
			XGR_PutSpr(PosX,PosY,SizeX,SizeY,FrameBuf + SizeX * SizeY * CurFrame,XGR_HIDDEN_FON | XGR_CLIPPED);

		XGR_SetClip(cl,ct,cr,cb);
	}
}

void XGR_Mouse::PutFon(void)
{
//std::cout<<"XGR_Mouse::PutFon"<<std::endl;
	int sx,sy;
	if(PosX + SizeX < XGR_MAXX && PosY + SizeY < XGR_MAXY){
		if(flags & XGM_HICOLOR)
			XGR_PutSpr16(PosX,PosY,SizeX,SizeY,BackBuf,XGR_BLACK_FON | XGR_NOCLIP);
		else
			XGR_PutSpr(PosX,PosY,SizeX,SizeY,BackBuf,XGR_BLACK_FON | XGR_NOCLIP);
	} else {
		sx = XGR_MAXX - PosX;
		sy = XGR_MAXY - PosY;

		//std::cout<<"XGR_Mouse::PutFon sx:"<<sx<<" sy:"<<sy<<" SizeX:"<<SizeX<<" SizeY:"<<SizeY<<" PosY:"<<PosY<<std::endl;
		if(sx > SizeX) sx = SizeX;
		if(sy > SizeY) sy = SizeY;
		
		if(flags & XGM_HICOLOR)
			XGR_PutSpr16(PosX,PosY,sx,sy,BackBuf,XGR_BLACK_FON | XGR_NOCLIP);
		else
			XGR_PutSpr(PosX,PosY,sx,sy,BackBuf,XGR_BLACK_FON | XGR_NOCLIP);
	}
}

void XGR_Mouse::GetFon(void)
{
//std::cout<<"XGR_Mouse::GetFon"<<std::endl;
	int sx,sy;
	if(PosX + SizeX < XGR_MAXX && PosY + SizeY < XGR_MAXY){
		if(flags & XGM_HICOLOR)
			XGR_GetSpr16(PosX,PosY,SizeX,SizeY,BackBuf);
		else
			XGR_GetSpr(PosX,PosY,SizeX,SizeY,BackBuf);
	}
	else {
		sx = XGR_MAXX - PosX;
		sy = XGR_MAXY - PosY;

		if(sx > SizeX) sx = SizeX;
		if(sy > SizeY) sy = SizeY;

		if(flags & XGM_HICOLOR)
			XGR_GetSpr16(PosX,PosY,sx,sy,BackBuf);
		else
			XGR_GetSpr(PosX,PosY,sx,sy,BackBuf);
	}
}

void XGR_Mouse::InitPos(int x,int y)
{
	//std::cout<<"XGR_Mouse::InitPos x:"<<x<<" y:"<<y<<std::endl;
	LastPosX = PosX;
	LastPosY = PosY;

	LastSizeX = SizeX;
	LastSizeY = SizeY;

	PosX = x;
	PosY = y;

	AdjustPos();
}

void XGR_Mouse::AdjustPos(void)
{
	/*if(PosX < ClipCoords[XGR_LEFT]) PosX = ClipCoords[XGR_LEFT];
	if(PosX >= ClipCoords[XGR_RIGHT]) PosX = ClipCoords[XGR_RIGHT] - 1;

	if(PosY < ClipCoords[XGR_TOP]) PosY = ClipCoords[XGR_TOP];
	if(PosY >= ClipCoords[XGR_BOTTOM]) PosY = ClipCoords[XGR_BOTTOM] - 1;*/
	
	if(PosX < 0)
		PosX = 0;
	if(PosX >= XGR_MAXX + SizeX)
		PosX = XGR_MAXX + SizeX - 1;
	
	if(PosY < 0)
		PosY = 0;
	if(PosY >= XGR_MAXY + SizeY)
		PosY = XGR_MAXY + SizeY - 1;
}

void XGR_Mouse::SetPos(int x,int y)
{
	//std::cout<<"XGR_Mouse::SetPos sx:"<<x<<" sy:"<<y<<std::endl;
//	static POINT pt;

	LastPosX = PosX;
	LastPosY = PosY;

	LastSizeX = SizeX;
	LastSizeY = SizeY;

	PosX = x;
	PosY = y;

	//AdjustPos();

//	pt.x = PosX;
//	pt.y = PosY;

//	ClientToScreen((HWND)XGR_hWnd,&pt);
/*
	int ret = ClientToScreen((HWND)XGR_hWnd,&pt);
	int id;
	if(!ret){
		id = GetLastError();

		char msg[256];
		memset(msg,0,256);
		ret = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			id,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			msg,
			256,
			NULL
		);
	}
*/
//	SetCursorPos(pt.x,pt.y);
}

void XGR_Mouse::Redraw(void)
{
	int x,y,x1,y1,sx,sy,sx1,sy1;
	
	if(XGR_InitFlag){
		sx = PosX + SizeX;
		sy = PosY + SizeY;

		sx1 = LastPosX + LastSizeX;
		sy1 = LastPosY + LastSizeY;

		x = (PosX < LastPosX) ? PosX : LastPosX;
		y = (PosY < LastPosY) ? PosY : LastPosY;

		x1 = (sx > sx1) ? sx : sx1;
		y1 = (sy > sy1) ? sy : sy1;

		if(x1 >= XGR_MAXX) x1 = XGR_MAXX;
		if(y1 >= XGR_MAXY) y1 = XGR_MAXY;

		XGR_Flush(x,y,x1 - x,y1 - y);
		//std::cout<<"XGR_Mouse::Redraw "<<x<<" "<<x1<<" "<<y<<" "<<y1<<std::endl;
		if(flags & XGM_PROMPT_ACTIVE)
			XGR_Flush(PromptX,PromptY,PromptSizeX,PromptSizeY);
	}
}

int XGR_Mouse::CheckRedraw(int x,int y,int sx,int sy)
{
	int x1,y1,dx,dy;

	if(!Visible()) return 0;

	x += sx/2;
	y += sy/2;

	x1 = PosX + SizeX/2;
	y1 = PosY + SizeY/2;

	dx = (sx + SizeX)/2;
	dy = (sy + SizeY)/2;

	if(abs(x - x1) <= dx && abs(y - y1) <= dy) return 1;
	return 0;
}

int XGR_Mouse::CheckPromptRedraw(int x,int y,int sx,int sy)
{
	int x1,y1,dx,dy;

	if(!(flags & XGM_PROMPT_ACTIVE) || !Visible() || (flags & XGM_DISABLE_PROMPT) || !promptData) return 0;

	x += sx/2;
	y += sy/2;

	x1 = PromptX + PromptSizeX/2;
	y1 = PromptY + PromptSizeY/2;

	dx = (sx + PromptSizeX)/2;
	dy = (sy + PromptSizeY)/2;

	if(abs(x - x1) <= dx && abs(y - y1) <= dy) return 1;
	return 0;
}

void XGR_Mouse::SetSeq(int beg,int end,int mode,int delta)
{
	BegSeqFrame = beg;
	EndSeqFrame = end;

	SeqMode = mode;
	SeqDelta = delta;
}

void XGR_Mouse::SetAlphaSeq(int beg,int end,int mode,int delta)
{
	BegSeqAlpha = beg;
	EndSeqAlpha = end;

	AlphaSeqMode = mode;
	AlphaSeqDelta = delta;
}

int XGR_Mouse::NextFrame(void)
{
//std::cout<<"XGR_Mouse::NextFrame "<<CurFrame<<" "<<SeqDelta<<std::endl;
	int ret = 0;
	if(SeqMode != XGM_NONE){
		CurFrame += SeqDelta;
		if(CurFrame < 0) CurFrame = NumFrames - 1;
		if(CurFrame >= NumFrames) CurFrame = 0;

		if(SeqMode == XGM_PLAY_ONCE && CurFrame == EndSeqFrame) SeqMode = XGM_NONE;

		ret = 1;
	}
	if(!AlphaData && AlphaSeqMode != XGM_NONE){
		CurAlpha += AlphaSeqDelta;
		if(CurAlpha < 0) CurAlpha = 256;
		if(CurAlpha > 256) CurAlpha = 0;

		if(AlphaSeqMode == XGM_PLAY_ONCE && CurAlpha == EndSeqAlpha) AlphaSeqMode = XGM_NONE;

		ret = 1;
	}
	return ret;
}

int XGR_Mouse::NextPromptFrame(void)
{
	if(!(flags & XGM_DISABLE_PROMPT)){
		if(Visible() && promptData && !(flags & XGM_PROMPT_ACTIVE)){
			promptData -> Timer ++;
			if(promptData -> Timer >= promptData -> ActivateTimer){
				InitPrompt();
				return 1;
			}
		}
	}
	return 0;
}



void XGR_Mouse::Press(int bt,int fl,int x,int y)
{
	switch(bt){
		case XGM_LEFT_BUTTON:
			if(lBt.Press) (*lBt.Press)(fl,x + SpotX,y + SpotY);
			lBt.Pressed = 1;
			break;
		case XGM_RIGHT_BUTTON:
			if(rBt.Press) (*rBt.Press)(fl,x + SpotX,y + SpotY);
			rBt.Pressed = 1;
			break;
		case XGM_MIDDLE_BUTTON:
			if(mBt.Press) (*mBt.Press)(fl,x + SpotX,y + SpotY);
			mBt.Pressed = 1;
			break;
	}
}

void XGR_Mouse::UnPress(int bt,int fl,int x,int y)
{
	switch(bt){
		case XGM_LEFT_BUTTON:
			if(lBt.UnPress) (*lBt.UnPress)(fl,x + SpotX,y + SpotY);
			lBt.Pressed = 0;
			break;
		case XGM_RIGHT_BUTTON:
			if(rBt.UnPress) (*rBt.UnPress)(fl,x + SpotX,y + SpotY);
			rBt.Pressed = 0;
			break;
		case XGM_MIDDLE_BUTTON:
			if(mBt.UnPress) (*mBt.UnPress)(fl,x + SpotX,y + SpotY);
			mBt.Pressed = 0;
			break;
	}
}

void XGR_Mouse::DblClick(int bt,int fl,int x,int y)
{
	switch(bt){
		case XGM_LEFT_BUTTON:
			if(lBt.DblClick) (*lBt.DblClick)(fl,x + SpotX,y + SpotY);
			break;
		case XGM_RIGHT_BUTTON:
			if(rBt.DblClick) (*rBt.DblClick)(fl,x + SpotX,y + SpotY);
			break;
		case XGM_MIDDLE_BUTTON:
			if(mBt.DblClick) (*mBt.DblClick)(fl,x + SpotX,y + SpotY);
			break;
	}
}

void XGR_Mouse::SetPressHandler(int bt,XGR_MOUSE_HANDLER p)
{
	switch(bt){
		case XGM_LEFT_BUTTON:
			lBt.Press = p;
			break;
		case XGM_RIGHT_BUTTON:
			rBt.Press = p;
			break;
		case XGM_MIDDLE_BUTTON:
			mBt.Press = p;
			break;
	}
}

void XGR_Mouse::SetUnPressHandler(int bt,XGR_MOUSE_HANDLER p)
{
	switch(bt){
		case XGM_LEFT_BUTTON:
			lBt.UnPress = p;
			break;
		case XGM_RIGHT_BUTTON:
			rBt.UnPress = p;
			break;
		case XGM_MIDDLE_BUTTON:
			mBt.UnPress = p;
			break;
	}
}

void XGR_Mouse::SetDblHandler(int bt,XGR_MOUSE_HANDLER p)
{
	switch(bt){
		case XGM_LEFT_BUTTON:
			lBt.DblClick = p;
			break;
		case XGM_RIGHT_BUTTON:
			rBt.DblClick = p;
			break;
		case XGM_MIDDLE_BUTTON:
			mBt.DblClick = p;
			break;
	}
}

void XGR_Mouse::Move(int fl,int x,int y)
{
	
	if(flags & XGM_PROMPT_ACTIVE){
		flags &= ~XGM_PROMPT_ACTIVE;
		XGR_Flush(PromptX,PromptY,PromptSizeX,PromptSizeY);
	}
	if(promptData)
		promptData -> Timer = 0;

	if(MoveH) (*MoveH)(fl,x + SpotX,y + SpotY);
}

XGR_MousePromptData::XGR_MousePromptData(int x,int y,int sx,int sy,int fnt)
{
	ID = 0;
	StartX = x;
	StartY = y;

	SizeX = sx;
	SizeY = sy;

	font = fnt;

	textData = NULL;
}

XGR_MousePromptData::XGR_MousePromptData(void)
{
	ID = 0;
	StartX = StartY = 0;
	SizeX = SizeY = 0;
	textSizeX = textSizeY = 0;
	flags = 0;

	font = XGR_FONT0_8x16;

	textData = NULL;
}

XGR_MousePromptData::~XGR_MousePromptData(void)
{
	if(textData && flags & XGR_PROMPT_MEM_ALLOC) delete textData;
}

void XGR_MousePromptData::init_text(char* p)
{
	int sz = strlen(p) + 1;
	textData = new char[sz];
	strcpy(textData,p);

	flags |= XGR_PROMPT_MEM_ALLOC;
	init();
}

void XGR_MousePromptData::set_text(char* p)
{
	textData = p;
	flags &= ~XGR_PROMPT_MEM_ALLOC;

	init();
}

void XGR_MousePromptData::init(void)
{
	if(!textData) return;

	if(XGR_TextXFnc)
		textSizeX = (*XGR_TextXFnc)(textData,font,1);
	else
		textSizeX = XGR_TextWidth(textData,font);

	if(XGR_TextYFnc)
		textSizeY = (*XGR_TextYFnc)(textData,font,1);
	else
		textSizeY = XGR_TextHeight(textData,font);
}

int XGR_MousePromptData::check_xy(int x,int y)
{
	if(x >= StartX && x < (StartX + SizeX) && y >= StartY && y < (StartY + SizeY)) return 1;
	return 0;
}

XGR_MousePromptScreen::XGR_MousePromptScreen(void)
{
	Timer = 0;
	ActivateTimer = 5;

	curFont = 0;
	curText = NULL;
	ClearList();
}

XGR_MousePromptScreen::~XGR_MousePromptScreen(void)
{
	XGR_MousePromptData* p,*p1;

	p = (XGR_MousePromptData*)fPtr;
	while(p){
		p1 = (XGR_MousePromptData*)p -> next;
		delete p;
		p = p1;
	}
	ClearList();
}

XGR_MousePromptData* XGR_MousePromptScreen::check_xy(int x,int y)
{
	XGR_MousePromptData* p;

	p = (XGR_MousePromptData*)fPtr;
	while(p){
		if(p -> check_xy(x,y)) return p;
		p = (XGR_MousePromptData*)p -> next;
	}
	return NULL;
}

void XGR_Mouse::InitPrompt(void)
{
	int x,y,x0,y0,sx,sy;
	XGR_MousePromptData* p;

	if(!promptData) return;

	x = PosX + SpotX + PromptDeltaX;
	y = PosY + SpotY + PromptDeltaY;

	p = promptData -> check_xy(x,y);
	if(!p) return;

	sx = SizeX/2;
	sy = SizeY/2;

	x0 = PosX + sx;
	y0 = PosY + sy;

	if((x0 + sx + p -> textSizeX) < XGR_MAXX){
		PromptX = x0 + sx;
	}
	else {
		if((x0 - sx - p -> textSizeX) >= 0){
			PromptX = x0 - sx - p -> textSizeX;
		}
		else
			return;
	}
	if((y0 + sy + p -> textSizeY) < XGR_MAXY){
		PromptY = y0 + sy;
	}
	else {
		if((y0 - sy - p -> textSizeY) >= 0){
			PromptY = y0 - sy - p -> textSizeY;
		}
		else
			return;
	}
	if(flags & XGM_HICOLOR){
		if(PromptFonBufSize < p -> textSizeX * p -> textSizeY * 2){
			if(PromptFon) delete PromptFon;
			PromptFonBufSize = p -> textSizeX * p -> textSizeY * 2;
			PromptFon = new char[PromptFonBufSize];
		}
	}
	else {
		if(PromptFonBufSize < p -> textSizeX * p -> textSizeY){
			if(PromptFon) delete PromptFon;
			PromptFonBufSize = p -> textSizeX * p -> textSizeY;
			PromptFon = new char[PromptFonBufSize];
		}
	}

	promptData -> curText = p -> textData;
	promptData -> curFont = p -> font;

	PromptSizeX = p -> textSizeX;
	PromptSizeY = p -> textSizeY;

	flags |= XGM_PROMPT_ACTIVE;
}

void XGR_Mouse::PutPrompt(void)
{
	if(promptData && promptData -> curText){
		if(XGR_TextOutFnc)
			(*(XGR_TextOutFnc))(PromptX,PromptY,PromptColor,promptData -> curText,promptData -> curFont,1,1);
		else
			XGR_OutText(PromptX,PromptY,PromptColor,promptData -> curText,promptData -> curFont);
	}
}

void XGR_Mouse::GetPromptFon(void)
{
	if(flags & XGM_HICOLOR)
		XGR_GetSpr16(PromptX,PromptY,PromptSizeX,PromptSizeY,PromptFon);
	else
		XGR_GetSpr(PromptX,PromptY,PromptSizeX,PromptSizeY,PromptFon);
}

void XGR_Mouse::PutPromptFon(void)
{
	if(flags & XGM_HICOLOR)
		XGR_PutSpr16(PromptX,PromptY,PromptSizeX,PromptSizeY,PromptFon,XGR_BLACK_FON | XGR_NOCLIP);
	else
		XGR_PutSpr(PromptX,PromptY,PromptSizeX,PromptSizeY,PromptFon,XGR_BLACK_FON | XGR_NOCLIP);
}

XGR_MousePromptData* XGR_MousePromptScreen::getData(int id)
{
	XGR_MousePromptData* p = (XGR_MousePromptData*)fPtr;
	while(p){
		if(p -> ID == id) return p;
		p = (XGR_MousePromptData*)p -> next;
	}
	return NULL;
}

void XGR_PrepareSprite(int sx,int sy,void* dst_buf,void* src_buf,XGR_Pal64K* pal)
{
	int i,j,index = 0;
	unsigned char* src = (unsigned char*)src_buf;
	unsigned short* dst = (unsigned short*)dst_buf;

	for(i = 0; i < sy; i ++){
		for(j = 0; j < sx; j ++){
			dst[index] = pal -> data[src[index]];
			index ++;
		}
	}
}

void XGR_SetTextOutFnc(XGR_OutTextHandler p)
{
	XGR_TextOutFnc = p;
}

void XGR_SetTextXFnc(XGR_TextWidthHandler p)
{
	XGR_TextXFnc = p;
}

void XGR_SetTextYFnc(XGR_TextHeightHandler p)
{
	XGR_TextYFnc = p;
}

/*void XGR_Pal64K::prepare(void* p)
{
	int i,R,G,B;
	unsigned char* ptr = (unsigned char*)p;

	for(i = 0; i < 256; i ++){
		if(XGR_MASK_R == XGR_MASK_R0){
			R = ptr[i * 3] >> 1;
			G = ptr[i * 3 + 1];
			B = ptr[i * 3 + 2] >> 1;
		}
		else {
			R = ptr[i * 3] >> 1;
			G = ptr[i * 3 + 1] >> 1;
			B = ptr[i * 3 + 2] >> 1;
		}
		data[i] = XGR_RGB64K(R,G,B);
	}
}*/

void XGR_MouseFnc(SDL_Event* p)
{
	int x,y,x1,y1,rec_flag = 0;
	//ErrH.Log("Mouse Event\n");

	// Mouse motion
	if (p->type == SDL_MOUSEMOTION) {
		if (p->motion.which==SDL_TOUCH_MOUSEID) {
			return;
		}
		//std::cout<<"x:"<<p->motion.x<<" y:"<<p->motion.y<<std::endl;
		x = p->motion.x;
		y = p->motion.y;

		x1 = XGR_MouseObj.PosX;
		y1 = XGR_MouseObj.PosY;

		XGR_MouseObj.InitPos(x,y);

		XGR_MouseObj.MovementX = x - x1;
		XGR_MouseObj.MovementY = y - y1;

		// TODO(amdmi3): first arg is button state, not used actually
		XGR_MouseObj.Move(0, XGR_MouseObj.PosX, XGR_MouseObj.PosY);
		//if(XGR_MouseVisible())
		//	XGR_MouseRedraw();
		rec_flag = 1;
		return;
	} else if (p->type == SDL_MOUSEWHEEL ) {
		XGR_MouseObj.LastPosZ = XGR_MouseObj.PosZ;
		// TODO (amdmi3): mouse wheel may be reversed; change 1 <-> -1 if so
		XGR_MouseObj.PosZ += XGR_MouseObj.MovementZ = (p->wheel.y > 0) ? 1 : -1;
		rec_flag = 1;
	} else if (p->type == SDL_MOUSEBUTTONDOWN || p->type == SDL_MOUSEBUTTONUP) {
		int flag = 0;
		switch (p->button.button) {
		case SDL_BUTTON_LEFT: flag = XGM_LEFT_BUTTON; break;
		case SDL_BUTTON_MIDDLE: flag = XGM_MIDDLE_BUTTON; break;
		case SDL_BUTTON_RIGHT: flag = XGM_RIGHT_BUTTON; break;
		}

		// TODO(amdmi3): secound arg is button state; needed?
		if (p->type == SDL_MOUSEBUTTONUP)
			XGR_MouseUnPress(flag, 0, XGR_MouseObj.PosX, XGR_MouseObj.PosY);
		else
			XGR_MousePress(flag, 0, XGR_MouseObj.PosX, XGR_MouseObj.PosY);
		rec_flag = 1;
	}
	if(rec_flag && XRec.flags & XRC_RECORD_MODE){
		//XRec.PutSysMessage(XRC_XMOUSE_MESSAGE,p -> message,p -> wParam,p -> lParam);
	}
}
