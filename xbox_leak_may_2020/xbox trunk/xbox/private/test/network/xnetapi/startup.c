/*
 * File             :       Startup.c
 * Author           :       Jim Y
 * Revision History
 *      Jun/02/2001   JimY        Created
 *
 * This file contains code for testing the XNetStartup() API
 */


#include <precomp.h>

#define PASS 0
#define FAIL 1


//
// Functions local to this module
//
#define BREAK_INTO_DEBUGGER __asm int 3
#define WARNFAIL(_apiname) \
        DbgPrint(#_apiname " failed: %d %d\n", err, GetLastError())




/*
 * Main 
 *
 *
 */

void DoStartup(HANDLE hLog) {

    DWORD dwLinkStatus = 0;
    INT nResult = 0;
    INT nSleepDelay = 2000;


    //
    // Verify the network is connected before running tests
    //

    Sleep(nSleepDelay);
    dwLinkStatus = XNetGetEthernetLinkStatus();
    DbgPrint("Startup.C: After dwLinkStatus = %d \n", dwLinkStatus);

    xStartVariation(hLog, "API Verify net link status");
    if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
        DbgPrint("Startup.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
        xLog(hLog, XLL_FAIL, "API Verify net link status");
        goto Stop;
    }
    xLog(hLog, XLL_PASS, "API Verify net link status");


    //
    // Testcase: API parameter is NULL
    //

    xStartVariation(hLog, "API parameter is NULL");
    DbgPrint("Startup.C: API parameter is NULL\n");

    nResult = XNetStartup(NULL);
    DbgPrint("Startup.C: After nResult = %d \n", nResult);

    if (nResult != PASS)
        xLog(hLog, XLL_FAIL, "API parameter is NULL");
    else
        xLog(hLog, XLL_PASS, "API parameter is NULL");





    xEndVariation( hLog );


Stop:

    DbgPrint("Startup.C: Test has finished running!\n");


}

