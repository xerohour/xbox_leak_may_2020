//
// Xbox Linker Tool Options
//
// [colint]
//

#ifndef _INCLUDE_OPTNLINK_XBOX_H
#define _INCLUDE_OPTNLINK_XBOX_H

#include "projprop.h"			// our option property ids
#include "prjoptn.h"			// macros used to decl/defn our tables
#include "..\..\ide\pkgs\bld\src\optnlink.h"	// our base option handler, the NT Linker 

//-----------------------------------------------
// our ption property page
//-----------------------------------------------

extern CRuntimeClass * g_linkXboxExeTabs[];
extern CRuntimeClass * g_linkXboxDllTabs[];

//----------------------------------------------------------------
// our Xbox Exe linker option data
//----------------------------------------------------------------

// someday we'll subclass the core linker's option handler to make
// GetFakeDirProp work right in our cases

// Xbox linker option handler
DEFN_OPTHDLR_PLATFORM
(
	LinkerXboxExe, /* name */
	szAddOnXbox, BCID_OptHdlr_Linker_XboxExe, /* Xbox linker option handler */
	szAddOnXbox, BCID_Tool_Linker_XboxExe, /* Xbox linker tool */
	szAddOnGeneric, BCID_OptHdlr_Linker /* base generic linker option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// our option pages
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_linkXboxExeTabs;}

	UINT GetFakePathDirProp(UINT idProp);
#if 0
	virtual GPT GetDefStrProp(UINT idProp, CString &val);
#endif

END_OPTHDLR()

#if 0
#define P_XbeName				0x1000

#define IDOPT_XBENAME           IDOPT_BASE

class CLinkerXboxExePage : public CLinkerPageTab
{
	DECLARE_DYNCREATE(CLinkerXboxExePage)
	DECLARE_IDE_CONTROL_MAP()
};
#endif

//----------------------------------------------------------------
// our Xbox Dll/Lib linker option data
//----------------------------------------------------------------

#ifdef XBOXDLL
// Xbox compiler option handler
DEFN_OPTHDLR_PLATFORM
(
	LinkerXboxDll, /* name */
	szAddOnXbox, BCID_OptHdlr_Linker_XboxDll, /* Xbox linker option handler */
	szAddOnXbox, BCID_Tool_Linker_XboxDll, /* Xbox linker tool */
	szAddOnGeneric, BCID_OptHdlr_Linker /* base generic linker option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()
#if 0
public:
	// our option pages
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_linkXboxDllTabs;}
#endif

END_OPTHDLR()
#endif

#endif // _INCLUDE_OPTNCPLR_XBOX_H
