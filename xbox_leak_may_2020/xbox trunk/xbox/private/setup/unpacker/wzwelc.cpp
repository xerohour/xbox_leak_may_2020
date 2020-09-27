//  WZWELC.CPP
//
//  Created 16-Mar-2001 [JonT]

#include "unpacker.h"

CWizardWelcome::CWizardWelcome(
    CWizard* pWizard,
    PROPSHEETPAGE* pPSP
    ) : CWizardPage(pWizard, pPSP, IDD_WELCOME)
{
    // Override any propsheet settings made by the parent for this specific page
    pPSP->dwFlags = PSP_DEFAULT | PSP_USETITLE | PSP_HIDEHEADER;
    pPSP->pszTitle = CUnpacker::GetProductName();
}


VOID
CWizardWelcome::OnInit()
{
    LPSTR lp;
    
    // This is the first page, and thus responsible for setting the PropSheetHwnd
    m_pWizard->SetPropSheetHwnd(::GetParent(m_hwnd));

    SendMessage(::GetParent(m_hwnd), WM_SETICON, (WPARAM)ICON_BIG,
                (LPARAM)LoadIcon(::GetInstanceHandle(), MAKEINTRESOURCE(IDI_XBOX)));

    // Update the welcome message
    lp = GetSettings()->GetString(CSTR_STRINGS, CSTR_WELCOMEMSG);
    if(lp)
    {
        if(*lp)
            SetWindowText(GetDlgItem(m_hwnd, IDC_TITLE_STATIC), lp);
        free(lp);
    }
    lp = GetSettings()->GetString(CSTR_STRINGS, CSTR_PRODUCTDESC);
    if(lp)
    {
        if(*lp)
            SetWindowText(GetDlgItem(m_hwnd, IDC_PRODUCT_DESC_TEXT), lp);
        free(lp);
    }

    // Update the install size.
    lp = GetSettings()->GetString(CSTR_DIRSIZES, CSTR_TOTALINSTALLSIZE);
    FormatWindowText(GetDlgItem(m_hwnd, IDC_INSTALL_SIZE_TEXT), lp);
    free(lp);

    // Make sure to call the base class init function as well
    CWizardPage::OnInit();
}


VOID
CWizardWelcome::OnSetActive()
{
    PropSheet_SetWizButtons(::GetParent(m_hwnd), PSWIZB_NEXT);
}

BOOL
CWizardWelcome::OnNext()
{
    CUnpacker* punpacker;
    static BOOL s_fBeenHereBefore = FALSE;

    punpacker = ::GetUnpacker();
    
    // This is a wizard so we can get called multiple times. Do this only on the
    // first try!
    if (!s_fBeenHereBefore)
    {
        // Don't do this again
        s_fBeenHereBefore = TRUE;

        // Sort of a hack to handle legacy issues revolving around the
        // old install shield based setup used by the XDK.
        punpacker->ProcessLegacySetup();

        // Do the presetup checks. Note that if these fail, the
        // temp directory is always nuked by the CUnpacker destructor.
        if (!punpacker->DoPreSetupChecks())
        {
            // In either failure case, assume that a string error message
            // was passed to WizSetLastError. Note that we default to the
            // error wizard page
            return TRUE;
        }

        //
        //  Check if we should skip the license page.  If so then we
        //  have to duplicate the things that would be done by the license page's
        //  OnNext.
        //
        LPSTR lpSkipLicensePage = GetSettings()->GetSetting(CSTR_SKIP_LICENSE_PAGE);
        if(lpSkipLicensePage)
        {
            if(toupper(*lpSkipLicensePage)=='Y')
            {
                punpacker->SetFlags(UNPACKER_FLAGS_SKIPLICENSEPAGE);
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
                } else
                {
                    SetNextPage(IDD_SELECTDIR);
                }
                free(lpSkipLicensePage);
                return TRUE;
            }
            free(lpSkipLicensePage);
        }
    }

    // Check for the skip licence page flag.  We could
    // hit this if we have been to the select dir page and back.
    if(UNPACKER_FLAGS_SKIPLICENSEPAGE&punpacker->GetFlags())
    {
        SetNextPage(IDD_SELECTDIR);
    } else
    {
        SetNextPage(IDD_LICENSE);
    }
    return TRUE;
}

