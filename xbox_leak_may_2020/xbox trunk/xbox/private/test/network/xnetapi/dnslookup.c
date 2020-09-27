/*
 * File             :       DNSLookup.c
 * Author           :       Jim Y
 * Revision History
 *      Jun/02/2001   JimY        Created
 *
 * This file contains code for testing the XNetGetEthernetLinkStatus() API
 * 
 * 
 *     INT     XNetDnsLookup(const char * pszHost, WSAEVENT hEvent, XNDNS ** ppxndns);
 *     INT     XNetDnsRelease(XNDNS * pxndns);
 * 
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

void DoDNSLookup(HANDLE hLog) {
    
    INT     result = 0;
    INT    iResult = 0;
    DWORD dwResult = 0;
    HANDLE  hEvent = NULL;
    XNDNS * pxndns = NULL;
    XNADDR  XnAddr;

    iResult = XNetStartup(NULL);

    while ( 0 == (dwResult = XNetGetTitleXnAddr(&XnAddr)) ) {
       Sleep(1000);	
    }


    //*****************************************************************************
    // Valid Lookup 
    //

    xStartVariation(hLog, "Valid DNSLookup test");
    DbgPrint("DoDNSLookup.C: Valid DNSLookup test \n");
    result = FAIL;

    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ResetEvent(hEvent);

    iResult = XNetDnsLookup("jimy1.redmond.corp.microsoft.com", hEvent, &pxndns);
    if (iResult != 0)  
       DbgPrint("DoDNSLookup.C: iResult failed with a result of %d = %d \n", iResult);
    else 
       DbgPrint("DoDNSLookup.C: iResult passed with a result of %d = %d \n", iResult);
       
    //
    // Check if WaitForSingleObject times out or not
    //
    if (WaitForSingleObject(hEvent, 10000) != WAIT_TIMEOUT)  {

       DbgPrint("DoDNSLookup.C: xNetDNSLookup() did NOT timeout.  hEvent = %d \n", hEvent);

       //
       // Verify pxndns->iStatus and pxndns->cina
       //
       DbgPrint("DoDNSLookup.C: iStatus = %d  cina = %d \n", pxndns->iStatus, pxndns->cina);
       if ((pxndns->iStatus == 0) && (pxndns->cina > 0))  {

          DbgPrint("DoDNSLookup.C: Host was found as expected. Setting result to PASS.\n");
          result = PASS;
          }

       else  {

          DbgPrint("DoDNSLookup.C: Host was not found but should have been found. \n");
          }

       }

    //
    // WaitForSingleObject timed out
    //
    else  {

       DbgPrint("DoDNSLookup.C: xNetDNSLookup() WaitForSingleObject timed out.  hEvent = %d \n", hEvent);
       BREAK_INTO_DEBUGGER
    }  // WaitForSingleObject



    //
    // Release handles that we used
    //
    XNetDnsRelease(pxndns);
    CloseHandle(hEvent);


    if (result != PASS)
        xLog(hLog, XLL_FAIL, "Valid DNSLookup test");
    else
        xLog(hLog, XLL_PASS, "Valid DNSLookup test");


    xEndVariation( hLog );





    //*****************************************************************************
    // Invalid Lookup 
    //

    xStartVariation(hLog, "Invalid DNSLookup test");
    DbgPrint("DoDNSLookup.C: Invalid DNSLookup test \n");
    result = FAIL;

    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ResetEvent(hEvent);

    iResult = XNetDnsLookup("jimybogus.redmond.corp.microsoft.com", hEvent, &pxndns);
    if (iResult != 0)  
       DbgPrint("DoDNSLookup.C: iResult failed with a result of %d = %d \n", iResult);
    else 
       DbgPrint("DoDNSLookup.C: iResult passed with a result of %d = %d \n", iResult);


    //
    // Check if WaitForSingleObject times out or not
    //
    if (WaitForSingleObject(hEvent, 10000) != WAIT_TIMEOUT)  {

       DbgPrint("DoDNSLookup.C: xNetDNSLookup() did NOT timeout.  hEvent = %d \n", hEvent);

       //
       // Verify pxndns->iStatus and pxndns->cina
       //
       DbgPrint("DoDNSLookup.C: iStatus = %d  cina = %d \n", pxndns->iStatus, pxndns->cina);
       if ((pxndns->iStatus == WSAHOST_NOT_FOUND) && (pxndns->cina == 0))  {

          DbgPrint("DoDNSLookup.C: Host was not found, as expected.  Setting result to PASS.\n");
          result = PASS;
          }

       else
             {

          DbgPrint("DoDNSLookup.C: Result should have been WSAHOST_NOT_FOUND but it was not. \n");
          }

       }

    //
    // WaitForSingleObject timed out
    //
    else  {

       DbgPrint("DoDNSLookup.C: xNetDNSLookup() timed out.  hEvent = %d \n", hEvent);
       BREAK_INTO_DEBUGGER
    }  // WaitForSingleObject



    //
    // Release handles that we used
    //
    XNetDnsRelease(pxndns);
    CloseHandle(hEvent);


    if (result != PASS)
        xLog(hLog, XLL_FAIL, "Invalid DNSLookup test");
    else
        xLog(hLog, XLL_PASS, "Invalid DNSLookup test");


    xEndVariation( hLog );



    //*****************************************************************************
    // Test iStatus result 
    //

    xStartVariation(hLog, "iStatus is WSAEINPROGRESS test");
    DbgPrint("DoDNSLookup.C: iStatus is WSAEINPROGRESS test \n");
    result = FAIL;

    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ResetEvent(hEvent);

    iResult = XNetDnsLookup("jimy1.redmond.corp.microsoft.com", hEvent, &pxndns);
    if (iResult != 0)  
       DbgPrint("DoDNSLookup.C: iResult failed with a result of %d = %d \n", iResult);
    else 
       DbgPrint("DoDNSLookup.C: iResult passed with a result of %d = %d \n", iResult);

    //
    // Immediately verify pxndns->iStatus
    //
    while (pxndns->iStatus == WSAEINPROGRESS)  {
       DbgPrint("DoDNSLookup.C: iStatus = %d (WSAEINPROGRESS). Setting result to PASS \n", pxndns->iStatus);
       result = PASS;
       }

    //
    // Verify pxndns->iStatus and pxndns->cina
    //
    DbgPrint("DoDNSLookup.C: iStatus = %d  cina = %d \n", pxndns->iStatus, pxndns->cina);
    if ((pxndns->iStatus == 0) && (pxndns->cina > 0))  {

       DbgPrint("DoDNSLookup.C: Host was found as expected. Setting result to PASS.\n");
       }

    else  {
       DbgPrint("DoDNSLookup.C: Host was not found but should have been found. \n");
       }


    //
    // Release handles that we used
    //
    XNetDnsRelease(pxndns);
    CloseHandle(hEvent);


    if (result != PASS)
        xLog(hLog, XLL_FAIL, "iStatus is WSAEINPROGRESS test");
    else
        xLog(hLog, XLL_PASS, "iStatus is WSAEINPROGRESS test");


    xEndVariation( hLog );










    DbgPrint("DoDNSLookup.C: Test has finished running!\n");


}

