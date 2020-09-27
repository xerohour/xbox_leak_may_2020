//
// Common Compiler Tool Options
//
// [matthewt]
//

#pragma once

#include "projtool.h"			// the CBuildTool classes

//----------------------------------------------------------------
// our COMMON compiler option data
//----------------------------------------------------------------

// COMMON compiler option handler
DEFN_OPTHDLR_COMMON
(
	CompilerCommon, /* name */
	szAddOnGeneric,	BCID_OptHdlr_Compiler, /* generic compiler option handler */
	szAddOnGeneric, BCID_Tool_Compiler /* generic compiler tool */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// should this be shown always?
	virtual BOOL AlwaysShowDefault(UINT idProp);

	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);
	virtual BOOL SetsFakePropValue(UINT idProp);

 	// return a 'fake' string prop
	virtual GPT GetDefStrProp(UINT idProp, CString & val);
	virtual GPT GetDefIntProp(UINT idProp, int & val);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

	// handle setting of option int props
	virtual void OnOptionIntPropChange(UINT idProp, int nVal);
	virtual void OnOptionStrPropChange(UINT idProp, const CString & strVal);

	// provide invalidation of option props
	virtual BOOL CheckDepOK(UINT idProp);

private:
	// form a 'fake' string prop
	virtual void FormFakeStrProp(UINT idProp, CString & strVal);

END_OPTHDLR()

// COMMON compiler tool properties
#define P_CplrNoLogo 					0x0000
#define P_MSExtension				   	0x0001
#define P_IncLinking			   		0x0002
#define P_WarningLevel					0x0003
#define P_WarnAsError					0x0004
#define P_StringPool					0x0005
#define P_PtrMbr1						0x0006
#define P_PtrMbr2						0x0007
#define P_VtorDisp						0x0008
#define P_DebugInfo						0x0009
#define P_AutoPch						0x000a
#define P_AutoPchUpTo					0x000b
#define P_Optimize						0x000c
#define P_Optimize_A					0x000d
#define P_Optimize_W					0x000e
#define P_Optimize_G					0x000f
#define P_Optimize_I					0x0010
#define P_Optimize_P					0x0011
#define P_Optimize_T					0x0012
#define P_Optimize_S					0x0013
#define P_Optimize_Y					0x0014
#define P_Optimize_X					0x0015
#define P_InlineControl					0x0016
#define P_CplrIgnInCPath				0x0017
#define P_Undefine						0x0018
#define P_ListAsm						0x0019
#define P_AsmListHasMC					0x001a
#define P_AsmListHasSrc					0x001b
#define P_GenBrowserInfo				0x001c
#define P_MacroNames					0x001d
#define P_MacroValues					0x0020
#define P_UndefMacros					0x0023
#define P_CplrIncludes					0x0026
#define P_TreatFileAsC1					0x0027
#define P_TreatFileAsC2					0x0028
#define P_TreatFileAsCPP1				0x0029
#define P_TreatFileAsCPP2				0x002a
#define P_PchCreate						0x002b
#define P_PchCreateUptoHeader			0x002c
#define P_PchUse						0x002d
#define P_PchUseUptoHeader				0x002e
#define P_OutputDir_Obj					0x002f
#define P_OutputDir_Pch					0x0030
#define P_OutputDir_Sbr					0x0031
#define P_OutputDir_Pdb					0x0032
#define P_CompileOnly					0x0033
#define P_CplrUnknownOption				0x0034
#define P_CplrUnknownString				0x0035
#define P_PchDefaultName				0x0036
#define P_PdbDefaultName				0x0037
#define P_GenBrowserInfoNoLcl			0x0038
#define P_CPP_Input_Ext					0x0039
#define P_Enable_EH						0x003a
#define P_OutputDir_Cod					0x003b
#define P_StackProbe					0x003c
#define P_StackProbeTHold				0x003d
#define P_MaxLenExtNames				0x003e
#define P_UseChasAsUnsigned				0x003f
#define P_EmbedString					0x0040
#define P_TurnOffWarnings				0x0041 
#define P_NoDefLibNameInObj				0x0042
#define P_PrepPreserveComments			0x0043
#define P_Preprocess					0x0044
#define P_PreprocessWOLines				0x0045
#define P_StackSize						0x0046
#define P_PrepprocessToFile				0x0047
#define P_GenFuncProto					0x0048
#define P_ForceIncludes					0x0049
#define P_CheckSyntax					0x004c
#define P_Enable_MR						0x004d
#define P_Enable_Incr					0x004e
#define P_Enable_RTTI					0x004f
#define P_PseudoPch						0x0050
#define P_Enable_FD						0x0051
#define P_Enable_FC						0x0052
#define P_Enable_dlp					0x0053
#define P_COM_EmitMergedSource          0x0054
#define P_COM_HresultKnowledge          0x0055

// Debug information constants
#define DebugInfoNone					0
#define DebugInfoLine					1
#define DebugInfoC7						2
// WARNING: DebugInfo constants greater than or
// equal to DebugInfoPdb assume the existence
// of a program database
#define DebugInfoPdb					3
#define DebugInfoENC					4

// COMMON compiler tool options (option ids)
#define IDOPT_CPLRNOLOGO				IDOPT_BASE + 0							
#define IDOPT_MSEXT						IDOPT_BASE + 4
#define IDOPT_INCLINK					IDOPT_BASE + 5
#define IDOPT_WARN						IDOPT_BASE + 6
#define IDOPT_WARNASERROR				IDOPT_BASE + 7
#define IDOPT_STRPOOL					IDOPT_BASE + 8
#define IDOPT_PTRMBR1					IDOPT_BASE + 9
#define IDOPT_PTRMBR2					IDOPT_BASE + 10
#define IDOPT_VTORDISP					IDOPT_BASE + 11
#define IDOPT_DEBUGINFO					IDOPT_BASE + 12
#define IDOPT_AUTOPCH					IDOPT_BASE + 13
#define IDOPT_OPTIMIZE					IDOPT_BASE + 14
#define IDOPT_OPT_A						IDOPT_BASE + 15
#define IDOPT_OPT_W						IDOPT_BASE + 16
#define IDOPT_OPT_G						IDOPT_BASE + 17
#define IDOPT_OPT_I						IDOPT_BASE + 18
#define IDOPT_OPT_P						IDOPT_BASE + 19
#define IDOPT_OPT_T						IDOPT_BASE + 20
#define IDOPT_OPT_S						IDOPT_BASE + 21
#define IDOPT_OPT_Y						IDOPT_BASE + 22
#define IDOPT_OPT_X						IDOPT_BASE + 23
#define IDOPT_INLINECTRL				IDOPT_BASE + 24
#define IDOPT_CPLRIGNINC				IDOPT_BASE + 25
#define IDOPT_UNDEFINE					IDOPT_BASE + 26
#define IDOPT_LISTASM					IDOPT_BASE + 27
#define IDOPT_LISTBSC					IDOPT_BASE + 28
#define IDOPT_TREATFILEAS_C1			IDOPT_BASE + 29
#define IDOPT_TREATFILEAS_CPP1			IDOPT_BASE + 30
#define IDOPT_MACROS					IDOPT_BASE + 31
#define IDOPT_UNDEFMACROS				IDOPT_BASE + 32
#define IDOPT_CPLRINCLUDES				IDOPT_BASE + 33
#define IDOPT_TREATFILEAS_C2			IDOPT_BASE + 34
#define IDOPT_TREATFILEAS_CPP2			IDOPT_BASE + 35
#define IDOPT_PCHNAME					IDOPT_BASE + 36
#define IDOPT_PCHCREATE					IDOPT_BASE + 37
#define IDOPT_PCHUSE					IDOPT_BASE + 38
#define IDOPT_OUTDIR_OBJ				IDOPT_BASE + 39
#define IDOPT_OUTDIR_PDB				IDOPT_BASE + 40
#define IDOPT_COMPILE_ONLY				IDOPT_BASE + 41
#define IDOPT_LISTBSCNOLCL				IDOPT_BASE + 42
#define IDOPT_ENABLE_EH					IDOPT_BASE + 43
#define IDOPT_OUTDIR_COD				IDOPT_BASE + 44
#define IDOPT_STACK_PROBE				IDOPT_BASE + 45
#define IDOPT_STACK_PROBE_THOLD			IDOPT_BASE + 46
#define IDOPT_MAXLEN_EXTNAME			IDOPT_BASE + 47
#define IDOPT_USECHAR_AS_UNSIGNED		IDOPT_BASE + 48
#define IDOPT_EMBED_STRING				IDOPT_BASE + 49
#define IDOPT_TURN_OFF_WARNINGS			IDOPT_BASE + 50
#define IDOPT_NO_DEFLIB_NAME			IDOPT_BASE + 51
#define IDOPT_PREP_COMMENTS				IDOPT_BASE + 52
#define IDOPT_PREPROCESS				IDOPT_BASE + 53
#define IDOPT_PREPROCESS_WO_LINES		IDOPT_BASE + 54
#define IDOPT_STACKSIZE					IDOPT_BASE + 55
#define IDOPT_PREPROCESS_TO_FILE		IDOPT_BASE + 56
#define IDOPT_GEN_FUNC_PROTO			IDOPT_BASE + 57
#define IDOPT_FORCEINCLUDES				IDOPT_BASE + 58
#define IDOPT_CHECK_SYNTAX				IDOPT_BASE + 59
#define IDOPT_ENABLE_MR					IDOPT_BASE + 60
#define IDOPT_ENABLE_INCR				IDOPT_BASE + 61
#define IDOPT_ENABLE_RTTI				IDOPT_BASE + 62
#define IDOPT_ENABLE_FD					IDOPT_BASE + 63
#define IDOPT_ENABLE_FC					IDOPT_BASE + 64
#define IDOPT_ENABLE_DLP				IDOPT_BASE + 65
#define IDOPT_CPP_INPUT_EXT				IDOPT_BASE + 66
#define IDOPT_COM_EmitMergedSource      IDOPT_BASE + 67
#define IDOPT_COM_ImplicitErrHandling   IDOPT_BASE + 68
#define IDOPT_COM_HresultKnowledge      IDOPT_BASE + 69

#include "optncplr.h2"	// our option control IDs

class  CCCompilerTool : public CBuildTool
{
	DECLARE_DYNAMIC (CCCompilerTool)
 
	CCCompilerTool();

	//	Ensure that the tool will build an MFC project item.
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC);
};

class  CCCompilerNTTool : public CCCompilerTool
{
	DECLARE_DYNAMIC (CCCompilerNTTool)

public:
	CCCompilerNTTool() : CCCompilerTool() {}

	//	Ensure that the tool will build an MFC project item.
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC);
};


