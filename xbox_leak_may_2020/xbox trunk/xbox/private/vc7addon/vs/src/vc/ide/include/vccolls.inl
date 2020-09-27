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

// Inlines for VCCOLLS.H

#if _MSC_VER > 1000
#pragma once
#endif


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCStringArray::GetSize() const
	{ return m_nSize; }
inline INT_PTR CVCStringArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CVCStringArray::RemoveAll()
	{ SetSize(0); }
inline CVCString CVCStringArray::GetAt(INT_PTR nIndex) const
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline void CVCStringArray::SetAt(INT_PTR nIndex, LPCTSTR newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline void CVCStringArray::SetAt(INT_PTR nIndex, const CVCString& newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline CVCString& CVCStringArray::ElementAt(INT_PTR nIndex)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline const CVCString* CVCStringArray::GetData() const
	{ return (const CVCString*)m_pData; }
inline CVCString* CVCStringArray::GetData()
	{ return (CVCString*)m_pData; }
inline INT_PTR CVCStringArray::Add(LPCTSTR newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline INT_PTR CVCStringArray::Add(const CVCString& newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline CVCString CVCStringArray::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
inline CVCString& CVCStringArray::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCStringList::GetCount() const
	{ return m_nCount; }
inline BOOL CVCStringList::IsEmpty() const
	{ return m_nCount == 0; }
inline CVCString& CVCStringList::GetHead()
	{ VSASSERT(m_pNodeHead != NULL, "GetHead called on empty CVCStringList!");
		return m_pNodeHead->data; }
inline CVCString CVCStringList::GetHead() const
	{ VSASSERT(m_pNodeHead != NULL, "GetHead called on empty CVCStringList!");
		return m_pNodeHead->data; }
inline CVCString& CVCStringList::GetTail()
	{ VSASSERT(m_pNodeTail != NULL, "GetTail called on empty CVCStringList!");
		return m_pNodeTail->data; }
inline CVCString CVCStringList::GetTail() const
	{ VSASSERT(m_pNodeTail != NULL, "GetTail called on empty CVCStringList!");
		return m_pNodeTail->data; }
inline VCPOSITION CVCStringList::GetHeadPosition() const
	{ return (VCPOSITION) m_pNodeHead; }
inline VCPOSITION CVCStringList::GetTailPosition() const
	{ return (VCPOSITION) m_pNodeTail; }
inline CVCString& CVCStringList::GetNext(VCPOSITION& rPosition) // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline CVCString CVCStringList::GetNext(VCPOSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline CVCString& CVCStringList::GetPrev(VCPOSITION& rPosition) // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline CVCString CVCStringList::GetPrev(VCPOSITION& rPosition) const // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline CVCString& CVCStringList::GetAt(VCPOSITION position)
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		return pNode->data; }
inline CVCString CVCStringList::GetAt(VCPOSITION position) const
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		return pNode->data; }
inline void CVCStringList::SetAt(VCPOSITION pos, LPCTSTR newElement)
	{ CNode* pNode = (CNode*) pos;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		pNode->data = newElement; }

inline void CVCStringList::SetAt(VCPOSITION pos, const CVCString& newElement)
	{ CNode* pNode = (CNode*) pos;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		pNode->data = newElement; }


////////////////////////////////////////////////////////////////////////////
inline INT_PTR CVCMapStringToPtr::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapStringToPtr::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapStringToPtr::SetAt(LPCTSTR key, void* newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapStringToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : VCBEFORE_START_POSITION; }
inline UINT CVCMapStringToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }


////////////////////////////////////////////////////////////////////////////
inline INT_PTR CVCMapStringToString::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapStringToString::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapStringToString::SetAt(LPCTSTR key, LPCTSTR newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapStringToString::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : VCBEFORE_START_POSITION; }
inline UINT CVCMapStringToString::GetHashTableSize() const
	{ return m_nHashTableSize; }

////////////////////////////////////////////////////////////////////////////
inline INT_PTR CVCMapStringWToStringW::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapStringWToStringW::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapStringWToStringW::SetAt(LPCOLESTR key, LPCOLESTR newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapStringWToStringW::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : VCBEFORE_START_POSITION; }
inline UINT CVCMapStringWToStringW::GetHashTableSize() const
	{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCStringWArray::GetSize() const
	{ return m_nSize; }
inline INT_PTR CVCStringWArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CVCStringWArray::RemoveAll()
	{ SetSize(0); }
inline CStringW CVCStringWArray::GetAt(INT_PTR nIndex) const
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline void CVCStringWArray::SetAt(INT_PTR nIndex, LPCOLESTR newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline void CVCStringWArray::SetAt(INT_PTR nIndex, const CStringW& newElement)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		m_pData[nIndex] = newElement; }

inline CStringW& CVCStringWArray::ElementAt(INT_PTR nIndex)
	{ VSASSERT(nIndex >= 0 && nIndex < m_nSize, "Index out of range!");
		return m_pData[nIndex]; }
inline const CStringW* CVCStringWArray::GetData() const
	{ return (const CStringW*)m_pData; }
inline CStringW* CVCStringWArray::GetData()
	{ return (CStringW*)m_pData; }
inline INT_PTR CVCStringWArray::Add(LPCOLESTR newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline INT_PTR CVCStringWArray::Add(const CStringW& newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline CStringW CVCStringWArray::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
inline CStringW& CVCStringWArray::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }

/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
inline INT_PTR CVCMapStringWToPtr::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapStringWToPtr::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapStringWToPtr::SetAt(LPCOLESTR key, void* newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapStringWToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : VCBEFORE_START_POSITION; }
inline UINT CVCMapStringWToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

inline INT_PTR CVCStringWList::GetCount() const
	{ return m_nCount; }
inline BOOL CVCStringWList::IsEmpty() const
	{ return m_nCount == 0; }
inline CStringW& CVCStringWList::GetHead()
	{ VSASSERT(m_pNodeHead != NULL, "GetHead called on empty CVCStringWList!");
		return m_pNodeHead->data; }
inline CStringW CVCStringWList::GetHead() const
	{ VSASSERT(m_pNodeHead != NULL, "GetHead called on empty CVCStringWList!");
		return m_pNodeHead->data; }
inline CStringW& CVCStringWList::GetTail()
	{ VSASSERT(m_pNodeTail != NULL, "GetTail called on empty CVCStringWList!");
		return m_pNodeTail->data; }
inline CStringW CVCStringWList::GetTail() const
	{ VSASSERT(m_pNodeTail != NULL, "GetTail called on empty CVCStringWList!");
		return m_pNodeTail->data; }
inline VCPOSITION CVCStringWList::GetHeadPosition() const
	{ return (VCPOSITION) m_pNodeHead; }
inline VCPOSITION CVCStringWList::GetTailPosition() const
	{ return (VCPOSITION) m_pNodeTail; }
inline CStringW& CVCStringWList::GetNext(VCPOSITION& rPosition) // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline CStringW CVCStringWList::GetNext(VCPOSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline CStringW& CVCStringWList::GetPrev(VCPOSITION& rPosition) // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline CStringW CVCStringWList::GetPrev(VCPOSITION& rPosition) const // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline CStringW& CVCStringWList::GetAt(VCPOSITION position)
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		return pNode->data; }
inline CStringW CVCStringWList::GetAt(VCPOSITION position) const
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		return pNode->data; }
inline void CVCStringWList::SetAt(VCPOSITION pos, LPCOLESTR newElement)
	{ CNode* pNode = (CNode*) pos;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		pNode->data = newElement; }

inline void CVCStringWList::SetAt(VCPOSITION pos, const CStringW& newElement)
	{ CNode* pNode = (CNode*) pos;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node found in list!");
		pNode->data = newElement; }

