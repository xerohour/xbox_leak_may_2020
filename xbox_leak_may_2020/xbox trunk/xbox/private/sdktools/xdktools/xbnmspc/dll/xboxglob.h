/*****************************************************************************
 *
 *	xboxglob.h - HGLOBAL babysitting
 *
 *	Included from xboxview.h
 *
 *****************************************************************************/

#ifndef _XBOXGLOB_H
#define _XBOXGLOB_H

/*****************************************************************************
 *
 *	This is the generic IUnknown wrapper that GlobalFree's its
 *	charge when released.
 *
 *****************************************************************************/

/*****************************************************************************
 *
 *	CXboxGlob
 *
 *	CXboxGlob is used as the punkForRelease when we hand a shared HGLOBAL
 *	out to somebody else.  We can't simply use the DataObject itself,
 *	because that would result in havoc if the DataObject itself decided
 *	to free the HGLOBAL while there were still outstanding references to it.
 *
 *	It's also used when we need a refcounted string.  Just put the
 *	string into the hglob and have fun.
 *
 *****************************************************************************/

class CXboxGlob          : public IUnknown
{
public:
    //////////////////////////////////////////////////////
    // Public Interfaces
    //////////////////////////////////////////////////////
    
    // *** IUnknown ***
    virtual STDMETHODIMP_(ULONG) AddRef(void);
    virtual STDMETHODIMP_(ULONG) Release(void);
    virtual STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);

public:
    CXboxGlob();
    ~CXboxGlob(void);


    // Public Member Functions
    HRESULT SetHGlob(HGLOBAL hglob) {m_hglob = hglob; return S_OK;};
    HGLOBAL GetHGlob(void) {return m_hglob;};
    LPCTSTR GetHGlobAsTCHAR(void) {return (LPCTSTR) m_hglob;};


    // Friend Functions
    friend IUnknown * CXboxGlob_Create(HGLOBAL hglob);
    friend CXboxGlob * CXboxGlob_CreateStr(LPCTSTR pszStr);

protected:
    // Private Member Variables
    int m_cRef;

    HGLOBAL                 m_hglob;		// The HGLOBAL we are babysitting
};

#endif // _XBOXGLOB_H
