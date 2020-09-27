//
// Linker Tool Options and Tool
//

#pragma once

//----------------------------------------------------------------
// our COMMON linker option data
//----------------------------------------------------------------

// option handler
DEFN_OPTHDLR_COMMON
(
	LinkerCommon, /* name */
	szAddOnGeneric, BCID_OptHdlr_Linker, /* generic linker option handler */
	szAddOnGeneric, BCID_Tool_Linker /* generic linker tool tool */
)
	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// is this a default of one of our 'fake' string props?
	// (we need special handling here)
	virtual BOOL IsDefaultStringProp(UINT idProp, CString & strVal);

	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);

	// should this be shown always?
	virtual BOOL AlwaysShowDefault(UINT idProp);

	// return a 'fake' string prop
	virtual GPT GetDefStrProp(UINT idProp, CString & strVal);
	virtual GPT GetDefIntProp(UINT idProp, int & nVal);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

	// provide invalidation of option props
	virtual BOOL CheckDepOK(UINT idProp);

	// called whenever a prop has changed
	// we can use this to delete a prop if we want the fake prop instead
	virtual void OnOptionIntPropChange(UINT idProp, int nVal);

private:
	// form a 'fake' string prop
	virtual void FormFakeStrProp(UINT idProp, CString & strVal);
	virtual void FormFakeIntProp(UINT idProp, int & nVal) ;

	BOOL m_fProjIsDll;	// HACK: for /DLL munging of /OUT:

END_OPTHDLR()

#define P_LinkNoLogo			0x0000
#define P_LinkVerbose			0x0001
#define P_GenMap				0x0002
#define P_MapName				0x0003
#define P_GenDebug				0x0004
#define P_DebugType				0x0005
#define P_NoDefaultLibs			0x0006
// 0x0007 - Add P_NoDefaultLibs
// 0x0008 - Subtract P_NoDefaultLibs
#define P_DefName				0x0009
#define P_Force					0x000b
#define P_OutName				0x000c
#define P_IncludeSym			0x000d
// 0x000e - Add P_IncludeSym
// 0x000f - Subtract P_IncludeSym
#define P_ExAllLibs				0x0010
#define P_ImpLibName			0x0011
#define P_GenDll				0x0012
#define P_IncrementalLink		0x0013
#define P_UsePDB				0x0014
#define P_UsePDBName			0x0015
#define P_Profile				0x0016
#define P_MachineType			0x0017
#define P_IgnoreExportLib		0x0018
#define P_LazyPdb				0x0019
#define P_LibPath				0x001a
// 0x001b - Add P_LibPath				
// 0x001c - Subtract P_LibPath				
#define P_Linker_Input_Ext		0x001d
#define P_LinkDelayLoadDLL		0x001e
// 0x001f - Add P_LinkDelayLoadDLL				
// 0x0020 - Subtract P_LinkDelayLoadDLL				
#define P_LinkDelayNoBind		0x0021
#define P_LinkDelayUnLoad		0x0022
#define P_LinkUnknownOption		0x0023
#define P_LinkUnknownString		0x0024
#define P_ResOnlyDLL			0x0025
#define P_BaseAddress			0x0026
#define P_VirtualDeviceDriver	0x0027
#define P_Link50Compat			0x0028
#define P_Align					0x0029
#define P_SetCheckSum			0x002a
#define P_MergeSections			0x002b
#define P_OrderFuncs			0x002c
#define P_TrimProcMem			0x002d
#define P_LargeAddress			0x002e
#define P_Fixed					0x002f
#define P_BaseAddr				0x0030
#define P_ExeDynamic			0x0031
#define P_Driver				0x0032
#define P_PreventBind			0x0033


// linker tool options (option ids)
#define IDOPT_LINKNOLOGO				IDOPT_BASE + 9
#define IDOPT_LINKVERBOSE				IDOPT_BASE + 26
#define IDOPT_MAPGEN					IDOPT_BASE + 27
#define IDOPT_DEBUG						IDOPT_BASE + 2
#define IDOPT_DEBUGTYPE					IDOPT_BASE + 3
#define IDOPT_EXALLLIB					IDOPT_BASE + 4
#define IDOPT_DEFNAME					IDOPT_BASE + 5
#define IDOPT_LIBPATH					IDOPT_BASE + 6
#define IDOPT_FORCE						IDOPT_BASE + 7
#define IDOPT_OUT						IDOPT_BASE + 8
#define IDOPT_NOLOGO					IDOPT_BASE + 9
#define IDOPT_NODEFAULTLIB				IDOPT_BASE + 10
#define IDOPT_INCLUDESYM				IDOPT_BASE + 11
#define IDOPT_IMPLIB					IDOPT_BASE + 12
#define IDOPT_DLLGEN					IDOPT_BASE + 13
#define IDOPT_INCREMENTALLINK			IDOPT_BASE + 14
#define IDOPT_USEPDB					IDOPT_BASE + 15
#define IDOPT_USEPDBLINK				IDOPT_BASE + 16
#define IDOPT_USEPDBNONE				IDOPT_BASE + 17
#define IDOPT_LAZYPDB					IDOPT_BASE + 18
#define IDOPT_PROFILE					IDOPT_BASE + 19
#define IDOPT_MACHINETYPE				IDOPT_BASE + 20
#define IDOPT_IGNOREEXPORTLIB			IDOPT_BASE + 21
#define IDOPT_LINKER_INPUT_EXT			IDOPT_BASE + 22
#define IDOPT_DELAY_LOAD_DLL			IDOPT_BASE + 23
#define IDOPT_DELAY_NOBIND				IDOPT_BASE + 24
#define IDOPT_DELAY_UNLOAD				IDOPT_BASE + 25
#define IDOPT_RESONLY_DLL				IDOPT_BASE + 28
#define IDOPT_BASE_ADDRESS				IDOPT_BASE + 29
#define IDOPT_VIRTUAL_DEVICE_DRIVER		IDOPT_BASE + 30
#define IDOPT_50COMPAT					IDOPT_BASE + 31
#define IDOPT_ALIGN						IDOPT_BASE + 32
#define IDOPT_SET_CHECKSUM				IDOPT_BASE + 33
#define IDOPT_MERGE_SECTIONS			IDOPT_BASE + 34
#define IDOPT_ORDER_FUNCS				IDOPT_BASE + 35
#define IDOPT_TRIM_PROC_MEM				IDOPT_BASE + 36
#define IDOPT_LARGEADDRESS				IDOPT_BASE + 37
#define IDOPT_FIXED						IDOPT_BASE + 38
#define IDOPT_EXE_BASE					IDOPT_BASE + 39
#define IDOPT_EXE_DYNAMIC				IDOPT_BASE + 40	
#define IDOPT_DRIVER					IDOPT_BASE + 41	
#define IDOPT_PREVENT_BIND				IDOPT_BASE + 42		

//----------------------------------------------------------------
// out NT linker option data
//----------------------------------------------------------------

// option handler
DEFN_OPTHDLR_PLATFORM
(
	LinkerNT, /* name */
	szAddOnGeneric, BCID_OptHdlr_LinkerNT, /* generic NT linker option handler */
	szAddOnGeneric, BCID_Tool_LinkerNT, /* generic NT linker tool */
	szAddOnGeneric, BCID_OptHdlr_Linker /* base generic linker option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// should this be shown always?
	virtual BOOL AlwaysShowDefault(UINT idProp);

	// provide invalidation of option props
	virtual BOOL CheckDepOK(UINT idProp);

END_OPTHDLR()

// NT linker tool properties
#define P_VersionMaj			0x1000
#define P_VersionMin			0x1001
#define P_StackReserve			0x1002
#define P_StackCommit			0x1003
#define P_EntryName				0x1004
#define P_SubSystem				0x1005
#define P_HeapReserve			0x1006
#define P_HeapCommit			0x1007

// NT linker tool options (option ids)
#define IDOPT_VERSION					5000
#define IDOPT_STACK						5001
#define IDOPT_ENTRYPOINT				5002
#define IDOPT_SUBSYSTEM					5003
#define IDOPT_HEAP						5004

#include "schmztl.h"			// the CSchmoozeTool class

class  CLinkerTool : public CSchmoozeTool
{
	DECLARE_DYNAMIC (CLinkerTool)

public:
	CLinkerTool();

	//	Ensure that the tool will build an MFC project item.
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC);
};

class  CLinkerNTTool : public CLinkerTool
{
	DECLARE_DYNAMIC (CLinkerNTTool)

public:
	CLinkerNTTool() : CLinkerTool() {}

	//	Ensure that the tool will build an MFC project item.
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC);
};

