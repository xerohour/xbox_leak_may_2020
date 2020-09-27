// (KPerry) This was the MFC collection stuff
// Now it has been purged of all other MFC influences and is a stand alone entity.

#ifndef __VCCOLLS_H__
#define __VCCOLLS_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "vcstring.h"
#include "vccoll.h"


/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

	// Special String variants
	class CVCStringArray;         // array ofCVCStrings
	class CVCStringList;          // list of CVCStrings
	class CVCMapStringToPtr;      // map from CVCString to void*
	class CVCMapStringToString;   // map from CVCString to CVCString

	class CVCStringWArray;         // array ofCStringWs
	class CVCStringWList;          // list of CStringWs
	class CVCMapStringWToPtr;     // map from CStringW to void*
	class CVCMapStringWToStringW;	// map from CStringW to CStringW

/////////////////////////////////////////////////////////////////////////////
// Other implementation helpers

#define VCBEFORE_START_POSITION ((VCPOSITION)-1L)

////////////////////////////////////////////////////////////////////////////

class CVCStringArray
{

public:

// Construction
	CVCStringArray();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	CVCString GetAt(INT_PTR nIndex) const;
	void SetAt(INT_PTR nIndex, LPCTSTR newElement);

	void SetAt(INT_PTR nIndex, const CVCString& newElement);

	CVCString& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const CVCString* GetData() const;
	CVCString* GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, LPCTSTR newElement);

	void SetAtGrow(INT_PTR nIndex, const CVCString& newElement);

	INT_PTR Add(LPCTSTR newElement);

	INT_PTR Add(const CVCString& newElement);

	INT_PTR Append(const CVCStringArray& src);
	void Copy(const CVCStringArray& src);

	// overloaded operator helpers
	CVCString operator[](INT_PTR nIndex) const;
	CVCString& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, LPCTSTR newElement, INT_PTR nCount = 1);

	void InsertAt(INT_PTR nIndex, const CVCString& newElement, INT_PTR nCount = 1);

	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void InsertAt(INT_PTR nStartIndex, CVCStringArray* pNewArray);

// Implementation
protected:
	CVCString* m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount

	void InsertEmpty(INT_PTR nIndex, INT_PTR nCount);


public:
	~CVCStringArray();


protected:
	// local typedefs for class templates
	typedef CVCString BASE_TYPE;
	typedef LPCTSTR BASE_ARG_TYPE;
};


/////////////////////////////////////////////////////////////////////////////

class CVCStringList
{


protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		CVCString data;
	};
public:

// Construction
	CVCStringList(INT_PTR nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	CVCString& GetHead();
	CVCString GetHead() const;
	CVCString& GetTail();
	CVCString GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list!
	CVCString RemoveHead();
	CVCString RemoveTail();

	// add before head or after tail
	VCPOSITION AddHead(LPCTSTR newElement);
	VCPOSITION AddTail(LPCTSTR newElement);

	VCPOSITION AddHead(const CVCString& newElement);
	VCPOSITION AddTail(const CVCString& newElement);


	// add another list of elements before head or after tail
	void AddHead(CVCStringList* pNewList);
	void AddTail(CVCStringList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	VCPOSITION GetHeadPosition() const;
	VCPOSITION GetTailPosition() const;
	CVCString& GetNext(VCPOSITION& rPosition); // return *VCPOSITION++
	CVCString GetNext(VCPOSITION& rPosition) const; // return *VCPOSITION++
	CVCString& GetPrev(VCPOSITION& rPosition); // return *VCPOSITION--
	CVCString GetPrev(VCPOSITION& rPosition) const; // return *VCPOSITION--

	// getting/modifying an element at a given VCPOSITION
	CVCString& GetAt(VCPOSITION VCPOSITION);
	CVCString GetAt(VCPOSITION VCPOSITION) const;
	void SetAt(VCPOSITION pos, LPCTSTR newElement);

	void SetAt(VCPOSITION pos, const CVCString& newElement);

	void RemoveAt(VCPOSITION VCPOSITION);

	// inserting before or after a given VCPOSITION
	VCPOSITION InsertBefore(VCPOSITION VCPOSITION, LPCTSTR newElement);
	VCPOSITION InsertAfter(VCPOSITION VCPOSITION, LPCTSTR newElement);

	VCPOSITION InsertBefore(VCPOSITION VCPOSITION, const CVCString& newElement);
	VCPOSITION InsertAfter(VCPOSITION VCPOSITION, const CVCString& newElement);


	// helper functions (note: O(n) speed)
	VCPOSITION Find(LPCTSTR searchValue, VCPOSITION startAfter = NULL) const;
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
	~CVCStringList();

	// local typedefs for class templates
	typedef CVCString BASE_TYPE;
	typedef LPCTSTR BASE_ARG_TYPE;
};



/////////////////////////////////////////////////////////////////////////////

class CVCMapStringToPtr
{

protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		CVCString key;
		void* value;
	};

public:

// Construction
	CVCMapStringToPtr(INT_PTR nBlockSize = 10);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(LPCTSTR key, void*& rValue) const;
	BOOL LookupKey(LPCTSTR key, LPCTSTR& rKey) const;

// Operations
	// Lookup and add if not there
	void*& operator[](LPCTSTR key);

	// add a new (key, value) pair
	void SetAt(LPCTSTR key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(LPCTSTR key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, CVCString& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(LPCTSTR key) const;

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
	CAssoc* GetAssocAt(LPCTSTR, UINT&, UINT&) const;

public:
	~CVCMapStringToPtr();

protected:
	// local typedefs for CTypedPtrMap class template
	typedef CVCString BASE_KEY;
	typedef LPCTSTR BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};



/////////////////////////////////////////////////////////////////////////////

class CVCMapStringToString
{

protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		CVCString key;
		CVCString value;
	};

public:

// Construction
	CVCMapStringToString(INT_PTR nBlockSize = 10);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(LPCTSTR key, CVCString& rValue) const;
	BOOL LookupKey(LPCTSTR key, LPCTSTR& rKey) const;

// Operations
	// Lookup and add if not there
	CVCString& operator[](LPCTSTR key);

	// add a new (key, value) pair
	void SetAt(LPCTSTR key, LPCTSTR newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(LPCTSTR key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, CVCString& rKey, CVCString& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(LPCTSTR key) const;

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
	CAssoc* GetAssocAt(LPCTSTR, UINT&, UINT&) const;

public:
	~CVCMapStringToString();


protected:
	// local typedefs for CTypedPtrMap class template
	typedef CVCString BASE_KEY;
	typedef LPCTSTR BASE_ARG_KEY;
	typedef CVCString BASE_VALUE;
	typedef LPCTSTR BASE_ARG_VALUE;
};

/////////////////////////////////////////////////////////////////////////////

class CVCMapStringWToStringW
{

protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		CStringW key;
		CStringW value;
	};

public:

// Construction
	CVCMapStringWToStringW(INT_PTR nBlockSize = 10, UINT nHashSize = 17);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(LPCOLESTR key, CStringW& rValue) const;
	BOOL LookupKey(LPCOLESTR key, LPCOLESTR& rKey) const;

// Operations
	// Lookup and add if not there
	CStringW& operator[](LPCOLESTR key);

	// add a new (key, value) pair
	void SetAt(LPCOLESTR key, LPCOLESTR newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(LPCOLESTR key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, CStringW& rKey, CStringW& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(LPCOLESTR key) const;

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
	CAssoc* GetAssocAt(LPCOLESTR, UINT&, UINT&) const;

public:
	~CVCMapStringWToStringW();


protected:
	// local typedefs for CTypedPtrMap class template
	typedef CStringW BASE_KEY;
	typedef LPCOLESTR BASE_ARG_KEY;
	typedef CStringW BASE_VALUE;
	typedef LPCOLESTR BASE_ARG_VALUE;
};

/////////////////////////////////////////////////////////////////////////////

class CVCStringWArray
{

public:

// Construction
	CVCStringWArray();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	CStringW GetAt(INT_PTR nIndex) const;
	void SetAt(INT_PTR nIndex, LPCOLESTR newElement);

	void SetAt(INT_PTR nIndex, const CStringW& newElement);

	CStringW& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const CStringW* GetData() const;
	CStringW* GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, LPCOLESTR newElement);

	void SetAtGrow(INT_PTR nIndex, const CStringW& newElement);

	INT_PTR Add(LPCOLESTR newElement);

	INT_PTR Add(const CStringW& newElement);

	INT_PTR Append(const CVCStringWArray& src);
	void Copy(const CVCStringWArray& src);

	// overloaded operator helpers
	CStringW operator[](INT_PTR nIndex) const;
	CStringW& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, LPCOLESTR newElement, INT_PTR nCount = 1);

	void InsertAt(INT_PTR nIndex, const CStringW& newElement, INT_PTR nCount = 1);

	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void InsertAt(INT_PTR nStartIndex, CVCStringWArray* pNewArray);

// Implementation
protected:
	CStringW* m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount

	void InsertEmpty(INT_PTR nIndex, INT_PTR nCount);


public:
	~CVCStringWArray();


protected:
	// local typedefs for class templates
	typedef CStringW BASE_TYPE;
	typedef LPCOLESTR BASE_ARG_TYPE;
};

/////////////////////////////////////////////////////////////////////////////

class CVCMapStringWToPtr
{

protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		CStringW key;
		void* value;
	};

public:

// Construction
	CVCMapStringWToPtr(INT_PTR nBlockSize = 10);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(LPCOLESTR key, void*& rValue) const;
	BOOL LookupKey(LPCOLESTR key, LPCOLESTR& rKey) const;

// Operations
	// Lookup and add if not there
	void*& operator[](LPCOLESTR key);

	// add a new (key, value) pair
	void SetAt(LPCOLESTR key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(LPCOLESTR key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, CStringW& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(LPCOLESTR key) const;

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
	CAssoc* GetAssocAt(LPCOLESTR, UINT&, UINT&) const;

public:
	~CVCMapStringWToPtr();

protected:
	// local typedefs for CTypedPtrMap class template
	typedef CStringW BASE_KEY;
	typedef LPCOLESTR BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};


/////////////////////////////////////////////////////////////////////////////

class CVCStringWList
{


protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		CStringW data;
	};
public:

// Construction
	CVCStringWList(INT_PTR nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	CStringW& GetHead();
	CStringW GetHead() const;
	CStringW& GetTail();
	CStringW GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list!
	CStringW RemoveHead();
	CStringW RemoveTail();

	// add before head or after tail
	VCPOSITION AddHead(LPCOLESTR newElement);
	VCPOSITION AddTail(LPCOLESTR newElement);

	VCPOSITION AddHead(const CStringW& newElement);
	VCPOSITION AddTail(const CStringW& newElement);


	// add another list of elements before head or after tail
	void AddHead(CVCStringWList* pNewList);
	void AddTail(CVCStringWList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	VCPOSITION GetHeadPosition() const;
	VCPOSITION GetTailPosition() const;
	CStringW& GetNext(VCPOSITION& rPosition); // return *VCPOSITION++
	CStringW GetNext(VCPOSITION& rPosition) const; // return *VCPOSITION++
	CStringW& GetPrev(VCPOSITION& rPosition); // return *VCPOSITION--
	CStringW GetPrev(VCPOSITION& rPosition) const; // return *VCPOSITION--

	// getting/modifying an element at a given VCPOSITION
	CStringW& GetAt(VCPOSITION VCPOSITION);
	CStringW GetAt(VCPOSITION VCPOSITION) const;
	void SetAt(VCPOSITION pos, LPCOLESTR newElement);

	void SetAt(VCPOSITION pos, const CStringW& newElement);

	void RemoveAt(VCPOSITION VCPOSITION);

	// inserting before or after a given VCPOSITION
	VCPOSITION InsertBefore(VCPOSITION VCPOSITION, LPCOLESTR newElement);
	VCPOSITION InsertAfter(VCPOSITION VCPOSITION, LPCOLESTR newElement);

	VCPOSITION InsertBefore(VCPOSITION VCPOSITION, const CStringW& newElement);
	VCPOSITION InsertAfter(VCPOSITION VCPOSITION, const CStringW& newElement);


	// helper functions (note: O(n) speed)
	VCPOSITION Find(LPCOLESTR searchValue, VCPOSITION startAfter = NULL) const;
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
	~CVCStringWList();

	// local typedefs for class templates
	typedef CStringW BASE_TYPE;
	typedef LPCOLESTR BASE_ARG_TYPE;
};

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#include <vccolls.inl>

#endif //!__VCCOLL_H__

/////////////////////////////////////////////////////////////////////////////
