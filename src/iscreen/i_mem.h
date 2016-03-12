
#define _iALLOC_(t,p)		p = (t*)(iMemHeap + iHeapOffs); iHeapOffs += sizeof(t);     \
				if(iHeapOffs >= iHeapSize) ErrH.Abort("MEMOUT...", iHeapSize)

#define _iALLOC_A_(t,p,n)	p = (t*)(iMemHeap + iHeapOffs); iHeapOffs += n * sizeof(t); \
				if(iHeapOffs >= iHeapSize) ErrH.Abort("MEMOUT...", iHeapSize)

#define _iFREE_(t,p)		p = NULL; iHeapOffs -= sizeof(t);

#define _iFREE_A_(t,p,n)	p = NULL; iHeapOffs -= n * sizeof(t);

#define _iALLOC_HEAP_(num,t)	iMemHeap = new char[num * sizeof(t)]; iHeapOffs = 0; iHeapSize = num * sizeof(t)
#define _iFREE_HEAP_(num,t)	if(iHeapSize){ delete[] iMemHeap; iHeapSize = 0; }

#define _iSTORE_OFFS_		int __ihp_offs = iHeapOffs;
#define _iRESTORE_OFFS_ 	int iHeapOffs = __ihp_offs;

extern char* iMemHeap;
extern unsigned iHeapOffs;
extern unsigned iHeapSize;

