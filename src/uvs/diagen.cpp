/*
	Dialogue Generator Core Module
	(C)1997 by K-D Lab, VANGERS Project
	Creator: K-D Lab::KranK
*/

#include "../global.h"
#include "../lang.h"

#include "../common.h"
#include "diagen.h"

#include "../actint/mlconsts.h"
#include "../actint/credits.h"

#include "../3d/parser.h"
#include "../units/uvsapi.h"
#include "../units/compas.h"
#include "univang.h"


/*#ifdef DIAGEN_TEST
#	include "../diagen/inp.h"
#endif*/


#ifdef DIAGEN_TEST
//#define RUSSIAN_VERSION
//#define GERMAN_VERSION
#endif

#define STARTUP_TESTING
#define SAVE_SINGE_LANGUAGE

#define SINGLE_LANGUAGE

#define DGF_NONE		0
#define DGF_DUAL		1

#define SUBJ_SYMBOL			'['
#define SUBJ_SYMBOL_RIGHT	']'
#define LINK_SYMBOL			'{'
#define LINK_SYMBOL_RIGHT	'}'
#define COMMAND_SYMBOL		'@'

struct FileBox {
	int cN;
	char** names;
	char** data;
	int* lens;

		FileBox(void){	cN = 0;
#ifdef DIAGEN_TEST
			memset(names = new char*[256],0,256*4);
			memset(data = new char*[256],0,256*4);
			memset(lens = new int[256],0,256*4);
#endif
			};

	void load(void);
	char* get(char* fname,int& len);
#ifdef DIAGEN_TEST
	void add(char* fname,char* buf,int len);
	void save(void);
#endif
	};

/* --------------------------- PROTOTYPE SECTION --------------------------- */
extern int RACE_WAIT;
//extern int aciCurCredits;
extern int aciEscaveDead;
extern int aciEscaveEmpty;
extern int NetworkON;
extern int uvsCurrentWorldUnableBefore;

//#ifndef DIAGEN_TEST
extern uvsGamerResult GamerResult;
//#endif

void dg_SendEvent(int code);
void aciOpenWorldLink(int,int);
void uvsSetTownName(const char* _name_ ); 
char* uvsGetLarvaWorld(int n);
int uvsgetDGdata(int code);
int getPreviousCycle(int cycle);
int isCycleExist(int cycle,char ew = 0);
int getWorldVisitStats(char wc);
int getPassangerStats(char efrom, char wto);
int getPassangerFrom(char efrom);
char* StringOfBoozeeniada(void);
int getSpobsState(void);
int getThreallState(void);
void addInfernalsKill2Protractor(void);
void addSpectorsKill2Messiah(void);
int isSpummyDeath(void);
void OpenCrypt(int world,int id);
void uvsBoorawchickZero(void);
int aciWorldLinkExist(int w1,int w2,int flag = 0);
void uvsLuckChange(int val);

void dgLog(char* string);
void dataList(int val = -1);
void loadList(void);
void saveList(void);
void showmap(void);
/* --------------------------- DEFINITION SECTION -------------------------- */
static char dgSyntaxError[] = "dgFile syntax error";

const int saveVersion = 3;

int MP_GAME = 0;

DiagenDispatcher* dgD;
int dgMood;
int dgAbortStatus;

static int* dgLevel;
static int enter_sStatus;
static int Qcode,checkStatus,ClearStatus,singleRandAvailable;
static int wasRubeecated;

static int isLoading;
static int loadedVersion;
static int oldVersion;
static int solidQ;
static int eventOUT;

static FileBox* FBox;

const char *DiagenTextName() {

    if (lang() == RUSSIAN) {
        return "diagen.text";
    }

    return "diagen_eng.text";
}
//DiagenDispatcher* nDD;

#ifdef DIAGEN_TEST
int DGdata[DG_EXTERNS::MAX];
//int NetworkON = 0;
//int aciEscaveDead;
//int aciEscaveEmpty;
#endif

union dual_char {
	char c[2];
	unsigned short sh;
	};

char* ConvertUTF8(const char* s,int back = 0) {
	static char* buffer = NULL;
	int len = strlen(s);
	buffer = new char[len+2];
	memset( buffer, 0, len+2);
	dual_char dc;
	bool get_dual=false;
	int i_dual_char=0, i;
	if (!back)
		for (i=0;i<len;i++)
			{
			//NOT IMPLEMENT NOW
			/*if((unsigned char)s[i]<128 && !get_dual)
				std::cout<<" "<<(int)s[i];
			if((unsigned char)s[i]>127 || get_dual)
				{
				dc.c[i_dual_char]=s[i];
				if(get_dual)
					{
					get_dual=false;
					i_dual_char=0;
					std::cout<<" "<<std::dec<<dc.sh;
					std::cout<<"-"<<std::hex<<(int)dc.c[0]<<(int)dc.c[1];
					}
				else
					{
					get_dual=true;
					i_dual_char++;
					}
				}*/
			buffer[i]=s[i];
			}
	else
		std::cout<<"ERROR:CP866 to UTF8 not implement!!!"<<std::endl;
	//std::cout<<std::endl;
	//std::cout<<"A:"<<(unsigned char*)buffer<<std::endl;
	return buffer;
}

char* Convert(const char* s,int back = 0) {
	if(!(lang() == RUSSIAN)) {
		char* buffer = NULL;
		int len = strlen(s)+1;
		buffer = new char[len + 2];
		memset(buffer, 0, len+2);
		strcpy(buffer, s);
		return buffer;
	}
	static int len = 0;
	static char* buffer = NULL;
	len = strlen(s)+1;
	buffer = new char[len + 2];
	memset(buffer, 0, len+2);
	int i;
	if (!back) {
		for (i=0;i<len;i++) {
			if ((unsigned char)s[i]>=192&&(unsigned char)s[i]<=239)
				buffer[i]=(char)((unsigned char)s[i]-64);
			else if ((unsigned char)s[i]>=240&&(unsigned char)s[i]<=255)
				buffer[i]=(char)((unsigned char)s[i]-16);
			else if ((unsigned char)s[i]==184)
				buffer[i]=(char)((unsigned char)241); //маленькое ё
			else if ((unsigned char)s[i]==168)
				buffer[i]=(char)((unsigned char)240); //большое Ё
			else if ((unsigned char)s[i]==185)
				buffer[i]=(char)((unsigned char)252); //знак №
			else if ((unsigned char)s[i]<128)
				buffer[i]=s[i];
			else
				buffer[i]=' ';
			
		}
	} else {
		for (i=0;i<len;i++) {
			if ((unsigned char)s[i]>=128&&(unsigned char)s[i]<=175)
				buffer[i]=(char)((unsigned char)s[i]+64);
			else if ((unsigned char)s[i]>=224&&(unsigned char)s[i]<=239)
				buffer[i]=(char)((unsigned char)s[i]+16);
			else if ((unsigned char)s[i]==241)
				buffer[i]=(char)((unsigned char)184); //маленькое ё
			else if ((unsigned char)s[i]==240)
				buffer[i]=(char)((unsigned char)168); //большое Ё
			else if ((unsigned char)s[i]==252)
				buffer[i]=(char)((unsigned char)185); //знак №
			else if ((unsigned char)s[i]<128)
				buffer[i]=s[i];
			else
				buffer[i]=' ';
		}
	}
	return buffer;
}

std::string cp866_to_cp1251(std::string in) {
	if(lang() != RUSSIAN) {
		return in;
	}
#ifdef WIN32
	return in;
#endif
	unsigned int i;
	for (i = 0; i < in.length(); i++) {
		if ((unsigned char)in[i]>=128&&(unsigned char)in[i]<=175)
			in[i]=(char)((unsigned char)in[i]+64);
		else if ((unsigned char)in[i]>=224&&(unsigned char)in[i]<=239)
			in[i]=(char)((unsigned char)in[i]+16);
		else if ((unsigned char)in[i]==241)
			in[i]=(char)((unsigned char)184); //маленькое ё
		else if ((unsigned char)in[i]==240)
			in[i]=(char)((unsigned char)168); //большое Ё
		else if ((unsigned char)in[i]==252)
			in[i]=(char)((unsigned char)185); //знак №
		else if ((unsigned char)in[i]<128)
			in[i]=in[i];
		else
			in[i]=' ';
	}
	return in;
}

std::string cp1251_to_utf8(std::string in_string) {
	if(lang() != RUSSIAN) {
		return in_string;
	}
#ifdef WIN32
	return in_string;
#endif
	
	char *out_mem = new char[in_string.length()*3+3];
	char *out = out_mem;
	const char *in = in_string.c_str();
	
	static const int table[128] = {
        0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
        0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
        0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
        0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
        0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,
        0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,
        0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,
        0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,
        0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
        0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
        0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
        0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
        0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
        0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
        0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
        0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
    };
    while (*in)
        if (*in & 0x80) {
            int v = table[(int)(0x7f & *in++)];
            if (!v)
                continue;
            *out++ = (char)v;
            *out++ = (char)(v >> 8);
            if (v >>= 16)
                *out++ = (char)v;
        }
        else
            *out++ = *in++;
    *out = 0;
	
	std::string string_out(out_mem);
	delete[] out_mem;
	return string_out;
}

#ifdef DIAGEN_TEST
static char* strLine = "------------------------";
static int CommandMax;
char* Command[16];
XStream ffsave(1);

void ParseCommand(char* s)
{
	int ind = 0;
	while(1){
		while(*s && (*s == ' ' || *s == '\t')) s++; if(!*s) return;
		Command[ind++] = s;
		while(*s && !(*s == ' ' || *s == '\t')) s++; if(!*s) return;
		*s++ = '\0';
		}
	CommandMax = ind;
}

void doCommand(void)
{
	char* c = Command[0];
	if(!strcmp(c,"GO")) dgD -> startSession(strdup(Convert(Command[1],1)));
	else if(!strcmp(c,"FINISH")) dgD -> endSession();
	else if(!strcmp(c,"STATUS")) dgD -> getStatus();
	else if(!strcmp(c,"ASK")) dgD -> getAnswer(Command[1]);
	else if(!strcmp(c,"ESTATUS")) dgD -> eStatus = atoi(Command[1]);
	else if(!strcmp(c,"BSTATUS")) dgD -> bStatus = atoi(Command[1]);
	else if(!strcmp(c,"ZSTATUS")) dgD -> zStatus = atoi(Command[1]);
	else if(!strcmp(c,"SAVE")) dgD -> save(ffsave);
	else if(!strcmp(c,"LOAD")) dgD -> load(ffsave);
	else if(!strcmp(c,"DATA")){
		DGdata[atoi(Command[1])] = atoi(Command[2]);
		saveList();
		}
	else if(!strcmp(c,"DATALIST")) dataList();
	else if(!strcmp(c,"DATALIST0")) dataList(0);
	else if(!strcmp(c,"MAP")) showmap();
	else if(!strcmp(c,"VISIT")){
		dgRoom* r = dgD -> seekR(Command[1],1);
		if(r){
			r -> visitCounter = atoi(Command[2]);
			r -> comingCounter = atoi(Command[3]);
			}
		}
	else std::cout << "\nError: unknown command...\n";
}

void console_clear() {
#ifdef WIN32
	system("CLS");
#else
	// Assume POSIX
	system ("clear");
#endif
}

void showmap(void)
{
	if(!dgD -> currentR) return;
	int sx = dgD -> currentR -> gridSX;
	int sy = dgD -> currentR -> gridSY;
	uchar* map = dgD -> currentR -> status;
	int i,j;
	console_clear();
	for(j = 0;j < sy;j++){
		for(i = 0;i < sx;i++)
			switch(map[j*sx + i]){
				case DG_CELLSTATUS::EMPTY:
					std::cout << " ";
					break;
				case DG_CELLSTATUS::DORMANT:
					std::cout << "D";
					break;
				case DG_CELLSTATUS::USED:
					std::cout << "U";
					break;
				case DG_CELLSTATUS::WAITING:
					std::cout << "W";
					break;
				case DG_CELLSTATUS::OPENED:
					std::cout << "O";
					break;
				case DG_CELLSTATUS::EXPLODED:
					std::cout << "E";
					break;
				}
			std::cout << "\n";
		}
}

void diagenEventHandle(int code)
{
	char* s = NULL;
	int i;
	switch(code){
		case DG_GET_NEXT_PHRASE:
			dgD -> getNextPhrase();
			break;
		case DG_SKIP_ALL:
			for(i = 0;i < 24;i++) dgD -> getNextPhrase();
			break;
		case DG_GET_QUERY_LIST:
			console_clear();
			std::cout << strLine << strLine << "\n";
			s = dgD -> getQprefix();
			if(s) std::cout << cp1251_to_utf8(cp866_to_cp1251(s)) << ":\n\n";
			s = dgD -> findQfirst();
			while(s){
//				dgLog(s);
				std::cout << cp1251_to_utf8(cp866_to_cp1251(s)) << "\n";
				s = dgD -> findQnext();
				}
			s = dgD -> getQpostfix();
			if(s) std::cout << "\n" << cp1251_to_utf8(cp866_to_cp1251(s)) << "\n";
			std::cout << strLine << strLine << "\n\n";
			break;
		case DG_TEST_QUERY:
			if(!dgD -> varAtom){
				console_clear();
				std::cout << strLine << strLine << "\n";
				s = dgD -> findQfirst();
				while(s){
					std::cout << cp1251_to_utf8(cp866_to_cp1251(s)) << "\n";
					dgD -> getAnswer(s);
					s = dgD -> findQnext();
					}
				std::cout << strLine << strLine << "\n\n";
				}
			break;
		case DG_ENTER_COMMAND:
			console_clear();
			//TODO stalkerg
			//inpS.initString();
			std::cout << "Command:\n";
			break;
		case DG_GET_COMMAND:
			//TODO stalkerg
			//s = inpS.getString();
			console_clear();
			if(s && *s){
				std::cout << "Accept command (" << s << ")...\n";
				ParseCommand(s);
				doCommand();
				}
			break;
		}
}

//TODO stalkerg redifined func
//char* uvsGetLarvaWorld(int n){ return "Glorx"; }
//char* StringOfBoozeeniada(void){ return "BooZZZ"; }
//int getSpobsState(void){ return 1; }
//int getThreallState(void){ return 1; }
//int isSpummyDeath(void){ return 0; }
//int aciWorldLinkExist(int w1,int w2,int flag){ return 0; }

#endif

char* getDGname(const char* name, const char* ext)
{
	static char buf[256];
	strcpy(buf,"data/");
	strcat(buf,name);
	strcat(buf,ext);
	return buf;
}

void dgLog(char* string)
{
/*
	static XStream ff(getDGname("current.log"),XS_OUT);
	ff < "\"" < string < "\"\r\n";
*/
	if(!string) return;
#ifdef DIAGEN_TEST
	if(checkStatus) return;
	std::cout << cp1251_to_utf8(cp866_to_cp1251(string)) << "\n";
	std::cout << strLine << strLine << "\n\n";
#endif
}

inline char* getSubj(char* p,int& m)
{
	int len = strlen(p);
	if(*p != SUBJ_SYMBOL || *(p + len - 1) != SUBJ_SYMBOL_RIGHT) {
		std::cout<<"dgSyntaxError p:"<<p<<"|"<<m<<std::endl;
		ErrH.Abort(dgSyntaxError);
	}
	*(p + len - 1) = '\0';
	char* add = ++p;
	while(*add && *add != ':') {
		add++;
	}
	if(*add) {
		*add++ = '\0';
	}
	switch(*add){
		case 'G': m = 1; break;
		case 'B': m = 2; break;
		default: m = 0;
	}
	return p;
}

inline char* getSubj(char* p)
{
#ifdef GERMAN_VERSION
	if(*p != '$')
#else
	if(*p != '#')
#endif
		ErrH.Abort(dgSyntaxError);
	return p + 1;
}

inline void cutLink(char*& p)
{
	if(p){
		*p++ = '\0';
		*(p + strlen(p) - 1) = '\0';
		}
}

int Comparision(char* opr,int a,int b)
{
	char mss[] = "Wrong operation";

	switch(*opr){
		case '=':
			switch(*(opr + 1)){
				case '=': return a == b;
				default: ErrH.Abort(mss);
				}
		case '>':
			switch(*(opr + 1)){
				case '=': return a >= b;
				case '\0': return a > b;
				default: ErrH.Abort(mss);
				}
		case '<':
			switch(*(opr + 1)){
				case '=': return a <= b;
				case '\0': return a < b;
				default: ErrH.Abort(mss);
				}
		default:
			ErrH.Abort(mss);
		}
	return 0;
}

void diagenPrepare(void)
{
	dgD = new DiagenDispatcher;
	dgD -> init();
#ifdef DIAGEN_TEST
	loadList();
#endif
}

int getSpectorsTaskStatus(void)
{
//#ifndef DIAGEN_TEST
	char es[] = "PIVLOZB";
	char ws[] = "WKBA";
	char esS[] = "???????";
	char wsS[] = "????";
	memcpy(esS,es,7);
	memcpy(wsS,ws,4);

	int i,j,res;
	for(i = 0;i < 4;i++)
		for(j = 0;j < 7;j++)
		{
		    auto res = getPassangerStats(es[j],ws[i]);
			if(res != 0)
			{
				esS[j] = '\0';
				wsS[i] = '\0';
			}
		}
	res = 0;
	for(i = 0;i < 4;i++) res += wsS[i];
	for(i = 0;i < 7;i++) res += esS[i];

	return !res;
/*#else
	return 0;
#endif*/
}

int getInfernalsTaskStatus(void)
{
//#ifndef DIAGEN_TEST
	int res = 0,i;
	for(i = 0;i < 3;i++) res += isCycleExist(i,'F');
	for(i = 0;i < 3;i++) res += isCycleExist(i,'G');
	for(i = 0;i < 3;i++) res += isCycleExist(i,'N');
	return !res;
/*#else
	return 1;
#endif*/
}

// ----------------------------------- dgFile ------------------------------------

bool ISEALPHA(char c) {
    if (lang() == RUSSIAN) {
        return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
    } else if (lang() == GERMAN) {
        return 	(c != '#' && !(c >= '0' && c <= '9'));
    }

    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}

char* detect_text(char *ret) {
	char* ret2 = ret;
	if (*ret2) {
		if (*ret2 == '$' && *(ret2+1) == '(') {
			std::cout<<"Detect variable in start string."<<std::endl;
			while(*ret2 && *ret2 != ')') { //Ищем конец переменной. Могут быть переполнения. Но мы будем писать аккуратно. :)
				ret2++;
			}
			ret2++;
			while(*ret2 && *ret2 == ' ') { //Пропускаем пробелы
				ret2++;
			}
		}
	}
	
	if(*ret2 == SUBJ_SYMBOL || *ret2 == COMMAND_SYMBOL) {
		return ret;
	}
	return  (*ret2 == LINK_SYMBOL || *ret2 == '<') ? ret2 + 1 : ret2;
}

void dgFile::load(char* fname,int _len, bool verbose)
{
	static char mss[] = "Wrong dgFile format";
	int handle = 0;
	if(!_len){
#ifdef DIAGEN_TEST
		external = 0;
		XStream ff(fname,XS_IN);
		buf = new char[(len = ff.size()) + 1];
		ff.read(buf,len);
		ff.close();
		buf[len] = '\0';
		handle = 1;
		if (verbose) std::cout<<"dgFile::load buf:"<<buf<<std::endl;
#else
		external = 1;
		buf = FBox -> get(fname,len);
#endif
	} else {
		external = 1;
		len = _len;
		buf = fname;
		handle = 1;
	}

	if(!handle) {
		index = 0;
		return;
	}

	int i = 0,mode = 1,j;
	char c;
	//(stalkerg)Разрезаем буфер там где есть комментарии
	while(i < len){
		c = buf[i];
		if(mode) {
			if(c == '/' && buf[i + 1] == '*') {
				buf[i] = buf[i + 1] = '\0';
				i++;
				mode = 0;
			}
		} else {
			if(c == '*' && buf[i + 1] == '/') {
				buf[i] = buf[i + 1] = '\0';
				i++;
				mode = 1;
			} else {
				buf[i] = '\0';
			}
		}
		i++;
	}
	i = 0;
	while(i < len){
		c = buf[i];
		if(c == '\"'){
			buf[i++] = '\0';
			j = i;
			while(i < len){
				if(buf[i] == '\"') {
					break;
				}
				if(buf[i] == SUBJ_SYMBOL || buf[i] == LINK_SYMBOL) {
					ErrH.Abort(fname,XERR_USER,-1,buf + i + 1);
				}
				i++;
			}
			if(i == len) {
				ErrH.Abort(mss,XERR_USER,-1,"\" absent");
			} else {
				buf[i] = '\0';
			}
		} else {
			if(c == SUBJ_SYMBOL) {
				i++;
				while(i < len) {
					if(buf[i] == SUBJ_SYMBOL_RIGHT) break;
					i++;
				}
				if(i == len) {
					ErrH.Abort(mss,XERR_USER,-1,"RightSubjSymbol absent");
				}
			} else {
				if(c == LINK_SYMBOL) {
					i++;
					while(i < len) {
						if(buf[i] == LINK_SYMBOL_RIGHT) break;
						i++;
						}
					if(i == len) {
						ErrH.Abort(mss,XERR_USER,-1,"RightLinkSymbol absent");
					}
				} else {
					if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
						buf[i] = '\0';
					}
				}
			}
		}
		i++;
	}
	index = 0;
#ifdef DIAGEN_TEST
	if(external) return;
	char* b = new char[len];
	memset(b,0,len);
	int off = 0,l;
	char* p, *p2;

#ifdef SAVE_SINGE_LANGUAGE
	int skip = !memcmp(fname + strlen(fname) - 3,"dil",3) || !memcmp(fname + strlen(fname) - 3,"lst",3);
#endif

	while(1){
		p = getElement(DGF_NONE,1);
		if(!p) break;
		
#ifdef SAVE_SINGE_LANGUAGE
		if(!skip){
			p2 = detect_text(p);
			if(!(*p2 == SUBJ_SYMBOL || *p2 == COMMAND_SYMBOL || *p2 == '<' || *p2 == '#')) {
				if(!((lang() != RUSSIAN && ISEALPHA(*p2)) || (lang() == RUSSIAN && !ISEALPHA(*p2)))) continue;
			}
			
		}
#endif
		l = strlen(p);
		memcpy(b + off,p,l);
		off += l + 1;
		}
	FBox -> add(fname,b,off);
	delete b;
#endif
	index = 0;
}



char* dgFile::getElement(int DualElements,int empty_available)
{
	char* p = buf + index;
	while(index < len && !(*p)) p++, index++;
	if(index == len){
		if(!empty_available)
			ErrH.Abort(dgSyntaxError);
		return NULL;
		}
	char* ret = p;
	
	
	while(index < len && *p) p++, index++;

#if defined(DIAGEN_TEST) || !defined(SINGLE_LANGUAGE)
	char* ret2 = ret; //(stalkerg) Для определения символа по которому можно понять язык
	char* elem;
	
	if(DualElements == DGF_DUAL) {
		if (*ret2) {
			if (*ret2 == '$' && *(ret2+1) == '(') {
				std::cout<<"Detect variable in start string."<<std::endl;
				while(*ret2 && *ret2 != ')') { //Ищем конец переменной. Могут быть переполнения. Но мы будем писать аккуратно. :)
					ret2++;
				}
				ret2++;
				while(*ret2 && *ret2 == ' ') { //Пропускаем пробелы
					ret2++;
				}
			}
		}
		if (!(*ret2)) {
			ErrH.Abort("Nope closing bracket for variable",XERR_USER,-1,"");
		}
		
		if(*ret2 == SUBJ_SYMBOL || *ret2 == COMMAND_SYMBOL) {
			return ret;
		}
		elem = (*ret2 == LINK_SYMBOL || *ret2 == '<') ? ret2 + 1 : ret2;
		if((lang() != RUSSIAN && ISEALPHA(*elem)) || (lang() == RUSSIAN && !ISEALPHA(*elem))) {
			return ret;
		} else {
			return getElement(DualElements, empty_available);
		}
	}
#endif
	return ret;
}

// ----------------------------------- dgAtom ------------------------------------
void dgAtom::link(dgAtom*& tail)
{
	if(!tail) tail = this;
	else {
		tail -> prev -> next = this;
		prev = tail -> prev;
		}
	next = NULL;
	tail -> prev = this;
}

char* dgAtom::read(dgFile* pf,char* s)
{
	data = s;
	char* p = pf -> getElement(DGF_DUAL,1);
	qlMax = 0;
	int log = 1,i;
	while(p && log)
		switch(*p){
			case LINK_SYMBOL:
				qlinks = p + 1;
				do { 
					qlMax++; 
					cutLink(p); 
					p = pf -> getElement(DGF_DUAL,1); 
					} while(p && *p == LINK_SYMBOL);
				break;
			case COMMAND_SYMBOL:
				p++;
				switch(*p){
					case '?':
						nvariant = atoi(pf -> getElement(DGF_NONE));
						if(nvariant <= 0)
							ErrH.Abort("Wrong nvariant",XERR_USER,-1,data);
						variants = new char*[3*nvariant];
						comments = variants + nvariant;
						vcodes = (int*)(comments + nvariant);
						for(i = 0;i < nvariant;i++)
							vcodes[i] = atoi(pf -> getElement(DGF_NONE));
						for(i = 0;i < nvariant;i++){
							variants[i] = pf -> getElement(DGF_DUAL);
							comments[i] = pf -> getElement(DGF_DUAL);
							}
						p = pf -> getElement(DGF_DUAL,1);
						break;
					default:
						p--; log = 0;
						break;
					}
				break;
			default:
				log = 0;
				break;
			}
	return p;
}

char* dgAtom::findQLfirst(void)
{
	if(!qlMax) return NULL;
	qlNum = 1;
	qlPos = 0;
	while(qlinks[qlPos]) qlPos++;
	return qlinks;
}

char* dgAtom::findQLnext(void)
{
	if(++qlNum > qlMax) return NULL;
	while(!qlinks[qlPos]) qlPos++;
	char* ret = qlinks + qlPos;
	while(qlinks[qlPos]) qlPos++;
	return ret;
}

int dgAtom::getVcode(char* vars,char*& cs)
{
	for(int i = 0;i < nvariant;i++)
		if(!strcmp(variants[i],vars)){
			cs = comments[i];
			return vcodes[i];
			}
	return 0;
}

// ----------------------------------- dgMolecule ------------------------------------
void dgMolecule::link(dgMolecule*& tail,int mode)
{
	switch(mode){
		case 0:
			if(!tail) tail = this;
			else {
				tail -> prev -> next = this;
				prev = tail -> prev;
				}
			next = NULL;
			tail -> prev = this;
			break;
		case 1:
			prev = tail -> prev;
			tail -> prev = this;
			next = tail;
			tail = this;
			break;
		case 2:
			next = tail -> next;
			prev = tail;
			if(!next) tail -> prev = this;
			else tail -> next -> prev = this;
			tail -> next = this;
			break;
		}
}

dgMolecule* dgMolecule::delink(dgMolecule*& tail)
{
	if(tail != this){
		prev -> next = next;
		if(!next) tail -> prev = prev;
		}
	else {
		tail = next;
		}
	if(next) next -> prev = prev;
	return this;
}

void dgMolecule::accept(dgFile* _dgf)
{
	dgf = _dgf;
	char* p = NULL;
	dgAtom* pa;
	while(1){
		if(!p) if(!(p = dgf -> getElement(DGF_DUAL,1))) break;
		(pa = new dgAtom) -> link(tail);
		if(!(p = pa -> read(dgf,p))) break;
		}
	curA = tail;
}

char* dgMolecule::getPhrase(int noHandle)
{ 
	char* ret = curA ? curA -> data : NULL; 
	if(!ret) {
		return NULL;
	}

	if(!noHandle) {
		static char* buf = new char[1311];
		int log = 1;
//		if(strlen(ret) >= 310)
//			ErrH.Abort("getPhrase() too long", XERR_USER,-1,name);
		strcpy(buf,ret);
		while(log) log = getHandledPhrase(buf);
		return buf;
	}
	return ret;
}

char* dgMolecule::getVarPhrase(char* s)
{
	static char nbuf[] = "\0\0\0\0\0\0\0\0\0\0";
	char* ret = NULL;
	if(!strcmp(s,"World of Larva0")) ret = dgD -> getWorldName(uvsGetLarvaWorld(0));
	else if(!strcmp(s,"World of Larva1")) ret = dgD -> getWorldName(uvsGetLarvaWorld(1));
	else if(!strcmp(s,"World of Larva2")) ret = dgD -> getWorldName(uvsGetLarvaWorld(2));
	else if(!strcmp(s,"Cirt Delivery")) ret = port_itoa(uvsgetDGdata(DG_EXTERNS::CIRT_DELIVERY),nbuf,10);
	else if(!strcmp(s,"Cycle Name of Locked Larva")) ret = dgD -> CycleName[lang() == RUSSIAN ? 1 : 0][dgD -> currentR -> bios][uvsgetDGdata(DG_EXTERNS::LARVA_CYCLE)];
	else if(!strcmp(s,"Boozeena Secret Code")) ret = StringOfBoozeeniada();

	if(!ret) return s;
//		ErrH.Abort("Unknown VarPhrase",XERR_USER,-1,s);
	if(strlen(ret) >= strlen(s)) ErrH.Abort("Too long VarPhrase");
	return ret;
}

int dgMolecule::getHandledPhrase(char* s)
{
	static char mss[] = "Wrong VarPhrase format";

	char* var = strchr(s,'$');
	if(!var) return 0;
	char* p = var + 1;
	if(*p != '(') ErrH.Abort(mss);
	p++; while(*p != ')' && *p) p++;
	if(!*p) ErrH.Abort(mss);
	*p = '\0'; p++;
	char* pp = p + strlen(p) - 1;
	char* ns = getVarPhrase(var + 2);
	strcpy(var,ns);
	strcpy(var + strlen(ns),p);
	while(*pp) *pp-- = '\0';

	return 1;
}

// ----------------------------------- dgQuery ------------------------------------
void dgQuery::link(dgQuery*& tail)
{
	if(!tail) tail = this;
	else {
		tail -> prev -> next = this;
		prev = tail -> prev;
		}
	next = NULL;
	tail -> prev = this;
}

dgQuery* dgQuery::delink(dgQuery*& tail)
{
	if(tail != this){
		prev -> next = next;
		if(!next) tail -> prev = prev;
		}
	else {
		tail = next;
		}
	if(next) next -> prev = prev;
	return this;
}

int dgQuery::getIndex(void)
{ 
	int n = lindex; 
	if(solidQ) {
		for(int i = 0;i < nanswer;i++) {
			if(levels[i] == *dgLevel) {
				lindex = i;
				return i;
			}
		}
	} else {
		if(n + 1 < nanswer && levels[n + 1] <= *dgLevel)
			lindex = ++n;
	}
	return n; 
}

// ----------------------------------- dgCell ------------------------------------
void dgCell::read(dgFile* dgf)
{
	x = atoi(dgf -> getElement(DGF_NONE));
	y = atoi(dgf -> getElement(DGF_NONE));
	char* p = dgf -> getElement(DGF_NONE);
	if(*p != ' ') Name = p;
	Type = atoi(dgf -> getElement(DGF_NONE));
	isWaiting = atoi(dgf -> getElement(DGF_NONE));
	isLooping = atoi(dgf -> getElement(DGF_NONE));
	p = dgf -> getElement(DGF_NONE); if(*p != ' ') { Access.load(p,strlen(p)); }
	p = dgf -> getElement(DGF_NONE); if(*p != ' ') { PostCMD.load(p,strlen(p)); }
	p = dgf -> getElement(DGF_NONE); if(*p != ' ') { StartCMD.load(p,strlen(p)); }
}

struct Q { enum {
	TRU,
	DATA,
	VISITE,
	VISITW,
	COMING,
	SINGLERAND,
	BSTATUS,
	ZSTATUS,
	SSTATUS,
	ENTER_SSTATUS,
	QCODE,
	MP_GAME,
	CYCLE_EXIST,
	PASSENGER_OUT,
	PASSENGER_ARRIVAL,
	SPECTORS_TASK,
	INFERNALS_TASK,
	SPOBS_ALIVE,
	THREALL_ALIVE,
	SUCCESS,
	FIASCO,
	SPUMMY_DEATH,
	KEY2GLORX_EXIST,
	KEY2ARKAZNOY_EXIST,
	KEY2BOOZEENA_EXIST,
	WAS_RUBEECATED
	}; };

struct Q2 { int id; const char* name; } Query[] = {
	{ Q::TRU, "true" },
	{ Q::DATA, "data" },
	{ Q::VISITE, "visitE" },
	{ Q::VISITW, "visitW" },
	{ Q::COMING, "coming" },
	{ Q::SINGLERAND, "singlerand" },
	{ Q::BSTATUS, "bstatus" },
	{ Q::ZSTATUS, "zstatus" },
	{ Q::SSTATUS, "sstatus" },
	{ Q::ENTER_SSTATUS, "enter_sstatus" },
	{ Q::QCODE, "qcode" },
	{ Q::MP_GAME, "mp_game" },
	{ Q::CYCLE_EXIST, "cycle_exist" },
	{ Q::PASSENGER_OUT, "passenger_out" },
	{ Q::PASSENGER_ARRIVAL, "passenger_arrival" },
	{ Q::SPECTORS_TASK, "spectors_task" },
	{ Q::INFERNALS_TASK, "infernals_task" },
	{ Q::SPOBS_ALIVE, "spobs_alive" },
	{ Q::THREALL_ALIVE, "threall_alive" },
	{ Q::SUCCESS, "success" },
	{ Q::FIASCO, "fiasco" },
	{ Q::SPUMMY_DEATH, "spummy_death" },
	{ Q::KEY2GLORX_EXIST,"key2glorx_exist" },
	{ Q::KEY2ARKAZNOY_EXIST,"key2arkaznoy_exist" },
	{ Q::KEY2BOOZEENA_EXIST,"key2boozeena_exist" },
	{ Q::WAS_RUBEECATED, "wasRubeecated" },
	{ -1, NULL }
	};

struct Q3 { int id; const char* name; } ArgData[] = {
	{ DG_EXTERNS::ELR_RESULT, "ELR_RESULT" },
	{ DG_EXTERNS::ELR_TOTAL, "ELR_TOTAL" },
	{ DG_EXTERNS::ELR_RATING, "ELR_RATING" },

	{ DG_EXTERNS::KER_RESULT, "KER_RESULT" },
	{ DG_EXTERNS::KER_TOTAL, "KER_TOTAL" },
	{ DG_EXTERNS::KER_RATING, "KER_RATING" },

	{ DG_EXTERNS::PIP_RESULT, "PIP_RESULT" },
	{ DG_EXTERNS::PIP_TOTAL, "PIP_TOTAL" },
	{ DG_EXTERNS::PIP_RATING, "PIP_RATING" },

	{ DG_EXTERNS::ZYK_RESULT, "ZYK_RESULT" },
	{ DG_EXTERNS::ZYK_TOTAL, "ZYK_TOTAL" },
	{ DG_EXTERNS::ZYK_RATING, "ZYK_RATING" },

	{ DG_EXTERNS::INV_BAD_CIRT, "INV_BAD_CIRT" }, 
	{ DG_EXTERNS::INV_GOOD_CIRT, "INV_GOOD_CIRT" },
	{ DG_EXTERNS::INV_NYMBOS, "INV_NYMBOS" },
	{ DG_EXTERNS::INV_PHLEGMA, "INV_PHLEGMA" },
	{ DG_EXTERNS::INV_HEROIN, "INV_HEROIN" },
	{ DG_EXTERNS::INV_SHRUB, "INV_SHRUB" },
	{ DG_EXTERNS::INV_POPONKA, "INV_POPONKA" },
	{ DG_EXTERNS::INV_UNIQUE_POPONKA, "INV_UNIQUE_POPONKA" },
	{ DG_EXTERNS::INV_TOXICK, "INV_TOXICK" },
	{ DG_EXTERNS::INV_ELEECH, "INV_ELEECH" },
	{ DG_EXTERNS::INV_DEAD_ELEECH, "INV_DEAD_ELEECH" },
	{ DG_EXTERNS::INV_PIPETKA, "INV_PIPETKA" },
	{ DG_EXTERNS::INV_KERNOBOO, "INV_KERNOBOO" },
	{ DG_EXTERNS::INV_WEEZYK, "INV_WEEZYK" },
	{ DG_EXTERNS::INV_RUBBOX,  "INV_RUBBOX" },
	{ DG_EXTERNS::INV_BAD_LARVA, "INV_BAD_LARVA" }, 
	{ DG_EXTERNS::INV_GOOD_LARVA, "INV_GOOD_LARVA" },

	{ DG_EXTERNS::INV_MESSIAH, "INV_MESSIAH" },
	{ DG_EXTERNS::INV_PROTRACTOR, "INV_PROTRACTOR" },
	{ DG_EXTERNS::INV_FUNCTION83, "INV_FUNCTION83" },
	{ DG_EXTERNS::INV_BOOTSECTOR, "INV_BOOTSECTOR" },
	{ DG_EXTERNS::INV_PEELOT, "INV_PEELOT" },
	{ DG_EXTERNS::INV_SPUMMY, "INV_SPUMMY" },

	{ DG_EXTERNS::INV_LEEPURINGA, "INV_LEEPURINGA" },
	{ DG_EXTERNS::INV_PALOCHKA, "INV_PALOCHKA" },
	{ DG_EXTERNS::INV_BOORAWCHICK, "INV_BOORAWCHICK" },
	{ DG_EXTERNS::INV_NOBOOL, "INV_NOBOOL" },
	{ DG_EXTERNS::INV_PIPKA, "INV_PIPKA" },
	{ DG_EXTERNS::INV_PEREPONKA, "INV_PEREPONKA" },
	{ DG_EXTERNS::INV_ZEEFICK, "INV_ZEEFICK" },

	{ DG_EXTERNS::INV_ELEEPOD, "INV_ELEEPOD" },
	{ DG_EXTERNS::INV_BEEBOORAT, "INV_BEEBOORAT" },
	{ DG_EXTERNS::INV_ZEEX, "INV_ZEEX" },

	{ DG_EXTERNS::THIEF_LEEPURINGA, "THIEF_LEEPURINGA" },
	{ DG_EXTERNS::THIEF_PALOCHKA, "THIEF_PALOCHKA" },
	{ DG_EXTERNS::THIEF_BOORAWCHICK, "THIEF_BOORAWCHICK" },
	{ DG_EXTERNS::THIEF_NOBOOL, "THIEF_NOBOOL" },
	{ DG_EXTERNS::THIEF_PIPKA, "THIEF_PIPKA" },
	{ DG_EXTERNS::THIEF_PEREPONKA, "THIEF_PEREPONKA" },
	{ DG_EXTERNS::THIEF_ZEEFICK, "THIEF_ZEEFICK" },

	{ DG_EXTERNS::HERE_LEEPURINGA, "HERE_LEEPURINGA" },
	{ DG_EXTERNS::HERE_PALOCHKA, "HERE_PALOCHKA" },
	{ DG_EXTERNS::HERE_BOORAWCHICK, "HERE_BOORAWCHICK" },
	{ DG_EXTERNS::HERE_NOBOOL, "HERE_NOBOOL" },
	{ DG_EXTERNS::HERE_PIPKA, "HERE_PIPKA" },
	{ DG_EXTERNS::HERE_PEREPONKA, "HERE_PEREPONKA" },
	{ DG_EXTERNS::HERE_ZEEFICK, "HERE_ZEEFICK" },

	{ DG_EXTERNS::KILLED_VANGERS, "KILLED_VANGERS" },
	{ DG_EXTERNS::MECHOS_CHANGED, "MECHOS_CHANGED" },
	{ DG_EXTERNS::MECHOS_TYPE, "MECHOS_TYPE" },
	{ DG_EXTERNS::CYCLE, "CYCLE" },
	{ DG_EXTERNS::BEEBS, "BEEBS" },
	{ DG_EXTERNS::LUCK, "LUCK" },
	{ DG_EXTERNS::DOMINANCE, "DOMINANCE" },
	{ DG_EXTERNS::TABUTASKS, "TABUTASKS" },
	{ DG_EXTERNS::FRESH_TABUTASKS, "FRESH_TABUTASKS" },
	{ DG_EXTERNS::WTABUTASKS, "WTABUTASKS" },
	{ DG_EXTERNS::TRUE_PASSENGER, "TRUE_PASSENGER" },
	{ DG_EXTERNS::BOORAWCHICK_RETURN, "BOORAWCHICK_RETURN" },

	{ DG_EXTERNS::PHLEGMA_DELIVERY, "PHLEGMA_DELIVERY" },
	{ DG_EXTERNS::NYMBOS_DELIVERY, "NYMBOS_DELIVERY" },
	{ DG_EXTERNS::HEROIN_DELIVERY, "HEROIN_DELIVERY" },
	{ DG_EXTERNS::SHRUB_DELIVERY, "SHRUB_DELIVERY" },
	{ DG_EXTERNS::POPONKA_DELIVERY, "POPONKA_DELIVERY" },
	{ DG_EXTERNS::TOXICK_DELIVERY, "TOXICK_DELIVERY" },
	{ DG_EXTERNS::CIRT_DELIVERY, "CIRT_DELIVERY" },
	{ DG_EXTERNS::LARVA_DELIVERY, "LARVA_DELIVERY" },

	{ DG_EXTERNS::PHLEGMA_BONUS, "PHLEGMA_BONUS" },
	{ DG_EXTERNS::NYMBOS_BONUS, "NYMBOS_BONUS" },
	{ DG_EXTERNS::HEROIN_BONUS, "HEROIN_BONUS" },
	{ DG_EXTERNS::SHRUB_BONUS, "SHRUB_BONUS" },
	{ DG_EXTERNS::POPONKA_BONUS, "POPONKA_BONUS" },
	{ DG_EXTERNS::TOXICK_BONUS, "TOXICK_BONUS" },

	{ DG_EXTERNS::WEEZYK_ASSIGNMENT, "WEEZYK_ASSIGNMENT" },
	{ DG_EXTERNS::LARVA_CYCLE, "LARVA_CYCLE" },

	{ DG_EXTERNS::ZIGGER_ZAGGER, "ZIGGER_ZAGGER" },
	{ DG_EXTERNS::CART, "CART" },
	{ DG_EXTERNS::PUT_PUT, "PUT_PUT" },
	{ DG_EXTERNS::BOX, "BOX" },
	{ DG_EXTERNS::RATTLE, "RATTLE" },
	{ DG_EXTERNS::HEAVY_IRON, "HEAVY_IRON" },
	{ DG_EXTERNS::STAR, "STAR" },
	{ DG_EXTERNS::WHIZZER, "WHIZZER" },
	{ DG_EXTERNS::HUSKER, "HUSKER" },
	{ DG_EXTERNS::COGGED_WIDGET, "COGGED_WIDGET" },

	{ -1, NULL }
	};

#ifdef DIAGEN_TEST
//TODO Need XCon position stalkerg
void dataList(int val)
{
	console_clear();
	int i = 0;
	while(ArgData[i].name){
		if(val != -1) DGdata[i] = val;
		//XCon.setpos(22*(i/25),i%25);
		//XCon <= i < ":" < ArgData[i].name < ": " <= DGdata[i];
		i++;
		}

	i = 0;
	dgRoom* r = dgD -> rtail;
	while(r){
		//XCon.setpos(60,i++);
		//XCon < "[" < *r -> roomName < "]: " <= r -> visitCounter < "/" <= r -> comingCounter < " (" < Convert(r -> roomName) < ")";
		r = r -> next;
		}
	//XCon.setpos(0,24);
	saveList();
}

void loadList(void)
{
	XStream ff(0);
	if(!ff.open("data.lst",XS_IN)) return;
	int i = 0;
	while(ArgData[i].name){
		ff >= DGdata[i] < "\r\n";
		i++;
		}
	dgRoom* r = dgD -> rtail;
	while(r){
		ff >= r -> visitCounter;
		ff >= r -> comingCounter;
		r = r -> next;
		}
	ff.close();
}

void saveList(void)
{
	XStream ff("data.lst",XS_OUT);
	int i = 0;
	while(ArgData[i].name){
		ff <= DGdata[i] < "\r\n";
		i++;
		}
	dgRoom* r = dgD -> rtail;
	while(r){
		ff <= r -> visitCounter < "\r\n";
		ff <= r -> comingCounter < "\r\n";
		r = r -> next;
		}
	ff.close();
}

/*int uvsgetDGdata(int code)
{
	return DGdata[code];
}*/
#endif

int dgCell::analyzeACCESS(char* p)
{
	int i = 0;
	do if(!strcasecmp(p,Query[i].name)) break;
	while(Query[++i].name);
	if(!Query[i].name)
		std::cout<<"Unknown query "<<p<<std::endl;
	char* opr,*arg2;
	int lvalue = 0,rvalue;
	dgRoom* r;

	switch(Query[i].id){
		case Q::TRU:
			return 1;
		case Q::DATA:
			p = Access.getElement(DGF_NONE);
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			if(*arg2 == '$'){
				arg2++;
				if(!strcmp(arg2,"ELR_TOTAL")) rvalue = uvsgetDGdata(DG_EXTERNS::ELR_TOTAL);
				else if(!strcmp(arg2,"KER_TOTAL")) rvalue = uvsgetDGdata(DG_EXTERNS::KER_TOTAL);
				else if(!strcmp(arg2,"PIP_TOTAL")) rvalue = uvsgetDGdata(DG_EXTERNS::PIP_TOTAL);
				else if(!strcmp(arg2,"ZYK_TOTAL")) rvalue = uvsgetDGdata(DG_EXTERNS::ZYK_TOTAL);
#ifdef DIAGEN_TEST
				else if(!strcmp(arg2,"PREV_CYCLE_1")) rvalue = 0;
				else if(!strcmp(arg2,"PREV_CYCLE_2")) rvalue = 1;
#else
				else if(!strcmp(arg2,"PREV_CYCLE_1")) rvalue = getPreviousCycle(1);
				else if(!strcmp(arg2,"PREV_CYCLE_2")) rvalue = getPreviousCycle(2);
#endif
#ifdef DIAGEN_TEST
				else{
					XBuffer buf;
					buf < arg2 < "/" < owner -> roomName;
					ErrH.Abort("Wrong ARG2",XERR_USER,-1,buf.GetBuf());
					}
#else
				else ErrH.Abort("Wrong ARG2");
#endif
			}
			else
				rvalue = atoi(arg2);
			i = 0; do if(!strcmp(p,ArgData[i].name)) break;
			while(ArgData[++i].name);
			if(!ArgData[i].name) ErrH.Abort("Unknown data",XERR_USER,-1,p);
			lvalue = uvsgetDGdata(ArgData[i].id);
			return Comparision(opr,lvalue,rvalue);
		case Q::VISITE:
			p = Access.getElement(DGF_NONE);
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			r = owner -> owner -> seekR(p,1);
			if(!r) ErrH.Abort("Unknown Escave-ID");
			lvalue = r -> visitCounter;
			rvalue = atoi(arg2);
			return Comparision(opr,lvalue,rvalue);
		case Q::VISITW:
			p = Access.getElement(DGF_NONE);
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
//#ifndef DIAGEN_TEST
			lvalue = getWorldVisitStats(*p);
//#endif
			rvalue = atoi(arg2);
			return Comparision(opr,lvalue,rvalue);
		case Q::COMING:
			p = Access.getElement(DGF_NONE);
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			r = owner -> owner -> seekR(p,1);
			if(!r) ErrH.Abort("Unknown Escave-ID");
			lvalue = r -> comingCounter;
			rvalue = atoi(arg2);
			return Comparision(opr,lvalue,rvalue);
		case Q::SINGLERAND:
			p = Access.getElement(DGF_NONE);
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			if(singleRandAvailable){
				lvalue = realRND(atoi(p));
				rvalue = atoi(arg2);
				if(Comparision(opr,lvalue,rvalue)){
					singleRandAvailable = 0;
					return 1;
					}
				else
					return 0;
				}
			else
				return 0;
		case Q::BSTATUS:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,owner -> owner -> bStatus,rvalue);
		case Q::ZSTATUS:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,owner -> owner -> zStatus,rvalue);
		case Q::SSTATUS:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,owner -> owner -> sStatus,rvalue);
		case Q::ENTER_SSTATUS:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,enter_sStatus,rvalue);
		case Q::QCODE:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,Qcode,rvalue);
		case Q::MP_GAME:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,MP_GAME,rvalue);
		case Q::CYCLE_EXIST:
			lvalue = atoi(Access.getElement(DGF_NONE));
			opr = Access.getElement(DGF_NONE);
			rvalue = atoi(Access.getElement(DGF_NONE));
#ifdef DIAGEN_TEST
			return 1;
#else
			return Comparision(opr,isCycleExist(lvalue),rvalue);
#endif
		case Q::PASSENGER_OUT:
			p = Access.getElement(DGF_NONE);
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
//#ifndef DIAGEN_TEST
			lvalue = getPassangerStats(*p,*(p + 1));
//#endif
			rvalue = atoi(arg2);
			return Comparision(opr,lvalue,rvalue);
		case Q::PASSENGER_ARRIVAL:
			p = Access.getElement(DGF_NONE);
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
//#ifndef DIAGEN_TEST
			lvalue = getPassangerFrom(*p);
//#endif
			rvalue = atoi(arg2);
			return Comparision(opr,lvalue,rvalue);
		case Q::SPECTORS_TASK:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,getSpectorsTaskStatus(),rvalue);
		case Q::INFERNALS_TASK:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,getInfernalsTaskStatus(),rvalue);
		case Q::SPOBS_ALIVE:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,getSpobsState(),rvalue);
		case Q::THREALL_ALIVE:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,getThreallState(),rvalue);
		case Q::SUCCESS:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,(int)realRND(100) < uvsgetDGdata(DG_EXTERNS::LUCK),rvalue);
		case Q::FIASCO:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,(int)realRND(60) >= uvsgetDGdata(DG_EXTERNS::LUCK),rvalue);
		case Q::SPUMMY_DEATH:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,isSpummyDeath(),rvalue);
		case Q::KEY2GLORX_EXIST:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,aciWorldLinkExist(0,1,0),rvalue);
		case Q::KEY2ARKAZNOY_EXIST:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,aciWorldLinkExist(2,9,0),rvalue);
		case Q::KEY2BOOZEENA_EXIST:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,aciWorldLinkExist(2,5,0),rvalue);
		case Q::WAS_RUBEECATED:
			opr = Access.getElement(DGF_NONE);
			arg2 = Access.getElement(DGF_NONE);
			rvalue = atoi(arg2);
			return Comparision(opr,wasRubeecated,rvalue);
		}
	return 0;
}

int dgCell::isACCESS(void)
{
	Access.reset();
	char* p = Access.getElement(DGF_NONE);
	int result = -1,r;
	int logic = 1;
	while(p){
		if(!strcmp(p,"(")){
			r = expressACCESS(1,p,logic);
			if(result != -1) result = logic ? (result && r) : (result || r);
			else result = r;
			}
		else if(!strcasecmp(p,"AND")){ logic = 1; p = Access.getElement(DGF_NONE); }
		else if(!strcasecmp(p,"OR")){ logic = 0; p = Access.getElement(DGF_NONE); }
		else{
			r = expressACCESS(0,p,logic);
			if(result != -1) result = logic ? (result && r) : (result || r);
			else result = r;
			}
		}

	return result;
}

int dgCell::expressACCESS(int endreq,char*& p,int& uplogic)
{
	if(endreq) p = Access.getElement(DGF_NONE);
	int logic = 1;
	int result = analyzeACCESS(p),r;
	p = Access.getElement(DGF_NONE,1);
	while(p){
		if(!strcmp(p,")")){ 
			if(!endreq)
				ErrH.Abort("Unexpected ')'");
			p = Access.getElement(DGF_NONE,1);
			return result; 
			}
		else if(!strcmp(p,"(")){ 
			if(endreq)  ErrH.Abort("Unexpected '('");
			p = Access.getElement(DGF_NONE);
			uplogic = logic;
			return result; 
			}
		else if(!strcasecmp(p,"AND")){ logic = 1; p = Access.getElement(DGF_NONE); }
		else if(!strcasecmp(p,"OR")){ logic = 0; p = Access.getElement(DGF_NONE); }
		else {
			r = analyzeACCESS(p);
			result = logic ? (result && r) : (result || r);
			p = Access.getElement(DGF_NONE,1);
			}
		}
	if(endreq) ErrH.Abort("Expected ')' missing");
	return result;
}

struct CMD { enum {
	ABORT,
	QUIT,
	BLOCK,
	UNBLOCK,
	CONTINUE,
	CLEAR,
	EPROMOTION,
	BPROMOTION,
	SPROMOTION,
	BDEGRADATION,
	FURY_LEVEL_INCREASE,
	FURY_LEVEL_DECREASE,
	ELR_WAITING_DECREASE,
	ELP_PRIZE_GIFT,
	KEY2GLORX_GIFT,
	KEY2NECROSS_GIFT,
	KEY2XPLO_GIFT,
	KEY2WEEXOW_GIFT,
	KEY2BOOZEENA_GIFT,
	KEY2KHOX_GIFT,
	KEY2ARKAZNOY_GIFT,
	KEY2THREALL_GIFT,
	EXPROPRIATION,
	ZERO_CASH,
	LAMPASSOTHE_END,
	TRANSITION2V,
	TRANSITION2L,
	TRANSITION2O,
	TRANSITION2B,
	CHANGE2RAFFA,
	ACCEPT_RUBBOX,
	ADD_RUBBOX,
	ZERO_KERRATING,
	ZERO_PIPRATING,
	BBR_PRIZE_GIFT,
	MAKE_WEEZYK,
	REGISTER_WEEZYK,
	COMINGOZERO,
	PENALTY1000,
	PENALTY10000,
	ADD_PROTRACTORKILL,
	ADD_MESSIAHKILL,
	OPEN_H00,
	OPEN_H01,
	OPEN_H10,
	OPEN_H11,
	ZERO_BOORAWCHICK,
	ZERO_ARKAZNOY,
	RUBEECATION
	}; };

struct Q4 { int id; const char *name; } CmdData[] = {
	{ CMD::ABORT, "abort" },
	{ CMD::QUIT, "quit" },
	{ CMD::BLOCK, "block" },
	{ CMD::UNBLOCK, "unblock" },
	{ CMD::CONTINUE, "continue" },
	{ CMD::CLEAR, "clear" },
	{ CMD::EPROMOTION, "ePromotion" },
	{ CMD::BPROMOTION, "bPromotion" },
	{ CMD::SPROMOTION, "sPromotion" },
	{ CMD::BDEGRADATION, "bDegradation" },
	{ CMD::FURY_LEVEL_INCREASE, "fury_level_increase" },
	{ CMD::FURY_LEVEL_DECREASE, "fury_level_decrease" },
	{ CMD::ELR_WAITING_DECREASE, "elr_waiting_decrease" },
	{ CMD::ELP_PRIZE_GIFT, "elp_prize_gift" },
	{ CMD::KEY2GLORX_GIFT, "key2glorx_gift" },
	{ CMD::KEY2NECROSS_GIFT, "key2necross_gift" },
	{ CMD::KEY2XPLO_GIFT, "key2xplo_gift" },
	{ CMD::KEY2WEEXOW_GIFT, "key2weexow_gift" },
	{ CMD::KEY2BOOZEENA_GIFT, "key2boozeena_gift" },
	{ CMD::KEY2KHOX_GIFT, "key2khox_gift" },
	{ CMD::KEY2ARKAZNOY_GIFT, "key2arkaznoy_gift" },
	{ CMD::KEY2THREALL_GIFT, "key2threall_gift" },
	{ CMD::EXPROPRIATION, "expropriation" },
	{ CMD::ZERO_CASH, "zeroCash" },
	{ CMD::LAMPASSOTHE_END, "LampassoTHE_END" },
	{ CMD::TRANSITION2V, "transition2V" },
	{ CMD::TRANSITION2L, "transition2L" },
	{ CMD::TRANSITION2O, "transition2O" },
	{ CMD::TRANSITION2B, "transition2B" },
	{ CMD::CHANGE2RAFFA, "change2raffa" },
	{ CMD::ACCEPT_RUBBOX, "acceptRubbox" },
	{ CMD::ADD_RUBBOX, "addRubbox" },
	{ CMD::ZERO_KERRATING, "zeroKERrating" },
	{ CMD::ZERO_PIPRATING, "zeroPIPrating" },
	{ CMD::BBR_PRIZE_GIFT, "bbr_prize_gift" },
	{ CMD::MAKE_WEEZYK, "makeWeezyk" },
	{ CMD::REGISTER_WEEZYK, "registerWeezyk" },
	{ CMD::COMINGOZERO, "comingOzero" },
	{ CMD::PENALTY1000, "penalty1000" },
	{ CMD::PENALTY10000, "penalty10000" },
	{ CMD::ADD_PROTRACTORKILL, "addProtractorKill" },
	{ CMD::ADD_MESSIAHKILL, "addMessiahKill" },
	{ CMD::OPEN_H00, "openH00" },
	{ CMD::OPEN_H01, "openH01" },
	{ CMD::OPEN_H10, "openH10" },
	{ CMD::OPEN_H11, "openH11" },
	{ CMD::ZERO_BOORAWCHICK, "zeroBoorawchick" },
	{ CMD::ZERO_ARKAZNOY, "zeroArkaznoy" },
	{ CMD::RUBEECATION, "Rubeecation" },
	{ -1, NULL }
	};

int dgCell::doCMD(int startup)
{
	dgFile* dgf = startup ? &StartCMD : &PostCMD;

//#ifndef DIAGEN_TEST
	int aciCurCredits;
//#endif
	dgf -> reset();
	char* p = dgf -> getElement(DGF_NONE,1);
	int i;
	int ret = 0;
	while(p){
		i = 0; do if(!strcmp(p,CmdData[i].name)) break;
		while(CmdData[++i].name);
		if(!CmdData[i].name) ErrH.Abort("Unknown CMD",XERR_USER,-1,p);
#ifdef DIAGEN_TEST
		if(!checkStatus) std::cout << "   [DOING]: " << CmdData[i].name << "\n";
#endif
		if(checkStatus){
			if(CmdData[i].id == CMD::ABORT)
				dgAbortStatus = 1;
			}
		else
			switch(CmdData[i].id){
				case CMD::CONTINUE:
					ret++;
					break;
				case CMD::CLEAR:
					ClearStatus = 1;
					break;
				case CMD::EPROMOTION:
					owner -> owner -> eStatus++;
					owner -> updateLevel();
					break;
				case CMD::BPROMOTION:
					owner -> owner -> bStatus++;
					owner -> updateLevel();
					break;
				case CMD::SPROMOTION:
					owner -> owner -> sStatus++;
					owner -> updateLevel();
					break;
				case CMD::BDEGRADATION:
					if(owner -> owner -> bStatus){
						owner -> owner -> bStatus--;
//#ifndef DIAGEN_TEST
						uvsLuckChange(-15);
//#endif
						}
					owner -> updateLevel();
//#ifndef DIAGEN_TEST
					GamerResult.game_ker_result = 0;
					GamerResult.game_pip_result = 0;
//#endif
					break;
				case CMD::MAKE_WEEZYK:
					owner -> owner -> zStatus = 2;
					break;
				case CMD::REGISTER_WEEZYK:
					owner -> owner -> zStatus = 1;
					owner -> updateLevel();
					break;
				case CMD::COMINGOZERO:
					owner -> owner -> seekR("O",1) -> comingCounter = 0;
					break;
				case CMD::RUBEECATION:
					wasRubeecated = 1;
					break;
//#ifndef DIAGEN_TEST
				case CMD::ABORT:
					dg_SendEvent(AML_KICK_OUT_FROM_ESCAVE);
					eventOUT = 1;
					break;
				case CMD::QUIT:
					dg_SendEvent(AML_ABORT_ESCAVE_SESSION);
					eventOUT = 1;
					break;
				case CMD::BLOCK:
					dg_SendEvent(AML_BLOCK_SHOP);
					break;
				case CMD::UNBLOCK:
					dg_SendEvent(AML_UNBLOCK_SHOP);
					break;
				case CMD::FURY_LEVEL_INCREASE:
					IncFuryFactor();
					break;
				case CMD::FURY_LEVEL_DECREASE:
					DecFuryFactor();
					break;
				case CMD::ELR_WAITING_DECREASE:
					RACE_WAIT = 5*RACE_WAIT/6;
					break;
				case CMD::ELP_PRIZE_GIFT:
					aciCurCredits = aciGetCurCredits();
					aciCurCredits += 3000;
					aciUpdateCurCredits(aciCurCredits);
					break;
				case CMD::BBR_PRIZE_GIFT:
					aciCurCredits = aciGetCurCredits();
					aciCurCredits += 5000;					
					aciUpdateCurCredits(aciCurCredits);
					break;
				case CMD::PENALTY1000:
					aciCurCredits = aciGetCurCredits();
					aciCurCredits -= 1000;
					if(aciCurCredits < 0) aciCurCredits = 0;
					aciUpdateCurCredits(aciCurCredits);
					break;
				case CMD::PENALTY10000:
					aciCurCredits = aciGetCurCredits();
					aciCurCredits -= 10000;
					if(aciCurCredits < 0) aciCurCredits = 0;
					aciUpdateCurCredits(aciCurCredits);
					break;
				case CMD::KEY2GLORX_GIFT:
					aciOpenWorldLink(0,1);
					break;
				case CMD::KEY2NECROSS_GIFT:
					aciOpenWorldLink(1,2);
					break;
				case CMD::KEY2XPLO_GIFT:
					aciOpenWorldLink(1,3);
					break;
				case CMD::KEY2WEEXOW_GIFT:
					aciOpenWorldLink(0,6);
					break;
				case CMD::KEY2BOOZEENA_GIFT:
					aciOpenWorldLink(2,5);
					break;
				case CMD::KEY2KHOX_GIFT:
					aciOpenWorldLink(1,4);
					break;
				case CMD::KEY2ARKAZNOY_GIFT:
					aciOpenWorldLink(2,9);
					break;
				case CMD::KEY2THREALL_GIFT:
					aciOpenWorldLink(3,8);
					break;
				case CMD::EXPROPRIATION:
					dg_SendEvent(AML_EXPROPRIATION);
					break;
				case CMD::ZERO_CASH:
					aciCurCredits = aciGetCurCredits();
					if(aciCurCredits > 600) aciCurCredits = 600;
					aciUpdateCurCredits(aciCurCredits);
					break;
				case CMD::LAMPASSOTHE_END:
					dg_SendEvent(AML_LAMPASSO_THEEND);
					break;
				case CMD::TRANSITION2V:
					uvsSetTownName("VigBoo"); 
					break;
				case CMD::TRANSITION2L:
					uvsSetTownName("Lampasso"); 
					break;
				case CMD::TRANSITION2O:
					uvsSetTownName("Ogorod"); 
					break;
				case CMD::TRANSITION2B:
					uvsSetTownName("B-Zone"); 
					break;
				case CMD::CHANGE2RAFFA:
					dg_SendEvent(AML_CHANGE_TO_RAFFA);
					break;
				case CMD::ACCEPT_RUBBOX:
					dg_SendEvent(AML_ACCEPT_RUBBOX);
					break;
				case CMD::ADD_RUBBOX:
					dg_SendEvent(AML_ADD_RUBBOX);
					break;
				case CMD::ZERO_KERRATING:
					GamerResult.game_ker_result = 0;
					break;
				case CMD::ZERO_PIPRATING:
					GamerResult.game_pip_result = 0;
					break;
				case CMD::ADD_PROTRACTORKILL:
					addInfernalsKill2Protractor();
					break;
				case CMD::ADD_MESSIAHKILL:
					addSpectorsKill2Messiah();
					break;
				case CMD::OPEN_H00:
					OpenCrypt(0,0);
					break;
				case CMD::OPEN_H01:
					OpenCrypt(0,1);
					break;
				case CMD::OPEN_H10:
					OpenCrypt(1,0);
					break;
				case CMD::OPEN_H11:
					OpenCrypt(1,1);
					break;
				case CMD::ZERO_BOORAWCHICK:
					uvsBoorawchickZero();
					break;
				case CMD::ZERO_ARKAZNOY:
					WorldTable[9] -> GamerVisit = 0;
					break;
//#endif
				}
		p = dgf -> getElement(DGF_NONE,1);
		}
	return ret;
}

// ----------------------------------- dgRoom ------------------------------------
void dgRoom::link(dgRoom*& tail)
{
	if(!tail) tail = this;
	else {
		tail -> prev -> next = this;
		prev = tail -> prev;
		}
	next = NULL;
	tail -> prev = this;
}

void dgRoom::read(dgFile* pf)
{
	roomName = getSubj(pf -> getElement(DGF_NONE));
	escaveNames[0] = roomName;
//#if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
	escaveNames[1] = pf -> getElement(DGF_NONE);
//#endif
	counsillorNames[0] = getSubj(pf -> getElement(DGF_NONE));
//#if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
	counsillorNames[1] = pf -> getElement(DGF_NONE);
//#endif
	if(!oldVersion) bios = atoi(pf -> getElement(DGF_NONE));
	worldNames[0] = getSubj(pf -> getElement(DGF_NONE));
//#if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
	worldNames[1] = pf -> getElement(DGF_NONE);
//#endif
	startX = atoi(pf -> getElement(DGF_NONE));
	startY = atoi(pf -> getElement(DGF_NONE));
	int MPstartX = atoi(pf -> getElement(DGF_NONE));
	int MPstartY = atoi(pf -> getElement(DGF_NONE));
	if(NetworkON){
		startX = MPstartX;
		startY = MPstartY;
		}
	if(!oldVersion) repeatable = atoi(pf -> getElement(DGF_NONE));
	acceptTEXT();
	acceptDIL();
	acceptQUERY();
#ifdef DIAGEN_TEST
	dgMolecule* m = mtail;
	static XStream ff("Check.LOG",XS_OUT);
	ff < "***Escave: " < roomName < "\r\n";
	int i;
	while(m){
		for(i = 0;i < gridSX*gridSY;i++)
			if(grid[i] && !strcmp(m -> name,grid[i] -> Name)) break;
		if(i == gridSX*gridSY) ff < "Unattached molecule: " < m -> name < "\r\n";
		m = m -> next;
		}
	ff < "\r\n";
#endif
}

void dgRoom::acceptTEXT(void)
{
	// std::cout<<"dgRoom::acceptTEXT "<<getDGname(roomName,".text")<<std::endl;
	dgFile* dgf = new dgFile(getDGname(roomName,".text"));

	char* p = NULL;
	int m;
	dgMolecule* pm;
	dgAtom* pa;
	while(1) {
		if(!p) {
			if(!(p = dgf -> getElement(DGF_DUAL,1)))
				break;
		}
		(pm = new dgMolecule(getSubj(p,m))) -> link(mtail);
		pm -> mood = m;
		p = dgf -> getElement(DGF_DUAL);
		
		while(p){
			if(*p == SUBJ_SYMBOL) break;
			(pa = new dgAtom) -> link(pm -> tail);
			p = pa -> read(dgf,p);
		}
		if(!pm -> tail) {
			delete dgf;
			XBuffer m;
			m < "Empty molecule: " < pm -> name;
			ErrH.Abort(m.GetBuf(), XERR_USER, -1, roomName);
		}
		if(!p) break;
	}
	delete dgf;
}

void dgRoom::acceptDIL(void)
{
	//std::cout<<"dgRoom::acceptDIL "<<getDGname(roomName,".dil")<<std::endl;
	dgFile* dgf = new dgFile(getDGname(roomName,".dil"));

	gridSX = atoi(dgf -> getElement(DGF_NONE));
	gridSY = atoi(dgf -> getElement(DGF_NONE));
	memset(grid = new dgCell*[gridSX*gridSY],0,sizeof(dgCell*)*gridSX*gridSY);
	memset(status = new uchar[2*gridSX*gridSY],DG_CELLSTATUS::EMPTY,2*gridSX*gridSY);
	backup_status = status + gridSX*gridSY;

	int num = atoi(dgf -> getElement(DGF_NONE)),i;
	dgf -> getElement(DGF_NONE);
	dgCell* pc;
	for(i = 0;i < num;i++){
		(pc = new dgCell(this)) -> read(dgf);
		setGRID(pc -> x,pc -> y,pc);
		setSTATUS(pc -> x,pc -> y,DG_CELLSTATUS::DORMANT);
		}
	if(getSTATUS(startX,startY) != DG_CELLSTATUS::EMPTY){
		setSTATUS(startX,startY,DG_CELLSTATUS::WAITING);
		explodeState(startX,startY);
		}
}

void dgRoom::acceptQUERY(void)
{
	static dgAtom* adim[64];
	static int ldim[64];
	
	//std::cout<<"dgRoom::acceptQUERY "<<getDGname(roomName,".query")<<std::endl;
	dgFile* pf = new dgFile(getDGname(roomName,".query"));

	prefixQ = pf -> getElement(DGF_DUAL,1);
	postfixQ = pf -> getElement(DGF_DUAL,1);
	char* p = NULL;
	int m;
	dgQuery* pq;
	int i,j;
	while(1) {
		if(!p) {
			if(!(p = pf -> getElement(DGF_DUAL,1))) {
				break;
			}
		}
		(pq = new dgQuery) -> link(qAtail);
		pq -> subj0 = getSubj(p,m);
		p = pf -> getElement(DGF_DUAL);
		pq -> subj1 = getSubj(p,m);//problem

		if (lang() == GERMAN) {
            pq -> subj = pq -> subj0;
		} else if (lang() == RUSSIAN) {
            pq -> subj = pq -> subj0;
		} else {
            pq -> subj = pq -> subj1;
		}

		pq -> mood = m;
		i = j = 0;
		p = pf -> getElement(DGF_DUAL);
		while(p) {
			if(*p == SUBJ_SYMBOL) {
				break;
			}
			if(*p == COMMAND_SYMBOL) {
				i = ldim[j] = atoi(p + 1);
				p = pf -> getElement(DGF_DUAL);
			} else {
				ldim[j] = i;
			}
			adim[j] = new dgAtom;
			p = adim[j++] -> read(pf,p);
			i++;
		}
		if(!j) {
			XBuffer m;
			m < "Empty query in " < roomName;
			ErrH.Abort(m.GetBuf(),XERR_USER,-1,pq -> subj);
		}
		pq -> nanswer = j;
		pq -> answers = new dgAtom*[2*j];
		pq -> levels = (int*)(pq -> answers + j);
		for(i = 0;i < j;i++) {
			pq -> answers[i] = adim[i];
			pq -> levels[i] = ldim[i];
		}
		if(!p) {
			break;
		}
	}
}

void dgRoom::explodeState(int x,int y)
{
	nextState(x - 1,y);
	nextState(x + 1,y);
	nextState(x,y - 1);
	nextState(x,y + 1);
}

void dgRoom::nextState(int x,int y)
{
	if(x < 0 || x >= gridSX || y < 0 || y >= gridSY) return;
	if(getSTATUS(x,y) == DG_CELLSTATUS::DORMANT){
		setSTATUS(x,y,DG_CELLSTATUS::WAITING);
		if(!getGRID(x,y) -> isWaiting) explodeState(x,y);
		}
}

void dgRoom::startSession(void)
{
	currentM = NULL;
	Qcode = eventOUT = 0;
	owner -> endSessionLog = isEmptySession();
	cIndex = -1;
	if(!isLoading){
		singleRandAvailable = 1;
		ClearStatus = 0;
		}
	updateLevel();
	solidQ = (bios == 1);
//#ifndef DIAGEN_TEST
	aciEscaveDead = uvsCurrentWorldUnableBefore;
	aciEscaveEmpty = (*roomName == 'V' && !uvsgetDGdata(DG_EXTERNS::HERE_BOORAWCHICK));
//#endif
	if(checkStatus) backup();
}

void dgRoom::updateLevel(void)
{
	switch(bios){
		case 0: dgLevel = &owner -> eStatus; break;
		case 1: dgLevel = &owner -> bStatus; break;
		case 2: dgLevel = &owner -> zStatus; break;
		case 3: dgLevel = &owner -> sStatus; break;
		}
}

void dgRoom::endSession(void)
{
	if(checkStatus) restore();
	else
		if(!repeatable && !((aciEscaveEmpty || aciEscaveDead))){
			owner -> restHandle(currentM);
			for(cIndex = 0;cIndex < gridSX*gridSY;cIndex++)
				switch(status[cIndex]){
					case DG_CELLSTATUS::WAITING:
						if(grid[cIndex] -> isACCESS()){
							status[cIndex] = DG_CELLSTATUS::OPENED;
							currentM = seekM(grid[cIndex] -> Name);
							if(currentM) currentM -> reset();
							owner -> restHandle(currentM);
							grid[cIndex] -> doCMD(1);
							grid[cIndex] -> doCMD(0);
							}
						break;
					}
			}
	currentM = NULL;
	if(!checkStatus && !(aciEscaveEmpty || aciEscaveDead)){
		int i,j,ind = 0;
		for(j = 0;j < gridSY;j++)
			for(i = 0;i < gridSX;i++,ind++)
				switch(getSTATUS(i,j)){
					case DG_CELLSTATUS::EXPLODED:
					case DG_CELLSTATUS::OPENED:
						if(grid[ind] -> isLooping)
							setSTATUS(i,j,DG_CELLSTATUS::WAITING);
						else {	
							explodeState(i,j);
							setSTATUS(i,j,DG_CELLSTATUS::USED);
							}
						break;
					case DG_CELLSTATUS::HALFLIFE:
						setSTATUS(i,j,DG_CELLSTATUS::WAITING);
						break;
					}
		}
}

int dgRoom::isEmptySession(void)
{
	if(checkStatus) return 0;
	for(int i = 0;i < gridSX*gridSY;i++)
		switch(status[i]){
			case DG_CELLSTATUS::WAITING:
				if(!grid[i] -> isACCESS()) continue;
			case DG_CELLSTATUS::OPENED:
				if(!strcasecmp(grid[i] -> Name,"empty") || ClearStatus) continue;
				return 0;
			}
	return 1;
}

void dgRoom::locateActiveMolecule(void)
{
	owner -> varAtom = NULL;
	currentM = NULL;
	for(cIndex = 0;cIndex < gridSX*gridSY;cIndex++)
		switch(status[cIndex]){
			case DG_CELLSTATUS::WAITING:
				if(!grid[cIndex] -> isACCESS()) continue;
				status[cIndex] = DG_CELLSTATUS::OPENED;
			case DG_CELLSTATUS::OPENED:
				if(!strcasecmp(grid[cIndex] -> Name,"empty") || ClearStatus) continue;
				currentM = seekM(grid[cIndex] -> Name);
				currentM -> reset();
#ifdef DIAGEN_TEST
				if(!checkStatus) std::cout << "\n   Molecule [" << currentM -> name << "] involved (mood " << currentM -> mood << "):" << "\n";
#endif	
				if(grid[cIndex] -> doCMD()) explodeState(cIndex%gridSX,cIndex/gridSX);
#ifdef DIAGEN_TEST
				if(!checkStatus) std::cout << "\n";
#endif	
				status[cIndex] = repeatable ? DG_CELLSTATUS::HALFLIFE : DG_CELLSTATUS::EXPLODED;
				return;
			}
	cIndex--;
}

dgMolecule* dgRoom::getCurrentMolecule(void)
{
	if(currentM && !currentM -> curA && cIndex >= 0 && grid[cIndex]){
		if(repeatable && status[cIndex] == DG_CELLSTATUS::HALFLIFE) status[cIndex] = DG_CELLSTATUS::EXPLODED;
		grid[cIndex] -> doCMD(0);
		}
	if(!currentM || (currentM && !currentM -> curA)) locateActiveMolecule();

	return currentM;
}

dgQuery* dgRoom::seekQall(char* _subj)
{
	if(!_subj) return NULL;
	dgQuery* q = qAtail;
	while(q){
		if(!strcmp(q -> subj,_subj)) return q;
		q = q -> next;
		}
//#ifndef DIAGEN_TEST
//	ErrH.Abort("Query not found",XERR_USER,-1,_subj);
//#endif
	return NULL;
}

dgQuery* dgRoom::seekQvisible(char* _subj)
{
	if(!_subj) return NULL;
	dgQuery* q = qVtail;
	while(q){
		if(!strcmp(q -> subj,_subj)) return q;
		q = q -> next;
		}
//#ifndef DIAGEN_TEST
//	ErrH.Abort("Query not found",XERR_USER,-1,_subj);
//#endif
	return NULL;
}

void dgRoom::save(XStream& ff)
{
	ff < visitCounter < comingCounter;

	int n = gridSX*gridSY;
	int cnt,i;
	char* name;
	for(int ind = DG_CELLSTATUS::USED;ind <= DG_CELLSTATUS::EXPLODED;ind++){
		for(cnt = 0,i = 0;i < n;i++) if(status[i] == ind) cnt++;
		ff < (short)cnt;
		for(i = 0;i < n;i++)
			if(status[i] == ind){
				name = seekM(grid[i] -> Name) -> name;
				ff < (uchar)strlen(name);
				ff.write(name,strlen(name) + 1);
				}
		}

	int num = 0;
	dgQuery* q = qVtail;
	while(q){
		num++;
		q = q -> next;
		}
	ff < num;
	q = qVtail;
	while(q){
		ff < (short)strlen(q -> subj) < (short)q -> lindex;
		ff.write(q -> subj,strlen(q -> subj) + 1);
		q = q -> next;
		}
}

void dgRoom::load(XStream& ff)
{
	currentM = NULL;
	ff > visitCounter > comingCounter;

	char buf[256];
	int n = gridSX*gridSY,i,ind;
	short cnt;
	uchar l;

/*
	if(loadedVersion == 0){
		ff > cIndex;
		dgRoom* r = nDD -> seekR(roomName);
		ff.read(r -> status,gridSX*gridSY);
		for(ind = DG_CELLSTATUS::USED;ind <= DG_CELLSTATUS::EXPLODED;ind++){
			for(i = 0;i < n;i++)
				if(r -> status[i] == ind && r -> grid[i])
					setCell(r -> grid[i] -> Name,ind);
			}
		}
	else {
*/
		for(ind = DG_CELLSTATUS::USED;ind <= DG_CELLSTATUS::EXPLODED;ind++){
			ff > cnt;
			for(i = 0;i < cnt;i++){
				ff > l;
				ff.read(buf,l + 1);
				setCell(buf,ind);
				}
			}
//		}

	dgQuery* q;
	int num;
	short len,li;
	ff > num;
	for(i = 0;i < num;i++){
		ff > len > li;
		ff.read(buf,len + 1);
		if((q = seekQall(buf)) != NULL){
			(q -> delink(qAtail)) -> link(qVtail);
			q -> lindex = li;
			}
		}
}

void dgRoom::setCell(char* name,int value)
{
	for(int i = 0;i < gridSX*gridSY;i++)
		if(grid[i] && !strcasecmp(name,grid[i] -> Name))
			status[i] = value;
//	ErrH.Abort("Wrong setCell() working",XERR_USER,-1,name);
}

dgMolecule* dgRoom::seekM(const char* _name)
{
	dgMolecule* m = mtail;
	while(m){
		if(!strcmp(m -> name,_name)) return m;
		m = m -> next;
		}
#ifdef DIAGEN_TEST
	XBuffer buf;
	buf < _name < "/" < roomName;
	ErrH.Abort("Molecule not found",XERR_USER,-1,buf.GetBuf());
#else		
//	ErrH.Abort("Molecule not found",XERR_USER,-1,_name);
#endif
	return NULL;
}

#ifdef DIAGEN_TEST
void dgRoom::test(void)
{
	int i = 0;
	while(ArgData[i].name) i++;
	if(i != DG_EXTERNS::MAX) ErrH.Abort("MAX comparing failed!");

#ifdef STARTUP_TESTING
	std::cout << "*** " << cp1251_to_utf8(roomName) << " ***\n";
	dgMolecule* m;
	owner -> currentR = this;
	for(i = 0;i < gridSX*gridSY;i++)
		if(grid[i]){
			grid[i] -> isACCESS();
			grid[i] -> doCMD(1);
			grid[i] -> doCMD(0);
			m = seekM(grid[i] -> Name);
			m -> reset();
			while(m -> getPhrase(bios == 4)) m -> goNext();
			m -> reset();
			}
	owner -> currentR = NULL;
	console_clear();
#endif
}
#endif

// ----------------------------------- DiagenDispatcher ------------------------------------
char* DiagenDispatcher::getQprefix(void) {
	if(!currentR)
		return NULL;
	return varAtom ? NULL : Convert(currentR -> prefixQ);
}
char* DiagenDispatcher::getQpostfix(void) {
	if(!currentR)
		return NULL;
	return varAtom ? NULL : Convert(currentR -> postfixQ);
}
char* DiagenDispatcher::getQempty(void) {
	return currentR ? Convert(currentR -> seekM("Empty Question") -> tail -> data) : NULL;
}
char* DiagenDispatcher::getQend(void) {
	return currentR ? (!ClearStatus ? currentR -> seekM("Empty Molecule") -> tail -> data : (char *)"") : NULL; 
}
char* DiagenDispatcher::getQblock(void) {
	return currentR ? Convert(currentR -> seekM("Block Molecule") -> tail -> data) : NULL;
}
char* DiagenDispatcher::getQout(void) {
	dgMood = 3;
	return currentR ? Convert(currentR -> seekM("Out Molecule") -> tail -> data) : NULL; 
}
char* DiagenDispatcher::getQdead(void)
{
	if(!currentR) return NULL;
	if(*currentR -> roomName == 'P' && isSpummyDeath()) return Convert(currentR -> seekM("Last Letter") -> tail -> data);
	return Convert(currentR -> seekM("Dead Molecule") -> tail -> data);
}

void DiagenDispatcher::init(void)
{
	std::cout<<"DiagenDispatcher::init "<<getDGname("room",".lst")<<std::endl;
	FBox = new FileBox;
#ifndef DIAGEN_TEST
	FBox -> load();
#endif	
	dgFile* pf = new dgFile(getDGname("room",".lst"), 0, true);
	char* p = NULL;
	dgRoom* pr;
	int i,j;
	int max  = oldVersion ? 8 : DG_ESCAVE_MAX;
	for (i = 0;i < max;i++) {
		(pr = new dgRoom(this)) -> link(rtail);
		pr -> read(pf);
	}
	if(!oldVersion) {
		for(j = 0;j < 3;j++){
			for(i = 0;i < 3;i++) CycleName[0][j][i] = getSubj(pf -> getElement(DGF_NONE));
//#if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
			for(i = 0;i < 3;i++) CycleName[1][j][i] = pf -> getElement(DGF_NONE);
//#endif
			}
		for(i = 0;i < DG_SECRETW_MAX;i++) SecretWname[0][i] = getSubj(pf -> getElement(DGF_NONE));
//#if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
		for(i = 0;i < DG_SECRETW_MAX;i++) SecretWname[1][i] = pf -> getElement(DGF_NONE);
//#endif
	}

//stalkerg Тест диалогов и скриптов, нужно отключить для большого теста
/*#ifdef DIAGEN_TEST
	pr = rtail;
	while(pr){
		pr -> test();
		pr = pr -> next;
		}
#endif*/

	invR = new dgRoom(this);
	invR -> roomName = "Inventory";
	invR -> acceptTEXT();

#ifdef DIAGEN_TEST
	FBox -> save();
#endif
#ifndef DIAGEN_TEST
	delete pf;
#endif
}

char* DiagenDispatcher::findQfirst(void)
{ 
	if(varAtom)
		return Convert(varAtom -> variants[cVind = 0]);
	else {
		if(!currentR) return NULL;
		cqp = currentR -> qVtail;
		return cqp ? Convert(cqp -> subj) : NULL;
		}
}

char* DiagenDispatcher::findQnext(void)
{ 
	if(varAtom){
		if(++cVind >= varAtom -> nvariant) return NULL;
		return Convert(varAtom -> variants[cVind]);
		}
	else {
		cqp = cqp -> next;
		return cqp ? Convert(cqp -> subj) : NULL;
		}
}

void DiagenDispatcher::startSession(const char* rname)
{
	if(currentR) endSession();

	dgAbortStatus = 0;
	endSessionLog = 0;
	dgRoom* r = seekR(rname);
#ifdef DIAGEN_TEST
	if(!r){
		std::cout << "\nError: room not found...\n\n";
		return;
		}
	std::cout << "\n" << strLine << " SESSION IN " << cp1251_to_utf8(r -> roomName) << " started" << strLine << "\n";
	checkSession(rname);
	if(dgAbortStatus) std::cout << "This session will be aborted!\n";
	std::cout << "\n";
#endif	
	(currentR = r) -> startSession();
	if(!isLoading){
		if(currentR != lastR) currentR -> visitCounter++;
		currentR -> comingCounter++;
		}
	isLoading = 0;
	enter_sStatus = sStatus;
}

void DiagenDispatcher::checkSession(const char* rname)
{
	if(currentR) endSession();
	dgRoom* r = seekR(rname);

	checkStatus = 1;
	(currentR = r) -> startSession();
	while(!dgAbortStatus && !endSessionLog) getNextPhrase();
	endSession();
	checkStatus = 0;
}

void DiagenDispatcher::endSession(void)
{
	if(currentR){
#ifdef DIAGEN_TEST
		if(!checkStatus) std::cout << "\n" << strLine << " SESSION IN " << cp1251_to_utf8(currentR -> roomName) << " finished" << strLine << "\n\n";
#endif
		currentR -> endSession();
		if(!checkStatus) lastR = currentR;
		currentR = NULL;
		}
}

void DiagenDispatcher::involveQ(dgAtom* atom)
{
	if(!atom) return;
	char* p = atom -> findQLfirst();
	dgQuery* q;
	dgRoom* r;
	while(p){
		r = rtail;
		while(r){
			if((q = r -> seekQall(p)) != NULL)
				(q -> delink(r -> qAtail)) -> link(r -> qVtail);
			r = r -> next;
			}
		p = atom -> findQLnext();
		}
}

char* DiagenDispatcher::getNextPhrase(void) {
	if(!currentR) {
		return NULL;
	}

	dgMolecule* m = currentR->getCurrentMolecule();

	char* phr = NULL;
	if(eventOUT) {
		endSessionLog = 1;
		return NULL;
	}
	if(m) {
		dgMood = m->mood;
		involveQ(m->curA);
		phr = m -> getPhrase(currentR->bios == 4);
		varAtom = m -> getVariantsAtom();
//#ifndef DIAGEN_TEST
		if(varAtom) 
			dg_SendEvent(AML_GET_QUESTION);
//#endif
		m->goNext();
	}

	if(!phr) {
		phr = getQend();
		endSessionLog = 1;
	} else {
		endSessionLog = 0;
	}
	phr = Convert(phr);
	dgLog(phr);
	return phr;
}

void DiagenDispatcher::restHandle(dgMolecule* m)
{
	if(!m) return;
	while(m -> curA){
		involveQ(m -> curA);
		m -> goNext();
		}
}

dgRoom* DiagenDispatcher::seekR(const char* _name,int onlyfirstchar)
{
	if(!*_name) return NULL;
	dgRoom* r = rtail;
	if(!onlyfirstchar)
		while(r){
			if(!strcmp(r -> roomName,_name)) return r;
			r = r -> next;
			}
	else
		while(r){
			if(*r -> roomName == *_name) return r;
			r = r -> next;
			}
//#ifndef DIAGEN_TEST
	ErrH.Abort("Room not found",XERR_USER,-1,_name);
//#endif
	return NULL;
}

#ifdef DIAGEN_TEST
void DiagenDispatcher::getStatus(void)
{
	console_clear();
	std::cout << "\nStatus:\n";
	if(currentR){
		std::cout << "\troom: " << currentR -> roomName << "\n";
		std::cout << "\tperson: " << currentR -> counsillorNames[lang() == RUSSIAN ? 1 : 0] << "\n";
		if(dgLevel) std::cout << "\tlevel: " << *dgLevel << "\n";
		}
	else
		std::cout << "\tin fly...\n";
}
#endif

char* DiagenDispatcher::getAnswer(char* subj)
{
#ifdef DIAGEN_TEST
	subj = strdup(subj);
#endif
	char* subject = Convert(subj,1);
	if(varAtom){
		char* ret = NULL;
		Qcode = varAtom -> getVcode(subject,ret);
//#ifndef DIAGEN_TEST
		dg_SendEvent(Qcode);
		dg_SendEvent(AML_CLEAR_QUESTION);
//#endif
		varAtom = NULL;
		return Convert(ret);
		}

//#ifndef DIAGEN_TEST
	if(aciEscaveEmpty || aciEscaveDead) return NULL;
//#endif
	dgQuery* q = currentR -> seekQvisible(subject);

#ifdef DIAGEN_TEST
	std::cout << "Subject: " << cp866_to_cp1251(cp1251_to_utf8(subj));
	free(subj);
#endif
	if(q){
		dgMood = q -> mood;
		int n = q -> getIndex();
#ifdef DIAGEN_TEST
		std::cout << " (" << n << ")\n";
#endif
		char* str = q -> answers[n] -> data;
		involveQ(q -> answers[n]);

		str = Convert(str);
#ifdef DIAGEN_TEST
		std::cout << str << "\n";
#endif
		return str;
		}
#ifdef DIAGEN_TEST
		std::cout << "\n" << (q ? "Not available...\n" : "Not found...\n");
#endif
	return NULL;
}

void DiagenDispatcher::save(XStream& ff)
{
/*#ifdef DIAGEN_TEST
	ff.open("test.sav",XS_OUT);
#endif*/

	ff < saveVersion;
	ff < eStatus < bStatus < zStatus < sStatus;
	ff < singleRandAvailable < ClearStatus < wasRubeecated;

	dgRoom* r = rtail;
	int num = 0;
	while(r){
		num++;
		r = r -> next;
		}
	ff < num;
	r = rtail;
	while(r){
		ff < (short)strlen(r -> roomName);
		ff.write(r -> roomName,strlen(r -> roomName) + 1);
		r -> save(ff);
		r = r -> next;
		}
}

void DiagenDispatcher::load(XStream& ff)
{
	std::cout<<"DiagenDispatcher::load"<<std::endl;
/*#ifdef DIAGEN_TEST
	ff.open("test.sav",XS_IN);
//	ff.seek(68734,XS_BEG);
#endif*/
	isLoading = 1;

	ff > loadedVersion;
//	if(loadedVersion < 0 || loadedVersion > saveVersion) ErrH.Abort("Intolerable dgVersion");
	if(!(loadedVersion == saveVersion || loadedVersion == saveVersion - 1)) ErrH.Abort("Intolerable dgVersion");

/*	if(loadedVersion == 0){
		strcpy(DiagenTextName + strlen(DiagenTextName) - 4,"lst");
		oldVersion = 1;
		nDD = new DiagenDispatcher;
		nDD -> init();
		strcpy(DiagenTextName + strlen(DiagenTextName) - 3,"text");
		oldVersion = 0;
		}
*/
	ff > eStatus > bStatus > zStatus;
	if(loadedVersion >= 2) ff > sStatus;
	ff > singleRandAvailable > ClearStatus;
	if(loadedVersion >= 3) ff > wasRubeecated;

	int num;
	short len;
	char buf[256];
	dgRoom* r,*rr;
	ff > num;
	for(int i = 0;i < num;i++){
		ff > len;
		ff.read(buf,len + 1);
		r = seekR(buf);
		if(r) r -> load(ff);
		}

	dgQuery* q,*qq;
	r = rtail;
	while(r){
		q = r -> qVtail;
		while(q){
			rr = rtail;
			while(rr){
				if((qq = rr -> seekQall(q -> subj)) != NULL)
					(qq -> delink(rr -> qAtail)) -> link(rr -> qVtail);
				rr = rr -> next;
				}
			q = q -> next;
			}
		r = r -> next;
		}
}

char* DiagenDispatcher::getWorldName(char* name)
{
	if(name){
		dgRoom* r = rtail;
		while(r){
			if(!strcmp(name,r -> worldNames[0])) return r -> worldNames[lang() == RUSSIAN ? 1 : 0];
			r = r -> next;
			}
		for(int i = 0;i < DG_SECRETW_MAX;i++)
			if(!strcmp(name,SecretWname[0][i])) return SecretWname[lang() == RUSSIAN ? 1 : 0][i];
		}
	ErrH.Abort("dgWorldName not found");
	return NULL;
}

char* DiagenDispatcher::getInvText(int type,int id,char* from,char* to)
{
	static XBuffer buf(64);
	dgMolecule* m;
	buf.init();
	switch(type){
		case DG_POPONKA:
			buf < "Poponka " <= id;
			m = invR -> seekM(buf.GetBuf());
			return Convert(m -> tail -> data);
		case DG_TABUTASK:
			buf < "Tabutask " < *from <= id + 1;
			m = invR -> seekM(buf.GetBuf());
			return Convert(m -> tail -> data);
		case DG_PASSENGER:
			buf < "Passenger " < *from < "->" < *to < "";
			m = invR -> seekM(buf.GetBuf());
			return Convert(m -> tail -> data);
		case DG_FINAL:
			buf < "Final " <= id;
			m = invR -> seekM(buf.GetBuf());
			return Convert(m -> tail -> data);
		}
	return NULL;
}

void FileBox::load(void)
{
#ifdef _ROAD_
	Parser ff(DiagenTextName());
	ff.set(2,XB_BEG);
#else
	XStream ff(DiagenTextName(),XS_IN);
	ff.seek(2,XS_BEG);
#endif
	ff > cN;
	names = new char*[cN];
	lens = new int[cN];
	data = new char*[cN];
	uchar l;
	for(int i = 0;i < cN;i++){
		ff > l; l++;
		names[i] = new char[l];
		ff.read(names[i],l);
		ff > lens[i];
		data[i] = new char[lens[i]];
		ff.read(data[i],lens[i]);
		}
}

char* FileBox::get(char* fname,int& len)
{
	for(int i = 0;i < cN;i++)
	{
		if(!strcmp(fname,names[i])){
			len = lens[i];
			return data[i];
			}
	}

	ErrH.Abort("FileBox: entry not found",XERR_USER,-1,fname);
	return NULL;
}

#ifdef DIAGEN_TEST
void FileBox::add(char* fname,char* buf,int len)
{
	names[cN] = strdup(fname);
	lens[cN] = len;
	memcpy(data[cN] = new char[len],buf,len);
	cN++;
}

void FileBox::save(void)
{
	static int ret = 0;
	if(ret) return;
	
	XStream ff("diagen_new.text",XS_OUT); //see definition of DIAGEN_TEXT

	ff.write("DG",2);
	ff < cN;
	for(int i = 0;i < cN;i++){
		ff < (uchar)strlen(names[i]);
		ff.write(names[i],strlen(names[i]) + 1);
		ff < lens[i];
		ff.write(data[i],lens[i]);
		}
	ff.close();
	std::cout << "Diagen.text saved..."<<std::endl;
	ret = 1;
}
#endif

void dgInThreall(void)
{ 
	dgD -> startSession("Threall"); 
}
void dgOutThreall(void)
{ 
	dgD -> endSession(); 
}
char* dgGetThreallPhrase(void)
{ 
	return dgD -> getNextPhrase(); 
}
