// 
// Resource Compiler Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop

#include "optnrc.h"	// our local header file
#include "project.h"

BEGIN_OPTSTR_TABLE(ResCompiler, P_ResUnknownOption, P_ResUnknownString, P_ResVerbose, P_ResUnknownString, FALSE)
	IDOPT_RESLANGID,		"l[ ]%1",	OPTARGS1(P_ResLangID),		single,
	IDOPT_DERIVED_OPTHDLR,		"",		NO_OPTARGS,					single,
	IDOPT_RESVERBOSE,		"v%T1",		OPTARGS1(P_ResVerbose),		single,
	IDOPT_RESIGNINC,		"x%T1",		OPTARGS1(P_ResIgnInCPath), 	single,
	IDOPT_OUTDIR_RES,		"fo%1",		OPTARGS1(P_OutNameRes),		single,
	IDOPT_RESINCLUDES,		"i[ ]%1",	OPTARGS1(P_ResIncludes),	multiple,
	IDOPT_RESMACROS,		"d[ ]%1",	OPTARGS1(P_ResMacroNames),	multiple,
	IDOPT_RES_INPUT_EXT,	"",			OPTARGS1(P_Res_Input_Ext),	single,
	IDOPT_UNKNOWN_OPTION,	"",			NO_OPTARGS,					single,
	IDOPT_UNKNOWN_STRING,	"",			NO_OPTARGS,					single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(ResCompiler)
	OPTDEF_BOOL(ResVerbose, FALSE)
	OPTDEF_BOOL(ResIgnInCPath, FALSE)
	OPTDEF_PATH(OutNameRes, "")
	OPTDEF_DIR_LIST(ResIncludes, "")
	OPTDEF_LIST(ResMacroNames, "")
	OPTDEF_STRING(Res_Input_Ext, "*.rc")
	OPTDEF_HEX(ResLangID, 0x00)
END_OPTDEF_MAP()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CRCCompilerTool, CBuildTool)
IMPLEMENT_DYNAMIC(CRCCompilerNTTool, CRCCompilerTool)

////////////////////////////////////////////////////
// GetOutputExtension
//		returns the RC output extension, depending on the platform
//		(ie: MAC's is rsc and NT's is res
////////////////////////////////////////////////////
BOOL OLD_OPTION_HANDLER(ResCompiler)::AlwaysShowDefault(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_ResLangID);
}

BOOL OLD_OPTION_HANDLER(ResCompiler)::CheckDepOK(UINT idProp)
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
				VSASSERT (pPlatform != NULL, "No platform for project type!");

				fValid = ((pPlatform->GetAttributes() & PIA_Enable_Language) != 0);
			}
			break;
		}

		default:
			break;
	}

	return fValid;
}

const CString  * OLD_OPTION_HANDLER(ResCompiler)::GetOutputExtension()
{
	CProjType * pProjType = ((CProjItem *)m_pSlob)->GetProjType();
	if (pProjType != (CProjType *)NULL)
	{
		const CPlatform * pPlatform = pProjType->GetPlatform();
		VSASSERT (pPlatform != NULL, "No platform for project type!");
		return &((pPlatform->GetToolInfo())->strRCExtension);
	}
	else
		return (const CString *)NULL;
}

// resource compiler tool option default map 'faking'
BOOL OLD_OPTION_HANDLER(ResCompiler)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_Res_Input_Ext || idPropL == P_ResIncludes || idPropL == P_ResLangID);
}

UINT OLD_OPTION_HANDLER(ResCompiler)::GetFakePathDirProp(UINT idProp)
{
	return COptionHandler::GetFakePathDirProp(idProp);
} 

BOOL OLD_OPTION_HANDLER(ResCompiler)::IsDefaultStringProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// if we say that a non-empty default resource includes list (caused when the rc file is in a different
	// directory than the dsp file) is the default, we get ourselves into all kinds of trouble.
	// Therefore, when asked about a non-empty res include value, we automagically say that it is *not* the default
	if (idPropL == P_ResIncludes)
		return strVal.IsEmpty();

	return COptionHandler::IsDefaultStringProp(idProp, strVal);
}

void OLD_OPTION_HANDLER(ResCompiler)::FormFakeIntProp(UINT idProp, int & nVal)
{
	// Assert this is a Resource Compiler 'fake' string prop?
	VSASSERT(IsFakeProp(idProp), "FormFakeIntProp called with non-fake prop!");

	nVal = GetUserDefaultLangID();
	return;
}

GPT OLD_OPTION_HANDLER(ResCompiler)::GetDefIntProp(UINT idProp, int & nVal)
{
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefIntProp(idProp, nVal);

	FormFakeIntProp(idProp, nVal);
	return valid;
}

void OLD_OPTION_HANDLER(ResCompiler)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
{
}

void OLD_OPTION_HANDLER(ResCompiler)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// Assert this a Resource Compiler 'fake' string prop?
	VSASSERT(IsFakeProp(idProp), "FormFakeStrProp called with non-fake prop!");

	if (idPropL == P_Res_Input_Ext)
	{
		// make sure the project is in our config.
		CProjItem * pItem = (CProjItem *)m_pSlob;
		CProjTempConfigChange projTempConfigChange(pItem->GetProject());
		projTempConfigChange.ChangeConfig((CConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord);

		CConfigurationRecord * pcr = (CConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord;
		CString strToolName = _TEXT("rc.exe");
		CString strExtList;
		pcr->GetExtListFromToolName(strToolName, &strExtList);
		if(strExtList)
		{
			strVal = strExtList;
		}
		return;

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
			strVal = GetRelativeName(dirRC, strProj);
		else
			strVal.Empty();

		// now we want to add target directory, but iff we've got ODL files to worry about
		CProject* pProject = pFileItem->GetProject();
		if (pProject)
		{
			if (pProject->m_bHaveODLFiles)
			{
				CString strTarget;
				CProjItem * pItem = pFileItem;

				// which output directory do we want to use?
				UINT idOutDirProp = P_OutDirs_Target;	// note: hard-coded

				GPT gpt = pItem->GetStrProp(idOutDirProp, strTarget);
				while (gpt != valid)
				{
					// *chain* the proper. config.
					CProjItem * pItemOld = pItem;
					pItem = (CProjItem *)pItem->GetContainerInSameConfig();
					if (pItemOld != m_pSlob)
						pItemOld->ResetContainerConfig();

					VSASSERT(pItem != (CSlob *)NULL, "Item's container is NULL!!!");
					gpt = pItem->GetStrProp(idOutDirProp, strTarget);
				}

				// reset the last container we found
				if (pItem != m_pSlob)
					pItem->ResetContainerConfig();

				if (gpt == valid)
				{
					if (!strVal.IsEmpty())
						strVal += _T(',');
					strVal += strTarget;
				}
			}
		}
	}

}

GPT OLD_OPTION_HANDLER(ResCompiler)::GetDefStrProp(UINT idProp, CString & val)
{
	// can we ignore the output directories?
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefStrProp(idProp, val);

	FormFakeStrProp(idProp, val);
	return valid;
}

BEGIN_OPTSTR_TABLE(ResCompilerNT, (UINT)NULL, (UINT)NULL, (UINT)NULL, (UINT)NULL, FALSE)
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(ResCompilerNT)
END_OPTDEF_MAP()

///////////////////////////////////////////////////////////////////////////////
//
// ----------------------------- CRCCompilerTool ------------------------------
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
CRCCompilerTool::CRCCompilerTool() : CBuildTool()
{
	m_strToolInput = _TEXT("*.rc");
	m_strToolPrefix = _TEXT("RSC");
	m_strName = _TEXT("rc.exe");
}	

///////////////////////////////////////////////////////////////////////////////
BOOL CRCCompilerNTTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	return FALSE;
}
