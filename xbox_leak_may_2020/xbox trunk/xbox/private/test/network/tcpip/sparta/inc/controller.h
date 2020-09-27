/*
 * File : Controller.h
 * Author : Sandeep Prabhu
 * Revision History
 *      27/4/2000       sandeep           Created
 *
 * The Controller object is teh client side of AutoSrv and exposes the APIs provided by
 * AutoSrv to the upper level clients - COM objects, C++ programs. It is totally transparent
 * to the underlying driver. The clients need to provide it Send and Receive callback funtions
 * along with an I/O context
 */
 
#ifndef __CONTROLLER__H
#define __CONTROLLER__H

#include <winsock2.h>
#include <iphlpapi.h>
#include <tchar.h>
#include <windows.h>
#include "CommandStructs.h"
                  
typedef DWORD (*SEND_HANDLER) (PVOID pvContextHandle, PVOID pvBuffer, ULONG ulLength);

typedef DWORD (*RECEIVE_HANDLER) (PVOID pvContextHandle, PVOID pvBuffer, ULONG ulLength,                                                   
                                  PULONG pulBytesRead, DWORD dwTimeout);   

/**********************************************************************************
 * Debug levels and macros
 **********************************************************************************/

#if (0)
#ifndef DEBUGMSG1
 
#define DEBUGMSG1(level, format, arg) (printf(format, arg))

#endif

#ifndef DEBUGMSG

#define DEBUGMSG(level, format) (printf(format))

#endif
#endif


//
// Debug levels
//
#define DBG_CONTROLLER_VERBOSE  3
#define DBG_CONTROLLER_ERROR    2
#define DBG_CONTROLLER_WARNING  1


//
// Winsock send and receive handlers
//
DWORD WinsockSendHandler(PVOID pvContextHandle, PVOID pvBuffer, ULONG ulLength);
DWORD WinsockRecvHandler(PVOID pvContextHandle, PVOID pvBuffer, ULONG ulLength,                             
                         PULONG pulBytesRead, DWORD dwTimeout);

typedef struct WSCONTEXT {
    SOCKET Socket;
    SOCKADDR_IN Address;
} WSCONTEXT, *PWSCONTEXT;


class CController {
private:
    LONG m_lTID;
    SEND_HANDLER m_pfnSendHandler;
    RECEIVE_HANDLER m_pfnReceiveHandler;
    PVOID m_pvContextHandle;
    INT m_iRexmitCount;
    DWORD m_dwInitRexmitTimeout;
    DWORD m_dwRexmitTimeoutDelta;
    BOOL m_fWinsock;


    DWORD GetUniqueResponseId();
    PVOID SendAndReceive(tGenericStruct *pCommandStruct, DWORD dwLength,                                  
                         PDWORD pdwCommandStatus, PDWORD pdwStatus);
    DWORD Cleanup();

public:
    CController(PVOID pvContextHandle, SEND_HANDLER pfnSendHandler, 
                RECEIVE_HANDLER pfnReceiveHandler);
    
    //
    // Will create a controller which uses Winsock to talk with autosrv
    //
    CController(PCHAR pszSrcIP, PCHAR pszDstIP, USHORT usSrcPort, USHORT usDstPort,                         
                PDWORD pdwStatus);
    ~CController();

    /*********************************************************************
     * Functions exposed by AutoSrv
     *********************************************************************/

    //
    // Miscellaneous commands
    //
    DWORD Reboot();
    DWORD SetPowerState(BOOL fSuspend, ULONG ulTimeout=0);
    DWORD Ping();
    DWORD System(PTCHAR pszCommand);


    //
    // IPHlpAPI functions
    //
    PMIB_IPNETTABLE GetArpCache(PDWORD pdwStatus);
    DWORD FlushArpCache(DWORD dwAdapterIndex);
    DWORD DeleteArpCacheEntry(DWORD dwAdapterIndex, PTCHAR pszIPAddress, 
                              PTCHAR pszMacAddress, DWORD dwType);
    DWORD AddArpCacheEntry(DWORD dwAdapterIndex, PTCHAR pszIPAddress, 
                           PTCHAR pszMacAddress, DWORD dwType);
    DWORD CheckArpCacheEntry(DWORD dwAdapterIndex, PTCHAR pszIPAddress, 
                             PTCHAR pszMacAddress, PBOOL pfPresent);
    PMIB_IPFORWARDTABLE GetRouteTable(PDWORD pdwStatus);
    DWORD DeleteRouteTableEntry(DWORD dwAdapterIndex, PTCHAR pszTargetIP, PTCHAR pszGWIP, 
                                PTCHAR pszMask, DWORD dwMetric, DWORD dwType);
    DWORD AddRouteTableEntry(DWORD dwAdapterIndex, PTCHAR pszTargetIP, PTCHAR pszGWIP, 
                             PTCHAR pszMask, DWORD dwMetric, DWORD dwType);
    DWORD GetNumInterfaces(PDWORD pdwCount);
    DWORD GetInterfaceIndex(PTCHAR pszFriendlyName, PDWORD pdwIndex);
    DWORD GetIndexByIP(PTCHAR pszIPAddress, PDWORD pdwIndex);
    DWORD DeleteIP(ULONG ulNTEContext);
    DWORD AddIP(DWORD dwAdapterIndex, PTCHAR pszIPAddress, PTCHAR pszMask, 
                PULONG pulNTEContext);
    PMACADDR SendArp(PTCHAR pszDestIP, PTCHAR pszSrcIP, PDWORD pdwStatus);

    //
    // Registry commands
    //
    DWORD CreateRegistryKey(HKEY hMainKey, PTCHAR pszSubKey, DWORD dwOptions, REGSAM Sam);
    DWORD DeleteRegistryKey(HKEY hMainKey, PTCHAR pszSubKey);
    DWORD DeleteRegistryValue(HKEY hMainKey, PTCHAR pszSubKey, PTCHAR pszValueName);
    DWORD DeleteRegistryValueByIp(HKEY hMainKey, PTCHAR pszIPAddress, PTCHAR pszValueName);
    DWORD SetRegistryValue(HKEY hMainKey, PTCHAR pszSubKey, PTCHAR pszValueName, DWORD dwType,
                           PBYTE pbValue, DWORD dwLength);
    tResponse_GetRegistryValue* GetRegistryValue(HKEY hMainKey, PTCHAR pszSubKey, 
                                                 PTCHAR pszValueName, PDWORD pdwStatus);
    DWORD SetRegistryValueByIp(HKEY hMainKey, PTCHAR pszIPAddress, PTCHAR pszValueName, 
                               DWORD dwType, PBYTE pbValue, DWORD dwLength);
    tResponse_GetRegistryValue* GetRegistryValueByIp(HKEY hMainKey, PTCHAR pszIPAddress,                                              
                                                     PTCHAR pszValueName, PDWORD pdwStatus);


    //
    // Session commands
    //
    DWORD Socket(INT iType, PDWORD pdwSessionId);
    DWORD Bind(DWORD dwSessionId, PTCHAR pszSrcIP, USHORT usSrcPort);
    DWORD CreateAndBind(INT iType, PTCHAR pszSrcIP, USHORT usSrcPort, PDWORD pdwSessionId);
    DWORD SetSocketOption(DWORD dwSessionId, INT iLevel, INT iOption, PCHAR pchValue, INT iSize);
    DWORD CreateAndConnect(PDWORD pdwTID, INT iType, PTCHAR pszSrcIP, USHORT usSrcPort,                   
                           PTCHAR pszDestIP, USHORT usDestPort, PDWORD pdwSessionId);
    DWORD Connect(PDWORD pdwTID, DWORD dwSessionId, PTCHAR pszDestIP, USHORT usDestPort);
    DWORD ConnectEx(PDWORD pdwTID, DWORD dwSessionId, PTCHAR pszDestIP, 
                    USHORT usDestPort, INT iBufferSize, PCHAR pchBuffer=NULL);
    DWORD CreateAndAccept(PDWORD pdwTID, INT iType, PTCHAR pszSrcIP, USHORT usSrcPort, 
                          INT iListenQ, PDWORD pdwSessionId);
    DWORD Accept(PDWORD pdwTID, DWORD dwSessionId, INT iListenQ);
    DWORD Receive(PDWORD pdwTID, DWORD dwSessionId, DWORD dwIterations, DWORD dwDelay,                        
                  INT iBufferSize);
    DWORD Send(PDWORD pdwTID, DWORD dwSessionId, DWORD dwIterations, DWORD dwDelay,                     
               INT iBufferSize, PCHAR pchBuffer=NULL);
    DWORD OverlappedSend(PDWORD pdwTID, DWORD dwSessionId, INT iNumBuffers, INT iBufferSize);
    DWORD SendAndDisconnect(PDWORD pdwTID, DWORD dwSessionId, INT iBufferSize, PCHAR pchBuffer=NULL);
    DWORD GetSessionStats(DWORD dwSessionId, PDWORD pdwBytesSent, PDWORD pdwBytesRcvd);
    //DWORD GetAsyncCompletionStatus(DWORD dwSessionId, DWORD dwAsyncTID, PDWORD pdwAsyncStatus);
    DWORD GetCurrentStatus(DWORD dwSessionId, DWORD dwAsyncTID, PDWORD pdwAsyncStatus);
    DWORD Stop(DWORD dwSessionId, DWORD dwAsyncTID, PDWORD pdwAsyncStatus);
    DWORD Shutdown(PDWORD pdwTID, DWORD dwSessionId);
    DWORD Close(PDWORD pdwTID, DWORD dwSessionId);
    DWORD Reset(PDWORD pdwTID, DWORD dwSessionId);
};
          
#endif

