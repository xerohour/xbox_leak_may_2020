#include "stdafx.h"
#include "mfcappwz.h"
#include "customaw.h"		// For export macros
#include "lang.h"

/////////////////////////////////////////////////////////////////////////////
// exports.cpp -- Implementation of C API's exported by AppWizard


void SetCustomAppWizClass(CCustomAppWiz* pAW)
{
	projOptions.m_UserExtension.SetCustomAppWizClass(pAW);
}

CAppWizStepDlg* GetDialog(AppWizDlgID nID)
{
	return theDLL.m_pMainDlg->ExtKeyFromID(nID);
}

void SetNumberOfSteps(int nSteps)
{
	theDLL.m_pMainDlg->SetNumberOfSteps(nSteps);
}

BOOL ScanForAvailableLanguages(CStringList& strlLanguages)
{
	if (!langDlls.m_AvailableLangDlls.ScanForAvailableLanguages())
		return FALSE;

	langDlls.m_AvailableLangDlls.GetAvailableLanguages(strlLanguages);
	return TRUE;
}

void SetSupportedLanguages(LPCTSTR szSupportedLangs)
{
	langDlls.m_AvailableLangDlls.SetSupportedLanguages(szSupportedLangs);
}

HRESULT FindInterface(REFIID riid, LPVOID FAR* ppvObj)
{
#ifdef VS_PACKAGE
	return NULL;
#else
	return theApp.FindInterface(riid, ppvObj);
#endif	// VS_PACKAGE
}
