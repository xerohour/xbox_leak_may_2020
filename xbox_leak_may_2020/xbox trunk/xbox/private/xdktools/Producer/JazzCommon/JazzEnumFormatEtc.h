// JazzEnumFormatEtc.h : Declaration of the CJazzEnumFormatEtc

#ifndef __JAZZENUMFORMATETC_H_
#define __JAZZENUMFORMATETC_H_

#include <objbase.h>

typedef struct FormatList
{
	FormatList *pNext;
	FORMATETC  *pFormat;
} FormatList;

class CJazzEnumFormatEtc : public IEnumFORMATETC
{
public:
	CJazzEnumFormatEtc();
	virtual ~CJazzEnumFormatEtc();

    // IUnknown functions
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid,
		LPVOID *ppv);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

	// IEnumFORMATETC functions
    virtual HRESULT STDMETHODCALLTYPE Next(
		/* [in] */ ULONG celt,
		/* [length_is][size_is][out] */ FORMATETC *rgelt,
        /* [out] */ ULONG *pceltFetched);
    virtual HRESULT STDMETHODCALLTYPE Skip(
		/* [in] */ ULONG celt);
    virtual HRESULT STDMETHODCALLTYPE Reset();
    virtual HRESULT STDMETHODCALLTYPE Clone(
		/* [out] */ IEnumFORMATETC **ppenum);

	// Additional Functions
	virtual HRESULT STDMETHODCALLTYPE AddFormat(
		FORMATETC *pFormat);
	virtual HRESULT STDMETHODCALLTYPE RemoveFormat(
		FORMATETC *pFormat);
	virtual HRESULT STDMETHODCALLTYPE RemoveAllFormats();
	virtual HRESULT STDMETHODCALLTYPE FormatInList(
		UINT cfFormat);

private:
	virtual HRESULT Initialize(
		CJazzEnumFormatEtc *pJEF);

	// Attributes
private:
	DWORD		m_dwRef;
	FormatList	*m_pFormatList;
	FormatList	*m_pCurrentFormat;
};

#endif