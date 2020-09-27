//  WZLICENSE.CPP
//
//  Created 27-Mar-2001 [JonT]

#include "unpacker.h"

CWizardLicense::CWizardLicense(
    CWizard* pWizard,
    PROPSHEETPAGE* pPSP
    ) : CWizardPage(pWizard, pPSP, IDD_LICENSE), m_pszTitle(NULL), m_pszSubtitle(NULL)
{
    // Override any propsheet settings made by the parent for this specific page
    pPSP->dwFlags = PSP_DEFAULT | PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
    pPSP->pszTitle = CUnpacker::GetProductName();

    // Read the title and subtitle from the ini file.
    m_pszTitle = GetSettings()->GetString(CSTR_STRINGS, CSTR_LICENSE_TITLE);
    if(*m_pszTitle)
    {
      pPSP->pszHeaderTitle = m_pszTitle;
      m_pszSubtitle = GetSettings()->GetString(CSTR_STRINGS, CSTR_LICENSE_SUBTITLE);
      if(*m_pszSubtitle)
        pPSP->pszHeaderSubTitle = m_pszSubtitle;
    }
    else
    {
      pPSP->pszHeaderTitle = MAKEINTRESOURCE(IDS_LICENSETITLE);
      pPSP->pszHeaderSubTitle = MAKEINTRESOURCE(IDS_LICENSESUBTITLE);
    }
}


VOID
CWizardLicense::OnSetActive()
{
    HANDLE hfile;
    LPVOID lp;
    TCHAR szLicenseFile[MAX_PATH];
    DWORD dwc;
    DWORD dwcActual;
    LPSTR lpLicenseFile = NULL;

    // Get the License File Name (in the CAB) from the INI
    lpLicenseFile = GetSettings()->GetSetting(CSTR_LICENSEFILE);

    // Read the license file
    strcpy(szLicenseFile, ::GetUnpacker()->GetUninstallDir());
    ::AppendSlash(szLicenseFile);
    strcat(szLicenseFile, lpLicenseFile);
    hfile = CreateFile(szLicenseFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        WizSetLastError(IDS_CORRUPT_INSTALLER, 1019);
        SetNextPage(IDD_WIZERROR);
        return;
    }

    dwc = GetFileSize(hfile, NULL);
    lp = malloc(dwc + 1);
    if (!ReadFile(hfile, lp, dwc, &dwcActual, NULL) || dwc != dwcActual)
    {
        WizSetLastError(IDS_CORRUPT_INSTALLER, 1020);
        SetNextPage(IDD_WIZERROR);
        return;
    }
    *((LPBYTE)lp + dwc) = 0;

    // Put the license text in the edit control
    SetWindowText(GetDlgItem(m_hwnd, IDC_LICENSEEDIT), (LPSTR)lp);

    // Update IDC_LICENSE_INSTRUCT with the string from the ini file.
    LPSTR lpInstructText = GetSettings()->GetString(CSTR_STRINGS, CSTR_LICENSE_INSTRUCT);
    SetWindowText(GetDlgItem(m_hwnd, IDC_LICENSE_INSTRUCT), lpInstructText);
    free(lpInstructText);

    // Clean up
    CloseHandle(hfile);
    free(lp);

    // Set default buttons
    PropSheet_SetWizButtons(::GetParent(m_hwnd), PSWIZB_BACK | PSWIZB_NEXT);
}

BOOL
CWizardLicense::OnNext()
{
    static BOOL s_fBeenHereBefore = FALSE;
    CUnpacker* punpacker = ::GetUnpacker();

    // This is a wizard, we can get moved back into
    if (!s_fBeenHereBefore)
    {
        // Load up all the target directories. If this returns TRUE, we're doing
        // an upgrade and can skip the select directory page
        if (punpacker->ProcessDirectories())
        {
            // Start the upgrade
            //
            // Do a sanity check of directories. If no good, skip to the error page.
            // Note that on the clean install case, this is done at the end of
            // the directory selection page.
            // Any disk space failure is fatal, unlike the clean install case where
            // we can let the user pick a new directory.
            // Once the directories are good, move the tempdir to the uninstall dir.
            if (!punpacker->ValidateDirectories() ||
                punpacker->CheckDiskSpace() != 0 ||
                !punpacker->MoveSettingsCab())
            {
                SetNextPage(IDD_WIZERROR);
            }
            else
            {
                SetNextPage(IDD_COPYING);
            }
        }

        // Don't do it again
        s_fBeenHereBefore = TRUE;
    }
    return TRUE;
}

VOID 
CWizardLicense::OnBack()
{
    SetNextPage(IDD_WELCOME);
}
