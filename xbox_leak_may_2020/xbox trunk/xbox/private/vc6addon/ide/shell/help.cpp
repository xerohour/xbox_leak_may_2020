// help.cpp : implementation of Olympus help support
//

#include "stdafx.h"

#include "shell.h"
#include "resource.h"

//#define NO_XBOX_F1
#ifndef NO_XBOX_F1
#include "xboxhelp.h"
#include "main.h"
#endif

#ifdef __DEVHELP98__ 
#include "helpsys.h" //TODO:[der] Move this file
#include "helpsysid.c"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Constants
//

// The following are used to enable the PSS Technical Support Menu.
const wchar_t c_pssHelpFile[] = L"vsintro.chm";
const long c_pssContextId = 151001; // This id can be found in %devbin%\htmlhelp\v6\include\ctxids.h

///////////////////////////////////////////////////////////////////////////////
//
// Prototypes
//
// Helper function for DoContextHelpMenu
CWnd* ChildWindowFromPointAvoidGroupBoxes(CWnd*, const CPoint& point);

#ifdef __DEVHELP98__ 
IVsHelpSystem* GetHelpInterface(BOOL bDisplayNoHelpMsg = TRUE);
#endif 

void CancelModes();
BOOL HelpDebug();
BOOL HelpDebug(LPCTSTR szKeyword);
BOOL HelpDebug(LPCTSTR szKeyword, LPCTSTR szFile);

///////////////////////////////////////////////////////////////////////////////
//
//
//
LPCTSTR HelpFileFromID(DWORD dwId, UINT type)
{
	LPCTSTR szFileName = NULL;
	EXESTRING_INDEX iExeString = DefaultAppHelpFile;
	CPackage::HELPFILE_TYPE hft = CPackage::APPLICATION_HELP;
	CPackage::RANGE_TYPE minRange;
	CPackage::RANGE_TYPE maxRange;
	switch (type)
	{
	case HELPTYPE_CONTROL:
		hft = CPackage::CONTROL_HELP;
		iExeString = DefaultControlHelpFile;
		// fall through
	case HELPTYPE_DIALOG:
		minRange = CPackage::MIN_RESOURCE;
		maxRange = CPackage::MAX_RESOURCE;
		break;

	case HELPTYPE_WINDOW:
		minRange = CPackage::MIN_WINDOW;
		maxRange = CPackage::MAX_WINDOW;
		break;

	case HELPTYPE_COMMAND:
		minRange = CPackage::MIN_COMMAND;
		maxRange = CPackage::MAX_COMMAND;
		break;

	default:
		ASSERT(FALSE);
	}

	POSITION pos = theApp.m_packages.GetHeadPosition();
	while (szFileName == NULL && pos != NULL)
	{
		CPackage* pPackage = (CPackage*)(theApp.m_packages.GetNext(pos));
		ASSERT(pPackage != NULL);
		if (pPackage->m_flags & PKS_HELPFILE)
		{
			if ((dwId >= pPackage->GetIdRange(minRange)) &&
				(dwId <= pPackage->GetIdRange(maxRange)))
			{
				szFileName = pPackage->GetHelpFileName(hft);
			}
		}
	}
	if (szFileName == NULL)
	{
		// if no other filename is found, use the exe's helpfile
		szFileName = theApp.GetExeString(iExeString);
	}
	ASSERT(szFileName != NULL);
	return szFileName;
}

///////////////////////////////////////////////////////////////////////////////
// Context aka 'Control' help

// Map of Control ID to Help ID 
//
// Required by WinHelp HELP_CONTEXTMENU and HELP_WM_HELP calls
// Terminated by zero pair.
//
// -1 for HID of a pair disables the context help/context menu.
//
static DWORD rgHID[] = {
	(DWORD)-2, (DWORD)-1,
	0,0
};

// Synthesize the WinHelp ID map for the current/clicked-on control
// based on HelpID properties in the control. If we found the help
// ID in something other than hwndCtrl, return it's hwnd through 
// pwndHelp.
//
static BOOL SetHID(HWND, HWND hwndCtrl, HWND * phwndHelp)
{
	DWORD idCtrl = ::GetDlgCtrlID(hwndCtrl);
	if (0 == idCtrl)
		return FALSE;
	if (phwndHelp)
		*phwndHelp = hwndCtrl;
	DWORD idHelp = ::GetWindowContextHelpId(hwndCtrl);
	rgHID[0] = idCtrl;
	rgHID[1] = idHelp ? idHelp : (DWORD)-1;

	CString strClass;
	::GetClassName(hwndCtrl, strClass.GetBuffer(16), 16);
	strClass.ReleaseBuffer();

	// Special behavior for standard buttons
	if (strClass.CompareNoCase("Button") == 0)
	{
		switch (idCtrl)
		{
		case IDOK:
		case IDCANCEL:
			// Fall through. Uncomment below to use standard help.
			//
			// WinHelp has built-in Help for standard buttons, so 
			// we'll use that.
			//rgHID[0] = (DWORD)-2;
			//rgHID[1] = (DWORD)-1;
			//return TRUE;
			//break;
		case IDCLOSE:
		case IDHELP:
			// Convention for standard: help id is control id
			rgHID[0] = rgHID[1] = idCtrl;
			return TRUE;

		default:
			break;
		}
	}  
	else if (strClass.CompareNoCase("Static") == 0)
  	// Label: probe for next control in tab order for help id
	//
	// We are using these rules for this: we check if the wndow class is "Static"
	// this handles IDC_STATIC and dynamic labels. We are explicitly NOT
	// handling Group Boxes in this code, but we could by adding the class test.
	// (Button with BS_GROUPBOX style). The old code did this for all IDC_STATIC items
	// and included group boxes.
	// So if it is a static item, we get the next control's help if
	// EITHER the id is IDC_STATIC OR there is no help id specified. If you want to
	// specify a help id you may not use IDC_STATIC, because winhelp does not like it.
	// bobz 9/25/96
	//
	// For dynamic labels, we could add some conventional IDs that get the help 
	// id probing, and can be addressed individually for text-changing purposes.
	// case IDC_DYNALABEL:

	// This code is now complicated by the fact that GetNextDlgTabItem requires the WS_TABSTOP style to
	// be set. However, in some cases, we want to find non-tabstop items because the label is labelling
	// a static item. To fix this problem, we will first manually probe forward to find statics with the
	// right configuration. If this search fails, or if we first meet a non-static, then we give up and
	// revert to the normal mechanism. martynl Oct 07 96
	//
	{
		if (idHelp == 0 || idCtrl == 65535 || idCtrl == 0xffffffff)	 // idctrl values for IDC_STATIC in Win95 and Win NT respectively
		{
			// probe forward for labelled static
			HWND hwndNext=hwndCtrl;		//
			BOOL bFinished=FALSE;		// TRUE if we hit a non-static, or a tabstopped control

			while(!bFinished)
			{
				// get the next control
				hwndNext=::GetNextWindow(hwndNext,GW_HWNDNEXT);

				// Nothing next, so this can't be used
				if(hwndNext!=NULL)
				{
					int nStyle=::GetWindowLong(hwndNext, GWL_STYLE);

					// next was a tabstop, which means normal means should be used
					if((nStyle & WS_TABSTOP)==0)
					{
						CString strNextClass;
						::GetClassName(hwndNext, strNextClass.GetBuffer(16), 16);
						strNextClass.ReleaseBuffer();

						// next wasn't static means normal mechanisms should be used
						if(strNextClass.CompareNoCase("Static") == 0)
						{
							DWORD idNextCtrl = ::GetDlgCtrlID(hwndNext);

							// next has bad ctrl ids means keep probing
							if (idNextCtrl != 65535 && idNextCtrl != 0xffffffff)	 // idctrl values for IDC_STATIC in Win95 and Win NT respectively
							{
								// now actually get help id
								DWORD dwId = ::GetWindowContextHelpId(hwndNext);
								if (dwId)
								{
									if (phwndHelp)
										*phwndHelp = hwndNext;
									rgHID[0] = idNextCtrl;
									rgHID[1] = dwId;
									return TRUE;
								}
								else
								{
									// continue probing forward
								}
							}
							else
							{
								// continue probing forward
							}
							
						}
						else
						{
							bFinished=TRUE;
						}
					}
					else
					{
						bFinished=TRUE;
					}
				}
				else
				{
					bFinished=TRUE;
				}
			}

			// probe forward for tabstop
			//hwndNext = ::GetNextDlgTabItem(hwndDlg, hwndCtrl, 0);
			if (hwndNext)
			{
				DWORD dwId = ::GetWindowContextHelpId(hwndNext);
				if (dwId)
				{
					if (phwndHelp)
						*phwndHelp = hwndNext;
					rgHID[0] = ::GetDlgCtrlID(hwndNext);
					rgHID[1] = dwId;
					return TRUE;
				}
			}
		}
	}

	return idHelp != 0;
}

///////////////////////////////////////////////////////////
//
// 
//
BOOL GetHID(HWND hwndDlg, HWND hwndCtrl, HWND * phwndHelp, DWORD* hid)
{
	BOOL bRetVal = SetHID(hwndDlg, hwndCtrl, phwndHelp);
	*hid = (DWORD)(LPVOID)rgHID;
	return bRetVal;
}
	
///////////////////////////////////////////////////////////
//
// GetFullHelpFile
// Look for szHelpfile in several places:
// - as specified
// - MSDEV\HELP
// - MSDEV\BIN
// - MSDEV\BIN\IDE
// if we don't find it anywhere around here, we let WinHelp fend for itself.

void GetFullHelpFile( LPCTSTR szHelpfile, CString & strHelp)
{
    if (FileExists(szHelpfile))
    {
        strHelp = szHelpfile;
        return;
    }

    CString strDir;
    GetHelpDir(strDir);
    strDir += '\\';

    // Try MSDEV\HELP\file
    CString str(strDir);  
    str += szHelpfile;
    if (FileExists(str))
    {
        strHelp = str;
        return;
    }

	// Convert "C:\FOO\BIN\MSDEV.EXE" to "C:\FOO\BIN\"
    ::GetModuleFileName(NULL, strDir.GetBufferSetLength(_MAX_PATH), _MAX_PATH);
	strDir.ReleaseBuffer();
	int i = strDir.ReverseFind('\\');
	ASSERT(i != -1);
	strDir.GetBufferSetLength(i + 1);
	strDir.ReleaseBuffer();

	// Try "C:\FOO\BIN\file"
    str = strDir + szHelpfile;
    if (FileExists(str))
    {
        strHelp = str;
        return;
    }

    // Now try "C:\FOO\BIN\extdir\file"
    str = strDir + theApp.GetExeString(DefaultExtensionDir);
    str += '\\';
    str += szHelpfile;
    if (FileExists(str))
    {
        strHelp = str;
        return;
    }

    // lastly, let WinHelp fend for itself
    strHelp = szHelpfile;
}

///////////////////////////////////////////////////////////
//
// This function handles the right mouse button context help
// for dialog boxes.
//
void DoHelpContextMenu(CWnd * pDialog, UINT idDlg, CPoint pt)
{
	// Orion Bug 12420	
	//	pDialog->ScreenToClient(&pt);
	//	CWnd * pCtrl = pDialog->ChildWindowFromPoint(pt, CWP_SKIPINVISIBLE);
	CWnd* pCtrl = ChildWindowFromPointAvoidGroupBoxes(pDialog, pt) ;
	if (pCtrl)
	{
		HWND hwndHelp;
		if (::SetHID(pDialog->m_hWnd, pCtrl->m_hWnd, &hwndHelp))
		{
            CString strHelp;
            GetFullHelpFile(HelpFileFromID(idDlg, HELPTYPE_CONTROL), strHelp);
            ASSERT(!strHelp.IsEmpty());
			if (HelpDebug())
			{
				char sz[34];
				_ultoa(rgHID[1], sz, 10);
				HelpDebug(sz, strHelp);
				return;
			}
	        ::WinHelp(hwndHelp, strHelp, HELP_CONTEXTMENU, (DWORD)(LPVOID)rgHID);
		}
	}
}

///////////////////////////////////////////////////////////
//
// This function handles getting the context help for a dialog item.
// it takes a pointer to the dialog, the id of the dialog and a
// pointer to the HELPINFO structure.
//
// Currently, it always returns TRUE ;
//
BOOL DoHelpInfo(CWnd * pDialog, UINT idDlg, HELPINFO* pInfo)
{
	if (pInfo->iContextType == HELPINFO_WINDOW)
	{
		HWND hwndHelp = NULL;
		HWND hwndCtrl = (HWND)pInfo->hItemHandle;
		if (!::SetHID(pDialog->m_hWnd, hwndCtrl, &hwndHelp))
		{
			// default F1 to dialog help in main help window
//			CWnd::OnHelp();
			return 1;
		}

        CString strHelp;
        GetFullHelpFile(HelpFileFromID(idDlg, HELPTYPE_CONTROL), strHelp);
        ASSERT(!strHelp.IsEmpty());
			if (HelpDebug())
			{
				char sz[34];
				_ultoa(rgHID[1], sz, 10);
				HelpDebug(sz, strHelp);
				return 1;
			}
		ASSERT(hwndHelp != NULL);
   		::WinHelp(hwndHelp, strHelp, HELP_WM_HELP, (DWORD)(LPVOID)rgHID);
	}
	return 1;
}

///////////////////////////////////////////////////////////
//
// 
//
void CMainFrame::WinHelp(DWORD dwData, UINT nCmd)
{
	UINT ht = HELPTYPE_DIALOG;
	// synthesize HELPTYPE and possibly un-munge id munged by MFC
	if ((dwData > HID_WND_TEXT) && (dwData < HID_WND_TEXT + 0x100))
	{
		ht = HELPTYPE_WINDOW;
	}
	else if (dwData > HID_BASE_RESOURCE)
	{
		dwData -= HID_BASE_RESOURCE;
		// We get this when nothing reasonable has the focus, or
		// some other bizarre circumstances -- we'll ignore this
		// because there's no good help to display.
		if (dwData == IDR_MAINFRAME)
			return;
	}
	else if (dwData == HID_BASE_RESOURCE)
	{
		// no id = no help
		return;
	}
	else if (dwData > HID_BASE_COMMAND)
	{
		return; // ignore commands
//              dwData -= HID_BASE_COMMAND;
//              ht = HELPTYPE_COMMAND;
	}
	theApp.HelpOnApplication(dwData, ht);
}

///////////////////////////////////////////////////////////
//
// 
//
void CMainFrame::OnFastHelp ()
{
	theApp.m_bFastHelp = TRUE;

	CMDIFrameWnd::OnHelp();

	theApp.m_bFastHelp = FALSE;
}

///////////////////////////////////////////////////////////
//
// 
//
void CMainFrame::OnHelpContents()
{
	theApp.HelpContents();
}

///////////////////////////////////////////////////////////
//
// 
//
#ifdef __DEVHELP98__
void CMainFrame::OnHelpKeywords()
{
	// If the current PartView has a search word to offer, use it.
/* BUG 10593
	CString strKeyword;
	CPartFrame* pFrame = (CPartFrame*) MDIGetActive();
	if (pFrame && pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
	{
		CPartView* pView = (CPartView*)pFrame->GetActiveView();
		ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));

		pView->GetHelpWord(strKeyword);
	}
*/
	theApp.HelpOnKeyword(NULL);
}
#endif

///////////////////////////////////////////////////////////
//
// 
//
void CMainFrame::OnHelpSearch()
{
	// If the current PartView has a search word to offer, use it.
	CString strSearch;
	CPartFrame* pFrame = (CPartFrame*) MDIGetActive();
	if (pFrame && pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
	{
		CPartView* pView = (CPartView*)pFrame->GetActiveView();
		ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));

		pView->GetHelpWord(strSearch);
	}

	theApp.HelpFind(strSearch, HF_Default);
}

///////////////////////////////////////////////////////////
//
// 
//
void CancelModes()
{
	// All of the following code is basically from CWnd::WinHelp.  It
	// just cancels special modes and mouse capture.
	theApp.m_bHelpMode = FALSE;

	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ExitHelpMode();

	/* cancel any tracking modes */
	pMainFrame->SendMessage(WM_CANCELMODE);
	pMainFrame->SendMessageToDescendants(WM_CANCELMODE, 0, 0, TRUE, TRUE);

	/* attempt to cancel capture */
	HWND hWndCapture = ::GetCapture();
	if (hWndCapture != NULL)
		::SendMessage(hWndCapture, WM_CANCELMODE, 0, 0);
}

///////////////////////////////////////////////////////////
//
// GetHelpInterface
//
#ifdef __DEVHELP98__ 
	static IVsHelpSystem* s_pIVsHelpSystem = NULL;

IVsHelpSystem* GetHelpInterface(BOOL bDisplayNoHelpMsg)
{
	if( NULL == s_pIVsHelpSystem )
	{
		HRESULT hr = theApp.GetService(SID_SHelpService, IID_IVsHelpSystem, (void**)&s_pIVsHelpSystem) ;
		if (FAILED(hr) || (s_pIVsHelpSystem == NULL))
		{
			if( bDisplayNoHelpMsg )
			{
				ASSERT(s_pIVsHelpSystem == NULL) ;
				CString strNoHelp;
				VERIFY(strNoHelp.LoadString(IDS_HELPLESS));
				MsgBox(Information, strNoHelp);
			}
		}
	}
	return s_pIVsHelpSystem ;
}
#endif

///////////////////////////////////////////////////////////
//
// 
//
void ReleaseHelpInterface()
{
#ifdef __DEVHELP98__ 
	if(s_pIVsHelpSystem)
	{
		s_pIVsHelpSystem->Release() ;
		s_pIVsHelpSystem = NULL;
	}
#endif
}

///////////////////////////////////////////////////////////
//
// HelpContents
//
void CTheApp::HelpContents()
{
	if (UseExtHelp())
	{
		HelpContentsExt();
		return;
	}
#ifdef __DEVHELP98__ 
	IVsHelpSystem* pIVsHelpSystem = GetHelpInterface();
	if (pIVsHelpSystem)
	{
		pIVsHelpSystem->ActivateHelpSystem(0) ;
	}
#endif
}


///////////////////////////////////////////////////////////
//
// HelpFind
//
void CTheApp::HelpFind(LPCTSTR szKeyword, HELP_FIND_PAGE hf)
{
	if (UseExtHelp())
	{
		HelpIndexExt(szKeyword);
		return;
	}
#ifdef __DEVHELP98__ 
	ASSERT(hf == HF_Default) ; //TODO: [der] Remove HELP_FIND_PAGE

	IVsHelpSystem* pIVsHelpSystem = GetHelpInterface();
	if (pIVsHelpSystem)
	{		
		BSTR bstrQuery = NULL ;
		if ((szKeyword != NULL) && (_tcslen(szKeyword) > 0))
		{
			// We need to trim the keyword. Easiest way is in a CString.
			CString strKeyword(szKeyword) ;
			strKeyword.TrimLeft() ;
			strKeyword.TrimRight() ;
			
			if (!strKeyword.IsEmpty())
			{
				// Put the keyword (if it exists) into a BSTR. 
				bstrQuery = strKeyword.AllocSysString() ;
				ASSERT(bstrQuery) ;
			}
		}

		HRESULT hr = pIVsHelpSystem->FullTextSearchDlg(	/*BSTR*/ bstrQuery, 
														/*dwFlags*/ NULL,
														/*lpContext*/ NULL);
		ASSERT(SUCCEEDED(hr)) ;

		if (bstrQuery)
		{
			SysFreeString(bstrQuery) ;
		}
	}
#endif //__DEVHELP98__
}

///////////////////////////////////////////////////////////
//
// HelpOnApplication
//
void CTheApp::HelpOnApplication(DWORD dwID, UINT type, LPCTSTR szFileName)
{
#ifdef __DEVHELP98__ 
	USES_CONVERSION;
#endif

	TRACE("Help: id = 0x%x  type = 0x%x\n", dwID, type);
	// if the type is not HELP_FINDER or HELP_CONTENTS and the context is 0,
	// beep and return
	if (HELP_CONTENTS != type && HELP_FINDER != type && dwID == 0)
	{
		// Never a valid ID
		::MessageBeep(0);
		return;
	}

	// Now we have to figure out what filename to look in
	if (szFileName == NULL)
		szFileName = HelpFileFromID(dwID, type);

	_TCHAR szExt[_MAX_EXT];
	_splitpath(szFileName, NULL, NULL, NULL, szExt);
	if (_tcsicmp(szExt, ".hlp") == 0)
	{
		// Use WinHelp
		CString strHelpFile;
		GetFullHelpFile(szFileName, strHelpFile);
		if (HelpDebug())
		{
			char sz[34];
			_ultoa(dwID, sz, 10);
			HelpDebug(sz, strHelpFile);
			return;
		}

		switch (type)
		{
		// Map old IV help types to HELP_CONTEXT
		case HELPTYPE_DIALOG:  
		case HELPTYPE_WINDOW:
		case HELPTYPE_COMMAND: 
			type = HELP_CONTEXT;
			break;

		// Pass through standard Winhelp command types (except TCARD)
		// From WINUSER.H
		case HELP_CONTEXT:
		case HELP_QUIT:
//		case HELP_INDEX: // same as HELP_CONTENTS
		case HELP_CONTENTS:
		case HELP_HELPONHELP:
		case HELP_SETINDEX:
//		case HELP_SETCONTENTS: // same as HELP_SETINDEX
		case HELP_CONTEXTPOPUP:
		case HELP_FORCEFILE:
		case HELP_KEY:
		case HELP_COMMAND:
		case HELP_PARTIALKEY:
		case HELP_MULTIKEY:
		case HELP_SETWINPOS:
		case HELP_CONTEXTMENU:
		case HELP_FINDER:
		case HELP_WM_HELP:
		case HELP_SETPOPUP_POS:
			break;

		// Ignore garbage
		default:
			ASSERT(0);// Shouldn't get anything but the known types above.
			return;
		}
		// All our help is displayed in this secondary window
		strHelpFile += _T(">VSMain");
		::WinHelp(AfxGetMainWnd()->m_hWnd, strHelpFile, type, dwID);
	}
	else
	{
		if (dwID == 0)
		{
			// Never a valid ID
			::MessageBeep(0);
			return;
		}

		// Use InfoViewer
		TCHAR szContext[50];
		LPCTSTR sz;

		// make context string from type and id
		switch (type)
		{
		case HELPTYPE_DIALOG:  sz = _T("HID_DIALOG_"); break;
		case HELPTYPE_WINDOW:  sz = _T("HID_WINDOW_"); break;
		case HELPTYPE_COMMAND: sz = _T("HID_COMMAND_"); break;
		default:
			sz = _T("HID_");
			ASSERT(0);
			break;
		}
		_tcscpy(szContext,sz);
		_ultot(dwID, _tcschr(szContext,0), 10);

		TRACE("Help: context='%s' file='%s'\n", (LPCTSTR)szContext, szFileName);

		if (HelpDebug(szContext, szFileName))
			return;

#ifdef __DEVHELP98__ 
		// We need to do a keyword help lookup.
	IVsHelpSystem* pIVsHelpSystem = GetHelpInterface();
	if (pIVsHelpSystem)
	{		
		LPOLESTR str = T2OLE(szContext) ;
		HRESULT hr = pIVsHelpSystem->ALinkSearch(str, 
											/*dwFlags*/ NULL,
											/*lpContext*/ NULL);

		ASSERT(SUCCEEDED(hr)) ;
	}
#endif //__DEVHELP98__
	}
	CancelModes();
}

///////////////////////////////////////////////////////////
//
// HelpOnKeyword
//
void CTheApp::HelpOnKeyword(LPCTSTR szKeyword, BOOL bAlink /*= FALSE*/, REF_DISPLAY_TYPE dt)
{

	if (HelpDebug(szKeyword))
		return;

	if (UseExtHelp())
	{
		HelpOnKeywordExt(szKeyword);
		return;
	}

#ifndef NO_XBOX_F1
	// Keywords are only built for english systems.
	if ( !IsJapaneseSystem() && DoXboxHelp(szKeyword) ) 
		return;
#endif // NO_XBOX_F1

#ifdef __DEVHELP98__ 
	IVsHelpSystem* pIVsHelpSystem = GetHelpInterface();
	if (pIVsHelpSystem)
	{		
		HRESULT hr = E_FAIL ;

		// We need to trim the keyword. Easiest way is in a CString.
		CString strKeyword(szKeyword) ;
		strKeyword.TrimLeft() ;
		strKeyword.TrimRight() ;

		if (!strKeyword.IsEmpty())
		{
			// Put the keyword (if it exists) into a BSTR. 
			BSTR bstrQuery = strKeyword.AllocSysString() ;
			ASSERT(bstrQuery) ;

			if (bAlink)
			{
				hr = pIVsHelpSystem->ALinkSearch(	bstrQuery, 
													/*dwFlags*/ NULL,
													/*lpContext*/ NULL);

			}
			else
			{
				hr = pIVsHelpSystem->KeywordSearch(	bstrQuery, 
													/*dwFlags*/ NULL,
													/*lpContext*/ NULL);
			}

			// Cleanup
			if (bstrQuery)
			{
				SysFreeString(bstrQuery) ;
			}
		}
		else
		{
			// No keyword. Do a KeywordSearchDlg
			hr = pIVsHelpSystem->KeywordSearchDlg(	NULL, 
													/*dwFlags*/ NULL,
													/*lpContext*/ NULL);
		}


		ASSERT(SUCCEEDED(hr)) ;
	}
#endif
	CancelModes();
}


///////////////////////////////////////////////////////////
//
// HelpDebug
//

//#ifdef _DEBUG
BOOL HelpDebug()
{
	// Determine whether the HelpDebug registry flag is set.
	if (GetRegInt("Help", "HelpDebug", FALSE))
		return TRUE;

#ifndef _SHIP
	// If we can confirm that there's no entry (because it gives us
	// the default regardless of whether that default is TRUE or FALSE)
	// then go ahead and create an entry; this will make it easier for
	// the UE team to modify the setting for the first time.
	else if (GetRegInt("Help", "HelpDebug", TRUE))
		WriteRegInt("Help", "HelpDebug", FALSE);
#endif

	return FALSE;
}

///////////////////////////////////////////////////////////
//
// HelpDebug
//
BOOL HelpDebug(LPCTSTR szKeyword)
{
	if (HelpDebug())
	{
		char sz [255];
		wsprintf(sz, "Help requested on keyword '%s'", szKeyword);
		MsgBox(Information,sz);
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////
//
// Help
//
BOOL HelpDebug(LPCTSTR szItem, LPCTSTR szFile)
{
	if (HelpDebug())
	{
		_TCHAR sz [255];
		_sntprintf(sz, sizeof(sz), _T("Help requested on '%s' in file '%s'"), 
			szItem, (szFile != NULL) ? szFile : _T("(unknown)"));
		MsgBox(Information,sz);
		return TRUE;
	}

	return FALSE;
}
//#endif


///////////////////////////////////////////////////////////////////////////
//
// WinHelp services
//
///////////////////////////////////////////////////////////
//
// Construct default help directory
//
void GetHelpDir(CString & strHelpDir)
{
	// Our best-guess default is a "help" directory which is at the same
	// level in the directory hierarchy as the current executable.
	
	::GetModuleFileName(NULL, strHelpDir.GetBufferSetLength(_MAX_PATH), _MAX_PATH);
	strHelpDir.ReleaseBuffer();

	// Convert C:\FOO\BIN\MSVC.EXE to C:\FOO\HELP.
	int i = strHelpDir.ReverseFind('\\');
	ASSERT(i != -1);
	strHelpDir.GetBufferSetLength(i);
	strHelpDir.ReleaseBuffer();

	// Now we have C:\FOO\BIN
	i = strHelpDir.ReverseFind('\\');
	if (i != -1)
	{
		strHelpDir.GetBufferSetLength(i);
		strHelpDir.ReleaseBuffer();
	}

	// Now we have C:\FOO (or just "C:", if the exe was in the root).
	strHelpDir += _TEXT("\\HELP");

}

///////////////////////////////////////////////////////////////////////////
//
// PSS Help
//
BOOL GetPSSHelpFile(CString & strHelpFile)
{
	// Used to enable the menu.
	return TRUE ;
/*
	GetHelpDir(strHelpFile);
	strHelpFile += _TEXT("\\pss.hlp");
	if (_access(strHelpFile, 04) == 0)      // check for read privs
		return TRUE;
	else

		return FALSE;
*/
}

void CVshellPackage::OnHelpPSS()
{
#ifdef __DEVHELP98__ 
	IVsHelpSystem* pIVsHelpSystem = GetHelpInterface();
	if (pIVsHelpSystem)
	{
		pIVsHelpSystem->DisplayTopicFromIdentifier(c_pssHelpFile, c_pssContextId, VHS_Localize);
	}
#else
	CString strPSSHelp;
	if (GetPSSHelpFile(strPSSHelp))
	{
		::WinHelp(AfxGetMainWnd()->m_hWnd, strPSSHelp, HELP_FINDER, 0);
	}
	else
		::MessageBeep(0);
#endif
}

///////////////////////////////////////////////////////////////////////////
// Microsoft on the Web Help

void CVshellPackage::OnHelpMSOnTheWeb(UINT nID)
{
	ASSERT( nID >= IDM_HELP_MOW_FIRST   &&  nID <= IDM_HELP_MOW_LAST );

	static WORD rgURLMap[] = {
		IDS_HELP_MOW_FREESTUFF,
		IDS_HELP_MOW_PRODUCTNEWS,
		IDS_HELP_MOW_FAQ,
		IDS_HELP_MOW_ONLINESUPPORT,
		IDS_HELP_MOW_DEVONLYHOMEPAGE,
		IDS_HELP_MOW_SENDFEEDBACK,
		IDS_HELP_MOW_BESTOFTHEWEB,
		IDS_HELP_MOW_SEARCHTHEWEB,
		IDS_HELP_MOW_WEBTUTORIAL,
		IDS_HELP_MOW_MSHOMEPAGE,
	};
	UINT uiOffset = nID - IDM_HELP_MOW_FIRST;

	CString strURL;
	VERIFY( strURL.LoadString( rgURLMap[uiOffset] ));

	OpenURL( strURL, eHonorIVOption );
}

void OpenURL( CString& strURL, EOpenURLOption eURLOpt /*=eHonorIVOption*/)
{
	if( eURLOpt != eExternalOnly )
	{
#ifdef __DEVHELP98__ 
		IVsHelpSystem* pIVsHelpSystem = GetHelpInterface();
		if (pIVsHelpSystem)
		{		
			BSTR bstrURL = strURL.AllocSysString() ;

			HRESULT hr = pIVsHelpSystem->DisplayTopicFromURL(	/*BSTR*/ bstrURL, 
																/*dwCommands*/ VHS_UseBrowser);

			if (bstrURL)
			{
				::SysFreeString(bstrURL) ;
			}
			ASSERT(SUCCEEDED(hr)) ;
		}
#endif //__DEVHELP98__ 
	}
	else
	{
	    if (strURL.GetLength() < _MAX_PATH)
		{
			// make null-terminated list of files from this filename
	        TCHAR lszFiles[_MAX_PATH+1];
		    _tcscpy(lszFiles,strURL);
			lszFiles[strURL.GetLength() + 1] = 0; // null terminate file list

	        SHELLEXECUTEINFO sei;
		    memset(&sei, 0, sizeof sei);
			sei.cbSize = sizeof sei;
	        sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
		    sei.hwnd = AfxGetMainWnd()->m_hWnd;
			sei.lpFile = lszFiles;
	        sei.nShow = 0;
		    if (ShellExecuteEx(&sei))
			   return;
		}
	}
}


///////////////////////////////////////////////////////////////////////////
// Extension Help

const TCHAR szkHelpExtension[] = _TEXT("Help\\Extension");
const TCHAR szkShowExtHelp[] = _TEXT("Enable");
const TCHAR szkExtHelpFile[] = _TEXT("Filename");
const TCHAR szkExtUse[] = _TEXT("State");

void EnsureExtHelpKeys()
{
	// make sure keys are there for user to fill in and in a consistent state
	CString str = GetRegString(szkHelpExtension, szkExtHelpFile);
	WriteRegString(szkHelpExtension, szkExtHelpFile, str);
	BOOL bShow = !str.IsEmpty() && GetRegInt(szkHelpExtension, szkShowExtHelp, 0);
	WriteRegInt(szkHelpExtension, szkShowExtHelp, bShow);
	WriteRegInt(szkHelpExtension, szkExtUse, bShow ? GetRegInt(szkHelpExtension, szkExtUse, 0) : 0);
}

// Is feature enabled?
BOOL ShowExtHelp()
{
	EnsureExtHelpKeys();
	return !!GetRegInt(szkHelpExtension, szkShowExtHelp, 0);
}

BOOL UseExtHelp()
{
	if (!ShowExtHelp())
		return FALSE;
	return !!GetRegInt(szkHelpExtension, szkExtUse, 0);
}

BOOL SetExtHelp(BOOL b)
{
	b = !!b;
	WriteRegInt(szkHelpExtension, szkExtUse, b);
	return b;
}

BOOL ToggleExtHelp()
{
	return SetExtHelp(!UseExtHelp());
}

BOOL GetExtHelpFile(CString & strHelpFile)
{
	strHelpFile = GetRegString(szkHelpExtension, szkExtHelpFile);
	if (strHelpFile.IsEmpty())
	{
		EnsureExtHelpKeys();
		return FALSE;
	}

	if (_access(strHelpFile, 04) == 0)
		return TRUE;
	// Look in help directory
	CString strHelpDir;
	GetHelpDir(strHelpDir);
	if (strHelpFile[0] != _T('\\'))
		strHelpDir += _T('\\');
	strHelpDir += strHelpFile;
	if (_access(strHelpDir, 04) == 0)
	{
		// update reg with full path
		WriteRegString(szkHelpExtension, szkExtHelpFile, strHelpDir);
		return TRUE;
	}
	return FALSE;
}

void CVshellPackage::OnHelpExtHelp()
{
	ToggleExtHelp();
}

void ExtHelpNotAvailable( LPCTSTR szFile )
{
	_TCHAR szMsg[1024];
	CString strFmtMsg;
	VERIFY(strFmtMsg.LoadString(IDS_NO_EXT_HELP));
	_sntprintf(szMsg, (sizeof szMsg)-1, (LPCTSTR)strFmtMsg, szFile );
	MsgBox( Information, szMsg );
}

BOOL HelpOnKeywordExt(LPCTSTR szKeyword)
{
	CWaitCursor waitCursor;
	BOOL bRet = FALSE;
	CString strExtHelpFile;
	bRet = GetExtHelpFile(strExtHelpFile);
	if (bRet)
	{
		bRet = ::WinHelp(AfxGetMainWnd()->m_hWnd, strExtHelpFile, 
			HELP_PARTIALKEY, szKeyword ? (DWORD)szKeyword : (DWORD)"");
	}
	else
		ExtHelpNotAvailable(strExtHelpFile);
	return bRet;
}

BOOL HelpContentsExt()
{
	CWaitCursor waitCursor;
	BOOL bRet;
	CString strExtHelpFile;
	bRet = GetExtHelpFile(strExtHelpFile);
	if (bRet)
	{
		bRet = ::WinHelp(AfxGetMainWnd()->m_hWnd, strExtHelpFile, 
			HELP_FINDER, 0 );
	}
	else
		ExtHelpNotAvailable(strExtHelpFile);
	return bRet;
}

BOOL HelpIndexExt(LPCTSTR szKeyword)
{
	CWaitCursor waitCursor;
	BOOL bRet;
	CString strExtHelpFile;
	bRet = GetExtHelpFile(strExtHelpFile);
	if (bRet)
	{
		bRet = ::WinHelp(AfxGetMainWnd()->m_hWnd, strExtHelpFile, 
			HELP_PARTIALKEY, szKeyword ? (DWORD)szKeyword : (DWORD)"" );
	}
	else
		ExtHelpNotAvailable(strExtHelpFile);
	return bRet;
}

///////////////////////////////////////////////////////////
//
// ChildWindowFromPointAvoidGroupBoxes
//
// Orion Bug # 12420 - DoHelpContextMenu called ChildWindowFromPoint
// which returns the first control containing the point. However,
// the group box is the first control, so items in the group box
// are not returned.
//
// Created: 22 Oct 96 - dalero
//
CWnd* ChildWindowFromPointAvoidGroupBoxes(
		CWnd* pWnd,				// Pointer to DialogBox or window with children.
		const CPoint& point		// Reference to a point in screen coordinates.
)
{
	ASSERT(pWnd != NULL) ;

	// Ensure that point is actually in the window.
	CRect rectChild ;	
	pWnd->GetWindowRect(rectChild) ;
	if (!rectChild.PtInRect(point))
	{
		// Point is not in window.
		return NULL ;
	}

	// Variable to cache a pointer to group box, if we encounter one.
	CWnd* pGroup = NULL;

	// Get first child.
	CWnd* pChild = pWnd->GetWindow(GW_CHILD) ;

	// iterate through the children
	while(pChild != NULL)
	{
		// Only check if the window is visible
		if (pChild ->IsWindowVisible())
		{
			// Get the rectangle for the child
			pChild ->GetWindowRect(&rectChild) ;

			// Check to see if the pt is in the child.
			if (rectChild.PtInRect(point))
			{
				// Check to see if this is a group box.
				if ((pGroup == NULL) &&						// Set pGroup only if not already set.
					(pChild->GetStyle() & BS_GROUPBOX))		// Set pGroup only if child is a group.
				{
					// This is a group box.
					pGroup = pChild ;
					// look for another child.
				}
				else
				{
					// This is not a group box, so we are done.
					return pChild  ;
				}
			}
		}
		// get the next control
		pChild = pChild->GetWindow(GW_HWNDNEXT) ;
	}
	return pGroup ? pGroup : pWnd;
}

