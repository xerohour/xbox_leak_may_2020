//
// Implementation for CUpdateDepDlg class
//
//////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard AFX include
#pragma hdrstop
#include "updepdlg.h"
#include "resource.h"
#include "msgboxes.h"
#include "bldiface.h"
#include "projitem.h"
#include "toolcplr.h"

#include <srcapi.h>
#include <srcguid.h>

#include "mrdepend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
extern BOOL g_bUpdateDep;

CMapStringToPtr CUpdateDepDlg::m_mapSelected;

///////////
// CUpdateDepDlg IMPLEMENTATION
///////////
CUpdateDepDlg::CUpdateDepDlg (CWnd * pParent /* = NULL */)
 			: C3dDialog (CUpdateDepDlg::IDD, pParent) 
{
	m_pProject = g_pActiveProject;
	SetHelpID(IDD);
}

BEGIN_MESSAGE_MAP (CUpdateDepDlg, C3dDialog)
	//{{AFX_MSG_MAP (CUpdateDepDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP	()

///////////////////////////////
// BOOL CUpdateDepDlg::OnInitDialog()
///////////////////////////////
BOOL CUpdateDepDlg::OnInitDialog()
{
	CString	strCurrentConfigName ;
	CString strProject, strConfig;
	CListBox * plbTarget = (CListBox *)GetDlgItem (IDC_TARGET_LIST) ;
	int i, iCurSel, nConfigs;
	const ConfigurationRecord * pcr;
	CPlatform * pPlatform;
	const CProject * pProject = NULL;
	ASSERT (m_pProject != NULL);

	// Subclass the check listbox
	VERIFY(m_lbConfigs.SubclassDlgItem(IDC_TARGET_LIST, this));
	m_lbConfigs.SetRedraw(FALSE);

	// call base class OnInitDialog
	C3dDialog::OnInitDialog() ;
	// get the project's current configuration
	m_pProject->GetStrProp(P_ProjActiveConfiguration, strCurrentConfigName);

	// First, enumerate all possible configurations and put the supported
	// ones in the list box
	m_lbConfigs.ResetContent();
	CClientDC dc(&m_lbConfigs);
	int width, maxwidth = 0;
	CProject::InitProjectEnum();
	while ((pProject = CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		const CPtrArray & ppcr = *pProject->GetConfigArray();
		nConfigs = ppcr.GetSize();
		for (i = 0; i < nConfigs; i++)
		{
			pcr = (ConfigurationRecord *)ppcr[i];
		
			// Only works for internal projects
			ASSERT(!pProject->m_bProjIsExe);

	 		// If the project is an internal makefile then we must get the
			// platform from the projtype of ppcr[i]
			CProjType * pprojtype;
			VERIFY(g_prjcompmgr.LookupProjTypeByName(pcr->GetOriginalTypeName(), pprojtype));
			pPlatform = pprojtype->GetPlatform();
 
			if (pPlatform->IsSupported() && (pPlatform->GetBuildable()==TRUE) )
			{
				strConfig = pcr->GetConfigurationName();
				int iCur = m_lbConfigs.AddString(strConfig);
				if (iCur !=LB_ERR)
					m_lbConfigs.SetCheck(iCur, FALSE);

				width = dc.GetTextExtent(strConfig, strConfig.GetLength()).cx;
				maxwidth = __max(maxwidth, width);
			}
		}
	}
		
	// might need horz scrollbars
	m_lbConfigs.SetHorizontalExtent(maxwidth + 14); // add width of checkbox

	// update # of configs actually supported
	nConfigs = m_lbConfigs.GetCount();

	iCurSel = plbTarget->FindStringExact (-1, (const TCHAR *)strCurrentConfigName) ;
	if (iCurSel!=LB_ERR) 
	{
		plbTarget->SetCurSel(iCurSel);
	}

	if( m_mapSelected.GetCount() )
	{
		for (i = 0; i < nConfigs; i++)
		{
			CString strConfigName;
			m_lbConfigs.GetText(i, strConfigName);
			void *pDummy;
			if( m_mapSelected.Lookup(strConfigName, pDummy) )
			{
				m_lbConfigs.SetCheck(i, TRUE);
			}
		}
	}
	else if (iCurSel!=LB_ERR) 
	{
		m_lbConfigs.SetCheck(iCurSel, TRUE);
	}

	m_lbConfigs.SetRedraw(TRUE);

	return TRUE ;
}

///////////////////////////////
// CUpdateDepDlg::OnOK()
///////////////////////////////
VOID CUpdateDepDlg::OnOK()
{
	C3dDialog::OnOK();
	ScanSelectedConfigs();
}

///////////////////////////////
// CUpdateDepDlg::GetConfigs()
///////////////////////////////
void CUpdateDepDlg::ScanSelectedConfigs()
{
	// Construct the list of configs to build
	int nItem, nConfigs;
	CString strConfigName;
	CString strConfigNameOld;
	CProject * pProject = NULL;
	HBLDTARGET hTarget;

	m_pProject->GetStrProp(P_ProjActiveConfiguration, strConfigNameOld);

	CWaitCursor wc;
	g_VPROJIdeInterface.GetOutputWindow()->ClearOutputWindow();
	g_VPROJIdeInterface.GetOutputWindow()->ShowOutputWindow();

	CString strStatus;
	CString strDone;
	VERIFY( strStatus.LoadString( IDS_UPDATING_DEPENDENCIES ) );
	StatusBeginPercentDone( strStatus );
	VERIFY( strDone.LoadString( IDS_UPDATING_DEPENDENCIES_OWIN ) );

	nConfigs = m_lbConfigs.GetCount();
	BOOL fRefreshDepCtr = FALSE;
	g_bUpdateDep = TRUE;
	// reset keystate for VK_ESCAPE so that scanner won't exit
	// unexpectely.
	GetAsyncKeyState(VK_ESCAPE);

	m_mapSelected.RemoveAll();  // reset selection

	// Prescan all lists so that we can update the progress bar correctly
	int nTotal = 0;
	for (nItem = 0; nItem < nConfigs; nItem++)
	{
		if (m_lbConfigs.GetCheck(nItem))
		{	
			m_lbConfigs.GetText(nItem, strConfigName);
 
			m_mapSelected.SetAt(strConfigName,NULL); // save selection
			hTarget = g_BldSysIFace.GetTarget(strConfigName, NO_BUILDER);
			ASSERT(hTarget);
			pProject = (CProject *)g_BldSysIFace.GetBuilder(hTarget);
			ASSERT(pProject);

			CProjTempConfigChange projTempConfigChange(pProject);
			projTempConfigChange.ChangeConfig(strConfigName);

			CTargetItem* pTarget = pProject->GetTarget(strConfigName);
#ifndef REFCOUNT_WORK
			nTotal += pTarget->GetRegistry()->GetContent()->GetCount(); // quick estimate
#else
			nTotal += pTarget->GetRegistry()->GetCount(); // quick estimate
#endif
		}
	}

	// Scan each config
	int nCurr = 0;
	int nLast = 0;
	for (nItem = 0; nItem < nConfigs; nItem++)
	{
		if (m_lbConfigs.GetCheck(nItem))
		{	
			m_lbConfigs.GetText(nItem, strConfigName);
 
			hTarget = g_BldSysIFace.GetTarget(strConfigName, NO_BUILDER);
			ASSERT(hTarget);
			pProject = (CProject *)g_BldSysIFace.GetBuilder(hTarget);
			ASSERT(pProject);

			CProjTempConfigChange projTempConfigChange(pProject);
			projTempConfigChange.ChangeConfig(strConfigName);

			// Set up a new cache for this target			
			g_ScannerCache.BeginCache();

			CTargetItem* pTarget = pProject->GetTarget(strConfigName);

			CObList ol;
			pTarget->GetBuildableItems(ol);
			if (!ol.IsEmpty())
			{
				// Test for a /Yc file, and parse the PCH file first.
				CProjItem *pYcItem = NULL;

				POSITION pos = ol.GetHeadPosition();
#ifdef _KIP_PCH_
				while (pos)
				{
					CProjItem *pTempItem = (CProjItem *) ol.GetNext( pos );
					ASSERT( pTempItem->IsKindOf( RUNTIME_CLASS(CProjItem) ) );
					if (pTempItem->IsKindOf( RUNTIME_CLASS(CFileItem) ))
					{
						BOOL b;
						CActionSlobList * pActions = pTempItem->GetActiveConfig()->GetActionList();
						if (!pActions->IsEmpty() ) {
							CBuildTool * pCurrentTool = (CBuildTool *)NULL;
							pCurrentTool = ((CActionSlob *)pActions->GetHead())->BuildTool();
							// Only do this is /Yc "somefile.h" is on command line !
							if( (pTempItem->GetIntProp( pCurrentTool->MapLogical(P_PchCreate), b ) == valid) && b ) {
								CString str;
								if( (pTempItem->GetStrProp( pCurrentTool->MapLogical(P_PchCreateUptoHeader), str ) == valid) && str.GetLength() ) {
									pTarget->GetPchMap()->RemoveAll();
									pTempItem->ScanDependencies();
									fRefreshDepCtr = TRUE;
									pYcItem = pTempItem;
								}
							}
						}
					}
				}

				// Now scan the deps. If a file was scanned in the pch phase,
				// ignore it.
				pos = ol.GetHeadPosition();
#endif // _KIP_PCH_

				// Important optimization!!
				// Try to get a pointer to the minimal rebuild interface so that calls
				// to IMreDependencies::GetMreDependenciesIFace within the scanning loop
				// will get the same pointer instead of creating a new interface pointer
				// each time.
				IMreDependencies* pMreDepend = NULL;
				if (g_bNewUpdateDepModel)
				{
					// Get IMreDependencies* so that it is cached.
					CPath pathIdb = pProject->GetMrePath(pProject->GetActiveConfig());
					if (!pathIdb.IsEmpty())
					{
						// Minimal rebuild dependency interface.
						pMreDepend = IMreDependencies::GetMreDependenciesIFace((LPCTSTR)pathIdb);
					}
				}

				while (pos)
				{
					CProjItem *pTempItem = (CProjItem *) ol.GetNext( pos );
					ASSERT( pTempItem->IsKindOf( RUNTIME_CLASS(CProjItem) ) );

					if( pTempItem == pYcItem ) continue;

					ASSERT_VALID( pTempItem );
					nCurr++;
					if (pTempItem->IsKindOf( RUNTIME_CLASS(CFileItem) ))
					{
						pTempItem->ScanDependencies();
						fRefreshDepCtr = TRUE;

						// update the indicator 20 times at most
						int nNew = nCurr * 100 / nTotal;
						if( (nNew/5) > nLast ){
							StatusPercentDone( nNew );
							nLast = nNew/5;
						}
					}
				}
				if (fRefreshDepCtr)
					pTarget->RefreshDependencyContainer();

				if (NULL!=pMreDepend)
					pMreDepend->Release();

				pProject->m_nScannedConfigIndex = -1; // force rescan during next export makefile
			}

			strStatus = strConfigName + _TEXT(" -- ") + strDone;
			g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow( strStatus, FALSE, TRUE );
			g_ScannerCache.EndCache();
		}
	}
	g_bUpdateDep = FALSE;

	//loop through g_FileForceUpdateListQ and remove the files updated from the list
	POSITION pos = g_FileForceUpdateListQ.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{	
		CDepUpdateRec *pdepRec = (CDepUpdateRec *)g_FileForceUpdateListQ.GetNext(pos);
		delete pdepRec;			
	}
	g_FileForceUpdateListQ.RemoveAll();

	StatusPercentDone(100);	// Let the user see this.
	StatusEndPercentDone();
}

///////////////////////////////
// CUpdateDepDlg::OnCancel()
///////////////////////////////
VOID CUpdateDepDlg::OnCancel()
{
	C3dDialog::OnCancel();
}

BOOL CUpdateDepDlg::LoadSelFromOpt(CArchive & archive)
{
	TRY
	{

		// Construct the list of configs to build
		int nItem, nConfigs;

		archive >> nConfigs;
		
		m_mapSelected.RemoveAll();
		for (nItem = 0; nItem < nConfigs; nItem++)
		{
			CString strConfigName;

			archive >> strConfigName;
			m_mapSelected.SetAt(strConfigName,NULL);
		}
	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
 	END_CATCH_ALL
	
	return TRUE;	// succeed
}


BOOL CUpdateDepDlg::SaveSelToOpt(CArchive & archive)
{
	TRY
	{
		
		// Construct the list of configs to build
		int nItem, nConfigs;

		nConfigs = m_mapSelected.GetCount();
		POSITION pos;

		archive << (nConfigs);

		pos = m_mapSelected.GetStartPosition();

		for (pos = 	m_mapSelected.GetStartPosition (); pos != NULL;)
		{
			CString strConfigName;
			void *pv;
			m_mapSelected.GetNextAssoc ( pos, strConfigName, pv );

			archive << (strConfigName);
		}

	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;
}
