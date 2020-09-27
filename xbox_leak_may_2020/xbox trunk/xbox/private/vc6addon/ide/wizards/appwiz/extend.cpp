#include "stdafx.h"
#include "mfcappwz.h"
#include "extend.h"
#include "codegen.h"	// For ReportAndThrow2
#include "symbols.h"

extern CMyCustomAppWiz MyAWX;

CUserExtension::CUserExtension()
{
	m_strUserDLL.Empty();
	m_hUserDLL = NULL;
	m_pAWX = &MyAWX;
	m_bCtlWiz = FALSE;
}

CUserExtension::~CUserExtension()
{
	// Detach before letting it go out of scope
	Detach();
}

/////////////////////////////////////////////////////////////////////////////
// Helper member functions

void CUserExtension::SetCustomAppWizClass(CCustomAppWiz* pAW)
{
	m_bSetCustomAppWizClassCalled = TRUE;

	if (pAW == NULL)
		m_bCalledSetCustomAppWizWithNULL = TRUE;
	else
	{
		m_bCalledSetCustomAppWizWithNULL = FALSE;
		m_pAWX = pAW;
	}
}

BOOL CUserExtension::Attach(const TCHAR* szUserExtension)
{
	// If we're already attached to something else, detach first.
	if (m_hUserDLL != NULL)
	{
		if (m_strUserDLL == szUserExtension)
		{
			// We've already attached to THIS extension
			return TRUE;       // So no need to reattach
		}
		else
		{
			// We're attached to something else, so detach first
			Detach();
		}
	}

	m_bSetCustomAppWizClassCalled = FALSE;
	HINSTANCE hLib = LoadLibrary(szUserExtension);
	if (hLib == NULL)
	{
		CString strPrompt;
		AfxFormatString1(strPrompt, IDP_CANT_LOAD_EXTENSION, szUserExtension);
		AfxMessageBox(strPrompt);
		return FALSE;
	}
	if (!m_bSetCustomAppWizClassCalled)
	{
		FreeLibrary(hLib);
		CString strPrompt;
		AfxFormatString1(strPrompt, IDP_EXT_NOT_CALL_SETAWX, szUserExtension);
		AfxMessageBox(strPrompt);
		return FALSE;
	}
	if (m_bCalledSetCustomAppWizWithNULL)
	{
		FreeLibrary(hLib);
		CString strPrompt;
		AfxFormatString1(strPrompt, IDP_SETAWX_WITH_NULL, szUserExtension);
		AfxMessageBox(strPrompt);
		return FALSE;
	}

	m_strUserDLL = szUserExtension;
	m_hUserDLL = hLib;

	projOptions.m_nProjType = PROJTYPE_MDI;
	if (theDLL.m_pMainDlg != NULL)
		theDLL.m_pMainDlg->SetNumberOfSteps(-1);
	SetProjectTypeSymbol();
	theDLL.m_pMainDlg->InitializeOptions();	// Init the extension's dictionary w/ defaults
	m_pAWX->InitCustomAppWiz();
	ASSERT (m_bCtlWiz == FALSE);

	return TRUE;
}

void CUserExtension::Detach()
{
	if (m_hUserDLL == NULL)
		return;
	if (theDLL.m_pMainDlg != NULL)
		theDLL.m_pMainDlg->DestroyUserDlgs();
	ASSERT(m_pAWX != NULL);
	m_pAWX->ExitCustomAppWiz();
	FreeLibrary(m_hUserDLL);
	m_strUserDLL.Empty();
	m_strExtensionTitle.Empty();
	m_hUserDLL = NULL;
	m_bCtlWiz = FALSE;
	m_pAWX = &MyAWX;
}

CCustomAppWiz* GetAWX()
{
	return projOptions.m_UserExtension.GetAWX();
}

BOOL IsUserAWX()
{
	return projOptions.m_UserExtension.IsUserAWX();
}

