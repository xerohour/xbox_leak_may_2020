#include "customaw.h"
#ifndef _AWX_
#define _AWX_

class CDialogChooser;

class CTestWizAppWizExtension : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
													  
protected:
	CDialogChooser* m_pChooser;
};

extern CTestWizAppWizExtension awx;

#endif //_AWX_
