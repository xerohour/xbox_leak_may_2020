
/*
 * File             :       XNetAPI.c
 * Author           :       Jim Y
 * Revision History
 *      Jun/02/2001   JimY        Created
 *
 * This file contains code for testing the XNetxxxxxxx() APIs
 */


#include <precomp.h>
#include <winsockx.h>
#include "xnetapi.h"

LPVOID lpHeapMemory = NULL;
HANDLE XNetAPIHeapHandle;

#define PASS 0
#define FAIL 1


//
// Functions local to this module
//
#define BREAK_INTO_DEBUGGER __asm int 3
#define WARNFAIL(_apiname) \
        DbgPrint(#_apiname " failed: %d %d\n", err, GetLastError())






// XBox entry point

BOOL WINAPI XNetAPIDllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
    {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    if(fdwReason == DLL_PROCESS_ATTACH)
        {
        XNetAPIHeapHandle = HeapCreate(0, 0, 0);

        if(!XNetAPIHeapHandle)
            {
            DbgPrint("XNetAPI.C: Unable to create heap\n");
            }
        }
    else if(fdwReason == DLL_PROCESS_DETACH)
        {
        if(XNetAPIHeapHandle)
            {
            HeapDestroy(XNetAPIHeapHandle);
            }
        }

    return TRUE;
    }






/*
Xbox:
Routine Description:


Arguments:

    HANDLE hLog - to the logging object
    bool stressmode - true for infinate loop

Return Value:

    DWORD

*/
DWORD RunAll(HANDLE hLog, BOOL stressmode /*=false*/)
    {

    DbgPrint("XNetAPI.C: Start of RunAll\n");

    do {

        } while(stressmode);

    DbgPrint("XNetAPI.C: End of RunAll\n");
    return 0;
    }





/*
XBox:
Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

*/
VOID WINAPI XNetAPIStartTest(HANDLE hLog)
    {

    DoGetEthernetLinkStatus(hLog);
    DoDNSLookup(hLog);
    DoStartup(hLog);
    RunAll(hLog, FALSE);
    DbgPrint("XNetAPI.C: Leaving StartTest()\n");

    }


/*
Xbox:
Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

*/
VOID WINAPI XNetAPIEndTest(void)
    {
    }




//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xnetapi )
#pragma data_seg()

BEGIN_EXPORT_TABLE( xnetapi )
    EXPORT_TABLE_ENTRY( "StartTest", XNetAPIStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XNetAPIEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", XNetAPIDllMain )
END_EXPORT_TABLE( xnetapi )

