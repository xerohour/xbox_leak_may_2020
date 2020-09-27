// 
// Library Manager Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "optnlib.h"	// our local header file

BEGIN_OPTSTR_TABLE(Lib, P_LibUnknownOption, P_LibUnknownString, P_LibUnknownString, P_DefNameLib, FALSE)
	IDOPT_UNKNOWN_STRING,	"",					NO_OPTARGS,								single,
	IDOPT_LIBNOLOGO,		"nologo%T1",		OPTARGS1(P_NoLogoLib),					single,
	IDOPT_DERIVED_OPTHDLR,	"",					NO_OPTARGS,								single,
	IDOPT_DEFLIB,			"def:%1", 			OPTARGS1(P_DefNameLib),					single,
	IDOPT_OUTLIB,			"out:%1",			OPTARGS1(P_OutNameLib),					single,	
	IDOPT_UNKNOWN_OPTION,	"",					NO_OPTARGS,								single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(Lib)
	OPTDEF_BOOL(NoLogoLib, FALSE);
	OPTDEF_PATH(DefNameLib, "")
	OPTDEF_PATH(OutNameLib, "")
END_OPTDEF_MAP()

IMPLEMENT_DYNCREATE(CLibPageTab, COptionMiniPage)
IMPLEMENT_DYNCREATE(CLibGeneralPage, CLibPageTab)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void OPTION_HANDLER(Lib)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_OutNameLib)
	{
 		if (strVal.IsEmpty())
			m_pSlob->GetPropBag()->RemovePropAndInform(MapLogical(P_OutNameLib), m_pSlob);
		m_pSlob->InformDependants(P_ProjItemFullPath);
		m_pSlob->InformDependants(P_TargetName);
	}
}

// lib tool option default map 'faking'

BOOL OPTION_HANDLER(Lib)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_OutNameLib);
}

UINT OPTION_HANDLER(Lib)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_OutNameLib)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

void OPTION_HANDLER(Lib)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// ASSERT this a Library Manager 'fake' string prop?
	ASSERT(IsFakeProp(idProp));

	// get the project base-name (ie. without extension)
	CString strBase;
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

	strVal = "";

	// do we need to know about the target directory?
	if (idPropL == P_OutNameLib)
	{
		const TCHAR * pchT;
	 	CString strOut;

		// which output directory do we want to use?
		UINT idOutDirProp = GetFakePathDirProp(idProp);
		ASSERT(idOutDirProp != (UINT)-1);

		VERIFY(m_pSlob->GetStrProp(idOutDirProp, strOut) == valid);

		if (!strOut.IsEmpty())
		{
			strVal = strOut;

			// If the output directory doesn't end in a forward slash
			// or a backslash, append one.

			pchT = (const TCHAR *)strVal + strVal.GetLength();
			pchT = _tcsdec((const TCHAR *)strVal, (TCHAR *)pchT);

			if (*pchT != _T('/') && *pchT != _T('\\'))
				strVal += _T('\\');
		}


		// Add the base, and extension prefix
		strVal += strBase;
		strVal += _T('.');

		// Append the extension
		// can we get a supplied default target extension?
		CString strExt;
		if (!m_pSlob->GetStrProp(P_Proj_TargDefExt, strExt))
			strExt = _TEXT("lib");	// no

		strVal += strExt;
	}
	else
	{
		ASSERT(FALSE);
	}
}

GPT OPTION_HANDLER(Lib)::GetDefStrProp(UINT idProp, CString & val)
{
	// can we ignore the output directories?
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefStrProp(idProp, val);

	FormFakeStrProp(idProp, val);
	return valid;
}

// lib tool option property page
CRuntimeClass * g_libTabs[] = 
{
	RUNTIME_CLASS(CLibGeneralPage),
	(CRuntimeClass *)NULL,
};

// lib tool option property page

BEGIN_IDE_CONTROL_MAP(CLibGeneralPage, IDDP_PROJ_LIB, IDS_LIBMGR)
	MAP_CHECK(IDC_LIBNOLOGO, P_NoLogoLib)
	MAP_EDIT(IDC_LIBOUT, P_OutNameLib)
END_IDE_CONTROL_MAP()

BOOL CLibPageTab::Validate()
{
	if (m_nValidateID == IDC_LIBOUT)
	{
		if (!Validate_Destination(IDC_LIBOUT, IDS_DEST_TRG,
							 	  "lib",	// must be extension
							 	  FALSE,	// must be directory
							  	  TRUE))	// cant be directory
			return FALSE;
	}

 	if (m_nValidateID == IDC_OPTSTR)
	{
		return Validate_DestinationProp(P_OutNameLib, IDOPT_OUTLIB,
										IDC_OPTSTR, IDS_DEST_TRG,
									    "lib",		// doesn't have extension
									    FALSE,	// must be directory
									    TRUE);	// cant be directory
	}

	return COptionMiniPage::Validate();
}
