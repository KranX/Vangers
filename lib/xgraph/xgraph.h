/*
*******************************************************************************
**									     **
**	      Low-level debug-graphics library	(Win32 compatible)	     **
**			    (C) 1997 K-D Lab::Steeler			     **
**				  Version 1.00				     **
**									     **
*******************************************************************************
*/

#ifndef __XGRAPH_H__
#define __XGRAPH_H__

#include "xglobal.h"

#ifdef WITH_OPENGL
#include <GL/gl.h>
#include <GL/glu.h>
#endif
// Some defines for 64K modes...
#define XGR_RGB64K(r,g,b)	(((r) << XGR_SHIFT_R) + ((g) << XGR_SHIFT_G) + ((b) << XGR_SHIFT_B))
#define XGR_64KR(c)		(((c) >> XGR_SHIFT_R) & XGR_COLOR_MASK_R)
#define XGR_64KG(c)		(((c) >> XGR_SHIFT_G) & XGR_COLOR_MASK_G)
#define XGR_64KB(c)		(((c) >> XGR_SHIFT_B) & XGR_COLOR_MASK_B)

// Directions for XGR_LineTo()
enum XGR_LINEDIR
{
	XGR_LEFT,
	XGR_TOP,
	XGR_RIGHT,
	XGR_BOTTOM
};

// Modes for XGR_PutSpr()
#define XGR_HIDDEN_FON		0x01
#define XGR_BLACK_FON		0x02
#define XGR_CLIPPED		0x04
#define XGR_NOCLIP		0x08
#define XGR_BOTTOM_UP		0x10

// Modes for XGR_Rectangle()
#define XGR_FILLED		0x00
#define XGR_OUTLINED		0x01

#define XGR_NUM_FONTS		0x02

typedef void (*XGR_OutTextHandler)(int x,int y,int col,void* text,int font,int hspace,int vspace);
typedef int  (*XGR_TextWidthHandler)(void* text,int font,int hspace);
typedef int  (*XGR_TextHeightHandler)(void* text,int font,int vspace);

// XGR_Font IDs...
#define XGR_FONT0_8x16		0
#define XGR_FONT0_8x8		1

struct XGR_Font
{
	int SizeX;
	int SizeY;

	unsigned char* LeftOffs;
	unsigned char* RightOffs;

	unsigned char* data;

	void init(int size,void* p);

	XGR_Font(void);
};

// XGR_Screen::flags...
#define XGR_EXCLUSIVE		0x01
#define XGR_REINIT		0x02
#define XGR_PALETTE		0x04
#define XGR_INIT		0x08
#define XGR_HICOLOR		0x10

// XGR_Screen::ClipMode values...
#define XGR_CLIP_PUTSPR 	0x00
#define XGR_CLIP_ALL		0x01

struct XGR_Pal64K {
	int ID;

	unsigned* data;
	void prepare(void* p);

	unsigned operator[](int ind) const {
		return data[ind];
	}
	unsigned& operator[](int ind) {
		return data[ind];
	}

	XGR_Pal64K(void) {
		ID = 0;
		data = new unsigned[256];
	}
	~XGR_Pal64K(void) {
		delete[] data;
	}
};

struct XGR_Screen
{
	int flags;

	int ScreenX;
	int ScreenY;
	int RealX;
	int RealY;

	unsigned char* ScreenBuf;

	SDL_Surface *XGR_ScreenSurface;
	//SDL_Surface *XGR_ScreenSurface2D;
	SDL_Surface *XGR32_ScreenSurface;
	//SDL_Surface *XGR32_ScreenSurface2D;
	SDL_Surface *HDBackgroundSurface;
	SDL_Surface *IconSurface;
	SDL_Texture *sdlTexture;
	//SDL_Texture *sdlTexture2D;
	SDL_Texture *HDBackgroundTexture;
	SDL_Window *sdlWindow;
	SDL_Renderer *sdlRenderer;
	
	//SDL_Color   XGR_Palette[256];
	SDL_Palette *XGR_Palette;
	SDL_Color averageColorPalette = {255,255,255,0};

	int ClipMode;

	int clipLeft;
	int clipTop;
	int clipRight;
	int clipBottom;

	int yStrOffs;
	int* yOffsTable;

#ifdef WITH_OPENGL
	SDL_Surface *XGR_ScreenSurface_Real;
	GLuint SurfToTexture(SDL_Surface *surf);
#endif	
	void set_pitch(int p);
	void set_clip(int left,int top,int right,int bottom);
	void get_clip(int& left,int& top,int& right,int& bottom);
	void set_clip_mode(int mode){ ClipMode = mode; }

	void set_fullscreen(bool fullscreen); 
	
	void setpixel(int x,int y,int col);
	int getpixel(int x,int y);

	void flush(int x,int y,int sx,int sy);
	void flip();

	void fill(int col);
	void erase(int x,int y,int sx,int sy,int col);

	void rectangle(int x,int y,int sx,int sy,int outcol,int incol,int mode);

	void line(int x1,int y1,int x2,int y2,int col);
	void lineto(int x,int y,int len,int dir,int col);

	int init(int x,int y,int flags);
	void close(void);
	void finit(void);

	void putspr(int x,int y,int sx,int sy,void* p,int mode);

	void getspr(int x,int y,int sx,int sy,void* p);

	void getpal(void* p);
	void setpal(void* pal,int start,int count);

	void capture_screen(char* bmp_name,char* pal_name);

	// HiColor functions...
	void putspr16(int x,int y,int sx,int sy,void* p,int mode);
	void putspr16a(int x,int y,int sx,int sy,void* p,int mode,int alpha);
	void putspr16ap(int x,int y,int sx,int sy,void* p,int mode,void* alpha_ptr);
	void getspr16(int x,int y,int sx,int sy,void* p);

	void erase16(int x,int y,int sx,int sy,int col);
	void fill16(int col);

	void setpixel16(int x,int y,int col);

	void line16(int x1,int y1,int x2,int y2,int col);
	void lineto16(int x,int y,int len,int dir,int col);

	void rectangle16(int x,int y,int sx,int sy,int outcol,int incol,int mode);
	
	void blitScreen(uint32_t *dst, uint8_t *src);

	void set_render_buffer(SDL_Surface *buf);
	
	XGR_Screen(void);
};

// XGR_MousePromptData::flags...
#define XGR_PROMPT_ACTIVE	0x01
#define XGR_PROMPT_MEM_ALLOC	0x02

struct XGR_MousePromptData : XListElement
{
	int ID;

	int StartX;
	int StartY;

	int SizeX;
	int SizeY;

	int font;

	int flags;

	int textSizeX;
	int textSizeY;
	char* textData;

	void init(void);
	void init_text(char* p);
	void set_text(char* p);

	int check_xy(int x,int y);

	XGR_MousePromptData(void);
	XGR_MousePromptData(int x,int y,int sx,int sy,int fnt);
	~XGR_MousePromptData(void);
};

struct XGR_MousePromptScreen : XList
{
	int Timer;
	int ActivateTimer;

	int curFont;
	char* curText;

	XGR_MousePromptData* check_xy(int x,int y);
	XGR_MousePromptData* getData(int id);

	XGR_MousePromptScreen(void);
	~XGR_MousePromptScreen(void);
};

typedef void (*XGR_MOUSE_HANDLER)(int, int, int);

struct XGR_MouseButton
{
	XGR_MOUSE_HANDLER Press;
	XGR_MOUSE_HANDLER UnPress;
	XGR_MOUSE_HANDLER DblClick;
	unsigned int Pressed;

	void SetPress(XGR_MOUSE_HANDLER p){ Press = p; }
	void SetUnPress(XGR_MOUSE_HANDLER p){ UnPress = p; }
	void SetDblClick(XGR_MOUSE_HANDLER p){ DblClick = p; }

	XGR_MouseButton(void){ Press = UnPress = DblClick = NULL; Pressed = 0; }
};

#define XGM_LEFT_BUTTON 	0x01
#define XGM_RIGHT_BUTTON	0x02
#define XGM_MIDDLE_BUTTON	0x03

// XGR_Mouse::flags...
#define XGM_INIT		0x01
#define XGM_VISIBLE		0x02
#define XGM_AUTOCLIP		0x04
#define XGM_PROMPT_ACTIVE	0x08
#define XGM_HICOLOR		0x10
#define XGM_DISABLE_PROMPT	0x20

// XGR_Mouse::SeqMode values...
#define XGM_NONE		0
#define XGM_PLAY_ONCE		1
#define XGM_PLAY_INFINITE	2
#define XGM_CURSOR_TIMER	1333

struct XGR_Mouse
{
	int flags;

	int ClipCoords[4];

	int SizeX;
	int SizeY;

	int MovementX; //Some comment
	int MovementY;
	int MovementZ;

	int NumFrames;
	unsigned char* FrameBuf;
	unsigned char* BackBuf;

	int CurFrame;

	int BegSeqFrame;
	int EndSeqFrame;
	int SeqDelta;
	int SeqMode;

	int CurAlpha;
	unsigned char* AlphaData;

	int BegSeqAlpha;
	int EndSeqAlpha;
	int AlphaSeqDelta;
	int AlphaSeqMode;

	int PosX;
	int PosY;
	int PosZ; // Wheel

	int OffsX;
	int OffsY;

	int LastPosX;
	int LastPosY;
	int LastPosZ;
	int LastSizeX;
	int LastSizeY;

	int SpotX;
	int SpotY;

	XGR_MouseButton lBt;
	XGR_MouseButton rBt;
	XGR_MouseButton mBt;

	XGR_MOUSE_HANDLER MoveH;

	int PromptDeltaX;
	int PromptDeltaY;

	int PromptX;
	int PromptY;

	int PromptColor;

	int PromptSizeX;
	int PromptSizeY;

	int PromptFonBufSize;
	char* PromptFon;

	XGR_MousePromptScreen* promptData;

	void Init(int x,int y,int sx,int sy,int num,void* p);

	void SetClipAuto(void);
	void SetClip(int l,int t,int r,int b){
		ClipCoords[XGR_LEFT] = l; ClipCoords[XGR_TOP] = t;
		ClipCoords[XGR_RIGHT] = r; ClipCoords[XGR_BOTTOM] = b;
	}
	void SetCursor(int sx,int sy,int num,void* p);
	void SetSpot(int x,int y){ SpotX = x; SpotY = y; }
	void SetSeq(int beg,int end,int mode,int delta);
	void SetAlphaSeq(int beg,int end,int mode,int delta);

	void InitPos(int x,int y);
	void SetPos(int x,int y);

	void Hide(void);
	void Show(void);

	void Redraw(void);
	void PutFrame(void);
	void PutFon(void);
	void GetFon(void);
	int NextFrame(void);
	int NextPromptFrame(void);

	void InitPrompt(void);
	void RedrawPrompt(void);
	void PutPrompt(void);
	void PutPromptFon(void);
	void GetPromptFon(void);
	void DisablePrompt(void){ flags |= XGM_DISABLE_PROMPT; }
	void EnablePrompt(void){ flags &= ~XGM_DISABLE_PROMPT; }

	void SetPromptData(XGR_MousePromptScreen* p){ promptData = p; }

	void AdjustPos(void);

	int Visible(void){ return (flags & XGM_VISIBLE); }
	int CheckRedraw(int x,int y,int sx,int sy);
	int CheckPromptRedraw(int x,int y,int sx,int sy);

	void SetMoveHandler(XGR_MOUSE_HANDLER p){ MoveH = p; }

	void SetPressHandler(int bt,XGR_MOUSE_HANDLER p);
	void SetUnPressHandler(int bt,XGR_MOUSE_HANDLER p);
	void SetDblHandler(int bt,XGR_MOUSE_HANDLER p);

	void Move(int fl,int x,int y);

	void Press(int bt,int fl,int x,int y);
	void UnPress(int bt,int fl,int x,int y);
	void DblClick(int bt,int fl,int x,int y);

	XGR_Mouse(void);
};

#define XGR_Init(x,y,fl)				XGR_Obj.init(x,y,fl)
#define XGR_ReInit(x,y,fl)				XGR_Obj.init(x,y,fl | XGR_REINIT)
#define XGR_Finit()					XGR_Obj.finit()
#define XGR_SetClip(left,top,right,bottom)		XGR_Obj.set_clip(left,top,right,bottom)
#define XGR_GetClip(left,top,right,bottom)		XGR_Obj.get_clip(left,top,right,bottom)
#define XGR_SetClipMode(mode)				XGR_Obj.set_clip_mode(mode)

#define XGR_Flush(x,y,sx,sy)				XGR_Obj.flush(x,y,sx,sy)
#define XGR_Flip()					XGR_Obj.flip()
#define XGR_Fill(col)					XGR_Obj.fill(col)
#define XGR_Erase(x,y,sx,sy,col)			XGR_Obj.erase(x,y,sx,sy,col)
#define XGR_Rectangle(x,y,sx,sy,outcol,incol,mode)	XGR_Obj.rectangle(x,y,sx,sy,outcol,incol,mode)

#define XGR_SetPixel(x,y,col)				XGR_Obj.setpixel(x,y,col)
#define XGR_GetPixel(x,y)				XGR_Obj.getpixel(x,y)

#define XGR_Line(x1,y1,x2,y2,col)			XGR_Obj.line(x1,y1,x2,y2,col)
#define XGR_LineTo(x,y,len,dir,col)			XGR_Obj.lineto(x,y,len,dir,col)

#define XGR_SetPal(ptr,st,cnt)				XGR_Obj.setpal(ptr,st,cnt)
#define XGR_GetPal(ptr) 				XGR_Obj.getpal(ptr)

#define XGR_PutSpr(x,y,sx,sy,p,mode)			XGR_Obj.putspr(x,y,sx,sy,p,mode)
#define XGR_GetSpr(x,y,sx,sy,p) 			XGR_Obj.getspr(x,y,sx,sy,p)

#define XGR_WriteScreen(bmp_name,pal_name)		XGR_Obj.capture_screen(bmp_name,pal_name)

#ifdef XGR_SUPPRESS_HICOLOR
#define XGR_PutSpr16(x,y,sx,sy,p,mode)				XGR_Obj.putspr(x,y,sx,sy,p,mode)
#define XGR_GetSpr16(x,y,sx,sy,p)				XGR_Obj.getspr(x,y,sx,sy,p)
#define XGR_PutSpr16a(x,y,sx,sy,p,mode,alpha)			XGR_Obj.putspr(x,y,sx,sy,p,mode)
#define XGR_PutSpr16ap(x,y,sx,sy,p,mode,alpha)			XGR_Obj.putspr(x,y,sx,sy,p,mode)
#define XGR_Erase16(x,y,sx,sy,col)				XGR_Obj.erase(x,y,sx,sy,col)
#define XGR_Fill16(col) 					XGR_Obj.fill(col)
#define XGR_SetPixel16(x,y,col) 				XGR_Obj.setpixel(x,y,col)
#define XGR_Line16(x1,y1,x2,y2,col)				XGR_Obj.line(x1,y1,x2,y2,col)
#define XGR_LineTo16(x,y,len,dir,col)				XGR_Obj.lineto(x,y,len,dir,col)
#define XGR_Rectangle16(x,y,sx,sy,outcol,incol,mode)		XGR_Obj.rectangle(x,y,sx,sy,outcol,incol,mode)
#else
#define XGR_PutSpr16(x,y,sx,sy,p,mode)				XGR_Obj.putspr16(x,y,sx,sy,p,mode)
#define XGR_GetSpr16(x,y,sx,sy,p)				XGR_Obj.getspr16(x,y,sx,sy,p)
#define XGR_PutSpr16a(x,y,sx,sy,p,mode,alpha)			XGR_Obj.putspr16a(x,y,sx,sy,p,mode,alpha)
#define XGR_PutSpr16ap(x,y,sx,sy,p,mode,alpha)			XGR_Obj.putspr16ap(x,y,sx,sy,p,mode,alpha)
#define XGR_Erase16(x,y,sx,sy,col)				XGR_Obj.erase16(x,y,sx,sy,col)
#define XGR_Fill16(col) 					XGR_Obj.fill16(col)
#define XGR_SetPixel16(x,y,col) 				XGR_Obj.setpixel16(x,y,col)
#define XGR_Line16(x1,y1,x2,y2,col)				XGR_Obj.line16(x1,y1,x2,y2,col)
#define XGR_LineTo16(x,y,len,dir,col)				XGR_Obj.lineto16(x,y,len,dir,col)
#define XGR_Rectangle16(x,y,sx,sy,outcol,incol,mode)		XGR_Obj.rectangle16(x,y,sx,sy,outcol,incol,mode)
#endif

#define XGR_MouseInit(x,y,sx,sy,num,p)			XGR_MouseObj.Init(x,y,sx,sy,num,p)
#define XGR_MouseSetCursor(sx,sy,num,p) 		XGR_MouseObj.SetCursor(sx,sy,num,p)
#define XGR_MouseSetSpot(x,y)				XGR_MouseObj.SetSpot(x,y)
#define XGR_MouseInitPos(x,y)				XGR_MouseObj.InitPos(x,y)
#define XGR_MouseVisible()				XGR_MouseObj.Visible()

#define XGR_MouseCheckRedraw(x,y,sx,sy) 		XGR_MouseObj.CheckRedraw(x,y,sx,sy)
#define XGR_MouseNextFrame()				XGR_MouseObj.NextFrame()
#define XGR_MouseSetSeq(beg,end,mode,delta)		XGR_MouseObj.SetSeq(beg,end,mode,delta)
#define XGR_MouseSetAlphaSeq(beg,end,mode,delta)	XGR_MouseObj.SetAlphaSeq(beg,end,mode,delta)

#define XGR_MouseRedraw()				XGR_MouseObj.Redraw()
#define XGR_MousePutFrame()				XGR_MouseObj.PutFrame()
#define XGR_MousePutFon()				XGR_MouseObj.PutFon()
#define XGR_MouseGetFon()				XGR_MouseObj.GetFon()

#define XGR_MouseSetPressHandler(bt,p)			XGR_MouseObj.SetPressHandler(bt,p)
#define XGR_MouseSetUnPressHandler(bt,p)		XGR_MouseObj.SetUnPressHandler(bt,p)
#define XGR_MouseSetDblHandler(bt,p)			XGR_MouseObj.SetDblHandler(bt,p)
#define XGR_MouseSetMoveHandler(p)			XGR_MouseObj.SetMoveHandler(p)

#define XGR_MousePress(bt,fl,x,y)			XGR_MouseObj.Press(bt,fl,x,y)
#define XGR_MouseUnPress(bt,fl,x,y)			XGR_MouseObj.UnPress(bt,fl,x,y)
#define XGR_MouseDblClick(bt,fl,x,y)			XGR_MouseObj.DblClick(bt,fl,x,y)
#define XGR_MouseMove(fl,x,y)				XGR_MouseObj.Move(fl,x,y)

#define XGR_MouseHide() 				XGR_MouseObj.Hide()
#define XGR_MouseShow() 				XGR_MouseObj.Show()

#define XGR_MouseSetPos(x,y)				XGR_MouseObj.SetPos(x,y)
#define XGR_MouseSetPromptData(p)			XGR_MouseObj.SetPromptData(p)

#define XGR_MouseCurFrame				XGR_MouseObj.CurFrame
#define XGR_MouseNumFrames				XGR_MouseObj.NumFrames

extern int xgrScreenSizeX;
extern int xgrScreenSizeY;

#define XGR_MAXX	xgrScreenSizeX
#define XGR_MAXY	xgrScreenSizeY

#define XGR_VIDEOBUF	XGR_Obj.ScreenBuf

extern XGR_Screen XGR_Obj;
extern XGR_Mouse XGR_MouseObj;

void XGR_OutString(int x,int y,int col,void* str,int font = 0,int space = 1,int pr_flag = 1);
int XGR_StrLen(void* str,int font = 0,int space = 1);

void XGR_OutText(int x,int y,int col,void* text,int font = 0,int hspace = 1,int vspace = 1,int pr_flag = 1);
int XGR_TextHeight(void* text,int font = 0,int hspace = 1);
int XGR_TextWidth(void* text,int font = 0,int vspace = 1);

inline unsigned char* XGR_GetVideoLine(int y){ return (XGR_Obj.ScreenBuf + XGR_Obj.yOffsTable[y]); }

inline void XGR_SetPixelFast(int x,int y,int col){ XGR_Obj.ScreenBuf[XGR_Obj.yOffsTable[y] + x] = col; }
inline void XGR_SetPixel16Fast(int x,int y,int col){
	unsigned short* p = (unsigned short*)(XGR_Obj.ScreenBuf + XGR_Obj.yOffsTable[y]);
	p[x] = col;
}

// Convert 8bit sprite into 16bit sprite...
void XGR_PrepareSprite(int sx,int sy,void* dst_buf,void* src_buf,XGR_Pal64K* pal);

// Set text output handlers...
void XGR_SetTextOutFnc(XGR_OutTextHandler p);
void XGR_SetTextXFnc(XGR_TextWidthHandler p);
void XGR_SetTextYFnc(XGR_TextHeightHandler p);

extern int XGR_MASK_R;
extern int XGR_MASK_G;
extern int XGR_MASK_B;

extern int XGR_SHIFT_R;
extern int XGR_SHIFT_G;
extern int XGR_SHIFT_B;

extern int XGR_COLOR_MASK_R;
extern int XGR_COLOR_MASK_G;
extern int XGR_COLOR_MASK_B;

extern const char* XGraphWndID;

extern int XGR_HighColorMode;

extern int UI_OR_GAME;

#endif /* __XGRAPH_H */


