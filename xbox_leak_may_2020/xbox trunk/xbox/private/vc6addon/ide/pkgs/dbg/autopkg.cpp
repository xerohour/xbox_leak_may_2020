// AutoPkg.cpp : implementation file for package-related automation code
//

#include "stdafx.h"

#ifdef DEBUGGER_AUTOMATION

#include "utilauto.h"
#include <ObjModel\dbgguid.h>
#include "AutoDbg.h"
#include "privguid.h"
#include <ObjModel\dbgdefs.h>
#include <ObjModel\appauto.h>

#include "autobp.h"
#include "autobps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// GetPackageExtension returns the Debugger object, although it will probably
// be renamed

LPDISPATCH CDebugPackage::GetPackageExtension(LPCTSTR szExtensionName)
{
	if (!_tcsicmp(szExtensionName, "debugger"))
	{
		return GetAutomationObject(TRUE);
	}

	return NULL;
}

IDispatch* CDebugPackage::GetAutomationObject(BOOL bAddRef)
{
	if (pAutoDebugger)
		return pAutoDebugger->GetIDispatch(bAddRef);

	ASSERT(bAddRef);	// The 'new' below is an implicit addref
	CAutoObj *pAutoObj = new CAutoDebugger(this);

	return pAutoObj->GetIDispatch(FALSE); // creation is an implicit AddRef
}

#endif // DEBUGGER_AUTOMATION


