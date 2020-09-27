#include "stdafx.h"
#include "stresstimer.h"

StressTimer::StressTimer()
    {
    next = NULL;
    name[0] = '\0';
    dwTime = 0;
    }

StressTimer::StressTimer(char *n)
    {
    // NOTE: any name over 1K will be truncated!
    next = NULL;
    name[0] = '\0';
    dwTime = 0;
    if(n)
        {
        strncpy(name, n, STRESS_TIMER_NAME_LEN);
        name[STRESS_TIMER_NAME_LEN-1] = '\0';
        }
    }

StressTimer::~StressTimer()
    {
    if(next) delete next;
    }

StressTimer* StressTimer::Add(char *n)
    {
    if(!n) return NULL;
    if(strcmp(name, n) == 0) return this;
    if(!next)
        {
        next = new StressTimer(n);
        return next;
        }
    return next->Add(n);
    }

StressTimer* StressTimer::Find(char *n)
    {
    if(!n) return NULL;
    if(strcmp(name, n) == 0) return this;
    if(next) return next->Find(n);
    return NULL;
    }

DWORD StressTimer::Start(void)
    {
    return dwTime = GetTickCount();
    }

DWORD StressTimer::Stop(void)
    {
    DWORD curTime = GetTickCount();
    DWORD t =  curTime - dwTime;
    avg.Insert(t);

    return t;
    }

DWORD StressTimer::GetTime(void)
    {
    DWORD curTime = GetTickCount();
    DWORD t =  curTime - dwTime;

    return t;
    }