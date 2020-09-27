// ConvertDlg.h : Declaration of the CConvertDlg

#ifndef __CONVERTDLG_H_
#define __CONVERTDLG_H_

#include <atlhost.h>
#include <multimon.h>

void WINAPI ClipOrCenterRectToMonitor(LPRECT prc);

/////////////////////////////////////////////////////////////////////////////
// CConvertDlg
class CConvertDlg : 
	public CAxDialogImpl<CConvertDlg>
{
public:
	CConvertDlg( LPCOLESTR szProject )
	{
		m_strProject = szProject;
	}

	~CConvertDlg()
	{
	}

	enum { IDD = IDD_CONVERTDLG };

BEGIN_MSG_MAP(CConvertDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	COMMAND_ID_HANDLER(IDC_YESBUTTON, OnYes)
	COMMAND_ID_HANDLER(IDC_NOBUTTON, OnNo)
	COMMAND_ID_HANDLER(IDC_YESTOALL, OnAll)
	COMMAND_ID_HANDLER(IDC_NOTOALL, OnNone)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		s_bCancel = TRUE;
		EndDialog(0);
		return 1;
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		s_bCancel = FALSE;
		CWindow wndText = GetDlgItem(IDC_CONVERTMSG);
		CStringW strTextW;
		strTextW.Format(IDS_CONVERTMSG, m_strProject);
		
		CString strText = strTextW;
		wndText.SetWindowText(strText);

		RECT rc;
	    GetWindowRect(&rc);
   		ClipOrCenterRectToMonitor(&rc);
	    SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		return 1;  // Let the system set the focus
	}

	LRESULT OnYes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnNo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		s_bCancel = TRUE;
		EndDialog(wID);
		return 0;
	}

	LRESULT OnAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		s_bConvertAll = TRUE;
		s_bConvertNone = FALSE;
		EndDialog(wID);
		return 0;
	}

	LRESULT OnNone(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		s_bCancel = TRUE;
		s_bConvertNone = TRUE;
		s_bConvertAll = FALSE;
		EndDialog(wID);
		return 0;
	}

	CStringW m_strProject;
	static BOOL s_bConvertAll;
	static BOOL s_bConvertNone;
	static BOOL s_bCancel;
};

//
//  ClipOrCenterRectToMonitor
//
//  The most common problem apps have when running on a
//  multimonitor system is that they "clip" or "pin" windows
//  based on the SM_CXSCREEN and SM_CYSCREEN system metrics.
//  Because of app compatibility reasons these system metrics
//  return the size of the primary monitor.
//
//  This shows how you use the new Win32 multimonitor APIs
//  to do the same thing.
//
void WINAPI ClipOrCenterRectToMonitor(LPRECT prc)
{

    RECT        rc;

	CComPtr<IVsUIShell> pUIShell;
	HWND hwnd;
	HRESULT hr;
	hr = GetBuildPkg()->GetVsUIShell(&pUIShell, TRUE /* in main thread */);
	if (SUCCEEDED(hr) && pUIShell)
	{
		hr = pUIShell->GetDialogOwnerHwnd(&hwnd);
	}
	::GetWindowRect(hwnd, &rc);
	
    int         w = prc->right  - prc->left;
    int         h = prc->bottom - prc->top;

    //
    // center or clip the passed rect to the monitor rect
    //
	prc->left   = rc.left + (rc.right  - rc.left - w) / 2;
	prc->top    = rc.top  + (rc.bottom - rc.top  - h) / 2;
	prc->right  = prc->left + w;
	prc->bottom = prc->top  + h;
}
#endif //__CONVERTDLG_H_
