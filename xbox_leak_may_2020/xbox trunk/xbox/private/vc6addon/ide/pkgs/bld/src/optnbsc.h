//
// Browser Database Make Tool Options
//
// [matthewt]
//

#ifndef _INCLUDE_OPTNBSC_H
#define _INCLUDE_OPTNBSC_H

#include "projprop.h"	// our option property ids
#include "prjoptn.h"	// macros used to decl/defn our tables
#include "optnbsc.h2"	// our option control IDs

#include "optncplr.h"	// our prop. ID for /Zn so we can link /Iu to it

//----------------------------------------------------------------
// our browser database make tool option property pages
//----------------------------------------------------------------

class CBscMakePageTab : public COptionMiniPage
{
	DECLARE_DYNCREATE(CBscMakePageTab)
public:
	virtual BOOL Validate();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

//----------------------------------------------------------------
// our 'General' option property page
//----------------------------------------------------------------

class CBscMakeGeneralPage : public CBscMakePageTab
{
	DECLARE_DYNCREATE(CBscMakeGeneralPage)
	DECLARE_IDE_CONTROL_MAP()
};

//----------------------------------------------------------------
// our option property pages
//----------------------------------------------------------------

extern CRuntimeClass * g_bscTabs[];

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
	// our option page
	virtual UINT GetNameID() {return IDS_BSCMAKE;}
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_bscTabs;}

	// reset our 'deferred' props (eg. On demand bscmake)
 	virtual void ResetPropsForConfig(ConfigurationRecord * pcr);
 	virtual BOOL CanResetPropsForConfig(ConfigurationRecord * pcr);

	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);

	// return a 'fake' string/int prop
	virtual GPT GetDefStrProp(UINT idProp, CString & val);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

	// handle setting of option string props
	virtual void OnOptionStrPropChange(UINT idProp, const CString & strVal);

	int GetTabDlgOrder()
		{ return 70; }

private:
	// form a 'fake' string prop
	virtual void FormFakeStrProp(UINT idProp, CString & strVal);

END_OPTHDLR()

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

#endif // _INCLUDE_OPTNBSC_H
