//
// Conversion of old VC++ 1.x builder files (.mak)
//
// [matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "bldrcvtr.h"	// our local header

IMPLEMENT_DYNAMIC(CBuilderConverter, CBldSysCmp)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Our CBuilderConverter class which is used to convert old VC++ 1.x builder files (.MAK).
// Each product variant should register one of these, e.g. the x86 C++ package and the x86 FORTRAN package.
CBuilderConverter::CBuilderConverter(const TCHAR * szPkg, WORD id)
{
	// set our id.
	SetId(GenerateComponentId(g_prjcompmgr.GenerateBldSysCompPackageId(szPkg), id));
}

CBuilderConverter::~CBuilderConverter()
{
}

// initialise the converter prior to builder conversion
BOOL CBuilderConverter::FInitialise()
{
	return TRUE;	// success
}


