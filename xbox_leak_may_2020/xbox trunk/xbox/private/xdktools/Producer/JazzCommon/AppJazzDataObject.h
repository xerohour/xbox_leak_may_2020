////////////////////////////////////////////////////////////////////////////////////////////////////
// AppJazzDataObject.h : Declaration of the CAppJazzDataObject
//
//	The purpose of CAppJazzDataObject is to insert the proper AFX_MANAGE_STATE macro before
//	calling the base class (CBassJazzDataObject) implementation of each method.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __APPJAZZDATAOBJECT_H_
#define __APPJAZZDATAOBJECT_H_

#include <objbase.h>
#include "BaseJazzDataObject.h"

class CAppJazzDataObject : public CBaseJazzDataObject
{
public:
    CAppJazzDataObject();
	virtual ~CAppJazzDataObject();

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
};

#endif