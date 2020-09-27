//  WZCOPY.CPP
//
//  Created 28-Mar-2001 [JonT]

#include "unpacker.h"

CWizardCopying::CWizardCopying(
    CWizard* pWizard,
    PROPSHEETPAGE* pPSP
    ) : CWizardPage(pWizard, pPSP, IDD_COPYING)
{
    // Override any propsheet settings made by the parent for this specific page
    pPSP->dwFlags =           PSP_DEFAULT | PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
    pPSP->pszHeaderTitle =    MAKEINTRESOURCE(IDS_COPYINGTITLE);
    pPSP->pszHeaderSubTitle = MAKEINTRESOURCE(IDS_COPYINGSUBTITLE);
    pPSP->pszTitle =          CUnpacker::GetProductName();
}

BOOL
CWizardCopying::OnMessage(
    UINT dwMessage,
    WPARAM wParam,
    LPARAM lParam
    )
{
    static DWORD dwProgress = 0;

    switch (dwMessage)
    {
    // Fatal error. Jump to error page
    case WMP_ERROR:
        WizSetLastError(wParam, lParam);
        PropSheet_SetCurSelByID(::GetParent(m_hwnd), IDD_WIZERROR);
        return TRUE;

    case WMP_COPYCOMPLETE:
        PropSheet_PressButton(::GetParent(m_hwnd), PSBTN_NEXT);
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            StopCopying();
        }
        return FALSE;
    }

    return CWizardPage::OnMessage(dwMessage, wParam, lParam);
}

VOID
CWizardCopying::OnSetActive()
{
    //Update the install text to have product name
    FormatWindowText(GetDlgItem(m_hwnd, IDC_INSTALL_TEXT), CUnpacker::GetProductName());

    // No buttons active until we're done
    PropSheet_SetWizButtons(::GetParent(m_hwnd), 0);

    // Including no cancel button
    EnableWindow(GetDlgItem(GetParent(m_hwnd), IDCANCEL), FALSE);

    // Kick off the file copy
    GetUnpacker()->StartCopy(m_hwnd, GetDlgItem(m_hwnd, IDC_PROGRESS_BAR),
        GetDlgItem(m_hwnd, IDC_ACTIONTEXT), GetDlgItem(m_hwnd, IDC_INPROGRESSTEXT));
}

