/*
		  Simple-Quick Interface by K-Division::KranK
			  All Rights Reserved (C)1995
				     (Menu)
*/

#include "../src/global.h"

#include "sqint.h"
#include "missed.h"

/* ---------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION --------------------------- */
/* --------------------------- DEFINITION SECTION -------------------------- */
const int MAXSEEK = 32;
const int SEEKDELAY = 10;

sqMenuBar::sqMenuBar(const unsigned char* s,sqPopupMenu* owner,int _status,int _value,int _rec)
{
	int m = owner -> margin;
	int sl = s?strlen((char*)s):0;
	int l = sl + 2*m;
	if(l > owner -> len) owner -> setlen(l);
	else l = owner -> len;
	data = new unsigned char[l + 1];
	original_data = (unsigned char*)strdup((char*)s);
	memset(data,' ',m);
	if(s) memcpy(data + m,s,sl);
	memset(data + m + sl,' ',l - sl - m);
	data[l] = '\0';
	if((status = _status) != 0) *data = 4;
	value = _value;
	rec = _rec;
}

void sqMenuBar::replace(sqPopupMenu* owner,unsigned char* s)
{
	int m = owner -> margin;
	int sl = s?strlen((char*)s):0;
	int l = sl + 2*m;
	if(l > owner -> len){
		owner -> setlen(l);
		delete data;
		data = new unsigned char[l + 1];
		}
	else
		l = owner -> len;
	if(s) memcpy(data + m,s,sl);
	memset(data + m + sl,' ',l - sl - m);
	data[l] = '\0';
}

sqPopupMenu::sqPopupMenu(sqElem* _owner,int _x,int _y,int _visibars,sqFont* _font,int _len0,int _margin)
{
	owner = _owner;
	x = _x; y = _y;
	sx = sy = 0;
	visibars = _visibars;
	margin = _margin;
	font = _font;
	len = len0 = _len0;
	if(_len0) sx = _len0*_font -> sx, set();
	maxbars = topbar = nbar = seekcounter = 0;
	first = pointer = insert = NULL;
	seeklen = 0;
	seekbuf = new unsigned char[MAXSEEK + 1];
}

sqPopupMenu& sqPopupMenu::operator* (sqMenuBar* obj)
{
	if(!first){
		first = pointer = obj;
		first -> next = first -> prev = NULL;
		}
	else {
		obj -> prev = insert;
		if(insert){
			obj -> next = insert -> next;
			if(insert -> next) insert -> next -> prev = obj;
			insert -> next = obj;
			}
		else {
			obj -> next = first;
			first -> prev = obj;
			first = obj;
			}
		}
	insert = obj;
	maxbars++;
	sy = MIN(maxbars,visibars)*font -> sy; set(height);
	return *this;
}

void sqPopupMenu::disconnect(sqMenuBar* obj,sqMenuBar** del)
{
	if(obj == insert)
		if(obj -> next) insert = obj -> next;
		else insert = obj -> prev;
	if(obj == first){
		if(obj -> next) obj -> next -> prev = NULL;
		first = obj -> next;
		}
	else {
		obj -> prev -> next = obj -> next;
		if(obj -> next) obj -> next -> prev = obj -> prev;
		}
	if(!del)
		delete obj;
	else {
		obj -> next = *del;
		*del = obj;
		}
	maxbars--;
	sy = MIN(maxbars,visibars)*font -> sy; set(height);
}

void sqPopupMenu::setlen(int l)
{
	sqMenuBar* p = first;
	unsigned char* s;
	if(l > len)
		while(p){
			s = new unsigned char[l + 1];
			memcpy(s,p -> data,len);
			memset(s + len,' ',l - len);
			s[l] = '\0';
			delete[] p -> data;
			p -> data = s;
			p = p -> next;
			}
	else
		if(l < len)
			while(p){
				p -> data[l] = '\0';
				p = p -> next;
				}
	len = l;
	sx = len*font -> sx; set(height);
}

void sqPopupMenu::draw(int self)
{
	nbar = getptr(pointer);
	sqMenuBar* p = getbar(topbar);
	int i = y,j = 0;
	while(p && j < visibars){
		if(p == pointer)
			font -> draw(getX() + x,getY() + i,p -> data,SQ_SYSCOLOR,SQ_SYSCOLOR + 6 + (2 - height));
		else
			font -> draw(getX() + x,getY() + i,p -> data,SQ_SYSCOLOR,SQ_SYSCOLOR + 5);
		i += font -> sy; j++;
		p = p -> next;
		}
	sqElem::draw();
	owner -> message(M_CHANGEOPTION);
}

sqMenuBar* sqPopupMenu::getbar(int n)
{
	sqMenuBar* p = first;
	while(p){
		if(!n) return p;
		p = p -> next;
		n--;
		}
	return NULL;
}

int sqPopupMenu::getptr(sqMenuBar* b)
{
	sqMenuBar* p = first;
	int i = 0;
	while(p){
		if(p == b) return i;
		p = p -> next;
		i++;
		}
	return 0;
}

void sqPopupMenu::keytrap(int key)
{
	switch(key){
		case SDLK_TAB:
			if(XKey.Pressed(SDLK_LSHIFT))
				owner -> message(M_PREVOBJ);
			else
				owner -> message(M_NEXTOBJ);
			seeklen = 0;
			break;
		case SDLK_F7:
		case SDLK_INSERT:
			owner -> message(M_INSERT);
			break;
		case SDLK_F8:
		case SDLK_DELETE:
			owner -> message(M_DELETE);
			break;
		case SDLK_SPACE:
			owner -> message(M_SETOPTION);
			break;
		case SDLK_RETURN:
			if(XKey.Pressed(SDLK_LSHIFT)){
				if(seeklen)
					if(pointer -> next)
						seek(pointer -> next);
					else
						seek(first);
				}
			else {
				seeklen = 0;
				owner -> message(M_CHOICE);
				}
			break;
		case SDLK_RSHIFT:
			sqInputString::rus = 1 - sqInputString::rus;
			break;
		case SDLK_UP:
			if(!pointer -> prev){
				pointer = getbar(maxbars - 1);
				topbar = (maxbars >= visibars)?maxbars - visibars:0;
				}
			else {
				if(pointer == getbar(topbar)) topbar--;
				pointer = pointer -> prev;
				}
			draw();
			seeklen = 0;
			break;
		case SDLK_DOWN:
			if(!pointer -> next){
				pointer = first;
				topbar = 0;
				}
			else {
				if(getptr(pointer) - topbar + 1 == visibars) topbar++;
				pointer = pointer -> next;
				}
			draw();
			seeklen = 0;
			break;
		case SDLK_HOME:
			if(pointer){
				pointer = first;
				topbar = 0;
				draw();
				}
			seeklen = 0;
			break;
		case SDLK_END:
			if(pointer -> next){
				pointer = getbar(maxbars - 1);
				topbar = (maxbars >= visibars)?maxbars - visibars:0;
				draw();
				}
			seeklen = 0;
			break;
		case SDLK_LEFT:
			if(pointer != getbar(topbar)){
				pointer = getbar(topbar);
				draw();
				}
			seeklen = 0;
			break;
		case SDLK_RIGHT:
			if(pointer != getbar(topbar + MIN(visibars,maxbars) - 1)){
				pointer = getbar(topbar + MIN(visibars,maxbars) - 1);
				draw();
				}
			seeklen = 0;
			break;
		case SDLK_PRIOR:
			if(pointer){
				topbar = getptr(pointer) - visibars;
				if(topbar < 0) topbar = 0;
				pointer = getbar(topbar);
				draw();
				}
			seeklen = 0;
			break;
		//@caiiiycuk: was VK_NEXT
		case SDLK_RETURN2:
			if(pointer != getbar(maxbars - 1)){
				topbar = getptr(pointer) + visibars;
				if(topbar >= maxbars) topbar = maxbars - 1;
				pointer = getbar(topbar);
				topbar -= MIN(maxbars,visibars) - 1;
				draw();
				}
			seeklen = 0;
			break;
		case SDLK_KP_PLUS:
			owner -> message(M_SETALL);
			break;
		case SDLK_KP_MINUS:
			owner -> message(M_UNSETALL);
			break;
		default:
			if(CLOCK() - seekcounter > SEEKDELAY) seeklen = 0;
			if(seeklen <= MAXSEEK){
				seekbuf[seeklen++] = key;		// !
				seek(pointer);
				seekcounter = CLOCK();
				}
			break;
		}
}

void sqPopupMenu::handler(sqEvent* e)
{
	int n;
	sqMenuBar* pb;
	switch(e -> code){
		case E_LBMPRESS:
//			if( XGR_MouseObj.GetLeftClip() != getX() + x + XGR_MouseObj.SizeX/2){
//				XGR_MouseObj.SetClip(getX() + x + XGR_MouseObj.SizeX/2, getY() + y + XGR_MouseObj.SizeY/2, getX() + x + sx - XGR_MouseObj.SizeX/2, getY() + y + sy - XGR_MouseObj.SizeY/2);
//				  XGR_MouseObj.left = getX() + x + XGR_MouseObj.SizeX/2;
//				  XGR_MouseObj.top = getY() + y + XGR_MouseObj.SizeY/2;
//				  XGR_MouseObj.right = getX() + x + sx - XGR_MouseObj.SizeX/2;
//				  XGR_MouseObj.bottom = getY() + y + sy - XGR_MouseObj.SizeY/2;
//				  XGR_MouseObj.change();
//				}
			owner -> message(M_SETOBJ,this);
			n = topbar + (e -> y - (getY() + y))/font -> sy;
			if(n >= maxbars) n = maxbars - 1;
			pb = pointer;
			pointer = getbar(n);
			if(XGR_MouseObj.LastPosY > XGR_MouseObj.PosY && topbar + visibars - 1 == getptr(pointer)){
				if(pointer -> next){
					topbar++;
					pointer = pointer -> next;
					}
				}
			else
				if(XGR_MouseObj.LastPosY < XGR_MouseObj.PosY && topbar == getptr(pointer)){
					if(pointer -> prev){
						topbar--;
						pointer = pointer -> prev;
						}
					}
			if(pointer != pb) draw();
			break;
		case E_LBMUNPRESS:
//			XGR_MouseObj.SetClip(sqScr -> x + XGR_MouseObj.SizeX/2, sqScr -> y + XGR_MouseObj.SizeY/2, sqScr -> x + sqScr -> sx - XGR_MouseObj.SizeX/2, sqScr -> y + sqScr -> sy - XGR_MouseObj.SizeY/2);
//			  XGR_MouseObj.left = sqScr -> x + XGR_MouseObj.SizeX/2;
//			  XGR_MouseObj.right = sqScr -> x + sqScr -> sx - XGR_MouseObj.SizeX/2;
//			  XGR_MouseObj.top = sqScr -> y + XGR_MouseObj.SizeY/2;
//			  XGR_MouseObj.bottom = sqScr -> y + sqScr -> sy - XGR_MouseObj.SizeY/2;
//			  XGR_MouseObj.change();
			break;
		case E_LBMDOUBLE:
			owner -> message(M_SETOBJ,this);
			n = topbar + (e -> y - (getY() + y))/font -> sy;
			if(n >= maxbars) n = maxbars - 1;
			pointer = getbar(n);
			draw();
			owner -> message(M_SETOPTION);
			break;
		}
}

void sqPopupMenu::activate(int force)
{
//	QuantObj = this;
	if(force) remove();
	set(1);
	if(force) draw();
}

void sqPopupMenu::deactivate(void)
{
	remove();
	set(2);
	draw();
}

void sqPopupMenu::seek(sqMenuBar* p0,int force)
{
	sqMenuBar* p = p0;
	while(p){
	    //@caiiiycuk TODO: was memicmp
		if(!memcmp(p -> data + margin,seekbuf,seeklen)){
			setpointer(p,force);
			return;
			}
		p = p -> next;
		}
	p = first;
	while(p != p0){
        //@scaiiiycuk TODO: was memicmp
		if(!memcmp(p -> data + margin,seekbuf,seeklen)){
			setpointer(p,force);
			return;
			}
		p = p -> next;
		}
}

void sqPopupMenu::setpointer(sqMenuBar* p,int force)
{
	int dif = getptr(pointer) - topbar;
	pointer = p;
	topbar = getptr(p) - dif;
	if(topbar < 0) topbar = 0;
	else topbar = MIN(topbar,maxbars - MIN(maxbars,visibars));
	if(force) draw();
}

void sqPopupMenu::free(void)
{
	sqMenuBar* p = first,*p0;
	while(p){
		p0 = p -> next;
		delete p;
		p = p0;
		}
	if(first){
		maxbars = topbar = nbar = seekcounter = 0;
		first = pointer = insert = NULL;
		len = len0;
		seeklen = 0;
		}
}

sqMultiMenu::sqMultiMenu(sqElem* _owner,int _x,int _y,int _visibars,sqFont* _font,int _maxlen)
{
	owner = _owner;
	x = _x; y = _y;
	maxlen = _maxlen;
	sx = _maxlen*_font -> sx + 2;
	sy = _font -> sy + 8 + _visibars*_font -> sy + 2;
	set(0);
	*this + (edit = new sqInputString(this,2,0,_font -> sx*(_maxlen + 1),_font,(unsigned char*)NULL,_maxlen));
	*this + (list = new sqPopupMenu(this,2,_font -> sy + 8,_visibars,_font,_maxlen + 2));
	obj = NULL;
	deleted = NULL;
}

void sqMultiMenu::keytrap(int key)
{
	obj -> keytrap(key);
}

void sqMultiMenu::activate(int force)
{
	obj = list;
	obj -> activate();
}

void sqMultiMenu::deactivate(void)
{
	obj -> deactivate();
	obj = NULL;
}

void sqMultiMenu::message(int code,sqElem* object)
{
	sqMenuBar* p;
	switch(code){
		case M_NEXTOBJ:
			obj -> deactivate();
			if(obj == edit){
				obj = list;
				obj -> activate();
				}
			else
				owner -> message(M_NEXTOBJ);
			break;
		case M_PREVOBJ:
			obj -> deactivate();
			if(obj == edit){
				obj = list;
				obj -> activate();
				}
			else
				owner -> message(M_PREVOBJ);
			break;
		case M_SETOBJ:
			if(object != obj){
				if((obj == edit && object == list) || (obj == list && object == edit))
					obj -> deactivate();
				else
					owner -> message(M_DEACTIVATE,this);
				obj = object;
				obj -> activate();
				}
			break;
		case M_CHOICE:
			if(obj == edit){
				list -> pointer -> replace(list,edit -> str);
				obj -> deactivate();
				obj = list;
				obj -> activate();
				}
			else {
				obj -> deactivate();
				obj = edit;
				obj -> activate();
				}
			break;
		case M_SETOPTION:
			list -> pointer -> status = 1 - list -> pointer -> status;
			if(list -> pointer -> status) *list -> pointer -> data = 4;
			else *list -> pointer -> data = ' ';
			list -> draw();
			break;
		case M_CHANGEOPTION:
			edit -> setstr(list -> pointer -> data + list -> margin);
			edit -> draw();
			owner -> message(M_CHANGEOPTION);
			break;
		case M_DELETE:
			if(list -> maxbars > 1){
				if(list -> pointer -> next) list -> insert = list -> pointer -> next;
				else list -> insert = list -> pointer -> prev;
				list -> remove();
				list -> disconnect(list -> pointer,&deleted);
				list -> pointer = list -> insert;
				list -> draw();
				}
			break;
		case M_INSERT:
			if(!edit -> insert) list -> insert = list -> pointer;
			else list -> insert = list -> pointer -> prev;
			*list * new sqMenuBar((unsigned char*)NULL,list);
			list -> pointer = list -> insert;
			list -> draw();
			message(M_CHOICE);
			break;
		case M_SETALL:
			p = list -> first;
			while(p){
				*p -> data = 4;
				p -> status = 1;
				p = p -> next;
				}
			list -> draw();
			break;
		case M_UNSETALL:
			p = list -> first;
			while(p){
				*p -> data = ' ';
				p -> status = 0;
				p = p -> next;
				}
			list -> draw();
			break;
		}
}

void sqMultiMenu::restore(int _x,int _y,int _sx,int _sy)
{
	XGR_Rectangle(_x,_y,_sx,_sy,SQ_SYSCOLOR + 4,SQ_SYSCOLOR + 4,XGR_FILLED);
}



