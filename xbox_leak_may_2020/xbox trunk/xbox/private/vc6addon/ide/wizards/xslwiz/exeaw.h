#if !defined(AFX_EXEAW_H__FF0FA092_B53D_11D0_AE6D_D8DC2A000000__INCLUDED_)
#define AFX_EXEAW_H__FF0FA092_B53D_11D0_AE6D_D8DC2A000000__INCLUDED_

// exeaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see exe.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CXslAppWiz : public CCustomAppWiz
{
public:
	//virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	//virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);
    virtual void GetPlatforms(CStringList& rPlatforms);

protected:
	//CDialogChooser* m_pChooser;
};

// This declares the one instance of the CExeAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global Exeaw.  (Its definition is in exeaw.cpp.)
extern CXslAppWiz Xslaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXEAW_H__FF0FA092_B53D_11D0_AE6D_D8DC2A000000__INCLUDED_)
