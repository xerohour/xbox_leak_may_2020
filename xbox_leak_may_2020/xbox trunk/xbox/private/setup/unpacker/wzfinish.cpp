//  WZFINISH.CPP
//
//  Created 28-Mar-2001 [JonT]

#include "unpacker.h"

CWizardFinish::CWizardFinish(
    CWizard* pWizard,
    PROPSHEETPAGE* pPSP
    ) : CWizardPage(pWizard, pPSP, IDD_FINISH)
{
    // Override any propsheet settings made by the parent for this specific page
    pPSP->dwFlags = PSP_DEFAULT | PSP_USETITLE | PSP_HIDEHEADER;
    pPSP->pszTitle = CUnpacker::GetProductName();
}


VOID
CWizardFinish::OnSetActive()
{
    char szTextBuffer[1024];
    PropSheet_SetWizButtons(::GetParent(m_hwnd), PSWIZB_FINISH);

    //Format the title
    FormatWindowText(GetDlgItem(m_hwnd, IDC_TITLE_STATIC), CUnpacker::GetProductName());

    if(GetUnpacker()->GetFlags()&UNPACKER_FLAGS_REBOOT_REQUIRED)
    {
        //Replace the now complete text, with the reboot required message.    
        FormatWindowText(GetDlgItem(m_hwnd, IDC_FINISH_TEXT), CUnpacker::GetProductName());
        rsprintf(GetInstanceHandle(), szTextBuffer, IDS_REBOOT_REQUIRED, CUnpacker::GetProductName());
        SetWindowText(GetDlgItem(m_hwnd, IDC_FINISH_TEXT), szTextBuffer);
        
        //Change the names on the buttons
        LoadString(GetInstanceHandle(), IDS_REBOOT_NOW, szTextBuffer, sizeof(szTextBuffer));
        PropSheet_SetFinishText(::GetParent(m_hwnd), szTextBuffer);
        LoadString(GetInstanceHandle(), IDS_REBOOT_LATER, szTextBuffer, sizeof(szTextBuffer));
        SetWindowText(GetDlgItem(GetParent(m_hwnd), IDCANCEL), szTextBuffer);
        EnableWindow(GetDlgItem(GetParent(m_hwnd), IDCANCEL), TRUE);
    } else
    {
        // Format the finish text with the product name.
        FormatWindowText(GetDlgItem(m_hwnd, IDC_FINISH_TEXT), CUnpacker::GetProductName());

        // Disable the cancel button
        EnableWindow(GetDlgItem(GetParent(m_hwnd), IDCANCEL), FALSE);
    }
}


VOID
CWizardFinish::OnFinish()
{
    if(GetUnpacker()->GetFlags()&UNPACKER_FLAGS_REBOOT_REQUIRED)
    {
        //Indicate that the user wants to reboot.
        m_pWizard->SetReturnValue(1);
    } else
    {
	    // We're done, set the return value
	    m_pWizard->SetReturnValue(0);
    }
}

BOOL
CWizardFinish::OnQueryCancel()
{
    if(GetUnpacker()->GetFlags()&UNPACKER_FLAGS_REBOOT_REQUIRED)
    {
        m_pWizard->SetReturnValue(0);
        SetWindowLong(m_hwnd, DWL_MSGRESULT, FALSE);
        return TRUE;
    } else
    {
        SetWindowLong(m_hwnd, DWL_MSGRESULT, TRUE);
        return TRUE;
    }
}
