//---------------------------------------------------------------------------
// Microsoft Visual Studio
//
// Microsoft Confidential
// Copyright (C) 1994 - 2000 Microsoft Corporation. 
// All Rights Reserved.
//
// Util.h -- utility classes
//---------------------------------------------------------------------------
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CString -- simple string class

class CString
{
public:
	CString() : m_psz(NULL)							{ Copy(L"", 0); }
	CString(const CString& str) : m_psz(NULL)		{ Copy(str); }
	CString(const WCHAR* psz) : m_psz(NULL)			{ Copy(psz); }
	CString(const WCHAR* psz, int iLen) : m_psz(NULL)	{ Copy(psz, iLen); }
	~CString();

	void Empty()									{ Copy(L"", 0); }
	CString& Append(const WCHAR* psz)				{ return Append(psz, lstrlenW(psz)); }
	CString& Append(const WCHAR* pch, int iAppend);
	CString& Prepend(const WCHAR* psz)				{ return Prepend(psz, lstrlenW(psz)); }
	CString& Prepend(const WCHAR* pch, int iPrepend);
	CString& Copy(const WCHAR* psz)					{ return Copy(psz, lstrlenW(psz)); }
	CString& Copy(const WCHAR* pch, int iLen);
	int Length() const								{ return lstrlenW(m_psz); }
	void SetLength(int iLen);
	WCHAR GetAt(int i) const						{ return m_psz[i]; }
	void SetAt(int i, WCHAR ch)						{ m_psz[i] = ch; }
	BOOL IsEqual(const WCHAR* psz) const			{ return IsEqual(psz, lstrlenW(psz)); }
	BOOL IsEqual(const WCHAR* pch, int iLen) const;
	int Compare(const WCHAR* psz) const				{ return Compare(m_psz, psz); }
	int CompareNoCase(const WCHAR* psz) const		{ return CompareNoCase(m_psz, psz); }

	static BOOL IsEqual(const WCHAR* pszW1, const WCHAR* pszW2);
	static int _stdcall Compare(const WCHAR* pszW1, const WCHAR* pszW2);
	static int _stdcall CompareNoCase(const WCHAR* pszW1, const WCHAR* pszW2);
	static int Compare(const WCHAR* pszW1, const WCHAR* pszW2, int iLen);
	static int CompareNoCase(const WCHAR* pszW1, const WCHAR* pszW2, int iLen);
	int LoadResourceString(UINT id, HINSTANCE hInst);

	CString& operator=(const CString& str)			{ return Copy(str); }
	CString& operator=(const WCHAR* psz)			{ return Copy(psz); }
	CString& operator+=(const WCHAR* psz)			{ return Append(psz); }
	BOOL operator==(const WCHAR* psz) const			{ return IsEqual(psz); }
	BOOL operator!=(const WCHAR* psz) const			{ return !IsEqual(psz); }
	BOOL IsNULL () const                            { return m_psz == NULL; }
	BOOL IsEmpty () const                           { return m_psz == NULL || *m_psz == 0; }
	operator const WCHAR*() const					{ return m_psz; }

#ifndef _UNICODE
	CString(const TCHAR* psz) : m_psz(NULL)			{ Copy(psz); }
	CString(const TCHAR* psz, int iLen) : m_psz(NULL)	{ Copy(psz, iLen); }
	CString& Append(const TCHAR* psz)				{ return Append(psz, lstrlen(psz)); }
	CString& Append(const TCHAR* pch, int iAppend);
	CString& Copy(const TCHAR* psz)					{ return Copy(psz, lstrlen(psz)); }
	CString& Copy(const TCHAR* pch, int iLen);
	CString& operator=(const TCHAR* psz)			{ return Copy(psz); }
	CString& operator+=(const TCHAR* psz)			{ return Append(psz); }
#endif

protected:
	WCHAR* m_psz;
};

/////////////////////////////////////////////////////////////////////////////
// CDictImpl -- implementation class for CDict

class CDictImpl
{
private:
	friend class CDictEnumImpl;

	struct CBucket
	{
		DWORD   dwHash;
		void	*pValue;
		CBucket	*pNext;
		WCHAR   szKey[1];
	};

	CBucket **m_ppBuckets;
	int		m_iCount;

	int		BucketIndex (DWORD dwHash) { return dwHash & (m_nBuckets-1); }

	// CDictImpl cannot be copied!
	CDictImpl(const CDictImpl&) {}
	CDictImpl& operator=(const CDictImpl&) { return *this; }

protected:
	BOOL	m_fIgnoreCase;
	int		m_nBuckets;

protected:
	void*	_Find (const WCHAR *pszName);
	BOOL	_Add (const WCHAR *pszName, void *pObj);
	BOOL	_RemoveValue (void *pObj, BOOL fRemoveAll);
	void	_Enum (CDictEnumImpl& e);
	
	virtual void _OnRemove (void *pObj) = 0;
	virtual void _OnAdd (void *pObj) = 0;

public:
	CDictImpl(BOOL fIgnoreCase = FALSE, int nBuckets = 32);
	~CDictImpl();

	DWORD	_Hash (const WCHAR *pszName);	// default hash algorithm

	BOOL	Remove (const WCHAR *pszName);
	void	RemoveAll ();
	int		Count () { return m_iCount; }

	virtual	DWORD Hash (const WCHAR *pszName) = 0;

#ifdef _DEBUG
	void	Dump ();
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CDictEnumImpl -- implementation class for CDictEnum

class CDictEnumImpl
{
private:
	friend class CDictImpl;

	CDictImpl* m_pDict;
	int m_iBucketIndex;
	CDictImpl::CBucket* m_pBucket;

protected:
	void	_Init (CDictImpl* pDict);
	void	*_Next (const WCHAR** ppszKey);

public:
	CDictEnumImpl ();

	BOOL    Next (const WCHAR **ppszKey) { return _Next (ppszKey) != NULL; }
	long    Count () { return m_pDict->Count(); }
	void	Reset ();
};

/////////////////////////////////////////////////////////////////////////////
// CDictCallback -- callback interface for CDict

template <class T>
class CDictCallback
{
public:
	virtual void OnRemove (T *pObj) const {}
	virtual void OnAdd (T *pObj) const {}
};

/////////////////////////////////////////////////////////////////////////////
// CDictEnum -- enumerator for elements in a CDict

template <class T>
class CDictEnum : public CDictEnumImpl
{
public:
	T		*Next (const WCHAR** ppszKey = NULL) { return (T *)_Next(ppszKey); }
};

/////////////////////////////////////////////////////////////////////////////
// CDict -- simple string-to-object dictionary

template <class T>
class CDict : public CDictImpl
{
private:
	const CDictCallback<T>	*m_pCallback;
	
protected:
    virtual void    _OnAdd (void *pObj)     { OnAdd ((T *)pObj); }
    virtual void    _OnRemove (void *pObj)  { OnRemove ((T *)pObj); }

public:
	CDict (const CDictCallback<T> *pCallback = NULL, BOOL fIgnoreCase = FALSE, int nBuckets = 32) :
		CDictImpl(fIgnoreCase, nBuckets)
	{
		m_pCallback = pCallback;
	}

	~CDict ()									{ RemoveAll (); }

	void    SetCallback (const CDictCallback<T> *pCallback) { m_pCallback = pCallback; }
	T		*Find (const WCHAR *pszName)					{ return (T *)_Find (pszName); }
	BOOL	Add (const WCHAR *pszName, T *pObj)				{ return _Add (pszName, pObj); }
	BOOL	RemoveValue (T *pObj, BOOL fRemoveAll = FALSE)	{ return _RemoveValue (pObj, fRemoveAll); }
	void	Enum (CDictEnum<T>& e)							{ _Enum(e); }

	virtual	DWORD Hash (const WCHAR *pszName)	{ return _Hash(pszName); }
	virtual void OnAdd (T *pObj)				{ if (m_pCallback) m_pCallback->OnAdd (pObj); }
	virtual void OnRemove (T *pObj)				{ if (m_pCallback) m_pCallback->OnRemove (pObj); }
};

/////////////////////////////////////////////////////////////////////////////
// CPtrArrayImpl -- implementation class for CPtrArray

class CPtrArrayImpl
{
private:
    enum { ALLOC_INCR = 8 };

    void**  m_ppData;
    int     m_iCount;
    int     m_iAlloc;

    // CPtrArrayImpl cannot be copied!
    CPtrArrayImpl(const CPtrArrayImpl&) {}
    CPtrArrayImpl& operator=(const CPtrArrayImpl&) { return *this; }

protected:
    void *  _GetAt      (int i);
    void    _SetAt      (int i, void* pData);
    BOOL    _Add        (void* pData);
    BOOL    _Remove     (void* pData);
    BOOL    _InsertAt   (int i, void* pObj);

    BOOL    _ReallocData (int iAlloc);

    void    _Sort    (                 int (__cdecl *compare )(const void *p1, const void *p2));
    int     _BSearch (const void *key, int (__cdecl *compare )(const void *p1, const void *p2));

    void    _Attach  (int cel, void** prgObj);
    void    _Detach  (void) { m_ppData = NULL; m_iCount = m_iAlloc = 0; }

public:
    CPtrArrayImpl(int iAlloc = 0);
    ~CPtrArrayImpl();

    BOOL    RemoveAt    (int i);
    void    RemoveAll   (void)  { m_iCount = 0; }
    void    SetTop      (int i) { m_iCount = i; }
    int     Count       (void)  { return m_iCount; }
};

inline CPtrArrayImpl::CPtrArrayImpl(int iAlloc) :
	m_ppData(NULL),
	m_iCount(0),
	m_iAlloc(0)
{
	_ReallocData(iAlloc);
}

/////////////////////////////////////////////////////////////////////////////
// CPtrArray -- simple array of pointers

template <class T>
class CPtrArray : public CPtrArrayImpl
{
public:
    CPtrArray (int iAlloc = 0) : CPtrArrayImpl(iAlloc) {}
    ~CPtrArray() { RemoveAll(); }

    T *     GetAt       (int i)                 { return (T*)_GetAt(i); }
    void    SetAt       (int i, T * pObj)       { _SetAt(i, pObj); }
    BOOL    Add         (T * pObj)              { return _Add(pObj); }
    BOOL    Remove      (T * pObj)              { return _Remove(pObj); }
    BOOL    InsertAt    (int i, T * pObj)       { return _InsertAt(i, pObj); }
                        
    void    Sort        (               int (__cdecl *compare)(T** p1, T** p2)) { _Sort((int (__cdecl *)(const void *, void const *))compare); }
    int     BSearch     (const T *pkey, int (__cdecl *compare)(T** p1, T** p2)) { return _BSearch((const void *)pkey, (int (__cdecl *)(const void *, void const *))compare); }
                        
    void    Attach      (int cel, T** prgObj)   { _Attach(cel, prgObj); }
    void    Detach      (void)                  { _Detach(); }
};

#define CArray CPtrArray	// for compatibility

/////////////////////////////////////////////////////////////////////////////
// CStructArrayImpl -- implementation class for CStructArray

class CStructArrayImpl
{
private:
    enum { ALLOC_INCR = 8 };

    BYTE*   m_pData;
    int     m_iElemSize;
    int     m_iCount;
    int     m_iAlloc;

    // CStructArrayImpl cannot be copied!
    CStructArrayImpl(const CStructArrayImpl&) {}
    CStructArrayImpl& operator=(const CStructArrayImpl&) { return *this; }

protected:
    void *  _GetElem        (int i)     { return m_pData + (i * m_iElemSize); }
    void *  _GetAt          (int i);
    void    _SetAt          (int i, void * pData);
    BOOL    _Add            (void * pData);
    BOOL    _Remove         (void * pData);
    BOOL    _InsertAt       (int i, void * pData);
    BOOL    _ReallocData    (int iAlloc);
    void    _Attach         (int cel, void * pData);
    void    _Detach         (void) { m_pData = 0; m_iCount = m_iAlloc = 0; }

public:
    CStructArrayImpl(int iElemSize, int iAlloc = 0);
    ~CStructArrayImpl();

    BOOL    RemoveAt    (int i);
    void    RemoveAll   (void)      { m_iCount = 0; }
    void    SetTop      (int i)     { m_iCount = i; }
    int     Count       (void)      { return m_iCount; }
};

inline CStructArrayImpl::CStructArrayImpl(int iElemSize, int iAlloc) :
	m_pData(NULL),
	m_iElemSize(iElemSize),
	m_iCount(0),
	m_iAlloc(0)
{
	_ReallocData(iAlloc);
}

/////////////////////////////////////////////////////////////////////////////
// CStructArray -- simple array of structs

template <class T>
class CStructArray : public CStructArrayImpl
{
public:
	CStructArray(int iAlloc = 0) : CStructArrayImpl(sizeof(T), iAlloc) {}
	~CStructArray()						{ RemoveAll(); }

	T*		GetAt       (int i)				{ return (T*)_GetAt(i); }
	void	SetAt       (int i, T* pStruct) { _SetAt(i, pStruct); }
	BOOL	Add         (T* pStruct)		{ return _Add(pStruct); }
	BOOL	Remove      (T* pStruct)		{ return _Remove(pStruct); }
	BOOL	InsertAt    (int i, T* pStruct)	{ return _InsertAt(i, pStruct); }
    void    Attach      (int cel, T* pData) { _Attach(cel, pData); }  // pData must be allocated with VSAlloc!
    void    Detach      (void)              { _Detach(); } 
};


//////////////////////////////////////////////////////////////////
// C*ArrayBuilder
// 
// Use an array builder when the array usage follows a two-phase pattern
// of building the array, then using the built array with few or no additions
// or deletions. The array builder can be much more efficient that a C*Array 
// when the total number of elements is unknown or can grow large.
// 
// After building your array with the array builder, get the final contiguous 
// memory buffer with Gimme. After calling Gimme, the builder is empty and the
// caller owns the buffer.
//
// You can attach the buffer to a C*Array to subsequently use it as an array.
// 

class CABBlock; // private array builder block class

// array builder implementation class
class CByteArrayBuilder
{
public:
    CByteArrayBuilder   (int cbBlock);
    ~CByteArrayBuilder  () { Clear(); }

    HRESULT     Add     (int cb, BYTE * pb);
    HRESULT     Gimme   (int * pcb, BYTE **ppData);
    void        Clear   (void);

    void        StreamInto  (BYTE * pb);    // Copy of added data. pb assumed to be large enough
    int         Size        (void);

private:
    CABBlock *  m_pLast;
    CABBlock *  m_pFirst;
    int         m_cbBlock;

    CByteArrayBuilder (); // NOT IMPLEMENTED - DO NOT USE
    // can't copy
    CByteArrayBuilder(const CByteArrayBuilder&) {}
    CByteArrayBuilder& operator=(const CByteArrayBuilder&) { return *this; }
};

inline CByteArrayBuilder::CByteArrayBuilder (int cbBlock) :
    m_pLast     (NULL),
    m_pFirst    (NULL),
    m_cbBlock   (cbBlock)
{
}

//////////////////////////////////////////////////////////////////
// CStructArrayBuilder
//
// Construct with the number of elements per block.
//
// When finished adding elements, call Gimme to obtain a contiguous
// array of elements. After Gimme, the builder is empty and the caller 
// owns the memory.
//
template <class T> 
class CStructArrayBuilder : public CByteArrayBuilder
{
public:
    CStructArrayBuilder (int celPerBlock) :
        CByteArrayBuilder (celPerBlock*sizeof(T))
    {
    }
    HRESULT Add     (T * pT)              { return CByteArrayBuilder::Add (    sizeof(T), reinterpret_cast<BYTE*>(pT)); }
    HRESULT Add     (int cel, T * pT)     { return CByteArrayBuilder::Add (cel*sizeof(T), reinterpret_cast<BYTE*>(pT)); }
    HRESULT Gimme   (int * pcel, T ** ppT)
    {
        if (!pcel || !ppT) return E_POINTER;
        int     cel = 0;
        HRESULT hr  = CByteArrayBuilder::Gimme (&cel, reinterpret_cast<BYTE**>(ppT));
        *pcel = (int)(cel ? cel/sizeof(T) : 0);
        return hr;
    }
};

//////////////////////////////////////////////////////////////////
// CPtrArrayBuilder
//
// Construct with the number of pointers per block.
//
// When finished adding elements, call Gimme to obtain a contiguous
// array of elements. After Gimme, the builder is empty and the caller 
// owns the memory.
//

class CPABBlock; // private array builder block class

void __stdcall VSDestroyElement  (void *pv); // destroys an element with VSFree
void __stdcall SysDestroyElement (BSTR bstr);// destroys an element with SysFreeString

class CPtrArrayBuilderImpl
{
public:
    CPtrArrayBuilderImpl   (int BlockSize) :
        m_pLast     (NULL),
        m_pFirst    (NULL),
        m_BlockSize (BlockSize)
    {
    }
    ~CPtrArrayBuilderImpl  () 
    { 
        Clear(); 
    }

    HRESULT     Add             (int cel, void ** ppv);
    HRESULT     Add             (void * pv);
    void **     Gimme           (int * pcel);

    void        StreamInto      (void ** ppv);  // Copy of added data. ppv assumed to be large enough
    int         Size            (void);         // elements, NOT bytes!
    void        Clear           (void);
    void        FreeElements    (void) { DestroyElements(VSDestroyElement); }
    void        DestroyElements (void (__stdcall *pfnDestroy)(void *));

private:
    CPABBlock * m_pLast;
    CPABBlock * m_pFirst;
    int         m_BlockSize;

    CPtrArrayBuilderImpl (); // NOT IMPLEMENTED - DO NOT USE
    // can't copy
    CPtrArrayBuilderImpl(const CPtrArrayBuilderImpl&) {}
    CPtrArrayBuilderImpl& operator=(const CPtrArrayBuilderImpl&) { return *this; }
};

template <class T>
class CPtrArrayBuilder : public CPtrArrayBuilderImpl
{
public:
    CPtrArrayBuilder (int celBlock) : CPtrArrayBuilderImpl(celBlock) {}
    HRESULT     Add     (int cel, T** ppT)       { return CPtrArrayBuilderImpl::Add(cel, ppT); }
    HRESULT     Add     (T * pT)                 { return CPtrArrayBuilderImpl::Add(pT); }
    T**         Gimme   (int * pcel) { return (T**)CPtrArrayBuilderImpl::Gimme(pcel); }
    void        DestroyElements (void (__stdcall *pfnDestroy)(T *))
    {
        CPtrArrayBuilderImpl::DestroyElements((void (__stdcall *)(void *))pfnDestroy);
    }
};

//////////////////////////////////////////////////////////////////
// Set*ArrayFromBuilder - transfers data from a C*ArrayBuilder to a C*Array
//
template <class T>
inline BOOL SetStructArrayFromBuilder (CStructArray<T> & Array, CStructArrayBuilder<T> & Builder)
{
    int cel;
    T * pT;

    if (SUCCEEDED(Builder.Gimme(&cel, &pT)))
        return Array.Attach(cel, pT);
    else
        return FALSE;
}

template <class T>
inline BOOL SetPtrArrayFromBuilder (CPtrArray<T> & Array, CPtrArrayBuilder<T> & Builder)
{
    int cel;
    T ** prgT;

    if (SUCCEEDED(Builder.Gimme(&cel, &prgT)))
        return Array.Attach(cel, prgT);
    else
        return FALSE;
}

