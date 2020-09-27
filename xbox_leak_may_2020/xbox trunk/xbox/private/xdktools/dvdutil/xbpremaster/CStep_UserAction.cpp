// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cstep_useraction.cpp
// Contents:  The first step in the xbPremaster process.  Allows the user to select the basic
//            action to be performed in this session.
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_UserAction::CStep_UserAction
// Purpose:   CStep_UserAction constructor.
// Arguments: pdlg          -- Dialog that this step will be displayed in.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CStep_UserAction::CStep_UserAction(CDlg *pdlg) : CStep(pdlg)
{
    // Make all of our controls visible
    m_pdlg->DisplayControl(IDC_USERACTION_STATIC2);
    m_pdlg->DisplayControl(IDC_USERACTION_TEXT_TITLE);
    m_pdlg->DisplayControl(IDC_USERACTION_TEXT_BODY);
    m_pdlg->DisplayControl(IDC_BTN_PREV);
    m_pdlg->DisplayControl(IDC_BTN_NEXT);
    m_pdlg->DisplayControl(IDC_BTN_EXIT);
    m_pdlg->DisplayControl(IDC_USERACTION_STATIC);
    m_pdlg->DisplayControl(IDC_USERACTION_STATIC3);
    m_pdlg->SetControlEnable(IDC_BTN_PREV, false);
    m_pdlg->SetControlEnable(IDC_BTN_NEXT, true);

    // Set the font for the text title
    m_pdlg->SetControlFont(IDC_USERACTION_TEXT_TITLE, -18, "Arial", 0);

    // Overload the background colors of some controls to white
    m_pdlg->OverloadBkColor(IDC_USERACTION_TEXT_TITLE);
    m_pdlg->OverloadBkColor(IDC_USERACTION_TEXT_BODY);
    
    // Set the Write button as the default control
    m_pdlg->SetCurControl(IDC_BTN_NEXT);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_UserAction::HandleButtonPress
// Purpose:   This function is called when the user presses a button.
// Arguments: nButtonId         -- the resource identifier of the button pressed.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_UserAction::HandleButtonPress(int nButtonId)
{
    CStep *pstepNext;
    
    switch (nButtonId)
    {
    case IDC_BTN_NEXT:
        // User wants to write the files out.
        pstepNext = new CStep_DestSource(m_pdlg);
        m_pdlg->SetNextStep(pstepNext);
        break;

    case IDC_BTN_EXIT:
    case ID_ESCAPE_KEY:
        // User wants to exit the application.  Inform the dialog that there is no 'next' step.
        m_pdlg->SetNextStep(NULL);
        break;
    }
}
