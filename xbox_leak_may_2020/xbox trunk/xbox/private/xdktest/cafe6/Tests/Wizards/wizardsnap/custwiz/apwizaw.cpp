// apwizaw.cpp : implementation file
//

#include "stdafx.h"
#include "apwiztes.h"
#include "apwizaw.h"
#include "chooser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CApwiztesAppWiz::InitCustomAppWiz()
{
	// Create a new dialog chooser; CDialogChooser's constructor initializes
	//  its internal array with pointers to the steps.
	m_pChooser = new CDialogChooser;

	// At first, we don't know the total number of steps, since there are two
	//  possible "tracks" (MDI/SDI app and dialog-based app).
	SetNumberOfSteps(-1);

	// Inform AppWizard of the languages we support
	SetSupportedLanguages("English [United States] (appwzenu.dll);0x40904e4");

	// TODO: Add any other custom AppWizard-wide initialization here.
}

// This is called just before the custom AppWizard is unloaded.
void CApwiztesAppWiz::ExitCustomAppWiz()
{
	// Deallocate memory used for the dialog chooser
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;

	// TODO: Add code here to deallocate resources used by the custom AppWizard
}

// This is called when the user clicks "Create..." on the New Project dialog
//  or "Next" on one of the custom AppWizard's steps.
CAppWizStepDlg* CApwiztesAppWiz::Next(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Next(pDlg);
}

// This is called when the user clicks "Back" on one of the custom
//  AppWizard's steps.
CAppWizStepDlg* CApwiztesAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Back(pDlg);
}

// Here we define one instance of the CApwiztesAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global apwiztesaw.
CApwiztesAppWiz apwiztesaw;

