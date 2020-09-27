/*==========================================================================
 *
 *  Copyright Microsoft Corporation.  All Rights Reserved.
 *
========================================================================== */

#include "xonp.h"
#include "xonver.h"
#include <cryptkeys.h>
#include <malloc.h>
#include <xboxp.h>

DefineTag(AuthVerbose,TAG_ENABLE);
DefineTag(AuthWarn,TAG_ENABLE);

#define TEMP_SEND_REQUESTS_SEQUENTIALLY 1

#define AP_REQUEST_OVERHEAD_ABOVE_TICKET 280

#define XONLINE_AS_EXCHANGE_BUFFER_SIZE 1460
#define XONLINE_TGS_EXCHANGE_BUFFER_SIZE 1460

//
// All possible states for different logon related tasks.
//
#define LOGON_TASK_INITIAL                  0
#define LOGON_XNETWAIT_IN_PROGRESS          1
#define LOGON_START_CACHE_LOOKUP            2
#define LOGON_CACHE_RETRIEVE_IN_PROGRESS    3
#define LOGON_START_CACHE_MISS              4
#define LOGON_DNS_IN_PROGRESS               5
#define LOGON_CONNECT_IN_PROGRESS           6
#define LOGON_MISC_IN_PROGRESS              7
#define LOGON_RECV_IN_PROGRESS              8
#define LOGON_MACS_IN_PROGRESS             10
#define LOGON_GET_TGT_IN_PROGRESS          11
#define LOGON_GET_TKT_IN_PROGRESS          12
#define LOGON_CS_IN_PROGRESS               13
#define LOGON_TASK_COMPLETE                99

//
// Transport information
//
#define KERB_KDC_PORT       88

//
// Retry info
//
DWORD g_dwNumKerbRetry = MAX_KERB_RETRIES;
DWORD g_dwNumMACSRetry = MAX_KERB_RETRIES;
DWORD g_dwKerbRetryDelayInMilliSeconds[MAX_KERB_RETRIES] = { 4000, 8000, 16000, 16000 };
DWORD g_dwMACSRetryDelayInMilliSeconds[MAX_KERB_RETRIES] = { 8000, 16000, 32000, 32000 };

#define GET_XNADDR_RETRY_PERIOD_IN_MS      200
#define MAX_TOTAL_LOGON_TIME_IN_MS       90000

//
// Task context structure for the implementation of Logon task
//
struct XONLINETASK_LOGON
{
    XONLINETASK_CONTEXT xonTask;
    HRESULT hr;
    DWORD dwState;
    BYTE abEventTimer[CBEVENTTIMER];
    DWORD  dwMaxEndingTickCount;
    HANDLE hCreatedEvent;
    
    XONLINETASK_HANDLE  hGetMachineAccountTask;
    XONLINE_USER machineUser;

    XONLINETASK_HANDLE  hGetCombinedTicketTask;
    PXKERB_TGT_CONTEXT  pCombinedTGTContext;

    DWORD adwServiceIDs[XONLINE_MAX_SERVICE_REQUEST];
    DWORD cServices;
    XONLINETASK_HANDLE hGetServiceTicketTask;
    PXKERB_SERVICE_CONTEXT pXKerbServiceContext;
    BOOL fLogonTimeCached;

    HANDLE hCombinedTicketCache;
    XONLINETASK_HANDLE hCombinedTicketCacheTask;
    DWORD dwCombinedTicketCacheState;
    BOOL fNeedToFlushCombinedTicketCache;
    XKERB_TGT_CONTEXT combinedContextToCache;

    HANDLE hServiceTicketCache;
    XONLINETASK_HANDLE hServiceTicketCacheTask;
    DWORD dwServiceTicketCacheState;
    BOOL fNeedToFlushServiceTicketCache;
    XKERB_SERVICE_CONTEXT serviceContextToCache;

    XONLINETASK_HANDLE hPresenceTask;
    BOOL fAliveSent;
};

//
// Task context structure for DNS Lookup
//
struct XONLINETASK_DNS_LOOKUP
{
    XONLINETASK_CONTEXT         xonTask;
    HRESULT                     hr;
    XONLINE_DNS_CACHE_ENTRY*    pCacheEntry;
    XNDNS*                      pCurrentLookup;
};

//
// Do work function for DNSLookup
//
HRESULT CXo::DNSLookupContinue(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    PXONLINETASK_DNS_LOOKUP  pDNSLookupTask = (PXONLINETASK_DNS_LOOKUP)hTask;
    XONLINE_DNS_CACHE_ENTRY* pCacheEntry = pDNSLookupTask->pCacheEntry;
    XNDNS* pCurrentLookup = pDNSLookupTask->pCurrentLookup;

    if (pDNSLookupTask->hr == XONLINETASK_S_RUNNING)
    {
        if (pCurrentLookup == NULL)
        {
            Assert( pCacheEntry != NULL );
            if (pCacheEntry->dwState == DNS_LOOKUP_COMPLETE)
            {
                pDNSLookupTask->hr = XONLINETASK_S_SUCCESS;
            }
            else if (pCacheEntry->dwState == DNS_LOOKUP_ERROR)
            {
                pDNSLookupTask->hr = XONLINE_E_LOGON_DNS_LOOKUP_FAILED;
            }
            else
            {
                Assert( pCacheEntry->dwState == DNS_LOOKUP_IN_PROGRESS );
            }
        }
        else if (pCurrentLookup->iStatus != WSAEINPROGRESS)
        {
            if (pCurrentLookup->iStatus == 0)
            {
                TraceSz6( AuthVerbose, "XNetDnsLookup for %s has %d result(s): %d.%d.%d.%d",
                    pCacheEntry->name, pCurrentLookup->cina,
                    pCurrentLookup->aina[0].S_un.S_un_b.s_b1, 
                    pCurrentLookup->aina[0].S_un.S_un_b.s_b2, 
                    pCurrentLookup->aina[0].S_un.S_un_b.s_b3, 
                    pCurrentLookup->aina[0].S_un.S_un_b.s_b4 );
                pCacheEntry->address = *(pCurrentLookup);
                pCacheEntry->dwState = DNS_LOOKUP_COMPLETE;
                pDNSLookupTask->pCurrentLookup = NULL;
                pDNSLookupTask->hr = XONLINETASK_S_SUCCESS;
            }
            else
            {
                TraceSz1( AuthWarn, "XNetDnsLookup failed status is 0x%X", pCurrentLookup->iStatus );
                pCacheEntry->dwState = DNS_LOOKUP_ERROR;
                pDNSLookupTask->pCurrentLookup = NULL;
                //pDNSLookupTask->hr = HRESULT_FROM_WIN32(pCurrentLookup->iStatus);
                pDNSLookupTask->hr = XONLINE_E_LOGON_DNS_LOOKUP_FAILED;
            }

            XNetDnsRelease(pCurrentLookup);
        }
    }
    
    return(pDNSLookupTask->hr);
}

//
// Do work function for DNSLookup
//
void CXo::DNSLookupClose(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    PXONLINETASK_DNS_LOOKUP  pDNSLookupTask = (PXONLINETASK_DNS_LOOKUP)hTask;
    XNDNS* pCurrentLookup = pDNSLookupTask->pCurrentLookup;

    if (pDNSLookupTask->hr == XONLINETASK_S_RUNNING)
    {
        if (pCurrentLookup != NULL)
        {
            XNetDnsRelease(pCurrentLookup);
            Assert( pDNSLookupTask->pCacheEntry != NULL );
            pDNSLookupTask->pCacheEntry->dwState = DNS_LOOKUP_ERROR;
        }
        pDNSLookupTask->hr = XONLINE_E_LOGON_DNS_LOOKUP_FAILED;
    }
    
    pDNSLookupTask->pCurrentLookup = NULL;
    pDNSLookupTask->pCacheEntry = NULL;
}

//
// Function to kick off a DNS Lookup
//
HRESULT CXo::DNSLookup(char * pszDNSName, HANDLE hWorkEvent, XONLINETASK_DNS_LOOKUP * pDNSLookupTask)
{
    HRESULT hr = S_OK;
    int err;
    int i;
    DWORD dwPrevState;
    XONLINE_DNS_CACHE_ENTRY* pCacheEntry = NULL;
    
    Assert(pszDNSName != NULL);
    Assert(pDNSLookupTask != NULL);

    RtlZeroMemory( pDNSLookupTask, sizeof(*pDNSLookupTask) );
    
    for (i=0; i<XONLINE_NUM_DNS_CACHE_ENTRYS; ++i)
    {
        if (_stricmp(pszDNSName, m_DNSCache[i].name) == 0)
        {
            pCacheEntry = &m_DNSCache[i];
            break;
        }
    }
    
    Assert(pCacheEntry != NULL);
    if (pCacheEntry == NULL)
    {
        TraceSz1( AuthWarn, "XNetDnsLookup call with bad name: %s", pszDNSName );
        hr = XONLINE_E_LOGON_DNS_LOOKUP_FAILED;
        goto Cleanup;        
    }

    TaskInitializeContext(&(pDNSLookupTask->xonTask));
    pDNSLookupTask->xonTask.pfnContinue = DNSLookupContinue;
    pDNSLookupTask->xonTask.pfnClose = DNSLookupClose;
    pDNSLookupTask->pCacheEntry = pCacheEntry;
    pDNSLookupTask->hr = XONLINETASK_S_RUNNING;
    
    dwPrevState = InterlockedCompareExchange((PLONG)&pCacheEntry->dwState, DNS_LOOKUP_IN_PROGRESS, 0);

    switch (dwPrevState)
    {
    case DNS_LOOKUP_COMPLETE:
        //
        // DNS lookup already complete for this entry
        // SetEvent to make sure DNSLookupContinue is called
        //
        SetEvent( hWorkEvent );
        //TraceSz( AuthVerbose, "SetEvent at DNSLookup DNS_LOOKUP_COMPLETE" );
        pDNSLookupTask->hr = XONLINETASK_S_SUCCESS;
        hr = S_OK;
        break;        
    case DNS_LOOKUP_ERROR:
        pDNSLookupTask->hr = hr = XONLINE_E_LOGON_DNS_LOOKUP_FAILED;
        break;        
    case DNS_LOOKUP_IN_PROGRESS:
        //
        // DNS lookup already in progress for this entry
        // SetEvent to make sure DNSLookupContinue is called
        //
        SetEvent( hWorkEvent );
        //TraceSz( AuthVerbose, "SetEvent at DNSLookup DNS_LOOKUP_IN_PROGRESS" );
        pDNSLookupTask->hr = XONLINETASK_S_RUNNING;
        hr = S_OK;
        break;        
    case 0:
        pDNSLookupTask->xonTask.hEventWorkAvailable = hWorkEvent;
        err = XNetDnsLookup(pszDNSName, hWorkEvent, &pDNSLookupTask->pCurrentLookup);
        if (err != 0 || pDNSLookupTask->pCurrentLookup == NULL)
        {
            TraceSz( AuthWarn, "XNetDnsLookup call failed!");
            pCacheEntry->dwState = DNS_LOOKUP_ERROR;
            pDNSLookupTask->pCurrentLookup = NULL;
            pDNSLookupTask->hr = hr = XONLINE_E_LOGON_DNS_LOOKUP_FAILED;
            goto Cleanup;
        }
        pDNSLookupTask->hr = XONLINETASK_S_RUNNING;
        hr = S_OK;
        break;        
    default:
        AssertSz(FALSE, "DNSLookupContinue: DNS lookup in bad state!");
    }
    
Cleanup:
    return(hr);
}

//
// Function to retrieve the results of a DNSLookup
//
HRESULT CXo::DNSLookupGetResults(XONLINETASK_HANDLE hTask, XNDNS ** ppDestAddr)
{
    Assert(hTask != NULL);
    Assert(ppDestAddr != NULL);

    PXONLINETASK_DNS_LOOKUP  pDNSLookupTask = (PXONLINETASK_DNS_LOOKUP)hTask;

    if (pDNSLookupTask->hr == XONLINETASK_S_SUCCESS)
    {
        *ppDestAddr = &pDNSLookupTask->pCacheEntry->address;
    }
    else
    {
        *ppDestAddr = NULL;
    }
    return(pDNSLookupTask->hr);
}

//
// Function to retrieve the results of a DNSLookup
//
BOOL CXo::DNSLookupIsValidTask(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    return(((PXONLINETASK_DNS_LOOKUP)hTask)->pCacheEntry != NULL);
}

//
// Define a task context structure for
//
struct XONLINETASK_GET_SINGLE_TICKET
{
    XONLINETASK_CONTEXT     xonTask;
    HRESULT                 hr;
    DWORD                   dwState;
    DWORD                   dwAdjustedTime;
    DWORD                   dwRetryCount;
    DWORD                   dwNextRetryTickCount;
    BYTE                    abEventTimer[CBEVENTTIMER];
    BOOL                    fMachineAccount;
    PXONLINE_USER           pUser;
    XONLINETASK_DNS_LOOKUP  DNSTask;
    XNDNS*                  pDestAddress;
    SOCKET                  socket;
    WSABUF                  WSABuffer;
    XONLINETASK_SOCKIO      SendRecvTask; // No XOnlineCloseTask needed
    BYTE                    recvBuffer[XONLINE_AS_EXCHANGE_BUFFER_SIZE];
    PXKERB_TGT_CONTEXT      pSingleTGTContext;
};

//
// Send AS Request for GetSingleTicket
//
HRESULT CXo::GetSingleTicketSendAsRequest(PXONLINETASK_GET_SINGLE_TICKET pGetSingleTicketTask)
{
    int err;
    HRESULT hr = S_OK;
    WSABUF WSABuffer;
    WSAOVERLAPPED wsaOverlapped;
    struct sockaddr_in ServerAddress;
    BYTE abBuffer[XONLINE_AS_EXCHANGE_BUFFER_SIZE];
    DWORD dwMessageLength = sizeof(abBuffer);

    Assert (pGetSingleTicketTask->pSingleTGTContext != NULL);
    
    hr = XkerbBuildAsRequest(
        pGetSingleTicketTask->fMachineAccount,
        (PXONLINEP_USER)pGetSingleTicketTask->pUser,
        abBuffer,
        &dwMessageLength,
        pGetSingleTicketTask->pSingleTGTContext
        );
    if (FAILED(hr))
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr = pGetSingleTicketTask->pDestAddress->aina[pGetSingleTicketTask->dwRetryCount % pGetSingleTicketTask->pDestAddress->cina];
    ServerAddress.sin_port = htons(KERB_KDC_PORT);

    WSABuffer.len = dwMessageLength;
    WSABuffer.buf = (char*)abBuffer;
    wsaOverlapped.hEvent = pGetSingleTicketTask->xonTask.hEventWorkAvailable;
    
    TraceSz5( AuthVerbose, "GetSingleTicketSendAsRequest sent %d bytes to: %d.%d.%d.%d",
        dwMessageLength,
        pGetSingleTicketTask->pDestAddress->aina[pGetSingleTicketTask->dwRetryCount % pGetSingleTicketTask->pDestAddress->cina].S_un.S_un_b.s_b1, 
        pGetSingleTicketTask->pDestAddress->aina[pGetSingleTicketTask->dwRetryCount % pGetSingleTicketTask->pDestAddress->cina].S_un.S_un_b.s_b2, 
        pGetSingleTicketTask->pDestAddress->aina[pGetSingleTicketTask->dwRetryCount % pGetSingleTicketTask->pDestAddress->cina].S_un.S_un_b.s_b3, 
        pGetSingleTicketTask->pDestAddress->aina[pGetSingleTicketTask->dwRetryCount % pGetSingleTicketTask->pDestAddress->cina].S_un.S_un_b.s_b4); 

    err = WSASendTo(
            pGetSingleTicketTask->socket,
            &WSABuffer, 1,
            &dwMessageLength, 0,
            (LPSOCKADDR) &ServerAddress, sizeof(ServerAddress),
            &wsaOverlapped, NULL);
    if (err == SOCKET_ERROR)
    {
        TraceSz1( AuthWarn, "GetSingleTicketSendAsRequest: WSASendTo Failed WSAGetLastError: 0x%X", WSAGetLastError() );
        hr = XONLINE_E_LOGON_AUTHENTICATION_FAILED;
        goto Cleanup;
    }

    pGetSingleTicketTask->dwNextRetryTickCount =
        GetTickCount() + g_dwKerbRetryDelayInMilliSeconds[pGetSingleTicketTask->dwRetryCount];

    XnSetEventTimer(
        pGetSingleTicketTask->abEventTimer,
        pGetSingleTicketTask->xonTask.hEventWorkAvailable,
        g_dwKerbRetryDelayInMilliSeconds[pGetSingleTicketTask->dwRetryCount] );
    
Cleanup:
    return hr;
}

//
// Do work function for GetSingleTicket
//
HRESULT CXo::GetSingleTicketContinue(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    HRESULT hr = S_OK;
    DWORD dwMessageLength;
    PXONLINETASK_GET_SINGLE_TICKET pGetSingleTicketTask = (PXONLINETASK_GET_SINGLE_TICKET)hTask;

    switch (pGetSingleTicketTask->dwState)
    {
    case LOGON_DNS_IN_PROGRESS:
        hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)&pGetSingleTicketTask->DNSTask );
        if (hr == XONLINETASK_S_SUCCESS)
        {
            hr = DNSLookupGetResults( (XONLINETASK_HANDLE)&pGetSingleTicketTask->DNSTask, &pGetSingleTicketTask->pDestAddress );
            Assert( hr == XONLINETASK_S_SUCCESS );
            
            pGetSingleTicketTask->socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
            if (pGetSingleTicketTask->socket == INVALID_SOCKET)
            {
                TraceSz1( AuthWarn, "GetSingleTicketContinue: socket create failed WSAGetLastError:0x%X", WSAGetLastError() );
                pGetSingleTicketTask->hr = XONLINE_E_LOGON_AUTHENTICATION_FAILED;
                goto Cleanup;
            }

            XnSetInsecure(pGetSingleTicketTask->socket);

            pGetSingleTicketTask->WSABuffer.len = sizeof(pGetSingleTicketTask->recvBuffer);
            pGetSingleTicketTask->WSABuffer.buf = (char*)pGetSingleTicketTask->recvBuffer;

            SendRecvInitializeContext(
                    pGetSingleTicketTask->socket,
                    &pGetSingleTicketTask->WSABuffer, 1, 0,
                    pGetSingleTicketTask->xonTask.hEventWorkAvailable,
                    &pGetSingleTicketTask->SendRecvTask );

            hr = GetSingleTicketSendAsRequest( pGetSingleTicketTask );
            if (FAILED(hr))
            {
                pGetSingleTicketTask->hr = hr;
                goto Cleanup;
            }
            
            //
            // Kick off the asynchronous recv operation
            //
            hr = RecvInternal(&pGetSingleTicketTask->SendRecvTask);
            if (FAILED(hr))
            {
                TraceSz1( AuthWarn, "GetSingleTicketContinue: RecvInternal failed hr:0x%X", hr );
                pGetSingleTicketTask->hr = XONLINE_E_LOGON_AUTHENTICATION_FAILED;
                goto Cleanup;
            }

            pGetSingleTicketTask->dwState = LOGON_RECV_IN_PROGRESS;
        }
        else if (FAILED(hr))
        {
            pGetSingleTicketTask->hr = hr;
            goto Cleanup;
        }
        else
        {
            Assert( hr == XONLINETASK_S_RUNNING );
        }
        
        break;

    case LOGON_RECV_IN_PROGRESS:
        hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)&pGetSingleTicketTask->SendRecvTask );
        if (hr == XONLINETASK_S_SUCCESS)
        {
            dwMessageLength = 0;
            hr = GetSendRecvResults( (XONLINETASK_HANDLE)&pGetSingleTicketTask->SendRecvTask, &dwMessageLength, NULL, NULL );
            Assert( hr == XONLINETASK_S_SUCCESS );
            Assert( dwMessageLength > 0 );
            
            TraceSz2( AuthVerbose, "GetSingleTicketContinue received %d bytes for user: %s", dwMessageLength, pGetSingleTicketTask->pUser->name);

            hr = XkerbVerifyAsReply(
                pGetSingleTicketTask->recvBuffer,
                dwMessageLength,
                pGetSingleTicketTask->pSingleTGTContext
                );
            if ( hr == (HRESULT) STATUS_TIME_DIFFERENCE_AT_DC )
            {
                TraceSz( AuthWarn, "GetSingleTicketContinue adjusted Xbox clock according to KDC!");
                if (pGetSingleTicketTask->dwAdjustedTime < MAX_TIME_ADJUSTMENTS)
                {
                    ++pGetSingleTicketTask->dwAdjustedTime;
                  
                    hr = GetSingleTicketSendAsRequest( pGetSingleTicketTask );
                    if (FAILED(hr))
                    {
                        pGetSingleTicketTask->hr = hr;
                        goto Cleanup;
                    }

                    //
                    // Kick off another new asynchronous recv operation
                    //
                    hr = RecvInternal(&pGetSingleTicketTask->SendRecvTask);
                    if (FAILED(hr))
                    {
                        TraceSz1( AuthWarn, "GetSingleTicketContinue: RecvInternal failed hr: 0x%X", hr );
                        pGetSingleTicketTask->hr = XONLINE_E_LOGON_AUTHENTICATION_FAILED;
                        goto Cleanup;
                    }

                    //
                    // State not changed, go back and continue waiting for a response
                    //
                    goto Cleanup;
                }
                else
                {
                    TraceSz1( AuthWarn, "GetSingleTicketContinue: adjusting time too many times: %d", pGetSingleTicketTask->dwAdjustedTime );
                    pGetSingleTicketTask->hr = XONLINE_E_LOGON_AUTHENTICATION_FAILED;
                    goto Cleanup;
                }
            }
            else if (FAILED(hr))
            {
                TraceSz1( AuthWarn, "GetSingleTicketContinue: XkerbVerifyAsReply failed with hr: 0x%X", hr );
                pGetSingleTicketTask->hr = XONLINE_E_LOGON_AUTHENTICATION_FAILED;
                goto Cleanup;
            }

            //
            // We succeeded!
            //
            pGetSingleTicketTask->dwState = LOGON_TASK_COMPLETE;
            pGetSingleTicketTask->hr = XONLINETASK_S_SUCCESS;
            if (pGetSingleTicketTask->pSingleTGTContext->pAccountCreationPreAuth != NULL)
            {
                SysFree( pGetSingleTicketTask->pSingleTGTContext->pAccountCreationPreAuth );            
            }
            break;
        }
        else if (FAILED(hr))
        {
            TraceSz1( AuthWarn, "GetSingleTicketContinue: recv failed with hr: 0x%X", hr );
            pGetSingleTicketTask->hr = XONLINE_E_LOGON_AUTHENTICATION_FAILED;
            goto Cleanup;            
        }

        //
        // Check if we timed out
        //
        Assert( hr == XONLINETASK_S_RUNNING );
        if (GetTickCount() >= pGetSingleTicketTask->dwNextRetryTickCount)
        {
            if ( ++pGetSingleTicketTask->dwRetryCount >= g_dwNumKerbRetry )
            {
                TraceSz1( AuthWarn, "GetSingleTicketContinue: timed out after %d retries", pGetSingleTicketTask->dwRetryCount );
                pGetSingleTicketTask->hr = XONLINE_E_LOGON_AUTHENTICATION_TIMEDOUT;
                goto Cleanup;
            }
            
            //
            // Need to retry with the next IP address
            //
            hr = GetSingleTicketSendAsRequest( pGetSingleTicketTask );
            if (FAILED(hr))
            {
                pGetSingleTicketTask->hr = hr;
                goto Cleanup;
            }
        }
        break;
    
    case LOGON_TASK_COMPLETE:
        break;
        
    default:
        AssertSz(FALSE, "GetSingleTicketContinue: Bad State");
    }

Cleanup:
    if (pGetSingleTicketTask->hr != XONLINETASK_S_RUNNING)
    {
        pGetSingleTicketTask->dwState = LOGON_TASK_COMPLETE;
    }
    
    return(pGetSingleTicketTask->hr);
}

//
// Close function for GetSingleTicket
//
void CXo::GetSingleTicketClose(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    PXONLINETASK_GET_SINGLE_TICKET pGetSingleTicketTask = (PXONLINETASK_GET_SINGLE_TICKET)hTask;
    
    if (DNSLookupIsValidTask( (XONLINETASK_HANDLE)&(pGetSingleTicketTask->DNSTask) ))
    {
        XOnlineTaskClose((XONLINETASK_HANDLE)&(pGetSingleTicketTask->DNSTask));
    }

    if (pGetSingleTicketTask->socket != INVALID_SOCKET)
    {
        WSACancelOverlappedIO( pGetSingleTicketTask->socket );
        closesocket(pGetSingleTicketTask->socket);
    }

    if (pGetSingleTicketTask->pSingleTGTContext != NULL)
    {
        Assert (pGetSingleTicketTask->pSingleTGTContext->pAccountCreationPreAuth == NULL);
        if (pGetSingleTicketTask->pSingleTGTContext->pInputTGTContext != NULL)
        {
            SysFree( pGetSingleTicketTask->pSingleTGTContext->pInputTGTContext );            
        }
        SysFree( pGetSingleTicketTask->pSingleTGTContext );
    }

    XnSetEventTimer(pGetSingleTicketTask->abEventTimer, NULL, 0xFFFFFFFF);

    SysFree(pGetSingleTicketTask);
}

//
// Function to kick off a GetSingleTicket
//
HRESULT CXo::GetSingleTicket(BOOL fMachineAccount, PXKERB_TGT_CONTEXT pInputTGTContext, PXONLINE_USER pUser, HANDLE hWorkEvent, PXONLINETASK_HANDLE phGetSingleTicketTask)
{
    HRESULT hr;
    PXONLINETASK_GET_SINGLE_TICKET pGetSingleTicketTask;
    
    Assert(pUser != NULL);
    Assert(phGetSingleTicketTask != NULL);

    pGetSingleTicketTask = (PXONLINETASK_GET_SINGLE_TICKET)SysAllocZ(sizeof(*pGetSingleTicketTask),
        PTAG_XONLINETASK_GET_SINGLE_TICKET);
    if (pGetSingleTicketTask == NULL)
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }
    
    TaskInitializeContext(&(pGetSingleTicketTask->xonTask));
    pGetSingleTicketTask->xonTask.pfnContinue = GetSingleTicketContinue;
    pGetSingleTicketTask->xonTask.pfnClose = GetSingleTicketClose;
    pGetSingleTicketTask->xonTask.hEventWorkAvailable = hWorkEvent;
    pGetSingleTicketTask->pUser = pUser;
    pGetSingleTicketTask->hr = XONLINETASK_S_RUNNING;
    pGetSingleTicketTask->socket = INVALID_SOCKET;
    pGetSingleTicketTask->fMachineAccount = fMachineAccount;
    
    pGetSingleTicketTask->pSingleTGTContext = (PXKERB_TGT_CONTEXT)SysAllocZ(sizeof(*(pGetSingleTicketTask->pSingleTGTContext)), PTAG_XKERB_TGT_CONTEXT);
    if ( pGetSingleTicketTask->pSingleTGTContext == NULL )
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }
    pGetSingleTicketTask->pSingleTGTContext->pInputTGTContext = pInputTGTContext;
    
    hr = DNSLookup(
            PASSPORT_AS_DNS,
            hWorkEvent,
            &pGetSingleTicketTask->DNSTask
            );
    pGetSingleTicketTask->dwState = LOGON_DNS_IN_PROGRESS;

    //
    // Pump it once to get things going here
    //
    GetSingleTicketContinue( (XONLINETASK_HANDLE) pGetSingleTicketTask );
    
    *phGetSingleTicketTask = (XONLINETASK_HANDLE) pGetSingleTicketTask;

Cleanup:
    if (FAILED(hr))
    {
        *phGetSingleTicketTask = NULL;
        if (pGetSingleTicketTask != NULL)
        {
            if (DNSLookupIsValidTask( (XONLINETASK_HANDLE)&(pGetSingleTicketTask->DNSTask) ))
            {
                XOnlineTaskClose((XONLINETASK_HANDLE)&(pGetSingleTicketTask->DNSTask));
            }

            SysFree(pGetSingleTicketTask);
        }        
    }    
    return(hr);
}

//
// Function to retrieve the results of a GetSingleTicket
//
HRESULT CXo::GetSingleTicketGetResults(XONLINETASK_HANDLE hTask, PXKERB_TGT_CONTEXT * ppSingleTGTContext)
{
    Assert(hTask != NULL);
    Assert(ppSingleTGTContext != NULL);

    PXONLINETASK_GET_SINGLE_TICKET  pGetSingleTicketTask = (PXONLINETASK_GET_SINGLE_TICKET)hTask;

    if (pGetSingleTicketTask->hr == XONLINETASK_S_SUCCESS)
    {
        if (pGetSingleTicketTask->pSingleTGTContext->pInputTGTContext != NULL)
        {
            SysFree( pGetSingleTicketTask->pSingleTGTContext->pInputTGTContext );       
            pGetSingleTicketTask->pSingleTGTContext->pInputTGTContext = NULL;
        }
        *ppSingleTGTContext = pGetSingleTicketTask->pSingleTGTContext;
        pGetSingleTicketTask->pSingleTGTContext = NULL;
    }
    else
    {
        *ppSingleTGTContext = NULL;
    }
    return(pGetSingleTicketTask->hr);
}


//
// Define a task context structure for
//
struct XONLINETASK_GET_MACHINE_ACCOUNT
{
    XONLINETASK_CONTEXT     xonTask;
    HRESULT                 hr;
    DWORD                   dwState;
    DWORD                   dwAdjustedTime;
    DWORD                   dwRetryCount;
    DWORD                   dwNextRetryTickCount;
    BYTE                    abEventTimer[CBEVENTTIMER];
    XONLINEP_USER           machineUser;
    XONLINETASK_DNS_LOOKUP  DNSTask;
    XNDNS*                  pDestAddress;
    SOCKET                  socket;
    WSABUF                  WSABuffer;
    XONLINETASK_SOCKIO      SendRecvTask; // No XOnlineCloseTask needed
    BYTE                    recvBuffer[XONLINE_AS_EXCHANGE_BUFFER_SIZE];
    PXKERB_TGT_CONTEXT      pSingleTGTContext;
};

//
// Send AS Request for GetSingleTicket
//
HRESULT CXo::GetMachineAccountSendAsRequest(PXONLINETASK_GET_MACHINE_ACCOUNT pGetMachineAccountTask)
{
    int err;
    HRESULT hr = S_OK;
    WSABUF WSABuffer;
    WSAOVERLAPPED wsaOverlapped;
    struct sockaddr_in ServerAddress;
    NTSTATUS Status;
    DWORD dwType;
    DWORD cbResultLength;
    XBOX_FACTORY_SETTINGS factorySettings;
    XONLINEP_USER onlineUser;
    BYTE abBuffer[XONLINE_AS_EXCHANGE_BUFFER_SIZE];
    DWORD dwMessageLength = sizeof(abBuffer);

#ifdef TODO_REALLY_READ_ONLINE_KEY_FROM_EEPROM
    //
    // Read Serial Number from the EEPROM
    //
    hr = _XOnlineGetSerialNumber((CHAR*)factorySettings.SerialNumber);
    if (FAILED(hr))
    {
        TraceSz1( AuthWarn, "GetMachineAccountSendAsRequest: _XOnlineGetSerialNumber Failed hr:0x%X", hr );
        hr = XONLINE_E_LOGON_INVALID_XBOX_ONLINE_INFO;
        goto Cleanup;
    }

    //
    // Read OnlineKey from the EEPROM
    //
    Status = ExQueryNonVolatileSetting(XC_FACTORY_ONLINE_KEY, &dwType, factorySettings.OnlineKey, sizeof(factorySettings.OnlineKey), &cbResultLength);
    if (Status != ERROR_SUCCESS)
    {
        TraceSz1( AuthWarn, "GetMachineAccountSendAsRequest: ExQueryNonVolatileSetting Failed Status:0x%X", Status );
        hr = XONLINE_E_LOGON_INVALID_XBOX_ONLINE_INFO;
        goto Cleanup;
    }
    else
    {
        Assert( dwType == REG_BINARY );
        Assert( cbResultLength == sizeof(factorySettings.OnlineKey) );
    }

    //
    // morph it
    //
    MorphKeyByHardDriveKeyEx( GetHdKey(), factorySettings.OnlineKey, sizeof(factorySettings.OnlineKey) );
#else
    RtlCopyMemory( factorySettings.SerialNumber, "999999999999", sizeof(factorySettings.SerialNumber) );
    RtlCopyMemory( factorySettings.OnlineKey, DEVKIT_ONLINE_RAND_KEY, sizeof(factorySettings.OnlineKey) );    
#endif

    //
    // Create machine account from EEPROM info
    //
    RtlZeroMemory( &onlineUser, sizeof(onlineUser) );
    RtlCopyMemory(onlineUser.name, factorySettings.SerialNumber, sizeof(factorySettings.SerialNumber));
    Assert( sizeof(onlineUser.key) == sizeof(factorySettings.OnlineKey) );
    RtlCopyMemory( onlineUser.key, factorySettings.OnlineKey, sizeof(onlineUser.key) );
    RtlCopyMemory( onlineUser.realm, MACS_REALM, sizeof(MACS_REALM) );

    //
    // Allocate memory for pSingleTGTContext
    //
    if (pGetMachineAccountTask->pSingleTGTContext == NULL)
    {
        pGetMachineAccountTask->pSingleTGTContext = (PXKERB_TGT_CONTEXT)SysAllocZ(sizeof(*(pGetMachineAccountTask->pSingleTGTContext)),
            PTAG_XKERB_TGT_CONTEXT);
        if ( pGetMachineAccountTask->pSingleTGTContext == NULL )
        {
            hr = XONLINE_E_OUT_OF_MEMORY;
            goto Cleanup;
        }
    }
    
    hr = XkerbBuildAsRequest(
        FALSE,
        &onlineUser,
        abBuffer,
        &dwMessageLength,
        pGetMachineAccountTask->pSingleTGTContext
        );
    if (FAILED(hr))
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr = pGetMachineAccountTask->pDestAddress->aina[pGetMachineAccountTask->dwRetryCount % pGetMachineAccountTask->pDestAddress->cina];
    ServerAddress.sin_port = htons(KERB_KDC_PORT);

    WSABuffer.len = dwMessageLength;
    WSABuffer.buf = (char*)abBuffer;
    wsaOverlapped.hEvent = pGetMachineAccountTask->xonTask.hEventWorkAvailable;
    
    TraceSz5( AuthVerbose, "GetMachineAccountSendAsRequest send %d bytes to: %d.%d.%d.%d",
        dwMessageLength,
        pGetMachineAccountTask->pDestAddress->aina[pGetMachineAccountTask->dwRetryCount % pGetMachineAccountTask->pDestAddress->cina].S_un.S_un_b.s_b1, 
        pGetMachineAccountTask->pDestAddress->aina[pGetMachineAccountTask->dwRetryCount % pGetMachineAccountTask->pDestAddress->cina].S_un.S_un_b.s_b2, 
        pGetMachineAccountTask->pDestAddress->aina[pGetMachineAccountTask->dwRetryCount % pGetMachineAccountTask->pDestAddress->cina].S_un.S_un_b.s_b3, 
        pGetMachineAccountTask->pDestAddress->aina[pGetMachineAccountTask->dwRetryCount % pGetMachineAccountTask->pDestAddress->cina].S_un.S_un_b.s_b4); 

    err = WSASendTo(
            pGetMachineAccountTask->socket,
            &WSABuffer, 1,
            &dwMessageLength, 0,
            (LPSOCKADDR) &ServerAddress, sizeof(ServerAddress),
            &wsaOverlapped, NULL);
    if (err == SOCKET_ERROR)
    {
        TraceSz1( AuthWarn, "GetMachineAccountSendAsRequest: WSASendTo Failed WSAGetLastError:0x%X", WSAGetLastError() );
        hr = XONLINE_E_LOGON_MACS_FAILED;
        goto Cleanup;
    }

    pGetMachineAccountTask->dwNextRetryTickCount =
        GetTickCount() + g_dwMACSRetryDelayInMilliSeconds[pGetMachineAccountTask->dwRetryCount];

    XnSetEventTimer(
        pGetMachineAccountTask->abEventTimer,
        pGetMachineAccountTask->xonTask.hEventWorkAvailable,
        g_dwMACSRetryDelayInMilliSeconds[pGetMachineAccountTask->dwRetryCount] );
    
Cleanup:
    return hr;
}

//
// Do work function for GetSingleTicket
//
HRESULT CXo::GetMachineAccountContinue(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    HRESULT hr = S_OK;
    DWORD dwMessageLength;
    PXONLINETASK_GET_MACHINE_ACCOUNT pGetMachineAccountTask = (PXONLINETASK_GET_MACHINE_ACCOUNT)hTask;
    
    switch (pGetMachineAccountTask->dwState)
    {
    case LOGON_DNS_IN_PROGRESS:
        hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)&pGetMachineAccountTask->DNSTask );
        if (hr == XONLINETASK_S_SUCCESS)
        {
            hr = DNSLookupGetResults( (XONLINETASK_HANDLE)&pGetMachineAccountTask->DNSTask, &pGetMachineAccountTask->pDestAddress );
            Assert( hr == XONLINETASK_S_SUCCESS );
            
            pGetMachineAccountTask->socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
            if (pGetMachineAccountTask->socket == INVALID_SOCKET)
            {
                TraceSz1( AuthWarn, "GetMachineAccountContinue: socket create failed WSAGetLastError:0x%X", WSAGetLastError() );
                pGetMachineAccountTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                goto Cleanup;
            }

            XnSetInsecure(pGetMachineAccountTask->socket);

            pGetMachineAccountTask->WSABuffer.len = sizeof(pGetMachineAccountTask->recvBuffer);
            pGetMachineAccountTask->WSABuffer.buf = (char*)pGetMachineAccountTask->recvBuffer;

            SendRecvInitializeContext(
                    pGetMachineAccountTask->socket,
                    &pGetMachineAccountTask->WSABuffer, 1, 0,
                    pGetMachineAccountTask->xonTask.hEventWorkAvailable,
                    &pGetMachineAccountTask->SendRecvTask );

            hr = GetMachineAccountSendAsRequest( pGetMachineAccountTask );
            if (FAILED(hr))
            {
                pGetMachineAccountTask->hr = hr;
                goto Cleanup;
            }
            
            //
            // Kick off the asynchronous recv operation
            //
            hr = RecvInternal(&pGetMachineAccountTask->SendRecvTask);
            if (FAILED(hr))
            {
                TraceSz1( AuthWarn, "GetMachineAccountContinue: RecvInternal failed hr:0x%X", hr );
                pGetMachineAccountTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                goto Cleanup;
            }

            pGetMachineAccountTask->dwState = LOGON_RECV_IN_PROGRESS;
            break;
        }
        else if (FAILED(hr))
        {
            pGetMachineAccountTask->hr = hr;
            goto Cleanup;
        }
        else
        {
            Assert( hr == XONLINETASK_S_RUNNING );
        }
        break;

    case LOGON_RECV_IN_PROGRESS:
        hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)&pGetMachineAccountTask->SendRecvTask );
        if (hr == XONLINETASK_S_SUCCESS)
        {
            dwMessageLength = 0;
            hr = GetSendRecvResults( (XONLINETASK_HANDLE)&pGetMachineAccountTask->SendRecvTask, &dwMessageLength, NULL, NULL );
            Assert( hr == XONLINETASK_S_SUCCESS );
            Assert( dwMessageLength > 0 );
            
            TraceSz1( AuthVerbose, "GetMachineAccountContinue received %d bytes!", dwMessageLength);

            hr = XkerbVerifyAsReply(
                pGetMachineAccountTask->recvBuffer,
                dwMessageLength,
                pGetMachineAccountTask->pSingleTGTContext
                );
            if ( hr == (HRESULT) STATUS_TIME_DIFFERENCE_AT_DC )
            {
                TraceSz( AuthWarn, "GetMachineAccountContinue adjusted Xbox clock according to KDC!");
                if (pGetMachineAccountTask->dwAdjustedTime < MAX_TIME_ADJUSTMENTS)
                {
                    ++pGetMachineAccountTask->dwAdjustedTime;
                    
                    hr = GetMachineAccountSendAsRequest( pGetMachineAccountTask );
                    if (FAILED(hr))
                    {
                        pGetMachineAccountTask->hr = hr;
                        goto Cleanup;
                    }

                    //
                    // Kick off another new asynchronous recv operation
                    //
                    hr = RecvInternal(&pGetMachineAccountTask->SendRecvTask);
                    if (FAILED(hr))
                    {
                        TraceSz1( AuthWarn, "GetMachineAccountContinue: RecvInternal failed hr:0x%X", hr );
                        pGetMachineAccountTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                        goto Cleanup;
                    }

                    //
                    // State not changed, go back and continue waiting for a response
                    //
                    goto Cleanup;
                }
                else
                {
                    TraceSz1( AuthWarn, "GetMachineAccountContinue: adjusting time too many times: %d", pGetMachineAccountTask->dwAdjustedTime );
                    pGetMachineAccountTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                    goto Cleanup;
                }
            }
            else if (FAILED(hr))
            {
                TraceSz1( AuthWarn, "GetMachineAccountContinue: XkerbVerifyAsReply failed with hr: 0x%X", hr );
                pGetMachineAccountTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                goto Cleanup;
            }

            //
            // Handle account creation preauth returned from MACS
            //
            if ( pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth == NULL )
            {
                TraceSz( AuthWarn, "GetMachineAccountContinue: Kerb reply contains no pre-auth" );
                pGetMachineAccountTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                goto Cleanup;
            }

            m_qwMachineId = pGetMachineAccountTask->machineUser.xuid.qwUserID = pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth->qwUserID;
            RtlCopyMemory( pGetMachineAccountTask->machineUser.name, pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth->name, sizeof(pGetMachineAccountTask->machineUser.name) );
            RtlCopyMemory( pGetMachineAccountTask->machineUser.kingdom, pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth->kingdom, sizeof(pGetMachineAccountTask->machineUser.kingdom) );
            RtlCopyMemory( pGetMachineAccountTask->machineUser.domain, pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth->domain, sizeof(pGetMachineAccountTask->machineUser.domain) );
            RtlCopyMemory( pGetMachineAccountTask->machineUser.realm, pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth->realm, sizeof(pGetMachineAccountTask->machineUser.realm) );
            RtlCopyMemory( pGetMachineAccountTask->machineUser.key, pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth->key, sizeof(pGetMachineAccountTask->machineUser.key) );

            if (!SetMachineAccount( &(pGetMachineAccountTask->machineUser) ) )
            {
                TraceSz( AuthWarn, "GetMachineAccountContinue: XoSetMachineAccount failed");
                pGetMachineAccountTask->hr = XONLINE_E_IO_ERROR;
                goto Cleanup;
            }

            //
            // We succeeded!
            //
            TraceSz4( AuthVerbose, "GetMachineAccountContinue recieved %d bytes new id:%I64X user:%s kingdom:%s", dwMessageLength, pGetMachineAccountTask->machineUser.xuid.qwUserID, pGetMachineAccountTask->machineUser.name, pGetMachineAccountTask->machineUser.kingdom );
            pGetMachineAccountTask->dwState = LOGON_TASK_COMPLETE;
            pGetMachineAccountTask->hr = XONLINETASK_S_SUCCESS;
            break;
        }
        else if (FAILED(hr))
        {
            TraceSz1( AuthWarn, "GetMachineAccountContinue: recv failed with hr: 0x%X", hr );
            pGetMachineAccountTask->hr = XONLINE_E_LOGON_MACS_FAILED;
            goto Cleanup;            
        }

        //
        // Check if we timed out
        //
        Assert( hr == XONLINETASK_S_RUNNING );
        if (GetTickCount() >= pGetMachineAccountTask->dwNextRetryTickCount)
        {
            if ( ++pGetMachineAccountTask->dwRetryCount >= g_dwNumMACSRetry )
            {
                TraceSz1( AuthWarn, "GetMachineAccountContinue: timed out after %d retries", pGetMachineAccountTask->dwRetryCount );
                pGetMachineAccountTask->hr = XONLINE_E_LOGON_MACS_TIMEDOUT;
                goto Cleanup;
            }
            
            //
            // Need to retry with the next IP address
            //
            hr = GetMachineAccountSendAsRequest( pGetMachineAccountTask );
            if (FAILED(hr))
            {
                pGetMachineAccountTask->hr = hr;
                goto Cleanup;
            }
        }
        break;
    
    case LOGON_TASK_COMPLETE:
        break;
        
    default:
        AssertSz(FALSE, "GetMachineAccountContinue: Bad State");
    }

Cleanup:
    if (pGetMachineAccountTask->hr != XONLINETASK_S_RUNNING)
    {
        pGetMachineAccountTask->dwState = LOGON_TASK_COMPLETE;
    }
    
    return(pGetMachineAccountTask->hr);
}

//
// Close function for GetSingleTicket
//
void CXo::GetMachineAccountClose(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    PXONLINETASK_GET_MACHINE_ACCOUNT pGetMachineAccountTask = (PXONLINETASK_GET_MACHINE_ACCOUNT)hTask;
    
    if (DNSLookupIsValidTask( (XONLINETASK_HANDLE)&(pGetMachineAccountTask->DNSTask) ))
    {
        XOnlineTaskClose((XONLINETASK_HANDLE)&(pGetMachineAccountTask->DNSTask));
    }

    if (pGetMachineAccountTask->socket != INVALID_SOCKET)
    {
        WSACancelOverlappedIO( pGetMachineAccountTask->socket );
        closesocket(pGetMachineAccountTask->socket);
    }

    if (pGetMachineAccountTask->pSingleTGTContext != NULL)
    {
        Assert (pGetMachineAccountTask->pSingleTGTContext->pInputTGTContext == NULL);
        if (pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth != NULL)
        {
            SysFree( pGetMachineAccountTask->pSingleTGTContext->pAccountCreationPreAuth );            
        }
        SysFree( pGetMachineAccountTask->pSingleTGTContext );
    }

    XnSetEventTimer(pGetMachineAccountTask->abEventTimer, NULL, 0xFFFFFFFF);

    SysFree(pGetMachineAccountTask);
}

//
// Function to kick off a GetSingleTicket
//
HRESULT CXo::GetMachineAccount(HANDLE hWorkEvent, PXONLINETASK_HANDLE phGetMachineAccountTask)
{
    HRESULT hr = S_OK;
    PXONLINETASK_GET_MACHINE_ACCOUNT pGetMachineAccountTask;
    XC_ONLINE_MACHINE_ACCOUNT_STRUCT* pMachineAccount;
    BYTE abConfigData[CONFIG_DATA_LENGTH];
    BOOL fUseExistingAccount = !m_fGetNewMachineAccount;
    
    Assert(phGetMachineAccountTask != NULL);

    m_fGetNewMachineAccount = FALSE;
    *phGetMachineAccountTask = NULL;
    pGetMachineAccountTask = (PXONLINETASK_GET_MACHINE_ACCOUNT)SysAllocZ(sizeof(*pGetMachineAccountTask),
        PTAG_XONLINETASK_GET_MACHINE_ACCOUNT);
    if (pGetMachineAccountTask == NULL)
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }
    
    TaskInitializeContext(&(pGetMachineAccountTask->xonTask));
    pGetMachineAccountTask->xonTask.pfnContinue = GetMachineAccountContinue;
    pGetMachineAccountTask->xonTask.pfnClose = GetMachineAccountClose;
    pGetMachineAccountTask->xonTask.hEventWorkAvailable = hWorkEvent;
    pGetMachineAccountTask->hr = XONLINETASK_S_RUNNING;
    pGetMachineAccountTask->socket = INVALID_SOCKET;
    
#ifdef XONLINE_FEATURE_INSECURE
    if (m_fForceNewMachineAccount)
    {
        fUseExistingAccount = FALSE;
    }
#endif

    //
    // Pull out information about the machine account from config sector
    //
    if (XNetLoadConfigSector(m_hConfigVolume, MACHINE_ACCOUNT_CONFIG_SECTOR, abConfigData, sizeof(abConfigData)))
    {
        if (fUseExistingAccount)
        {
            pMachineAccount = (XC_ONLINE_MACHINE_ACCOUNT_STRUCT*)abConfigData;
            if (VerifyOnlineUserSignature(pMachineAccount))
            {
                DecryptKeyWithHardDriveKeyEx( GetHdKey(), pMachineAccount->key, sizeof(pMachineAccount->key) );

                RtlCopyMemory( &(pGetMachineAccountTask->machineUser), pMachineAccount, sizeof(*pMachineAccount) );
                m_qwMachineId = pMachineAccount->xuid.qwUserID;

                pGetMachineAccountTask->dwState = LOGON_TASK_COMPLETE;
                pGetMachineAccountTask->hr = XONLINETASK_S_SUCCESS;
            }
        }
    }
    
    if ( pGetMachineAccountTask->hr == XONLINETASK_S_RUNNING )
    {
        hr = DNSLookup(
                MACS_KDC_DNS,
                hWorkEvent,
                &pGetMachineAccountTask->DNSTask
                );
        pGetMachineAccountTask->dwState = LOGON_DNS_IN_PROGRESS;
    }
    
    *phGetMachineAccountTask = (XONLINETASK_HANDLE) pGetMachineAccountTask;

Cleanup:
    if (FAILED(hr))
    {
        *phGetMachineAccountTask = NULL;
        if (pGetMachineAccountTask != NULL)
        {
            SysFree(pGetMachineAccountTask);
        }
    }
    return(hr);
}

//
// Function to retrieve the results of a GetSingleTicket
//
HRESULT CXo::GetMachineAccountGetResults(XONLINETASK_HANDLE hTask, PXONLINE_USER pMachineUser)
{
    Assert(hTask != NULL);
    Assert(pMachineUser != NULL);

    PXONLINETASK_GET_MACHINE_ACCOUNT  pGetMachineAccountTask = (PXONLINETASK_GET_MACHINE_ACCOUNT)hTask;

    if (pGetMachineAccountTask->hr == XONLINETASK_S_SUCCESS)
    {
        RtlCopyMemory( pMachineUser, &pGetMachineAccountTask->machineUser, sizeof(*pMachineUser) );
    }

    return(pGetMachineAccountTask->hr);
}

//
// Define a task context structure for
//
struct XONLINETASK_GET_COMBINED_TICKET
{
    XONLINETASK_CONTEXT     xonTask;
    HRESULT                 hr;
    DWORD                   dwState;
    PXONLINE_USER           pMachineUser;
    DWORD                   dwCurrentUniqueUser;
    XONLINETASK_HANDLE      hGetTGTTask;
    XONLC_CONTEXT           cacheContext;
    BOOL                    fTicketFromCache;
    PXKERB_TGT_CONTEXT      pTGTContext;
};

//
// Do work function for GetCombinedTicket
//
HRESULT CXo::GetCombinedTicketContinue(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    DWORD i;
    HRESULT hr = S_OK;
    BOOL fMoreWorkToDo;
    TimeStamp CurrentTime;
    PXONLINETASK_GET_COMBINED_TICKET pGetCombinedTicketTask = (PXONLINETASK_GET_COMBINED_TICKET)hTask;

    do
    {
        fMoreWorkToDo = FALSE;
        
        switch (pGetCombinedTicketTask->dwState)
        {
        case LOGON_START_CACHE_LOOKUP:
            if (m_pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_OPENING)
            {    
                break;
            }

            if (m_pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_OPENED ||
                m_pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_UPDATED)            
            {
                Assert( m_pLogonTask->hCombinedTicketCache != NULL );
                Assert( pGetCombinedTicketTask->pTGTContext != NULL );
                
                RtlZeroMemory(&pGetCombinedTicketTask->cacheContext, sizeof(pGetCombinedTicketTask->cacheContext));
                pGetCombinedTicketTask->cacheContext.dwRecordBufferSize = sizeof(XKERB_TGT_CONTEXT) - FIELD_OFFSET(XKERB_TGT_CONTEXT, ClientName);
                pGetCombinedTicketTask->cacheContext.pbRecordBuffer = (BYTE*)&pGetCombinedTicketTask->pTGTContext->ClientName;
                pGetCombinedTicketTask->cacheContext.pbIndexData = (BYTE*)pGetCombinedTicketTask->pTGTContext;

                hr = CacheLookup(m_pLogonTask->hCombinedTicketCache, &pGetCombinedTicketTask->cacheContext);

                if (FAILED(hr) || !pGetCombinedTicketTask->cacheContext.bValidDataInCache)
                {
                    TraceSz3( AuthVerbose, "GetCombinedTicketContinue cache miss 0x%X %d (%8X)", hr, pGetCombinedTicketTask->cacheContext.bValidDataInCache, XConfigChecksum(pGetCombinedTicketTask->cacheContext.pbIndexData, FIELD_OFFSET(XKERB_TGT_CONTEXT, StartTime)) );
                    pGetCombinedTicketTask->dwState = LOGON_START_CACHE_MISS;
                    fMoreWorkToDo = TRUE;
                    break;
                }

                GetSystemTimeAsFileTime((LPFILETIME)&CurrentTime);
                if (pGetCombinedTicketTask->pTGTContext->ExpirationTime.QuadPart < CurrentTime.QuadPart)
                {
                    //
                    // Ticket has expired, nuke the entry and take cache miss path
                    //
                    TraceSz2( AuthVerbose, "GetCombinedTicketContinue cache miss expired ticket 0x%016I64X < 0x%016I64X", pGetCombinedTicketTask->pTGTContext->ExpirationTime.QuadPart, CurrentTime.QuadPart );
                    CacheDelete( m_pLogonTask->hCombinedTicketCache, &pGetCombinedTicketTask->cacheContext );
                    pGetCombinedTicketTask->dwState = LOGON_START_CACHE_MISS;
                    fMoreWorkToDo = TRUE;
                    break;
                }

                if (pGetCombinedTicketTask->cacheContext.bCallRetrieveNext)
                {
                    hr = CacheRetrieve( m_pLogonTask->hCombinedTicketCache, m_pLogonTask->hCombinedTicketCacheTask, &pGetCombinedTicketTask->cacheContext );
                    if (FAILED(hr))
                    {
                        pGetCombinedTicketTask->dwState = LOGON_START_CACHE_MISS;
                        fMoreWorkToDo = TRUE;
                        break;
                    }
                    TraceSz1( AuthVerbose, "GetCombinedTicketContinue cache hit using retrieve (%8X)", XConfigChecksum(pGetCombinedTicketTask->cacheContext.pbIndexData, FIELD_OFFSET(XKERB_TGT_CONTEXT, StartTime)) );
                    pGetCombinedTicketTask->dwState = LOGON_CACHE_RETRIEVE_IN_PROGRESS;
                    fMoreWorkToDo = TRUE;
                    break;
                }
                else
                {
                    TraceSz1( AuthVerbose, "GetCombinedTicketContinue cache hit from MRU (%8X)", XConfigChecksum(pGetCombinedTicketTask->cacheContext.pbIndexData, FIELD_OFFSET(XKERB_TGT_CONTEXT, StartTime)) );
                    pGetCombinedTicketTask->fTicketFromCache = TRUE;
                    pGetCombinedTicketTask->hr = XONLINETASK_S_SUCCESS;
                    break;
                }
            }
            else
            {
                pGetCombinedTicketTask->dwState = LOGON_START_CACHE_MISS;
                fMoreWorkToDo = TRUE;
                break;
            }
            break;
            
        case LOGON_CACHE_RETRIEVE_IN_PROGRESS:
            hr = XOnlineTaskContinue( m_pLogonTask->hCombinedTicketCacheTask );
            if ( hr == XONLINETASK_S_SUCCESS )
            {
                pGetCombinedTicketTask->fTicketFromCache = TRUE;
                pGetCombinedTicketTask->hr = XONLINETASK_S_SUCCESS;
                m_pLogonTask->fNeedToFlushCombinedTicketCache = TRUE;
                break;
            }
            else if (FAILED(hr))
            {
                pGetCombinedTicketTask->dwState = LOGON_START_CACHE_MISS;
                fMoreWorkToDo = TRUE;
                break;
            }
            break;

        case LOGON_START_CACHE_MISS:
            if (pGetCombinedTicketTask->pTGTContext != NULL)
            {
                SysFree( pGetCombinedTicketTask->pTGTContext );
                pGetCombinedTicketTask->pTGTContext = NULL;
            }

            if (m_dwUniqueLogonUsers > 0)
            {
                hr = GetSingleTicket( FALSE, NULL, m_apUniqueLogonUsers[pGetCombinedTicketTask->dwCurrentUniqueUser], pGetCombinedTicketTask->xonTask.hEventWorkAvailable, &pGetCombinedTicketTask->hGetTGTTask );
                if ( FAILED(hr) )
                {
                    pGetCombinedTicketTask->hr = hr;
                    goto Cleanup;
                }                    
            }
            else
            {
                fMoreWorkToDo = TRUE;
            }
            pGetCombinedTicketTask->dwState = LOGON_MISC_IN_PROGRESS;
            break;

        case LOGON_MISC_IN_PROGRESS:
            if (pGetCombinedTicketTask->hGetTGTTask != NULL)
            {
                hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)pGetCombinedTicketTask->hGetTGTTask );
                if ( hr == XONLINETASK_S_SUCCESS )
                {
                    Assert( pGetCombinedTicketTask->pTGTContext == NULL );
                    hr = GetSingleTicketGetResults( (XONLINETASK_HANDLE)pGetCombinedTicketTask->hGetTGTTask, &pGetCombinedTicketTask->pTGTContext );
                    Assert( hr == XONLINETASK_S_SUCCESS );
                    Assert(pGetCombinedTicketTask->hGetTGTTask != NULL);
                    XOnlineTaskClose(pGetCombinedTicketTask->hGetTGTTask);
                    pGetCombinedTicketTask->hGetTGTTask = NULL;
                    pGetCombinedTicketTask->dwCurrentUniqueUser++;
                    if (pGetCombinedTicketTask->dwCurrentUniqueUser < m_dwUniqueLogonUsers)
                    {
                        Assert(pGetCombinedTicketTask->pTGTContext != NULL);
                        hr = GetSingleTicket( FALSE, pGetCombinedTicketTask->pTGTContext, m_apUniqueLogonUsers[pGetCombinedTicketTask->dwCurrentUniqueUser], pGetCombinedTicketTask->xonTask.hEventWorkAvailable, &pGetCombinedTicketTask->hGetTGTTask );
                        if ( FAILED(hr) )
                        {
                            pGetCombinedTicketTask->hr = hr;
                            goto Cleanup;
                        }                    
                        pGetCombinedTicketTask->pTGTContext = NULL;
                    }
                }
                else if (FAILED(hr))
                {
                    if (pGetCombinedTicketTask->dwCurrentUniqueUser > m_dwUniqueLogonUsers && hr == XONLINE_E_LOGON_AUTHENTICATION_FAILED)
                    {
                        //
                        // Mark it as such so that the logon task knows to retry via creating a new machine account
                        //
                        hr = XONLINE_E_LOGON_MACHINE_AUTHENTICATION_FAILED;
                    }
                    
                    pGetCombinedTicketTask->hr = hr;
                    goto Cleanup;
                }
                else
                {
                    Assert( hr == XONLINETASK_S_RUNNING );
                }
            }

            if (pGetCombinedTicketTask->hGetTGTTask == NULL)
            {
                Assert( pGetCombinedTicketTask->dwCurrentUniqueUser >= m_dwUniqueLogonUsers );
                if (pGetCombinedTicketTask->dwCurrentUniqueUser == m_dwUniqueLogonUsers)
                {
                    //
                    // Got a combined ticket with all users, now make the final machine account cross-realm request
                    //
                    hr = GetSingleTicket( TRUE, pGetCombinedTicketTask->pTGTContext, pGetCombinedTicketTask->pMachineUser, pGetCombinedTicketTask->xonTask.hEventWorkAvailable, &pGetCombinedTicketTask->hGetTGTTask );
                    if ( FAILED(hr) )
                    {
                        pGetCombinedTicketTask->hr = hr;
                        goto Cleanup;
                    }                    
                    pGetCombinedTicketTask->pTGTContext = NULL;
                    pGetCombinedTicketTask->dwCurrentUniqueUser++;
                }
                else
                {
                    //
                    // Got the final combined ticket
                    //
                    Assert(pGetCombinedTicketTask->pTGTContext != NULL);
                    pGetCombinedTicketTask->dwState = LOGON_TASK_COMPLETE;
                    pGetCombinedTicketTask->hr = XONLINETASK_S_SUCCESS;
                }
            }
            break;
        
        case LOGON_TASK_COMPLETE:
            break;
            
        default:
            AssertSz(FALSE, "GetCombinedTicketContinue: Bad State");
        }
    } while (fMoreWorkToDo);
    
Cleanup:
    if (pGetCombinedTicketTask->hr != XONLINETASK_S_RUNNING)
    {
        pGetCombinedTicketTask->dwState = LOGON_TASK_COMPLETE;
    }
    
    return(pGetCombinedTicketTask->hr);
}

//
// Close function for GetCombinedTicket
//
void CXo::GetCombinedTicketClose(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    DWORD i;
    PXONLINETASK_GET_COMBINED_TICKET pGetCombinedTicketTask = (PXONLINETASK_GET_COMBINED_TICKET)hTask;

    if (pGetCombinedTicketTask->hGetTGTTask != NULL)
    {
        XOnlineTaskClose( pGetCombinedTicketTask->hGetTGTTask );
    }
    
    if (pGetCombinedTicketTask->pTGTContext != NULL)
    {
        SysFree( pGetCombinedTicketTask->pTGTContext );
    }

    SysFree(pGetCombinedTicketTask);
}

//
// Function to kick off a GetCombinedTicket
//
HRESULT CXo::GetCombinedTicket(PXONLINE_USER pMachineUser, BOOL fUseCache, HANDLE hWorkEvent, PXONLINETASK_HANDLE phGetCombinedTicketTask)
{
    DWORD i;
    HRESULT hr;
    PXONLINETASK_GET_COMBINED_TICKET pGetCombinedTicketTask;
    PXONLINE_USER pTempUser;
    
    Assert(pMachineUser != NULL);
    Assert(phGetCombinedTicketTask != NULL);

    *phGetCombinedTicketTask = NULL;
    pGetCombinedTicketTask = (PXONLINETASK_GET_COMBINED_TICKET)SysAllocZ(sizeof(*pGetCombinedTicketTask), PTAG_XONLINETASK_GET_COMBINED_TICKET);
    if (pGetCombinedTicketTask == NULL)
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }
    
    TaskInitializeContext(&(pGetCombinedTicketTask->xonTask));
    pGetCombinedTicketTask->xonTask.pfnContinue = GetCombinedTicketContinue;
    pGetCombinedTicketTask->xonTask.pfnClose = GetCombinedTicketClose;
    pGetCombinedTicketTask->xonTask.hEventWorkAvailable = hWorkEvent;
    pGetCombinedTicketTask->hr = XONLINETASK_S_RUNNING;
    pGetCombinedTicketTask->dwState = LOGON_MISC_IN_PROGRESS;
    pGetCombinedTicketTask->pMachineUser = pMachineUser;

    if (fUseCache)
    {
        pGetCombinedTicketTask->pTGTContext = (PXKERB_TGT_CONTEXT)SysAllocZ(sizeof(*(pGetCombinedTicketTask->pTGTContext)), PTAG_XKERB_TGT_CONTEXT);
        if ( pGetCombinedTicketTask->pTGTContext == NULL )
        {
            hr = XONLINE_E_OUT_OF_MEMORY;
            goto Cleanup;
        }

        for (i=0; i<m_dwUniqueLogonUsers; ++i)
        {
            pGetCombinedTicketTask->pTGTContext->UserID[i] = m_apUniqueLogonUsers[i]->xuid.qwUserID;
        }

        pGetCombinedTicketTask->dwState = LOGON_START_CACHE_LOOKUP;
    }
    else
    {
        pGetCombinedTicketTask->dwState = LOGON_START_CACHE_MISS;
    }
    
    GetCombinedTicketContinue( (XONLINETASK_HANDLE)pGetCombinedTicketTask );
    
    *phGetCombinedTicketTask = (XONLINETASK_HANDLE)pGetCombinedTicketTask;

	hr = S_OK;

Cleanup:
    if (FAILED(hr))
    {
        *phGetCombinedTicketTask = NULL;
        if (pGetCombinedTicketTask != NULL)
        {
            if (pGetCombinedTicketTask->hGetTGTTask != NULL)
            {
                XOnlineTaskClose( pGetCombinedTicketTask->hGetTGTTask );
            }

            SysFree(pGetCombinedTicketTask);
        }
    }
    
    return(hr);
}


//
// Function to retrieve the results of a GetCombinedTicket
//
HRESULT CXo::GetCombinedTicketGetResults(XONLINETASK_HANDLE hTask, PXKERB_TGT_CONTEXT * ppCombinedTGTContext, BOOL* pfTicketFromCache)
{
    Assert(hTask != NULL);
    Assert(ppCombinedTGTContext != NULL);
    Assert(pfTicketFromCache != NULL);

    PXONLINETASK_GET_COMBINED_TICKET   pGetCombinedTicketTask = (PXONLINETASK_GET_COMBINED_TICKET)hTask;

    if (pGetCombinedTicketTask->hr == XONLINETASK_S_SUCCESS)
    {
        *pfTicketFromCache = pGetCombinedTicketTask->fTicketFromCache;
        *ppCombinedTGTContext = pGetCombinedTicketTask->pTGTContext;
        pGetCombinedTicketTask->pTGTContext = NULL;
    }
    else
    {
        *ppCombinedTGTContext = NULL;
    }
    return(pGetCombinedTicketTask->hr);
}


//
// Define a task context structure for
//
struct XONLINETASK_GET_SERVICE_TICKET
{
    XONLINETASK_CONTEXT     xonTask;
    HRESULT                 hr;
    DWORD                   dwState;
    DWORD                   dwAdjustedTime;
    DWORD                   dwRetryCount;
    DWORD                   dwNextRetryTickCount;
    BYTE                    abEventTimer[CBEVENTTIMER];
    PXKERB_TGT_CONTEXT      pCombinedTGTContext;
    XONLINETASK_DNS_LOOKUP  DNSTask;
    HRESULT                 hrDNS;
    XNDNS*                  pDestAddress;
    SOCKET                  socket;
    WSABUF                  WSABuffer;
    XONLINETASK_SOCKIO      SendRecvTask; // No XOnlineCloseTask needed
    BYTE                    recvBuffer[XONLINE_TGS_EXCHANGE_BUFFER_SIZE];
    XONLC_CONTEXT           cacheContext;
    BOOL                    fTicketFromCache;
    PXKERB_SERVICE_CONTEXT  pXKerbServiceContext;
};

//
// Send TGS Requests for GetServiceTicket
//
HRESULT CXo::GetServiceTicketSendTgsRequest(PXONLINETASK_GET_SERVICE_TICKET pGetServiceTicketTask)
{
    int err;
    HRESULT hr = S_OK;
    WSABUF WSABuffer;
    WSAOVERLAPPED wsaOverlapped;
    struct sockaddr_in ServerAddress;
    BYTE abBuffer[XONLINE_AS_EXCHANGE_BUFFER_SIZE];
    DWORD dwMessageLength = sizeof(abBuffer);

    Assert(pGetServiceTicketTask->pXKerbServiceContext != NULL);
    
    hr = XkerbBuildTgsRequest(
        pGetServiceTicketTask->pCombinedTGTContext,
        abBuffer,
        &dwMessageLength,
        pGetServiceTicketTask->pXKerbServiceContext
        );
    if (FAILED(hr))
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr = pGetServiceTicketTask->pDestAddress->aina[pGetServiceTicketTask->dwRetryCount % pGetServiceTicketTask->pDestAddress->cina];
    ServerAddress.sin_port = htons(KERB_KDC_PORT);

    WSABuffer.len = dwMessageLength;
    WSABuffer.buf = (char*)abBuffer;
    wsaOverlapped.hEvent = pGetServiceTicketTask->xonTask.hEventWorkAvailable;
    
    TraceSz5( AuthVerbose, "GetServiceTicketSendTgsRequest send %d bytes to: %d.%d.%d.%d",
        dwMessageLength,
        pGetServiceTicketTask->pDestAddress->aina[pGetServiceTicketTask->dwRetryCount % pGetServiceTicketTask->pDestAddress->cina].S_un.S_un_b.s_b1, 
        pGetServiceTicketTask->pDestAddress->aina[pGetServiceTicketTask->dwRetryCount % pGetServiceTicketTask->pDestAddress->cina].S_un.S_un_b.s_b2, 
        pGetServiceTicketTask->pDestAddress->aina[pGetServiceTicketTask->dwRetryCount % pGetServiceTicketTask->pDestAddress->cina].S_un.S_un_b.s_b3, 
        pGetServiceTicketTask->pDestAddress->aina[pGetServiceTicketTask->dwRetryCount % pGetServiceTicketTask->pDestAddress->cina].S_un.S_un_b.s_b4); 

    err = WSASendTo(
            pGetServiceTicketTask->socket,
            &WSABuffer, 1,
            &dwMessageLength, 0,
            (LPSOCKADDR) &ServerAddress, sizeof(ServerAddress),
            &wsaOverlapped, NULL);
    if (err == SOCKET_ERROR)
    {
        TraceSz1( AuthWarn, "GetServiceTicketSendTgsRequest: WSASendTo Failed WSAGetLastError:0x%X", WSAGetLastError() );
        hr = XONLINE_E_LOGON_AUTHORIZATION_FAILED;
        goto Cleanup;
    }

    pGetServiceTicketTask->dwNextRetryTickCount =
        GetTickCount() + g_dwKerbRetryDelayInMilliSeconds[pGetServiceTicketTask->dwRetryCount];

    XnSetEventTimer(
        pGetServiceTicketTask->abEventTimer,
        pGetServiceTicketTask->xonTask.hEventWorkAvailable,
        g_dwKerbRetryDelayInMilliSeconds[pGetServiceTicketTask->dwRetryCount] );
    
Cleanup:
    return hr;
}


//
// Do work function for GetServiceTicket
//
HRESULT CXo::GetServiceTicketContinue(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    HRESULT hr;
    DWORD dwMessageLength;
    XOKERBINFO* pXoKerbInfo;
    TimeStamp CurrentTime;
    BYTE abUserPerm[XONLINE_MAX_LOGON_USERS];
    PXONLINETASK_GET_SERVICE_TICKET pGetServiceTicketTask = (PXONLINETASK_GET_SERVICE_TICKET)hTask;
    BOOL fMoreWorkToDo;

    if (DNSLookupIsValidTask((XONLINETASK_HANDLE)&pGetServiceTicketTask->DNSTask))
    {
        pGetServiceTicketTask->hrDNS = XOnlineTaskContinue( (XONLINETASK_HANDLE)&pGetServiceTicketTask->DNSTask );
        if ( pGetServiceTicketTask->hrDNS == XONLINETASK_S_SUCCESS )
        {
            hr = DNSLookupGetResults( (XONLINETASK_HANDLE)&pGetServiceTicketTask->DNSTask, &pGetServiceTicketTask->pDestAddress );
            Assert( hr == XONLINETASK_S_SUCCESS );
        }
    }

    do
    {
        fMoreWorkToDo = FALSE;
        
        switch (pGetServiceTicketTask->dwState)
        {
        case LOGON_START_CACHE_LOOKUP:
            if (m_pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_OPENING)
            {    
                break;
            }

            if (m_pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_OPENED ||
                m_pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_UPDATED)
            {
                Assert( m_pLogonTask->hServiceTicketCache != NULL );
                Assert( pGetServiceTicketTask->pXKerbServiceContext != NULL );
                
                RtlZeroMemory(&pGetServiceTicketTask->cacheContext, sizeof(pGetServiceTicketTask->cacheContext));
                pGetServiceTicketTask->cacheContext.dwRecordBufferSize = sizeof(XKERB_SERVICE_CONTEXT) - FIELD_OFFSET(XKERB_SERVICE_CONTEXT, siteIPAddress);
                pGetServiceTicketTask->cacheContext.pbRecordBuffer = (BYTE*)&pGetServiceTicketTask->pXKerbServiceContext->siteIPAddress;
                pGetServiceTicketTask->cacheContext.pbIndexData = (BYTE*)pGetServiceTicketTask->pXKerbServiceContext;

                hr = CacheLookup(m_pLogonTask->hServiceTicketCache, &pGetServiceTicketTask->cacheContext);

                if (FAILED(hr) || !pGetServiceTicketTask->cacheContext.bValidDataInCache)
                {
                    TraceSz3( AuthVerbose, "GetServiceTicketContinue cache miss 0x%X %d (%8X)", hr, pGetServiceTicketTask->cacheContext.bValidDataInCache, XConfigChecksum(pGetServiceTicketTask->cacheContext.pbIndexData, FIELD_OFFSET(XKERB_SERVICE_CONTEXT, StartTime)) );
                    pGetServiceTicketTask->dwState = LOGON_START_CACHE_MISS;
                    fMoreWorkToDo = TRUE;
                    break;
                }

                GetSystemTimeAsFileTime((LPFILETIME)&CurrentTime);
                if (pGetServiceTicketTask->pXKerbServiceContext->ExpirationTime.QuadPart < CurrentTime.QuadPart)
                {
                    //
                    // Ticket has expired, nuke the entry and take cache miss path
                    //
                    TraceSz2( AuthVerbose, "GetServiceTicketContinue cache miss expired ticket 0x%016I64X < 0x%016I64X", pGetServiceTicketTask->pXKerbServiceContext->ExpirationTime.QuadPart, CurrentTime.QuadPart );
                    CacheDelete( m_pLogonTask->hServiceTicketCache, &pGetServiceTicketTask->cacheContext );
                    pGetServiceTicketTask->dwState = LOGON_START_CACHE_MISS;
                    fMoreWorkToDo = TRUE;
                    break;
                }

                if (pGetServiceTicketTask->cacheContext.bCallRetrieveNext)
                {
                    hr = CacheRetrieve( m_pLogonTask->hServiceTicketCache, m_pLogonTask->hServiceTicketCacheTask, &pGetServiceTicketTask->cacheContext );
                    if (FAILED(hr))
                    {
                        pGetServiceTicketTask->dwState = LOGON_START_CACHE_MISS;
                        fMoreWorkToDo = TRUE;
                        break;
                    }
                    TraceSz1( AuthVerbose, "GetServiceTicketContinue cache hit using retrieve (%8X)", XConfigChecksum(pGetServiceTicketTask->cacheContext.pbIndexData, FIELD_OFFSET(XKERB_SERVICE_CONTEXT, StartTime)) );
                    pGetServiceTicketTask->dwState = LOGON_CACHE_RETRIEVE_IN_PROGRESS;
                    fMoreWorkToDo = TRUE;
                    break;
                }
                else
                {
                    TraceSz1( AuthVerbose, "GetServiceTicketContinue cache hit from MRU (%8X)", XConfigChecksum(pGetServiceTicketTask->cacheContext.pbIndexData, FIELD_OFFSET(XKERB_SERVICE_CONTEXT, StartTime)) );
                    pGetServiceTicketTask->fTicketFromCache = TRUE;
                    pGetServiceTicketTask->hr = XONLINETASK_S_SUCCESS;
                    break;
                }
            }
            else
            {
                pGetServiceTicketTask->dwState = LOGON_START_CACHE_MISS;
                fMoreWorkToDo = TRUE;
                break;
            }
            break;
            
        case LOGON_CACHE_RETRIEVE_IN_PROGRESS:
            hr = XOnlineTaskContinue( m_pLogonTask->hServiceTicketCacheTask );
            if ( hr == XONLINETASK_S_SUCCESS )
            {
                pGetServiceTicketTask->fTicketFromCache = TRUE;
                pGetServiceTicketTask->hr = XONLINETASK_S_SUCCESS;
                m_pLogonTask->fNeedToFlushServiceTicketCache = TRUE;
                break;
            }
            else if (FAILED(hr))
            {
                pGetServiceTicketTask->dwState = LOGON_START_CACHE_MISS;
                fMoreWorkToDo = TRUE;
                break;
            }
            break;

        case LOGON_START_CACHE_MISS:
            if (FAILED(pGetServiceTicketTask->hrDNS))
            {
                pGetServiceTicketTask->hr = pGetServiceTicketTask->hrDNS;
                goto Cleanup;
            }
            else if ( pGetServiceTicketTask->hrDNS != XONLINETASK_S_SUCCESS )
            {
                //
                // Still running, need to wait
                //
                break;
            }
            
            Assert( pGetServiceTicketTask->pCombinedTGTContext != NULL );
            
            if (pGetServiceTicketTask->socket == INVALID_SOCKET)
            {
                pGetServiceTicketTask->socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
                if (pGetServiceTicketTask->socket == INVALID_SOCKET)
                {
                    TraceSz1( AuthWarn, "GetServiceTicketContinue: socket create failed WSAGetLastError:0x%X", WSAGetLastError() );
                    pGetServiceTicketTask->hr = XONLINE_E_LOGON_AUTHORIZATION_FAILED;
                    goto Cleanup;
                }

                XnSetInsecure(pGetServiceTicketTask->socket);

                pGetServiceTicketTask->WSABuffer.len = sizeof(pGetServiceTicketTask->recvBuffer);
                pGetServiceTicketTask->WSABuffer.buf = (char*)pGetServiceTicketTask->recvBuffer;

                SendRecvInitializeContext(
                        pGetServiceTicketTask->socket,
                        &pGetServiceTicketTask->WSABuffer, 1, 0,
                        pGetServiceTicketTask->xonTask.hEventWorkAvailable,
                        &pGetServiceTicketTask->SendRecvTask );
            }
            
            hr = GetServiceTicketSendTgsRequest( pGetServiceTicketTask );
            if (FAILED(hr))
            {
                pGetServiceTicketTask->hr = hr;
                goto Cleanup;
            }
            
            //
            // Kick off the asynchronous recv operation
            //
            hr = RecvInternal(&pGetServiceTicketTask->SendRecvTask);
            if (FAILED(hr))
            {
                TraceSz1( AuthWarn, "GetServiceTicketContinue: RecvInternal failed hr:0x%X", hr );
                pGetServiceTicketTask->hr = XONLINE_E_LOGON_AUTHORIZATION_FAILED;
                goto Cleanup;
            }

            pGetServiceTicketTask->dwState = LOGON_RECV_IN_PROGRESS;
            break;

        case LOGON_RECV_IN_PROGRESS:
            hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)&pGetServiceTicketTask->SendRecvTask );
            if (hr == XONLINETASK_S_SUCCESS)
            {
                dwMessageLength = 0;
                hr = GetSendRecvResults( (XONLINETASK_HANDLE)&pGetServiceTicketTask->SendRecvTask, &dwMessageLength, NULL, NULL );
                Assert( hr == XONLINETASK_S_SUCCESS );
                Assert( dwMessageLength > 0 );
                
                TraceSz1( AuthVerbose, "GetServiceTicketContinue received %d bytes!", dwMessageLength );

                hr = XkerbVerifyTgsReply(
                    (PXONLINEP_USER)m_rgLogonUsers,
                    pGetServiceTicketTask->pCombinedTGTContext,
                    pGetServiceTicketTask->recvBuffer,
                    dwMessageLength,
                    pGetServiceTicketTask->pXKerbServiceContext
                    );
                if ( hr == (HRESULT) STATUS_TIME_DIFFERENCE_AT_DC )
                {
                    TraceSz( AuthWarn, "GetServiceTicketContinue adjusted Xbox clock according to KDC!");
                    if (pGetServiceTicketTask->dwAdjustedTime < MAX_TIME_ADJUSTMENTS)
                    {
                        ++pGetServiceTicketTask->dwAdjustedTime;

                        pGetServiceTicketTask->dwState = LOGON_START_CACHE_MISS;
                        fMoreWorkToDo = TRUE;
                        break;
                    }
                    else
                    {
                        TraceSz1( AuthWarn, "GetServiceTicketContinue: adjusting time too many times: %d", pGetServiceTicketTask->dwAdjustedTime );
                        pGetServiceTicketTask->hr = XONLINE_E_LOGON_AUTHORIZATION_FAILED;
                        goto Cleanup;
                    }
                }
                else if (FAILED(hr))
                {
                    TraceSz1( AuthWarn, "GetServiceTicketContinue: XkerbVerifyTgsReply failed with hr: 0x%X", hr );
                    pGetServiceTicketTask->hr = XONLINE_E_LOGON_AUTHORIZATION_FAILED;
                    goto Cleanup;
                }

    #ifdef XNET_FEATURE_TRACE
                {
                    char buffer1[256],buf[32];
                    DWORD k;
                    buffer1[0] = '\0';
                    for (k=0; k<pGetServiceTicketTask->pXKerbServiceContext->dwNumServices; ++k)
                    {
                        sprintf(buf," %d(%X)", pGetServiceTicketTask->pXKerbServiceContext->dwServiceID[k], pGetServiceTicketTask->pXKerbServiceContext->hrService[k]);
                        strcat(buffer1, buf);
                    }
                    TraceSz2( AuthVerbose, "GetServiceTicketContinue XKDC response hr=0x%X Service: %s", pGetServiceTicketTask->pXKerbServiceContext->hr, buffer1 );
                }
    #endif

                //
                // Check the master hr returned from the server
                //
                if (FAILED(pGetServiceTicketTask->pXKerbServiceContext->hr))
                {
                    pGetServiceTicketTask->hr = pGetServiceTicketTask->pXKerbServiceContext->hr;
                    goto Cleanup;
                }

                pGetServiceTicketTask->hr = XONLINETASK_S_SUCCESS;
                break;                
            }
            else if (FAILED(hr))
            {
                TraceSz1( AuthWarn, "GetServiceTicketContinue: recv failed with hr: 0x%X", hr );
                pGetServiceTicketTask->hr = XONLINE_E_LOGON_AUTHORIZATION_FAILED;
                goto Cleanup;
            }

            Assert( hr == XONLINETASK_S_RUNNING );
            //
            // Check if we timed out
            //
            if (GetTickCount() >= pGetServiceTicketTask->dwNextRetryTickCount)
            {
                if ( ++pGetServiceTicketTask->dwRetryCount >= g_dwNumKerbRetry )
                {
                    TraceSz1( AuthWarn, "GetServiceTicketContinue: timed out after %d retries", pGetServiceTicketTask->dwRetryCount );
                    pGetServiceTicketTask->hr = XONLINE_E_LOGON_AUTHORIZATION_TIMEDOUT;
                    goto Cleanup;
                }
                
                //
                // Need to retry with the next IP address
                //
                hr = GetServiceTicketSendTgsRequest( pGetServiceTicketTask );
                if (FAILED(hr))
                {
                    pGetServiceTicketTask->hr = hr;
                    goto Cleanup;
                }
            }
            break;
            
        case LOGON_TASK_COMPLETE:
            break;
            
        default:
            AssertSz(FALSE, "GetServiceTicketContinue: Bad State");
        }
    } while (fMoreWorkToDo);
    
Cleanup:
    if (pGetServiceTicketTask->hr != XONLINETASK_S_RUNNING)
    {
        pGetServiceTicketTask->dwState = LOGON_TASK_COMPLETE;
    }
    
    return(pGetServiceTicketTask->hr);
}

//
// Close function for GetServiceTicket
//
void CXo::GetServiceTicketClose(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    PXONLINETASK_GET_SERVICE_TICKET pGetServiceTicketTask = (PXONLINETASK_GET_SERVICE_TICKET)hTask;
    
    if (DNSLookupIsValidTask( (XONLINETASK_HANDLE)&(pGetServiceTicketTask->DNSTask) ))
    {
        XOnlineTaskClose((XONLINETASK_HANDLE)&(pGetServiceTicketTask->DNSTask));
    }

    if (pGetServiceTicketTask->socket != INVALID_SOCKET)
    {
        WSACancelOverlappedIO( pGetServiceTicketTask->socket );
        closesocket(pGetServiceTicketTask->socket);
    }

    if (pGetServiceTicketTask->pXKerbServiceContext != NULL)
    {
        SysFree( pGetServiceTicketTask->pXKerbServiceContext );
    }

    XnSetEventTimer(pGetServiceTicketTask->abEventTimer, NULL, 0xFFFFFFFF);

    SysFree(pGetServiceTicketTask);
}

//
// Function to kick off a GetServiceTicket
//
HRESULT CXo::GetServiceTicket(
            PXKERB_TGT_CONTEXT pCombinedTGTContext,
            DWORD* pdwServiceIDs,
            DWORD cServices,
            BOOL fUseCache,
            HANDLE hWorkEvent,
            PXONLINETASK_HANDLE phGetServiceTicketTask
            )
{
    HRESULT hr;
    DWORD i;
    PXONLINETASK_GET_SERVICE_TICKET pGetServiceTicketTask;
    
    Assert(phGetServiceTicketTask != NULL);

    pGetServiceTicketTask = (PXONLINETASK_GET_SERVICE_TICKET)SysAllocZ(sizeof(*pGetServiceTicketTask),
        PTAG_XONLINETASK_GET_SERVICE_TICKET);
    if (pGetServiceTicketTask == NULL)
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }
    
    TaskInitializeContext(&(pGetServiceTicketTask->xonTask));
    pGetServiceTicketTask->xonTask.pfnContinue = GetServiceTicketContinue;
    pGetServiceTicketTask->xonTask.pfnClose = GetServiceTicketClose;
    pGetServiceTicketTask->xonTask.hEventWorkAvailable = hWorkEvent;
    pGetServiceTicketTask->hr = XONLINETASK_S_RUNNING;
    pGetServiceTicketTask->socket = INVALID_SOCKET;
    pGetServiceTicketTask->pCombinedTGTContext = pCombinedTGTContext;
    
    pGetServiceTicketTask->pXKerbServiceContext = (PXKERB_SERVICE_CONTEXT)SysAllocZ(sizeof(*(pGetServiceTicketTask->pXKerbServiceContext)), PTAG_XKERB_SERVICE_CONTEXT);
    if ( pGetServiceTicketTask->pXKerbServiceContext == NULL )
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }

    for (i=0; i<m_dwUniqueLogonUsers; ++i)
    {
        pGetServiceTicketTask->pXKerbServiceContext->UserID[i] = m_apUniqueLogonUsers[i]->xuid.qwUserID;
    }
    pGetServiceTicketTask->pXKerbServiceContext->dwTitleID = m_dwTitleId;
    
    for (i=0; i<cServices; ++i)
    {
        pGetServiceTicketTask->pXKerbServiceContext->dwServiceID[i] = pdwServiceIDs[i];
    }
    pGetServiceTicketTask->pXKerbServiceContext->dwNumServices = cServices;

    hr = S_OK;

    pGetServiceTicketTask->hrDNS = DNSLookup( XBOX_KDC_DNS, hWorkEvent, &pGetServiceTicketTask->DNSTask );

    if (fUseCache)
    {
        pGetServiceTicketTask->dwState = LOGON_START_CACHE_LOOKUP;
    }
    else
    {
        pGetServiceTicketTask->dwState = LOGON_START_CACHE_MISS;
    }
    
    GetServiceTicketContinue( (XONLINETASK_HANDLE) pGetServiceTicketTask );
    
    *phGetServiceTicketTask = (XONLINETASK_HANDLE) pGetServiceTicketTask;

Cleanup:
    if (FAILED(hr))
    {
        *phGetServiceTicketTask = NULL;

        if (pGetServiceTicketTask != NULL)
        {
            if (DNSLookupIsValidTask( (XONLINETASK_HANDLE)&(pGetServiceTicketTask->DNSTask) ))
            {
                XOnlineTaskClose((XONLINETASK_HANDLE)&(pGetServiceTicketTask->DNSTask));
            }

            SysFree(pGetServiceTicketTask);
        }
    }    
    return(hr);
}

//
// Function to retrieve the results of a GetServiceTicket
//
HRESULT CXo::GetServiceTicketGetResults(XONLINETASK_HANDLE hTask, PXKERB_SERVICE_CONTEXT * ppXKerbServiceContext, BOOL* pfTicketFromCache)
{
    Assert(hTask != NULL);
    Assert(ppXKerbServiceContext != NULL);
    Assert(pfTicketFromCache != NULL);

    PXONLINETASK_GET_SERVICE_TICKET  pGetServiceTicketTask = (PXONLINETASK_GET_SERVICE_TICKET)hTask;

    if (pGetServiceTicketTask->hr == XONLINETASK_S_SUCCESS)
    {
        *pfTicketFromCache = pGetServiceTicketTask->fTicketFromCache;
        *ppXKerbServiceContext = pGetServiceTicketTask->pXKerbServiceContext;
        pGetServiceTicketTask->pXKerbServiceContext = NULL;
    }
    else
    {
        *ppXKerbServiceContext = NULL;
    }
    return(pGetServiceTicketTask->hr);
}

//
// Main do work function for Logon
//
HRESULT CXo::LogonTaskContinue(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

    HRESULT hr;
    XNADDR xna;
    DWORD dwStatus;
    DWORD i,j;
    XOKERBINFO* pXoKerbInfo;
    BYTE abUserPerm[XONLINE_MAX_LOGON_USERS];
    IN_ADDR serviceAddr;
    XONLINE_SERVICE_INFO serviceInfo;
    XONLINE_RECENT_LOGON_TIME recentLogonTime;
    PXONLINETASK_LOGON pLogonTask = (PXONLINETASK_LOGON)hTask;
    BOOL fTicketFromCache;
    BOOL fSomeServicesFailed;
    BOOL fMoreWorkToDo;

    //
    // Always perform any remaining service ticket cache pumping
    //
    hr = S_OK;
    if (pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_OPENING ||
        pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_UPDATING ||
        pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_FLUSHING )
    {
        hr = XOnlineTaskContinue( pLogonTask->hServiceTicketCacheTask );
        if (hr == XONLINETASK_S_SUCCESS)
        {
            if (pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_OPENING)
            {
                pLogonTask->dwServiceTicketCacheState = XON_CACHE_STATE_OPENED;
            }
            else if (pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_UPDATING)
            {
                pLogonTask->dwServiceTicketCacheState = XON_CACHE_STATE_UPDATED;
            }
            else if (pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_FLUSHING)
            {
                TraceSz( AuthVerbose, "LogonTaskContinue: service ticket cache successfully flushed" );
                pLogonTask->dwServiceTicketCacheState = XON_CACHE_STATE_DONE;
            }
        }
    }
    if (pLogonTask->fNeedToFlushServiceTicketCache &&
        pLogonTask->dwState >= LOGON_CS_IN_PROGRESS &&
        (pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_OPENED ||
        pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_UPDATED))
    {
        hr = CacheIndexFlush( pLogonTask->hServiceTicketCache, pLogonTask->hServiceTicketCacheTask );
        if (SUCCEEDED(hr))
        {
            pLogonTask->dwServiceTicketCacheState = XON_CACHE_STATE_FLUSHING;            
        }
        pLogonTask->fNeedToFlushServiceTicketCache = FALSE;
    }
    if (FAILED(hr))
    {
        if (pLogonTask->hServiceTicketCacheTask != NULL)
        {
            XOnlineTaskClose(pLogonTask->hServiceTicketCacheTask);
            pLogonTask->hServiceTicketCacheTask = NULL;
        }
        
        if (pLogonTask->hServiceTicketCache != NULL)
        {
            CacheClose(pLogonTask->hServiceTicketCache);
            pLogonTask->hServiceTicketCache = NULL;
        }
        pLogonTask->dwServiceTicketCacheState = XON_CACHE_STATE_ERROR;
    }

    //
    // Always perform any remaining combined ticket cache pumping
    //
    hr = S_OK;
    if (pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_OPENING ||
        pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_UPDATING ||
        pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_FLUSHING )
    {
        hr = XOnlineTaskContinue( pLogonTask->hCombinedTicketCacheTask );
        if (hr == XONLINETASK_S_SUCCESS)
        {
            if (pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_OPENING)
            {
                pLogonTask->dwCombinedTicketCacheState = XON_CACHE_STATE_OPENED;
            }
            else if (pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_UPDATING)
            {
                pLogonTask->dwCombinedTicketCacheState = XON_CACHE_STATE_UPDATED;
            }
            else if (pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_FLUSHING)
            {
                TraceSz( AuthVerbose, "LogonTaskContinue: combined ticket cache successfully flushed" );
                pLogonTask->dwCombinedTicketCacheState = XON_CACHE_STATE_DONE;
            }
        }
    }
    if (pLogonTask->fNeedToFlushCombinedTicketCache &&
        pLogonTask->dwState >= LOGON_CS_IN_PROGRESS &&
        (pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_OPENED ||
        pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_UPDATED))
    {
        hr = CacheIndexFlush( pLogonTask->hCombinedTicketCache, pLogonTask->hCombinedTicketCacheTask );
        if (SUCCEEDED(hr))
        {
            pLogonTask->dwCombinedTicketCacheState = XON_CACHE_STATE_FLUSHING;            
        }
        pLogonTask->fNeedToFlushCombinedTicketCache = FALSE;
    }
    if (FAILED(hr))
    {
        if (pLogonTask->hCombinedTicketCacheTask != NULL)
        {
            XOnlineTaskClose(pLogonTask->hCombinedTicketCacheTask);
            pLogonTask->hCombinedTicketCacheTask = NULL;
        }
        
        if (pLogonTask->hCombinedTicketCache != NULL)
        {
            CacheClose(pLogonTask->hCombinedTicketCache);
            pLogonTask->hCombinedTicketCache = NULL;
        }
        pLogonTask->dwCombinedTicketCacheState = XON_CACHE_STATE_ERROR;
    }

    //
    // Always perform any logon time cache pumping
    //
    if (m_dwLogonTimeCacheState != XON_CACHE_STATE_ERROR)
    {
        //
        // Pump the task if we are still opening or flushing
        //
        if (m_dwLogonTimeCacheState == XON_CACHE_STATE_OPENING ||
            m_dwLogonTimeCacheState == XON_CACHE_STATE_FLUSHING )
        {
            hr = XOnlineTaskContinue( m_hLogonTimeCacheTask );
            if (hr == XONLINETASK_S_SUCCESS)
            {
                if (m_dwLogonTimeCacheState == XON_CACHE_STATE_OPENING)
                {
                    m_dwLogonTimeCacheState = XON_CACHE_STATE_OPENED;
                }
                else if (m_dwLogonTimeCacheState == XON_CACHE_STATE_FLUSHING)
                {
                    m_dwLogonTimeCacheState = XON_CACHE_STATE_DONE;
                }
            }
        }
        
        //
        // Record the logon time for this logon if it hasn't been done yet
        //
        if ( !pLogonTask->fLogonTimeCached &&
            (m_dwLogonTimeCacheState == XON_CACHE_STATE_OPENED ||
            m_dwLogonTimeCacheState == XON_CACHE_STATE_DONE))
        {
            GetSystemTimeAsFileTime(&recentLogonTime.lastLogon);
            for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
            {
                if (m_rgLogonUsers[i].xuid.qwUserID != 0 && !XOnlineIsUserGuest(m_rgLogonUsers[i].xuid.dwUserFlags))
                {
                    recentLogonTime.qwUserID = m_rgLogonUsers[i].xuid.qwUserID;
                    hr = CacheUpdate(m_hLogonTimeCache, m_hLogonTimeCacheTask, (BYTE*)&recentLogonTime, NULL, FALSE);
                    if (SUCCEEDED(hr))
                    {
                        //
                        // This update operation should not hit the disk, therefore it is O.K to busy loop for it to complete.
                        //
                        do
                        {
                            hr = XOnlineTaskContinue( m_hLogonTimeCacheTask );
                        } while (hr == XONLINETASK_S_RUNNING);
                    }
                }
            }
            
            hr = CacheIndexFlush(m_hLogonTimeCache, m_hLogonTimeCacheTask);
            if (SUCCEEDED(hr))
            {      
                m_dwLogonTimeCacheState = XON_CACHE_STATE_FLUSHING;
            }
            pLogonTask->fLogonTimeCached = TRUE;
        }

        //
        // If any failures pop up, just forget about this whole cache
        //
        if (FAILED(hr))
        {
            if (m_hLogonTimeCacheTask != NULL)
            {
                XOnlineTaskClose(m_hLogonTimeCacheTask);
                m_hLogonTimeCacheTask = NULL;
            }
            
            if (m_hLogonTimeCache != NULL)
            {
                CacheClose(m_hLogonTimeCache);
                m_hLogonTimeCache = NULL;
            }
            m_dwLogonTimeCacheState = XON_CACHE_STATE_ERROR;
        }
    }

    //
    // Check if we timed out
    //
    if (pLogonTask->dwState > LOGON_XNETWAIT_IN_PROGRESS &&
        pLogonTask->dwState < LOGON_TASK_COMPLETE &&
        GetTickCount() >= pLogonTask->dwMaxEndingTickCount)
    {
        TraceSz( AuthWarn, "LogonTaskContinue: whole logon process timed out" );
        pLogonTask->hr = XONLINE_E_LOGON_SERVICE_UNAVAILABLE;
        goto Cleanup;
    }

    //
    // Now handle the logon state machine
    //
    do
    {
        fMoreWorkToDo = FALSE;        

        switch (pLogonTask->dwState)
        {
        case LOGON_XNETWAIT_IN_PROGRESS:
            dwStatus = XNetGetTitleXnAddr(&xna);
            if (dwStatus != XNET_GET_XNADDR_PENDING)
            {
                if ((dwStatus & XNET_GET_XNADDR_GATEWAY) == 0)
                {
                    TraceSz1( AuthWarn, "LogonTaskContinue: XNetGetTitleXnAddr could not find a gateway, dwStatus:0x%X", dwStatus );
                    pLogonTask->hr = XONLINE_E_LOGON_NO_IP_ADDRESS;
                    goto Cleanup;
                }
                if ((dwStatus & XNET_GET_XNADDR_DNS) == 0)
                {
                    TraceSz1( AuthWarn, "LogonTaskContinue: XNetGetTitleXnAddr could not find a DNS server, dwStatus:0x%X", dwStatus );
                    pLogonTask->hr = XONLINE_E_LOGON_NO_DNS_SERVICE;
                    goto Cleanup;
                }

                //
                // Set max timeout for all of remaining logon work
                //
                pLogonTask->dwMaxEndingTickCount = GetTickCount() + MAX_TOTAL_LOGON_TIME_IN_MS;
                XnSetEventTimer(pLogonTask->abEventTimer, pLogonTask->xonTask.hEventWorkAvailable, MAX_TOTAL_LOGON_TIME_IN_MS );

                //
                // Kick off getting the machine account
                //
                Assert( pLogonTask->hGetMachineAccountTask == NULL );
                hr = GetMachineAccount( pLogonTask->xonTask.hEventWorkAvailable, &pLogonTask->hGetMachineAccountTask );
                if ( FAILED(hr) )
                {
                    pLogonTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                    goto Cleanup;
                }

                pLogonTask->dwState = LOGON_MACS_IN_PROGRESS;
                fMoreWorkToDo = TRUE;
            }
            else
            {
                XnSetEventTimer(
                    pLogonTask->abEventTimer,
                    pLogonTask->xonTask.hEventWorkAvailable,
                    GET_XNADDR_RETRY_PERIOD_IN_MS );

                TraceSz( AuthVerbose, "LogonTaskContinue: Still waiting for XNetGetTitleXnAddr..." );
            }
            break;
            
        case LOGON_MACS_IN_PROGRESS:
            Assert (pLogonTask->hGetMachineAccountTask != NULL);
            hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)pLogonTask->hGetMachineAccountTask );
            if ( hr == XONLINETASK_S_SUCCESS )
            {
                hr = GetMachineAccountGetResults( (XONLINETASK_HANDLE)pLogonTask->hGetMachineAccountTask, &pLogonTask->machineUser );
                Assert( hr == XONLINETASK_S_SUCCESS );
                Assert(pLogonTask->machineUser.xuid.qwUserID != 0);
                XOnlineTaskClose(pLogonTask->hGetMachineAccountTask);
                pLogonTask->hGetMachineAccountTask = NULL;

                //
                // Start combined ticket phase
                //
                Assert( pLogonTask->hGetCombinedTicketTask == NULL );
                hr = GetCombinedTicket( &pLogonTask->machineUser, TRUE, pLogonTask->xonTask.hEventWorkAvailable, &pLogonTask->hGetCombinedTicketTask );
                if ( FAILED(hr) )
                {
                    pLogonTask->hr = hr;
                    goto Cleanup;
                }
                pLogonTask->dwState = LOGON_GET_TGT_IN_PROGRESS;
                fMoreWorkToDo = TRUE;                
            }
            else if FAILED(hr)
            {
                pLogonTask->hr = hr;
                goto Cleanup;
            }
            else
            {
                Assert( hr == XONLINETASK_S_RUNNING );
            }
            break;
                
        case LOGON_GET_TGT_IN_PROGRESS:
            Assert (pLogonTask->hGetCombinedTicketTask != NULL);
            hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)pLogonTask->hGetCombinedTicketTask );
            if ( hr == XONLINETASK_S_SUCCESS )
            {
                hr = GetCombinedTicketGetResults( (XONLINETASK_HANDLE)pLogonTask->hGetCombinedTicketTask, &pLogonTask->pCombinedTGTContext, &fTicketFromCache );
                Assert( hr == XONLINETASK_S_SUCCESS );
                Assert(pLogonTask->pCombinedTGTContext != NULL);
                XOnlineTaskClose(pLogonTask->hGetCombinedTicketTask);
                pLogonTask->hGetCombinedTicketTask = NULL;

                //
                // Store in Cache if we got some good results back. Best effort only.
                //
                if (!fTicketFromCache &&
                    (pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_OPENED ||
                    pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_UPDATED ||
                    pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_DONE))
                {
                    RtlCopyMemory( &pLogonTask->combinedContextToCache, pLogonTask->pCombinedTGTContext, sizeof(pLogonTask->combinedContextToCache) );
                    hr = CacheUpdate(
                        pLogonTask->hCombinedTicketCache,
                        pLogonTask->hCombinedTicketCacheTask,
                        (BYTE*)&pLogonTask->combinedContextToCache,
                        (BYTE*)&pLogonTask->combinedContextToCache.ClientName,
                        TRUE );
                    if (SUCCEEDED(hr))
                    {
                        TraceSz2( AuthVerbose, "LogonTaskContinue updating cache with combined ticket that expires in %d seconds (%8X)",
                            (ULONG)((pLogonTask->pCombinedTGTContext->ExpirationTime.QuadPart -
                            pLogonTask->pCombinedTGTContext->StartTime.QuadPart) / 10000000),
                            XConfigChecksum((BYTE*)pLogonTask->pCombinedTGTContext, FIELD_OFFSET(XKERB_TGT_CONTEXT, StartTime))
                            );
                        pLogonTask->dwCombinedTicketCacheState = XON_CACHE_STATE_UPDATING;
                        pLogonTask->fNeedToFlushCombinedTicketCache = TRUE;
                    }
                }
                
                //
                // Start service ticket phase
                //
                pLogonTask->cServices = 0;
                for ( i=0; i<XONLINE_MAX_SERVICE_REQUEST; ++i )
                {
                    if (m_rgServiceContexts[i].serviceInfo.dwServiceID != XONLINE_INVALID_SERVICE)
                    {
                        pLogonTask->adwServiceIDs[pLogonTask->cServices++] = m_rgServiceContexts[i].serviceInfo.dwServiceID;
                    }
                }        

                Assert( pLogonTask->hGetServiceTicketTask == NULL );
                hr = GetServiceTicket( pLogonTask->pCombinedTGTContext, pLogonTask->adwServiceIDs, pLogonTask->cServices, TRUE, pLogonTask->xonTask.hEventWorkAvailable, &pLogonTask->hGetServiceTicketTask );
                if ( FAILED(hr) )
                {
                    pLogonTask->hr = hr;
                    goto Cleanup;
                }
                pLogonTask->dwState = LOGON_GET_TKT_IN_PROGRESS;
                fMoreWorkToDo = TRUE;                
            }
            else if FAILED(hr)
            {
                if (hr == XONLINE_E_LOGON_MACHINE_AUTHENTICATION_FAILED)
                {
                    //
                    // Machine account does not exists in Passport DB.
                    // Create a fresh new machine account to solve this problem.
                    //
                    TraceSz( AuthVerbose, "LogonTaskContinue: machine account not authenticating, creating new one");
                    
                    XOnlineTaskClose(pLogonTask->hGetCombinedTicketTask);
                    pLogonTask->hGetCombinedTicketTask = NULL;

                    m_fGetNewMachineAccount = TRUE;
                    Assert( pLogonTask->hGetMachineAccountTask == NULL );
                    hr = GetMachineAccount( pLogonTask->xonTask.hEventWorkAvailable, &pLogonTask->hGetMachineAccountTask );
                    if ( FAILED(hr) )
                    {
                        pLogonTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                        goto Cleanup;
                    }
                    pLogonTask->dwState = LOGON_MACS_IN_PROGRESS;
                    fMoreWorkToDo = TRUE;
                    break;
                }
                else
                {
                    //
                    // Propagate error code up
                    //
                    pLogonTask->hr = hr;
                    goto Cleanup;
                }
            }
            else
            {
                Assert( hr == XONLINETASK_S_RUNNING );
            }
            break;

        case LOGON_GET_TKT_IN_PROGRESS:
            Assert (pLogonTask->hGetServiceTicketTask != NULL);
            hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)pLogonTask->hGetServiceTicketTask );
            if (hr == XONLINETASK_S_SUCCESS)
            {
                hr = GetServiceTicketGetResults( (XONLINETASK_HANDLE)pLogonTask->hGetServiceTicketTask, &pLogonTask->pXKerbServiceContext, &fTicketFromCache );
                Assert( hr == XONLINETASK_S_SUCCESS );
                XOnlineTaskClose(pLogonTask->hGetServiceTicketTask);
                pLogonTask->hGetServiceTicketTask = NULL;

                Assert( pLogonTask->pXKerbServiceContext != NULL );

                //
                // Scan results and see what hr we got for each service
                //
                fSomeServicesFailed = FALSE;
                pLogonTask->cServices = 0;
                for (i=0; i<pLogonTask->pXKerbServiceContext->dwNumServices; ++i)
                {
                    Assert( pLogonTask->pXKerbServiceContext->dwServiceID[i] != XONLINE_INVALID_SERVICE );
                    j = GetServiceArrayIndex(pLogonTask->pXKerbServiceContext->dwServiceID[i]);
                    Assert(j != XONLINE_INVALID_SERVICE);
                    Assert(m_rgServiceContexts[j].serviceInfo.dwServiceID == pLogonTask->pXKerbServiceContext->dwServiceID[i]);
                    m_rgServiceContexts[j].hr = pLogonTask->pXKerbServiceContext->hrService[i];

                    if (FAILED(m_rgServiceContexts[j].hr))
                    {
                        fSomeServicesFailed = TRUE;
                    }
                    else if (m_rgServiceContexts[j].hr == S_FALSE)
                    {
                        pLogonTask->adwServiceIDs[pLogonTask->cServices++] = m_rgServiceContexts[i].serviceInfo.dwServiceID;
                    }
                    else
                    {
                        Assert( m_rgServiceContexts[j].hr == S_OK );
                        m_rgServiceContexts[j].serviceInfo.wServicePort = pLogonTask->pXKerbServiceContext->wServicePort[i];
                        m_rgServiceContexts[j].serviceInfo.serviceIP = pLogonTask->pXKerbServiceContext->siteIPAddress;
                        m_rgServiceContexts[j].pXKerbServiceContext = pLogonTask->pXKerbServiceContext;
                    }
                }

                //
                // Store in Cache if we got some good results back. Best effort only.
                //
                if (!fTicketFromCache && !fSomeServicesFailed &&
                    (pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_OPENED ||
                    pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_UPDATED ||
                    pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_DONE))
                {
                    RtlCopyMemory( &pLogonTask->serviceContextToCache, pLogonTask->pXKerbServiceContext, sizeof(pLogonTask->serviceContextToCache) );
                    hr = CacheUpdate(
                        pLogonTask->hServiceTicketCache,
                        pLogonTask->hServiceTicketCacheTask,
                        (BYTE*)&pLogonTask->serviceContextToCache,
                        (BYTE*)&pLogonTask->serviceContextToCache.siteIPAddress,
                        TRUE );
                    if (SUCCEEDED(hr))
                    {
                        TraceSz2( AuthVerbose, "LogonTaskContinue updating cache with service ticket that expires in %d seconds (%8X)",
                            (ULONG)((pLogonTask->pXKerbServiceContext->ExpirationTime.QuadPart -
                            pLogonTask->pXKerbServiceContext->StartTime.QuadPart) / 10000000),
                            XConfigChecksum((BYTE*)pLogonTask->pXKerbServiceContext, FIELD_OFFSET(XKERB_SERVICE_CONTEXT, StartTime))
                            );
                        pLogonTask->dwServiceTicketCacheState = XON_CACHE_STATE_UPDATING;
                        pLogonTask->fNeedToFlushServiceTicketCache = TRUE;
                    }
                }
                
                //
                // Compute g^x and other stuff needed by lower Xnet layer
                //
                pXoKerbInfo = &(pLogonTask->pXKerbServiceContext->xoKerbInfo);
                XNetRandom( pXoKerbInfo->_abDhX, sizeof(pXoKerbInfo->_abDhX) );
                Assert( sizeof(pXoKerbInfo->_abDhGX) == sizeof(g_abOakleyGroup1Base) );
                if (!XcModExp( (DWORD*)pXoKerbInfo->_abDhGX, (DWORD*)g_abOakleyGroup1Base, (DWORD*)pXoKerbInfo->_abDhX, (DWORD*)g_abOakleyGroup1Mod, sizeof(pXoKerbInfo->_abDhGX) / sizeof(DWORD) ) )
                {
                    TraceSz( AuthWarn, "LogonTaskContinue: XcModExp failed" );
                    pLogonTask->hr = XONLINE_E_INTERNAL_ERROR;
                    goto Cleanup;
                }

                RtlCopyMemory( pXoKerbInfo->_abKey, pLogonTask->pXKerbServiceContext->SessionKey, sizeof(pXoKerbInfo->_abKey) );
                pXoKerbInfo->_dwTicketId = m_ctAllocatedServiceContexts;
                pXoKerbInfo->_cbApReqMax = pLogonTask->pXKerbServiceContext->dwTicketLength + AP_REQUEST_OVERHEAD_ABOVE_TICKET;

                //
                // Compute user permutation DWORD
                //
                Assert( XONLINE_MAX_LOGON_USERS == sizeof(DWORD) );
                *((DWORD*)abUserPerm) = 0;
                for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
                {
                    if (m_rgLogonUsers[i].xuid.qwUserID != 0)
                    {
                        for (j=0; j<XONLINE_MAX_LOGON_USERS; ++j)
                        {
                            if (m_rgLogonUsers[i].xuid.qwUserID == pLogonTask->pXKerbServiceContext->UserID[j])
                            {
                                abUserPerm[i] = (BYTE)((j << KEYEX_USER_PERMUTATION_SHIFT) | KEYEX_USER_PERMUTE_MASK | (m_rgLogonUsers[i].xuid.dwUserFlags & KEYEX_USER_GUEST_MASK));
                                break;                    
                            }
                        }
                    }
                }
                pXoKerbInfo->_dwUserPerm = *((DWORD*)abUserPerm);
                
                //
                // Record the context into a global
                // Make sure we use a new context next time
                //
                m_pAllocatedServiceContexts[m_ctAllocatedServiceContexts++] = pLogonTask->pXKerbServiceContext;
                pLogonTask->pXKerbServiceContext = NULL;
                
                //
                // If more services left, then start another service ticket request and loop back
                //
                if (pLogonTask->cServices > 0)
                {
                    Assert( pLogonTask->hGetServiceTicketTask == NULL );
                    hr = GetServiceTicket( pLogonTask->pCombinedTGTContext, pLogonTask->adwServiceIDs, pLogonTask->cServices, TRUE, pLogonTask->xonTask.hEventWorkAvailable, &pLogonTask->hGetServiceTicketTask );
                    if ( FAILED(hr) )
                    {
                        pLogonTask->hr = hr;
                        goto Cleanup;
                    }

                    fMoreWorkToDo = TRUE;
                    break;
                }

                //
                // Tickets to all services that we can get have already been obtained. Prepare to move to next state
                //
                
                //
                // Mark unrequested services
                //
                for (i=0; i<XONLINE_MAX_SERVICE_REQUEST; ++i)
                {
                    if (m_rgServiceContexts[i].serviceInfo.dwServiceID == XONLINE_INVALID_SERVICE)
                    {
                        m_rgServiceContexts[i].hr = XONLINE_E_LOGON_SERVICE_NOT_REQUESTED;
                    }
                }
                
                //
                // Convert all IPs into fake IPs the Xnet stack understands
                //
                for (i=0; i<XONLINE_MAX_SERVICE_REQUEST; ++i)
                {
                    if (m_rgServiceContexts[i].hr == S_OK)
                    {
                        Assert( m_rgServiceContexts[i].pXKerbServiceContext != NULL );
                        dwStatus = XNetServerToInAddr(
                            m_rgServiceContexts[i].serviceInfo.serviceIP,
                            m_rgServiceContexts[i].serviceInfo.dwServiceID,
                            &serviceAddr );
                        if(dwStatus != 0)
                        {
                            TraceSz1( AuthWarn, "LogonTaskContinue: XNetServerToInAddr failed, dwStatus: 0x%X", dwStatus );
                            pLogonTask->hr = XONLINE_E_LOGON_SG_SERVICE_FAILED;
                            goto Cleanup;
                        }
                        m_rgServiceContexts[i].serviceInfo.serviceIP.s_addr = serviceAddr.s_addr;
                    }
                }

                ULONGLONG aqwUserId[XONLINE_MAX_LOGON_USERS];
                IN_ADDR inaLogon;

                for (i = 0; i < XONLINE_MAX_LOGON_USERS; ++i)
                {
                    //@@@ drm: Should guests be filtered out here?
                    aqwUserId[i] = m_rgLogonUsers[i].xuid.qwUserID;
                }

                hr = XOnlineGetServiceInfo(XONLINE_PRESENCE_SERVICE, &serviceInfo);
                inaLogon.s_addr = SUCCEEDED(hr) ? serviceInfo.serviceIP.s_addr : 0;

                XnLogon(inaLogon, aqwUserId, pLogonTask->xonTask.hEventWorkAvailable);

                if (inaLogon.s_addr == 0)
                {
                    pLogonTask->hr = XONLINE_S_LOGON_CONNECTION_ESTABLISHED;
                    m_fLogonCompletedSuccessfully = TRUE;
                }
                else
                {
                    pLogonTask->dwState = LOGON_CS_IN_PROGRESS;
                }
                break;
            }
            else if (FAILED(hr))
            {
                if (hr == XONLINE_E_LOGON_XBOX_ACCOUNT_INVALID)
                {
                    //
                    // Machine account exists in Passport, but not in xbox UODB
                    // Create a fresh new machine account to solve this problem
                    //
                    TraceSz( AuthVerbose, "LogonTaskContinue: machine account not in UODB, creating new one");

                    XOnlineTaskClose(pLogonTask->hGetServiceTicketTask);
                    pLogonTask->hGetServiceTicketTask = NULL;

                    m_fGetNewMachineAccount = TRUE;
                    Assert( pLogonTask->hGetMachineAccountTask == NULL );
                    hr = GetMachineAccount( pLogonTask->xonTask.hEventWorkAvailable, &pLogonTask->hGetMachineAccountTask );
                    if ( FAILED(hr) )
                    {
                        pLogonTask->hr = XONLINE_E_LOGON_MACS_FAILED;
                        goto Cleanup;
                    }
                    pLogonTask->dwState = LOGON_MACS_IN_PROGRESS;
                    fMoreWorkToDo = TRUE;
                    break;
                }
                else
                {
                    //
                    // Propagate error code up
                    //
                    pLogonTask->hr = hr;
                    goto Cleanup;
                }
            }
            else
            {
                Assert( hr == XONLINETASK_S_RUNNING );
            }
            break;

        case LOGON_CS_IN_PROGRESS:
            dwStatus = XnLogonGetStatus(NULL);
        
            switch(dwStatus)
            {
            case XN_LOGON_STATE_ONLINE:
            
                if (pLogonTask->fAliveSent == FALSE)
                {
                    Assert( pLogonTask->hPresenceTask == NULL );
                    hr = NotificationSendAlive( m_rgLogonUsers, pLogonTask->xonTask.hEventWorkAvailable, &pLogonTask->hPresenceTask);
                    if (FAILED(hr))
                    {
                        TraceSz1( AuthWarn, "LogonTaskContinue: NotificationSendAlive Failed: hr:0x%X", hr );
                        pLogonTask->hr = XONLINE_E_LOGON_PRESENCE_SERVICE_FAILED;
                        goto Cleanup;
                    }
                    pLogonTask->fAliveSent = TRUE;
                }
                break;

            case XN_LOGON_STATE_OFFLINE:
            
                TraceSz( AuthWarn, "LogonTaskContinue: XnLogonGetStatus returned XN_LOGON_STATE_OFFLINE" );
                pLogonTask->hr = XONLINE_E_LOGON_SG_SERVICE_FAILED;
                goto Cleanup;
            }

            if (pLogonTask->hPresenceTask != NULL)
            {
                hr = XOnlineTaskContinue(pLogonTask->hPresenceTask);
                if (hr != XONLINETASK_S_RUNNING)
                {
                    XOnlineTaskClose(pLogonTask->hPresenceTask);
                    pLogonTask->hPresenceTask = NULL;
                    if (hr == XONLINETASK_S_SUCCESS)
                    {
                        pLogonTask->hr = XONLINE_S_LOGON_CONNECTION_ESTABLISHED;
                        m_fLogonCompletedSuccessfully = TRUE;
                    }
                    else
                    {
                        TraceSz1( AuthWarn, "LogonTaskContinue: presence task returned hr = 0x%X", hr );
                        pLogonTask->hr = XONLINE_E_LOGON_PRESENCE_SERVICE_FAILED;
                    }
                }
            }
            break;
        
        case LOGON_TASK_COMPLETE:
            if (SUCCEEDED(pLogonTask->hr) && pLogonTask->fAliveSent)
            {
                dwStatus = XnLogonGetStatus(NULL);
        
                switch(dwStatus)
                {
                case XN_LOGON_STATE_OFFLINE:
                    TraceSz( AuthWarn, "LogonTaskContinue: XnLogonGetStatus returned XN_LOGON_STATE_OFFLINE in LOGON_TASK_COMPLETE state" );
                    pLogonTask->hr = XONLINE_E_LOGON_CONNECTION_LOST;
                    goto Cleanup;
                }
            }

            break;
            
        default:
            AssertSz(FALSE, "LogonTaskContinue: Bad State");
        }
    } while (fMoreWorkToDo);    
        
Cleanup:
    if (pLogonTask->hr != XONLINETASK_S_RUNNING)
    {
        pLogonTask->dwState = LOGON_TASK_COMPLETE;
    }

    return(pLogonTask->hr);
}

//
// Close function for Logon
//
void CXo::LogonTaskClose(XONLINETASK_HANDLE hTask)
{
    DWORD i;
    Assert(hTask != NULL);

    PXONLINETASK_LOGON pLogonTask = (PXONLINETASK_LOGON)hTask;
    
    TraceSz2( AuthVerbose, "LogonTaskClose: hr:0x%X, dwState:%d", pLogonTask->hr, pLogonTask->dwState );
    
    if ( pLogonTask->hGetMachineAccountTask != NULL )
    {
        XOnlineTaskClose( pLogonTask->hGetMachineAccountTask );
    }
    
    if ( pLogonTask->hGetCombinedTicketTask != NULL )
    {
        XOnlineTaskClose( pLogonTask->hGetCombinedTicketTask );
    }
    
    if ( pLogonTask->hGetServiceTicketTask != NULL )
    {
        XOnlineTaskClose( pLogonTask->hGetServiceTicketTask );
    }
    
    if (pLogonTask->hPresenceTask != NULL)
    {
        XOnlineTaskClose(pLogonTask->hPresenceTask);
    }

    if (pLogonTask->hCombinedTicketCacheTask != NULL)
    {
        XOnlineTaskClose(pLogonTask->hCombinedTicketCacheTask);
    }
    
    if (pLogonTask->hCombinedTicketCache != NULL)
    {
        CacheClose(pLogonTask->hCombinedTicketCache);
    }

    if (pLogonTask->hServiceTicketCacheTask != NULL)
    {
        XOnlineTaskClose(pLogonTask->hServiceTicketCacheTask);
    }
    
    if (pLogonTask->hServiceTicketCache != NULL)
    {
        CacheClose(pLogonTask->hServiceTicketCache);
    }

    if (pLogonTask->hCreatedEvent != NULL)
    {
        CloseHandle(pLogonTask->hCreatedEvent);
    }

    XnSetEventTimer(pLogonTask->abEventTimer, NULL, 0xFFFFFFFF);

    XnLogoff();
    
    m_fGetNewMachineAccount = FALSE;    
    m_fLogonCompletedSuccessfully = FALSE;
    
    for (i=0; i<m_ctAllocatedServiceContexts; ++i)
    {
        SysFree( m_pAllocatedServiceContexts[i] );
        m_pAllocatedServiceContexts[i] = NULL;
    }
    m_ctAllocatedServiceContexts = 0;
    
    if ( pLogonTask->pCombinedTGTContext != NULL )
    {
        SysFree( pLogonTask->pCombinedTGTContext );
    }
    
    if ( pLogonTask->pXKerbServiceContext != NULL )
    {
        SysFree( pLogonTask->pXKerbServiceContext );
    }
    
    RtlZeroMemory( m_rgServiceContexts, sizeof(m_rgServiceContexts) );

    Assert( m_pLogonTask == pLogonTask );
    m_pLogonTask = NULL;

    SysFree(pLogonTask);
}

//
// Main Logon routine
//
HRESULT CXo::XOnlineLogon(
    IN PXONLINE_USER pUsers,
    IN DWORD* pdwServiceIDs,
    IN DWORD cServices,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE pHandle
    )
{
    XoEnter("XOnlineLogon");
    XoCheck((cServices == 0) == (pdwServiceIDs == NULL));
    XoCheck(pHandle != NULL);

    BOOL fHasUsers = FALSE;
    HRESULT hr;
    LONG i,j;
    DWORD dwServices;
    DWORD dwServiceIndex;
    PXONLINE_USER pTempUser;
    PXONLINETASK_LOGON pLogonTask = NULL;
    PXONLINETASK_LOGON pPrevLogonTask;
    WSADATA WsaData;
    DWORD err;

    //
    // Check if the ethernet is connected
    //
    if ((XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE) == 0)
    {
        TraceSz( AuthWarn, "XOnlineLogon: XNetGetEthernetLinkStatus reported no ethernet connection" );
        hr = XONLINE_E_LOGON_NO_NETWORK_CONNECTION;
        goto Cleanup;
    }

    //
    // Allocate the logon task
    //
    pLogonTask = (PXONLINETASK_LOGON)SysAllocZ(sizeof(XONLINETASK_LOGON), PTAG_XONLINETASK_LOGON);
    if (pLogonTask == NULL)
    {
        hr = XONLINE_E_OUT_OF_MEMORY;
        goto Cleanup;
    }

    //
    // Make sure only one logon is happening at a time
    //
    pPrevLogonTask = (PXONLINETASK_LOGON)InterlockedCompareExchangePointer(
                (PVOID*)&m_pLogonTask,
                pLogonTask,
                NULL);

    if (pPrevLogonTask != NULL)
    {
        AssertSz( FALSE, "Previous logon task must be closed before next XOnlineLogon" );
        hr = XONLINE_E_TASK_BUSY;
        goto Cleanup;
    }
    
    if (pUsers != NULL)
    {
        TraceSz8( AuthVerbose, "XOnlineLogon called for users 0x%016I64X(%s) 0x%016I64X(%s) 0x%016I64X(%s) 0x%016I64X(%s)",
            pUsers[0].xuid.qwUserID, pUsers[0].name,
            pUsers[1].xuid.qwUserID, pUsers[1].name,
            pUsers[2].xuid.qwUserID, pUsers[2].name,
            pUsers[3].xuid.qwUserID, pUsers[3].name );
    }

    for (i=0; i<XONLINE_MAX_SERVICE_REQUEST; ++i)
    {
        m_rgServiceContexts[i].serviceInfo.dwServiceID = XONLINE_INVALID_SERVICE;
        m_rgServiceContexts[i].hr = S_FALSE;
    };

    dwServices = 0;
    
    for (i=0; i<(LONG)cServices; ++i)
    {
        XoCheck(pdwServiceIDs[i] != XONLINE_PRESENCE_SERVICE);
        XoCheck(pdwServiceIDs[i] != XONLINE_MESSAGE_SERVICE);
        if (pdwServiceIDs[i] < XONLINE_NUMBER_SEQUENTIAL_SERVICES)
        {
            Assert(m_rgServiceContexts[pdwServiceIDs[i]].serviceInfo.dwServiceID == XONLINE_INVALID_SERVICE);
            m_rgServiceContexts[pdwServiceIDs[i]].serviceInfo.dwServiceID = pdwServiceIDs[i];
        }
        else
        {
            if (m_rgServiceContexts[XONLINE_NUMBER_SEQUENTIAL_SERVICES].serviceInfo.dwServiceID == XONLINE_INVALID_SERVICE)
            {
                m_rgServiceContexts[XONLINE_NUMBER_SEQUENTIAL_SERVICES].serviceInfo.dwServiceID = pdwServiceIDs[i];                
            }
            else if (m_rgServiceContexts[XONLINE_NUMBER_SEQUENTIAL_SERVICES+1].serviceInfo.dwServiceID == XONLINE_INVALID_SERVICE)
            {
                m_rgServiceContexts[XONLINE_NUMBER_SEQUENTIAL_SERVICES+1].serviceInfo.dwServiceID = pdwServiceIDs[i];                
            }
            else
            {
                AssertSz( FALSE, "Requesting too many non built in services" );
            }
        }
        
        dwServices++;
    }

    //
    // Check if there are any users
    //
    if (pUsers != NULL)
    {
        for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
        {
            if (pUsers[i].xuid.qwUserID != 0)
            {
                fHasUsers = TRUE;
            }
        }
    }

    //
    // Add hidden services for users if users are present
    //
    if (fHasUsers)
    {
        Assert( m_rgServiceContexts[XONLINE_PRESENCE_SERVICE].serviceInfo.dwServiceID == XONLINE_INVALID_SERVICE );
        m_rgServiceContexts[XONLINE_PRESENCE_SERVICE].serviceInfo.dwServiceID = XONLINE_PRESENCE_SERVICE;
        dwServices++;
    }

    //
    // Message service is always asked for
    //
    m_rgServiceContexts[XONLINE_MESSAGE_SERVICE].serviceInfo.dwServiceID = XONLINE_MESSAGE_SERVICE;
    dwServices++;

    XoCheck(dwServices > 0);
    XoCheck(dwServices <= XONLINE_MAX_NUMBER_SERVICE);

#ifdef XNET_FEATURE_TRACE
    {
        char buffer1[64],buffer2[64],buf[8];
        DWORD k;
        buffer1[0] = buffer2[0] = '\0';
        for (k=0; k<cServices; ++k)
        {
            sprintf(buf," %d", pdwServiceIDs[k]);
            strcat(buffer1, buf);
        }
        for (k=0; k<XONLINE_MAX_SERVICE_REQUEST; ++k)
        {
            if (m_rgServiceContexts[k].serviceInfo.dwServiceID != XONLINE_INVALID_SERVICE)
            {
                sprintf(buf," %d", m_rgServiceContexts[k].serviceInfo.dwServiceID);
                strcat(buffer2, buf);
            }
        }
        TraceSz2( AuthVerbose, "XOnlineLogon asked for services:%s actually getting services:%s", buffer1, buffer2 );
    }
#endif

    //
    // Make copy of logon users
    //
    if (pUsers)
    {
        RtlCopyMemory( m_rgLogonUsers, pUsers, sizeof( m_rgLogonUsers ) );
    }
    else
    {
        RtlZeroMemory( m_rgLogonUsers, sizeof( m_rgLogonUsers ) );
    }

    for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
    {
        m_rgLogonUsers[i].index = i;
    }

    //
    // Check that hosts of all guests are present
    //
    for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
    {
        if (m_rgLogonUsers[i].xuid.qwUserID != 0 && XOnlineIsUserGuest(m_rgLogonUsers[i].xuid.dwUserFlags))
        {
            for (j=0; j<XONLINE_MAX_LOGON_USERS; ++j)
            {
                if (m_rgLogonUsers[j].xuid.qwUserID == m_rgLogonUsers[i].xuid.qwUserID &&
                    !XOnlineIsUserGuest(m_rgLogonUsers[j].xuid.dwUserFlags))
                {
                    // Found the host
                    break;
                }
            }
            if (j == XONLINE_MAX_LOGON_USERS)
            {
                AssertSz( FALSE, "User logged on as guest does not have host present" );
                hr = XONLINE_E_NO_GUEST_ACCESS;
                goto Cleanup;
            }
        }
    }

    //
    // Find all the unique users
    //
    m_dwUniqueLogonUsers = 0;
    for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
    {
        if (m_rgLogonUsers[i].xuid.qwUserID != 0)
        {
            for (j=0; j<(LONG)m_dwUniqueLogonUsers; ++j)
            {
                if (m_apUniqueLogonUsers[j]->xuid.qwUserID == m_rgLogonUsers[i].xuid.qwUserID)
                {
                    break;
                }
            }
            if (j == (LONG)m_dwUniqueLogonUsers)
            {
                m_apUniqueLogonUsers[m_dwUniqueLogonUsers++] = &m_rgLogonUsers[i];
            }
        }
    }

    //
    // Bubble sort the users
    //
    for (i=0; i<(((LONG)m_dwUniqueLogonUsers)-1); ++i)
    {
        for (j=i+1; j<(LONG)m_dwUniqueLogonUsers; ++j)
        {
            if (m_apUniqueLogonUsers[i]->xuid.qwUserID > m_apUniqueLogonUsers[j]->xuid.qwUserID)
            {
                pTempUser = m_apUniqueLogonUsers[i];
                m_apUniqueLogonUsers[i] = m_apUniqueLogonUsers[j];
                m_apUniqueLogonUsers[j] = pTempUser;
            }
        }
    }
    
    TaskInitializeContext(&(pLogonTask->xonTask));
    pLogonTask->xonTask.pfnContinue = LogonTaskContinue;
    pLogonTask->xonTask.pfnClose = LogonTaskClose;
    pLogonTask->hr = XONLINETASK_S_RUNNING;

    //
    // Create an event if one doesn't exist
    //
    if (hEvent == NULL)
    {
        pLogonTask->hCreatedEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
        if (pLogonTask->hCreatedEvent == NULL)
        {
            hr = XONLINE_E_OUT_OF_MEMORY;
            goto Cleanup;
        }
        pLogonTask->xonTask.hEventWorkAvailable = pLogonTask->hCreatedEvent;
    }
    else
    {
        pLogonTask->xonTask.hEventWorkAvailable = hEvent;
    }
    
    pLogonTask->dwState = LOGON_XNETWAIT_IN_PROGRESS;
    
    *pHandle = (XONLINETASK_HANDLE)pLogonTask;

	//
	// Open the combined ticket cache
	//
	if ( pLogonTask->dwCombinedTicketCacheState == XON_CACHE_STATE_INITIAL )
	{
        hr = CacheOpen(XONLC_TYPE_COMBINED_USER_TICKETS, NULL, hEvent, &pLogonTask->hCombinedTicketCache, &pLogonTask->hCombinedTicketCacheTask);
        if (FAILED(hr))
        {
            pLogonTask->hCombinedTicketCache = NULL;
            pLogonTask->hCombinedTicketCacheTask = NULL;
            pLogonTask->dwCombinedTicketCacheState = XON_CACHE_STATE_ERROR;
        }
        else
        {
            pLogonTask->dwCombinedTicketCacheState = XON_CACHE_STATE_OPENING;
        }
    }
    
	//
	// Open the service ticket cache
	//
	if ( pLogonTask->dwServiceTicketCacheState == XON_CACHE_STATE_INITIAL )
	{
        hr = CacheOpen(XONLC_TYPE_SERVICE_TICKETS, NULL, hEvent, &pLogonTask->hServiceTicketCache, &pLogonTask->hServiceTicketCacheTask);
        if (FAILED(hr))
        {
            pLogonTask->hServiceTicketCache = NULL;
            pLogonTask->hServiceTicketCacheTask = NULL;
            pLogonTask->dwServiceTicketCacheState = XON_CACHE_STATE_ERROR;
        }
        else
        {
            pLogonTask->dwServiceTicketCacheState = XON_CACHE_STATE_OPENING;
        }
    }

    //
    // Get things rolling
    //
    LogonTaskContinue( (XONLINETASK_HANDLE)pLogonTask );
    
    hr = S_OK;
    
Cleanup:
    if (FAILED(hr))
    {
        RtlZeroMemory( m_rgServiceContexts, sizeof(m_rgServiceContexts) );

        *pHandle = NULL;
        if (pLogonTask != NULL)
        {
            if (m_pLogonTask == pLogonTask)
            {
                m_pLogonTask = NULL;
            }
            SysFree(pLogonTask);
        }
    }    

    return(XoLeave(hr));
}

//
// Function to retrieve the results of a Logon Task without pumping it.
//
HRESULT CXo::XOnlineLogonTaskGetResults(XONLINETASK_HANDLE hTask)
{
    XoEnter("XOnlineLogonTaskGetResults");
    XoCheck(hTask != NULL);

    PXONLINETASK_LOGON pLogonTask = (PXONLINETASK_LOGON)hTask;

    return(XoLeave(pLogonTask->hr));
}


PXONLINE_USER CXo::XOnlineGetLogonUsers()
{
    XoEnter_("XOnlineGetLogonUsers", NULL);

    if ( m_fLogonCompletedSuccessfully )
    {
        return m_rgLogonUsers;
    }
    else
    {
        return NULL;
    }
}


HRESULT CXo::XOnlineGetServiceInfo(IN DWORD dwServiceID, OUT PXONLINE_SERVICE_INFO pServiceInfo)
{
    XoEnter("XOnlineGetServiceInfo");
    DWORD dwServiceIndex;
    
    if (m_pLogonTask == NULL)
    {
        AssertSz( FALSE, "XOnlineGetServiceInfo cannot be called after logon task is closed" );
        return XONLINE_E_NOT_INITIALIZED;
    }
    
    dwServiceIndex = GetServiceArrayIndex( dwServiceID );

    if (pServiceInfo != NULL)
    {
        RtlCopyMemory( pServiceInfo, &m_rgServiceContexts[dwServiceIndex].serviceInfo, sizeof(XONLINE_SERVICE_INFO) );
    }
    
    return m_rgServiceContexts[dwServiceIndex].hr;
}

XOKERBINFO * CXo::XkerbGetInfo(DWORD dwServiceID)
{
    PXKERB_SERVICE_CONTEXT pXKerbServiceContext;
    DWORD dwServiceIndex = GetServiceArrayIndex( dwServiceID );

    Assert( SUCCEEDED(m_rgServiceContexts[dwServiceIndex].hr) );
    pXKerbServiceContext = m_rgServiceContexts[dwServiceIndex].pXKerbServiceContext;
    Assert( pXKerbServiceContext != NULL );

    return &(pXKerbServiceContext->xoKerbInfo);
}

NTSTATUS CXo::XkerbBuildApReq(DWORD dwServiceID, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, BYTE * pbApReq, UINT * pcbApReq)
{
    NTSTATUS Status;
    PXKERB_SERVICE_CONTEXT pXKerbServiceContext;
    DWORD dwServiceIndex = GetServiceArrayIndex( dwServiceID );
    
    Assert( SUCCEEDED(m_rgServiceContexts[dwServiceIndex].hr) );
    pXKerbServiceContext = m_rgServiceContexts[dwServiceIndex].pXKerbServiceContext;
    Assert( pXKerbServiceContext != NULL );

    Status = XkerbBuildApRequest( pXKerbServiceContext, pliTime, pbSha, cbSha, pbApReq, pcbApReq );
    if (!NT_SUCCESS(Status))
    {
        TraceSz1( AuthWarn, "XkerbBuildApReq: XkerbBuildApRequest failed with Status: 0x%X", Status );
    }

    return (Status);
}

NTSTATUS CXo::XkerbCrackApRep(DWORD dwServiceID, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, BYTE * pbApRep, UINT cbApRep)
{
    NTSTATUS Status;
    PXKERB_SERVICE_CONTEXT pXKerbServiceContext;
    DWORD dwServiceIndex = GetServiceArrayIndex( dwServiceID );
    
    Assert( SUCCEEDED(m_rgServiceContexts[dwServiceIndex].hr) );
    pXKerbServiceContext = m_rgServiceContexts[dwServiceIndex].pXKerbServiceContext;
    Assert( pXKerbServiceContext != NULL );

    Status = XkerbVerifyApReply( pXKerbServiceContext, pliTime, pbSha, cbSha, pbApRep, cbApRep );
    if (!NT_SUCCESS(Status))
    {
        TraceSz1( AuthWarn, "XkerbCrackApRep: XkerbVerifyApReply failed with Status: 0x%X", Status );
    }

    return (Status);
}

#ifdef XONLINE_FEATURE_INSECURE

void CXo::XkerbGetAuthData(XKERB_AD_XBOX * pAuthData)
{
    DWORD i;
    
    RtlZeroMemory( pAuthData, sizeof(XKERB_AD_XBOX) );

    pAuthData->wAuthDataVersion = XONLINE_AUTHDATA_VERSION;

#ifdef XONLINE_FEATURE_XBOX
    NTSTATUS Status;
    ULONG ulType, ulSize;
    Status = ExQueryNonVolatileSetting(XC_FACTORY_ETHERNET_ADDR, &ulType, pAuthData->abEthernetAddr, sizeof(pAuthData->abEthernetAddr), &ulSize);
    Assert(NT_SUCCESS(Status) && ulSize == sizeof(pAuthData->abEthernetAddr));
#endif

    pAuthData->dwAuthDataSize = sizeof(XKERB_AD_XBOX);
    pAuthData->clientVersion = g_ClientVersion;
    pAuthData->dwTitleID = m_dwTitleId;
    pAuthData->dwTitleVersion = m_dwTitleVersion;
    pAuthData->dwTitleRegion = m_dwTitleGameRegion;

    for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
    {
        pAuthData->users[i] = m_rgLogonUsers[i].xuid; 
    }

    for (i=0; i<XONLINE_MAX_SERVICE_REQUEST; ++i)
    {
        if ( m_rgServiceContexts[i].serviceInfo.dwServiceID != XONLINE_INVALID_SERVICE )
        {
            pAuthData->dwServiceID[pAuthData->dwNumServices++] = m_rgServiceContexts[i].serviceInfo.dwServiceID;
        }
    }

    _XOnlineGetMachineID(&pAuthData->qwXboxID);
}

#endif

// ---------------------------------------------------------------------------------------
// CXoBase support routines
// ---------------------------------------------------------------------------------------

XOKERBINFO * CXoBase::XoKerbGetInfo(DWORD dwServiceId)
{
    return(((CXo *)this)->XkerbGetInfo(dwServiceId));
}

BOOL CXoBase::XoKerbBuildApReq(DWORD dwServiceId, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, BYTE * pbApReq, UINT * pcbApReq)
{
    return(NT_SUCCESS(((CXo *)this)->XkerbBuildApReq(dwServiceId, pliTime, pbSha, cbSha, pbApReq, pcbApReq)));
}

BOOL CXoBase::XoKerbCrackApRep(DWORD dwServiceId, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, BYTE * pbApRep, UINT cbApRep)
{
    return(NT_SUCCESS(((CXo *)this)->XkerbCrackApRep(dwServiceId, pliTime, pbSha, cbSha, pbApRep, cbApRep)));
}

#ifdef XONLINE_FEATURE_INSECURE

void CXoBase::XoKerbGetAuthData(XKERB_AD_XBOX * pAuthData)
{
    ((CXo *)this)->XkerbGetAuthData(pAuthData);
}

#endif
