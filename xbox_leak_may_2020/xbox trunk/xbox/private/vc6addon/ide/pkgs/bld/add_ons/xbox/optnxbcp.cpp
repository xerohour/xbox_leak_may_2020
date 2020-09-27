// 
// Xbcp Tool Options
//
// [colint]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "optnxbcp.h"	// our local header file

#ifdef XBCP

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


BEGIN_OPTSTR_TABLE(Xbcp, P_XbcpUnknownOption, P_XbcpUnknownString, P_XbcpUnknownOption, P_XbcpUnknownString, FALSE)
	IDOPT_UNKNOWN_STRING,		"",		NO_OPTARGS,			single,
	IDOPT_UNKNOWN_OPTION,		"",		NO_OPTARGS,
	single,
	IDOPT_XBCP_NOLOGO,		"NOLOGO",	NO_OPTARGS,  			single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(Xbcp)
END_OPTDEF_MAP()

CRuntimeClass * g_XbcpTabs[] =
{
	(CRuntimeClass *)NULL,
};


void OPTION_HANDLER(Xbcp)::ResetPropsForConfig(ConfigurationRecord * pcr)
{
	// do the base-class thing first
	COptionHandler::ResetPropsForConfig(pcr);

	// reset our 'deferred' prop.
	pcr->BagCopy(BaseBag, CurrBag, P_DeferredMecr, P_DeferredMecr, TRUE);
}

BOOL OPTION_HANDLER(Xbcp)::CanResetPropsForConfig(ConfigurationRecord * pcr)
{
	// do the base-class thing first
	if (COptionHandler::CanResetPropsForConfig(pcr))
		return TRUE;	// no need to check anymore

	// check our 'deferred' prop.
	return !pcr->BagSame(BaseBag, CurrBag, P_DeferredMecr, P_DeferredMecr, TRUE);
}

#endif
