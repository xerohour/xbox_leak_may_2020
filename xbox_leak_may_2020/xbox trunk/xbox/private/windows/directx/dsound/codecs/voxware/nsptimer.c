/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:   NspTimer.c                                                                    
*                                                                              
* Purpose:    High resolution((1 microsecond)Win32 Timer                                                                
*                                                                              
* Functions:  
*
*           TSTAPI(void)   tstTimerStart(int No)   : start Timer #No
*
*           TSTAPI(void)   tstTimerStop(int No)    : Stop  Timer #No
*
*           TSTAPI(double) tstTimerClock(int No)   : return Timer #No Counts 
*                                                    in clock ticks
*
*           TSTAPI(double) tstTimerSec(int No)     : return Timer #No Counts
*                                                    in second
*
*           TSTAPI(double) tstTimerUSec(int No)    : return Timer #No Counts 
*                                                    in micro-second
*
*                                                                         
* Author/Date: Xiangdong Fu
*              (originated from Intel NSP Lib)  1/6/97                                                              
********************************************************************************
* Modifications:
*                   
* Comments:      Up to ten timers is available for use(0-9)
*                Users can increase the available timer by 
*                increase TST_MAX_TIMER
*                
*                
*
*
* Concerns:
*
*******************************************************************************/

#ifdef WIN32

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <windows.h>

#include "NspTimer.h"

#include "xvocver.h"


/*****************************************************************/
/* --- Timer relatied routines ----------------------------------*/
/*****************************************************************/

/* #define DBG_PRINT(s) printf s*/
#define DBG_PRINT(s)

#define USE_WIN32_TIMER
//#define HIGH_PRIORITY
#define REAL_TIME_PRIORITY

/*---------------------------------------------------------------*/
/* --- Define the timer structure -------------------------------*/
/*---------------------------------------------------------------*/
/* Timers [0..TST_MAX_TIMER], where               */
/*        the first half are High Priority Timers */
#define TST_MAX_TIMER		9
#define TST_MIN_HP_TIMER	0
#define TST_MAX_HP_TIMER	TST_MAX_TIMER/2

#define TIMER_STOPPED 0
#define TIMER_RUNNING 1

typedef struct _Timer_t {
   int state;
   double total;
   double start;
} Timer_t;

static Timer_t     Timer[TST_MAX_TIMER+1]={{0}};
static int         HighPriorityTimerCount=0;

/*---------------------------------------------------------------*/
/* --- Decide if priority will raise priority while looping -----*/
/*---------------------------------------------------------------*/

#ifdef HIGH_PRIORITY 
static void tstHighPriority(void) {
   SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
}
static void tstNormalPriority(void) {
   SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
}
#elif defined REAL_TIME_PRIORITY
static void tstHighPriority(void) {
   SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
   SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
}
static void tstNormalPriority(void) {
   SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
   SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
}

#else
#define tstHighPriority()
#define tstNormalPriority()
#endif

/*---------------------------------------------------------------*/
/* --- Private Timing routines : use Win32 high performance -----*/
/* ---    Counter or  use clock function    ---------------------*/
/*---------------------------------------------------------------*/

#ifdef USE_WIN32_TIMER

static int first_time = TRUE;
static double tick_frequency;
static LARGE_INTEGER tick_counts = {{0,0}};

static double getCounter(void)
{
   if (!QueryPerformanceCounter(&tick_counts)) 
      return (double)clock();
   return ((double)tick_counts.u.HighPart * 65536.0 * 65536.0 +
      (double)tick_counts.u.LowPart);
}

static double getFrequency(void)
{
   LARGE_INTEGER cps;
   if (!first_time) return tick_frequency;

   if (!QueryPerformanceFrequency(&cps))  {
      tick_frequency = (double)CLOCKS_PER_SEC;
   } else {
      tick_frequency = (double)cps.u.HighPart * 65536.0 * 65536.0 +
         (double)cps.u.LowPart;
   }
   first_time = FALSE;
   return tick_frequency;
}
#else /* use clock() function */
#define getCounter(ptr)		(double)clock()
#define getFrequency(ptr)	(double)CLOCKS_PER_SEC
#endif

/*****************************************************************/
/* ---      API Timer routines ----------------------------------*/
/*****************************************************************/

TSTAPI(void) tstTimerStart(int No)
{
   if ((No<0) || (No>TST_MAX_TIMER) || 
      (Timer[No].state ==TIMER_RUNNING)) return;

   /* Priority Timers will raise priority */
   if ((No>=TST_MIN_HP_TIMER) && (No<=TST_MAX_HP_TIMER)) {
      if (HighPriorityTimerCount==0) tstHighPriority();
      HighPriorityTimerCount++;
   }

   Timer[No].state=TIMER_RUNNING;
   Timer[No].total=0.0;
   Timer[No].start=getCounter();
   DBG_PRINT(("\nIn TimerStart/start: %10.2f ", Timer[No].start));
   return;
}

TSTAPI(void) tstTimerStop(int No)
{
   double tempClock = getCounter();
   DBG_PRINT(("\nIn TimerStop/stop : %10.2f ", tempClock));

   if ((No<0) || (No>TST_MAX_TIMER) || 
      (Timer[No].state==TIMER_STOPPED)) return;

   /* Priority Timers will raise priority */
   if ((No>=TST_MIN_HP_TIMER) && (No<=TST_MAX_HP_TIMER)) {
      HighPriorityTimerCount--;
      if (HighPriorityTimerCount==0) tstNormalPriority();
   }

   Timer[No].state=TIMER_STOPPED;
   Timer[No].total=Timer[No].total+tempClock-Timer[No].start;
   DBG_PRINT(("\nIn TimerStop/total: %10.2f ", Timer[No].total));
   return;
}

TSTAPI(double) tstTimerClock(int No)
{
   double tempClock = getCounter();

   if ((No<0) || (No>TST_MAX_TIMER)) return 0;
   if (Timer[No].state==TIMER_STOPPED)
      return Timer[No].total;
   else
      return Timer[No].total-Timer[No].start+tempClock;
}

TSTAPI(double) tstTimerSec(int No)
{
   return tstTimerClock(No) / getFrequency();
}

TSTAPI(double) tstTimerUSec(int No)
{
   return tstTimerClock(No) / getFrequency() * USECS;
}

/*****************************************************************/
/* --- End of timer routines ------------------------------------*/
/*****************************************************************/
#endif
