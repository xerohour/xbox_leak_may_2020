//
// MERCED Linker Tool
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "mcdtooll.h"	// our local header file

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CLinkerMercedTool, CLinkerNTTool)

CLinkerMercedTool::CLinkerMercedTool() : CLinkerNTTool()
{
}
