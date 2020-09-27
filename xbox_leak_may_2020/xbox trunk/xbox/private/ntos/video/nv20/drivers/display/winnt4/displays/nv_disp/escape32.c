//***************************** Module Header **********************************
//
// Module Name: escape.c
//
// Escape handler for the 64 bit driver for 32 bit clients
//
// Copyright (c) 1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 2000 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"

#if defined(_WIN64)

#include "driver.h"
#include "nvReg.h"
#include "oglDD.h"
#include "pixelfmt.h"

//******************************************************************************
//
//  Function:   DrvEscape32(SURFOBJ *, ULONG, ULONG, VOID *, ULONG cjOut, VOID *pvOut)
//
//  Routine Description:
//
//      Driver escape entry point.  This function should return TRUE for any
//      supported escapes in response to QUERYESCSUPPORT, and FALSE for any
//      others.  All supported escapes are called from this routine.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

static ULONG DoOpenGLEscapeCommand32(PPDEV ppdev, PVOID in, PVOID out);

ULONG DrvEscape32(SURFOBJ *pso, ULONG iEsc,
                  ULONG cjIn, VOID *pvIn,
                  ULONG cjOut, VOID *pvOut)

{
    PDEV    *ppdev = (PDEV *)pso->dhpdev;

    switch(iEsc)
    {
    case NV_ESC_PRIMARY_INFO:
        {
        NV_PRIMARY_INFO32 *primaryInfoIn = (NV_PRIMARY_INFO32 *)pvIn;
        NV_PRIMARY_INFO32 *primaryInfoOut = (NV_PRIMARY_INFO32 *)pvOut;
        LONG status;
        unsigned int numActiveDACs, dacMask, activeDACs, d;

        try
            {
            if (primaryInfoIn && primaryInfoOut && primaryInfoIn->ulNVSignature == NV_SIGNATURE)
                {
                primaryInfoOut->ulWidth         = ppdev->cxScreen;
                primaryInfoOut->ulHeight        = ppdev->cyScreen;
                primaryInfoOut->ulDepth         = ppdev->cjPelSize << 3;
                primaryInfoOut->ulPitch         = ppdev->lDelta;
                primaryInfoOut->ulPrimaryOffset = ppdev->ulPrimarySurfaceOffset;
                primaryInfoOut->ulNumDACs       = ppdev->ulNumberDacsOnBoard;
                primaryInfoOut->ulNumActiveDACs = ppdev->ulNumberDacsActive;
                primaryInfoOut->ulPanningPtr    = NULL;
                primaryInfoOut->ulReturnCode    = TRUE;

                // Compute a bitmap of active DACs by looping over all displays
                // until we've checked them all, or found ulNumberDacsActive
                // unique DACs--whichever comes first.
                for (d = activeDACs = numActiveDACs = 0; (d < NV_NO_DACS) &&
                     (numActiveDACs < ppdev->ulNumberDacsActive); d++)
                    {
                    if (ppdev->ulDeviceDisplay[d] < NV_NO_DACS)
                        {
                        dacMask = 1 << ppdev->ulDeviceDisplay[d];
                        if (!(dacMask & activeDACs))
                            {
                            numActiveDACs++;
                            activeDACs |= dacMask;
                            }
                        }
                    }
                primaryInfoOut->ulActiveDACs = activeDACs;
                }
            else
                {
                return (ULONG)FALSE;
                }
            }
        except(EXCEPTION_EXECUTE_HANDLER)
            {
            status = GetExceptionCode();
            DISPDBG((1, "NV_ESC_PRIMARY_INFO: exception = 0x%lx", status));
            return (ULONG)FALSE;
            }
        }
        return (ULONG)TRUE;
        break;
    case ESC_NV_OPENGL_ESCAPE:
        {
        return((ULONG)DoOpenGLEscapeCommand32(ppdev, (PVOID)pvIn, (PVOID)pvOut));
        }
        break;
    case ESC_NV_QUERYSET_REGISTRY_KEY:
        {
            __PNVRegKeyInfo32 inParms, outParms;
            WCHAR tmpStrBuf[NV_MAX_REG_KEYNAME_LEN];
            ULONG  inpStrLen, outStrLen;
            ULONG ii, returnedDataLen;
            NV_REGISTRY_STRUCT regStruct;
            NV_REGISTRY_STRUCT regStructInput, regStructOutput;
            DWORD retStatus;
            

            inParms = (__PNVRegKeyInfo32) pvIn;


            if (!inParms->keyName)
                return (FALSE);

            inpStrLen = strlen(inParms->keyName);

            if ((inpStrLen+1) > NV_MAX_REG_KEYNAME_LEN)
                return (FALSE);
            /*
            for (ii = 0; ii < inpStrLen; ii++)
                tmpStrBuf[ii] = inParms->keyName[ii];
            tmpStrBuf[ii] = 0;
            */

            EngMultiByteToUnicodeN((LPWSTR)tmpStrBuf, NV_MAX_REG_KEYNAME_LEN*sizeof(WCHAR), &outStrLen,
                                   inParms->keyName, (inpStrLen+1)*sizeof(CHAR));



            switch(inParms->type)
            {
            case NV_QUERY_REGISTRY_BINARY_KEY:
                outParms = (__PNVRegKeyInfo32) pvOut;
                *outParms = *inParms;

                regStructInput.keyName = (PWSTR)tmpStrBuf;
                regStructInput.keyVal = outParms->keyVal;
                regStructInput.keyValSize = outParms->keyValMaxSize;

                retStatus = EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_QUERY_REGISTRY_BINARY_VAL,
                               (LPVOID)&regStructInput,
                               sizeof(NV_REGISTRY_STRUCT),
                               (LPVOID) (&regStructOutput),
                               sizeof(NV_REGISTRY_STRUCT),
                               &(returnedDataLen));

                outParms = (__PNVRegKeyInfo32) pvOut;

                outParms->retStatus = retStatus;

                // Copy the returned registry value size into outParams
                outParms->keyValSize = regStructOutput.keyValSize;
                // The KeyVal has already been copied by the miniport

                break;

            case NV_QUERY_REGISTRY_KEY:

                outParms = (__PNVRegKeyInfo32) pvOut;
                *outParms = *inParms;

                retStatus = EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                               (LPVOID)tmpStrBuf,
                               outStrLen,
                               (LPVOID) (outParms->keyVal),
                               outParms->keyValMaxSize,
                               &(outParms->keyValSize));

                outParms->retStatus = retStatus;

                break;

            case NV_SET_REGISTRY_KEY:

                outParms = (__PNVRegKeyInfo32) pvOut;
                *outParms = *inParms;

                regStruct.keyName = (PWSTR)tmpStrBuf;
                regStruct.keyVal = inParms->keyVal;
                regStruct.keyValSize = inParms->keyValSize;

                retStatus = EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SET_REGISTRY_VALUE,
                               (LPVOID)(&regStruct),
                               sizeof(NV_REGISTRY_STRUCT),
                               (LPVOID) NULL,
                               0,
                               &returnedDataLen);

                outParms->retStatus = retStatus;

                break;

            default:
                break;
            }
            return (TRUE);
        }
        break;
    default:
        break;
    }

    return (ULONG)FALSE;
}

static ULONG DoOpenGLEscapeCommand32(PPDEV ppdev, PVOID pvIn, PVOID pvOut)
{
    NV_OPENGL_COMMAND32 *inCommand;
    LONG                status;
    ULONG               ulRet = FALSE;

    ASSERT(ppdev);

    inCommand = (NV_OPENGL_COMMAND32 *)pvIn;
    try
        {
        ppdev->numDevices = inCommand->numDevices;
        }
    except(EXCEPTION_EXECUTE_HANDLER)
        {
        status = GetExceptionCode();
        DISPDBG((1, "DoOpenGLEscapeCommand32: exception = 0x%lx", status));
        return(1 + (ULONG)FALSE);
        }
    switch(inCommand->command)
    {
    case ESC_NV_OPENGL_SHARED_LIBRARY_SIZES:
        ulRet = ((ULONG)TRUE);
        break;
    case ESC_NV_OPENGL_SUPPORT:
        if (OglSupportEnabled(ppdev) == (ULONG)TRUE)
        {
            if (inCommand->NVSignature == NV_SIGNATURE)
            {
                ulRet = ((ULONG)TRUE);
                break;
            }
            else
            {
                ulRet = ((ULONG)FALSE);
                break;
            }
         }
         ulRet = ((ULONG)FALSE);
         break;
    case ESC_NV_OPENGL_REGISTER_RM_CLIENT:
        OglAcquireGlobalMutex();
        OglRegisterRmClient(ppdev, (ULONG)(inCommand->other));
        OglReleaseGlobalMutex();
        ulRet = ((ULONG)TRUE);
        break;
    case ESC_NV_OPENGL_UNREGISTER_RM_CLIENT:
        OglAcquireGlobalMutex();
        OglUnRegisterRmClient(ppdev, (ULONG)(inCommand->other));
        OglReleaseGlobalMutex();
        ulRet = ((ULONG)TRUE);
        break;
    case ESC_NV_OPENGL_DESTROY_DRAWABLE:
        OglAcquireGlobalMutex();
        DestroyDrawableClientInfo(ppdev, inCommand->hWnd, inCommand->hDC, TRUE);
        OglReleaseGlobalMutex();
        ulRet = ((ULONG)TRUE);
        break;
    case ESC_NV_OPENGL_START_STATE_CLIENT_TRACKING:
        OglAcquireGlobalMutex();
        ulRet = CreateContextClientInfo(ppdev, inCommand->hWnd, inCommand->hClient);
        OglReleaseGlobalMutex();
        break;
    case ESC_NV_OPENGL_STOP_STATE_CLIENT_TRACKING:
        OglAcquireGlobalMutex();
        DestroyContextClientInfo(ppdev, inCommand->hClient, TRUE);
        OglReleaseGlobalMutex();
        ulRet = ((ULONG)TRUE);
        break;
    case ESC_NV_OPENGL_ALLOC_SHARED_MEMORY:
         {
            void *globalData, *pfifoAddress, *pbusAddress;
            void * __ptr32 * outPtr = (void * __ptr32 *)pvOut;

            OglAcquireGlobalMutex();
            CreateSharedClientInfo(ppdev, inCommand->hClient, inCommand->processHandle, &globalData, &pfifoAddress, &pbusAddress);
            outPtr[0] = (void * __ptr32)globalData;
            outPtr[1] = (void * __ptr32)pfifoAddress;
            outPtr[2] = (void * __ptr32)pbusAddress;
            OglReleaseGlobalMutex();
            ulRet = ((ULONG)TRUE);
        }
        break;
    case ESC_NV_OPENGL_FREE_SHARED_MEMORY:
        OglAcquireGlobalMutex();
        DestroySharedClientInfo(ppdev, inCommand->hClient, inCommand->processHandle, inCommand->globalData, TRUE);
        OglReleaseGlobalMutex();
        ulRet = ((ULONG)TRUE);
        break;
    case ESC_NV_OPENGL_INIT_STATE:
        ppdev->oglSyncGdi       = 0;
        ulRet = ((ULONG)TRUE);
        break;
    // this is used by W2K in DrvDescribePixelFormat
    case ESC_NV_OPENGL_PFD_CHECK:
    {
        ULONG *pulPfdCheckFlags = pvOut;           
        *pulPfdCheckFlags = 0;

        ulRet = bOglPfdCheckFlags(ppdev, pulPfdCheckFlags) ? 1 : (ULONG)-1;
        break;
    }
     case ESC_NV_OPENGL_PURGE_DEVICE_BITMAP:
        ((ULONG *)pvOut)[0] = (ULONG)0; // means nothing got punted
        if (ppdev->cbGdiHeap)   // contains #bytes allocated by GDI
        {
            bMoveAllDfbsFromOffscreenToDibs(ppdev);
            ((ULONG *)pvOut)[0] = (ULONG)1; // means something got punted
        }
        ulRet = (ULONG) TRUE;
        break;
     case ESC_NV_OPENGL_FLUSH:
        {
            __GLNVflushInfo flushInfo;
            __GLNVcmdInfo cmdInfo;

            NvCopyStructIn(inCommand->other, &flushInfo, "IPPIP[IIII]IIIIi");
            NvCopyStructIn(flushInfo.cmdInfo, &cmdInfo, "PIIPPIIICIIIIIIIIIII");
            flushInfo.cmdInfo = &cmdInfo;

            ppdev->pfnAcquireOglMutex(ppdev);
            ulRet = NV_OglFlushClipped(ppdev,
                                       inCommand->hWnd,
                                       inCommand->hDC,
                                       flushInfo.hClient,
                                       &flushInfo);
            ppdev->pfnReleaseOglMutex(ppdev);
            NvCopyStructOut(inCommand->other, &flushInfo, "ippip[iiii]iiiiI");
        }
        break;
    case ESC_NV_OPENGL_ALLOC_UNIFIED_SURFACES:
        OglAllocUnifiedSurfaces(ppdev, inCommand->other);
        ulRet = (ppdev->bOglSingleBackDepthSupported &&
                 ppdev->bOglSingleBackDepthCreated);
        break;
    case ESC_NV_OPENGL_FREE_UNIFIED_SURFACES:
        OglFreeUnifiedSurfaces(ppdev);
        ulRet = (ppdev->bOglSingleBackDepthSupported &&
                 !ppdev->bOglSingleBackDepthCreated);
        break;
    case ESC_NV_OPENGL_GET_SURFACE_INFO:
        {
            NvU32 fbInfo;
            ULONG *outPtr = (ULONG *)pvOut;
                
            fbInfo = OglGetUnifiedSurfaceInfo(ppdev, inCommand->other);
            try
            {
                *outPtr = (ULONG)fbInfo;
            }
            except(EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
                DISPDBG((1, "ESC_NV_OPENGL_GET_SURFACE_INFO: ppdev: exception = 0x%lx", status));
            }
            if (fbInfo)
            {
                ulRet = ((ULONG)TRUE);
            }
            else
            {
                ulRet = ((ULONG)FALSE);
            }
        }
        break;
    case ESC_NV_OPENGL_GET_CLIP_LIST_COUNT:
        {
            ULONG *outPtr = (ULONG *)pvOut;
            try
            {
                NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
                NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

                clientList = OglFindClientInfoFromHWndHDC(ppdev, inCommand->hWnd, inCommand->hDC);
                if (clientList)
                {
                    clientInfo = &clientList->clientInfo;
                }
                if (clientInfo)
                {
                    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;

                    clientDrawableInfo = clientInfo->clientDrawableInfo;
                    *outPtr = clientDrawableInfo->numClipRects;
                }
                else
                {
                    *outPtr = -1; // client not found
                }
            }
            except(EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
                DISPDBG((1, "ESC_NV_OPENGL_GET_CLIP_LIST_COUNT: ppdev: exception = 0x%lx", status));
                }
            ulRet = ((ULONG)TRUE);
        }
        break;
    case ESC_NV_OPENGL_GET_CLIP_LIST:
        {
            ULONG *outPtr = (ULONG *)pvOut;
            try
            {
                NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

                clientList = OglFindClientInfoFromHWndHDC(ppdev, inCommand->hWnd, inCommand->hDC);
                if (clientList)
                {
                    NV_OPENGL_CLIP_LIST_DATA32 *nvClipList;
                    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;
                    int       i, rgnDataSize;
                    LPRGNDATA rgnData = NULL;
                    RECTL     *rectList = NULL, *rect = NULL;
                    static RECTL nullRect = { 0,0,0,0 };

                    clientDrawableInfo = clientList->clientInfo.clientDrawableInfo;
                    nvClipList = (NV_OPENGL_CLIP_LIST_DATA32 *)inCommand->other;

                    rect        = (RECTL *)nvClipList->rect; // bounds rectangle
                    rgnDataSize = sizeof(RGNDATAHEADER) + (clientDrawableInfo->numClipRects * sizeof(RECT));
                    rgnData     = (LPRGNDATA)nvClipList->rgnData;

                    if (rgnDataSize > nvClipList->rgnDataSize)
                    {
                        *outPtr = rgnDataSize;  // Insufficient space--return space needed.
                    }
                    else
                    {
                        rgnDataSize           = nvClipList->rgnDataSize;
                        rgnData->rdh.dwSize   = sizeof(RGNDATAHEADER);
                        rgnData->rdh.iType    = RDH_RECTANGLES;
                        rgnData->rdh.nCount   = clientDrawableInfo->numClipRects;
                        rgnData->rdh.nRgnSize = rgnDataSize;

                        rgnData->rdh.rcBound = clientDrawableInfo->rect;
                        rectList = (RECTL *)&rgnData->rdh.rcBound;
                        if (rectList->left >= rectList->right ||
                            rectList->top  >= rectList->bottom)
                       {
                            // NULL clip list
                            rgnData->rdh.nCount = 0;
                            *rectList = nullRect; // rgnData->rdh.rcBound set to NULL rectangle
                       }

                        rectList = (RECTL *)&rgnData->Buffer;
                        if ((int)rgnData->rdh.nCount <= 0)
                        {
                            rectList[0] = nullRect;
                        }
                        else
                        {
                            for (i = 0; i < (int)rgnData->rdh.nCount; i++)
                            {
                                rectList[i] = clientDrawableInfo->copyWindowRect[i];
                                if (rectList[i].left >= rectList[i].right ||
                                    rectList[i].top  >= rectList[i].bottom)
                                {
                                    rectList[i] = nullRect;
                                }
                            }
                        }

                        nvClipList->clipChangedCount = clientDrawableInfo->clipChangedCount;
                        *outPtr = 1;
                    }
                }
                else
                {
                    *outPtr = -1; // client not found
                }
            }
            except(EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
                DISPDBG((1, "ESC_NV_OPENGL_GET_CLIP_LIST: ppdev: exception = 0x%lx", status));
            }
            ulRet = ((ULONG)TRUE);
        }
        break;
    default:
        ulRet = ((ULONG)FALSE);
        break;
    }

    return(ulRet + 1);
    // Offset the return value by +1 so that the ESC_NV_OPENGL_ESCAPE escape
    // handler always returns >0, so a zero return from ExtEscape() will
    // indicate a "hard" GDI error.
}

VOID NvCopyStructIn(VOID *ptr32, VOID *ptr64, CHAR *fmt)
{
    char f;

    while ((f = *fmt++) != '\0') {
        switch(f) {
        case 'c':
            ((char *)ptr32)++;
            ((char *)ptr64)++;
            break;
        case 'C':
            *((char *)ptr64)++ = *((char *)ptr32)++;
            break;
        case 's':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x1) & ~0x1);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x1) & ~0x1);
            ((short *)ptr32)++;
            ((short *)ptr64)++;
            break;
        case 'S':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x1) & ~0x1);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x1) & ~0x1);
            *((short *)ptr64)++ = *((short *)ptr32)++;
            break;
        case 'i':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x3) & ~0x3);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x3) & ~0x3);
            ((int *)ptr32)++;
            ((int *)ptr64)++;
            break;
        case 'I':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x3) & ~0x3);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x3) & ~0x3);
            *((int *)ptr64)++ = *((int *)ptr32)++;
            break;
        case 'p':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x3) & ~0x3);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x7) & ~0x7);
            ((void * __ptr32 *)ptr32)++;
            ((void **)ptr64)++;
            break;
        case 'P':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x3) & ~0x3);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x7) & ~0x7);
            *((void **)ptr64)++ = *((void * __ptr32 *)ptr32)++;
            break;
        case '{':
        case '}':
            break;
        }
    }
}

VOID NvCopyStructOut(VOID *ptr32, VOID *ptr64, CHAR *fmt)
{
    char f;

    while ((f = *fmt++) != '\0') {
        switch(f) {
        case 'c':
            ((char *)ptr32)++;
            ((char *)ptr64)++;
            break;
        case 'C':
            *((char *)ptr32)++ = *((char *)ptr64)++;
            break;
        case 's':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x1) & ~0x1);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x1) & ~0x1);
            ((short *)ptr32)++;
            ((short *)ptr64)++;
            break;
        case 'S':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x1) & ~0x1);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x1) & ~0x1);
            *((short *)ptr32)++ = *((short *)ptr64)++;
            break;
        case 'i':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x3) & ~0x3);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x3) & ~0x3);
            ((int *)ptr32)++;
            ((int *)ptr64)++;
            break;
        case 'I':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x3) & ~0x3);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x3) & ~0x3);
            *((int *)ptr32)++ = *((int *)ptr64)++;
            break;
        case 'p':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x3) & ~0x3);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x7) & ~0x7);
            ((void **)ptr64)++;
            ((void * __ptr32 *)ptr32)++;
            break;
        case 'P':
            ptr32 = (void *)(((uintptr_t)ptr32 + 0x3) & ~0x3);
            ptr64 = (void *)(((uintptr_t)ptr64 + 0x7) & ~0x7);
            *((void * __ptr32 *)ptr32)++ = (void * __ptr32)*((void **)ptr64)++;
            break;
        case '{':
        case '}':
            break;
        }
    }
}
#endif // defined(_WIN64)
