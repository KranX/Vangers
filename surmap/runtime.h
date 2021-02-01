
//const RTO_SURMAP_TIMER	= CLOCKS_PER_SEC/20;
const int RTO_SURMAP_TIMER	= 0;

// Runtime objects...
enum SurmapRuntimeObjID
{
	RTO_SURMAP_ID  = 1,

	RTO_MAX_ID
};

struct SurmapRTO : XRuntimeObject
{
	virtual int Quant(void);

	SurmapRTO(void) : XRuntimeObject(RTO_SURMAP_ID){ Timer = RTO_SURMAP_TIMER; }
};
