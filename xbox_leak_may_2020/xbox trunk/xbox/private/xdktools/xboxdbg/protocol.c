/*
**
** protocol.c
**
** Protocol send/receive stuff for the debug protocol
**
*/

#include "precomp.h"
#include "resources.h"

#define XBOX_NAME_KEY "Software\\Microsoft\\XboxSDK"
#define XBOX_NAME_VALUE "XboxName"

SCI g_sci;
BOOL fWSAInit;
WSADATA wsad;

void EnsureWSAInited(void)
{
	if(!fWSAInit)
		fWSAInit = WSAStartup(2, &wsad) == 0;
}

HRESULT HrUseSharedConnection(SCI *psci, BOOL fShare)
{
	if(!psci->fAllowSharing == !fShare)
		return XBDM_NOERR;
	EnterCriticalSection(&psci->csSharedConn);
	/* If we're turning off sharing and the shared connection is not in use,
	 * then we'll close it, and if it is in use, we'll orphan it */
	if(!fShare && psci->pdconShared) {
        if(psci->tidShared == 0)
		    DmCloseConnection(psci->pdconShared);
		psci->pdconShared = NULL;
	}
	psci->fAllowSharing = fShare;
	LeaveCriticalSection(&psci->csSharedConn);
	return XBDM_NOERR;
}

HRESULT DmUseSharedConnection(BOOL fShare)
{
    return HrUseSharedConnection(&g_sci, fShare);
}

HRESULT DmGetXboxName(LPSTR sz, LPDWORD pcch)
{
	HRESULT hr;
	DWORD ich;

	if(!sz || !pcch)
		return E_INVALIDARG;

	/* Make sure it's appropriately set first */
	hr = DmSetXboxName(NULL);
	if(FAILED(hr))
		return hr;
	for(ich = 0; ich < *pcch; ++ich)
		if(!(sz[ich] = g_sci.szXboxName[ich])) {
			*pcch = ich;
			return XBDM_NOERR;
		}
	sz[--*pcch] = 0;
	return XBDM_BUFFER_TOO_SMALL;
}

HRESULT DmSetXboxName(LPCSTR sz)
{
	HRESULT hr;
	HKEY h;

    g_sci.ulXboxIPAddr = 0;
	if(!sz) {
		/* We're being asked to validate the name that's in the global.  If
		 * no data is in the global, we'll check the registry */
		DWORD dwType;
		DWORD cb;

		if(*g_sci.szXboxName)
			return XBDM_NOERR;

		if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, XBOX_NAME_KEY, 0,
				KEY_QUERY_VALUE, &h))
			return XBDM_NOXBOXNAME;
		cb = sizeof g_sci.szXboxName;
		if(ERROR_SUCCESS != RegQueryValueEx(h, XBOX_NAME_VALUE, 0, &dwType,
				g_sci.szXboxName, &cb) || dwType != REG_SZ) {
			*g_sci.szXboxName = 0;
			hr = XBDM_NOXBOXNAME;
		} else
			hr = XBDM_NOERR;
		RegCloseKey(h);
	} else {
		DWORD cch;
	
		cch = strlen(sz);
		if(cch >= MAX_XBOX_NAME)
			return E_INVALIDARG;
		strcpy(g_sci.szXboxName, sz);
		if(ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, XBOX_NAME_KEY, 0,
				NULL, 0, KEY_SET_VALUE, NULL, &h, NULL)) {
			RegSetValueEx(h, XBOX_NAME_VALUE, 0, REG_SZ, g_sci.szXboxName,
                1 + cch);
			RegCloseKey(h);
		}
		hr = XBDM_NOERR;
	}

	return hr;
}

HRESULT DmSetXboxNameNoRegister(LPCSTR sz)
{
    if(!sz || strlen(sz) >= MAX_XBOX_NAME)
        return E_INVALIDARG;
    g_sci.ulXboxIPAddr = 0;
    strcpy(g_sci.szXboxName, sz);
    return XBDM_NOERR;
}

HRESULT HrFromStatus(LPSTR sz)
{
	int nCode;

	nCode = (sz[0] - '2') * 100 + (sz[1] - '0') * 10 + (sz[2] - '0');
	if(*sz == '4')
		/* An error code */
		return MAKE_HRESULT(1, FACILITY_XBDM, nCode - 200);
	else
		return MAKE_HRESULT(0, FACILITY_XBDM, nCode);
}

typedef struct _NM {
	BYTE bRequest;
	BYTE cchName;
	char szName[255];
} NM;

BOOL FNmQuery(NM *pnmIn, NM *pnmOut, struct sockaddr_in *psin, BOOL fBroadcast)
{
	struct sockaddr_in sinU;
	SOCKET s;
	int cRetries;
	const DWORD dwRetry = 500;
    NM *pnmResult;
    NM nm;
    BOOL f;

    /* Set up the UDP socket */
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(s == INVALID_SOCKET)
		return E_FAIL;
    if(fBroadcast) {
	    f = TRUE;
	    if(0 != setsockopt(s, SOL_SOCKET, SO_BROADCAST, (PVOID)&f, sizeof f)) {
		    closesocket(s);
		    return E_FAIL;
        }
	    sinU.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    } else
	    sinU.sin_addr.s_addr = psin->sin_addr.s_addr;
	sinU.sin_family = AF_INET;
	sinU.sin_port = htons(DEBUGGER_PORT);

    /* Set up the result */
    pnmResult = pnmOut ? pnmOut : &nm;

    /* Keep retrying every 1/2 second */
    f = FALSE;
	for(cRetries = 4; !f && cRetries--; ) {
        DWORD tickTry = GetTickCount() + dwRetry;
		if(sendto(s, (PVOID)pnmIn, pnmIn->cchName + 2, 0,
			(struct sockaddr *)&sinU, sizeof sinU) == pnmIn->cchName + 2)
		{
			int fSel;

            do {
                int tickRetry;

                /* Look for a response.  If we've already waited for a full
                 * half second, then don't bother any longer */
                tickRetry = tickTry - GetTickCount();
                if(tickRetry <= 0)
                    fSel = 0;
                else {
			        fd_set fds;
			        struct timeval tv;

			        FD_ZERO(&fds);
			        FD_SET(s, &fds);
			        tv.tv_sec = tickRetry / 1000;
			        tv.tv_usec = (tickRetry % 1000) * 1000;
			        fSel = select(0, &fds, NULL, NULL, &tv);
                }

			    if(fSel > 0) {
				    NM nmT;
				    int cbAddr = sizeof sinU;
				    /* Got some data, is it for us? */
				    if(recvfrom(s, (PVOID)pnmResult, sizeof *pnmResult, 0,
						    (struct sockaddr *)&sinU, &cbAddr) <= 0)
					    fSel = -1;
				    else {
					    if(pnmResult->bRequest == 2 && (pnmOut ||
                            (pnmResult->cchName == pnmIn->cchName &&
                            !_strnicmp(pnmIn->szName, pnmResult->szName,
                            pnmIn->cchName))))
                        {
						    /* Got it! */
                            f = TRUE;
                            fSel = 0;
						    break;
                        }
				    }
			    }
                if(fSel < 0) {
				    /* Error, need to sleep */
				    Sleep(tickRetry);
                    fSel = 0;
                }
            } while(fSel);
		} else {
			int err = WSAGetLastError();
			/* Send failed, need to sleep */
			Sleep(dwRetry);
		}
	}

	closesocket(s);

    /* We have our answer */
    if(f)
	    psin->sin_addr.s_addr = sinU.sin_addr.s_addr;
	return f;
}

HRESULT HrResolveXboxName(LPCSTR szName, struct sockaddr_in *psin)
{
	NM nm;

    /* Mark the packet as a name request */
    nm.bRequest = 1;
	for(nm.cchName = 0; szName[nm.cchName]; ++nm.cchName)
		nm.szName[nm.cchName] = szName[nm.cchName];
    return FNmQuery(&nm, NULL, psin, TRUE) ? XBDM_NOERR : XBDM_CANNOTCONNECT;
}

HRESULT HrGetNameOfXbox(PDM_CONNECTION s, LPSTR szName, LPDWORD lpdwSize,
    BOOL fResolvable)
{
    char sz[512];
    DWORD cch;
    NM nmWild, nmName;
    struct sockaddr_in sin, sinT;
    HRESULT hr;
    int cb;

    if(!szName || !lpdwSize)
        return E_INVALIDARG;

    /* First, gather the box's IP address */
    cb = sizeof sin;
    if(0 != getpeername(s->s, (struct sockaddr *)&sin, &cb))
        return E_UNEXPECTED;

    /* First ask the box for its name */
    cch = sizeof sz;
    hr = DmSendCommand(s, "DEBUGNAME", sz, &cch);
    if(SUCCEEDED(hr)) {
        /* Stick this name in the NM */
        if(cch - 5 >= sizeof nmName.szName)
            nmName.cchName = sizeof nmName.szName;
        else
            nmName.cchName = (UCHAR)(cch - 5);
        memcpy(nmName.szName, sz + 5, nmName.cchName);
    } else {
        /* The box apparently doesn't know how to answer its name.  See if
         * we can use the UDP scheme to query the box for its name by
         * wildcard matching */
        nmWild.bRequest = 3;
        if(!FNmQuery(&nmWild, &nmName, &sin, FALSE))
            return XBDM_UNDEFINED;
    }
    if((ULONG)nmName.cchName + 1 >= *lpdwSize)
        return XBDM_BUFFER_TOO_SMALL;
    memcpy(szName, nmName.szName, nmName.cchName);
    szName[nmName.cchName] = 0;
    *lpdwSize = nmName.cchName;

    /* If we require that the name be resolvable, make sure we can resolve it
     * to the same address */
    if(fResolvable) {
        nmName.bRequest = 1;
        if(!FNmQuery(&nmName, NULL, &sinT, TRUE) || sin.sin_addr.s_addr !=
                sinT.sin_addr.s_addr)
            return XBDM_CANNOTCONNECT;
    }

    return XBDM_NOERR;
}

HRESULT HrResolveNameIP(LPCSTR szName, struct sockaddr_in *psin)
{
    HRESULT hr;

	EnsureWSAInited();
	psin->sin_addr.s_addr = inet_addr(szName);
	if(psin->sin_addr.s_addr == INADDR_NONE)
		hr = HrResolveXboxName(szName, psin);
    else
        hr = XBDM_NOERR;
	return hr;
}

PDM_CONNECTION PdconWrapSocket(SOCKET s)
{
	PDM_CONNECTION pdcon;

	pdcon = LocalAlloc(LMEM_FIXED, sizeof *pdcon);
	if(pdcon) {
        memset(pdcon, 0, sizeof *pdcon);
		pdcon->s = s;
		pdcon->cbBuf = pdcon->ibBuf = 0;
        pdcon->grbitFlags = 0;
	}
	return pdcon;
}

HRESULT HrOpenConnectionCore(SCI *psci, PDM_CONNECTION *ppdcon)
{
    struct sockaddr_in sin;
	HRESULT hr;
	PDM_CONNECTION pdcon;
    BOOL fNonBlock;
    BOOL fError;
    struct timeval tv;
    fd_set fds;

    /* Special check to see whether we need to update from the registry */
    if(psci == &g_sci) {
        hr = DmSetXboxName(NULL);
        if(FAILED(hr))
            return hr;
    }

    if(psci->ulXboxIPAddr && psci->fCacheAddr)
        sin.sin_addr.s_addr = psci->ulXboxIPAddr;
    else {
        hr = HrResolveNameIP(psci->szXboxName, &sin);
        if(FAILED(hr))
            return hr;
        psci->ulXboxIPAddr = sin.sin_addr.s_addr;
    }

	pdcon = PdconWrapSocket(INVALID_SOCKET);
	if(!pdcon)
		return E_OUTOFMEMORY;

    /* Set up the socket */
    pdcon->psci = psci;
	pdcon->s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(pdcon->s == INVALID_SOCKET) {
		hr = XBDM_CANNOTCONNECT;
		goto error;
	}

    /* If we have a timeout value, make the socket nonblocking so we can
     * select on the connect */
    if(psci->dwConnectionTimeout) {
        fNonBlock = TRUE;
        if(0 != ioctlsocket(pdcon->s, FIONBIO, &fNonBlock))
            goto error;
    }

    /* Now attempt to connect the socket */
	sin.sin_family = AF_INET;
	sin.sin_port = htons(DEBUGGER_PORT);
	if(0 != connect(pdcon->s, (struct sockaddr *)&sin, sizeof sin)) {
        if(psci->dwConnectionTimeout && WSAGetLastError() == WSAEWOULDBLOCK) {
            /* Attempt to wait for the connection */
            tv.tv_sec = psci->dwConnectionTimeout / 1000;
            tv.tv_usec = (psci->dwConnectionTimeout % 1000) * 1000;
            FD_ZERO(&fds);
            FD_SET(pdcon->s, &fds);
            fError = select(0, NULL, &fds, NULL, &tv) <= 0;
        } else
            fError = TRUE;
        if(fError) {
		    hr = XBDM_CANNOTCONNECT;
            goto error;
        }
	}

    /* If we haven't already made the socket nonblocking, we do so now */
    if(!psci->dwConnectionTimeout) {
        fNonBlock = TRUE;
        if(0 != ioctlsocket(pdcon->s, FIONBIO, &fNonBlock))
            goto error;
    }

	*ppdcon = pdcon;
	return XBDM_NOERR;
error:
    if(pdcon->s != INVALID_SOCKET)
        closesocket(pdcon->s);
	LocalFree(pdcon);
	return hr;
}

HRESULT HrOpenConnection(SCI *psci, PDM_CONNECTION *ppdcon, BOOL fRequireAccess)
{
	HRESULT hr, hrT;
    char sz[256];
    DWORD cch;
    ULARGE_INTEGER luBoxId;
    ULARGE_INTEGER luConnectNonce;

	if(!ppdcon)
		return E_INVALIDARG;

    hr = HrOpenConnectionCore(psci, ppdcon);
    if(FAILED(hr))
        return hr;

    cch = sizeof sz;
	hr = DmReceiveStatusResponse(*ppdcon, sz, &cch);
	if(FAILED(hr)) {
		DmCloseConnection(*ppdcon);
		return hr;
	}

    /* If this box has security enabled, we need to handshake */
    if(FGetQwordParam(sz, "BOXID", &luBoxId) &&
        FGetQwordParam(sz, "NONCE", &luConnectNonce))
    {
        hrT = HrAuthenticateUser(*ppdcon, &luBoxId, &luConnectNonce);
        if(fRequireAccess)
            hr = hrT;
        if(FAILED(hr)) {
            DmCloseConnection(*ppdcon);
            return hr;
        }
    }

	return XBDM_NOERR;
}

HRESULT HrOpenNonprivilegedConnection(SCI *psci, PDM_CONNECTION *ppdcon)
{
	HRESULT hr;

	if(!ppdcon)
		return E_INVALIDARG;

    hr = HrOpenConnectionCore(psci, ppdcon);
    if(FAILED(hr))
        return hr;

	hr = DmReceiveStatusResponse(*ppdcon, NULL, 0);
	if(FAILED(hr))
		DmCloseConnection(*ppdcon);

    return hr;
}

HRESULT HrOpenSecureConnection(SCI *psci, PDM_CONNECTION *ppdcon, LPCSTR szPasswd)
{
    PDM_CONNECTION pdcon;
    HRESULT hr;
    char sz[256];
    DWORD cch;
    ULARGE_INTEGER luBoxId;
    ULARGE_INTEGER luConnectNonce;
    ULARGE_INTEGER luResponse;
    ULARGE_INTEGER luPasswd;

    if(!ppdcon) {
        /* We're being asked to open a secure shared connection.  If the
         * shared connection is already secure, then we'll just use it; if
         * not, we get ready to open it */
        EnterCriticalSection(&psci->csSharedConn);
        if(psci->fSecureConnection) {
            hr = XBDM_NOERR;
            goto errorwithoutconn;
        }
        if(!psci->fAllowSharing) {
            hr = E_INVALIDARG;
            goto errorwithoutconn;
        }
        if(psci->pdconShared) {
            if(psci->tidShared) {
                /* The shared connection is presently in use.  We'll leave it
                 * open but mark it nonshared */
                psci->tidShared = 0;
            } else {
                /* The shared connection is not presently in use, so we can
                 * close it */
                DmCloseConnection(psci->pdconShared);
            }
            psci->pdconShared = NULL;
        }
    }

    hr = HrOpenConnectionCore(psci, &pdcon);
    if(FAILED(hr))
        goto errorwithoutconn;

    cch = sizeof sz;
	hr = DmReceiveStatusResponse(pdcon, sz, &cch);
	if(FAILED(hr)) {
error:
		DmCloseConnection(pdcon);
errorwithoutconn:
        if(!ppdcon)
            LeaveCriticalSection(&psci->csSharedConn);
		return hr;
	}

    /* If this box has security enabled, we need to send the admin passwd */
    if(FGetQwordParam(sz, "BOXID", &luBoxId) &&
        FGetQwordParam(sz, "NONCE", &luConnectNonce))
    {
        /* Figure out the password */
        luPasswd.QuadPart = 0;
        XBCHashData(&luPasswd, szPasswd, strlen(szPasswd));

        /* Calculate the response */
        XBCCross(&luPasswd, &luConnectNonce, &luResponse);

        /* See if we can authenticate */
        sprintf(sz, "AUTHUSER ADMIN RESP=0q%08x%08x", luResponse.HighPart,
            luResponse.LowPart);
        hr = DmSendCommand(pdcon, sz, NULL, 0);
        if(FAILED(hr))
            goto error;
    }

    /* We now have our secure connection.  If this is supposed to be the
     * shared connection, we mark it shared */
    if(!ppdcon) {
        psci->pdconShared = pdcon;
        psci->fSecureConnection = TRUE;
        LeaveCriticalSection(&psci->csSharedConn);
    }

    return XBDM_NOERR;
}

HRESULT HrGetAltAddress(SCI *psci, LPDWORD lpdw)
{
    PDM_CONNECTION s;
    HRESULT hr;
    char sz[64];
    DWORD cch;

    hr = HrDoOpenSharedConnection(psci, &s);
    if(FAILED(hr))
        return hr;
    cch = sizeof sz;
    hr = DmSendCommand(s, "ALTADDR", sz, &cch);
    if(hr == XBDM_NOERR) {
        if(!FGetDwParam(sz, "addr", lpdw))
            hr = XBDM_UNDEFINED;
    } else if(SUCCEEDED(hr)) {
        TerminateConnection(s);
        hr = E_UNEXPECTED;
    }
    DoCloseSharedConnection(psci, s);
    return hr;
}

HRESULT DmOpenConnection(PDM_CONNECTION *ppdcon)
{
    return HrOpenConnection(&g_sci, ppdcon, TRUE);
}

HRESULT DmOpenSecureConnection(PDM_CONNECTION *ppdcon, LPCSTR szPasswd)
{
    return HrOpenSecureConnection(&g_sci, ppdcon, szPasswd);
}

HRESULT DmCloseConnection(PDM_CONNECTION pdcon)
{
	if(!pdcon)
		return E_INVALIDARG;

	if(pdcon->s != INVALID_SOCKET) {
		DmSendBinary(pdcon, "BYE\015\012", 5);
		closesocket(pdcon->s);
	}
	LocalFree(pdcon);
	return XBDM_NOERR;
}

void TerminateConnection(PDM_CONNECTION s)
{
    closesocket(s->s);
    s->s = INVALID_SOCKET;
}

HRESULT HrDoOpenSharedConnection(SCI *psci, PDM_CONNECTION *ppdcon)
{
	HRESULT hr;
	BOOL fCanShare = FALSE;

	*ppdcon = NULL;
	if(psci->fAllowSharing) {
		EnterCriticalSection(&psci->csSharedConn);
		/* In order to successfully share, the shared connection can't
		 * be in use */
        if(psci->pdconShared && psci->pdconShared->s == INVALID_SOCKET) {
			/* Stale connection; get rid of it */
			DmCloseConnection(psci->pdconShared);
            psci->pdconShared = NULL;
        }
		if(psci->pdconShared == NULL) {
			/* No current connection, so we'll open one */
			fCanShare = TRUE;
			hr = HrOpenConnection(psci, &psci->pdconShared, FALSE);
			if(FAILED(hr))
				psci->pdconShared = NULL;
		} else if(psci->tidShared == 0) {
			fCanShare = TRUE;
			hr = XBDM_NOERR;
		}
		if(fCanShare && psci->pdconShared) {
			psci->tidShared = GetCurrentThreadId();
			*ppdcon = psci->pdconShared;
		}
		LeaveCriticalSection(&psci->csSharedConn);
	}

	if(!fCanShare)
		hr = HrOpenConnection(psci, ppdcon, FALSE);
	return hr;
}

void DoCloseSharedConnection(SCI *psci, PDM_CONNECTION pdcon)
{
	/* We want to close this connection if it's not the shared connection, or
	 * if we're going to disallow sharing.  If it is the shared connection,
	 * we'll just mark it not in use */

	EnterCriticalSection(&psci->csSharedConn);
	if(!pdcon) {
		/* The shared connection is toast, so we're going to get rid of it.
         * If it's in use, we'll orphan it; if not, we'll just close it */
		HrUseSharedConnection(psci, FALSE);
		HrUseSharedConnection(psci, TRUE);

        /* The cached IP address is potentially invalid now, so flush it */
        psci->ulXboxIPAddr = 0;
	} else if(pdcon == psci->pdconShared) {
		if(psci->fAllowSharing)
			pdcon = NULL;
		else
			psci->pdconShared = NULL;
		psci->tidShared = 0;
	}
	LeaveCriticalSection(&psci->csSharedConn);

	if(pdcon)
		DmCloseConnection(pdcon);
}

void CloseThreadConnection(void)
{
    // BUGBUG -- should find all potential SCIs
	if(GetCurrentThreadId() == g_sci.tidShared)
		CloseSharedConnection(g_sci.pdconShared);
}

HRESULT DmDedicateConnection(PDM_CONNECTION pdcon, LPCSTR szHandler)
{
	char sz[256];
	LPCSTR psz;

	if(szHandler) {
		sprintf(sz, "DEDICATE HANDLER=%s", szHandler);
		psz = sz;
	} else
		psz = "DEDICATE GLOBAL";
	return DmSendCommand(pdcon, psz, NULL, NULL);
}

HRESULT DmSendBinary(PDM_CONNECTION pdcon, LPCVOID pv, DWORD cb)
{
	int cbT;
    int err;
    struct timeval tv;
    struct fd_set fds;

	if (!pdcon || !pv)
		return E_INVALIDARG;

    /* Because calls to send require only room in the send buffer but not
     * an actual acknowledgement, we don't need to observe the standard
     * timeout.  However, we could be in a situation where the box is gone
     * and the send buffer is full.  In that case, the socket will time out
     * and reset inside of winsock.  We'll wait arbitrarily long for the
     * send to complete, but we'll wait in one-second increments so we can
     * properly detect a dead socket */

    if(pdcon->s == INVALID_SOCKET)
        return XBDM_CONNECTIONLOST;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
	while(cb) {
		cbT = send(pdcon->s, pv, cb, 0);
		if(cbT <= 0) {
            err = WSAGetLastError();
			if(err == WSAEINTR)
				continue;
            else if(err == WSAEWOULDBLOCK) {
                FD_ZERO(&fds);
                FD_SET(pdcon->s, &fds);
                if(select(0, NULL, &fds, NULL, NULL) >= 0)
                    continue;
            }
			/* Connection lost */
            break;
		}
		pv = (LPCVOID)((BYTE*)pv + cbT);
		cb -= cbT;
	}
    if(cb) {
		/* Connection lost */
		if(pdcon->s != INVALID_SOCKET)
            TerminateConnection(pdcon);
		return XBDM_CONNECTIONLOST;
    }
	return XBDM_NOERR;
}

int CbReceiveBinary(PDM_CONNECTION pdcon, LPVOID pv, DWORD cb)
{
	int cbT;
    int err;
    struct timeval tv, *ptv;
    struct fd_set fds;

    /* Try to receive data */
    cbT = recv(pdcon->s, pv, cb, 0);

    /* If we got good data, send it back */
    if(cbT > 0)
        return cbT;

    /* Figure out what went wrong */
    err = WSAGetLastError();

    /* If we were interrupt, we won't consider it to be fatal */
    if(err == WSAEINTR)
        return 0;

    if(err == WSAEWOULDBLOCK) {
        /* No data yet, so wait for it */
        FD_ZERO(&fds);
        FD_SET(pdcon->s, &fds);
        if(pdcon->psci && pdcon->psci->dwConversationTimeout) {
            ptv = &tv;
            ptv->tv_sec = pdcon->psci->dwConversationTimeout / 1000;
            ptv->tv_usec = (pdcon->psci->dwConversationTimeout % 1000) * 1000;
        } else
            ptv = NULL;
        if(select(0, &fds, NULL, NULL, ptv) > 0)
            /* Data is ready */
            return 0;
    }

    /* Connection lost */
    TerminateConnection(pdcon);
    return -1;
}

HRESULT DmReceiveBinary(PDM_CONNECTION pdcon, LPVOID pv, DWORD cb, LPDWORD pcbRet)
{
	DWORD cbRet = 0;
	int cbT;
    int err;
    struct timeval tv;
    struct fd_set fds;

	if (!pdcon || !pv)
		return E_INVALIDARG;

    if(pdcon->s == INVALID_SOCKET)
        return XBDM_CONNECTIONLOST;
	if(cb) {
		/* Suck data from the buffer first */
		cbT = pdcon->cbBuf - pdcon->ibBuf;
		if((DWORD)cbT > cb)
			cbT = cb;
		if(cbT) {
			memcpy(pv, pdcon->rgbBuf + pdcon->ibBuf, cbT);
			pdcon->ibBuf += cbT;
			pv = (LPVOID)((BYTE *)pv + cbT);
			cbRet += cbT;
			cb -= cbT;
		}
		while(cb) {
			cbT = CbReceiveBinary(pdcon, pv, cb);
			if(cbT < 0)
                break;
			pv = (LPVOID)((BYTE*)pv + cbT);
			cbRet += cbT;
			cb -= cbT;
		}
	}

	if(pcbRet)
		*pcbRet = cbRet;
	return cb ? XBDM_CONNECTIONLOST : XBDM_NOERR;
}

HRESULT DmSendCommand(PDM_CONNECTION pdcon, LPCSTR szCommand, LPSTR szResult,
	LPDWORD pcchResult)
{
	int i;
	DWORD cch;
	HRESULT hr;
	char szT[512];
	BOOL fShared = !pdcon;

	if(szResult && !pcchResult)
		return E_INVALIDARG;

	if(fShared) {
		/* Let's see if we can find a shared connection */
		hr = HrOpenSharedConnection(&pdcon);
		if(FAILED(hr))
			return hr;
	}

    if(!pdcon)
		return XBDM_CONNECTIONLOST;

    if(!szCommand) {
        /* We just wanted to tickle the shared connection */
        if(fShared)
            CloseSharedConnection(pdcon);
        return XBDM_NOERR;
    }

	cch = strlen(szCommand);
	if(cch + 3 < sizeof szT) {
		/* Send the line + crlf in one packet */
		memcpy(szT, szCommand, cch);
		szT[cch] = '\015';
		szT[cch+1] = '\012';
		hr = DmSendBinary(pdcon, szT, cch + 2);
	} else {
		hr = DmSendBinary(pdcon, szCommand, cch);
		if(SUCCEEDED(hr))
			hr = DmSendBinary(pdcon, "\015\012", 2);
	}

	if(SUCCEEDED(hr))
		hr = DmReceiveStatusResponse(pdcon, szResult, pcchResult);

	if(fShared)
		CloseSharedConnection(pdcon);
	return hr;
}

HRESULT DmReceiveStatusResponse(PDM_CONNECTION pdcon, LPSTR sz, LPDWORD pcch)
{
	char szBuf[256];
	int cch;
	HRESULT hr;

	if(sz && !pcch)
		return E_INVALIDARG;

	if(!sz) {
		sz = szBuf;
		cch = sizeof szBuf;
		pcch = &cch;
	}

	hr = DmReceiveSocketLine(pdcon, sz, pcch);
	if(FAILED(hr))
		return hr;

	/* Now we have a three-digit result code.  We'll return an HRESULT with the
	 * three digits of the code (decimal) as the code portion, and we'll
	 * indicate an error if it's an error code */
 	return HrFromStatus(sz);
}

HRESULT DmReceiveSocketLine(PDM_CONNECTION pdcon, LPSTR szResult, LPDWORD pcch)
{
	char ch;
	int cch;

	if (!szResult || !pcch || *pcch == 0 || !pdcon)
		return E_INVALIDARG;

	cch = *pcch - 1;

	/* We can only receive one line, so we'll process a byte at a time and
	 * stop at CRLF.  To be safe, we'll stop at LF and ignore CR */
	do {
		/* Make sure we have some data */
		while(pdcon->ibBuf >= pdcon->cbBuf) {
			pdcon->ibBuf = 0;
			pdcon->cbBuf = CbReceiveBinary(pdcon, pdcon->rgbBuf,
                sizeof pdcon->rgbBuf);
            if((int)pdcon->cbBuf < 0)
				return XBDM_CONNECTIONLOST;
		}
		ch = pdcon->rgbBuf[pdcon->ibBuf++];
		if(ch == '\015')
			continue;
		if(ch != '\012' && cch) {
			*szResult++ = ch;
			--cch;
		}
	} while(ch != '\012');
	*szResult = 0;
	*pcch -= cch + 1;
	return XBDM_NOERR;
}

HRESULT DmReceiveBinaryDataSize(PDM_CONNECTION pdcon, LPDWORD pcb)
{
    if (!pdcon || !pcb)
        return E_INVALIDARG;

	return DmReceiveBinary(pdcon, pcb, sizeof *pcb, NULL);
}

#if 0
BOOL FGetLineData(LPCSTR sz, LPCSTR szKey, const char **ppchData)
{
	int cch = strlen(szKey);

	if(_strnicmp(sz, szKey, cch))
		return FALSE;
	if(sz[cch] == '=') {
		if(ppchData)
			*ppchData = sz + cch;
		return TRUE;
	}
	if(sz[cch] == 0) {
		if(ppchData)
			*ppchData = NULL;
		return TRUE;
	}
	return FALSE;
}
#endif

__inline BOOL FIsSpace(char ch)
{
	return !ch || isspace(ch);
}

const char *PchGetParam(LPCSTR szCmd, LPCSTR szKey, BOOL fNeedValue,
	BOOL fNoCommand)
{
	const char *pchTok;
	int cchTok;
	BOOL fQuote = FALSE;

	/* Skip the command, unless there isn't one */
	pchTok = szCmd;
	if(!fNoCommand)
		while(!FIsSpace(*pchTok))
			++pchTok;

	while(*pchTok) {
		/* Skip leading spaces */
		while(*pchTok && FIsSpace(*pchTok))
			++pchTok;
		if(!*pchTok)
			return NULL;
        for(cchTok = 0; !FIsSpace(pchTok[cchTok]); ++cchTok) {
            if(pchTok[cchTok] == '=') {
                if(!_strnicmp(szKey, pchTok, cchTok))
				    return pchTok + cchTok + 1; /* Skip the '=' */
                break;
            }
        }
		/* If we didn't see the '=' we need to check anyway */
		if(!fNeedValue && pchTok[cchTok] != '=' && !_strnicmp(szKey, pchTok,
                cchTok) && !szKey[cchTok])
			return pchTok + cchTok;
		/* No match, so we need to skip past the value */
		pchTok += cchTok;
		while(*pchTok && (!FIsSpace(*pchTok) || fQuote))
			if(*pchTok++ == '"')
				fQuote = !fQuote;
	}
	return NULL;
}

BOOL FGetSzParam(LPCSTR szLine, LPCSTR szKey, LPSTR szBuf)
{
	BOOL fQuote;
	LPCSTR pch = PchGetParam(szLine, szKey, TRUE, TRUE);

	if(!pch)
		return FALSE;
	fQuote = FALSE;
	while(*pch && (fQuote || !isspace(*pch))) {
		if(*pch == '"') {
			fQuote = !fQuote;
			++pch;
		} else
			*szBuf++ = *pch++;
	}
	*szBuf = 0;
	return TRUE;
}

BOOL FGetDwParamFromSz(LPCSTR sz, DWORD *pdw)
{
	if(!sz)
		return FALSE;
	if(sz[0] == '0') {
		if(sz[1] == 'x') {
			sscanf(sz+2, "%x", pdw);
			return TRUE;
		}
		/* Don't support octal for now */
		return FALSE;
	}
	*pdw = atoi(sz);
	return TRUE;
}

BOOL FGetQwordParam(LPCSTR szLine, LPCSTR szKey, ULARGE_INTEGER *plu)
{
    int cch;
    char sz[32];
    LPCSTR pch;

    if(!FGetSzParam(szLine, szKey, sz))
        return FALSE;
    sz[sizeof sz - 1] = 0;

    /* Verify the 0q prefix */
    if(sz[0] != '0' || sz[1] != 'q')
        return FALSE;
    /* Make sure we have a bunch of hex characters */
    for(cch = 2; cch < sizeof sz && !FIsSpace(sz[cch]); ++cch) {
        if(!(sz[cch] >= '0' && sz[cch] <= '9' ||
                sz[cch] >= 'A' && sz[cch] <= 'F' ||
                sz[cch] >= 'a' && sz[cch] <= 'f'))
            return FALSE;
    }
    cch -= 2;
    if(cch <= 0)
        return FALSE;

    /* Move the text out to the end of the string and fill the preceding
     * characters with zeroes */
    memmove(&sz[sizeof sz - 1 - cch], &sz[2], cch);
    memset(sz, '0', sizeof sz - 1 - cch);

    /* Now parse out the two dwords */
    sscanf(&sz[sizeof sz - 9], "%x", &plu->LowPart);
    sz[sizeof sz - 9] = 0;
    sscanf(&sz[sizeof sz - 17], "%x", &plu->HighPart);
    return TRUE;
}

#ifndef XBDBGS  // DmTranslateError is not available in the static version of the lib
HRESULT DmTranslateErrorA(HRESULT hr, LPSTR lpBuffer, int nBufferMax)
{
    UINT    resourceId;
    int     len;
    HRESULT hrRes = XBDM_NOERR;

    //Parameter check.
    if( (!lpBuffer) || (nBufferMax <= 0))
    {
        return E_INVALIDARG;
    }

    //Get the resource ID
    resourceId = FGetErrorStringResourceId(hr);
    if(0==resourceId) 
    {
        hrRes = XBDM_NOERRORSTRING;
        resourceId = IDC_XBDM_NOERRORSTRING;
    }

    //Load the string
    len = LoadStringA(hXboxdbg, resourceId, lpBuffer, nBufferMax);
    if(0==len)
    {
        hrRes = HRESULT_FROM_WIN32(GetLastError());
    }
    return hrRes;
}

HRESULT DmTranslateErrorW(HRESULT hr, LPWSTR lpBuffer, int nBufferMax)
{
    UINT    resourceId;
    int     len;
    HRESULT hrRes = S_OK;

    //Parameter check.
    if( (!lpBuffer) || (nBufferMax <= 0))
    {
        return E_INVALIDARG;
    }
    
    //Get the resource ID    
    resourceId = FGetErrorStringResourceId(hr);
    if(0==resourceId) 
    {
        hrRes = XBDM_NOERRORSTRING;
        resourceId = IDC_XBDM_NOERRORSTRING;
    }

    //Load the string
    len = LoadStringW(hXboxdbg, resourceId, lpBuffer, nBufferMax);
    if(0==len)
    {
        hrRes = HRESULT_FROM_WIN32(GetLastError());
    }
    return hrRes;
}

UINT FGetErrorStringResourceId(HRESULT hr)
{
	switch(hr) {
	case E_FAIL:
	case E_UNEXPECTED:
		return IDC_E_UNEXPECTED;
	case E_INVALIDARG:
		return IDC_E_INVALIDARG;
    default:
        if(FAILED(hr) && HRESULT_FACILITY(hr) == FACILITY_XBDM)
            return 0x8000 | (hr & 0xfff);
        break;
	}
    return 0;
}
#endif //XBDBGS

HRESULT HrDoOneLineCmd(PDM_CONNECTION s, LPCSTR sz)
{
	HRESULT hr;

	hr = DmSendCommand(s, sz, NULL, 0);
	if(hr == XBDM_READYFORBIN || hr == XBDM_MULTIRESPONSE ||
		hr == XBDM_BINRESPONSE)
	{
		TerminateConnection(s);
		hr = E_UNEXPECTED;
	}
	return hr;
}

HRESULT HrOneLineCmd(LPCSTR sz)
{
	PDM_CONNECTION s;
	HRESULT hr;

	hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = HrDoOneLineCmd(s, sz);
    	CloseSharedConnection(s);
	}
	return hr;
}
