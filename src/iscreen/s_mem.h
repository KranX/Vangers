
// Macro expansion mixes character strings and ScriptBlock objects in one arena.
// Align each typed allocation before exposing it to the parser.
#define _sALLOC_A_(t, p, n)                                                 \
	sHeapOffs = (sHeapOffs + alignof(t) - 1) & ~(alignof(t) - 1);           \
	if (sHeapOffs > sHeapSize || (n) > (sHeapSize - sHeapOffs) / sizeof(t)) \
		ErrH.Abort("MEMOUT...", sHeapSize);                                 \
	p = (t *)(sMemHeap + sHeapOffs);                                        \
	sHeapOffs += (n) * sizeof(t)

#define _sALLOC_(t, p) _sALLOC_A_(t, p, 1)

#define _sFREE_(t, p) \
	p = NULL;         \
	sHeapOffs -= sizeof(t);

#define _sFREE_A_(t, p, n) \
	p = NULL;              \
	sHeapOffs -= (n) * sizeof(t);

#define _sALLOC_HEAP_(num, t)             \
	sMemHeap = new char[num * sizeof(t)]; \
	sHeapOffs = 0;                        \
	sHeapSize = (num) * sizeof(t)
#define _sFREE_HEAP_(num, t) \
	if (sHeapSize) {         \
		delete[] sMemHeap;   \
		sHeapSize = 0;       \
	}

extern char *sMemHeap;
extern unsigned sHeapOffs;
extern unsigned sHeapSize;
