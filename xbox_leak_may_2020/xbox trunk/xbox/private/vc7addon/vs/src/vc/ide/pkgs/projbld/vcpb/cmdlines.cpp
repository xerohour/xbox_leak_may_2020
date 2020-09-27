#include "stdafx.h"
#include "CmdLines.h"

/////////////////////////////////////////////////////////////////////////////
//

HRESULT CVCCommandLineList::CreateInstance(IVCCommandLineList** ppCmdLineList)
{
	CHECK_POINTER_NULL(ppCmdLineList);
	*ppCmdLineList = NULL;

	CComObject<CVCCommandLineList> *pObj;
	HRESULT hr = CComObject<CVCCommandLineList>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		CVCCommandLineList *pVar = pObj;
		pVar->AddRef();
		*ppCmdLineList = pVar;
	}
	return hr;
}

STDMETHODIMP CVCCommandLineList::Next(IVCCommandLine **ppCmdLine)
{
	CHECK_POINTER_NULL(ppCmdLine);
	RETURN_ON_NULL2(m_posEnumerator, S_FALSE);

	IVCCommandLine* pCmd = (IVCCommandLine*)m_commands.GetNext(m_posEnumerator);
	if (pCmd)
		pCmd->AddRef();
	*ppCmdLine = pCmd;
	return S_OK;
}

STDMETHODIMP CVCCommandLineList::get_Count(long *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = (long) m_commands.GetCount();
	return S_OK;
}

STDMETHODIMP CVCCommandLineList::Reset()
{
	m_posEnumerator = m_commands.GetHeadPosition();
	return S_OK;
}

STDMETHODIMP CVCCommandLineList::Add(IVCCommandLine *pCmdLine, BOOL bAddFront)
{
	RETURN_ON_NULL2(pCmdLine, S_OK);	// nothing there, nothing to add

	pCmdLine->AddRef();
	if (bAddFront)
		m_commands.AddHead(pCmdLine);
	else
		m_commands.AddTail(pCmdLine);

	return S_OK;
}

STDMETHODIMP CVCCommandLineList::RemoveAll()
{
	m_posEnumerator = m_commands.GetHeadPosition();
	while (m_posEnumerator != NULL)
	{
		IVCCommandLine* pCmd = (IVCCommandLine*)m_commands.GetNext(m_posEnumerator);
		pCmd->Release();
	}
	m_commands.RemoveAll();
	return S_OK;
}

STDMETHODIMP CVCCommandLineList::AddList(IVCCommandLineList* pCmdLineList, BOOL bAddFront)
{
	RETURN_ON_NULL2(pCmdLineList, S_OK);

	pCmdLineList->Reset();
	HRESULT hr = S_OK;
	while (hr == S_OK)
	{
		CComPtr<IVCCommandLine> spCmd;
		hr = pCmdLineList->Next(&spCmd);
		if (FAILED(hr) || hr == S_FALSE)
			break;
		if (spCmd == NULL)
			continue;
		if (bAddFront)
			m_commands.AddHead(spCmd);
		else
			m_commands.AddTail(spCmd);
		spCmd.Detach();
	}

	return S_OK;
}

STDMETHODIMP CVCCommandLineList::get_UseConsoleCodePageForSpawner(VARIANT_BOOL* pbConsoleCP)
{
	CHECK_POINTER_NULL(pbConsoleCP);
	*pbConsoleCP = m_bUseConsoleCodePageForSpawner;
	return S_OK;
}

STDMETHODIMP CVCCommandLineList::put_UseConsoleCodePageForSpawner(VARIANT_BOOL bConsoleCP)
{
	m_bUseConsoleCodePageForSpawner = bConsoleCP;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//

HRESULT CVCCommandLine::CreateInstance(IVCCommandLine** ppCmdLine)
{
	CHECK_POINTER_NULL(ppCmdLine);
	*ppCmdLine = NULL;

	CComObject<CVCCommandLine> *pObj;
	HRESULT hr = CComObject<CVCCommandLine>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		CVCCommandLine *pVar = pObj;
		pVar->AddRef();
		*ppCmdLine = pVar;
	}
	return hr;
}

STDMETHODIMP CVCCommandLine::get_Description(BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	m_bstrDescription.CopyTo(pVal);
	return S_OK;
}

STDMETHODIMP CVCCommandLine::put_Description(BSTR newVal)
{
	m_bstrDescription = newVal;
	return S_OK;
}

STDMETHODIMP CVCCommandLine::get_CommandLineContents(BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	m_bstrCommandLineContents.CopyTo(pVal);
	return S_OK;
}

STDMETHODIMP CVCCommandLine::put_CommandLineContents(BSTR newVal)
{
	m_bstrCommandLineContents = newVal;
	return S_OK;
}

STDMETHODIMP CVCCommandLine::get_NumberOfProcessors(long* pnProcessors)
{
	CHECK_POINTER_NULL(pnProcessors);
	*pnProcessors = m_nProcessors;
	return S_OK;
}

STDMETHODIMP CVCCommandLine::put_NumberOfProcessors(long nProcessors)
{
	m_nProcessors = nProcessors;
	return S_OK;
}
