//
// `VmStress' - stress test for virtual space manager 
// Copyright (c) 1991  Microsoft Corporation
// 
// Module Name:
// 
//     vmstress.c
// 
// Abstract:
// 
//     Test stress program for virtual memory.
// 
// Author:
// 
//     Lou Perazzoli (LouP) 26-Jul-91
// 
// Revision History:
// 
//     S. Somasegar (Somase) Jul 18, 1991 
// 
//         - Ported to a Win32 application
// 
//     G. Bailey (GBailey) Oct 7, 1995 
//  
//         - added logging for HCT purposes
// 
//     P. Lunia (PankajL) Dec 7, 1995 
// 
//         - fixed access violation related to not
//           existing when cannot
//         
//         -  allocated memory for startva0 in main.
// 
//     S.Calinoiu (SilviuC) Apr 4, 1997
// 
//         - added code to dump information to aid debug stress failures
// 
//         - added some useful comments and error checking code
//
//         - fixed bug 80814 that caused AV under special stress conditions
//
//
//     J. Sirimarco ( Digital Equipment Corporation ), 18-May-1998
//
//         - Ported to Win64 

//


extern "C" {
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
}

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#pragma warning(disable:4201)
#include <xtl.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <xtestlib.h>

//
// MY_RAND
//

#define MY_RAND()       ((random()<<16) + (random()<<1) + (random()&1))


//
// Start of definition additions made by Gbailey
// to improve test logging for better HCT support
//

#define MAX_ALLOCS      5
#define START           1
#define FAIL            0

int fCreateLog = FALSE;

HANDLE VmStressLogHandle;

void myPrintf (LPSTR lpStr, ...);
void GetPageFileInfo( char *szRet, int GetPageFile );
void LogDateTime( char *szTmp );
void PrintStatus( BOOL Type );
void PrintMemoryStat( char *szTmp );
void ValidateMemoryRegion (PVOID Address, SIZE_T Size);

struct _PAGE_FILE 
{
    CHAR  szPageFilePath[35];
    CHAR  szDriveLetter[5];
    DWORD iTotalSpace;
    DWORD iFreeSpace;
    struct _PAGE_FILE * Next;
} *stPageFileInfo;

//
// end of Gbailey definition additions
//

//
// INIT_ARG struct
//
// Va: start of virtual space region to be tested
// Size: size of the virtual space region to be tested
//

//
// J. Sirimarco ( DEC ), 14-May-1998
//
// Changed the _INIT_ARG structure for 64-bit NT porting.  The Size 
// variable was changed from type DWORD to type SIZE_T.  
//
// Original: 
//
//   typedef struct _INIT_ARG {
//        PDWORD Va;
//        SIZE_T Size;
//   } INIT_ARG, *PINITARG;
//

typedef struct _INIT_ARG {
    PDWORD Va;
    SIZE_T Size;
} INIT_ARG, *PINITARG;


//
// GlobalDebugInformation
//
// Global variable used to keep the information that should be dumped
// when the test fails.
//

//
// J. Sirimarco ( DEC ), 14-May-1998
//
// Changed the DebugInformation structure for 64-bit NT porting.  
// The data type of RegionSize was changed from DWORD to SIZE_T. 
// The data type of CurrentSize was changed from DWORD to SIZE_T.   
//
// Original: 
//
//   struct DebugInformation
//   {
//       PVOID   RegionStart [2];
//
//       DWORD   RegionSize [2]; 
//
//       PVOID   NextAddressAccessed [2];
//       BOOL    NextAccessIsWrite [2];
//       int     CurrentIValue [2];
//       int     CurrentJValue [2];
//
//       DWORD   CurrentIteration;
//       PVOID   CurrentStart;
//
//       DWORD   CurrentSize; 
//
//       DWORD   WaitResult;
//
//       HANDLE TestThread [2];
//       BOOL ThreadActive [2];
//
//   } GlobalDebugInformation;
//

//

struct DebugInformation
{
    PVOID   RegionStart [2];

    SIZE_T  RegionSize [2]; 

    PVOID   NextAddressAccessed [2];
    BOOL    NextAccessIsWrite [2];
    int     CurrentIValue [2];
    int     CurrentJValue [2];

    DWORD   CurrentIteration;
    PVOID   CurrentStart;

    SIZE_T  CurrentSize; 

    DWORD   WaitResult;

    HANDLE TestThread [2];
    BOOL ThreadActive [2];

} GlobalDebugInformation;


//
// GenerateRandomExceptions
//
// Used for debugging purposes. If it is set to true the program will
// generate randomly access violations. It is set to true if the
// program is launched with `-generate-random-exceptions'.
//

BOOL GenerateRandomExceptions = TRUE;


//
// FailSecondThreadCreation
//
// Used for debugging purposes. If it is set to true the creation of the second
// thread will fail. This is used to reveal a bug that was present in the 
// test (bug #80814). Set to true if `-fail-second-thread-creation' is specified
// in the command line.
//

BOOL FailSecondThreadCreation = FALSE;


//
// VerboseOutput
//
// If false the program will be completely quiet. It will not print anything
// into kernel debugger, except if a fatal failure occurs. Set to true by
// `-verbose' command line option.
//

BOOL VerboseOutput = FALSE;


//
// ProgramVersion
//

char ProgramVersion[] = "VmStress 0.01 - Apr 30, 1997";

//
// DebugPrint
//

void DebugPrint (char *format, ...)
{
    va_list params;
    //char szTemp [2048];

    va_start (params, format);
    //vsprintf (szTemp, format, params);
    //OutputDebugStringA(szTemp);
    // printf ("VmDebug: %s", szTemp);
    va_end (params);
}


ULONG Seed = GetTickCount();

int
random(
    void
    )
{
    return RtlRandom( &Seed );
}


//
// DumpDebugInformation
//

void DumpDebugInformation ()
{
    DebugPrint ("-------------------------------------------------------\n");

    DebugPrint ("vmstress: current iteration: %u\n", 
        GlobalDebugInformation.CurrentIteration);
    DebugPrint ("vmstress: address: %0p, size: %0p\n",
        GlobalDebugInformation.CurrentStart,
        GlobalDebugInformation.CurrentSize);
    DebugPrint ("vmstress: threads: %0p, %0p\n",
        GlobalDebugInformation.TestThread[0],
        GlobalDebugInformation.TestThread[1]);
    DebugPrint ("vmstress: wait result: %08X\n", 
        GlobalDebugInformation.WaitResult);
    DebugPrint ("\n");


    DebugPrint ("Thread 0: address: %0p, size: %0p\n",
        GlobalDebugInformation.RegionStart[0],
        GlobalDebugInformation.RegionSize[0]);
    DebugPrint ("Thread 0: active: %s\n",
        GlobalDebugInformation.ThreadActive[0] ? "true" : "false");
    DebugPrint ("          next access: %0p, operation: %s\n",
        GlobalDebugInformation.NextAddressAccessed[0],
        GlobalDebugInformation.NextAccessIsWrite[0] ? "write" : "read");
    DebugPrint ("          iteration: i: %d, j: %d\n",
        GlobalDebugInformation.CurrentIValue[0],
        GlobalDebugInformation.CurrentJValue[0]);
    DebugPrint ("\n");


    DebugPrint ("Thread 1: address: %0p, size: %0p\n",
        GlobalDebugInformation.RegionStart[1],
        GlobalDebugInformation.RegionSize[1]);
    DebugPrint ("Thread 1: active: %s\n",
        GlobalDebugInformation.ThreadActive[1] ? "true" : "false");
    DebugPrint ("          next access: %0p, operation: %s\n",
        GlobalDebugInformation.NextAddressAccessed[1],
        GlobalDebugInformation.NextAccessIsWrite[1] ? "write" : "read");
    DebugPrint ("          iteration: i: %d, j: %d\n",
        GlobalDebugInformation.CurrentIValue[1],
        GlobalDebugInformation.CurrentJValue[1]);
    DebugPrint ("\n");

    DebugPrint ("----------------------------------------- VmStress ---\n");

    ValidateMemoryRegion (
        GlobalDebugInformation.RegionStart[0],
        GlobalDebugInformation.RegionSize[0]);

    DebugPrint ("----------------------------------------- VmStress ---\n");
}


//
// BreakIntoDebugger ()
//

void BreakIntoDebugger ()
{
    DumpDebugInformation ();
    DebugBreak ();
}


//
// ExceptionFilter
//
// For any exception we will print all debug information and then we will
// break into debugger.
//

DWORD ExceptionFilter ()
{
    DebugPrint ("vmstress: exception caught ...\n");
    DumpDebugInformation ();
    //DebugBreak ();

    return EXCEPTION_EXECUTE_HANDLER;
}


//
// VmRandom1 (parameter)
//
// The function of the first thread that tests the virtual memory manager.
// `parameter' is actually a pointer to an `INIT_ARG' structure specifying
// the virtual space region to be tested.
//

VOID
VmRandom1 (
    LPVOID ThreadParameter
    )
{

    PINITARG InitialArg;
    DWORD Seed = 8373833;

    SIZE_T  size; 

    PDWORD startva0;
    PDWORD Va;
    DWORD i,j;

    //
    // Break into debugger if we get a null thread parameter. This should be
    // a pointer to a structure and it shouldn't be null.
    //

    if (ThreadParameter == NULL)
      {
        DebugPrint ("Thread 0: thread parameter: %0p (null!)\n", ThreadParameter);
        BreakIntoDebugger ();
      }

    //
    // Write debug information.
    //

    GlobalDebugInformation.RegionStart [0] = ((PINITARG)ThreadParameter)->Va;
    GlobalDebugInformation.RegionSize [0] = ((PINITARG)ThreadParameter)->Size;
    GlobalDebugInformation.ThreadActive [0] = TRUE;

    //
    // Capture virtual space region characteristics
    //

    InitialArg = (PINITARG)ThreadParameter;

    startva0 = InitialArg->Va;
    size = InitialArg->Size;

    //
    // Perform `10 * 2500' memory operations (r/w).
    //

    for (j = 1; j < 10; j++) 
      {
        for (i = 1 ; i < 2500; i++) 
          {
             Seed = MY_RAND();
             Va = startva0 + (Seed % (size / sizeof(DWORD)));

             // if (i % 1000 == 0)
             //     printf ("Sample address: %08X\n", Va);

             GlobalDebugInformation.NextAddressAccessed[0] = Va;
             GlobalDebugInformation.NextAccessIsWrite[0] = FALSE;
             GlobalDebugInformation.CurrentIValue[0] = i;
             GlobalDebugInformation.CurrentJValue[0] = j;
             
             __try 
               {
                 if (*Va == ((PtrToUlong(Va) + 1))) 
                   {
                     GlobalDebugInformation.NextAccessIsWrite[0] = FALSE;
                     
                     //
                     // Force an access violation randomly. This is
                     // used for debugging of the test.
                     //

  
                     if (GenerateRandomExceptions && ((random () & 0xFFF) == 0xABC))
                         *((BYTE *)0) = (BYTE)0xAB;
                 

                     *Va = PtrToUlong(Va);
                   } 
                 else 
                   {
                     if (*Va != PtrToUlong(Va)) 
                       {
                         DebugPrint ("Thread 0: wrong value at address %0p\n", Va);
                         DebugPrint ("Thread 0: expected: %0p, found: %08X\n", 
                                     Va, *Va);
                         BreakIntoDebugger ();

                         if( fCreateLog )
                             myPrintf("Bad random value in cell %0p was %lx\n", 
                                      Va, *Va);
                       }
                   }
               }
             __except (ExceptionFilter ())
               {
               }
          }

        //
        // Sleep 150 msecs after every 2500 iterations
        //

        Sleep (150);
      }

    GlobalDebugInformation.ThreadActive [0] = FALSE;
}


//
// VmRandom2 (parameter)
//
// The function of the second thread that tests the virtual memory manager.
// `parameter' is actually a pointer to an 'INIT_ARG' structure specifying
// the virtual space region to be tested.
//

VOID
VmRandom2 (
    LPVOID ThreadParameter
    )
{

    PINITARG InitialArg;
    DWORD Seed = 8373839;

    SIZE_T size;

    PDWORD startva0;
    PDWORD Va;
    DWORD i,j;


    //
    // Break into debugger if we get a null thread parameter. This should be
    // a pointer to a structure and it shouldn't be null.
    //

    if (ThreadParameter == NULL)
      {
        DebugPrint ("Thread 1: thread parameter: %0p (null!)\n", ThreadParameter);
        BreakIntoDebugger ();
      }

    //
    // Write debug information.
    //

    GlobalDebugInformation.RegionStart [1] = ((PINITARG)ThreadParameter)->Va;
    GlobalDebugInformation.RegionSize [1] = ((PINITARG)ThreadParameter)->Size;
    GlobalDebugInformation.ThreadActive [1] = TRUE;

    //
    // Capture virtual space region characteristics
    //

    InitialArg = (PINITARG)ThreadParameter;

    startva0 = InitialArg->Va;
    size = InitialArg->Size;


    //
    // Perform `10 * 2500' memory operations (r/w).
    //

    for (j = 1; j < 10; j++) 
      {
        for (i = 1 ; i < 2500; i++) 
          {
             Seed = MY_RAND();
             Va = startva0 + (Seed % (size / sizeof(DWORD)));

             GlobalDebugInformation.NextAddressAccessed[1] = Va;
             GlobalDebugInformation.NextAccessIsWrite[1] = FALSE;
             GlobalDebugInformation.CurrentIValue[1] = i;
             GlobalDebugInformation.CurrentJValue[1] = j;

             __try
               {
//

                 if (*Va == ((PtrToUlong(Va) + 1))) 
                   {
                     GlobalDebugInformation.NextAccessIsWrite[1] = FALSE;
                     
                     //
                     // Force an access violation randomly. This is
                     // used for debugging of the test.
                     //
                     
                     if (GenerateRandomExceptions && ((random () & 0xFFF) == 0xABC))
                         *((BYTE *)0) = (BYTE)0xAB;
                 
                     *Va = PtrToUlong(Va);
                   } 
                 else 
                   {
                     if (*Va != PtrToUlong(Va) ) 
                       {
                         DebugPrint ("Thread 1: wrong value at address %0p\n", Va);
                         DebugPrint ("Thread 1: expected: %0p, found: %08X\n", 
                                     Va, *Va);
                         BreakIntoDebugger ();

                         if( fCreateLog )
                             myPrintf("Bad random value in cell %0p was %lx\n", 
                                      Va, *Va);
                       }
                   }
               }
             __except (ExceptionFilter ())
               {
               }
          }

        //
        // Sleep 150 msecs after every 2500 iterations
        //
        
        Sleep (150);
      }
        
    GlobalDebugInformation.ThreadActive [1] = FALSE;
}


//
// main
//

int  __cdecl main_function (
    int argc,
    char *argv[],
    char *envp[]
    )
{
    HANDLE Objects[2];
    MEMORYSTATUS MemStatus = {0};
    INIT_ARG InitialArg;
    PDWORD Va;
    PDWORD EndVa;

    SIZE_T size;

    LPVOID startva0;
    BOOL   bstatus;
    DWORD ThreadId1, ThreadId2;
    DWORD count = 0;
    DWORD VallocCnt = 1;
    DWORD dError;
//    DWORD CurrentIteration = 0;           // Not referenced.  
    DWORD WaitResult;

    if (argc == 2 && strcmp (argv[1], "-version") == 0)
      {
        myPrintf ("VmStress - stress program for virtual memory manager   \n");
        myPrintf ("Copyright (c) Microsoft Corporation, 1997              \n");
        myPrintf ("                                                       \n");
        myPrintf ("Author:  Lou Perazzoli                                 \n");
        myPrintf ("Owner:   Silviu Calinoiu                               \n");
        myPrintf ("Version: 0.0.1                                         \n");
        myPrintf ("Updated: Jun 05, 1997                                  \n");

        return 1;
      }

    if (argc > 1) 
      {
        if ( *(argv[1]+1) == 'h' || *(argv[1]+1) == 'H')
            fCreateLog = TRUE;
      }

    if (argc == 2 && strcmp (argv[1], "-generate-random-exceptions") == 0)
      {
        GenerateRandomExceptions = TRUE;
        VerboseOutput = TRUE;
      }

    if (argc == 2 && strcmp (argv[1], "-fail-second-thread-creation") == 0)
      {
        FailSecondThreadCreation = TRUE;
        VerboseOutput = TRUE;
      }

    if (argc == 2 && strcmp (argv[1], "-verbose") == 0)
      {
        VerboseOutput = TRUE;
      }

    myPrintf("Starting Virtual Memory Stress Test\n");

    if( fCreateLog )
        PrintStatus( START );

    GlobalDebugInformation.CurrentIteration = 0;
    GlobalDebugInformation.CurrentStart = 0;
    GlobalDebugInformation.CurrentSize = 0;
    GlobalDebugInformation.TestThread[0] = 0;
    GlobalDebugInformation.TestThread[1] = 0;
    GlobalDebugInformation.WaitResult = 0;

    GlobalDebugInformation.RegionStart[0] = 0;
    GlobalDebugInformation.RegionSize[0] = 0;
    GlobalDebugInformation.NextAddressAccessed[0] = 0;
    GlobalDebugInformation.NextAccessIsWrite[0] = ( BOOL ) 0;

    GlobalDebugInformation.CurrentIValue[0] = 0;
    GlobalDebugInformation.CurrentJValue[0] = 0;

    GlobalDebugInformation.RegionStart[1] = 0;
    GlobalDebugInformation.RegionSize[1] = 0;
    GlobalDebugInformation.NextAddressAccessed[1] = 0;
    GlobalDebugInformation.NextAccessIsWrite[1] = 0;
    GlobalDebugInformation.CurrentIValue[1] = 0;
    GlobalDebugInformation.CurrentJValue[1] = 0;

    GlobalDebugInformation.ThreadActive [0] = FALSE;
    GlobalDebugInformation.ThreadActive [1] = FALSE;

    for (;;) 
      {
        GlobalDebugInformation.CurrentIteration++;

        //GlobalMemoryStatus(&MemStatus);
        
        size = MemStatus.dwAvailPhys + (4096*10);
        startva0 = NULL;

        //
        // Create a region of private memory based on the number of
        // available pages on this system.
        //

        //GlobalMemoryStatus(&MemStatus);

        size = MemStatus.dwAvailPhys;
        if (size == 0) 
          {
            size = 4096;
        }
        
        if (size > 64000) 
          {

            if ( size > 0x180000 ) // 1024 * 1024 * 1.5
                size = ( SIZE_T ) 0x180000;
            else
                size -= 4*4096;
          }

        startva0 = VirtualAlloc(
            NULL,
            size,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE);

        GlobalDebugInformation.CurrentStart = startva0;
        GlobalDebugInformation.CurrentSize =  size;

        if (startva0 == NULL) 
          {
            dError = GetLastError();

            DebugPrint ("vmstress: allocating %0p failed (error: %u), "
                        "even if %0p available\n",
                         size, dError, MemStatus.dwAvailPhys);

            myPrintf ("#%d)Error(%d) in virtual allocation of size %ldMB when "
                      "available phys. memory is %ldMB \n",
                      VallocCnt, dError, size / (1024 * 1024),
                      MemStatus.dwAvailPhys / (1024 * 1024));

            if(fCreateLog) 
              { // If being run with HCT System Stress
                myPrintf ("(%d)Error(%d) in virtual allocation of size %ldMB "
                          "when available phys. memory is %ldMB \n",
                          VallocCnt,
                          dError,
                          size/ (1024*1024),
                          MemStatus.dwAvailPhys/ (1024*1024));

                // commented out code for VallocCnt checking to avoid NULL cast 
                // and de-reference
                // of startva0 later. (PankajL)

                if( VallocCnt == MAX_ALLOCS ) 
                  {
                    DebugPrint ("vmstress: exiting ...\n");
                    PrintStatus(FAIL);
                    return dError;
                  }
                else 
                  {
                    VallocCnt++;
                    continue;
                  }
              }
            else
              {
                DebugPrint ("vmstress: exiting ...\n");
                return dError;
              }
          }

        if (VerboseOutput)
          {
            DebugPrint ("vmstress: region: %0p, size: %0p\n", startva0, size);
          }

        myPrintf("Created region address: %0p, size: %0p\n", startva0, size);

        if( fCreateLog && VallocCnt > 1 ) 
          {
            VallocCnt = 1;
          }

        InitialArg.Va = (PULONG)startva0;
        InitialArg.Size = size;

        //
        // Set all memory to known values (not zeroes).
        //

        myPrintf("Initializing Memory\n");

        EndVa = (PDWORD)startva0 + (size/sizeof(DWORD));

        Va = (PULONG)startva0;

        while (Va < EndVa) {
            *Va = PtrToUlong(Va) + 1;
            Va += 1;
        }

        //
        // Validate the memory region just to make sure we are starting
        // with correct data.
        //

        ValidateMemoryRegion (InitialArg.Va, InitialArg.Size);

        //
        // Create the two worker threads
        //

        Objects[0] = CreateThread(
            NULL,
            0L,
            (LPTHREAD_START_ROUTINE)VmRandom1,
            (LPVOID)&InitialArg,
            0,
            &ThreadId1);

        //assert (Objects[0]);

        if (FailSecondThreadCreation)
          {
            Objects[1] = NULL;
          }
        else
          {
            Objects[1] = CreateThread(
                NULL,
                0L,
                (LPTHREAD_START_ROUTINE)VmRandom2,
                (LPVOID)&InitialArg,
                0,
                &ThreadId2);

            //assert (Objects[1]);
          }

        //
        // We will not print any message about thread creation failure because
        // under stress this might happen often and we will spend all our time
        // printing messages into debugger.
        //

        GlobalDebugInformation.TestThread[0] = Objects[0];
        GlobalDebugInformation.TestThread[1] = Objects[1];

        if (Objects[0] && Objects[1])
          {
            if (VerboseOutput)
                DebugPrint ("vmstress: two threads created\n");
        
            WaitResult = WaitForMultipleObjects (
                2,
                Objects,
                TRUE,
                INFINITE );
          }
        else if(Objects [0])
          {
            if (VerboseOutput)
                DebugPrint ("vmstress: thread 0 created\n");
            WaitResult = WaitForSingleObject (Objects[0], INFINITE);
          }
        else if(Objects [1])
          {
            if (VerboseOutput)
                DebugPrint ("vmstress: thread 1 created\n");
            WaitResult = WaitForSingleObject (Objects[1], INFINITE);
          }
        else
          {
            WaitResult = 0;
          }

        //
        // We will not break into debugger if the wait operation fails
        // because this is might indeed happen under stress conditions.
        //

        GlobalDebugInformation.WaitResult = WaitResult;
        
        if (WaitResult == WAIT_FAILED)
            if (VerboseOutput)
                DebugPrint ("vmstress: wait operation failed\n");

        //
        // Validate the memory region after both threads have finished.
        //

        ValidateMemoryRegion (InitialArg.Va, InitialArg.Size);

        count += 1;
        myPrintf("Stress test pass number %ld complete\n",count);

        CloseHandle (Objects[0]);
        CloseHandle (Objects[1]);

        if (GlobalDebugInformation.ThreadActive[0] 
            || GlobalDebugInformation.ThreadActive[1])
          {
            DebugPrint ("vmstress: test bug - freeing memory still used\n");
            BreakIntoDebugger ();
          }

        bstatus = VirtualFree (
            startva0,
            0,               // free the whole chunk
            MEM_RELEASE);

        if (! bstatus) 
          {
            DebugPrint ("vmstress: error %u when freeing memory at address %0p\n",
                GetLastError(), startva0);

            myPrintf ("Error in freeing virtual memory \n");
            dError = GetLastError();
            if(fCreateLog) 
              { // If being run with HCT System Stress
                myPrintf("Error(%d) in freeing virtual memory of size "
                         "%0p\nVMStress test result = FAIL\n", 
                         dError, size);
              }

            DebugPrint ("vmstress: exiting ...\n");
            return dError;
          }

        //
        // Mark memory region as freed.
        //

        GlobalDebugInformation.CurrentStart = ( PVOID ) ( ( INT_PTR ) 0xFFFFFFFF );
        
        //
        // Sleep 5 secs after each stress cycle.
        //

        Sleep (5000);

        if ( !XTestLibIsStressTesting() && count > 10 ) {
            break;
        }
      }
    
    argc;
    argv;
    envp;               // to shut the compiler up

    return 0;
}


void PrintStatus( BOOL Type )
{

    CHAR szBuf[1024];
    CHAR szTmp[512];

    memset(szTmp, '\0',sizeof(szTmp) );
    memset(szBuf, '\0',sizeof(szBuf) );


    if( Type == START )
        sprintf(szBuf,"\n***** Virtual Memory Test Start *****\n");

    if(Type == FAIL)
        sprintf(szBuf,
                "\n*****************\nVMStress test result = FAIL\nMemory Values at Failure Below\n*****************\n");

    LogDateTime( ( char * ) &szTmp );
    strcat(szBuf,szTmp);
    memset(szTmp, '\0',sizeof(szTmp) );

    PrintMemoryStat( ( char * )&szTmp );
    strcat(szBuf,szTmp);
    memset(szTmp, '\0',sizeof(szTmp) );

    // write report to log file
    myPrintf(szBuf);

}

//
// PrintMemoryStat will call GlobalMemoryStatus and format string with
// all relevant memory information.  Only called when -h parm is used.
//
void
PrintMemoryStat(
    char *szTmp
    )
{

   MEMORYSTATUS MemStatus = {0};
   char szMemoryStatusStr[512];
   char szbuf[128];


   sprintf(szMemoryStatusStr,"\n\t*************Memory Information in MB**************\n");
   strcat (szMemoryStatusStr,"\tPhysical\tVirtual\t\tPageFile\n");
   strcat (szMemoryStatusStr,"\tTotal\tAvail.\tTotal\tAvail.\tTotal\tAvail.\n");
   strcat (szMemoryStatusStr,"\t=======\t=======\t=======\t=======\t=======\t=======\n");

   GlobalMemoryStatus(&MemStatus);
   sprintf(szbuf,"\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n",
                 MemStatus.dwTotalPhys/ (1024*1024),
                 MemStatus.dwAvailPhys/ (1024*1024),
                 MemStatus.dwTotalVirtual/ (1024*1024),
                 MemStatus.dwAvailVirtual/ (1024*1024),
                 MemStatus.dwTotalPageFile/ (1024*1024),
                 MemStatus.dwAvailPageFile/ (1024*1024) );
   strcat(szMemoryStatusStr,szbuf);

   strcpy(szTmp,szMemoryStatusStr);

}

void
LogDateTime
   (
   char *szTmp
   )
{
    SYSTEMTIME   SystemTime;

    GetSystemTime (&SystemTime);
    sprintf(
        szTmp,
        "Date and Time: %lu-%lu-%lu at %lu:%02lu \n",
        SystemTime.wMonth,
        SystemTime.wDay,
        SystemTime.wYear,
        SystemTime.wHour,
        SystemTime.wMinute
        );
}

void myPrintf (LPSTR lpStr,
               ...)
{
    va_list marker;
    //char    szBuffer[512];
    //FILE *  fpLogFile;


    va_start (marker, lpStr);
    //vsprintf (szBuffer, lpStr, marker);
    va_end(marker);
    //OutputDebugStringA( szBuffer );

    //fpLogFile = fopen("c:\\vmstress.log","a");
    //fprintf(fpLogFile, szBuffer);
    //fclose(fpLogFile);
}

//
// Function:
//
//     ValidateMemoryRegion
//
// Description:
//
//     This function traverses a memory region and
//     makes sure that all values stored are address or
//     address + 1. It dumps all regions that are not right.
//

void
ValidateMemoryRegion (

    PVOID Address,
    SIZE_T Size)
{
    BOOL FoundProblem = FALSE;
    PDWORD Current = (PDWORD) Address;
    PDWORD End = Current + Size/sizeof(DWORD);

    for ( ; Current < End; Current += 0x1000)

        if ((*Current != PtrToUlong( Current ) ) && 
            (*Current != (PtrToUlong(Current) + 1)))
          {
            DebugPrint ("Invalid contents: %0p\n", Current);
            FoundProblem = TRUE;
          }

    if (FoundProblem == FALSE)
        myPrintf ("Memory region check: ok\n");
}


VOID
WINAPI
VmStressStartTest(
    HANDLE hLog
    )

/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    VmStressLogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/

{
    MEMORYSTATUS stat;
    GlobalMemoryStatus( &stat );

    if(stat.dwTotalPhys >= 100*1024*1024)
        {
        KdPrint(( "VMSTRESS: StartTest\n" ));
        VmStressLogHandle = hLog;
        main_function( 0, NULL, NULL );
        }
    else
        {
        KdPrint(( "VMSTRESS: Exiting - not enough memory\n" ));
        }
}


VOID
WINAPI
VmStressEndTest(
    VOID
    )

/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/

{
    KdPrint(( "VMSTRESS: EndTest\n" ));
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( vmstress )
#pragma data_seg()

BEGIN_EXPORT_TABLE( vmstress )
    EXPORT_TABLE_ENTRY( "StartTest", VmStressStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", VmStressEndTest )
END_EXPORT_TABLE( vmstress )
