/*
 *
 * tlxbox.cpp
 *
 * VC-Xbox transport
 *
 */

#include "precomp.h"
#include "tlutil.h"

HINSTANCE hInstance;
LPDBF lpdbf;
TLCALLBACKTYPE lpfnTlCallback;

#if DBG
    DEBUG_VERSION ('T', 'L', "TCP/IP Transport for Xbox");
#else
    RELEASE_VERSION ('T', 'L', "TCP/IP Transport for Xbox");
#endif

HPID g_hpid;
PDM_CONNECTION pdconMain;
PDM_CONNECTION pdconPoll;
BYTE *pbEMBuffer;
DWORD cbEMBuffer;
DWORD ibEMBuffer;
BYTE rgbReceiveBuffer[1024];
int ibReceiveBuffer;

BOOL WINAPI
DllMain(
    HINSTANCE   hInst,
    ULONG       uReason,
    LPVOID      lpReserved
    )
{

    switch (uReason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls (hInst);
            hInstance = hInst;
        break;

        case DLL_PROCESS_DETACH:
            hInstance = NULL;
        break;

    }

    return TRUE;
}

LPAVS
DBGVersionCheck(
    )
{
    return &Avs;
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
	assert(!w);
	return ib;
}

XOSD XosdSendTlPacket(PDM_CONNECTION s, char ch, DWORD cb, PVOID pv, BOOL fReply)
{
	HRESULT hr;
	char sz[512];
	DWORD cch = sizeof sz;
	XOSD xosd;
	BYTE *pbRet;
	DWORD *pcbRet;
	DWORD cbMax;

	if(fReply) {
		pbRet = pbEMBuffer;
		pcbRet = &ibEMBuffer;
		cbMax = cbEMBuffer;
	} else {
		pbRet = rgbReceiveBuffer;
		pcbRet = (DWORD *)&ibReceiveBuffer;
		cbMax = sizeof rgbReceiveBuffer;
	}

	sprintf(sz, "msvc!%c ", ch);
	if(ch == '0') {
		/* We're not actually sending a packet, we're going to send a
		 * command.  We're doing it through here so we can get the xosd
		 * return */
		hr = DmSendCommand(s, (LPCSTR)pv, sz, &cch);
	} else if(pv) {
		/* First we're going to send.  We follow the logic that 256 bytes is
		 * the breakeven between encoded and binary */
		if(cb > 256) {
			int cbT;

			sprintf(sz + 6, "g%d", cb);
			hr = DmSendCommand(s, sz, NULL, 0);
			if(FAILED(hr))
				return xosdGeneral;
			assert(hr == XBDM_READYFORBIN);
			if(FAILED(DmSendBinary(s, pv, cb)))
				return xosdGeneral;
			hr = DmReceiveStatusResponse(s, sz, &cch);
		} else {
			FillPacket64(sz + 7, (LPBYTE)pv, cb);
			hr = DmSendCommand(s, sz, sz, &cch);
		}
	} else
		/* No packet; we're going to do a get */
		hr = DmSendCommand(s, "msvc!g", sz, &cch);

	if(FAILED(hr))
		xosd = xosdGeneral;
	else if(hr == XBDM_BINRESPONSE) {
		/* We got a packet coming back */
		BYTE *pb = (BYTE *)pcbRet;
		DWORD cb = sizeof(DWORD);
		int i;

		/* First we read the data size, then we read the data */
		for(i = 0; i < 2; ++i) {
			if(FAILED(DmReceiveBinary(s, pb, cb, NULL)))
				return xosdGeneral;

			/* Got the data size, so set up for the data */
			if(!i) {
				pb = pbRet;
				cb = *pcbRet;
				assert(cb <= cbMax);
			}
		}
		xosd = xosdNone;
	} else {
		int i;

		/* We got back a valid response */
		switch(sz[5]) {
		case 'p':
			/* We have a packet in the reply */
			i = GetPacket64(sz + 6, pbRet, cbMax);
			if(i < 0) {
				assert(false);
				*pcbRet = cbMax;
			} else
				*pcbRet = i;
			xosd = xosdNone;
			break;
		case 'g':
			/* The packet is there for the taking */
			xosd = XosdSendTlPacket(s, 'g', cb, pv, fReply);
			break;
		case 'x':
			/* No packet, but an xosd */
			if(!sscanf(sz + 6, "%d", &i))
				xosd = xosdUnknown;
			else
				xosd = i;
			break;
		}
	}

	return xosd;
}

DWORD Notify(LPCSTR sz)
{
	if(sz[6] == 'g') {
		if(!pdconPoll || XosdSendTlPacket(pdconPoll, 'g', 0, NULL, TRUE) !=
				xosdNone)
			/* Can't get the packet */
			return 0;
	} else {
		ibReceiveBuffer = GetPacket64(sz + 7, rgbReceiveBuffer, sizeof
			rgbReceiveBuffer);
		assert(ibReceiveBuffer >= 0);
	}

	/* Do we care what kind of packet this is? */
	lpfnTlCallback(g_hpid, ibReceiveBuffer, (LPARAM)rgbReceiveBuffer);
	return 0;
}

XOSD WINAPI
TLFunc(
    TLF     wCommand,
    HPID    hpid,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
	XOSD xosd = xosdNone;
	char szHostName[256];
	HRESULT hr;
	WSADATA wsad;

	switch(wCommand) {
	case tlfInit:
		lpdbf = (LPDBF)wParam;
		lpfnTlCallback = (TLCALLBACKTYPE)lParam;
		break;
		
	case tlfDestroy:
		break;

	case tlfGetVersion:
		memcpy((PVOID)lParam, &Avs, sizeof Avs);
		break;

	case tlfConnect:
		if(hpid == NULL)
			/* Just snooping around */
			return xosdNone;

		_asm int 3
		assert(!pdconMain);
		assert(g_hpid == NULL);
		g_hpid = hpid;

		/* Find the Xbox */
		if(GetHostName(mptUnknown, szHostName, sizeof szHostName,
				lpdbf->lpfnGetSet))
			DmSetXboxName(szHostName);

		/* Make sure we'll be able to receive notifications */
		if(FAILED(DmNotify(DM_PERSISTENT, NULL)))
			return xosdCannotConnect;
		if(FAILED(DmRegisterNotificationProcessor("msvc", Notify)))
			return xosdUnknown;

		/* Connect two sockets -- one for requests, and one to get reply
		 * packets */
		if(FAILED(DmOpenConnection(&pdconMain)))
			return xosdCannotConnect;
		if(FAILED(DmSendCommand(pdconMain, "kd disable", NULL, 0)) ||
			FAILED(DmSendCommand(pdconMain, "msvc!c", NULL, 0)) ||
			FAILED(DmOpenConnection(&pdconPoll)))
		{
			DmCloseConnection(pdconMain);
			return xosdCannotConnect;
		}

		/* We're connected now */
		break;

	case tlfDisconnect:
		DmNotify(0, NULL);
		if(pdconMain) {
			DmCloseConnection(pdconMain);
			pdconMain = NULL;
		}
		if(pdconPoll) {
			DmCloseConnection(pdconPoll);
			pdconPoll = NULL;
		}
		g_hpid = NULL;
		break;

	case tlfLoadDM:
		if(FAILED(DmSendCommand(pdconMain, "msvc!l", NULL, 0)))
			xosd = xosdGeneral;
		break;

	case tlfSetBuffer:
		pbEMBuffer = (LPBYTE)lParam;
		cbEMBuffer = wParam;
		break;

	case tlfRequest:
		xosd = XosdSendTlPacket(pdconMain, 'q', wParam, (LPVOID)lParam, TRUE);
		break;

	case tlfReply:
		xosd = XosdSendTlPacket(pdconPoll, 'p', wParam, (LPVOID)lParam, FALSE);
		break;

	case tlfDebugPacket:
		xosd = XosdSendTlPacket(pdconMain, 'd', wParam, (LPVOID)lParam, FALSE);
		break;

	default:
		assert(FALSE);
		xosd = xosdUnknown;
		break;
	}

	return xosd;
}
