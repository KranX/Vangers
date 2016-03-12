
#define _sALLOC_(t,p)		p = (t*)(sMemHeap + sHeapOffs); sHeapOffs += sizeof(t);     \
				if(sHeapOffs >= sHeapSize) ErrH.Abort("MEMOUT...", sHeapSize)

#define _sALLOC_A_(t,p,n)	p = (t*)(sMemHeap + sHeapOffs); sHeapOffs += (n) * sizeof(t); \
				if(sHeapOffs >= sHeapSize) ErrH.Abort("MEMOUT...", sHeapSize)

#define _sFREE_(t,p)		p = NULL; sHeapOffs -= sizeof(t);

#define _sFREE_A_(t,p,n)	p = NULL; sHeapOffs -= (n) * sizeof(t);

#define _sALLOC_HEAP_(num,t)	sMemHeap = new char[num * sizeof(t)]; sHeapOffs = 0; sHeapSize = (num) * sizeof(t)
#define _sFREE_HEAP_(num,t)	if(sHeapSize){ delete[] sMemHeap; sHeapSize = 0; }

extern char* sMemHeap;
extern unsigned sHeapOffs;
extern unsigned sHeapSize;

