//
// Common Compiler Tool Options
//
// [matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "optncplr.h"	// our local header file

IMPLEMENT_DYNCREATE(CCompilerPageTab, COptionMiniPage)
IMPLEMENT_DYNCREATE(CCompilerGeneralPage, CCompilerPageTab)
IMPLEMENT_DYNCREATE(CCompilerCustomPage, CCompilerPageTab)
IMPLEMENT_DYNCREATE(CCompilerCustomCppPage, CCompilerPageTab)
IMPLEMENT_DYNCREATE(CCompilerListfilePage, CCompilerPageTab)
IMPLEMENT_DYNCREATE(CCompilerPreprocessPage, CCompilerPageTab)
IMPLEMENT_DYNCREATE(CCompilerDebugOptPage, CCompilerPageTab)
IMPLEMENT_DYNCREATE(CCompilerPchPage, CCompilerPageTab)
IMPLEMENT_DYNCREATE(CCompilerOptimizePage, CCompilerPageTab)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//----------------------------------------------------------------
// our COMMON compiler option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(CompilerCommon, P_CplrUnknownOption, P_CplrUnknownString, P_CplrNoLogo, P_Enable_dlp, TRUE)
	IDOPT_CPLRNOLOGO,		"nologo%T1",		OPTARGS1(P_CplrNoLogo),					single,
	IDOPT_DERIVED_OPTHDLR,	"",					NO_OPTARGS,								single,
	IDOPT_MSEXT,		"Z%{e|a}1",				OPTARGS1(P_MSExtension),				single,
	IDOPT_TURN_OFF_WARNINGS,	"w%<1>1",		OPTARGS1(P_WarningLevel),				single,
	IDOPT_WARN,			"W%{0|1|2|3|4}1",		OPTARGS1(P_WarningLevel), 				single,

	IDOPT_WARNASERROR,	"WX%T1",				OPTARGS1(P_WarnAsError),				single,
	IDOPT_ENABLE_MR,	"Gm%{-|}1",				OPTARGS1(P_Enable_MR),					single,
	IDOPT_ENABLE_INCR,	"Gi%{-|}1",				OPTARGS1(P_Enable_Incr),				single,
	IDOPT_PTRMBR1,		"vm%{b|g}1",			OPTARGS1(P_PtrMbr1),					single,
	IDOPT_PTRMBR2,		"vm%{v|s|m}1",			OPTARGS1(P_PtrMbr2),					single,
	IDOPT_VTORDISP,		"vd%{1|0}1",			OPTARGS1(P_VtorDisp),					single,
	IDOPT_ENABLE_RTTI,	"GR%{-|}1",				OPTARGS1(P_Enable_RTTI),				single,
/* temporarily reverse the logic for PDC (BamBam) 'cos the compiler default will be /GX- for this
** release .. ie. we need [X] Enable EH in the options dialog
	IDOPT_DISABLE_EH,	"GX%{|-}1",				OPTARGS1(P_Disable_EH),					single,
*/
	IDOPT_ENABLE_EH,	"GX%{-|}1",				OPTARGS1(P_Enable_EH),					single,
	IDOPT_DEBUGINFO,	"Z%{d|7|i|I}1",			OPTARGS1(P_DebugInfo),					single,
	IDOPT_OPTIMIZE,		"O%{d|2|1}1",			OPTARGS1(P_Optimize),					single,
	IDOPT_OPT_X,		"Ox%T1%<4>2",			OPTARGS2(P_Optimize_X, P_Optimize),		single,
	IDOPT_OPT_T,		"Ot%T1%<4>2",			OPTARGS2(P_Optimize_T, P_Optimize),		single,
	IDOPT_OPT_A,		"Oa%T1%<4>2",			OPTARGS2(P_Optimize_A, P_Optimize),		single,
	IDOPT_OPT_W,		"Ow%T1%<4>2",			OPTARGS2(P_Optimize_W, P_Optimize),		single,
	IDOPT_OPT_G,		"Og%T1%<4>2",			OPTARGS2(P_Optimize_G, P_Optimize),		single,
	IDOPT_OPT_I,		"Oi%T1%<4>2",			OPTARGS2(P_Optimize_I, P_Optimize),		single,
	IDOPT_OPT_S,		"Os%T1%<4>2",			OPTARGS2(P_Optimize_S, P_Optimize),		single,
	IDOPT_OPT_P,		"Op%{-|}1",				OPTARGS1(P_Optimize_P),					single,
	IDOPT_OPT_Y,		"Oy%{-|}1",				OPTARGS1(P_Optimize_Y),					single,
	IDOPT_INLINECTRL,	"Ob%{0|1|2}1",			OPTARGS1(P_InlineControl),				single,
	IDOPT_STRPOOL,		"Gf%T1",				OPTARGS1(P_StringPool),					single,
	IDOPT_INCLINK,		"Gy%T1",				OPTARGS1(P_IncLinking),					single,
	IDOPT_CPLRIGNINC,	"X%T1",					OPTARGS1(P_CplrIgnIncPath),				single,
	IDOPT_CPLRINCLUDES,	"I[ ]%1",				OPTARGS1(P_CplrIncludes),				multiple,
	IDOPT_FORCEINCLUDES,	"FI%1",				OPTARGS1(P_ForceIncludes),				multiple,
	IDOPT_UNDEFINE,		"u%T1",					OPTARGS1(P_Undefine),					single,
	IDOPT_MACROS,		"D[ ]%1",				OPTARGS1(P_MacroNames),					multiple,

// FUTURE (matthewt): how to handle this in the property page?
//	IDOPT_MACROS,		"D[ ]%1[=%2]",			OPTARGS2(P_MacroNames, P_MacroValues),	multiple,

	IDOPT_UNDEFMACROS,	"U[ ]%1",				OPTARGS1(P_UndefMacros),				multiple,
	IDOPT_LISTASM,		"FA%{%f1%f2|cs%t1%t2|c%t1%f2|s%f1%t2}3",	OPTARGS3(P_AsmListHasMC, P_AsmListHasSrc, P_ListAsm),						single,
	IDOPT_OUTDIR_COD,	"Fa%1",					OPTARGS1(P_OutputDir_Cod),				single,
	IDOPT_LISTBSCNOLCL,	"Fr%T1%t2[%3]",			OPTARGS3(P_GenBrowserInfo, P_GenBrowserInfoNoLcl, P_OutputDir_Sbr),	single,
	IDOPT_LISTBSC,		"FR%T1%f2[%3]",			OPTARGS3(P_GenBrowserInfo, P_GenBrowserInfoNoLcl, P_OutputDir_Sbr),	single,
	IDOPT_PCHNAME,		"Fp%1",					OPTARGS1(P_OutputDir_Pch),				single,
	IDOPT_AUTOPCH,		"YX%T1[%2]",			OPTARGS2(P_AutoPch, P_AutoPchUpTo),				single,
	IDOPT_PCHCREATE,	"Yc%T1[%2]",			OPTARGS2(P_PchCreate, P_PchCreateUptoHeader),	single,
	IDOPT_PCHUSE,		"Yu%T1[%2]",			OPTARGS2(P_PchUse, P_PchUseUptoHeader),			single,
	IDOPT_OUTDIR_OBJ,	"Fo%1",					OPTARGS1(P_OutputDir_Obj),				single,
	IDOPT_OUTDIR_PDB,	"Fd%1",					OPTARGS1(P_OutputDir_Pdb),				single,
	IDOPT_STACK_PROBE,	"Ge%T1",				OPTARGS1(P_StackProbe),					single,
	IDOPT_STACK_PROBE_THOLD,	"Gs%1",			OPTARGS1(P_StackProbeTHold),			single,
	IDOPT_MAXLEN_EXTNAME,		"H%1",			OPTARGS1(P_MaxLenExtNames),				single,
	IDOPT_USECHAR_AS_UNSIGNED,	"J%T1",			OPTARGS1(P_UseChasAsUnsigned),			single,
	IDOPT_EMBED_STRING,			"V%1",			OPTARGS1(P_EmbedString),				single,
	IDOPT_NO_DEFLIB_NAME,		"Zl%T1",		OPTARGS1(P_NoDefLibNameInObj),			single,
	IDOPT_PREP_COMMENTS,		"C%T1",			OPTARGS1(P_PrepPreserveComments),		single,
	IDOPT_PREPROCESS,			"E%T1",			OPTARGS1(P_Preprocess),					single,
	IDOPT_PREPROCESS_WO_LINES,	"EP%T1",		OPTARGS1(P_PreprocessWOLines),			single,
	IDOPT_ENABLE_FD,	"FD%{-|}1",				OPTARGS1(P_Enable_FD),					single,
	IDOPT_ENABLE_FC,	        "FC%T1",		        OPTARGS1(P_Enable_FC),			single,
	IDOPT_STACKSIZE,			"F%1",			OPTARGS1(P_StackSize),					single,
	IDOPT_ENABLE_DLP,			"dlp%{-|}1",	OPTARGS1(P_Enable_dlp),					single,
	IDOPT_PREPROCESS_TO_FILE,	"P%T1",			OPTARGS1(P_PrepprocessToFile),			single,
	IDOPT_GEN_FUNC_PROTO,		"Zg%T1",		OPTARGS1(P_GenFuncProto),				single,
	IDOPT_CHECK_SYNTAX,			"Zs%T1",		OPTARGS1(P_CheckSyntax),				single,
	IDOPT_UNKNOWN_OPTION,	"",					NO_OPTARGS,								single,
	IDOPT_UNKNOWN_STRING,	"",					NO_OPTARGS,								single,
	IDOPT_COMPILE_ONLY,	"c%T1",					OPTARGS1(P_CompileOnly),				single,
	IDOPT_TREATFILEAS_C,	"Tc%T1",			OPTARGS1(P_TreatFileAsC),				single,
	IDOPT_TREATFILEAS_CPP,	"Tp%T1",			OPTARGS1(P_TreatFileAsCPP),				single,	
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(CompilerCommon)
	OPTDEF_BOOL(CplrNoLogo, FALSE)
	OPTDEF_BOOL(MSExtension, FALSE)
	OPTDEF_BOOL(IncLinking, FALSE)
	OPTDEF_INT(WarningLevel, 2)
	OPTDEF_BOOL(WarnAsError, FALSE)
	OPTDEF_BOOL(StringPool, FALSE)
	OPTDEF_INT(PtrMbr1, 1)
	OPTDEF_INT(PtrMbr2, 1)
	OPTDEF_BOOL(VtorDisp, FALSE)
/* temporarily reverse the logic for PDC (BamBam) 'cos the compiler default will be /GX- for this
** release .. ie. we need [X] Enable EH in the options dialog
	OPTDEF_BOOL(Disable_EH, FALSE)
*/
	OPTDEF_BOOL(Enable_EH, FALSE)
	OPTDEF_BOOL(Enable_MR, FALSE)
	OPTDEF_BOOL(Enable_RTTI, FALSE)
	OPTDEF_BOOL(Enable_Incr, FALSE)
	OPTDEF_INT(DebugInfo, 0)
	OPTDEF_BOOL(AutoPch, FALSE)
	OPTDEF_PATH(AutoPchUpTo, "")
	OPTDEF_INT(Optimize, 0)
	OPTDEF_BOOL(Optimize_A, FALSE)
	OPTDEF_BOOL(Optimize_W, FALSE)
	OPTDEF_BOOL(Optimize_G, FALSE)
	OPTDEF_BOOL(Optimize_I, FALSE)
	OPTDEF_BOOL(Optimize_P, FALSE)
	OPTDEF_BOOL(Optimize_T, FALSE)
	OPTDEF_BOOL(Optimize_S, FALSE)
	OPTDEF_BOOL(Optimize_Y, FALSE)
	OPTDEF_BOOL(Optimize_X, FALSE)
	OPTDEF_INT(InlineControl, 1)
	OPTDEF_BOOL(Undefine, FALSE)
	OPTDEF_INT(ListAsm, 0)
	OPTDEF_BOOL(AsmListHasMC, FALSE)
	OPTDEF_BOOL(AsmListHasSrc, FALSE)
	OPTDEF_BOOL(GenBrowserInfoNoLcl, FALSE)
	OPTDEF_BOOL(GenBrowserInfo, FALSE)
	OPTDEF_BOOL(CplrIgnIncPath, FALSE)
	OPTDEF_LIST(MacroNames, "")
	OPTDEF_LIST(MacroValues, "")
	OPTDEF_LIST(UndefMacros, "")
	OPTDEF_LIST(CplrIncludes, "")
	OPTDEF_LIST(ForceIncludes, "")
	OPTDEF_PATH(OutputDir_Obj, "")
	OPTDEF_PATH(OutputDir_Pch, "")
	OPTDEF_PATH(OutputDir_Pdb, "")
	OPTDEF_PATH(OutputDir_Sbr, "")
	OPTDEF_PATH(OutputDir_Cod, "")
	OPTDEF_BOOL(PchCreate, FALSE)
	OPTDEF_PATH(PchCreateUptoHeader, "")
	OPTDEF_BOOL(PchUse, FALSE)
	OPTDEF_PATH(PchUseUptoHeader, "")
	OPTDEF_BOOL(TreatFileAsC, FALSE)
	OPTDEF_BOOL(TreatFileAsCPP, FALSE)
	OPTDEF_BOOL(CompileOnly, FALSE)
	// The following two props are not actually compiler flags, nor are they
	// accessible via the options dialogs.  They are present in the default
	// option table purely so that assumptions about these names are in one
	// place only.
	OPTDEF_PATH(PchDefaultName, "msvc.pch")
	OPTDEF_PATH(PdbDefaultName, "msvc.pdb")
	OPTDEF_BOOL(StackProbe, FALSE)
	OPTDEF_INT(StackProbeTHold, 4096)
	OPTDEF_INT(MaxLenExtNames, -1)
	OPTDEF_BOOL(UseChasAsUnsigned, FALSE)
	OPTDEF_STRING(EmbedString, "")
	OPTDEF_BOOL(NoDefLibNameInObj, FALSE)
	OPTDEF_BOOL(PrepPreserveComments, FALSE)
	OPTDEF_BOOL(Preprocess, FALSE)
	OPTDEF_BOOL(PreprocessWOLines, FALSE)
	OPTDEF_HEX(StackSize, -1)
	OPTDEF_BOOL(PrepprocessToFile, FALSE)
	OPTDEF_BOOL(GenFuncProto, FALSE)
	OPTDEF_BOOL(CheckSyntax, FALSE)
	OPTDEF_INT(PseudoPch, 1)
	OPTDEF_BOOL(Enable_FD, FALSE)
	OPTDEF_BOOL(Enable_FC, FALSE)
	// this option is an internal flag that is incompatible with /ZI
	OPTDEF_BOOL(Enable_dlp, FALSE)
END_OPTDEF_MAP()

BOOL OPTION_HANDLER(CompilerCommon)::AlwaysShowDefault(UINT idProp)
{
	return FALSE;
}

// COMMON compiler tool option default map 'faking'

BOOL OPTION_HANDLER(CompilerCommon)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (
			idPropL == P_OutputDir_Obj ||
			idPropL == P_OutputDir_Pch ||
			idPropL == P_OutputDir_Sbr ||
			idPropL == P_OutputDir_Pdb ||
			idPropL == P_OutputDir_Cod ||
			idPropL == P_PchDefaultName ||
			idPropL == P_PseudoPch
		   );
}

BOOL OPTION_HANDLER(CompilerCommon)::SetsFakePropValue(UINT idProp)
{
	ASSERT(IsFakeProp(idProp));

	UINT idPropL = MapActual(idProp);
	switch (idPropL)
	{
	case P_PchDefaultName:
	case P_OutputDir_Pch:
		return FALSE;		// never defined at file level (no multiple .pch support)

	case P_OutputDir_Obj:
	case P_OutputDir_Sbr:
	case P_OutputDir_Pdb:
	case P_OutputDir_Cod:
		// these are solely dependent on the output/target directory (which is checked
		// separately).
		return FALSE;
	}

	return TRUE;	// we don't know anything about this property
}

UINT OPTION_HANDLER(CompilerCommon)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	// our .pdb/.icc files are always in the target directory
	if (idPropL == P_OutputDir_Pdb)
		return P_OutDirs_Intermediate;

	// our .PCH files are always in the project-level intermediate directory
	// we'll make sure we use this level in ::FormFakeStrProp()
	else if (idPropL == P_OutputDir_Pch)
		return P_OutDirs_Intermediate;

	// all other files use the intermediate directory
	else if (idPropL == P_OutputDir_Obj || idPropL == P_OutputDir_Sbr || idPropL == P_OutputDir_Cod)
		return P_OutDirs_Intermediate;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

void OPTION_HANDLER(CompilerCommon)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	CPath pathFp, path;
	CDir dirFp;
	CString strDefName;

	// ASSERT this is a compiler 'fake' string prop?
	ASSERT(IsFakeProp(idProp));

	// handle our default props
	if (idPropL == P_PchDefaultName)
	{
		// do we have /Fp, if so then just return this
		if (m_pSlob->GetStrProp(MapLogical(P_OutputDir_Pch), strVal) == invalid)
			strVal = "";	// dep. check caused invalid, default ""

		// is this empty or a directory?
		if (!strVal.IsEmpty())
		{
			const TCHAR * pchFp = (const TCHAR *)strVal + strVal.GetLength();
			pchFp = _tcsdec((const TCHAR *)strVal, (TCHAR *)pchFp);

			if (*pchFp == _T('/') || *pchFp == _T('\\'))
			{
				if (COptionHandler::GetDefStrProp(idProp, strDefName) == invalid)
					strDefName = "";	// dep. caused invalid, default ""

				strVal += strDefName;
			}
		}

		return;	// we're done
	}

	// which output directory do we want to use?
	UINT idOutDirProp = GetFakePathDirProp(idProp);
	ASSERT(idOutDirProp != (UINT)-1);

	// the rest are output directory related
	// find one somewhere in the hierarchy

	// make sure the project is in our config.
	CProjItem * pItem = (CProjItem *)m_pSlob;
	CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord);

	// .pch uses project-level intermediate directory...
	// we can't use file-level because we don't have
	// support for multiple .pchs
	if (idPropL == P_OutputDir_Pch)
		pItem = pItem->GetProject();

	GPT gpt = pItem->GetStrProp(idOutDirProp, strVal);
	while (gpt != valid)
	{
		pItem = (CProjItem *)pItem->GetContainer();
		ASSERT(pItem != (CSlob *)NULL);
		gpt = pItem->GetStrProp(idOutDirProp, strVal);
	}

	// reset the project config.
	projTempConfigChange.Release();

	// Make sure this ends in a slash.
	if (!strVal.IsEmpty())
	{
		TCHAR * pchStart = strVal.GetBuffer(1);
		TCHAR * pchT;

		pchT = pchStart + _tcslen(pchStart);	// point to nul terminator
		pchT = _tcsdec(pchStart, pchT);			// back up one char, DBCS safe

		if (*pchT != _T('\\') && *pchT != _T('/'))
		{
			strVal.ReleaseBuffer();
			strVal += _T('/');
		}
	}

	// if we are /Fp (.pch) or /Fd (.pdb/.icc) then tag on the basename of the project
	if (idPropL == P_OutputDir_Pch)
	{
		ASSERT(g_pActiveProject);
		CString strBase; 

        CProjItem * pItem = ((CProjItem *)m_pSlob);
		CDir dirWorkspace = pItem->GetProject()->GetWorkspaceDir();
		CDir dirProject = pItem->GetProject()->GetProjDir(pItem->GetActiveConfig());
		CString strWorkspace = (const TCHAR *)dirWorkspace;
		CString strProject = (const TCHAR *)dirProject;

		if (strWorkspace.CompareNoCase(strProject) == 0)
		{
			const CPath * ppathProj = pItem->GetProject()->GetFilePath();
			ppathProj->GetBaseNameString(strBase);
		}
		else
		{
			// subproject
			ASSERT( (strProject.GetLength() > strWorkspace.GetLength()) );
			strBase = strProject.Right(strProject.GetLength()-strWorkspace.GetLength()-1);
		}

		strVal += strBase;
		strVal += _TEXT(".pch");
	}
}

GPT OPTION_HANDLER(CompilerCommon)::GetDefStrProp(UINT idProp, CString & val)
{
	UINT idPropL = MapActual(idProp);
	BOOL fIgnoreFake = FALSE;

	// we don't have OutputDir_Pdb with no /Zi or /Gi or /FD
	if (idPropL == P_OutputDir_Pdb)
	{
		int nVal;
		// N.B. 3 == /Zi
		if ((m_pSlob->GetIntProp(MapLogical(P_DebugInfo), nVal) == invalid || nVal < DebugInfoPdb) &&
			(m_pSlob->GetIntProp(MapLogical(P_Enable_Incr), nVal) == invalid || !nVal) &&
			(m_pSlob->GetIntProp(MapLogical(P_Enable_FD), nVal) == invalid || !nVal))
		{
			fIgnoreFake = TRUE;	// not a faked prop in this context
		}
	}

	// just return the intermediate output directory for these 'fakes'
	if (IsFakeProp(idProp))
	{
		FormFakeStrProp(idProp, val);
		return valid;
	}

	return COptionHandler::GetDefStrProp(idProp, val);
}

void OPTION_HANDLER(CompilerCommon)::FormFakeIntProp(UINT idProp, int & nVal)
{
}

GPT OPTION_HANDLER(CompilerCommon)::GetDefIntProp(UINT idProp, int & val)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_Optimize_Y)
	{
		// default is /Oy with /Ox or /O2, and /Oy- otherwise
		int nVal;
		val = (m_pSlob->GetIntProp(MapLogical(P_Optimize_X), nVal) == valid && nVal) ||
			  (m_pSlob->GetIntProp(MapLogical(P_Optimize), nVal) == valid && (nVal == 2));
		return valid;
	}
	else if (idPropL == P_Optimize_P)
	{
		// default is /Op with /Za, and /Op- otherwise
		int nVal;
		val = (m_pSlob->GetIntProp(MapLogical(P_MSExtension), nVal) == valid && nVal);
		return valid;
	}
	else if (idPropL == P_InlineControl)
	{
		// default is /Ob1 with /O2, /O1 and /Ox
		int nVal;
		val = (m_pSlob->GetIntProp(MapLogical(P_Optimize_X), nVal) == valid && nVal) ||
			  (m_pSlob->GetIntProp(MapLogical(P_Optimize), nVal) == valid && (nVal == 2 || nVal == 3)) ? 2 : 1;
		return valid;
	}
	else if (idPropL == P_PseudoPch)
	{
		int nVal;
		if (m_pSlob->GetIntProp(MapLogical(P_PchCreate), nVal) == valid && nVal)
			val = 3;
		else if (m_pSlob->GetIntProp(MapLogical(P_PchUse), nVal) == valid && nVal)
			val = 4;
		else if (m_pSlob->GetIntProp(MapLogical(P_AutoPch), nVal) == valid && nVal)
			val = 2;
		else
			val = 1;

		return valid;
	}
	else if (idPropL == P_StringPool || idPropL == P_IncLinking)
	{
		// default is /Gy, /Gf for /O2 and /O1
		int nVal;
		val = (m_pSlob->GetIntProp(MapLogical(P_Optimize), nVal) == valid && (nVal == 2 || nVal == 3));
		return valid;
	}

	return COptionHandler::GetDefIntProp(idProp, val);
}

void OPTION_HANDLER(CompilerCommon)::OnOptionIntPropChange(UINT idProp, int nVal)
{
	ASSERT(m_pSlob != (CSlob *)NULL);

	UINT idPropL = MapActual(idProp);
	int nCurrVal;

	// do we need special processing?
	switch (idPropL)
	{
		case P_ListAsm:
			// handle .asm file combinations
			m_pSlob->SetIntProp(MapLogical(P_AsmListHasMC), nVal == 2 || nVal == 3);
			m_pSlob->SetIntProp(MapLogical(P_AsmListHasSrc), nVal == 2 || nVal == 4);
			break;

		case P_PseudoPch:
			// ignore if the current property bag
			if (((CProjItem *)m_pSlob)->UsePropertyBag() != CurrBag)
			{
				// handle .pch weirdness with our radio group UI (P_PseudoPch)
				if (m_pSlob->GetIntProp(MapLogical(P_AutoPch), nCurrVal) != valid || (nCurrVal != (nVal == 2)))
					m_pSlob->SetIntProp(MapLogical(P_AutoPch), nVal == 2);

				if (m_pSlob->GetIntProp(MapLogical(P_PchCreate), nCurrVal) != valid || (nCurrVal != (nVal == 3)))
					m_pSlob->SetIntProp(MapLogical(P_PchCreate), nVal == 3);
				 
				if (m_pSlob->GetIntProp(MapLogical(P_PchUse), nCurrVal) != valid || (nCurrVal != (nVal == 4)))
					m_pSlob->SetIntProp(MapLogical(P_PchUse), nVal == 4);
			}

			// remove the pseudo-prop from this bag!
			m_pSlob->GetPropBag()->RemoveProp(idProp);
			break;

		default:
			break;
	}

	// DOLPHIN bug fix #12677
	// FUTURE (matthewt): fix this properly by writing individual props. into project file
	// make sure that properties that 'rely' on other
	// prop. values to be shown as an option have the 'other'
	// props. set (no need to do for non-file items)
	if (m_pSlob->GetContainer() != (CSlob *)NULL)
	{
		switch (idPropL)
		{
			case P_GenBrowserInfoNoLcl:
				// make sure we should be able to do this
				if (m_pSlob->GetIntProp(MapLogical(P_GenBrowserInfo), nCurrVal) == valid && nCurrVal)
					m_pSlob->SetIntProp(MapLogical(P_GenBrowserInfo), TRUE);
				break;

			case P_Optimize_X:
			case P_Optimize_T:
			case P_Optimize_A:
			case P_Optimize_W:
			case P_Optimize_G:
			case P_Optimize_I:
			case P_Optimize_S: 
				// make sure we should be able to do this
				if (m_pSlob->GetIntProp(MapLogical(P_Optimize), nCurrVal) == valid && (nCurrVal == 4))
					m_pSlob->SetIntProp(MapLogical(P_Optimize), 4);
				break;

			default:
				break;
		}
	}
}

void OPTION_HANDLER(CompilerCommon)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// DOLPHIN bug fix #12677
	// FUTURE (matthewt): fix this properly by writing individual props. into project file
	// make sure that properties that 'rely' on other
	// prop. values to be shown as an option have the 'other'
	// props. set (no need to do for non-file items)
	if (m_pSlob->GetContainer() != (CSlob *)NULL)
	{
		switch (idPropL)
		{
			case P_AutoPchUpTo:
				m_pSlob->SetIntProp(MapLogical(P_AutoPch), TRUE);
				break;
 
			case P_PchCreateUptoHeader:
				m_pSlob->SetIntProp(MapLogical(P_PchCreate), TRUE);
				break;

			case P_PchUseUptoHeader:
				m_pSlob->SetIntProp(MapLogical(P_PchUse), TRUE);
				break;

			case P_OutputDir_Sbr:
				m_pSlob->SetIntProp(MapLogical(P_GenBrowserInfo), TRUE);
				break;

			default:
				break;
		}
	}
}

BOOL OPTION_HANDLER(CompilerCommon)::CheckDepOK(UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fValid = TRUE;			// valid by default

	// placeholders for values
	int nVal;	

	// which prop are we checking the dep. for?
	switch (idPropL)
	{
		// Incr. compilation and RTTI not available on all platforms
		case P_Enable_RTTI:
		{
			CPlatform * pPlatform = ((CProjItem *)m_pSlob)->GetProject()->GetCurrentPlatform();
			fValid = (pPlatform->GetAttributes() & PIA_Supports_RTTI) != 0;
			break;
		}

		case P_Enable_Incr:
		{
			CPlatform * pPlatform = ((CProjItem *)m_pSlob)->GetProject()->GetCurrentPlatform();
			fValid = (pPlatform->GetAttributes() & PIA_Supports_IncCplr) != 0;
			break;
		}

		case P_Enable_MR:
		{
			CPlatform * pPlatform = ((CProjItem *)m_pSlob)->GetProject()->GetCurrentPlatform();
			fValid = (pPlatform->GetAttributes() & PIA_Supports_MinBuild) != 0;

			// only valid for /Zi
			if (fValid)
			{
				(void) m_pSlob->GetIntProp(MapLogical(P_DebugInfo), nVal);
				fValid = (nVal >= DebugInfoPdb);
			}
			break;
		}

		// General-Purpose combo only enabled if PtrMbr1 is
		// 'general' representation
		case P_PtrMbr2:
			(void)m_pSlob->GetIntProp(MapLogical(P_PtrMbr1), nVal);
			fValid = (nVal == 2);	
			break;

		// Can't have these without 'Gen Browser Info' checked
		case P_GenBrowserInfoNoLcl:
		case P_OutputDir_Sbr:
			(void)m_pSlob->GetIntProp(MapLogical(P_GenBrowserInfo), nVal);
			fValid = !!nVal;
			break;

		// Can't name the listfile without setting listfile options
		case P_OutputDir_Cod:
			(void)m_pSlob->GetIntProp(MapLogical(P_ListAsm), nVal);
			fValid = !!nVal;
			break;

		// Can't have any custom optimizations without 'CustomOpt' on
		case P_Optimize_A:
		case P_Optimize_W:
		case P_Optimize_G:
		case P_Optimize_I:
		case P_Optimize_T:
		case P_Optimize_S:
		case P_Optimize_X:
			(void)m_pSlob->GetIntProp(MapLogical(P_Optimize), nVal);
			fValid = (nVal == 4);
			break;

		// These are both enabled given /O2, there is no way to turn-off
		// ie. no /Gf- or /Gy-, on the command-line
		case P_IncLinking:
		case P_StringPool:
			(void)m_pSlob->GetIntProp(MapLogical(P_Optimize), nVal);
			fValid = !(nVal == 2);
			if (fValid) {
				// ZI forces /Gy and /Gf 
				// Let's disable and use helpid to inform user accordingly
				(void) m_pSlob->GetIntProp(MapLogical(P_DebugInfo), nVal);
				fValid = (nVal != DebugInfoENC);
			}
			break;

		// Precompiled headers mondo enable/disable
		case P_OutputDir_Pch:
		{
			// need /YX, /Yc or /Yu
			(void) m_pSlob->GetIntProp(MapLogical(P_AutoPch), nVal);
			if (!nVal)
			{
				(void) m_pSlob->GetIntProp(MapLogical(P_PchCreate), nVal);
				if (!nVal)
				{
					(void) m_pSlob->GetIntProp(MapLogical(P_PchUse), nVal);
					fValid = nVal;
					break;
				}
			}

			fValid = TRUE;
			break;
		}

		// Don't try to rename a .pdb/.icc directory if we are not generating one
		case P_OutputDir_Pdb:
		{
			// do we have /Zi or /Gi or /FD?
			(void) m_pSlob->GetIntProp(MapLogical(P_DebugInfo), nVal);
			fValid = (nVal >= DebugInfoPdb);
			if (!fValid)
			{
				(void) m_pSlob->GetIntProp(MapLogical(P_Enable_Incr), nVal);
				fValid = !!nVal;
				if (!fValid)
				{
					(void) m_pSlob->GetIntProp(MapLogical(P_Enable_FD), nVal);
					fValid = !!nVal;
				}
			}
			break;
		}

		case P_PchCreateUptoHeader:
		{
			UINT idProp = MapLogical(P_PchCreate);
			if (m_pSlob->GetIntProp(idProp, nVal) != valid)
				nVal = FALSE;
			fValid = nVal && CheckDepOK(idProp);
			break;
		}

		case P_PchUseUptoHeader:
		{
			UINT idProp = MapLogical(P_PchUse);
			if (m_pSlob->GetIntProp(idProp, nVal) != valid)
				nVal = FALSE;
			fValid = nVal && CheckDepOK(idProp);
			break;
		}

		case P_AutoPchUpTo:
		{
			UINT idProp = MapLogical(P_AutoPch);
			if (m_pSlob->GetIntProp(idProp, nVal) != valid)
				nVal = FALSE;
			fValid = nVal && CheckDepOK(idProp);
			break;
		}

		case P_PchCreate:
		case P_PchUse:
		case P_AutoPch:
		{
			UINT idPropC = MapLogical(P_PchCreate);
			UINT idPropU = MapLogical(P_PchUse);
			UINT idPropX = MapLogical(P_AutoPch);

			int nPriCreate = 0, nPriUse = 0, nPriAuto = 0;
			int nVal;

			// figure the precedence of these
			// using a value calc'd below
			// /Yc overrides /Yu overrides /YX

			// set initial priorities (use defaults)
			if (m_pSlob->GetIntProp(idPropC, nVal) == valid && nVal)
				nPriCreate = 3;	// /Yc >> /Yu >> /YX

			else if (m_pSlob->GetIntProp(idPropU, nVal) == valid && nVal)
				nPriUse = 2;	// /Yu >> /YX

			else if (m_pSlob->GetIntProp(idPropX, nVal) == valid && nVal)
				nPriAuto = 1;	// /YX >> (nothing)

			// though per-file /Y? overrides project-level /Y?
			if (m_pSlob->GetContainer() != (CSlob *)NULL)
			{
				OptBehaviour optbeh = ((CProjItem *)m_pSlob)->GetOptBehaviour();

				// turn-off inherit for per-file
				((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh & ~OBInherit);

				// get priorities
				if (m_pSlob->GetIntProp(idPropC, nVal) == valid && nVal)
					nPriCreate += 4;

				else if (m_pSlob->GetIntProp(idPropU, nVal) == valid && nVal)
					nPriUse += 4; 

				else if (m_pSlob->GetIntProp(idPropX, nVal) == valid && nVal)
					nPriAuto += 4; 

				// reset option behaviour
				((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh);
			}

			int nOur, nOther1, nOther2;
			if (idPropL == P_PchCreate)
			{
				nOur = nPriCreate;
				nOther1 = nPriUse; nOther2 = nPriAuto;
			}
			else if (idPropL == P_PchUse)
			{
				nOur = nPriUse;
				nOther1 = nPriCreate; nOther2 = nPriAuto;
			}
			else
			{
				nOur = nPriAuto;
				nOther1 = nPriUse; nOther2 = nPriCreate;
			}

			// valid only if our priority is greater or equal to the others
			fValid = nOur >= nOther1 && nOur >= nOther2;
			break;
		}

		default:
			break;
	}

	return fValid;
}

//----------------------------------------------------------------
// our compiler tool option property pages
//----------------------------------------------------------------

CRuntimeClass * g_cplrTabs[] =
{
	RUNTIME_CLASS(CCompilerGeneralPage),
	RUNTIME_CLASS(CCompilerPchPage),
	RUNTIME_CLASS(CCompilerOptimizePage),
	RUNTIME_CLASS(CCompilerCustomPage),
	RUNTIME_CLASS(CCompilerCustomCppPage),
	RUNTIME_CLASS(CCompilerPreprocessPage),
	RUNTIME_CLASS(CCompilerListfilePage),
	(CRuntimeClass *)NULL,
};

// our listbox entries

DEFINE_LOCALIZED_ENUM(WarnLevelEnum)
	LOCALIZED_ENUM_ENTRY(IDS_WarnLevel0 /*"None"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_WarnLevel1 /*"Level 1 *"*/, 2)
	LOCALIZED_ENUM_ENTRY(IDS_WarnLevel2 /*"Level 2"*/, 3)
	LOCALIZED_ENUM_ENTRY(IDS_WarnLevel3 /*"Level 3"*/, 4)
	LOCALIZED_ENUM_ENTRY(IDS_WarnLevel4 /*"Level 4"*/, 5)
END_LOCALIZED_ENUM_LIST()

DEFINE_LOCALIZED_ENUM(OptimizeEnum)
	LOCALIZED_ENUM_ENTRY(IDS_Optimize0 /*"Default"*/, 0)
	LOCALIZED_ENUM_ENTRY(IDS_Optimize1 /*"Disable (Debug)"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_Optimize2 /*"Maximize Speed"*/, 2)
	LOCALIZED_ENUM_ENTRY(IDS_Optimize3 /*"Minimize Size"*/, 3)
	LOCALIZED_ENUM_ENTRY(IDS_Optimize4 /*"Customize"*/, 4)
END_LOCALIZED_ENUM_LIST()

class CDebugInfoEnum : public CLocalizedEnum
{
public:
	CDebugInfoEnum() {dwAllowDbgFilter = 
		DbgNone | DbgLine | DbgC7 | DbgPdb | DbgENC;}

	virtual CLocalizedEnumerator* GetListL() {return &c_list[0];}
	virtual POSITION GetHeadPosition()
	{
		POSITION pos = CLocalizedEnum::GetHeadPosition(), posT;
		CLocalizedEnumerator * pEnum;																			  	
		posT = pos;
		while (!AllowEnum(posT, pEnum)) {pos = posT;}
		return pos;
	}

	virtual CLocalizedEnumerator* GetNextL(POSITION& pos)
	{
		CLocalizedEnumerator * pEnum, * pEnumT;
		pEnum = CLocalizedEnum::GetNextL(pos);

		POSITION posT = pos;
		// skip ones we don't recognise
 		while (!AllowEnum(posT, pEnumT)) {pos = posT;}
		return pEnum;
	}

	typedef enum {
		DbgNone	= 0x00000001, 
		DbgLine	= 0x00000002, 
		DbgC7	= 0x00000004,
		DbgPdb	= 0x00000008,
		DbgENC	= 0x00000010
	} AllowInc;

	BOOL AllowEnum(POSITION & pos, CLocalizedEnumerator * & pEnum);
	__inline void AllowDbg(DWORD dwAllow) {dwAllowDbgFilter = dwAllow;}

	static CLocalizedEnumerator c_list[];

private:
	DWORD dwAllowDbgFilter;
};

BOOL CDebugInfoEnum::AllowEnum(POSITION & pos, CLocalizedEnumerator * & pEnum)
{
	if (pos == (POSITION)NULL)	return TRUE;
	pEnum = CLocalizedEnum::GetNextL(pos);
	return ((pEnum->val == DebugInfoNone && (dwAllowDbgFilter & DbgNone) != 0) ||
			(pEnum->val == DebugInfoLine && (dwAllowDbgFilter & DbgLine) != 0) ||
			(pEnum->val == DebugInfoC7 && (dwAllowDbgFilter & DbgC7) != 0) ||
			(pEnum->val == DebugInfoPdb && (dwAllowDbgFilter & DbgPdb) != 0) ||
			(pEnum->val == DebugInfoENC && (dwAllowDbgFilter & DbgENC) != 0)
		   );	// filter
}

CDebugInfoEnum DebugInfoEnum;

BEGIN_LOCALIZED_ENUM_LIST(CDebugInfoEnum)
	LOCALIZED_ENUM_ENTRY(IDS_DebugInfo0 /*"None"*/, DebugInfoNone)
	LOCALIZED_ENUM_ENTRY(IDS_DebugInfo1 /*"Line Numbers Only"*/, DebugInfoLine)
	LOCALIZED_ENUM_ENTRY(IDS_DebugInfo2 /*"C7 Compatible"*/, DebugInfoC7)
	LOCALIZED_ENUM_ENTRY(IDS_DebugInfo3 /*"Program Database"*/, DebugInfoPdb)
	LOCALIZED_ENUM_ENTRY(IDS_DebugInfo4 /*"PDB - Edit & Continue"*/, DebugInfoENC)
END_LOCALIZED_ENUM_LIST()

DEFINE_LOCALIZED_ENUM(ListAsmEnum)
	LOCALIZED_ENUM_ENTRY(IDS_ListAsm0 /*"No Listing"*/, 0)
	LOCALIZED_ENUM_ENTRY(IDS_ListAsm1 /*"Assembly-Only Listing"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_ListAsm2 /*"Assembly, Machine Code, and Source"*/, 2)
	LOCALIZED_ENUM_ENTRY(IDS_ListAsm3 /*"Assembly with Machine Code"*/, 3)
	LOCALIZED_ENUM_ENTRY(IDS_ListAsm4 /*"Assembly with Source Code"*/, 4)
END_LOCALIZED_ENUM_LIST()

DEFINE_LOCALIZED_ENUM(InlineEnum)
	LOCALIZED_ENUM_ENTRY(IDS_Inline0 /*"Disable *"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_Inline1 /*"Only __inline"*/, 2)
	LOCALIZED_ENUM_ENTRY(IDS_Inline2 /*"Any Suitable"*/, 3)
END_LOCALIZED_ENUM_LIST()

DEFINE_LOCALIZED_ENUM(OptimizeOptEnum)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt0 /*"Assume No Aliasing"*/, P_Optimize_A)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt1 /*"Assume Aliasing Across Function Calls"*/, P_Optimize_W)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt2 /*"Global Optimizations"*/, P_Optimize_G)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt3 /*"Generate Intrinsic Functions"*/, P_Optimize_I)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt4 /*"Improve Float Consistency"*/, P_Optimize_P)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt5 /*"Favor Small Code"*/, P_Optimize_S)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt6 /*"Favor Fast Code"*/, P_Optimize_T)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt7 /*"Frame-Pointer Omission"*/, P_Optimize_Y)
	LOCALIZED_ENUM_ENTRY(IDS_OptimizeOpt8 /*"Full Optimization"*/, P_Optimize_X)
END_LOCALIZED_ENUM_LIST()

DEFINE_LOCALIZED_ENUM(PtrMbr1Enum)
	LOCALIZED_ENUM_ENTRY(IDS_PtrMbr10 /*"Best-Case Always *"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_PtrMbr11 /*"General-Purpose Always"*/, 2)
END_LOCALIZED_ENUM_LIST()

DEFINE_LOCALIZED_ENUM(PtrMbr2Enum)
	LOCALIZED_ENUM_ENTRY(IDS_PtrMbr20 /*"Point to Any Class *"*/, 1)
	LOCALIZED_ENUM_ENTRY(IDS_PtrMbr21 /*"Point to Single- and Multiple-Inheritance Classes"*/, 3)
	LOCALIZED_ENUM_ENTRY(IDS_PtrMbr22 /*"Point to Single-Inheritance Classes"*/, 2)
END_LOCALIZED_ENUM_LIST()


// N.B. we don't call the base-class Validate()
// 'cos IDC_OPTSTR is no in the control map, we are 'faking'
// control validation by setting explicitly m_nValidateID and
// PostMessage(WM_VALIDATE_REQ) in the base-class
BOOL CCompilerPageTab::Validate()
{
	if (m_nValidateID == IDC_OPTSTR)
	{
		ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
		
		int fExcludeLocals = FALSE;
		// we need to map through the root option handler
		m_pSlob->GetIntProp(m_popthdlr->GetRootOptionHandler()->MapLogical(P_GenBrowserInfoNoLcl),
						    fExcludeLocals); 
							 
		return Validate_DestinationProp(P_OutputDir_Sbr, fExcludeLocals ? IDOPT_LISTBSCNOLCL :IDOPT_LISTBSC,
										IDC_OPTSTR, IDS_DEST_SBR,
									    "sbr",										// must be extension
									    ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject)) ||
									    ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProjGroup)),	// must be directory
									    FALSE);										// cant be directory
	}
	else if(IDC_INCLUDE==m_nValidateID)
	{
		//
		// Strip quotes.
		//
		CString strOption;
		CWnd * pWnd = GetDlgItem(m_nValidateID);
		ASSERT(pWnd != (CWnd *)NULL);
		pWnd->GetWindowText(strOption);

		int index;

		while (-1 != (index=strOption.Find('"')) )
		{
			CString Left;
			CString Right;

			if (0!=index)
				Left = strOption.Left(index);

			if (index+1 < strOption.GetLength())
				Right = strOption.Mid(index+1);

			strOption = Left + Right;
		}

		pWnd->SetWindowText(strOption);
	}

	return COptionMiniPage::Validate();
}


BOOL IsLearningBox();

void CCompilerGeneralPage::InitPage(void)
{
	// Check what kind of debug info is allowed by platform
	CPlatform * pPlatform = ((CProjItem *)m_pSlob)->GetProject()->GetCurrentPlatform();
	DWORD dwAttrib = pPlatform->GetAttributes();
	BOOL fEnc = dwAttrib & PIA_Supports_ENC;
	DebugInfoEnum.AllowDbg(
		CDebugInfoEnum::DbgNone |
		CDebugInfoEnum::DbgLine |
		CDebugInfoEnum::DbgC7 |
		CDebugInfoEnum::DbgPdb |
		(fEnc  ? CDebugInfoEnum::DbgENC : 0)
		);

	CCompilerPageTab::InitPage();
}

BOOL CCompilerGeneralPage::OnPropChange(UINT idProp)
{
	if( IsLearningBox() )
	{
		GetDlgItem(IDC_OPTIMIZE)->EnableWindow(FALSE);
	}
	// call our base-class
	return CCompilerPageTab::OnPropChange(idProp);
}



BEGIN_IDE_CONTROL_MAP(CCompilerGeneralPage, IDDP_PROJ_COMPILER, IDS_COMPILER32)
	MAP_COMBO_LIST(IDC_OPTWARN, P_WarningLevel, WarnLevelEnum)
	MAP_CHECK(IDC_WARNASERR, P_WarnAsError)
	MAP_CHECK(IDC_LISTBSC, P_GenBrowserInfo)
	MAP_COMBO_LIST(IDC_OPTIMIZE, P_Optimize, OptimizeEnum)
	MAP_COMBO_LIST(IDC_DEBUG, P_DebugInfo, DebugInfoEnum)
	MAP_EDIT(IDC_MACROS, P_MacroNames)
END_IDE_CONTROL_MAP()


BEGIN_IDE_CONTROL_MAP(CCompilerCustomPage, IDDP_COMPILER_CUSTOM, IDS_CAT_CUSTOM)
	MAP_CHECK(IDC_OPTEXT, P_MSExtension)
	MAP_CHECK(IDC_OPTINCLINK, P_IncLinking)
	MAP_CHECK(IDC_STRPOOL, P_StringPool)
	MAP_CHECK(IDC_CPLRNOLOGO, P_CplrNoLogo)
	MAP_CHECK(IDC_ENABLE_GI, P_Enable_Incr)
	MAP_CHECK(IDC_ENABLE_MR, P_Enable_MR)
END_IDE_CONTROL_MAP()

BOOL CCompilerCustomPage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	// we want to enable/disable prop page items
	// - we'll enable/disable the static text
	// - OnPropChange() will enable/disable the rest
	switch (idPropL)
	{
		case P_Optimize:
			// O1, O2 alter the default for /Gf and /Gy
			CCompilerPageTab::OnPropChange(MapLogical(P_StringPool));
			CCompilerPageTab::OnPropChange(MapLogical(P_IncLinking));
			break;

		case P_Enable_Incr:
		case P_DebugInfo:
			// /Zi and /Gi alter /Gm
			CCompilerPageTab::OnPropChange(MapLogical(P_Enable_MR));
			break;

		default:
			break;
	}

	// call our base-class
	return CCompilerPageTab::OnPropChange(idProp);
}

BEGIN_IDE_CONTROL_MAP(CCompilerCustomCppPage, IDDP_COMPILER_CUSTOMCPP, IDS_CAT_CUSTOMCPP)
	MAP_COMBO_LIST(IDC_PTRMBR1, P_PtrMbr1, PtrMbr1Enum)
	MAP_COMBO_LIST(IDC_PTRMBR2, P_PtrMbr2, PtrMbr2Enum)
	MAP_CHECK(IDC_VTORDISP, P_VtorDisp)
/* temporarily reverse the logic for PDC (BamBam) 'cos the compiler default will be /GX- for this
** release .. ie. we need [X] Enable EH in the options dialog
	MAP_CHECK(IDC_DISABLE_EH, P_Disable_EH)
*/
	MAP_CHECK(IDC_ENABLE_EH, P_Enable_EH)
	MAP_CHECK(IDC_ENABLE_RTTI, P_Enable_RTTI)
END_IDE_CONTROL_MAP()

BOOL CCompilerCustomCppPage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	// we want to enable/disable prop page items
	// - we'll enable/disable the static text
	// - OnPropChange() will enable/disable the rest
	switch (idPropL)
	{
		case P_PtrMbr1:
		{
			int nVal;

			CCompilerPageTab::OnPropChange(MapLogical(P_PtrMbr2));

			GetDlgItem(IDC_TXT_PTRMBR2)->EnableWindow(m_pSlob->GetIntProp(MapLogical(P_PtrMbr2), nVal) != invalid);
			break;
		}

		default:
			break;
	}

	// call our base-class
	return CCompilerPageTab::OnPropChange(idProp);
}


BEGIN_IDE_CONTROL_MAP(CCompilerListfilePage, IDDP_COMPILER_LISTFILE, IDS_CAT_LISTFILE)
	MAP_CHECK(IDC_LISTBSC, P_GenBrowserInfo)
	MAP_CHECK(IDC_LISTLOCAL, P_GenBrowserInfoNoLcl)
	MAP_COMBO_LIST(IDC_LISTASM, P_ListAsm, ListAsmEnum)
	MAP_EDIT(IDC_LISTBSC_NME, P_OutputDir_Sbr)
	MAP_EDIT(IDC_LISTASM_NME, P_OutputDir_Cod)
END_IDE_CONTROL_MAP()

BOOL CCompilerListfilePage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fOldRefresh = m_pToolOptionTab->m_fRefresh;
	CString strVal;

	// we want to enable/disable prop page items
	// - we'll enable/disable the static text
	// - OnPropChange() will enable/disable the rest
	switch (idPropL)
	{
		case P_GenBrowserInfo:
		{
			// batch up these and then refresh on last one!
			m_pToolOptionTab->m_fRefresh = FALSE;
			CCompilerPageTab::OnPropChange(MapLogical(P_GenBrowserInfoNoLcl));
			m_pToolOptionTab->m_fRefresh = fOldRefresh;
			CCompilerPageTab::OnPropChange(MapLogical(P_OutputDir_Sbr));

			GetDlgItem(IDC_TXT_LISTBSC)->EnableWindow(m_pSlob->GetStrProp(MapLogical(P_OutputDir_Sbr), strVal) != invalid);
			break;
		}

		case P_ListAsm:
		{
			CCompilerPageTab::OnPropChange(MapLogical(P_OutputDir_Cod));

			GetDlgItem(IDC_TXT_LISTASM1)->EnableWindow(m_pSlob->GetStrProp(MapLogical(P_OutputDir_Cod), strVal) != invalid);
			break;
		}

		default:
			break;
	}

	// call our base-class
	return CCompilerPageTab::OnPropChange(idProp);
}

BOOL CCompilerListfilePage::Validate()
{
	if (m_nValidateID == IDC_LISTBSC_NME)
	{
		ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));

		if (!Validate_Destination(IDC_LISTBSC_NME, IDS_DEST_SBR,
								  "sbr",										// must be extension
							 	  ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject)) ||
							 	  ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProjGroup)),	// must be directory
							 	  FALSE))										// cant be directory
			return FALSE;
	}

	return CCompilerPageTab::Validate();
}


BEGIN_IDE_CONTROL_MAP(CCompilerPreprocessPage, IDDP_COMPILER_PREPROCESS, IDS_CAT_PREPROCESS)
	MAP_EDIT(IDC_MACROS, P_MacroNames)
	MAP_EDIT(IDC_UNDEFS, P_UndefMacros)
	MAP_CHECK(IDC_UNDEFINE, P_Undefine)
	MAP_EDIT(IDC_INCLUDE, P_CplrIncludes)
	MAP_CHECK(IDC_IGNINC, P_CplrIgnIncPath)
END_IDE_CONTROL_MAP()


BEGIN_IDE_CONTROL_MAP(CCompilerDebugOptPage, IDDP_COMPILER_DEBUG, IDS_CAT_DEBUGOPT)
	MAP_COMBO_LIST(IDC_DEBUG, P_DebugInfo, DebugInfoEnum)
END_IDE_CONTROL_MAP()


BEGIN_IDE_CONTROL_MAP(CCompilerPchPage, IDDP_COMPILER_PCH, IDS_CAT_PCH)
	MAP_RADIO(IDC_NOPCH, IDC_USEPCH, 1, P_PseudoPch)
	MAP_EDIT(IDC_AUTOPCHNAME, P_AutoPchUpTo)
	MAP_EDIT(IDC_CREATENAME, P_PchCreateUptoHeader)
	MAP_EDIT(IDC_USENAME, P_PchUseUptoHeader)
END_IDE_CONTROL_MAP()

BOOL CCompilerPchPage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fOldRefresh = m_pToolOptionTab->m_fRefresh;
	int nVal;

	// we want to enable/disable prop page items
	// - we'll enable/disable the static text
	// - OnPropChange() will enable/disable the rest
	switch (idPropL)
	{
		case P_PseudoPch:
		{
			if (m_pSlob->GetIntProp(idProp, nVal) != valid)
				nVal = 1;	// no selection

			m_pToolOptionTab->m_fRefresh = FALSE;

			// enable/disable text
			GetDlgItem(IDC_TXT_AUTOPCH)->EnableWindow(nVal == 2);
			GetDlgItem(IDC_TXT_CREATE)->EnableWindow(nVal == 3);
			GetDlgItem(IDC_TXT_USE)->EnableWindow(nVal == 4);

			CCompilerPageTab::OnPropChange(MapLogical(P_AutoPchUpTo));
			CCompilerPageTab::OnPropChange(MapLogical(P_PchCreateUptoHeader));

			m_pToolOptionTab->m_fRefresh = fOldRefresh;

			CCompilerPageTab::OnPropChange(MapLogical(P_PchUseUptoHeader));
		}

		default:
			break;
	}

	// call our base-class
	return CCompilerPageTab::OnPropChange(idProp);
}


BEGIN_IDE_CONTROL_MAP(CCompilerOptimizePage, IDDP_COMPILER_OPTIMIZE, IDS_CAT_OPTIMIZE)
	MAP_CHECK_LIST(IDC_OPTIMIZE, OptimizeOptEnum)
	MAP_COMBO_LIST(IDC_OPTIMIZE2, P_Optimize, OptimizeEnum)
	MAP_COMBO_LIST(IDC_INLINECTRL, P_InlineControl, InlineEnum)
END_IDE_CONTROL_MAP()


BOOL CCompilerOptimizePage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fOldRefresh = m_pToolOptionTab->m_fRefresh;
	if( IsLearningBox() )
	{
		GetDlgItem(IDC_OPTIMIZE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_INLINECTRL)->EnableWindow(FALSE);
	}

	// we want to enable/disable prop page items
	// - we'll enable/disable the static text
	// - OnPropChange() will enable/disable the rest
	switch (idPropL)
	{
		case P_MSExtension:
			// Za alters default for /Op
			CCompilerPageTab::OnPropChange(MapLogical(P_Optimize_P));
			break;
		
		case P_Optimize_X:
			// Ox alters default for /Ob
			CCompilerPageTab::OnPropChange(MapLogical(P_InlineControl));

			// Ox alters default for /Oy
			CCompilerPageTab::OnPropChange(MapLogical(P_Optimize_Y));
			break;

		case P_Optimize:
		{
			// O1, /O2 alter default for /Ob
			CCompilerPageTab::OnPropChange(MapLogical(P_InlineControl));

			// Ox alters default for /Oy
			CCompilerPageTab::OnPropChange(MapLogical(P_Optimize_Y));

			// we'll use Optimize_A to determine the availability of the optimize check-list
			// make sure we update our text view of the options
			CCompilerPageTab::OnPropChange(MapLogical(P_Optimize_A));

			// they are collectively updated, bypass OnPropChange() for efficiency reasons
			GetDlgItem(IDC_OPTIMIZE)->Invalidate(FALSE);

			// they are collectively enabled/disabled
			int nVal;
			GetDlgItem(IDC_OPTIMIZE)->EnableWindow(m_pSlob->GetIntProp(MapLogical(P_Optimize_A), nVal) != invalid);
			break;
		}

		default:
			break;
	}

	// call our base-class
	return CCompilerPageTab::OnPropChange(idProp);
}
