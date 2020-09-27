// ScriptExecutor.h: interface for the CScriptExecutor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTEXECUTOR_H__D6BB9575_049A_11D3_8D31_00C04F8EEA30__INCLUDED_)
#define AFX_SCRIPTEXECUTOR_H__D6BB9575_049A_11D3_8D31_00C04F8EEA30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <OBJSAFE.H>
#include <activscp.h>
#include "comlist.h"

class CScriptExecutor  : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IActiveScriptSite,
	public IActiveScriptSiteWindow
{
public:
	CScriptExecutor();
	virtual ~CScriptExecutor();
	STDMETHOD(Run)();
	STDMETHOD(LoadScript)(BSTR szProjectPath);
	STDMETHOD(put_Script)(BSTR bstrScript) { m_bstrScript = bstrScript; return S_OK;};
	STDMETHOD(get_Script)(BSTR *pbstrScript) { m_bstrScript.CopyTo(pbstrScript); return S_OK;};
	STDMETHOD(put_strHeaderEnd)(BSTR bstrHeaderEnd) { m_strHeaderEnd = bstrHeaderEnd; return S_OK;};
	STDMETHOD(get_strHeaderEnd)(BSTR *pbstrHeaderEnd) { *pbstrHeaderEnd = m_strHeaderEnd.AllocSysString(); return S_OK;};
	STDMETHOD(put_Application)(IDispatch *pDispApplication) { m_pApplication = NULL; m_pApplication = pDispApplication; return S_OK;};
	STDMETHOD(get_Application)(IUnknown **ppDispApplication) { m_pApplication.CopyTo(ppDispApplication); return S_OK;};
	STDMETHOD(put_EngineName)(BSTR bstrEngineName) { m_bstrEngineName = bstrEngineName; return S_OK;};
	STDMETHOD(get_EngineName)(BSTR *pbstrEngineName) { m_bstrEngineName.CopyTo(pbstrEngineName); return S_OK;};
	STDMETHOD(put_TypeLibraryGuid)(const GUID *pguid) { m_pguid = pguid; return S_OK;};
	STDMETHOD(get_TypeLibraryGuid)(const GUID **ppguid) { *ppguid = m_pguid; return S_OK;};

	STDMETHOD(EnsureEngine)();
	STDMETHOD(EnsureParser)();
	STDMETHOD(put_Wnd)(/*[in]*/ HWND hwnd);
	STDMETHOD(get_Wnd)(/*[out, retval]*/ HWND *phwnd);

	void Close( void )
	{
		if( m_pScript )
			m_pScript->Close();
		m_pApplication.Release();
		m_pParseScript.Release();
		m_pScript.Release();
		if( m_perrors ) 
			m_perrors->RemoveAll();
	}

BEGIN_COM_MAP(CScriptExecutor)
	COM_INTERFACE_ENTRY(IActiveScriptSite)
	COM_INTERFACE_ENTRY(IActiveScriptSiteWindow)
END_COM_MAP()


// IActiveScriptSite
public:
        STDMETHOD(GetLCID)( /* [out] */ LCID *plcid);

        STDMETHOD(GetItemInfo)(
                /* [in] */  LPCOLESTR pstrName,
                /* [in] */  DWORD     dwReturnMask,
                /* [out] */ IUnknown  **ppiunkItem,
                /* [out] */ ITypeInfo **ppti
				);

        STDMETHOD(GetDocVersionString)( /* [out] */ BSTR      *pbstrVersion );

        STDMETHOD(OnScriptTerminate)( /* [in] */  const VARIANT     *pvarResult,
                /* [in] */  const EXCEPINFO   *pexcepinfo );

        STDMETHOD(OnStateChange)( /* [in] */  SCRIPTSTATE       ssScriptState );

        STDMETHOD(OnScriptError)( /* [in] */  IActiveScriptError *pscripterror );

        STDMETHOD(OnEnterScript)(void);

        STDMETHOD(OnLeaveScript)(void);

// IActiveScriptSiteWindow
public:
        STDMETHOD(GetWindow)(/* [out] */ HWND *phwnd);
        STDMETHOD(EnableModeless)(/* [in] */ BOOL fEnable);

protected:
	CComBSTR m_bstrScriptFile;
	CComBSTR m_bstrScript;
	CComBSTR m_bstrEngineName; // eg., L"VBScript"
	CString m_strHeaderEnd; // eg., _T("# ** DO NOT EDIT **")	// yes, ANSI
	CComPtr<IUnknown> m_pApplication; // eg. VCProjectEngine
	CComQIPtr<IActiveScriptParse> m_pParseScript;
	HWND m_hwnd;
	
	const GUID *m_pguid;		// type library to add (single, for now).
	CComPtr<IActiveScript>			m_pScript; // for project load
	CComDynamicList *m_perrors;	// collection of IActiveScriptError's for the current load
};

#endif // !defined(AFX_SCRIPTEXECUTOR_H__D6BB9575_049A_11D3_8D31_00C04F8EEA30__INCLUDED_)
