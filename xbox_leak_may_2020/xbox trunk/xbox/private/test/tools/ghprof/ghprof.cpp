/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    GhProf.cpp

Abstract:

    The /Gh compiler flag inserts a call to __penter() at the beginning 
    of each function. 

    The __pexit() function is not explicitly called by the /Gh flag. So
    inside of __penter() we modify the return address to jump inside of our
    __pexit, call our cleanup function, then jump back to where it was 
    supposed to go in the first place. 

    WARNING!
        1.  penterAddStats MODIFIES the stack at where the return address 
            should be in relation to the base pointer.
        2.  This means that FPO (Frame-Pointer Omission) must NOT be enabled
        3.  Nothing is protected via semaphores, but here are the resons:
            a.  Blocking threads on our objects could drastically change the 
                normal behaviour of the code (we want to be as minimalistic 
                as possible).
            b.  Blocking the threads could easily intruduce deadlocks because
                of locks in the code we instrument.
            c.  We are on a single processor system so trashing of our 
                internal structures will be difficult.
        4.  Note that __pexit should NEVER be explicitly called, only jumped 
            to and only with the proper setup via our penterAddStats method.

    NOTES:
        1.  The following APIs will not be profiled:
                QueryPerformanceCounter
                GetCurrentThreadId
                DebugPrint
                ExAllocatePool
                ExFreePool
            Or any call from a high IRQ priority.

Author:

    jpoley

Environment:

    XBox

Revision History:

*/


#include <stdio.h>
#include <time.h>

#include "GhProf.h"

namespace GhProfiler {

//
// globals
//
static RetAddrStack addressStack[MAX_ADDR_STACKS];
static HashTable<MethodStats*, 64> penterStats(DestroyMethodStats);

/*****************************************************************************

Object Description:

    MethodStats

    This class keeps track of how many times an API has been called, as well
    as timing information
    
Methods:


Properties:


Notes:

    DestroyMethodStats and PrintStats are non-member helper functions.    

*****************************************************************************/
MethodStats::MethodStats()
    {
    hitCount = 0;
    exitCount = 0;
    failures = 0;
    runningTime.QuadPart = 0;
    penterTime.QuadPart = 0;
    childTime.QuadPart = 0;
    maxTime.QuadPart = 0;
    minTime.QuadPart = ~0;
    }


// Called by the HashTable to clean up the entries
// As per the cleanup function
static void DestroyMethodStats(DWORD key, void *p, void *param)
    {
    if(!p) return;
    MethodStats *v = *(MethodStats**)p;
    delete v;
    }

static void PrintStats(DWORD key, void *p, void *param)
    {
    if(!p) return;

    MethodStats *v = *(MethodStats**)p;
    FILE* f = (FILE*)param;
    fprintf(f, "%08x, %5lu, %5lu, %5lu, %8I64u, %8I64u, %8I64u, %8I64u, %8I64u\n", key, v->hitCount, v->exitCount, v->failures, v->runningTime.QuadPart/v->hitCount, v->maxTime.QuadPart, v->minTime.QuadPart==~0?0:v->minTime.QuadPart, v->penterTime.QuadPart/v->hitCount, v->childTime.QuadPart/v->hitCount);
    }


/*****************************************************************************

Object Description:

    RetAddrStack

    This class contains a stack of return addresses. We need to save this 
    information so we can modify the return value on the stack and then later
    jump to that address.

    This class is used in an array so we can keep thread call stacks seperate.
    
Methods:


Properties:


Notes:

    DestroyMethodStats and PrintStats are non-member helper functions.    

*****************************************************************************/
RetAddrStack::RetAddrStack()
    {
    threadContext = 0;
    length = 0;
    maxLength = 0;
    }

RetAddrStack::~RetAddrStack()
    {
    delete[] stack;
    }

inline void RetAddrStack::Init(void)
    {
    maxLength = 32;
    stack = new PenterCallStack[maxLength];
    }

void RetAddrStack::Grow(void)
    {
    unsigned i;
    PenterCallStack *temp;

    maxLength *= 2;

    temp = new PenterCallStack[maxLength];
    for(i=0; i<length; i++)
        {
        temp[i] = stack[i];
        }
    delete[] stack;
    stack = temp;
    }

unsigned RetAddrStack::Push(void)
    {
    if(length >= maxLength)
        {
        if(maxLength == 0) Init();
        else Grow();
        }

    return length++;
    }

unsigned RetAddrStack::Pop(void)
    {
    if(length == 0)
        {
        return ~0;
        }

    if(length == 1)
        {
        threadContext = 0;
        }

    return --length;
    }

unsigned RetAddrStack::GetAddress(void)
    {
    if(length < 1)
        {
        return 0;
        }

    return stack[length-1].methodAddress;
    }

RetAddrStack* FindThreadAddrStack(void)
    {
    DWORD thread = GetCurrentThreadId();
    RetAddrStack *firstBlank = NULL;

    // scan through the entire list looking for a match
    for(unsigned i=0; i<MAX_ADDR_STACKS; i++)
        {
        if(addressStack[i].threadContext == thread)
            {
            return &addressStack[i];
            }
        if(!firstBlank && addressStack[i].threadContext == 0)
            {
            firstBlank = &addressStack[i];
            }
        }

    if(firstBlank) firstBlank->threadContext = thread;
    return firstBlank;
    }

} // namespace GhProfiler


/*****************************************************************************

Routine Description:

    penterAddStats
    
    This method is called via __penter() which logs the calling API address,
    and modifies the return addresses on the stack to call __pexit().

        stack[0] == the address of the calling API (+ a small offset)
        stack[1] == is where the API was called from (the return address)

Arguments:

    None

Return Value:

    No return value

*****************************************************************************/
void __stdcall penterAddStats(void)
    {
    using namespace GhProfiler;
    unsigned base;
    unsigned index;
    LARGE_INTEGER time1, time2;

    // skip high priority items
    if(KeGetCurrentIrql() > DISPATCH_LEVEL) return;

    __asm mov base, ebp; // grab our base pointer

    base += 40; // +40 moves from our base ptr to __penter's stack ptr
                // 32 from pushad in __penter, 8 from stack offset in this api

    unsigned *stack = (unsigned*)base;
    unsigned callerAddress = stack[0]-5; // -5 for the "call __penter" offset

    // any API that we call cannot be instrumented - if it is we bail
    if(callerAddress == (unsigned)QueryPerformanceCounter) return;
    if(callerAddress == (unsigned)GetCurrentThreadId) return;
    if(callerAddress == (unsigned)DebugPrint) return;
    if(callerAddress == (unsigned)ExAllocatePool) return;
    if(callerAddress == (unsigned)ExFreePool) return;

    QueryPerformanceCounter(&time1);

    // log the API call
    MethodStats* method;
    if(!penterStats.Get(callerAddress, method))
        {
        method = penterStats.Add(callerAddress, new MethodStats());
        }
    ++method->hitCount;

    // modify the return address on the stack to merge in the __pexit() code
    RetAddrStack *addrStack = FindThreadAddrStack();
    if(addrStack)
        {
        index = addrStack->Push();
        addrStack->stack[index].returnAddress = stack[1];
        addrStack->stack[index].methodAddress = callerAddress;
        stack[1] = (unsigned)&_pexit;

        QueryPerformanceCounter(&time2);
        addrStack->stack[index].startTime.QuadPart = time2.QuadPart;
        addrStack->stack[index].penterTime.QuadPart = (unsigned __int64)time2.QuadPart - (unsigned __int64)time1.QuadPart;
        }
    else
        {
        ++method->failures;
        }
    }


/*****************************************************************************

Routine Description:

    penterCleanup
    
    This method is called via __pexit() which logs the calling API end time,
    and returns to the appropriate caller.

Arguments:

    None

Return Value:

    unsigned - returns the original return address of the function this
        value gets passed back to __pexit() so it can restore the program
        flow.

*****************************************************************************/
unsigned __stdcall penterCleanup(void)
    {
    using namespace GhProfiler;
    LARGE_INTEGER curTime, penterTime, time1, time2;

    QueryPerformanceCounter(&time1);

    // find the return address
    unsigned index;
    RetAddrStack *addrStack = FindThreadAddrStack();
    if(!addrStack || (index=addrStack->Pop()) == ~0)
        {
        // We got to penterCleanup, so the stack was modified. But we were
        // unable to get the actual return address, so something very very
        // bad happened, and we dont know where to return to.
        DebugPrint("penterCleanup: ABOUT TO JUMP TO OZ!!!!!\n");
        __asm int 3;
        return 0;
        }

    // update the stats for the method
    MethodStats* method;
    if(penterStats.Get(addrStack->stack[index].methodAddress, method))
        {
        ++method->exitCount;

        curTime.QuadPart = (unsigned __int64)time1.QuadPart - (unsigned __int64)addrStack->stack[index].startTime.QuadPart;
        method->runningTime.QuadPart += (unsigned __int64)curTime.QuadPart;
        if((unsigned __int64)curTime.QuadPart > (unsigned __int64)method->maxTime.QuadPart) method->maxTime.QuadPart = curTime.QuadPart;
        if((unsigned __int64)curTime.QuadPart < (unsigned __int64)method->minTime.QuadPart) method->minTime.QuadPart = curTime.QuadPart;

        MethodStats *parent = NULL;
        penterStats.Get(addrStack->GetAddress(), parent);

        QueryPerformanceCounter(&time2);
        penterTime.QuadPart = (unsigned __int64)time2.QuadPart - (unsigned __int64)time1.QuadPart + (unsigned __int64)addrStack->stack[index].penterTime.QuadPart;
        method->penterTime.QuadPart += penterTime.QuadPart;

        if(parent)
            {
            parent->childTime.QuadPart += (unsigned __int64)penterTime.QuadPart + (unsigned __int64)curTime.QuadPart;
            }
        }

    // __pexit will jump to the 'pexitRetAddr' address 
    return addrStack->stack[index].returnAddress;
    }


/*****************************************************************************

Routine Description:

    penterDumpStats
    
    This method logs out all the stats currently in memory

Arguments:

    None

Return Value:

    No return value

*****************************************************************************/
void __stdcall penterDumpStats(void)
    {
    using namespace GhProfiler;
    char dbuffer[9];
    char tbuffer[9];
    _strdate(dbuffer);
    _strtime(tbuffer);

    LARGE_INTEGER freq;
    freq.QuadPart = 0;
    QueryPerformanceFrequency(&freq);

    FILE *f = fopen("t:\\GhProf.log", "a+");
    if(f)
        {
        fprintf(f, "[%s, %s] *********************************************************\n", dbuffer, tbuffer);
        fprintf(f, "Freq=%I64u\n", (unsigned __int64)freq.QuadPart);
        fprintf(f, " Address, Count,  Exit, Error, Avg Time, Max Time, Min Time,  Gh Time, ChldTime\n");
        penterStats.ForAll(PrintStats, (void*)f);
        fclose(f);
        }
    }

