////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseJazzDataObject.h : Declaration of the CBaseJazzDataObject
//
//	The purpose of CBaseJazzDataObject is to wrap the IDataObject interface to make implementation of
//	edit operations and drag 'n' drop functionality simpler for Jazz components.  Jazz components
//	should access the additional functions when using CBaseJazzDataObjects rather than directly
//	accessing the interface methods.  See the implementation of each function for its purpose.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BASEJAZZDATAOBJECT_H_
#define __BASEJAZZDATAOBJECT_H_

#include <afxtempl.h>
#include <objbase.h>

#include "JazzEnumFormatEtc.h"



/////////////////////////////////////////////////////////////////////////////
// CJzClipData

class CJzClipData
{
// Constructor
public:
	CJzClipData( UINT uClipFormat, IStream* pIStream );
	virtual ~CJzClipData();

// Attributes
public:
	UINT		m_uClipFormat;
	IStream*	m_pIStream;
};


class CBaseJazzDataObject : public IDataObject
{
public:
    CBaseJazzDataObject();
	virtual ~CBaseJazzDataObject();

    // IUnknown functions
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // IDataObject functions
    virtual HRESULT STDMETHODCALLTYPE GetData(
		/* [in] */ FORMATETC *pformatetcIn, 
		/* [out] */ STGMEDIUM *pmedium);
    virtual HRESULT STDMETHODCALLTYPE GetDataHere(
		/* [in] */ FORMATETC *pformatetc,
        /* [out][in] */ STGMEDIUM *pmedium);
    virtual HRESULT STDMETHODCALLTYPE QueryGetData(
		/* [in] */ FORMATETC *pformatetc);
    virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
		/* [in] */ FORMATETC *pformatetcIn,
        /* [out] */ FORMATETC *pformatetcOut);
    virtual HRESULT STDMETHODCALLTYPE SetData(
		/* [in] */ FORMATETC *pformatetc,
        /* [in] */ STGMEDIUM *pmedium,
		/* [in] */ BOOL fRelease);
    virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(
		/* [in] */ DWORD dwDirection,
        /* [out] */ IEnumFORMATETC **ppenumFormatEtc);
    virtual HRESULT STDMETHODCALLTYPE DAdvise(
		/* [in] */ FORMATETC *pformatetc,
        /* [in] */ DWORD advf,
		/* [in] */ IAdviseSink *pAdvSink,
		/* [out] */ DWORD *pdwConnection);
    virtual HRESULT STDMETHODCALLTYPE DUnadvise(
		/* [in] */ DWORD dwConnection);
    virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(
		/* [out] */ IEnumSTATDATA **ppenumAdvise);

	// Additional functions
private:
	IStream* FindStream( UINT uClipFormat );

public:
	virtual HRESULT AddClipFormat( UINT uClipFormat, IStream* pIStream );
	virtual HRESULT	IsClipFormatAvailable( IDataObject *pIDataObject, UINT uClipFormat );
	virtual HRESULT	AttemptRead( IDataObject *pIDataObject, UINT uClipFormat, IStream** ppIStream );

	// Attributes
protected:
	DWORD				m_dwRef;

	CJazzEnumFormatEtc*	m_pEnumFormats;		// Enumeration interface used by OLE to get the list of
											// supported formats.
    CTypedPtrList<CPtrList, CJzClipData*> m_lstClips;
											// List contains clipboard format IDs with associated
											// streams of data.
};

#endif