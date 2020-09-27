/*
 *
 * threadbrk.c
 *
 * Thread and breakpoint routines
 *
 */

#include "precomp.h"

HRESULT DmGo(void)
{
	return HrOneLineCmd("GO");
}

HRESULT DmStop(void)
{
	return HrOneLineCmd("STOP");
}

HRESULT DmSetBreakpoint(PVOID addr)
{
	char sz[64];

	sprintf(sz, "BREAK ADDR=0x%08x", addr);
	return HrOneLineCmd(sz);
}

HRESULT DmRemoveBreakpoint(PVOID addr)
{
	char sz[128];

	sprintf(sz, "BREAK ADDR=0x%08x CLEAR", addr);
	return HrOneLineCmd(sz);
}

HRESULT DmIsBreakpoint(PVOID addr, LPDWORD pdwType)
{
    HRESULT hr;
    char szCmd[128];
    char szResp[128];
    DWORD cchResp;
    PDM_CONNECTION s;

    if(!pdwType)
        return E_INVALIDARG;
    hr = HrOpenSharedConnection(&s);
    if(FAILED(hr))
        return hr;
    sprintf(szCmd, "ISBREAK ADDR=0x%08x", addr);
    cchResp = sizeof szResp;
    hr = DmSendCommand(s, szCmd, szResp, &cchResp);
    if(XBDM_NOERR == hr) {
        if(!FGetDwParam(szResp, "type", pdwType))
            hr = E_UNEXPECTED;
    } else if(SUCCEEDED(hr)) {
        TerminateConnection(s);
        hr = E_UNEXPECTED;
    }
    CloseSharedConnection(s);
    return hr;
}

HRESULT DmSetDataBreakpoint(PVOID pvAddr, DWORD dwType, DWORD dwSize)
{
	char sz[128];
	LPCSTR szType;
	BOOL fClear = dwType == DMBREAK_NONE;

	if(!fClear && !(dwSize == 1 || dwSize == 2 || dwSize == 4))
		return E_INVALIDARG;

	switch(dwType) {
	case DMBREAK_NONE:
		/* For none, we just say "write" because it doesn't really matter */
	case DMBREAK_WRITE:
		szType = "WRITE";
		break;
	case DMBREAK_READWRITE:
		szType = "READ";
		break;
	case DMBREAK_EXECUTE:
		szType = "EXECUTE";
		break;
	default:
		return E_INVALIDARG;
	}

	sprintf(sz, "BREAK %s=0x%08x SIZE=%lu %s", szType, pvAddr, dwSize,
		fClear ? "CLEAR" : "");
	return HrOneLineCmd(sz);
}

typedef struct _XFLOATSAVE {
    USHORT  ControlWord;
    USHORT  StatusWord;
    USHORT  TagWord;
    USHORT  ErrorOpcode;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;
    ULONG   DataSelector;
    ULONG   MXCsr;
    ULONG   Reserved2;
    UCHAR   RegisterArea[128];
    UCHAR   XmmRegisterArea[128];
} XFLOATSAVE, *PXFLOATSAVE;

HRESULT HrGetExtContext(PDM_CONNECTION s, DWORD tid, PXFLOATSAVE pxfs)
{
	char sz[64];
	DWORD cb;
	BYTE rgbBuf[512];
	HRESULT hr;

	sprintf(sz, "GETEXTCONTEXT THREAD=%d", tid);
	hr = DmSendCommand(s, sz, NULL, 0);
	if(FAILED(hr))
		return hr;
	hr = DmReceiveBinary(s, &cb, sizeof cb, NULL);
	if(FAILED(hr))
		return hr;
	if(cb < sizeof *pxfs) {
		DmReceiveBinary(s, pxfs, cb, NULL);
		hr = E_UNEXPECTED;
	} else {
		hr = DmReceiveBinary(s, pxfs, sizeof *pxfs, NULL);
		cb -= sizeof *pxfs;
		if(SUCCEEDED(hr)) {
			while(cb) {
				DWORD cbT = cb;
				if(cbT > sizeof rgbBuf)
					cbT = sizeof rgbBuf;
				if(FAILED(DmReceiveBinary(s, rgbBuf, cb, NULL)))
					cb = cbT;
				cb -= cbT;
			}
		}
	}
	return hr;
}

HRESULT DmGetThreadContext(DWORD tid, PCONTEXT pcr)
{
	PDM_CONNECTION s;
	HRESULT hr;
	char sz[128];
	DWORD dwFlags;
	BOOL fWantCtx = FALSE;
	BOOL fWantExtCtx = FALSE;
	XFLOATSAVE xfs;
	PXFLOATSAVE pxfs;
	DWORD dwCr0Npx;

    if (!pcr)
        return E_INVALIDARG;

	dwFlags = pcr->ContextFlags;

    if((BYTE *)&pcr->Edi - (BYTE *)pcr != 0x9c) {
		_asm int 3
	}

	pcr->ContextFlags = 0;
	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		return hr;
	sprintf(sz, "GETCONTEXT THREAD=%d", tid);
	if((dwFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {
		strcat(sz, " CONTROL");
		fWantCtx = TRUE;
	}
	if((dwFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {
		strcat(sz, " INT");
		fWantCtx = TRUE;
	}
	if((dwFlags & CONTEXT_FLOATING_POINT) == CONTEXT_FLOATING_POINT) {
		strcat(sz, " FP");
		fWantCtx = TRUE;
		fWantExtCtx = TRUE;
		pxfs = &xfs;
	}
	if((dwFlags & CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS) {
		fWantExtCtx = TRUE;
		pxfs = (PXFLOATSAVE)&pcr->ExtendedRegisters;
	}
	if(fWantCtx) {
		hr = DmSendCommand(s, sz, NULL, 0);
		if(SUCCEEDED(hr) && hr != XBDM_MULTIRESPONSE) {
			TerminateConnection(s);
			hr = E_UNEXPECTED;
		}
	} else
		hr = XBDM_NOERR;
	/* Get the standard context portions */
	if(SUCCEEDED(hr) && fWantCtx) {
		for(;;) {
#define FGR(reg) FGetDwParam(sz, #reg, &pcr->reg)
			DWORD cch;
			char *pch;

			cch = sizeof sz;
			hr = DmReceiveSocketLine(s, sz, &cch);
			if(FAILED(hr) || sz[0] == '.')
				break;
			if(FGR(Esp))
				pcr->ContextFlags |= CONTEXT_CONTROL;
			FGR(Ebp);
			FGR(Eip);
			FGR(EFlags);
			if(FGR(Eax))
				pcr->ContextFlags |= CONTEXT_INTEGER;
			FGR(Ebx);
			FGR(Ecx);
			FGR(Edx);
			FGR(Esi);
			FGR(Edi);
			FGR(SegCs);
			FGR(SegSs);
			if(FGetDwParam(sz, "Cr0NpxState", &dwCr0Npx))
				pcr->ContextFlags |= CONTEXT_FLOATING_POINT;
#undef FGR
		}
	}
	/* Now get the extended context if we need it */
	if(SUCCEEDED(hr) && fWantExtCtx) {
		HRESULT hrT;
		memset(pxfs, 0, sizeof *pxfs);
		hrT = HrGetExtContext(s, tid, pxfs);
		if(SUCCEEDED(hrT)) {
			pcr->ContextFlags |= dwFlags & CONTEXT_EXTENDED_REGISTERS;
			/* We return the fp area not based on whether it was requested, but
			 * on whether we got the FP control regs earlier */
			if((pcr->ContextFlags & CONTEXT_FLOATING_POINT) ==
				CONTEXT_FLOATING_POINT)
			{
#define FGFpR(reg) pcr->FloatSave.reg = pxfs->reg
				int ireg;

				/* We need to pick apart the extended registers into the FP
				 * state */
				pcr->FloatSave.Cr0NpxState = dwCr0Npx;
				FGFpR(ControlWord);
				FGFpR(StatusWord);
				FGFpR(TagWord);
				FGFpR(ErrorOffset);
				FGFpR(ErrorSelector);
				FGFpR(DataOffset);
				FGFpR(DataSelector);
				for(ireg = 0; ireg < 8; ++ireg)
					memcpy(&pcr->FloatSave.RegisterArea[10 * ireg],
						&pxfs->RegisterArea[16 * ireg], 10);
#undef FGFpR
			}
		} else {
			pcr->ContextFlags &= CONTEXT_FULL;
			if((pcr->ContextFlags & CONTEXT_CONTROL) != CONTEXT_CONTROL &&
					(pcr->ContextFlags & CONTEXT_INTEGER) != CONTEXT_INTEGER)
				pcr->ContextFlags = 0;
			/* whether we return failure depends on whether we got
			 * any other context data */
			if(!pcr->ContextFlags)
				hr = hrT;
		}
	}
	CloseSharedConnection(s);
	return hr;
}

#define SR(reg) (sprintf(szT, " " #reg "=0x%x", pcr->reg), strcat(sz, szT))

HRESULT DmSetThreadContext(DWORD tid, PCONTEXT pcr)
{
	PDM_CONNECTION s;
	HRESULT hr;
	char szT[64];
	char sz[1024];
	XFLOATSAVE xfs;
	PXFLOATSAVE pxfs = NULL;

    if (!pcr)
        return E_INVALIDARG;

	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		return hr;
	sprintf(sz, "SETCONTEXT THREAD=%d", tid);
	if((pcr->ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {
		SR(Esp);
		SR(Ebp);
		SR(Eip);
		SR(EFlags);
	}
	if((pcr->ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {
		SR(Eax);
		SR(Ebx);
		SR(Ecx);
		SR(Edx);
		SR(Esi);
		SR(Edi);
	}
	if((pcr->ContextFlags & CONTEXT_EXTENDED_REGISTERS) ==
			CONTEXT_EXTENDED_REGISTERS)
		pxfs = (PXFLOATSAVE)&pcr->ExtendedRegisters;
	if((pcr->ContextFlags & CONTEXT_FLOATING_POINT) ==
		CONTEXT_FLOATING_POINT)
	{
#define SFpR(reg) pxfs->reg = pcr->FloatSave.reg;
#define SFpSR(reg) pxfs->reg = (USHORT)pcr->FloatSave.reg;
		if(!pxfs) {
			/* We're only modifying part of the extended register set, so we
			 * need to start with the full extended registers */
			if(SUCCEEDED(HrGetExtContext(s, tid, &xfs)))
				pxfs = &xfs;
		}
		if(pxfs) {
			int ireg;

			sprintf(szT, " Cr0NpxState=0x%x", pcr->FloatSave.Cr0NpxState);
			strcat(sz, szT);
			/* Fill in the extended registers */
			SFpSR(ControlWord);
			SFpSR(StatusWord);
			SFpSR(TagWord);
			SFpSR(ErrorOffset);
			SFpR(ErrorSelector);
			SFpR(DataOffset);
			SFpR(DataSelector);
			for(ireg = 0; ireg < 8; ++ireg)
				memcpy(&pxfs->RegisterArea[16 * ireg],
					&pcr->FloatSave.RegisterArea[10 * ireg], 10);
		}
#undef SFpR
#undef SFpSR
	}

	if(pxfs) {
		sprintf(szT, " ext=%d", sizeof *pxfs);
		strcat(sz, szT);
	}
	hr = DmSendCommand(s, sz, NULL, 0);
	if(hr == XBDM_READYFORBIN) {
		 DmSendBinary(s, pxfs, sizeof *pxfs);
		 hr = DmReceiveStatusResponse(s, NULL, 0);
	}
	CloseSharedConnection(s);
	return hr;
}

HRESULT DmGetThreadList(LPDWORD rgdw, LPDWORD pdw)
{
	DWORD cthread;
	PDM_CONNECTION s;
	HRESULT hr;
	char sz[256];
	
    if (!rgdw || !pdw)
        return E_INVALIDARG;

	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		return hr;
	hr = DmSendCommand(s, "THREADS", NULL, 0);
	if(SUCCEEDED(hr) && hr != XBDM_MULTIRESPONSE) {
		TerminateConnection(s);
		hr = E_UNEXPECTED;
	}
	if(SUCCEEDED(hr)) {
		for(cthread = 0;; ++cthread) {
			DWORD dwLen = sizeof sz;
			hr = DmReceiveSocketLine(s, sz, &dwLen);
			sz[dwLen] = 0;
			if(FAILED(hr) || sz[0] == '.')
				break;
			if(cthread < *pdw)
				rgdw[cthread] = atoi(sz);
		}
		if(cthread > *pdw)
			hr = XBDM_BUFFER_TOO_SMALL;
		else {
			*pdw = cthread;
			hr = XBDM_NOERR;
		}
	}
	CloseSharedConnection(s);
	return hr;
}

HRESULT DmHaltThread(DWORD tid)
{
	char sz[64];

	sprintf(sz, "HALT THREAD=%d", tid);
	return HrOneLineCmd(sz);
}

HRESULT DmContinueThread(DWORD tid, BOOL fException)
{
	char sz[64];

	sprintf(sz, "CONTINUE THREAD=%d %s", tid, fException ? "EXCEPTION" : "");
	return HrOneLineCmd(sz);
}

HRESULT DmGetMemory(const LPBYTE lpbAddr, DWORD cb, LPBYTE lpbBuf, LPDWORD pcbRet)
{
	PDM_CONNECTION s;
	HRESULT hr;
	DWORD cbRet;
	char sz[256];
	char *pch;

	if(!lpbBuf)
		return E_INVALIDARG;

	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		return hr;
	sprintf(sz, "GETMEM ADDR=0x%08x LENGTH=0x%08x", lpbAddr, cb);
	hr = DmSendCommand(s, sz, NULL, 0);
	if(SUCCEEDED(hr) && hr != XBDM_MULTIRESPONSE) {
		TerminateConnection(s);
		hr = E_UNEXPECTED;
	}

	if(SUCCEEDED(hr)) {
		cbRet = 0;
		for(;;) {
			DWORD cch = sizeof sz;
			hr = DmReceiveSocketLine(s, sz, &cch);
			if(*sz == '.' || FAILED(hr))
				break;
			if(!lpbBuf)
				continue;
			for(pch = sz; *pch; pch += 2)
			{
				BYTE b = 0;
				int i = 0;
				if(*pch == '?') {
					/* End of mapped memory */
					lpbBuf = NULL;
					break;
				}
				for(i = 0; i < 2; ++i)
				{
					BYTE bT = pch[i] - '0';
					if(bT > 9)
						bT -= 'A' - '0' - 10;
					if(bT > 15)
						bT -= 'a' - 'A';
					if(bT > 15) {
						/* Don't know what we're looking at */
						lpbBuf = NULL;
						break;
					}
					b = 16*b + bT;
				}
				*lpbBuf++ = b;
				++cbRet;
			}
		}
	}
	CloseSharedConnection(s);
	if(SUCCEEDED(hr)) {
		if(pcbRet)
			*pcbRet = cbRet;
		else if(cbRet != cb)
			hr = XBDM_MEMUNMAPPED;
	}
	return hr;
}

HRESULT DmSetMemory(const BYTE *lpbAddr, DWORD cb, LPBYTE lpbBuf, LPDWORD pcbRet)
{
	PDM_CONNECTION s;
	HRESULT hr;
	char sz[512];
	DWORD cbT;
	char *pch;
	DWORD cch;
	int cbSent;

	if(!lpbBuf)
		return E_INVALIDARG;
	if(pcbRet)
		*pcbRet = cb;
	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		return hr;
	while(cb && SUCCEEDED(hr)) {
		/* We write 128 bytes at a time */
		cbT = 128;
		if(cbT > cb)
			cbT = cb;
		sprintf(sz, "setmem addr=0x%08x data=", lpbAddr);
		for(pch = sz; *pch; ++pch);
		for(cbSent = cbT; cbSent--; ) {
			pch[1] = (*lpbBuf & 0xF) + '0';
			if(pch[1] > '9')
				pch[1] += 'A' - '9' - 1;
			pch[0] = ((*lpbBuf >> 4) & 0xF) + '0';
			if(pch[0] > '9')
				pch[0] += 'A' - '9' - 1;
			++lpbBuf;
			pch += 2;
		}
		*pch = 0;
		cch = sizeof sz;
		hr = DmSendCommand(s, sz, sz, &cch);
		if(hr == XBDM_NOERR || hr == XBDM_MEMUNMAPPED)
			/* Our return string is of the form "xxx- set nnn bytes */
			sscanf(sz + 9, "%d", &cbSent);
		else
			cbSent = 0;
        lpbAddr += cbSent;
		cb -= cbSent;
	}
	if(pcbRet) {
		*pcbRet -= cb;
		if(*pcbRet)
			hr = XBDM_NOERR;
	} else
		hr = XBDM_MEMSETINCOMPLETE;
	CloseSharedConnection(s);
	return hr;
}

HRESULT DmIsThreadStopped(DWORD tid, PDM_THREADSTOP pdmts)
{
	PDM_CONNECTION s;
	HRESULT hr;
	char sz[512];
	DWORD cch;

    if (!pdmts)
        return E_INVALIDARG;
    
	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		return hr;
	sprintf(sz, "ISSTOPPED THREAD=%d", tid);
	cch = sizeof sz;
	hr = DmSendCommand(s, sz, sz, &cch);
	if(SUCCEEDED(hr)) {
		/* The string we got back is in the form "200- notification string",
		 * so we just pass off that string to the notifier to construct the
		 * data we want */
		if(!FGetNotificationData(sz + 5, &pdmts->NotifiedReason, &pdmts->u))
			pdmts->NotifiedReason = DM_NONE;
	}
	CloseSharedConnection(s);
	return hr;
}

HRESULT DmGetThreadInfo(DWORD tid, PDM_THREADINFO pdmti)
{
	PDM_CONNECTION s;
	HRESULT hr;
	char sz[256];

    if (!pdmti)
        return E_INVALIDARG;

	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		return hr;
	sprintf(sz, "THREADINFO THREAD=%d", tid);
	hr = DmSendCommand(s, sz, NULL, 0);
	if(hr != XBDM_MULTIRESPONSE) {
        if(SUCCEEDED(hr)) {
		    TerminateConnection(s);
            hr = E_UNEXPECTED;
        }
        CloseSharedConnection(s);
		return hr;
	}
	memset(pdmti, 0, sizeof *pdmti);
	for(;;) {
		DWORD cch = sizeof sz;

		hr = DmReceiveSocketLine(s, sz, &cch);
		if(FAILED(hr) || *sz == '.')
			break;
		FGetDwParam(sz, "suspend", &pdmti->SuspendCount);
		FGetDwParam(sz, "priority", &pdmti->Priority);
		FGetDwParam(sz, "tlsbase", (DWORD *)&pdmti->TlsBase);
	}
	CloseSharedConnection(s);
	return hr;
}

HRESULT DmGetXtlData(PDM_XTLDATA pdmxd)
{
    char sz[256];
    PDM_CONNECTION s;
    HRESULT hr;

    if(!pdmxd)
        return E_INVALIDARG;
    hr = HrOpenSharedConnection(&s);
    if(FAILED(hr))
        return hr;
    hr = DmSendCommand(s, "XTLINFO", NULL, 0);
	if(hr != XBDM_MULTIRESPONSE) {
        if(SUCCEEDED(hr)) {
		    TerminateConnection(s);
            hr = E_UNEXPECTED;
        }
        CloseSharedConnection(s);
		return hr;
	}
    memset(pdmxd, 0, sizeof *pdmxd);
    for(;;) {
		DWORD cch = sizeof sz;

		hr = DmReceiveSocketLine(s, sz, &cch);
		if(FAILED(hr) || *sz == '.')
			break;
        FGetDwParam(sz, "lasterr", &pdmxd->LastErrorOffset);
    }
    CloseSharedConnection(s);
    return hr;
}

HRESULT DmSuspendThread(DWORD tid)
{
	char sz[64];

	sprintf(sz, "SUSPEND THREAD=%d", tid);
	return HrOneLineCmd(sz);
}

HRESULT DmResumeThread(DWORD tid)
{
	char sz[64];

	sprintf(sz, "RESUME THREAD=%d", tid);
	return HrOneLineCmd(sz);
}

HRESULT DmSetInitialBreakpoint(void)
{
	return HrOneLineCmd("BREAK START");
}

HRESULT DmStopOn(DWORD dwStop, BOOL fStop)
{
	char sz[256];
	
	strcpy(sz, fStop ? "STOPON" : "NOSTOPON");
	if(dwStop & DMSTOP_CREATETHREAD)
		strcat(sz, " CREATETHREAD");
	if(dwStop & DMSTOP_FCE)
		strcat(sz, " FCE");
	if(dwStop & DMSTOP_DEBUGSTR)
		strcat(sz, " DEBUGSTR");
	return HrOneLineCmd(sz);
}

HRESULT DmReboot(DWORD dwFlags)
{
	char *szEmpty = "";
	char *szWait;
	char *szWarm = dwFlags & DMBOOT_WARM ? " WARM" : szEmpty;
    char *szNDebug = dwFlags & DMBOOT_NODEBUG ? " NODEBUG" : szEmpty;
	char sz[64];

    if(dwFlags & DMBOOT_STOP)
        szWait = " STOP";
    else if(dwFlags & DMBOOT_WAIT)
        szWait = " WAIT";
    else
        szWait = szEmpty;
	sprintf(sz, "REBOOT%s%s%s", szWait, szWarm, szNDebug);
	return HrOneLineCmd(sz);
}

HRESULT DmSetConfigValue(ULONG ulValueIndex, ULONG ulType, PVOID pValue, ULONG cbValueLength)
{
    if (!pValue)
        return E_INVALIDARG;

	/* Note: only setting DWORD values for now */
	if ((REG_DWORD == ulType) && (NULL != pValue) && (sizeof(DWORD) == cbValueLength))
	{
		char sz[128];
		sprintf(sz, "SETCONFIG INDEX=0x%08x VALUE=0x%08x", ulValueIndex, *((PDWORD) pValue));
		return HrOneLineCmd(sz);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT DmConnectDebugger(BOOL fConnect)
{
    char sz[32];

    sprintf(sz, "DEBUGGER %s", fConnect ? "CONNECT" : "DISCONNECT");
	return HrOneLineCmd(sz);
}

HRESULT DmSetupFunctionCall(DWORD tid)
{
	char sz[256];

	sprintf(sz, "FUNCCALL THREAD=%lu", tid);
	return HrOneLineCmd(sz);
}

HRESULT DmCAPControl(LPCSTR action)
{
    char sz[128];
    if (!action)
        return E_INVALIDARG;
    sprintf(sz, "CAPCONTROL %s", action);
    return HrOneLineCmd(sz);
}

