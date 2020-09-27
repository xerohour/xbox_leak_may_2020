/******************************Module*Header*******************************\
* Module Name: oglclients.c
*
* This module contains the functions to support the OpenGL clients.
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
#include "nvapi.h"
#include "oglDD.h"
#include "oglutils.h"

#if DBG
#define OglDebugPrint OglDebugPrintFunc
#else
#define OglDebugPrint
#endif

//******************************************************************************
//
//  Function: FindClientListFromClientInfo
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//
//******************************************************************************
NV_OPENGL_CLIENT_INFO_LIST *OglFindClientListFromClientInfo(PDEV *ppdev, NV_OPENGL_CLIENT_INFO *clientInfo)
    {
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;

    if (clientInfo)
        {
        node = globalOpenGLData.oglClientListHead.clientList;
        while (node)
            {
            if (clientInfo == &node->clientInfo)
                {
                return(node);
                break;
                }
            node = node->next;
            }
        // Although this happens in DrvClipChanged after the ICD destroyed
        // the servers client info, it's a bug to work on invalidated data!
        DISPDBG((1, "WARNING in OglFindClientListFromClientInfo: invalid clientInfo: 0x%p", clientInfo));
        }

    return(NULL);
    }

//******************************************************************************
//
//  Function: OglAppendToClientInfoList
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//******************************************************************************
NV_OPENGL_CLIENT_INFO_LIST *OglAppendToClientInfoList(
PDEV    *ppdev, 
DWORD   flags,                               // bits indicating allocated/free resources
HDRVOBJ hDrvObj,                             // driver object handle for this client
NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo, // drawable information for this client
NV_OPENGL_CONTEXT_INFO  *clientContextInfo)  // context information for this client
    {
    NV_OPENGL_CLIENT_INFO_LIST *newNode, *node;
 
    if (globalOpenGLData.oglClientCount >= MAX_OGL_CLIENTS)
    {
        DISPDBG((2, "OglAppendToClientInfoList: Too many OpenGL clients"));
        return NULL;
    }

    newNode = EngAllocMem(FL_ZERO_MEMORY, sizeof(NV_OPENGL_CLIENT_INFO_LIST), ALLOC_TAG);
    if (newNode == NULL)
        {
        OglDebugPrint("OglAppendToClientInfoList - Failed EngAllocMem");
        return(FALSE);
        }
    memset(newNode, 0, sizeof(NV_OPENGL_CLIENT_INFO_LIST));

    newNode->refCount++;
    newNode->next                          = NULL;
    newNode->clientInfo.flags              = flags;
    newNode->clientInfo.hDrvObj            = hDrvObj;
    newNode->clientInfo.modeSwitchCount    = ppdev->dwGlobalModeSwitchCount;
    newNode->clientInfo.clientDrawableInfo = clientDrawableInfo;
    newNode->clientInfo.clientContextInfo  = clientContextInfo;
    newNode->clientInfo.dwProcessID        = GetCurrentProcessID();

    if (NULL == globalOpenGLData.oglClientListHead.clientList)
        {
        globalOpenGLData.oglClientListHead.clientList = newNode;
        }
    else
        {
        node = globalOpenGLData.oglClientListHead.clientList;
        while (node->next)
            {
            node = node->next;
            }
        node->next = newNode;
        }

    OglIncClientCount(ppdev);
    return(newNode);
    }

//******************************************************************************
//
//  Function: OglRemoveFromClientInfoList
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//******************************************************************************
void OglRemoveFromClientInfoList(
PDEV *ppdev, 
NV_OPENGL_CLIENT_INFO_LIST *toFree)
    {
    NV_OPENGL_CLIENT_INFO_LIST *node, *parNode;

    if (toFree == NULL ||
        ppdev == NULL ||
        globalOpenGLData.oglClientListHead.clientList == NULL)
        {
        return;
        }

    if (toFree->clientInfo.pwo)
        {
        // Although it's not allowed to call that here, I need to invalidate the pwo because we
        // still get called with this client, but we delete it here.
        // reproduce: run quadroview on nt5 and open a model -> page fault
        // however: will work most of the time, dunno why ?!?! [WNDOBJ_vSetConsumer() is just setting the pvConsumer entry // maybe windows is using the pvConsumer somehow ?!?!]
#if 0
        WNDOBJ_vSetConsumer(toFree->clientInfo.pwo, NULL);
#endif
        }

    if (toFree == globalOpenGLData.oglClientListHead.clientList)
        {
        if (globalOpenGLData.oglClientListHead.clientList)
            {
            globalOpenGLData.oglClientListHead.clientList->refCount--;
            if (globalOpenGLData.oglClientListHead.clientList->refCount == 0)
                {
                //
                // Long explanation:
                //    refCount is used to indicate that multiple PDEVs
                //    are using globalOpenGLData.oglClientListHead.clientList.
                //    If refCount > 0 then we cannot free the memory but
                //    we can set globalOpenGLData.oglClientListHead.clientList to
                //    NULL.  If refCount == 0 then we can free the memory
                //    associated with globalOpenGLData.oglClientListHead.clientList
                //    and set it to NULL.
                //
                //    It is very important to remember that the pointer
                //    and memory associated with globalOpenGLData.oglClientListHead.clientList
                //    maybe be shared between two different PEVs.  Multiple
                //    PDEVs get access to one globalOpenGLData.oglClientListHead.clientList
                //    in ResetPDEV (enable.c).
                //
                globalOpenGLData.oglClientListHead.clientList = 
                    globalOpenGLData.oglClientListHead.clientList->next;
                EngFreeMem(toFree); // free memory
                OglDecClientCount(ppdev);
                }
            }
        }
    else
        {
        parNode = globalOpenGLData.oglClientListHead.clientList;
        node    = globalOpenGLData.oglClientListHead.clientList->next;
        while (node)
            {
            if (node == toFree)
                {
                parNode->next = node->next;
                EngFreeMem(toFree);
                OglDecClientCount(ppdev);
                break;
                }
            parNode = node;
            node    = node->next;
            }
        }

    return;
    }


//******************************************************************************
//
//  Function: OglFindClientInfoRmClient
//
//  Routine Description:
//
//  Arguments:
//
//        Handle to resource manager client
//
//  Return Value:
//        Pointer to client information node
//
//******************************************************************************
NV_OPENGL_CLIENT_INFO *OglFindClientInfoRmClient(
PDEV *ppdev, 
ULONG rmClient)
    {
    NV_OPENGL_CLIENT_INFO_LIST *node       = NULL;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;

    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
        {
        if (node->clientInfo.hClient == rmClient)
            {
            clientInfo = &node->clientInfo;
            break;
            }
        node = node->next;
        }

    return(clientInfo);
    }


//******************************************************************************
//
//  Function: OglFindDrawableInClientInfoList
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//        Pointer to client information node
//
//******************************************************************************
NV_OPENGL_CLIENT_INFO *OglFindDrawableInClientInfoList(
PDEV *ppdev, 
NV_OPENGL_DRAWABLE_INFO *inDrawablePtr)
    {
    NV_OPENGL_CLIENT_INFO_LIST *node       = NULL;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;

    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
        {
        if (node->clientInfo.clientDrawableInfo == inDrawablePtr)
            {
            clientInfo = &node->clientInfo;
            break;
            }
        node = node->next;
        }

    return(clientInfo);
    }

//******************************************************************************
//
//  Function: OglFindClientInfoGlobalData
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//******************************************************************************
static NV_OPENGL_CLIENT_INFO_LIST *OglFindClientInfoGlobalData(
PDEV *ppdev, 
ULONG processHandle, 
PVOID globalData)
    {
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;

    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
        {
        if (node->clientInfo.processHandle == processHandle &&
            node->clientInfo.oglGlobalPagePtr == globalData)
            {
            break;
            }
        node = node->next;
        }

    return(node);
    }

//******************************************************************************
//
//  Function:   OglEngDeleteDriverObj
//
//  Routine Description:
//
//        This routine frees the driver object if it is not in use.
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
BOOL OglEngDeleteDriverObj(
PDEV *ppdev,
NV_OPENGL_CLIENT_INFO *clientInfo,
ULONG flags
)
    {
        if ((clientInfo->flags & flags) == 0)
        {
            if (clientInfo->hDrvObj)
            {
            DRIVEROBJ *drvObj;

            OglDebugPrint("*********************** OglEngDeleteDriverObj clientInfo = 0x%lx\n", clientInfo);

            drvObj = EngLockDriverObj(clientInfo->hDrvObj);
            if (drvObj)
                {
                OglDebugPrint("*********************** OglEngDeleteDriverObj : GOING to EngDeleteDriverObj clientInfo = 0x%lx\n", clientInfo);
                if (EngDeleteDriverObj(clientInfo->hDrvObj, FALSE, TRUE) == FALSE)
                    {
                    OglDebugPrint("*********************** OglEngDeleteDriverObj EngDeleteDriverObj FAILED! clientInfo = 0x%lx\n", clientInfo);
                    EngUnlockDriverObj(clientInfo->hDrvObj);
                    }
                OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));
                }
            else
                {
                OglDebugPrint("*********************** OglEngDeleteDriverObj EngLockDriverObj FAILED! clientInfo = 0x%lx\n", clientInfo);
                return(FALSE);
                }
            return(TRUE);
            }
        }
        return(FALSE);
    }

//******************************************************************************
//
//  Function:   OglDecClientCount
//
//  Routine Description:
//
//        This routine decrements the client count.
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
void OglDecClientCount(
PDEV *ppdev
)
{
    //
    // Decrement client count.
    //
    globalOpenGLData.oglClientCount--;

#if DBG
    // Walk the linked list of verify the count.
    OglVerifyClientList(ppdev, "OglDecClientCount");
#endif // DBG
}

//******************************************************************************
//
//  Function:   OglIncClientCount
//
//  Routine Description:
//
//        This routine increments the client count.
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
void OglIncClientCount(
PDEV *ppdev
)
{
    //
    // Increment client count.
    //
    globalOpenGLData.oglClientCount++;

#if DBG
    // Walk the linked list of verify the count.
    OglVerifyClientList(ppdev, "OglIncClientCount");
#endif // DBG
}

/* XXX put into separate file called oglGlobal.c
 */
BOOL CALLBACK ClientSharedDrvFreeObj(DRIVEROBJ *pDriverObj);
BOOL CALLBACK RmClientDrvFreeObj(DRIVEROBJ *pDriverObj);

//******************************************************************************
//
//  Function:   OglRegisterRmClient
//
//  Routine Description:
//
//        Create a node in the OpenGL list and a driver object for the user's RM client.
//
//  Arguments:
//
//        rmClient - RM client handle to track for process abnormal termination
//
//******************************************************************************
void OglRegisterRmClient(PDEV *ppdev, ULONG rmClient)
    {
    int i;
    LONG                   status;
    HDRVOBJ                hDrvObj = (HDRVOBJ)NULL;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

    DISPDBG((2, "OglRegisterRmClient - %ld", rmClient));

    //
    // Get pointer to new client.
    //
    clientList = OglAppendToClientInfoList(ppdev, 
                                           0, 
                                           0, 
                                           NULL, 
                                           NULL);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }
    else
        {
        OglDebugPrint("OglRegisterRmClient: OglAppendToClientInfoList FAILED\n");
        return;
        }

    //
    // Save client data in node of list
    //
    clientInfo->hClient = rmClient;

    //
    // Set shared info is present bit in flags.
    //
    clientInfo->flags |= NV_OGL_CLIENT_RM_CLIENT;

    //
    // Create driver object that tracks this client index with the client process
    //
    hDrvObj = EngCreateDriverObj((PVOID)clientInfo, RmClientDrvFreeObj, ppdev->hdevEng);
    if (hDrvObj == NULL)
        {
        DISPDBG((1, "OglRegisterRmClient: FAILED EngCreateDriverObj"));
        OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));
        OglDecClientCount(ppdev);
        return;
        }

    //
    // Save driver object and mode switch count.
    //
    clientInfo->hDrvObj = hDrvObj;

    OglDebugPrint("OglRegisterRmClient = 0x%p 0x%p\n", ppdev, clientInfo);
    }

//******************************************************************************
//
//  Function:   OglUnRegisterRmClient
//
//  Routine Description:
//
//        Free a previously allocated node in the client list.  The node
//        freed is associated with the rmClient.
//
//  Arguments:
//
//        rmClient - RM client handle to track for process abnormal termination
//
//******************************************************************************
void OglUnRegisterRmClient(PDEV *ppdev, ULONG rmClient)
    {
    int i;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    long status;

    OglDebugPrint("OglUnRegisterRmClient = 0x%p 0x%p\n", ppdev, rmClient);

    clientInfo = OglFindClientInfoRmClient(ppdev, rmClient);
    if (clientInfo && clientInfo->flags & NV_OGL_CLIENT_RM_CLIENT)
        {
        //
        // Free channel and such.
        //
        NvFree(ppdev->hDriver, rmClient, NV01_NULL_OBJECT, rmClient);

        clientInfo->hClient = 0;    // just for good measure...

        //
        // Mark it as freed.
        //
        clientInfo->flags &= ~NV_OGL_CLIENT_RM_CLIENT;

        if (OglEngDeleteDriverObj(ppdev, clientInfo, NV_OGL_CLIENT_RM_CLIENT) == TRUE)
            {
            OglDebugPrint("OglUnRegisterRmClient: client count = %ld\n", globalOpenGLData.oglClientCount);
            }

        OglDebugPrint("OglUnRegisterRmClient = 0x%p 0x%p\n", ppdev, clientInfo);
        }
    }

//******************************************************************************
//
//  Function:   RmClientDrvFreeObj
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
BOOL CALLBACK RmClientDrvFreeObj(
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
    OglDebugPrint("RmClientDrvFreeObj = 0x%p 0x%p\n", ppdev, clientInfo);

    if (OglFindClientListFromClientInfo(ppdev, clientInfo) == NULL)
        {
        // clientInfo was removed from clientList prior to this callback
        // just return because there is nothing to do...
        OglReleaseGlobalMutex();
        return(TRUE);
        }

    //
    // Cleanup client info.
    //
    if (clientInfo->flags & NV_OGL_CLIENT_RM_CLIENT)
        {
        try
            {
            //
            // Free channel and such.
            //
            NvFree(ppdev->hDriver, clientInfo->hClient, NV01_NULL_OBJECT, clientInfo->hClient);
            }
        except(EXCEPTION_EXECUTE_HANDLER)
            {
            status = GetExceptionCode();
            DISPDBG((1, "RmClientDrvFreeObj: exception = 0x%lx", status));
            OglReleaseGlobalMutex();
            return(TRUE);
            }
        clientInfo->flags &= ~NV_OGL_CLIENT_RM_CLIENT;
        clientInfo->hClient = 0;    // just for good measure...
        }

    clientInfo->hDrvObj = 0;
    clientInfo->flags   = 0;

    //
    // Remove from global client list.
    //
    OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));

    OglDebugPrint("RmClientDrvFreeObj CALLBACK: client count = %ld\n", globalOpenGLData.oglClientCount);

    OglReleaseGlobalMutex();

    return(TRUE);
}

//******************************************************************************
//
//  Function:   CreateSharedClientInfo
//
//  Routine Description:
//
//        Maps the global page (GDI mutex and mode switch counter) into the
//        client address space.  It adds a node to the linked list and register
//        the node with the GDI subsystem.
//
//        Also maps the fifo registers, and the pci config space registers
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
void CreateSharedClientInfo(
PDEV *ppdev, 
ULONG hClient,
ULONG processHandle, 
PVOID *globalData, 
PVOID *pfifoAddress,
PVOID *pbusAddress)
    {
    int i;
    LONG                   status;
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryOut;
    HDRVOBJ                hDrvObj = (HDRVOBJ)NULL;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

    DISPDBG((2, "CreateSharedClientInfo - %ld", processHandle));

    *globalData = *pfifoAddress = NULL;

    if (OglMapGlobalPageAddress(ppdev, globalData) == FALSE)
        {
        OglDebugPrint("CreateSharedClientInfo: OglMapGlobalPageAddress FAILED\n");
        return;
        }

    if (OglMapPFifoAddress(ppdev, pfifoAddress) == FALSE)
        {
        OglUnmapGlobalPageAddress(ppdev, *globalData);
        OglDebugPrint("CreateSharedClientInfo: OglMapPFifoAddress FAILED\n");
        return;
        }

    if (OglMapPBusAddress(ppdev, pbusAddress) == FALSE)
        {
        OglUnmapPFifoAddress(ppdev, *pfifoAddress);
        OglUnmapGlobalPageAddress(ppdev, *globalData);
        OglDebugPrint("CreateSharedClientInfo: OglMapPBusAddress FAILED\n");
        return;
        }

    //
    // Get pointer to new client.
    //
    clientList = OglAppendToClientInfoList(ppdev, 
                                           0, 
                                           0, 
                                           NULL, 
                                           NULL);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }
    else
        {
        OglUnmapPBusAddress(ppdev, *pbusAddress);
        OglUnmapPFifoAddress(ppdev, *pfifoAddress);
        OglUnmapGlobalPageAddress(ppdev, *globalData);
        OglDebugPrint("CreateSharedClientInfo: OglAppendToClientInfoList FAILED\n");
        return;
        }

    //
    // Save client data in node of list
    //
    clientInfo->hClient          = hClient;       // Per-device client handle
    clientInfo->processHandle    = processHandle; // Process handle from client associated with globalData
    clientInfo->oglGlobalPagePtr = *globalData;   // Pointer to shared memory
    clientInfo->pfifoAddress     = *pfifoAddress; // Address of FIFO in client address space
    clientInfo->pbusAddress      = *pbusAddress;  // Address of BUS registers in client addr space

    //
    // Set shared info is present bit in flags.
    //
    clientInfo->flags |= NV_OGL_CLIENT_SHARED_INFO;

    OglDebugPrint("****************** CreateShared: client count = %ld\n", globalOpenGLData.oglClientCount);

    //
    // Create driver object that tracks this client index with the client process
    //
    hDrvObj = EngCreateDriverObj((PVOID)clientInfo, ClientSharedDrvFreeObj, ppdev->hdevEng);
    if (hDrvObj == NULL)
        {
        DISPDBG((1, "CreateSharedClientInfo: FAILED EngCreateDriverObj"));
        OglDebugPrint("****************** FAILURE CreateShared: client count = %ld\n", globalOpenGLData.oglClientCount);
        OglUnmapGlobalPageAddress(ppdev, clientInfo->oglGlobalPagePtr);
        OglUnmapPFifoAddress(ppdev, clientInfo->pfifoAddress);
        OglUnmapPBusAddress(ppdev, clientInfo->pbusAddress);
        OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));
        return;
        }

    //
    // Save driver object and mode switch count.
    //
    clientInfo->hDrvObj         = hDrvObj;
    clientInfo->modeSwitchCount = ppdev->dwGlobalModeSwitchCount;

    OglDebugPrint("CreateShared = 0x%p 0x%p\n", ppdev, clientInfo);
    }

//******************************************************************************
//
//  Function:   DestroySharedClientInfo
//
//  Routine Description:
//
//        This routine is used to unmap the memory and destroy the client node.
//        The node is removed from the list and the GDI tracking of the object
//        is deleted.
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
void DestroySharedClientInfo(
PDEV *ppdev, 
ULONG hClient, 
ULONG processHandle, 
PVOID globalData, 
ULONG deleteFlag)
    {
    int i;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    long status;

    OglDebugPrint("DestroySharedClientInfo = 0x%p %ld 0x%p\n", ppdev, processHandle, globalData);

    clientList = OglFindClientInfoGlobalData(ppdev, processHandle, globalData);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }

    if (clientInfo && clientInfo->flags & NV_OGL_CLIENT_SHARED_INFO)
        {
        //
        // Free channel and such.
        //
        NvFree(ppdev->hDriver, hClient, NV01_NULL_OBJECT, hClient);

        //
        // Free page mappings from display driver to client.
        //
        OglUnmapGlobalPageAddress(ppdev, clientInfo->oglGlobalPagePtr);
        OglUnmapPFifoAddress(ppdev, clientInfo->pfifoAddress);
        OglUnmapPBusAddress(ppdev, clientInfo->pbusAddress);

        clientInfo->processHandle    = 0;    // just for good measure...
        clientInfo->oglGlobalPagePtr = NULL; // just for good measure...
        clientInfo->pfifoAddress     = NULL; // just for good measure...
        clientInfo->pbusAddress      = NULL; // just for good measure...

        //
        // Mark it as freed.
        //
        clientInfo->flags &= ~NV_OGL_CLIENT_SHARED_INFO;

        if (deleteFlag)
            {
            //
            // Delete driver object if possible
            //
            if (OglEngDeleteDriverObj(ppdev, clientInfo, NV_OGL_CLIENT_SHARED_INFO) == TRUE)
                {
                OglDebugPrint("****************** DestroyShared: client count = %ld\n", globalOpenGLData.oglClientCount);
                }
            }

        OglDebugPrint("DestroyShared = 0x%p 0x%p\n", ppdev, clientInfo);
        }
    }

//******************************************************************************
//
//  Function:   ClientSharedDrvFreeObj
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
BOOL CALLBACK ClientSharedDrvFreeObj(
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
    OglDebugPrint("SHARED ***************** SHARED DrvFreeObj = 0x%p 0x%p\n", ppdev, clientInfo);

    if (OglFindClientListFromClientInfo(ppdev, clientInfo) == NULL)
        {
        // clientInfo was removed from clientList prior to this callback
        // just return because there is nothing to do...
        OglReleaseGlobalMutex();
        return(TRUE);
        }

    //
    // Cleanup client info.
    //
    if (clientInfo->flags & NV_OGL_CLIENT_SHARED_INFO)
        {
        try
            {
            DestroySharedClientInfo(ppdev, clientInfo->hClient, clientInfo->processHandle, clientInfo->oglGlobalPagePtr, FALSE);
            }
        except(EXCEPTION_EXECUTE_HANDLER)
            {
            status = GetExceptionCode();
            DISPDBG((1, "DestroySharedClientInfo: exception = 0x%lx", status));
            OglReleaseGlobalMutex();
            return(TRUE);
            }
        clientInfo->flags &= ~NV_OGL_CLIENT_SHARED_INFO;
        clientInfo->processHandle    = 0;    // just for good measure...
        clientInfo->hClient          = 0;    // just for good measure...
        clientInfo->oglGlobalPagePtr = NULL; // just for good measure...
        clientInfo->pfifoAddress     = NULL; // just for good measure...
        }

    clientInfo->hDrvObj = 0;
    clientInfo->flags   = 0;

    //
    // Remove from global client list.
    //
    OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));

    //
    // Decrement client count.
    //
    OglDebugPrint("****************** Shared CALLBACK: client count = %ld\n", globalOpenGLData.oglClientCount);

    OglReleaseGlobalMutex();

    return(TRUE);
}

//******************************************************************************
//
//  Function:   OglMapGlogalPageAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to map the kernel, system
//      address for the shared information into the caller's address space.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the mapping was a success.
//
//******************************************************************************
BOOL OglMapGlobalPageAddress(
PDEV *ppdev, 
PVOID *globalData
)
    {
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn, sysmemShareMemoryOut;
    ULONG dwReturnedDataLength;

    DISPDBG((2, "OglMapGlobalPageAddress - Entry"));

    *globalData = NULL;

    sysmemShareMemoryIn.pSystemMdl         = ppdev->oglGlobalPageMdl; // miniport will NOT allocate MDL
    sysmemShareMemoryIn.bDontModifyMDL     = TRUE;
    sysmemShareMemoryIn.physicalAddress    = 0;
    sysmemShareMemoryIn.userVirtualAddress = (PVOID)NULL;

    sysmemShareMemoryIn.ddVirtualAddress   = (PVOID)ppdev->oglGlobalPagePtr;
    sysmemShareMemoryIn.byteLength         = ppdev->oglGlobalPageSize;

    if (EngDeviceIoControl(ppdev->hDriver,
        IOCTL_VIDEO_MAP_TO_USER,
        &sysmemShareMemoryIn,
        sizeof(NV_SYSMEM_SHARE_MEMORY),
        &sysmemShareMemoryOut,
        sizeof(NV_SYSMEM_SHARE_MEMORY),
        &dwReturnedDataLength))
    {
        *globalData = (PVOID)NULL;
        DISPDBG((2, "OglMapGlobalPageAddress - IOCTL_VIDEO_MAP_TO_USER failed"));
        return(FALSE);
    }

    // save user address in shared info structure
    *globalData = (PVOID)sysmemShareMemoryOut.userVirtualAddress;

    DISPDBG((2, "OglMapGlobalPageAddress - Exit"));

    return(TRUE);
    }

//******************************************************************************
//
//  Function:   OglUnmapGlobalPageAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to map the user virtual address
//      into system address space where the display driver can get to it.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the mapping was a success.
//
//******************************************************************************
BOOL OglUnmapGlobalPageAddress(
PDEV *ppdev, 
PVOID globalData
)
    {
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn, sysmemShareMemoryOut;
    ULONG dwReturnedDataLength;
    LONG status;

    DISPDBG((2, "OglUnmapGlobalPageAddress - Entry"));

    if (globalData)
        {
        sysmemShareMemoryIn.userVirtualAddress = (PVOID)globalData;
        sysmemShareMemoryIn.pSystemMdl         = ppdev->oglGlobalPageMdl;
        sysmemShareMemoryIn.bDontModifyMDL     = TRUE;  // miniport will NOT free MDL

        if (EngDeviceIoControl(ppdev->hDriver,
            IOCTL_VIDEO_UNMAP_FROM_USER,
            &sysmemShareMemoryIn,
            sizeof(NV_SYSMEM_SHARE_MEMORY),
            &sysmemShareMemoryOut,
            sizeof(NV_SYSMEM_SHARE_MEMORY),
            &dwReturnedDataLength))
            {
            DISPDBG((2, "OglUnmapGlobalPageAddress - IOCTL_VIDEO_UNMAP_FROM_USER failed"));
            }
        }
    else
        {
        DISPDBG((2, "OglUnmapGlobalPageAddress - globalData NULL"));
        }

    DISPDBG((2, "OglUnmapGlobalPageAddress - Exit"));
    return(TRUE);
    }

//******************************************************************************
//
//  Function:   OglMapPFifoAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to map the NV FIFO into the client.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the mapping was a success.
//
//******************************************************************************
BOOL OglMapPFifoAddress(
PDEV *ppdev, 
PVOID *pfifoAddress
)
    {
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn;
    ULONG  returnedDataLength;

    *pfifoAddress = NULL;

    memset(&sysmemShareMemoryIn, 0, sizeof(NV_SYSMEM_SHARE_MEMORY));
    sysmemShareMemoryIn.pSystemMdl     = ppdev->oglGlobalPageMdl; // miniport will NOT allocate MDL
    sysmemShareMemoryIn.bDontModifyMDL = TRUE;

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_MAP_NV_PFIFO_USER,
                           &sysmemShareMemoryIn, // not really used in mapping...
                           sizeof(HANDLE),
                           (PULONG)pfifoAddress,
                           sizeof(PULONG),
                           &returnedDataLength))
        {
        DISPDBG((2, "MapPFifoAddress - can't map NV PFIFO"));
        return(FALSE);
        }

    return(TRUE);
    }

//******************************************************************************
//
//  Function:   OglUnmapPFifoAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to unmap the NV FIFO from the client.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the unmapping was a success.
//
//******************************************************************************
BOOL OglUnmapPFifoAddress(
PDEV *ppdev, 
PVOID pfifoAddress
)
    {
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn, sysmemShareMemoryOut;
    ULONG                  returnedDataLength;

    if (pfifoAddress)
        {
        sysmemShareMemoryIn.userVirtualAddress = ((ULONG *)pfifoAddress);
        sysmemShareMemoryIn.pSystemMdl         = ppdev->oglGlobalPageMdl;
        sysmemShareMemoryIn.bDontModifyMDL     = TRUE;  // miniport will NOT free MDL
        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_UNMAP_NV_PFIFO_USER,
                               &sysmemShareMemoryIn,
                               sizeof(NV_SYSMEM_SHARE_MEMORY),
                               (PULONG) &sysmemShareMemoryOut,
                               sizeof(NV_SYSMEM_SHARE_MEMORY),
                               &returnedDataLength))
            {
            DISPDBG((2, "UnmapPFifoAddress - can't unmap NV PFIFO"));
            return(FALSE);
            }
        }
    else
        {
        DISPDBG((2, "UnmapPFifoAddress - pfifoAddress NULL"));
        }

    return(TRUE);
    }

//******************************************************************************
//
//  Function:   OglMapPBusAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to map the NV PCI bus registers
//      into the client.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the mapping was a success.
//
//******************************************************************************
BOOL OglMapPBusAddress(
PDEV *ppdev, 
PVOID *pbusAddress
)
    {
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn;
    ULONG  returnedDataLength;

    *pbusAddress = NULL;

    memset(&sysmemShareMemoryIn, 0, sizeof(NV_SYSMEM_SHARE_MEMORY));
    sysmemShareMemoryIn.pSystemMdl     = ppdev->oglGlobalPageMdl; // miniport will NOT allocate MDL
    sysmemShareMemoryIn.bDontModifyMDL = TRUE;

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_MAP_NV_PBUS_USER,
                           &sysmemShareMemoryIn, // not really used in mapping...
                           sizeof(HANDLE),
                           (PULONG)pbusAddress,
                           sizeof(PULONG),
                           &returnedDataLength))
        {
        DISPDBG((2, "MapPBusAddress - can't map NV PBUS"));
        return(FALSE);
        }

    return(TRUE);
    }

//******************************************************************************
//
//  Function:   OglUnmapPBusAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to unmap the NV PCI bus registers
//      from the client.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the unmapping was a success.
//
//******************************************************************************
BOOL OglUnmapPBusAddress(
PDEV *ppdev, 
PVOID pbusAddress
)
    {
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn, sysmemShareMemoryOut;
    ULONG                  returnedDataLength;

    if (pbusAddress)
        {
        sysmemShareMemoryIn.userVirtualAddress = ((ULONG *)pbusAddress);
        sysmemShareMemoryIn.pSystemMdl         = ppdev->oglGlobalPageMdl;
        sysmemShareMemoryIn.bDontModifyMDL     = TRUE;  // miniport will NOT free MDL
        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_UNMAP_NV_PBUS_USER,
                               &sysmemShareMemoryIn,
                               sizeof(NV_SYSMEM_SHARE_MEMORY),
                               (PULONG) &sysmemShareMemoryOut,
                               sizeof(NV_SYSMEM_SHARE_MEMORY),
                               &returnedDataLength))
            {
            DISPDBG((2, "UnmapPBusAddress - can't unmap NV PBUS"));
            return(FALSE);
            }
        }
    else
        {
        DISPDBG((2, "UnmapPBusAddress - pbusAddress NULL"));
        }

    return(TRUE);
    }

#if DBG
//******************************************************************************
//
//  Function:   OglVerifyClientList
//
//  Routine Description:
//
//      This is a debug routine that walks the client linked list and verifies
//      that the count equals the client count.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the count are equal.
//
//******************************************************************************
BOOL OglVerifyClientList(
PDEV *ppdev,
char *s
)
    {
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;
    ULONG i;

    i = 0;
    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
        {
        i++;
        node = node->next;
        }
    OglDebugPrint(">>>>>>>>>> %s clientCount = %ld ? %ld\n", s, globalOpenGLData.oglClientCount, i);
    return((BOOL)(i == globalOpenGLData.oglClientCount));
    }
#endif // DBG

