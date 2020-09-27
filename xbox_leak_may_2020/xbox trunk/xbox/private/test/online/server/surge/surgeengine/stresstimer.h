#ifndef _STRESSTIMER_H_
#define _STRESSTIMER_H_

#include "reporting.h"

class StressTimer
    {
    #define STRESS_TIMER_NAME_LEN 1024
    public:
        char name[STRESS_TIMER_NAME_LEN];
        DWORD dwTime;
        StressTimer *next;
        Average avg;

    public:
        StressTimer* Add(char *n);
        StressTimer* Find(char *n);

    public:
        StressTimer();
        StressTimer(char *n);
        ~StressTimer();

    public:
        DWORD Start(void);
        DWORD Stop(void);
        DWORD GetTime(void);
    };

#endif // _STRESSTIMER_H_