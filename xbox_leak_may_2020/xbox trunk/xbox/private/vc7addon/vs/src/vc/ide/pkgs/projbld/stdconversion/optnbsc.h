//
// Browser Database Make Tool Options and Tool
//

#pragma once

#include "schmztl.h"			// the CSchmoozeTool class

//----------------------------------------------------------------
// our COMMON bscmake option data
//----------------------------------------------------------------

// option handler
DEFN_OPTHDLR_COMMON
(
	BscMake, /* name */
	szAddOnGeneric, BCID_OptHdlr_BscMake, /* generic bscmake option handler */
	szAddOnGeneric, BCID_Tool_BscMake /* generic bscmake tool */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);

	// return a 'fake' string/int prop
	virtual GPT GetDefStrProp(UINT idProp, CString & val);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

	// handle setting of option string props
	virtual void OnOptionStrPropChange(UINT idProp, const CString & strVal);

private:
	// form a 'fake' string prop
	virtual void FormFakeStrProp(UINT idProp, CString & strVal);

END_OPTHDLR()

class CBscmakeTool : public CSchmoozeTool
{
	DECLARE_DYNAMIC (CBscmakeTool)

public:
	CBscmakeTool();
};

// bscmake tool properties
#define P_BscMakeUnknownOption				0x0000
#define P_BscMakeUnknownString				0x0001
#define P_BscMakeOutputFilename				0x0002
#define P_InclUnref							0x0003
#define P_BscMakeNoLogo						0x0004

// bscmake tool options (option ids)
#define IDOPT_BSCMAKE_OUTNAME				IDOPT_BASE + 0
#define IDOPT_BSCMAKE_INCUNREF				IDOPT_BASE + 1
#define IDOPT_BSCMAKE_NOLOGO				IDOPT_BASE + 2
