// 
// Linker Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "optnlink.h"	// our local header file

BEGIN_OPTSTR_TABLE(LinkerCommon, P_LinkUnknownOption, P_LinkUnknownString, P_LinkNoLogo, P_LinkUnknownString, FALSE)
	IDOPT_UNKNOWN_STRING,	"",							NO_OPTARGS,								single,
	IDOPT_LINKNOLOGO,	"nologo%T1",					OPTARGS1(P_LinkNoLogo),					single,
	IDOPT_DERIVED_OPTHDLR,	"",							NO_OPTARGS,								single,
	IDOPT_LINKVERBOSE,	"verbose%T1",					OPTARGS1(P_LinkVerbose), 				single,
	IDOPT_DLLGEN,		"dll%T1",						OPTARGS1(P_GenDll),						single,
	IDOPT_PROFILE,		"profile%T1",					OPTARGS1(P_Profile),					single,
	IDOPT_USEPDBNONE,	"pdb:none%F1",					OPTARGS1(P_UsePDB),						single,
	IDOPT_INCREMENTALLINK, "incremental:%{no|yes}1",	OPTARGS1(P_IncrementalLink),			single,
	IDOPT_USEPDB,		"pdb:%T1%2",					OPTARGS2(P_UsePDB, P_UsePDBName),		single,
	IDOPT_MAPGEN,		"m[ap]%T1[:%2]",				OPTARGS2(P_GenMap, P_MapName), 			single,
	IDOPT_DEBUG,		"debug%T1",						OPTARGS1(P_GenDebug),					single,
	IDOPT_DEBUGTYPE,	"debugtype:%{cv|coff|both}1",	OPTARGS1(P_DebugType),					single,
	IDOPT_MACHINETYPE, 	"machine:%{I386|IX86|M68K|ALPHA|MPPC}1",	OPTARGS1(P_MachineType),	single,
	IDOPT_NODEFAULTLIB,	"nod[efaultlib]:%1",			OPTARGS1(P_NoDefaultLibs),			multiple,
	IDOPT_EXALLLIB,		"nod[efaultlib]%T1",			OPTARGS1(P_ExAllLibs),				single,
	IDOPT_INCLUDESYM,	"include:%1",					OPTARGS1(P_IncludeSym),					multiple,
	IDOPT_DEFNAME,		"def:%1",						OPTARGS1(P_DefName),					single,
	IDOPT_FORCE,		"force%T1",						OPTARGS1(P_Force),						single,
	IDOPT_OUT,			"out:%1",						OPTARGS1(P_OutName),					single,	
	IDOPT_IMPLIB,		"implib:%1",					OPTARGS1(P_ImpLibName),					single,
	IDOPT_LAZYPDB,		"pdbtype:%{sept|con}1",			OPTARGS1(P_LazyPdb),					single,
	IDOPT_LIBPATH,		"libpath:%1",					OPTARGS1(P_LibPath),					multiple,
	IDOPT_UNKNOWN_OPTION,	"",							NO_OPTARGS,								single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerCommon)
	OPTDEF_BOOL(LinkNoLogo, FALSE)
	OPTDEF_BOOL(LinkVerbose, FALSE)
	OPTDEF_BOOL(GenDll, FALSE)
	OPTDEF_BOOL(GenMap, FALSE)
	OPTDEF_PATH(MapName, "")
	OPTDEF_PATH(OutName, "")
	OPTDEF_BOOL(GenDebug, FALSE)
	OPTDEF_INT(DebugType, 1)
	OPTDEF_BOOL(ExAllLibs, FALSE) 
	OPTDEF_LIST(NoDefaultLibs, "")
	OPTDEF_LIST(IncludeSym, "")
	OPTDEF_PATH(DefName, "")
	OPTDEF_BOOL(UsePDB, TRUE)
	OPTDEF_BOOL(IncrementalLink, TRUE)
	OPTDEF_PATH(UsePDBName, "")
	OPTDEF_BOOL(Force, FALSE)
	OPTDEF_PATH(ImpLibName, "")
	OPTDEF_BOOL(Profile, FALSE)
	OPTDEF_INT(MachineType, 2)
	OPTDEF_INT(IgnoreExportLib, 0)
	OPTDEF_LIST(LibPath, "")
	OPTDEF_INT(LazyPdb, 0)
END_OPTDEF_MAP()

IMPLEMENT_DYNCREATE(CLinkerGeneralPage, CLinkerPageTab)
IMPLEMENT_DYNCREATE(CLinkerPageTab, COptionMiniPage)
IMPLEMENT_DYNCREATE(CLinkerDebugPage, CLinkerPageTab)
IMPLEMENT_DYNCREATE(CLinkerCustomPage, CLinkerPageTab)
IMPLEMENT_DYNCREATE(CLinkerInputPage, CLinkerPageTab)
IMPLEMENT_DYNCREATE(CLinkerWin32OutputPage, CLinkerPageTab)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// linker tool option default map 'faking'
BOOL OPTION_HANDLER(LinkerCommon)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_MapName || idPropL == P_OutName ||
			idPropL == P_ImpLibName || idPropL == P_UsePDBName ||
			idPropL == P_DefName || idPropL == P_IgnoreExportLib ||
			idPropL == P_IncrementalLink);
}

UINT OPTION_HANDLER(LinkerCommon)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_MapName)
		return P_OutDirs_Intermediate;

	// .exe, .lib, .pdb use the target directory
	else if (idPropL == P_OutName || idPropL == P_ImpLibName || idPropL == P_UsePDBName)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

void OPTION_HANDLER(LinkerCommon)::OnOptionIntPropChange(UINT idProp, int nVal)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_GenDll)
		m_pSlob->InformDependants(MapLogical(P_OutName));

	else if (idProp == P_ItemExcludedFromBuild)
	{
		const CPath * pPath = ((CProjItem *)m_pSlob)->GetFilePath();
		CFileRegistry* pFileRegistry = ((CProjItem *)m_pSlob)->GetRegistry();

		CPtrList * pFileList;					  
		if (pPath != (CPath *)NULL &&	// ie. only for CFileItems

			// is this a .def file being included in the build??
			pFileRegistry->GetFileList(pPath, (const CPtrList * &)pFileList) &&
			pFileList == pFileRegistry->GetDEFFileList() &&	// is this a .def file?
			!nVal)	// included in build?
		{
			// start tracking /DEF: by removing the current prop. from the project-level
			const ConfigurationRecord * pcr = ((CProjItem *)m_pSlob)->GetActiveConfig();
			if (pcr == (const ConfigurationRecord *)NULL)
			{
				ASSERT(FALSE);
				return;
			}
			((ConfigurationRecord *)pcr->m_pBaseRecord)->GetPropBag(CurrBag)->RemoveProp(MapLogical(P_DefName));
		}
	}		
}

void OPTION_HANDLER(LinkerCommon)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
{ 
	UINT idPropL = MapActual(idProp);

	// which prop. is this?
	switch (idPropL)
	{
		case P_UsePDBName:
			if (strVal.IsEmpty())
				m_pSlob->GetPropBag()->RemovePropAndInform(MapLogical(P_UsePDBName), m_pSlob);
			break;

		case P_OutName:
 			if (strVal.IsEmpty())
				m_pSlob->GetPropBag()->RemovePropAndInform(MapLogical(P_OutName), m_pSlob);
			m_pSlob->InformDependants(P_ProjItemFullPath);
			m_pSlob->InformDependants(P_TargetName);	// Inform the project item so that the project settings updates ok
			break;
 
		case P_DefName:
		{
			// we want to make sure this .DEF is included in the build
			CObList oblistDEFFile;
			const ConfigurationRecord * pcrBase, * pcr = ((CProjItem *)m_pSlob)->GetActiveConfig();
			if (pcr == (const ConfigurationRecord *)NULL)
			{
				ASSERT(FALSE);
				return;
			}
			pcrBase = pcr->m_pBaseRecord;

			CFileRegistry* pFileRegistry;
			CTargetItem * pTarget = NULL;

			pFileRegistry = ((CProjItem *)m_pSlob)->GetRegistry();
			pTarget = ((CProjItem *)m_pSlob)->GetTarget();

			if (pFileRegistry == NULL)
			{
				// no registry (so we probably have a CProxySlob)
				ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
				if (((CProxySlob*)m_pSlob)->IsSingle())
				{
					OptTreeNode* pOptNode = ((CProxySlob*)m_pSlob)->GetSingle();
					CString strTargetName = pOptNode->pcr->GetConfigurationName();
					pTarget =  g_pActiveProject->GetTarget(strTargetName);
					ASSERT(pTarget);
					pFileRegistry = pTarget->GetRegistry();
				}
			}
			ASSERT(pFileRegistry);
			
 			pFileRegistry->GetFileItemList(pTarget, pFileRegistry->GetDEFFileList(),
										   oblistDEFFile,
										   FALSE, pcrBase);
			if (oblistDEFFile.GetCount() == 0)	// none?
				break;

			// create a path for this def file
			CPath pathDEF;
			if (!pathDEF.CreateFromDirAndFilename((const CDir &)g_pActiveProject->GetProjDir(),
												  (TCHAR *)(const TCHAR *)strVal))
				break;							// can't create def path

			POSITION posDEF = (POSITION)NULL, pos = oblistDEFFile.GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				POSITION posCurr = pos;
				if (pathDEF == (const CPath &)*((CFileItem * )oblistDEFFile.GetNext(pos))->GetFilePath())
				{
					posDEF = posCurr;	// found a matching def file
					break;
				}
			}

			int nVal;

			// set all of the other defs to 'excluded from build'
			POSITION posT = oblistDEFFile.GetHeadPosition();
			while (posT != (POSITION)NULL)
			{
				BOOL fSkip = posT == posDEF;
				CFileItem * pItem =	(CFileItem * )oblistDEFFile.GetNext(posT);
				if (fSkip)	continue;	// skip our def we got		

				pItem->ForceBaseConfigActive((ConfigurationRecord *)pcrBase);
				if (pItem->GetIntProp(P_ItemExcludedFromBuild, nVal) != valid || !nVal)
					pItem->SetIntProp(P_ItemExcludedFromBuild, TRUE);
				pItem->ForceConfigActive();
			}

			if (posDEF != (POSITION)NULL)
			{
				// set this one to 'included in build' ?
				CFileItem * pItem =	(CFileItem * )oblistDEFFile.GetAt(posDEF);
				pItem->ForceBaseConfigActive((ConfigurationRecord *)pcrBase);
				if (pItem->GetIntProp(P_ItemExcludedFromBuild, nVal) != valid || nVal)
					pItem->SetIntProp(P_ItemExcludedFromBuild, FALSE);
				pItem->ForceConfigActive();
			}
			break;
		}

		default:
			break;
	}

	// change our outputs?
}

BOOL OPTION_HANDLER(LinkerCommon)::AlwaysShowDefault(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_UsePDB || idPropL == P_MachineType);
}

BOOL OPTION_HANDLER(LinkerCommon)::IsDefaultStringProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_OutName || idPropL == P_ImpLibName || idPropL == P_DefName)
	{
		CString strFake;
		if (idPropL == P_OutName || idPropL == P_ImpLibName)
			// both 'foobar.exe' and 'foobar.dll' are defaults!
			VERIFY(m_pSlob->GetIntProp(MapLogical(P_GenDll), m_fProjIsDll) == valid);

		FormFakeStrProp(idProp, strFake);
		if (strFake.CompareNoCase(strVal) == 0)
			return TRUE;

		if (idPropL == P_OutName)
		{
			m_fProjIsDll = !m_fProjIsDll;

			FormFakeStrProp(idProp, strFake);
			if (strFake.CompareNoCase(strVal) == 0)
				return TRUE;
		}

		// if we failed to match a faked prop, then is this a default in 
		// the 'default map'?
	}

	return COptionHandler::IsDefaultStringProp(idProp, strVal);
}

void OPTION_HANDLER(LinkerCommon)::FormFakeIntProp(UINT idProp, int & nVal)
{
	UINT idPropL = MapActual(idProp);

	// should not call this if it is not a fake property
	ASSERT (IsFakeProp (idProp));

	// the default value of Incremental Link depends whether
	// it is a 'debug' or 'release' mode
	if (idPropL == P_IncrementalLink)
		if (m_pSlob->GetIntProp(P_UseDebugLibs, nVal) != valid) 
			nVal = FALSE;

	if (idPropL == P_IgnoreExportLib){
		nVal = TRUE;
		if( m_pSlob->GetIntProp(P_Proj_IgnoreExportLib, nVal) == invalid ){
			if (m_pSlob->GetIntProp(MapLogical(P_GenDll), nVal) == invalid )	// /DLL must be set
				nVal = TRUE;
			else
				nVal = FALSE;
		}
	}
}

GPT OPTION_HANDLER(LinkerCommon)::GetDefIntProp(UINT idProp, int & nVal)
{
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefIntProp(idProp, nVal);

	FormFakeIntProp(idProp, nVal);
	return valid;
}

void OPTION_HANDLER(LinkerCommon)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// ASSERT this a Linker 'fake' string prop?
	ASSERT(IsFakeProp(idProp));

	// do we want to form /DEF:foobar.def ?
	if (idPropL == P_DefName)
	{	
		// do we have *one* 'buildable' .DEF in project?
		const ConfigurationRecord * pcrBase, * pcr = ((CProjItem *)m_pSlob)->GetActiveConfig();
		if (pcr == (const ConfigurationRecord *)NULL)
		{
			// no config. (perhaps CProxySlob multi-config)
			return;
		}
		pcrBase = pcr->m_pBaseRecord;

		CFileRegistry* pFileRegistry;
		CTargetItem* pTarget = NULL;

		pFileRegistry = ((CProjItem *)m_pSlob)->GetRegistry();
		pTarget = ((CProjItem *)m_pSlob)->GetTarget();

		if (pFileRegistry == NULL)
		{
			// no registry (so we probably have a CProxySlob)
			ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
			if (((CProxySlob*)m_pSlob)->IsSingle())
			{
				OptTreeNode* pOptNode = ((CProxySlob*)m_pSlob)->GetSingle();
				CString strTargetName = pOptNode->pcr->GetConfigurationName();
				pTarget = g_pActiveProject->GetTarget(strTargetName);
				ASSERT(pTarget);
				pFileRegistry = pTarget->GetRegistry();
			}
		}
		ASSERT(pFileRegistry);
 
		CObList oblistDEFFile;
		pFileRegistry->GetFileItemList(pTarget,
									   pFileRegistry->GetDEFFileList(),
									   oblistDEFFile,
									   TRUE, pcrBase);

		if (oblistDEFFile.GetCount() == 1)
		{
			CDir dirProj; 
			dirProj = ((CProjItem *)m_pSlob)->GetProject()->GetWorkspaceDir();

			// get the path of the *only* .DEF file in this list
			const CPath * pPath = ((CFileItem *)oblistDEFFile.GetHead())->GetFilePath();
			pPath->GetRelativeName(dirProj, strVal);	// relativizer this to project dir.
		}
		else
			strVal = "";	// no unique 'buildable' .DEF in project

		return;	// we're done
	}

	// form the other fake string props...

	// get the project base-name (ie. without extension)
	CString strBase;

    // If we are faking the output name then we use the projects base-name
    // (without extension) as the basis of the faked name.
    // If we are faking the map name, pdb name, or implib name then we
    // use the base of the output name as the basis of the faked name.
    if (idPropL == P_OutName)
	{
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
	}
    else
	{
		CString strPath;
		VERIFY(m_pSlob->GetStrProp(MapLogical(P_OutName), strPath) == valid);
	  	
		CPath path;
	 	if (path.Create(strPath))
			path.GetBaseNameString(strBase);
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
	strVal += _T('.');

	// Append the extension
	CString strExt;
	if (idPropL == P_MapName)
	{
		strExt = _TEXT("map");
	}
	else if (idPropL == P_OutName || idPropL == P_ImpLibName || idPropL == P_UsePDBName)
	{
		if (idPropL == P_ImpLibName)
		{
			strExt = _TEXT("lib");
		}
		else if (idPropL == P_UsePDBName)
		{
			strExt = _TEXT("pdb");
		}
		else
		{
			// can we get a supplied default target extension?
			if (!m_pSlob->GetStrProp(P_Proj_TargDefExt, strExt))
				strExt = (m_fProjIsDll ? _TEXT("dll") : _TEXT("exe"));	// no
		}
	}
	else	
	{
		ASSERT(FALSE);
	}

	strVal += strExt;
}

GPT OPTION_HANDLER(LinkerCommon)::GetDefStrProp(UINT idProp, CString & val)
{
	UINT idPropL = MapActual(idProp);

	BOOL fIgnoreFake = FALSE;

	// we don't have ImpLibName with no /DLL
	if (idPropL == P_ImpLibName)
	{
		int nVal;
		if (m_pSlob->GetIntProp(MapLogical(P_GenDll), nVal) == invalid || !nVal)	// /DLL must be set
			fIgnoreFake = TRUE;	// not a faked prop in this context
	}
	
	// can we ignore the output directories?
	if (fIgnoreFake || !IsFakeProp(idProp))
		return COptionHandler::GetDefStrProp(idProp, val);

	if (idPropL == P_OutName || idPropL == P_ImpLibName)
		VERIFY(m_pSlob->GetIntProp(MapLogical(P_GenDll), m_fProjIsDll) == valid);

	FormFakeStrProp(idProp, val);
	return valid;
}

BOOL OPTION_HANDLER(LinkerCommon)::CheckDepOK(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	BOOL fValid = TRUE;			// valid by default
	
	// placeholders for values
	int nVal, nVal2;	
	CPlatform * pPlatform ;

	// which prop are we checking the dep. for?
	switch (idPropL)
	{
		// Incremental not valid if request to generate a Map
		case P_IncrementalLink:
			pPlatform = g_pActiveProject->GetCurrentPlatform() ;
			if (pPlatform->GetAttributes() & PIA_Supports_IncLink)
			{
				(void) m_pSlob->GetIntProp(MapLogical(P_UsePDB), nVal) ;
				(void) m_pSlob->GetIntProp (MapLogical(P_Profile), nVal2) ;
				fValid = nVal && !nVal2;
			}
			else
				fValid = FALSE ;
			break;
		// Use PDB not valid if Profiling is checked 
		case P_UsePDB:
			(void) m_pSlob->GetIntProp (MapLogical(P_Profile), nVal) ;
			fValid = !nVal ;
			break ;

		// Generate Mapfile must be disabled if Profile is checked
		case P_GenMap:
			//(void)m_pSlob->GetIntProp (MapLogical(P_Profile), nVal) ;
			fValid = TRUE;
			break ;

		// Can only type map-name if generating a mapfile!
		case P_MapName:
			(void) m_pSlob->GetIntProp(MapLogical(P_GenMap), nVal);
			fValid = nVal;
			break;

		// Can't specify the debug info type if not gen. debug info
		case P_DebugType:
			(void) m_pSlob->GetIntProp(MapLogical(P_GenDebug), nVal);
			pPlatform = g_pActiveProject->GetCurrentPlatform() ;
			if (pPlatform->GetAttributes() & PIA_Enable_AllDebugType)
				fValid = nVal;
			else
				 fValid = FALSE;
			break;

		// Not allowed to type in a .PDB name without use PDB file
		case P_UsePDBName:
			VERIFY(m_pSlob->GetIntProp(MapLogical(P_UsePDB), nVal) == valid);
			(void) m_pSlob->GetIntProp(MapLogical(P_Profile), nVal2) ;
			fValid = nVal && !nVal2;
			break;
		// not allowed to select lazy pdb unless pdb in use
		case P_LazyPdb:
			VERIFY(m_pSlob->GetIntProp(MapLogical(P_UsePDB), nVal) == valid);
			(void) m_pSlob->GetIntProp(MapLogical(P_Profile), nVal2) ;
			fValid = nVal && !nVal2;
			break;
	/*
		case P_StubName:
			pPlatform = g_pActiveProject->GetCurrentPlatform();
			if (pPlatform->GetAttributes() & PIA_Enable_Stub)
				fValid = TRUE;
			else
				fValid = FALSE;
			break;
	*/
		default:
			break;
	}

	return fValid;
}

// linker tool option property page
CRuntimeClass * g_linkTabs[] = 
{
	RUNTIME_CLASS(CLinkerGeneralPage),
	RUNTIME_CLASS(CLinkerInputPage),
	RUNTIME_CLASS(CLinkerCustomPage),
	RUNTIME_CLASS(CLinkerDebugPage),
	(CRuntimeClass *)NULL,
};


BEGIN_IDE_CONTROL_MAP(CLinkerGeneralPage, IDDP_PROJ_LINKER, IDS_LINKCOFF)
	MAP_EDIT(IDC_LIBS, P_LinkUnknownString)
	MAP_CHECK(IDC_EXLIB, P_ExAllLibs)
	MAP_CHECK(IDC_MAPGEN, P_GenMap)
	MAP_CHECK(IDC_LNK_DBGFULL, P_GenDebug)
	MAP_EDIT(IDC_OUT, P_OutName)
	MAP_CHECK(IDC_INCLINK, P_IncrementalLink)
	MAP_CHECK(IDC_PROFILE, P_Profile)
	MAP_CHECK(IDC_IGNOREEXPORTLIB, P_IgnoreExportLib)
END_IDE_CONTROL_MAP()

BOOL IsLearningBox();

BOOL CLinkerGeneralPage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if( IsLearningBox() )
	{
		GetDlgItem(IDC_PROFILE)->EnableWindow(FALSE);
	}

	if (idPropL == P_Profile)
	{
		OnPropChange(MapLogical(P_GenMap));
		OnPropChange(MapLogical(P_UsePDB));
	}
	else if (idPropL == P_UsePDB)
	{
		OnPropChange(MapLogical(P_IncrementalLink));
	}
	else if (idPropL == P_IgnoreExportLib)
	{
		int nVal;
		m_pSlob->GetIntProp(idProp, nVal) ;
		m_pSlob->SetIntProp(P_Proj_IgnoreExportLib, nVal);
		BOOL fDll = m_pSlob->GetIntProp(MapLogical(P_GenDll), nVal) == valid && nVal;
		if( fDll ) 
			GetDlgItem(IDC_IGNOREEXPORTLIB)->ShowWindow(SW_SHOW);
		else
			GetDlgItem(IDC_IGNOREEXPORTLIB)->ShowWindow(SW_HIDE);
	}
	return CLinkerPageTab::OnPropChange(idProp);
}


BOOL CLinkerPageTab::Validate()
{
	if (m_nValidateID == IDC_OUT)
	{
		if (!Validate_Destination(IDC_OUT, IDS_DEST_TRG,
								  "",		// doesn't have extension
								  FALSE,	// must be directory
								  TRUE))	// cant be directory
			return FALSE;
	}

	if (m_nValidateID == IDC_OPTSTR)
	{
		if (!Validate_DestinationProp(P_OutName,
									  IDOPT_OUT,
									  IDC_OPTSTR, IDS_DEST_TRG,
									  "",		// doesn't have extension
									  FALSE,	// must be directory
									  TRUE))	// cant be directory
			return FALSE;

		return (Validate_DestinationProp(P_MapName,
										 IDOPT_MAPGEN,
										 IDC_OPTSTR, IDS_DEST_MAP,
								  		 "map",	// must have extension
								  		 FALSE,	// must be directory
								  		 TRUE));	// cant be directory
	}

	return COptionMiniPage::Validate();
}


BEGIN_IDE_CONTROL_MAP(CLinkerDebugPage, IDDP_LINKER_DEBUG, IDS_CAT_DEBUG)
	MAP_CHECK(IDC_MAPGEN, P_GenMap)
	MAP_EDIT(IDC_MAPNAME, P_MapName)
	MAP_CHECK(IDC_LNK_DBGFULL, P_GenDebug)
	MAP_CHECK(IDC_LAZYPDB, P_LazyPdb)
	MAP_RADIO(IDC_LNK_DBGCV, IDC_LNK_DBGBOTH, 1, P_DebugType)
END_IDE_CONTROL_MAP()


BOOL CLinkerDebugPage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	// we want to enable/disable prop page items
	// - we'll enable/disable the static text
	// - OnPropChange() will enable/disable the rest
	switch (idPropL)
	{
		case P_GenDebug:
		{
			CLinkerPageTab::OnPropChange(MapLogical(P_DebugType));
			break;
		}

		case P_GenMap:
		{
			int nVal;																			  

			CLinkerPageTab::OnPropChange(MapLogical(P_MapName));

			GetDlgItem(IDC_TXT_MAPNAME)->EnableWindow(m_pSlob->GetIntProp(MapLogical(P_GenMap), nVal) == valid && !!nVal);
			break;
		}
		case P_UsePDB:
		{
			int nVal = FALSE ;
			m_pSlob->GetIntProp(MapLogical(P_UsePDB), nVal) ;
			// GetDlgItem (IDC_TXT_PDBNAME)->EnableWindow(nVal) ;
			CLinkerPageTab::OnPropChange(MapLogical(P_LazyPdb));
		}
		default:
			break;
	}

	// call our base-class
	return CLinkerPageTab::OnPropChange(idProp);
}

BOOL CLinkerDebugPage::Validate()
{
	if (m_nValidateID == IDC_MAPNAME)
	{
		if (!Validate_Destination(IDC_MAPNAME, IDS_DEST_MAP,
								  "map",	// must have extension
								  FALSE,	// must be directory
							  	  TRUE))	// cant be directory
			return FALSE;
	}

	return CLinkerPageTab::Validate();
}


BEGIN_IDE_CONTROL_MAP(CLinkerCustomPage, IDDP_LINKER_CUSTOM, IDS_CAT_CUSTOMLINK)

	MAP_CHECK(IDC_USEPDB, P_UsePDB)
	MAP_EDIT(IDC_PDBNAME, P_UsePDBName)
	MAP_CHECK(IDC_INCLINK, P_IncrementalLink)
	MAP_CHECK(IDC_VERBOSE, P_LinkVerbose)
	MAP_CHECK(IDC_LINKNOLOGO, P_LinkNoLogo)
	MAP_CHECK(IDC_FORCE, P_Force)
	MAP_EDIT(IDC_OUT, P_OutName)
END_IDE_CONTROL_MAP()

BOOL CLinkerCustomPage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	// we want to enable/disable prop page items
	// - we'll enable/disable the static text
	// - OnPropChange() will enable/disable the rest
	switch (idPropL)
	{
		
		case P_UsePDB:
		{
			int nVal = FALSE ;
			m_pSlob->GetIntProp(MapLogical(P_UsePDB), nVal) ;
			GetDlgItem (IDC_TXT_PDBNAME)->EnableWindow(nVal) ; 
			CLinkerPageTab::OnPropChange(MapLogical(P_UsePDBName));
			CLinkerPageTab::OnPropChange(MapLogical(P_IncrementalLink));
			break;
		}

		// this is needed when the user type /PROFILE manually
		// in the option edit box.
		case P_Profile:
			OnPropChange(MapLogical(P_GenMap));
			OnPropChange(MapLogical(P_UsePDB));
			break;
		default:
			break;
	}

	// if our OnPropChange is because we are currently editing it, ignore
	// it .. we don't want the SlobPage updating the edit-control as we are
	// typing especially 'cos we do special stuff
	if ((idPropL == P_VersionMaj && m_nCurrentEditID == IDC_VERMAJOR) ||
		(idPropL == P_VersionMin && m_nCurrentEditID == IDC_VERMINOR))
	{
		m_pToolOptionTab->Refresh();		// option string update!
		return TRUE;	// no page update ... ok!
	}

	// call our base-class
	return CLinkerPageTab::OnPropChange(idProp);
}

BOOL CLinkerCustomPage::Validate()
{
	return CLinkerPageTab::Validate();
}


BEGIN_IDE_CONTROL_MAP(CLinkerInputPage, IDDP_LINKER_INPUT, IDS_CAT_INPUT)
	MAP_EDIT(IDC_LIBS, P_LinkUnknownString)
	MAP_CHECK(IDC_EXLIB, P_ExAllLibs)
	MAP_EDIT(IDC_IGNLIBS, P_NoDefaultLibs)
	MAP_EDIT(IDC_INCLSYM, P_IncludeSym)
	MAP_EDIT(IDC_LIBPATH, P_LibPath)
END_IDE_CONTROL_MAP()

void CLinkerInputPage::InitPage()
{
	COptionMiniPage::InitPage();
/*
	BOOL bEnableStubName = TRUE;
	((CProxySlob *)m_pSlob)->InitPlatformEnum();
	const CPlatform * pPlatform;
	while (bEnableStubName && (pPlatform = ((CProxySlob *)m_pSlob)->NextPlatform()) != (const CPlatform *)NULL)
		bEnableStubName = bEnableStubName && ((pPlatform->GetAttributes() & PIA_Enable_Stub) != 0);

	GetDlgItem (IDC_STUB)->EnableWindow (bEnableStubName);
	GetDlgItem (IDC_TXT_STUB)->EnableWindow (bEnableStubName);
 */
}

//------------------------------------------------------
// NT linker option handler
//------------------------------------------------------

BEGIN_OPTSTR_TABLE(LinkerNT, (UINT)NULL, (UINT)NULL, P_VersionMaj, P_SubSystem, FALSE)
	IDOPT_DERIVED_OPTHDLR,	"",							NO_OPTARGS,								single,
	IDOPT_EXE_BASE,		"base:%1",						OPTARGS1(P_BaseAddr),					single,
	IDOPT_VERSION,		"version:%1[.%2]",				OPTARGS2(P_VersionMaj, P_VersionMin),	single,
	IDOPT_STACK,		"st[ack]:%1[,%2]",				OPTARGS2(P_StackReserve, P_StackCommit),single,
	IDOPT_ENTRYPOINT,	"entry:%1",						OPTARGS1(P_EntryName),					single,
	IDOPT_SUBSYSTEM,	"subsystem:%{windows|console}1",OPTARGS1(P_SubSystem),					single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerNT)
	OPTDEF_INT(VersionMaj, -1)	// no default
	OPTDEF_INT(VersionMin, -1)	// no default
	OPTDEF_HEX(StackReserve, 0)
	OPTDEF_HEX(StackCommit, 0)
	OPTDEF_STRING(BaseAddr, "")
	OPTDEF_STRING(EntryName, "")
	OPTDEF_INT(SubSystem, -1)	// no default
END_OPTDEF_MAP()

BOOL OPTION_HANDLER(LinkerNT)::AlwaysShowDefault(UINT idProp)
{
	return FALSE;	// none
}

BOOL OPTION_HANDLER(LinkerNT)::CheckDepOK(UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fValid = TRUE;			// valid by default

	// placeholders for values
	int nVal;

	// which prop are we checking the dep. for?
	switch (idPropL)
	{
		// Stack Commit only enabled if Reserve size set
		case P_StackCommit:
			(void) m_pSlob->GetIntProp(MapLogical(P_StackReserve), nVal);
			fValid = (nVal != 0);	
			break;

		// Version Minor only enabled if Major version set
		case P_VersionMin:
		{
			CString strVal;
			(void) m_pSlob->GetStrProp(MapLogical(P_VersionMaj), strVal);
			fValid = (strVal != "");
			break;
	   	}

		default:
			break;
	}

	return fValid;
}

// NT linker tool option property page
CRuntimeClass * g_linkNTTabs[] = 
{
	RUNTIME_CLASS(CLinkerWin32OutputPage),
	(CRuntimeClass *)NULL
};


BEGIN_IDE_CONTROL_MAP(CLinkerWin32OutputPage, IDDP_LINKER_WIN32_OUTPUT, IDS_CAT_WIN32_OUTPUT)
	MAP_AUTO_EDIT(IDC_STACKRESERVE, P_StackReserve)
	MAP_AUTO_EDIT(IDC_STACKCOMMIT, P_StackCommit)
	MAP_AUTO_EDIT(IDC_BASEADDR, P_BaseAddr)
	MAP_EDIT(IDC_ENTRYPOINT, P_EntryName)
	MAP_AUTO_EDIT(IDC_VERMAJOR, P_VersionMaj)
	MAP_AUTO_EDIT(IDC_VERMINOR, P_VersionMin)
END_IDE_CONTROL_MAP()

BOOL CLinkerWin32OutputPage::OnPropChange(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	// we want to enable/disable prop page items
	// - we'll enable/disable the static text
	// - OnPropChange() will enable/disable the rest
	switch (idPropL)
	{
		case P_StackReserve:
		{
			int nVal;

			CLinkerPageTab::OnPropChange(MapLogical(P_StackCommit));

			GetDlgItem(IDC_TXT_STK_COM)->EnableWindow(m_pSlob->GetIntProp(MapLogical(P_StackCommit), nVal) == valid);
			break;
		}

		case P_VersionMaj:
		{
			int nVal;

			CLinkerPageTab::OnPropChange(MapLogical(P_VersionMin));

			GetDlgItem(IDC_TXT_VERMIN)->EnableWindow(m_pSlob->GetIntProp(MapLogical(P_VersionMin), nVal) == valid);
			break;
		}

		default:
			break;
	}

	// if our OnPropChange is because we are currently editing it, ignore
	// it .. we don't want the SlobPage updating the edit-control as we are
	// typing especially 'cos we do special stuff
	if ((idPropL == P_StackReserve && m_nCurrentEditID == IDC_STACKRESERVE) ||
		(idPropL == P_StackCommit && m_nCurrentEditID == IDC_STACKCOMMIT) ||
		(idPropL == P_BaseAddr && m_nCurrentEditID == IDC_BASEADDR) ||
		(idPropL == P_VersionMaj && m_nCurrentEditID == IDC_VERMAJOR) ||
		(idPropL == P_VersionMin && m_nCurrentEditID == IDC_VERMINOR))
	{
		m_pToolOptionTab->Refresh();		// option string update!
		return TRUE;	// no page update ... ok!
	}

	// call our base-class
	return CLinkerPageTab::OnPropChange(idProp);
}
