/*
 * File             :       GetEthernetLinkStatus.c
 * Author           :       Jim Y
 * Revision History
 *      Jun/02/2001   JimY        Created
 *
 * This file contains code for testing the XNetGetEthernetLinkStatus() API
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

void DoGetEthernetLinkStatus(HANDLE hLog) {

    DWORD dwLinkStatus = 0;
    INT LinkStatusTestcase, result = 0;
    INT nSleepDelay = 2000;


    //
    // Read info from .INI file
    //
    LinkStatusTestcase = GetProfileInt(L"XNetAPI", L"LinkStatusTestcase", 0);
    DbgPrint("GetEthernetLinkStatus.C: Read from INI file that LinkStatusTestcase = %d \n", LinkStatusTestcase);


    //
    // If LinkStatusTestcase is 1 then run Basic set of link tests
    //

    if (LinkStatusTestcase == 1)  {

       //
       // Wall switch testcase 
       //
       xStartVariation(hLog, "100mbit switch test");
       DbgPrint("GetEthernetLinkStatus.C: Will now test for a 100MBit Switch Full Duplex (your wall switch) , do a (G)o in your debugger.\n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_100MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 100mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_FULL_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be FULL DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "100mbit switch test");
       else
           xLog(hLog, XLL_PASS, "100mbit switch test");



       //
       // 10mbit Hub testcase 
       //
       xStartVariation(hLog, "10mbit hub test");
       DbgPrint("GetEthernetLinkStatus.C: Now, connect the XBox to a 10mbit hub HALF Duplex (A 10mbit Hub) then do a (G)o in the debugger \n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_10MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 10mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_HALF_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be HALF DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "10mbit hub test");
       else
           xLog(hLog, XLL_PASS, "10mbit hub test");


       //
       // 100mbit Hub testcase 
       //
       xStartVariation(hLog, "100mbit hub test");
       DbgPrint("GetEthernetLinkStatus.C: Now, connect the XBox to a 100mbit hub HALF Duplex (A 100mbit Hub) then do a (G)o in the debugger \n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_100MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 100mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_HALF_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be HALF DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "100mbit hub test");
       else
           xLog(hLog, XLL_PASS, "100mbit hub test");




       //
       // Crossover Cable 
       //
       xStartVariation(hLog, "Crossover cable (100mbit FULL DUPLEX) test");
       DbgPrint("GetEthernetLinkStatus.C: Now, connect the XBox using a CROSSOVER cable (CROSSOVER CABLE) then do a (G)o in the debugger \n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_100MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 100mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_FULL_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be FULL DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "Crossover cable (100mbit FULL DUPLEX) test");
       else
           xLog(hLog, XLL_PASS, "Crossover cable (100mbit FULL DUPLEX) test");



       //
       // Disconnected Cable 
       //
       xStartVariation(hLog, "Disconnected cable test");
       DbgPrint("GetEthernetLinkStatus.C: Now, DISCONNECT the XBox network cable then do a (G)o in the debugger \n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 != (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to be ACTIVE [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "Disconnected cable test");
       else
           xLog(hLog, XLL_PASS, "Disconnected cable test");


       xEndVariation( hLog );


       //
       // Write to the INI file so that LinkStatusTestcase = 2 for the next boot.
       //

       DbgPrint("GetEthernetLinkStatus.C: Writing to the TESTINI.INI file to set LinkStatusTestcase = %d \n", 2);
       WriteProfileString(L"XNetAPI", L"LinkStatusTestcase", L"2");
       Sleep(nSleepDelay);
       DbgPrint("GetEthernetLinkStatus.C: Now, DISCONNECT the XBox network cable then do a .REBOOT in the debugger \n");
       BREAK_INTO_DEBUGGER

    }



    //
    // If LinkStatusTestcase is 2 then run the disconnected cable test
    //

    else if (LinkStatusTestcase == 2)  {

       //
       // Disconnected Cable 
       //
       xStartVariation(hLog, "PreBoot Disconnected cable test");
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 != (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to be ACTIVE [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "PreBoot Disconnected cable test");
       else
           xLog(hLog, XLL_PASS, "PreBoot Disconnected cable test");


       xEndVariation( hLog );

    }


    //
    // If LinkStatusTestcase is 3 then run exhaustive switch configured link tests
    //

    else if (LinkStatusTestcase == 3)  {

       xStartVariation(hLog, "XNetGetEthernetLinkStatus() EXHAUSTIVE switch-based tests");

       //
       // Switch port is set to AutoNegotiate
       //
       xStartVariation(hLog, "AutoNegotiate Test");
       DbgPrint("GetEthernetLinkStatus.C: Will now test an AutoNegotiated port, do a (G)o in your debugger.\n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_100MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 100mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_FULL_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be FULL DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "AutoNegotiate Test");
       else
           xLog(hLog, XLL_PASS, "AutoNegotiate Test");


       //
       // Switch port is set to 100 ForceFullDuplex
       //
       xStartVariation(hLog, "100mbit ForceFullDuplex");
       DbgPrint("GetEthernetLinkStatus.C: Will now test an 100mbit ForceFullDuplex port, do a (G)o in your debugger.\n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_100MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 100mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_FULL_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be FULL DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "100mbit ForceFullDuplex");
       else
           xLog(hLog, XLL_PASS, "100mbit ForceFullDuplex");


       //
       // Switch port is set to 100 ForceHalfDuplex
       //
       xStartVariation(hLog, "100mbit ForceHalfDuplex");
       DbgPrint("GetEthernetLinkStatus.C: Will now test an 100mbit ForceHalfDuplex port, do a (G)o in your debugger.\n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_100MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 100mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_HALF_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be HALF DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "100mbit ForceHalfDuplex");
       else
           xLog(hLog, XLL_PASS, "100mbit ForceHalfDuplex");


       //
       // Switch port is set to 10 ForceFullDuplex
       //
       xStartVariation(hLog, "10mbit ForceFullDuplex");
       DbgPrint("GetEthernetLinkStatus.C: Will now test an 10mbit ForceFullDuplex port, do a (G)o in your debugger.\n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_10MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 10mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_FULL_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be FULL DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "10mbit ForceFullDuplex");
       else
           xLog(hLog, XLL_PASS, "10mbit ForceFullDuplex");


       //
       // Switch port is set to 10 ForceHalfDuplex
       //
       xStartVariation(hLog, "10mbit ForceHalfDuplex");
       DbgPrint("GetEthernetLinkStatus.C: Will now test an 10mbit ForceHalfDuplex port, do a (G)o in your debugger.\n");
       BREAK_INTO_DEBUGGER
       Sleep(nSleepDelay);
       dwLinkStatus = XNetGetEthernetLinkStatus();
       DbgPrint("GetEthernetLinkStatus.C: After dwLinkStatus = %d \n", dwLinkStatus);

       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_ACTIVE) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT active [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_10MBPS) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be 10mbit [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }
       if (0 == (dwLinkStatus & XNET_ETHERNET_LINK_HALF_DUPLEX) )   {
           DbgPrint("GetEthernetLinkStatus.C: ERROR - LinkStatus appears to NOT be HALF DUPLEX [Status = %d]! \n", dwLinkStatus);
           result = FAIL;
       }

       if (result != PASS)
           xLog(hLog, XLL_FAIL, "10mbit ForceHalfDuplex");
       else
           xLog(hLog, XLL_PASS, "10mbit ForceHalfDuplex");



       xEndVariation( hLog );

    }


    //
    // If LinkStatusTestcase is not 1 or 2 or 3 then skip running the 
    // XNetGetEthernetLinkStatus() API tests.
    //

    else  {
       xStartVariation(hLog, "XNetGetEthernetLinkStatus() API tests");
       DbgPrint("GetEthernetLinkStatus.C: Warning - we have chosen to skip the XNetGetEthernetLinkStatus() \n");
       xLog(hLog, XLL_INFO, "XNetGetEthernetLinkStatus() API tests were skipped");
       xEndVariation( hLog );
    }


    DbgPrint("GetEthernetLinkStatus.C: Test has finished running!\n");


}

