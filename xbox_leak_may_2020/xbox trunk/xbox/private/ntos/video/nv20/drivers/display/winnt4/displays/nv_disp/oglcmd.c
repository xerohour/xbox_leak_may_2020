//******************************Module*Header***********************************
// Module Name: oglcmd.c
//
// OpenGL command dispatcher
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

// import
#include "precomp.h"
#include "driver.h"
#include "nvdoublebuf.h"
#include "oglDD.h"
#include "oglutils.h"
#include "oglclip.h"
#include "Nvcm.h"

#if (NVARCH >= 0x4)

ULONG
NV_OglOperation(
    PDEV *ppdev,
    HWND hWnd,
    HDC hDC,
    __GLNVoperation *op
)
{
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    NV_OPENGL_DRAWABLE_INFO    *clientDrawableInfo = NULL;
    RECTL                      *windowRectList;
    ULONG                      numWindowRects;

    /* TODO -
     * perform the windowChangedCount test here
     * cache window-relative clip list so we don't have to compute it here
     */

    clientList = OglFindClientInfoFromHWndHDC(ppdev, hWnd, hDC);
    if (clientList) {
        clientInfo = &clientList->clientInfo;
    }  else {
        OglDebugPrint("NV_OglOperation: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        op->retStatus = NVOP_STATUS_ERROR; // OGL_ERROR_CLIENT_NOT_FOUND
        return FALSE;
    }

    if (clientInfo) {
        clientDrawableInfo = clientInfo->clientDrawableInfo;
    } else {
        OglDebugPrint("NV_OglOperation: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        op->retStatus = NVOP_STATUS_ERROR; // OGL_ERROR_CLIENT_NOT_FOUND
        return FALSE;
    }

    if (FALSE == NV_OglUpdateClipLists(ppdev, clientDrawableInfo)) {
        OglDebugPrint("NV_OglOperation: bailing OGL_ERROR_REALLOC_FAILURE\n");
        op->retStatus = NVOP_STATUS_ERROR; // OGL_ERROR_REALLOC_FAILURE;
        return FALSE;
    }

    op->clip.numRects = clientDrawableInfo->cachedNumClipRects;
    op->clip.rects = (__GLregionRect *)clientDrawableInfo->cachedClipRectList;
    
    op->cmdInfo->ntOther      = (void *)ppdev;
    op->cmdInfo->ntDrawable   = (void *)clientDrawableInfo;
    op->cmdInfo->ntClientInfo = clientInfo;
    ASSERT(bOglHasDrawableInfo(op->cmdInfo->ntClientInfo));

    switch (op->opCode) {
    case NVOP_CLEAR:
        //__glNVSetupClear(op);
        return TRUE;
    case NVOP_SWAP: // not yet
    default:
        op->retStatus = NVOP_STATUS_UNKNOWN_OP;
        return FALSE;
    }
}

#endif  // #if (NVARCH >= 0x4)

