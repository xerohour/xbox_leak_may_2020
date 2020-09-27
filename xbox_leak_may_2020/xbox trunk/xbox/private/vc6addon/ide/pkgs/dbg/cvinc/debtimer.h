/** debtimer.h - quick include file for simple CV timing
 *
 */



#include <time.h>
#include <sys\timeb.h>

typedef struct _timearray {
	clock_t 	tWinMain;
	clock_t 	tMain;
	clock_t 	tExeLoadStart;
	clock_t 	tExeLoadEnd;
	clock_t 	tOmfLoadStart;
	clock_t 	tOmfLoadEnd;
	clock_t 	tMkModuleStart;
	clock_t 	tMkModuleEnd;
	clock_t 	tPublicsStart;
	clock_t 	tPublicsEnd;
	clock_t 	tGlobalsStart;
	clock_t 	tGlobalsEnd;
	clock_t 	tTypesStart;
	clock_t 	tTypesEnd;
	clock_t 	tDupStart;
	clock_t 	tDupEnd;
	clock_t 	tSymLoad;
	clock_t 	tSrcModLoad;
	clock_t 	tLoadEnd;
	clock_t 	tExit;

} TIMEARRAY;

extern TIMEARRAY	TimeArray;


#if defined (_DEBUG)
#define TIMER(name) TimeArray.name=clock()
#else
#define TIMER(name) /*TimeArray.name=clock()*/
#endif

#define InitTimer()
#define StartTimer(itm)
#define StopTimer(itm)
#define ShowElapsedTime(itm)
