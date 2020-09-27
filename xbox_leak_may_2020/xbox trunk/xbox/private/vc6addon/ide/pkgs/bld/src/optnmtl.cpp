//
// MkTypLib Tool Options
//
//

#include "stdafx.h" // standard AFX include
#pragma hdrstop
#include "optnmtl.h" // local header file

BEGIN_OPTSTR_TABLE (MkTypLib, P_MTLUnknownOption, P_MTLUnknownString, P_MTLNologo, P_MTLUnknownString, FALSE)
	IDOPT_MTLNOLOGO,		"nologo%T1",		OPTARGS1(P_MTLNologo),			single,
	IDOPT_MTLINCLUDES,		"I[ ]%1",			OPTARGS1(P_MTLIncludes),		multiple,
	IDOPT_MTLMACROS,		"D[ ]%1",			OPTARGS1(P_MTLMacros), 			multiple,
	IDOPT_MTLOUTPUTTLB,		"tlb[ ]%1",			OPTARGS1(P_MTLOutputTlb),		single,
	IDOPT_MTLOUTPUTINC,		"h[ ]%1",			OPTARGS1(P_MTLOutputInc),		single,
	IDOPT_MTLOUTPUTUUID,	"iid[ ]%1",			OPTARGS1(P_MTLOutputUuid),		single,
	IDOPT_MTLOUTPUTDIR,		"out[ ]%1",			OPTARGS1(P_MTLOutputDir),		single,
	IDOPT_MTLMTLCOMPATIBLE,	"mktyplib203%T1",	OPTARGS1(P_MTLMtlCompatible), 	single,
	IDOPT_MTLOICF,			"Oicf%T1",			OPTARGS1(P_MTLOicf), 		single,
	IDOPT_MTLNOCLUTTER,		"o[ ]%1",			OPTARGS1(P_MTLNoClutter), 		single,
	IDOPT_UNKNOWN_OPTION,		"",				NO_OPTARGS,						single,
	IDOPT_UNKNOWN_STRING,		"",				NO_OPTARGS,						single,
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
	OPTDEF_BOOL(MTLOicf, FALSE)
	OPTDEF_PATH(MTLNoClutter, "")
END_OPTDEF_MAP()

IMPLEMENT_DYNCREATE(CMkTypLibPageTab, COptionMiniPage)
IMPLEMENT_DYNCREATE(CMkTypLibGeneralPage, CMkTypLibPageTab)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////
BOOL OPTION_HANDLER(MkTypLib)::IsFakeProp (UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_MTLOutputTlb);
}

//////////////////////////////////////////////////////////////
UINT OPTION_HANDLER(MkTypLib)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_MTLOutputTlb)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

//////////////////////////////////////////////////////////////
void OPTION_HANDLER(MkTypLib)::OnOptionStrPropChange(UINT idProp, const CString & strVal)
{
}

//////////////////////////////////////////////////////////////
void OPTION_HANDLER (MkTypLib)::FormFakeStrProp (UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// ASSERT this is a MkTypLib 'fake' string prop ?
	ASSERT (IsFakeProp (idProp));
	
	// get the project base-name (ie. without extension)
	CString strProj, strBase;
	CPath pathProj;
	const TCHAR * pchT;

	if ((m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)) &&
	     ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CFileItem)))
		||
		m_pSlob->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		if (idPropL == P_MTLOutputTlb)
		{
			VERIFY(m_pSlob->GetStrProp(P_ProjItemName, strProj) == valid);
			VERIFY(pathProj.Create(strProj));
			pathProj.GetBaseNameString(strBase);
	
			// which output directory do we want to use?
			UINT idOutDirProp = GetFakePathDirProp(idProp);
			ASSERT(idOutDirProp != (UINT)-1);

			CProjItem * pItem = (CProjItem *)m_pSlob;
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
				// If the output directory doesn't end in a forward slash
				// or a backslash, append one.
				pchT = (const TCHAR *)strVal + strVal.GetLength();
				pchT = _tcsdec((const TCHAR *)strVal, (TCHAR *)pchT);

				if (*pchT != _T('/') && *pchT != _T('\\'))
					strVal += _T('/');
			}

			strVal += strBase + _TEXT(".tlb") ;
		}
		else ASSERT (FALSE) ;
	}
	else
		strVal.Empty() ;
}

/////////////////////////////////////////////////////////////////////////
GPT OPTION_HANDLER (MkTypLib)::GetDefStrProp(UINT idProp, CString & val)
{
	if (!IsFakeProp (idProp))
		return COptionHandler::GetDefStrProp(idProp, val) ;	

	FormFakeStrProp (idProp, val) ;
	return valid ;
}
/////////////////////////////////////////////////////////////////////////
BOOL OPTION_HANDLER (MkTypLib)::CheckDepOK (UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fValid = TRUE;

	if (idPropL == P_MTLOutputTlb || idPropL == P_MTLOutputInc|| idPropL == P_MTLOutputUuid)
	{
		fValid = m_pSlob->IsKindOf (RUNTIME_CLASS (CFileItem)) ||
				 (m_pSlob->IsKindOf (RUNTIME_CLASS (CProxySlob)) &&
				  ((CProxySlob *)m_pSlob)->IsSortOf (RUNTIME_CLASS (CFileItem)));
	}
	return fValid;
} 
		
// MkTypLib tool option property page
CRuntimeClass * g_mtlTabs[] = 
{
	RUNTIME_CLASS(CMkTypLibGeneralPage),
	(CRuntimeClass *)NULL,
};

// MkTypLib option property page

BEGIN_IDE_CONTROL_MAP (CMkTypLibGeneralPage, IDDP_PROJ_MKTYPLIB, IDS_MKTYPLIB)
	MAP_CHECK (IDC_MTLNOLOGO, P_MTLNologo)
	MAP_EDIT (IDC_MTLINCLUDES, P_MTLIncludes)
	MAP_EDIT (IDC_MTLMACROS, P_MTLMacros) 
	MAP_EDIT (IDC_MTLOUTPUTTLB, P_MTLOutputTlb)
	MAP_EDIT (IDC_MTLOUTPUTINC, P_MTLOutputInc)
	MAP_EDIT (IDC_MTLOUTPUTUUID, P_MTLOutputUuid)
	MAP_CHECK (IDC_MTLOICF, P_MTLOicf)
	MAP_CHECK(IDC_MTLCOMPATIBLE, P_MTLMtlCompatible)
END_IDE_CONTROL_MAP()

/////////////////////////////////////////////////////////////////
void CMkTypLibGeneralPage::InitPage()
{
	COptionMiniPage::InitPage();

	// we need to disable TLB and Include File dialog item
	// if we are not in CFileItem (ie. we have no container)
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	if (!((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CFileItem)) 
           )
	{
		GetDlgItem(IDC_MTLOUTPUTTLB)->EnableWindow(FALSE) ;
		GetDlgItem(IDC_MTLOUTPUTINC)->EnableWindow(FALSE) ;
		GetDlgItem(IDC_MTLOUTPUTUUID)->EnableWindow(FALSE) ;
		GetDlgItem(IDC_MTLTEXT1)->EnableWindow(FALSE) ;
		GetDlgItem(IDC_MTLTEXT2)->EnableWindow(FALSE) ;
	}
}
/////////////////////////////////////////////////////////////////
BOOL CMkTypLibPageTab::Validate()
{
	if (m_nValidateID == IDC_MTLOUTPUTTLB)
	{
		if (!Validate_Destination (IDC_MTLOUTPUTTLB, // window ID
								IDS_DEST_TLB, // description stringID
								"tlb", 	// must have extension
								FALSE,	// must be directory
								TRUE))  // can't be directory
			return FALSE;
	}

	if (m_nValidateID == IDC_MTLOUTPUTINC)
	{
		if (!Validate_Destination (IDC_MTLOUTPUTINC, // window ID
								IDS_DEST_INCLUDE, // description string ID
								"h",	// must have extension
								FALSE, 	// must be directory
								TRUE)) // can't be directory
			return FALSE;
	}

	if (m_nValidateID == IDC_MTLOUTPUTUUID)
	{
		if (!Validate_Destination (IDC_MTLOUTPUTUUID, // window ID
								IDS_DEST_UUID, // description string ID
								"c",	// must have extension
								FALSE, 	// must be directory
								TRUE)) // can't be directory
			return FALSE;
	}

	if (m_nValidateID == IDC_OPTSTR)
	{
		// validate tlb file first, then check for include file as well
		if (!Validate_DestinationProp (MapLogical(P_MTLOutputTlb),
									IDOPT_MTLOUTPUTTLB,
									IDC_OPTSTR, IDS_DEST_TLB,
									"tlb", // must have extension
									FALSE, // must be directory
									TRUE)) // can't be directory
			return FALSE ;

		if (!Validate_DestinationProp (MapLogical(P_MTLOutputUuid),
									IDOPT_MTLOUTPUTUUID,
									IDC_OPTSTR, IDS_DEST_UUID,
									"c", // must have extension
									FALSE, // must be directory
									TRUE)) // can't be directory
			return FALSE ;

	// REVIEW: validate P_MtlOutputDir also?

		return Validate_DestinationProp (MapLogical(P_MTLOutputInc),

											IDOPT_MTLOUTPUTINC,
											IDC_OPTSTR, IDS_DEST_INCLUDE,
											"h", // must have extension
											FALSE, // must be directory
											TRUE); // can't be directory

	}
	return COptionMiniPage::Validate();
} 
