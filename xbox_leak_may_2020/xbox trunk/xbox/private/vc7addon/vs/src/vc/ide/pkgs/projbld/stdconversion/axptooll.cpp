//
// ALPHA Linker Tool
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "axptooll.h"	// our local header file

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CLinkerAlphaTool, CLinkerNTTool)

CLinkerAlphaTool::CLinkerAlphaTool() : CLinkerNTTool()
{
}
