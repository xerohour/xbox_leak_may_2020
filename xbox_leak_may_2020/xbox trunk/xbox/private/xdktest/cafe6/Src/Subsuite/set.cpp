/////////////////////////////////////////////////////////////////////////////
// set.cpp
//
// email	date		change
// briancr	08/02/95	created
//
// copyright 1995 Microsoft

// Implementation of the CSet class

#include "stdafx.h"
#include "set.h"

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////////////////
// CSet class

template <class T> CSet<T>::CSet()
: m_pSet(NULL)
{
	m_pSet = new SetArray;
}

template <class T> CSet<T>::CSet(const T item)
: m_pSet(NULL)
{
	m_pSet = new SetArray;
	Add(item);
}

template <class T> CSet<T>::CSet(const CSet<T>& set)
: m_pSet(NULL)
{
	m_pSet = new SetArray;
	Copy(set);
}

template <class T> CSet<T>::~CSet()
{
	// delete the array
	// if the user wants items in the array
	// to be deleted, he/she should write an
	// appropriate DestructElements function
	RemoveAll();
	delete m_pSet;
}

template <class T> void CSet<T>::Add(const T item)
{
#pragma warning (disable: 4239)
	// a set cannot have duplicates, so only add the item, if
	// it's not already in the set
	if (!Contains(item)) {
		m_pSet->Add((T)item);
	}
#pragma warning (default: 4239)
}

template <class T> void CSet<T>::Add(const CSet<T>& set)
{
	// a set cannot have duplicates, so add each item from the
	// given set individually (Add(T) checks for duplicates
	for (POSITION pos = set.GetStartPosition(); pos != NULL; ) {
		T item = set.GetNext(pos);
		Add(item);
	}
}

template <class T> void CSet<T>::Copy(const CSet<T>& set)
{
	// clear out this set
	RemoveAll();
	// add the given set to this set
	Add(set);
}

template <class T> void CSet<T>::RemoveAll(void)
{
	// just remove all items from the array
	m_pSet->RemoveAll();
}

template <class T> POSITION CSet<T>::GetStartPosition(void) const
{
	// if the set is empty, we can't iterate
	if (GetSize() <= 0) {
		return NULL;
	}
	return (POSITION)1;
}

template <class T> T CSet<T>::GetNext(POSITION& pos) const
{
	// the position cannot be NULL
	ASSERT(pos);

	// store the current item
	T item = (*m_pSet)[(int)pos - 1];

	// increment and if the index is no longer in range, set pos to NULL
	pos = (POSITION)((int)pos + 1);
	if ((int)pos > GetSize()) {
		pos = NULL;
	}
	return item;
}

template <class T> BOOL CSet<T>::Contains(const T& item) const
{
	// look for the item in the set
	for (POSITION pos = GetStartPosition(); pos != NULL; ) {
		T setitem = GetNext(pos);
		if (setitem == item) {
			return TRUE;
		}
	}
	return FALSE;
}

template <class T> BOOL CSet<T>::IsEmpty(void) const
{
	return (GetSize() <= 0);
}

template <class T> CSet<T>& CSet<T>::operator=(const CSet<T>& set)
{
	// copy the set
	Copy(set);

	return *this;
}

template <class T> BOOL CSet<T>::operator==(const CSet<T>& set)
{
	// two sets are equal if they have the same number of items
	// and all the items are in both sets
	if (GetSize() != set.GetSize()) {
		return FALSE;
	}
	for (POSITION pos = GetStartPosition(); pos != NULL; ) {
		T item = GetNext(pos);
		if (!set.Contains(item)) {
			return FALSE;
		}
	}
	return TRUE;
}

template <class T> BOOL CSet<T>::operator!=(const CSet<T>& set)
{
	return !(operator==(set));
}

template <class T> CSet<T> CSet<T>::operator+(const CSet<T>& set)
{
	CSet<T> result;
	// the union of both sets may not be a valid set,
	// i.e., there may be duplicate items
	// we can simply copy the first set, since it's valid
	result.Copy(*this);
	// and add in the second set, which will check for duplicates
	result.Add(set);

	return result;
}

template <class T> int CSet<T>::GetSize(void) const
{
	return m_pSet->GetSize();
}

// specialize for CVertex
#include "vertex.h"

template CSet<CVertex>;
