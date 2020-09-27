#include "precomp.h"
#include "resource.h"
#include "tlsetup.h"
#include "tlutil.h"

TSetupDialog*	TSetupDialog::_this	= NULL;


BOOL APIENTRY
TSetupDialog::DialogProc(
    HWND    hWnd,
    UINT    msg,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
	if (msg == WM_INITDIALOG)
	{
		_this = (TSetupDialog*) lParam;
		_this->m_hWnd = hWnd;
	}
	
    switch (msg)
    {
		case WM_INITDIALOG: return _this->OnInitDialog ();
		case WM_COMMAND: return _this->OnCommand (wParam);
		default: return FALSE;
	}

	return FALSE;
}


BOOL
TSetupDialog::OnCommand(
	WPARAM	cmd
	)
{
	switch (cmd)
	{
		case IDHELP: return OnCommand_Help ();
		case IDOK: return OnCommand_OK ();
		case IDCANCEL: return OnCommand_Cancel ();
		default: return FALSE;
	}

	return FALSE;
}

		
BOOL
TSetupDialog::OnInitDialog(
	)
{
	SetDlgItemText (m_hWnd, IDNAME, m_hostName);
//	SetDlgItemText (m_hWnd, IDPW, lptlctl->szATPassword);
//	SetWindowText (m_hWnd, GetDialogTitle (m_mpt));

#if 0			
		   
	//
	//  The new spec doesn't seem to have anything in the TLSS about
	//  help.  Investigate this.
	
	hCtrl = GetDlgItem (hdlg, IDHELP);
	Assert (hCtrl != NULL);
	EnableWindow (hCtrl, !!(ptlui->lpfnInvokeHelp));
#endif

	return TRUE;
}

#if 0

BOOL
TSetupDialog::OnCommandHelp(
	)
{
	return TRUE;

	if (ptlui->lpfnInvokeHelp != NULL)
	{
		ptlui->lpfnInvokeHelp (IDD_TL_HELP);
		SetWindowLong (hdlg, DWL_MSGRESULT, TRUE);
		return TRUE;
	}
}
#endif


BOOL
TSetupDialog::OnCommand_Help(
	)
{

#if 0
	// can only display help from dev studio, not remote monitor.

	if(glptlssSav->fRMAttached)
	{
		TCHAR tchMsgTitle[MAX_MSG_TITLE];
		if(LoadString(hInstance, IDS_MSGBOX_TITLE, tchMsgTitle, MAX_MSG_TITLE))
		{
			TCHAR tchMsgText[MAX_MSG_TEXT];
			if(LoadString(hInstance, IDS_HELP, tchMsgText, MAX_MSG_TEXT))
				MessageBox(NULL, tchMsgText, tchMsgTitle, MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
		}
	}
	
	// else

	// TODO(michma): TL help needs to be plugged in for dev studio.
#endif

	return TRUE;

}

BOOL
TSetupDialog::OnCommand_OK(
	)
{
	UINT iRet;

	iRet = GetDlgItemText (m_hWnd, IDNAME, m_hostName, HOST_NAME_LEN);
//	iRet = GetDlgItemText ( hdlg, IDPW, szTmp, _MAX_PATH );

	EndDialog (m_hWnd, IDOK);
	return TRUE;
}


BOOL
TSetupDialog::OnCommand_Cancel(
	)
{
	*m_hostName = '\000';
	EndDialog (m_hWnd, IDCANCEL);
	return TRUE;
}


int
TSetupDialog::DoModal(
	HWND	hWndParent
	)
{
	return DialogBoxParam (hInstance, _T ("SETUP"), hWndParent, DialogProc,
						   (LPARAM) this);
}


extern "C" __declspec(dllexport)
VOID WINAPI
TLSetup(
    const TLSS* tlss
    )
{
    TSetupDialog    dlg;

    if (tlss->fInteractive)
    {
        if (!GetHostName (tlss->mpt, dlg.m_hostName, HOST_NAME_LEN,
                          tlss->lpfnGetSet))
        {
            *dlg.m_hostName = '\000';
        }

        dlg.m_mpt = tlss->mpt;

        if (dlg.DoModal ((HWND) tlss->lpvPrivate) == IDOK)
        {
            SetHostName (tlss->mpt, dlg.m_hostName, HOST_NAME_LEN,
                         tlss->lpfnGetSet);
        }
    }
}
