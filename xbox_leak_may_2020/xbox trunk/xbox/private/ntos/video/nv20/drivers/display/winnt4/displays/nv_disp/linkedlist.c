//******************************Module*Header***********************************
// Module Name: linkedlist.c
//
// Manage a linked list containing any data
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
// Usage:
// 
// 1. Define new class type in LL_CLASS_ID enum (or use LL_CLASS_ANY_CLASS).
// 2. Call bLinkedListCreate to initialize the linked list
// 3. Add items to linked list using bLinkedListAddElement
// 4. Work with the items
// 5. Remove items from linked list using bLinkedListRemoveElement
// 6. Destroy linked list with a bLinkedListDestroy call
//
// You need unique identifier (which of course can be a pointer) and optional
// data you want to store. The identifier is used to reference the data.
//
// Example:
//
// VOID vLinkedListTest(VOID)
// {
//     // NOTE: we don't need the definition of the structure for a pointer!
//     struct _LINKEDLISTHEAD *pHead;
// 
//     // create linked list head
//     pHead = NULL;
//     if (bLinkedListCreate(&pHead, LL_CLASS_MYCLASS))
//     {
//         ULONG ul;
//         PVOID pData=&ul;
// 
//         // Add elements to linked list
//         for (ul=0; ul<10; ul++)
//         {
//             if (!bLinkedListAddElement(pHead, LL_CLASS_MYCLASS, ul, pData, 0))
//                 errorhandling();
//         }
// 
//         // Work with data (e.g. get element with id 5)
//         if (bLinkedListGetElement(pHead,LL_CLASS_MYCLASS, 5, &pData))
//         {
//             ASSERT(NULL==pHead);
//             //work with pData
//         }
//         else 
//             errorhandling();
// 
//         // Remove elements from linked list
//         for (ul=0; ul<10; ul++)
//         {
//             if (!bLinkedListRemoveElement(pHead,LL_CLASS_MYCLASS,ul))
//                 errorhandling();
//         }
// 
//         // destroy linked list head linked list should be empty!
//         bLinkedListDestroy(&pHead);
//         ASSERT(NULL==pHead);
//     }
// }
//
//
//
//******************************************************************************

// import
#include "precomp.h"
#include "driver.h"

// export
#include "linkedlist.h"

//*************************************************************************
// LINKEDLIST
// 
// structure defines one double linked node containing data of type ClassID
// with unique identifier ulIdent.
//*************************************************************************
typedef struct _LINKEDLIST
{
    LL_CLASS_ID         ClassID;            // Type of data 
    ULONG_PTR           ulIdent;            // unique identifier for data (can be pointer)
    PVOID               pData;              // poiter to any kind of data
    struct _LINKEDLIST *pPrev;              // previous element in linked list or NULL if there is none
    struct _LINKEDLIST *pNext;              // next element in linked list or NULL if there is none
    ULONG               ulAllocatedSize;    // 0 if pData is allocated outside, != 0 if allocated in this module
    ULONG               ulReferenceCount;   // number of references to this node
} LINKEDLIST;


//*************************************************************************
// LINKEDLISTHEAD
// 
// head to double linked list of any data. If ClassID is set to 
// LL_CLASS_ANY_CLASS the linked list can store any class. If it is set to
// something else out of the enum LL_CLASS_ID it can only store this type.
//*************************************************************************
typedef struct _LINKEDLISTHEAD
{
    ULONG        ulMagic;         // must be set to LL_MAGIC to identify a valid head
    LL_CLASS_ID  ClassID;         // specifies type of data
    LINKEDLIST  *pFirst;          // points to first element in list
    LINKEDLIST  *pLast;           // points to last element in list
    LINKEDLIST  *pCurrent;        // used internaly to handle first/get
    ULONG        ulNumberOfNodes; // total number of nodes in linked list
} LINKEDLISTHEAD;

#define LL_MAGIC    'derf'        // magic value to check validity of linked list header


//******************************************************************************
//
// Internal functions
//

//*************************************************************************
// bLLValidList
// 
// returns TRUE if the linked list to which pAnchor is pointing is a valid 
// linked list. Returns FALSE if it is invalid or corrupted.
//
// NOTE: As we access the pointers, we will crash if they are invalid!
//*************************************************************************
BOOL bLLValidList(
    IN LINKEDLISTHEAD *pAnchor)
{
    LINKEDLIST *pNode         = NULL;
    LINKEDLIST *pNodeLast     = NULL;
    ULONG       ulCount       = 0;
    BOOL        bFoundCurrent = FALSE;
    BOOL        bRet          = FALSE;

    //
    // Check pAnchor and base members
    //

    if (NULL==pAnchor)
    {
        DISPDBG((0, "ERROR in linked list: pAnchor==NULL"));
        goto Exit;
    }

    if (LL_MAGIC != pAnchor->ulMagic)
    {
        DISPDBG((0, "ERROR in linked list: pAnchor invalid ulMagic"));
        goto Exit;
    }

    if (LL_CLASS_INVALID == pAnchor->ClassID)
    {
        DISPDBG((0, "ERROR in linked list: pAnchor invalid ClassID"));
        goto Exit;
    }


    //
    // Test simple conditions with pFirst and pLast depending on number of stored nodes
    //

    if (   (0 == pAnchor->ulNumberOfNodes)
        && ( (NULL != pAnchor->pFirst) || (NULL != pAnchor->pLast) || (NULL != pAnchor->pCurrent)  ) )
    {
        DISPDBG((0, "ERROR in linked list: 0 nodes but pointers!= NULL (pLast:0x%p, pFirst:0x%p, pCurrent:0x%p)", pAnchor->pFirst, pAnchor->pLast, pAnchor->pCurrent));
        goto Exit;
    }

    if (   (1 == pAnchor->ulNumberOfNodes)
        && (pAnchor->pFirst != pAnchor->pLast) )
    {
        DISPDBG((0, "ERROR in linked list: 1 node but first!=last (pLast:0x%p, pFirst:0x%p)", pAnchor->pFirst, pAnchor->pLast));
        goto Exit;
    }

    if (   (0 != pAnchor->ulNumberOfNodes)
        && (   (pAnchor->pFirst == NULL) 
            || (pAnchor->pLast  == NULL) 
           ) )
    {
        DISPDBG((0, "ERROR in linked list: %d nodes and first and last invalid (pLast:0x%p, pFirst:0x%p)", pAnchor->ulNumberOfNodes, pAnchor->pFirst, pAnchor->pLast));
        goto Exit;
    }

    if (   (pAnchor->ulNumberOfNodes > 1)
        && (pAnchor->pFirst == pAnchor->pLast) )
    {
        DISPDBG((0, "ERROR in linked list: %d nodes but first and last invalid (pLast:0x%p, pFirst:0x%p)", pAnchor->ulNumberOfNodes, pAnchor->pFirst, pAnchor->pLast));
        goto Exit;
    }

    if (   (NULL != pAnchor->pFirst)
        && (NULL != pAnchor->pFirst->pPrev) )
    {
        DISPDBG((0, "ERROR in linked list: first element not pointing to NULL"));
        goto Exit;
    }

    if (   (NULL != pAnchor->pLast)
        && (NULL != pAnchor->pLast->pNext) )
    {
        DISPDBG((0, "ERROR in linked list: last element not pointing to NULL"));
        goto Exit;
    }

    //
    // Walk through list and check linkage
    //

    if (NULL==pAnchor->pCurrent)
        bFoundCurrent = TRUE;
    else
        bFoundCurrent = FALSE;

    for (pNode = pAnchor->pFirst, ulCount = 0, pNodeLast = NULL; 
         (NULL != pNode) && (ulCount < pAnchor->ulNumberOfNodes+5); 
         pNodeLast = pNode, pNode = pNode->pNext)
    {
        if (pNode->pPrev != pNodeLast)
        {
            DISPDBG((0, "ERROR in linked list: wrong prev linkage node %d: 0x%p==pNode->pPrev != pNodeLast==0x%p", ulCount,pNode->pPrev,pNodeLast));
            goto Exit;
        }

        if (   (pAnchor->ClassID != LL_CLASS_ANY_CLASS)
            && (pAnchor->ClassID != pNode->ClassID) )
        {
            DISPDBG((0, "ERROR in linked list: node 0x%p has wrong ClassID: 0x%x (0x%x expected)", pNode, pNode->ClassID,pAnchor->ClassID));
            goto Exit;
        }

        if (   (pNode->ulAllocatedSize != 0)
            && (NULL==pNode->pData) )
        {
            DISPDBG((0, "ERROR in linked list: node 0x%p pData NULL although %d bytes allocated", pNode, pNode->ulAllocatedSize));
            goto Exit;
        }

        if (pAnchor->pCurrent==pNode)
            bFoundCurrent=TRUE;
        ulCount++;
    }

    if (NULL != pNode)
    {
        DISPDBG((0, "ERROR in linked list: didn't find end of list after %d nodes with expected pAnchor->ulNumberOfNodes=%d", ulCount,pAnchor->ulNumberOfNodes));
        goto Exit;
    }

    if (ulCount != pAnchor->ulNumberOfNodes)
    {
        DISPDBG((0, "ERROR in linked list: pAnchor->ulNumberOfNodes=%d not valid (found %d nodes)",pAnchor->ulNumberOfNodes, ulCount));
        goto Exit;
    }

    if (pNodeLast!=pAnchor->pLast)
    {
        DISPDBG((0, "ERROR in linked list: pAnchor->pLast==0x%p instead of 0x%p", pAnchor->pLast,pNodeLast));
        goto Exit;
    }

    if (!bFoundCurrent)
    {
        DISPDBG((0, "ERROR in linked list: pAnchor->pCurrent:0x%p not found",pAnchor->pCurrent));
        goto Exit;
    }

    // couldn't find an error
    bRet = TRUE;
Exit:
    return (bRet);
}


//*************************************************************************
// bLLFindNode
// 
// find node with class ClassID and identifier ulIdent and return node. 
// returns TRUE if node was found; *ppNode is valid; 
// returns FALSE if node wasn't found, *ppNode invalid (not touched)
//*************************************************************************
BOOL bLLFindNode(
    IN  LINKEDLISTHEAD *pAnchor, // root of linked list 
    IN  LL_CLASS_ID     ClassID, // class if data to look for
    IN  ULONG_PTR       ulIdent, // unique identifier of node to finde
    OUT LINKEDLIST    **ppNode)  // contains found node if return is TRUE
{
    LINKEDLIST *pNode   = NULL;
    BOOL        bRet    = FALSE;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID != ClassID);

    if (   (NULL != pAnchor)
        && (   (pAnchor->ClassID == ClassID)                // either specified ClassID
            || (pAnchor->ClassID == LL_CLASS_ANY_CLASS) ) ) // or list can store any class
    {
        // run through linked list
        pNode = pAnchor->pFirst;
        while (pNode)
        {
            if (   (pNode->ulIdent == ulIdent)
                && (pNode->ClassID == ClassID) )
            {                
                // found node
                *ppNode = pNode;
                bRet    = TRUE;
                break;
            }
            pNode = pNode->pNext;
        }
    }

    return (bRet);
}


//******************************************************************************
//
// Exported functions
//

//*************************************************************************
// bLinkedListCreate
// 
// must be called to create and initialize a linked list. A linked list
// can either store any data types (classes) if LL_CLASS_ANY_CLASS is 
// given or only one type.
// returns TRUE if a linked list was created and *ppAnchor contains a 
// valid head.
// returns FALSE if it failed.
//*************************************************************************
BOOL bLinkedListCreate(
    OUT LINKEDLISTHEAD **ppAnchor, // root of linked list 
    IN  LL_CLASS_ID      ClassID)  // class of linked list to create (or 
{
    LINKEDLISTHEAD *pAnchor = NULL;
    BOOL            bRet    = FALSE;

    ASSERT(NULL != ppAnchor);
    ASSERT(NULL == *ppAnchor);
    ASSERT(LL_CLASS_INVALID!=ClassID);

    pAnchor = EngAllocMem(FL_ZERO_MEMORY, sizeof(LINKEDLISTHEAD), ALLOC_TAG);

    if (NULL != pAnchor)
    {
        // initialize head
        pAnchor->ulMagic = LL_MAGIC;
        pAnchor->ClassID = ClassID;

        *ppAnchor = pAnchor;
        ASSERT(bLLValidList(*ppAnchor));
        bRet = TRUE;
    }

    return bRet;
}


//*************************************************************************
// bLinkedListDestroy
// 
// free and destroy a linked list given by ppAnchor. 
// NOTE this function doesn't free nodes!
// returns TRUE if a linked list was destroyed and *ppAnchor is invalidated
// returns FALSE if it failed.
//*************************************************************************
BOOL bLinkedListDestroy(
    IN OUT LINKEDLISTHEAD **ppAnchor)
{
    LINKEDLISTHEAD *pAnchor = NULL;
    BOOL            bRet    = FALSE;

    ASSERT(bLLValidList(*ppAnchor));

    if (NULL != ppAnchor)
    {
        ASSERT(0 == (*ppAnchor)->ulNumberOfNodes);

        (*ppAnchor)->ulMagic = 0;

        EngFreeMem(*ppAnchor);
        *ppAnchor = NULL;
        bRet = TRUE;
    }

    return bRet;
}


//*************************************************************************
// bLinkedListAddElement
// 
// adds an element with the unique identifier ulIdent and the data pData 
// to the linked list. 
//
// NOTE: ulAllocSize is reserved for a future extension, where this would
//       specify additonal memory to allocate and internaly set pData.
//       Currently there is no mehtod to return the allocated pointer.
//
// Returns TRUE if succeeded, use ulIdent and ClassID to access later.
// Returns FALSE if failed.
//*************************************************************************
BOOL bLinkedListAddElement(
    IN LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    IN ULONG_PTR       ulIdent,     // unique identifier of data (can be a pointer)
    IN PVOID           pData,       // pointer to data to store in the node, can be NULL!
    IN ULONG           ulAllocSize) // unused for now! must be 0!
{
    LINKEDLIST *pNode   = NULL;
    BOOL        bRet    = FALSE;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID != ClassID);
    ASSERT(0==ulAllocSize);

    if (   (NULL != pAnchor)
        && (   (pAnchor->ClassID == ClassID)
            || (pAnchor->ClassID == LL_CLASS_ANY_CLASS) ) )
    {
        //
        // Create new node at top of linked list
        //

        pNode = EngAllocMem(FL_ZERO_MEMORY, sizeof(LINKEDLIST)+ulAllocSize, ALLOC_TAG);

        if (NULL != pNode)
        {
            // Add at top of linked list
            pNode->pNext   = pAnchor->pFirst;
            pNode->pPrev   = NULL;

            // let next element point back to us
            if (NULL != pNode->pNext)
                pNode->pNext->pPrev = pNode;

            // replace anchor entries.
            pAnchor->pFirst = pNode;

            if (NULL == pAnchor->pLast)
                pAnchor->pLast = pNode;

            pAnchor->ulNumberOfNodes++;

            // Insert Data to node

            pNode->ClassID = ClassID;
            pNode->ulIdent = ulIdent;
            if (ulAllocSize)
            {
                // cannot allocate if pData already is assigned
                ASSERT(NULL == pData);

                pNode->ulAllocatedSize = ulAllocSize;
                pNode->pData           = (PVOID)(pNode+1); // data starts behind node struct
            }
            else
            {
                // remember data
                pNode->pData   = pData;
            }
            pNode->ulReferenceCount = 0;
            bRet = TRUE;
        }
    }

    ASSERT(bLLValidList(pAnchor));
    ASSERT(bRet);

    return bRet;
}


//*************************************************************************
// bLinkedListRemoveElement
// 
// remove element with unique identifier ulIdent of type ClassID from 
// linked list.
//
// Returns TRUE if succeeded.
// Returns FALSE if failed.
//*************************************************************************
BOOL bLinkedListRemoveElement(
    IN LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    IN ULONG_PTR       ulIdent)     // unique identifier of data (can be a pointer)
{
    LINKEDLIST *pNode   = NULL;
    BOOL        bRet    = FALSE;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID!=ClassID);
    
    if (bLLFindNode(pAnchor, ClassID, ulIdent, &pNode))
    {
        ASSERT(NULL!=pNode);

        // Unlink from linked list
        if (NULL != pNode->pNext)
            pNode->pNext->pPrev = pNode->pPrev;

        if (NULL != pNode->pPrev)
            pNode->pPrev->pNext = pNode->pNext;

        // Unlink from anchor
        if (pAnchor->pFirst == pNode)
        {
            ASSERT(NULL==pNode->pPrev);
            pAnchor->pFirst = pNode->pNext;
        }

        if (pAnchor->pLast == pNode)
        {
            ASSERT(NULL==pNode->pNext);
            pAnchor->pLast = pNode->pPrev;
        }

        if (pAnchor->pCurrent == pNode)
        {
            pAnchor->pCurrent = pNode->pPrev;
        }

        //
        // Nothing should point to pNode anymore
        // 

        EngFreeMem(pNode);

        ASSERT(pAnchor->ulNumberOfNodes!=0);
        pAnchor->ulNumberOfNodes--;

        bRet = TRUE;
    }

    ASSERT(bLLValidList(pAnchor));
    ASSERT(bRet);

    return bRet;
}


//*************************************************************************
// bLinkedListGetElement
// 
// find and return data element of node with unique identifier ulIdent of 
// type ClassID. 
//
// Returns TRUE if element was found (and returns data if ppData != NULL). 
// Returns FALSE if element wasn't found. ppData isn't touched.
//*************************************************************************
BOOL bLinkedListGetElement(
    IN  LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN  LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    IN  ULONG_PTR       ulIdent,     // unique identifier of data (can be a pointer)
    OUT PVOID          *ppData)      // pointer to data returned if bRet==TRUE, ppData can be NULL
{
    LINKEDLIST *pNode   = NULL;
    BOOL        bRet    = FALSE;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID!=ClassID);
    
    if (bLLFindNode(pAnchor, ClassID, ulIdent, &pNode))
    {
        // node found
        if (NULL != ppData)
        {
            // if data returned increase reference counter
            pNode->ulReferenceCount++;
            *ppData = pNode->pData;
        }
        bRet = TRUE;
    }

    return bRet;
}

//*************************************************************************
// bLinkedListGetFirstElement
// bLinkedListGetNextElement
// 
// used to enumerate a linked list from top to bottom. It returns the 
// unique identifier and data of first element with type ClassID 
// within linked list. This is the initializer for a get first/next
// enumeration loop
//
// NOTE: a single or both out params can be NULL if the data isn't needed.
// NOTE 2: Use bLinkedListGetNextElement to enumerate the list after it has
//         been resetted with this bLinkedListGetFirstElement.
//
// Returns TRUE and OUT data if succeed. 
// Returns FALSE if failed or list empty, OUT data not touched
//*************************************************************************
BOOL bLinkedListGetFirstElement(
    IN  LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN  LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    OUT ULONG_PTR      *pulIdent,    // contains unique identifier of data if pulIdent != NULL
    OUT PVOID          *ppData)      // contains pointer to data if ppData != NULL
{
    LINKEDLIST *pNode   = NULL;
    BOOL        bRet    = FALSE;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID!=ClassID);

    // Initialize pCurrent with top of linked list
    pAnchor->pCurrent = pAnchor->pFirst;

    bRet = bLinkedListGetNextElement(pAnchor,ClassID, pulIdent, ppData); 

    return bRet;
}

// see above
BOOL bLinkedListGetNextElement(
    IN  LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN  LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    OUT ULONG_PTR      *pulIdent,    // contains unique identifier of data if pulIdent != NULL
    OUT PVOID          *ppData)      // contains pointer to data if ppData != NULL
{
    LINKEDLIST *pNode   = NULL;
    BOOL        bRet    = FALSE;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID!=ClassID);
    
    // start at last positon
    pNode = pAnchor->pCurrent;

    // find next element matching the type
    while (   (NULL != pNode)
           && (ClassID != pNode->ClassID)
           && (ClassID != LL_CLASS_ANY_CLASS) )
    {
        pNode = pNode->pNext;
    }

    if (NULL != pNode)
    {
        // found node
        if (pulIdent)
            *pulIdent = pNode->ulIdent;
        if (ppData)
            *ppData = pNode->pData;

        // if data returned increase reference counter
        if (pulIdent || ppData)
            pNode->ulReferenceCount++;

        // initialize current with next element (as this one already is returned)
        pAnchor->pCurrent = pNode->pNext;
        bRet = TRUE;
    }
    else
    {
        // cannot continue
        pAnchor->pCurrent = NULL;
    }

    return bRet;
}


//*************************************************************************
// bLinkedListGetLastElement
// bLinkedListGetPrevElement
// 
// same as bLinkedListGetFirstElement and bLinkedListGetNextElement runing
// from bottom to top.
//
// Returns TRUE and OUT data if succeed. 
// Returns FALSE if failed or list empty, OUT data not touched
//*************************************************************************
BOOL bLinkedListGetLastElement(
    IN  LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN  LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    OUT ULONG_PTR      *pulIdent,    // contains unique identifier of data if pulIdent != NULL
    OUT PVOID          *ppData)      // contains pointer to data if ppData != NULL
{
    LINKEDLIST *pNode   = NULL;
    BOOL        bRet    = FALSE;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID!=ClassID);

    // Initialize pCurrent with top of linked list
    pAnchor->pCurrent = pAnchor->pLast;

    bRet = bLinkedListGetPrevElement(pAnchor,ClassID, pulIdent, ppData); 

    return bRet;
}

// see above
BOOL bLinkedListGetPrevElement(
    IN  LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN  LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    OUT ULONG_PTR      *pulIdent,    // contains unique identifier of data if pulIdent != NULL
    OUT PVOID          *ppData)      // contains pointer to data if ppData != NULL
{
    LINKEDLIST *pNode   = NULL;
    BOOL        bRet    = FALSE;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID!=ClassID);

    // start at bottom of linked list
    pNode = pAnchor->pCurrent;

    // find previous element matching the type
    while (   (NULL != pNode)
           && (   (ClassID != pNode->ClassID)
               && (ClassID != LL_CLASS_ANY_CLASS) ) )
    {
        pNode = pNode->pPrev;
    }

    if (NULL != pNode)
    {
        // found node
        if (pulIdent)
            *pulIdent = pNode->ulIdent;
        if (ppData)
            *ppData = pNode->pData;

        // if data returned increase reference counter
        if (pulIdent || ppData)
            pNode->ulReferenceCount++;
        pAnchor->pCurrent = pNode->pPrev;
        bRet = TRUE;
    }
    else
    {
        pAnchor->pCurrent = NULL;
    }

    return bRet;
}


//*************************************************************************
// bLinkedListGetNewestElement
// 
// returns the data of the newset element in the linked list of given type.
//
// Returns TRUE and OUT data if succeed. 
// Returns FALSE if failed or list empty, OUT data not touched
//*************************************************************************
BOOL bLinkedListGetNewestElement(
    IN  LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN  LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    OUT ULONG_PTR      *pulIdent,    // contains unique identifier of data if pulIdent != NULL
    OUT PVOID          *ppData)      // contains pointer to data if ppData != NULL
{
    ASSERT(bLLValidList(pAnchor));
    return(bLinkedListGetFirstElement(pAnchor, ClassID, pulIdent, ppData)); 
}


//*************************************************************************
// bLinkedListGetOldestElement
// 
// returns the data of the oldest element in the linked list of given type.
//
// Returns TRUE and OUT data if succeed. 
// Returns FALSE if failed or list empty, OUT data not touched
//*************************************************************************
BOOL bLinkedListGetOldestElement(
    IN  LINKEDLISTHEAD *pAnchor,     // root of linked list 
    IN  LL_CLASS_ID     ClassID,     // type of this date (must be valid for this linked list)
    OUT ULONG_PTR      *pulIdent,    // contains unique identifier of data if pulIdent != NULL
    OUT PVOID          *ppData)      // contains pointer to data if ppData != NULL
{
    ASSERT(bLLValidList(pAnchor));
    return(bLinkedListGetLastElement(pAnchor, ClassID, pulIdent, ppData)); 
}


//*************************************************************************
// ulLinkedListGetCountOfElements
// 
// returns number of node stored in linked list that have type ClassID.
//*************************************************************************
ULONG ulLinkedListGetCountOfElements(
    LINKEDLISTHEAD *pAnchor,  // root of linked list 
    LL_CLASS_ID     ClassID)  // type of this date (must be valid for this linked list)
{
    BOOL  bFound;
    ULONG ulRet  = 0;

    ASSERT(bLLValidList(pAnchor));
    ASSERT(LL_CLASS_INVALID!=ClassID);

    if (   (ClassID == pAnchor->ClassID)
        && (ClassID != LL_CLASS_ANY_CLASS) )
    {
        // Only one class is stored, return all linked nodes
        ulRet = pAnchor->ulNumberOfNodes;
    }
    else if (ClassID == LL_CLASS_ANY_CLASS)
    {
        // type doesn't matter, return all nodes
        ulRet = pAnchor->ulNumberOfNodes;
    }
    else
    {
        // Only count members of class ClassID
        // need to enumerate and find nodes.
        if (bLinkedListGetFirstElement(pAnchor, ClassID, NULL, NULL))
        {
            ulRet++;

            while (bLinkedListGetNextElement(pAnchor, ClassID, NULL, NULL))
            {
                ulRet++;
            }
        }
    }

    ASSERT(ulRet <= pAnchor->ulNumberOfNodes);
    
    return ulRet;
}

// End of linkedlist.c
