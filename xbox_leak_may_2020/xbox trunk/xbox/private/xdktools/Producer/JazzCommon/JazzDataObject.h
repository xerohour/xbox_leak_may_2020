////////////////////////////////////////////////////////////////////////////////////////////////////
// JazzDataObject.h : Declaration of the CJazzDataObject
//
//	The purpose of CJazzDataObject is to wrap the IDataObject interface to make implementation of
//	edit operations and drag 'n' drop functionality simpler for Jazz components.  Jazz components
//	should access the additional functions when using CJazzDataObjects rather than directly
//	accessing the interface methods.  See the implementation of each function for its purpose.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __JAZZDATAOBJECT_H_
#define __JAZZDATAOBJECT_H_

#include <objbase.h>
#include "JazzEnumFormatEtc.h"

class CJazzDataObject : public IDataObject
{
public:
    CJazzDataObject(IStream *pIStream = NULL);
	virtual ~CJazzDataObject();

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
	virtual HRESULT Initialize(UINT uClipFormat, IStream *pIStream);
	virtual HRESULT GetIStream(IStream **ppIStream);
	virtual HRESULT SetIStream(IStream *pIStream);
	virtual HRESULT AddClipFormat(UINT uClipFormat);
	virtual HRESULT RemoveClipFormat(UINT uClipFormat);
	virtual HRESULT SetCurrentClipFormat(UINT uClipFormat);
	virtual HRESULT	DataObjectHasClipFormat(UINT uClipFormat, IDataObject *pIDataObject);
	virtual HRESULT AttemptRead(IDataObject *pIDataObject);

	// Attributes
protected:
	DWORD				m_dwRef;
	BOOL				m_bValid;			// TRUE when this object has valid stream and Enum
											// pointers: (m_pIStream != NULL && m_pJEF != NULL)
	IStream				*m_pIStream;		// Pointer to the stream containing the current data.
	FORMATETC			m_feCurrentFormat;	// The current format 
	CJazzEnumFormatEtc	*m_pEnumFormats;	// Enumeration interface used by OLE to get the list of
											// supported formats.
};

#endif