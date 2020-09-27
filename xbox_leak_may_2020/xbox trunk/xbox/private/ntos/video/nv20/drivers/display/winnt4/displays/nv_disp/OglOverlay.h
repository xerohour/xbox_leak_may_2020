#ifndef __OGLOVERLAY_H
#define __OGLOVERLAY_H
//
// MSchwarzer 09/28/2000: new
//
void OglInitLayerPalette(void);

BOOL bOglFindGlobalLayerPaletteHWND(
    IN  HWND hWnd, 
    OUT NV_LAYER_PALETTE **ppLayerPalette);

BOOL OglSetLayerPalette(__GLNVpaletteData *pSourcePalette);
BOOL OglGetLayerPalette(__GLNVpaletteData *pDestPalette);
BOOL OglDestroyLayerPalette(HWND hWnd);

BOOL bClearOverlayForClient(PPDEV ppdev, WNDOBJ *pwo, HWND hWnd, BOOL bClearFrontBuffer, BOOL bClearBackBuffer);
BOOL bClearOverlayArea(PPDEV ppdev, WNDOBJ *pwo, ULONG ulTransparentColor, BOOL bClearFrontBuffer, BOOL bClearBackBuffer);
BOOL bCopyMainPlaneFrontToPrimaryForClient(PPDEV ppdev, WNDOBJ *pwo, HWND hWnd);
BOOL bCopyIntersectingMainPlaneFrontToPSO(PPDEV  ppdev, SURFOBJ *psoSrc, SURFOBJ *psoDst, RECTL* prclDst, POINTL* pptlSrc);
BOOL bDoMergeBlitOnIntersectingRectangle(PPDEV  ppdev, RECTL* prclDst);
BOOL bOglSwapMergeBlit(PPDEV ppdev, struct __GLMergeBlitDataRec   *pMergeBlitData);

BOOL bAddPointerToList( PPOINTER_LIST, PVOID);
BOOL bIsPointerInList( PPOINTER_LIST, PVOID);
BOOL bRemovePointerFromList( PPOINTER_LIST, PVOID);
BOOL bClearPointerList( PPOINTER_LIST);
BOOL bIsPointerListEmpty( PPOINTER_LIST pList );
BOOL bRemoveFirstElementFromList( PPOINTER_LIST pList );
BOOL ulGetNumberOfPointersInList( PPOINTER_LIST pList );

BOOL bOglGetNextOverlayClient(struct _NV_OPENGL_CLIENT_INFO **ppClientInfo);
BOOL bOglGetNextVisibleOverlayClient(struct _NV_OPENGL_CLIENT_INFO **ppClientInfo);
BOOL bOglIsOverlayClient(struct _NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL bOglIsOverlayModeEnabled(PPDEV ppdev);
BOOL bOglOverlayModeUpdate(PPDEV ppdev);
BOOL bOglOverlayModeEnable(PPDEV ppdev);
BOOL bOglOverlayModeDisable(PPDEV ppdev);

ULONG cOglOverlayClients(PPDEV ppdev);
BOOL  bOglOverlaySurfacesAllowed(PPDEV ppdev);
#endif // __OGLOVERLAY_H

