//  WIZPAGE.CPP
//
//  Created 18-Mar-2001 [JonT]

#include "unpacker.h"

//---------------------------------------------------------------------
//  CWizardPage methods

CWizardPage::CWizardPage(
    CWizard* pWizard,
    PROPSHEETPAGE* pPSP,
    UINT uPageID,
    DLGPROC pfnDlgProc,
    PUINT puResult
    )
: m_hwnd(NULL), 
  m_pWizard(pWizard)
{
    GetPageByID(pPSP, uPageID, pfnDlgProc);

    // Set up the font for the titles on the intro and ending pages
    NONCLIENTMETRICS ncm = {0};
    ncm.cbSize = sizeof (ncm);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

    // Create the intro/end title font
    LOGFONT TitleLogFont = ncm.lfMessageFont;
    TitleLogFont.lfWeight = FW_BOLD;
    lstrcpy(TitleLogFont.lfFaceName, TEXT("Verdana Bold"));

    HDC hdc = GetDC(NULL); // gets the screen DC
    INT FontSize = 12;
    TitleLogFont.lfHeight = 0 - GetDeviceCaps(hdc, LOGPIXELSY) * FontSize / 72;
    m_hTitleFont = CreateFontIndirect(&TitleLogFont);
    ReleaseDC(NULL, hdc);
}


BOOL CALLBACK 
CWizardPage::DlgProc(
    HWND hDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    BOOL bMsgHandled = FALSE;

    // WM_INITDIALOG: initialize some stuff then call the OnInit routine
    if (uMsg == WM_INITDIALOG)
    {
        CWizardPage* pDlg = (CWizardPage*) ((PROPSHEETPAGE*)lParam)->lParam;
        ::SetWindowLongA(hDlg, DWL_USER, (LONG)pDlg);
        pDlg->m_hwnd = hDlg;

        // Set the title font.
        if (pDlg->m_hTitleFont)
            ::SendMessage(::GetDlgItem(pDlg->m_hwnd, IDC_TITLE_STATIC), WM_SETFONT,
                          (WPARAM)pDlg->m_hTitleFont, 0);

        pDlg->OnInit();
        return TRUE;
    }

    // Any other message pass on to the OnMessage method
    CWizardPage* pDlg = (CWizardPage*) ::GetWindowLongA(hDlg, DWL_USER);
    if (pDlg != NULL)
        return pDlg->OnMessage(uMsg, wParam, lParam);
    else
        return FALSE;
}


BOOL
CWizardPage::OnMessage(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    RECT rect;
    int x;
    int y;
    HDC hdc;
    HWND hwndParent;

    
    switch (uMsg)
    {
    case WM_SIZE:
        // Only do the centering once. This method ensures that
        if (!m_pWizard->TestAndSetCentered())
        {
            hwndParent = GetParent(m_hwnd);
            GetWindowRect(hwndParent, &rect);
            rect.right -= rect.left;
            rect.bottom -= rect.top;
            hdc = CreateCompatibleDC(NULL);
            x = GetDeviceCaps(hdc, HORZRES) / 2 - rect.right / 2;
            y = GetDeviceCaps(hdc, VERTRES) / 2 - rect.bottom / 2;
            DeleteDC(hdc);
            SetWindowPos(hwndParent, NULL, x, y, 0, 0, SWP_NOSIZE);
        }
        break;

    case WM_NOTIFY:
        switch (((NMHDR*)lParam)->code)
        {
        case PSN_SETACTIVE:
            OnSetActive();
            return TRUE;

        case PSN_QUERYCANCEL:
            return OnQueryCancel();
        
        case PSN_WIZNEXT:
            return OnNext();
        
        case PSN_WIZBACK:
            OnBack();
            return TRUE;

        case PSN_WIZFINISH:
            OnFinish();
            return TRUE;
        }
        break;
    }
    
    return FALSE;
}

BOOL
CWizardPage::OnQueryCancel()
{
    // Check first to see if the cancel button is disabled. If it is, this must be
    // from the close button and we want to ignore it too
    if (!IsWindowEnabled(GetDlgItem(GetParent(m_hwnd), IDCANCEL)))
    {
        SetWindowLong(m_hwnd, DWL_MSGRESULT, TRUE);
        return TRUE;
    }

    // Confirmation messagebox. If yes is returned, allow the cancel.
    // On no, cancel the cancel
    TCHAR szConfirmation[MAX_PATH];
    LoadString(::GetInstanceHandle(), IDS_MBCANCEL, szConfirmation, MAX_PATH);
    if (MessageBox(GetParent(m_hwnd), szConfirmation, CUnpacker::GetProductName(), MB_YESNO) == IDYES)
        SetWindowLong(m_hwnd, DWL_MSGRESULT, FALSE);
    else
        SetWindowLong(m_hwnd, DWL_MSGRESULT, TRUE);
        return TRUE;
}

//  CWizardPage::SetNextPage
//      Sets page to go to next. Note that this works to go back to the same
//      page as well as on back.

VOID
CWizardPage::SetNextPage(
    UINT uPageID
    )
{
    SetWindowLong(m_hwnd, DWL_MSGRESULT, uPageID);
}


// Note: GetPageByID() needs to be virtual since "this" is used.
VOID 
CWizardPage::GetPageByID(
    PROPSHEETPAGE* pPSP,
    UINT uPageID,
    DLGPROC pfnDlgProc
    )
{
    pPSP->dwSize = sizeof (PROPSHEETPAGE);
    pPSP->dwFlags = PSP_DEFAULT;
    pPSP->hInstance = ::GetInstanceHandle();
    pPSP->pszTemplate = MAKEINTRESOURCE(uPageID);
    pPSP->pszIcon = NULL;
    pPSP->pfnDlgProc = pfnDlgProc;
    pPSP->pszTitle = NULL;
    pPSP->lParam = (LPARAM) this;
    pPSP->pfnCallback = NULL;
    pPSP->pcRefParent = NULL;
}


