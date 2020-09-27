#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H
//******************************Module*Header***********************************
// Module Name: linkedlist.h
//
// Manage a linked list containing any data
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

typedef enum _LL_CLASS_ID
{
    LL_CLASS_INVALID = 0,   // invalid type, do not use!
    LL_CLASS_ANY_CLASS,     // only allowed in call to bLinkedListCreate
    LL_CLASS_SAVESCREENBITS,
#ifdef NV_MAIN_MULTIMON
    LL_CLASS_OGLPDEV,
#endif
    // add more classes here
} LL_CLASS_ID;

BOOL bLinkedListCreate          (IN OUT struct _LINKEDLISTHEAD **ppAnchor, IN LL_CLASS_ID ClassID);
BOOL bLinkedListDestroy         (IN OUT struct _LINKEDLISTHEAD **ppAnchor);
BOOL bLinkedListAddElement      (IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, IN  ULONG_PTR   ulIdent, PVOID pData, ULONG ulAllocSize);
BOOL bLinkedListRemoveElement   (IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, IN  ULONG_PTR   ulIdent);
BOOL bLinkedListGetElement      (IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, IN  ULONG_PTR   ulIdent, OUT PVOID *ppData);
BOOL bLinkedListGetFirstElement (IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, OUT ULONG_PTR *pulIdent, OUT PVOID *ppData);
BOOL bLinkedListGetNextElement  (IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, OUT ULONG_PTR *pulIdent, OUT PVOID *ppData);
BOOL bLinkedListGetLastElement  (IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, OUT ULONG_PTR *pulIdent, OUT PVOID *ppData);
BOOL bLinkedListGetPrevElement  (IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, OUT ULONG_PTR *pulIdent, OUT PVOID *ppData);
BOOL bLinkedListGetNewestElement(IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, OUT ULONG_PTR *pulIdent, OUT PVOID *ppData);
BOOL bLinkedListGetOldestElement(IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID, OUT ULONG_PTR *pulIdent, OUT PVOID *ppData);
ULONG ulLinkedListGetCountOfElements(IN struct _LINKEDLISTHEAD *pAnchor, IN LL_CLASS_ID ClassID);

#endif // _LINKEDLIST_H
