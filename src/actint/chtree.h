
// Element IDs...
#define ACT_ROOT	0x00
#define ACT_DATA	0x01

struct aciCHTreeElement : XListElement
{
	int ID;
	int data;

	XList* nextLevel;

	aciCHTreeElement* findNext(int dt);

	void save(XBuffer* fh);
	void load(XBuffer* fh);

	aciCHTreeElement(void);
	~aciCHTreeElement(void);
};

struct aciCHTree
{
	aciCHTreeElement* rootEl;
	aciCHTreeElement* currentEl;

	void add_string(char* str,int id);
	int quant(int k);

	void reset(void){ currentEl = rootEl; }

	void save(char* fname);
	void load(const char* fname);

	aciCHTree(void);
	~aciCHTree(void);
};
