/*
 *
 * optnxbe.h
 *
 * Option handlers for the Xbe Builder tool
 *
 */

#include "projprop.h"
#include "prjoptn.h"

extern CRuntimeClass *g_XbeBuildTabs[]; 

DEFN_OPTHDLR_COMMON
(
	XbeBuilder, /* name */
	szAddOnXbox, BCID_OptHdlr_XbeBuilder, /* Xbox linker option handler */
	szAddOnXbox, BCID_Tool_XbeBuilder /* Xbox linker tool */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	virtual UINT GetNameID() {return IDS_XBEBUILD_TOOL_UI;}
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_XbeBuildTabs;}

	virtual BOOL IsFakeProp(UINT idProp);
	virtual void FormFakeStrProp(UINT idProp, CString &strVal);
	virtual GPT GetDefStrProp(UINT idProp, CString & val);
	virtual UINT GetFakePathDirProp(UINT idProp);

#ifdef XBCP
	virtual BOOL CanResetPropsForConfig(ConfigurationRecord * pcr);
	virtual void ResetPropsForConfig(ConfigurationRecord * pcr);
#endif

	int GetTabDlgOrder()
		{ return 41; }
END_OPTHDLR()

// P_XbeName MUST be first, and it MUST be zero!
#define P_XbeName	    0x0000
#define P_XbeDebug	    0x0001
#define P_XbeTitleId    0x0002
#define P_XbeTitleName  0x0003
#define P_XbePubName    0x0004  // removed
#define P_XbeStack	    0x0005
#define P_XbeLimitMem   0x0006
#define P_XbeTitleImage 0x0007
#define P_XbeTitleInfo  0x0008
#define P_XbeSaveImage  0x0009
#define P_XbeInitFlags  0x000a
#define P_XbeRegion     0x000b
#define P_XbeRating     0x000c
#define P_UnknownOption 0x003e
#define P_UnknownString 0x003f


#define IDOPT_XBENAME	    IDOPT_BASE + 0
#define IDOPT_XBEDEBUG	    IDOPT_BASE + 1
#define IDOPT_TITLEID	    IDOPT_BASE + 2
#define IDOPT_TITLENAME	    IDOPT_BASE + 3
#define IDOPT_PUBNAME	    IDOPT_BASE + 4
#define IDOPT_XESTACK	    IDOPT_BASE + 5
#define IDOPT_XBENOLOGO	    IDOPT_BASE + 6
#define IDOPT_LIMITMEM      IDOPT_BASE + 7
#define IDOPT_TITLEINFO     IDOPT_BASE + 8
#define IDOPT_SAVEIMAGE     IDOPT_BASE + 9
#define IDOPT_TITLEIMAGE    IDOPT_BASE + 10
#define IDOPT_XEFLAGS       IDOPT_BASE + 11
#define IDOPT_XBEREGION     IDOPT_BASE + 12
#define IDOPT_XBERATING     IDOPT_BASE + 13

class CXbeBuilderPageTab : public COptionMiniPage
{
	DECLARE_DYNCREATE(CXbeBuilderPageTab)
public:
	// inherit validation for now
	virtual BOOL Validate();
};

class CXbeBuilderGeneralPage : public CXbeBuilderPageTab
{
	DECLARE_DYNCREATE(CXbeBuilderGeneralPage)
	DECLARE_IDE_CONTROL_MAP()
};

class CXbeBuilderTitlePage : public CXbeBuilderPageTab
{
	DECLARE_DYNCREATE(CXbeBuilderTitlePage)
	DECLARE_IDE_CONTROL_MAP()
};

class CXbeBuilderCertificatePage : public CXbeBuilderPageTab
{
	DECLARE_DYNCREATE(CXbeBuilderCertificatePage)
	DECLARE_IDE_CONTROL_MAP()
};
