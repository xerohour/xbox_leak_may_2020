#include "stdafx.h"
#include "mfcappwz.h"
#include "seqdlg.h"

#ifndef VS_PACKAGE
#include <dlgbase.h>
#endif	// VS_PACKAGE

void MakeOKDefault(CMainDlg* pMainDlg);
BOOL IsaMainBtn(char c);		// Defined in mfcappwz.cpp

//////////////////////////////////////////////////////////////////////////////
// CSeqDlg dialog class

CSeqDlg::CSeqDlg(UINT IDD, CWnd *pParent)
    : C3dDialog(IDD, pParent)
{
    m_pParentWnd = pParent;
    m_nTemplate = IDD;
}

BOOL CSeqDlg::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
    return C3dDialog::Create(MAKEINTRESOURCE(nIDTemplate), pParentWnd);
}

LRESULT CSeqDlg::OnGotoBeginning(WPARAM wparam, LPARAM lparam)
{
	((CMainDlg*) GetParent())->ActivateDlg(0);
    return 0;
}

BOOL CSeqDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
    // Make sure the proper button is default
    MakeOKDefault((CMainDlg*) m_pParentWnd);
    return C3dDialog::OnCommand(wParam, lParam );
}

BOOL CSeqDlg::PreTranslateMessage(MSG* pMsg)
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

 #ifndef PRIVATE	
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
					return C3dDialog::PreTranslateMessage(pMsg);
				}
			}
			pMsg->hwnd = dlg->GetDlgItem(IDOK)->GetSafeHwnd();
			// ::SetFocus(pMsg->hwnd);
			dlg->GetDlgItem(IDC_BOGUS_DEFBTN)->SetFocus();
			return dlg->IsDialogMessage(pMsg);
		}	
				

		else
			return C3dDialog::PreTranslateMessage(pMsg);
	}
#else //PRIVATE */

	CDialog* dlg = (CDialog*) m_pParentWnd;
	if (dlg->IsDialogMessage(pMsg))
		return TRUE;
// #endif //PRIVATE

    return C3dDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CSeqDlg, C3dDialog)
    //{{AFX_MSG_MAP(CSeqDlg)
	ON_MESSAGE(WM_GOTO_BEGINNING, OnGotoBeginning)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
    //}}AFX_MSG_MAP(CSeqDlg)
END_MESSAGE_MAP()


void CSeqDlg::OnContextMenu(CWnd* pWnd, CPoint pt)
{
#ifndef VS_PACKAGE
	// call helper in shell
	::DoHelpContextMenu(this, m_nIDHelp, pt);
#endif	// VS_PACKAGE
}

BOOL CSeqDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
#ifdef VS_PACKAGE
	return FALSE;
#else
	// call helper in shell
	return ::DoHelpInfo(this, m_nIDHelp, pHelpInfo);
#endif	// VS_PACKAGE
}

