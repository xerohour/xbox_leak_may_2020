//  WZERROR.CPP
//
//  Created 27-Mar-2001 [JonT]

#include "unpacker.h"

    TCHAR CWizardError::m_szLastError[1024];
    DWORD CWizardError::m_idLastError;
    DWORD CWizardError::m_dwErrorCode;

CWizardError::CWizardError(
    CWizard* pWizard,
    PROPSHEETPAGE* pPSP
    ) : CWizardPage(pWizard, pPSP, IDD_WIZERROR)
{
    // Override any propsheet settings made by the parent for this specific page
    pPSP->dwFlags =           PSP_DEFAULT | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
    pPSP->pszHeaderTitle =    MAKEINTRESOURCE(IDS_ERRORTITLE);
    pPSP->pszHeaderSubTitle = MAKEINTRESOURCE(IDS_ERRORSUBTITLE);

    if(pPSP->pszTitle)
    {
        pPSP->dwFlags |= PSP_USETITLE;
        pPSP->pszTitle = CUnpacker::GetProductName();
    }

    // Make sure we don't have any strings
    m_idLastError = 0;
    m_szLastError[0] = 0;
    m_dwErrorCode = 0;
}

VOID
CWizardError::OnSetActive()
{
    TCHAR szErrorText[4096];

    // If there's no error set, display the general one
    if (m_idLastError == 0 && m_szLastError[0] == 0)
        m_idLastError = IDS_ERROR_GENERAL;

    // Display the error message
    if (m_idLastError)
    {
        if(
            (IDS_ERRORNOTWINDOWS2000 == m_idLastError) ||
            (IDS_SETUP_NOT_COMPLETE == m_idLastError) ||
            (IDS_ERRORINSTALLOVERNEWER == m_idLastError)
        ) {
            rsprintf(GetInstanceHandle(), szErrorText, m_idLastError, CUnpacker::GetProductName());
        } else
            LoadString(GetInstanceHandle(), m_idLastError, szErrorText, 4096);
        SetWindowText(GetDlgItem(m_hwnd, IDC_STATIC_TEXT), szErrorText);
    }
    else
        SetWindowText(GetDlgItem(m_hwnd, IDC_STATIC_TEXT), m_szLastError);

    // Set the error code in place
    sprintf(szErrorText, "S%d", m_dwErrorCode);
    SetWindowText(GetDlgItem(m_hwnd, IDC_ERROR_CODE), szErrorText);

    // Enable just the finish button
    PropSheet_SetWizButtons(::GetParent(m_hwnd), PSWIZB_FINISH);

    // Disable the cancel button
    EnableWindow(GetDlgItem(GetParent(m_hwnd), IDCANCEL), FALSE);
}

VOID
CWizardError::OnFinish()
{
    PostQuitMessage(-1);
}

