//
// ALPHA (RISC) Compiler Tool
//
// [v-danwh],[matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "axptoolc.h"	// our local header file

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCCompilerAlphaTool, CCCompilerNTTool)

CCCompilerAlphaTool::CCCompilerAlphaTool() : CCCompilerNTTool()
{
}

///////////////////////////////////////////////////////////////////////////////
// Ensure that this project compiler Intelx86 options are ok to use/not use MFC
BOOL CCCompilerAlphaTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	// Set the thread type usage
	pProjItem->SetIntProp(MapLogical(P_ThreadAlpha), 1);

	return CCCompilerNTTool::PerformSettingsWizard(pProjItem, fDebug, iUseMFC);
}
