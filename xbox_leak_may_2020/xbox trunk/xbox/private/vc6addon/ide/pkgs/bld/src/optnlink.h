//
// Linker Tool Options
//
// [matthewt]
//

#ifndef _INCLUDE_OPTNLINK_H
#define _INCLUDE_OPTNLINK_H

#include "projprop.h"	// our option property ids
#include "prjoptn.h"	// macros used to decl/defn our tables
#include "optnlink.h2"	// our option control IDs

//----------------------------------------------------------------
// our linker tool option property pages
//----------------------------------------------------------------

class BLD_IFACE CLinkerPageTab : public COptionMiniPage
{
	DECLARE_DYNCREATE(CLinkerPageTab)
public:
	virtual BOOL Validate();
};

//----------------------------------------------------------------
// our 'General' option property page
//----------------------------------------------------------------

class CLinkerGeneralPage : public CLinkerPageTab
{
	DECLARE_DYNCREATE(CLinkerGeneralPage)
	DECLARE_IDE_CONTROL_MAP()
public:
	// special handling of property changes
	virtual BOOL OnPropChange(UINT);
};

//----------------------------------------------------------------
// our option property pages
//----------------------------------------------------------------

extern CRuntimeClass * g_linkTabs[];

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
	// our option pages
	virtual UINT GetNameID() {return IDS_LINKCOFF;}
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_linkTabs;}

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
	virtual void OnOptionStrPropChange(UINT idProp, const CString & strVal);
	virtual void OnOptionIntPropChange(UINT idProp, int nVal);

	int GetTabDlgOrder()
		{ return 40; }
		
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
#define P_LinkUnknownOption		0x001d
#define P_LinkUnknownString		0x001e


// linker tool options (option ids)
#define IDOPT_LINKNOLOGO				IDOPT_BASE + 9
#define IDOPT_LINKVERBOSE				IDOPT_BASE + 0
#define IDOPT_MAPGEN					IDOPT_BASE + 1
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

//
// linker tool 'Advanced' option tabs
//

class CLinkerDebugPage : public CLinkerPageTab
{
	DECLARE_DYNCREATE(CLinkerDebugPage)
	DECLARE_IDE_CONTROL_MAP()
public:
	// special handling of property changes
	virtual BOOL OnPropChange(UINT);
	virtual BOOL Validate();
};

class CLinkerCustomPage : public CLinkerPageTab
{
	DECLARE_DYNCREATE(CLinkerCustomPage)
	DECLARE_IDE_CONTROL_MAP()
public:
	// special handling of property changes
	virtual BOOL OnPropChange(UINT);
	virtual BOOL Validate();
};

class CLinkerInputPage : public CLinkerPageTab
{
	DECLARE_DYNCREATE(CLinkerInputPage)
	DECLARE_IDE_CONTROL_MAP()
public:
	virtual void InitPage();
};

//----------------------------------------------------------------
// our NT Linker property pages
//----------------------------------------------------------------

extern CRuntimeClass * g_linkNTTabs[];

//----------------------------------------------------------------
// our 'Win32 Output' option property page
//----------------------------------------------------------------

class CLinkerWin32OutputPage : public CLinkerPageTab
{
	DECLARE_DYNCREATE(CLinkerWin32OutputPage)
	DECLARE_IDE_CONTROL_MAP()
public:
	// special handling of property changes
	virtual BOOL OnPropChange(UINT);
};

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
	// out option pages
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_linkNTTabs;}

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
#define P_BaseAddr				0x1004
#define P_EntryName				0x1005
#define P_SubSystem				0x1006

// NT linker tool options (option ids)
#define IDOPT_VERSION					IDOPT_BASE + 0
#define IDOPT_STACK						IDOPT_BASE + 1
#define IDOPT_EXE_BASE					IDOPT_BASE + 2
#define IDOPT_ENTRYPOINT				IDOPT_BASE + 3
#define IDOPT_SUBSYSTEM					IDOPT_BASE + 4

#endif // _INCLUDE_OPTN_LINK_H
