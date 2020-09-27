/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvObjectLists.cpp                                                 *
*   Routines for management of NvObject lists added in DX7                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe (paulvdk)   3 Mar 2001 created                   *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//--------------------------------------------------------------------------

// Creates an empty list of NvObjects to be associated with the
// given DIRECTDRAW_LCL

PNV_OBJECT_LIST nvCreateObjectList
(
    DWORD dwDDLclID,
    NV_OBJECT_LIST **ppList
)
{
    PNV_OBJECT_LIST pNvObjectList;
    DWORD           dwSize;

    dbgTracePush ("nvCreateObjectList");

    // make sure we have a DD_LCL
    nvAssert (dwDDLclID);

    // allocate space for the new list and initialize its contents
    dwSize = sizeof(NV_OBJECT_LIST);
    pNvObjectList = (PNV_OBJECT_LIST) AllocIPM (dwSize);
    if (!pNvObjectList) {
        DPF("Out of memory creating object list");
        dbgTracePop();
        return NULL;
    }
    memset (pNvObjectList, 0, dwSize);

    dwSize = NV_OBJECT_LIST_SIZE_DEFAULT * sizeof(CNvObject *);
    pNvObjectList->ppObjectList = (CNvObject **) AllocIPM (dwSize);
    if (!pNvObjectList->ppObjectList) {
        DPF("Out of memory creating object list");
        FreeIPM(pNvObjectList);
        dbgTracePop();
        return NULL;
    }
    memset (pNvObjectList->ppObjectList, 0, dwSize);
    pNvObjectList->dwListSize = NV_OBJECT_LIST_SIZE_DEFAULT;
    pNvObjectList->dwDDLclID = dwDDLclID;

    // insert the new list at the beginning of the list of lists
    pNvObjectList->pNext = *ppList;
    *ppList = pNvObjectList;

    dbgTracePop();
    return pNvObjectList;
}

//--------------------------------------------------------------------------

// Deletes the list of NvObjects being maintained for the given DD_LCL

BOOL nvDeleteObjectList
(
    DWORD dwDDLclID,
    NV_OBJECT_LIST **ppList
)
{
    PNV_OBJECT_LIST pNvObjectList;
    PNV_OBJECT_LIST pNvObjectListNext;

    dbgTracePush ("nvDeleteObjectList");

    // make sure we have a DD_LCL
    nvAssert (dwDDLclID);

    // get the head of the list of lists
    pNvObjectList = *ppList;

    if (!pNvObjectList) {
        // nothing to delete. no list was ever created in the first place
        dbgTracePop();
        return (FALSE);
    }

    // check for the case in which the list to be deleted is at the head
    if (pNvObjectList->dwDDLclID == dwDDLclID) {
        if (pNvObjectList->dwListSize) {
            FreeIPM (pNvObjectList->ppObjectList);
        }
        *ppList = pNvObjectList->pNext;
        FreeIPM (pNvObjectList);

        dbgTracePop();
        return (TRUE);
    }

    // if not at the head, then search down the list
    while (pNvObjectList->pNext != NULL) {
        if (pNvObjectList->pNext->dwDDLclID == dwDDLclID) break;
        pNvObjectList = pNvObjectList->pNext;
    }

    // now pNvObjectList->pNext is the one we actually want to delete
    pNvObjectListNext = pNvObjectList->pNext;
    if (pNvObjectListNext == NULL) {
        // no list for this DD_LCL was found.
        dbgTracePop();
        return (FALSE);
    }

    if (pNvObjectListNext->dwListSize) {
        FreeIPM (pNvObjectListNext->ppObjectList);
    }
    pNvObjectList->pNext = pNvObjectListNext->pNext;
    FreeIPM (pNvObjectListNext);

    dbgTracePop();
    return (TRUE);
}

//--------------------------------------------------------------------------

// Allocates more memory and enlarges a list of NvObjects (private)

BOOL nvExpandObjectList
(
    PNV_OBJECT_LIST pNvObjectList,
    DWORD           dwHandle
)
{
    DWORD       dwNewListSize;
    CNvObject **ppNewList;

    dbgTracePush ("nvExpandObjectList");

    // allocate a bigger list
    dwNewListSize = (dwHandle + NV_OBJECT_LIST_SIZE_INCREMENTAL) & ~(NV_OBJECT_LIST_SIZE_INCREMENTAL - 1);

    DWORD dwSize = dwNewListSize * sizeof(CNvObject *);
    ReallocIPM(pNvObjectList->ppObjectList, dwSize, (void**)&ppNewList);

    if (!ppNewList) {
        DPF ("memory allocation failed. could not expand object list");
        dbgD3DError();
        dbgTracePop();
        return FALSE;
    }

    memset (&ppNewList[pNvObjectList->dwListSize], 0, (dwNewListSize - pNvObjectList->dwListSize) * sizeof(CNvObject*));
    pNvObjectList->dwListSize = dwNewListSize;
    pNvObjectList->ppObjectList = ppNewList;

    dbgTracePop();
    return TRUE;
}

//--------------------------------------------------------------------------

// Returns the list of NvObjects associated with a particular DIRECTDRAW_LCL

PNV_OBJECT_LIST nvFindObjectList
(
    DWORD dwDDLclID,
    NV_OBJECT_LIST **ppList
)
{
    PNV_OBJECT_LIST pNvObjectList;

    dbgTracePush ("nvFindObjectList");

    // make sure we have a DD_LCL
    nvAssert (dwDDLclID);
    
    // get the head of the list of lists
    pNvObjectList = *ppList;

    // find the list for this DD_LCL
    while (pNvObjectList != NULL) {
        if (pNvObjectList->dwDDLclID == dwDDLclID) break;
        pNvObjectList = pNvObjectList->pNext;
    }

    // if no object list was found, create one
    if (pNvObjectList == NULL) {
        dbgTracePop();
        return nvCreateObjectList(dwDDLclID, ppList);
    }
    
    dbgTracePop();
    return (pNvObjectList);
}

//--------------------------------------------------------------------------

// Searches the list of NvObject pointers associated with 'lpDDLcl'
// and returns the pointer to the object indexed by 'handle'

CNvObject *nvGetObjectFromHandle
(
    DWORD dwDDLclID,
    DWORD dwHandle,
    NV_OBJECT_LIST **ppList
)
{
    PNV_OBJECT_LIST pNvObjectList;

    dbgTracePush ("nvGetObjectFromHandle");

    if (global.dwDXRuntimeVersion < 0x0700 && (*ppList == global.pNvSurfaceLists))
    {
        // For DX6 the handle is a pointer to the surface local structure.
        // We just get the object from there.
        if (!dwHandle) {
            dbgTracePop();
            return NULL;
        }
        return (GET_PNVOBJ ((LPDDRAWI_DDRAWSURFACE_LCL)dwHandle));
    }

    pNvObjectList = nvFindObjectList (dwDDLclID, ppList);
    if (!pNvObjectList) {
        dbgTracePop();
        return NULL;
    }

    // the handle should be less than the current list size.
    if (dwHandle >= pNvObjectList->dwListSize) {
        DPF ("handle exceeded list size in nvGetObjectFromHandle");
        dbgTracePop();
        return NULL;
    }

    dbgTracePop();
    return pNvObjectList->ppObjectList[dwHandle];
}

//--------------------------------------------------------------------------

BOOL nvClearObjectListEntry
(
    CNvObject *pObj,
    NV_OBJECT_LIST **ppList
)
{
    dbgTracePush ("nvClearObjectListEntry");

    // make sure we're dealing with DX7 or higher
    if (global.dwDXRuntimeVersion < 0x0700 && (*ppList == global.pNvSurfaceLists)) {
        dbgTracePop();
        return FALSE;
    }

    nvAssert (pObj);
    DWORD dwDDLclID = pObj->getDDLclID();

    if (dwDDLclID) {

        PNV_OBJECT_LIST pNvObjectList = nvFindObjectList (dwDDLclID, ppList);
        if (!pNvObjectList) {
            dbgTracePop();
            return FALSE;
        }

        // we should never get a handle less than 1 (this is because
        // internally, MS uses list[0] to store the list size)
        DWORD dwHandle = pObj->getHandle();

#ifdef WINNT
        // under WIN NT we often get a DestroyDriver call before all the surfaces
        // are destroyed. in this case the handle value may exceed the list size.
        // ignore the request in this case
        if (dwHandle >= pNvObjectList->dwListSize) {
            dbgTracePop();
            return TRUE;
        }
#else
        // the handle should be less than the current list size.
        nvAssert (dwHandle < pNvObjectList->dwListSize);
#endif

        // if this object had a handle associated with it and the handle
        // is still being used for it, clear the entry
        if ((dwHandle) && (pNvObjectList->ppObjectList[dwHandle] == pObj)) {
            pNvObjectList->ppObjectList[dwHandle] = NULL;
        }

    }

    dbgTracePop();
    return TRUE;
}

//--------------------------------------------------------------------------

DWORD nvAddObjectToList
(
    PNV_OBJECT_LIST pNvObjectList,
    CNvObject       *pObj,
    DWORD           dwHandle
)
{
    dbgTracePush ("nvAddObjectToList");
    
    // make sure we get a valid object and handle
    nvAssert (pObj);
    //nvAssert (dwHandle);      //we now have defaults objects of handle 0 when all else fails on vertexshader
    
    // grow the list if necessary
    if (dwHandle >= pNvObjectList->dwListSize) {
        BOOL bRes = nvExpandObjectList (pNvObjectList, dwHandle);
        if (!bRes) {
            dbgTracePop();
            return DDERR_OUTOFMEMORY;
        }
    }
    
    // insert the obbject in the list
    pNvObjectList->ppObjectList[dwHandle] = pObj;
    
    // the object has to know what list it's part of
    pObj->setDDLclID (pNvObjectList->dwDDLclID);
    pObj->setHandle (dwHandle);
    
    // mark the object as dirty, this is used to ensure that rendertargets are up-to-date after mode switches
    pObj->setDirty();
    
    dbgTracePop();
    return DD_OK;
}

#endif  // NVARCH >= 0x04
