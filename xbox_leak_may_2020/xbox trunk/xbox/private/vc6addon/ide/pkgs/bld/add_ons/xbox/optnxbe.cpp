/*
 *
 * optnxbe.cpp
 *
 * Option handlers for the image builder tool
 *
 */

#include "stdafx.h"
#pragma hdrstop
#include "optnxbe.h"

BEGIN_OPTSTR_TABLE(XbeBuilder, P_UnknownOption, P_UnknownString, P_XbeName, P_UnknownString, FALSE)
	IDOPT_XBENOLOGO,	"nologo",					NO_OPTARGS,				single,
	IDOPT_TITLEID,		"testid:%1",				OPTARGS1(P_XbeTitleId),	single,
	IDOPT_TITLENAME,	"testname:%1",				OPTARGS1(P_XbeTitleName), single,
	IDOPT_XESTACK,		"stack:%1",					OPTARGS1(P_XbeStack),	single,
    IDOPT_XEFLAGS,      "initflags:%1",             OPTARGS1(P_XbeInitFlags),   single,
	IDOPT_XBEDEBUG,		"debug%T1",					OPTARGS1(P_XbeDebug),	single,
	IDOPT_XBENAME,		"out:%1",					OPTARGS1(P_XbeName),	single,
    IDOPT_LIMITMEM,     "limitmem%T1",              OPTARGS1(P_XbeLimitMem),    single,
    IDOPT_TITLEINFO,    "titleinfo:%1",             OPTARGS1(P_XbeTitleInfo),   single,
    IDOPT_TITLEIMAGE,   "titleimage:%1",            OPTARGS1(P_XbeTitleImage),  single,
    IDOPT_XBERATING,    "testratings:%{0xFFFFFFFF|0|1|2|3|4|5|6}1",  OPTARGS1(P_XbeRating),  single,
    IDOPT_XBEREGION,    "testregion:%1",            OPTARGS1(P_XbeRegion),  single,
    IDOPT_SAVEIMAGE,    "defaultsaveimage:%1",      OPTARGS1(P_XbeSaveImage),   single,
	IDOPT_UNKNOWN_OPTION, "", NO_OPTARGS, single,
	IDOPT_UNKNOWN_STRING, "", NO_OPTARGS, single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(XbeBuilder)
	OPTDEF_STRING(XbeName, "")
	OPTDEF_STRING(XbeTitleId, "")
	OPTDEF_STRING(XbeTitleName, "")
	OPTDEF_HEX(XbeStack, 0)
    OPTDEF_HEX(XbeInitFlags, 1)
	OPTDEF_BOOL(XbeDebug, FALSE);
    OPTDEF_STRING(XbeTitleImage, "")
    OPTDEF_STRING(XbeTitleInfo, "")
    OPTDEF_HEX(XbeRegion, 0xFFFFFFFF)
    OPTDEF_INT(XbeRating, 0)
    OPTDEF_STRING(XbeSaveImage, "")
    OPTDEF_BOOL(XbeLimitMem, FALSE)
END_OPTDEF_MAP()

DEFINE_LOCALIZED_ENUM(RatingEnum)
	LOCALIZED_ENUM_ENTRY(IDS_RatingNone /* (none) */, 1)
	LOCALIZED_ENUM_ENTRY(IDS_Rating0 /* RP */, 2)
	LOCALIZED_ENUM_ENTRY(IDS_Rating1 /* AO */, 3)
	LOCALIZED_ENUM_ENTRY(IDS_Rating2 /* M */, 4)
	LOCALIZED_ENUM_ENTRY(IDS_Rating3 /* T */, 5)
	LOCALIZED_ENUM_ENTRY(IDS_Rating4 /* E */, 6)
	LOCALIZED_ENUM_ENTRY(IDS_Rating5 /* K-A */, 7)
	LOCALIZED_ENUM_ENTRY(IDS_Rating6 /* EC */, 8)
END_LOCALIZED_ENUM_LIST()

IMPLEMENT_DYNCREATE(CXbeBuilderPageTab, COptionMiniPage);
IMPLEMENT_DYNCREATE(CXbeBuilderGeneralPage, CXbeBuilderPageTab);
IMPLEMENT_DYNCREATE(CXbeBuilderTitlePage, CXbeBuilderPageTab);
IMPLEMENT_DYNCREATE(CXbeBuilderCertificatePage, CXbeBuilderPageTab);

CRuntimeClass *g_XbeBuildTabs[] =
{
	RUNTIME_CLASS(CXbeBuilderGeneralPage),
	RUNTIME_CLASS(CXbeBuilderTitlePage),
	RUNTIME_CLASS(CXbeBuilderCertificatePage),
	NULL
};

BEGIN_IDE_CONTROL_MAP(CXbeBuilderGeneralPage, IDDP_XBEBLD_GENERAL, IDS_CAT_XBEBLD_GENERAL)
	MAP_EDIT(IDC_XBENAME, P_XbeName)
	MAP_EDIT(IDC_XESTACK, P_XbeStack)
    MAP_CHECK(IDC_LIMITMEM, P_XbeLimitMem)
	MAP_CHECK(IDC_XEDEBUG, P_XbeDebug)
    MAP_EDIT(IDC_XEFLAGS, P_XbeInitFlags)
#ifdef XBCP
	MAP_CHECK(IDC_XBCP_DEFERRED, P_DeferredMecr)
#endif
END_IDE_CONTROL_MAP()

BEGIN_IDE_CONTROL_MAP(CXbeBuilderTitlePage, IDDP_XBEBLD_TITLEINFO, IDS_CAT_XBEBLD_TITLEINFO)
	MAP_EDIT(IDC_TITLENAME, P_XbeTitleName)
    MAP_EDIT(IDC_TITLEINFO, P_XbeTitleInfo)
    MAP_EDIT(IDC_TITLEIMAGE, P_XbeTitleImage)
    MAP_EDIT(IDC_SAVEIMAGE, P_XbeSaveImage)
END_IDE_CONTROL_MAP()

BEGIN_IDE_CONTROL_MAP(CXbeBuilderCertificatePage, IDDP_XBEBLD_CERTIFICATE, IDS_CAT_XBEBLD_CERTIFICATE)
	MAP_EDIT(IDC_TITLEID, P_XbeTitleId)
    MAP_EDIT(IDC_XBEREGION, P_XbeRegion)
    MAP_COMBO_LIST(IDC_XBERATING, P_XbeRating, RatingEnum)
END_IDE_CONTROL_MAP()

BOOL OPTION_HANDLER(XbeBuilder)::IsFakeProp(UINT idProp)
{
	return MapActual(idProp) == P_XbeName;
}

void OPTION_HANDLER(XbeBuilder)::FormFakeStrProp(UINT idProp, CString &strVal)
{
	ASSERT(MapActual(idProp) == P_XbeName);

	/* Use the linker's logic */

    CProjItem * pItem = ((CProjItem *)m_pSlob);
	CDir dirWorkspace = pItem->GetProject()->GetWorkspaceDir();
	CDir dirProject = pItem->GetProject()->GetProjDir(pItem->GetActiveConfig());
	CString strWorkspace = (const TCHAR *)dirWorkspace;
	CString strProject = (const TCHAR *)dirProject;
	CString strBase;

	if (strWorkspace.CompareNoCase(strProject) == 0)
	{
		const CPath * ppathProj = pItem->GetProject()->GetFilePath();
		ppathProj->GetBaseNameString(strBase);
	}
	else
	{
		// subproject
		ASSERT( (strProject.GetLength() > strWorkspace.GetLength()) );
		strBase = strProject.Right(strProject.GetLength()-strWorkspace.GetLength()-1);
	}

	// which output directory do we want to use?
	UINT idOutDirProp = GetFakePathDirProp(idProp);
	ASSERT(idOutDirProp != (UINT)-1);

	CString strOut;
	VERIFY(m_pSlob->GetStrProp(idOutDirProp, strOut) == valid);

	const TCHAR * pchT;
	strVal = "";

	// If the output directory doesn't end in a forward slash
	// or a backslash, append one.
	if (!strOut.IsEmpty())
	{
		strVal = strOut;

		pchT = (const TCHAR *)strVal + strVal.GetLength();
		pchT = _tcsdec((const TCHAR *)strVal, (TCHAR *)pchT);

		if (*pchT != _T('/') && *pchT != _T('\\'))
			strVal += _T('/');
	}

	// Add the base, and extension prefix
	strVal += strBase;
	strVal += _T(".xbe");
}

GPT OPTION_HANDLER(XbeBuilder)::GetDefStrProp(UINT idProp, CString & val)
{
	if(!IsFakeProp(idProp))
		return COptionHandler::GetDefStrProp(idProp, val);

	FormFakeStrProp(idProp, val);
	return valid;
}

UINT OPTION_HANDLER(XbeBuilder)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	/* For us, the .exe is an intermediate file */
	if (idPropL == P_XbeName)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
}

BOOL CXbeBuilderPageTab ::Validate()
{
	if(m_nValidateID == IDC_OPTSTR)
		/* We'll just require that our output name end in ".xbe" */
		return Validate_DestinationProp(P_XbeName, IDOPT_XBENAME, IDC_OPTSTR,
			IDS_DEST_XBE, "xbe", FALSE, TRUE);
	return COptionMiniPage::Validate();
}

#ifdef XBCP

void OPTION_HANDLER(XbeBuilder)::ResetPropsForConfig(ConfigurationRecord * pcr)
{
    // do the base-class thing first
    COptionHandler::ResetPropsForConfig(pcr);

    // reset our 'deferred' prop.
    pcr->BagCopy(BaseBag, CurrBag, P_DeferredMecr, P_DeferredMecr, TRUE);
}

BOOL OPTION_HANDLER(XbeBuilder)::CanResetPropsForConfig(ConfigurationRecord * pcr)
{
    // do the base-class thing first
    if (COptionHandler::CanResetPropsForConfig(pcr))
        return TRUE;    // no need to check anymore

    // check our 'deferred' prop.
    return !pcr->BagSame(BaseBag, CurrBag, P_DeferredMecr, P_DeferredMecr, TRUE);
}

#endif // XBCP
