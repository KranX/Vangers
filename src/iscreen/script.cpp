/* ---------------------------- INCLUDE SECTION ----------------------------- */
#include <zlib.h>
#include "../global.h"


#include "s_mem.h"
#include "iscript.h"

#if defined(_BINARY_SCRIPT_) || defined(_SAVE_BINARY_SCRIPT_)
#include "zip.h"
#endif

/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void handle_error(const char* prefix,const char* subj = NULL);
int GetKeyCode(char* p);

int GetOptionID(char* p);
int aGetOptionID(char* p);
int acsGetOptionID(char* p);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define SCRIPT_CONV_BUF_SIZE		2048

#define MAX_SCAN_KEY		89

char* sMemHeap;
unsigned sHeapOffs = 0;
unsigned sHeapSize = 0;

#ifndef _BINARY_SCRIPT_
static char* scMathOpNames[MT_MAX_CODE] = { "+", "-", "*", "/", "(", ")" };
static int scMathOpLevels[MT_MAX_CODE] = { 0, 0, 1, 1, -1, -1 };
#ifdef _iSCRIPT_DEBUG_
int mtLogFile = 0;
#endif
#endif

#if defined(_SAVE_BINARY_SCRIPT_) || defined(_BINARY_SCRIPT_)
static const char* BSCR_SIGN = "BIN_SCR_1_00";
#endif

const int MAX_STRIP_CHAR = 6;
static char StripChars[MAX_STRIP_CHAR] = { ' ', '\r', '\t', ';', ',', '{' };

ScriptFile* script;
#ifndef _BINARY_SCRIPT_
scMathExpression* mExpr;
#endif

ScriptFileBuffer::ScriptFileBuffer(void)
{
	prev = next = NULL;
	buffer = NULL;
	offset = size = offset_size = 0;
}

ScriptFileBuffer::~ScriptFileBuffer(void)
{
	if(buffer) {
		delete[] buffer;
	}
}

ScriptLine::ScriptLine(void)
{
	init();
}

ScriptFile::ScriptFile(void)
{
	firstLine = lastLine = NULL;
	curBlock = NULL;
	fDef = lDef = NULL;

	CompressLog = 0;

	NumBufs = 0;
	fBuf = lBuf = NULL;

	buffer = NULL;

	NumStr = NumDef = 0;
	EOF_Flag = 0;
#ifdef _BINARY_SCRIPT_
	binConvPtr = NULL;
#endif
	binConvBuf = new char[SCRIPT_CONV_BUF_SIZE];
	binConvBufSize = 0;

#ifndef _BINARY_SCRIPT_
	prevStrBuf = new char[2048];
#endif

#ifdef _SAVE_BINARY_SCRIPT_
	bScriptName = NULL;
#endif
}

ScriptFile::~ScriptFile(void)
{
	ScriptLine* l,*line = lastLine;
	ScriptDefine* d,*def = lDef;

#ifndef _BINARY_SCRIPT_
	delete[] prevStrBuf;
#endif

	while(def){
		d = def -> prev;
		delete def;
		def = d;
	}

	while(line){
		l = line -> prev;
		delete line;
		line = l;
	}

	delete[] binConvBuf;

#ifdef _BINARY_SCRIPT_
	free_bscript();
#endif

	if(buffer) {
		delete[] buffer;
	}
}

void ScriptLine::init(void)
{
	first = last = NULL;
	Size = 0;
}

void ScriptFileBuffer::load(XStream* fh)
{
	if(buffer) delete buffer;
	size = fh -> size();
	buffer = new char[size];
	fh -> read(buffer,size);
}

void ScriptFileBuffer::save(XStream* fh)
{
	fh -> write(buffer,size);
}

void ScriptLine::merge(ScriptBlock* p,ScriptBlock* beg,ScriptBlock* end)
{
	ScriptBlock* p1,*tmp = beg;

	while(tmp != end){
		_sALLOC_(ScriptBlock,p1);
		_sALLOC_A_(char,p1 -> data,strlen(tmp -> data) + 1);
		strcpy(p1 -> data,tmp -> data);
		insert(p,p1);
		p = p -> next;
		tmp = tmp -> next;
	}
	_sALLOC_(ScriptBlock,p1);
	_sALLOC_A_(char,p1 -> data,strlen(tmp -> data) + 1);
	strcpy(p1 -> data,tmp -> data);
	insert(p,p1);
	p = p -> next;
}

void ScriptFile::load(const char* fname)
{
	XStream fh;
#ifdef _BINARY_SCRIPT_
	load_bscript(fname);
#else
	int inc_count = 0;
	fh.open(fname,XS_IN);
	Size = fh.size();

	buffer = new char[Size];
	fh.read(buffer,Size);

	fh.close();
	while(process_includes()){
		inc_count ++;
		if(inc_count > iSCRIPT_INCLUDE_MAX) ErrH.Abort("process_includes() failed...");
	}

#ifdef _SAVE_BINARY_SCRIPT_
	open_bscript(fname);
#endif
#endif

	cur_ptr = buffer;
	CurOffs = 0;
}

void ScriptFile::reset(void)
{
	CurOffs = 0;
	cur_ptr = buffer;
	EOF_Flag = 0;
	curLine = firstLine;
	if(curLine)
		curBlock = curLine -> first;
	else
		curBlock = NULL;
}

int ScriptFile::Seek(char* s)
{
	int i,sz = strlen(s);
	for(i = CurOffs; i <= Size - sz; i ++){
		if(*cur_ptr == *s){
			if(!strncmp(cur_ptr,s,sz))
				return 1;
		}
		cur_ptr ++;
		CurOffs ++;
	}
	return 0;
}

int ScriptFile::SeekPos(char* s)
{
	int i,sz = strlen(s);
	for(i = 0; i <= Size - (sz + CurOffs); i ++){
		if(*(cur_ptr + i) == *s){
			if(!strncmp(cur_ptr + i,s,sz))
				return i;
		}
	}
	return -1;
}

int ScriptFile::SeekComm(int mode)
{
	int i;
	char c1 = 0,c2 = 0;
	switch(mode){
		case C_COMMENT:
			c1 = '/';
			c2 = '*';
			break;
		case CPP_COMMENT:
			c1 = c2 = '/';
			break;
		case C_COMMENT_END:
			c1 = '*';
			c2 = '/';
			break;
		case CPP_COMMENT_END:
			c1 = '\r';
			c2 = '\n';
			break;
	}

	for(i = CurOffs; i <= Size - 2; i ++){
		if(*cur_ptr == c1){
			if(*(cur_ptr + 1) == c2)
				return 1;
		}
		cur_ptr ++;
		CurOffs ++;
	}
	return 0;
}

int ScriptFile::SeekCommPos(int mode)
{
	int i;
	char c1 = 0,c2 = 0;
	switch(mode){
		case C_COMMENT:
			c1 = '/';
			c2 = '*';
			break;
		case CPP_COMMENT:
			c1 = c2 = '/';
			break;
		case C_COMMENT_END:
			c1 = '*';
			c2 = '/';
			break;
		case CPP_COMMENT_END:
			c1 = '\r';
			c2 = '\n';
			break;
	}

	for(i = 0; i <= Size - (2 + CurOffs); i ++){
		if(*(cur_ptr + i) == c1){
			if(*(cur_ptr + i + 1) == c2)
				return i;
		}
	}
	return -1;
}

int ScriptFile::SeekChar(char s)
{
	int i;
	for(i = CurOffs; i <= Size - 1; i ++){
		if(*cur_ptr == s){
			return 1;
		}
		cur_ptr ++;
		CurOffs ++;
	}
	return 0;
}

int ScriptFile::SeekCharPos(char s)
{
	int i;
	for(i = 0; i <= Size - (1 + CurOffs); i ++){
		if(*(cur_ptr + i) == s){
			return i;
		}
	}
	return -1;
}

void ScriptFile::next(void)
{
	int sz;
	if((sz = strlen(cur_ptr)) != 0){
		cur_ptr += sz;
		CurOffs += sz;
	}
	while(!(*cur_ptr)){
		if(++CurOffs > Size - 1) return;
		cur_ptr++;
	}
}

void ScriptFile::strip_comments(void)
{
	int i,offs;
	while(SeekComm(CPP_COMMENT)){
		if((offs = SeekCommPos(CPP_COMMENT_END)) != -1){
			for(i = 0; i < offs + 2; i ++)
				if(cur_ptr[i] != '\n')
					cur_ptr[i] = 0;
			cur_ptr += offs + 2;
			CurOffs += offs + 2;
		}
		else
			handle_error("Bad script comment");

		if(CurOffs >= Size)
			break;
	}

	reset();
	while(SeekComm(C_COMMENT)){
		if((offs = SeekCommPos(C_COMMENT_END)) != -1){
			for(i = 0; i < offs + 2; i ++)
				if(cur_ptr[i] != '\n')
					cur_ptr[i] = 0;
			cur_ptr += offs + 2;
			CurOffs += offs + 2;
		}
		else
			handle_error("Bad script comment");
		if(CurOffs >= Size)
			break;
	}
	reset();
}

void ScriptFile::prepare(void)
{
#ifndef _BINARY_SCRIPT_
	int i,j,offs,flag;
	ScriptDefine* def;
	ScriptBlock* p,*p1;
	ScriptLine* line,*l;

	strip_comments();
	for(i = 0; i < Size; i ++){
		if(*cur_ptr == '\"'){
			cur_ptr ++;
			i ++;
			if((offs = SeekCharPos('\"')) != -1){
				cur_ptr += offs;
				i += offs;
			}
			else
				handle_error("Bad \"\" statement");
		}
		for(j = 0; j < MAX_STRIP_CHAR; j ++)
			if(*cur_ptr == StripChars[j]) *cur_ptr = 0;
		cur_ptr ++;
	}
	reset();
	preprocess();
	reset();

	while(!script -> EOF_Flag){
		if(curBlock && curBlock -> data && *curBlock -> data == '#' && !strcmp(curBlock -> data,"#define")){
			if(curLine -> Size < 2)
				handle_error("Bad define statement");
			add_def(curLine);
			lDef -> StartLine = curLine -> ID;
			def = lDef -> prev;
			while(def){
				if(!strcmp(def -> line -> first -> next -> data,lDef -> line -> first -> next -> data)){
					def -> EndLine = lDef -> StartLine;
					def = NULL;
				}
				else
					def = def -> prev;
			}
			while(*lDef -> line -> last -> data == '\\'){
				lDef -> line -> last -> clear();
				if(!script -> EOF_Flag){
					script -> next_line();
					lDef -> add(curLine);
				}
			}
		}
		script -> next_line();
	}
	reset();
	while(!script -> EOF_Flag){
		if(curBlock && curBlock -> data && *curBlock -> data == '$'){
			def = lDef;
			flag = 0;
			while(def){
				if(curLine -> ID >= def -> StartLine && curLine -> ID < def -> EndLine && !strcmp(curBlock -> data + 1,def -> line -> first -> next -> data)){
					flag = 1;
					curBlock -> clear();
					if(def -> line -> Size > 2)
						curLine -> merge(curBlock,def -> line -> first -> next -> next,def -> line -> last);
					def = NULL;
				}
				else
					def = def -> prev;
			}
			if(!flag) handle_error("Bad define",curBlock -> data + 1);
		}
		next_ptr();
	}
	reset();
	for(i = 0; i < NumStr; i ++){
		p = curLine -> last;
		while(p){
			p1 = p -> prev;
			if(!(*p -> data)){
				curLine -> dconnect(p);
			}
			p = p1;
		}
		curLine = curLine -> next;
	}
	reset();

	for(i = 0; i < NumStr; i ++){
		p = curLine -> last;
		while(p){
			p1 = p -> prev;
			if(*p -> data == '\"'){
				*p -> data = 0;
				p -> data ++;
				if(*(p -> data + strlen(p -> data) - 1) == '\"')
					*(p -> data + strlen(p -> data) - 1) = 0;
			}
			p = p1;
		}
		curLine = curLine -> next;
	}
	reset();

	for(i = 0; i < NumStr; i ++){
		p = curLine -> last;
		while(p){
			p1 = p -> prev;
			if(*p -> data == '\"'){
				*p -> data = 0;
				p -> data ++;
				if(*(p -> data + strlen(p -> data) - 1) == '\"')
					*(p -> data + strlen(p -> data) - 1) = 0;
			}
			p = p1;
		}
		curLine = curLine -> next;
	}
	reset();

	for(i = 0; i < Size; i ++){
		if(*cur_ptr == '\"')
			*cur_ptr = 0;
		cur_ptr ++;
	}
	reset();

	line = curLine;
	for(i = 0; i < NumStr; i ++){
		l = line -> next;
		if(!line -> Size){
			remove_line(line);
			delete line;
		}
		line = l;
	}
	reset();

#ifdef _iSCRIPT_DEBUG_
	write_log();
#endif
#endif
}

int ScriptFile::check_math(void)
{
	if(EOF_Flag) return 0;

#ifndef _BINARY_SCRIPT_
	int i;
	if(!curBlock || !curBlock -> data) return 0;
	for(i = 0; i < MT_MAX_CODE; i ++){
		if(*curBlock -> data == *scMathOpNames[i] && !strcmp(curBlock -> data,scMathOpNames[i])) return i + 1;
	}
#endif
	return 0;
}

void ScriptLine::connect(ScriptBlock* p)
{
	if(last){
		last -> next = p;
		p -> prev = last;
		p -> next = first;
		last  = p;
	}
	else{
		last = p;
		first = p;
		p -> next = p;
		p -> prev = 0;
	}
	Size ++;
}

void ScriptLine::dconnect(ScriptBlock* p)
{
	if(p -> next -> prev)
		p -> next -> prev = p -> prev;
	else
		last = p -> prev;
	if(p -> prev)
		p -> prev -> next = p -> next;
	else {
		first = p -> next;
		last -> next = p -> next;
	}
	Size --;
}

void ScriptLine::insert(ScriptBlock* p,ScriptBlock* p1)
{
	if(p == last){
		p -> next = p1;
		p1 -> prev = p;
		p1 -> next = first;
		last = p1;
	}
	else {
		p1 -> next = p -> next;
		p1 -> prev = p;
		p -> next -> prev = p1;
		p -> next = p1;
	}
	Size ++;
}

void ScriptFile::add_line(ScriptLine* p)
{
	if(lastLine){
		lastLine -> next = p;
		p -> prev = lastLine;
		p -> next = firstLine;
		lastLine  = p;
	}
	else{
		lastLine = p;
		firstLine = p;
		p -> next = p;
		p -> prev = 0;
	}
	NumStr ++;
}

void ScriptFile::add_buf(ScriptFileBuffer* p)
{
	if(lBuf){
		lBuf -> next = p;
		p -> prev = lBuf;
		p -> next = fBuf;
		lBuf  = p;
	}
	else{
		lBuf = p;
		fBuf = p;
		p -> next = p;
		p -> prev = 0;
	}
	NumBufs ++;
}

void ScriptFile::free_buffers(void)
{
	ScriptFileBuffer* p,*p1;
	p = lBuf;
	while(p){
		p1 = p -> prev;
		delete p;
		p = p1;
	}
	fBuf = lBuf = NULL;
	NumBufs = 0;
}

void ScriptFile::remove_line(ScriptLine* p)
{
	if(p -> next -> prev)
		p -> next -> prev = p -> prev;
	else
		lastLine = p -> prev;
	if(p -> prev)
		p -> prev -> next = p -> next;
	else {
		firstLine = p -> next;
		lastLine-> next = p -> next;
	}
	NumStr --;
}

void ScriptFile::preprocess(void)
{
	int cur_line_id = 0;
	ScriptLine* line = NULL;
	ScriptBlock* p;

	if(!(*cur_ptr))
		next();

	line = new ScriptLine;
	line -> ID = cur_line_id;

	while(CurOffs < Size){
		if(*cur_ptr == '\n'){
			*cur_ptr = 0;
			cur_line_id ++;
			add_line(line);
			line = new ScriptLine;
			line -> ID = cur_line_id;
		}
		else {
			_sALLOC_(ScriptBlock,p);
			p -> data = cur_ptr;
			line -> connect(p);
		}
		next();
	}
}

char* ScriptFile::get_ptr(void)
{
#ifndef _BINARY_SCRIPT_
	if(curBlock){
		strcpy(prevStrBuf,curBlock -> data);
		return curBlock -> data;
	}
	return NULL;
#else
	return binConvBuf;
#endif
}

char* ScriptFile::get_conv_ptr(void)
{
	return binConvBuf;
}

int ScriptFile::get_cur_str(void)
{
	return curLine -> ID;
}

void ScriptFile::next_ptr(void)
{
	if(curBlock)
		curBlock = curBlock -> next;
	if(curBlock == curLine -> first){
		curLine = curLine -> next;
		curBlock = curLine -> first;
		if(curLine == firstLine)
			EOF_Flag = 1;
	}
}

void ScriptFile::next_line(void)
{
	if(curLine){
		curLine = curLine -> next;
		curBlock = curLine -> first;
		if(curLine == firstLine)
			EOF_Flag = 1;
	}
}

#ifdef _iSCRIPT_DEBUG_
void ScriptFile::write_log(void)
{
	int i,j;
	ScriptBlock* p;
	ScriptLine* line = firstLine;
	XStream fh("ISCREEN\\setup.dat",XS_OUT);
	for(i = 0; i < NumStr; i ++){
		p = line -> first;
		if(line -> Size){
			fh <= line -> ID < ":\t";
			for(j = 0; j < line -> Size; j ++){
				fh < p -> data < " ";
				p = p -> next;
			}
			fh < "\r\n";
		}
		line = line -> next;
	}
	fh.close();
}
#endif

ScriptDefine::ScriptDefine(void)
{
	line = new ScriptLine;
	line -> ID = -1;
	prev = next = NULL;
}

ScriptDefine::~ScriptDefine(void)
{
	delete line;
}

void ScriptFile::add_def(ScriptLine* p)
{
	ScriptDefine* d = new ScriptDefine;
	d -> add(p);
	d -> EndLine = NumStr;

	if(lDef){
		lDef -> next = d;
		d -> prev = lDef;
		d -> next = fDef;
		lDef  = d;
	}
	else{
		lDef = d;
		fDef = d;
		d -> next = d;
		d -> prev = NULL;
	}
	NumDef ++;
}

void ScriptDefine::add(ScriptLine* p)
{
	if(line -> ID == -1 || !line -> Size){
		line -> Size = p -> Size;
		line -> first = p -> first;
		line -> last = p -> last;
		line -> ID = p -> ID;
	}
	else {
		line -> Size += p -> Size;
		line -> last -> next = p -> first;
		p -> first -> prev = line -> last;
		line -> last = p -> last;
		line -> last -> next = line -> first;
		line -> ID = p -> ID;
	}

	p -> Size = 0;
	p -> first = p -> last = NULL;
}

void ScriptBlock::clear(void)
{
	memset(data,0,strlen(data));
}

void handle_error(const char* prefix, const char* subj)
{
	XBuffer XBuf;
#ifndef _BINARY_SCRIPT_
	XBuf < prefix < "...";
	ErrH.Abort(XBuf.GetBuf(),XERR_USER,script -> get_cur_str() + 1,subj);
#else
	XBuf < prefix < "...";
	ErrH.Abort(XBuf.GetBuf(),XERR_USER,script -> binConv -> tell(),subj);
#endif
}

#ifdef _SAVE_BINARY_SCRIPT_
void ScriptFile::open_bscript(const char* fname)
{
	if(!bScriptName){
		bScriptName = new char[256];
		strcpy(bScriptName,fname);
		memcpy(bScriptName + strlen(bScriptName) - 3,"scb",3);
	}

	bin_fh = new XStream;
	bin_fh -> open(bScriptName,XS_OUT);

#ifndef _SAVE_PACKED_SCRIPT_
	*bin_fh < BSCR_SIGN < '\0' < CompressLog;
#endif
}

void ScriptFile::write_end(void)
{
	write_idata(iSCRIPT_EOF);
}

void ScriptFile::close_bscript(void)
{
#ifndef _BINARY_SCRIPT_
	write_end();
	bin_fh -> close();

#ifdef _SAVE_PACKED_SCRIPT_
	int sz,out_sz;
	char* p,*p1;

	bin_fh -> open(bScriptName,XS_IN);
	sz = bin_fh -> size();
	p = new char[sz];
	p1 = new char[sz];

	bin_fh -> read(p,sz);
	bin_fh -> close();

	out_sz = ZIP_compress(p1,sz,p,sz);
	if(!out_sz) ErrH.Abort("Bad compressed script size...");

	CompressLog = 1;
	bin_fh -> open(bScriptName,XS_OUT);
	*bin_fh < BSCR_SIGN < '\0' < CompressLog;
	bin_fh -> write(p1,out_sz);
	bin_fh -> close();

	delete p;
	delete p1;
#endif

	delete bin_fh;
#endif
}

void ScriptFile::write_idata(int val)
{
	*bin_fh < val;
}

void ScriptFile::write_pdata(char* p)
{
	int sz = strlen(p);
	*bin_fh < sz < p < '\0';
}
#endif

#ifdef _BINARY_SCRIPT_
void ScriptFile::load_bscript(const char* fname)
{
	int sz,out_sz;
	char* p,*p1;

	XStream fh;

	char* s;
	char* buf;

	s = new char[256];

	strcpy(s,fname);
	memcpy(s + strlen(s) - 3,"scb",3);

	fh.open(s,XS_IN);
	sz = fh.size();
	buf = new char[sz];
	fh.read(buf,sz);
	fh.close();

	std::cout<<"ScriptFile::load_bscript"<<fname<<" file."<<std::endl;
	
	binConv = new XBuffer(buf,sz);

	if(!check_bsign())
		ErrH.Abort("Wrong bscript file",XERR_USER,0,s);

	if(CompressLog){
		std::cout<<"ScriptFile::load_bscript DeCompress "<<s<<" file."<<std::endl;
		sz -= strlen(BSCR_SIGN) + 1 + sizeof(int);
		p = buf + strlen(BSCR_SIGN) + 1 + sizeof(int);

		out_sz = *(unsigned int*)(p + 2) + 12;//ZIP_GetExpandedSize(p);
		
		p1 = new char[out_sz];
		//ZIP_expand(p1,out_sz,p,sz);
		/* ZLIB realisation (stalkerg)*/
		if(*(short*)(p)) { //if label = 0 not compress
			int stat = uncompress((Bytef*)p1,(uLongf*)&out_sz,(Bytef*)(p+2+4),sz-2-4);
			switch(stat){
				case Z_OK: std::cout<<"DeCompress ok."<<std::endl; break;
				case Z_MEM_ERROR: std::cout<<"DeCompress not enough memory."<<std::endl; break;
				case Z_BUF_ERROR: std::cout<<"DeCompress not enough room in the output buffer."<<std::endl; break;
				case Z_DATA_ERROR: std::cout<<"DeCompress error data."<<std::endl; break;
			};
		} else {
			memcpy(p1, p + 2 + 4,(unsigned)(sz - 2 - 4));
		}
		delete binConv;
		delete[] buf;
		binConv = new XBuffer(p1,out_sz);
		binConvPtr = p1;
	}

	delete[] s;
}

void ScriptFile::free_bscript(void)
{
	delete binConv;
	if(binConvPtr)
		delete[] binConvPtr;
}

int ScriptFile::check_bsign(void)
{
	int ret_val = 0;
	char* p = new char[256];
	*binConv > p > CompressLog;
	if(!strcmp(p,BSCR_SIGN)) ret_val = 1;
	delete[] p;

	return ret_val;
}
#endif

int ScriptFile::read_idata(void)
{
	int val;
#ifdef _BINARY_SCRIPT_
	*binConv > val;

	if(binConv -> end())
		EOF_Flag = 1;
#else
	val = parse_math();

#ifdef _SAVE_BINARY_SCRIPT_
	write_idata(val);
#endif
#endif
	return val;
}

int ScriptFile::parse_math(void)
{
#ifndef _BINARY_SCRIPT_
	int val = 0,op_code = 0;
	scMathExpression* m;

	ScriptBlock* p = curBlock;
	val = check_math();
	next_ptr();
	op_code = check_math();

	if(val || op_code){
		m = new scMathExpression;
		m -> add2list(p);
	}
	else {
		return atoi(p -> data);
	}
	val = 1;
	while(val){
		m -> add2list(curBlock);
		next_ptr();
		if(*curBlock -> data == '}' ){
			next_ptr();
			val = 0;
		}
	}

	m -> prepare_list();
	val = m -> eval();
	delete m;
	return val;
#else
	return 0;
#endif
}

int ScriptFile::get_idata(void)
{
	return binConvData;
}

void ScriptFile::read_pdata(char** ptr,int mode)
{
	int sz;
#ifdef _BINARY_SCRIPT_
	*binConv > sz;
	if(mode)
		ptr[0] = new char[sz + 1];

	*binConv > ptr[0];

	if(binConv -> end())
		EOF_Flag = 1;
#else
	sz = strlen(get_ptr());
	if(mode)
		ptr[0] = new char[sz + 1];

	strcpy(ptr[0],get_ptr());
	next_ptr();
#ifdef _SAVE_BINARY_SCRIPT_
	write_pdata(ptr[0]);
#endif
#endif
}

void ScriptFile::get_pdata(char** ptr,int mode)
{
	int sz = binConvBufSize;
	if(mode)
		ptr[0] = new char[sz + 1];

	strcpy(ptr[0],binConvBuf);
}

char* ScriptFile::get_pdata_ptr(void)
{
	return binConvBuf;
}

void ScriptFile::prepare_pdata(void)
{
#ifdef _BINARY_SCRIPT_
	*binConv > binConvBufSize;
	*binConv > binConvBuf;

	if(binConv -> end())
		EOF_Flag = 1;
#else
	binConvBufSize = strlen(get_ptr());
	if(binConvBufSize >= SCRIPT_CONV_BUF_SIZE)
		ErrH.Abort("Script buffer overflow...");
	strcpy(binConvBuf,get_ptr());

	next_ptr();
#ifdef _SAVE_BINARY_SCRIPT_
	write_pdata(binConvBuf);
#endif
#endif
}

void ScriptFile::prepare_idata(void)
{
#ifdef _BINARY_SCRIPT_
	*binConv > binConvData;

	if(binConv -> end())
		EOF_Flag = 1;
#else
	binConvData = parse_math();
#ifdef _SAVE_BINARY_SCRIPT_
	write_idata(binConvData);
#endif
#endif
}

int ScriptFile::read_option(int mode)
{
	int val = -1;
#ifdef _BINARY_SCRIPT_
	val = read_idata();
#else
	if(mode == 0)
		val = aGetOptionID(get_ptr());
	if(mode == 1)
		val = GetOptionID(get_ptr());
	if(mode == -1)
		val = acsGetOptionID(get_ptr());
	next_ptr();
#ifdef _SAVE_BINARY_SCRIPT_
	write_idata(val);
#endif
#endif
	return val;
}

int ScriptFile::read_key(void)
{
	int val = 0;
#ifdef _BINARY_SCRIPT_
	val = read_idata();
#else
	val = GetKeyCode(get_ptr());
	next_ptr();
#ifdef _SAVE_BINARY_SCRIPT_
	write_idata(val);
#endif
#endif
	return val;
}

int GetKeyCode(char* p)
{
	char* stopstr = p;
	return strtol(p,&stopstr,16);
}

void ScriptFile::next_src_line(void)
{
	while(*cur_ptr == '\n'){
		CurOffs ++;
		cur_ptr ++;
	}
	while(*cur_ptr != '\n'){
		if(++CurOffs > Size - 1) return;
		cur_ptr++;
	}
	if(*cur_ptr == '\n'){
		CurOffs ++;
		cur_ptr ++;
	}
}

void ScriptFile::read_src_line(void)
{
	int i = 0;
	memset(binConvBuf,0,SCRIPT_CONV_BUF_SIZE);
	while(cur_ptr[i] != '\n'){
		if(i + CurOffs > Size - 1) return;
		if(i >= SCRIPT_CONV_BUF_SIZE)
			ErrH.Abort("Script buffer overflow...");
		binConvBuf[i] = cur_ptr[i];
		i++;
	}
}

static const char* INC_STR = "#include";
int ScriptFile::process_includes(void)
{
	int ret = 0;
	char* p,*p1,*p2;
	XStream fh;
	ScriptFileBuffer* b,*b1;

	int i,offs = 0,sz = strlen(INC_STR);

	reset();
	while(CurOffs < Size - 1){
		read_src_line();
		p = strchr(binConvBuf,'#');
		if(p){
			if(!strncmp(p,INC_STR,sz)){
				p1 = strchr(p,'\"');
				if(!p1) ErrH.Abort("Bad include line...");
				p1 ++;

				p2 = strchr(p1,'\"');
				if(!p2) ErrH.Abort("Bad include line...");
				*p2 = 0;

				b = new ScriptFileBuffer;
				fh.open(p1,XS_IN);
				b -> load(&fh);
				fh.close();
				b -> offset = CurOffs;
				next_src_line();
				b -> offset_size = CurOffs - b -> offset;

				add_buf(b);
				ret = 1;
			}
			else
				next_src_line();
		}
		else
			next_src_line();
	}
	reset();
	b = fBuf;
	if(!NumBufs) return 0;

	fh.open("iscreen/temp.scr",XS_OUT);
	for(i = 0; i < NumBufs; i ++){
		if(offs != b -> offset)
			fh.write(buffer + offs,b -> offset - offs);
		offs = b -> offset + b -> offset_size;
		b -> save(&fh);

		b = b -> next;
	}
	fh.write(buffer + offs,Size - offs);
	fh.close();
	delete[] buffer;

	b = lBuf;
	while(b){
		b1 = b -> prev;
		delete b;
		b = b1;
	}
	NumBufs = 0;
	fBuf = lBuf = NULL;

	fh.open("iscreen/temp.scr",XS_IN);
	Size = fh.size();
	buffer = new char[Size];
	fh.read(buffer,Size);
	fh.close();
	reset();

	return ret;
}

#ifndef _BINARY_SCRIPT_
scMathExpression::scMathExpression(void)
{
	clear();
}

scMathExpression::~scMathExpression(void)
{
	free_list();
}

void scMathExpression::free_list(void)
{
	scMathNode* p,*p1;

	p = last;
	while(p){
		p1 = p -> prev;
		delete p;
		p = p1;
	}
	clear();
}

void scMathExpression::add2list(ScriptBlock* p)
{
	int code;
	scMathNode* n = new scMathNode;
	code = check_math(p -> data);
	if(code)
		n -> op_code = code - 1;
	else {
		n -> value = atoi(p -> data);
	}
	connect(n);
}

void scMathExpression::connect(scMathNode* p)
{
	if(last){
		last -> next = p;
		p -> prev = last;
		p -> next = first;
		last  = p;
	}
	else{
		last = p;
		first = p;
		p -> next = p;
		p -> prev = 0;
	}
	Size ++;
}

void scMathExpression::dconnect(scMathNode* p)
{
	Size --;
	if(!Size){
		last = first = NULL;
		return;
	}
	if(p -> next -> prev)
		p -> next -> prev = p -> prev;
	else
		last = p -> prev;
	if(p -> prev)
		p -> prev -> next = p -> next;
	else {
		first = p -> next;
		last -> next = p -> next;
	}
}

int scMathExpression::check_math(char* p)
{
	int i;
	for(i = 0; i < MT_MAX_CODE; i ++){
		if(*p == *scMathOpNames[i] && !strcmp(p,scMathOpNames[i])) return (i + 1);
	}
	return 0;
}

void scMathExpression::prepare_list(void)
{
	int i,delta = 0;
	scMathNode* p,*p1;

	p = first;
	for(i = 0; i < Size; i ++){
		if(p -> op_code != -1){
			if(p -> op_code == MT_LEFT_BR){
				delta += 2;
			}
			else {
				if(p -> op_code == MT_RIGHT_BR){
					delta -= 2;
				}
				else {
					p -> value = delta + scMathOpLevels[p -> op_code];
					if(MaxValue < p -> value) MaxValue = p -> value;
				}
			}
		}
		p = p -> next;
	}
	if(delta)
		handle_error("Math expression error");
	p = last;
	while(p){
		p1 = p -> prev;
		if(p -> op_code == MT_LEFT_BR || p -> op_code == MT_RIGHT_BR){
			dconnect(p);
			delete p;
		}
		p = p1;
	}
}

int scMathExpression::eval(void)
{
	if(!Size) return 0;
#ifdef _iSCRIPT_DEBUG_
	int file_out = 0;
	if(Size > 1){
		if(mtLogFile){
			logFile.open("ISCREEN\\math.dat",XS_OUT | XS_APPEND | XS_NOREPLACE);
		}
		else {
			mtLogFile = 1;
			logFile.open("ISCREEN\\math.dat",XS_OUT);
		}
		logFile < "\r\n---------------------- EXPRESSION START ----------------------\r\n";
		write();
		file_out = 1;
	}
#endif
	while(Size > 1){
		eval_quant();
	}

#ifdef _iSCRIPT_DEBUG_
	if(file_out){
		logFile < "----------------------- EXPRESSION END -----------------------\r\n";
		logFile.close();
	}
#endif
	return first -> value;
}

void scMathExpression::eval_quant(void)
{
	int flag = 0;
	scMathNode* p,*pp,*pn;
	p = last;
	while(p){
		if(p -> op_code != -1 && p -> value == MaxValue){
			pp = p -> prev;
			pn = p -> next;
			if(!pp || pn == first || pp -> op_code != -1 || pn -> op_code != -1)
				handle_error("Math expression error");
#ifdef _iSCRIPT_DEBUG_
			logFile < "Step -> " <= pp -> value < " " < scMathOpNames[p -> op_code] < " " <= pn -> value < "\r\n";
#endif
			switch(p -> op_code){
				case MT_PLUS:
					pp -> value += pn -> value;
					break;
				case MT_MINUS:
					pp -> value -= pn -> value;
					break;
				case MT_MUL:
					pp -> value *= pn -> value;
					break;
				case MT_DIV:
					pp -> value /= pn -> value;
					break;
			}
			dconnect(p);
			dconnect(pn);
			delete p;
			delete pn;
			break;
		}
		p = p -> prev;
	}
	p = last;
	while(p){
		if(p -> op_code != -1 && p -> value == MaxValue) flag = 1;
		p = p -> prev;
	}
	if(!flag && MaxValue) MaxValue --;
}

#ifdef _iSCRIPT_DEBUG_
void scMathExpression::write(void)
{
	int i;
	scMathNode* p = first;
	logFile <= script -> get_cur_str() < ":\t";
	for(i = 0; i < Size; i ++){
		if(p -> op_code == -1)
			logFile <= p -> value;
		else
			logFile < " " < scMathOpNames[p -> op_code] < " ";
		p = p -> next;
	}
	logFile < "\r\n";
}
#endif

#endif

#ifdef _SAVE_BINARY_SCRIPT_
void ScriptFile::set_bscript_name(char* fname)
{
	int sz = strlen(fname) + 1;
	bScriptName = new char[sz];
	strcpy(bScriptName,fname);
}
#endif


