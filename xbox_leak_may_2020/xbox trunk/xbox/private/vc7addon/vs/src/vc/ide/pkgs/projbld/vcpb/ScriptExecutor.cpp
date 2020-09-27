// ScriptExecutor.cpp: implementation of the CScriptExecutor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comlist.h"
#include "ScriptExecutor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptExecutor::CScriptExecutor()
{
	m_perrors = NULL;
	m_hwnd = NULL;
}

CScriptExecutor::~CScriptExecutor()
{
	if (m_perrors)
	{
		delete m_perrors;
		m_perrors = NULL;
	}
}


STDMETHODIMP CScriptExecutor::put_Wnd(/*[in]*/ HWND hwnd)
{
	m_hwnd = hwnd;
	return S_OK;
}

STDMETHODIMP CScriptExecutor::get_Wnd(/*[out, retval]*/ HWND *phwnd)
{
	*phwnd = m_hwnd;
	return S_OK;
}


STDMETHODIMP CScriptExecutor::Run()
{
	CComVariant var;
	EXCEPINFO excepInfo;
	HRESULT hr = E_FAIL;

	if (m_pParseScript)
	{
		// Add named items
		hr = m_pParseScript->ParseScriptText(m_bstrScript, NULL, NULL, NULL, 0, 1, 0, &var, &excepInfo);
		if (SUCCEEDED(hr))
		{
			// Run script
			hr = m_pScript->SetScriptState(SCRIPTSTATE_CONNECTED);
		}
		else if (hr == DISP_E_EXCEPTION)
		{
			SysFreeString(excepInfo.bstrSource);
			SysFreeString(excepInfo.bstrDescription);
			SysFreeString(excepInfo.bstrHelpFile);
		}
	}

	return hr;
}

STDMETHODIMP CScriptExecutor::EnsureParser()
{
	HRESULT hr = E_NOINTERFACE;
	m_pParseScript = m_pScript;
	if (m_pParseScript)
	{
		if (m_perrors)
		{
			delete m_perrors;
			m_perrors = NULL;
		}

		CComQIPtr<IObjectSafety> pObjectSafety;
		pObjectSafety = m_pParseScript;

		if (pObjectSafety)
		{
			hr = pObjectSafety->SetInterfaceSafetyOptions(__uuidof(IActiveScriptParse),
				INTERFACESAFE_FOR_UNTRUSTED_DATA|INTERFACESAFE_FOR_UNTRUSTED_CALLER,
				INTERFACESAFE_FOR_UNTRUSTED_DATA|INTERFACESAFE_FOR_UNTRUSTED_CALLER);
			if (FAILED(hr))
			{
				hr = pObjectSafety->SetInterfaceSafetyOptions(__uuidof(IActiveScriptParse),
					INTERFACESAFE_FOR_UNTRUSTED_DATA,
					INTERFACESAFE_FOR_UNTRUSTED_DATA);
			}
		}	

		hr = m_pParseScript->InitNew();
		VSASSERT(SUCCEEDED(hr), "Unable to initialize script parser!");
		if (SUCCEEDED(hr))
		{
			CComBSTR bstrItem;

			bstrItem = L"Application";
			hr = m_pScript->AddNamedItem(bstrItem, SCRIPTITEM_ISVISIBLE | 
							 SCRIPTITEM_ISSOURCE | SCRIPTITEM_GLOBALMEMBERS);
			VSASSERT(SUCCEEDED(hr), "Unable to add the Application to the script's environment");
			hr = m_pScript->AddTypeLib(*m_pguid, 1, 0, SCRIPTTYPELIB_ISCONTROL);
		}
	}
	return hr;
}

STDMETHODIMP CScriptExecutor::EnsureEngine()
{
	HRESULT hr = E_NOINTERFACE;
	CLSID clsidEngine;
	CComBSTR bstrScriptEngine;

	bstrScriptEngine = m_bstrEngineName;
	if (bstrScriptEngine.Length() == 0)
	{
		bstrScriptEngine = L"VBScript";
		// LATER (maybe)... get <SCRIPT="VBScript"> from script itself
		// right now, descision is to keep things simpler.
	}

	hr = CLSIDFromProgID(bstrScriptEngine, &clsidEngine);
	if (SUCCEEDED(hr))
	{
		hr = m_pScript.CoCreateInstance(clsidEngine);
		// assert to let us know we couldn't load vbscript
		VSASSERT( SUCCEEDED( hr ), "Unable to create VBScript object. Make sure vbscript.dll is on your system and registered" );
		if (SUCCEEDED(hr))
		{
			hr = m_pScript->SetScriptSite(this);
			VSASSERT(SUCCEEDED(hr), "Unable to set script site");
		}
	}
	return hr;
}

STDMETHODIMP CScriptExecutor::LoadScript(BSTR szProjectPath)
{
	HRESULT hr;

	m_bstrScriptFile = szProjectPath;

	hr = EnsureEngine();
	if (FAILED(hr))
		return hr;

	hr = EnsureParser();
	if (FAILED(hr))
		return hr;

	// load the script...
	FILE *pFile;
	struct _stat stat;
	CString strScript;	// yes, ANSI
	TCHAR * pszScript;	// yes, ANSI

	hr = E_HANDLE;
	m_bstrScript.Empty();
	CString strProjectDir = szProjectPath;	// yes, ANSI
	if (_tstat(strProjectDir, &stat) == 0)
	{
		pszScript = strScript.GetBufferSetLength(stat.st_size);
		pFile = _tfopen(strProjectDir, _T("rb"));
		if (pFile)
		{
			fread(pszScript, stat.st_size, 1, pFile);
			fclose(pFile);
			hr = S_OK;
		}
		strScript.ReleaseBuffer();


		// remove "header"
		int i,j;
		i = strScript.Find(m_strHeaderEnd, 0);
		if (i >= 0)
		{
			j = strScript.Find(_T('\n'), i);
			strScript = strScript.Mid(j);
		}
		
		
		CComBSTR szProjectDir;
//		CString strProjectDir;	// yes, ANSI

//		strProjectDir = szProjectPath;
		i = strProjectDir.ReverseFind(_T('\\'));
		if (i >= 0)
		{
			strProjectDir = strProjectDir.Left(i+1);
		}
		else
		{
			strProjectDir = _T("\\"); // must be at root!
		}
		szProjectDir = strProjectDir;

		m_bstrScript = L"dim ScriptPath\nScriptPath = \"";
		m_bstrScript.Append(szProjectPath);
		m_bstrScript.Append(L"\"\n");
		m_bstrScript.Append(L"dim ScriptDir\nScriptDir = \"");
		m_bstrScript.Append(szProjectDir);
		m_bstrScript.Append(L"\"\n");

		wchar_t *pStr = new wchar_t[strScript.GetLength()+1];
		if( pStr )
		{
			MultiByteToWideChar( CP_ACP,0,strScript,-1, pStr, (int)(strScript.GetLength()+1));
			m_bstrScript.Append(pStr);
			delete []pStr;
		}
	}
	return hr;
}



// ------------ Script Site...

// IActiveScriptSite
STDMETHODIMP CScriptExecutor::GetLCID(ULONG * plcid)
{
	CHECK_POINTER_NULL(plcid);
	return E_NOTIMPL;
}
STDMETHODIMP CScriptExecutor::GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown * * ppiunkItem, ITypeInfo * * ppti)
{
	CHECK_POINTER_NULL(ppiunkItem);

	CComBSTR bstrName = pstrName;
	if (bstrName == L"Application" && m_pApplication)
	{
		if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
		{
			m_pApplication.CopyTo( ppiunkItem );
		}
		if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
		{
			VSASSERT(0, _T("Script engine wanted type info"));
			CHECK_POINTER_NULL(ppti);
			*ppti = NULL;
		}
		return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CScriptExecutor::GetDocVersionString(BSTR * pbstrVersion)
{
	CHECK_POINTER_NULL(pbstrVersion);
	return E_NOTIMPL;
}

STDMETHODIMP CScriptExecutor::OnScriptTerminate(const VARIANT * pvarResult, const EXCEPINFO * pexcepinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CScriptExecutor::OnStateChange(tagSCRIPTSTATE ssScriptState)
{
	return S_OK;
}

STDMETHODIMP CScriptExecutor::OnScriptError(IActiveScriptError * pscripterror)
{
	if (pscripterror == NULL)
		return E_INVALIDARG;

	ULONG ulLineNumber;
	long lCharacterPosition;
	CComBSTR bstrLine;
	DWORD_PTR dwSourceContext;
	EXCEPINFO excepinfo;

	pscripterror->GetSourceLineText(&bstrLine);
	pscripterror->GetSourcePosition(&dwSourceContext, &ulLineNumber, &lCharacterPosition);
	pscripterror->GetExceptionInfo(&excepinfo);
#ifdef _DEBUG
	CStringW strScript, strError, strDesc;
	CString strErrorMsg;	// yes, this needs to be ANSI
	int i, j;

	if (excepinfo.bstrSource)
		strError = excepinfo.bstrSource;
	if (excepinfo.bstrDescription)
		strDesc = excepinfo.bstrDescription;
	strScript = bstrLine;
	i = 1;
	j = i + 1; // just to get the condition later to work
	if (strScript.IsEmpty())
	{
		strScript = m_bstrScript;
		i = 0;
		for (j = ulLineNumber; j > 0 && i >= 0; j--)
		{
			i = strScript.Find(L'\n', i+1);
		}
		j = strScript.Find(L'\n', i+1);
	}
	ulLineNumber -= 1; // account for added lines...
	if (i >= 0 && j > i)
	{
		strScript = strScript.Mid(i+1, j - i);

		AtlTrace(_T("Script Error (%ls): Line %d(%d)\n%ls\n"), m_bstrScriptFile, ulLineNumber, lCharacterPosition, strScript);
		if (!strError.IsEmpty())
		{
			AtlTrace(_T("%ls\n"), strError);
		}
		if (!strDesc.IsEmpty())
		{
			AtlTrace(_T("%ls\n"), strDesc);
		}
		AtlTrace(_T("%ls(%d): %s\n"), m_bstrScriptFile, ulLineNumber, strScript);
		strErrorMsg.Format(_T("%ls(%d): %s\nCheck Debug Output also"), m_bstrScriptFile, ulLineNumber, strScript);
		MessageBox(NULL, strErrorMsg, "VC Load Script Error", MB_OK);
	}
	else
	{
		AtlTrace(_T("Script Error (%ls): Line %d(%d)\n"), m_bstrScriptFile, ulLineNumber, lCharacterPosition);
		if (!strError.IsEmpty())
		{
			AtlTrace(_T("%ls\n"), strError);
		}
		if (!strDesc.IsEmpty())
		{
			AtlTrace(_T("%ls\n"), strDesc);
		}
		AtlTrace(_T("%ls(%d)\n"), m_bstrScriptFile, ulLineNumber);
		strErrorMsg.Format(_T("%ls(%d)\nCheck Debug Output also\n"), m_bstrScriptFile, ulLineNumber);
		MessageBox(NULL, strErrorMsg, "VC Load Script Error", MB_OK);
	}
#endif
	if (m_perrors == NULL)
	{
		m_perrors = new CComDynamicList;
	}
	if (m_perrors)
	{
		m_perrors->Add(pscripterror);
	}
	return S_OK;
}

STDMETHODIMP CScriptExecutor::OnEnterScript()
{
	return E_NOTIMPL;
}

STDMETHODIMP CScriptExecutor::OnLeaveScript()
{
	return E_NOTIMPL;
}

STDMETHODIMP CScriptExecutor::GetWindow(/* [out] */ HWND *phwnd)
{
	CHECK_POINTER_NULL(phwnd);
	*phwnd = m_hwnd;
	return S_OK;
}

STDMETHODIMP CScriptExecutor::EnableModeless(/* [in] */ BOOL fEnable)
{
	return E_NOTIMPL;
}
