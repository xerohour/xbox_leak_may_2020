//
// Xbox Compiler Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "xbxoptnc.h"	// our local header file

//----------------------------------------------------------------
// our Xbox compiler option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(CompilerXbox, (UINT)NULL, (UINT)NULL, P_CallConv, P_Thread, TRUE)
	IDOPT_CALLCONV,		"G%{d|r|z}1",				OPTARGS1(P_CallConv),					single,
	IDOPT_BYTE,			"Zp%{2|4|8|16|[1]}1",		OPTARGS1(P_ByteAlign),					single,
	IDOPT_THREAD,		"M%{Ld|Td|L|T}1",	OPTARGS1(P_Thread),						single,

// compatibility with VC++ 1.x compiler options, we'll read them
// in then nuke them ;-)
	IDOPT_FLOATPOINT,	"FP%{i87|i|a|c87|c}1",		OPTARGS1(P_FloatPoint),					single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(CompilerXbox)
	OPTDEF_INT(CallConv, 1)
	OPTDEF_INT(ByteAlign, 3)
	OPTDEF_INT(Thread, 4)
	OPTDEF_INT(FloatPoint, -1)
END_OPTDEF_MAP()


DEFINE_LOCALIZED_ENUM(CallConvEnum)
	LOCALIZED_ENUM_ENTRY(IDS_CallConv0 /*"__cdecl *"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_CallConv1 /*"__fastcall"*/, 2)
	LOCALIZED_ENUM_ENTRY(IDS_CallConv2 /*"__stdcall"*/, 3)
END_LOCALIZED_ENUM_LIST()

DEFINE_LOCALIZED_ENUM(ByteAlignEnum)
	LOCALIZED_ENUM_ENTRY(IDS_ByteAlign0 /*"1 Byte"*/, 5)
	LOCALIZED_ENUM_ENTRY(IDS_ByteAlign1 /*"2 Bytes"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_ByteAlign2 /*"4 Bytes"*/, 2)
	LOCALIZED_ENUM_ENTRY(IDS_ByteAlign3 /*"8 Bytes *"*/, 3)
	LOCALIZED_ENUM_ENTRY(IDS_ByteAlign4 /*"16 Bytes"*/, 4)
END_LOCALIZED_ENUM_LIST()

DEFINE_LOCALIZED_ENUM(ThreadEnum)
	LOCALIZED_ENUM_ENTRY(IDS_Thread0 /*"Single-threaded (libcmt.lib)"*/, 3)
	LOCALIZED_ENUM_ENTRY(IDS_Thread0d /*"Debug Single-threaded (libcmt.lib)"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_Thread1 /*"Multithreaded (libcmt.lib)"*/, 4)
	LOCALIZED_ENUM_ENTRY(IDS_Thread1d /*"Debug Multithreaded (libcmtd.lib)"*/, 2)
END_LOCALIZED_ENUM_LIST()
	
IMPLEMENT_DYNCREATE(CCompilerXboxPage, CCompilerPageTab)

CRuntimeClass * g_cplrXboxTabs[] =
{
	RUNTIME_CLASS (CCompilerXboxPage),
	(CRuntimeClass *)NULL,
};


BEGIN_IDE_CONTROL_MAP(CCompilerXboxPage, IDDP_COMPILER_XBOX, IDS_CAT_CODEGEN)
	MAP_COMBO_LIST(IDC_CALLCONV, P_CallConv, CallConvEnum)
	MAP_COMBO_LIST(IDC_BYTE, P_ByteAlign, ByteAlignEnum)
	MAP_COMBO_LIST(IDC_THREAD, P_Thread, ThreadEnum)
END_IDE_CONTROL_MAP()

// compiler tool option default map 'faking'
BOOL OPTION_HANDLER(CompilerXbox)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_Thread);
}


GPT OPTION_HANDLER(CompilerXbox)::GetDefIntProp(UINT idProp, int & nVal)
{
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefIntProp(idProp, nVal);

	FormFakeIntProp(idProp, nVal);
	return valid;
}

void OPTION_HANDLER(CompilerXbox)::FormFakeIntProp(UINT idProp, int & nVal)
{
	UINT idPropL = MapActual(idProp);

	// should not call this if it is not a fake property
	ASSERT (IsFakeProp (idProp)) ;

	BOOL bUseDebugLibs = FALSE;
	((CProjItem *)m_pSlob)->GetIntProp(P_UseDebugLibs, bUseDebugLibs);

	if (bUseDebugLibs)
		{
		nVal = 2;   //MTd
		}
	else
		{
		nVal = 4;   //MT
		}
	return;
}
