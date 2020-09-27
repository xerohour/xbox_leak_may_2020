#ifndef __XDPVOICE_H
#define __XDPVOICE_H

#include <xtl.h>
#include <dsound.h>
#include <dplay8.h>
#include <dvoice.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TARGETS 64
extern DVID g_dvidTargets[MAX_TARGETS];
extern DWORD g_dwNumTargets;

HRESULT XDVInit(HRESULT (WINAPI *DVMsgHandler)(void *, DWORD, void *));

HRESULT XDVCreate(DWORD dwSessionType, GUID guidCT);
HRESULT XDVConnect(DWORD dwFlags);
HRESULT XDVDisconnect();
HRESULT XDVStopSession();
HRESULT XDVDoWork();

HRESULT XDVSetTransmitTargets();
HRESULT XDVClearTransmitTargets();
HRESULT XDVRemoveTransmitTarget(DVID dvidTarget);
HRESULT XDVAddTransmitTarget(DVID dvidTarget);

extern IDirectPlayVoiceServer *g_pDVServer;				// server object
extern IDirectPlayVoiceClient *g_pDVClient;				// client object

#ifdef __cplusplus
}
#endif

#endif