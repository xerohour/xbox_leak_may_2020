/*
 *
 * tldxt.c
 *
 * Transport layer for the VC debug extension
 *
 */

#include "precomp.h"

int
WINAPI
DmDllMain(
    HANDLE hModule,
    DWORD  dwReason,
    DWORD  dwReserved
    );

VOID FAR PASCAL
DMFunc(
    DWORD cb,
    LPDBB lpdbb
    );

XOSD FAR PASCAL
DMInit(
    DMTLFUNCTYPE lpfnTl,
    LPTSTR        lpch
    );

BOOL fTlConnected;
BOOL fDMConnected;
LPVOID pvDMBuffer;
DWORD cbDMBuffer;
DWORD ibDMBuffer;
RTL_CRITICAL_SECTION csDMRequest;
RTL_CRITICAL_SECTION csEMRequest;
RTL_CRITICAL_SECTION csPacket;
KEVENT kevtReply;
KEVENT kevtRequest;
KEVENT kevtPacket;
PDM_CMDCONT g_pdmcc;
BYTE rgbSendPacketBuffer[1024];
int ibSendPacketBuffer;
BYTE rgbReceivePacketBuffer[1024];
int ibReceivePacketBuffer;
BOOL fUsingReceivePacketBuffer;

void AcquireSendPacket(void)
{
	/* We need to use the global packet buffer.  First we wait for its
	 * availability */
	EnterCriticalSection(&csPacket);
	KeWaitForSingleObject(&kevtPacket, UserRequest, KernelMode, FALSE,
		NULL);
	KeResetEvent(&kevtPacket);
	LeaveCriticalSection(&csPacket);
}

void FillPacket64(LPSTR sz, BYTE *pb, DWORD cb)
{
	WORD w = 0;
	int ibit = 0;
	while(cb--) {
		w |= *pb++ << ibit;
		ibit += 8;
		while(ibit >= 6) {
			*sz++ = (w & 0x3f) + '!';
			w >>= 6;
			ibit -= 6;
		}
	}
	if(ibit)
		*sz++ = (w & 0x3f) + '!';
	*sz = 0;
}

int GetPacket64(LPCSTR sz, BYTE *pb, DWORD cb)
{
	DWORD ib = 0;
	WORD w = 0;
	int ibit = 0;
	while(*sz) {
		w |= (*sz++ - '!') << ibit;
		ibit += 6;
		while(ibit >= 8) {
			if(++ib > cb)
				return -1;
			*pb++ = w & 0xff;
			w >>= 8;
			ibit -= 8;
		}
	}
	return ib;
}

XOSD SendTlPacket(char ch, DWORD cb, PVOID pv)
{
	XOSD xosd = xosdNone;

	if(ch != 'p') {
		/* We have an available packet, so set it up */
		char sz[512];
		sprintf(sz, "msvc!%c ", ch);

		if(cb < 256) {
			/* Short packets can be translated and sent on the notification
			 * line */
			FillPacket64(sz + 7, pv, cb);
			DmSendNotificationString(sz);
		} else {
			AcquireSendPacket();
			if(cb <= sizeof rgbSendPacketBuffer) {
				ibSendPacketBuffer = cb;
				memcpy(rgbSendPacketBuffer, pv, cb);
				sz[6] = 'g';
				DmSendNotificationString(sz);
			} else
				/* Should probably assert here */
				xosd = xosdUnknown;
		}
	} else if(g_pdmcc) {
		DPRINT(5, ("dxt: reply\n"));
		g_pdmcc->DataSize = cb;
		memcpy(g_pdmcc->Buffer, pv, cb);
		KeSetEvent(&kevtRequest, EVENT_INCREMENT, FALSE);
	} else {
		/* What are we replying to? */
		assert(FALSE);
	}
	return xosd;
}

XOSD TLFunc(TLF tlf, HPID hpid, LPARAM wParam, LPARAM lParam)
{
	XOSD xosd = xosdNone;

	switch(tlf)
	{
	case tlfSetBuffer:
		pvDMBuffer = (LPVOID)lParam;
		cbDMBuffer = wParam;
		break;
	
	case tlfRequest:
		if(!fTlConnected)
			xosd = xosdLineNotConnected;
		else {
			EnterCriticalSection(&csDMRequest);
            DPRINT(5, ("dxt: dmrequest\n"));
			SendTlPacket('q', wParam, (PVOID)lParam);
			KeWaitForSingleObject(&kevtReply, UserRequest, KernelMode, FALSE,
				NULL);
            DPRINT(5, ("dxt: got dmreply\n"));
			LeaveCriticalSection(&csDMRequest);
			/* Reply packets come back into the correct buffer, so we just
			 * need to extract the xosd and we're done */
			if(ibDMBuffer == 0)
				xosd = xosdInvalidParameter;
		}
		break;

	case tlfReply:
		if(!fTlConnected)
			xosd = xosdLineNotConnected;
		else
			xosd = SendTlPacket('p', wParam, (PVOID)lParam);
		break;

	case tlfDebugPacket:
		if(!fTlConnected)
			xosd = xosdLineNotConnected;
		else
			SendTlPacket('d', wParam, (PVOID)lParam);
		break;
	}

	return xosd;
}

HRESULT HrGetPacket(LPCSTR szCommand, PDM_CMDCONT pdmcc)
{
	HRESULT hr;
	BOOL fAllocated = FALSE;

	/* Get a buffer to stick this packet into */
	if(szCommand[6] == 'g') {
		/* Binary data coming in, so get its length and set things up */
		const char *pch = szCommand + 7;
		pdmcc->BytesRemaining = 0;
		while(*pch)
			pdmcc->BytesRemaining = 10 * pdmcc->BytesRemaining +
				(*pch++ - '0');
		if(!pdmcc->BytesRemaining)
			/* No data means no packet */
			return E_FAIL;
		if(!pdmcc->Buffer) {
			if(InterlockedExchange(&fUsingReceivePacketBuffer, TRUE)) {
				pdmcc->Buffer = ExAllocatePoolWithTag(PagedPool,
					pdmcc->BytesRemaining, 'cvsm');
				if(!pdmcc->Buffer)
					return E_OUTOFMEMORY;
				pdmcc->BufferSize = pdmcc->BytesRemaining;
				fAllocated = TRUE;
			} else {
				pdmcc->Buffer = rgbReceivePacketBuffer;
				pdmcc->BufferSize = sizeof rgbReceivePacketBuffer;
			}
		}
		if(pdmcc->BytesRemaining > pdmcc->BufferSize) {
TooBig:
			/* Too much data */
			ExFreePoolWithTag(pdmcc->Buffer, 'cvsm');
			return E_FAIL;
		}
		pdmcc->CustomData = (PVOID)pdmcc->BytesRemaining;
		hr = XBDM_READYFORBIN;
	} else {
		int ib;

		if(!pdmcc->Buffer) {
			pdmcc->BufferSize = sizeof rgbReceivePacketBuffer;
			if(InterlockedExchange(&fUsingReceivePacketBuffer, TRUE)) {
				pdmcc->Buffer = ExAllocatePoolWithTag(PagedPool,
					pdmcc->BufferSize, 'cvsm');
				if(!pdmcc->Buffer)
					return E_OUTOFMEMORY;
				fAllocated = TRUE;
			} else
				pdmcc->Buffer = rgbReceivePacketBuffer;
		}
		ib = GetPacket64(szCommand + 7, pdmcc->Buffer,
			pdmcc->BufferSize);
		if(ibReceivePacketBuffer < 0)
			goto TooBig;

		pdmcc->DataSize = ib;
		hr = XBDM_NOERR;
	}
	return hr;
}

HRESULT SendPacketData(PDM_CMDCONT pdmcc, LPSTR szResp, DWORD cchResp)
{
	HRESULT hr;

	if(pdmcc->BytesRemaining == 0) {
		/* We're done with the send buffer now */
		KeSetEvent(&kevtPacket, EVENT_INCREMENT, FALSE);
		hr = XBDM_ENDOFLIST;
	} else if(pdmcc->Buffer == pdmcc) {
		/* We haven't sent anything yet.  We need to send the length first */
		pdmcc->Buffer = &pdmcc->BytesRemaining;
		pdmcc->DataSize = sizeof pdmcc->BytesRemaining;
		hr = XBDM_NOERR;
	} else {
		/* We've sent the length, now send the data */
		pdmcc->Buffer = pdmcc->CustomData;
		pdmcc->DataSize = pdmcc->BytesRemaining;
		pdmcc->BytesRemaining = 0;
		hr = XBDM_NOERR;
	}
	return hr;
}

HRESULT HrDeliverPacket(PDM_CMDCONT pdmcc, LPSTR szResp, DWORD cchResp,
	BOOL fCanBinSend)
{
	HRESULT hr;
	int cbTextMax;

	/* Now what do we do with the reply packet?  The theory is that anything
	 * less than 256 bytes is best sent text-encoded in the response, and that
	 * anything bigger than that should be sent in a binary response.  If we
	 * can't send a binary response, we'll send as text if it will fit into the
	 * text buffer */
	cbTextMax = (cchResp - 8) * 3 / 4;
	if(fCanBinSend && cbTextMax > 256)
		cbTextMax = 256;
	if(pdmcc->DataSize < (DWORD)cbTextMax) {
		if(pdmcc->DataSize == 0) {
			/* Huh? */
			_asm int 3
		}
		szResp[0] = 'p';
		FillPacket64(szResp + 1, pdmcc->Buffer, pdmcc->DataSize);
		if(pdmcc->Buffer = rgbSendPacketBuffer)
			/* We're done with the send buffer now */
			KeSetEvent(&kevtPacket, EVENT_INCREMENT, FALSE);
		hr = XBDM_NOERR;
	} else if(fCanBinSend) {
		/* We're able to send a binary response, so we'll do so */
		pdmcc->HandlingFunction = SendPacketData;
		pdmcc->BytesRemaining = pdmcc->DataSize;
		pdmcc->CustomData = pdmcc->Buffer;
		pdmcc->Buffer = pdmcc;
		hr = XBDM_BINRESPONSE;
	} else {
		/* We need to advertise that the response is available as a
		 * get-packet */
		szResp[0] = 'g';
		szResp[1] = 0;
		hr = XBDM_NOERR;
	}
	return hr;
}

HRESULT HrDoRequest(PDM_CMDCONT pdmcc, LPSTR szResp, DWORD cchResp,
	BOOL fCanBinSend)
{
	BYTE *pbRequest = pdmcc->Buffer;
	DWORD cbRequest = pdmcc->DataSize;
	int cbTextMax;

	/* Prepare to receive the reply and do the action */
	EnterCriticalSection(&csEMRequest);
	g_pdmcc = pdmcc;
	AcquireSendPacket();
	pdmcc->Buffer = rgbSendPacketBuffer;
	pdmcc->BufferSize = sizeof rgbSendPacketBuffer;
	pdmcc->DataSize = 0;
	DPRINT(5, ("dxt: request\n"));
	DMFunc(cbRequest, (LPDBB)pbRequest);
	/* Wait for the reply to come through before continuing */
	KeWaitForSingleObject(&kevtRequest, UserRequest, KernelMode, FALSE,
		NULL);
	DPRINT(5, ("dxt: got reply\n"));
	g_pdmcc = NULL;
	LeaveCriticalSection(&csEMRequest);
	/* OK, we have our response data.  Send it now if we can, or mark
	 * it available for later.  In either case, we're done with the
	 * receive buffer so we can accept another request */
	if(pbRequest != rgbReceivePacketBuffer)
		ExFreePoolWithTag(pbRequest, 'cvsm');
	fUsingReceivePacketBuffer = FALSE;
	return HrDeliverPacket(pdmcc, szResp, cchResp, fCanBinSend);
}

void DoReply(PDM_CMDCONT pdmcc)
{
	ibDMBuffer = pdmcc->DataSize;
    DPRINT(5, ("dxt: dmreply\n"));
	KeSetEvent(&kevtReply, EVENT_INCREMENT, FALSE);
}

BOOL FHandleIncomingData(PDM_CMDCONT pdmcc)
{
	if(!pdmcc->DataSize) {
		/* We've lost our socket, so we'll discard what we've got and clean
		 * up */
		pdmcc->Buffer = (LPBYTE)pdmcc->Buffer + pdmcc->BytesRemaining -
			(DWORD)pdmcc->CustomData;
		if(pdmcc->Buffer != rgbReceivePacketBuffer)
			ExFreePoolWithTag(pdmcc->Buffer, 'cvsm');
		return FALSE;
	}
	pdmcc->Buffer = (LPBYTE)pdmcc->Buffer + pdmcc->DataSize;
	pdmcc->BytesRemaining -= pdmcc->DataSize;
	if(pdmcc->BytesRemaining == 0) {
		/* Restore the buffer pointer */
		pdmcc->Buffer = (LPBYTE)pdmcc->Buffer - (DWORD)pdmcc->CustomData;
		pdmcc->DataSize = (DWORD)pdmcc->CustomData;
	}
	return TRUE;
}

HRESULT HandleRequest(PDM_CMDCONT pdmcc, LPSTR szResp, DWORD cchResp)
{
	if(!FHandleIncomingData(pdmcc) || pdmcc->BytesRemaining != 0)
		return XBDM_NOERR;
	return HrDoRequest(pdmcc, szResp, cchResp, FALSE);
}

HRESULT HandleReply(PDM_CMDCONT pdmcc, LPSTR szResp, DWORD cchResp)
{
	if(!FHandleIncomingData(pdmcc) || pdmcc->BytesRemaining != 0)
		return XBDM_NOERR;
	DoReply(pdmcc);
	return XBDM_NOERR;
}

HRESULT HandleDebugPacket(PDM_CMDCONT pdmcc, LPSTR szResp, DWORD cchResp)
{
	if(!FHandleIncomingData(pdmcc) || pdmcc->BytesRemaining != 0)
		return XBDM_NOERR;
	DMFunc(pdmcc->DataSize, pdmcc->Buffer);
	return XBDM_NOERR;
}

HRESULT HrMsvcCmdProcessor(LPCSTR szCommand, LPSTR szResp, DWORD cchResp,
	PDM_CMDCONT pdmcc)
{
	XOSD xosd;
	HRESULT hr;

	switch(szCommand[5]) {
	case 'c':
		/* tlfConnect */
		if(fTlConnected)
			/* Can't connect twice */
			hr = E_FAIL;
		else {
			fTlConnected = TRUE;
			hr = XBDM_DEDICATED;
		}
		break;

	case 'l':
		/* tlfLoadDM */
		if(!fTlConnected)
			xosd = xosdLineNotConnected;
		else if(fDMConnected)
			xosd = xosdNone;
		else
			xosd = DMInit(TLFunc, NULL) ? xosdUnknown : xosdNone;
		hr = 0;
		break;

	case 'q':
		/* tlfRequest */
		if(!fTlConnected) {
			sprintf(szResp, "x%d", xosdLineNotConnected);
			hr = XBDM_NOERR;
		} else {
			pdmcc->HandlingFunction = HandleRequest;
			pdmcc->Buffer = NULL;
			/* First get the incoming packet */
			hr = HrGetPacket(szCommand, pdmcc);
			if(hr == XBDM_NOERR)
				hr = HrDoRequest(pdmcc, szResp, cchResp, TRUE);
		}
		break;

	case 'p':
		/* tlfReply */
		if(!fTlConnected) {
			sprintf(szResp, "x%d", xosdLineNotConnected);
			hr = XBDM_NOERR;
		} else {
			pdmcc->HandlingFunction = HandleReply;
			pdmcc->Buffer = pvDMBuffer;
			pdmcc->BufferSize = cbDMBuffer;
			hr = HrGetPacket(szCommand, pdmcc);
			if(hr == XBDM_NOERR)
				DoReply(pdmcc);
		}
		break;

	case 'd':
		/* tlfDebugPacket */
		if(!fTlConnected) {
			sprintf(szResp, "x%d", xosdLineNotConnected);
			hr = XBDM_NOERR;
		} else {
			pdmcc->HandlingFunction = HandleDebugPacket;
			pdmcc->Buffer = NULL;
			hr = HrGetPacket(szCommand, pdmcc);
			if(hr == XBDM_NOERR)
				DMFunc(pdmcc->DataSize, pdmcc->Buffer);
		}
		break;

	case 'g':
		/* not a tlf -- need to get the available debug packet */
		if(!fTlConnected) {
			sprintf(szResp, "x%d", xosdLineNotConnected);
			hr = XBDM_NOERR;
		} else if(!kevtPacket.Header.SignalState)
			/* No packet available */
			hr = E_FAIL;
		else {
			pdmcc->Buffer = rgbSendPacketBuffer;
			pdmcc->DataSize = ibSendPacketBuffer;
			hr = HrDeliverPacket(pdmcc, szResp, cchResp, TRUE);
		}
		break;

	default:
		hr = XBDM_INVALIDCMD;
		break;
	}

	if(!hr) {
		sprintf(szResp, "x%d", xosd);
		hr = XBDM_NOERR;
	}
	return hr;
}

void DxtEntry(ULONG unused1, ULONG unused2, ULONG unused3)
{
	/* Set up some things */
	InitializeCriticalSection(&csDMRequest);
	InitializeCriticalSection(&csEMRequest);
	InitializeCriticalSection(&csPacket);
	KeInitializeEvent(&kevtReply, SynchronizationEvent, FALSE);
	KeInitializeEvent(&kevtRequest, SynchronizationEvent, FALSE);
	KeInitializeEvent(&kevtPacket, NotificationEvent, TRUE);

	/* Register our command processor */
	DmRegisterCommandProcessor("MSVC", HrMsvcCmdProcessor);

	/* Send a DLL init to the DM */
	DmDllMain(NULL, DLL_PROCESS_ATTACH, 0);
}
