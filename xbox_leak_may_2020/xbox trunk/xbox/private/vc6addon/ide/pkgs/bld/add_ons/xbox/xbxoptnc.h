//
// Xbox Compiler Tool Options
//
// [matthewt]
//

#ifndef _INCLUDE_OPTNCPLR_XBOX_H
#define _INCLUDE_OPTNCPLR_XBOX_H

#include "projprop.h"			// our option property ids
#include "prjoptn.h"			// macros used to decl/defn our tables
#include "..\..\ide\pkgs\bld\src\optncplr.h"	// our base option handler, the Common Compiler

//-----------------------------------------------
// our option property page
//-----------------------------------------------

extern CRuntimeClass * g_cplrXboxTabs[];

//----------------------------------------------------------------
// our Xbox compiler option data
//----------------------------------------------------------------

// Xbox compiler option handler
DEFN_OPTHDLR_PLATFORM
(
	CompilerXbox, /* name */
	szAddOnXbox, BCID_OptHdlr_Compiler_Xbox, /* Xbox compiler option handler */
	szAddOnXbox, BCID_Tool_Compiler_Xbox, /* Xbox compiler tool */
	szAddOnGeneric, BCID_OptHdlr_Compiler /* base generic compiler option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// our option pages
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_cplrXboxTabs;}

	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);

	// return a 'fake' int prop
	virtual GPT GetDefIntProp(UINT idProp, int & nVal);

private:
	// form a 'fake' string prop
	virtual void FormFakeIntProp(UINT idProp, int & nVal);

END_OPTHDLR()

// Xbox compiler tool properties
#define P_CallConv						0x1000
#define P_ByteAlign				   		0x1001
#define P_FloatPoint					0x1002
#define P_Thread						0x1003

// Xbox compiler tool options (option ids)
#define IDOPT_CPUX86					IDOPT_BASE + 0
#define IDOPT_CALLCONV					IDOPT_BASE + 1
#define IDOPT_BYTE						IDOPT_BASE + 2
#define IDOPT_FLOATPOINT				IDOPT_BASE + 3
#define IDOPT_THREAD					IDOPT_BASE + 4

class CCompilerXboxPage : public CCompilerPageTab
{
	DECLARE_DYNCREATE(CCompilerXboxPage)
	DECLARE_IDE_CONTROL_MAP()
};

#endif // _INCLUDE_OPTNCPLR_XBOX_H


