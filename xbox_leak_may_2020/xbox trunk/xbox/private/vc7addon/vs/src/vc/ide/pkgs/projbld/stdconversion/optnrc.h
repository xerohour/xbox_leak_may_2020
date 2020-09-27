//
// Resource Compiler Tool Options and Tool
//

#pragma once

//----------------------------------------------------------------
// our COMMON resource compiler option data
//----------------------------------------------------------------

// option handler
DEFN_OPTHDLR_COMMON
(
	ResCompiler, /* name */
	szAddOnGeneric, BCID_OptHdlr_RcCompiler, /* generic resource compiler option handler */
	szAddOnGeneric, BCID_Tool_RcCompiler /* generic resource compiler tool */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// check deps.
	virtual BOOL CheckDepOK(UINT idProp);

	// is this a default of one of our 'fake' string props?
	// (we need special handling here)
	virtual BOOL IsDefaultStringProp(UINT idProp, CString & strVal);

	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);

	// return a 'fake' string prop
	virtual GPT GetDefStrProp(UINT idProp, CString & strVal);

	// always show this?
	virtual BOOL AlwaysShowDefault(UINT idProp);

	// return a 'fake' int prop
	virtual GPT GetDefIntProp(UINT idProp, int & nVal);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

	// called whenever a prop has changed
	// we can use this to delete a prop if we want the fake prop instead
	virtual void OnOptionStrPropChange(UINT idProp, const CString & strVal) ;

	// get the output extension
	const CString *GetOutputExtension();

private:
	// form a 'fake' string prop
	virtual void FormFakeIntProp(UINT idProp, int & nVal);
	virtual void FormFakeStrProp(UINT idProp, CString & strVal);

	BOOL m_fEnableLangID;

END_OPTHDLR()

// resource compiler properties
#define P_ResVerbose					 0x0000
#define P_ResIgnInCPath					 0x0001
#define P_ResIncludes					 0x0002
#define P_OutNameRes					 0x0005
#define P_ResMacroNames					 0x0006
#define P_Res_Input_Ext					 0x0009
#define P_ResLangID						 0x000a
#define P_ResUnknownOption				 0x000b
#define P_ResUnknownString				 0x000c

// resource compiler options (option ids)
#define IDOPT_RESVERBOSE				IDOPT_BASE + 0
#define IDOPT_RESIGNINC					IDOPT_BASE + 1
#define IDOPT_RESMACROS					IDOPT_BASE + 2
#define IDOPT_RESINCLUDES				IDOPT_BASE + 3
#define IDOPT_OUTDIR_RES				IDOPT_BASE + 4
#define IDOPT_RESLANGID					IDOPT_BASE + 5
#define IDOPT_RES_INPUT_EXT				IDOPT_BASE + 6

// option handler
DEFN_OPTHDLR_PLATFORM
(
	ResCompilerNT, /* name */
	szAddOnGeneric, BCID_OptHdlr_RcCompilerNT, /* generic NT resource compiler option handler */
	szAddOnGeneric, BCID_Tool_RcCompilerNT, /* generic NT resource compiler tool */
	szAddOnGeneric, BCID_OptHdlr_RcCompiler /* base generic resource compiler option handler */
)
	
	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()
 
END_OPTHDLR()

#include "projtool.h"		// the CBuildTool classes

class  CRCCompilerTool : public CBuildTool
{
	DECLARE_DYNAMIC (CRCCompilerTool)

public:
	CRCCompilerTool();
};

// NT RC Compiler Tool
class  CRCCompilerNTTool : public CRCCompilerTool 
{
	DECLARE_DYNAMIC (CRCCompilerNTTool)

public:
	//	Ensure that the tool will build an MFC project item.
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC);
};

