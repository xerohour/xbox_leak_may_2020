/*
**
** dminit.c
**
** Debug module initialization
**
*/

#include "dmp.h"
#include <xboxp.h>
#include "xconfig.h"
#include <xprofp.h>

BOOL fBootWait; // 2 means wait forever, 3 means wait once with no timeout
PKPROCESS pprocSystem;
DMINIT g_dmi;
DMGD g_dmgd;
DMXAPI g_dmxapi;
char rgchTitleDir[MAX_OBJ_PATH+1];
char rgchTitleName[64];
char *pszCmdLine;
RTL_CRITICAL_SECTION csIniFile;
HANDLE g_hIniFile;
BOOL g_fFinishedLoadingIniFile;
HAL_SHUTDOWN_REGISTRATION hsrAPIC = { DisableAPIC, 0 };
ULONG g_ulDbgIP;
KEVENT kevtNull;

// This is what we're calling our ini file for now.  It will change someday
char szIniName[] = "E:\\xbdm.ini";
char szDxtDir[] = "E:\\dxt";
OCHAR oszDxtTemplate[] = OTEXT("*.DXT");

VOID CreateThreadNotify(HANDLE pid, HANDLE tid, BOOLEAN fCreate);
void LoadIniFile(void);
void LoadPlugIns(void);

ULONG DmEntryPoint(PVOID pvXbdmBase, PDMINIT pdmi, ULONG unused)
{
    WSADATA wsad;
    NTSTATUS st;
    KIRQL irqlSav;

    // Pass a pointer to the kernel's global profile data structure
    DmProfileData = (XProfpGlobals*)pdmi->XProfpDataPtr;

    /* Finish the process of fixing up our image */
    if(!FFixupXbdm(pvXbdmBase, pdmi))
        return STATUS_DRIVER_ENTRYPOINT_NOT_FOUND;

	ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    /* Set things up */
    if(!FInitThreadDebugData(PsGetCurrentThread()))
        return STATUS_NO_MEMORY;
	pprocSystem = PsGetCurrentProcess();
	g_dmi = *pdmi;
    g_dmi.D3DDriverData = &g_dmgd;
    g_dmi.XapiData = &g_dmxapi;
	InitializeCriticalSection(&csIniFile);
    HalRegisterShutdownNotification(&hsrAPIC, TRUE);
    KeInitializeEvent(&kevtNull, SynchronizationEvent, TRUE);
    InitSecurity();
    InitBreakpoints();
    InitNotifications();
	InitServ();
    InitLoader();
    InitCounters();
	KeGetCurrentPrcb()->DebugMonitorData = &g_dmi;
	dwExecState = DMN_EXEC_PENDING;

    // register for thread notifications (so we can keep our per-thread data)
    st = PsSetCreateThreadNotifyRoutine(CreateThreadNotify);
    if(!NT_SUCCESS(st)) {
        ASSERT(FALSE);
        return st;
    }

    /* If we've loaded from CD, mark our paths correctly */
    if(pdmi->Flags & DMIFLAG_CDBOOT)
        szIniName[0] = szDxtDir[0] = 1;

    // Load the ini file
    LoadIniFile();
	
    // start networking
    st = XNetStartup(NULL);
    ASSERT(st == NO_ERROR);

	// start winsock
    if(0 != WSAStartup(2, &wsad))
        return STATUS_NO_MEMORY;

    /* Start our listening thread */
    if(!FStartDebugService())
        return STATUS_NO_MEMORY;

    /* Reconnect persistent notification channels.  We're not necessarily
     * ready to start getting commands from them, but we won't give them any
     * reason to do anything yet */
	EnsurePersistentSockets();

    /* Load all plugins */
    LoadPlugIns();

    /* If we're in an active debug session, we need to wait until we've
     * reconnected with the host */
    if(fBootWait && DmGetCurrentDmtd()->DebugEvent) {
		LARGE_INTEGER li;
        BOOL fWait = fBootWait;

		DbgPrint("dm: waiting for debugger connection\n");
		if(fBootWait != 2) {
			/* One wait is all we'll do */
			fBootWait = FALSE;
			WriteIniFile();
		}
		DmGetCurrentDmtd()->DebugFlags |= DMFLAG_STOPPED;
		NotifyComponents(DM_EXEC, dwExecState = DMN_EXEC_PENDING);
		/* We'll sit in a wait state for only fifteen seconds unless we've been
		 * told to wait forever */
		li.QuadPart = Int32x32To64(15000, -10000);
        st = KeWaitForSingleObject(DmGetCurrentDmtd()->DebugEvent, UserRequest,
            UserMode, FALSE, fWait == 1 ? &li : NULL);
		if(st == STATUS_TIMEOUT)
			NotifyComponents(DM_EXEC, dwExecState = DMN_EXEC_START);
		DmGetCurrentDmtd()->DebugFlags &= ~(DMFLAG_STOPPED | DMFLAG_EXCEPTION);
	} else
		NotifyComponents(DM_EXEC, dwExecState = DMN_EXEC_START);

	/* If we have a specified title directory, let's ensure it's a valid
	 * directory and then map it in */
	FTitleExists(*rgchTitleDir ? rgchTitleDir : 0, rgchTitleName, pszCmdLine,
		TRUE);
    irqlSav = KeRaiseIrqlToDpcLevel();
    if(pszCmdLine) {
        DmFreePool(pszCmdLine);
        pszCmdLine = NULL;
    }
    KeLowerIrql(irqlSav);

	*pdmi = g_dmi;
	return STATUS_SUCCESS;
}

VOID CreateThreadNotify(HANDLE pid, HANDLE tid, BOOLEAN fCreate)
{
    if(fCreate) {
		NTSTATUS st;
		PETHREAD pthr;

		st = PsLookupThreadByThreadId(tid, &pthr);
		if(NT_SUCCESS(st)) {
			if(!FInitThreadDebugData(pthr))
				DbgPrint("dm: could not alloc thread data\n");
			else if(pthr->Tcb.TlsData) {
				/* We want to advertise this thread's creation, but we need to
				 * do that within the context of the created thread, not our
				 * thread, so we replace the thread's system routine with our
				 * own, which will send the appropriate notifications */
				PDMTD pdmtd = (PDMTD)pthr->DebugData;
				PKSWITCHFRAME pksf = (PKSWITCHFRAME)pthr->Tcb.KernelStack;
				PKSYSTEM_ROUTINE *ppfn = (PVOID)(pksf + 1);

				pdmtd->DebugFlags |= DMFLAG_STARTTHREAD;
				pdmtd->SystemStartupRoutine = *ppfn;
				*ppfn = DmpThreadStartup;
			}
            ObDereferenceObject(pthr);
		} else
			DbgPrint("dm: could not get new thread\n");
    } else {
	    DMTD *pdmtd;

		/* Advertise this thread's destruction */
		if(PsGetCurrentThread()->Tcb.TlsData)
			NotifyComponents(DM_DESTROYTHREAD,
				(DWORD)PsGetCurrentThread()->UniqueThread);
        pdmtd = PsGetCurrentThread()->DebugData;
		if(pdmtd) {
			PsGetCurrentThread()->DebugData = NULL;
            if(pdmtd->ExceptionStack) {
                MmDeleteKernelStack(pdmtd->ExceptionStack,
                    (PUCHAR)pdmtd->ExceptionStack - 0x2000);
            }
			if(pdmtd->DebugEvent) {
				pdmtd->DebugEvent = NULL;
				/* There shouldn't be anybody waiting on this event */
				ASSERT(IsListEmpty(&pdmtd->DebugEventData.Header.WaitListHead));
			}
			DmFreePool(pdmtd);
		}
    }
}

VOID SetIPParam(ULONG ulValueIndex, LPCSTR pszStaticIP)
{
	ULONG ulAddr = htonl(UlAddrFromSz(pszStaticIP));

	ExSaveNonVolatileSetting(ulValueIndex, REG_DWORD, &ulAddr, sizeof(ulAddr));
}

BOOL FInitThreadDebugData(PETHREAD pthr)
{
    PDMTD pdmtd;

    /* Allocate a new thread data structure, if we don't already have one */
	pdmtd = pthr->DebugData;
	if(!pdmtd) {
		pdmtd = pthr->DebugData = DmAllocatePoolWithTag(sizeof(DMTD), 'dtmD');
		if(!pdmtd)
			return FALSE;
		RtlZeroMemory(pdmtd, sizeof(DMTD));

        /* Allocate an 8k stack on which we can process exceptions.  If this
         * allocation fails, we'll run exceptions on the thread's stack and
         * hope we don't run out of stack */
        pdmtd->ExceptionStack = MmCreateKernelStack(0x2000, TRUE);
	}

    /* Mark this as an XAPI thread */
	if(pthr->Tcb.TlsData)
		pdmtd->DebugFlags |= DMFLAG_XAPITHREAD;

    /* Set up a synchronization event for exception notification */
    if(pdmtd->DebugEvent) {
        ASSERT(FALSE);
        return TRUE;
    }
    KeInitializeEvent(&pdmtd->DebugEventData, NotificationEvent, FALSE);
	pdmtd->DebugEvent = &pdmtd->DebugEventData;
    return TRUE;
}

void DoIniLine(LPCSTR sz)
{
	char rgchIPAddr[20];
	int cchCmd = CchOfWord(sz);

	if(cchCmd == 0); // nothing to do
    else if(FEqualRgch("wait", sz, cchCmd)) {
        if(PchGetParam(sz, "forever", FALSE))
            fBootWait = 2;
        else if(PchGetParam(sz, "stop", FALSE))
            fBootWait = 3;
        else
            fBootWait = 1;
    } else if(FEqualRgch("notify", sz, cchCmd)) {
		/* Set up a notification channel here */
	} else if(FEqualRgch("notifyat", sz, cchCmd)) {
		FNotifyAtCmd(INVALID_SOCKET, sz, TRUE);
	} else if(FEqualRgch("staticip", sz, cchCmd)) {
        FGetDwParam(sz, "addr", &g_ulDbgIP);
#if 0
    // deprecated network commands
	} else if(FEqualRgch("staticip", sz, cchCmd)) {
		if(!FGetSzParam(sz, "addr", rgchIPAddr, sizeof rgchIPAddr)) {
			*rgchIPAddr = 0;
		}
		SetIPParam(XC_ONLINE_IP_ADDRESS, rgchIPAddr);
	} else if(FEqualRgch("subnetmask", sz, cchCmd)) {
		if(!FGetSzParam(sz, "addr", rgchIPAddr, sizeof rgchIPAddr)) {
			*rgchIPAddr = 0;
		}
		SetIPParam(XC_ONLINE_SUBNET_ADDRESS, rgchIPAddr);
	} else if(FEqualRgch("defgateway", sz, cchCmd)) {
		if(!FGetSzParam(sz, "addr", rgchIPAddr, sizeof rgchIPAddr)) {
			*rgchIPAddr = 0;
		}
		SetIPParam(XC_ONLINE_DEFAULT_GATEWAY_ADDRESS, rgchIPAddr);
#endif
	} else
		/* Process this as a normal command if we can */
		HandleCommand(-1, sz);
}

void LoadIniFile(void)
{
	NTSTATUS st;
	IO_STATUS_BLOCK iosb;
	char *rgchBuf;
	char *pch;
	char *pchLine;
	int cb;
	const int cbBuf = 0x1000;

	/* Make sure we can get a read buffer */
	rgchBuf = DmAllocatePoolWithTag(cbBuf + 1, 'mdbX');
	if(!rgchBuf)
		return;

	RtlEnterCriticalSection(&csIniFile);

    /* We leave the ini file open so that we can keep a reference to the FCB
     * and restrict access to the ini file if somebody tries to copy it */
    st = FCreateFile(&g_hIniFile, GENERIC_READ | SYNCHRONIZE, szIniName,
        NULL, 0, 0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);

	if(NT_SUCCESS(st)) {
		/* Repeatedly read in chunks of file until there's no more file */
		cb = 0;
		for(;;) {
			st = NtReadFile(g_hIniFile, NULL, NULL, NULL, &iosb, rgchBuf + cb,
				cbBuf - cb, NULL);
            if(NT_SUCCESS(st))
				cb += iosb.Information;
			if(!cb)
				break;
			/* Keep pulling lines out of this block to process */
			pchLine = rgchBuf;
			for(pch = rgchBuf; pch < rgchBuf + cb; ++pch) {
				if(*pch == '\012' || *pch == '\015') {
					*pch = 0;
					DoIniLine(pchLine);
					pchLine = pch + 1;
				}
			}
			/* If we've miraculously failed to hit any line breaks in the
			 * data we have, we'll treat the whole thing as one big line */
			if(pchLine == rgchBuf) {
				*pch = 0;
				DoIniLine(pchLine);
				cb = 0;
			} else {
				/* We've processed a bunch of lines and have a partial line
				 * left over, so set it aside and get ready to read more
				 * file */
				memmove(rgchBuf, pchLine, rgchBuf + cb - pchLine);
				cb -= pchLine - rgchBuf;
			}
		}
	}
    g_fFinishedLoadingIniFile = TRUE;
	RtlLeaveCriticalSection(&csIniFile);
	DmFreePool(rgchBuf);
}

#if 0
void LoadPlugIn(const OCHAR *oszName)
{
	PVOID pvImage;
    HANDLE h;

    if(SUCCEEDED(DmLoadExtension(oszName, &h, &pvImage))) {
		PDM_ENTRYPROC pfn;

		pfn = (PDM_ENTRYPROC)((ULONG_PTR)pvImage +
			RtlImageNtHeader(pvImage)->OptionalHeader.AddressOfEntryPoint);
		//_asm int 3
		pfn();
	}
}
#endif

void LoadPlugIns(void)
{
	HANDLE h;
	NTSTATUS st;
	IO_STATUS_BLOCK iosb;
	FILE_DIRECTORY_INFORMATION fda;
	OCHAR oszName[256];
    struct
    {
	    FILE_DIRECTORY_INFORMATION fna;
        OCHAR ozName[256];
    } fna;
	BOOLEAN fFirst = TRUE;
    OBJECT_STRING templateName;

	st = FCreateFile(&h, FILE_LIST_DIRECTORY | SYNCHRONIZE, szDxtDir, NULL,
		0, FILE_SHARE_READ, FILE_OPEN, FILE_DIRECTORY_FILE |
		FILE_SYNCHRONOUS_IO_NONALERT);
	if(NT_SUCCESS(st)) {
        templateName.Buffer = oszDxtTemplate;
        templateName.Length = sizeof oszDxtTemplate - sizeof(OCHAR);
        templateName.MaximumLength = sizeof oszDxtTemplate;

		do {
			st = NtQueryDirectoryFile(h, NULL, NULL, NULL, &iosb, &fna.fna,
				sizeof fna, FileDirectoryInformation, &templateName, fFirst);
			if(NT_SUCCESS(st)) {
				int cch = fna.fna.FileNameLength / sizeof(OCHAR);
				soprintf(oszName, OTEXT("%s\\%*.*s"),
					szDxtDir, cch, cch, fna.fna.FileName);
                DmLoadExtension(oszName, NULL, NULL);
			}
			fFirst = FALSE;
		} while (NT_SUCCESS(st));
		NtClose(h);
	}
}

void WriteIniData(INF *pinf, const BYTE *pb, int cb)
{
	IO_STATUS_BLOCK iosb;

	if(pb) {
		while(cb) {
			int cbT = pinf->cbBuf - pinf->cbUsed;
			if(cb >= cbT) {
				memcpy(pinf->pbBuf + pinf->cbUsed, pb, cbT);
				NtWriteFile(pinf->h, NULL, NULL, NULL, &iosb, pinf->pbBuf,
					pinf->cbBuf, NULL);
				pb += cbT;
				cb -= cbT;
				pinf->cbUsed = 0;
			} else {
				memcpy(pinf->pbBuf + pinf->cbUsed, pb, cb);
				pb += cb;
				pinf->cbUsed += cb;
				cb = 0;
			}
		}
	} else {
		/* Flush */
		NtWriteFile(pinf->h, NULL, NULL, NULL, &iosb, pinf->pbBuf,
			pinf->cbUsed, NULL);
		pinf->cbUsed = 0;
	}
}

void WriteIniSzNoCRLF(INF *pinf, LPCSTR sz)
{
	WriteIniData(pinf, sz, strlen(sz));
}

void WriteIniSz(INF *pinf, LPCSTR sz)
{
	WriteIniSzNoCRLF(pinf, sz);
	WriteIniData(pinf, "\015\012", 2);
}

void WriteIniFile(void)
{
	NTSTATUS st;
	INF inf;
    char sz[64];

    /* We don't write if we're a CD boot.  We also don't write if we haven't
     * finished reading the ini file yet */
    if((g_dmi.Flags & DMIFLAG_CDBOOT) || !g_fFinishedLoadingIniFile)
        return;

	inf.cbBuf = 0x1000;
	inf.cbUsed = 0;
	/* Make sure we can get a write buffer */
	inf.pbBuf = DmAllocatePoolWithTag(inf.cbBuf, 'mdbX');
	if(!inf.pbBuf)
		return;

	RtlEnterCriticalSection(&csIniFile);
    if(g_hIniFile)
        NtClose(g_hIniFile);

    st = FCreateFile(&inf.h, GENERIC_WRITE | SYNCHRONIZE, szIniName, NULL,
        0, 0, FILE_OVERWRITE_IF, FILE_SYNCHRONOUS_IO_NONALERT);
	if(NT_SUCCESS(st)) {
        g_hIniFile = inf.h;
		WriteIniSz(&inf, "[xbdm]");
		/* Write all the data we know about */
        switch(fBootWait) {
        case 1:
            WriteIniSz(&inf, "wait");
            break;
        case 2:
            WriteIniSz(&inf, "wait forever");
            break;
        case 3:
            WriteIniSz(&inf, "wait stop");
            break;
        }
#if 0
		/* For now, titledir is not reboot persistent */
		if(rgchTitleDir[0]) {
			WriteIniSzNoCRLF(&inf, "titledir name=");
			WriteIniSz(&inf, rgchTitleDir);
		}
#endif
		if(rgchDbgName[0]) {
			WriteIniSzNoCRLF(&inf, "dbgname name=");
			WriteIniSz(&inf, rgchDbgName);
		}
        if(g_ulDbgIP) {
            sprintf(sz, "staticip addr=0x%08x", g_ulDbgIP);
            WriteIniSz(&inf, sz);
        }
		WriteIniNotifiers(&inf);
        WriteIniSecurity(&inf);
		/* Flush */
		WriteIniData(&inf, NULL, 0);
    } else
        g_hIniFile = NULL;

	RtlLeaveCriticalSection(&csIniFile);
	DmFreePool(inf.pbBuf);
}

__declspec(naked) void DisableAPIC(PHAL_SHUTDOWN_REGISTRATION phsr)
{
    _asm {
        // Find the local APIC
        xor edx, edx
        xor eax, eax
        mov ecx, 0x1b
        rdmsr

        // Do nothing if already disabled
        test ax, 0x800
        jz disabled

        // Disable it -- shouldn't need to mark as s/w disabled
        and ah, 0xf7
        wrmsr
disabled:
        ret 4
    }
}

/* And here are some functions just to replace missing kernel functions */
#undef PsGetCurrentThreadId
HANDLE PsGetCurrentThreadId( VOID )
{
    return PsGetCurrentThread()->UniqueThread;
}
