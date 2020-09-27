
#include "stdafx.h"
#include "mfcappwz.h"
#include "customaw.h"		// Has declaration of CAppWizStepDlg
#include "codegen.h"		// For ReportErrorExt
#ifndef VS_PACKAGE
#include <dlgbase.h>
#endif	// VS_PACKAGE

// This flag is used to sense whether AppWizard-proper or an extension is
//  calling the CAppWizStepDlg constructor or destructor.  The extension
//  may not call the constructor with 0, and may NEVER call the destructor.
static BOOL s_bAppWizCallingStructor;
void AppWizCallingStructor(BOOL bAppWizCallingStructor)
{
	s_bAppWizCallingStructor = bAppWizCallingStructor;
}

/////////////////////////////////////////////////////////////////////////////
// pagedlg.cpp -- Implementation of CAppWizStepDlg

BOOL IsaMainBtn(char c);		// Defined in mfcappwz.cpp

//////////////////////////////////////////////////////////////////////////////
// CAppWizStepDlg dialog class

CAppWizStepDlg::CAppWizStepDlg(UINT nIDTemplate)
	: CDialog()
{
	// Before constructing, make sure extension isn't calling us with 0
	if (nIDTemplate == 0				// We're being called with 0
		&& !s_bAppWizCallingStructor)		// And AppWizard isn't doing it
	{
		ReportErrorExt(IDP_AWX_CALLED_BAD_CONSTRUCTOR);
	}
	m_pParentWnd = (CWnd*) theDLL.m_pMainDlg;
	m_lpszTemplateName = MAKEINTRESOURCE(nIDTemplate);
	m_nIDTemplate = nIDTemplate;
	m_nIDHelp = nIDTemplate;
}

CAppWizStepDlg::~CAppWizStepDlg()
{
	// Before destructing, make sure extension isn't trying to delete
	//  a seqdlg key
	if (m_nIDTemplate == 0				// It's a seqdlg key
		&& !s_bAppWizCallingStructor)	// And AppWizard isn't doing it
	{
		ReportErrorExt(IDP_AWX_CALLED_BAD_DESCRUCTOR);
		theDLL.m_pMainDlg->UserDeletedStandardStep(this);
	}
}

BOOL CAppWizStepDlg::Create(UINT nIDTemplate, CWnd* pParentWnd /* = NULL */)
{
	LPCTSTR lpszTemplateName = MAKEINTRESOURCE(nIDTemplate);
	C3dDialogTemplate dt;

	if (dt.Load(lpszTemplateName))
	{
		SetStdFont(dt);
	}
	
	return CDialog::CreateIndirect(dt.GetTemplate(), pParentWnd);
}

BOOL CAppWizStepDlg::OnDismiss()
{
	return TRUE;
}

BOOL CAppWizStepDlg::PreTranslateMessage(MSG* pMsg)
{
    // CSeqDlg shouldn't be sent a close command.
/*    if (pMsg->message == WM_SYSCOMMAND && pMsg->wParam == SC_CLOSE)
        return TRUE;

	// Escape should close the main dialog.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
		// Explicitly set focus to button to avoid IME problem.
		((CDialog*) m_pParentWnd)->GetDlgItem(IDCANCEL)->SetFocus();
		m_pParentWnd->SendMessage(WM_COMMAND,
			MAKEWPARAM(IDCANCEL, BN_CLICKED),
            (LPARAM) (m_pParentWnd->GetDlgItem(IDCANCEL)->m_hWnd));
        return TRUE;
	}

	// Return should go to next dialog, since focus is in CSeqDlg, and thus
	//  IDOK is the default button.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
		// First, make sure that the control active doesn't want the return
		CWnd* pWnd = CWnd::GetFocus();
		if (pWnd != NULL)
		{
			// Someone has focus.  Is it an edit control?
			HWND hwnd = pWnd->GetSafeHwnd();
			ASSERT (hwnd != NULL);
			char szClassName[256];
			::GetClassName(hwnd, szClassName, 256);
			if (_tcscmp(szClassName, "EDIT"))
			{
				// It's an edit control-- does it want return?
				if (pWnd->GetStyle() & 0x1000L)
					// Yes, it does, so give it the return.
					return CDialog::PreTranslateMessage(pMsg);
			}
		}

		// Don't have to worry about giving the return to a control, so
		//  advance as if Next were hit.
        DWORD nID = ((CDialog*) m_pParentWnd)->GetDefID();
        if (nID == 0) nID = IDOK;
        nID &= 0x0000FFFF;
		// Explicitly set focus to button to avoid IME problem.
		((CDialog*) m_pParentWnd)->GetDlgItem(nID)->SetFocus();
        m_pParentWnd->SendMessage(WM_COMMAND,
            MAKEWPARAM(nID, BN_CLICKED),
            (LPARAM) (m_pParentWnd->GetDlgItem(nID)->m_hWnd));
        return TRUE;
    }
	
	// Handle tabbing back into the tab area when tabbing away from
    // either end of the subject's tab order
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB &&
        (SendMessage(WM_GETDLGCODE) & (DLGC_WANTALLKEYS | DLGC_WANTMESSAGE | DLGC_WANTTAB)) == 0)
    {
        CWnd* pCtl = GetFocus();
        if (IsChild(pCtl))
        {
            do
            {
                int nKeyState = GetKeyState(VK_SHIFT);
                pCtl = pCtl->GetWindow(nKeyState < 0 ? GW_HWNDPREV : GW_HWNDNEXT);

                if (pCtl == NULL)
                {
                    if (nKeyState < 0)
                    {
                        // Shift-tabbing, so highlight last button
                        if (((CMainDlg*) m_pParentWnd)->m_nCurrIndex
                            == ((CMainDlg*) m_pParentWnd)->m_nCurrLastIndex)
                        {
                            // On last dialog, so highlight Create button
                            m_pParentWnd->GetDlgItem(IDOK)->SetFocus();
                        }
                        else
                        {
                            // On non-last dialog, so highlight rightmost button
							int nRightmostBtn = ((CMainDlg*) m_pParentWnd)->m_nRightmostBtn;
                            m_pParentWnd->GetDlgItem(nRightmostBtn)->SetFocus();
							((CDialog*) m_pParentWnd)->SetDefID(nRightmostBtn);
							if (nRightmostBtn != IDOK)
								((CButton*) (m_pParentWnd->GetDlgItem(IDOK)))
									->SetButtonStyle(BS_PUSHBUTTON);
                        }
                    }
                    else
                    {
                        // Regular-tabbing, so highlight HELP
                        m_pParentWnd->GetDlgItem(ID_HELP)->SetFocus();
						((CDialog*) m_pParentWnd)->SetDefID(ID_HELP);
						((CButton*) (m_pParentWnd->GetDlgItem(IDOK)))
							->SetButtonStyle(BS_PUSHBUTTON);
                    }
                    return TRUE;
                }
            }
            while ((pCtl->GetStyle() & (WS_DISABLED | WS_TABSTOP)) != WS_TABSTOP);
        }
	}

	else if (pMsg->message == WM_SYSCHAR || pMsg->message == WM_CHAR)
	{
		// If user tried kbd equivalent for CMainDlg's buttons, pass the
		//  the message to CMainDlg.
		if (IsaMainBtn((char) pMsg->wParam))
		{
			CDialog* dlg = (CDialog*) m_pParentWnd;
			if (pMsg->message == WM_CHAR)
			{
				// We only pass the message to CMainDlg if a button
				//  currently has the focus.
				HWND hwnd = ::GetFocus();
				char szClassName[256];
				::GetClassName(hwnd, szClassName, 256);
				if (strcmp(szClassName, "Button"))
				{
					// Not a button, so do the default.
					return CDialog::PreTranslateMessage(pMsg);
				}
			}
			pMsg->hwnd = dlg->GetDlgItem(IDOK)->GetSafeHwnd();
			// ::SetFocus(pMsg->hwnd);
			dlg->GetDlgItem(IDC_BOGUS_DEFBTN)->SetFocus();
			return dlg->IsDialogMessage(pMsg);
		}	
				

		else
			return CDialog::PreTranslateMessage(pMsg);
	}*/


	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
		// First, make sure that the control active doesn't want the return
		CWnd* pWnd = CWnd::GetFocus();
		if (pWnd != NULL)
		{
			// Someone has focus.  Is it an edit control?
			HWND hwnd = pWnd->GetSafeHwnd();
			ASSERT (hwnd != NULL);
			char szClassName[256];
			::GetClassName(hwnd, szClassName, 256);
			if (!_tcsicmp(szClassName, "EDIT"))
			{
				// It's an edit control-- does it want return?
				if (pWnd->GetStyle() & 0x1000L)
					// Yes, it does, so give it the return.
					return CDialog::PreTranslateMessage(pMsg);
			}
			else if (!_tcsicmp(szClassName, "BUTTON"))
			{
				DWORD dwBtnStyles = pWnd->GetStyle() & 0x0000000F;
				if ((dwBtnStyles == BS_PUSHBUTTON) || (dwBtnStyles == BS_DEFPUSHBUTTON))
				{
					int nID = pWnd->GetDlgCtrlID();
					SendMessage(WM_COMMAND,
						MAKEWPARAM(nID, BN_CLICKED),
						(LPARAM) (hwnd));
						return TRUE;
				}
			}
		}
    }
	// Escape should close the main dialog.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
		// Explicitly set focus to button to avoid IME problem.
		((CDialog*) m_pParentWnd)->GetDlgItem(IDCANCEL)->SetFocus();
		m_pParentWnd->SendMessage(WM_COMMAND,
			MAKEWPARAM(IDCANCEL, BN_CLICKED),
            (LPARAM) (m_pParentWnd->GetDlgItem(IDCANCEL)->m_hWnd));
        return TRUE;
	}


	CDialog* dlg = (CDialog*) m_pParentWnd;
	if (dlg->IsDialogMessage(pMsg))
		return TRUE;

    return CDialog::PreTranslateMessage(pMsg);
}
