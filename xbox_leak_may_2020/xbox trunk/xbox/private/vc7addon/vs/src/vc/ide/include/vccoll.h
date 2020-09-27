// (KPerry) This was the MFC collection stuff
// Now it has been purged of all other MFC influences and is a stand alone entity.

#pragma once
#include "plex.h"

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

	// Arrays
	class CVCByteArray;           // array of BYTE
	class CVCWordArray;           // array of WORD
	class CVCDWordArray;          // array of DWORD
	class CVCUIntArray;           // array of UINT
	class CVCPtrArray;            // array of void*
	// template CVCTypedPtrArray

	// Lists
	class CVCPtrList;             // list of void*

	// Maps (aka Dictionaries)
	class CVCMapWordToPtr;        // map from WORD to void*
	class CVCMapDWordToPtr;		  // map from DWORD to void*
	class CVCMapPtrToWord;        // map from void* to WORD
	class CVCMapPtrToPtr;         // map from void* to void*


typedef void *VCPOSITION;
#define BEFORE_START_VCPOSITION ((VCPOSITION)-1L)

// VCIsValidAddress() returns TRUE if the passed parameter points
// to at least nBytes of accessible memory. If bReadWrite is TRUE,
// the memory must be writeable; if bReadWrite is FALSE, the memory
// may be const.
BOOL __stdcall VCIsValidAddress(const void* lp, SIZE_T nBytes, BOOL bReadWrite = TRUE);
// VCIsValidAddress() returns TRUE if the passed parameter points
// to at least nBytes of accessible memory. If bReadWrite is TRUE,
// the memory must be writeable; if bReadWrite is FALSE, the memory
// may be const.
BOOL inline __stdcall VCIsValidAddress(const void* lp, SIZE_T nBytes,
	BOOL bReadWrite /* = TRUE */)
{
	// simple version using Win-32 APIs for pointer validation.
	return (lp != NULL && !IsBadReadPtr(lp, nBytes) &&
		(!bReadWrite || !IsBadWritePtr((LPVOID)lp, nBytes)));
}

#ifndef VCVERIFY
#ifdef _DEBUG
#define VCVERIFY(x) _ASSERTE(x)
#else
#define VCVERIFY(x) (void)(x)
#endif	// _DEBUG
#endif	// VCVERIFY

////////////////////////////////////////////////////////////////////////////

class CVCByteArray
{

public:

// Construction
	CVCByteArray();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	BYTE GetAt(INT_PTR nIndex) const;
	void SetAt(INT_PTR nIndex, BYTE newElement);

	BYTE& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const BYTE* GetData() const;
	BYTE* GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, BYTE newElement);

	INT_PTR Add(BYTE newElement);

	INT_PTR Append(const CVCByteArray& src);
	void Copy(const CVCByteArray& src);

	// overloaded operator helpers
	BYTE operator[](INT_PTR nIndex) const;
	BYTE& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, BYTE newElement, INT_PTR nCount = 1);

	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void InsertAt(INT_PTR nStartIndex, CVCByteArray* pNewArray);

// Implementation
protected:
	BYTE* m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount


public:
	~CVCByteArray();

protected:
	// local typedefs for class templates
	typedef BYTE BASE_TYPE;
	typedef BYTE BASE_ARG_TYPE;
};


////////////////////////////////////////////////////////////////////////////

class CVCWordArray
{

public:

// Construction
	CVCWordArray();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	WORD GetAt(INT_PTR nIndex) const;
	void SetAt(INT_PTR nIndex, WORD newElement);

	WORD& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const WORD* GetData() const;
	WORD* GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, WORD newElement);

	INT_PTR Add(WORD newElement);

	INT_PTR Append(const CVCWordArray& src);
	void Copy(const CVCWordArray& src);

	// overloaded operator helpers
	WORD operator[](INT_PTR nIndex) const;
	WORD& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, WORD newElement, INT_PTR nCount = 1);

	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void InsertAt(INT_PTR nStartIndex, CVCWordArray* pNewArray);

// Implementation
protected:
	WORD* m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount


public:
	~CVCWordArray();


protected:
	// local typedefs for class templates
	typedef WORD BASE_TYPE;
	typedef WORD BASE_ARG_TYPE;
};


////////////////////////////////////////////////////////////////////////////

class CVCDWordArray
{

public:

// Construction
	CVCDWordArray();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	DWORD GetAt(INT_PTR nIndex) const;
	void SetAt(INT_PTR nIndex, DWORD newElement);

	DWORD& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const DWORD* GetData() const;
	DWORD* GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, DWORD newElement);

	INT_PTR Add(DWORD newElement);

	INT_PTR Append(const CVCDWordArray& src);
	void Copy(const CVCDWordArray& src);

	// overloaded operator helpers
	DWORD operator[](INT_PTR nIndex) const;
	DWORD& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, DWORD newElement, INT_PTR nCount = 1);

	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void InsertAt(INT_PTR nStartIndex, CVCDWordArray* pNewArray);

// Implementation
protected:
	DWORD* m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount


public:
	~CVCDWordArray();


protected:
	// local typedefs for class templates
	typedef DWORD BASE_TYPE;
	typedef DWORD BASE_ARG_TYPE;
};


////////////////////////////////////////////////////////////////////////////

class CVCUIntArray
{

public:

// Construction
	CVCUIntArray();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	UINT GetAt(INT_PTR nIndex) const;
	void SetAt(INT_PTR nIndex, UINT newElement);

	UINT& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const UINT* GetData() const;
	UINT* GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, UINT newElement);

	INT_PTR Add(UINT newElement);

	INT_PTR Append(const CVCUIntArray& src);
	void Copy(const CVCUIntArray& src);

	// overloaded operator helpers
	UINT operator[](INT_PTR nIndex) const;
	UINT& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, UINT newElement, INT_PTR nCount = 1);

	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void InsertAt(INT_PTR nStartIndex, CVCUIntArray* pNewArray);

// Implementation
protected:
	UINT* m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount


public:
	~CVCUIntArray();

protected:
	// local typedefs for class templates
	typedef UINT BASE_TYPE;
	typedef UINT BASE_ARG_TYPE;
};


////////////////////////////////////////////////////////////////////////////

class CVCPtrArray
{

public:

// Construction
	CVCPtrArray();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	void* GetAt(INT_PTR nIndex) const;
	void SetAt(INT_PTR nIndex, void* newElement);

	void*& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const void** GetData() const;
	void** GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, void* newElement);

	INT_PTR Add(void* newElement);

	INT_PTR Append(const CVCPtrArray& src);
	void Copy(const CVCPtrArray& src);

	// overloaded operator helpers
	void* operator[](INT_PTR nIndex) const;
	void*& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, void* newElement, INT_PTR nCount = 1);

	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void InsertAt(INT_PTR nStartIndex, CVCPtrArray* pNewArray);

// Implementation
protected:
	void** m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount


public:
	~CVCPtrArray();

protected:
	// local typedefs for class templates
	typedef void* BASE_TYPE;
	typedef void* BASE_ARG_TYPE;
};



/////////////////////////////////////////////////////////////////////////////

class CVCPtrList
{


protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		void* data;
	};
public:

// Construction
	CVCPtrList(INT_PTR nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	void*& GetHead();
	void* GetHead() const;
	void*& GetTail();
	void* GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list!
	void* RemoveHead();
	void* RemoveTail();

	// add before head or after tail
	VCPOSITION AddHead(void* newElement);
	VCPOSITION AddTail(void* newElement);


	// add another list of elements before head or after tail
	void AddHead(CVCPtrList* pNewList);
	void AddTail(CVCPtrList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	VCPOSITION GetHeadPosition() const;
	VCPOSITION GetTailPosition() const;
	void*& GetNext(VCPOSITION& rPosition); // return *VCPOSITION++
	void* GetNext(VCPOSITION& rPosition) const; // return *VCPOSITION++
	void*& GetPrev(VCPOSITION& rPosition); // return *VCPOSITION--
	void* GetPrev(VCPOSITION& rPosition) const; // return *VCPOSITION--

	// getting/modifying an element at a given VCPOSITION
	void*& GetAt(VCPOSITION pos);
	void* GetAt(VCPOSITION pos) const;
	void SetAt(VCPOSITION pos, void* newElement);

	void RemoveAt(VCPOSITION pos);

	// inserting before or after a given VCPOSITION
	VCPOSITION InsertBefore(VCPOSITION pos, void* newElement);
	VCPOSITION InsertAfter(VCPOSITION pos, void* newElement);


	// helper functions (note: O(n) speed)
	VCPOSITION Find(void* searchValue, VCPOSITION startAfter = NULL) const;
						// defaults to starting at the HEAD
						// return NULL if not found
	VCPOSITION FindIndex(INT_PTR nIndex) const;
						// get the 'nIndex'th element (may return NULL)

// Implementation
protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	INT_PTR m_nCount;
	CNode* m_pNodeFree;
	struct CVCPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);

public:
	~CVCPtrList();
	// local typedefs for class templates
	typedef void* BASE_TYPE;
	typedef void* BASE_ARG_TYPE;
};




/////////////////////////////////////////////////////////////////////////////

class CVCMapWordToPtr
{

protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;

		WORD key;
		void* value;
	};

public:

// Construction
	CVCMapWordToPtr(INT_PTR nBlockSize = 10);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(WORD key, void*& rValue) const;

// Operations
	// Lookup and add if not there
	void*& operator[](WORD key);

	// add a new (key, value) pair
	void SetAt(WORD key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(WORD key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, WORD& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(WORD key) const;

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	INT_PTR m_nCount;
	CAssoc* m_pFreeList;
	struct CVCPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(WORD, UINT&, UINT&) const;

public:
	~CVCMapWordToPtr();

protected:
	// local typedefs for CTypedPtrMap class template
	typedef WORD BASE_KEY;
	typedef WORD BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};



/////////////////////////////////////////////////////////////////////////////

class CVCMapDWordToPtr
{

protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;

		DWORD key;
		void* value;
	};

public:

// Construction
	CVCMapDWordToPtr(INT_PTR nBlockSize = 10);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(DWORD key, void*& rValue) const;

// Operations
	// Lookup and add if not there
	void*& operator[](DWORD key);

	// add a new (key, value) pair
	void SetAt(DWORD key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(DWORD key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, DWORD& rKey, void*& rValue) const;

	// advanced features for derived classes
	DWORD GetHashTableSize() const;
	void InitHashTable(DWORD hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	DWORD HashKey(DWORD key) const;

// Implementation
protected:
	CAssoc** m_pHashTable;
	DWORD m_nHashTableSize;
	INT_PTR m_nCount;
	CAssoc* m_pFreeList;
	struct CVCPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(DWORD, DWORD&, DWORD&) const;

public:
	~CVCMapDWordToPtr();

protected:
	// local typedefs for CTypedPtrMap class template
	typedef DWORD BASE_KEY;
	typedef DWORD BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};


/////////////////////////////////////////////////////////////////////////////

class CVCMapPtrToWord 
{

protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;

		void* key;
		WORD value;
	};

public:

// Construction
	CVCMapPtrToWord(INT_PTR nBlockSize = 10);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(void* key, WORD& rValue) const;

// Operations
	// Lookup and add if not there
	WORD& operator[](void* key);

	// add a new (key, value) pair
	void SetAt(void* key, WORD newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(void* key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, void*& rKey, WORD& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(void* key) const;

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	INT_PTR m_nCount;
	CAssoc* m_pFreeList;
	struct CVCPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(void*, UINT&, UINT&) const;

public:
	~CVCMapPtrToWord();


protected:
	// local typedefs for CTypedPtrMap class template
	typedef void* BASE_KEY;
	typedef void* BASE_ARG_KEY;
	typedef WORD BASE_VALUE;
	typedef WORD BASE_ARG_VALUE;
};


/////////////////////////////////////////////////////////////////////////////

class CVCMapPtrToPtr 
{

protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;

		void* key;
		void* value;
	};

public:

// Construction
	CVCMapPtrToPtr(INT_PTR nBlockSize = 10);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(void* key, void*& rValue) const;

// Operations
	// Lookup and add if not there
	void*& operator[](void* key);

	// add a new (key, value) pair
	void SetAt(void* key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(void* key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, void*& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(void* key) const;

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	INT_PTR m_nCount;
	CAssoc* m_pFreeList;
	struct CVCPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(void*, UINT&, UINT&) const;

public:
	~CVCMapPtrToPtr();

	void* GetValueAt(void* key) const;


protected:
	// local typedefs for CTypedPtrMap class template
	typedef void* BASE_KEY;
	typedef void* BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};



/////////////////////////////////////////////////////////////////////////////
// CVCList<TYPE, ARG_TYPE>

template<class TYPE, class ARG_TYPE>
class CVCList 
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		TYPE data;
	};
public:
// Construction
	CVCList(INT_PTR nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	TYPE& GetHead();
	TYPE GetHead() const;
	TYPE& GetTail();
	TYPE GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list !
	TYPE RemoveHead();
	TYPE RemoveTail();

	// add before head or after tail
	VCPOSITION AddHead(ARG_TYPE newElement);
	VCPOSITION AddTail(ARG_TYPE newElement);

	// add another list of elements before head or after tail
	void AddHead(CVCList* pNewList);
	void AddTail(CVCList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	VCPOSITION GetHeadPosition() const;
	VCPOSITION GetTailPosition() const;
	TYPE& GetNext(VCPOSITION& rPosition); // return *Position++
	TYPE GetNext(VCPOSITION& rPosition) const; // return *Position++
	TYPE& GetPrev(VCPOSITION& rPosition); // return *Position--
	TYPE GetPrev(VCPOSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	TYPE& GetAt(VCPOSITION position);
	TYPE GetAt(VCPOSITION position) const;
	void SetAt(VCPOSITION pos, ARG_TYPE newElement);
	void RemoveAt(VCPOSITION position);

	// inserting before or after a given position
	VCPOSITION InsertBefore(VCPOSITION position, ARG_TYPE newElement);
	VCPOSITION InsertAfter(VCPOSITION position, ARG_TYPE newElement);

	// helper functions (note: O(n) speed)
	VCPOSITION Find(ARG_TYPE searchValue, VCPOSITION startAfter = NULL) const;
		// defaults to starting at the HEAD, return NULL if not found
	VCPOSITION FindIndex(INT_PTR nIndex) const;
		// get the 'nIndex'th element (may return NULL)

// Implementation
protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	INT_PTR m_nCount;
	CNode* m_pNodeFree;
	struct CVCPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);

public:
	~CVCList();
};


/////////////////////////////////////////////////////////////////////////////
// CVCTypedPtrArray<BASE_CLASS, TYPE>

template<class BASE_CLASS, class TYPE>
class CVCTypedPtrArray : public BASE_CLASS
{
public:
	// Accessing elements
	TYPE GetAt(INT_PTR nIndex) const
		{ return (TYPE)BASE_CLASS::GetAt(nIndex); }
	TYPE& ElementAt(INT_PTR nIndex)
		{ return (TYPE&)BASE_CLASS::ElementAt(nIndex); }
	void SetAt(INT_PTR nIndex, TYPE ptr)
		{ BASE_CLASS::SetAt(nIndex, ptr); }

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, TYPE newElement)
	   { BASE_CLASS::SetAtGrow(nIndex, newElement); }
	INT_PTR Add(TYPE newElement)
	   { return BASE_CLASS::Add(newElement); }
	INT_PTR Append(const CVCTypedPtrArray<BASE_CLASS, TYPE>& src)
	   { return BASE_CLASS::Append(src); }
	void Copy(const CVCTypedPtrArray<BASE_CLASS, TYPE>& src)
		{ BASE_CLASS::Copy(src); }

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, TYPE newElement, INT_PTR nCount = 1)
		{ BASE_CLASS::InsertAt(nIndex, newElement, nCount); }
	void InsertAt(INT_PTR nStartIndex, CVCTypedPtrArray<BASE_CLASS, TYPE>* pNewArray)
	   { BASE_CLASS::InsertAt(nStartIndex, pNewArray); }

	// overloaded operator helpers
	TYPE operator[](INT_PTR nIndex) const
		{ return (TYPE)BASE_CLASS::operator[](nIndex); }
	TYPE& operator[](INT_PTR nIndex)
		{ return (TYPE&)BASE_CLASS::operator[](nIndex); }
};

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#include <vccoll.inl>

#ifdef _VC_REDEF__ASSERTE
	#pragma pop_macro("_ASSERTE")
	#undef _VC_REDEF__ASSERTE
#endif

/////////////////////////////////////////////////////////////////////////////
