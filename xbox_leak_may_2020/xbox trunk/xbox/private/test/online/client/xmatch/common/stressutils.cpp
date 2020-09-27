//==================================================================================
// stressutils.cpp: implementation of various helper classes for stress tests
//==================================================================================
#include "stressutils.h"

//==================================================================================
// CXMStressList
//==================================================================================
template <class T>
CXMStressList<T>::CXMStressList()
{
	m_dwItemCount = 0;
	m_pFirstItem = NULL;
	m_pCurrentItem = NULL;
}

template <class T>
CXMStressList<T>::~CXMStressList()
{
	PXMSTRESS_ITEM pNext = NULL, pCurrent = m_pFirstItem;

	while(pCurrent)
	{
		pNext = pCurrent->pNext;
		delete pCurrent;
		pCurrent = pNext;
	}
}

template <class T>
T* CXMStressList<T>::GetFirstItem()
{
	if(m_pFirstItem)
	{
		m_pCurrentItem = m_pFirstItem->pNext;
		return (T*) m_pFirstItem->pItem;
	}

	return NULL;
}

template <class T>
T* CXMStressList<T>::GetNextItem()
{
	PXMSTRESS_ITEM pReturnItem = m_pCurrentItem;

	// If there is another item, then move the scanning pointer ahead so that
	// the next call to GetNextItem will work
	if(m_pCurrentItem)
	{
		m_pCurrentItem = m_pCurrentItem->pNext;
	}

	return pReturnItem;
}

// Add a new item to the head
template <class T>
BOOL CXMStressList<T>::AddItem( T* pItem )
{
	PXMSTRESS_ITEM pHead = m_pFirstItem;

	m_pFirstItem = new XMSTRESS_ITEM;

	// If we can't allocate a new item, then set the first item
	// back to its original value and return FALSE;
	if(!m_pFirstItem)
	{
		m_pFirstItem = pHead;
		return FALSE;
	}

	m_pFirstItem->pItem = pItem;
	m_pFirstItem->pNext = pHead;

	++m_dwItemCount;

	return TRUE;
}

// Remove a specific item from the list
template <class T>
BOOL CXMStressList<T>::RemoveItem( T* pItem )
{
	PXMSTRESS_ITEM pPrevious = NULL, pCurrent = m_pFirstItem;

	// Search the entire list for the same pointer
	while(pCurrent)
	{
		if(pCurrent->pItem == pItem)
		{
			// If someone is scanning through the list via GetNextItem, then we need
			// to make sure the scanning pointer doesn't point to something we're deleting
			if(m_pCurrentItem = pCurrent)
				m_pCurrentItem = pCurrent->pNext;

			// In this case, we are not removing the first item of the list
			if(pPrevious)
			{
				pPrevious->pNext = pCurrent->pNext;
			}
			// In this case, we are removing the first item of the list
			else
			{
				m_pFirstItem = pCurrent->pNext;
			}

			// Delete the item and exit
			delete pCurrent;
			--m_dwItemCount;

			return TRUE;
		}

		// Continue scanning items
		pPrevious = pCurrent;
		pCurrent = pCurrent->pNext;
	}

	return FALSE;
}
