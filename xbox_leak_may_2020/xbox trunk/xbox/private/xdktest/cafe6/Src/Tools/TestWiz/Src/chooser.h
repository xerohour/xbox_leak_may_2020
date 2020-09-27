#ifndef __CHOOSER_H__
#define __CHOOSER_H__

#define NUM_DLGS 1

class CDialogChooser
{
public:
	CDialogChooser();
	~CDialogChooser();
	CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);

protected:
	int nCurrDlg;
	CAppWizStepDlg* pDlgs[NUM_DLGS+1];
};

#endif //__CHOOSER_H__
