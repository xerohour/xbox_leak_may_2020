// 
// Library Manager Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop

#include "optnlib.h"	// our local header file
#include "project.h"

BEGIN_OPTSTR_TABLE(Lib, P_LibUnknownOption, P_LibUnknownString, P_LibUnknownString, P_Lib_Input_Ext, FALSE)
	IDOPT_UNKNOWN_STRING,	"",					NO_OPTARGS,								single,
	IDOPT_LIBNOLOGO,		"nologo%T1",		OPTARGS1(P_NoLogoLib),					single,
	IDOPT_DERIVED_OPTHDLR,	"",					NO_OPTARGS,								single,
	IDOPT_DEFLIB,			"def:%1", 			OPTARGS1(P_DefNameLib),					single,
	IDOPT_OUTLIB,			"out:%1",			OPTARGS1(P_OutNameLib),					single,	
	IDOPT_LIB_INPUT_EXT,	"",					OPTARGS1(P_Lib_Input_Ext),				single,	
	IDOPT_UNKNOWN_OPTION,	"",					NO_OPTARGS,								single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(Lib)
	OPTDEF_BOOL(NoLogoLib, FALSE);
	OPTDEF_PATH(DefNameLib, "")
	OPTDEF_PATH(OutNameLib, "")
	OPTDEF_STRING(Lib_Input_Ext, "*.obj")
END_OPTDEF_MAP()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CLibTool, CLinkerTool)

void OLD_OPTION_HANDLER(Lib)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
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

BOOL OLD_OPTION_HANDLER(Lib)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_OutNameLib || idPropL == P_Lib_Input_Ext);
}

UINT OLD_OPTION_HANDLER(Lib)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_OutNameLib)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

void OLD_OPTION_HANDLER(Lib)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	if (m_pSlob == NULL)
		return;

	UINT idPropL = MapActual(idProp);

	// assert this a Library Manager 'fake' string prop?
	VSASSERT(IsFakeProp(idProp), "FormFakeStrProp called with non-fake prop!");

	// get the project base-name (ie. without extension)
	CString strBase;
    CProjItem * pItem = ((CProjItem *)m_pSlob);

	if (idPropL == P_Lib_Input_Ext)
	{
		// make sure the project is in our config.
		CProjTempConfigChange projTempConfigChange(pItem->GetProject());
		projTempConfigChange.ChangeConfig((CConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord);

		CConfigurationRecord * pcr = (CConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord;
		CString strToolName = _T("lib.exe");
		CString strExtList = _T("");
		pcr->GetExtListFromToolName(strToolName, &strExtList);
		if(strExtList)
		{
			strVal = strExtList;
		}
		return;

	}

	const CPath * ppathProj = pItem->GetProject()->GetFilePath();
	ppathProj->GetBaseNameString(strBase);

	strVal = "";

	// do we need to know about the target directory?
	if (idPropL == P_OutNameLib)
	{
		const TCHAR * pchT;
	 	CString strOut;

		// which output directory do we want to use?
		UINT idOutDirProp = GetFakePathDirProp(idProp);
		VSASSERT(idOutDirProp != (UINT)-1, "Failed to get proper fake path dir!");

		GPT gptVal = m_pSlob->GetStrProp(idOutDirProp, strOut);
		VSASSERT(gptVal == valid, "Failed to pick up idOutDirProp!");

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
		VSASSERT(FALSE, "Unhandled fake str prop case!");
	}
}

GPT OLD_OPTION_HANDLER(Lib)::GetDefStrProp(UINT idProp, CString & val)
{
	// can we ignore the output directories?
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefStrProp(idProp, val);

	FormFakeStrProp(idProp, val);
	return valid;
}

///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------- CLibTool ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////
CLibTool::CLibTool() : CLinkerTool()
{
	// tool input file set
	m_strToolInput = _TEXT("*.obj;*.res;*.lib");
	m_strToolPrefix =  _TEXT ("LIB32");
}

