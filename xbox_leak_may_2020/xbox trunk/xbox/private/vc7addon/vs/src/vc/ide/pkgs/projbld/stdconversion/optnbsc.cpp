// 
// Browser Database Make Tool Options and Tool
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop

#include "optnbsc.h"	// our local header file
#include "project.h"

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CBscmakeTool, CSchmoozeTool)

// bscmake tool option default map 'faking'

BOOL OLD_OPTION_HANDLER(BscMake)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_BscMakeOutputFilename);
}

UINT OLD_OPTION_HANDLER(BscMake)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_BscMakeOutputFilename)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

void OLD_OPTION_HANDLER(BscMake)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	VSASSERT(idPropL == P_BscMakeOutputFilename, "Calling FormFakeStrProp with non-fake prop!");

	CString strBase, strOutDir;

    CProjItem * pItem = ((CProjItem *)m_pSlob);
	const CPath * ppathProj = pItem->GetProject()->GetFilePath();
	ppathProj->GetBaseNameString(strBase);

	// which output directory do we want to use?
	UINT idOutDirProp = GetFakePathDirProp(idProp);
	VSASSERT(idOutDirProp != (UINT)-1, "Failed to get proper fake path dir!");

	GPT gptVal = m_pSlob->GetStrProp(idOutDirProp, strOutDir);
	VSASSERT(gptVal == valid, "Failed to pick up idOutDirProp!");

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

GPT OLD_OPTION_HANDLER(BscMake)::GetDefStrProp(UINT idProp, CString & val)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_BscMakeOutputFilename)
	{
		FormFakeStrProp(idProp, val);
		return valid;
	}

	return COptionHandler::GetDefStrProp(idProp, val);
}

void OLD_OPTION_HANDLER(BscMake)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// do we need special processing?
	if (idPropL == P_BscMakeOutputFilename)
	{
		CProjItem * pItem = (CProjItem *)m_pSlob;

		if (strVal.IsEmpty())
			pItem->GetPropBag()->RemovePropAndInform(idProp, m_pSlob);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBscmakeTool
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBscmakeTool::CBscmakeTool() : CSchmoozeTool()
{
	// tool exe name and input file set
	m_strToolInput = _TEXT("*.sbr");
	m_strToolPrefix = _TEXT("BSC32");
	m_strName = _T("BSC");
}
