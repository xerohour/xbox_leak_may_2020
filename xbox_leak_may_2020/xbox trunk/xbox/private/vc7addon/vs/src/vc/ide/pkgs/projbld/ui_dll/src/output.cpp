#include "stdafx.h"
#include "output.h"

CTestableSection CVCBuildOutput::s_sectionOutWindowPane;

/////////////////////////////////////////////////////////////////////////////
// CVCBuildOutput

CVCBuildOutput::CVCBuildOutput()
{
	m_pStreamOutPane = NULL;
}

HRESULT CVCBuildOutput::CreateInstance(IVCBuildOutput** ppOutput, IUnknown* pUnkWindowPane)
{
	HRESULT hr;
	CVCBuildOutput *pVar;
	CComObject<CVCBuildOutput> *pObj;
	hr = CComObject<CVCBuildOutput>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		hr = pVar->InitInstance(pUnkWindowPane);
		pVar->Create();
		*ppOutput = reinterpret_cast<IVCBuildOutput *>(pVar);
	}
	return(hr);
}

HRESULT CVCBuildOutput::InitInstance(IUnknown* pUnkWindowPane)
{
	m_pStreamOutPane = NULL;
	HRESULT hr = AtlMarshalPtrInProc(pUnkWindowPane, IID_IVsOutputWindowPane, &m_pStreamOutPane);
	VSASSERT(SUCCEEDED(hr), "Failed to marshal output window pane; was our stream created in the primary thread (required)?");
	m_spUnkWindowPane = pUnkWindowPane;

	return S_OK;
}

CVCBuildOutput::~CVCBuildOutput()
{
	VSASSERT(m_rgpVsOutputWindowPane.GetCount() == 0, "Still have an output window pane at destruction time!");
	if (m_rgpVsOutputWindowPane.GetCount() != 0)
		CloseAll();

	HRESULT hr = AtlFreeMarshalStream(m_pStreamOutPane);
	VSASSERT(SUCCEEDED(hr), "Failed to free marshal an output window pane");
	m_pStreamOutPane = NULL; // get rid of this... we don't want to re-use this accidently
	m_spUnkWindowPane.Release();
}

STDMETHODIMP CVCBuildOutput::SameWindowPane(IUnknown* pUnkPane)
{
	return (pUnkPane == m_spUnkWindowPane) ? S_OK : S_FALSE;
}

STDMETHODIMP CVCBuildOutput::Close()
{
	return CloseWithCount(NULL);
}

STDMETHODIMP CVCBuildOutput::CloseWithCount(int *pCount)
{
	CritSectionT cs(s_sectionOutWindowPane);

	int idThread = GetCurrentThreadId();
	IVsOutputWindowPane *pPane;
	if (m_rgpVsOutputWindowPane.Lookup(idThread, pPane) != 0)
	{
		pPane->Release();
		m_rgpVsOutputWindowPane.RemoveKey(idThread);
	}
	if (pCount != NULL)
		*pCount = m_rgpVsOutputWindowPane.GetCount();

	return S_OK;
}

STDMETHODIMP CVCBuildOutput::CloseAll()
{
	CritSectionT cs(s_sectionOutWindowPane);

	VCPOSITION pos = m_rgpVsOutputWindowPane.GetStartPosition();
	while (pos)
	{
		IVsOutputWindowPane *pPane;
		int idThread;
		m_rgpVsOutputWindowPane.GetNextAssoc(pos, idThread, pPane);
		if (pPane)
			pPane->Release();
	}
	m_rgpVsOutputWindowPane.RemoveAll();

	return S_OK;
}

IVsOutputWindowPane *CVCBuildOutput::GetOutWindowPane()
{
	HRESULT hr;
	IVsOutputWindowPane *pPane = NULL;

	int id;
	id = GetCurrentThreadId();
	CritSectionT cs(s_sectionOutWindowPane);
	if (m_rgpVsOutputWindowPane.Lookup(id, pPane) == 0 && m_pStreamOutPane != NULL)
	{
		hr = AtlUnmarshalPtr(m_pStreamOutPane, IID_IVsOutputWindowPane, (IUnknown **)&pPane);
		VSASSERT(SUCCEEDED(hr), "Failed to unmarshal output window pane stream!");
		if (pPane)
			m_rgpVsOutputWindowPane[id] = pPane;
	}

	return(pPane);
}

STDMETHODIMP CVCBuildOutput::get_OutputWindowPane(IUnknown** ppOutWindowPane)
{
	CHECK_POINTER_NULL(ppOutWindowPane);

	IVsOutputWindowPane* pPane = GetOutWindowPane();
	RETURN_ON_NULL(pPane);

	pPane->AddRef();
	*ppOutWindowPane = pPane;
	return S_OK;
}

STDMETHODIMP CVCBuildOutput::Create()
{
	return(S_OK);
}

STDMETHODIMP CVCBuildOutput::AddString(BSTR bstr, BOOL bAddCRLF)
{
	IVsOutputWindowPane *pPane = GetOutWindowPane();
	RETURN_ON_NULL(pPane);

	CComBSTR bstrOut;
	if (bAddCRLF)
	{
		CStringW strCRLF = bstr;
		strCRLF += L"\n";
		bstrOut = strCRLF;
	}
	else
		bstrOut = bstr;
	HRESULT hr = pPane->OutputString(bstrOut);
	VSASSERT(SUCCEEDED(hr), "Failed to write a string to the output window pane!");
	return hr;
}

STDMETHODIMP CVCBuildOutput::Clear()
{
	IVsOutputWindowPane *pPane = GetOutWindowPane();
	if (pPane)
		pPane->Clear();

	EnsureVisible();
	return S_OK;
}

STDMETHODIMP CVCBuildOutput::Activate()
{
	IVsOutputWindowPane *pPane = GetOutWindowPane();
	if (pPane)
		pPane->Activate();

	return S_OK;
}

HRESULT CVCBuildOutput::EnsureVisible()
{
/*
	find the containing window for our output window. This assumes that
	we are always contained by this. We also assume that our pane is inside
	this frame.
*/
	return E_FAIL;
}


STDMETHODIMP CVCBuildOutput::FlushToTaskList()
{
	IVsOutputWindowPane *pPane = GetOutWindowPane();
	if (pPane)
		pPane->FlushToTaskList();

	return S_OK;
}


STDMETHODIMP CVCBuildOutput::OutputTaskItemString(LPCOLESTR pszOutputString, LONG /*VSTASKPRIORITY*/ nPriority, 
	LONG /*VSTASKCATEGORY*/ nCategory, LPCOLESTR pszSubcategory, LONG /*VSTASKBITMAP*/ nBitmap, LPCOLESTR pszFilename, 
	ULONG nLineNum, LPCOLESTR pszTaskItemText)

{
	IVsOutputWindowPane *pPane = GetOutWindowPane();
	RETURN_ON_NULL(pPane);

	HRESULT hr = pPane->OutputTaskItemString(pszOutputString, (VSTASKPRIORITY)nPriority, 
		(VSTASKCATEGORY)nCategory, pszSubcategory, (VSTASKBITMAP)nBitmap, pszFilename, nLineNum, pszTaskItemText);
	VSASSERT(SUCCEEDED(hr), "Failed to write a string to the task list; did we get a parameter wrong?");
	if (pPane)
		pPane->FlushToTaskList();
	return hr;
}


