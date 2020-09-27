//
// MERCED Compiler Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "mcdoptn.h"	// our local header file

//----------------------------------------------------------------
// our MERCED compiler option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(CompilerMerced, (UINT)NULL, (UINT)NULL, P_CpuMerced, P_BBThresholdMerced, TRUE)
	IDOPT_CALLCONV_MERCED,	"G%{d}1",		OPTARGS1(P_CallConvMerced),					single,
	IDOPT_BYTE_MERCED,	"Zp%{2|4|8|16|[1]}1",	OPTARGS1(P_ByteAlignMerced),					single,
	IDOPT_THREAD_MERCED,	"M%{L|T|D}1",		OPTARGS1(P_ThreadMerced),						single,
    IDOPT_GTVALUE_MERCED, "Gt%1",     OPTARGS1(P_GtvalueMerced), single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(CompilerMerced)
	OPTDEF_INT(CpuMerced, 1)
	OPTDEF_INT(CallConvMerced, 1)
	OPTDEF_INT(ByteAlignMerced, 3)
	OPTDEF_INT(ThreadMerced, 1)
    OPTDEF_INT(GtvalueMerced, 0)
    OPTDEF_INT(BBThresholdMerced, 2000)
END_OPTDEF_MAP()


//----------------------------------------------------------------
// our MERCED linker option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(LinkerMerced, (UINT)NULL, (UINT)NULL, (UINT)NULL, (UINT)NULL, TRUE)
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerMerced)
END_OPTDEF_MAP()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCCompilerMercedTool, CCCompilerNTTool)
IMPLEMENT_DYNAMIC(CLinkerMercedTool, CLinkerNTTool)

BOOL OLD_OPTION_HANDLER(CompilerMerced)::AlwaysShowDefault(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_GtvalueMerced || idPropL == P_BBThresholdMerced);
}

//////////////////////////////////////////////////////////////////////////////////
// CCompilerMercedTool
//////////////////////////////////////////////////////////////////////////////////
CCCompilerMercedTool::CCCompilerMercedTool() : CCCompilerNTTool()
{
}

//////////////////////////////////////////////////////////////////////////////////
// CLinkerMercedTool
//////////////////////////////////////////////////////////////////////////////////

CLinkerMercedTool::CLinkerMercedTool() : CLinkerNTTool()
{
}
