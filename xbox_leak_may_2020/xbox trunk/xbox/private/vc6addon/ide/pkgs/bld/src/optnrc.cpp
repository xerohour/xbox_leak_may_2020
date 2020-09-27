// 
// Resource Compiler Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "optnrc.h"	// our local header file

BEGIN_OPTSTR_TABLE(ResCompiler, P_ResUnknownOption, P_ResUnknownString, P_ResVerbose, P_ResUnknownString, FALSE)
	IDOPT_RESLANGID,		"l[ ]%1",	OPTARGS1(P_ResLangID),		single,
	IDOPT_DERIVED_OPTHDLR,		"",		NO_OPTARGS,					single,
	IDOPT_RESVERBOSE,		"v%T1",		OPTARGS1(P_ResVerbose),		single,
	IDOPT_RESIGNINC,		"x%T1",		OPTARGS1(P_ResIgnIncPath), 	single,
	IDOPT_OUTDIR_RES,		"fo%1",		OPTARGS1(P_OutNameRes),		single,
	IDOPT_RESINCLUDES,		"i[ ]%1",	OPTARGS1(P_ResIncludes),	multiple,
	IDOPT_RESMACROS,		"d[ ]%1",	OPTARGS1(P_ResMacroNames),	multiple,
	IDOPT_UNKNOWN_OPTION,	"",			NO_OPTARGS,					single,
	IDOPT_UNKNOWN_STRING,	"",			NO_OPTARGS,					single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(ResCompiler)
	OPTDEF_BOOL(ResVerbose, FALSE)
	OPTDEF_BOOL(ResIgnIncPath, FALSE)
	OPTDEF_PATH(OutNameRes, "")
	OPTDEF_DIR_LIST(ResIncludes, "")
	OPTDEF_LIST(ResMacroNames, "")
	OPTDEF_HEX(ResLangID, 0x00)
END_OPTDEF_MAP()

IMPLEMENT_DYNCREATE(CResCompilerPage, COptionMiniPage)
IMPLEMENT_DYNCREATE(CResCompilerGeneralPage, CResCompilerPage)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////
// string constant definitions
const TCHAR * CResCompilerPage::strOutputExtension = _TEXT("res");

////////////////////////////////////////////////////
// GetOutputExtension
//		returns the RC output extension, depending on the platform
//		(ie: MAC's is rsc and NT's is res
////////////////////////////////////////////////////
BOOL OPTION_HANDLER(ResCompiler)::AlwaysShowDefault(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_ResLangID);
}

BOOL OPTION_HANDLER(ResCompiler)::CheckDepOK(UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fValid = TRUE;			// valid by default

	// which prop are we checking the dep. for?
	switch (idPropL)
	{
		// only do this if language enabled for the item's platform
		case P_ResLangID:
		{
			CProjType * pProjType = ((CProjItem *)m_pSlob)->GetProjType();
			if (pProjType != (CProjType *)NULL)
			{
				const CPlatform * pPlatform = pProjType->GetPlatform();
				ASSERT (pPlatform != NULL);

				fValid = ((pPlatform->GetAttributes() & PIA_Enable_Language) != 0);
			}
			break;
		}

		default:
			break;
	}

	return fValid;
}

const CString  * OPTION_HANDLER(ResCompiler)::GetOutputExtension()
{
	CProjType * pProjType = ((CProjItem *)m_pSlob)->GetProjType();
	if (pProjType != (CProjType *)NULL)
	{
		const CPlatform * pPlatform = pProjType->GetPlatform();
		ASSERT (pPlatform != NULL);
		return &((pPlatform->GetToolInfo())->strRCExtension);
	}
	else
		return (const CString *)NULL;
}

// resource compiler tool option default map 'faking'
BOOL OPTION_HANDLER(ResCompiler)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_OutNameRes || idPropL == P_ResIncludes || idPropL == P_ResLangID);
}

UINT OPTION_HANDLER(ResCompiler)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_OutNameRes)
		return P_OutDirs_Intermediate;

	if (idPropL == P_OutNameRes)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

void OPTION_HANDLER(ResCompiler)::FormFakeIntProp(UINT idProp, int & nVal)
{
	// ASSERT this is a Resource Compiler 'fake' string prop?
	ASSERT(IsFakeProp(idProp));

	nVal = GetUserDefaultLangID();
	return;
}

GPT OPTION_HANDLER(ResCompiler)::GetDefIntProp(UINT idProp, int & nVal)
{
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefIntProp(idProp, nVal);

	FormFakeIntProp(idProp, nVal);
	return valid;
}

void OPTION_HANDLER(ResCompiler)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_OutNameRes)
	{
		if (strVal.IsEmpty())
			m_pSlob->GetPropBag()->RemovePropAndInform(MapLogical(P_OutNameRes), m_pSlob);
	}		
}

void OPTION_HANDLER(ResCompiler)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// ASSERT this a Resource Compiler 'fake' string prop?
	ASSERT(IsFakeProp(idProp));

	if (idPropL == P_OutNameRes)
	{
		CProjItem * pItem = (CProjItem *)m_pSlob;
		
		// which output directory do we want to use?
		UINT idOutDirProp = GetFakePathDirProp(idProp);
		ASSERT(idOutDirProp != (UINT)-1);

		GPT gpt = pItem->GetStrProp(idOutDirProp, strVal);
		while (gpt != valid)
		{
			// *chain* the proper. config.
			CProjItem * pItemOld = pItem;
			pItem = (CProjItem *)pItem->GetContainerInSameConfig();
			if (pItemOld != m_pSlob)
				pItemOld->ResetContainerConfig();

			ASSERT(pItem != (CSlob *)NULL);
			gpt = pItem->GetStrProp(idOutDirProp, strVal);
		}

		// reset the last container we found
		if (pItem != m_pSlob)
			pItem->ResetContainerConfig();
	
		if (!strVal.IsEmpty())
		{
			TCHAR * pchStart = strVal.GetBuffer(1);
			TCHAR * pchT;

			pchT = pchStart + _tcslen(pchStart);	// point to nul terminator
			pchT = _tcsdec(pchStart, pchT);			// back up one char, DBCS safe

			if (*pchT != _T('\\') && *pchT != _T('/'))
				strVal += _T('/');

			strVal.ReleaseBuffer();
		}

		CString strProj, strBase;
		const CString * pstrExt = GetOutputExtension();

		CObList oblist;
		pItem->GetProject()->GetResourceFileList(oblist, TRUE,
												 (ConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord);

		// do we have exactly *one* buildable .RC file and a a valid extension?
		if (oblist.GetCount() != 1 || pstrExt == (const CString *)NULL)
		{
			// no .rc file in current config, so return empty string
			strVal.Empty();
			return;
		}

		// get the name of this .RC file
		VERIFY (((CProjItem *)oblist.GetHead())->GetStrProp(P_ProjItemName, strProj) == valid);
		CPath pathProj;
		VERIFY (pathProj.Create (strProj));
		pathProj.GetBaseNameString (strBase);
		strVal += strBase + _T('.') + *pstrExt;
	}
	else if (idPropL == P_ResIncludes && m_pSlob->IsKindOf (RUNTIME_CLASS(CFileItem)))
	{
		CFileItem * pFileItem = (CFileItem *)m_pSlob;

		const CPath * pPathRC = pFileItem->GetFilePath();
		CDir dirRC; dirRC.CreateFromPath(*pPathRC);

		CDir dirProj = pFileItem->GetProject()->GetWorkspaceDir();

		CString strProj = dirProj;
		strProj += _T('\\');
		
		if (dirRC != dirProj)
			strVal = GetRelativeName (dirRC, strProj);
		else
			strVal.Empty();

		// now we want to add target directory
		CString strTarget;

		strTarget.Empty();
		CProjItem * pItem = pFileItem;

		// which output directory do we want to use?
		UINT idOutDirProp = P_OutDirs_Target;	// note: hard-coded
		ASSERT(idOutDirProp != (UINT)-1);

		GPT gpt = pItem->GetStrProp(idOutDirProp, strTarget);
		while (gpt != valid)
		{
			// *chain* the proper. config.
			CProjItem * pItemOld = pItem;
			pItem = (CProjItem *)pItem->GetContainerInSameConfig();
			if (pItemOld != m_pSlob)
				pItemOld->ResetContainerConfig();

			ASSERT(pItem != (CSlob *)NULL);
			gpt = pItem->GetStrProp(idOutDirProp, strTarget);
		}

		// reset the last container we found
		if (pItem != m_pSlob)
			pItem->ResetContainerConfig();

		// file registry for this target?
		CFileRegistry * pregistry = pFileItem->GetRegistry();
		CTargetItem * pTarget = pFileItem->GetTarget();

		CObList obFilelist;
		pregistry->GetFileItemList(pTarget, pregistry->GetODLFileList(), obFilelist, TRUE, pItem->GetActiveConfig());

		if (gpt == valid && !strTarget.IsEmpty() && !obFilelist.IsEmpty())
		{
			if (!strVal.IsEmpty())
				strVal += _T(',');
			strVal += strTarget;
		}
	}

}

GPT OPTION_HANDLER(ResCompiler)::GetDefStrProp(UINT idProp, CString & val)
{
	// can we ignore the output directories?
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefStrProp(idProp, val);

	FormFakeStrProp(idProp, val);
	return valid;
}

// resource compiler option property page

DWORD rgLangID[] = 
{
	0x0401, 	0x0402, 	0x0403, 	0x0404,
	0x0804, 	0x0405, 	0x0406, 	0x0407,
 	0x0807, 	0x0408, 	0x0409, 	0x0809,
 	0x0c09, 	0x1009, 	0x040a, 	0x080a,
 	0x0c0a, 	0x040b, 	0x040c, 	0x080c,
	0x0c0c, 	0x100c, 	0x040d, 	0x040e,
 	0x040f, 	0x0410, 	0x0810, 	0x0411,
  	0x0412, 	0x0413, 	0x0813, 	0x0414,
	0x0814, 	0x0415, 	0x0416, 	0x0816,
	0x0417, 	0x0418, 	0x0419, 	0x041a,
 	0x081a, 	0x041b, 	0x041c, 	0x041d,
 	0x041e, 	0x041f, 	0x0420, 	0x0421
};
#define rgLangID_MAX (sizeof(rgLangID) / sizeof(DWORD))

CEnumLangID AFX_DATA_EXPORT LangIDEnum;

void CEnumLangID::FillComboBox(CComboBox* pWnd, BOOL bClear, CSlob* pSlob)
{
	ASSERT(pWnd != NULL);
	ASSERT(pSlob != NULL);
 
 	char sz [101];
 
	pWnd->SetRedraw(FALSE);

	if (bClear)
		pWnd->ResetContent();

	for (int i = 0 ; i < rgLangID_MAX ; i++)
	{
		VerLanguageName(rgLangID[i], sz, 100);
		pWnd->SetItemData(pWnd->AddString(sz), rgLangID[i]);
 	}

	pWnd->SetRedraw();
	pWnd->Invalidate();
}


//////////////////////////////////////////////////////
void CResCompilerGeneralPage::InitPage()
{
	COptionMiniPage::InitPage() ;

	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	if (!((CProxySlob *)m_pSlob)->IsSingle())
	{
		GetDlgItem (IDC_RESOUTNAME)->EnableWindow (FALSE) ;
		GetDlgItem (IDC_REST1)->EnableWindow (FALSE) ;
	}

	// (Use the CProxySlob platform enumeration in case of multi-selection)
	// This is to decide whether we want to hide the language option or not in the 
	// RC option page.
	// We should hide if any of the platform does not support the (/I) option
	BOOL bEnableLanguage = TRUE;	// multi-selection of items' platform supports the language option (/I)

	((CProxySlob *)m_pSlob)->InitPlatformEnum();
	const CPlatform * pPlatform;
	while ((pPlatform = ((CProxySlob *)m_pSlob)->NextPlatform()) != (const CPlatform *)NULL)
		bEnableLanguage = bEnableLanguage && ((pPlatform->GetAttributes() & PIA_Enable_Language) != 0);

	GetDlgItem (IDC_REST4)->ShowWindow (bEnableLanguage);
}

BOOL CResCompilerGeneralPage::OnPropChange (UINT idProp)
{	  
	UINT idPropL = MapActual(idProp);

	ASSERT (m_pSlob->IsKindOf (RUNTIME_CLASS (CProxySlob))) ;
	if (idPropL == P_OutNameRes)
		GetDlgItem (IDC_REST1)->EnableWindow (((CProxySlob *)m_pSlob)->IsSingle());

	return CResCompilerPage::OnPropChange(idProp);
}

BOOL CResCompilerPage::Validate()
{
	if (m_nValidateID == IDC_RESOUTNAME)
	{
		if (!Validate_Destination(IDC_RESOUTNAME, IDS_DEST_RES,
							  	  GetOutputExtension(),	// must have extension
							  	  FALSE,	// must be directory
							  	  TRUE))	// cant be directory
			return FALSE;
	}

	if (m_nValidateID == IDC_OPTSTR)
	{
		return Validate_DestinationProp(P_OutNameRes, IDOPT_OUTDIR_RES,
										IDC_OPTSTR, IDS_DEST_RES,
									    GetOutputExtension(),	// must have extension
									    FALSE,	// must be directory
									    TRUE);	// cant be directory
	}

	if (IDC_RESINCLUDES==m_nValidateID)
	{
		//
		// Strip quotes.
		//
		CString strOption;
		CWnd * pWnd = GetDlgItem(m_nValidateID);
		ASSERT(pWnd != (CWnd *)NULL);
		pWnd->GetWindowText(strOption);

		int index;

		while (-1 != (index=strOption.Find('"')) )
		{
			CString Left;
			CString Right;

			if (0!=index)
				Left = strOption.Left(index);

			if (index+1 < strOption.GetLength())
				Right = strOption.Mid(index+1);

			strOption = Left + Right;
		}

		pWnd->SetWindowText(strOption);
	}

	return COptionMiniPage::Validate();
}

// rc tool option property page
CRuntimeClass * g_rcTabs[] = 
{
	RUNTIME_CLASS(CResCompilerGeneralPage),
	(CRuntimeClass *)NULL,
};

BEGIN_IDE_CONTROL_MAP(CResCompilerGeneralPage, IDDP_PROJ_RESCOMPILE, IDS_RESCOMPILER32)
	MAP_CHECK(IDC_RESIGNPATH, P_ResIgnIncPath)
	MAP_EDIT(IDC_RESOUTNAME, P_OutNameRes)
	MAP_EDIT(IDC_RESINCLUDES, P_ResIncludes)
	MAP_EDIT(IDC_RESMACNAMES, P_ResMacroNames)
	MAP_COMBO_LIST(IDC_RESLANGID, P_ResLangID, LangIDEnum)
END_IDE_CONTROL_MAP()

BEGIN_OPTSTR_TABLE(ResCompilerNT, (UINT)NULL, (UINT)NULL, (UINT)NULL, (UINT)NULL, FALSE)
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(ResCompilerNT)
END_OPTDEF_MAP()

