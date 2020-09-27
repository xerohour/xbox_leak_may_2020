// Make Type Library Tool Options
//
// [ignatius]
// everything is still bogus, just want to check if it works fine
// or not

#ifndef _INCLUDE_OPTNMTL_H
#define _INCLUDE_OPTNMTL_H

#include "projprop.h"  // our option property ids
#include "prjoptn.h" // macros used to decl/defn our tables
#include "optnmtl.h2" // our option control IDs

//----------------------------------------------------------------
// our make type library tool option property pages
//----------------------------------------------------------------

class CMkTypLibPageTab : public COptionMiniPage
{
	DECLARE_DYNCREATE(CMkTypLibPageTab)
public:
	virtual BOOL Validate();
};

//----------------------------------------------------------------
// our 'General' option property page
//----------------------------------------------------------------

extern CRuntimeClass * g_cplrTabs[];

class CMkTypLibGeneralPage : public CMkTypLibPageTab
{
	DECLARE_DYNCREATE(CMkTypLibGeneralPage)
	DECLARE_IDE_CONTROL_MAP()
public:
	virtual void InitPage();
};

//----------------------------------------------------------------
// our option property pages
//----------------------------------------------------------------

extern CRuntimeClass * g_mtlTabs[];

//----------------------------------------------------------------
// our mktyplib option data
//----------------------------------------------------------------

// option handler
DEFN_OPTHDLR_COMMON
(
	MkTypLib, /* name */
	szAddOnGeneric, BCID_OptHdlr_MkTypLib, /* generic mktyplib option handler */
	szAddOnGeneric, BCID_Tool_MkTypLib /* our generic associated mktyplib tool */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// our option page
	virtual UINT GetNameID() {return IDS_MKTYPLIB;}
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_mtlTabs;}

	// is this a 'fake' prop?
	virtual BOOL IsFakeProp (UINT idProp);

	// return a 'fake' string prop
	virtual GPT GetDefStrProp (UINT idProp, CString & strVal);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

	// called whenever a prop has changed
	// we can use this to delete a prop if we want the fake prop instead
	virtual void OnOptionStrPropChange(UINT idProp, const CString & strVal);

	// determine the validity of the property
	virtual BOOL CheckDepOK (UINT idProp);

	int GetTabDlgOrder() {return 60;}

private:
	// form a 'fake' string prop
	virtual void FormFakeStrProp (UINT idProp, CString & strVal);

END_OPTHDLR()

// MkTypLib properties
#define P_MTLNologo				0x0000
#define P_MTLIncludes			0x0001
// 0x0002 - P_MTLIncludes Add Part
// 0x0003 - P_MTLIncludes Substract Part
#define P_MTLMacros				0x0004
// 0x0005 - P_MTLMacros Add Part
// 0x0006 - P_MTLMacros Subtract Part
#define P_MTLOutputTlb			0x0007
#define P_MTLOutputInc			0x0008
#define P_MTLOutputUuid			0x0009
#define P_MTLOutputDir			0x000a
#define P_MTLMtlCompatible      0x000b
#define P_MTLOicf		    	0x000c
#define P_MTLNoClutter		    0x000d
#define P_MTLUnknownOption		0x000e
#define P_MTLUnknownString		0x000f

// MkTypLib options (option ids)				   
#define IDOPT_MTLNOLOGO			IDOPT_BASE + 0
#define IDOPT_MTLINCLUDES 		IDOPT_BASE + 1
#define IDOPT_MTLIGNINC			IDOPT_BASE + 2
#define IDOPT_MTLMACROS			IDOPT_BASE + 3
#define IDOPT_MTLOUTPUTTLB		IDOPT_BASE + 4
#define IDOPT_MTLOUTPUTINC		IDOPT_BASE + 5
#define IDOPT_MTLOUTPUTUUID		IDOPT_BASE + 6
#define IDOPT_MTLMTLCOMPATIBLE	IDOPT_BASE + 7
#define IDOPT_MTLOICF   		IDOPT_BASE + 8
#define IDOPT_MTLOUTPUTDIR		IDOPT_BASE + 9
#define IDOPT_MTLNOCLUTTER   	IDOPT_BASE + 10

#endif // _INCLUDE_OTPTNMTL_H
