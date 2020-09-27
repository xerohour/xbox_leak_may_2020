//
// Build Slob
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "resource.h"

#include <prjapi.h>
#include <prjguid.h>

#include "bldslob.h"	// our local header
#include "bldnode.h"
#include "vwslob.h"
#include "targdlg.h"
#include "cursor.h"
#include "oleref.h"
#include <utilauto.h>	// For Automation CAutoApp


#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#undef new
#endif

CBuildIt::CBuildIt(BOOL bFireEvents /* = TRUE */)
{
	m_actualErrs = 0;
	m_warns = 0;
	m_bFireEvents = bFireEvents;
	ASSERT(m_depth==0);

	m_depth++;
	m_mapMissingEnvVars.RemoveAll();
	m_mapConfigurationsBuilt.RemoveAll();

	if (m_bFireEvents)
	{
		theApp.m_pAutoApp->FireBeforeBuildStart();
	}
}

CBuildIt::~CBuildIt() {
	CString strVar;
	void *dummy;
	CErrorContext * pEC = g_buildengine.OutputWinEC();
	POSITION pos = m_mapMissingEnvVars.GetStartPosition();

	if (m_bFireEvents) // REVIEW
	{
		if( pos ){
			CString strMissing;
			strMissing.LoadString(IDS_MISSING_ENV_VAR);
			pEC->AddString(strMissing);
		}
		while( pos ){
			m_mapMissingEnvVars.GetNextAssoc(pos,strVar,dummy);
			// dump strVar to output window.
			strVar = "$(" + strVar + ")";
			pEC->AddString(strVar);
		}
	}

	if (m_bFireEvents)
	{
		theApp.m_pAutoApp->FireBuildFinish(m_actualErrs, m_warns);
	}

	m_depth--;
}

CMapStringToPtr  CBuildIt::m_mapConfigurationsBuilt;
CMapStringToPtr  CBuildIt::m_mapMissingEnvVars;
int 			 CBuildIt::m_actualErrs = -1;
int 			 CBuildIt::m_warns = -1;
int 			 CBuildIt::m_depth = 0;
BOOL			 CBuildIt::m_bFireEvents = FALSE;

#define MENU_TEXT_CCH 200

#define SCC_FOLDERS

IMPLEMENT_SERIAL(CBuildSlob, CProjSlob, 1)

BOOL CBuildSlob::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (nID == IDM_SLOB_DEFAULTCMD && nCode == CN_COMMAND)
	{
		// If we represent a folder then let the project window code do the default thing
		if (!GetContentList()->IsEmpty())
			return FALSE;
	}

	// WARNING! This does not fall through to a message map.  You must
	// handle both CN_COMMAND and CN_UPDATE_COMMAND_UI if you want your
	// commands to be enabled.

	if (nCode == CN_COMMAND && pExtra == NULL)
	{
		switch (nID)
		{
		case IDM_PROJITEM_OPEN:
			OnOpenItem();
			return TRUE;
		case IDM_PROJITEM_CCOMPILE:
			OnCompileItem();
			return TRUE;
		case IDM_PROJITEM_CBUILD:
			OnBuild();
			return TRUE;
		case IDM_PROJITEM_CBUILD_NO_DEPS:
			OnBuildNoDeps();
			return TRUE;
		case IDM_PROJECT_CLEAN:
			OnClean();
			return TRUE;
		case IDM_PROJITEM_ADD_GROUP:
			OnAddGroup();
			return TRUE;
		case IDM_SLOB_DEFAULTCMD:
			OnOpenItem();
			return TRUE;
		case IDM_INSERT_FILES_POP:
			OnPopupFilesIntoProject();
			return TRUE;
		}
	}
	else if (nCode == CN_UPDATE_COMMAND_UI)
	{
		ASSERT(pExtra != NULL);
		CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		ASSERT(pCmdUI->m_nID == nID);           // sanity assert


		switch (nID)
		{
		case IDM_PROJITEM_OPEN:
			OnUpdateOpenItem(pCmdUI);
			return TRUE;
		case IDM_PROJITEM_CCOMPILE:
			OnUpdateCompileItem(pCmdUI);
			return TRUE;
		case IDM_PROJITEM_CBUILD:
		case IDM_PROJITEM_CBUILD_NO_DEPS:
			OnUpdateBuild(pCmdUI);
			return TRUE;
		case IDM_PROJECT_CLEAN:
			OnUpdateClean(pCmdUI);
			return TRUE;
		case IDM_PROJITEM_ADD_GROUP:
			OnUpdateAddGroup(pCmdUI);
			return TRUE;
		case IDM_SLOB_DEFAULTCMD:
			pCmdUI->Enable(TRUE);
			return TRUE;
		case IDM_INSERT_FILES_POP:
			OnUpdateFilesIntoProject(pCmdUI);
			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildSlob property map

#define theClass CBuildSlob
BEGIN_SLOBPROP_MAP(CBuildSlob, CProjSlob)
	STR_PROP(ProjWinTitle)
	BOOL_PROP(IsExpanded)
	BOOL_PROP(IsExpandable)
	LONG_PROP(IPkgProject)
	SLOB_PROP(ProxyContainer)
END_SLOBPROP_MAP()
#undef theClass

/////////////////////////////////////////////////////////////////////////////
// Image wells used to draw our glyphs

extern CImageWell g_imageWell;	// project item glyphs

/////////////////////////////////////////////////////////////////////////////
// HACK ALERT, HACK ALERT, here are the infamous state variables

// Are we currently processing an inform
BOOL CBuildSlob::m_bThunkMoves = TRUE;

/////////////////////////////////////////////////////////////////////////////
// The inform list

CObList CBuildSlob::m_lstInform;

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob construction/destruction

CBuildSlob::CBuildSlob()
{
	// No item, or config initially
	m_pItem = NULL;
	m_pcr = NULL;

	// Data structures for safe fixup/unfixup.
	m_pTempConfigChange = NULL;
	m_dwFixupRefCount = 0;

	// Not ignoring informs to begin with
	m_bIgnoreInforms = FALSE;

	m_bIsExpanded = FALSE;
	m_bIsExpandable = TRUE;
	m_lIPkgProject = NULL;
	m_pProxyContainer = NULL;
}

CBuildSlob::CBuildSlob(CProjItem * pItem, ConfigurationRecord * pcr)
{
	ASSERT(pItem);

	// Set up our item and config
	m_pItem = pItem;
	m_pcr = pcr;

	// Data structures for safe fixup/unfixup.
	m_pTempConfigChange = NULL;
	m_dwFixupRefCount = 0;

	// Not ignoring informs to begin with
	m_bIgnoreInforms = FALSE;

	m_bIsExpanded = FALSE;
	m_bIsExpandable = TRUE;
	// WinslowF this needs to be initialized for Insert Files Into Folder.
	m_lIPkgProject = NULL;
	m_pProxyContainer = NULL;

	// If this item represents a target, or a target reference
	// then it will need to know about changes in configuration names
	// so we add it to our select list of CBuildSlobs
	if (m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
 		m_lstInform.AddTail(this);
}

CBuildSlob::~CBuildSlob()
{
	ASSERT(m_dwFixupRefCount == 0);
	ASSERT(m_pTempConfigChange == NULL);

	// If we still have an associated item then we'd better make ourselves
	// no longer a dependant of it
	if (m_pItem)
		m_pItem->RemoveDependant(this);

	// If the build slob is in the undo buffer, then it is the
	// sole owner of this item, so delete the item.
	if ((GetContainer() == &theUndoSlob) && (m_pItem != NULL) && (m_pItem->GetContainer() != &theUndoSlob))
		delete m_pItem;
	else if (GetContainer() == &theClipboardSlob )
	{
		//
		// Deleting from clipboard should not record.
		//
		theUndoSlob.Pause();
		delete m_pItem;
		theUndoSlob.Resume();
	}
 
	// Get rid of our contents
 	CObList * pContentList = GetContentList();
	if (pContentList)
	{
		POSITION pos = pContentList->GetHeadPosition();
		while (pos != NULL)
		{
			CBuildSlob *pSlob = (CBuildSlob *)pContentList->GetNext(pos);
			delete pSlob;
		}
	}

	// If we are on the select list of CBuildSlobs that get informs about
	// configuration name changes then get us off of it!!!!
	POSITION pos = m_lstInform.Find(this);
	if (pos != NULL)
		m_lstInform.RemoveAt(pos);
}

CSlobWnd * CBuildSlob::GetSlobWnd() const
{
	CSlobWnd * pSlobWnd = CSlob::GetSlobWnd();
	if (pSlobWnd == NULL)
	{
		CSlob * pViewSlob = GetBuildNode()->GetFirstTarget();
		if (pViewSlob)
			pSlobWnd = pViewSlob->GetSlobWnd();
	}

	return pSlobWnd;
}

/////////////////////////////////////////////////////////////////////////////
// Property page stuff. Basically thunks through to the CProjItems, great eh!!

BOOL CBuildSlob::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	BOOL bRet = m_pItem->SetupPropertyPages(pNewSel, bSetCaption);
	CProjSlob::SetupPropertyPages(pNewSel, FALSE);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::CanAdd, we determine here whether we have any buildslobs to
// paste into this target. Only CBuildSlobs can be pasted in targets

BOOL CBuildSlob::CanAdd(CSlob * pAddSlob)
{
	ASSERT(m_pItem != NULL);
	if ((!m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem))) &&
		(!m_pItem->IsKindOf(RUNTIME_CLASS(CProjGroup))))
		return FALSE;

	if (g_Spawner.SpawnActive())
		return FALSE;

	// Handle multislobs
	if (pAddSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob)) || pAddSlob == &theClipboardSlob)
	{
		POSITION pos = pAddSlob->GetHeadPosition();
		while (pos != NULL)
		{
			// Get the next slob
			CSlob * pSlob = (CSlob *)pAddSlob->GetNext(pos);
			ASSERT(pSlob->IsKindOf(RUNTIME_CLASS(CSlob)));

			// If this is a CBuildSlob then we have something that we can add
			if (!pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
				return FALSE;				

			CProjItem * pItem = ((CBuildSlob *)pSlob)->GetProjItem();
			if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
				continue;

			// we can now pseudo-drop dependencies, which get
			// converted to CFileItems before they are dropped
			if (pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)))
				continue;

			if (!pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
				return FALSE;

			// ensure we're not trying to add a group to itself
			CProjItem * pContainer = m_pItem;
			while ((pContainer != NULL) && (pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup))))
			{
				if (pContainer == pItem)
					return FALSE; // can't add this group
				pContainer = (CProjItem *)pContainer->GetContainer();
			}
		}

		return TRUE;
	}
	// We can only add CBuildSlobs
	else if (pAddSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
	{
		CProjItem * pItem = ((CBuildSlob *)pAddSlob)->GetProjItem();

		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
			return TRUE;

		// we can now pseudo-drop dependencies, which get
		// converted to CFileItems before they are dropped
		if (pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)))
			return TRUE;

		if (!pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
			return FALSE;

		// ensure we're not trying to add a group to itself
		CProjItem * pContainer = m_pItem;
		while ((pContainer != NULL) && (pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup))))
		{
			if (pContainer == pItem)
				return FALSE; // can't add this group
			pContainer = (CProjItem *)pContainer->GetContainer();
		}
		return TRUE;
	}

	// No CBuildSlobs so nothing to add
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Property management. Lotsa thunks. Basically the method here is
// 1, If the property is one of our own (i.e. CBuildSlob) then handle it here
// 2. Else, make sure we are using the right property bag on the CProjItem,
// 3. And put the Project in the config that this wrapper uses.
// 4. Do the required operation.
// 5. Reset everything
// 6. Return

BOOL CBuildSlob::SetIntProp(UINT idProp, int val)
{
	// Handle the wrappers own properties
	if (idProp == P_IsExpandable || idProp == P_IsExpanded)
	{
		if (idProp == P_IsExpanded && val && m_pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		{
			//
			// This is where external dependencies are updated.
			//
			BOOL bNeedsDepUpdate = FALSE;
			CDependencyContainer* pDepCntr = (CDependencyContainer*)m_pItem;
			FixupItem();
			if (NULL==pDepCntr->GetTarget())
			{
				UnFixupItem();
				return FALSE;
			}
			else if (valid==pDepCntr->GetTarget()->GetIntProp(P_TargNeedsDepUpdate,bNeedsDepUpdate) &&
				bNeedsDepUpdate )
			{
#if 0
				if (NULL==pDepCntr->GetHeadPosition())
#endif
					pDepCntr->GetProject()->DoTopLevelScan(FALSE);
				pDepCntr->GetTarget()->SetIntProp(P_TargNeedsDepUpdate,FALSE);

				BOOL bReturnFalse = pDepCntr->GetContentList()->IsEmpty();
				if (bReturnFalse)
				{
					//
					//
					//
					CProject* pProject = pDepCntr->GetProject();
					if (NULL!=pProject)
						pProject->SetIntProp(P_HasExternalDeps,FALSE);

					UnFixupItem();
					//
					// Set to not expandable.
					//
					CSlob::SetIntProp(P_IsExpandable, FALSE);
					return FALSE;
				}
			}
			UnFixupItem();
		}
   		return CSlob::SetIntProp(idProp, val);
	}

	// Ok this should be a property of the projitem so we
	// attempt to thunk through to the real projitem 
	ASSERT_VALID (m_pItem);
	ASSERT_VALID (m_pcr);
	
	if (m_pItem == NULL || m_pcr == NULL)
		return FALSE;

	// Make sure we are using the right property bag
	int nOldBag = m_pItem->UsePropertyBag(CurrBag);

	// Set up the config
	CProject * pProject = m_pItem->GetProject();
  	ConfigurationRecord * pcrBase = (ConfigurationRecord *)m_pcr->m_pBaseRecord;
	CProjTempConfigChange projTempConfigChange(pProject);
 	projTempConfigChange.ChangeConfig(pcrBase);

	// Do the SetIntProp
	BOOL fRet = m_pItem->SetIntProp(idProp, val);

	// Reset the config
	projTempConfigChange.Release();

	// Reset the property bag
	m_pItem->UsePropertyBag(nOldBag);

	// Return the result
 	return fRet;
}

BOOL CBuildSlob::SetStrProp(UINT idProp, const CString& val)
{
	ASSERT_VALID (m_pItem);
	ASSERT_VALID (m_pcr);

	if (m_pItem == NULL || m_pcr == NULL)
		return FALSE;

	// Make sure we are using the right property bag
	int nOldBag = m_pItem->UsePropertyBag(CurrBag);

	// Set up the config
	CProject * pProject = m_pItem->GetProject();
 	ConfigurationRecord * pcrBase = (ConfigurationRecord *)m_pcr->m_pBaseRecord;
	CProjTempConfigChange projTempConfigChange(pProject);
 	projTempConfigChange.ChangeConfig(pcrBase);

	// We should not be setting the title for a wrapper item
	ASSERT(idProp != P_Title);

	// Do the SetStrProp
	BOOL fRet = m_pItem->SetStrProp(idProp, val);

	// Reset the config
	projTempConfigChange.Release();

	// Reset the property bag
	m_pItem->UsePropertyBag(nOldBag);

	// Return the result
	return fRet;
}

GPT CBuildSlob::GetIntProp(UINT idProp, int &val)
{
	// Handle the wrappers own properties
	if (idProp == P_IsExpandable || idProp == P_IsExpanded || idProp == P_HasGlyphTipText)
	{
		switch (idProp)
		{
			case P_IsExpandable:
				//
				// Always TRUE for CDependencyContainer that hasn't been intialized with
				// dependencies yet.
				//
				if (m_pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
				{
					val = !(GetContentList()->IsEmpty());

					// Return TRUE if updating contents has been deferred.
					if (!val)
					{
						FixupItem();
						CDependencyContainer* pDepCntr = (CDependencyContainer*)m_pItem;
						if (NULL!=pDepCntr->GetTarget())
						{
							pDepCntr->GetTarget()->GetIntProp(P_TargNeedsDepUpdate,val);
						}
						UnFixupItem();
					}
				}
				else
					val = !(GetContentList()->IsEmpty());
 				return valid;
	
			case P_IsExpanded:
				return CSlob::GetIntProp(idProp, val);

			case P_HasGlyphTipText:
				val = FALSE; // default
				ASSERT(m_pItem);
				if (m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
				{
 					if (m_pItem->GetProject()->GetFileRegHandle() != NULL)
					{
						val = (g_pSccManager->IsSccInstalled() == S_OK);
					}

				}
				else if (m_pItem->GetFileRegHandle() != NULL)
				{
					val = (g_pSccManager->IsSccInstalled() == S_OK);
				}
				return valid;

			default:
				ASSERT(FALSE);
                		return invalid;
        	}
	}
 
	// Ok this should be a property of the projitem so we
	// attempt to thunk through to the real projitem 

	if (m_pItem == NULL || m_pcr == NULL)
		return invalid;

	ASSERT_VALID (m_pItem);
	
	if (m_pcr->m_pBaseRecord != m_pcr->m_pBaseRecord->m_pBaseRecord)
		return invalid;	   // could happen during delete

	ASSERT_VALID (m_pcr);  // REVIEW: make sure we never hit this

	// Make sure we are using the right property bag
	int nOldBag = m_pItem->UsePropertyBag(CurrBag);
	CProject * pProject = m_pItem->GetProject();

	// this can happen during cut & paste, etc.
	if (pProject == NULL)
		return invalid;

	// Set up the config
 	ConfigurationRecord * pcrBase = (ConfigurationRecord *)m_pcr->m_pBaseRecord;
	CProjTempConfigChange projTempConfigChange(pProject);
 	projTempConfigChange.ChangeConfig(pcrBase);

	// Do the GetIntProp
	GPT gpt = m_pItem->GetIntProp(idProp, val);

	// Reset the config
	projTempConfigChange.Release();

	// Reset the property bag
	m_pItem->UsePropertyBag(nOldBag);

	// Return the result
	return gpt;
}

GPT CBuildSlob::GetStrProp(UINT idProp, CString &val)
{
	if (m_pItem == NULL || m_pcr == NULL)
		return invalid;

	ASSERT_VALID (m_pItem);
	if (NULL==m_pItem->ConfigRecordFromBaseConfig(const_cast<ConfigurationRecord*>(m_pcr->m_pBaseRecord)))
		return invalid;	   // could happen during delete

	ASSERT_VALID (m_pcr); // REVIEW: make sure we never hit this

	CProject * pProject = m_pItem->GetProject();
	if (pProject == NULL)
		return invalid;

    GPT gpt = invalid;

	// Make sure we are using the right property bag
	int nOldBag = m_pItem->UsePropertyBag(CurrBag);

	// Set up the config
	ConfigurationRecord * pcrBase = (ConfigurationRecord *)m_pcr->m_pBaseRecord;
	CProjTempConfigChange projTempConfigChange(pProject);
 	projTempConfigChange.ChangeConfig(pcrBase);

	// If we are getting the title of this node then we
	// check to see if this is a wrapper around a Target
	// item, if so we want to get the name of the primary
	// target file produced by this target as the title
	if (idProp == P_Title)
	{
 		if (m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		{
			static CString str;
			if (str.IsEmpty())
				VERIFY(str.LoadString(IDS_SP_FILES));
/*bc
			const CPath * pPath = pProject->GetTargetFileName();
		
            CTargetItem * pTarget = (CTargetItem *)m_pItem;

			// Have to reset config so that we handle non-matching configs correctly
			projTempConfigChange.Reset();
			GetBldSysIFace()->GetTargetNameFromFileSet((HFILESET)pTarget, val, (HBUILDER)pProject, TRUE);
			projTempConfigChange.ChangeConfig(pcrBase);
            
			// We may fail to get the primary target filename
			// in which case we revert to the configuration name
			if (pPath)
 				val = val + _T(" (") + pPath->GetFileName() + _T(')');
 */
			val = m_pItem->GetTargetName();
			// g_BldSysIFace.GetFileSetName((HFILESET)m_pItem, val, (HBUILDER)pProject);
			val += str;

			gpt = valid;
		}
		// Everything else just gets the projitem name normally
		else
			gpt = m_pItem->GetStrProp(P_ProjItemName, val);
	}
	else if (idProp == P_ProjWinTitle)
		gpt = CSlob::GetStrProp(idProp, val);
    else
	// Do the GetStrProp
	    gpt = m_pItem->GetStrProp(idProp, val);

	// reset and free up the critical section.
	projTempConfigChange.Release();

	// Reset the property bag
	m_pItem->UsePropertyBag(nOldBag);
 
	// Return the result
	return gpt;
}

BOOL CBuildSlob::DrawGlyph(CDC *pDC, LPCRECT lpRect)
{
	// Is there a matching configuration
	CString strConfig;
    CTargetItem * pTarget = m_pItem->GetTarget();
	if (!pTarget)
	{
		ASSERT(0);
		return FALSE;
	}

	CProject * pProject = pTarget->GetProject();
	ASSERT(pProject);


	// Draw container glyphs
 	CObList * pContentList = GetContentList();
	if ((m_pItem!=NULL) && ((m_pItem->IsKindOf(RUNTIME_CLASS(CProjGroup))) ||
		(m_pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))))

	{
		return FALSE;	// Default is to use project workspace's glyphs
  	}

	// 0: part of build, 1: excluded from build; 2: not buildable
	ASSERT(m_pItem);

	FixupItem();

	const CPath * pPath = m_pItem->GetFilePath();

	int nGlyph = 2;

	if (m_pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		BOOL bExcluded = FALSE;
		if ((GetIntProp(P_ItemExcludedFromBuild, bExcluded) == valid) && (bExcluded))
		{
			nGlyph = 1;
		}
		else if (((CFileItem*)m_pItem)->GetSourceTool() != (CBuildTool *)NULL)
		{
			nGlyph = 0;
		}
	}
	else if (m_pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)) ||
		 m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
	{
		CProject * pRefProject = NULL;
		if (m_pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		{
			BOOL bExcluded = TRUE;
			GetIntProp(P_ItemExcludedFromBuild, bExcluded);
			nGlyph = bExcluded ? 10 : 7;

			CTargetItem * pRefTarget = ((CProjectDependency *)m_pItem)->GetProjectDep();
			if (pRefTarget != NULL)
				pRefProject = pRefTarget->GetProject();
		}
		else
		{
			nGlyph = 4; // project glyph
			pRefProject = pProject;
		}

		// different glyphs for different proj types
		if (pRefProject != NULL)
		{
			// need to get path from assoc. project for these
			pPath = pRefProject->GetFilePath();
			nGlyph += pRefProject->GetGlyphIndex(); // 0, 1 or 2
		}
	}

	int status = 0;
	BOOL bIsSccActive = (g_pSccManager->IsSccActive() == S_OK);
	if (bIsSccActive && (pPath != NULL))
	{
		g_pSccManager->GetStatus(&status, (LPCTSTR)*pPath);	
		if ((status != -1) && ((status & (SCC_STATUS_CONTROLLED|SCC_STATUS_DELETED))==SCC_STATUS_CONTROLLED))
		{
			nGlyph += 13;
			if ((status & SCC_STATUS_SHARED)!=0)
			{
				nGlyph +=13;
			}
		}
	}


	CPoint	pt(lpRect->left, lpRect->top);
	g_imageWell.DrawImage(pDC, pt, nGlyph);

	// may need to draw SCC glyph as well (overlay)
	if (status > 0)
	{
		ASSERT(g_pSccManager!=NULL);
		g_pSccManager->DrawGlyph(pDC, lpRect, status, FALSE);
	}

	UnFixupItem();

	return TRUE;
}

//  Get filename associated with this slob, if any:
const CPath * CBuildSlob::GetFilePath() const
{
	if (m_pItem==NULL)
		return NULL;

	return m_pItem->GetFilePath();
}

FileRegHandle CBuildSlob::GetFileRegHandle() const
{
	if (m_pItem==NULL)
		return NULL;

	return m_pItem->GetFileRegHandle();
}

int CBuildSlob::GetAssociatedFiles(CStringArray & saFiles)
{
#ifdef SCC_FOLDERS
	if (m_pItem != NULL && m_pItem->IsKindOf(RUNTIME_CLASS(CProjGroup))) {
		// recurse for folders
		return Inherited::GetAssociatedFiles(saFiles);
	}
#endif
	const CPath * pPath = GetFilePath();
	if ((pPath==NULL) || (pPath->IsEmpty()))
		return 0;
	
	saFiles.Add((LPCTSTR)*pPath);
	return 1;
}

void CBuildSlob::OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint)
{
	// Check if this inform is about a dependency container, if so then
	// we always accept informs about dep containers
	BOOL bDontIgnoreInform = FALSE;
	if (pChangedSlob->IsKindOf(RUNTIME_CLASS(CProjItem)))
	{
		if (pChangedSlob->IsKindOf(RUNTIME_CLASS(CTargetItem)) && dwHint &&
			((CProjItem *)dwHint)->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
			bDontIgnoreInform = TRUE;
	}

	// Are we ignoring informs at the moment
	if (m_bIgnoreInforms && !bDontIgnoreInform)
		return;

 	ASSERT_VALID (pChangedSlob);
	if (!pChangedSlob->IsKindOf(RUNTIME_CLASS(CProjItem)) && pChangedSlob != m_pItem)
	{
		CSlob::OnInform(pChangedSlob, idChange, dwHint);
		return;
	}

	UnThunkMoves();

	// shouldn't use these notifications with project stuff
	ASSERT (idChange != SN_SAVED && idChange != SN_LOADED &&
			idChange != SN_DIRTY && idChange != SN_CLEAN);

	switch (idChange)
	{
		case SN_DESTROY_CONFIG:
		{
			// Is the config that is going away the one that we
			// care about
			ConfigurationRecord * pcr = (ConfigurationRecord *)dwHint;

			if (m_pcr && pcr && pcr->m_pBaseRecord == m_pcr->m_pBaseRecord)
			{
				ASSERT_VALID (m_pcr);

				if (m_pItem->IsKindOf (RUNTIME_CLASS (CTargetItem)))
				{
					// Ok our config is going away, so we'd better go too
					GetBuildNode()->RemoveBuildSlob(this);

					// Destroy ourselves.
					theUndoSlob.Pause();
					MoveInto(NULL);
					theUndoSlob.Resume();
				}
				else
				{
					m_pcr = NULL;
				}
			}
			break;
		}

 		case SN_DESTROY:
			// ignore destroy messages
 			// Our item has gone away on us!!
			if (pChangedSlob == m_pItem)
				m_pItem = NULL;

			// We may need to remove ourselves from the build nodes map
//			ASSERT(NULL!=GetBuildNode());
			if (NULL!=GetBuildNode())
				GetBuildNode()->RemoveBuildSlob(this);

			CSlob::OnInform (pChangedSlob, idChange, dwHint);

			// Delete us
			theUndoSlob.Pause();
			MoveInto(NULL);
		 	theUndoSlob.Resume();
			break;
 	
		case SN_ADD:
		{
 			// Create the new wrappers here
            CProjItem * pItem = (CProjItem *)dwHint;

			// This had better be a notification from a CProjItem
			if (pItem->IsKindOf(RUNTIME_CLASS(CProjItem)))
            {
				if (!theUndoSlob.InUndoRedo() || (!pItem->CanAct(act_insert_into_undo_slob)))
				{
					ASSERT_VALID (m_pcr);

					if (!pItem->CanAct(act_insert_into_undo_slob))
						theUndoSlob.Pause();

					ConfigurationRecord * pcrBase = (ConfigurationRecord *)m_pcr->m_pBaseRecord;
					GetBuildNode()->CreateSlobs(pItem, pcrBase, this);
					if (!pItem->CanAct(act_insert_into_undo_slob))
						theUndoSlob.Resume();
				}
            }
            else
 			    // pass onto the base-class
			    CSlob::OnInform (pChangedSlob, idChange, dwHint);
				 
			break;
		}

		case SN_REMOVE:
		{
 			// Remove wrapper slobs here
            CProjItem * pItem = (CProjItem *)dwHint;

			// This had better be a notification from a CProjItem
			if (pItem->IsKindOf(RUNTIME_CLASS(CProjItem)))
            {
  				CObList * pContentList = GetContentList();
				if (pContentList)
				{
					POSITION pos = pContentList->GetHeadPosition();
					while (pos != NULL)
					{
						CBuildSlob * pSlob = (CBuildSlob *)pContentList->GetNext(pos);
						if (pSlob->GetProjItem() == pItem && (bDontIgnoreInform || (!pSlob->IgnoringInforms())))
						{
							// Found wrapper corresponding to the item being removed
							// so lets get rid of the wrapper itself!
 							ASSERT(pItem->CanAct(act_insert_into_undo_slob) || (!theUndoSlob.IsRecording()));
							CBuildSlob * pNextSlob;
							while ((pos != NULL) && ((pNextSlob = (CBuildSlob *)pContentList->GetAt(pos))->GetProjItem() != pItem) && (!pNextSlob->CanAct(act_insert_into_undo_slob))) (void)GetNext(pos);

							pSlob->MoveInto(NULL);
 							// break;
						}
					}
				}
            }
            else
  			    // pass onto the base-class
			    CSlob::OnInform (pChangedSlob, idChange, dwHint);
   
			break;
		}

		case P_SccStatus:
		{

			InformDependants(P_ProjWinTitle);

			// pass onto the base-class
			CSlob::OnInform (pChangedSlob, idChange, dwHint);
			break;
		}

		default:
		{
			// Check to see if this prop notify is to do with the right config.
			CProjItem * pItem = (CProjItem *)pChangedSlob;

			// This had better be a notification from a CProjItem
			if (pItem->IsKindOf(RUNTIME_CLASS(CProjItem)))
			{
				ConfigurationRecord * pcr = pItem->GetActiveConfig();
				if (pcr == NULL || m_pcr == NULL)
					break;

				if (pcr->m_pBaseRecord != m_pcr->m_pBaseRecord)
					break;
		
				// only do this if the item changing concerns our config.
				if (((CProjItem *)pChangedSlob)->UsePropertyBag() != CurrBag)
					break;
		
				// notify the view of this change if needed
				// o has the containment changed for a slob?
				// o has the view of the slob changed?
				// o has the title text or excluded from build state changed?
				// o has the name of the project node in the window changed? (config. change)
				if (idChange == P_Container || idChange == P_GroupName || 
					idChange == P_ItemExcludedFromBuild || idChange == P_ID || 
					idChange == SN_CONTENT || idChange == SN_FILE_NAME || 
					idChange == P_ProjConfiguration || idChange == P_TargetName)
				{
					// Do something here to handle view changes!!!
					ASSERT(idChange);

					if (idChange == P_ItemExcludedFromBuild || idChange == SN_FILE_NAME || 
						idChange == P_ProjConfiguration || idChange == P_TargetName)
						InformDependants(P_ProjWinTitle);
 				}
			}

			// pass onto the base-class
			CSlob::OnInform (pChangedSlob, idChange, dwHint);
			break;
		}
	}

	ThunkMoves();
}

void CBuildSlob::SetMatchingConfig(LPCTSTR pszConfig, BOOL bContents /* = TRUE */)
{
	m_pcr = m_pItem->ConfigRecordFromConfigName(pszConfig);
 	ASSERT(m_pcr);
	if (!m_pcr)
		return;

	if (bContents && (GetContentList() != NULL))
	{
		POSITION pos = GetContentList()->GetHeadPosition();
		while (pos != NULL)
		{
			CBuildSlob *pSlob = (CBuildSlob *)GetContentList()->GetNext(pos);
			pSlob->SetMatchingConfig(pszConfig, bContents);
		}
	}
}

BOOL CBuildSlob::PreMoveItem(CSlob * & pContainer, BOOL fFromBldSlob, BOOL fToBldSlob, BOOL fPrjChanged)
{
	// If we are moving this into NULL then we'd better
	// disconnect from the real projitem
	//if (pContainer == NULL && m_pItem)
	//	m_pItem->RemoveDependant(this);

    if (fToBldSlob && (!fFromBldSlob) && m_bThunkMoves)
    {
        ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CBuildSlob)));

		((CBuildSlob *)pContainer)->IgnoreInforms();

        CProjItem * pItem = ((CBuildSlob *)pContainer)->GetProjItem();
//		theUndoSlob.Pause();
		m_pItem->MoveInto(pItem);
// 		theUndoSlob.Resume();

		if (m_pcr == NULL)
		{
			// Set up the pcr for the wrapper
			ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CBuildSlob)));
			// don't try to set if parent not set yet
			if (((CBuildSlob *)pContainer)->GetConfig() != NULL)
			{
				CString strConfig = ((CBuildSlob *)pContainer)->GetConfigurationName();
 				SetMatchingConfig(strConfig);
				ASSERT(m_pcr);
			}
		}

#if 0
		if ((!GetContentList()) || (GetContentList()->IsEmpty()))
		{
			ConfigurationRecord * pcrBase =  NULL;
			if (m_pcr)
				pcrBase = (ConfigurationRecord *)m_pcr->m_pBaseRecord;
			
			// need to reconstruct contents at this point
			if (m_pItem->GetContentList() != NULL)
			{
				POSITION pos = m_pItem->GetContentList()->GetHeadPosition();
				while (pos != NULL)
				{
					CProjItem * pItem = (CProjItem *)m_pItem->GetContentList()->GetNext(pos);
#ifdef _DEBUG
					CBuildSlob * pChild = 
#endif
					GetBuildNode()->CreateSlobs(pItem, pcrBase, this, TRUE);
				}
			}
		}
#endif

#if 0	// bogus hack
		// if moving from undo or clipboard, net to ensure our contents are hooked up properly
		if (GetContentList())
		{
			POSITION pos = GetContentList()->GetHeadPosition();
			while (pos != NULL)
			{
				CBuildSlob *pSlob = (CBuildSlob *)GetContentList()->GetNext(pos);
				CSlob * pThis = this;
				pSlob->PreMoveItem(pThis, fFromBldSlob, fToBldSlob, fPrjChanged);
			}
		}
#endif
		((CBuildSlob *)pContainer)->UnIgnoreInforms();
    }
	else if (fToBldSlob && fFromBldSlob)
	{
        ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CBuildSlob)));

		((CBuildSlob *)pContainer)->IgnoreInforms();
		((CBuildSlob *)GetContainer())->IgnoreInforms();

		// FUTURE (billjoy) we REALLY need to find out why HoldUpdates
		// doesn't keep paint messages from coming thru when we have to 
		// put up the rc dialog ("you can only have one rc file in a 
		// project" or something).  The paint causes a GetStrProp for the
		// title of this rc slob, but m_pcr hasn't been set yet (it is
		// set a few lines down).  The same change should be made to 
		// fix the code in the above "if" block
        CProjItem * pItem = ((CBuildSlob *)pContainer)->GetProjItem();
		theUndoSlob.Pause();
		m_pItem->MoveInto(pItem);
 		theUndoSlob.Resume();

		// Set up the pcr for the wrapper
		ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CBuildSlob)));
				// don't try to set if parent not set yet
		ASSERT(((CBuildSlob *)pContainer)->GetConfig() != NULL);
		CString strConfig = ((CBuildSlob *)pContainer)->GetConfigurationName();
 		SetMatchingConfig(strConfig);

		((CBuildSlob *)GetContainer())->UnIgnoreInforms();
		((CBuildSlob *)pContainer)->UnIgnoreInforms();
	} // UNDONE: handle third case!!!

	return TRUE;
}

BOOL CBuildSlob::MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromBldSlob, BOOL fToBldSlob, BOOL fPrjChanged)
{
	// removing item altogether from project?
	if (fFromBldSlob && (!fToBldSlob) && m_bThunkMoves)
	{
		IgnoreInforms();
		((CBuildSlob *)pOldContainer)->IgnoreInforms();

#if 0
		// Make a copy of the item we are deleting so that the proxy can
		// refer to it.
		CProjItem * pCloneItem = (CProjItem *)m_pItem->Clone();
#endif
 
		// Disconnect associated projitem here
//		theUndoSlob.Pause();
		m_pItem->MoveInto(NULL);
// 		theUndoSlob.Resume();

		SetConfig(NULL);
#if 0
		// Set up our cloned item (no content yet)
 		SetItem(pCloneItem);
#if 0
#else

		// Set up the pcr for content, as it will have changed
		ASSERT(((CBuildSlob *)pOldContainer)->GetConfig() != NULL);
		CString strConfig = ((CBuildSlob *)pOldContainer)->GetConfigurationName();
		SetMatchingConfig(strConfig);
		ASSERT(m_pcr);
#endif
#endif
#if 0
		ConfigurationRecord * pcrBase = (ConfigurationRecord *)m_pcr->m_pBaseRecord;
		// need to reconstruct contents at this point
		if (pCloneItem->GetContentList())
		{
			POSITION pos = pCloneItem->GetContentList()->GetHeadPosition();
			while (pos != NULL)
			{
				CProjItem * pItem = (CProjItem *)pCloneItem->GetContentList()->GetNext(pos);
#ifdef _DEBUG
				CBuildSlob * pChild = 
#endif
				GetBuildNode()->CreateSlobs(pItem, pcrBase, this, TRUE);
			}
		}
#endif

		((CBuildSlob *)pOldContainer)->UnIgnoreInforms();
		UnIgnoreInforms();
	}

	return CSlob::SetSlobProp(P_Container, pContainer);
}

BOOL CBuildSlob::PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromBldSlob, BOOL fToBldSlob, BOOL fPrjChanged)
{
	if (fToBldSlob && (/* fPrjChanged || */!fFromBldSlob) && m_bThunkMoves)
	{
		m_pItem->AddDependant(this);  // UNDONE: recurse?
	}

	if (fToBldSlob && (m_pcr == NULL))
	{
		// Set up the pcr for the wrapper
		ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CBuildSlob)));
		// don't try to set if parent not set yet
		if (((CBuildSlob *)pContainer)->GetConfig() != NULL)
		{
			CString strConfig = ((CBuildSlob *)pContainer)->GetConfigurationName();
 			SetMatchingConfig(strConfig);
			ASSERT(m_pcr);
		}
	}
	if (fFromBldSlob && pOldContainer && theUndoSlob.IsRecording())
	{
		ASSERT(pOldContainer->IsKindOf(RUNTIME_CLASS(CBuildSlob)));
		CProjContainer * pItemContainer = (CProjContainer *)((CBuildSlob *)pOldContainer)->m_pItem;
		if ((pItemContainer) && (pItemContainer->IsKindOf(RUNTIME_CLASS(CProjContainer))) && (pItemContainer->GetProject() != NULL) && (pItemContainer->GetProject()->m_bProjectComplete) && (pItemContainer->IsKindOf(RUNTIME_CLASS(CTargetItem)) || pItemContainer->GetTarget() != NULL))
			pItemContainer->RefreshAllMirroredDepCntrs();
	}

	if (fToBldSlob && pContainer && theUndoSlob.IsRecording())
	{
		ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CBuildSlob)));
		CProjContainer * pItemContainer = (CProjContainer *)((CBuildSlob *)pContainer)->m_pItem;
		if ((pItemContainer) && (pItemContainer->IsKindOf(RUNTIME_CLASS(CProjContainer))) &&  (pItemContainer->GetProject() != NULL) && (pItemContainer->GetProject()->m_bProjectComplete) && (pItemContainer->IsKindOf(RUNTIME_CLASS(CTargetItem)) || pItemContainer->GetTarget() != NULL))
			pItemContainer->RefreshAllMirroredDepCntrs();
	}

	return TRUE;
}

BOOL CBuildSlob::SetSlobProp(UINT idProp, CSlob * val)
{
 	// only interested in container property changes
	if (idProp != P_Container)
		// pass on to the base-class
		return CSlob::SetSlobProp(idProp, val);

	// old container?
	CSlob * pOldContainer = m_pContainer; 

	if (val != NULL && val->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)))
	{
		// Really mean to move it into the build slob
		val = ((CBuildViewSlob *)val)->GetBuildSlob();
	}

	// from a Build Slob?
	BOOL fFromBldSlob = m_pContainer && m_pContainer->IsKindOf(RUNTIME_CLASS(CBuildSlob));

	// into a Build Slob?
	BOOL fToBldSlob = val && val->IsKindOf(RUNTIME_CLASS(CBuildSlob));

	BOOL fPrjChanged = FALSE;

	if (fFromBldSlob && fToBldSlob)
	{
		ASSERT(val->IsKindOf(RUNTIME_CLASS(CBuildSlob)));
		CProjItem * pItem = ((CBuildSlob *)val)->m_pItem;
		if (pItem) 
		{
			fPrjChanged = (pItem->GetProject() != m_pItem->GetProject());
		}
	}

	// pre-move item
	if (!PreMoveItem(val, fFromBldSlob, fToBldSlob, fPrjChanged))
		return FALSE;

	// do the 'containment' part of the move
	if (!MoveItem(val, pOldContainer, fFromBldSlob, fToBldSlob, fPrjChanged))
		return FALSE;

	// post-move item, only call if not deleted!
	// ie. not deleted if moving to another container.
	if ((val || theUndoSlob.IsRecording()) &&
		!PostMoveItem(val, pOldContainer, fFromBldSlob, fToBldSlob, fPrjChanged))
		return FALSE;

	return TRUE;	// success
}

CBuildSlob * CBuildSlob::SetContainer(CSlob * pSlob)
{
	CBuildSlob * pOldContainer = (CBuildSlob *)m_pContainer;
	m_pContainer = pSlob;
	return pOldContainer;
}

void CBuildSlob::FixupItem()
{
	if (m_pcr == NULL)
		return;

	ASSERT_VALID (m_pcr);
/*
	What this should reallydo is get the active config from the active project and attempt to
	place this project in the configuration. Question is how, and what do we do if there is no match.
*/

	CString strConfig;
	CTargetItem *pTarg = GetProjItem()->GetProject()->GetActiveTarget();
	BOOL bMatchingConfig = g_BldSysIFace.GetTargetNameFromFileSet((HFILESET)pTarg, strConfig, ACTIVE_BUILDER, TRUE );

	if( !bMatchingConfig ){
		return;
	}
	if ( m_dwFixupRefCount == 0 )
	{
		m_pTempConfigChange = new CProjTempConfigChange(GetProjItem()->GetProject());
		m_pTempConfigChange->ChangeConfig(strConfig);
	}
	else {
		ASSERT(m_pTempConfigChange);
	}
	m_dwFixupRefCount++;

}

void CBuildSlob::UnFixupItem()
{

	// If m_pcr was NULL in fixup, the ref count wasn't bumped
	if (m_dwFixupRefCount == 0)
		return;

	ASSERT(m_dwFixupRefCount > 0);

	if (--m_dwFixupRefCount == 0 )
	{
		delete m_pTempConfigChange;
		m_pTempConfigChange = NULL;
	}
}

#ifdef VB_MAKEFILES
extern BOOL g_bVBInstalled;
#endif

void CBuildSlob::GetCmdIDs(CWordArray& aVerbs, CWordArray& aCmds)
{
	ASSERT(m_pItem != NULL);
	const CPath* pPath = m_pItem->GetFilePath();
	CStringArray paths;
	if (pPath != NULL) {
		paths.Add(*pPath);
	}
	HBUILDER builder = NULL;

    if (m_pItem->IsKindOf(RUNTIME_CLASS(CProjContainer)))
	{
		CProject* pProject = m_pItem->GetProject();
		builder = (HBUILDER) pProject;
		FixupItem();
		BOOL bSupported = (!pProject->m_bProjIsExe && pProject->GetProjType()->IsSupported() && pProject->CanDoTopLevelBuild());
		BOOL bBuildable = FALSE;
		if( bSupported )
		 	bBuildable = pProject->GetProjType()->GetPlatform()->GetBuildable();
		UnFixupItem();

		if (m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		{
			pPath = pProject->GetFilePath();
			paths.Add(*pPath);

			if (bSupported && bBuildable)
			{
				aVerbs.Add(IDM_PROJITEM_CBUILD);
				aVerbs.Add(IDM_PROJITEM_CBUILD_NO_DEPS);
				aVerbs.Add(IDM_PROJECT_CLEAN);
			}

			if (bSupported ){
				aVerbs.Add(IDM_PROJITEM_ADD_GROUP);
				aVerbs.Add(IDM_INSERT_FILES_POP);
			}
#ifdef VB_MAKEFILES
			if( pProject->m_bVB == TRUE && g_bVBInstalled ){
 				aVerbs.Add(IDM_PROJECT_EDIT);
			}
#endif
		}
		else if (m_pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
#ifdef SCC_FOLDERS
			GetAssociatedFiles(paths);
#endif
			if (bSupported)
			{
				aVerbs.Add(IDM_PROJITEM_ADD_GROUP);
				aVerbs.Add(IDM_INSERT_FILES_POP);
			}
		}
	}

	if (!m_pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)) &&
		!m_pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
	{
    	aCmds.Add(IDM_PROJECT_SETTINGS_POP);
	}

	if (g_pSccManager->IsSccInstalled() == S_OK)
	{
		CStringArray* pPath = &paths; // hack to reduce diffs
		if (g_pSccManager->IsValidOp(*pPath, SccOutOp) == S_OK)
			aCmds.Add(IDM_SCC_OUT);

		if (g_pSccManager->IsValidOp(*pPath, SccInOp) == S_OK)
			aCmds.Add(IDM_SCC_IN);

		if (g_pSccManager->IsValidOp(*pPath, SccUnOutOp) == S_OK)
			aCmds.Add(IDM_SCC_UNOUT);

		CProject* pProject = m_pItem->GetProject();
		builder = (HBUILDER) pProject;
		if (g_pSccManager->IsValidOp(*pPath, SccAddOp) == S_OK)
			aCmds.Add(IDM_SCC_ADD);
		else if (builder != NULL && g_pSccManager->IsBuilderControlled((HPROJECT)builder) == S_FALSE)
			aCmds.Add(IDM_SCC_ADD);
	}

	// Can only open non proj containers.
	if (!m_pItem->IsKindOf(RUNTIME_CLASS(CProjContainer)) && !m_pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
     	aVerbs.Add(IDM_PROJITEM_OPEN);

	if (m_pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		// Determine if we are part of an external target
		// or not
		CProject* pProject = m_pItem->GetProject();
        FixupItem();
		BOOL bBuildable = (!pProject->m_bProjIsExe && pProject->GetProjType()->IsSupported() && pProject->GetProjType()->GetPlatform()->GetBuildable() && pProject->CanDoTopLevelBuild());

        BOOL bExternalTarget = pProject->IsExternalTarget();
        UnFixupItem();			


        // Compile only available when we are not building
        if (!g_Spawner.SpawnActive())
        {
            // Check to see if this is in an external target
            // Compile is only valid for non external targets
         	if (!bExternalTarget && bBuildable )
        		aVerbs.Add(IDM_PROJITEM_CCOMPILE);
        }
	}
}

int CBuildSlob::CompareSlob(CSlob * pCmpSlob)
{
	ASSERT(pCmpSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)));
	CProjItem * pItem = GetProjItem();
	CProjItem * pCmpItem = ((CBuildSlob *)pCmpSlob)->GetProjItem();

	// Put dependency containers at the end of this container
	if (pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		return 1;

	if (pCmpItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		return -1;

	// Put Groups at the start of this container
	if (pCmpItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		return 1;

	if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		return -1;

	// Put subprojects at the start of this container
	if (pCmpItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return 1;

	if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return -1;

	CString strExt, strCmpExt; 
	BOOL bCmpIsFileItem = ((CProjItem *)pCmpItem)->IsKindOf(RUNTIME_CLASS(CFileItem));
	BOOL bIsFileItem = pItem->IsKindOf(RUNTIME_CLASS(CFileItem));
	if (bCmpIsFileItem)
		strCmpExt = ((CFileItem *)pCmpItem)->GetFilePath()->GetExtension();
	if (bIsFileItem)
		strExt = pItem->GetFilePath()->GetExtension();

	// Put .lib files last and don't sort them alphabetically
	if (bIsFileItem && (strExt.CompareNoCase(".lib")==0))
		return 1;

	if (bCmpIsFileItem && (strCmpExt.CompareNoCase(".lib")==0))
		return -1;

	// Put .obj files last and don't sort them alphabetically
	if (bIsFileItem && (strExt.CompareNoCase(".obj")==0))
		return 1;

	if (bCmpIsFileItem && (strCmpExt.CompareNoCase(".obj")==0))
		return -1;

	// Everything else is sorted based on their P_Title property, i.e alphabetically
	int retval = CProjSlob::CompareSlob(pCmpSlob);

	// as a tie breaker, use the absolute path to at least get consistency
	if (retval == 0)
	{
		ASSERT(pItem->GetFilePath() != NULL);
		ASSERT(pCmpItem->GetFilePath() != NULL);
		retval = _tcsicmp((LPCTSTR)*pItem->GetFilePath(), (LPCTSTR)*pCmpItem->GetFilePath());
	}
	ASSERT(retval != 0); // should never be equal
	return retval;
}

void CBuildSlob::OnOpenItem()
{
	// Pass on to the item to open
	m_pItem->OnCmdMsg(IDM_PROJITEM_OPEN, CN_COMMAND, NULL, NULL);
}

void CBuildSlob::OnUpdateOpenItem(CCmdUI * pCmdUI)
{
	// Open is only available on non-folders and non-subfolders

	// a folder is an item with a non empty content list
	BOOL bFolder = !(GetContentList()->IsEmpty());

	// a subfolder is defined as a project dependancy
	BOOL bSubfolder = FALSE;
	if (GetProjItem()->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		bSubfolder = TRUE;

	BOOL bEnable =  !bFolder && !bSubfolder && !GetProjItem()->IsKindOf(RUNTIME_CLASS(CProjContainer));
	pCmdUI->Enable( bEnable );
}

void CBuildSlob::OnCompileItem()
{
	CBuildTool * pTool;

	// has a multi select build been stopped by the user ?
	if( g_bStopBuild == TRUE )
		return;

	// Make sure we are in the right config
    FixupItem();

	if (!m_pItem->IsKindOf(RUNTIME_CLASS(CFileItem)) ||
		((pTool = m_pItem->GetSourceTool()) == (CBuildTool *)NULL))
	{
		CString	strError;
		MsgBox(	Information,
				MsgText(strError,
						IDS_CANT_COMPILE_NO_TOOL,
						(const TCHAR *)*m_pItem->GetFilePath()));
        UnFixupItem();
		return;
	}

	// Also make sure the item itself thinks it's OK to build.
	if (!m_pItem->CanDoTopLevelBuild())
	{
		ASSERT(FALSE);	// Command should have been disabled
        UnFixupItem();
		return;
	}

	// FINALLY, we can compile the damn thing.
	m_pItem->CmdCompile(FALSE);
    
	// Reset the config
	UnFixupItem();
}

void CBuildSlob::OnUpdateCompileItem(CCmdUI * pCmdUI)
{
	BOOL bEnable = FALSE;
	if( !g_pActiveProject ){
		pCmdUI->Enable(FALSE);
		return;
	}

	// Is there a matching configuration
	CString strConfig;
	CBuildSlob *pSlob = (CBuildSlob *)GetContainer();
	CProject * pProject = pSlob->GetProjItem()->GetProject();

	BOOL bMatchingConfig = g_BldSysIFace.GetTargetNameFromFileSet((HFILESET)(pSlob->GetProjItem()), strConfig, (HBUILDER)pProject, TRUE);

	FixupItem();

	// No compile for exe projects
	if (g_BldSysIFace.GetBuilderType() == ExeBuilder)
		bEnable = FALSE;

	CString			strCompileMenu, strTarget;
	TCHAR			szMenuText[MENU_TEXT_CCH];
	CBuildTool * pTool;


	// Can only compile certain fileitems
	if (m_pItem != (CProjItem *)NULL &&
		m_pItem->IsKindOf(RUNTIME_CLASS(CFileItem)) &&
		(pTool = m_pItem->GetSourceTool()) != (CBuildTool *)NULL)
	{
		ASSERT(m_pItem->GetFilePath());
		strTarget = m_pItem->GetFilePath()->GetFileName();
		bEnable = TRUE;
 	}
	else
		bEnable = FALSE;
	
	strCompileMenu.LoadString(IDS_CPOP_COMPILE);

    pProject = m_pItem->GetProject();
    ASSERT(pProject);

	CProjType *pProjType = pProject->GetProjType();
	ASSERT(pProjType);

	// Is the projtype supported
 	BOOL bIsSupported = pProjType->IsSupported() && pProjType->GetPlatform()->GetBuildable();


	// Enable menuitem and set text
	wsprintf(szMenuText, strCompileMenu,
		strTarget.Left(MENU_TEXT_CCH - strCompileMenu.GetLength()));
	pCmdUI->SetText(szMenuText);
	pCmdUI->Enable(bEnable && bIsSupported && bMatchingConfig && g_Spawner.CanSpawn());

	UnFixupItem();
}

void CBuildSlob::OnBuild()
{
	// has a multi select build been stopped by the user ?
	if( g_bStopBuild == TRUE )
		return;

	FixupItem();
	
	CProject * pProject = m_pItem->GetProject();
	ASSERT(pProject);

	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	UINT nPos;
	VERIFY(SUCCEEDED(pProjSysIFace->GetVerbPosition(&nPos)));

 	BOOL bClearOutputWindow = (nPos == 0);

	CBuildIt wholeBuild;

	// Don't do a build on the target item, instead thunk it
	// upwards to the project, however we do build file items
	// directly
	if (m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		pProject->DoTopLevelBuild(FALSE, NULL, fcaNeverQueried, TRUE, bClearOutputWindow, TRUE);
	else
		m_pItem->DoTopLevelBuild(FALSE, NULL, fcaNeverQueried, TRUE, bClearOutputWindow, TRUE);

	UnFixupItem();
}

void CBuildSlob::OnBuildNoDeps()
{
	// has a multi select build been stopped by the user ?
	if( g_bStopBuild == TRUE )
		return;

	FixupItem();
	
	CProject * pProject = m_pItem->GetProject();
	ASSERT(pProject);

	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	UINT nPos;
	VERIFY(SUCCEEDED(pProjSysIFace->GetVerbPosition(&nPos)));

 	BOOL bClearOutputWindow = (nPos == 0);

	// Don't do a build on the target item, instead thunk it
	// upwards to the project, however we do build file items
	// directly

	CBuildIt wholeBuild;

	if (m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		pProject->DoTopLevelBuild(FALSE, NULL, fcaNeverQueried, TRUE, bClearOutputWindow, FALSE );
	else
		m_pItem->DoTopLevelBuild(FALSE, NULL, fcaNeverQueried, TRUE, bClearOutputWindow, FALSE);


	UnFixupItem();
}

void CBuildSlob::OnUpdateBuild(CCmdUI * pCmdUI)
{
	if( !g_pActiveProject ){
		pCmdUI->Enable(FALSE);
		return;
	}
		
	// Is there a matching configuration
	CString strConfig;
	CBuildSlob *pSlob = (CBuildSlob *)GetContainer();
	CProject * pProject = pSlob->GetProjItem()->GetProject();
	BOOL bMatchingConfig = g_BldSysIFace.GetTargetNameFromFileSet((HFILESET)(pSlob->GetProjItem()), strConfig, (HBUILDER)pProject, TRUE);

	FixupItem();

	pProject = m_pItem->GetProject();
	pCmdUI->Enable(!pProject->m_bProjIsExe && bMatchingConfig && pProject->GetProjType()->IsSupported() && pProject->CanDoTopLevelBuild());

	UnFixupItem();
}

void CBuildSlob::OnClean()
{
	FixupItem();

	CProject * pProject = m_pItem->GetProject();

	// can we use the src pkg iface to select build output window
	if (g_VPROJIdeInterface.Initialize())
		g_VPROJIdeInterface.GetOutputWindow()->ShowOutputWindow();

	ASSERT_VALID (m_pcr);

	if (m_pcr == NULL)
		return;

	CErrorContext * pEC = g_buildengine.OutputWinEC();

	// perform the clean operation on the appropriate graph
	DWORD dw = (DWORD)pEC;
	CBuildIt wholeBuild(FALSE);
	pProject->DoTopLevelBuild(TOB_Clean,NULL,fcaNeverQueried,TRUE,TRUE, FALSE);
	// g_buildengine.GetDepGraph(m_pcr)->PerformOperation(CFileDepGraph::Clean, dw);

	UnFixupItem();
}

void CBuildSlob::OnUpdateClean(CCmdUI * pCmdUI)
{
	if( !g_pActiveProject ){
		pCmdUI->Enable(FALSE);
		return;
	}
	// Is there a matching configuration
	CString strConfig;
	CBuildSlob *pSlob = (CBuildSlob *)GetContainer();
	CProject * pProject = pSlob->GetProjItem()->GetProject();
	BOOL bMatchingConfig = g_BldSysIFace.GetTargetNameFromFileSet((HFILESET)(pSlob->GetProjItem()), strConfig, (HBUILDER)pProject, TRUE);

	FixupItem();

	pProject = m_pItem->GetProject();
	pCmdUI->Enable(!pProject->m_bProjIsExe && bMatchingConfig && pProject->GetProjType()->IsSupported() && pProject->CanDoTopLevelBuild());
	UnFixupItem();
}

void CBuildSlob::OnAddGroup()
{
	FixupItem();
	CProject *pProject = m_pItem->GetProject();
	pProject->CreateNewGroup();
	UnFixupItem();
}

void CBuildSlob::OnUpdateAddGroup(CCmdUI * pCmdUI)
{
	LPPROJECTWORKSPACEWINDOW pInterface = FindProjWksWinIFace();
	if ((pInterface == NULL) || (g_Spawner.SpawnActive()))
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CMultiSlob *pMultiSlob;
	VERIFY(SUCCEEDED(pInterface->GetSelectedSlob(&pMultiSlob)));

	if (pMultiSlob->GetContentList() == NULL ||	pMultiSlob->GetContentList()->GetCount() > 1)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CBuildSlob::OnPopupFilesIntoProject()
{
	FixupItem();
	CProject *pProject = m_pItem->GetProject();
	pProject->ActPopupFilesIntoProject();
	UnFixupItem();
}

void CBuildSlob::OnUpdateFilesIntoProject(CCmdUI *pCmdUI)
{
	FixupItem();
	CProject *pProject = m_pItem->GetProject();
	pProject->UpdPopupFilesIntoProject(pCmdUI);
	UnFixupItem();
}

void CBuildSlob::InformBldSlobs(CSlob * pChangedSlob, UINT idChange, DWORD dwHint)
{
	CBuildSlob * pBuildSlob;

	POSITION pos = m_lstInform.GetHeadPosition();
	while (pos != NULL)
	{
		// Get the build slob, and determine if the associated item is a target reference
		pBuildSlob = (CBuildSlob *)m_lstInform.GetNext(pos);
 		
		// Do the inform
		pBuildSlob->OnInform(pChangedSlob, idChange, dwHint);
	}
}

CSlob * CBuildSlob::Clone()
{
	CProjItem * pCloneItem = (CProjItem *)m_pItem->Clone();
#if 0
	CBuildSlob * pClone = (CBuildSlob *)CSlob::Clone();

	pClone->SetItem(pCloneItem);
	pClone->SetConfig(NULL);
#else
	// do our own pseudo-clone and fixup contents later
	// theUndoSlob.Pause();
	// ConfigurationRecord * pcrBase = (ConfigurationRecord *)m_pcr->m_pBaseRecord;
	CBuildSlob * pClone = GetBuildNode()->CreateSlobs(pCloneItem, NULL, NULL, TRUE);
	CopyProps(pClone);	// REVIEW: needed?

	// theUndoSlob.Resume();

#endif

    return pClone;
}

void CBuildSlob::PrepareAddList(CMultiSlob* pAddList, BOOL fPasting)
{
	CBuildSlob * pBldSlob;
	CMapPtrToPtr mapFileRegs;
	CProjItem * pItem, *pDupItem;
	CObList * pAddListContents = pAddList->GetContentList();
	CTargetItem * pTarget = GetProjItem()->GetTarget();
	ASSERT(pTarget);

 	FileRegHandle hndFileReg;
	CObList * pGrpContents = GetContentList();
	
 	POSITION pos = pGrpContents->GetHeadPosition();
	while (pos != NULL)	 // UNDONE: need to check withing group contents also!
	{
		pBldSlob = (CBuildSlob *) pGrpContents->GetNext(pos);
		pItem = (CFileItem *) pBldSlob->GetProjItem();
		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			VERIFY((hndFileReg = pItem->GetFileRegHandle()) != 0);
			mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
		}
	}

	// Now cycle through items to be added (backwards!) and eliminate any dupes
	pos = pAddListContents->GetTailPosition();
	while (pos != NULL)
	{
		pBldSlob = (CBuildSlob *) pAddListContents->GetPrev(pos);
		pItem = (CProjItem *) pBldSlob->GetProjItem();
		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			FileRegHandle hndFileReg = pItem->GetFileRegHandle();
			ASSERT(hndFileReg); // if in file reg, check if in this group
			if (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pDupItem))
			{
#if 0
				// new behaviour: duplicate: remove original, replace in map
				mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
				if (pDupItem->GetContainer()==GetProjItem())
					{	// if removing an existing item, move into UndoSlob
						pBldSlob->MoveInto(NULL);
					}
				else	// if dup is also from AddList, just delete it
#endif
					{
						theUndoSlob.Pause();
						pAddList->Remove(pBldSlob);	// removes dependency
						delete pBldSlob;			// prevent memory leaks
						theUndoSlob.Resume();
					}
			}
			else if (pTarget->IsFileInTarget(hndFileReg, TRUE))
			{
				// keep the original in this case
				theUndoSlob.Pause();
				pAddList->Remove(pBldSlob);	// removes dependency
				delete pBldSlob;			// prevent memory leaks
				theUndoSlob.Resume();
			}
			else
			{
				mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
			}
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			BOOL bReject = FALSE;
			CProjItem * pContainer = GetProjItem();
			while ((pContainer != NULL) && (pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup))))
			{
				if (pContainer == pItem)
				{
					// can't add group to itself or its children
					theUndoSlob.Pause();
					pAddList->Remove(pBldSlob);
					delete pBldSlob;				// prevent memory leaks
					theUndoSlob.Resume();
					bReject = TRUE;
					break;
				}
				pContainer = (CProjItem *)pContainer->GetContainer();

			}
			// reject duplicates
			if (!bReject)
			{
				// reject duplicates in group content
				CObList ol;
				pItem->FlattenSubtree( ol, CProjItem::flt_Normal | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups | CProjItem::flt_ExcludeDependencies | CProjItem::flt_RespectTargetExclude | CProjItem::flt_ExcludeProjDeps);
				POSITION pos = ol.GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					// the contained project item
					CProjItem * pContainedItem = (CProjItem *)ol.GetNext(pos);
					ASSERT(pContainedItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
					if (!pContainedItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
						continue;

					FileRegHandle hndFileReg = pContainedItem->GetFileRegHandle();
					ASSERT(hndFileReg); // if in file reg, check if in this group
					if (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pDupItem))
					{
						theUndoSlob.Pause();
						pContainedItem->MoveInto(NULL);
						theUndoSlob.Resume();
					}
					else if (pTarget->IsFileInTarget(hndFileReg, TRUE))
					{
						theUndoSlob.Pause();
						pContainedItem->MoveInto(NULL);
						theUndoSlob.Resume();
					}
					else
					{
						mapFileRegs.SetAt((void *)hndFileReg, (void *)pContainedItem);
					}
				}
				ol.RemoveAll();
			}
		}
		else
		{
			// ignore all non-fileitems
			theUndoSlob.Pause();
			pAddList->Remove(pBldSlob); 	// removes dependency
			delete pBldSlob;				// prevent memory leaks
			theUndoSlob.Resume();
		}	
	}

}

void CBuildSlob::GetGlyphTipText(CString & str)
{
	str.Empty();
	const CPath * pPath = m_pItem->GetFilePath();
	if ((pPath != NULL) && (g_pSccManager->IsActive() == S_OK))
	{
		VERIFY(SUCCEEDED(g_pSccManager->GetStatusText(*pPath, str)));
	}
}

///////////////////////////////////////////////////////////////////////////////
HCURSOR CBuildSlob::GetCursor(int ddcCursorStyle)
{
	int iCursors = 0;	// At the moment we only have 1 set of cursors!

	switch (ddcCursorStyle)
	{
		case DDC_SINGLE:
			return g_Cursors.GetDDCur(iCursors);

		case DDC_COPY_SINGLE:
			return g_Cursors.GetDDCopyCur(iCursors);

		case DDC_MULTI:
			return g_Cursors.GetDDMultiCur(iCursors);

		case DDC_COPY_MULTI:
			return g_Cursors.GetDDMultiCopyCur(iCursors);

		default:
			ASSERT(FALSE);
			return NULL;
	}
}

CSlobDragger *CBuildSlob::CreateDragger(CSlob *pDragObject, CPoint screenPoint)
{
	if (g_pProjWksWinIFace == NULL)
		return NULL;

	CSlobDraggerEx *pSlobDraggerEx = NULL;
	VERIFY(SUCCEEDED(g_pProjWksWinIFace->CreateDragger(&pSlobDraggerEx)));

	pSlobDraggerEx->AddItem(this);
	return pSlobDraggerEx;
}

void CBuildSlob::DoDrop(CSlob *pSlob, CSlobDragger *pDragger)
{
	ASSERT(g_pProjWksWinIFace != NULL);

	if (!m_pItem->IsKindOf(RUNTIME_CLASS(CProjContainer)))
	{
		CBuildSlob * pContainer = (CBuildSlob *)GetContainer();
		ASSERT(pContainer != NULL);
		pContainer->DoDrop(pSlob, pDragger);
		return;
	}

    GetBuildNode()->HoldUpdates();

	CProject* pProject = m_pItem->GetProject();
	if (NULL!=pProject)
		pProject->BeginBatch();

	// Remove any unneeded slobs identified in PrepareDrop
	while (!m_DeathRow.IsEmpty())
 		((CSlob *)m_DeathRow.RemoveHead())->MoveInto(NULL);
	
	POSITION pos = pSlob->GetHeadPosition();
	while (pos != NULL)
	{
		CSlob* pItemSlob = pSlob->GetNext(pos);
		
		CProjItem * pItem = (CProjItem *)((CBuildSlob *)pItemSlob)->GetProjItem();
		if (pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)))
		{
			// convert to CFileItem and drop
			// must do this here so undo works properly
			g_pProjWksWinIFace->DeselectSlob(pItemSlob); // must unselect!
			CFileItem * pNewItem = new CFileItem;
			pNewItem->SetFile(pItem->GetFilePath());
			pItemSlob = new CBuildSlob(pNewItem, NULL);
			// old one is cleaned up elsewhere
		}
		if (CanAdd(pItemSlob))
		{
			pItemSlob->MoveInto(this);
		}
		else
		{
			pItemSlob->MoveInto(NULL);
		}
	}

	if (NULL!=pProject)
		pProject->EndBatch();

    GetBuildNode()->EnableUpdates();
}

void CBuildSlob::DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint)
{
	pDragger->Show(TRUE);
}

void RecordContents(CBuildSlob * pBldSlob, CMapPtrToPtr & mapFiles, BOOL bRecurse = TRUE)
{
#ifdef _DEBUG
	void * pVoid;
#endif
	CProjItem * pItem;
 	FileRegHandle hndFileReg;
	CObList * pGrpContents = pBldSlob->GetContentList();
	POSITION pos = pGrpContents->GetHeadPosition();
	while (pos != NULL)
	{
		pBldSlob = (CBuildSlob *) pGrpContents->GetNext(pos);
		pItem = (CFileItem *) pBldSlob->GetProjItem();
		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			VERIFY((hndFileReg = pItem->GetFileRegHandle()) != 0);
			ASSERT(!mapFiles.Lookup((void *)hndFileReg, pVoid));
			mapFiles.SetAt((void *)hndFileReg, (void *)pBldSlob);
		}
		else if (bRecurse && pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			RecordContents(pBldSlob, mapFiles);
		}
	}
}

void CBuildSlob::FlattenContents(CObList & ol, BOOL bItemsOnly /* = TRUE */)
{
	CBuildSlob * pBldSlob;
	CProjItem * pItem;
	CObList * pListContents = GetContentList();
	POSITION pos = pListContents->GetHeadPosition();
	while (pos != NULL)
	{
		pBldSlob = (CBuildSlob *) pListContents->GetNext(pos);
		pItem = (CProjItem *) pBldSlob->GetProjItem();
		if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			if (!bItemsOnly)
				ol.AddTail(pBldSlob);
			pBldSlob->FlattenContents(ol, bItemsOnly);
		}
		else
		{
			ol.AddTail(pBldSlob);
		}
	}
}

void CBuildSlob::PrepareDrop(CDragNDrop *pInfo)
{
	ASSERT(g_pProjWksWinIFace);
	CSlob* pDragSlob;
 	
	pDragSlob = pInfo->m_dragSlob;

	CMapPtrToPtr mapFileRegs;
	CMapPtrToPtr mapProjFileRegs;
	CProjItem * pItem;
    CBuildSlob * pDupItem;
	CObList * pAddListContents = pDragSlob->GetContentList();
	CTargetItem * pTarget = GetProjItem()->GetTarget();
	ASSERT(pTarget);
	CBuildSlob * pProjContainer = this;

	while (pProjContainer->GetContainer() != NULL)
	{
		pProjContainer = (CBuildSlob *)pProjContainer->GetContainer();
	}

 	FileRegHandle hndFileReg;
	ASSERT(!theUndoSlob.IsRecording());	// no need for pause/resume for Remove()

	CProject * pProject = GetProjItem()->GetProject();	
	ASSERT((pProjContainer->GetProjItem() == pTarget) || 
		(pProjContainer->GetProjItem() == pProject));

	if (!pProject->AreFilesUnique(pDragSlob))
	{
		InformationBox(IDS_DUPES_IN_SELECTION);
	}

	CBuildSlob * pBldSlob;
	::RecordContents(this, mapFileRegs, FALSE); // immediate contents
	::RecordContents(pProjContainer, mapProjFileRegs, TRUE); // proj contents

	// Can't delete slobs just yet, so just keep track of which ones to
	// delete later in DoDrop()
	ASSERT(m_DeathRow.IsEmpty());

	// Now cycle through items to be added (backwards!) and reject any we can't drop
	POSITION pos = pAddListContents->GetTailPosition();
	while (pos != NULL)
	{
		pBldSlob = (CBuildSlob *) pAddListContents->GetPrev(pos);
		pItem = (CProjItem *) pBldSlob->GetProjItem();
		if (pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)))
		{
			CMultiSlob * pSelection;
			if (SUCCEEDED(g_pProjWksWinIFace->GetSelectedSlob(&pSelection)) && pSelection != NULL && pSelection->GetContentList()->GetCount() == 1)
			{
				// UNDONE: this doesn't work
				CSlob * pProjSlob = pBldSlob->GetContainer()->GetContainer();
				g_pProjWksWinIFace->SelectSlob(pProjSlob, FALSE); // must select something valid
			}
			g_pProjWksWinIFace->DeselectSlob(pBldSlob); // must unselect!
		}
		if ((pItem->IsKindOf(RUNTIME_CLASS(CFileItem))) ||
			(pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile))))
		{
			FileRegHandle hndFileReg = pItem->GetFileRegHandle();
			ASSERT(hndFileReg); // if in file reg, check if in this group
			if (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pDupItem))
			{
				// duplicate: remove from group
				if (pBldSlob == pDupItem)
				{
					// Dropping into the same container
					// so we just remove from add list
					pDragSlob->Remove(pBldSlob);
				}
				else
				{
					// replace duplicate with pItem
					if (pDupItem->GetContainer() == this)
					{
						// remove duplicate from group
						m_DeathRow.AddHead((void *)pDupItem);
					}
					else
					{
						// the dupe is something we were adding,
						// so we must remove it from the list
						pDragSlob->Remove(pDupItem);

						// only delete it if we're doing a move
						// or copying a clone
						if (pDupItem->GetContainer()==NULL)
						{
							delete pDupItem;	// delete clone	
						}
						else if (!pInfo->IsCopy())
						{
							m_DeathRow.AddHead((void *)pDupItem);
						}
					}
					mapFileRegs.SetAt((void *)hndFileReg, (void *)pBldSlob);
				}			
			}
			else if (mapProjFileRegs.Lookup((void *)hndFileReg, (void * &) pDupItem))
			{
				if (pDupItem != pBldSlob)
				{
					VERIFY(mapProjFileRegs.RemoveKey((void *)hndFileReg));
					// remove duplicate from project
					m_DeathRow.AddHead((void *)pDupItem);
					mapFileRegs.SetAt((void *)hndFileReg, (void *)pBldSlob);
				}
				else if (pInfo->IsCopy())
				{
					// don't allow copy of this item within the project
					// so we just remove from add list
					pDragSlob->Remove(pBldSlob);
				}
			}
			else
			{
				mapFileRegs.SetAt((void *)hndFileReg, (void *)pBldSlob);
			}
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			BOOL bReject = FALSE;
			CProjItem * pContainer = GetProjItem();
			ASSERT(pContainer != NULL);
			if (pContainer == pItem->GetContainer())
			{
				// can't add group to container it is already in
				pDragSlob->Remove(pBldSlob);
				bReject = TRUE;
			}
			else
			{
				while ((pContainer != NULL) && (pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup))))
				{
					if (pContainer == pItem)
					{
						// can't add group to itself or its children
						pDragSlob->Remove(pBldSlob);
						bReject = TRUE;
						break;
					}
					pContainer = (CProjItem *)pContainer->GetContainer();
				}
			}
			// reject duplicates
			if (!bReject)
			{
				// reject duplicates in group content
				CObList ol;
				pBldSlob->FlattenContents(ol, TRUE);
				POSITION pos = ol.GetTailPosition();
				while (pos != (POSITION)NULL)
				{
					CBuildSlob * pContainedSlob = (CBuildSlob *)ol.GetPrev(pos);
					CProjItem * pContainedItem = pContainedSlob->GetProjItem();
					// the contained project item
					CBuildSlob * pDupeSlob;
					CBuildSlob * pProjDupeSlob;
					if (!pContainedItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
						continue;

					FileRegHandle hndFileReg = pContainedItem->GetFileRegHandle();
					ASSERT(hndFileReg); // if in file reg, check if in this group
					BOOL bInGroup = (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pDupeSlob));
					BOOL bInProj = (mapProjFileRegs.Lookup((void *)hndFileReg, (void * &) pProjDupeSlob));
					if (bInGroup && !bInProj)
					{
						// the dupe is something we were adding,
						// so we must remove it from the list
						pDragSlob->Remove(pDupeSlob);

						// only delete it if we're doing a move
						// or copying a clone
						if (pDupeSlob->GetContainer()==NULL)
						{
							delete pDupeSlob;	// delete clone	
						}
						else if (!pInfo->IsCopy())
						{
							m_DeathRow.AddHead((void *)pDupeSlob);
						}
					}
					else if (bInProj)
					{
						if (pProjDupeSlob != pContainedSlob)
						{
							VERIFY(mapProjFileRegs.RemoveKey((void *)pProjDupeSlob));
							m_DeathRow.AddHead((void *)pProjDupeSlob);
							if (bInGroup)
							{
								ASSERT(pProjDupeSlob == pDupeSlob);
							}
							mapFileRegs.SetAt((void *)hndFileReg, (void *)pContainedSlob);
					
						}
						else if (pInfo->IsCopy())
						{
							// don't allow copy of file already in project
							m_DeathRow.AddHead((void *)pContainedSlob);
						}
					}
					else
					{
						mapFileRegs.SetAt((void *)hndFileReg, (void *)pContainedSlob);
					}
				}
				ol.RemoveAll();
			}
		}
		else
		{
			// ignore all non-fileitems
			pDragSlob->Remove(pBldSlob); 	// removes dependency
		}	
	}

	if (pAddListContents->IsEmpty())
	{
		// Kill death row residents here, since we will never get to DoDrop
		while (!m_DeathRow.IsEmpty())
 			((CSlob *)m_DeathRow.RemoveHead())->MoveInto(NULL);
	}
	mapFileRegs.RemoveAll();
	mapProjFileRegs.RemoveAll();
}
