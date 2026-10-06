#define _GNU_SOURCE
#include <SDL3/SDL.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* External test observer; no game code or render parameters are changed.
 * Commands are appended to BASELINE_COMMANDS: wait MS; key NAME;
 * move X Y; down BUTTON; up BUTTON; text UTF8; shot NAME; quit.
 * SDL event injection is NOT a physical keyboard/IME test.
 */
static FILE *commands;
static Uint64 until;
static SDL_WindowID window_id;
static float mouse_x, mouse_y;
static char shot[1024];
static unsigned long frame, shot_frame;
static SDL_MouseButtonFlags buttons;
static void push(SDL_Event *e) {
    if (!SDL_PushEvent(e)) fprintf(stderr,"BASELINE event error: %s\n",SDL_GetError());
}
static void command(void) {
    if (shot[0] || SDL_GetTicks() < until) return;
    if (!commands) {
        const char *path=getenv("BASELINE_COMMANDS");
        if (!path) return;
        commands=fopen(path,"r");
        if (!commands) return;
    }
    char line[2048];
    clearerr(commands);
    if (!fgets(line,sizeof(line),commands)) return;
    line[strcspn(line,"\r\n")]=0;
    if (!line[0] || line[0]=='#') return;
    fprintf(stderr,"BASELINE frame=%lu ticks=%llu command=%s\n",frame,(unsigned long long)SDL_GetTicks(),line);
    SDL_Event e={0};
    if (!strncmp(line,"wait ",5)) { until=SDL_GetTicks()+strtoul(line+5,NULL,10); }
    else if (!strncmp(line,"key ",4)) {
        e.type=SDL_EVENT_KEY_DOWN; e.key.windowID=window_id; e.key.down=true;
        e.key.key=SDL_GetKeyFromName(line+4); e.key.scancode=SDL_GetScancodeFromKey(e.key.key,NULL);
        push(&e); e.type=SDL_EVENT_KEY_UP; e.key.down=false; push(&e);
    } else if (!strncmp(line,"move ",5)) {
        float x,y; if(sscanf(line+5,"%f %f",&x,&y)!=2) return;
        e.type=SDL_EVENT_MOUSE_MOTION; e.motion.windowID=window_id;
        e.motion.x=x; e.motion.y=y; e.motion.xrel=x-mouse_x; e.motion.yrel=y-mouse_y;
        e.motion.state=buttons; mouse_x=x; mouse_y=y; push(&e);
    } else if (!strncmp(line,"down ",5) || !strncmp(line,"up ",3)) {
        bool down=line[0]=='d'; unsigned b=(unsigned)atoi(line+(down?5:3));
        if (b<1 || b>5) return;
        if(down) buttons|=SDL_BUTTON_MASK(b); else buttons&=~SDL_BUTTON_MASK(b);
        e.type=down?SDL_EVENT_MOUSE_BUTTON_DOWN:SDL_EVENT_MOUSE_BUTTON_UP;
        e.button.windowID=window_id; e.button.button=b; e.button.down=down; e.button.clicks=1;
        e.button.x=mouse_x; e.button.y=mouse_y; push(&e);
    } else if (!strncmp(line,"text ",5)) {
        e.type=SDL_EVENT_TEXT_INPUT; e.text.windowID=window_id;
        /* Keep text valid through event consumption. Tiny intentional test-process allocation. */
        e.text.text=strdup(line+5); push(&e);
    } else if (!strncmp(line,"shot ",5)) {
        const char *dir=getenv("BASELINE_CAPTURES");
        if(!dir || strchr(line+5,'/') || strstr(line+5,"..")) return;
        if (strlen(dir) + strlen(line+5) + 6 > sizeof(shot)) {
            fprintf(stderr,"BASELINE capture path too long\n");
            return;
        }
        strcpy(shot,dir); strcat(shot,"/"); strcat(shot,line+5); strcat(shot,".bmp");
        shot_frame=frame+3;
    } else if (!strcmp(line,"quit")) {e.type=SDL_EVENT_QUIT;push(&e);}
}
bool SDL_PollEvent(SDL_Event *event) {
    static bool (*real)(SDL_Event *);
    if(!real) real=dlsym(RTLD_NEXT,"SDL_PollEvent");
    command(); return real(event);
}
bool SDL_RenderPresent(SDL_Renderer *renderer) {
    static bool (*real)(SDL_Renderer *);
    if(!real) real=dlsym(RTLD_NEXT,"SDL_RenderPresent");
    ++frame; window_id=SDL_GetWindowID(SDL_GetRenderWindow(renderer));
    if(shot[0] && frame>=shot_frame) {
        SDL_Surface *surface=SDL_RenderReadPixels(renderer,NULL);
        if(!surface) fprintf(stderr,"BASELINE capture error: %s\n",SDL_GetError());
        else {
            bool ok=SDL_SaveBMP(surface,shot);
            fprintf(stderr,"BASELINE capture=%s size=%dx%d frame=%lu driver=%s renderer=%s ok=%d\n",shot,surface->w,surface->h,frame,SDL_GetCurrentVideoDriver(),SDL_GetRendererName(renderer),ok);
            SDL_DestroySurface(surface);
        }
        shot[0]=0;
    }
    return real(renderer);
}
