#ifndef __OGLUTILS_H
#define __OGLUTILS_H

/*
**
** MODULE: oglutils.h
**
** little opengl helper functions should reside here
**
** Copyright (C) 2000 NVidia Corporation. All Rights Reserved.
**
*/

void __cdecl NV_OglDDrawSurfaceCreated(PDEV *);
void __cdecl NV_OglDDrawSurfaceDestroyed(PDEV *);
BOOL  bOglDirectDrawAppRunning(PPDEV);
BOOL  bOglPOWERlibAppRunning(PPDEV);
BOOL  bOglIsVisibleClient(struct _NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL  bOglIsUnclippedClient(struct _NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL  bOglIsFullscreenClient(struct _NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL  bOglIsComplexClippedClient(PPDEV ppdev, struct _NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL  bOglHasDrawableInfo(struct _NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL  bOglGetNextClient(struct _NV_OPENGL_CLIENT_INFO **ppClientInfo);
BOOL  bOglGetNextVisibleClient(struct _NV_OPENGL_CLIENT_INFO **ppClientInfo);
ULONG cOglVisibleClients(PPDEV ppdev);
ULONG cOglWindows(PPDEV ppdev);

void CreateOglGlobalMemory(PDEV *);
void DestroyOglGlobalMemory(PDEV *);
void OglDisableModeSwitchUpdate(PDEV *);
void OglEnableModeSwitchUpdate(PDEV *);
BOOL OglLockDownMemory(PDEV *, ULONG);
BOOL OglUnlockMemory(PDEV *ppdev);

BOOL bOglServicesInit(PPDEV ppdev);
BOOL bOglServicesDisable(PPDEV ppdev);
BOOL bOglAssertMode(PPDEV ppdev, BOOL bEnable);
BOOL bOglResetPDEV(PPDEV ppdevOld, PPDEV ppdevNew);

#ifdef NV_MAIN_MULTIMON
void OglInitPDEVList(void);
void OglDestroyPDEVList(void);
BOOL OglClientIntersectsDevice(PDEV *ppdev, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);
void OglSyncEngineUsingPDEV(PDEV *ppdev);
void OglAddPDEVToList(PDEV *ppdev);
void OglRemovePDEVFromList(PDEV *ppdev);

BOOL bOglGetNextPdev( IN OUT PPDEV* pppdev );
BOOL bOglRetrieveValidPDEVForDevice( IN OUT PPDEV* pppdev, IN ULONG ulDeviceNumber );
BOOL bOglRetrieveValidPDEVForOrigin( IN OUT PPDEV* pppdev, IN LONG lLeft, IN LONG lTop );
#endif

DWORD GetCurrentProcessID(VOID);
#endif //#ifndef __OGLUTILS_H
