// 
// Browser Database Make Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "optnbsc.h"	// our local header file

BEGIN_OPTSTR_TABLE(BscMake, P_BscMakeUnknownOption, P_BscMakeUnknownString, P_BscMakeUnknownOption, P_BscMakeNoLogo, TRUE)
	IDOPT_BSCMAKE_NOLOGO,	"nologo%T1",		OPTARGS1(P_BscMakeNoLogo),				single,
	IDOPT_BSCMAKE_INCUNREF,	"Iu%T1",			OPTARGS1(P_InclUnref),					single,
	IDOPT_BSCMAKE_OUTNAME,	"o%1",				OPTARGS1(P_BscMakeOutputFilename),		single,
	IDOPT_UNKNOWN_OPTION,	"",					NO_OPTARGS,								single,
	IDOPT_UNKNOWN_STRING,	"",					NO_OPTARGS,								single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(BscMake)
	OPTDEF_PATH(BscMakeOutputFilename, "")
	OPTDEF_BOOL(InclUnref, FALSE)
	OPTDEF_BOOL(BscMakeNoLogo, FALSE)
END_OPTDEF_MAP()

IMPLEMENT_DYNCREATE(CBscMakePageTab, COptionMiniPage)
IMPLEMENT_DYNCREATE(CBscMakeGeneralPage, CBscMakePageTab)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// bscmake tool option default map 'faking'

BOOL OPTION_HANDLER(BscMake)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_BscMakeOutputFilename);
}

UINT OPTION_HANDLER(BscMake)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_BscMakeOutputFilename)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

void OPTION_HANDLER(BscMake)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	ASSERT(idPropL == P_BscMakeOutputFilename);

	CString strBase, strOutDir;

    CProjItem * pItem = ((CProjItem *)m_pSlob);
	CDir dirWorkspace = pItem->GetProject()->GetWorkspaceDir();
	CDir dirProject = pItem->GetProject()->GetProjDir(pItem->GetActiveConfig());
	CString strWorkspace = (const TCHAR *)dirWorkspace;
	CString strProject = (const TCHAR *)dirProject;

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

	VERIFY(m_pSlob->GetStrProp(idOutDirProp, strOutDir) == valid);

	strVal = "";

	if (!strOutDir.IsEmpty())
	{
		TCHAR * pchStart;
		TCHAR * pchLast;

		strVal = strOutDir;

		// Ensure the copied output directory string ends in a backslash.
		pchStart = (TCHAR *)(const TCHAR *)strVal;
		pchLast = pchStart + strVal.GetLength();
		pchLast = _tcsdec(pchStart, pchLast);

		if (*pchLast != _T('/') && *pchLast != _T('\\'))
			strVal += _T('/');
	}

	strVal += strBase;
	strVal += _T(".bsc");
}

GPT OPTION_HANDLER(BscMake)::GetDefStrProp(UINT idProp, CString & val)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_BscMakeOutputFilename)
	{
		FormFakeStrProp(idProp, val);
		return valid;
	}

	return COptionHandler::GetDefStrProp(idProp, val);
}

void OPTION_HANDLER(BscMake)::ResetPropsForConfig(ConfigurationRecord * pcr)
{
	// do the base-class thing first
	COptionHandler::ResetPropsForConfig(pcr);

	// reset our 'deferred' prop.
	pcr->BagCopy(BaseBag, CurrBag, P_NoDeferredBscmake, P_NoDeferredBscmake, TRUE);
}

BOOL OPTION_HANDLER(BscMake)::CanResetPropsForConfig(ConfigurationRecord * pcr)
{
	// do the base-class thing first
	if (COptionHandler::CanResetPropsForConfig(pcr))
		return TRUE;	// no need to check anymore

	// check our 'deferred' prop.
	return !pcr->BagSame(BaseBag, CurrBag, P_NoDeferredBscmake, P_NoDeferredBscmake, TRUE);
}

void OPTION_HANDLER(BscMake)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// do we need special processing?
	if (idPropL == P_BscMakeOutputFilename)
	{
		CProjItem * pItem = (CProjItem *)m_pSlob;

		if (strVal.IsEmpty())
			pItem->GetPropBag()->RemovePropAndInform(idProp, m_pSlob);

		CProject * pProject = pItem->GetProject();
		if (pItem->UsePropertyBag() == CurrBag && pProject->m_bProjectComplete)
			// the user has potentially changed the .BSC name
			// (put other possible target changes here)
			theApp.NotifyPackages(PN_CHANGE_BSC);
	}
}

BOOL CBscMakePageTab::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (COptionMiniPage::OnCommand(wParam,lParam))
	{
		HWND hWndCtl = (HWND)(lParam);
		UINT nNotify = HIWORD(wParam);
		UINT nId = LOWORD(wParam);

		if (nId == IDC_BSCMAKE_NOTDEFERRED && IsDlgButtonChecked(IDC_BSCMAKE_NOTDEFERRED) )
		{
			//
			// Warn user if setting to build browse info file but compiler settings
			// do not build .sbr files.
			//
			CProjItem * pItem = ((CProjItem *)m_pSlob);
			CProject * pProject = pItem->GetProject();
			COptionHandler * popthdlr;
			VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Compiler),
													 (CBldSysCmp *&)popthdlr));

			UINT idCompilerProp = popthdlr->MapLogical(P_GenBrowserInfo);
			int nCompilerGeneratesSbr = 0;

			if (m_pSlob->GetIntProp(idCompilerProp, nCompilerGeneratesSbr) != valid || 0==nCompilerGeneratesSbr)
			{
				//
				// Warn user.
				//
				AfxMessageBox( IDS_WRN_SETBROWSEINFO, MB_OK | MB_ICONWARNING );
			}
		}

		return TRUE;
	}
	return FALSE;
}


BOOL CBscMakePageTab::Validate()
{
	if (m_nValidateID == IDC_BSCOUTNAME)
	{
		if (!Validate_Destination(IDC_BSCOUTNAME, IDS_DEST_BSC,
							 	  "bsc",	// must be extension
								  FALSE,	// must be directory
							 	  TRUE))	// cant be directory
			return FALSE;
	}

	if (m_nValidateID == IDC_OPTSTR)
	{
		if (!Validate_DestinationProp(P_BscMakeOutputFilename, IDOPT_BSCMAKE_OUTNAME,
									  IDC_OPTSTR, IDS_DEST_BSC,
									  "bsc",	// must have extension
									  FALSE,	// must be directory
									  TRUE))	// cant be directory
			return FALSE;

		return TRUE;
	}

	return COptionMiniPage::Validate();
}

// bscmake tool option property page
CRuntimeClass * g_bscTabs[] = 
{
	RUNTIME_CLASS(CBscMakeGeneralPage),
	(CRuntimeClass *)NULL,
};

// bscmake tool option property page

BEGIN_IDE_CONTROL_MAP(CBscMakeGeneralPage, IDDP_PROJ_BSCMAKE, IDS_BSCMAKE)
	MAP_CHECK(IDC_BSCNOLOGO, P_BscMakeNoLogo)
	MAP_CHECK(IDC_BSCMAKE_NOTDEFERRED, P_NoDeferredBscmake)
	MAP_EDIT(IDC_BSCOUTNAME, P_BscMakeOutputFilename)
END_IDE_CONTROL_MAP()
