#include "stdafx.h"
#include "errcontext2.h"
#include "bldpkg.h"
#include "output.h"
#include "path2.h"

HRESULT ShowToolWindow(REFGUID guidWindow)
{
	CComPtr<IVsUIShell> spUIShell;
	HRESULT hr = GetBuildPkg()->GetVsUIShell(&spUIShell);
	VSASSERT(SUCCEEDED(hr), "Failed to get IVsUIShell interface");
	RETURN_ON_FAIL_OR_NULL(hr, spUIShell);

	CComPtr<IVsWindowFrame> spFrame;
	hr = spUIShell->FindToolWindow(0, guidWindow, &spFrame);
	RETURN_ON_FAIL_OR_NULL2(hr, spFrame, S_FALSE);

	//...and if we find it, show it.
	return spFrame->Show();
}

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ CBldOutputWinEC --------------------------------
//
///////////////////////////////////////////////////////////////////////////////

// An error context for writing to the output window through the CVPROJIdeInterface:
#define MAX_OUTPUT  2047	// Max chars debugger can show in output window

CBldOutputWinEC::CBldOutputWinEC()
{
	m_nWarnings = 0;
	m_nErrors = 0;
	m_bShowOutput = VARIANT_TRUE;
}

CBldOutputWinEC::~CBldOutputWinEC()
{
	if (m_spModelessOutputWindow)
		m_spModelessOutputWindow->CloseAll();
}

STDMETHODIMP CBldOutputWinEC::get_Errors( long *pnErr )
{
	CHECK_POINTER_NULL(pnErr);

	*pnErr = m_nErrors;
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::get_Warnings( long *pnWrn )
{
	CHECK_POINTER_NULL(pnWrn);

	*pnWrn = m_nWarnings;
	return S_OK;
}

HRESULT CBldOutputWinEC::CreateInstance(IVCBuildErrorContext** ppContext, CBldOutputWinEC** ppContextObj, 
	IVCBuildEngine* pBldEngine, BSTR dir, IVCBuildOutput* pModelessOutputWindow)
{
	if (ppContext)
		*ppContext = NULL;
	if (ppContextObj)
		*ppContextObj = NULL;

	IVCBuildErrorContext *pVar;
	CComObject<CBldOutputWinEC> *pObj;
	HRESULT hr = CComObject<CBldOutputWinEC>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = (IVCBuildErrorContext*)pObj;
		pVar->AddRef();
		if (ppContext)
			*ppContext = pVar;
		if (ppContextObj)
			*ppContextObj = pObj;
			
		CStringW strDir = dir;
		if (!strDir.IsEmpty())
			pObj->m_dir.CreateFromString(strDir);
		pObj->m_spBuildEngine = pBldEngine;
		pObj->m_spModelessOutputWindow = pModelessOutputWindow;
	}
	return(hr);
}

STDMETHODIMP CBldOutputWinEC::get_BaseDirectory(BSTR* pbstrBaseDir)
{
	CHECK_POINTER_NULL(pbstrBaseDir);

	if (m_dir.IsInit())
	{
		CStringW strDir = m_dir;
		*pbstrBaseDir = strDir.AllocSysString();
	}
	else
		*pbstrBaseDir = NULL;

	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::put_BaseDirectory(BSTR bstrBaseDir)
{
	CStringW strDir = bstrBaseDir;
	if (m_dir.CreateFromString(strDir))
		return S_OK;

	return E_FAIL;
}

STDMETHODIMP CBldOutputWinEC::get_ProjectName(BSTR* pbstrBaseDir)
{
	return m_bstrName.CopyTo(pbstrBaseDir);
}

STDMETHODIMP CBldOutputWinEC::put_ProjectName(BSTR bstrName)
{
	m_bstrName = bstrName;
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::get_AssociatedBuildEngine(IDispatch** ppBldEngine)
{
	CHECK_POINTER_NULL(ppBldEngine);
	CComQIPtr<IDispatch> spDispBldEngine = m_spBuildEngine;
	*ppBldEngine = spDispBldEngine.Detach();
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::put_AssociatedBuildEngine(IDispatch* pBldEngine)
{
	m_spBuildEngine = pBldEngine;
	return S_OK;
}
STDMETHODIMP CBldOutputWinEC::get_ShowOutput(VARIANT_BOOL* pbShow)
{
	CHECK_POINTER_NULL(pbShow);
	*pbShow = m_bShowOutput;
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::put_ShowOutput(VARIANT_BOOL bShow)
{
	m_bShowOutput = bShow;
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::Close()
{
	int iCnt; // when this is zero, we close out too...
	if (m_spModelessOutputWindow)
		m_spModelessOutputWindow->CloseWithCount(&iCnt);
	return S_OK;
}

HRESULT CBldOutputWinEC::DetermineWindowActivation(VARIANT_BOOL bForce, VSHPROPID propid, BOOL& bActivate)
{
	if (bForce == VARIANT_TRUE)
	{
		bActivate = TRUE;
		return S_OK;
	}

	bActivate = FALSE;
	CComPtr<IVsShell> spVsShell;
	HRESULT hr = GetBuildPkg()->GetVsShell(&spVsShell);
	RETURN_ON_FAIL2(hr, S_FALSE);

	CComVariant var;
	spVsShell->GetProperty(propid, &var);
	VSASSERT(SUCCEEDED(hr) && var.vt == VT_BOOL, "Failed to get ShowOutputPaneOnBuildBegin/ShowTasklistOnBuildEnd property");
	if (V_BOOL(&var))
		bActivate = TRUE;

	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::ActivateWindow(VARIANT_BOOL bForce)
{
	if (m_spModelessOutputWindow == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	m_spModelessOutputWindow->Activate();

	BOOL bActivate = FALSE;
	HRESULT hr = DetermineWindowActivation(bForce, VSSPROPID_ShowOutputPaneOnBuildBegin, bActivate);
	if (hr != S_OK || !bActivate)
		return hr;

	return ShowToolWindow(GUID_OutputWindow);
}

STDMETHODIMP CBldOutputWinEC::ActivateTaskList(VARIANT_BOOL bForce)
{	// ONLY activates task list if task list is supposed to be activated at the end of a build
	if (bForce == VARIANT_FALSE && m_nErrors == 0)
		return S_OK;

	BOOL bActivate = FALSE;
	HRESULT hr = DetermineWindowActivation(bForce, VSSPROPID_ShowTasklistOnBuildEnd, bActivate);
	if (hr != S_OK || !bActivate)
		return hr;

	CComPtr<IVsTaskList> spTaskList;
	hr = GetBuildPkg()->GetTaskList(&spTaskList);
	RETURN_ON_FAIL_OR_NULL(hr, spTaskList);

	spTaskList->AutoFilter2(GUID_VsTaskListViewCompilerTasks);
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::ClearWindow()
{
	if (m_spModelessOutputWindow)
		m_spModelessOutputWindow->Clear();
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::AddError( BSTR bstrMessage , BSTR bstrHelpKeyword, BSTR bstrFile, long nLine, BSTR bstrFullMsg)
{
	m_nErrors++;
	if (m_bShowOutput == VARIANT_FALSE)
		return S_OK;

	// Format the Message
	CComBSTR bstrFullMessage(bstrFullMsg);
	if( bstrFullMessage.Length() == 0 )
	{
		if( nLine )
		{
			CStringW strFullMsg;
			strFullMsg.Format(L"%s(%d) : error %s : %s\n",bstrFile, nLine, bstrHelpKeyword, bstrMessage);
			bstrFullMessage = strFullMsg;
		}
		else
		{
			CStringW strFullMsg;
			strFullMsg.Format(L"%s : error %s : %s\n",bstrFile, bstrHelpKeyword, bstrMessage);
			bstrFullMessage = strFullMsg;
		}
	}
	else
	{
		bstrFullMessage.Append("\n");
	}
	
	// First Write to the log
	if(m_spBuildEngine)
	{
		VARIANT_BOOL bHaveLogging = VARIANT_FALSE;
		HRESULT hr = m_spBuildEngine->get_HaveLogging(&bHaveLogging);
		if( hr == S_OK && bHaveLogging == VARIANT_TRUE)
			m_spBuildEngine->LogTrace(eLogOutput, bstrFullMessage);
	}

	// Then write to the output window and task list
	if (m_spModelessOutputWindow != NULL) 
	{
		CComQIPtr<IVsOutputWindowPane> pPane;
		IUnknown* pUnkPane = NULL;
		m_spModelessOutputWindow->get_OutputWindowPane(&pUnkPane);
		pPane = pUnkPane;
		if( pPane )
		{
			if( nLine > 0 )
			{
				pPane->OutputTaskItemStringEx(bstrFullMessage, TP_HIGH, CAT_BUILDCOMPILE, L"", BMP_COMPILE, bstrFile, nLine-1, bstrMessage, bstrHelpKeyword);
			}
			else
			{
				pPane->OutputTaskItemStringEx(bstrFullMessage, TP_HIGH, CAT_BUILDCOMPILE, L"", BMP_COMPILE, bstrFile, nLine, bstrMessage, bstrHelpKeyword);
			}
			pPane->FlushToTaskList();
		}
		if( pUnkPane )
		{
			pUnkPane->Release();
		}
	}
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::AddWarning( BSTR bstrMessage, BSTR bstrHelpKeyword, BSTR bstrFile, long nLine, BSTR bstrFullMsg )
{
	m_nWarnings++;
	if (m_bShowOutput == VARIANT_FALSE)
		return S_OK;

	// Format the Message
	CComBSTR bstrFullMessage(bstrFullMsg);
	if( bstrFullMessage.Length() == 0 )
	{
		if( nLine )
		{
			CStringW strFullMsg;
			strFullMsg.Format(L"%s(%d) : warning %s : %s\n",bstrFile, nLine, bstrHelpKeyword, bstrMessage);
			bstrFullMessage = strFullMsg;
		}
		else
		{
			CStringW strFullMsg;
			strFullMsg.Format(L"%s : warning %s : %s\n",bstrFile, bstrHelpKeyword, bstrMessage);
			bstrFullMessage = strFullMsg;
		}
	}
	else
	{
		bstrFullMessage.Append(L"\n");
	}
	
	// First Write to the log
	if(m_spBuildEngine)
	{
		VARIANT_BOOL bHaveLogging = VARIANT_FALSE;
		HRESULT hr = m_spBuildEngine->get_HaveLogging(&bHaveLogging);
		if( hr == S_OK && bHaveLogging == VARIANT_TRUE)
			m_spBuildEngine->LogTrace(eLogOutput, bstrFullMessage);
	}

	// Then write to the output window and task list
	if (m_spModelessOutputWindow != NULL) 
	{
		CComQIPtr<IVsOutputWindowPane> pPane;
		IUnknown* pUnkPane = NULL;
		m_spModelessOutputWindow->get_OutputWindowPane(&pUnkPane);
		pPane = pUnkPane;
		if( pPane )
		{
			if( nLine > 0 )
			{
				pPane->OutputTaskItemStringEx(bstrFullMessage, TP_NORMAL, CAT_BUILDCOMPILE, L"", BMP_COMPILE, bstrFile, nLine-1, bstrMessage, bstrHelpKeyword );
			}
			else
			{
				pPane->OutputTaskItemStringEx(bstrFullMessage, TP_NORMAL, CAT_BUILDCOMPILE, L"", BMP_COMPILE, bstrFile, nLine, bstrMessage, bstrHelpKeyword );
			}
			pPane->FlushToTaskList();
		}
		if( pUnkPane )
		{
			pUnkPane->Release();
		}
	}
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::AddInfo( BSTR bstrMessage )
{
	if (m_bShowOutput == VARIANT_FALSE)
		return S_OK;

	// First Write to the log
	if(m_spBuildEngine)
	{
		VARIANT_BOOL bHaveLogging = VARIANT_FALSE;
		HRESULT hr = m_spBuildEngine->get_HaveLogging(&bHaveLogging);
		if( hr == S_OK && bHaveLogging == VARIANT_TRUE)
			m_spBuildEngine->LogTrace(eLogOutput, bstrMessage);
	}

	// Then write to the output window and task list
	if (m_spModelessOutputWindow != NULL) 
	{
		IUnknown* pUnkPane = NULL;
		m_spModelessOutputWindow->get_OutputWindowPane(&pUnkPane);
		CComQIPtr<IVsOutputWindowPane> pPane = pUnkPane;
		if( pPane )
		{
			HRESULT hr = pPane->OutputString(bstrMessage);
		}
		if( pUnkPane )
			pUnkPane->Release();
	}
	return S_OK;
}

STDMETHODIMP CBldOutputWinEC::AddLine(BSTR bstrMessage )
{
	return AddInfo(bstrMessage);
}


STDMETHODIMP CBldOutputWinEC::WriteLog(BSTR bstrString)
{
	if(m_spBuildEngine)
	{
		VARIANT_BOOL bHaveLogging = VARIANT_FALSE;
		HRESULT hr = m_spBuildEngine->get_HaveLogging(&bHaveLogging);
		if( hr == S_OK && bHaveLogging == VARIANT_TRUE)
			m_spBuildEngine->LogTrace(eLogOutput, bstrString);
	}

	return S_OK;
}

