//  WIZ.CPP
//
//  Created 16-Mar-2001 [JonT]

#include "unpacker.h"

// The CWizard singleton instance.
    CWizard* CWizard::m_spThis = NULL; 

//---------------------------------------------------------------------
//  CWizard methods

int
CWizard::DoModal(
    HWND hwndParent
    )
{
    HRESULT hr;
    PROPSHEETPAGE psp[MAX_WIZARD_PAGES];
    PROPSHEETHEADER psh;

    CWizardWelcome AddPageWelcome(this, &psp[0]);
    CWizardError AddPageError(this, &psp[1]);
    CWizardLicense AddPageLicense(this, &psp[2]);
    CWizardSelectDir AddPageSelectDir(this, &psp[3]);
    CWizardCopying AddPageCopying(this, &psp[4]);
    CWizardFinish AddPageFinish(this, &psp[5]);

    ZeroMemory(&psh, sizeof (psh));
    psh.dwSize = sizeof (PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_WIZARD97 | PSH_NOAPPLYNOW |
        PSH_WATERMARK | PSH_HEADER | PSH_USEICONID;
    psh.hwndParent = hwndParent;
    psh.hInstance = ::GetInstanceHandle();
    psh.pszIcon = MAKEINTRESOURCE(IDI_XBOX);
    psh.pszCaption = MAKEINTRESOURCE(IDS_WIZARD_TITLE);
    psh.nPages = MAX_WIZARD_PAGES;
    psh.pszbmWatermark = MAKEINTRESOURCE(IDB_XWATERMARK);
    psh.pszbmHeader = MAKEINTRESOURCE(IDB_XHEADER);
    psh.nStartPage = 0;
    psh.ppsp = psp;

    m_fCentered = FALSE;

	// Assume not finished yet
	m_nReturn = -1;

    ::PropertySheet(&psh);

	return m_nReturn;
}

