
//#define _iSCRIPT_DEBUG_
//#define _BINARY_SCRIPT_
//#define _SAVE_BINARY_SCRIPT_
//#define _SAVE_PACKED_SCRIPT_

//#define _BINARY_SCRIPT_
#ifndef _BINARY_SCRIPT_
//zhfo - _SAVE_BINARY_SCRIPT_ работает нормально только если не продефайнен _BINARY_SCRIPT_
#define _SAVE_BINARY_SCRIPT_
//#define _SAVE_PACKED_SCRIPT_ //znfo - а мы не знаем зачем это тут вообще
#endif

const int 	iSCRIPT_INCLUDE_MAX	= 10;
const int 	iSCRIPT_EOF		= 8383;
const int 	iSCRIPT_HEAP_SIZE	= 3000000;

struct ScriptFileBuffer
{
	int size;
	int offset;
	int offset_size;
	char* buffer;

	ScriptFileBuffer* prev;
	ScriptFileBuffer* next;

	void save(XStream* fh);
	void load(XStream* fh);

	ScriptFileBuffer(void);
	~ScriptFileBuffer(void);
};

struct ScriptBlock
{
	char* data;

	ScriptBlock* next;
	ScriptBlock* prev;

	void clear(void);
};

struct ScriptLine
{
	int ID;
	int Size;

	ScriptBlock* first;
	ScriptBlock* last;

	void connect(ScriptBlock* p);
	void dconnect(ScriptBlock* p);
	void init(void);

	void insert(ScriptBlock* p,ScriptBlock* p1);
	void merge(ScriptBlock* p,ScriptBlock* beg,ScriptBlock* end);

	ScriptLine* next;
	ScriptLine* prev;

	ScriptLine(void);
};

struct ScriptDefine
{
	ScriptLine* line;

	int StartLine;
	int EndLine;

	ScriptDefine* next;
	ScriptDefine* prev;

	void add(ScriptLine* p);

	~ScriptDefine(void);
	ScriptDefine(void);
};

// Modes for SeekComm() & SeekCommPos()
#define C_COMMENT		0
#define CPP_COMMENT		1
#define C_COMMENT_END		2
#define CPP_COMMENT_END 	3

struct ScriptFile
{
	int Size;
	int EOF_Flag;
	int CurOffs;
	char* buffer;
	char* cur_ptr;
	char* prevStrBuf;

	int CompressLog;

#ifdef _SAVE_BINARY_SCRIPT_
	XStream* bin_fh;
#endif

#ifdef _BINARY_SCRIPT_
	XBuffer* binConv;
	char* binConvPtr;
#endif
	char* binConvBuf;
	int binConvBufSize;

	int binConvData;

	int NumStr;
	ScriptLine* firstLine;
	ScriptLine* lastLine;
	ScriptLine* curLine;

	int NumBufs;
	ScriptFileBuffer* fBuf;
	ScriptFileBuffer* lBuf;

#ifdef _SAVE_BINARY_SCRIPT_
	char* bScriptName;
#endif

	ScriptBlock* curBlock;

	int NumDef;
	ScriptDefine* fDef;
	ScriptDefine* lDef;

	int SeekPos(char* s);
	int Seek(char* s);

	int SeekCharPos(char s);
	int SeekChar(char s);

	int SeekComm(int mode);
	int SeekCommPos(int mode);

	int get_cur_str(void);
	char* get_ptr(void);
	char* get_conv_ptr(void);
	void next_ptr(void);
	void next_line(void);

	void next_src_line(void);
	void read_src_line(void);

	void load(const char* fname);
	void next(void);
	void prepare(void);
	void preprocess(void);
	void strip_comments(void);

	int check_math(void);
	int parse_math(void);

	void reset(void);

#ifdef _iSCRIPT_DEBUG_
	void write_log(void);
#endif

#ifdef _SAVE_BINARY_SCRIPT_
	void set_bscript_name(const char* fname);
	void open_bscript(const char* fname);
	void close_bscript(void);

	void write_idata(int val);
	void write_pdata(char* ptr);

	void write_end(void);
#ifdef _SAVE_PACKED_SCRIPT_
	void pack_script(void);
#endif
#endif

#ifdef _BINARY_SCRIPT_
	void load_bscript(const char* fname);
	void free_bscript(void);

	int check_bsign(void);
#endif
	int read_option(int mode = 1);
	int read_key(void);

	int read_idata(void);
	void read_pdata(char** ptr,int mode = 0);

	int get_idata(void);
	void get_pdata(char** ptr,int mode = 0);
	char* get_pdata_ptr(void);

	void prepare_pdata(void);
	void prepare_idata(void);

	void add_line(ScriptLine* p);
	void remove_line(ScriptLine* p);
	void add_def(ScriptLine* p);

	void add_buf(ScriptFileBuffer* p);
	void free_buffers(void);
	int process_includes(void);

	ScriptFile(void);
	~ScriptFile(void);
};

/* ------------------------- Math Expression Parser ------------------------- */

#ifndef _BINARY_SCRIPT_
// Operation codes...
enum scMathOpCodes
{
	MT_PLUS,	// 0
	MT_MINUS,	// 1
	MT_MUL, 	// 2
	MT_DIV, 	// 3
	MT_LEFT_BR,	// 4
	MT_RIGHT_BR,	// 5

	MT_MAX_CODE
};

struct scMathNode
{
	int value;		// = priority for operations...
	int op_code;		// = -1 for operands...

	scMathNode* next;
	scMathNode* prev;

	scMathNode(void){ op_code = -1; value = 0; }
};

struct scMathExpression
{
	int Size;
	int MaxValue;

	scMathNode* first;
	scMathNode* last;

#ifdef _iSCRIPT_DEBUG_
	XStream logFile;

	void write(void);
#endif

	void clear(void){ first = last = NULL; Size = 0; MaxValue = 0; }

	int check_math(char* p);

	void add2list(ScriptBlock* p);
	void free_list(void);

	void prepare_list(void);
	int eval(void);
	void eval_quant(void);

	void connect(scMathNode* p);
	void dconnect(scMathNode* p);

	scMathExpression(void);
	~scMathExpression(void);
};
#endif
