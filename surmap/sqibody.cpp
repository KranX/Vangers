/*
		  Simple-Quick Interface by K-Division::KranK
			  All Rights Reserved (C)1995
			      (Main Constructions)
*/

#include "../src/global.h"

#include "sqint.h"
#include "missed.h"

/* ---------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION --------------------------- */
/* --------------------------- DEFINITION SECTION -------------------------- */
int sqInputString::insert = 1,sqInputString::rus;

static const char* EventMsg = "EventQueue overflow. Reboot?";

inline char* sqstrNcpy(char* dest,char* src,int len)
{
	register int i = 0;
	while(src[i] && i < len) { dest[i] = src[i]; i++; }
	dest[i] = '\0';
	return dest;
}

void sqEventQueue::put(int _code)
{
	int n = (top + 1)%max;
	if(n == bottom) return; 	//ErrH.Abort(EventMsg);
	sqEvent* e = queue + top;
	e -> code = _code; e -> type = E_COMMON;
	top = n;
}

void sqEventQueue::put(int _code,int _type,int _x,int _y)
{
	int n = (top + 1)%max;
	if(n == bottom) return; 	//ErrH.Abort(EventMsg);
	sqEvent* e = queue + top;
	e -> code = _code; e -> x = _x; e -> y = _y; e -> type = _type;
	top = n;
}

void sqEventQueue::put(int _code,int _type,int _x,int _y,void* _pointer,int _value)
{
	int n = (top + 1)%max;
	if(n == bottom) return; 	//ErrH.Abort(EventMsg);
	sqEvent* e = queue + top;
	e -> code = _code; e -> x = _x; e -> y = _y; e -> type = _type; e -> pointer = _pointer; e -> value = _value;
	top = n;
}

sqEvent* sqEventQueue::get(void)
{
	if(top == bottom){
		return NULL;
		}
	sqEvent* r = queue + bottom;
	bottom = (bottom + 1)%max;
	return r;
}

sqElem::sqElem(int _height,int _dxl,int _dyl,int _dxr,int _dyr)
{
	dxl = _dxl; dyl = _dyl;
	dxr = _dxr; dyr = _dyr;
	height = _height;
	tail = owner = NULL;
}

int* sqElem::getXY(void){ return NULL; }
void sqElem::handler(sqEvent* e){}
void sqElem::accept(void){}
void sqElem::restore(int _x,int _y,int _sx,int _sy){}
void sqElem::keytrap(int key){}
void sqElem::message(int code,sqElem* object){}
void sqElem::activate(int force){}
void sqElem::deactivate(void){}
void sqElem::flush(void){}

void sqElem::quant(void)
{
	sqElem* p = tail;
	if(p)
		do {
			p -> quant();
			p = p -> next;
		} while(p != tail);
}

void sqElem::remove(void)
{
	if(owner) owner -> restore(getX() + xl - height,getY() + yl,xr - xl + height + 1,yr - yl + height + 1);
}

void sqElem::draw(int self)
{
#ifdef _SELFDRAW_
	if(self){
		if(height >= 1){
			XGR_LineTo(getX() + xl - 1,getY() + yr + 1,xr - xl + 1,2,SQ_SYSCOLOR);
			XGR_LineTo(getX() + xl - 1,getY() + yl + 1,yr - yl,3,SQ_SYSCOLOR);
			}
		if(height >= 2){
			XGR_LineTo(getX() + xl - 2,getY() + yr + 2,xr - xl + 1,2,SQ_SYSCOLOR);
			XGR_LineTo(getX() + xl - 2,getY() + yl + 2,yr - yl,3,SQ_SYSCOLOR);
			}
		}
#endif
	sqElem* p = tail;
	if(p)
		do {
			p -> draw();
			p = p -> next;
		} while(p != tail);
}

int sqElem::ishere(int x,int y)
{
	if(x >= getX() + xl && x <= getX() + xr && y >= getY() + yl && y <= getY() + yr) return 1;
	return 0;
}

void sqElem::set(int _height)
{
	int* d = getXY();
	xl = d[0] + dxl; yl = d[1] + dyl;
	xr = d[0] + d[2] - 1 + dxr; yr = d[1] + d[3] - 1 + dyr;
	height = _height;
}

sqElem& sqElem::operator+ (sqElem* e)
{
	if(tail){
		e -> prev = tail -> prev;
		e -> next = tail;
		tail -> prev -> next = e;
		tail -> prev = e;
		}
	else
		tail = e -> prev = e -> next = e;
	e -> owner = this;
	e -> accept();
	return *this;
}

sqElem& sqElem::operator- (sqElem* e)
{
	e -> reject();
	if(e == tail){
		if(tail -> next != tail){
			tail -> next -> prev = tail -> prev;
			tail -> prev -> next = tail -> next;
			tail = tail -> next;
			}
		else tail = NULL;
		}
	else {
		e -> next -> prev = e -> prev;
		e -> prev -> next = e -> next;
		}
	delete e;
	return *this;
}

void sqElem::reject(void)
{
	while(tail) *this - tail;
}

int sqElem::process(sqEvent* event)
{
	if(event -> type == E_COMMON) handler(event);
	sqElem* p = tail;
	if(p)
		do {
			if(p -> process(event)) return 1;
			p = p -> next;
		} while(p != tail);
	if(event -> type == E_REGION && ishere(event -> x,event -> y)){
		handler(event);
		return 1;
		}
	return 0;
}

void sqBox::draw(int self)
{
#ifdef _SELFDRAW_
	if(self){
		XGR_LineTo(getX() + x - 1,getY() + y - 1,sx + 2,2,SQ_SYSCOLOR + 7);
		XGR_LineTo(getX() + x - 1,getY() + y + sy,sx + 2,2,SQ_SYSCOLOR + 7);
		XGR_LineTo(getX() + x - 1,getY() + y,sy,3,SQ_SYSCOLOR + 7);
		XGR_LineTo(getX() + x + sx,getY() + y,sy,3,SQ_SYSCOLOR + 7);

		XGR_LineTo(getX() + x,getY() + y,sx,2,SQ_SYSCOLOR + 1);
		XGR_LineTo(getX() + x + sx - 1,getY() + y,sy,3,SQ_SYSCOLOR + 1);
		XGR_LineTo(getX() + x,getY() + y + sy - 1,sx - 1,2,SQ_SYSCOLOR + 5);
		XGR_LineTo(getX() + x,getY() + y + 1,sy - 2,3,SQ_SYSCOLOR + 5);

		for(int i = 0;i < sy - 2;i++)
			XGR_LineTo(getX() + x + 1,getY() + y + 1 + i,sx - 2,2,SQ_SYSCOLOR + 4);
		}
#endif
	sqElem::draw();
}

void sqBox::restore(int _x,int _y,int _sx,int _sy)
{
#ifdef _SELFDRAW_
	XGR_LineTo(getX() + x,getY() + y,sx,2,SQ_SYSCOLOR + 1);
	XGR_LineTo(getX() + x + sx - 1,getY() + y,sy,3,SQ_SYSCOLOR + 1);
	XGR_LineTo(getX() + x,getY() + y + sy - 1,sx - 1,2,SQ_SYSCOLOR + 5);
	XGR_LineTo(getX() + x,getY() + y + 1,sy - 2,3,SQ_SYSCOLOR + 5);

	for(int i = 0;i < sy - 2;i++)
		XGR_LineTo(getX() + x + 1,getY() + y + 1 + i,sx - 2,2,SQ_SYSCOLOR + 4);
#endif
}

sqButton::sqButton(sqElem* _owner,int _x,int _y,int _sx,int _sy,int _shift,int _pressed)
{
	owner = _owner;
	x = _x; y = _y; sx = _sx; sy = _sy;
	pressed = _pressed; shift = _shift; set();
}

void sqButton::draw(int self)
{
#ifdef _SELFDRAW_
	if(self){
		XGR_Rectangle(getX() + x,getY() + y,sx,sy,SQ_SYSCOLOR + 4,SQ_SYSCOLOR + 4,XGR_FILLED);
		XGR_LineTo(getX() + x - 1,getY() + y - 1,sx + 2,2,SQ_SYSCOLOR + 6);
		XGR_LineTo(getX() + x + sx,getY() + y,sy,3,SQ_SYSCOLOR + 6);
		XGR_LineTo(getX() + x - 1,getY() + y + sy,sx + 1,2,SQ_SYSCOLOR + 2);
		XGR_LineTo(getX() + x - 1,getY() + y,sy,3,SQ_SYSCOLOR + 2);
		}
#endif
	sqElem::draw();
}

void sqButton::handler(sqEvent* e)
{
	if(e -> code == E_LBMPRESS){
		remove();
		if(pressed) x += shift,y -= shift,set(++height);
		else x -= shift,y += shift,set(--height);
		pressed = 1 - pressed;
		draw();
		}
}

sqInputBox::sqInputBox(sqElem* _owner,int _x,int _y,int _sx,int _sy,sqFont* _font,const char* _title)
: sqBox(_owner,0,0,0,0)
{
	owner = _owner;
	sx = _sx; sy = _sy;
	x = _x - _sx/2; y = _y - _sy/2;
//	  if(x < getX() + 4) x = getX() + 4;
//	  if(y < getY() + 2) y = getY() + 2;
//	  if(x + sx + 2 >= getRX()) x = getRX() - sx - 2;
//	  if(y + sy + 4 >= getRY()) y = getRY() - sy - 4;
	set();
	title = _title == nullptr ? nullptr : strdup(_title);
	font = _font;

	KeyTrapObj = this;
}

sqInputBox::~sqInputBox() {
	free(title);
}

void sqInputBox::close(void)
{
	sqElem* o = KeyTrapObj = owner;
	QuantObj = owner;
	*o - this;
	o -> draw();
}

void sqInputBox::keytrap(int key)
{
	switch(key){
		case SDLK_F10:
			message(M_ACCEPT);
			close();
			break;
		case SDLK_ESCAPE:
			message(M_CANCEL);
			close();
			break;
		default:
			if(obj) obj -> keytrap(key);
//			  owner -> keytrap(key);
			break;
		}
}

void sqInputBox::message(int code,sqElem* object)
{
	switch(code){
		case M_CHOICE:
		case M_NEXTOBJ:
			obj -> deactivate();
			obj = obj -> next;
			obj -> activate();
			break;
		case M_PREVOBJ:
			obj -> deactivate();
			obj = obj -> prev;
			obj -> activate();
			break;
		case M_SETOBJ:
			if(object != obj){
				if(obj) obj -> deactivate();
				obj = object;
				obj -> activate();
				}
			break;
		case M_DEACTIVATE:
			obj -> deactivate();
			obj = object;
			break;
		}
}

void sqInputBox::draw(int self)
{
	sqBox::draw(self);
#ifdef _SELFDRAW_
	if(self && title){
		XGR_Rectangle(getX() + x + 1,getY() + y + 1,sx - 2,font -> sy,SQ_SYSCOLOR + 2,SQ_SYSCOLOR + 2,XGR_FILLED);
		font -> draw(getX() + x + 1 + (sx - 2 - font -> sx*strlen(title))/2,getY() + y + 1,(unsigned char*)title,SQ_SYSCOLOR + 6,SQ_SYSCOLOR + 2);
		XGR_LineTo(getX() + x,getY() + y + 1 + font -> sy,sx,2,SQ_SYSCOLOR + 6);
		XGR_LineTo(getX() + x,getY() + y + 2 + font -> sy,sx,2,SQ_SYSCOLOR + 1);
		}
#endif
}

void sqInputBox::restore(int _x,int _y,int _sx,int _sy)
{
#ifdef _SELFDRAW_
	XGR_LineTo(_x,_y + 2,_sy - 2,3,SQ_SYSCOLOR + 4);
	XGR_LineTo(_x + 1,_y + _sy - 1,_sx - 1,2,SQ_SYSCOLOR + 4);
#endif
}

void sqset2null(unsigned char* s,int l,int d)
{
	memset(s,' ',l);
	if(d > 0){
		memset(s + l - d - 2,'0',d + 2);
		s[l - d - 1] = '.';
		}
	else
		s[l - 1] = '0';
}

sqInputString::sqInputString(sqElem* _owner,int _x,int _y,int _size,sqFont* _font,unsigned char* _str,int _len,int _type,int _dec)
{
	owner = _owner;
	x = _x; y = _y; size = _size; font = _font;
	sy = font -> sy;
	if(!(_type & T_FIELD)) sy += 4;
	if(!_len)
		len = (_size - 6)/_font -> sx;
	else {
		len = _len;
		if(size < 0) size = len*font -> sx;
		if(!(_type & T_FIELD)) size += 9;
		}
	type = _type; dec = _dec;
	if(!(type & T_FIELD))
		offset = (size - len*_font -> sx - 6)/2;
	else
		offset = 0;
	str = new unsigned char[len + 1 + 64];
	memset(str,' ',len);
	str[len] = '\0';
	if(_str){
		if(type & T_NUMERIC)
			memcpy((char*)str + len - strlen((char*)_str),(char*)_str,strlen((char*)_str));
		else
			memcpy((char*)str,(char*)_str,len);
//		  if(strlen((char*)_str) < len)
//			  memset(str + strlen((char*)_str),' ',len - strlen((char*)_str));
//		  if(type & T_NUMERIC){
//			  if(str[len - 1 - dec] == ' ' || !str[len - 1 - dec]) sqset2null(str,len,dec);
//			  }
		}
	savestr = new unsigned char[len];
	index = blink = clen = 0;
	fstatus = 1;
	if(!(type & T_FIELD))
		set();
	else
		set(0);
}

void sqInputString::draw(int self)
{
	if(!(type & T_FIELD)){
		XGR_Rectangle(getX() + x,getY() + y,size,font -> sy + 4,SQ_SYSCOLOR + 6 - (height - 1),SQ_SYSCOLOR + 6 - (height - 1),XGR_FILLED);
		font -> draw(getX() + x + 2 + offset,getY() + y + 2,str,SQ_SYSCOLOR,SQ_SYSCOLOR + 6 - (height - 1));
		sqElem::draw();
		}
	else
		if(QuantObj == this)
			font -> draw(getX() + x,getY() + y,str,SQ_SYSCOLOR,SQ_SYSCOLOR + 6);
		else
			font -> draw(getX() + x,getY() + y,str,SQ_SYSCOLOR,SQ_SYSCOLOR + 5);
}

void sqInputString::quant(void)
{
	static int last = 0;
	int t = CLOCK(),c,cdy;
	if(last != t){
		if(!(t%2)){
			if(!(type & T_FIELD)){
				c = (blink) ? SQ_SYSCOLOR : (SQ_SYSCOLOR + 6 - (height - 1));
				cdy = insert ? 1 : 9;
				XGR_Rectangle(getX() + x + 2 + offset + index*font -> sx,getY() + y + 2 + font -> sy - cdy,font -> sx,cdy + 1,c,c,XGR_FILLED);
				if(!insert && !blink) font -> drawchar(getX() + x + 2 + offset + index*font -> sx,getY() + y + 2,str[index],SQ_SYSCOLOR,SQ_SYSCOLOR + 6 - (height - 1));
				}
			else {
				c = (blink) ? SQ_SYSCOLOR : SQ_SYSCOLOR + 6;
				cdy = insert ? 2 : 10;
				XGR_Rectangle(getX() + x + index*font -> sx,getY() + y + font -> sy - cdy,font -> sx,cdy,c,c,XGR_FILLED);
				if(!blink) font -> drawchar(getX() + x + index*font -> sx,getY() + y,str[index],SQ_SYSCOLOR,SQ_SYSCOLOR + 6);
				}
			blink = 1 - blink;
			}
//		  else
//			  sqE -> put(E_REFRESH);
		last = t;
		}
}

void sqInputString::handler(sqEvent* e)
{
	int i;
	if(e -> code == E_LBMPRESS){
		owner -> message(M_SETOBJ,this);
		delcursor();
		if(!(type & T_FIELD))
			i = (e -> x - (getX() + x + 2))/font -> sx;
		else
			i = (e -> x - (getX() + x))/font -> sx;
		if(i >= 0 && i < clen) index = i;
		else index = (clen < len)?clen:len - 1;
		}
}

void sqInputString::delcursor(void)
{
	if(!(type & T_FIELD)){
		XGR_Rectangle(getX() + x + 2 + offset + index*font -> sx,getY() + y + 2 + font -> sy - (insert?1:9),font -> sx,insert?2:10,SQ_SYSCOLOR + 6 - (height - 1),SQ_SYSCOLOR + 6 - (height - 1),XGR_FILLED);
		if(!insert) font -> drawchar(getX() + x + 2 + offset + index*font -> sx,getY() + y + 2,str[index],SQ_SYSCOLOR,SQ_SYSCOLOR + 6 - (height - 1));
		}
	else {
		XGR_Rectangle(getX() + x + index*font -> sx,getY() + y + font -> sy - (insert?2:10),font -> sx,insert?2:10,SQ_SYSCOLOR + 6,SQ_SYSCOLOR + 6,XGR_FILLED);
		font -> drawchar(getX() + x + index*font -> sx,getY() + y,str[index],SQ_SYSCOLOR,SQ_SYSCOLOR + 6);
		}
}

void sqInputString::keytrap(int key)
{
	static int fhome,fend;
	int newindex = -1;
	int dp = ((type & T_NUMERIC) && dec > 0) ? len - dec - 1 : 0;
	unsigned char c;
	int xadd,yadd,col,i;
	if(!(type & T_FIELD)){
		xadd = 2 + offset;
		yadd = 2;
		col = 6 - (height - 1);
		}
	else {
		xadd = yadd = 0;
		col = 6;
		}
	switch(key){
		case SDLK_TAB:
			if(XKey.Pressed(SDLK_LSHIFT))
				owner -> message(M_PREVOBJ);
			else
				owner -> message(M_NEXTOBJ);
			break;
		case SDLK_DOWN:
			owner -> message(M_NEXTOBJ);
			break;
		case SDLK_UP:
			owner -> message(M_PREVOBJ);
			break;
		case SDLK_RETURN:
			owner -> message(M_CHOICE);
			return;
		case SDLK_INSERT:
			delcursor();
			insert = 1 - insert;
			break;
		case SDLK_RSHIFT:
			rus = 1 - rus;
			break;
		case SDLK_LEFT:
			if(index){
				newindex = index - 1;
				if(newindex && XKey.Pressed(SDLK_LCTRL)){
					if(!isalnum(str[newindex]))
						while(newindex && !isalnum(str[newindex])) newindex--;
					while(newindex && isalnum(str[newindex])) newindex--;
					if(newindex) newindex++;
					}
				else
					if(dp && newindex == dp) newindex--;
				}
			break;
		case SDLK_RIGHT:
			if(index < len - 1){
				newindex = index + 1;
				if(newindex < len - 1 && XKey.Pressed(SDLK_LCTRL)){
					if(isalnum(str[newindex]))
						while(newindex < len - 1 && isalnum(str[newindex])) newindex++;
					while(newindex < len - 1 && !isalnum(str[newindex])) newindex++;
					}
				else
					if(newindex == dp) newindex++;
				}
			break;
		case SDLK_HOME:
			if(fhome <= 0){
				if(index) newindex = 0;
				fhome = 2;
				}
			else {
				for(i = 0;i < len;i++)
					if(str[i] != ' ') break;
				if(i != len) newindex = i;
				}
			break;
		case SDLK_END:
			if(fend <= 0){
				if(index < len - 1) newindex = len - 1;
				fend = 2;
				}
			else {
				for(i = len - 2;i >= 0;i--)
					if(str[i] != ' ') break;
				if(i >= 0) newindex = i + 1;
				}
			break;
		case SDLK_BACKSPACE:
			if(XKey.Pressed(SDLK_LCTRL)){
				memcpy(str,savestr,len);
				font -> draw(getX() + x + xadd,getY() + y + yadd,str,SQ_SYSCOLOR,SQ_SYSCOLOR + col);
				}
			else
				if(index){
					newindex = index - 1;
					if(!dp){
						memmove(str + newindex,str + index,len - index);
						str[len - 1] = ' ';
						font -> draw(getX() + x + xadd + font -> sx*newindex,getY() + y + yadd,str + newindex,SQ_SYSCOLOR,SQ_SYSCOLOR + col);
						}
					else {
						if(index < dp){
							memmove(str + newindex,str + index,dp - index);
							str[dp - 1] = ' ';
							}
						if(index > dp)
							if(newindex == dp) str[--newindex] = ' ';
							else {
								memmove(str + newindex,str + index,len - index);
								str[len - 1] = ' ';
								}
						font -> draw(getX() + x + xadd,getY() + y + yadd,str,SQ_SYSCOLOR,SQ_SYSCOLOR + col);
						}
					}
			break;
		case SDLK_DELETE:
			if(!dp){
				if(index != clen){
					newindex = index;
					memmove(str + index,str + index + 1,len - index - 1);
					str[len - 1] = ' ';
					font -> draw(getX() + x + xadd + font -> sx*index,getY() + y + yadd,str + index,SQ_SYSCOLOR,SQ_SYSCOLOR + col);
					}
				}
			else {
				newindex = index;
				if(index < dp){
					if(index < dp - 1) memmove(str + index,str + index + 1,dp - index - 1);
					str[dp - 1] = ' ';
					}
				if(index > dp){
					if(index < len - 1) memmove(str + index,str + index + 1,len - index - 1);
					str[len - 1] = ' ';
					}
				font -> draw(getX() + x + xadd,getY() + y + yadd,str,SQ_SYSCOLOR,SQ_SYSCOLOR + col);
				}
			break;
		default:
			if(fstatus && !index){
				memset(str,' ',len);
				index = 0;
				if(dp) sqset2null(str,len,dec);
				clen = len;
				draw();
				}
			c = key;	// !
			if(c == SDLK_MINUS || c == SDLK_KP_MINUS) c = '-';
			if(!dp){
				if((type & T_STRING) || ((type & T_NUMERIC) && !dec && ((isdigit(c) || c == ' ' || c == '-')))){
					if(index < len - 1){
						newindex = index + 1;
						if(insert && index < len - 2) memmove(str + newindex,str + index,len - index - 1);
						}
					str[index] = c;
					if(insert && index < len - 2)
						font -> draw(getX() + x + xadd + index*font -> sx,getY() + y + yadd,str + index,SQ_SYSCOLOR,SQ_SYSCOLOR + col);
					else
						font -> drawchar(getX() + x + xadd + index*font -> sx,getY() + y + yadd,str[index],SQ_SYSCOLOR,SQ_SYSCOLOR + col);
					}
				}
			else {
				if(isdigit(c) || c == '.' || c == ' ' || c == '-'){
					if(c == '.'){
						newindex = dp + 1;
						if(index && index < dp) memmove(str + dp - index,str,index), memset(str,' ',index);
						if(!index || str[dp - 1] == ' ') memset(str,' ',dp - 1), str[dp - 1] = '0';
						}
					else {
						if(index < dp - 1 && insert) memmove(str + index + 1,str + index,dp - index - 1);
						if(index < dp) newindex = index + 1;
						if(newindex == dp) newindex++;
						if(index > dp && insert && index < len - 1) memmove(str + index + 1,str + index,len - index - 1), newindex = index + 1;
						str[index] = c;
						}
					font -> draw(getX() + x + xadd,getY() + y + yadd,str,SQ_SYSCOLOR,SQ_SYSCOLOR + col);
					}
				}
		}
	if(newindex != -1){
		delcursor();
		blink = 1;
		if(newindex != -1) index = newindex;
		}
	--fhome; --fend;
	fstatus = 0;
}

void sqInputString::activate(int force)
{
//	QuantObj = this;
	memcpy(savestr,str,len);
	if(force) remove();
	if(!(type & T_FIELD))
		set(1);
	else
		set(0);
	if(force) draw();
}

void sqInputString::deactivate(void)
{
	remove();
	if(!(type & T_FIELD))
		set(2);
	else
		set(0);
	if(type & T_NUMERIC){
		XBuffer ff;
		ff < "%" <= len < "." <= dec < "f";
		sprintf((char*)str,ff.GetBuf(),atof((char*)str));
		}
	draw();
}

char* sqInputString::getstr(void)
{
	register int i = len - 1;
	while(i && str[i] == ' ') i--;
	i++;
	char* s = new char[i + 1];
	memcpy(s,str,i);
	s[i] = '\0';
	return s;
}

void sqInputString::setstr(unsigned char* s)
{
	sqstrNcpy((char*)str,(char*)s,len);
	clen = index = strlen((char*)str) - 1;
	while(str[index] == ' ') index--;
	if(index) index++;
	str[index] = '\0';
	clen = index;
	if(clen == len) clen--,index--;
}

sqField::sqField(sqElem* _owner,const char* _prompt,int _x,int _y,int _size,sqFont* _font,unsigned char* _str,int _len,int _type,int _dec)
: sqInputString(_owner,_x + strlen(_prompt)*_font -> sx + 3,_y,_size - strlen(_prompt)*_font -> sx - 3,_font,_str,_len,_type,_dec)
{
	prompt = (unsigned char*)strdup(_prompt);
	x0 = _x;
	y0 = _y + 2;
}

void sqField::draw(int self)
{
	font -> draw(getX() + x0,getY() + y0,prompt,SQ_SYSCOLOR,SQ_SYSCOLOR + 4);
	sqInputString::draw();
}

sqTextButton::sqTextButton(sqElem* _owner,int _x,int _y,const char* _text,sqFont* _font,int _sx)
{
	owner = _owner;
	x = _x; y = _y;
	text = strdup(_text);
	font = _font;
	if(_sx)
		sx = _sx;
	else
		sx = strlen(_text)*_font -> sx + 6;
	sy = font -> sy + 4;
	set();
	offset = (sx - strlen(_text)*_font -> sx)/2;
}

sqTextButton::~sqTextButton() {
	free(text);
}

void sqTextButton::draw(int self)
{
	const int mask[2*23] = { 6,7,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,3,2,0,
				 6,7,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,3,2,0 };
	int ind = (height == 2)?0:2;

//	  c_putspr(getX() + x - 6,getY() + y - 3,ButElem[ind] -> x_size,ButElem[ind] -> y_size,ButElem[4*(2 - height) + ind] -> data,BLACK_FON);
//	  c_putspr(getX() + x + sx - 2,getY() + y - 3,ButElem[ind + 1] -> x_size,ButElem[ind + 1] -> y_size,ButElem[4*(2 - height) + ind + 1] -> data,BLACK_FON);

	for(int i = 0;i < 23;i++)
		XGR_LineTo(getX() + x + 2,getY() + y - 3 + i,sx - 4,2,SQ_SYSCOLOR + mask[23*(height - 1) + i]);
	if(!ind)
		XGR_LineTo(getX() + x + 2,getY() + y + 20,sx - 4,2,SQ_SYSCOLOR);

	font -> draw(getX() + x + offset,getY() + y,(unsigned char*)text,SQ_SYSCOLOR,SQ_SYSCOLOR + 7 - height);
}

void sqTextButton::handler(sqEvent* e)
{
	if(e -> code == E_LBMPRESS)
		owner -> message(M_SETOBJ,this);
}

void sqTextButton::keytrap(int key)
{
	switch(key){
		case SDLK_TAB:
			if(XKey.Pressed(SDLK_RSHIFT))
				owner -> message(M_PREVOBJ);
			else
				owner -> message(M_NEXTOBJ);
			break;
		case SDLK_LEFT:
		case SDLK_UP:
			owner -> message(M_PREVOBJ);
			break;
		case SDLK_RIGHT:
		case SDLK_DOWN:
			owner -> message(M_NEXTOBJ);
			break;
		case SDLK_RETURN:
		case SDLK_SPACE:
			owner -> message(M_CHOICE);
			break;
		}
}

void sqTextButton::activate(int force)
{
//	  QuantObj = this;
	if(force) remove();
	set(1);
	if(force) draw();
}

void sqTextButton::deactivate(void)
{
	remove();
	set(2);
	draw();
}

void sqTextButton::remove(void)
{
	XGR_LineTo(getX() + x - 3,getY() + y + 20,sx + 4,2,SQ_SYSCOLOR + 4);
}
