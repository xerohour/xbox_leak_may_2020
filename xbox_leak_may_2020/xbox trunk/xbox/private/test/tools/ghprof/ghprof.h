/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    GhProf.h

Abstract:

    The /Gh compiler flag inserts a call to __penter() at the beginning 
    of each function. 

    The __pexit() function is not explicitly called by the /Gh flag. So
    inside of __penter() we modify the return address to jump inside of our
    __pexit, call our cleanup function, then jump back to where it was 
    supposed to go in the first place. Note that __pexit should NEVER be
    called, only jumped to and only with the proper setup via our __penter
    method.

    WARNING!
        1.  penterAddStats MODIFIES the stack at where the return address 
            should be in relation to the base pointer.
        2.  This means that FPO (Frame-Pointer Omission) must NOT be enabled

Author:

    jpoley

Environment:

    XBox

Revision History:

*/

#ifndef _GHPROF_H_
#define _GHPROF_H_

#include <xtl.h>

#ifndef DISPATCH_LEVEL
#define DISPATCH_LEVEL 2
#endif

extern "C"
    {
    ULONG DebugPrint(char* Format, ...);
    UCHAR KeGetCurrentIrql();

    // from wdm.h
    void* ExAllocatePool(size_t NumberOfBytes);
    void ExFreePool(void* P);
    }



//
// publics
//
void __stdcall penterDumpStats(void);

//
// privates
//
void __stdcall penterAddStats(void);
unsigned __stdcall penterCleanup(void);
extern "C" void __cdecl _pexit(void);



/*****************************************************************************

Routine Description:

    new & delete
    
    We override new and delete to use Pools instead of the heap because we
    dont want to be doing "pageable" heap operations in "nonpageable" OS calls

*****************************************************************************/
static void* __cdecl operator new(size_t memSize)
    {
    return ExAllocatePool(memSize);
    }
static void __cdecl operator delete(void* p)
    {
    if(!p) return;
    ExFreePool(p);
    }
static void __cdecl operator delete[](void* p)
    {
    if(!p) return;
    ExFreePool(p);
    }


//
// internal structures
//
namespace GhProfiler {
/*
class PerfTimer
    {
    public:
        LARGE_INTEGER time;

    public:
        PerfTimer()         {}
        PerfTimer(int a)    { time.QuadPart = a; }
        void GetTime(void)  { QueryPerformanceCounter(&time); }
        void GetSpan(void)  { LARGE_INTEGER temp; QueryPerformanceCounter(&temp); time.QuadPart = temp.QuadPart - time.QuadPart; }
        operator unsigned __int64() { return (unsigned __int64)time.QuadPart; }
    };
*/
class MethodStats
    {
    public:
        DWORD hitCount;
        DWORD exitCount;
        DWORD failures;
        LARGE_INTEGER runningTime;
        LARGE_INTEGER maxTime;
        LARGE_INTEGER minTime;
        LARGE_INTEGER penterTime;
        LARGE_INTEGER childTime;

    public:
        MethodStats();
    };

static void DestroyMethodStats(DWORD key, void *p, void *param);
static void PrintStats(DWORD key, void *p, void *param);

struct PenterCallStack
    {
    unsigned returnAddress;
    unsigned methodAddress;
    LARGE_INTEGER startTime;
    LARGE_INTEGER penterTime;
    LARGE_INTEGER childTime;
    };

#define MAX_ADDR_STACKS 25
class RetAddrStack
    {
    public:
        DWORD threadContext;
        unsigned length;
        PenterCallStack *stack;

    public:
        RetAddrStack();
        ~RetAddrStack();
        inline void Init(void);

    public:
        unsigned Push(void);
        unsigned Pop(void);
        unsigned GetAddress(void);

    private:
        unsigned maxLength;
        void Grow(void);
    };
RetAddrStack* FindThreadAddrStack(void);




/*****************************************************************************

Class Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
typedef void (*HASHOBJ_FUNC)(DWORD, void*, void*);
template <class T> class HashEntry
    {
    public:
        DWORD key;
        T obj;
        HashEntry <T> *next;

    public:
        HashEntry();
        HashEntry(DWORD k, T o);
        ~HashEntry();
    };

/*****************************************************************************

Class Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE> class HashTable
    {
    public:
        HashTable();
        HashTable(HASHOBJ_FUNC c) { clean = c; }
        ~HashTable();

        HASHOBJ_FUNC clean;
        size_t m_size;

    public:
        HashEntry<T> *table[SIZE];
    
        BOOL Get(DWORD key, T& out);
        T Add(DWORD key, T o);
        BOOL ForAll(HASHOBJ_FUNC fn, void *param);
        void SetCleanupFunct(HASHOBJ_FUNC c) { clean = c; }
    };

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T>
HashEntry<T>::HashEntry()
    {
    clean = NULL;
    key = NULL;
    next = NULL;
    }


/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T>
HashEntry<T>::HashEntry(DWORD k, T o)
    {
    key = k;
    obj = o;
    next = NULL;
    }

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T>
HashEntry<T>::~HashEntry()
    {
    if(next) delete next;
    }

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
HashTable<T, SIZE>::HashTable()
    {
    m_size = SIZE;
    clean = NULL;
    for(size_t i=0; i<SIZE; i++)
        {
        table[i] = NULL;
        }
    }


/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
HashTable<T, SIZE>::~HashTable()
    {
    if(clean) ForAll(clean, NULL);
    for(size_t i=0; i<SIZE; i++)
        {
        if(table[i]) delete table[i];
        }
    }

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
BOOL HashTable<T, SIZE>::Get(DWORD key, T& out)
    {
    size_t i = key % SIZE;

    if(!table[i]) return FALSE;

    for(HashEntry<T> *h = table[i]; h; h=h->next)
        {
        if(h->key == key)
            {
            out = h->obj;
            return TRUE;
            }
        }

    return FALSE;
    }

/*****************************************************************************

Routine Description:

    Add the key and the associated object. Add the new HashEntry at the end 
    of the list to give precedence to items added first.

Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
T HashTable<T, SIZE>::Add(DWORD key, T o)
    {
    size_t i = key % SIZE;
    
    if(!table[i])
        {
        table[i] = new HashEntry<T>(key, o);
        return o;
        }
    else
        {
        HashEntry<T> *h = table[i];
        do
            {
            // replace entry with the same key
            if(h->key == key)
                {
                if(clean) clean(h->key, &(h->obj), NULL);
                h->obj = o;
                return o;
                }

            // otherwise add it to the end
            if(!h->next)
                {
                h->next = new HashEntry<T>(key, o);
                return o;
                }

            h=h->next;
            } while(h);
        }

    return NULL;
    }

/*****************************************************************************

Routine Description:



Arguments:



Return Value:



Notes:



*****************************************************************************/
template <class T, size_t SIZE>
BOOL HashTable<T, SIZE>::ForAll(HASHOBJ_FUNC fn, void *param)
    {
    for(size_t i=0; i<SIZE; i++)
        {
        if(table[i]) 
            {
            for(HashEntry<T> *h = table[i]; h; h=h->next)
                {
                fn(h->key, &(h->obj), param);
                }
            }
        }
    return TRUE;
    }

} // namespace GhProfiler


#endif // _GHPROF_H_
