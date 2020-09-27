// apwizaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see apwiztes.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CApwiztesAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();

protected:
	CDialogChooser* m_pChooser;
};

// This declares the one instance of the CApwiztesAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global Apwiztesaw.  (Its definition is in apwizaw.cpp.)
extern CApwiztesAppWiz apwiztesaw;
