#if !defined(AFX_STRIPWIZARDAW_H__EE6B614A_1416_4F02_A5FA_2515EEB789C6__INCLUDED_)
#define AFX_STRIPWIZARDAW_H__EE6B614A_1416_4F02_A5FA_2515EEB789C6__INCLUDED_

// StripWizardaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see StripWizard.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CStripWizardAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);
};

// This declares the one instance of the CStripWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global StripWizardaw.  (Its definition is in StripWizardaw.cpp.)
extern CStripWizardAppWiz StripWizardaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRIPWIZARDAW_H__EE6B614A_1416_4F02_A5FA_2515EEB789C6__INCLUDED_)
