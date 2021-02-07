#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <crtdbg.h>
#include <malloc.h>

#ifdef _DEBUG
int __GlobalSize__;
int __cdecl MyAllocHook(  int  nAllocType,  void   * pvData,  unsigned int   nSize,  int      nBlockUse,  long     lRequest,  const char * szFileName,  int      nLine  )
{
/*   char *operation[] = { "", "allocating", "re-allocating", "freeing" };
   char *blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };*/

   /*if ( nBlockUse == _CRT_BLOCK )   // Ignore internal C runtime library allocations
      return( 1 );*/


   switch(nAllocType){
	 case _HOOK_ALLOC:   __GlobalSize__ += nSize;
		 break;
	case _HOOK_FREE:   
		__GlobalSize__ -= nSize;
		break;
   }

   return( 1 );         // Allow the memory operation to proceed
}
#endif

void win32_debugSet(void)
{
#ifdef _DEBUG
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_REPORT_FLAG;

	tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;	// включает проверку heap'а
//	tmpDbgFlag &= ~_CRTDBG_ALLOC_MEM_DF;		// выключае проверку hеap'а

//	tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;		// проверка heap'а при каждом выделении памяти
	tmpDbgFlag |= _CRTDBG_CHECK_CRT_DF;			// проверка внутреннего runtime-heap'а
//	  tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;	// не освобождать на самом деле память
//	  tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;			// ловить лики
	_CrtSetDbgFlag(tmpDbgFlag);

	_CrtSetAllocHook( MyAllocHook );
#endif
}

void win32_check(void)
{
	_ASSERTE(_CrtCheckMemory()) ;
}

void win32_getMemStat(int& max,int& cur)
{
#ifdef _DEBUG
	_CrtMemState state;
	_CrtMemCheckpoint(&state);
	max = state.lHighWaterCount;
	cur = state.lTotalCount;
#endif
}
