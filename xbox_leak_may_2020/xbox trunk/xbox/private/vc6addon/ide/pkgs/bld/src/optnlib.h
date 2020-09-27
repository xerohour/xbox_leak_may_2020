//
// Library Manager Tool Options
//
// [matthewt]
//

#ifndef _INCLUDE_OPTNLIB_H
#define _INCLUDE_OPTNLIB_H

#include "projprop.h"	// our option property ids
#include "prjoptn.h"	// macros used to decl/defn our tables
#include "optnlib.h2"	// our option control IDs

//----------------------------------------------------------------
// our library manager tool option property pages
//----------------------------------------------------------------

class CLibPageTab : public COptionMiniPage
{
	DECLARE_DYNCREATE(CLibPageTab)
public:
	virtual BOOL Validate();
};

//----------------------------------------------------------------
// our 'General' option property page
//----------------------------------------------------------------

class CLibGeneralPage : public CLibPageTab
{
	DECLARE_DYNCREATE(CLibGeneralPage)
	DECLARE_IDE_CONTROL_MAP()
};

//----------------------------------------------------------------
// our option property pages
//----------------------------------------------------------------

extern CRuntimeClass * g_libTabs[];

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
	// our option pages
	virtual UINT GetNameID() {return IDS_LIBMGR;}
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_libTabs;}

	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);

	// return a 'fake' string prop
	virtual GPT GetDefStrProp(UINT idProp, CString & strVal);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

	int GetTabDlgOrder()
		{ return 40; }

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

// lib tool options (option ids)
#define IDOPT_OUTLIB					IDOPT_BASE + 0
#define IDOPT_DEFLIB					IDOPT_BASE + 1
#define IDOPT_LIBNOLOGO					IDOPT_BASE + 2

#endif // _INCLUDE_OPTNLIB_H
