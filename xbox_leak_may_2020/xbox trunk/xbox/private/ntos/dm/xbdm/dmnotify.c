/*
 *
 * dmnotify.c
 *
 * Debug notifications
 *
 */

#include "dmp.h"

#define MAX_NOTIFY_SOCKETS 8

SOCKET rgsockNotify[MAX_NOTIFY_SOCKETS];
PKDEBUG_ROUTINE pfnKdpTrap;
BOOL fAllowKd = TRUE;
DWORD g_grbitStopOn;
RTL_CRITICAL_SECTION csNotify;
RTL_CRITICAL_SECTION csNotifySockList;
PETHREAD pthrUsingNotifyList;
KDPC dpcBreakIn;
KEVENT evtNotifyList;

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
PDMN_SESSION psessFirst;
PDMN_SESSION psessFree;
BOOL fUsingSessionList;
BOOL fPruneList;

struct {
	ULONG ulAddr;
	USHORT usPort;
	DWORD dwFlags;
} rgnnPersist[MAX_NOTIFY_SOCKETS];

PDFN g_pdfnFirst;
PDFN *g_ppdfnNext;

/* double fault data */
CONTEXT ctxDF;
BYTE rgbDFStack[4096];

BOOLEAN DmTrapHandler(PKTRAP_FRAME ptf, PKEXCEPTION_FRAME pef,
	PEXCEPTION_RECORD per, PCONTEXT pcr, BOOLEAN fSecondChance);
void EnterDebuggerDpc(PKDPC, PVOID, PVOID, PVOID);
void ClockInt(PKTRAP_FRAME ptf);
void ProfInt(PKTRAP_FRAME ptf);
void __fastcall CtxSwap(DWORD, DWORD);
BOOLEAN CanDebugDF(KTSS *ptss);
ULONG TellDebugger(ULONG, PVOID);

LPCSTR rgszExec[] = {
	/* These have to go in the same order as the DMN_EXEC values */
	"stopped",
	"started",
	"rebooting",
	"pending"
};

void InitNotifications()
{
	int isock;

	InitializeCriticalSection(&csNotify);
	InitializeCriticalSection(&csNotifySockList);
	KeInitializeEvent(&evtNotifyList, NotificationEvent, TRUE);
	for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock)
		rgsockNotify[isock] = INVALID_SOCKET;

	g_ppdfnNext = &g_pdfnFirst;
	fAllowKd = *KdDebuggerEnabled;
	pfnKdpTrap = *g_dmi.DebugRoutine;
	*g_dmi.DebugRoutine = DmTrapHandler;
	*g_dmi.ProfIntRoutine = ProfInt;
	KeInitializeDpc(&dpcBreakIn, EnterDebuggerDpc, NULL);
	g_dmi.TellRoutine = TellDebugger;
	KeGetCurrentPrcb()->DebugDoubleFault = CanDebugDF;
}

BOOL FAddNotifySock(SOCKET s)
{
	int isock;
	int iPass;

	RtlEnterCriticalSection(&csNotifySockList);
	for(iPass = 0; iPass < 2; ++iPass) {
		for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock) {
			if(rgsockNotify[isock] == INVALID_SOCKET) {
				rgsockNotify[isock] = s;
				RtlLeaveCriticalSection(&csNotifySockList);
				return TRUE;
			}
		}
		/* Nothing yet, let's send a keepalive to all of the existing sockets
		 * to make sure they're really all in use */
		if(!iPass)
			NotifyComponents(DM_NONE, 0);
	}
	RtlLeaveCriticalSection(&csNotifySockList);
	return FALSE;
}

BOOL FConvertToNotifySock(SOCKET s)
{
	BOOL fRet = FAddNotifySock(s);
	if(s) {
		char sz[64];
		PrintSockLine(s, "205- now a notification channel");
		if(FGetNotifySz(DM_EXEC, dwExecState, sz))
			PrintSockLine(s, sz);
	}
	return fRet;
}

BOOL FPrintNotifySock(int isock, LPCSTR sz)
{
	BOOL fRet = PrintSockLine(rgsockNotify[isock], sz);
	if(!fRet && rgsockNotify[isock] != INVALID_SOCKET) {
		closesocket(rgsockNotify[isock]);
		rgsockNotify[isock] = INVALID_SOCKET;
	}
	return fRet;
}

SOCKET SockCreateNotify(ULONG ulAddr, USHORT usPort, BOOL fNonBlock)
{
	SOCKET s;
	struct sockaddr_in sin;
	BOOL f;
	char sz[64];
    LARGE_INTEGER lu;
    PLARGE_INTEGER plu;

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == s)
		goto err1;
	f = TRUE;
	if(0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (PVOID)&f, sizeof f))
		goto err2;
	sin.sin_family = AF_INET;
    /* We need to pick a random port so that we don't reset the connection
     * we previously held with our peer -- if we do that, they may well miss
     * any previously sent notifications */
    KeQuerySystemTime(plu = &lu);
    _asm {
        mov ecx, plu
        rdtsc
        xor [ecx], eax
        xor [ecx+4], edx
    };
    sin.sin_port = (USHORT)(((lu.HighPart ^ lu.LowPart) & 0x3FFF) + 0x1000);
    sin.sin_port = htons(sin.sin_port);
	sin.sin_addr.s_addr = 0;
	if(0 != bind(s, (struct sockaddr *)&sin, sizeof sin))
		goto err2;
	if(0 != ioctlsocket(s, FIONBIO, &fNonBlock))
		goto err2;
	sin.sin_addr.s_addr = htonl(ulAddr);
	sin.sin_port = htons(usPort);
	if(fNonBlock) {
		/* We expect the call to connect to return EWOULDBLOCK, so we just
		 * accept that error and don't try to send the initial string */
		if(0 == connect(s, (struct sockaddr *)&sin, sizeof sin) ||
				WSAGetLastError() != WSAEWOULDBLOCK)
			goto err2;
	} else {
		if(0 != connect(s, (struct sockaddr *)&sin, sizeof sin))
			goto err2;
		if(!FGetNotifySz(DM_EXEC, dwExecState, sz) || !PrintSockLine(s, sz))
			goto err2;
	}
	return s;
err2:
	closesocket(s);
err1:
	return INVALID_SOCKET;
}

void WriteIniNotifiers(INF *pinf)
{
	int inn;
	BYTE *pb;
	char sz[128];

	for(inn = 0; inn < MAX_NOTIFY_SOCKETS; ++inn) {
		if(rgnnPersist[inn].ulAddr) {
			pb = (BYTE *)&rgnnPersist[inn].ulAddr;
			sprintf(sz, "notifyat addr=%d.%d.%d.%d port=%d%s", pb[3], pb[2],
				pb[1], pb[0], rgnnPersist[inn].usPort,
				rgnnPersist[inn].dwFlags & DM_DEBUGSESSION ? " debug" : "");
			WriteIniSz(pinf, sz);
		}
	}
}

void UnNotifyAt(ULONG ulAddr, USHORT usPort)
{
	int inn;

	for(inn = 0; inn < MAX_NOTIFY_SOCKETS; ++inn) {
		if(rgnnPersist[inn].ulAddr == ulAddr && rgnnPersist[inn].usPort ==
				usPort)
			rgnnPersist[inn].ulAddr = 0;
	}
}

BOOL FNotifyAt(ULONG ulAddr, USHORT usPort, DWORD dwFlags, BOOL fBatch)
{
	int inn;

	if(!ulAddr)
		return FALSE;
	RtlEnterCriticalSection(&csNotifySockList);
	for(inn = 0; inn < MAX_NOTIFY_SOCKETS; ++inn)
		if(rgnnPersist[inn].ulAddr == 0)
			break;
	if(inn == MAX_NOTIFY_SOCKETS) {
		RtlLeaveCriticalSection(&csNotifySockList);
		return FALSE;
	}
	rgnnPersist[inn].ulAddr = ulAddr;
	rgnnPersist[inn].usPort = usPort;
	rgnnPersist[inn].dwFlags = dwFlags;
	RtlLeaveCriticalSection(&csNotifySockList);

	if(!fBatch) {
		SOCKET s = SockCreateNotify(ulAddr, usPort, FALSE);
		if(INVALID_SOCKET == s || !FAddNotifySock(s)) {
			rgnnPersist[inn].ulAddr = 0;
			if(INVALID_SOCKET != s)
				closesocket(s);
			/* We don't want this guy bothering us on reboot */
			UnNotifyAt(ulAddr, usPort);
			return FALSE;
		} else if (dwFlags & DM_DEBUGSESSION) {
			g_fDebugging = TRUE;
			fAllowKd = FALSE;
		}
	}
	return TRUE;
}

BOOL FNotifyAtCmd(SOCKET s, LPCSTR sz, BOOL fBatch)
{
	char szAddr[20];
	ULONG ulAddr;
	DWORD dwPort;
	struct sockaddr_in sin;
	int cbAddr = sizeof sin;
	DWORD dwFlags = 0;

	if(!FGetDwParam(sz, "port", &dwPort))
		return FALSE;
	if(FGetSzParam(sz, "addr", szAddr, sizeof szAddr))
		ulAddr = UlAddrFromSz(szAddr);
	else {
		if(s == INVALID_SOCKET)
			return FALSE;
		if(0 != getpeername(s, (struct sockaddr *)&sin, &cbAddr))
			return FALSE;
		ulAddr = ntohl(sin.sin_addr.s_addr);
	}

	if(PchGetParam(sz, "drop", FALSE)) {
		/* We want to remove this address/port from the list */
		UnNotifyAt(ulAddr, (USHORT)dwPort);
		return TRUE;
	}
	if(PchGetParam(sz, "debug", FALSE))
		dwFlags |= DM_DEBUGSESSION;
	return FNotifyAt(ulAddr, (USHORT)dwPort, dwFlags, fBatch);
}

BOOL FEnsureIPReady(int nTimeout)
{
    XNADDR xnaddr;

    while(nTimeout > 0) {
        if(XNetGetDebugXnAddr(&xnaddr) != 0)
            return TRUE;
        Sleep(100);
        nTimeout -= 100;
    }
    return FALSE;
}

void EnsurePersistentSockets(void)
{
	SOCKET rgsock[MAX_NOTIFY_SOCKETS];
	int isock;
	struct timeval tv;
	BOOL f = FALSE;
	LARGE_INTEGER liTimeout;
	NTSTATUS st;

	RtlEnterCriticalSection(&csNotifySockList);
	for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock) {
		if(rgnnPersist[isock].ulAddr) {
            /* See if we can get an IP address (wait up to 10 seconds), but
             * don't error out if we can't */
            FEnsureIPReady(10000);
			/* The peer sockets are expecting the command server to be up and
			 * running as soon as they're notified, so we need to wait for
			 * the server -- but we'll only wait 30 seconds before we decide
			 * we have no network */
			liTimeout.QuadPart = -30000 * 10000;
			st = KeWaitForSingleObject(&kevtServ, UserRequest, UserMode,
				FALSE, &liTimeout);
			if(!NT_SUCCESS(st) || st == STATUS_TIMEOUT)
			{
				DbgPrint("xbdm callback: timed out waiting for server\n");
				return;
			}
			rgsock[isock] = SockCreateNotify(rgnnPersist[isock].ulAddr,
				rgnnPersist[isock].usPort, TRUE);
		} else
			rgsock[isock] = INVALID_SOCKET;
	}

	/* wait no more than 60 seconds for the sockets to come online */
	tv.tv_sec = 60;
	tv.tv_usec = 0;
	for(;;) {
		int csock = 0;
		fd_set fds;

		FD_ZERO(&fds);
		for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock)
			if(rgsock[isock] != INVALID_SOCKET) {
				FD_SET(rgsock[isock], &fds);
				++csock;
			}
		if(!csock)
			/* Nobody left to wait for */
			break;
		if(select(0, NULL, &fds, NULL, &tv) == 0) {
			/* The remaining sockets are not going to connect, so give up on
			 * them */
			for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock)
				if(rgsock[isock] != INVALID_SOCKET) {
					closesocket(rgsock[isock]);
					/* Failure now means we won't ever try this port again */
					rgnnPersist[isock].ulAddr = 0;
				}
			break;
		} else {
			/* Whichever sockets are ready need to be made nonblocking and
			 * marked as notify sockets */
			for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock)
				if(FD_ISSET(rgsock[isock], &fds)) {
					if(0 != ioctlsocket(rgsock[isock], FIONBIO, &f))
						/* Give up on this socket */
						closesocket(rgsock[isock]);
					else {
						/* Try to send the initial string */
						if(!PrintSockLine(rgsock[isock], "hello")) {
							/* No socket there.  Give up on this port */
							closesocket(rgsock[isock]);
							rgnnPersist[isock].ulAddr = 0;
						} else {
							FAddNotifySock(rgsock[isock]);
							if(rgnnPersist[isock].dwFlags & DM_DEBUGSESSION) {
								g_fDebugging = TRUE;
								fAllowKd = FALSE;
							}
						}
					}
					/* Don't worry about this socket anymore */
					rgsock[isock] = INVALID_SOCKET;
				}
		}
	}
	RtlLeaveCriticalSection(&csNotifySockList);
	/* With our actual list of sockets set up, we need to flush the ini file
	 * so we remember where our successes were */
	WriteIniFile();
}

void ProcessDeferredNotifications(PDFN pdfn)
{
	ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

	while(pdfn) {
		PDFN pdfnNext = pdfn->pdfnNext;

		NotifyComponents(DM_DEBUGSTR, (DWORD)&pdfn->dmds);
		DmFreePool(pdfn);
		pdfn = pdfnNext;
	}
}

void SendDebugStrs(DWORD dwNotification, PDMN_DEBUGSTR pdmds)
{
	int isock;
	char szCmd1[64];
	char szCmd2[32];
	int cchCmd;
	char *szNotification;
	WSABUF wsabufs[4];

	if((dwNotification & DM_NOTIFICATIONMASK) == DM_ASSERT)
		szNotification = "assert";
	else
		szNotification = "debugstr";

	sprintf(szCmd1, "%s %sthread=%d ", szNotification, dwNotification &
		DM_STOPTHREAD ? "stop ": "", pdmds->ThreadId);
	cchCmd = strlen(szCmd1);

	if(KeGetCurrentIrql() == PASSIVE_LEVEL) {
		KeEnterCriticalRegion();
		RtlEnterCriticalSection(&csNotify);
		for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock) {
			SOCKET s = rgsockNotify[isock];
			ULONG ichFirst, ichLim, ichMac;
			int cchSkip;
			char *szCrlf;
			DWORD cb, bufcnt;
			WSABUF* bufs;

			if(s == INVALID_SOCKET) continue;

			ichFirst = 0;
			while(ichFirst < pdmds->Length) {
                /* We'll only send 2k worth of data at a time */
                ichMac = ichFirst + 2048;
                if(ichMac > pdmds->Length)
                    ichMac = pdmds->Length;
				for(ichLim = ichFirst; ichLim < ichMac; ++ichLim)
					if(pdmds->String[ichLim] == '\012' ||
						pdmds->String[ichLim] == '\015')
						break;
				/* We need to figure out what's terminating this line */
				if(ichLim == ichMac) {
					cchSkip = 0;
					szCrlf = "";
				} else if(pdmds->String[ichLim] == '\015') {
					cchSkip = 1;
					szCrlf = "cr ";
				} else if(pdmds->String[ichLim] == '\012') {
					if(ichLim + 1 < pdmds->Length &&
						pdmds->String[ichLim + 1] == '\015')
					{
						cchSkip = 2;
						szCrlf = "crlf ";
					} else {
						cchSkip = 1;
						szCrlf = "lf ";
					}
				} else
					ASSERT(FALSE);

				/* We've got something to print.  Start the command */
				bufs = wsabufs;
				bufs[0].len = cchCmd;
				bufs[0].buf = szCmd1;

				/* Send our terminator */
				sprintf(szCmd2, "%sstring=", szCrlf);
				bufs[1].len = strlen(szCmd2);
				bufs[1].buf = szCmd2;

				/* Whatever we have from ichFirst to ichLim is a valid
				 * string that needs to be sent */

				bufs[2].len = ichLim - ichFirst;
				bufs[2].buf = (char*) &pdmds->String[ichFirst];

				bufs[3].len = 2;
				bufs[3].buf = "\015\012";

				bufcnt = 4;
			senddata:
				if (WSASend(s, bufs, bufcnt, &cb, 0, NULL, NULL) == SOCKET_ERROR) {
					closesocket(s);
					rgsockNotify[isock] = INVALID_SOCKET;
					break;
				}

				/* Handle the case where some data were not sent by WSASend */
				while (cb) {
					DWORD n = min(cb, bufs->len);
					cb -= n;
					bufs->buf += n;
					bufs->len -= n;
					if (bufs->len == 0)
						bufs++, bufcnt--;
				}
				if (bufcnt) goto senddata;

				ichFirst = ichLim + cchSkip;
			}
		}
		RtlLeaveCriticalSection(&csNotify);
		KeLeaveCriticalRegion();
	}
}

void GetModLoadSz(LPSTR sz, PDMN_MODLOAD pdmml)
{
	sprintf(sz, "name=\"%s\" base=0x%08x size=0x%08x check=0x%08x "
		"timestamp=0x%08x%s%s", pdmml->Name, pdmml->BaseAddress,
		pdmml->Size, pdmml->CheckSum, pdmml->TimeStamp,
		pdmml->Flags & DMN_MODFLAG_TLS ? " tls" : "",
        pdmml->Flags & DMN_MODFLAG_XBE ? " xbe" : "");
}

void GetSectLoadSz(LPSTR sz, PDMN_SECTIONLOAD pdmsl)
{
	sprintf(sz, "name=\"%s\" base=0x%08x size=0x%08x index=%d flags=%d",
		pdmsl->Name, pdmsl->BaseAddress, pdmsl->Size, pdmsl->Index,
		pdmsl->Flags);
}

BOOL FGetNotifySz(DWORD dwNotification, DWORD dwParam, LPSTR sz)
{
	char *pch;

	switch(dwNotification & DM_NOTIFICATIONMASK) {
#define pdmnbr ((PDMN_BREAK)dwParam)
	case DM_BREAK:
		sprintf(sz, "break addr=0x%08x thread=%d", pdmnbr->Address,
			pdmnbr->ThreadId);
		break;
	case DM_SINGLESTEP:
		sprintf(sz, "singlestep addr=0x%08x thread=%d", pdmnbr->Address,
			pdmnbr->ThreadId);
		break;
#undef pdmnbr
	case DM_DATABREAK:
		{
		PDMN_DATABREAK pdmdb = (PDMN_DATABREAK)dwParam;
		LPCSTR szType;
		switch(pdmdb->BreakType) {
		case DMBREAK_READWRITE:
			szType = "read";
			break;
		case DMBREAK_WRITE:
			szType = "write";
			break;
		case DMBREAK_EXECUTE:
			szType = "execute";
			break;
		default:
			return FALSE;
		}
		sprintf(sz, "data %s=0x%08x addr=0x%08x thread=%lu", szType,
			pdmdb->DataAddress, pdmdb->Address, pdmdb->ThreadId);
		}
		break;
	case DM_EXEC:
		sprintf(sz, "execution %s", rgszExec[dwParam]);
		break;
	case DM_EXCEPTION:
#define pdme ((PDMN_EXCEPTION)dwParam)
		sprintf(sz, "exception code=0x%08x thread=%d address=0x%08x",
			pdme->Code, pdme->ThreadId, pdme->Address);
		for(pch = sz; *pch; ++pch);
		if(pdme->Code == EXCEPTION_ACCESS_VIOLATION)
			sprintf(pch, " %s=0x%08x", pdme->Information[0] ? "write" : "read",
				pdme->Information[1]);
		if(pdme->Flags & DM_EXCEPT_FIRSTCHANCE)
			strcat(pch, " first");
		if(pdme->Flags & DM_EXCEPT_NONCONTINUABLE)
			strcat(pch, " noncont");
#undef pdme
		break;
	case DM_CREATETHREAD:
#define pdmct ((PDMN_CREATETHREAD)dwParam)
		sprintf(sz, "create thread=%d start=0x%08x", pdmct->ThreadId,
			pdmct->StartAddress);
#undef pdmct
		break;
	case DM_DESTROYTHREAD:
		sprintf(sz, "terminate thread=%d", dwParam);
		break;
	case DM_MODLOAD:
		strcpy(sz, "modload ");
		GetModLoadSz(sz + strlen(sz), (PDMN_MODLOAD)dwParam);
		break;
	case DM_SECTIONLOAD:
		strcpy(sz, "sectload ");
SectionStr:
		GetSectLoadSz(sz + strlen(sz), (PDMN_SECTIONLOAD)dwParam);
		break;
	case DM_SECTIONUNLOAD:
		strcpy(sz, "sectunload ");
		goto SectionStr;
#if 0
	case DM_FIBER:
#define pdmfib ((PDMN_FIBER)dwParam)
		if(pdmfib->Create)
			sprintf(sz, "fiber id=0x%08x start=0x%08x", pdmfib->FiberId,
				pdmfib->StartAddress);
		else
			sprintf(sz, "fiber id=0x%08x delete", pdmfib->FiberId);
		break;
#undef pdmfib
#endif // FIBER
	case DM_RIP:
#define pdmds ((PDMN_DEBUGSTR)dwParam)
		if(pdmds->String) {
			sprintf(sz, "rip%s thread=%lu string=%-*.*s", dwNotification &
				DM_STOPTHREAD ? " stop" : "", pdmds->ThreadId, pdmds->Length,
				pdmds->Length, pdmds->String);
			return TRUE;
		}
		sprintf(sz, "rip thread=%lu", pdmds->ThreadId);
#undef pdmds
		break;
	default:
		/* We don't know what this is */
		return FALSE;
	}
	if(dwNotification & DM_STOPTHREAD)
		strcat(sz, " stop");
	return TRUE;
}

void EnterSessionList(BOOL fWriteLock)
{
	if(fWriteLock) {
		/* We need to ensure that we have exclusive access to the list */
		KIRQL irqlSav;

		irqlSav = KfRaiseIrql(DISPATCH_LEVEL);
		KeResetEvent(&evtNotifyList);
		while(fUsingSessionList) {
			KeLowerIrql(irqlSav);
			KeWaitForSingleObject(&evtNotifyList, UserRequest, UserMode,
				FALSE, NULL);
			KfRaiseIrql(DISPATCH_LEVEL);
		}
		++fUsingSessionList;
	} else
		InterlockedIncrement(&fUsingSessionList);
}

void LeaveSessionList(void)
{
	PDMN_SESSION psess;
	PDMN_SESSION *ppsess;

	/* Try not to raise irql unless we think we need to */
	if(fPruneList && fUsingSessionList == 1) {
		KIRQL irqlSav = KfRaiseIrql(HIGH_LEVEL);
		if(0 == InterlockedDecrement(&fUsingSessionList)) {
			/* Free all of the sessions marked dead */
			ppsess = &psessFirst;
			while(*ppsess) {
				psess = *ppsess;
				if(psess->dwFlags & DMSESS_DEAD) {
					*ppsess = psess->psessNext;
					psess->dwSig = 0;
					psess->psessNext = psessFree;
					psessFree = psess;
				} else
					ppsess = &psess->psessNext;
			}
		}
		if(irqlSav <= DISPATCH_LEVEL && psessFirst) {
			/* Free the free list */
			psess = psessFree;
			psessFree = NULL;
			KeLowerIrql(DISPATCH_LEVEL);
			while(psess) {
				PDMN_SESSION psessNext = psess->psessNext;
				DmFreePool(psess);
				psess = psessNext;
			}
		}
		KfLowerIrql(irqlSav);
	} else
		InterlockedDecrement(&fUsingSessionList);
	if(KeGetCurrentIrql() <= DISPATCH_LEVEL && fUsingSessionList == 0)
		KeSetEvent(&evtNotifyList, 0, FALSE);
}

DWORD NotifyLocalComponents(DWORD dwNotification, DWORD dwParam)
{
	PDMN_SESSION psess;
	int ifn;
	DWORD dwNot = dwNotification & DM_NOTIFICATIONMASK;

	EnterSessionList(FALSE);
	for(psess = psessFirst; psess; psess = psess->psessNext) {
		if(!(psess->dwFlags & DMSESS_DEAD)) {
			ifn = psess->rgcfnNotify[dwNot];
			while(ifn--)
				(*psess->rgpfnNotify[dwNot][ifn])(dwNotification, dwParam);
		}
	}
	LeaveSessionList();
	return 0;
}

DWORD NotifyComponents(DWORD dwNotification, DWORD dwParam)
{
	int isock;
	char sz[512];
	int csockNotify;
	DWORD dwNot = dwNotification & DM_NOTIFICATIONMASK;
	KIRQL irql = KeGetCurrentIrql();

	/* We only let certain notifications go through at raised irql. */
	switch(dwNot) {
	case DM_CLOCKINT:
#if 0
	case DM_PROFINT:
#endif
		break;
	case DM_DEBUGSTR:
		if(irql == PASSIVE_LEVEL)
			break;
		if(irql <= DISPATCH_LEVEL) {
			/* Defer the print */
			PDMN_DEBUGSTR pdmds = (PDMN_DEBUGSTR)dwParam;
			PDFN pdfn;

			pdfn = DmAllocatePoolWithTag(sizeof *pdfn + pdmds->Length, 'nfdD');
			if(pdfn) {
				DWORD ef;

				pdfn->pdfnNext = NULL;
				pdfn->dmds.ThreadId = pdmds->ThreadId;
				pdfn->dmds.Length = pdmds->Length;
				pdfn->dmds.String = (PVOID)(pdfn + 1);
				memcpy(pdfn + 1, pdmds->String, pdmds->Length);
				QueueDfn(pdfn);
				SetupHalt(HF_DEFERRED);
			}
		}
		return 0;
	case DM_ASSERT:
		if(irql != PASSIVE_LEVEL)
			return 0;
		/* We need to grab the critical section now so that nobody gets a
		 * chance to interject between the assert strings and the final
		 * assert notification */
		KeEnterCriticalRegion();
		RtlEnterCriticalSection(&csNotify);
		break;
	default:
		if(irql != PASSIVE_LEVEL)
			return 0;
	}

	/* First we notify all local components */
	NotifyLocalComponents(dwNotification, dwParam);

	/* Then we notify all of the remotes */
	if(!FGetNotifySz(dwNotification, dwParam, sz)) {
		switch(dwNot) {
		case DM_ASSERT:
			SendDebugStrs(DM_ASSERT, (PDMN_DEBUGSTR)dwParam);
			sprintf(sz, "assert prompt thread=%d",
				((PDMN_DEBUGSTR)dwParam)->ThreadId);
			if(dwNotification & DM_STOPTHREAD)
				strcat(sz, " stop");
			break;
		case DM_DEBUGSTR:
			SendDebugStrs(dwNotification, (PDMN_DEBUGSTR)dwParam);
			return 0;
		case DM_NONE:
			/* This is just a keepalive event, only for the notification
			 * sockets */
			sprintf(sz, "null");
			break;
		default:
			/* We don't know what this is */
			return 0;
		}
	}
	csockNotify = 0;
	if(KeGetCurrentIrql() == PASSIVE_LEVEL) {
		/* We don't allow concurrent notifications in multiple threads */
		KeEnterCriticalRegion();
		RtlEnterCriticalSection(&csNotify);
		for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock)
			if(rgsockNotify[isock] != INVALID_SOCKET) {
				if(FPrintNotifySock(isock, sz))
					++csockNotify;
			}
		RtlLeaveCriticalSection(&csNotify);
		KeLeaveCriticalRegion();
	}
	if(dwNot == DM_ASSERT) {
		RtlLeaveCriticalSection(&csNotify);
		KeLeaveCriticalRegion();
	}
	return csockNotify;
}

struct TST {
	BOOL fStop;
	PKSYSTEM_ROUTINE pfn;
};

void FStartThread(struct TST *ptst, PKSTART_ROUTINE pfnStart, PVOID pvContext)
{
	DMN_CREATETHREAD dmct;
	DWORD dwNotification = DM_CREATETHREAD;
	PDMTD pdmtd = DmGetCurrentDmtd();

	/* We only notify for XAPI threads */
	if(PsGetCurrentThread()->Tcb.TlsData) {
		_asm cli
		ptst->fStop = g_dmi.Flags & DMIFLAG_BREAK;
		g_dmi.Flags &= ~DMIFLAG_BREAK;
		_asm sti
		ptst->pfn = pdmtd->SystemStartupRoutine;
		dmct.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
		dmct.StartAddress = pfnStart;
		// should we suppress the stop if we're going to break anyway?
		if(g_grbitStopOn & DMSTOP_CREATETHREAD) {
			dwNotification |= DM_STOPTHREAD;
			PrepareToStop();
		}
		NotifyComponents(dwNotification, (DWORD)&dmct);
		if(dwNotification & DM_STOPTHREAD)
			FStopAtException();
	}
	ASSERT(pdmtd);
	ASSERT(pdmtd->DebugFlags & DMFLAG_STARTTHREAD);
	pdmtd->DebugFlags &= ~DMFLAG_STARTTHREAD;
}

void EnterDebuggerDpc(PKDPC pdpc, PVOID pv1, PVOID pv2, PVOID pv3)
{
	fAllowKd = TRUE;
	/* Fool our exception handler into thinking the debugger is connected
	 * (we know it is, otherwise how would we be here) -- if something goes
	 * wrong, kd can always clean it up later */
	//*KdDebuggerNotPresent = FALSE;
	ExceptionsToKd();
}

BOOLEAN EnterDebugger(void)
{
	PLIST_ENTRY ple;
	PETHREAD pthr;
	PDMTD pdmtd;
	BOOLEAN fNoBreak = TRUE;

	ASSERT(KeGetCurrentIrql() >= DISPATCH_LEVEL);

	/* Walk the thread list and find out whether any threads have exceptions
	 * pending.  If so, we need to schedule a DPC to forward them */
	ple = pprocSystem->ThreadListHead.Flink;
	while(ple != &pprocSystem->ThreadListHead) {
		pthr = (PETHREAD)CONTAINING_RECORD(ple, KTHREAD, ThreadListEntry);
		ple = ple->Flink;
		pdmtd = (PDMTD)pthr->DebugData;
		if(pdmtd && pdmtd->ExceptionContext)
			fNoBreak = FALSE;
	}

	if(!fNoBreak)
		KeInsertQueueDpc(&dpcBreakIn, NULL, NULL);
	return fNoBreak;
}

BOOLEAN TrapHandler(PEXCEPTION_TRAP pexc)
{
	BOOL fPassAlong = TRUE;
	BOOLEAN fRet;
	BOOL fHaveKd = fAllowKd && !*KdDebuggerNotPresent;
	DMTD *pdmtd = DmGetCurrentDmtd();
	//static int nLock = -1;
	BOOL fDebuggable;
    PEXCEPTION_RECORD per;
    PCONTEXT pcr;
	BOOL fDebuggableThread;

    per = pexc->ExceptionRecord;
    pcr = pexc->ContextRecord;

	/* We can only do any work if we haven't been reentered -- plus we don't
	 * allow any debugging of the debugger itself (except at DPC or higher),
	 * nor do we allow any debugging inside of a bugcheck */
	fDebuggableThread = KeGetCurrentThread()->TlsData != NULL ||
		(pdmtd && !(pdmtd->DebugFlags & DMFLAG_DEBUGTHREAD));
	fDebuggable = pdmtd && !pdmtd->ExceptionContext && (fDebuggableThread ||
		KeGetCurrentIrql() >= DISPATCH_LEVEL);
	if(!(g_dmi.Flags & DMIFLAG_BUGCHECK) && fDebuggable) {
		if(pdmtd) {
			pdmtd->ExceptionRecord = per;
			pdmtd->ExceptionContext = pcr;
			pdmtd->ExceptionDr6 = DwExchangeDr6(0);
			pdmtd->DebugFlags &= ~DMFLAG_KDEXCEPT;
		}

		if(per->ExceptionCode == STATUS_BREAKPOINT) {
			/* If this is a breakpoint trap, it could be one of a number of
			 * things */
			switch(per->ExceptionInformation[0]) {
			case BREAKPOINT_BREAK:
				{
				DMN_BREAK dmnbr;
				BOOL fOurBP;
				DWORD dwNotification = DM_BREAK;

				dmnbr.Address = (void *)pcr->Eip;
				dmnbr.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
				fOurBP = FIsBreakpoint((BYTE *)pcr->Eip);
				/* We'll stop here for all breakpoints, with a few exceptions.
				 * If it's not our breakpoint and KD is installed, we won't
				 * stop, but let the breakpoint go along to KD.  If the irql
				 * is too high to stop, we won't bother to stop and we'll clean
				 * up after the breakpoint.  If there's nobody listening,
				 * we'll go ahead and stop, assuming that somebody may soon
				 * come along.  If the debugger is present and we're at greater
				 * than passive level, pass the breakpoint on to KD */
				fPassAlong = !fOurBP && (fHaveKd || (KeGetCurrentIrql() != PASSIVE_LEVEL && !*KdDebuggerNotPresent));
				if(!fPassAlong && KeGetCurrentIrql() == PASSIVE_LEVEL) {
					dwNotification |= DM_STOPTHREAD;
					PrepareToStop();
				}
				NotifyComponents(dwNotification, (DWORD)&dmnbr);
				fRet = TRUE;
				if(!fPassAlong) {
					if(KeGetCurrentIrql() == PASSIVE_LEVEL)
						FStopAtException();
					/* Pass to kd if requested.  We'll fix up the breakpoint
					 * for our breakpoints, but for breakpoints that aren't
					 * ours, we'll leave eip alone in case kd needs it */
					if(pdmtd->DebugFlags & DMFLAG_KDEXCEPT)
						fPassAlong = TRUE;
					if(fOurBP || !fPassAlong)
						ResumeAfterBreakpoint(pcr);
				}
				}
				break;
			case BREAKPOINT_PRINT:
				{
				DMN_DEBUGSTR dmds;
				DWORD dwNotification = DM_DEBUGSTR;
				BOOL fStop;

				dmds.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
				dmds.Length = ((PSTRING)per->ExceptionInformation[1])->Length;
				dmds.String = ((PSTRING)per->ExceptionInformation[1])->Buffer;
				fStop = (g_grbitStopOn & DMSTOP_DEBUGSTR) &&
					KeGetCurrentIrql() ==  PASSIVE_LEVEL;
				if(fStop) {
					dwNotification |= DM_STOPTHREAD;
					PrepareToStop();
				}
				NotifyComponents(dwNotification, (DWORD)&dmds);
				if(fStop)
					FStopAtException();
				}
				break;

			case BREAKPOINT_RTLASSERT:
				{
				DWORD dwNotification = DM_ASSERT;
				DMN_DEBUGSTR dmds;
				DWORD dwEipSav;

				dwEipSav = pcr->Eip;
				dmds.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
				dmds.Length = ((PSTRING)per->ExceptionInformation[1])->Length;
				dmds.String = ((PSTRING)per->ExceptionInformation[1])->Buffer;
				/* We stop if it's the end of the assert, and we're at lowered
				 * irql, and the assert isn't going to kd */
				fPassAlong = fHaveKd || KeGetCurrentIrql() != PASSIVE_LEVEL;
				if(!fPassAlong) {
					dwNotification |= DM_STOPTHREAD;
					PrepareToStop();
				}
				NotifyComponents(dwNotification, (DWORD)&dmds);
				if(!fPassAlong) {
					FStopAtException();
					if(pdmtd->DebugFlags & DMFLAG_KDEXCEPT)
						fPassAlong = TRUE;
				}
				if(!fPassAlong) {
					if(pcr->Eip == dwEipSav)
						// need to skip the breakpoint
						++pcr->Eip;
				}
				}
				fRet = TRUE;
				break;

			case BREAKPOINT_RIP:
				{
				DMN_DEBUGSTR dmds;
				DWORD dwNotification = DM_RIP;
				DWORD dwEipSav;

				dwEipSav = pcr->Eip;
				dmds.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
				dmds.String = (LPSTR)per->ExceptionInformation[1];
				/* If the string contains a newline, we won't be able to send
				 * anything past it, so we consider that the end of the
				 * string */
				dmds.Length = 0;
				while(dmds.String[dmds.Length] && dmds.String[dmds.Length] !=
						'\n')
					++dmds.Length;
				fPassAlong = fHaveKd || KeGetCurrentIrql() != PASSIVE_LEVEL;
				if(!fPassAlong) {
					dwNotification |= DM_STOPTHREAD;
					PrepareToStop();
				}
				NotifyComponents(dwNotification, (DWORD)&dmds);
				if(!fPassAlong) {
					FStopAtException();
					if(pdmtd->DebugFlags & DMFLAG_KDEXCEPT)
						fPassAlong = TRUE;
				}
				if(!fPassAlong) {
					if(pcr->Eip == dwEipSav)
						// need to skip the breakpoint
						++pcr->Eip;
				}
				}
				fRet = TRUE;
				break;

			case BREAKPOINT_LOAD_SYMBOLS:
				{
				DMN_MODLOAD dmml;
				ANSI_STRING *pasz = (ANSI_STRING *)per->ExceptionInformation[1];
				KD_SYMBOLS_INFO *pkdsi = (KD_SYMBOLS_INFO *)per->ExceptionInformation[2];

				strncpy(dmml.Name, pasz->Buffer, pasz->Length);
				dmml.Name[pasz->Length] = 0;
				dmml.BaseAddress = pkdsi->BaseOfDll;
				dmml.Size = pkdsi->SizeOfImage;
				dmml.CheckSum = pkdsi->CheckSum;
				dmml.TimeStamp = 0;

                if (dmml.BaseAddress == XeImageHeader() ||
                    (MmDbgReadCheck(&XeImageHeader()->NtBaseOfDll) &&
                    dmml.BaseAddress == XeImageHeader()->NtBaseOfDll))
                {
                    FixupBistroImage();
                    dmml.Flags = DwXmhFlags(&dmml);
                } else {
                    dmml.Flags = 0;
                }

				NotifyComponents(DM_MODLOAD, (DWORD)&dmml);
				}
				break;

			case BREAKPOINT_LOAD_XESECTION:
				{
				DMN_SECTIONLOAD dmsl;
				PXBEIMAGE_SECTION pxsh = (PVOID)per->ExceptionInformation[1];

				DmslFromXsh(pxsh, &dmsl);
				NotifyComponents(DM_SECTIONLOAD, (DWORD)&dmsl);
				/* If any breakpoints were placed in this section, we need
				 * to enable them now */
				EnableBreakpointsInRange((PBYTE)dmsl.BaseAddress,
					(PBYTE)dmsl.BaseAddress + dmsl.Size, TRUE);
				}
				break;

			case BREAKPOINT_UNLOAD_XESECTION:
				{
				DMN_SECTIONLOAD dmsl;
				PXBEIMAGE_SECTION pxsh = (PVOID)per->ExceptionInformation[1];

				DmslFromXsh(pxsh, &dmsl);
				/* Any breakpoints that were placed in this section need to
				 * be disabled now */
				DisableBreakpointsInRange((PBYTE)dmsl.BaseAddress,
					(PBYTE)dmsl.BaseAddress + dmsl.Size);
				NotifyComponents(DM_SECTIONUNLOAD, (DWORD)&dmsl);
				}
				break;

#if 0
			case BREAKPOINT_CREATE_FIBER:
				{
				DMN_FIBER dmfib;

				dmfib.Create = TRUE;
				dmfib.FiberId = per->ExceptionInformation[1];
				dmfib.StartAddress = (PVOID)per->ExceptionInformation[2];
				NotifyComponents(DM_FIBER, (DWORD)&dmfib);
				}
				break;

			case BREAKPOINT_DELETE_FIBER:
				{
				DMN_FIBER dmfib;

				dmfib.Create = FALSE;
				dmfib.FiberId = per->ExceptionInformation[1];
				NotifyComponents(DM_FIBER, (DWORD)&dmfib);
				}
				break;
#endif
			}
		} else if(per->ExceptionCode == STATUS_SINGLE_STEP) {
			DMN_DATABREAK dmdb;

			/* Figure out whether this was a single step due to a hw breakpoint
			 * or a trace */
			if(FMatchDataBreak(pdmtd->ExceptionDr6, &dmdb)) {
				/* It's a real data breakpoint, so we need to notify for it */
				DWORD dwNotification = DM_DATABREAK;
				BOOL fStop = KeGetCurrentIrql() == PASSIVE_LEVEL;
				dmdb.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
				dmdb.Address = (PVOID)pcr->Eip;
				if(fStop) {
					dwNotification |= DM_STOPTHREAD;
					PrepareToStop();
				}
				NotifyComponents(dwNotification, (DWORD)&dmdb);
				if(fStop)
					FStopAtException();
				fPassAlong = FALSE;
				fRet = TRUE;
			} else if(FBreakTrace(pcr)) {
				/* If this is because we're running past a breakpoint, we just
				 * patch things up and act like nothing happened */
				fPassAlong = FALSE;
				fRet = TRUE;
			} else {
				DMN_BREAK dmbr;
				DWORD dwNotification = DM_SINGLESTEP;
				BOOL fStop;

				/* We stop like it's a breakpoint unless KD is around */
				dmbr.Address = (PVOID)pcr->Eip;
				dmbr.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
				fStop = pdmtd && !fHaveKd && KeGetCurrentIrql() ==
					PASSIVE_LEVEL;
				if(fStop) {
					dwNotification |= DM_STOPTHREAD;
					PrepareToStop();
				}
				NotifyComponents(dwNotification, (DWORD)&dmbr);
				if(fStop)
					FStopAtException();
				fPassAlong = fHaveKd || (pdmtd->DebugFlags & DMFLAG_KDEXCEPT);
				fRet = TRUE;
			}
		} else {
			BOOL fException;
			DMN_EXCEPTION dme;
			DWORD dwNotification = DM_EXCEPTION;
			BOOL fStop;

			/* Notify everybody of the exception */
			dme.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
			dme.Code = per->ExceptionCode;
			dme.Address = per->ExceptionAddress;
			dme.Flags = per->ExceptionFlags & EXCEPTION_NONCONTINUABLE ?
				DM_EXCEPT_NONCONTINUABLE : 0;
			/* Stop on first chance exceptions if we're supposed to do so, and
			 * stop on second-chance exceptions always, unless kd is supposed
			 * to see the exception */
			fStop = pdmtd && KeGetCurrentIrql() == PASSIVE_LEVEL;
			if(!pexc->SecondChance) {
				dme.Flags |= DM_EXCEPT_FIRSTCHANCE;
				if(!(g_grbitStopOn & DMSTOP_FCE))
					fStop = FALSE;
				if(pdmtd)
					pdmtd->DebugFlags |= DMFLAG_FCE;
			} else if(fHaveKd)
				fStop = FALSE;
			if(dme.Code == EXCEPTION_ACCESS_VIOLATION) {
				dme.Information[0] = per->ExceptionInformation[0];
				dme.Information[1] = per->ExceptionInformation[1];
			}
			if(fStop) {
				dwNotification |= DM_STOPTHREAD;
				PrepareToStop();
			}
			NotifyComponents(dwNotification, (DWORD)&dme);

			if(fStop)
				fException = FStopAtException();
			else
				fException = TRUE;

			/* We pass the exception along if we want to let KD see it, unless
			 * we got a continue without processing */
			fPassAlong = (fHaveKd && fException) ||
				pdmtd->DebugFlags & DMFLAG_KDEXCEPT;
			/* If this isn't going to KD, we allow further processing based on
			 * whether the continue requested it */
			fRet = !fException;
		}
		if(pdmtd) {
			pdmtd->DebugFlags &= ~DMFLAG_FCE;
			pdmtd->ExceptionContext = NULL;
			pdmtd->ExceptionRecord = NULL;
		}
    } else if((pdmtd && !fDebuggableThread) ||
        (per->ExceptionCode == STATUS_BREAKPOINT &&
        (per->ExceptionInformation[0] == BREAKPOINT_PRINT ||
        per->ExceptionInformation[0] == BREAKPOINT_KDPRINT)))
    {
        /* This can go straight down to kd, which we're already set up to
         * do, so do nothing here */
	} else if(!pdmtd) {
		/* This thread got created without thread-specific debug data and
		 * we're not in a position to try to process this exception. */
	} else if(per->ExceptionCode == STATUS_BREAKPOINT &&
			per->ExceptionInformation[0] == BREAKPOINT_BREAK) {
		/* We're reentrant on a breakpoint, so we need to skip it */
		KIRQL irqlSav;

		KeRaiseIrql(HIGH_LEVEL, &irqlSav);
		/* If the old irql was high, then we may well have reentered here,
		 * because perhaps there's a breakpoint set in the breakpoint search
		 * code.  If so, we'll just pass it down to kd (or bugcheck if there's
		 * no kd).  If we are able to search the breakpoint list, and find
		 * that this is our breakpoint, we'll try just to disable it */
		if(irqlSav < HIGH_LEVEL && FIsBreakpoint((BYTE *)pcr->Eip)) {
			DisableBreakpoint((BYTE *)pcr->Eip);
			fPassAlong = FALSE;
			fRet = TRUE;
		}
		KeLowerIrql(irqlSav);
    }

	if(fPassAlong) {
		fRet = (*pfnKdpTrap)(pexc->TrapFrame, pexc->ExceptionFrame, per, pcr,
            pexc->SecondChance);
		/* If we have data bps set, kd may have just stomped on them, so we
		 * reset all the ones we have enabled */
		SyncHwbps();
	}
	DwExchangeDr6(0);
	return fRet;
}

BOOLEAN DmTrapHandler(PKTRAP_FRAME ptf, PKEXCEPTION_FRAME pef,
	PEXCEPTION_RECORD per, PCONTEXT pcr, BOOLEAN fSecondChance)
{
    DMTD *pdmtd = DmGetCurrentDmtd();
    if(pdmtd)
        return (BOOLEAN)CallOnStack(TrapHandler, &pdmtd->ExceptionStack, &ptf);
    else
        return TrapHandler((PEXCEPTION_TRAP)&ptf);
}

void SetupKernelNotifier(DWORD dw, PDMN_SESSION psess)
{
	BOOL fInstall = FALSE;
	PVOID *ppvKernelFunc;
	PVOID pvFunc;

	switch(dw) {
	case DM_CLOCKINT:
		ppvKernelFunc = g_dmi.ClockIntRoutine;
		pvFunc = ClockInt;
		break;
#if 0
	case DM_PROFINT:
		ppvKernelFunc = g_dmi.ProfIntRoutine;
		pvFunc = ProfInt;
		break;
#endif
	case DM_THREADSWITCH:
		ppvKernelFunc = g_dmi.CtxSwapNotifyRoutine;
		pvFunc = CtxSwap;
		break;
	default:
		ppvKernelFunc = NULL;
		break;
	}

	if(ppvKernelFunc) {
		while(psess) {
			if(!(psess->dwFlags & DMSESS_DEAD) && psess->rgcfnNotify[dw])
				fInstall = TRUE;
			psess = psess->psessNext;
		}
		*ppvKernelFunc = fInstall ? pvFunc : NULL;
	}
}

HRESULT DmOpenNotificationSession(DWORD dwFlags, PDMN_SESSION *ppsess)
{
	KIRQL irqlSav;

	if (!ppsess)
		return E_INVALIDARG;

	irqlSav = KeGetCurrentIrql();
	ASSERT(irqlSav == PASSIVE_LEVEL);
	*ppsess = DmAllocatePoolWithTag(sizeof(DMN_SESSION), 'snmd');
	if(!*ppsess)
		return E_OUTOFMEMORY;
	EnterSessionList(TRUE);
	RtlZeroMemory(*ppsess, sizeof(DMN_SESSION));
	(*ppsess)->dwFlags = dwFlags;
	(*ppsess)->dwSig = 'xnot';
	(*ppsess)->psessNext = psessFirst;
	psessFirst = *ppsess;
	LeaveSessionList();
	KeLowerIrql(irqlSav);
	return XBDM_NOERR;
}

HRESULT DmCloseNotificationSession(PDMN_SESSION psess)
{
	PDMN_SESSION *ppsess;

	if (!psess || psess->dwSig != 'xnot')
		return E_INVALIDARG;

	EnterSessionList(FALSE);
	psess->dwFlags |= DMSESS_DEAD;
	fPruneList = TRUE;
	LeaveSessionList();

	return XBDM_NOERR;
}

HRESULT DmNotify(PDMN_SESSION psess, DWORD dw, PDM_NOTIFY_FUNCTION pfnHandler)
{
	HRESULT hr;
	PDMN_SESSION psessSav;
	KIRQL irqlSav;

	if (!psess || psess->dwSig != 'xnot')
		return E_INVALIDARG;

	irqlSav = KeGetCurrentIrql();
	ASSERT(irqlSav == PASSIVE_LEVEL);
	dw &= DM_NOTIFICATIONMASK;
	EnterSessionList(TRUE);
	/* Temporarily unhook the notification list */
	psessSav = psessFirst;
	psessFirst = NULL;
	if (dw == DM_NONE) {
		if(pfnHandler) {
			/* We need to remove this handler from all of the lists */
			while(++dw <= DM_NOTIFYMAX) {
				int ifn;

				for(ifn = 0; ifn < psess->rgcfnNotify[dw]; ++ifn)
					if(psess->rgpfnNotify[dw][ifn] == pfnHandler)
						psess->rgpfnNotify[dw][ifn--] =
							psess->rgpfnNotify[dw][--psess->rgcfnNotify[dw]];
				SetupKernelNotifier(dw, psessSav);
			}
		} else {
			/* We need to empty all lists */
			while(++dw <= DM_NOTIFYMAX) {
				psess->rgcfnNotify[dw] = 0;
				SetupKernelNotifier(dw, psessSav);
			}
		}
		hr = XBDM_NOERR;
	} else if(dw <= DM_NOTIFYMAX && pfnHandler) {
		int ifn;

		for(ifn = psess->rgcfnNotify[dw]; ifn--; )
			if(psess->rgpfnNotify[dw][ifn] == pfnHandler)
				break;
		if(ifn < 0) {
			if(psess->rgcfnNotify[dw] == MAX_NOTIFY) {
				hr = E_OUTOFMEMORY;
				pfnHandler = NULL;
			} else {
				psess->rgpfnNotify[dw][psess->rgcfnNotify[dw]++] = pfnHandler;
				SetupKernelNotifier(dw, psessSav);
			}
		} else
			pfnHandler = NULL;
		if(dw == DM_EXEC && pfnHandler)
			/* This is a special case and every new handler has to be
			 * immediately informed of the current state, if
			 * available */
			(*pfnHandler)(dw, dwExecState);
		hr = XBDM_NOERR;
	} else
		hr = E_INVALIDARG;
	psessFirst = psessSav;
	LeaveSessionList();
	KeLowerIrql(irqlSav);
	return hr;
}

HRESULT DmRegisterNotificationProcessor(PDMN_SESSION psess, LPCSTR sz,
	PDM_EXT_NOTIFY_FUNCTION pfn)
{
    int ich;
	int ienh;
	ENH *penh;
	HRESULT hr;
	PDMN_SESSION psessSav;
	KIRQL irqlSav;

	if (!psess || psess->dwSig != 'xnot')
		return E_INVALIDARG;
    if(pfn && !sz)
        return E_INVALIDARG;

	irqlSav = KeGetCurrentIrql();
	ASSERT(irqlSav == PASSIVE_LEVEL);
	EnterSessionList(TRUE);
	/* Temporarily unhook the notification list */
	psessSav = psessFirst;
	psessFirst = NULL;
    if(!pfn) {
        /* We're unregistering.  If we have a string, then only unhook those
         * handlers that match the string */
        if(sz)
            for(ich = 0; sz[ich]; ++ich);
        for(ienh = 0; ienh < MAX_ENH; ++ienh) {
            if(psess->rgpenh[ienh]) {
                if(sz && !FEqualRgch(psess->rgpenh[ienh]->szExtName, sz, ich))
                    continue;
                DmFreePool(psess->rgpenh[ienh]);
                psess->rgpenh[ienh] = NULL;
            }
        }
        penh = NULL;
		hr = XBDM_NOERR;
    } else {
	    /* Let's find somewhere to put it */
	    penh = DmAllocatePoolWithTag(sizeof *penh, 'hcmd');
	    if(!penh)
		    hr = E_OUTOFMEMORY;
	    else {
		    for(ienh = 0; ienh < MAX_ENH; ++ienh)
			    if(!psess->rgpenh[ienh])
				    break;
		    if(ienh < MAX_ENH) {
			    int ich;

			    for(ich = 0; sz[ich] && ich < sizeof penh->szExtName - 1; ++ich)
				    penh->szExtName[ich] = sz[ich];
			    if(sz[ich])
				    hr = E_INVALIDARG;
			    else {
				    penh->szExtName[ich] = 0;
				    penh->pfnHandler = pfn;
				    psess->rgpenh[ienh] = penh;
				    penh = NULL;
				    hr = XBDM_NOERR;
			    }
		    } else
			    hr = E_OUTOFMEMORY;
	    }
    }
	psessFirst = psessSav;
	if(penh)
		DmFreePool(penh);
	LeaveSessionList();
	KeLowerIrql(irqlSav);
	return hr;
}

DMHRAPI DmSendNotificationString(LPCSTR sz)
{
	int ich;
	int ienh;
	int isock;
	PDMN_SESSION psess;

	if (!sz)
		return E_INVALIDARG;

	/* Make sure this is directed at somebody */
	for(ich = 0; sz[ich] != '!'; ++ich)
		if(sz[ich] == ' ' || sz[ich] < ' ')
			return E_INVALIDARG;

	/* Send this to anybody who's asked for it */
	EnterSessionList(FALSE);
	for(psess = psessFirst; psess; psess = psess->psessNext) {
		if(!(psess->dwFlags & DMSESS_DEAD)) {
			for(ienh = 0; ienh < MAX_ENH; ++ienh)
				if(psess->rgpenh[ienh] && FEqualRgch(sz,
						psess->rgpenh[ienh]->szExtName, ich))
					psess->rgpenh[ienh]->pfnHandler(sz);
		}
	}
	LeaveSessionList();

	/* Now send this over the network */
	if(KeGetCurrentIrql() == PASSIVE_LEVEL) {
		KeEnterCriticalRegion();
		RtlEnterCriticalSection(&csNotify);
		for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock)
			if(rgsockNotify[isock] != INVALID_SOCKET)
				FPrintNotifySock(isock, sz);
		RtlLeaveCriticalSection(&csNotify);
		KeLeaveCriticalRegion();
	}
	return XBDM_NOERR;
}

void ClockInt(PKTRAP_FRAME ptf)
{
	/* Send this to local components only */
	NotifyLocalComponents(DM_CLOCKINT, 0);
}

#if 0
void ProfInt(PKTRAP_FRAME ptf)
{
	DMN_PROFINT dmpi;

	/* Send this to local components only, and only if we think we're
	 * profiling */
	if(g_dmi.Profiling) {
		dmpi.SegCs = (USHORT)ptf->SegCs;
		dmpi.EFlags = ptf->EFlags;
		dmpi.Eip = ptf->Eip;
		NotifyLocalComponents(DM_PROFINT, (DWORD)&dmpi);
	}
}
#endif

void __fastcall CtxSwap(DWORD tidOld, DWORD tidNew)
{
	DMN_THREADSWITCH dmts;

	/* Send this to local components only */
	dmts.OldThreadId = tidOld;
	dmts.NewThreadId = tidNew;
	NotifyLocalComponents(DM_THREADSWITCH, (DWORD)&dmts);
}

void PrepareToReboot(void)
{
    int isock;
    SOCKET s;

    /* Tell everybody we're rebooting */
	NotifyComponents(DM_EXEC, DMN_EXEC_REBOOT);

    /* Stop the command server and all XAPI threads */
    StopServ();
    DoStopGo(FALSE);

    /* Close all notification sockets */
    for(isock = 0; isock < MAX_NOTIFY_SOCKETS; ++isock) {
        s = InterlockedExchange(&rgsockNotify[isock], INVALID_SOCKET);
        if(s != INVALID_SOCKET)
            closesocket(s);
    }

    /* Clean up our state */
	WriteIniFile();
	RemoveAllBreakpoints();

    /* Wait for pending I/O to complete */
    Sleep(200);
}

void ReportDoubleFault(void)
{
	DMN_EXCEPTION dme;
	DMTD *pdmtd = DmGetCurrentDmtd();
	EXCEPTION_RECORD er;

	/* We need to advertise the context we extracted.  But if we already have
	 * a saved context, then we probably overflowed the stack while preparing
	 * to notify of some other exception.  We'll turn it into a stack overflow
	 * but we'll use the old context */
	if(!pdmtd->ExceptionContext)
		pdmtd->ExceptionContext = &ctxDF;

	/* We're going to report this as stack overflow */
	dme.ThreadId = (DWORD)PsGetCurrentThread()->UniqueThread;
	dme.Code = STATUS_STACK_OVERFLOW;
	dme.Address = (PVOID)pdmtd->ExceptionContext->Eip;
	dme.Flags = DM_EXCEPT_NONCONTINUABLE;

	/* We need to make sure we have an exception record around as well */
	RtlZeroMemory(&er, sizeof er);
	er.ExceptionCode = dme.Code;
	er.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
	er.ExceptionAddress = dme.Address;
	pdmtd->ExceptionRecord = &er;

	/* We don't want to be able to continue this thread, but we don't want to
	 * upset anybody who tries, so we'll just continually report this
	 * exception */
	for(;;) {
		PrepareToStop();
		NotifyComponents(DM_EXCEPTION | DM_STOPTHREAD, (DWORD)&dme);
		FStopAtException();
	}
}

BOOLEAN CanDebugDF(KTSS *ptss)
{
	/* We've only gotten here if we actually are prepared to handle the
	 * double fault, so we should be able copy out the context and set up
	 * the handler.  But if we're supposed to let kd handle reporting, then we
	 * just give it up */
	if(fAllowKd && !*KdDebuggerNotPresent)
		return FALSE;

	/* We've got to figure out whether we'll be able to do anything with this
	 * even if we can report */
	if(KeGetCurrentIrql() != PASSIVE_LEVEL || DmGetCurrentDmtd() == NULL)
		return FALSE;

	/* For now, no FP registers */
	ctxDF.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
#define GR(reg) (ctxDF.reg = ptss->reg)
	GR(Eip);
	GR(Esp);
	GR(Ebp);
	GR(EFlags);
	ctxDF.SegCs = ptss->Cs;
	ctxDF.SegSs = ptss->Ss;

	GR(Eax);
	GR(Ebx);
	GR(Ecx);
	GR(Edx);
	GR(Esi);
	GR(Edi);
#undef GR

	/* Now we set up to actually report the double fault */
	ptss->Esp = (ULONG)(rgbDFStack + (sizeof(rgbDFStack) - 1));
	ptss->Ebp = 0;
	ptss->Eip = (ULONG)ReportDoubleFault;
	return TRUE;
}

ULONG TellDebugger(ULONG ulCode, PVOID pvData)
{
	switch(ulCode)
	{
	case DMTELL_ENTERDEBUGGER:
		EnterDebugger();
		break;
	case DMTELL_REBOOT:
		PrepareToReboot();
		break;
	case DMTELL_MAPDRIVE:
#define pdr ((PDMDRIVE)pvData)
		MapDebugDrive(pdr->Drive, pdr->Path);
		break;
#undef pdr
	case DMTELL_FRAMECOUNT:
		g_dmgd.FrameCounter = pvData;
		break;
	case DMTELL_GPUPERF:
		g_dmgd.PerfCounters = pvData;
		return SUCCEEDED(DmEnableGPUCounter(pvData != NULL));
	case DMTELL_REGISTERPERFCOUNTER:
		{
			PDMREGISTERPERFCOUNTERPARAMBLOCK pB = (PDMREGISTERPERFCOUNTERPARAMBLOCK) pvData;
			return SUCCEEDED(DmRegisterPerformanceCounter(pB->szName, pB->dwType, pB->pvArg));
		}
	case DMTELL_UNREGISTERPERFCOUNTER:
		{
			return SUCCEEDED(DmUnregisterPerformanceCounter((LPCCH) pvData));
		}
    case DMTELL_SETDBGIP:
        g_ulDbgIP = (ULONG)pvData;
        // flush the data
        WriteIniFile();
        // fall through
    case DMTELL_GETDBGIP:
        return g_ulDbgIP;
	}

	return 0;
}
