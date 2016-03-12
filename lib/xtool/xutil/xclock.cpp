
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "xglobal.h"

long long tick_per_sec=1000;
long long beg_tick=0;
const unsigned int MS_PER_PERIOD=1;

//stalkerg:Function for get ticks
long long getRDTSC()
/*#define CPUID __asm _emit 0xf __asm _emit 0xa2
#define RDTSC __asm _emit 0xf __asm _emit 0x31
{
	__int64 timeRDTS;
	__asm {
		push ebx
		push ecx
		push edx
		RDTSC
		mov dword ptr [timeRDTS],eax
		mov dword ptr [timeRDTS+4],edx
		pop edx
		pop ecx
		pop ebx
	}
	return time;
}*/
{
	return SDL_GetTicks(); //stalkerg: winnt function for get tick milliseconds.
}


void initclock()
{
	if(tick_per_sec!=0)
		return;
/*	clock_t t1,t2;
	__int64 tickb,ticke;
	t1=t2=clock();
	while(t1==t2){ 
		t2=clock();
	};

	tickb=getRDTSC();
	t1=t2;
	while(t1+1000 > t2){
		t2=clock();
	}
	ticke=getRDTSC();
	tick_per_sec = (ticke-tickb)/1000;
	beg_tick=getRDTSC();
*/
	
//	timeBeginPeriod(1);

/*	unsigned int t1,t2;
	__int64 tickb,ticke;
	t1=t2=timeGetTime();
	while(t1==t2){ 
		t2=timeGetTime();
	};

	tickb=getRDTSC();
	t1=t2;
	while(t1+MS_PER_PERIOD > t2){
		t2=timeGetTime();
	}
	ticke=getRDTSC();
	tick_per_sec = (ticke-tickb)/MS_PER_PERIOD;
	beg_tick=getRDTSC();
	*/

	/*WORKunsigned int t1,t2;
	long long tickb,ticke;
	t1=t2=getRDTSC();
	while(t1==t2){ 
		t2=getRDTSC();
	};

	tickb=getRDTSC();
	t1=t2;
	while(t1+MS_PER_PERIOD > t2){
		t2=getRDTSC();
	}
	ticke=getRDTSC();
	tick_per_sec = (ticke-tickb)/MS_PER_PERIOD;*/
	beg_tick=getRDTSC();


std::cout<<"initclock() "<<tick_per_sec<<" b:"<<beg_tick<<std::endl;
//	timeEndPeriod(1);
	

/*	LARGE_INTEGER TK_PER_SEC;
	QueryPerformanceFrequency(&TK_PER_SEC);

	__int64 t1,t2;
	__int64 tickb,ticke;
	t1=t2=getQPC(); 
	while(t1==t2){ 
		t2=getQPC();
	};

	tickb=getRDTSC();
	t1=t2;
	while(t1+TK_PER_SEC.QuadPart > t2){
		t2=getQPC();
	}
	ticke=getRDTSC();
	tick_per_sec = (ticke-tickb)/1000;//MS_PER_SEC;
	beg_tick=getRDTSC();*/

}

double clockf()
{
//std::cout<<"clockf() "<<(double)(getRDTSC()-beg_tick)/(double)tick_per_sec<<std::endl;
//	return (double)(getRDTSC()-beg_tick)/(double)tick_per_sec;
	return (double)SDL_GetTicks()*MS_PER_PERIOD;
} 

int clocki()
{
//std::cout<<"clocki() "<<(int)((getRDTSC()-beg_tick)/tick_per_sec)<<std::endl;
//	return (int)((getRDTSC()-beg_tick)/tick_per_sec);
//	return (int)((getRDTSC()-beg_tick)*1000);
	return (SDL_GetTicks()*MS_PER_PERIOD);
}

