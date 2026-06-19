
//const RTO_SURMAP_TIMER	= CLOCKS_PER_SEC/20;
const int RTO_SURMAP_TIMER	= 0;
#ifdef DEFINE_GAME_RTO_TIMERS
#define EXTERN /* nothing */
#else
#define EXTERN extern
#endif /* DEFINE_GAME_RTO_TIMERS */
EXTERN int RTO_GAME_QUANT_TIMER;
EXTERN double GAME_TIME_COEFF; // speed correction coefficient, use for static parameters only, like bullet speed, animation speed...


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
