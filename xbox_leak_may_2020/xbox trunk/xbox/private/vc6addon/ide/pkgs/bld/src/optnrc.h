//
// Resource Compiler Tool Options
//
// [matthewt]
//

#ifndef _INCLUDE_OPTNRC_H
#define _INCLUDE_OPTNRC_H

#include "projprop.h"	// our option property ids
#include "prjoptn.h"	// macros used to decl/defn our tables
#include "optnrc.h2"	// our option control IDs

//----------------------------------------------------------------
// our resource compiler tool option property pages
//----------------------------------------------------------------

class BLD_IFACE CResCompilerPage : public COptionMiniPage
{
	DECLARE_DYNCREATE(CResCompilerPage)
public:
	virtual BOOL Validate();
private:
	static const TCHAR *strOutputExtension ;
	
	// get the output extension
	virtual const TCHAR *GetOutputExtension() {return strOutputExtension;};

};

//----------------------------------------------------------------
// our 'General' option property page
//----------------------------------------------------------------

class BLD_IFACE CResCompilerGeneralPage : public CResCompilerPage
{
	DECLARE_DYNCREATE(CResCompilerGeneralPage)
	DECLARE_IDE_CONTROL_MAP()
public:
	virtual void InitPage() ; 
	virtual BOOL OnPropChange(UINT nProp) ;
};

//----------------------------------------------------------------
// our option property pages
//----------------------------------------------------------------

extern CRuntimeClass * g_rcTabs[];

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
	// our option pages
	virtual UINT GetNameID() {return IDS_RESCOMPILER32;}
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_rcTabs;}

	// check deps.
	virtual BOOL CheckDepOK(UINT idProp);

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

	int GetTabDlgOrder()
		{ return 50; }
		
private:
	// form a 'fake' string prop
	virtual void FormFakeIntProp(UINT idProp, int & nVal);
	virtual void FormFakeStrProp(UINT idProp, CString & strVal);

	BOOL m_fEnableLangID;

END_OPTHDLR()

// resource compiler properties
#define P_ResVerbose					 0x0000
#define P_ResIgnIncPath					 0x0001
#define P_ResIncludes					 0x0002
// 0x0003 - Add P_ResIncludes
// 0x0004 - Subtract P_ResIncludes
#define P_OutNameRes					 0x0005
#define P_ResMacroNames					 0x0006
// 0x0007 - Add P_ResMacroNames
// 0x0008 - Subtract P_ResMacroNames
#define P_ResLangID						 0x0009
#define P_ResUnknownOption				 0x000a
#define P_ResUnknownString				 0x000b

// resource compiler options (option ids)
#define IDOPT_RESVERBOSE				IDOPT_BASE + 0
#define IDOPT_RESIGNINC					IDOPT_BASE + 1
#define IDOPT_RESMACROS					IDOPT_BASE + 2
#define IDOPT_RESINCLUDES				IDOPT_BASE + 3
#define IDOPT_OUTDIR_RES				IDOPT_BASE + 4
#define IDOPT_RESLANGID					IDOPT_BASE + 5

class BLD_IFACE CEnumLangID : public CEnum
{
public:
	virtual void FillComboBox(CComboBox* pWnd, BOOL bClear = TRUE, CSlob* pSlob = NULL);
};

extern BLD_IFACE CEnumLangID LangIDEnum;

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

#endif // _INCLUDE_OPTNRC_H
