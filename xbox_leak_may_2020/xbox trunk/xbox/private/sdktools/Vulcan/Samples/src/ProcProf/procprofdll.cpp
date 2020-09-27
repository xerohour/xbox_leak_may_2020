/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: ProcProfDll.cpp
*
* File Comments:
*
*
***********************************************************************/
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "stdlib.h"

#include "procprof.h"

// Routine to read Pentium time stamp counter
_inline _declspec( naked ) __int64 RDTSC()
{
   _asm RDTSC
   _asm ret
}

#pragma pack (1)

// We maintain a "shadow stack" for each thread so we don't have to unwind the stack.
struct SHADOWSTACKENTRY
{
   // The first 10 bytes of a shadow stack entry are code.
   // We patch the a return address of each function call with
   // the address of the associated shadow stack entry.
   // This way, we always know where we are returning to.
   BYTE push;               // 0x68 or push dword on x86;
   SHADOWSTACKENTRY *psse;  // This is a pointer to this shadow stack entry
   BYTE jump;               // 0xE9 or jump dword on x86
   DWORD ibProcExit;        // Offset to our ProcExitProf routine

   void *pvRASave;          // The real return address assocated with this entry
   void *pvTarget;          // The procedure which was called
   __int64 timeEnter;       // The RDTSC time we entered this routine
};

// A single structure which represents the shadow stack for each thread
struct THREADDATA
{
   THREADDATA *pNext;               // Free List next pointer
   SHADOWSTACKENTRY *pTop;          // A pointer to the top entry in this stack
   SHADOWSTACKENTRY rgEntries[200]; // Array of entries on this stack

   void *operator new( size_t cb );
   void operator delete( void *pv );
   static THREADDATA *s_pFreeList;
};

// Global Variables
int v_iTls;                  // The thread local storage slot we used to store our shadow stack
FUNDATA *v_rgFunData = NULL; // A pointer to the FUNDATA we put in the instrumented binary
int v_cProc;                 // The number of FUNDATA entries in the instrumented binary

// An silly allocator that reduces our interfernce with the running program
void *_fastcall Allocate(int cb)
{
    static BYTE *v_pbAllocNext = NULL;  // A pointer to the next allocation
    static BYTE *v_pbAllocMax;

    if (NULL == v_pbAllocNext ||
        v_pbAllocMax - v_pbAllocNext < cb)
    {
        // We preserve the last error because some programs use it, and VirtualAlloc touches it
        DWORD dwError = GetLastError();
       
        v_pbAllocNext = (BYTE*)VirtualAlloc( NULL, 64*1024, MEM_COMMIT, PAGE_READWRITE );
        v_pbAllocMax = v_pbAllocNext + 64*1024;

        SetLastError(dwError);
    }

    void *pvRet = v_pbAllocNext;
    v_pbAllocNext += ((cb + 3) & ~3); //Keep DWORD aligned

    return pvRet;
}

THREADDATA *THREADDATA::s_pFreeList;
void *THREADDATA::operator new( size_t cb )
{
   // Only 1 thread can be destroyed or created at a time
   if (s_pFreeList)
   {
      THREADDATA *pRet = s_pFreeList;
      s_pFreeList = s_pFreeList->pNext;
      return (void *)pRet;
   }
   else
   {
      return Allocate( cb );
   }
}

void THREADDATA::operator delete( void *pv )
{
   // Only 1 thread can be destroyed or created at a time
   THREADDATA *pData = (THREADDATA *)pv;
   pData->pNext = s_pFreeList;
   s_pFreeList = pData;
}

#pragma warning( disable: 4200 )

// A linked list of FUNSTACK describes each different stack in the program,
// and how much time was spent there.

struct FUNSTACK
{
    __int64 time;       // This will be the total time spent with this stack as the stack (timeLeave - timeEntry)
    __int64 timeEx;     // This will be the exclusive time spent with this stack on the stack (time - time from all children)
    DWORD c;            // The number of times this routine got called with this exact stack
    DWORD dwKey;        // This is a hash key we will use to identify this stack
    FUNSTACK *pNext;    // This is a pointer to the next stack in this hash
    DWORD cdwAddr;      // This is a the number of addresses on this stack
    DWORD rgdwAddr[0];  // This is an array of addresses of function that were on the stack
};

// We have a little hash table here... 4k entries... a linked list to change

const int cFunStack = 1024*4;
FUNSTACK **rgFunStackHash = (FUNSTACK **)Allocate( cFunStack * sizeof(FUNSTACK *) );
#pragma warning(disable:4311 4312)

void AddFunStackTime( SHADOWSTACKENTRY *pTop, SHADOWSTACKENTRY *pBase, __int64 time )
{
   // This routine is called when a function returns to add the time spent
   // inside it to the assocated FUNSTACK

   // Create the hash key based on the addresses on the stack

   DWORD dwKey = 0;
   for (SHADOWSTACKENTRY *p = pBase; p < pTop; p++)
   {
      dwKey ^= (DWORD)p->pvTarget;
      dwKey  = _rotl( dwKey, 1 );
   }

   // Compute the hash bucket this stack will be in

   DWORD hash = dwKey % cFunStack;

   // And the height of the stack

   DWORD c = pTop - pBase;

   // We search for the entry in that hash bucket for this key

   for (FUNSTACK *pStack = rgFunStackHash[hash]; pStack; pStack = pStack->pNext)
   {
      if (pStack->dwKey == dwKey &&
          pStack->cdwAddr == c)
      {
         // Is this really the right one?

         p = pBase;
         int i = 0;
         while (p < pTop)
         {
            if ((DWORD)p->pvTarget != pStack->rgdwAddr[i])
            {
               // This is not the right one
               break;
            }

            p++; i++;
         }

         if (p == pTop)
         {
            // Yes, this is the right entry

            break;
         }
      }
   }

   if (pStack)
   {
      // We have seen this stack before... add the time and count

      pStack->time += time;
      pStack->c++;
   }
   else
   {
      // We have not seen this stack before.... make a new entry
      // Note that there is an small chance in a multithreaded app
      // that this would happen more than once for a stack.

      FUNSTACK *pStack = (FUNSTACK *)Allocate( c * sizeof(DWORD) + sizeof(FUNSTACK) );
      pStack->time = time;
      pStack->dwKey = dwKey;
      pStack->cdwAddr = c;
      pStack->c = 1;

      int i = 0;
      for (p = pBase; p < pTop; p++)
      {
          pStack->rgdwAddr[i] = (DWORD)p->pvTarget;
          i++;
      }

      // We must be thread safe as we add this entry to the list
      // so we don't lose stacks.

      do
      {
         pStack->pNext = rgFunStackHash[hash];
      }
      while (pStack->pNext != 
#ifdef InterlockedCompareExchangePointer
               InterlockedCompareExchangePointer( (void **)&rgFunStackHash[hash],
                                                  pStack,
                                                  pStack->pNext ));
#else //InterlockedCompareExchangePointer
               InterlockedCompareExchange( (void **)&rgFunStackHash[hash],
                                           pStack,
                                           pStack->pNext ));
#endif //InterlockedCompareExchangePointer

   }
}

int _cdecl CmpPFunStack( const void *ppv1, const void *ppv2)
{
    // This routine allows us to sort the stack before we write them out

    FUNSTACK *p1 = *(FUNSTACK **)ppv1;
    FUNSTACK *p2 = *(FUNSTACK **)ppv2;

    DWORD c = min(p1->cdwAddr, p2->cdwAddr);

    for (DWORD i=0; i<c; i++)
    {
        if (p2->rgdwAddr[i] != p1->rgdwAddr[i])
        {
            return p1->rgdwAddr[i]- p2->rgdwAddr[i];
        }
    }

    return p1->cdwAddr - p2->cdwAddr;
}

int _cdecl CmpFunData( const void * pvKey, const void *pv )
{
    // This routine help us to find the name of a routine from an address

    FUNDATA *pfd = (FUNDATA *)pv;
    DWORD addr = (DWORD)pvKey;

    if (addr >=  pfd->addr &&
        addr <   (pfd + 1)->addr)
    {
       return 0;
    }
    else
    {
       return addr - pfd->addr;
    }
}

const char *NameFromAddr( DWORD addr )
{
    // This routine lets us find the name of a routine from an address

    static char szTemp[256];
    FUNDATA *pfd = (FUNDATA *)bsearch( (void *)(addr), v_rgFunData, v_cProc, sizeof(FUNDATA), CmpFunData );
    if (pfd && pfd->szFun)
    {
        return pfd->szFun;
    }
    else
    {
        sprintf(szTemp, "0x%08X", addr );
        return szTemp;
    }
}

void DumpData()
{
   // This routine is called when the program exits to write out
   // the profile information.

   FILE *pf = fopen( "procprof.log", "w" );
   int cStack = 0;
   DWORD cMaxDepth = 0;
   for (int i=0; i<cFunStack; i++)
   {
      for (FUNSTACK *pStack = rgFunStackHash[i]; pStack; pStack = pStack->pNext)
      {
         cStack++;

         if (pStack->cdwAddr > cMaxDepth)
         {
             cMaxDepth = pStack->cdwAddr;
         }
      }
   }

   FUNSTACK **rgpStack = new FUNSTACK*[ cStack ];
   int iStack = 0;
   for (i=0; i<cFunStack; i++)
   {
      for (FUNSTACK *pStack = rgFunStackHash[i]; pStack; pStack = pStack->pNext)
      {
         rgpStack[iStack++] = pStack;
      }
   }

   qsort( rgpStack, cStack, sizeof(FUNSTACK *), CmpPFunStack );

   // Compute exclusive time

   FUNSTACK **rgParents = new FUNSTACK*[cMaxDepth];
   rgParents[0] = NULL; // hasn't returned yet

   for (iStack=0; iStack<cStack; iStack++)
   {
      FUNSTACK *pStack = rgpStack[iStack];
      pStack->timeEx = pStack->time;

      // Subtract me from my parents

      if (pStack->cdwAddr > 1)
      {
         FUNSTACK *pParent = rgParents[pStack->cdwAddr-2];
         if (pParent)
         {
             pParent->timeEx -= pStack->time;
         }
      }

      rgParents[pStack->cdwAddr - 1] = pStack;
   }

   fprintf(pf, "Inclusive\tExclusive\tCount\tStack\n");
   for (iStack=0; iStack<cStack; iStack++)
   {
      FUNSTACK *pStack = rgpStack[iStack];
      fprintf(pf, "%I64d\t%I64d\t%d", pStack->time, pStack->timeEx, pStack->c );

      for (DWORD j=0; j<pStack->cdwAddr; j++)
      {
         // fprintf(pf, "\t0x%08X", pStack->rgdwAddr[j] );

         fprintf(pf, "\t\"%s\"", NameFromAddr(pStack->rgdwAddr[j]) );
      }

      fprintf(pf, "\n");
   }

   fclose(pf);
}


extern "C" _declspec (dllexport) void _cdecl InitProcProf( FUNDATA *rgFunData, int cProc )
{
   // This is the initialization routine.  Note the we don't handle instrumenting
   // more than one program per process.

   if (v_rgFunData == NULL)
   {
      v_rgFunData = rgFunData;
      v_cProc = cProc;
   }
}

void * _fastcall ProcExitProf( SHADOWSTACKENTRY *psseLeave )
{
   // This routine is called as each function returns

   __int64 time = RDTSC();

   THREADDATA *ptd = (THREADDATA *)TlsGetValue( v_iTls );

   for (SHADOWSTACKENTRY *psse = ptd->pTop - 1; psse >= psseLeave; psse--)
   {
       __int64 delta = time - psse->timeEnter;
       AddFunStackTime( psse + 1, ptd->rgEntries, delta );
   }

   ptd->pTop = psseLeave;

   return psseLeave->pvRASave;
}

_declspec(naked) void _stdcall ProcExitProfAsm()
{
   // This assembly routine is called as each function returns
   // Note that we must preserve volitle registers and the flags

                             // 12 param (psseLeave)
   _asm push ecx             // 8
   _asm push edx             // 4
   _asm pushfd               // 0
   _asm xchg [esp + 12], eax // Save old eax where the return address will go
   _asm mov ecx, eax         // _fastcall ProcExitProf uses ecx for param

   _asm call ProcExitProf    // eax contains the return value

   _asm popfd
   _asm pop edx
   _asm pop ecx
   _asm xchg [esp], eax      // swap eax and our new target

   _asm ret
}


extern "C"
BOOL WINAPI DllMain(HINSTANCE, DWORD dwReason, LPVOID)
{
   switch (dwReason)
   {
      case DLL_PROCESS_ATTACH:
         v_iTls = TlsAlloc();
         // Fall through to thread attach which isn't called for the first thread

      case DLL_THREAD_ATTACH:
      {
          THREADDATA *ptd = new THREADDATA;
          TlsSetValue( v_iTls, ptd );

          // Build the code for each entry in the shadow stack (this never changes after this point)
          ptd->pTop = ptd->rgEntries;
          for ( SHADOWSTACKENTRY *psse = ptd->rgEntries; psse < ptd->rgEntries + 200; psse++)
          {
              psse->push = 0x68;
              psse->psse = psse;
              psse->jump = 0xE9;
              psse->ibProcExit = (BYTE *)&ProcExitProfAsm - (BYTE *)&psse->pvRASave;
          }
          break;
      }

      case DLL_THREAD_DETACH:
      {
          // Free data assocated with this thread
          THREADDATA *ptd = (THREADDATA *)TlsGetValue( v_iTls );
          delete ptd;
          break;
      }

      case DLL_PROCESS_DETACH:
      {
          // Time to write out the profile information
          DumpData();
          break;
      }

   }

   return TRUE;
}

void _stdcall ProcProfEnter( DWORD **ppvRA, DWORD * pvTarget )
{
   // This routine is called as we enter each procedure

   __int64 time = RDTSC();

   // pvRA points at the RA for the function we just called

   THREADDATA *ptd = (THREADDATA *)TlsGetValue( v_iTls );
   SHADOWSTACKENTRY *psse = ptd->pTop;
   ptd->pTop++;

   // Fill in the rest of the SHADOWSTACKENTRY structure

   psse->timeEnter = time;
   psse->pvRASave = *ppvRA;
   psse->pvTarget = pvTarget;

   // Patch the stack with our exit routine

   *ppvRA = (DWORD *)psse;
}

extern "C" _declspec (dllexport) _declspec(naked) void _stdcall ProcProf()
{
   // Assembly routine called a the beginning of each instrumented routine
   // Volatile register and flags are live

                     // 20 caller ra
                     // 16 ra
   _asm push eax     // 12
   _asm push ecx     // 8
   _asm push edx     // 4
   _asm pushfd       // 0

   _asm lea eax, [esp + 20]  // compute ppvRA
   _asm push [esp + 16]      // push pvTarget
   _asm push eax             // push ppvRA
   _asm call ProcProfEnter

   _asm popfd
   _asm pop edx
   _asm pop ecx
   _asm pop eax
   _asm ret
}
