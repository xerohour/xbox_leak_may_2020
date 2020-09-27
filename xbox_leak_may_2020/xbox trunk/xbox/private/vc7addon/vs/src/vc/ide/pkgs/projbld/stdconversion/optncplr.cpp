//
// Common Compiler Tool Options and Tool
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop

#include "optncplr.h"	// our local header file
#include "project.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCCompilerTool, CBuildTool)
IMPLEMENT_DYNAMIC(CCCompilerNTTool, CCCompilerTool)

//----------------------------------------------------------------
// our COMMON compiler option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(CompilerCommon, P_CplrUnknownOption, P_CplrUnknownString, P_CplrNoLogo, P_COM_HresultKnowledge, TRUE)
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
	IDOPT_STRPOOL,		"G%{f|F}1",				OPTARGS1(P_StringPool),					single,
	IDOPT_INCLINK,		"Gy%T1",				OPTARGS1(P_IncLinking),					single,
	IDOPT_CPLRIGNINC,	"X%T1",					OPTARGS1(P_CplrIgnInCPath),				single,
	IDOPT_CPLRINCLUDES,	"I[ ]%1",				OPTARGS1(P_CplrIncludes),				multiple,
	IDOPT_FORCEINCLUDES,	"FI%1",				OPTARGS1(P_ForceIncludes),				multiple,
	IDOPT_UNDEFINE,		"u%T1",					OPTARGS1(P_Undefine),					single,
	IDOPT_MACROS,		"D[ ]%1",				OPTARGS1(P_MacroNames),					multiple,
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
	IDOPT_ENABLE_FC,	        "FC%T1",		OPTARGS1(P_Enable_FC),					single,
	IDOPT_COM_EmitMergedSource,	"Fx%T1",		OPTARGS1(P_COM_EmitMergedSource),       single,
	IDOPT_STACKSIZE,			"F%1",			OPTARGS1(P_StackSize),					single,
	IDOPT_ENABLE_DLP,			"dlp%{-|}1",	OPTARGS1(P_Enable_dlp),					single,
	IDOPT_PREPROCESS_TO_FILE,	"P%T1",			OPTARGS1(P_PrepprocessToFile),			single,
	IDOPT_GEN_FUNC_PROTO,		"Zg%T1",		OPTARGS1(P_GenFuncProto),				single,
	IDOPT_CHECK_SYNTAX,			"Zs%T1",		OPTARGS1(P_CheckSyntax),				single,
	IDOPT_UNKNOWN_OPTION,	"",					NO_OPTARGS,								single,
	IDOPT_UNKNOWN_STRING,	"",					NO_OPTARGS,								single,
	IDOPT_COMPILE_ONLY,	"c%T1",					OPTARGS1(P_CompileOnly),				single,
	IDOPT_TREATFILEAS_C1,	"Tc%T1",			OPTARGS1(P_TreatFileAsC1),				single,
	IDOPT_TREATFILEAS_C2,	"TC%T1",			OPTARGS1(P_TreatFileAsC2),				single,
	IDOPT_TREATFILEAS_CPP1,	"Tp%T1",			OPTARGS1(P_TreatFileAsCPP1),			single,	
	IDOPT_TREATFILEAS_CPP2,	"TP%T1",			OPTARGS1(P_TreatFileAsCPP2),			single,	
	IDOPT_CPP_INPUT_EXT,		"",				OPTARGS1(P_CPP_Input_Ext),				single,
	IDOPT_COM_HresultKnowledge,	"noHRESULT%{|-}1",	OPTARGS1(P_COM_HresultKnowledge),	single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(CompilerCommon)
	OPTDEF_BOOL(CplrNoLogo, FALSE)
	OPTDEF_BOOL(MSExtension, FALSE)
	OPTDEF_BOOL(IncLinking, FALSE)
	OPTDEF_INT(WarningLevel, 2)
	OPTDEF_BOOL(WarnAsError, FALSE)
	OPTDEF_INT(StringPool, 0)
	OPTDEF_INT(PtrMbr1, 1)
	OPTDEF_INT(PtrMbr2, 1)
	OPTDEF_BOOL(VtorDisp, FALSE)
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
	OPTDEF_BOOL(CplrIgnInCPath, FALSE)
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
	OPTDEF_BOOL(TreatFileAsC1, FALSE)
	OPTDEF_BOOL(TreatFileAsC2, FALSE)
	OPTDEF_BOOL(TreatFileAsCPP1, FALSE)
	OPTDEF_BOOL(TreatFileAsCPP2, FALSE)
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
	OPTDEF_STRING(CPP_Input_Ext, "*.cpp;*.c")
	OPTDEF_BOOL(COM_EmitMergedSource, FALSE)
	OPTDEF_BOOL(COM_HresultKnowledge, FALSE)
END_OPTDEF_MAP()

BOOL OLD_OPTION_HANDLER(CompilerCommon)::AlwaysShowDefault(UINT idProp)
{
	return FALSE;
}

// COMMON compiler tool option default map 'faking'

BOOL OLD_OPTION_HANDLER(CompilerCommon)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (
			idPropL == P_OutputDir_Obj ||
			idPropL == P_CPP_Input_Ext	   ||
			idPropL == P_OutputDir_Pch ||
			idPropL == P_OutputDir_Sbr ||
			idPropL == P_OutputDir_Pdb ||
			idPropL == P_OutputDir_Cod ||
			idPropL == P_PchDefaultName ||
			idPropL == P_PseudoPch
		   );
}

BOOL OLD_OPTION_HANDLER(CompilerCommon)::SetsFakePropValue(UINT idProp)
{
	VSASSERT(IsFakeProp(idProp), "Should only be called with fake props!");

	UINT idPropL = MapActual(idProp);
	switch (idPropL)
	{
	case P_PchDefaultName:
	case P_OutputDir_Pch:
		return FALSE;		// never defined at file level (no multiple .pch support)

	case P_OutputDir_Obj:
	case P_CPP_Input_Ext	:
	case P_OutputDir_Sbr:
	case P_OutputDir_Pdb:
	case P_OutputDir_Cod:
		// these are solely dependent on the output/target directory (which is checked
		// separately).
		return FALSE;
	}

	return TRUE;	// we don't know anything about this property
}

UINT OLD_OPTION_HANDLER(CompilerCommon)::GetFakePathDirProp(UINT idProp)
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

void OLD_OPTION_HANDLER(CompilerCommon)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	CPath pathFp, path;
	CDir dirFp;
	CString strDefName;

	// assert this is a compiler 'fake' string prop?
	VSASSERT(IsFakeProp(idProp), "FormFakeProp called with non-fake prop ID");

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


	// make sure the project is in our config.
	CProjItem * pItem = (CProjItem *)m_pSlob;
	CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	projTempConfigChange.ChangeConfig((CConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord);

	if (idPropL == P_CPP_Input_Ext)
	{
		CConfigurationRecord * pcr = (CConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord;
		CString strToolName = _T("cl.exe");
		CString strExtList = _T("");
		pcr->GetExtListFromToolName(strToolName, &strExtList);
		if(strExtList)
		{
			strVal = strExtList;
		}
		return;
	}

	// which output directory do we want to use?
	UINT idOutDirProp = GetFakePathDirProp(idProp);
	VSASSERT(idOutDirProp != (UINT)-1, "Failed to get proper fake path dir!");

	// the rest are output directory related
	// find one somewhere in the hierarchy

	
	// .pch uses project-level intermediate directory...
	// we can't use file-level because we don't have
	// support for multiple .pchs
	if (idPropL == P_OutputDir_Pch)
		pItem = pItem->GetProject();

	GPT gpt = pItem->GetStrProp(idOutDirProp, strVal);
	while (gpt != valid)
	{
		pItem = (CProjItem *)pItem->GetContainer();
		VSASSERT(pItem != (CSlob *)NULL, "Item's container is NULL!!!");
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
		VSASSERT(g_pActiveProject, "Active project not set!");
		CString strBase; 

        CProjItem * pItem = ((CProjItem *)m_pSlob);
		const CPath * ppathProj = pItem->GetProject()->GetFilePath();
		ppathProj->GetBaseNameString(strBase);

		strVal += strBase;
		strVal += _TEXT(".pch");
	}
}

GPT OLD_OPTION_HANDLER(CompilerCommon)::GetDefStrProp(UINT idProp, CString & val)
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

GPT OLD_OPTION_HANDLER(CompilerCommon)::GetDefIntProp(UINT idProp, int & val)
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

void OLD_OPTION_HANDLER(CompilerCommon)::OnOptionIntPropChange(UINT idProp, int nVal)
{
	VSASSERT(m_pSlob != (CSlob *)NULL, "No slob associated with this option handler!");

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

void OLD_OPTION_HANDLER(CompilerCommon)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
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

BOOL OLD_OPTION_HANDLER(CompilerCommon)::CheckDepOK(UINT idProp)
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
				((CProjItem *)m_pSlob)->SetOptBehaviour((OptBehaviour)(optbeh & ~OBInherit));

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

//////////////////////////////////////////////////////////////////////////////////////////////////
// CCompilerTool
//////////////////////////////////////////////////////////////////////////////////////////////////
CCCompilerTool::CCCompilerTool() : CBuildTool()
{
	m_strToolInput = _TEXT("*.c;*.cpp;*.cxx");
	m_strToolPrefix = _TEXT("CPP");
	m_strName = _TEXT("cl.exe");
}

///////////////////////////////////////////////////////////////////////////////
// Ensure that this project compiler options are ok to use/not use MFC
BOOL CCCompilerTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	CString strDefine;

	COptionHandler * popthdlr = GetOptionHandler(); VSASSERT(popthdlr != (COptionHandler *)NULL, "Missing option handler for CL!");

	if (iUseMFC != NoUseMFC)
		pProjItem->SetIntProp(MapLogical(P_Enable_EH), TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// CCompilerNTTool
/////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CCCompilerNTTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	return FALSE;
}
