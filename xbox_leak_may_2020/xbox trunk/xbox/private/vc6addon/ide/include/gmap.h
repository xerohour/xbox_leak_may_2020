#ifndef INCLUDED_GMAP_H
#define INCLUDED_GMAP_H

#include <afx.h> // CObject
#include <afxtempl.h> // CMap
#include <limits.h> // INT_MAX

/* these are typedef's, not types.

	// GrowingMaps (aka Dictionaries)
	class CGrowingMapWordToOb;         // map from WORD to CObject*
	class CGrowingMapWordToPtr;        // map from WORD to void*
	class CGrowingMapPtrToWord;        // map from void* to WORD
	class CGrowingMapPtrToPtr;         // map from void* to void*

	// Special String variants
	class CGrowingMapStringToPtr;      // map from CString to void*
	class CGrowingMapStringToOb;       // map from CString to CObject*
	class CGrowingMapStringToString;   // map from CString to CString

*/

class CGrowingMapNonTemplate
{
public:
/*inline just to live in header*/
	inline static unsigned PickSize(int nElements);
};

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CGrowingMap :
	public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>,
	public CGrowingMapNonTemplate
{
private:
	typedef CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> Inherited;
protected:
	typedef KEY BASE_KEY; // for CTypedPtrMap
	typedef ARG_KEY BASE_ARG_KEY; // for CTypedPtrMap
	typedef VALUE BASE_VALUE; // for CTypedPtrMap
	typedef ARG_VALUE BASE_ARG_VALUE; // for CTypedPtrMap
public:
	CGrowingMap();
	CGrowingMap(int nBlockSize);
	void Rehash(int nBuckets);
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);
	VALUE& operator[](ARG_KEY key); // override
	void SetAt(ARG_KEY key, ARG_VALUE newValue)
	{
		(*this)[key] = newValue;
	}
	void GrowMaxLoad(float);
	// rehash when GetCount() > when GetHashTableSize() * growMaxLoad
	// presently assumed > 1

	void GrowMinLoad(float);
	// rehash so that GetCount() ~ when GetHashTableSize() * growMinLoad
	// but don't actually shrink to meet it.
	// presently assumed ~ 1 by PickSize

	void GrowDoubling()
	{ // not good with multiplicative hash functions that favor prime hash sizes
		ASSERT(!"NYI");
	}
	void GrowInSteps()
	{ // sort of assumed (vague meaning, pretty much tied to PickSize)
	}
	void GrowPreferPrimes(BOOL)
	{ // assumed
		ASSERT(!"NYI");
	}

	void Lock();
	void Unlock();
	int GetLock() const;
protected:
	int m_nLock;
	unsigned m_nRehashThreshold; // nBuckets * m_fMaxGrowLoad
	float m_fMaxGrowLoad; // might not need to keep this around, presently assumed > 1
	float m_fMinGrowLoad; // presently assumed == 1
};

// These typedefs probably cause actual repeated compile time instantiation followed
// by link time dead stripping. They should probably be removed, or changed
// back to declarations with one time instantiation in a .cpp.

//typedef
//	CGrowingMap<CMapWordToOb, WORD, WORD, CObject*, CObject*>
//	CGrowingMapWordToOb;
typedef
	CGrowingMap<WORD, WORD, void*, void*>
	CGrowingMapWordToPtr;
typedef
	CGrowingMap<void*, void*, WORD, WORD>
	CGrowingMapPtrToWord;
typedef
	CGrowingMap<void*, void*, void*, void*>
	CGrowingMapPtrToPtr;
typedef
	CGrowingMap<CString, LPCTSTR, void*, void*>
	CGrowingMapStringToPtr;
//typedef
//	CGrowingMap<CString, LPCTSTR, CObject*, CObject*>
//	CGrowingMapStringToOb;
typedef
	CGrowingMap<CString, LPCTSTR, CString, LPCTSTR>
	CGrowingMapStringToString;

// REVIEW, why does MFC use LPCTSTR instead of const CString&?
// Just to avoid CString construction and destruction?

// constructors
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CGrowingMap(int nBlockSize)
	: Inherited(nBlockSize)
{
	ASSERT(nBlockSize > 0);
	m_nLock = 0;
	GrowInSteps();
	GrowMaxLoad(2);
	GrowMinLoad(1);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CGrowingMap()
{
	m_nLock = 0;
	GrowInSteps();
	GrowMaxLoad(2);
	GrowMinLoad(1);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
// This rehashes
	ASSERT_VALID(this);
//	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (GetCount() == 0) {
		Inherited::InitHashTable(nHashSize, bAllocNow);
	} else { // rehash
		Rehash(nHashSize);
	}
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Rehash(int nBuckets)
{
// new "relinking" version
#ifdef _DEBUG
#ifdef _IOSTREAM_
	// output to console if it's being used
	std::cerr
			<< "std::cerr: "
			<< "rehashing "
			<< GetCount()
			<< " elements from "
			<< m_nHashTableSize 
			<< " buckets to "
			<< nBuckets
			<< " buckets.\n";
#endif
	// output to debugger
	afxDump
			<< "afxDump: "
			<< "rehashing "
			<< GetCount()
			<< " elements from "
			<< m_nHashTableSize 
			<< " buckets to "
			<< nBuckets
			<< " buckets.\n";
#endif
	if (m_pHashTable == NULL || GetCount() == 0)
	{
		return;
	}
	ASSERT(nBuckets > 0);

	// first, stash pointers to all Assocs, based on RemoveAll
	CAssoc** aryAssocs = new CAssoc* [GetCount()];
	CAssoc** iter = aryAssocs;
	UINT nHash;
	for (nHash = 0; nHash < m_nHashTableSize; nHash++)
	{
		CAssoc* pAssoc;
		for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
		{
			*iter++ = pAssoc;
		}
	}
	ASSERT(iter == aryAssocs + GetCount());

	m_nHashTableSize = nBuckets;
	// now rehash and relink in new m_pHashTable
	delete [] m_pHashTable;
	m_pHashTable = new CAssoc* [m_nHashTableSize];
	memset(m_pHashTable, 0, sizeof(CAssoc*) * m_nHashTableSize);
	UINT n;
	for (n = 0 ; n < GetCount() ; n++)
	{
		// compute hash
		UINT nHash = HashKey<ARG_KEY>(aryAssocs[n]->key) % m_nHashTableSize;
		aryAssocs[n]->nHashValue = nHash; // cache for iteration
// map from WORD, DWORD, void*, CObject* cheaply rehash instead during interation
// instead of caching the hash. So far, CGrowingMap loses here on size (double!).

		// put into hash table
		aryAssocs[n]->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = aryAssocs[n];
	}
	delete[] aryAssocs;
	m_nRehashThreshold = GetHashTableSize() * m_fMaxGrowLoad;
}


template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	ASSERT_VALID(this);
	const int sizeBefore = GetCount();
	VALUE& resultIfWeDontRehash = Inherited::operator[](key);
	const int sizeAfter = GetCount();
	ASSERT(sizeBefore == sizeAfter || sizeBefore+1 == sizeAfter);
// OPTIMIZE
// If we dig into CMap, we can optimize the check for growth.
	if (sizeBefore != sizeAfter && sizeAfter > m_nRehashThreshold && m_nLock == 0)
	{
		int next_number_of_buckets;
// FUTURE : support more sizing strategies
//		if (m_bGrowDoubling)
//		{
//			ASSERT(!m_bPreferPrimes);
//			ASSERT(m_bGrowInSteps);
		//			next_size = nBlockSize+nBlockSize;
//		}
//		if (m_bGrowInSteps)
//		{
			next_number_of_buckets = PickSize(GetCount());
			if (next_number_of_buckets <= GetHashTableSize())
			{
				// primes table has been maxed out, either pin the size
				// or double it. Currently we just pin it.
				m_nRehashThreshold = INT_MAX; // no more rehashing.
				return resultIfWeDontRehash;
			}
			else
			{
				Rehash(next_number_of_buckets);
			}
//		}
	}
	// we can return the nonRehash result because Rehash is now in place.
	return resultIfWeDontRehash;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GrowMaxLoad(float load)
{
	m_fMaxGrowLoad = load;
	m_nRehashThreshold = GetHashTableSize() * m_fMaxGrowLoad;
	ASSERT(load > 1 || !"NYI");
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GrowMinLoad(float load)
{
	ASSERT(load == 1 || !"NYI");
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lock()
{
	m_nLock++;
	ASSERT(m_nLock >= 0); // leak
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Unlock()
{
	m_nLock--;
	ASSERT(m_nLock >= 0); // underflow.
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
int CGrowingMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetLock() const
{
	return m_nLock;
}

/*static*/ inline /*inline just to live in header*/
unsigned CGrowingMapNonTemplate::PickSize(int nElements)
{
	unsigned x = nElements;
	static const unsigned primes[] = { 47, 251, 509, 1021, 1531, 2039, 2557, 3067, 3583, 4093, 4603, 5119 };
	const unsigned MAX_PRIME_INDEX = sizeof(primes)/sizeof(primes[0]) - 1;
// TODO have this pay attention to m_nMinLoad
	if (x < 50)
		x = 0;
	else if (x < 256)
		x = 1;
	else
	{
		x = (x / 512) + 2;
		if (x > MAX_PRIME_INDEX)
			x = MAX_PRIME_INDEX;
	}
	return primes[x];
}

/////////////////////////////////////////////////////////////////////////////
// CTypedPtrMap<BASE_CLASS, KEY, VALUE>

// Some changes compared to CTypedPtrMap
// extra two ARG_ parameters allow CString's to work, cast through LPCTSTR

template<class BASE_CLASS, class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CTypedPtrGrowingMap :
	public BASE_CLASS
{
public:

// Construction
	CTypedPtrGrowingMap(int nBlockSize = 10)
		: BASE_CLASS(nBlockSize)
	{
	}

	// Lookup
	BOOL Lookup(ARG_KEY key, VALUE& rValue) const
	{
		return BASE_CLASS::Lookup(key, (BASE_CLASS::BASE_VALUE&)rValue);
	}

	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key)
	{
		return (VALUE&)BASE_CLASS::operator[](key);
	}

	// add a new key (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue)
	{
		BASE_CLASS::SetAt(key, newValue);
	}
// REVIEW might need cast newValue BASE_CLASS::BASE_ARG_VALUE for CStrings
// Nope, CANNOT map to CString as if it is a CObject* or void*

	// removing existing (key, ?) pair
	BOOL RemoveKey(ARG_KEY key)
	{
		return BASE_CLASS::RemoveKey(key);
	}

	// iteration
	void GetNextAssoc(POSITION& rPosition, KEY& rKey, VALUE& rValue) const
	{
		BASE_CLASS::GetNextAssoc(rPosition, (BASE_CLASS::BASE_KEY&)rKey,
			(BASE_CLASS::BASE_VALUE&)rValue);
	}
};

typedef CTypedPtrGrowingMap
<
	CGrowingMapStringToPtr,//CGrowingMap<CString, LPCTSTR, void*, void*>,
	CString, LPCTSTR,
	CObject*, CObject*
>
CGrowingMapStringToOb;

typedef CTypedPtrGrowingMap
<
	CGrowingMapWordToPtr,//CGrowingMap<WORD, WORD, void*, void*>,
	WORD, WORD,
	CObject*, CObject*
>
CGrowingMapWordToOb;

// TODO specialize some maps to not waste 4 bytes to cache a cheaply computed
// hash and recompute it while iterating, like MFC does.

// TODO reintroduce the optimizations of gmap3 and gmap4 that preinstantiated
// common maps in one place and inhibited instantiation otherwise.

#endif // the whole file
