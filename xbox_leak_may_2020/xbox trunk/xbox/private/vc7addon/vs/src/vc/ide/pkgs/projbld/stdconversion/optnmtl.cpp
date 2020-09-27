//
// MkTypLib Tool Options
//
//

#include "stdafx.h" // standard AFX include
#pragma hdrstop

#include "optnmtl.h" // local header file
#include "project.h"

BEGIN_OPTSTR_TABLE (MkTypLib, P_MTLUnknownOption, P_MTLUnknownString, P_MTLNologo, P_MTL_Server, FALSE)
	IDOPT_MTLNOLOGO,		"nologo%T1",		OPTARGS1(P_MTLNologo),			single,
	IDOPT_MTLINCLUDES,		"I[ ]%1",			OPTARGS1(P_MTLIncludes),		multiple,
	IDOPT_MTLDLLDATA,		"dlldata[ ]%1",		OPTARGS1(P_MTLDllData),			single,
	IDOPT_MTLMACROS,		"D[ ]%1",			OPTARGS1(P_MTLMacros), 			multiple,
	IDOPT_MTLOUTPUTTLB,		"tlb[ ]%1",			OPTARGS1(P_MTLOutputTlb),		single,
	IDOPT_MTLOUTPUTINC,		"h[ ]%1",			OPTARGS1(P_MTLOutputInc),		single,
	IDOPT_MTLOUTPUTUUID,	"iid[ ]%1",			OPTARGS1(P_MTLOutputUuid),		single,
	IDOPT_MTLOUTPUTDIR,		"out[ ]%1",			OPTARGS1(P_MTLOutputDir),		single,
	IDOPT_MTLMTLCOMPATIBLE,	"mktyplib203%T1",	OPTARGS1(P_MTLMtlCompatible), 	single,
	IDOPT_MTL_STUBLESS_PROXY,	"Oicf%T1",		OPTARGS1(P_MTLStublessProxy),	single,
	IDOPT_MTLNOCLUTTER,		"o[ ]%1",			OPTARGS1(P_MTLNoClutter), 		single,
	IDOPT_MTL_INPUT_EXT,	"",					OPTARGS1(P_MTL_Input_Ext),		single,
	IDOPT_UNKNOWN_OPTION,		"",				NO_OPTARGS,						single,
	IDOPT_UNKNOWN_STRING,		"",				NO_OPTARGS,						single,
	IDOPT_MTL_CHAR,			"char %{signed|ascii7|unsigned}1",	OPTARGS1(P_MTL_Char),	single,
	IDOPT_MTL_CLIENT,		"client %{stub|none}1",	OPTARGS1(P_MTL_Client),		single,
	IDOPT_MTL_SERVER,		"server %{stub|none}1",	OPTARGS1(P_MTL_Server),		single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP (MkTypLib)
	OPTDEF_BOOL(MTLNologo, FALSE)
	OPTDEF_LIST(MTLIncludes, "")
	OPTDEF_LIST(MTLMacros, "") 
	OPTDEF_PATH(MTLOutputTlb, "")
	OPTDEF_PATH(MTLOutputInc, "")
	OPTDEF_PATH(MTLOutputUuid, "")
	OPTDEF_PATH(MTLOutputDir, "")
	OPTDEF_BOOL(MTLMtlCompatible, FALSE)
	OPTDEF_BOOL(MTLStublessProxy, FALSE)
	OPTDEF_PATH(MTLNoClutter, "")
	OPTDEF_STRING(MTL_Input_Ext, "*.idl")
	OPTDEF_INT(MTL_Char, 0)
	OPTDEF_PATH(MTLDllData, "")
	OPTDEF_INT(MTL_Client, 0)
	OPTDEF_INT(MTL_Server, 0)
END_OPTDEF_MAP()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMkTypLibTool, CBuildTool)

//////////////////////////////////////////////////////////
BOOL OLD_OPTION_HANDLER(MkTypLib)::IsFakeProp (UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_MTL_Input_Ext || idPropL == P_MTLOutputTlb);
}

//////////////////////////////////////////////////////////////
UINT OLD_OPTION_HANDLER(MkTypLib)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_MTLOutputTlb)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

//////////////////////////////////////////////////////////////
void OLD_OPTION_HANDLER (MkTypLib)::FormFakeStrProp (UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// Assert this is a MkTypLib 'fake' string prop ?
	VSASSERT (IsFakeProp (idProp), "FormFakeStrProp called with non-fake prop!");
	
	// get the project base-name (ie. without extension)
	CString strProj, strBase;
	CPath pathProj;
	const TCHAR * pchT;

	if (idPropL == P_MTL_Input_Ext)
	{
		// make sure the project is in our config.
		CProjItem * pItem = (CProjItem *)m_pSlob;
		CProjTempConfigChange projTempConfigChange(pItem->GetProject());
		projTempConfigChange.ChangeConfig((CConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord);

		CConfigurationRecord * pcr = (CConfigurationRecord *)pItem->GetActiveConfig()->m_pBaseRecord;
		CString strToolName = _T("midl.exe");
		CString strExtList = _T("");
		pcr->GetExtListFromToolName(strToolName, &strExtList);
		if(strExtList)
		{
			strVal = strExtList;
		}
		return;

	}

	if (idPropL == P_MTLOutputTlb)
	{
		GPT gpt = m_pSlob->GetStrProp(P_ProjItemName, strProj);
		strBase.Empty();
		if (gpt == valid && !strProj.IsEmpty())
		{
			BOOL bOK = pathProj.Create(strProj);
			VSASSERT(bOK, "Failed to create project path!");
			pathProj.GetBaseNameString(strBase);
		}

		if (!strBase.IsEmpty())
		{
			// which output directory do we want to use?
			UINT idOutDirProp = GetFakePathDirProp(idProp);
			VSASSERT(idOutDirProp != (UINT)-1, "Failed to get proper fake path dir!");

			CProjItem * pItem = (CProjItem *)m_pSlob;
			GPT gpt = pItem->GetStrProp(idOutDirProp, strVal);
			while (gpt != valid)
			{
				// *chain* the proper. config.
				CProjItem * pItemOld = pItem;
				pItem = (CProjItem *)pItem->GetContainerInSameConfig();
				if (pItemOld != m_pSlob)
					pItemOld->ResetContainerConfig();

				VSASSERT(pItem != (CSlob *)NULL, "Item's container is NULL!!!");
				gpt = pItem->GetStrProp(idOutDirProp, strVal);
			}

			// reset the last container we found
			if (pItem != m_pSlob)
				pItem->ResetContainerConfig();

			if (!strVal.IsEmpty())
			{
				// If the output directory doesn't end in a forward slash
				// or a backslash, append one.
				pchT = (const TCHAR *)strVal + strVal.GetLength();
				pchT = _tcsdec((const TCHAR *)strVal, (TCHAR *)pchT);

				if (*pchT != _T('/') && *pchT != _T('\\'))
					strVal += _T('/');
			}

			strVal += strBase + _TEXT(".tlb") ;
		}
	}
	else
		strVal.Empty() ;
}

/////////////////////////////////////////////////////////////////////////
GPT OLD_OPTION_HANDLER (MkTypLib)::GetDefStrProp(UINT idProp, CString & val)
{
	if (!IsFakeProp (idProp))
		return COptionHandler::GetDefStrProp(idProp, val) ;	

	FormFakeStrProp (idProp, val) ;
	return valid ;
}

///////////////////////////////////////////////////////////////////////////////
// CMkTypLibTool
///////////////////////////////////////////////////////////////////////////////
CMkTypLibTool::CMkTypLibTool() : CBuildTool()
{
	// tool exe name and input file set
	m_strToolInput = _TEXT("*.odl;*.idl");
	m_strToolPrefix = _TEXT("MTL");
	m_strName = _TEXT("midl.exe");
}

