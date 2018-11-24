/* ---------------------------- INCLUDE SECTION ----------------------------- */
#include <zlib.h>
#include "../global.h"

#include "chtree.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */

typedef unsigned long ulong;

/*ulong ZIP_compress(char* trg,ulong trgsize,char* src,ulong srcsize);
ulong ZIP_GetExpandedSize(char* p);
void ZIP_expand(char* trg,ulong trgsize,char* src,ulong srcsize);*/

/* --------------------------- DEFINITION SECTION --------------------------- */

int aciCurCredits04 = 0;
int aciXData = 0x8383;

aciCHTreeElement::aciCHTreeElement(void)
{
	ID = ACT_ROOT;
	data = 0;
	nextLevel = new XList;
}

aciCHTreeElement::~aciCHTreeElement(void)
{
	aciCHTreeElement* p,*p1;

	p = (aciCHTreeElement*)nextLevel -> fPtr;
	while(p){
		p1 = (aciCHTreeElement*)p -> next;
		delete p;
		p = p1;
	}
	delete nextLevel;
}

aciCHTree::aciCHTree(void)
{
	rootEl = NULL;
	currentEl = NULL;
}

aciCHTree::~aciCHTree(void)
{
	delete rootEl;
}

void aciCHTree::add_string(char* str,int id)
{
	int i,i0,sz,v;
	aciCHTreeElement* curEl,*prevEl;

	sz = strlen(str);

	if(!rootEl){
		curEl = new aciCHTreeElement;
		curEl -> ID = ACT_ROOT;
		prevEl = rootEl = curEl;

		for(i = 0; i < sz; i ++){
			curEl = new aciCHTreeElement;
			curEl -> data = str[i];
			curEl -> data ^= aciXData;
			curEl -> ID = ACT_DATA;

			prevEl -> nextLevel -> AddElement((XListElement*)curEl);
			prevEl = curEl;
		}
		prevEl -> ID = id;
	}
	else {
		i0 = 0;
		curEl = prevEl = rootEl;
		while(curEl){
			prevEl = curEl;
			v = str[i0] ^ aciXData;
			curEl = curEl -> findNext(v);
			i0 ++;
		}
		for(i = i0 - 1; i < sz; i ++){
			curEl = new aciCHTreeElement;
			curEl -> data = str[i];
			curEl -> data ^= aciXData;
			curEl -> ID = ACT_DATA;

			prevEl -> nextLevel -> AddElement((XListElement*)curEl);
			prevEl = curEl;
		}
		prevEl -> ID = id;
	}
}

void aciCHTreeElement::save(XBuffer* fh)
{
	aciCHTreeElement* p;
	*fh < ID < data < nextLevel -> ListSize;

	p = (aciCHTreeElement*)nextLevel -> fPtr;
	while(p){
		p -> save(fh);
		p = (aciCHTreeElement*)p -> next;
	}
}

void aciCHTreeElement::load(XBuffer* fh)
{
	int i,sz;
	aciCHTreeElement* p;
	*fh > ID > data > sz;

	for(i = 0; i < sz; i ++){
		p = new aciCHTreeElement;
		p -> load(fh);
		nextLevel -> AddElement((XListElement*)p);
	}
}

aciCHTreeElement* aciCHTreeElement::findNext(int dt)
{
	aciCHTreeElement* p;

	p = (aciCHTreeElement*)nextLevel -> fPtr;
	while(p){
		if(p -> data == dt)
			return p;
		p = (aciCHTreeElement*)p -> next;
	}
	return NULL;
}

int aciCHTree::quant(int k)
{
	k ^= aciXData;
	if(!currentEl) currentEl = rootEl;
	currentEl = currentEl -> findNext(k);

	if(currentEl)
		return currentEl -> ID;

	return 0;
}

void aciCHTree::save(char* fname)
{
	int sz,sz1;
	XStream fh;
	char* p,*p1;
	XBuffer* XBuf = new XBuffer(64000);

	rootEl->save(XBuf);
	p = XBuf->address();
	sz1= sz = XBuf->tell();
	sz1+=12;
	p1 = new char[sz];

	//sz1 = ZIP_compress(p1,sz,p,sz);
	/* ZLIB realization (stalkerg)*/
	p1[0] = (char)(8 & 0xFF); //8 it is DEFLATE method
	p1[1] = (char)((8 >> 8) & 0xFF);
	*(unsigned int*)(p1 + 2) = (unsigned int)sz;
	sz1-=2+4;
	int stat = compress((Bytef*)(p1+2+4),(uLongf*)&(sz1),(Bytef*)p,sz);
	sz1+=2+4;
	switch(stat){
		case Z_OK: std::cout<<"Compress ok."<<std::endl; break;
		case Z_MEM_ERROR: std::cout<<"not enough memory."<<std::endl; break;
		case Z_BUF_ERROR: std::cout<<"not enough room in the output buffer."<<std::endl; break;
	};

	fh.open(fname, XS_OUT);
	fh.write(p1, sz1);
	fh.close();

	delete XBuf;
	delete[] p1;
}

void aciCHTree::load(const char* fname)
{
	long sz, sz1;
	char* p, *p1;
	XBuffer* XBuf;

	XStream fh(fname,XS_IN);
	sz = fh.size();
	p = new char[sz];
	fh.read(p,sz);
	fh.close();

	sz1 = *(unsigned int*)(p + 2) + 12;//ZIP_GetExpandedSize(p);
	p1 = new char[sz1];
	//ZIP_expand(p1,sz1,p,sz);
	/* ZLIB realisation (stalkerg)*/
	if(*(short*)(p)) { //if label = 0 not compress
		//std::cout<<"aciCHTree::load DeCompress "<<fname<<" file."<<std::endl;
		int stat = uncompress((Bytef*)p1,(uLongf*)&sz1,(Bytef*)(p+2+4),sz-2-4);
		switch(stat){
			//case Z_OK: std::cout<<"DeCompress ok."<<std::endl; break;
			case Z_MEM_ERROR: std::cout<<"DeCompress not enough memory."<<std::endl; break;
			case Z_BUF_ERROR: std::cout<<"DeCompress not enough room in the output buffer."<<std::endl; break;
			case Z_DATA_ERROR: std::cout<<"DeCompress error data."<<std::endl; break;
		};
	} else {
		memcpy(p1, p + 2 + 4,(unsigned)(sz - 2 - 4));
	}

	XBuf = new XBuffer(p1,sz1);

	rootEl = new aciCHTreeElement;
	rootEl -> load(XBuf);

	delete XBuf;
	delete[] p;
	delete[] p1;
}


