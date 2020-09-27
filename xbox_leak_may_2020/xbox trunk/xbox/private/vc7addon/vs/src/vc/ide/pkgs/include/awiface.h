//
// CAppWizIFace
//
// AppWizard Interface to New/Insert Project dialog
//
// [davbr]
//

#ifndef _INCLUDE_APPWIZIFACE_H
#define _INCLUDE_APPWIZIFACE_H

// Size of m_pnPlatforms array in build system's CPromptDlg.  This represents the
//  maximum number of platforms we can display at once.
#define MAX_PLATFORMS 8

#include "utilctrl.h"
#include "projbld2.h"
#include <bldtypes.h>

enum {APPWIZTERMTYPE_CANCEL, APPWIZTERMTYPE_EMPTYPROJ, APPWIZTERMTYPE_APPWIZPROJ, APPWIZTERMTYPE_RERUNPROMPTDLG};

class CProjTypeList;

class CAppWizIFace
{
public:

	// Each time the new/insert project dialog pops up, call this before
	//  calling any of the other APIs.
	virtual void InitAppWiz(HWND hWnd, IServiceProvider *pSp);

	IServiceProvider *m_pSp ;
#if 0
	IBuildWizardX *m_pBldWizX ;
	IBuildPlatformsX *m_pBldPlatsX;
#endif

	void ReleasePtrs(void)
	{
#if 0
		if (m_pBldPlatsX)
		{
			m_pBldPlatsX->Release() ;
			m_pBldPlatsX = NULL ;
		}

		if (m_pBldWizX)
		{
			m_pBldWizX->Release() ;
			m_pBldWizX = NULL ;
		}
#endif

		if (m_pSp)
		{
			m_pSp->Release() ;
			m_pSp = NULL ;
		}
	}

	CAppWizIFace() 
	{ 
		m_pSp = NULL; 
#if 0
		m_pBldPlatsX = NULL ;
		m_pBldWizX = NULL ;
#endif
	}

	~CAppWizIFace() 
	{ 
		ReleasePtrs();
	}

	// This takes the projtype list box, and fills it with the standard appwiz
	//  project types.  Returns number of appwiz types.

	virtual int AddAppWizProjectTypesAtTop(CListBox* pList);
	virtual int AddAppWizProjectTypesAtTop(CProjTypeList* pList);

	// This takes the projtype list box, and fills it with any custom appwiz's it finds.
	//  It returns whether number of custom AppWizards
	virtual int AddCustomAppWizProjectTypesAtBottom(CListBox* pList);
	virtual int AddCustomAppWizProjectTypesAtBottom(CProjTypeList* pList);

	// Fills the platforms checklist with platforms supported by the indicated
	//  custom AppWizard
	virtual void FillPlatformsListFromCustomAppWizType
		(LPCTSTR szExtName, CCheckList* pChklstPlatforms);
	virtual void FillPlatformsListFromCustomAppWizType
		(LPCTSTR szExtName, CStringArray* pChklstPlatforms);

	// Fills the platforms checklist with platforms supported by the indicated
	//  AppWizard type (its index into the project type combo box).
	virtual void FillPlatformsListFromAppWizType
		(int nProjType, CCheckList* pChklstPlatforms);
	virtual void FillPlatformsListFromAppWizType
		(int nProjType, CStringArray* pChklstPlatforms);

	// After the user clicks "Create" with an appwiz type selected, but before
	//  you call RunAppWizSteps, call this and make sure it returns TRUE.  Otherwise,
	//  don't dismiss new/insert project.
	virtual BOOL CanRunAppWizSteps(LPCTSTR szProj, LPCTSTR szProjDir, LPCTSTR szExtName);

	// This runs the wizard.  It returns one of the APPWIZTERMTYPE_ values.
	// TODO: These values are currently defined in ide\include\appwzshd.h.  They
	//  need to moved here after the build system starts using this interface and
	//  no longer uses appwzshd.h.
	virtual int RunAppWizSteps();

	// If RunAppWizSteps was successful, the caller should create & pass a new HBUILDER if
	//  a new workspace is being created, otherwise the current HBUILDER if this is
	//  from insert project.  This function takes the HBUILDER and adds the AppWizard
	//  targets.
	virtual void AddAppWizTargets(HBUILDER hBld);

	virtual BOOL GetAppWizDialog( CString &strDialog );

	// Call this so AppWizard deallocates stuff.  If you run appwiz steps, call after
	//  AddAppWizTargets.  Otherwise, call after the new/insert project dialog is
	//  dismissed.
	virtual void ExitAppWiz();
};	


#define GETAPPWIZIFACE  (MAKEINTRESOURCE(1))
typedef CAppWizIFace* (WINAPI* PGETAPPWIZIFACE)();

extern CAppWizIFace g_AppWizIFace;

#endif // _INCLUDE_APPWIZIFACE_H
