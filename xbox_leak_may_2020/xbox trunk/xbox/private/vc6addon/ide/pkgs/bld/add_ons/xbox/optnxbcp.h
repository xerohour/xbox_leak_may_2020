//
// Macintosh Xbcp Tool Options
//
// [colint]
//

#ifndef _INCLUDE_OPTNXBCP_H
#define _INCLUDE_OPTNXBCP_H

#ifdef XBCP

#include "projprop.h"	// our option property ids
#include "prjoptn.h"	// macros used to decl/defn our tables

//----------------------------------------------------------------
// our option property pages
//----------------------------------------------------------------

extern CRuntimeClass * g_XbcpTabs[];

//----------------------------------------------------------------
// our COMMON Macintosh Xbcp option data
//----------------------------------------------------------------

// option handler
DEFN_OPTHDLR_COMMON
(
	Xbcp, /* name */
	szAddOnXbox, BCID_OptHdlr_Xbcp, /* Mac68k mecr option handler */
	szAddOnXbox, BCID_Tool_Xbcp /* Mac68k mecr tool */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// our option pages
	virtual UINT GetNameID() {return IDS_XBCP_TOOL;}
	virtual CRuntimeClass * * GetOptionPages(BOOL & fUseBasePages) {return g_XbcpTabs;}

 	// reset our 'deferred' props (eg. On demand remote file copy to host)
 	virtual void ResetPropsForConfig(ConfigurationRecord * pcr);
 	virtual BOOL CanResetPropsForConfig(ConfigurationRecord * pcr);

END_OPTHDLR()

// Xbcp Properties
#define P_XbcpUnknownOption			0x0000
#define P_XbcpUnknownString			0x0001

// Macintosh Xbcp options (option ids)
#define IDOPT_XBCP_NOLOGO			IDOPT_BASE + 0

#endif // XBCP

#endif // _INCLUDE_OPTNXBCP_H
