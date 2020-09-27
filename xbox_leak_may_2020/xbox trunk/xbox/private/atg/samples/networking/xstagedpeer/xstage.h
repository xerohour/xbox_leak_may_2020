#ifndef __XSTAGE_H
#define __XSTAGE_H

#ifdef __cplusplus
extern "C" {
#endif

void XStage_Init();
void XStage_Shutdown();
void XStage_DoMPStage(WCHAR *hostname);
void XStage_AddPlayer(WCHAR *pName, DPNID dpnidPlayer);
void XStage_DeletePlayer(DPNID dpnid);
DWORD XStage_MsgHandler(void *msg);

#ifdef __cplusplus
}
#endif
#endif