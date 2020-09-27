//
// MERCED (RISC) Compiler Tool
//
// [v-danwh],[matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "mcdtoolc.h"	// our local header file

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCCompilerMercedTool, CCCompilerNTTool)

CCCompilerMercedTool::CCCompilerMercedTool() : CCCompilerNTTool()
{
}
