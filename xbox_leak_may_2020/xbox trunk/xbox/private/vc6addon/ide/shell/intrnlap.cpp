#include "stdafx.h"

#include "shell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CBuilderWizardManager *CInternalApp::GetBuilderWizardManager()
{
	return &m_builderWizardManager;
}
