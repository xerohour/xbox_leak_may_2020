// CCallExeForDebug.h : Declaration of the CCCallExeForDebug

#ifndef __CCALLEXEFORDEBUG_H_
#define __CCALLEXEFORDEBUG_H_

#include <atlhost.h>

#include <stdafx.h>

#include "utils.h"
#include <vcdlg.h>
#include <register.h>

const WORD rgDebugCommands[] = {
	VCDSID_Command_IExplore,
	VCDSID_Command_ATLTest,
	VCDSID_Command_RegSvr,
	VCDSID_Command_Browse,
};	

#define ELEM_COUNT(x) (sizeof((x))/sizeof((*x)))

/////////////////////////////////////////////////////////////////////////////
// CCCallExeForDebug
class CCCallExeForDebug : 
	public CVCAxDialogImpl<CCCallExeForDebug>
{
public:
	CCCallExeForDebug(VCConfiguration* pConfig, HWND hwndParent) : 
		m_hwndParent( hwndParent ),
		m_exeForDbg(L"ComboBox", this, 1)
	{
		m_spConfig = pConfig;
	}

	~CCCallExeForDebug()
	{
	}

	enum { IDD = IDD_EXE_FOR_DEBUG };

BEGIN_MSG_MAP(CCCallExeForDebug)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(IDC_EXE_FOR_DEBUG_HELP, OnHelp)
	MESSAGE_HANDLER(WM_HELP, OnHelp)
	COMMAND_HANDLER(IDC_CALLING_PROG, CBN_SELCHANGE, OnSelectExeForDbg)
	COMMAND_HANDLER(IDC_CALLING_PROG, CBN_DROPDOWN, OnSelectExeForDbgDropdown)
ALT_MSG_MAP(1)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

		// set the appropriate font and size for the dlg and controls
		// get the font to use
		UIDLGLOGFONT logFont;
		CComPtr<IUIHostLocale> pLocale;
		HRESULT hr = ExternalQueryService( SID_SUIHostLocale, IID_IUIHostLocale, (void**)&pLocale );
		RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, pLocale, 1);
		pLocale->GetDialogFont( &logFont );
		m_hFont = CreateFontIndirectW( (LOGFONTW*)&logFont );
	
		SetFont( m_hFont );
		HDC dc = GetDC();
		SelectObject( dc, m_hFont );

		// set the appropriate size for the dlg and controls
		// get a base "unit" for sizing the dlg
		SIZE unit;
		GetTextExtentPoint32W( dc, L"X", 1, &unit );
		ReleaseDC( dc );
		
		int xBorder, yBorder;
		xBorder = GetSystemMetrics( SM_CXDLGFRAME );
		yBorder = GetSystemMetrics( SM_CYDLGFRAME ) + GetSystemMetrics( SM_CYCAPTION );

		// set the dlg size
		RECT r;
		r.left = 0;
		r.top = 0;
		r.right = (long)(67.5*unit.cx) + xBorder;
		r.bottom = (long)(10.5*unit.cy) + yBorder;
		SetWindowPos( HWND_TOP, &r, 0 );
		CenterWindow( m_hwndParent );

		// set the control sizes
		// static description
		CWindow staticDesc( GetDlgItem( IDC_CALLING_PROG_TXT2 ) );
		staticDesc.SetFont( m_hFont );
		r.left = (long)(0.5*unit.cx);
		r.top = (long)(0.5*unit.cy);
		r.right = (long)(r.left + (long)(55*unit.cx));
		r.bottom = (long)(r.top + (2.5*unit.cy));
		staticDesc.SetWindowPos( HWND_TOP, &r, 0 );
		// static exe name
		CWindow staticExeName( GetDlgItem( IDC_CALLING_PROG_TXT ) );
		staticExeName.SetFont( m_hFont );
		r.left = (long)(0.5*unit.cx);
		r.top = (long)(3*unit.cy);
		r.right = (long)(r.left + (55*unit.cx));
		r.bottom = (long)(r.top + (unit.cy));
		staticExeName.SetWindowPos( HWND_TOP, &r, 0 );
		// combo box
		CWindow combo( GetDlgItem( IDC_CALLING_PROG ) );
		combo.SetFont( m_hFont );
		r.left = (long)(0.5*unit.cx);
		r.top = (long)(4.5*unit.cy);
		r.right = (long)(r.left + (50*unit.cx));
		r.bottom = (long)(r.top + (1.5*unit.cy));
		combo.SetWindowPos( HWND_TOP, &r, 0 );
		// static URL
		CWindow staticURL( GetDlgItem( IDC_HTTP_URL_TXT ) );
		staticURL.SetFont( m_hFont );
		r.left = (long)(0.5*unit.cx);
		r.top = (long)(6.5*unit.cy);
		r.right = (long)(r.left + (55*unit.cx));
		r.bottom = (long)(r.top + (1*unit.cy));
		staticURL.SetWindowPos( HWND_TOP, &r, 0 );
		// edit field
		CWindow editURL( GetDlgItem( IDC_HTTP_URL ) );
		editURL.SetFont( m_hFont );
		r.left = (long)(0.5*unit.cx);
		r.top = (long)(8*unit.cy);
		r.right = (long)(r.left + (50*unit.cx));
		r.bottom = (long)(r.top + (1.5*unit.cy));
		editURL.SetWindowPos( HWND_TOP, &r, 0 );
		// ok button
		CWindow ok( GetDlgItem( IDOK ) );
		ok.SetFont( m_hFont );
		r.left = (long)(56.5*unit.cx);
		r.top = (long)(0.5*unit.cy);
		r.right = (long)(r.left + (10*unit.cx));
		r.bottom = (long)(r.top + (2*unit.cy));
		ok.SetWindowPos( HWND_TOP, &r, 0 );
		// cancel button
		CWindow cancel( GetDlgItem( IDCANCEL ) );
		cancel.SetFont( m_hFont );
		r.left = (long)(56.5*unit.cx);
		r.top = (long)(3*unit.cy);
		r.right = (long)(r.left + (10*unit.cx));
		r.bottom = (long)(r.top + (2*unit.cy));
		cancel.SetWindowPos( HWND_TOP, &r, 0 );
		// help button
		CWindow help( GetDlgItem( IDC_EXE_FOR_DEBUG_HELP ) );
		help.SetFont( m_hFont );
		r.left = (long)(56.5*unit.cx);
		r.top = (long)(5.5*unit.cy);
		r.right = (long)(r.left + (10*unit.cx));
		r.bottom = (long)(r.top + (2*unit.cy));
		help.SetWindowPos( HWND_TOP, &r, 0 );

		if( m_bstrExe.Length() != 0 )
			::SetDlgItemTextW( m_hWnd, IDC_CALLING_PROG, m_bstrExe );
		if( m_bstrUrl.Length() != 0 )
			::SetDlgItemTextW( m_hWnd, IDC_HTTP_URL, m_bstrUrl );

		CComBSTR bstrProjName;
        if (m_spConfig)
        {
            CComPtr<IDispatch> spDispProj;
            m_spConfig->get_Project(&spDispProj);
            CComQIPtr<VCProject> spProject = spDispProj;
            if (spProject)
            {
                spProject->get_Name(&bstrProjName);
            }

            CStringW strTitle;
            strTitle.Format(IDS_EXE_FOR_DBG_TITLE, bstrProjName);
            ::SetWindowTextW( m_hWnd, strTitle );
        }

		// turn autocompletion on
		HWND hwndEdit;
		hwndEdit = GetDlgItem( IDC_CALLING_PROG );
		SHAutoComplete( hwndEdit, 0 );
		m_exeForDbg.SubclassWindow(hwndEdit);
		hwndEdit = GetDlgItem( IDC_HTTP_URL );
		SHAutoComplete( hwndEdit, 0 );

		const int cItems = ELEM_COUNT(rgDebugCommands);
		for (int iItem = 0; iItem < cItems; iItem++)
		{
			CStringW str;
			if (str.LoadString(rgDebugCommands[iItem]))
			{
				m_listExeForDbgChoices.Add(rgDebugCommands[iItem]);
				m_listExeForDbgStrings.Add(str);
			}
		}

		return 1;  // Let the system set the focus
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CStringW strText;
		::GetDlgItemTextW( m_hWnd, IDC_CALLING_PROG, strText.GetBuffer(500), 500 );
		m_bstrExe = strText;
		strText.ReleaseBuffer();
		::GetDlgItemTextW( m_hWnd, IDC_HTTP_URL, strText.GetBuffer(500), 500 );
		m_bstrUrl = strText;
		strText.ReleaseBuffer();
		
		CComBSTR bstrProjName;
		if (m_spConfig)
		{
			CComPtr<IDispatch> spDispProj;
			m_spConfig->get_Project(&spDispProj);
			CComQIPtr<VCProject> spProject = spDispProj;
			if (spProject)
			{
				spProject->get_Name(&bstrProjName);
			}
		}

		if (m_bstrExe.Length() == 0 && m_bstrUrl.Length() == 0)
		{
			//the user didn't fill in either field
			strText.LoadString(IDS_E_DBG_BLANK);
			::MessageBoxW(m_hWnd, strText, bstrProjName, MB_OK|MB_ICONERROR);
			return wID;
		}
		else if (m_bstrExe.Length())
		{
			//the user typed in a file name, make sure that it exists on disk...
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind = ::FindFirstFile(m_bstrExe, &FindFileData);

			if (INVALID_HANDLE_VALUE == hFind)
			{
				strText.Format(IDS_E_DLG_FILENOTFOUND, m_bstrExe);
				if (IDYES == ::MessageBoxW(m_hWnd, strText, bstrProjName, MB_YESNO | MB_ICONERROR))
					return wID;
			}
		}

		DeleteObject( m_hFont );

		EndDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		DeleteObject( m_hFont );
		EndDialog(wID);
		return 0;
	}
    LRESULT OnHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HRESULT hr = S_OK;
		CComPtr<IVsHelp> pIHelp;
		if (SUCCEEDED(hr = ExternalQueryService(SID_SVsHelp, IID_IVsHelp, reinterpret_cast<void**>(&pIHelp))) && pIHelp)
		{
			hr = pIHelp->DisplayTopicFromF1Keyword(L"vs.debug.exefordebug");
		}
		
		return 0;
	}
	
	LRESULT OnHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		return OnHelp(WM_HELP, (WPARAM)0, (LPARAM)0, bHandled);
	}
	LRESULT OnSelectExeForDbgDropdown(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{	// about to drop the list box down, so save off anything the user had typed in here...
		::GetDlgItemTextW( m_hWnd, IDC_CALLING_PROG, m_strSavedText.GetBuffer(500), 500 );

		// we reset the content every time we drop the list down because we had to add a fake entry
		// at the end if the user actually selected something
		m_exeForDbg.SendMessage(CB_RESETCONTENT, 0L, 0L);
		INT_PTR cItems = m_listExeForDbgStrings.GetSize();
		for (INT_PTR iItem = 0; iItem < cItems; iItem++)
			UtilSendMessageWString( m_exeForDbg, CB_ADDSTRING, NULL, (LPARAM)(const wchar_t*)m_listExeForDbgStrings[iItem]);
		return 0;
	}

	LRESULT OnSelectExeForDbg(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		INT nSel = (INT)m_exeForDbg.SendMessage(CB_GETCURSEL, 0L, 0L);
		if (nSel < 0 || nSel >= ELEM_COUNT(rgDebugCommands))
			return 0;

		UINT id = m_listExeForDbgChoices[nSel];

		HRESULT hr = S_OK;
		CStringW strCommand;
		switch (id)
		{
		case VCDSID_Command_IExplore:
			//
			// User selected default browser, go find it
			//
			if (FAILED(hr = GetBrowserCommand(strCommand)))
			{
				VSFAIL ("Couldn't find a browser!");
			}
			break;

		case VCDSID_Command_ATLTest:
			hr = GetTestContainerCommand(strCommand);
			break;
			
		case VCDSID_Command_RegSvr:
			hr = GetRegSvrCommand(strCommand);
			break;
			
		case VCDSID_Command_Browse:
			hr = BrowseForCommand(strCommand);
			break;
			
		default:
			//
			// User must have typed something in - use it as-is.
			//
			return 0;
		}

		if (SUCCEEDED(hr))
			m_strSavedText = strCommand;

		UtilSendMessageWString( m_exeForDbg, CB_ADDSTRING, NULL, (LPARAM)(const wchar_t*)m_strSavedText );
		m_exeForDbg.SendMessage(CB_SETCURSEL, (WPARAM)m_listExeForDbgChoices.GetSize(), 0L);
		
		bHandled = TRUE;
		return 1;
	}

	//
	// Return a browser command w/ %s for the URL to open.  For example:
	//
	//  c:\W3C\Arena.exe -startpage "%s"
	//
	HRESULT GetBrowserCommand( CStringW& strCommand )
	{
		CRegKey regKey;
		CStringW strBrowser;
		ULONG result;
		DWORD dwSize = _MAX_PATH;
		CStringW strCommandLine;

		if (regKey.Open (HKEY_CLASSES_ROOT, L"http\\shell\\open\\command") != ERROR_SUCCESS)
		{
	        return E_FAIL;
		}

		{
			CString strA;
			result = regKey.QueryStringValue (NULL, strA.GetBuffer (dwSize), &dwSize);
			strA.ReleaseBuffer ();
			strBrowser = strA;
		}
		if (result != ERROR_SUCCESS)
		{
			return E_FAIL;
		}

		if (strBrowser.IsEmpty())
		{
			return E_FAIL;
		}

		{
			// From here there are two possible formats for the registry entry:
			// Netscape uses c:\progra~1\...\Netscape.exe -h "%1"
			// while IE uses "C:\Program Files\...\IEXPLORE.EXE" -nohome
			// The bstrExe that we return must be the executable alone without quotes, 
			// and we cannot pass args except through the bstrArg parameter.
			int iMarker = strBrowser.Find (L"%1");
			if (iMarker != -1)
			{
				// Netscape-style entry -- make a %s
				strCommandLine = strBrowser;
				((LPWSTR)(LPCWSTR)strCommandLine)[ iMarker + 1 ] = L's';
			}
			else
			{
				// IE-style entry
				strCommandLine = strBrowser + L" %s";
			}
		}

		//
		// Spit into EXE paths and args
		//

		int iSplit;
		if (strCommandLine[0] == L'\"')
		{
			// Quoted exe
			iSplit = strCommandLine.Find (L'\"', 1);
			VSASSERT (iSplit != -1, "Quote not found");
			VSASSERT (iSplit > 0, "Empty EXE Path");

			strCommand = strCommandLine.Mid (1, iSplit - 1);
		}

		else
		{
			iSplit = strCommandLine.Find (L' ');
			VSASSERT (iSplit != -1, "Space not found");
			VSASSERT (iSplit > 0, "Empty EXE Path");

			strCommand = strCommandLine.Left (iSplit);
		}

		return S_OK;
	}

	HRESULT GetTestContainerCommand( CStringW& strCommand )
	{
		GetTestContainerName(strCommand);
		return S_OK;
	}	

	HRESULT GetRegSvrCommand( CStringW& strCommand )
	{
		LPTSTR pszFile;

		CStringW str;
		str.ReleaseBuffer(SearchPathW(
			NULL,
			L"regsvr32.exe",
			NULL,
			_MAX_PATH + 1,
			str.GetBuffer( _MAX_PATH + 1 ),
			&pszFile
			));

		strCommand = str;

		return strCommand.IsEmpty() ? E_FAIL : S_OK;
	}

	HRESULT BrowseForCommand(CStringW& strCommand)
	{
		VSOPENFILENAMEW openFileName = {0};
		CStringW strFilter( MAKEINTRESOURCE( IDS_EXEFilter ));
		CStringW strTitle( MAKEINTRESOURCE( IDS_DebugProgram ));

		strFilter.Replace( L';', 0 );
		strFilter += L'\0';

		openFileName.lStructSize = sizeof (VSOPENFILENAMEW);
		openFileName.hwndOwner = m_hWnd;
		openFileName.pwzDlgTitle = strTitle;
		openFileName.pwzFileName = strCommand.GetBuffer( _MAX_PATH );
		openFileName.nMaxFileName = _MAX_PATH;
		openFileName.pwzInitialDir = NULL;
		openFileName.pwzFilter = strFilter;  
		openFileName.nFilterIndex = 0;
		openFileName.nFileOffset = 0;
		openFileName.nFileExtension = 0;
		openFileName.dwHelpTopic = 0;
		openFileName.dwFlags = 0;

		CComPtr<IVsUIShell> pShell;
		ExternalQueryService( SID_SVsUIShell, IID_IVsUIShell, (void**)&pShell );
		VSASSERT( pShell, "Can't get the VsUIShell service! How can there not be a shell?!" );
		if( pShell )
			pShell->GetOpenFileNameViaDlg( &openFileName );

		strCommand = openFileName.pwzFileName;
		
		return strCommand.IsEmpty() ? E_FAIL : S_OK;
	}

	BOOL GetTestContainerName(CStringW& strTestContainer)
	{
		InitializeTestContainer();
		strTestContainer = s_strTestContainer;
		return !strTestContainer.IsEmpty();
	}

	void InitializeTestContainer()
	{
		if (s_bTestContainerInit)
			return;

		s_bTestContainerInit = TRUE;

		//
		// Search order:
		//  1) Common7\Tools
		//	2) vc7 directory
		//	3) devenv.exe directory
		//	4) Use SearchPath()


		// Try Common7\Tools
		///////////////////////////////////
		UtilGetCommon7Folder(s_strTestContainer);
		s_strTestContainer += L"Tools\\tstcon32.exe";
		CPathW pathExe1;
		if (pathExe1.Create(s_strTestContainer) && pathExe1.ExistsOnDisk())
			return;

		// Try vc7\bin
		///////////////////////////////////

		UtilGetVCInstallFolder(s_strTestContainer);
		s_strTestContainer += L"bin\\tstcon32.exe";
		CPathW pathExe2;
		if (pathExe2.Create(s_strTestContainer) && pathExe2.ExistsOnDisk())
			return;

		//
		// Try devenv.exe directory
		//
		UtilGetDevenvFolder(s_strTestContainer);
		s_strTestContainer += L"tstcon32.exe";

		CPathW pathExe3;
		if (pathExe3.Create(s_strTestContainer) && pathExe3.ExistsOnDisk())
			return;


		//
		// Try the path
		//
		wchar_t * psz, szPath[_MAX_PATH + 1];
		DWORD dw = SearchPathW(NULL, L"tstcon32.exe", NULL, _MAX_PATH, szPath, &psz);
		if ( 0 < dw && _MAX_PATH > dw )
			s_strTestContainer = szPath;
		else
			s_strTestContainer.Empty();
	}

public:
	CComBSTR m_bstrExe;
	CComBSTR m_bstrUrl;
	CComPtr<VCConfiguration> m_spConfig;

protected:
	CContainedWindow m_exeForDbg;
	CVCUIntArray m_listExeForDbgChoices;
	CVCStringWArray m_listExeForDbgStrings;
	CStringW m_strSavedText;
	HFONT m_hFont;
	HWND m_hwndParent;

	static BOOL s_bTestContainerInit;
	static CStringW s_strTestContainer;
};

#endif //__CCALLEXEFORDEBUG_H_
