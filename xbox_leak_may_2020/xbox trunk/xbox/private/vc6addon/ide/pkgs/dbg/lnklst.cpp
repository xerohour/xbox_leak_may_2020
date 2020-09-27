#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define HUGE

// Linked list variables used to store lists of objects
// Node structure
typedef struct NodeTag {
	DWORD length;	// length of current node
	char string [1]; // used to be []
	} NODE, FAR *LPNODE;

// Header structure
typedef struct {
	HGLOBAL hMem;	// Handle to the node
	DWORD NbItem;	// Nb of items in the structure
	DWORD Size;		// Allocated size in bytes
	DWORD UsedSize;	// Used size in bytes
	DWORD LastNode;	// Offset to last node
	UINT Id;		// Id of the list
	} LISTHEADER;

// 4 linked lists
#define cLists	4
// Initial list size
#define LIST_SIZE	1024
static LISTHEADER ListHeader[cLists];


/****************************************************************************

		  FUNCTION:   GetListIndex(int)

		  PURPOSE:    Internal: given a list ID, returns an index

****************************************************************************/
static int GetListIndex(int listId)
{
	int	iList;

	switch (listId)
	{
	case DLG_EXCEP_LIST:
		iList = 0;
		break;
	case DLG_EXCEP_LIST_BACKUP:
		iList = 1;
		break;
	case DLG_SYSTEM_EXCEP_LIST:
		iList = 2;
		break;
	case DLG_THREAD_LIST:
		iList = 3;
		break;
	default:
		// AuxPrintf2("Val=%d",listId);
		ASSERT(FALSE);
		return 0;
	}

	ASSERT(iList < cLists);
	return iList;
}
/****************************************************************************

		  FUNCTION:   GetObjectPointer(DWORD, int, LPSTR *, DWORD *)

		  PURPOSE:    Internal: Return a pointer to a particular object
					  in a list.

		  INPUT:	  index = index into list
					  ListIndex = index of the list

		  OUTPUT:	  *plpMem = ptr to memory block (must be unlocked later)
					  *poffset = offset of item in list

****************************************************************************/
static BOOL GetObjectPointer(DWORD index, int ListIndex, LPSTR *plpMem, DWORD *poffset)
{
LPNODE lpNode;
LPSTR lpMem;
DWORD i;
DWORD offset;
	if (ListIndex == -1)
		return FALSE;
	if (index >= ListHeader[ListIndex].NbItem)
		return (FALSE);

	// Get pointer to block
	ASSERT (ListHeader[ListIndex].hMem != NULL);
	lpMem = (LPSTR)GlobalLock(ListHeader[ListIndex].hMem);
	if (lpMem == NULL)
		return (FALSE);
	// Get position for new node
	i = 0;
	offset = 0;
	do
	{
		lpNode = (LPNODE)(lpMem + offset);
		ASSERT (lpNode->length < 500);
		if (lpNode->length > 500)	// Assuming length won't be that large
		{
			GlobalUnlock(ListHeader[ListIndex].hMem);
			return (FALSE);
		}
		offset += lpNode->length;
	}
	while (i++ < index);
	offset -= lpNode->length;

	*plpMem = lpMem;
	*poffset = offset;
	return TRUE;
}
/****************************************************************************

		  FUNCTION:   MaybeGrowBuffer(int, DWORD)

		  PURPOSE:    Internal: Grow a buffer if necessary

****************************************************************************/
static BOOL MaybeGrowBuffer(int ListIndex, DWORD AddedSize)
{
HGLOBAL hMem;
	if (AddedSize + ListHeader[ListIndex].UsedSize
												> ListHeader[ListIndex].Size)
	{
		hMem = GlobalReAlloc(ListHeader[ListIndex].hMem,
							 LIST_SIZE + ListHeader[ListIndex].Size,
							 GMEM_MOVEABLE/*|GMEM_DISCARDABLE*/);
		if (hMem == NULL)
		{
			ErrorBox(ERR_Memory_Is_Low);
			return FALSE;
		}
		ListHeader[ListIndex].hMem = hMem;
		ListHeader[ListIndex].Size += LIST_SIZE;
	}
	return TRUE;
}
/****************************************************************************

		  FUNCTION:   InitList(int)

		  PURPOSE:    Create a list if it does not exist

		  RETURN:     TRUE if OK

****************************************************************************/
BOOL InitList(int listId)
{
int ListIndex;

	// Get list array index from Id
	ListIndex = GetListIndex(listId);
	if (ListIndex == -1)
		return (FALSE);

	// Allocate memory if the list isn't created yet
	if (ListHeader[ListIndex].hMem == NULL)
	{
		ListHeader[ListIndex].hMem =
						GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE,LIST_SIZE);
		if (ListHeader[ListIndex].hMem == NULL)
			return FALSE;
		ListHeader[ListIndex].Size = LIST_SIZE;
		ListHeader[ListIndex].UsedSize = 0;
		ListHeader[ListIndex].NbItem = 0;
		ListHeader[ListIndex].LastNode = 0;
		ListHeader[ListIndex].Id = listId;
	}

	return (TRUE);
}
/****************************************************************************

		  FUNCTION:   EmptyList(int)

		  PURPOSE:    Reset the content of a list

		  RETURN:     TRUE if OK

****************************************************************************/
BOOL EmptyList(int listId)
{
int ListIndex;

	// Get list array index from Id
	ListIndex = GetListIndex(listId);
	if (ListIndex == -1)
		return (FALSE);

	ListHeader[ListIndex].NbItem = 0;
	ListHeader[ListIndex].LastNode = 0;
	ListHeader[ListIndex].UsedSize = 0;
	ListHeader[ListIndex].Id = listId;

	return (TRUE);
}
/****************************************************************************

		  FUNCTION:   FreeList(int listId)

		  PURPOSE:    Free allocated memory  for the given list
					  This function is called when msvc exit

****************************************************************************/
void FreeList(void)
{
int ListIndex;

	for (ListIndex = 0; ListIndex < cLists; ListIndex++)
	{
		if (ListHeader[ListIndex].hMem != NULL)
		{
			GlobalFree(ListHeader[ListIndex].hMem);
			ListHeader[ListIndex].hMem = NULL;
			ListHeader[ListIndex].Size = 0;
			ListHeader[ListIndex].NbItem = 0;
			ListHeader[ListIndex].LastNode = 0;
		}
	}
}
/****************************************************************************

		  FUNCTION:   ListCopy(int newlist,int oldlist)

		  PURPOSE:    Duplicate a list

****************************************************************************/
BOOL ListCopy(int newId, int oldId, LPSTR buffer, UINT cch)
{
int i,count;
	if (GetListIndex(newId) == -1 || GetListIndex(oldId) == -1)
		return (FALSE);

	InitList(newId);
	EmptyList(newId);
	count = (int)ListGetCount(oldId);
	for (i=0; i<count; i++)
	{
		if (!ListGetObject(i,oldId,buffer))
		{
			ASSERT(FALSE);
			EmptyList(newId);
			return (FALSE);
		}
		if (!ListAddObject(newId,buffer,cch))
		{
			ASSERT(FALSE);
			EmptyList(newId);
			return (FALSE);
		}
	}
	return (TRUE);
}
/****************************************************************************

		  FUNCTION:   ListGetCount(int)

		  RETURN:     number of items in the list

****************************************************************************/
DWORD ListGetCount(int listId)
{
int ListIndex;
	ListIndex = GetListIndex(listId);
	if (ListIndex == -1)
		return 0;
	return (ListHeader[ListIndex].NbItem);
}
/****************************************************************************

		  FUNCTION:   ListInsertObject(int,int,LPSTR, int)

		  PURPOSE:    Insert an object into a linked list

		  RETURN:     TRUE if success

****************************************************************************/
BOOL ListInsertObject(DWORD index, int listId, LPSTR string, int cch)	// cch includes the null terminating byte
{
int ListIndex;
DWORD AddedSize;
LPNODE lpNode;
LPSTR lpMem;
DWORD offset;
	ListIndex = GetListIndex(listId);
	if (ListIndex == -1)
		return FALSE;
	if (index == ListHeader[ListIndex].NbItem)
		return ListAddObject(listId, string, cch);	// append
	AddedSize = sizeof(NODE) + cch;
	AddedSize = (AddedSize + 3) & ~3L;	// align on dword boundary

	// Realloc buffer if necessary
	if (!MaybeGrowBuffer(ListIndex, AddedSize))
		return FALSE;
	// Get offset where we need to insert
	if (!GetObjectPointer(index, ListIndex, &lpMem, &offset))
		return FALSE;
	lpNode = (LPNODE)(lpMem + offset);
	// Shift old nodes
	_fmemmove((LPSTR)lpNode+AddedSize,lpNode,ListHeader[ListIndex].UsedSize-offset);
	// Create new node
	_fmemmove(lpNode->string,string,cch);
	lpNode->length = AddedSize;
	// Update list size
	if (offset < ListHeader[ListIndex].UsedSize)
		ListHeader[ListIndex].LastNode += AddedSize;
	ListHeader[ListIndex].UsedSize += AddedSize;
	ListHeader[ListIndex].NbItem++;
	GlobalUnlock(ListHeader[ListIndex].hMem);
	return (TRUE);
}
/****************************************************************************

		  FUNCTION:   ListAddObject(int, LPSTR, int)

		  PURPOSE:    Add an object at the end of a linked list

		  RETURN:     TRUE if success

****************************************************************************/
BOOL ListAddObject(int listId, LPSTR string, int cch)	// cch includes the null terminating byte
{
int ListIndex;
DWORD AddedSize;
LPNODE lpNode;
LPSTR lpMem;
	ListIndex = GetListIndex(listId);
	if (ListIndex == -1)
		return FALSE;
	AddedSize = sizeof(NODE) + cch;
	AddedSize = (AddedSize + 3) & ~3L;	// align on dword boundary
	ASSERT (ListHeader[ListIndex].hMem != NULL);

	// Realloc buffer if necessary
	if (!MaybeGrowBuffer(ListIndex, AddedSize))
		return FALSE;
	// Get pointer to block
	lpMem = (LPSTR)GlobalLock(ListHeader[ListIndex].hMem);
	if (lpMem == NULL)
		return (FALSE);
	// Create node
	lpNode = (LPNODE)(lpMem + ListHeader[ListIndex].UsedSize);
	_fmemmove(lpNode->string,string,cch);
	lpNode->length = AddedSize;
	// Link the new node
	ListHeader[ListIndex].LastNode = ListHeader[ListIndex].UsedSize;
	ListHeader[ListIndex].UsedSize += AddedSize;
	ListHeader[ListIndex].NbItem++;
	GlobalUnlock(ListHeader[ListIndex].hMem);
	return (TRUE);
}
/****************************************************************************

		  FUNCTION:   ListUpdateObject(int,int,LPSTR)

		  PURPOSE:    Get a string in a linked list

		  RETURN:     TRUE if success

****************************************************************************/
BOOL ListUpdateObject(DWORD index, int listId, LPSTR string, int cch)
{
int ListIndex;
DWORD offset;
LPNODE lpNode;
LPSTR lpMem;
	ListIndex = GetListIndex(listId);
	if (ListIndex == -1)
		return FALSE;
	if (index > ListHeader[ListIndex].NbItem)
		return (FALSE);

	// Get pointer to block
	if (!GetObjectPointer(index, ListIndex, &lpMem, &offset))
		return FALSE;
	lpNode = (LPNODE)(lpMem + offset);

	_fmemmove(lpNode->string, string, (size_t)(lpNode->length-sizeof(NODE)));
	GlobalUnlock(ListHeader[ListIndex].hMem);
	return (TRUE);
}

/****************************************************************************

		  FUNCTION:   ListGetObject(int,int,LPSTR)

		  PURPOSE:    Get a string in a linked list

		  RETURN:     TRUE if success

****************************************************************************/
BOOL ListGetObject(DWORD index, int listId, LPSTR string)
{
int ListIndex;
DWORD offset;
LPNODE lpNode;
LPSTR lpMem;
	ListIndex = GetListIndex(listId);
	if (ListIndex == -1)
		return FALSE;

	// Get pointer to block
	if (!GetObjectPointer(index, ListIndex, &lpMem, &offset))
		return FALSE;
	lpNode = (LPNODE)(lpMem + offset);

	_fmemmove(string, lpNode->string, (size_t)(lpNode->length-sizeof(NODE)));
	GlobalUnlock(ListHeader[ListIndex].hMem);
	return (TRUE);
}

/****************************************************************************

		  FUNCTION:   ListDeleteObject(int,int)

		  PURPOSE:    Delete a string in a linked list

		  RETURN:     TRUE if success

****************************************************************************/
BOOL ListDeleteObject(DWORD index, int listId)
{
int ListIndex;
DWORD offset,NodeLength;
LPNODE lpNode;
LPSTR lpMem;
char HUGE *lp1, HUGE *lp2;
	ListIndex = GetListIndex(listId);
	if (ListIndex == -1)
		return FALSE;
	if (index > ListHeader[ListIndex].NbItem)
		return (FALSE);

	// Get pointer to block
	if (!GetObjectPointer(index, ListIndex, &lpMem, &offset))
		return FALSE;
	lpNode = (LPNODE)(lpMem + offset);

	// Shift the remaining bytes so the node is erased
	NodeLength = lpNode->length;	// length of node to delete
	lp1 = (char HUGE*)lpNode;		// point to current node
	lp2 = lp1 + NodeLength;			// point to next node
	while (offset < ListHeader[ListIndex].UsedSize)
	{
		*lp1++ = *lp2++;
		offset++;
	}
	ListHeader[ListIndex].UsedSize -= NodeLength;
	ListHeader[ListIndex].LastNode -= NodeLength;
	ListHeader[ListIndex].NbItem--;
	GlobalUnlock(ListHeader[ListIndex].hMem);
	return (TRUE);
}


