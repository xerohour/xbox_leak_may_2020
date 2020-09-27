/*
 * Some standard libraries
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

/*
 * include NVUCL
 */
#include "..\nvucl.h"
#pragma hdrstop

/*
 * globals
 */
NVUCL_PROFILER profiler = NULL;

/*
 * creation code
 */
int createProfiler (void)
{
    if (nvuclGetInterface(NVUCL_PROFILER_ID,&profiler))
    {
        return 0; // fail
    }
    return 1; // success
}
int destroyProfiler (void)
{
    profiler->pVPtr->detach(profiler);
    return 1;
}


/*
 * main entry point
 */
int main (void) 
{
    DWORD i;
    
    /*
     * create a profiler instance
     */
    if (!createProfiler()) return 1;

    /*
     * create alphabet
     */
    profiler->pVPtr->registerEvent (profiler,"event.1",0x00);
    profiler->pVPtr->registerCount (profiler,"count.1",0x11);
    profiler->pVPtr->registerCount (profiler,"count.2",0x22);
    profiler->pVPtr->registerTime  (profiler,"time.1", 0x33);

    /*
     * start capture
     */
    profiler->pVPtr->startCapture(profiler,"test.btd");

    /*
     * do something
     */
    for (i=0; i<1000; i++)
    {
        /*
         * flush the file buffers at known times so that we can work around
         * timing the HDD writing. logXXX functions will flush if internal
         * buffer overflows - but we consider that uncontrolled
         */
        if ((i % 100) == 99)
        {
            /*
             * stop all active timers here
             */
            // ...
            /*
             * flush
             */
            profiler->pVPtr->flush(profiler);
            /*
             * restart all active timers here
             */
            // ...
        }

        switch (rand() & 3)
        {
            case 0: profiler->pVPtr->logEvent (profiler,0x00);
                    break;
            case 1: profiler->pVPtr->logCount (profiler,0x11,rand());
                    break;
            case 2: profiler->pVPtr->logCount (profiler,0x22,rand());
                    break;
            case 3: {
                __int64 time1,time2;
                profiler->pVPtr->getCPUTime (profiler,&time1);
                Sleep (rand() & 15);
                profiler->pVPtr->getCPUTime (profiler,&time2);
                profiler->pVPtr->logTime    (profiler,0x33,time2-time1);
                break;
            }
        }
    }
 
    /*
     * stop capture
     */
    profiler->pVPtr->stopCapture(profiler);

    /*
     * done
     */
    if (!destroyProfiler()) return 1;
    return 0;
}