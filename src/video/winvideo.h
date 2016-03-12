#ifndef __WINVIDEO_H__
#define __WINVIDEO_H__

struct IGraphBuilder;
struct IMediaControl;
struct IVideoWindow;
struct IMediaEvent;

struct sWinVideo
{
	sWinVideo()											{ pGraphBuilder=0; pMediaControl=0; pVideoWindow=0; pMediaEvent=0; hWnd=0; }
	~sWinVideo()
		{
		//WORK Close();
		}
	static void Init();					// initilize DirectShow Lib
	static void Done();					// uninitilize DirectShow Lib
	// direct	
	void SetWin(void *hWnd,int x=0,int y=0,int xsize=0,int ysize=0);
	int Open(char *fname);				// if it's all OK, then function return 0
	void Play();
	void Stop();
	void Close();
	void WaitEnd();
	int IsComplete();
	// util	
	void HideCursor(int hide=1);		// 1 - hide cursor, 0 - unhide cursor
	void GetSize(int *xsize,int *ysize);
	void SetSize(int xsize,int ysize);
	void FullScreen(int bFullScreen=1);	// 1 - FullScreen, 0 - Window
	int IsPlay();

private:
	IGraphBuilder	*pGraphBuilder;
	IMediaControl	*pMediaControl;
	IVideoWindow	*pVideoWindow;
    IMediaEvent		*pMediaEvent;
	void			*hWnd;
};

#endif //__WINVIDEO_H__
