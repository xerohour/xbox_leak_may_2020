/*
**
** notify.c
**
** Receive notifications from the Xbox and pass them along
**
*/

#include "precomp.h"

static PDM_CONNECTION pdconNotify;
static SOCKET sockPersist = INVALID_SOCKET;
static HANDLE hthrNotifier;
static CRITICAL_SECTION csNotifyList;
static CRITICAL_SECTION csEnsureNotify;
static HANDLE hevtSockData;
static HANDLE hevtPersistConn;
static HANDLE hevtDropConnection;
static HANDLE hevtTerminate;
static DWORD dwExecState = DMN_EXEC_REBOOT;
static USHORT usPersistPort;
static CRITICAL_SECTION csAssert;
static DMN_DEBUGSTR g_dmdsAssert;

/* Extended notification handler */
typedef struct _ENH {
	char szExtName[64];
	PDM_EXT_NOTIFY_FUNCTION pfnHandler;
} ENH;
#define MAX_ENH 16

#define MAX_NOTIFY 32
typedef struct _DMN_SESSION {
	DWORD dwSig;
	PDMN_SESSION psessNext;
	DWORD dwFlags;
	PDM_NOTIFY_FUNCTION rgpfnNotify[DM_NOTIFYMAX][MAX_NOTIFY];
	int rgcfnNotify[DM_NOTIFYMAX];
	ENH *rgpenh[MAX_ENH];
} DMN_SESSION;
#define DMSESS_DEAD 0x10000
#define DMSESS_MSVC 0x20000
static PDMN_SESSION psessFirst;
static BOOL fUsingSessionList;

/* Notifier Request Queue */
typedef struct _NRQ {
    union {
        struct {
            BOOL fReset:1,
                fAsync:1,
                unused:30;
        };
        BOOL grfFlags;
    };
    HRESULT hr;
    HANDLE hevt;
    PDM_NOTIFY_FUNCTION pfn;
    struct _NRQ *pnrqNext;
} NRQ, *PNRQ;
static HANDLE hevtNRQ;
static PNRQ g_pnrq;

void ResetConnection(PDM_CONNECTION pdcon)
{
	SOCKET s = pdcon->s;

	pdcon->s = INVALID_SOCKET;
    if(s != INVALID_SOCKET) {
        /* We need this connection to RST, not to FIN */
        struct linger lin;
        lin.l_onoff = 1;
        lin.l_linger = 0;
        setsockopt(s, SOL_SOCKET, SO_LINGER, (PVOID)&lin,
            sizeof lin);
		closesocket(s);
    }
}

BOOL FEnterSessionList(void)
{
	BOOL fPrev;
	EnterCriticalSection(&csNotifyList);
	fPrev = fUsingSessionList;
	fUsingSessionList = TRUE;
	return fPrev;
}

void LeaveSessionList(BOOL fStillInList)
{
	PDMN_SESSION psess;
	PDMN_SESSION *ppsess;

	fUsingSessionList = fStillInList;
	if(!fUsingSessionList) {
		/* Free all of the sessions marked dead */
		ppsess = &psessFirst;
		while(*ppsess) {
			psess = *ppsess;
			if(psess->dwFlags & DMSESS_DEAD) {
				*ppsess = psess->psessNext;
				psess->dwSig = 0;
				LocalFree(psess);
			} else
				ppsess = &psess->psessNext;
		}
	}
	if(!psessFirst)
		StopAllNotifications();
	LeaveCriticalSection(&csNotifyList);
}

void SendNotification(DWORD dwNotification, DWORD dw)
{
	DWORD dwIndex = (dwNotification & DM_NOTIFICATIONMASK) - 1;
	int ifn;
	PDMN_SESSION psess;
	BOOL f;

	f = FEnterSessionList();
	for(psess = psessFirst; psess; psess = psess->psessNext) {
		ifn = psess->rgcfnNotify[dwIndex];
		while(ifn-- && !(psess->dwFlags & DMSESS_DEAD))
			(*psess->rgpfnNotify[dwIndex][ifn])(dwNotification, dw);
	}
	LeaveSessionList(f);
}

void GetBreak(LPCSTR sz, PDMN_BREAK pdmbr)
{
	DMN_BREAK dmbr;

	if(!FGetDwParam(sz, "addr", (DWORD*)&pdmbr->Address))
		dmbr.Address = NULL;
	if(!FGetDwParam(sz, "thread", &pdmbr->ThreadId))
		dmbr.ThreadId = 0;
}

void FreeAssertData(void)
{
	EnterCriticalSection(&csAssert);
	if(g_dmdsAssert.String)
		LocalFree((PVOID)g_dmdsAssert.String);
	g_dmdsAssert.String = NULL;
	g_dmdsAssert.Length = 0;
	LeaveCriticalSection(&csAssert);
}

void AddAssertString(PDMN_DEBUGSTR pdmds)
{
	LPSTR szNew = LocalAlloc(LMEM_FIXED, g_dmdsAssert.Length + pdmds->Length +
        1);
	
	EnterCriticalSection(&csAssert);
	if(szNew) {
		if(g_dmdsAssert.String) {
			memcpy(szNew, g_dmdsAssert.String, g_dmdsAssert.Length);
			LocalFree((PVOID)g_dmdsAssert.String);
		}
		memcpy(szNew + g_dmdsAssert.Length, pdmds->String, pdmds->Length + 1);
		g_dmdsAssert.Length += pdmds->Length;
		g_dmdsAssert.String = szNew;
	}
	LeaveCriticalSection(&csAssert);
}

void SetDmdsSuffixAndLength(LPCSTR sz, PDMN_DEBUGSTR pdmds)
{
    char *szSuffix;
    char *szString = (char *)pdmds->String;

	for(pdmds->Length = 0; ; ++pdmds->Length) {
		switch(pdmds->String[pdmds->Length]) {
		case '\n':
		case '\r':
		case 0:
            if(sz) {
                if(PchGetParam(sz, "cr", FALSE, FALSE))
                    szString[pdmds->Length++] = '\015';
                else if(PchGetParam(sz, "lf", FALSE, FALSE))
                    szString[pdmds->Length++] = '\012';
                else if(PchGetParam(sz, "crlf", FALSE, FALSE)) {
                    szString[pdmds->Length++] = '\015';
                    szString[pdmds->Length++] = '\012';
                }
            }
            return;
		}
	}
}

BOOL FGetNotificationData(LPCSTR sz, DWORD *pdwNotification, PVOID pv)
{
	int cchCmd;
	for(cchCmd = 0; sz[cchCmd] && !isspace(sz[cchCmd]); ++cchCmd);
	if(cchCmd == 0)
		return FALSE;

	if(!_strnicmp(sz, "break", cchCmd)) {
		*pdwNotification = DM_BREAK;
		GetBreak(sz, (PDMN_BREAK)pv);
		return TRUE;
	} else if(!_strnicmp(sz, "singlestep", cchCmd)) {
		*pdwNotification = DM_SINGLESTEP;
		GetBreak(sz, (PDMN_BREAK)pv);
		return TRUE;
	} else if(!_strnicmp(sz, "data", cchCmd)) {
#define pdmdb ((PDMN_DATABREAK)pv)
		*pdwNotification = DM_DATABREAK;
		GetBreak(sz, (PDMN_BREAK)pv);
		if(FGetDwParam(sz, "write", (DWORD *)&pdmdb->DataAddress))
			pdmdb->BreakType = DMBREAK_WRITE;
		else if(FGetDwParam(sz, "read", (DWORD *)&pdmdb->DataAddress))
			pdmdb->BreakType = DMBREAK_READWRITE;
		else if(FGetDwParam(sz, "execute", (DWORD *)&pdmdb->DataAddress))
			pdmdb->BreakType = DMBREAK_EXECUTE;
		else {
			pdmdb->DataAddress = NULL;
			pdmdb->BreakType = DMBREAK_NONE;
		}
		return TRUE;
#undef pdmdb
	} else if(!_strnicmp(sz, "exception", cchCmd)) {
#define pdme ((PDMN_EXCEPTION)pv)
		*pdwNotification = DM_EXCEPTION;
		memset(pdme, 0, sizeof *pdme);
		if(!FGetDwParam(sz, "code", &pdme->Code))
			return FALSE;
		FGetDwParam(sz, "thread", &pdme->ThreadId);
		FGetDwParam(sz, "address", (DWORD *)&pdme->Address);
		if(PchGetParam(sz, "first", FALSE, FALSE))
			pdme->Flags |= DM_EXCEPT_FIRSTCHANCE;
		if(PchGetParam(sz, "noncont", FALSE, FALSE))
			pdme->Flags |= DM_EXCEPT_NONCONTINUABLE;
		FGetDwParam(sz, "read", &pdme->Information[1]);
		pdme->Information[0] = FGetDwParam(sz, "write", &pdme->Information[1]);
		return TRUE;
#undef pdme
	} else if(!_strnicmp(sz, "rip", cchCmd)) {
#define pdmds ((PDMN_DEBUGSTR)pv)
		*pdwNotification = DM_RIP;
		memset(pdmds, 0, sizeof *pdmds);
		FGetDwParam(sz, "thread", &pdmds->ThreadId);
		/* We won't actually fill in the string; that will be the job of the
		 * caller if they want it */
		return TRUE;
#undef pdmds
	} else if(!_strnicmp(sz, "assert", cchCmd)) {
		DWORD tid;
		BOOL fRet;
        DMN_DEBUGSTR dmds;

		*pdwNotification = DM_ASSERT;
		if(!FGetDwParam(sz, "thread", &tid))
			return FALSE;
		EnterCriticalSection(&csAssert);
		/* We don't accept thread-interleaved assert data, so if this thread
		 * id doesn't match the one we have stored, we'll dump the stored
		 * assert */
		if(g_dmdsAssert.ThreadId != tid) {
			FreeAssertData();
			g_dmdsAssert.ThreadId = tid;
		}
        /* If we have a string, then we need to process it.  Otherwise, if
		 * this is the end of the assert, we'll have a "prompt" keyword.
		 * In that case, we need to return the aggregate assert, or if there
		 * is none, we say there's an empty string */
        fRet = FALSE;
        dmds.String = PchGetParam(sz, "string", TRUE, FALSE);
        if(dmds.String) {
            ((char *)dmds.String)[-1] = 0;
            SetDmdsSuffixAndLength(sz, &dmds);
            AddAssertString(&dmds);
        } else if(PchGetParam(sz, "prompt", FALSE, FALSE)) {
			fRet = TRUE;
			memcpy(pv, &g_dmdsAssert, sizeof g_dmdsAssert);
		}
		LeaveCriticalSection(&csAssert);
		return fRet;
	}
	return FALSE;
}

void HandleNotification(LPCSTR sz)
{
	int cchCmd;
	int ich;
	DWORD dwFlags;
	DM_THREADSTOP dmts;
	const char *pchStop;
	
	for(cchCmd = 0; sz[cchCmd] && !isspace(sz[cchCmd]); ++cchCmd);
	if(cchCmd == 0)
		return;

	/* Check for external notifications */
	for(ich = 0; ich < cchCmd; ++ich) {
		if(sz[ich] == '!') {
			/* This needs to go the external handlers.  Find out who */
			PDMN_SESSION psess;
			BOOL f;

			f = FEnterSessionList();
			for(psess = psessFirst; psess; psess = psess->psessNext) {
				int ienh;

				for(ienh = 0; ienh < MAX_ENH; ++ienh) {
					if(psess->rgpenh[ienh] && !_strnicmp(sz,
							psess->rgpenh[ienh]->szExtName, ich) &&
							!psess->rgpenh[ienh]->szExtName[ich])
						psess->rgpenh[ienh]->pfnHandler(sz);
				}
			}
			LeaveSessionList(f);
			return;
		}
	}

	dwFlags = 0;
	pchStop = PchGetParam(sz, "stop", FALSE, FALSE);
	if(pchStop)
		dwFlags |= DM_STOPTHREAD;
	if(FGetNotificationData(sz, &dmts.NotifiedReason, &dmts.u)) {
		if(dmts.NotifiedReason == DM_RIP) {
			const char *pchStr = PchGetParam(sz, "string", TRUE, FALSE);
			if(pchStop && pchStr && pchStop >= pchStr)
				/* Not a legimitate stop */
				dwFlags &= DM_STOPTHREAD;
			dmts.u.DebugStr.String = pchStr;
			if(pchStr)
				SetDmdsSuffixAndLength(NULL, &dmts.u.DebugStr);
		}
		SendNotification(dmts.NotifiedReason | dwFlags, (DWORD)&dmts.u);
		if(dmts.NotifiedReason == DM_ASSERT)
			/* We're done with the assert data now */
			FreeAssertData();
	} else if(!_strnicmp(sz, "execution", cchCmd)) {
		DWORD dwExecPrev = dwExecState;
		if(PchGetParam(sz, "started", FALSE, FALSE))
			dwExecState = DMN_EXEC_START;
		else if(PchGetParam(sz, "stopped", FALSE, FALSE))
			dwExecState = DMN_EXEC_STOP;
		else if(PchGetParam(sz, "pending", FALSE, FALSE))
			dwExecState = DMN_EXEC_PENDING;
		else if(PchGetParam(sz, "rebooting", FALSE, FALSE)) {
			dwExecState = DMN_EXEC_REBOOT;
			/* We're also going to lose our connection here */
			if(pdconNotify)
                ResetConnection(pdconNotify);
			/* We're also going to have to terminate the shared connection */
			CloseSharedConnection(NULL);
		} else
			return; // can't process this one
		/* Only inform of state changes */
		if(dwExecState != dwExecPrev)
			SendNotification(DM_EXEC, dwExecState);
	} else if(!_strnicmp(sz, "debugstr", cchCmd)) {
		DMN_DEBUGSTR dmds;
		const char *pchTok;

		dmds.String = PchGetParam(sz, "string", TRUE, FALSE);
		if(!dmds.String)
			/* If there's no string, there's no point */
			return;
        ((char *)dmds.String)[-1] = 0;
		pchTok = PchGetParam(sz, "thread", TRUE, FALSE);
		if(!FGetDwParamFromSz(pchTok, &dmds.ThreadId))
			dmds.ThreadId = 0;
		pchTok = PchGetParam(sz, "stop", FALSE, FALSE);
		dwFlags &= ~DM_STOPTHREAD;
		if(pchTok)
			dwFlags |= DM_STOPTHREAD;
		SetDmdsSuffixAndLength(sz, &dmds);
		SendNotification(DM_DEBUGSTR | dwFlags, (DWORD)&dmds);
	} else if(!_strnicmp(sz, "create", cchCmd)) {
		DMN_CREATETHREAD dmct;

		if(!FGetDwParam(sz, "thread", &dmct.ThreadId))
			return;
		if(!FGetDwParam(sz, "start", (DWORD*)&dmct.StartAddress))
			dmct.StartAddress = NULL;
		SendNotification(DM_CREATETHREAD | dwFlags, (DWORD)&dmct);
	} else if(!_strnicmp(sz, "terminate", cchCmd)) {
		DWORD tid;
		if(FGetDwParam(sz, "thread", &tid))
			SendNotification(DM_DESTROYTHREAD | dwFlags, tid);
    } else if(!_strnicmp(sz, "modload", cchCmd)) {
        DMN_MODLOAD dmml;

        if(FUnpackDmml(sz, &dmml, TRUE))
            SendNotification(DM_MODLOAD | dwFlags, (DWORD)&dmml);
    } else if(!_strnicmp(sz, "sectload", cchCmd)) {
        DMN_SECTIONLOAD dmsl;

        if(FUnpackDmsl(sz, &dmsl, TRUE))
            SendNotification(DM_SECTIONLOAD | dwFlags, (DWORD)&dmsl);
    } else if(!_strnicmp(sz, "sectunload", cchCmd)) {
        DMN_SECTIONLOAD dmsl;

        if(FUnpackDmsl(sz, &dmsl, TRUE))
            SendNotification(DM_SECTIONUNLOAD | dwFlags, (DWORD)&dmsl);
    } else if(!_strnicmp(sz, "fiber", cchCmd)) {
        DMN_FIBER dmfib;

        if(!FGetDwParam(sz, "id", &dmfib.FiberId))
            return;
        if(FGetDwParam(sz, "start", (PDWORD)&dmfib.StartAddress))
            dmfib.Create = TRUE;
        else if(PchGetParam(sz, "delete", FALSE, TRUE))
            dmfib.Create = FALSE;
        else
            return;
        SendNotification(DM_FIBER | dwFlags, (DWORD)&dmfib);
	}
}

void TerminateNotifyThread(void)
{
	FreeLibraryAndExitThread(hXboxdbg, 0);
}

void ClosePersistentNotification(void)
{
	PDM_CONNECTION s;

    if(sockPersist != INVALID_SOCKET) {
        if(SUCCEEDED(HrOpenSharedConnection(&s))) {
		    char sz[128];
		    sprintf(sz, "notifyat port=%d drop", usPersistPort);
		    DmSendCommand(s, sz, NULL, 0);
    		CloseSharedConnection(s);
        }
		closesocket(sockPersist);
		sockPersist = INVALID_SOCKET;
	}
}

void SatisfyNrqList(HRESULT hr)
{
    PNRQ pnrq;
    BOOL fAsync;

    EnterCriticalSection(&csEnsureNotify);
    while(g_pnrq) {
        pnrq = g_pnrq;
        g_pnrq = pnrq->pnrqNext;
        pnrq->pnrqNext = NULL;
        pnrq->hr = hr;
        if(pnrq->pfn && SUCCEEDED(hr))
            /* We were asked to send exec notification when the connection
             * successfully completed */
            (*pnrq->pfn)(DM_EXEC, dwExecState);
        fAsync = pnrq->fAsync;
        if(pnrq->hevt) {
            SetEvent(pnrq->hevt);
            /* Once we've signaled the event, we can't look at pnrq anymore */
            pnrq = NULL;
        }
        if(fAsync) {
            LocalFree(pnrq);
            pnrq = NULL;
        }
    }
    LeaveCriticalSection(&csEnsureNotify);
}

HRESULT HrMakePersistent()
{
	PDM_CONNECTION s;
	char sz[256];
	struct sockaddr_in sin;
	HRESULT hr;
	BOOL f;
	struct hostent *he;
    DWORD dwPersistFlags;
    PDMN_SESSION psess;

    if(sockPersist != INVALID_SOCKET)
		return XBDM_NOERR;

	/* Make sure we have somebody to talk to */
	s = NULL;
	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		f = FALSE;
	else {
		f = TRUE;
		hr = E_UNEXPECTED;
	}

	/* Make sure we can get a socket */
	if(f)
		f = (sockPersist = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) !=
			INVALID_SOCKET;
	else
		sockPersist = INVALID_SOCKET;
	/* Make sure we can give it a port */
	if(f) {
		sin.sin_family = AF_INET;
		sin.sin_port = 0;
		sin.sin_addr.s_addr = 0;
		f = 0 == bind(sockPersist, (struct sockaddr *)&sin, sizeof sin);
	}
	/* Start listening */
	if(f)
		f = 0 == listen(sockPersist, 2);
	/* And find out where we're listening */
	if(f) {
		int cb = sizeof sin;
		f = 0 == getsockname(sockPersist, (struct sockaddr *)&sin, &cb);
		usPersistPort = ntohs(sin.sin_port);
	}
	/* Make sure we'll know when an incoming connection is ready */
	if(f)
		f = 0 == WSAEventSelect(sockPersist, hevtPersistConn, FD_ACCEPT);

	if(!f && sockPersist != INVALID_SOCKET) {
		closesocket(sockPersist);
		sockPersist = INVALID_SOCKET;
	}

    /* Run down the list of open notification sessions and make sure we
     * build up the appropriate flags */
    dwPersistFlags = 0;
    EnterCriticalSection(&csNotifyList);
    for(psess = psessFirst; psess; psess = psess->psessNext) {
        if(!(psess->dwFlags & DMSESS_DEAD))
            dwPersistFlags |= psess->dwFlags;
    }
    LeaveCriticalSection(&csNotifyList);

	/* Now tell the remote machine that we're listening */
	if(f) {
		sprintf(sz, "NOTIFYAT PORT=%d%s", usPersistPort, dwPersistFlags &
            DM_DEBUGSESSION ? " debug" : "");
		hr = DmSendCommand(s, sz, NULL, NULL);
	}

	if(s)
		CloseSharedConnection(s);

	return hr;
}

HRESULT HrEstablishNotificationConnection()
{
    HRESULT hr;
	HANDLE rgh[2];
	fd_set fds;
    struct timeval tv;
    DWORD tickStart;
    DWORD tickNow;
    int n;

	rgh[0] = hevtPersistConn;
	rgh[1] = hevtTerminate;

	for(;;) {
        /* Make sure we have a persistent connection available */
    	hr = HrMakePersistent();
        if(FAILED(hr))
            return hr;

        /* Wait for 60 seconds for a call to come back.  If we've just opened
         * the notification callback, this should happen instantaneously.
         * If the box is rebooting, it will happen when the reboot is
         * complete */
        tickStart = GetTickCount();
        while((tickNow = GetTickCount()) - tickStart < 60000) {
            n = WaitForMultipleObjects(2, rgh, FALSE, 60000 - (tickNow -
                tickStart));
            if(n == WAIT_OBJECT_0) {
                /* We think we have a pending connection, so attempt to accept
                 * it */
                SOCKET s = accept(sockPersist, NULL, NULL);
			    if(s != INVALID_SOCKET) {
				    pdconNotify = PdconWrapSocket(s);
				    break;
                } else {
                    n = WSAGetLastError();
                    if(n != WSAEINTR && n != WSAEWOULDBLOCK) {
                        /* Our persistent connection somehow died, so reset
                         * it */
                        closesocket(sockPersist);
                        sockPersist = INVALID_SOCKET;
                        break;
                    }
                }
            } else if(n == WAIT_OBJECT_0 + 1) {
                /* We're supposed to terminate.  If we have pending requests
                 * to establish a connection, we'll ignore the request to
                 * terminate so we don't have to spin up another thread again
                 * right away */
                EnterCriticalSection(&csEnsureNotify);
                if(!g_pnrq) {
                    closesocket(sockPersist);
                    sockPersist = INVALID_SOCKET;
                    CloseHandle(hthrNotifier);
                    hthrNotifier = NULL;
                    LeaveCriticalSection(&csEnsureNotify);
                    TerminateNotifyThread();
                }
                LeaveCriticalSection(&csEnsureNotify);
            }
        }

        if(pdconNotify)
            /* We have the desired connection */
            return XBDM_NOERR;

		/* Our persistent channel did us no good, so get rid of it, and
         * try again */
		closesocket(sockPersist);
		sockPersist = INVALID_SOCKET;
    }
}

DWORD WINAPI NotifyThread(LPVOID unused)
{
	char szBuf[4096];
	int cbBuf;
	BOOL fSkipString = FALSE;
	HANDLE rgh[4];
	DWORD dwWait;
    NRQ *pnrq;
    HRESULT hr;

	rgh[0] = hevtSockData;
	rgh[1] = hevtNRQ;
	rgh[2] = hevtPersistConn;
	rgh[3] = hevtTerminate;

	for(;;) {
		/* Make sure we've got a socket to listen on */
        hr = HrEstablishNotificationConnection();
        
        /* Run down any pending list of notification requests */
        EnterCriticalSection(&csEnsureNotify);
        SatisfyNrqList(hr);
        ResetEvent(hevtNRQ);
        if(FAILED(hr)) {
            /* We failed to establish a notification connection, so we're
             * going to shut down the thread */
            ClosePersistentNotification();
            CloseHandle(hthrNotifier);
            hthrNotifier = NULL;
            LeaveCriticalSection(&csEnsureNotify);
			TerminateNotifyThread();
        }

		ResetEvent(hevtSockData);
		if(0 != WSAEventSelect(pdconNotify->s, hevtSockData, FD_READ)) {
			/* Act as if no socket */
            ResetConnection(pdconNotify);
			DmCloseConnection(pdconNotify);
			pdconNotify = NULL;
		}
        LeaveCriticalSection(&csEnsureNotify);

		/* Now we just wait until we've got a big enough line to do something
		 * with, or until some other notable event happens */
		cbBuf = 0;
		while(pdconNotify && pdconNotify->s != INVALID_SOCKET) {
			/* Look for something to do */
			if(pdconNotify->ibBuf < pdconNotify->cbBuf)
				dwWait = WAIT_OBJECT_0;
			else
				dwWait = WaitForMultipleObjects(4, rgh, FALSE, INFINITE);
			switch(dwWait) {
			case WAIT_OBJECT_0:
				if(pdconNotify->ibBuf >= pdconNotify->cbBuf) {
					int cb;
					/* Data to receive */
					cb = recv(pdconNotify->s, pdconNotify->rgbBuf, sizeof
						pdconNotify->rgbBuf - 1, 0);
					if(cb <= 0) {
						switch(WSAGetLastError()) {
						case WSAEINTR:
						case WSAEWOULDBLOCK:
							break;
						default:
							/* Lost the connection.  Close it and prepare for
							 * it to be opened again next time we need to
							 * ensure notification */
                            ResetConnection(pdconNotify);
							break;
						}
						cb = 0;
					}
					pdconNotify->cbBuf = cb;
					pdconNotify->ibBuf = 0;
				}
				/* Let's see if we have a delimiting character in the data that
				 * just came in */
				while(pdconNotify && pdconNotify->ibBuf < pdconNotify->cbBuf) {
					szBuf[cbBuf] = pdconNotify->rgbBuf[pdconNotify->ibBuf++];
					if(szBuf[cbBuf] == '\012' || szBuf[cbBuf] == '\015') {
						/* We've got a string, but process it only if it's not
						 * the tail end of some other data */
						szBuf[cbBuf++] = 0;
						if(!fSkipString)
							HandleNotification(szBuf);
						/* Now shift out the data that we've processed */
						cbBuf = 0;
					} else
						++cbBuf;
					if(cbBuf == sizeof szBuf - 1) {
						/* We never got the newline, so we'll just process what
						 * we have and discard the rest when it gets here */
						szBuf[cbBuf] = 0;
						if(!fSkipString)
							HandleNotification(szBuf);
						cbBuf = 0;
						fSkipString = TRUE;
					}
				}
				break;
			case WAIT_OBJECT_0 + 2:
				/* We got a new incoming connection.  That means that the
				 * remote machine rebooted without telling us.  Synthesize
				 * a reboot notification, which will close the connection */
				HandleNotification("execution rebooting");
				/* We also need to set the event again, because our call to
				 * wait caused a reset and we're going to wait again before
				 * calling accept */
				SetEvent(rgh[2]);
				break;
			case WAIT_OBJECT_0 + 3:
				/* We're going to go away now.  Close the existing connection.
                 * If we find a pending request to get a notifier, then we
                 * won't really exit, since our continued existence has been
                 * demanded.  Otherwise, we'll quit */
                EnterCriticalSection(&csEnsureNotify);
                if(!g_pnrq) {
                    ResetConnection(pdconNotify);
                    DmCloseConnection(pdconNotify);
                    ClosePersistentNotification();
    				pdconNotify = NULL;
                    CloseHandle(hthrNotifier);
                    hthrNotifier = NULL;
                    LeaveCriticalSection(&csEnsureNotify);
				    TerminateNotifyThread();
                }
                LeaveCriticalSection(&csEnsureNotify);
                // fall through
			case WAIT_OBJECT_0 + 1:
                /* We've been asked to ensure the notifier.  Walk down the
                 * request list.  If we have any reset requests, then we'll
                 * close the current connection and leave the requests
                 * pending.  Otherwise we'll satisfy them all */
                EnterCriticalSection(&csEnsureNotify);
                for(pnrq = g_pnrq; pnrq; pnrq = pnrq->pnrqNext) {
                    if(pnrq->fReset) {
                        ResetConnection(pdconNotify);
                        ClosePersistentNotification();
                    }
                }
                if(pdconNotify->s != INVALID_SOCKET)
                    SatisfyNrqList(XBDM_NOERR);
				ResetEvent(rgh[1]);
                LeaveCriticalSection(&csEnsureNotify);
                break;
            }
		}
	    if(pdconNotify && pdconNotify->s == INVALID_SOCKET) {
		    DmCloseConnection(pdconNotify);
		    pdconNotify = NULL;
	    }
    }
}

void StopAllNotifications(void)
{
    /* The notification thread will do all of the real cleanup */
	if(hthrNotifier)
		SetEvent(hevtTerminate);
}

void InitNotificationEvents(void)
{
	InitializeCriticalSection(&g_sci.csSharedConn);
	InitializeCriticalSection(&csNotifyList);
	InitializeCriticalSection(&csEnsureNotify);
	InitializeCriticalSection(&csAssert);
	hevtSockData = CreateEvent(NULL, FALSE, FALSE, NULL);
	hevtNRQ = CreateEvent(NULL, TRUE, FALSE, NULL);
	hevtPersistConn = CreateEvent(NULL, FALSE, FALSE, NULL);
	hevtTerminate = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void DestroyNotificationEvents(void)
{
	CloseHandle(hevtSockData);
	CloseHandle(hevtNRQ);
	CloseHandle(hevtPersistConn);
	CloseHandle(hevtTerminate);
	DeleteCriticalSection(&g_sci.csSharedConn);
	DeleteCriticalSection(&csNotifyList);
	DeleteCriticalSection(&csEnsureNotify);
	DeleteCriticalSection(&csAssert);
}

HRESULT HrEnsureNotifier(BOOL fReset, BOOL fAsync, PDM_NOTIFY_FUNCTION pfn)
{
    HRESULT hr;
    NRQ nrq;
    PNRQ pnrq;

    /* For synchronization reasons, only the notification thread is allowed to
     * actually establish notification connections.  So we post a request
     * to it and wait for it to complete */
    if(fAsync) {
        pnrq = LocalAlloc(LMEM_FIXED, sizeof nrq);
        pnrq->grfFlags = 0;
        if(!pnrq)
            return E_OUTOFMEMORY;
        pnrq->fAsync = TRUE;
        pnrq->hevt = NULL;
    } else {
        pnrq = &nrq;
        pnrq->grfFlags = 0;
        pnrq->hevt = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
    pnrq->fReset = !!fReset;
    pnrq->pfn = pfn;
    EnterCriticalSection(&csEnsureNotify);
    pnrq->pnrqNext = g_pnrq;
    g_pnrq = pnrq;

    /* Before we kick off this request, we need to make sure we actually have a
     * notification thread ready to handle it */
	if(!hthrNotifier) {
		char szFileName[MAX_PATH + 1];
		HMODULE hmod;

		if(!GetModuleFileName(hXboxdbg, szFileName, sizeof szFileName))
			hr = E_UNEXPECTED;
		else {
			hmod = LoadLibrary(szFileName);
			if(hmod == hXboxdbg)
				hr = XBDM_NOERR;
			else {
				hr = E_UNEXPECTED;
				if(hmod)
					FreeLibrary(hmod);
			}
		}
		if(SUCCEEDED(hr)) {
			hthrNotifier = CreateThread(NULL, 0, NotifyThread, NULL,
				CREATE_SUSPENDED, NULL);
			if(!hthrNotifier) {
				hr = E_UNEXPECTED;
				FreeLibrary(hXboxdbg);
			} else
				ResumeThread(hthrNotifier);
		}

        /* If we failed to create the thread, then we walk down the pending
         * request list and mark them all as failed */
        if(FAILED(hr))
            SatisfyNrqList(hr);
    }

    LeaveCriticalSection(&csEnsureNotify);

    /* Now tell the the notification thread to process the queue */
    SetEvent(hevtNRQ);

    /* If this was an async request, we'll return success */
    if(fAsync)
        return XBDM_NOERR;

    /* Wait for our request to be handled.  If we failed to create the
     * thread, it was handled in the loop above. */
    WaitForSingleObject(nrq.hevt, INFINITE);
    CloseHandle(nrq.hevt);

	return nrq.hr;
}

HRESULT DmOpenNotificationSession(DWORD dwFlags, PDMN_SESSION *ppsess)
{
	HRESULT hr;

	if(!ppsess)
		return E_INVALIDARG;

    if(dwFlags & DMSESS_MSVC) {
        PDMN_SESSION psess;
        /* This is VC trying to recycle a previously allocated session.  See
         * if we can find it */
        EnterCriticalSection(&csNotifyList);
        for(psess = psessFirst; psess; psess = psess->psessNext)
            if((psess->dwFlags & (DMSESS_MSVC | DMSESS_DEAD)) == DMSESS_MSVC)
                break;
        LeaveCriticalSection(&csNotifyList);
        if(psess) {
            *ppsess = psess;
            return HrEnsureNotifier(TRUE, dwFlags & DM_ASYNCSESSION, NULL);
        }
    }
	*ppsess = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(DMN_SESSION));
	if(!*ppsess)
		return E_OUTOFMEMORY;
	(*ppsess)->dwFlags = dwFlags;
	(*ppsess)->dwSig = 'xnot';
	EnterCriticalSection(&csNotifyList);
	(*ppsess)->psessNext = psessFirst;
	psessFirst = *ppsess;
	LeaveCriticalSection(&csNotifyList);
	hr = HrEnsureNotifier(FALSE, dwFlags & DM_ASYNCSESSION, NULL);
    if(FAILED(hr)) {
        /* Failed to get the notifier, so nuke this session */
        (*ppsess)->dwFlags |= DMSESS_DEAD;
        *ppsess = NULL;
        LeaveSessionList(FEnterSessionList());
	}
	return hr;
}

HRESULT DmCloseNotificationSession(PDMN_SESSION psess)
{
	PDMN_SESSION *ppsess;
	BOOL f;

	if(!psess || psess->dwSig != 'xnot')
		return E_INVALIDARG;

	f = FEnterSessionList();
	psess->dwFlags |= DMSESS_DEAD;
	LeaveSessionList(f);

	return XBDM_NOERR;
}

HRESULT DmNotify(PDMN_SESSION psess, DWORD dw, PDM_NOTIFY_FUNCTION pfnHandler)
{
	HRESULT hr;

	if(!psess || psess->dwSig != 'xnot')
		return E_INVALIDARG;

    if(dw != DM_NONE) {
	    hr = HrEnsureNotifier(FALSE, psess->dwFlags & DM_ASYNCSESSION, NULL);
        if(FAILED(hr))
		    return hr;
    }

	EnterCriticalSection(&csNotifyList);
	if(dw == DM_NONE) {
		if(pfnHandler) {
            /* We need to remove this handler from all of the lists */
			for(dw = 0; dw < DM_NOTIFYMAX; ++dw) {
				int ifn;

				for(ifn = 0; ifn < psess->rgcfnNotify[dw]; ++ifn) {
					if(psess->rgpfnNotify[dw][ifn] == pfnHandler)
						psess->rgpfnNotify[dw][ifn--] =
							psess->rgpfnNotify[dw][--psess->rgcfnNotify[dw]];
				}
			}
		} else {
		    /* We need to empty all lists */
			for(dw = 0; dw < DM_NOTIFYMAX; ++dw)
			    psess->rgcfnNotify[dw] = 0;
        }
		hr = XBDM_NOERR;
	} else if(dw <= DM_NOTIFYMAX && pfnHandler) {
		PDMN_SESSION psessT = NULL;
		int cfn = 0;

		--dw;

#if 0
		/* If this function is on any lists already, then we do less work */
		for(psessT = psessFirst; psessT; psessT = psessT->psessNext) {
			cfn = psessT->rgcfnNotify[dw];
			while(cfn--) {
				if(psessT->rgpfnNotify[dw][cfn] == pfnHandler)
					break;
			}
		}
#endif

		if(psess != psessT) {
			if(psess->rgcfnNotify[dw] == MAX_NOTIFY)
				hr = E_OUTOFMEMORY;
			else {
				psess->rgpfnNotify[dw][psess->rgcfnNotify[dw]++] = pfnHandler;
				if(psessT)
					/* Take it off the other list */
					psessT->rgpfnNotify[dw][cfn] =
						psessT->rgpfnNotify[dw][--psessT->rgcfnNotify[dw]];
                else if(dw+1 == DM_EXEC) {
					/* This is a special case and every new handler has to be
					 * immediately informed of the current state, if
					 * available -- except that async handlers only get
                     * called once a connection is available */
                     if(psess->dwFlags & DM_ASYNCSESSION)
                         HrEnsureNotifier(FALSE, TRUE, pfnHandler);
                     else
                        (*pfnHandler)(DM_EXEC, dwExecState);
                }
			}
		}
		hr = XBDM_NOERR;
	} else
		hr = E_INVALIDARG;
	LeaveCriticalSection(&csNotifyList);
	return hr;
}

HRESULT DmRegisterNotificationProcessor(PDMN_SESSION psess, LPCSTR sz,
	PDM_EXT_NOTIFY_FUNCTION pfn)
{
	int ienh;
	ENH *penh;
	HRESULT hr;

	if (!psess || psess->dwSig != 'xnot')
		return E_INVALIDARG;
    if(pfn && !sz)
        return E_INVALIDARG;

	EnterCriticalSection(&csNotifyList);
    if(!pfn) {
        /* We're unregistering.  If we have a string, then only unhook those
         * handlers that match the string */
        for(ienh = 0; ienh < MAX_ENH; ++ienh) {
            if(psess->rgpenh[ienh]) {
                if(sz && _stricmp(psess->rgpenh[ienh]->szExtName, sz))
                    continue;
                LocalFree(psess->rgpenh[ienh]);
                psess->rgpenh[ienh] = NULL;
            }
        }
		hr = XBDM_NOERR;
    } else {
	    /* Let's find somewhere to put it */
	    for(ienh = 0; ienh < MAX_ENH; ++ienh)
		    if(!psess->rgpenh[ienh])
			    break;
	    if(ienh < MAX_ENH) {
		    penh = LocalAlloc(LMEM_FIXED, sizeof *penh);
	    } else
		    penh = NULL;
	    if(penh) {
		    int ich;

		    for(ich = 0; sz[ich] && ich < sizeof penh->szExtName - 1; ++ich)
			    penh->szExtName[ich] = sz[ich];
		    if(sz[ich]) {
			    LocalFree(penh);
			    hr = E_INVALIDARG;
		    } else {
			    penh->szExtName[ich] = 0;
			    penh->pfnHandler = pfn;
			    psess->rgpenh[ienh] = penh;
			    hr = XBDM_NOERR;
		    }
        } else
		    hr = E_OUTOFMEMORY;
    }
	LeaveCriticalSection(&csNotifyList);
	return hr;
}
