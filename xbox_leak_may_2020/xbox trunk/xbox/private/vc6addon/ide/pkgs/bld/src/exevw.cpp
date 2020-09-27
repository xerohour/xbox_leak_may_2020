//
// Exe View Slob
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "resource.h"
#include "bldslob.h"
#include "bldnode.h"
#include "exevw.h"	// our local header

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#undef new
#endif

IMPLEMENT_SERIAL(CExeViewSlob, CProjSlob, 1)

#define theClass CExeViewSlob
BEGIN_SLOBPROP_MAP(CExeViewSlob, CProjSlob)
	STR_PROP(Title)
	BOOL_PROP(IsInvisibleNode)
END_SLOBPROP_MAP()
#undef theClass

CExeViewSlob::CExeViewSlob()
{
	m_bIsInvisibleNode = FALSE;

	m_pProject = g_pActiveProject;
}

CExeViewSlob::~CExeViewSlob()
{
	GetBuildNode()->RemoveNode(this);
}

void CExeViewSlob::Serialize(CArchive & ar)
{
	CProject * pProject = g_pActiveProject;

    if (ar.IsStoring())
    {
    }
    else
    {
 		if (pProject->IsExeProject())
		{
			SetIntProp(P_IsInvisibleNode, FALSE);
			GetBuildNode()->AddNode(this);
		}
		else
		{
			SetIntProp(P_IsInvisibleNode, TRUE);
		}
    }
}

void CExeViewSlob::GetCmdIDs(CWordArray& aVerbs, CWordArray& aCmds)
{
    // Not really much available for exe projects

    // FUTURE (colint): We may want to add useful commands
    // such as Debug, Stop debugging etc.. here

    // Build settings can be done for an exe project
    aCmds.Add(IDM_PROJECT_SETTINGS_POP);
}

BOOL CExeViewSlob::SetIntProp(UINT idProp, int val)
{
	BOOL bRetVal;

	switch (idProp)
	{
		case P_IsExpandable:
		case P_IsExpanded:
		case P_IsInvisibleNode:
			bRetVal = CSlob::SetIntProp(idProp, val);
			break;

		default:
			bRetVal = m_pProject->SetIntProp(idProp, val);
			break;
	}

	return bRetVal;
}

BOOL CExeViewSlob::SetStrProp(UINT idProp, const CString& val)
{
	// We should not be setting the title for a wrapper item
	ASSERT(idProp != P_Title);

	return m_pProject->SetStrProp(idProp, val);
}

GPT CExeViewSlob::GetIntProp(UINT idProp, int &val)
{
  	GPT gpt;
	
	switch (idProp)
	{
		case P_IsExpandable:
			val = FALSE;
			gpt = valid;
			break;

		case P_IsExpanded:
		case P_IsInvisibleNode:
			gpt = CSlob::GetIntProp(idProp, val);
			break;

		default:
			gpt = m_pProject->GetIntProp(idProp, val);
			break;
  	}

	return gpt;
}

GPT CExeViewSlob::GetStrProp(UINT idProp, CString &val)
{
 	if (idProp == P_Title)
	{
  		idProp = P_ProjItemName;
	}

	return m_pProject->GetStrProp(idProp, val);
}

BOOL CExeViewSlob::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	BOOL bRet = m_pProject->SetupPropertyPages(pNewSel, bSetCaption);

	CProjSlob::SetupPropertyPages(pNewSel, FALSE);

	return bRet;
}
