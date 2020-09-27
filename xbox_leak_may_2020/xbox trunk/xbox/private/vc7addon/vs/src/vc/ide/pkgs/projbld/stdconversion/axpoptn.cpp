//
// ALPHA Compiler Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "axpoptn.h"	// our local header file

//----------------------------------------------------------------
// our ALPHA compiler option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(CompilerAlpha, (UINT)NULL, (UINT)NULL, P_CpuAlpha, P_BBThresholdAlpha, TRUE)
	IDOPT_CALLCONV_ALPHA,	"G%{d}1",		OPTARGS1(P_CallConvAlpha),					single,
	IDOPT_BYTE_ALPHA,	"Zp%{2|4|8|16|[1]}1",	OPTARGS1(P_ByteAlignAlpha),					single,
	IDOPT_THREAD_ALPHA,	"M%{L|T|D}1",		OPTARGS1(P_ThreadAlpha),						single,
    IDOPT_GTVALUE_ALPHA, "Gt%1",     OPTARGS1(P_GtvalueAlpha), single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(CompilerAlpha)
	OPTDEF_INT(CpuAlpha, 1)
	OPTDEF_INT(CallConvAlpha, 1)
	OPTDEF_INT(ByteAlignAlpha, 3)
	OPTDEF_INT(ThreadAlpha, 1)
    OPTDEF_INT(GtvalueAlpha, 0)
    OPTDEF_INT(BBThresholdAlpha, 2000)
END_OPTDEF_MAP()

//----------------------------------------------------------------
// our ALPHA linker option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(LinkerAlpha, (UINT)NULL, (UINT)NULL, (UINT)NULL, (UINT)NULL, TRUE)
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerAlpha)
END_OPTDEF_MAP()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCCompilerAlphaTool, CCCompilerNTTool)
IMPLEMENT_DYNAMIC(CLinkerAlphaTool, CLinkerNTTool)

BOOL OLD_OPTION_HANDLER(CompilerAlpha)::AlwaysShowDefault(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_GtvalueAlpha || idPropL == P_BBThresholdAlpha);
}

//////////////////////////////////////////////////////////////////////////////////
// CCompilerAlphaTool
//////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////
// CLinkerAlphaTool
//////////////////////////////////////////////////////////////////////////////////
CLinkerAlphaTool::CLinkerAlphaTool() : CLinkerNTTool()
{
}
