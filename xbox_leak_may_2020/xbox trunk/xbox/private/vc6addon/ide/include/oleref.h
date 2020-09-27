// compview.h : interface of the CComponentView class
//

#ifndef __OLEREF_H__
#define __OLEREF_H__

#ifdef _DEBUG
#define TRACEOLE(sz)            (if (afxTraceFlags & traceOle) \
								::AfxTrace(_T("%s"), _T(sz)))	
#else   // _DEBUG
#define TRACEOLE(sz)
#endif

void FreeOlePtr(void * pv);

// OleRelease - Decrement an arbitrary OLE interface (if its not NULL) and set it
//             to NULL (NOTE: The call to Release is made through a temporary pointer)
template <class TYPE> inline unsigned long OleRelease
(
TYPE&	rpunk
)
	{
	if (rpunk)
		{
		TYPE	punkTemp = rpunk;
		rpunk = NULL;
		return punkTemp->Release();
		}
	return 0;
	}
	
/*-----------------------------------------------------------------------------
Name:	COleRef

Description:
This class performs automatic reference count decrementing (users must ensure
that AddRef has already been called)on interface pointers.
-----------------------------------------------------------------------------*/
template <class TYPE> class COleRef					// Hungarian:	srp
{
public:
	COleRef();					// Interface is initialized with NULL
	COleRef(TYPE* punk);		// Passed interface is saved
	COleRef(IUnknown *pIUnknown, REFIID riid); // QueryInterface() for riid
	~COleRef();

	operator TYPE*() const;

	TYPE *	operator->() const;
	TYPE**	operator&();		// Asserts emptiness, use InOut for passing as an IN/OUT parameter
	TYPE*	operator=(TYPE* punk);

	TYPE*			Disown();		// Set internal pointer to NULL (w/o calling Release on the interface)
	TYPE**			InOut();		// Same as address-of operator, use instead when passing as in IN/OUT parameter
	const TYPE*&	Reference();	// Return a const reference
	ULONG			SRelease();		// Release the interface and set internal pointer to NULL
	HRESULT			AssignFromQI(IUnknown *pIUnknown, REFIID riid); // Do QI on pIUnknown for riid and assign to this
	BOOL			IsNull() const;

protected:
	TYPE*	m_punk;
	
private:
	// Dis-allow copy and assignment
	COleRef(const COleRef<TYPE>& srp);
	COleRef<TYPE>& operator=(const COleRef<TYPE>& srp);
};

/*-----------------------------------------------------------------------------
Name:	COleMalloc

Description:
This class automatically releases the memory allocated by the OLE IMalloc
interface.
-----------------------------------------------------------------------------*/
template <class TYPE> class COleMalloc	
{
public:
	COleMalloc(TYPE* pItem = NULL);
	~COleMalloc();

	operator TYPE*() const;

//	TYPE *	operator->() const;
	TYPE&	operator*() const;
	TYPE**	operator&();
	TYPE*	operator=(TYPE* pItem);

	void	Delete();					// Delete the memory and set internal pointer to NULL
	TYPE*	Disown();					// Set the internal pointer to NULL (w/o deleting the memory)

private:
	TYPE*	m_pItem;

	// Dis-allow copy and assignment
	COleMalloc(const COleMalloc<TYPE>& smp);
	COleMalloc<TYPE>& operator=(const COleMalloc<TYPE>& smp);
};


/*-----------------------------------------------------------------------------
Name:	COleStrPtr 

Description:
This class automatically releases the string allocated by the OLE memory allocator
on destruction.
-----------------------------------------------------------------------------*/
class COleStrPtr : public COleMalloc<WCHAR>	// Hungarian:	smsz
{
public:
	COleStrPtr(WCHAR * pItem = NULL) : COleMalloc<WCHAR>(pItem) { };

private:
	// Dis-allow copy and assignment
	COleStrPtr(const COleStrPtr& sstr);
	COleStrPtr& operator=(const COleStrPtr& sstr);
};


/*-----------------------------------------------------------------------------
Name:	COleRef::COleRef

Description:
Construct a COleRef by taking ownership of the passed interface (if any)
-----------------------------------------------------------------------------*/
template <class TYPE> inline COleRef<TYPE>::COleRef()
 : m_punk(NULL)
	{
	return;
	}

template <class TYPE> inline COleRef<TYPE>::COleRef
(
TYPE*	punk		// IUnknown interface pointer to reference count
) :
m_punk(punk)
	{
	return;
	}

template <class TYPE> inline COleRef<TYPE>::COleRef
(
IUnknown *pIUnknown,
REFIID riid
) : m_punk(NULL)
{
	void *pvTemp = NULL;
	ASSERT(pIUnknown != NULL);
	HRESULT hr = pIUnknown->QueryInterface(riid, &pvTemp);
	if (SUCCEEDED(hr))
		m_punk = reinterpret_cast<TYPE *>(pvTemp);
}

/*-----------------------------------------------------------------------------
Name:	COleRef::~COleRef

Description:
Destroy a COleRef object by decrementing the reference count of the
saved interface.
-----------------------------------------------------------------------------*/
template <class TYPE> inline COleRef<TYPE>::~COleRef()
	{
	OleRelease(m_punk);
	return;
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::operator TYPE*

Description:
Convert a COleRef to a TYPE*
-----------------------------------------------------------------------------*/
template <class TYPE> inline COleRef<TYPE>::operator TYPE*() const
	{
	return m_punk;
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::operator->

Description:
Return interface pointer.
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE* COleRef<TYPE>::operator->() const
	{
	ASSERT(m_punk);
	return m_punk;
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::operator&

Description:
Return the address of the pointer
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE** COleRef<TYPE>::operator&()
	{
	ASSERT(!m_punk);
	return &m_punk;
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::operator=

Description:
Assign a new interface
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE* COleRef<TYPE>::operator=
(
TYPE*	punk
)
	{
	ASSERT(!m_punk);
	return (m_punk = punk);
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::Disown

Description:
Drop ownership of the interface (without calling Release)
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE* COleRef<TYPE>::Disown()
	{
	TYPE*	punk = m_punk;
	m_punk = NULL;
	return punk;
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::InOut

Description:
Return the address of the pointer (without asserting that it's empty)
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE** COleRef<TYPE>::InOut()
	{
	return &m_punk;
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::Reference

Description:
Return a const reference to the pointer
-----------------------------------------------------------------------------*/
template <class TYPE> inline const TYPE*& COleRef<TYPE>::Reference()
	{
	ASSERT(m_punk);
	return m_punk;
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::SRelease

Description:
Release the held resource and clear the internal pointer
-----------------------------------------------------------------------------*/
template <class TYPE> inline ULONG COleRef<TYPE>::SRelease()
	{
	return OleRelease(m_punk);
	}


/*-----------------------------------------------------------------------------
Name:	COleRef::AssignFromQI

Description:
Queries a passed in IUnknown * for a given interface and assigns it to this.
-----------------------------------------------------------------------------*/
template <class TYPE> inline HRESULT COleRef<TYPE>::AssignFromQI
(
IUnknown *pIUnknown,
REFIID riid
)
{
	void *pvTemp = NULL;
	HRESULT hr;

	ASSERT(pIUnknown != NULL);
	ASSERT(m_punk == NULL);

	hr = pIUnknown->QueryInterface(riid, &pvTemp);
	if (SUCCEEDED(hr))
	{
		m_punk = reinterpret_cast<TYPE *>(pvTemp);
	}

	return hr;
}

/*-----------------------------------------------------------------------------
Name:	COleRef::IsNull

Description:
Returns TRUE if the COleRef does not reference an object currently.
-----------------------------------------------------------------------------*/
template <class TYPE> inline BOOL COleRef<TYPE>::IsNull() const
{
	return (m_punk == NULL);
}

/*-----------------------------------------------------------------------------
Name:	COleMalloc::COleMalloc

Description:
Construct a COleMalloc object by allocating on the heap an instance of the
associated object.
-----------------------------------------------------------------------------*/
template <class TYPE> inline COleMalloc<TYPE>::COleMalloc
(
TYPE*	pItem
)	: m_pItem(pItem)
	{
	return;
	}

/*-----------------------------------------------------------------------------
Name:	COleMalloc::~COleMalloc

Description:
Construct a COleMalloc object by allocating on the heap an instance of the
associated object.
-----------------------------------------------------------------------------*/
template <class TYPE> inline COleMalloc<TYPE>::~COleMalloc()
	{
	if (m_pItem != NULL)
		::FreeOlePtr(m_pItem);
	}
	

/*-----------------------------------------------------------------------------
Name:	COleMalloc::operator TYPE*

Description:
Overload the "TYPE*" conversion operator so that instances of COleMalloc may
be treated exactly like instances of TYPE*.
-----------------------------------------------------------------------------*/
template <class TYPE> inline COleMalloc<TYPE>::operator TYPE*() const
	{
	return m_pItem;
	}


/*-----------------------------------------------------------------------------
Name:	COleMalloc::operator->

Description:
Overload the "->" operator so that instances of COleMalloc may be used as
real pointers.
-----------------------------------------------------------------------------*/
/*template <class TYPE> inline TYPE* COleMalloc<TYPE>::operator->() const
	{
	ASSERT(m_pItem);
	return m_pItem;
	}
*/

/*-----------------------------------------------------------------------------
Name:	COleMalloc::operator*

Description:
Overload the "*" (indirection) operator so that instances of COleMalloc may
be used as real pointers.
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE& COleMalloc<TYPE>::operator*() const
	{
	ASSERT(m_pItem);
	return *m_pItem;
	}


/*-----------------------------------------------------------------------------
Name:	COleMalloc::operator&

Description:
Overload the "&" operator so that instances may be passed to functions
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE** COleMalloc<TYPE>::operator&()
	{
	ASSERT(!m_pItem);
	return &m_pItem;
	}


/*-----------------------------------------------------------------------------
Name:	COleMalloc::operator=

Description:
Overload the "=" operator to allow replacement of the pointer (the existing
pointer, if any, is deleted)
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE* COleMalloc<TYPE>::operator=
(
TYPE*	pItem
)
	{
	ASSERT(!m_pItem);
	return (m_pItem = pItem);
	}


/*-----------------------------------------------------------------------------
Name:	COleMalloc::Delete

Description:
Free the held resource and clear the internal pointer
-----------------------------------------------------------------------------*/
template <class TYPE> inline void COleMalloc<TYPE>::Delete()
	{
	if (m_pItem)
		{
		::FreeOlePtr(m_pItem);
		m_pItem = NULL;
		}
	return;
	}


/*-----------------------------------------------------------------------------
Name:	COleMalloc::Disown

Description:
Drop ownership of the pointer
-----------------------------------------------------------------------------*/
template <class TYPE> inline TYPE* COleMalloc<TYPE>::Disown()
	{
	TYPE*	pItem = m_pItem;
	m_pItem = NULL;
	return pItem;
	}

/////////////////////////////////////////////////////////////////////////////
// random useful stuff

#define IMPLEMENT_METHOD_(rettype, base, derived, method, defargs, useargs)		\
	rettype base::X##derived::method defargs									\
	{																			\
		METHOD_PROLOGUE(base, derived)											\
		return pThis->External##method useargs;									\
	}

#define IMPLEMENT_ADDREF(base, derived)						\
	IMPLEMENT_METHOD_(ULONG, base, derived, AddRef, (), ())
#define IMPLEMENT_RELEASE(base, derived)					\
	IMPLEMENT_METHOD_(ULONG, base, derived, Release, (), ())
#define IMPLEMENT_QUERYINTERFACE(base, derived)				\
	IMPLEMENT_METHOD_(HRESULT, base, derived, QueryInterface,		\
									 (REFIID iid, void FAR* FAR* ppvObj), (&iid, ppvObj))


#endif //__OLEREF_H__
