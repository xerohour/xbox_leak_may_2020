/******************************Module*Header*******************************\
* Module Name: oglctx.c
*
* This module contains the functions to support the OpenGL client side
* context interface.
*
* Copyright (c) 1998 NVidia Corporation
\**************************************************************************/

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#pragma hdrstop
#include "nv32.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvReg.h"
#include "nvapi.h"
#include "oglDD.h"
#include "oglutils.h"
#include "nvcom.h"          // drivers/common/inc

#if DBG
#define OglDebugPrint OglDebugPrintFunc
#else
#define OglDebugPrint
#endif

#define OGL_DEBUG_PREFIX "NV: "

BOOL CALLBACK ClientContextDrvFreeObj(DRIVEROBJ *pDriverObj);

//******************************************************************************
//
//  Function:   OglDebugPrintFunc
//
//  Routine Description:
//
//      This routine prints output messages for the OpenGL portion of the driver.
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************
VOID __cdecl OglDebugPrintFunc(
    PCHAR DebugMessage,
    ...
    )
{
    va_list ap;

    va_start(ap, DebugMessage);
    EngDebugPrint(OGL_DEBUG_PREFIX, DebugMessage, ap);
    va_end(ap);
}

//******************************************************************************
//
//  Function:   CreateContextClientInfo
//
//  Routine Description:
//
//      This routine associates a handle to the resource manager client and
//      a HWND with an OpenGL client.  This routine is called in response to
//      a client side escape.
//
//  Arguments:
//
//  Return Value:
//
//      FALSE indicates an allocate problem; otherwise TRUE
//
//******************************************************************************
ULONG CreateContextClientInfo(
    PDEV  *ppdev,
    HWND  hWnd, // for debugging
    ULONG hClient // key for searching for context nodes
)
{
    int i;
    LONG                   status;
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryOut;
    NV_OPENGL_CONTEXT_INFO *clientContextInfo;
    HDRVOBJ                hDrvObj = (HDRVOBJ)NULL;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

    DISPDBG((2, "CreateContextClientInfo - 0x%p %ld", hWnd, hClient));

    clientContextInfo = EngAllocMem(FL_ZERO_MEMORY,
                                    (sizeof(NV_OPENGL_CONTEXT_INFO) + PAGE_SIZE - 1),
                                    ALLOC_TAG);
    if (clientContextInfo == NULL)
    {
        DISPDBG((2, "CreateContextClientInfo - Failed EngAllocMem"));
        return((ULONG)FALSE);
    }

    OglDebugPrint("ALLOC: clientContextInfo = 0x%p\n", clientContextInfo);


    //
    // Get pointer to new client.
    //
    clientList = OglAppendToClientInfoList(ppdev,
                                           0,
                                           0,
                                           NULL,
                                           clientContextInfo);
    if (clientList)
    {
        clientInfo = &clientList->clientInfo;
    }
    else
    {
        EngFreeMem(clientContextInfo);
        return((ULONG)FALSE);
    }

    clientContextInfo->hWnd    = hWnd; // for debugging
    clientContextInfo->hClient = hClient;

    clientInfo->clientContextInfo = clientContextInfo;

    //
    // Set context is present bit in flags.
    //
    clientInfo->flags |= NV_OGL_CLIENT_CONTEXT_INFO;

    OglDebugPrint("****************** CreateContext: client count = %ld\n", globalOpenGLData.oglClientCount);

    //
    // Create driver object that tracks this client index with the client process
    //
    hDrvObj = EngCreateDriverObj((PVOID)clientInfo, ClientContextDrvFreeObj, ppdev->hdevEng);
    if (hDrvObj == NULL)
    {
        DISPDBG((1, "CreateContextClientInfo: FAILED EngCreateDriverObj"));
        OglDebugPrint("****************** FAILURE CreateContext: client count = %ld\n", globalOpenGLData.oglClientCount);
        OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));
        OglDebugPrint("FREE: clientContextInfo = 0x%p\n", clientContextInfo);
        OglDecClientCount(ppdev);
        return((ULONG)FALSE);
    }

    //
    // Save driver object and mode switch count.
    //
    clientInfo->hDrvObj         = hDrvObj;
    clientInfo->modeSwitchCount = ppdev->dwGlobalModeSwitchCount;

    OglDebugPrint("CreateContext = 0x%p 0x%p 0x%p\n", ppdev, clientInfo, clientContextInfo);

    return((ULONG)TRUE);
}

//******************************************************************************
//
//  Function:   DestroyContextClientInfo
//
//  Routine Description:
//
//      This routine frees the client drawable info structure and unbinds it
//      to the window.  This is called in reponse to the client side escape
//      or when the client side process dies.
//
//  Arguments:
//
//  Return Value:
//
//      VOID
//
//******************************************************************************
void DestroyContextClientInfo(
    PDEV  *ppdev,
    ULONG hClient,
    ULONG deleteFlag
)
{
    int i;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    NV_OPENGL_CONTEXT_INFO     *clientContextInfo = NULL;
    long status;

    DISPDBG((2, "DestroyContextClientInfo - %ld", hClient));

    clientList = OglFindClientInfoHClient(ppdev, hClient);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }

    if (clientInfo && clientInfo->flags & NV_OGL_CLIENT_CONTEXT_INFO && clientInfo->clientContextInfo)
    {
        DISPDBG((1, "DestroyContextClientInfo: hClient = %ld", (ULONG)hClient));

        clientContextInfo = clientInfo->clientContextInfo;

        //
        // Free channel and such.
        //
        NvFree(ppdev->hDriver, hClient, NV01_NULL_OBJECT, hClient);

        //
        // Free client structure memory.
        //
        EngFreeMem(clientContextInfo);
        clientInfo->clientContextInfo = NULL;

        //
        // Mark it as freed.
        //
        clientInfo->flags &= ~NV_OGL_CLIENT_CONTEXT_INFO;

        if (deleteFlag)
            {
            //
            // Delete driver object if possible
            //
            if (OglEngDeleteDriverObj(ppdev, clientInfo, NV_OGL_CLIENT_CONTEXT_INFO) == TRUE)
                {
                OglDebugPrint("****************** DestroyContext: client count = %ld\n", globalOpenGLData.oglClientCount);
                }
            }

        OglDebugPrint("DestroyContext = 0x%p 0x%p 0x%p\n", ppdev, clientInfo, clientContextInfo);
    }
}

//******************************************************************************
//
//  Function:   OglFindClientInfoHClient
//
//  Routine Description:
//
//      Find a client information structure given a resource manager hClient.
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
NV_OPENGL_CLIENT_INFO_LIST *OglFindClientInfoHClient(
    PDEV  *ppdev,
    ULONG hClient
)
{
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;

    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
        {
        if (node->clientInfo.clientContextInfo &&
            node->clientInfo.clientContextInfo->hClient == hClient)
            {
            return(node);
            }
        node = node->next;
        }

    return(NULL);
}


//******************************************************************************
//
//  Function:   OglGetClientInfoHWnd
//
//  Routine Description:
//
//      returns the hWnd belonging to the client info.
//
//  Arguments:      pClientInfo: pointer to the client info
//                  phWnd:       pointer to a hWnd this is the desired returnvalue             
//
//  Return Value:   TRUE: found hWnd
//                  FALSE: found no hWnd
//
//  MSchwarzer 09/28/2000: new
//
//******************************************************************************
BOOL bGetOglClientInfoHWnd( NV_OPENGL_CLIENT_INFO *pClientInfo, HWND* phWnd)
{
    BOOL bRet = FALSE;

    ASSERTDD( NULL != phWnd, "" );

    *phWnd = 0;

    if( pClientInfo )
    {
        if ( bOglHasDrawableInfo(pClientInfo) )
        {
            *phWnd = pClientInfo->clientDrawableInfo->hWnd;
            bRet = TRUE;
        }
        else if(  ( pClientInfo->flags & NV_OGL_CLIENT_CONTEXT_INFO )
                &&( pClientInfo->clientContextInfo )
               )
        {
            *phWnd = pClientInfo->clientContextInfo->hWnd;
            bRet = TRUE;
        }

    }
    return bRet;
}

//******************************************************************************
//
//  Function:   ClientContextDrvFreeObj
//
//  Routine Description:
//
//      This routine is the callback for freeing the driver object.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the resources are freed.
//
//******************************************************************************
BOOL CALLBACK ClientContextDrvFreeObj(
    DRIVEROBJ *pDriverObj
)
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    LONG                  status;
    int                   i, clientIndex;
    PDEV                  *ppdev;

    clientInfo = (NV_OPENGL_CLIENT_INFO *)pDriverObj->pvObj;
    ppdev = (PDEV *)pDriverObj->dhpdev;


    OglAcquireGlobalMutex();

    OglDebugPrint("CONTEXT ***************** CONTEXT DrvFreeObj = 0x%p 0x%p 0x%p 0x%p\n", ppdev, clientInfo, clientInfo->clientDrawableInfo, clientInfo->clientContextInfo);

    if (OglFindClientListFromClientInfo(ppdev, clientInfo) == NULL)
        {
        // clientInfo was removed from clientList prior to this callback
        // just return because there is nothing to do...
        OglReleaseGlobalMutex();
        return(TRUE);
        }

    //
    // Cleanup context info.
    //
    if (clientInfo->flags & NV_OGL_CLIENT_CONTEXT_INFO)
    {
        if (clientInfo->clientContextInfo)
        {
            DWORD hClient;

            try
            {
                hClient = clientInfo->clientContextInfo->hClient;
                DestroyContextClientInfo(ppdev, hClient, FALSE);
            }
            except(EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
                DISPDBG((1, "DestroyContextClientInfo: exception = 0x%lx", status));
                OglReleaseGlobalMutex();
                return(TRUE);
            }
        }
        clientInfo->flags &= ~NV_OGL_CLIENT_CONTEXT_INFO;
        clientInfo->clientContextInfo = NULL; // just for good measure...
    }

    clientInfo->hDrvObj = 0;
    clientInfo->flags   = 0;

    //
    // Remove from global client list.
    //
    OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));

    OglDebugPrint("****************** Context CALLBACK: client count = %ld\n", globalOpenGLData.oglClientCount);

    OglReleaseGlobalMutex();

    return(TRUE);
}

#if ( NVARCH >= 0x4)
void NV_OglInitState(PDEV *ppdev, void *cmdInfo)
{
    __glNVInitSharedFnPtrs((__GLNVcmdInfo *)cmdInfo);
    ppdev->oglSyncGdi       = ((__GLNVcmdInfo *)cmdInfo)->syncGdi;
}
#endif

