//
// Library Manager Tool Options and Tool
//

#pragma once

//----------------------------------------------------------------
// our COMMON lib option data
//----------------------------------------------------------------

// option handler
DEFN_OPTHDLR_COMMON
(
	Lib, /* name */
	szAddOnGeneric, BCID_OptHdlr_Lib, /* generic library manager option handler */
	szAddOnGeneric, BCID_Tool_Lib /* generic library manager tool */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);

	// return a 'fake' string prop
	virtual GPT GetDefStrProp(UINT idProp, CString & strVal);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

private:
	// called whenever a prop has changed
	virtual void OnOptionStrPropChange(UINT idProp, const CString & strVal);

	// form a 'fake' string prop
	virtual void FormFakeStrProp(UINT idProp, CString & strVal);

END_OPTHDLR()

// lib tool properties

#include "optnlink.h"

#define P_LibUnknownString				0x0000
#define P_LibUnknownOption				0x0001
#define P_NoLogoLib						0x0002
#define P_OutNameLib					0x0003
#define P_DefNameLib					0x0004
#define P_Lib_Input_Ext					0x0005

// lib tool options (option ids)
#define IDOPT_OUTLIB					IDOPT_BASE + 0
#define IDOPT_DEFLIB					IDOPT_BASE + 1
#define IDOPT_LIBNOLOGO					IDOPT_BASE + 2
#define IDOPT_LIB_INPUT_EXT				IDOPT_BASE + 3

///////////////////////////////////////////////////////////////////////////////
// CLibTool is derived from CLinkerTool. There are only minor differences
// such as different option handler, name of tool etc.
///////////////////////////////////////////////////////////////////////////////

class  CLibTool : public CLinkerTool
{
	DECLARE_DYNAMIC (CLibTool)

public:
	CLibTool();
};
