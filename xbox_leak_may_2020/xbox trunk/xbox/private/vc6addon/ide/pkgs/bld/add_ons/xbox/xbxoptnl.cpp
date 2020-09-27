//
// Xbox Linker Tool Options
//
// [colint]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "xbxoptnl.h"	// our local header file
#include "xbxtooll.h"

//----------------------------------------------------------------
// our Xbox Exe linker option strings
//----------------------------------------------------------------

#if 0
BEGIN_OPTSTR_TABLE(LinkerXboxExe, (UINT)NULL, (UINT)NULL, P_XbeName, P_XbeName, TRUE)
	IDOPT_XBENAME,		"xbe:out:%1",				OPTARGS1(P_XbeName),	single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerXboxExe)
	OPTDEF_STRING(XbeName, "")
END_OPTDEF_MAP()

IMPLEMENT_DYNCREATE(CLinkerXboxExePage, CLinkerPageTab)

CRuntimeClass * g_linkXboxExeTabs[] =
{
	RUNTIME_CLASS(CLinkerXboxExePage),
	(CRuntimeClass *)NULL,
};

BEGIN_IDE_CONTROL_MAP(CLinkerXboxExePage, IDDP_LINKER_XBOXEXE, IDS_CAT_XBEBUILD)
	MAP_EDIT(IDC_XBENAME, P_XbeName)
END_IDE_CONTROL_MAP()

GPT OPTION_HANDLER(LinkerXboxExe)::GetDefStrProp(UINT idProp, CString &val)
{
	GPT gpt;
	UINT nProp = MapActual(idProp);

	if(nProp == P_XbeName)
	{
		UINT idPropNameActual;
		COptionHandler *popthdlr = this;

		/* Figure out which option handler owns P_OutName */
		while(popthdlr->GetDepth() != OHGetDepth(P_OutName))
		{
			popthdlr = popthdlr->GetBaseOptionHandler();
			ASSERT(popthdlr);
			if(!popthdlr)
				return invalid;
		}
		/* Now get the actual prop id for the output name */
		idPropNameActual = popthdlr->MapLogical(P_OutName);

		/* Finally, get the output name */
		CString strOut;
		if(valid != m_pSlob->GetStrProp(popthdlr->MapLogical(P_OutName),
			strOut))
		{
			ASSERT(FALSE);
			val = "";
			return valid;
		}

		/* Turn the name from .exe into .xbe */
		GetXbeFromExe(strOut, val);
		return valid;
	} else {
		ASSERT(FALSE);
		return invalid;
	}
}
#endif

//----------------------------------------------------------------
// our Xbox Dll linker option strings
//----------------------------------------------------------------
#ifdef XBOXDLL
BEGIN_OPTSTR_TABLE(LinkerXboxDll, (UINT)NULL, (UINT)NULL, 0, 0, TRUE)
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerXboxDll)
END_OPTDEF_MAP()

CRuntimeClass * g_linkXboxDllTabs[] =
{
	(CRuntimeClass *)NULL,
};
#endif

BEGIN_OPTSTR_TABLE(LinkerXboxExe, (UINT)NULL, (UINT)NULL, (UINT)NULL, (UINT)NULL, TRUE)
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerXboxExe)
END_OPTDEF_MAP()

CRuntimeClass * g_linkXboxExeTabs[] =
{
	(CRuntimeClass *)NULL,
};

UINT OPTION_HANDLER(LinkerXboxExe)::GetFakePathDirProp(UINT idProp)
{
	COptionHandler *popthdlrLinker = GetBaseOptionHandler();
	ASSERT(popthdlrLinker);
	/* Find out which linker property we're dealing with */
	UINT idPropL = popthdlrLinker->MapActual(idProp);

	/* For us, the .exe is an intermediate file */
	if (idPropL == P_OutName)
		return P_OutDirs_Intermediate;

	return popthdlrLinker->GetFakePathDirProp(idProp);
} 
