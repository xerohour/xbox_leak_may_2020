/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvObjectLists.h                                                   *
*   Routines for management of NvObject lists added in DX7                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe (paulvdk)   3 Mar 2001 created                   *
*                                                                           *
\***************************************************************************/

#ifndef _NVLIST_H_
#define _NVLIST_H_

#ifdef __cplusplus
extern "C" {
#endif
    
PNV_OBJECT_LIST  nvCreateObjectList       (DWORD dwDDLclID, NV_OBJECT_LIST **ppList);
BOOL             nvDeleteObjectList       (DWORD dwDDLclID, NV_OBJECT_LIST **ppList);
PNV_OBJECT_LIST  nvFindObjectList         (DWORD dwDDLclID, NV_OBJECT_LIST **ppList);
CNvObject       *nvGetObjectFromHandle    (DWORD dwDDLclID, DWORD dwHandle, NV_OBJECT_LIST **ppList);
BOOL             nvClearObjectListEntry   (CNvObject *pObj, NV_OBJECT_LIST **ppList);
DWORD            nvAddObjectToList        (PNV_OBJECT_LIST pNvGenObjList, CNvObject *pGenObj, DWORD dwHandle);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NVLIST_H_
