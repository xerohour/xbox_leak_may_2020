// (KPerry) This was the MFC collection stuff
// Now it has been purged of all other MFC influences and is a stand alone entity.

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for VCCOLL.H

#if _MSC_VER > 1000
#pragma once
#endif



////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCByteArray::GetSize() const
	{ return m_nSize; }
inline INT_PTR CVCByteArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CVCByteArray::RemoveAll()
	{ SetSize(0); }
inline BYTE CVCByteArray::GetAt(INT_PTR nIndex) const
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline void CVCByteArray::SetAt(INT_PTR nIndex, BYTE newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline BYTE& CVCByteArray::ElementAt(INT_PTR nIndex)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline const BYTE* CVCByteArray::GetData() const
	{ return (const BYTE*)m_pData; }
inline BYTE* CVCByteArray::GetData()
	{ return (BYTE*)m_pData; }
inline INT_PTR CVCByteArray::Add(BYTE newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline BYTE CVCByteArray::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
inline BYTE& CVCByteArray::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCWordArray::GetSize() const
	{ return m_nSize; }
inline INT_PTR CVCWordArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CVCWordArray::RemoveAll()
	{ SetSize(0); }
inline WORD CVCWordArray::GetAt(INT_PTR nIndex) const
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline void CVCWordArray::SetAt(INT_PTR nIndex, WORD newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline WORD& CVCWordArray::ElementAt(INT_PTR nIndex)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline const WORD* CVCWordArray::GetData() const
	{ return (const WORD*)m_pData; }
inline WORD* CVCWordArray::GetData()
	{ return (WORD*)m_pData; }
inline INT_PTR CVCWordArray::Add(WORD newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline WORD CVCWordArray::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
inline WORD& CVCWordArray::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCDWordArray::GetSize() const
	{ return m_nSize; }
inline INT_PTR CVCDWordArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CVCDWordArray::RemoveAll()
	{ SetSize(0); }
inline DWORD CVCDWordArray::GetAt(INT_PTR nIndex) const
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline void CVCDWordArray::SetAt(INT_PTR nIndex, DWORD newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline DWORD& CVCDWordArray::ElementAt(INT_PTR nIndex)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline const DWORD* CVCDWordArray::GetData() const
	{ return (const DWORD*)m_pData; }
inline DWORD* CVCDWordArray::GetData()
	{ return (DWORD*)m_pData; }
inline INT_PTR CVCDWordArray::Add(DWORD newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline DWORD CVCDWordArray::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
inline DWORD& CVCDWordArray::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCUIntArray::GetSize() const
	{ return m_nSize; }
inline INT_PTR CVCUIntArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CVCUIntArray::RemoveAll()
	{ SetSize(0); }
inline UINT CVCUIntArray::GetAt(INT_PTR nIndex) const
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline void CVCUIntArray::SetAt(INT_PTR nIndex, UINT newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline UINT& CVCUIntArray::ElementAt(INT_PTR nIndex)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline const UINT* CVCUIntArray::GetData() const
	{ return (const UINT*)m_pData; }
inline UINT* CVCUIntArray::GetData()
	{ return (UINT*)m_pData; }
inline INT_PTR CVCUIntArray::Add(UINT newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline UINT CVCUIntArray::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
inline UINT& CVCUIntArray::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCPtrArray::GetSize() const
	{ return m_nSize; }
inline INT_PTR CVCPtrArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CVCPtrArray::RemoveAll()
	{ SetSize(0); }
inline void* CVCPtrArray::GetAt(INT_PTR nIndex) const
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline void CVCPtrArray::SetAt(INT_PTR nIndex, void* newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline void*& CVCPtrArray::ElementAt(INT_PTR nIndex)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline const void** CVCPtrArray::GetData() const
	{ return (const void**)m_pData; }
inline void** CVCPtrArray::GetData()
	{ return (void**)m_pData; }
inline INT_PTR CVCPtrArray::Add(void* newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline void* CVCPtrArray::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
inline void*& CVCPtrArray::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCPtrList::GetCount() const
	{ return m_nCount; }
inline BOOL CVCPtrList::IsEmpty() const
	{ return m_nCount == 0; }
inline void*& CVCPtrList::GetHead()
	{ VSASSERT(m_pNodeHead != NULL, "GetHead called on empty CVCPtrList!");
		return m_pNodeHead->data; }
inline void* CVCPtrList::GetHead() const
	{ VSASSERT(m_pNodeHead != NULL, "GetHead called on empty CVCPtrList!");
		return m_pNodeHead->data; }
inline void*& CVCPtrList::GetTail()
	{ VSASSERT(m_pNodeTail != NULL, "GetTail called on empty CVCPtrList!");
		return m_pNodeTail->data; }
inline void* CVCPtrList::GetTail() const
	{ VSASSERT(m_pNodeTail != NULL, "GetTail called on empty CVCPtrList!");
		return m_pNodeTail->data; }
inline VCPOSITION CVCPtrList::GetHeadPosition() const
	{ return (VCPOSITION) m_pNodeHead; }
inline VCPOSITION CVCPtrList::GetTailPosition() const
	{ return (VCPOSITION) m_pNodeTail; }
inline void*& CVCPtrList::GetNext(VCPOSITION& rPosition) // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline void* CVCPtrList::GetNext(VCPOSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline void*& CVCPtrList::GetPrev(VCPOSITION& rPosition) // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline void* CVCPtrList::GetPrev(VCPOSITION& rPosition) const // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline void*& CVCPtrList::GetAt(VCPOSITION position)
	{ CNode* pNode = (CNode*) position;
		return pNode->data; }
inline void* CVCPtrList::GetAt(VCPOSITION position) const
	{ CNode* pNode = (CNode*) position;
		return pNode->data; }
inline void CVCPtrList::SetAt(VCPOSITION pos, void* newElement)
	{ CNode* pNode = (CNode*) pos;
		pNode->data = newElement; }



////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCMapWordToPtr::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapWordToPtr::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapWordToPtr::SetAt(WORD key, void* newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapWordToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_VCPOSITION; }
inline UINT CVCMapWordToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCMapDWordToPtr::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapDWordToPtr::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapDWordToPtr::SetAt(DWORD key, void* newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapDWordToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_VCPOSITION; }
inline DWORD CVCMapDWordToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCMapPtrToWord::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapPtrToWord::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapPtrToWord::SetAt(void* key, WORD newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapPtrToWord::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_VCPOSITION; }
inline UINT CVCMapPtrToWord::GetHashTableSize() const
	{ return m_nHashTableSize; }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCMapPtrToPtr::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapPtrToPtr::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapPtrToPtr::SetAt(void* key, void* newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapPtrToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_VCPOSITION; }
inline UINT CVCMapPtrToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CVCList<TYPE, ARG_TYPE> inline and out of line functions

template<class TYPE, class ARG_TYPE>
inline INT_PTR CVCList<TYPE, ARG_TYPE>::GetCount() const
	{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
inline BOOL CVCList<TYPE, ARG_TYPE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CVCList<TYPE, ARG_TYPE>::GetHead()
	{ VSASSERT(m_pNodeHead != NULL, "GetHead called on empty CVCList<>!");
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CVCList<TYPE, ARG_TYPE>::GetHead() const
	{ VSASSERT(m_pNodeHead != NULL, "GetHead called on empty CVCList<>!");
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CVCList<TYPE, ARG_TYPE>::GetTail()
	{ VSASSERT(m_pNodeTail != NULL, "GetTail called on empty CVCList<>!");
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CVCList<TYPE, ARG_TYPE>::GetTail() const
	{ VSASSERT(m_pNodeTail != NULL, "GetTail called on empty CVCList<>!");
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline VCPOSITION CVCList<TYPE, ARG_TYPE>::GetHeadPosition() const
	{ return (VCPOSITION) m_pNodeHead; }
template<class TYPE, class ARG_TYPE>
inline VCPOSITION CVCList<TYPE, ARG_TYPE>::GetTailPosition() const
	{ return (VCPOSITION) m_pNodeTail; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CVCList<TYPE, ARG_TYPE>::GetNext(VCPOSITION& rPosition) // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CVCList<TYPE, ARG_TYPE>::GetNext(VCPOSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CVCList<TYPE, ARG_TYPE>::GetPrev(VCPOSITION& rPosition) // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CVCList<TYPE, ARG_TYPE>::GetPrev(VCPOSITION& rPosition) const // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CVCList<TYPE, ARG_TYPE>::GetAt(VCPOSITION position)
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CVCList<TYPE, ARG_TYPE>::GetAt(VCPOSITION position) const
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline void CVCList<TYPE, ARG_TYPE>::SetAt(VCPOSITION pos, ARG_TYPE newElement)
	{ CNode* pNode = (CNode*) pos;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		pNode->data = newElement; }

template<class TYPE, class ARG_TYPE>
CVCList<TYPE, ARG_TYPE>::CVCList(INT_PTR nBlockSize)
{
	VSASSERT(nBlockSize > 0, "Must initialize with positive block size");

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class TYPE, class ARG_TYPE>
void CVCList<TYPE, ARG_TYPE>::RemoveAll()
{
	VSASSERT(this, "Bad this pointer!");

	// destroy elements
	CNode* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		VCDestructElements<TYPE>(&pNode->data, 1);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class TYPE, class ARG_TYPE>
CVCList<TYPE, ARG_TYPE>::~CVCList()
{
	RemoveAll();
	VSASSERT(m_nCount == 0, "Elements still in list after RemoveAll!!");
}

/////////////////////////////////////////////////////////////////////////////
// Node helpers
//
// Implementation note: CNode's are stored in CVCPlex blocks and
//  chained together. Free blocks are maintained in a singly linked list
//  using the 'pNext' member of CNode with 'm_pNodeFree' as the head.
//  Used blocks are maintained in a doubly linked list using both 'pNext'
//  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
//   as the head/tail.
//
// We never free a CVCPlex block unless the List is destroyed or RemoveAll()
//  is used - so the total number of CVCPlex blocks may grow large depending
//  on the maximum past size of the list.
//

template<class TYPE, class ARG_TYPE>
CVCList<TYPE, ARG_TYPE>::CNode*
CVCList<TYPE, ARG_TYPE>::NewNode(CVCList::CNode* pPrev, CVCList::CNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		CVCPlex* pNewBlock = CVCPlex::Create(m_pBlocks, m_nBlockSize,
				 sizeof(CNode));

		// chain them into free list
		CNode* pNode = (CNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (INT_PTR i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
	VSASSERT(m_pNodeFree != NULL, "we must have something on the free list");

	CVCList::CNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	VSASSERT(m_nCount > 0, "make sure we don't overflow");

	VCConstructElements<TYPE>(&pNode->data, 1);
	return pNode;
}

template<class TYPE, class ARG_TYPE>
void CVCList<TYPE, ARG_TYPE>::FreeNode(CVCList::CNode* pNode)
{
	VCDestructElements<TYPE>(&pNode->data, 1);
	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	VSASSERT(m_nCount >= 0, "make sure we don't underflow"); 

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class TYPE, class ARG_TYPE>
VCPOSITION CVCList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{
	VSASSERT(this, "Bad this pointer!");

	CNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (VCPOSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
VCPOSITION CVCList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{
	VSASSERT(this, "Bad this pointer!");

	CNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (VCPOSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CVCList<TYPE, ARG_TYPE>::AddHead(CVCList* pNewList)
{
	VSASSERT(this, "Bad this pointer!");
	VSASSERT(pNewList != NULL, "Cannot add NULL list!");

	// add a list of same elements to head (maintain order)
	VCPOSITION pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

template<class TYPE, class ARG_TYPE>
void CVCList<TYPE, ARG_TYPE>::AddTail(CVCList* pNewList)
{
	VSASSERT(this, "Bad this pointer!");
	VSASSERT(pNewList != NULL, "Cannot add NULL list!");

	// add a list of same elements
	VCPOSITION pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

template<class TYPE, class ARG_TYPE>
TYPE CVCList<TYPE, ARG_TYPE>::RemoveHead()
{
	VSASSERT(this, "Bad this pointer!");
	VSASSERT(m_pNodeHead != NULL, "don't call RemoveHead on empty list !!!");
	VSASSERT(VCIsValidAddress(m_pNodeHead, sizeof(CNode)), "Invalid node head!");

	CNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
TYPE CVCList<TYPE, ARG_TYPE>::RemoveTail()
{
	VSASSERT(this, "Bad this pointer!");
	VSASSERT(m_pNodeTail != NULL, "don't call RemoveTail on empty list !!!");
	VSASSERTE(VCIsValidAddress(m_pNodeTail, sizeof(CNode)), "Invalid node tail!");

	CNode* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
VCPOSITION CVCList<TYPE, ARG_TYPE>::InsertBefore(VCPOSITION position, ARG_TYPE newElement)
{
	VSASSERT(this, "Bad this pointer!");

	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
		VSASSERT(VCIsValidAddress(pOldNode->pPrev, sizeof(CNode)), "Bad node in list!");
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		VSASSERT(pOldNode == m_pNodeHead, "Better be working with node at the head of the list!");
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (VCPOSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
VCPOSITION CVCList<TYPE, ARG_TYPE>::InsertAfter(VCPOSITION position, ARG_TYPE newElement)
{
	VSASSERT(this, "Bad this pointer!");

	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	VSASSERT(VCIsValidAddress(pOldNode, sizeof(CNode)), "Bad node in list!");
	CNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
		VSASSERT(VCIsValidAddress(pOldNode->pNext, sizeof(CNode)), "Bad node in list!");
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		VSASSERT(pOldNode == m_pNodeTail, "Better be working with node at the end of the list!");
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (VCPOSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CVCList<TYPE, ARG_TYPE>::RemoveAt(VCPOSITION position)
{
	VSASSERT(this, "Bad this pointer!");

	CNode* pOldNode = (CNode*) position;
	VSASSERT(VCIsValidAddress(pOldNode, sizeof(CNode)), "Bad node in list!");

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		VSASSERT(VCIsValidAddress(pOldNode->pPrev, sizeof(CNode)), "Bad node in list!");
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		VSASSERT(VCIsValidAddress(pOldNode->pNext, sizeof(CNode)), "Bad node in list!");
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
VCPOSITION CVCList<TYPE, ARG_TYPE>::FindIndex(INT_PTR nIndex) const
{
	VSASSERT(this, "Bad this pointer!");

	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	CNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		pNode = pNode->pNext;
	}
	return (VCPOSITION) pNode;
}

template<class TYPE, class ARG_TYPE>
VCPOSITION CVCList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, VCPOSITION startAfter) const
{
	VSASSERT(this, "Bad this pointer!");

	CNode* pNode = (CNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; pNode != NULL; pNode = pNode->pNext)
		if (CompareElements<TYPE>(&pNode->data, &searchValue))
			return (VCPOSITION)pNode;
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CVCTypedPtrList<BASE_CLASS, TYPE>

template<class BASE_CLASS, class TYPE>
class CVCTypedPtrList : public BASE_CLASS
{
public:
// Construction
	CVCTypedPtrList(INT_PTR nBlockSize = 10)
		: BASE_CLASS(nBlockSize) { }

	// peek at head or tail
	TYPE& GetHead()
		{ return (TYPE&)BASE_CLASS::GetHead(); }
	TYPE GetHead() const
		{ return (TYPE)BASE_CLASS::GetHead(); }
	TYPE& GetTail()
		{ return (TYPE&)BASE_CLASS::GetTail(); }
	TYPE GetTail() const
		{ return (TYPE)BASE_CLASS::GetTail(); }

	// get head or tail (and remove it) - don't call on empty list!
	TYPE RemoveHead()
		{ return (TYPE)BASE_CLASS::RemoveHead(); }
	TYPE RemoveTail()
		{ return (TYPE)BASE_CLASS::RemoveTail(); }

	// add before head or after tail
	VCPOSITION AddHead(TYPE newElement)
		{ return BASE_CLASS::AddHead(newElement); }
	VCPOSITION AddTail(TYPE newElement)
		{ return BASE_CLASS::AddTail(newElement); }

	// add another list of elements before head or after tail
	void AddHead(CVCTypedPtrList<BASE_CLASS, TYPE>* pNewList)
		{ BASE_CLASS::AddHead(pNewList); }
	void AddTail(CVCTypedPtrList<BASE_CLASS, TYPE>* pNewList)
		{ BASE_CLASS::AddTail(pNewList); }

	// iteration
	TYPE& GetNext(VCPOSITION& rPosition)
		{ return (TYPE&)BASE_CLASS::GetNext(rPosition); }
	TYPE GetNext(VCPOSITION& rPosition) const
		{ return (TYPE)BASE_CLASS::GetNext(rPosition); }
	TYPE& GetPrev(VCPOSITION& rPosition)
		{ return (TYPE&)BASE_CLASS::GetPrev(rPosition); }
	TYPE GetPrev(VCPOSITION& rPosition) const
		{ return (TYPE)BASE_CLASS::GetPrev(rPosition); }

	// getting/modifying an element at a given position
	TYPE& GetAt(VCPOSITION position)
		{ return (TYPE&)BASE_CLASS::GetAt(position); }
	TYPE GetAt(VCPOSITION position) const
		{ return (TYPE)BASE_CLASS::GetAt(position); }
	void SetAt(VCPOSITION pos, TYPE newElement)
		{ BASE_CLASS::SetAt(pos, newElement); }
};