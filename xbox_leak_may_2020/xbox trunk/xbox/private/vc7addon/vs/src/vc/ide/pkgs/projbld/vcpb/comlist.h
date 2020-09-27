// CComDynamicListTyped
// Implements an ordered list of CComPtr's
#ifndef _COMLIST_H_
#define _COMLIST_H_

// change IUnknown's to CComPtr's, or do the addref/release....
const long _DEFAULT_DYNLIST_LENGTH = 2;

template <class StoredType>
class CComDynamicListTyped
{
public:
	CComDynamicListTyped()
	: m_bAnItemHasBeenRemoved(false), m_nSize(0), m_ppUnk(NULL)
	{
	}

	~CComDynamicListTyped()
	{
		Clear();
	}
	long Add(StoredType* pUnk);
	long AddHead(StoredType* pUnk);
	long AddTail(StoredType* pUnk);
	BOOL RemoveCookie(INT_PTR dwCookie, bool bRelease = true);
	BOOL RemoveAtIndex(long nItem);
	BOOL Remove(StoredType* pUnk);
	void RemoveAll();
	long Find(StoredType* pUnk);
	static long WINAPI GetCookie(StoredType** pp)
	{
		return (long)*pp;
	}
	static StoredType* WINAPI GetUnknown(long dwCookie)
	{
		return (StoredType*)dwCookie;
	}
	StoredType* GetAt(long nIndex)
	{
		if (nIndex < 0 || nIndex >= m_nSize || m_ppUnk == NULL)
			return NULL;

		return m_ppUnk[nIndex];
	}
	long GetSize() const
	{
		return m_nSize;
	}
	long GetUpperBound()
	{
		if (m_nSize == 0 || m_ppUnk == NULL)
			return 0;
		
		long cItems = m_nSize;
		for (long idx = m_nSize-1; idx >= 0 ;idx--)
		{
			StoredType* pItem = m_ppUnk[idx];
			if (!pItem)
				cItems--;
		}
		return cItems;
	}
	long GetCount()
	{
		if (m_nSize == 0 || m_ppUnk == NULL)
			return 0;

		long cItems = 0;
		for (long idx = 0; idx < m_nSize; idx++)
		{
			StoredType* pItem = m_ppUnk[idx];
			if (pItem)
				cItems++;
		}
		return cItems;
	}

	void Concatenate(CComDynamicListTyped *pList)
	{
		if (pList == NULL || pList->m_nSize == 0)
			return;

		for (long idx = 0; idx < pList->GetSize(); idx++)
		{
			StoredType* pItem = pList->GetAt(idx);
			if (pItem)
				Add(pItem);
		}
	}

	void Clear()
	{
		RemoveAll();
		if (m_nSize > 0)
			free(m_ppUnk);
		m_nSize = 0;
		m_bAnItemHasBeenRemoved = false;
	}

	StoredType** GetArray()
	{
		return m_ppUnk;
	}

	bool HasAnItemBeenRemoved(void)
	{
		return m_bAnItemHasBeenRemoved;
	}

protected:
	BOOL AddFront(StoredType* pUnk);
	BOOL AddBack(StoredType* pUnk);

protected:
	StoredType** m_ppUnk;
	long m_nSize;
	bool m_bAnItemHasBeenRemoved;
};

template <class StoredType>
inline void CComDynamicListTyped<StoredType>::RemoveAll()
{
	if (m_nSize == 0 || m_ppUnk == NULL)
		return;

	for (long idx = 0; idx < m_nSize; idx++)
	{
		StoredType* pItem = m_ppUnk[idx];
		if (pItem)
		{
			pItem->Release();
			m_ppUnk[idx] = NULL;
		}
	}

	m_nSize = 0;
	m_bAnItemHasBeenRemoved = false;
}

template <class StoredType>
inline long CComDynamicListTyped<StoredType>::Add(StoredType* pUnk)
{ // WARNING: HAS EARLY RETURNS
	VSASSERT(pUnk, "Trying to add a NULL pointer to pointer list.  Bad program, bad program.");
	if (pUnk == NULL)
		return -1;

	long lItem = Find(pUnk);
	if (lItem >= 0) // Don't add if it is already there!
		return lItem; // WARNING: EARLY RETURN (not from IRS)

	StoredType** pp;
	if (m_nSize == 0) // no connections
	{
		//create array
		pp = (StoredType**)malloc(sizeof(StoredType*)*_DEFAULT_DYNLIST_LENGTH);
		if (pp == NULL)
			return -1;
		memset(pp, 0, sizeof(StoredType*)*_DEFAULT_DYNLIST_LENGTH);
		m_ppUnk = pp;
		m_nSize = _DEFAULT_DYNLIST_LENGTH;
	}

	if (m_ppUnk == NULL)
		return -1;			// something must be very, very wrong to hit this...

	for (long idx = 0; idx < m_nSize; idx++)
	{
		StoredType* pItem = m_ppUnk[idx];
		if (pItem == NULL)
		{
			m_ppUnk[idx] = pUnk;
			pUnk->AddRef();
			return idx;  // WARNING: EARLY RETURN (not from IRS)
		}
	}

	long nAlloc = m_nSize*2;
	long nSizeT = m_nSize;
	pp = (StoredType**)realloc(m_ppUnk, sizeof(StoredType*)*nAlloc);
	if (pp == NULL)
		return -1;  // WARNING: EARLY RETURN (not from IRS)
	m_ppUnk = pp;
	memset(&m_ppUnk[m_nSize], 0, sizeof(StoredType*)*m_nSize);
	m_ppUnk[nSizeT] = pUnk;
	pUnk->AddRef();
	m_nSize = nAlloc;
	return (long)nSizeT; // WARNING: HAS EARLY RETURNS
}

template <class StoredType>
inline long CComDynamicListTyped<StoredType>::AddHead(StoredType* pUnk)
{ // WARNING: HAS EARLY RETURNS
	VSASSERT(pUnk, "Trying to add a NULL pointer to pointer list.  Bad program, bad program.");
	if (pUnk == NULL)
		return -1;

	long lItem = Find(pUnk);
	if (lItem >= 0) // Don't add if it is already there!
		return lItem; // WARNING: EARLY RETURN (not from IRS)

	StoredType** pp = NULL;
	if (m_nSize == 0) // no connections
	{
		//create array
		pp = (StoredType**)malloc(sizeof(StoredType*)*_DEFAULT_DYNLIST_LENGTH);
		if (pp == NULL)
			return -1;
		memset(pp, 0, sizeof(StoredType*)*_DEFAULT_DYNLIST_LENGTH);
		m_ppUnk = pp;
		m_nSize = _DEFAULT_DYNLIST_LENGTH;
	}
	if (m_ppUnk == NULL)	// something very, very wrong if this is true...
		return -1;

	// only allowed to add to front of list
	if (m_ppUnk[0] == NULL)
	{
		m_ppUnk[0] = pUnk;
		pUnk->AddRef();
		return (long)0;  // WARNING: EARLY RETURN (not from IRS)
	}

	if (AddFront(pUnk))
		return (long)0;  // WARNING: EARLY RETURN (not from IRS)

	// no empty spot, so need to realloc
	long nAlloc = m_nSize*2;
	long nSizeT = m_nSize;
	pp = (StoredType**)realloc(m_ppUnk, sizeof(StoredType*)*nAlloc);
	if (pp == NULL)
		return -1;  // WARNING: EARLY RETURN (not from IRS)
	m_ppUnk = pp;
	memset(&m_ppUnk[m_nSize], 0, sizeof(StoredType*)*m_nSize);
	m_nSize = nAlloc;

	AddFront(pUnk);	// should be room now
	return 0; // WARNING: HAS EARLY RETURNS
}


template <class StoredType>
inline BOOL CComDynamicListTyped<StoredType>::AddFront(StoredType* pUnk)
{	// is there a blank spot we can compress everything up by?
	if (m_ppUnk == NULL)
		return FALSE;		// something is very, very wrong...

	long idxHole;
	for (idxHole = 0; idxHole < m_nSize && m_ppUnk[idxHole] != NULL; idxHole++)
	{}	// just finding that hole

	if (idxHole < m_nSize && m_ppUnk[idxHole] == NULL)
	{
		for (long idx2 = idxHole-1; idx2 >= 0; idx2--)
		{
			// move everything up one
			m_ppUnk[idx2+1] = m_ppUnk[idx2];
			m_ppUnk[idx2] = NULL;
		}
		m_ppUnk[0] = pUnk;
		pUnk->AddRef();
		return TRUE;  // WARNING: EARLY RETURN (not from IRS)
	}

	return FALSE;
}

template <class StoredType>
inline long CComDynamicListTyped<StoredType>::AddTail(StoredType* pUnk)
{ // WARNING: HAS EARLY RETURNS
	VSASSERT(pUnk, "Trying to add a NULL pointer to pointer list.  Bad program, bad program.");

	long lItem;
	
	lItem = Find(pUnk);
	if (lItem >= 0) // Don't add if it is already there!
	{
		return lItem; // WARNING: EARLY RETURN (not from IRS)
	}

	StoredType** pp = NULL;
	if (m_nSize == 0) // no connections
	{
		//create array
		pp = (StoredType**)malloc(sizeof(StoredType*)*_DEFAULT_DYNLIST_LENGTH);
		if (pp == NULL)
			return -1;
		memset(pp, 0, sizeof(StoredType*)*_DEFAULT_DYNLIST_LENGTH);
		m_ppUnk = pp;
	
		m_nSize = _DEFAULT_DYNLIST_LENGTH;
	}

	if (m_ppUnk == NULL)
		return -1;		// something is very, very wrong...

	// only allowed to add to back of list
	long idx = (m_nSize == 0) ? 0 : m_nSize-1;
	if (m_ppUnk[idx] == NULL)
	{
		m_ppUnk[idx] = pUnk;
		pUnk->AddRef();
		return idx;  // WARNING: EARLY RETURN (not from IRS)
	}

	if (AddBack(pUnk))
		return Find(pUnk);  // WARNING: EARLY RETURN (not from IRS)

	// no empty spot, so need to realloc
	long nAlloc = m_nSize*2;
	long nSizeT = m_nSize;
	pp = (StoredType**)realloc(m_ppUnk, sizeof(StoredType*)*nAlloc);
	if (pp == NULL)
		return -1;  // WARNING: EARLY RETURN (not from IRS)
	m_ppUnk = pp;
	memset(&m_ppUnk[m_nSize], 0, sizeof(StoredType*)*m_nSize);
	m_ppUnk[nSizeT] = pUnk;
	pUnk->AddRef();
	m_nSize = nAlloc;
	return (long)nSizeT; // WARNING: HAS EARLY RETURNS
}


template <class StoredType>
inline BOOL CComDynamicListTyped<StoredType>::AddBack(StoredType* pUnk)
{	// is there a blank spot we can compress everything down by?
	if (m_ppUnk == NULL)
		return FALSE;	// something is very, very wrong...

	long idxHole;
	for (idxHole = m_nSize-1; idxHole >= 0 && m_ppUnk[idxHole] != NULL; idxHole--)
	{}		// just looking for that hole
	if (idxHole >= 0 && m_ppUnk[idxHole] == NULL)
	{
		for (long idx2 = idxHole+1; idx2 < m_nSize; idx2++)
		{
			// move everything down one
			m_ppUnk[idx2-1] = m_ppUnk[idx2];
			m_ppUnk[idx2] = NULL;
		}
		m_ppUnk[m_nSize-1] = pUnk;
		pUnk->AddRef();
		return TRUE;  // WARNING: EARLY RETURN (not from IRS)
	}

	return FALSE;
}

template <class StoredType>
inline BOOL CComDynamicListTyped<StoredType>::RemoveCookie(INT_PTR dwCookie, bool bRelease /* = true */)
{
	if (dwCookie == NULL || m_nSize == 0 || m_ppUnk == NULL)
		return FALSE;

	for (long idx = 0; idx < m_nSize; idx++)
	{
		StoredType* pItem = m_ppUnk[idx];
		if ((INT_PTR)pItem == dwCookie)
		{
			if (bRelease)
				pUnk->Release();
			m_ppUnk[idx] = NULL;
			return TRUE;
		}
	}
	return FALSE;
}

template <class StoredType>
inline BOOL CComDynamicListTyped<StoredType>::RemoveAtIndex(long nIndex)
{
	if (nIndex < 0 || nIndex >= m_nSize || m_nSize == 0 || m_ppUnk == NULL)
		return FALSE;

	StoredType *pUnk = m_ppUnk[nIndex];
	if (pUnk)
	{
		pUnk->Release();
		m_ppUnk[nIndex] = NULL;
		m_bAnItemHasBeenRemoved = true;
		return TRUE;
	}

	return FALSE;
}

template <class StoredType>
inline BOOL CComDynamicListTyped<StoredType>::Remove(StoredType* pUnk)
{
	if (pUnk == NULL || m_nSize == 0 || m_ppUnk == NULL)
		return FALSE;
	for (long idx = 0; idx < m_nSize; idx++)
	{
		StoredType* pItem = m_ppUnk[idx];
		if (pItem == pUnk)
		{
			pItem->Release();
			m_ppUnk[idx] = NULL;
			m_bAnItemHasBeenRemoved = true;
			return TRUE;
		}
	}
	return FALSE;
}

template <class StoredType>
inline long CComDynamicListTyped<StoredType>::Find(StoredType* pUnk)
{
	if (pUnk == NULL || m_nSize == 0 || m_ppUnk == NULL)
		return -1;

	for (long idx = 0; idx < m_nSize; idx++)
	{
		StoredType* pItem = m_ppUnk[idx];
		if (pItem == pUnk)
			return idx;
	}

	return -1;
}

class CComDynamicList : public CComDynamicListTyped<IUnknown>
{
};

#endif
