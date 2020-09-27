// Debugger.cpp : Implementation of CDebugger
#include "stdafx.h"
#include "Debugger.h"

/////////////////////////////////////////////////////////////////////////////
// CDebugMonitor
CDebugger::CDebugger()
    {
    WSADATA version;
    WSAStartup(2, &version);
    sock = INVALID_SOCKET;
    }
CDebugger::~CDebugger()
    {
    if(sock == INVALID_SOCKET) Disconnect();
    WSACleanup();
    }

/*****************************************************************************

Routine Description:

    IsDataAvailable

    Checks to see if there is new data in the receive buffer

Arguments:

    none

Return Value:

    0 or SOCKET_ERROR - no data is available
    1 - data is available to recv

*****************************************************************************/
int CDebugger::IsDataAvailable(void)
    {
    FD_SET bucket;
    TIMEVAL timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 250000;

    bucket.fd_count = 1;
    bucket.fd_array[0] = sock;

    return select(0, &bucket, NULL, NULL, &timeout);
    }



STDMETHODIMP CDebugger::Connect(const BSTR ipAddress)
    {
    USES_CONVERSION;
    if(!ipAddress) return E_INVALIDARG;

	struct sockaddr_in dest;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
        {
        return E_FAIL;
        }

    dest.sin_family = AF_INET;
	dest.sin_port = htons(DEBUGGER_PORT);
	dest.sin_addr.s_addr = inet_addr(W2A(ipAddress));
    if(dest.sin_addr.s_addr == INADDR_NONE)
        {
        // ipAddress wasnt an IP address, we will try to resolve it
        BSTR addr;
        SearchForIP(ipAddress, &addr);
        Unicode2AnsiHack(addr);
	    dest.sin_addr.s_addr = inet_addr((char*)addr);
        SysFreeString(addr);
        }
	int err = connect(sock, (struct sockaddr *)&dest, sizeof(sockaddr_in));

    if(err == SOCKET_ERROR)
        {
        closesocket(sock);
        return E_FAIL;
        }

    return S_OK;
    }

STDMETHODIMP CDebugger::Disconnect(void)
    {
    closesocket(sock);

    return S_OK;
    }

STDMETHODIMP CDebugger::GetResponse(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    char buffer[1024];

    if(IsDataAvailable() == 1)
        {
        int err = recv(sock, buffer, 1024, 0);
        buffer[err] = '\0';
        *pVal = CreateBstrFromAnsi(buffer);
        }
    else
        {
        *pVal = CreateBstrFromAnsi("No Data is available\r\n");
        return E_PENDING;
        }

    return S_OK;
    }

STDMETHODIMP CDebugger::SendCommand(const BSTR cmd)
    {
    USES_CONVERSION;
    if(!cmd) return E_INVALIDARG;

    char *buffer = W2A(cmd);

    int err = send(sock, buffer, strlen(buffer), 0);

    return S_OK;
    }

STDMETHODIMP CDebugger::Test()
    {
    // TODO: Add your implementation code here

    return S_OK;
    }

STDMETHODIMP CDebugger::SearchForIP(BSTR machineName, BSTR *pVal)
    {
    USES_CONVERSION;

    typedef struct _NM
        {
        BYTE bRequest;
        BYTE cchName;
        char szName[256];
        } NM;

    struct sockaddr_in sinU;
    NM nm;
    SOCKET s;
    int cRetries;
    DWORD dwRetry = 2000;
    BOOL f;
    char *szName = W2A(machineName);
    
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s == INVALID_SOCKET)
        {
        *pVal = CreateBstrFromAnsi("");
        return E_FAIL;
        }

    f = TRUE;
    if(0 != setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&f, sizeof f))
        {
        closesocket(s);
        *pVal = CreateBstrFromAnsi("");
        return E_FAIL;
        }
    sinU.sin_family = AF_INET;
    sinU.sin_port = htons(DEBUGGER_PORT);
    sinU.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    nm.bRequest = 1;
    for(nm.cchName = 0; szName[nm.cchName]; ++nm.cchName)
        nm.szName[nm.cchName] = szName[nm.cchName];

    for(cRetries = 3; cRetries--; )
        {
        if(sendto(s, (char*)&nm, nm.cchName + 2, 0, (struct sockaddr *)&sinU, sizeof sinU) == nm.cchName + 2)
            {
            fd_set fds;
            struct timeval tv;
            int fSel;
            
            FD_ZERO(&fds);
            FD_SET(s, &fds);
            tv.tv_sec = dwRetry / 1000;
            tv.tv_usec = dwRetry % 1000;
            fSel = select(0, &fds, NULL, NULL, &tv);
            if(fSel > 0)
                {
                NM nmT;
                int cbAddr = sizeof sinU;
                // Got some data, is it for us?
                if(recvfrom(s, (char*)&nmT, sizeof nmT, 0, (struct sockaddr *)&sinU, &cbAddr) <= 0)
                    fSel = -1;
                else
                    {
                    if(nmT.bRequest == 2 && nmT.cchName == nm.cchName && !_strnicmp(nm.szName, nmT.szName, nm.cchName))
                        break; // Got it!
                    
                    ++cRetries; // We don't count this bogus data as a retry
                    }
                }
            if(fSel < 0) // Error, need to sleep
                Sleep(dwRetry);
            }
        else
            {
            Sleep(dwRetry);
            }
        }

    closesocket(s);
    if(cRetries < 0)
        {
        *pVal = CreateBstrFromAnsi("");
        return S_OK;
        }

    // We have our answer
    *pVal = CreateBstrFromAnsi(inet_ntoa(sinU.sin_addr));

    return S_OK;
    }

STDMETHODIMP CDebugger::Test2(int val/*=22*/)
    {
    // TODO: Add your implementation code here

    int tool = val;



    return S_OK;
    }

