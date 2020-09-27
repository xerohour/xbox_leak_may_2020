#include "stdafx.h"
#include "TestWiz.h"
#include "testawx.h"
#include "chooser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void CTestWizAppWizExtension::InitCustomAppWiz()
{
	m_pChooser = new CDialogChooser;
	SetNumberOfSteps(NUM_DLGS);
}

void CTestWizAppWizExtension::ExitCustomAppWiz()
{
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;
}

CAppWizStepDlg* CTestWizAppWizExtension::Next(CAppWizStepDlg* rpDlg)
{
	return m_pChooser->Next(rpDlg);
}

CAppWizStepDlg* CTestWizAppWizExtension::Back(CAppWizStepDlg* rpDlg)
{
	return m_pChooser->Back(rpDlg);
}

CTestWizAppWizExtension awx;
