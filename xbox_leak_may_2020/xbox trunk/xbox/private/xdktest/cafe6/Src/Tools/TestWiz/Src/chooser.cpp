// chooser.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TestWiz.h"
#include "chooser.h"
#include "page1dlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CDialogChooser::CDialogChooser()
{
	pDlgs[0] = NULL;

	pDlgs[1] = new CPage1Dlg;
//	pDlgs[2] = new CPage2Dlg;
//	pDlgs[2] = GetDialog(APWZDLG_DATABASE);
//	pDlgs[3] = GetDialog(APWZDLG_OLE);
//Dlgs[2] = GetDialog(APWZDLG_DOCAPPOPTIONS);

	nCurrDlg = 0;
}

CDialogChooser::~CDialogChooser()
{
	const int LAST_TestWiz_DLG = 1;
	for (int i=1; i <= LAST_TestWiz_DLG; i++)
	{
		ASSERT(pDlgs[i] != NULL);
		delete pDlgs[i];
	}
}

CAppWizStepDlg* CDialogChooser::Next(CAppWizStepDlg* pDlg)
{
	ASSERT(0 <= nCurrDlg && nCurrDlg < NUM_DLGS);
	ASSERT(pDlg == pDlgs[nCurrDlg]);

	nCurrDlg++;
	return pDlgs[nCurrDlg];
}

CAppWizStepDlg* CDialogChooser::Back(CAppWizStepDlg* pDlg)
{
	ASSERT(1 <= nCurrDlg && nCurrDlg <= NUM_DLGS);
	ASSERT(pDlg == pDlgs[nCurrDlg]);

	nCurrDlg--;
	return pDlgs[nCurrDlg];
}
