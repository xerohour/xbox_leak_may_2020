//
// Build View Slob
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "resource.h"

#include <prjapi.h>
#include <prjguid.h>

#include "bldslob.h"
#include "bldnode.h"
#include "vwslob.h"	// our local header
#include "targdlg.h"

#include "oleref.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#undef new
#endif

IMPLEMENT_SERIAL(CBuildViewSlob, CProjSlob, 1)

/*virtual*/ int CBuildViewSlob::GetAssociatedFiles(CStringArray& files)
// return the number of associated files? can't you just get that 
// with like files.GetSize() ?
{
typedef CSlob Inherited;
// this code based on CBuildSlob::GetCmdIDs
	ASSERT(m_pTargetSlob);

#if 0
#define X(x) TRACE("JAYK this->" #x ":%s\n", (const TCHAR*) (x)) ;
#define XX(x) X(x); \
	TRACE("JAYK *this->" #x ":%s\n", x ? (const TCHAR*) *(x) : "*null")

	XX(GetFilePath());
	XX(m_pTargetSlob->GetFilePath());
//	XX(m_pTargetSlob->GetProject()->GetFilePath());
	XX(m_pTargetSlob->GetProjItem()->GetFilePath());
	XX(m_pTargetSlob->GetProjItem()->GetProject()->GetFilePath());
	X(m_pTargetSlob->GetProjItem()->GetProject()->m_strProjItemName);

	m_pTargetSlob->FixupItem();
	TRACE("JAYK m_pTargetSlob->FixupItem\n");

	XX(GetFilePath());
	XX(m_pTargetSlob->GetFilePath());
//	X(m_pTargetSlob->GetProject()->GetFilePath());
	XX(m_pTargetSlob->GetProjItem()->GetFilePath());
	XX(m_pTargetSlob->GetProjItem()->GetProject()->GetFilePath());
	X(m_pTargetSlob->GetProjItem()->GetProject()->m_strProjItemName);

// results
	// FixupItem makes no differences
	// The first two are null, third doesn't compile, fourth is null
	// fifth is correct if derefed, sixth is just tail ("foo.bld")

	return 0;
#undef X
#undef XX
#endif

	CPath path = *(m_pTargetSlob->GetProjItem()->GetProject()->GetFilePath());
	files.Add((const TCHAR*)path);
	if (g_bAlwaysExportMakefile)
	{
		path.ChangeExtension(_T(".mak"));
		if (g_pSccManager->IsControlled((const TCHAR *)path) == S_OK)
		{
			// add .mak file as well
			files.Add((const TCHAR*)path);
			if (g_bAlwaysExportDeps)
			{
				path.ChangeExtension(_T(".dep"));
				if (g_pSccManager->IsControlled((const TCHAR *)path) == S_OK)
				{
					// add .dep file as well
					files.Add((const TCHAR*)path);
					return 3;
				}
			}
			return 2;
		}
	}
	return 1; //+Inherited::GetAssociated(files);
}

BOOL CBuildViewSlob::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// WARNING! This does not fall through to a message map.  You must
	// handle both CN_COMMAND and CN_UPDATE_COMMAND_UI if you want your
	// commands to be enabled.

	if (nCode == CN_COMMAND && pExtra == NULL)
	{
		switch (nID)
		{
			case IDM_PROJITEM_CBUILD:
				OnBuild();
				return TRUE;
#ifdef VB_MAKEFILES
 			case IDM_PROJECT_EDIT:
 				OnLaunchVB();
 				return TRUE;
#endif
			case IDM_PROJITEM_CBUILD_NO_DEPS:
				OnBuildNoDeps();
				return TRUE;
			case IDM_PROJECT_CLEAN:
				OnClean();
				return TRUE;
			case IDM_PROJITEM_ADD_GROUP:
				OnAddGroup();
				return TRUE;
			case IDM_PROJECT_SET_AS_DEFAULT:
				OnSetDefault();
				return TRUE;
			case IDM_PROJECT_UNLOAD:
				OnUnloadProject();
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
			case IDM_PROJITEM_CBUILD:
			case IDM_PROJITEM_CBUILD_NO_DEPS:
			case IDM_PROJECT_CLEAN:
				{
					if( !g_pActiveProject ){
						pCmdUI->Enable(FALSE);
						return TRUE;
					}

					CString strTarget;

					if (g_BldSysIFace.GetTargetNameFromFileSet(g_BldSysIFace.GetFileSet(m_hBld, m_hTarget), strTarget, ACTIVE_BUILDER, (CProject *)m_hBld!=g_pActiveProject) ){				
						pCmdUI->Enable(TRUE);
						return TRUE;		
					}
				}
			case IDM_PROJITEM_ADD_GROUP:
				OnUpdateAddGroup(pCmdUI);
				return TRUE;		
			case IDM_PROJECT_SET_AS_DEFAULT:
#ifdef VB_MAKEFILES
 			case IDM_PROJECT_EDIT:
#endif
				pCmdUI->Enable(TRUE);
				return TRUE;		
			case IDM_PROJECT_UNLOAD:
				pCmdUI->Enable(!g_Spawner.SpawnActive());
				return TRUE;
			case IDM_INSERT_FILES_POP:
				OnUpdateFilesIntoProject(pCmdUI);
				return TRUE;
		}
	}

	return FALSE;
}

void CBuildViewSlob::OnUnloadProject()
{
	CProject *pProject = m_pTargetSlob->GetProjItem()->GetProject();

	if (g_Spawner.SpawnActive() || !g_BldSysIFace.SaveBuilder((HBUILDER)pProject))
		return;

	LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();

	IBSProject * pIBSProject = pProject->GetInterface();
	IPkgProject *pPkgProj;
	pIBSProject->QueryInterface(IID_IPkgProject, (void **)&pPkgProj);
	pIBSProject->Release();

	pProjSysIFace->CloseProject(pPkgProj);
	pPkgProj->Release();
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob property map

#define theClass CBuildViewSlob
BEGIN_SLOBPROP_MAP(CBuildViewSlob, CProjSlob)
	STR_PROP(ProjWinTitle)
	STR_PROP(Title)
	STR_PROP(ConfigName)
	BOOL_PROP(IsInvisibleNode)
	BOOL_PROP(IsExpanded)
	LONG_PROP(IPkgProject)
END_SLOBPROP_MAP()
#undef theClass

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob construction/destruction

CBuildViewSlob::CBuildViewSlob(HBLDTARGET hTarget, HBUILDER hBld)
{
	m_hBld = hBld;

	m_bIsInvisibleNode = FALSE;

	// Null out our CBuildSlob pointer
	m_pTargetSlob = NULL;

	m_bIsExpanded = FALSE;
	m_lIPkgProject = NULL;

	// Set the current target we represent
	SetFilterTarget(hTarget);
}

CBuildViewSlob::CBuildViewSlob()
{
	if (m_pTargetSlob != NULL)
		m_pTargetSlob->SetSlobProp(P_ProxyContainer, NULL);

	// Null out our CBuildSlob pointer
	m_pTargetSlob = NULL;
	m_hBld = NO_BUILDER;
	m_bIsExpanded = FALSE;
	m_bIsInvisibleNode = FALSE;
}

CBuildViewSlob::~CBuildViewSlob()
{
	// Remove this node from the build nodes list of CBuildViewSlob's
	GetBuildNode()->RemoveNode(this);

	// If we still have an associated CBuildSlob then we'd
	// better disconnect it from us as a dependent.
	if (m_pTargetSlob != NULL)
		m_pTargetSlob->RemoveDependant(this);
}

void GetExpandedState(CObList * pContentList, UINT & fExpanded, UINT & fMask, BOOL fIgnoreDeps = FALSE);
void SetExpandedState(CObList * pContentList, UINT & fExpanded);

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::SetFilterTarget, this sets the current target that we
// represent

void CBuildViewSlob::SetFilterTarget(HBLDTARGET hTarget)
{
	// If we are already hooked up to a target slob
	// then remove us as a dependancy.
	if (m_pTargetSlob != NULL)
		m_pTargetSlob->RemoveDependant(this);

	// Are we being destroyed
	if (hTarget == NULL)
	{
		// This should only happen if the item is really being destroyed
		m_hTarget = NULL;
		return;
	}

	// Get the new target name
	CString strTarget;
	g_BldSysIFace.GetTargetName(hTarget, strTarget, m_hBld);

	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();

	// save the expanded state of the parent node (this) and the CDependentContainer.
	// Do it before we change the target, as that might change the content list we search.
	// we will restore these after calling InformDependents. If the Debug/Release
	// state changes, the contentlist of "this" will change, so we can't just
	// save the dependency pointer. If the dependency node is expanded, we
	// will have to walk the new content list to find its pointer.
	// bobz 8/17/95
	// "this" is what we first look at to see if it is expanded. If it is look to see if it has an expanded dependent
	UINT fExpandedState = 0;
	UINT fMask = 1;

	int val;

	if (GetIntProp(P_IsExpanded, val) == valid)
	{
		if (val)
		{
			fExpandedState = fMask;
			CObList* pContentList = GetContentList();
			ASSERT(pContentList != NULL);
			if (pContentList != NULL)
			{
				::GetExpandedState(pContentList, fExpandedState, fMask);
			}
		}
	}

	// Get the CBuildSlob for the new target
	m_pTargetSlob = GetBuildNode()->GetTargetSlob(hTarget);
	if (m_pTargetSlob == NULL)
		return; // could happen during destruction

	// make the "view" slob available from target "build" slob
	m_pTargetSlob->SetSlobProp(P_ProxyContainer, this);

	// Set up our config name property
	CString strFlavour;
	CString strConfigName = m_pTargetSlob->GetConfigurationName();
	g_BldSysIFace.GetFlavourFromConfigName(strConfigName, strFlavour);
	CSlob::SetStrProp(P_ConfigName, strFlavour);

	// Add us as a dependant of the real wrapper, so that
	// if the real wrapper gets destroyed then we will get
	// notified.
	m_pTargetSlob->AddDependant(this);

	// Ok we have changed our target now.
 	m_hTarget = hTarget;

	// Force a repaint of our target node
    GetBuildNode()->HoldUpdates();

	InformDependants(SN_ALL);

	// reexpand if needed
	if (fExpandedState)
	{
		VERIFY(SUCCEEDED(pProjSysIFace->ExpandSlob(this, FALSE)));

		// need to walk the new content list and find the dependency node
		// expand it if we find it (we should)
		fExpandedState >>= 1;
		if (fExpandedState != 0)
		{

			CObList* pContentList = GetContentList();
			if (pContentList != NULL)
			{
				SetExpandedState(pContentList, fExpandedState);
			}
		}
	}
    GetBuildNode()->EnableUpdates();

	// add the IPkgProject pointer to the property map--this will
	// enable "Remove Project from Workspace" for this project node.

	CProject *pProject = g_BldSysIFace.CnvHBuilder(m_hBld);
	COleRef<IBSProject> pIBSProject = pProject->GetInterface();
	COleRef<IPkgProject> pIPkgProject;
	pIBSProject->QueryInterface(IID_IPkgProject, (LPVOID FAR *)&pIPkgProject);
	SetLongProp(P_IPkgProject, (long)(DWORD)(LPVOID)pIPkgProject);
	m_pTargetSlob->SetLongProp(P_IPkgProject, (long)(DWORD)(LPVOID)pIPkgProject);
}

UINT CBuildViewSlob::GetExpandedState( BOOL fIgnoreDeps /* = FALSE */)
{
	UINT fExpandedState = 0;
	UINT fMask = 1;

	int val;

	if (GetIntProp(P_IsExpanded, val) == valid)
	{
		if (val)
		{
			fExpandedState = fMask;
			CObList* pContentList = GetContentList();
			ASSERT(pContentList != NULL);
			if (pContentList != NULL)
			{
				::GetExpandedState(pContentList, fExpandedState, fMask, fIgnoreDeps);
			}
		}
	}
	return fExpandedState;
}

void GetExpandedState(CObList * pContentList, UINT & fExpanded, UINT & fMask, BOOL fIgnoreDeps /* = FALSE */)
{
	ASSERT(fMask!=0);
	ASSERT(pContentList != NULL);
	int val;
	int valExp;
	POSITION pos = pContentList->GetHeadPosition();
	while (pos != NULL)
	{
		CBuildSlob * pSlob = (CBuildSlob *)pContentList->GetNext(pos);
		if (pSlob != NULL)
		{
			VERIFY(pSlob->GetIntProp(P_IsExpandable, val) == valid);
			if (val)
			{
				CProjItem *pProjItem = pSlob->GetProjItem();
				ASSERT(pProjItem->IsKindOf(RUNTIME_CLASS(CProjContainer)));
				if ((!fIgnoreDeps) || (!pProjItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer))))
				{
					ASSERT(fMask != 0x80000000);
					fMask <<= 1;
					VERIFY(pSlob->GetIntProp(P_IsExpanded, valExp) == valid);
					if (valExp)
					{
						fExpanded |= fMask;  // mark this node as expanded and recurse
						if (pProjItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
							GetExpandedState(pSlob->GetContentList(), fExpanded, fMask);
					}
				}
			}
		}
	}
}

void SetExpandedState(CObList * pContentList, UINT & fExpanded)
{
	ASSERT(pContentList != NULL);
	int val;
	POSITION pos = pContentList->GetHeadPosition();
	ASSERT(pos != NULL);
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();

	while ((pos != NULL) && (fExpanded != 0))
	{
		CBuildSlob * pSlob = (CBuildSlob *)pContentList->GetNext(pos);
		if (pSlob != NULL)
		{
			VERIFY(pSlob->GetIntProp(P_IsExpandable, val) == valid);
			if (val)
			{
				if ((fExpanded & 1)!=0)
				{
					CProjItem *pProjItem = pSlob->GetProjItem();
					ASSERT(pProjItem->IsKindOf(RUNTIME_CLASS(CProjContainer)));
					VERIFY(SUCCEEDED(pProjSysIFace->ExpandSlob(pSlob, FALSE)));
					fExpanded >>= 1;
					if ((fExpanded != 0) && (pProjItem->IsKindOf(RUNTIME_CLASS(CProjGroup))))
						SetExpandedState(pSlob->GetContentList(), fExpanded);
				}
				else
				{
					fExpanded >>= 1;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::SetupPropertyPages, gets our property pages. We add in the 
// target filter selection property page, otherwise we thunk through to the 
// corresponding CBuildSlob

BOOL CBuildViewSlob::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
 	// thunk through to the CBuildSlob
	ASSERT(m_pTargetSlob);
	BOOL bRet = m_pTargetSlob->SetupPropertyPages(pNewSel, FALSE);

	CProjSlob::SetupPropertyPages(pNewSel, FALSE);

	if( bSetCaption )
		SetPropertyCaption(CString(MAKEINTRESOURCE(IDS_TARGET_FOLDER)));

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::CanAct, we can usually delete target nodes, unless we
// are the one and only remaining target

BOOL CBuildViewSlob::CanAct(ACTION_TYPE action)
{
	switch (action)
	{
	case act_cut:
	case act_copy:
	case act_paste:
	case act_delete:
	case act_rename:
		return FALSE;
	}

	return CSlob::CanAct(action);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::GetFilePath, get associated filename, if any:
const CPath * CBuildViewSlob::GetFilePath() const
{
	if (m_pTargetSlob==NULL)
		return NULL;

	return m_pTargetSlob->GetFilePath();
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::GetFileRegHandle, get associated file, if any
FileRegHandle CBuildViewSlob::GetFileRegHandle() const
{
	if (m_pTargetSlob==NULL)
		return NULL;

	return m_pTargetSlob->GetFileRegHandle();
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::OnInform, this handles notifications

void CBuildViewSlob::OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint)
{
	// ignore destroy messages
	if (idChange == SN_DESTROY)
	{
		// Do the base class thing
		CSlob::OnInform (pChangedSlob, idChange, dwHint);

		// Our target slob has gone away on us, so we'd
		// better null out our CBuildSlob pointer
		if (pChangedSlob == m_pTargetSlob)
			m_pTargetSlob = NULL;

		return;
	}
	
 	ASSERT_VALID (pChangedSlob);

	// Pass on to the project workspace window so it can update
	// if necessary.

	// shouldn't use these notifications with project stuff
	ASSERT (idChange != SN_SAVED && idChange != SN_LOADED &&
			idChange != SN_DIRTY && idChange != SN_CLEAN);

	// Forward tree changes to the tree slob
	if (idChange == SN_ADD || idChange == SN_REMOVE)
	{
		// Get the new slob
		CBuildSlob * pSlob = (CBuildSlob *)dwHint;
		
		// For add's and removes we have to really make our parent faking
		// look good. This means altering the containment of the slob being
		// added or deleted to really be contained in us. This is just to
		// ensure that the project window can do its stuff safely

		// Setup the container
		CBuildSlob * pOldContainer = pSlob->SetContainer((CSlob *)this);

		// Notify everybody
		InformDependants(idChange, dwHint);

		// Reset the container
		pSlob->SetContainer(pOldContainer);
		return;
	}

	// Handle our real properties and pass on any informs to the tree
	if (idChange == P_Title || idChange == P_IsExpanded || idChange == P_IsExpandable)
	{
		// We need to repaint so notify everybody
		InformDependants(idChange, dwHint);
		return;
	}

	// P_ProjWinTitle is used only to force items to be repainted so we always do
	// the repaint, regardless of what configs are involved.
	if (idChange == P_ProjWinTitle)
	{
		InformDependants(idChange, dwHint);
		return;
	}

	// only do this if the item changing concerns our config.
	if (((CProjItem *)pChangedSlob)->UsePropertyBag() != CurrBag)
		return;	// not interested
 	
	InformDependants(idChange, dwHint);

	// pass onto the base-class
	CSlob::OnInform (pChangedSlob, idChange, dwHint);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::SetSlobProp, override this so we can disconnect ourselves
// from the CBuildSlob hierachy

BOOL CBuildViewSlob::SetSlobProp(UINT idProp, CSlob * val)
{
	CString strTargetName;
    BOOL bTargetExists;
	BOOL bRemovingAllNodes = GetBuildNode()->RemovingAllNodes();

	// Make sure that the faked contents don't get moved too if we
	// are being moved into the undo slob or being deleted
	if (val == NULL)
	{
		// Get the name of this target before we delete ourselves
		// by moving into NULL.
		bTargetExists = g_BldSysIFace.GetTargetName(m_hTarget, strTargetName, m_hBld);
 
		// Remove ourselves as a dependent of the CBuildSlob
		if (m_pTargetSlob != NULL)
 			m_pTargetSlob->RemoveDependant(this);
 	}

	// Fix up target slob pointer if we are being moved into the
	// view
	if (val != NULL && m_hTarget != NULL)
		SetFilterTarget(m_hTarget);

	// Stop undo recording as we cannot undo addition/deletion of
	// a target
	theUndoSlob.Pause();
	 
	// Do the move
	BOOL bRetVal = CSlob::SetSlobProp(idProp, val);

	// Can only do this if undo recording is on, otherwise above SetSlobProp call
	// will have deleted us.
	if (val == NULL && theUndoSlob.IsRecording())
		m_pTargetSlob = NULL;

	// NOTE: when we have moved this slob into NULL, then at this point the object
	// will no longer exist, and so you cannot refer to member data from here to
	// the end of this function.

	// resume undo recording
 	theUndoSlob.Resume();

	// Were we successful
	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::Serialize, this is where we serialize the top level target
// nodes on display

void CBuildViewSlob::Serialize(CArchive & ar)
{
	CProject * pProject = g_pActiveProject;

    if (ar.IsStoring())
    {
		if (pProject->IsExeProject())
		{
			ar << m_strTarget;
		}
		else
		{
			// Convert our target handle into the real target name
			// and store that
			CString strTarget;
			g_BldSysIFace.GetTargetName(m_hTarget, strTarget, m_hBld);
			ar << strTarget;
		}
    }
    else
    {
        // Read the target name and set up the target we represent
		// based on it
        ar >> m_strTarget;

		if (pProject->IsExeProject())
		{
			SetIntProp(P_IsInvisibleNode, TRUE);	
		}
		else
		{
			SetIntProp(P_IsInvisibleNode, FALSE);

			m_hTarget = g_BldSysIFace.GetTarget(m_strTarget, m_hBld);
			SetFilterTarget(m_hTarget);

			// Add this new CBuildViewSlob node to the list maintained
			// by the build node
			GetBuildNode()->AddNode(this);
		}
    }
}

CSlobDragger *CBuildViewSlob::CreateDragger(CSlob *pDragObject, CPoint screenPoint)
{
	if (g_pProjWksWinIFace == NULL)
		return NULL;

	CSlobDraggerEx *pSlobDraggerEx = NULL;
	VERIFY(SUCCEEDED(g_pProjWksWinIFace->CreateDragger(&pSlobDraggerEx)));

	pSlobDraggerEx->AddItem(this);
	return pSlobDraggerEx;
}

void CBuildViewSlob::DoDrop(CSlob *pSlob, CSlobDragger *pDragger)
{
	CProject * pToProject = m_pTargetSlob->GetProjItem()->GetProject();
	
	if( pToProject ) 
		pToProject->BeginBatch();

	m_pTargetSlob->DoDrop(pSlob, pDragger);

	if( pToProject ) 
		pToProject->EndBatch();
}

void CBuildViewSlob::DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint)
{
	pDragger->Show(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::GetCmdIDs, top level target nodes support the "Add Project
// dependency" verb and also any verbs/commands that the associated CBuildSlob
// supports

void CBuildViewSlob::GetCmdIDs(CWordArray& aVerbs, CWordArray& aCmds)
{
	ASSERT(m_pTargetSlob);

	// Add in our CBuildSlobs commands and verbs
    m_pTargetSlob->GetCmdIDs(aVerbs, aCmds);

	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	CMultiSlob *pSelection;
	VERIFY(SUCCEEDED(pProjSysIFace->GetSelectedSlob(&pSelection)));

	if (pSelection->GetContentList()->GetCount() >= 1)
	{
		aVerbs.Add(IDM_PROJECT_SET_AS_DEFAULT);
		if (!g_Spawner.SpawnActive())
			aVerbs.Add(IDM_PROJECT_UNLOAD);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::SetIntProp, thunk nearly everything through to the CBuildSlob

BOOL CBuildViewSlob::SetIntProp(UINT idProp, int val)
{
	// Handle the wrappers own properties
	BOOL bRetVal;
	switch (idProp)
	{
		case P_IsExpanded:
 		case P_IsInvisibleNode:
			if (m_pTargetSlob != NULL)
				bRetVal = CSlob::SetIntProp(idProp, val);
			else
				bRetVal = TRUE;
			break;

		case P_IsExpandable:
            bRetVal = FALSE;
			break;

		default:
			ASSERT(m_pTargetSlob);
			bRetVal = m_pTargetSlob->SetIntProp(idProp, val);
			break;
	}

	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::SetStrProp, thunk everything through to the CBuildSlob

BOOL CBuildViewSlob::SetStrProp(UINT idProp, const CString& val)
{
    CString strNewName;

	if (idProp == P_ConfigName)
	{
        BOOL bRetVal;

		// Get the projects config array
		CProject * pProject = g_pActiveProject;

		CSlob::SetStrProp(idProp, val);

		theUndoSlob.Pause();

 		CString strConfigName = m_pTargetSlob->GetConfigurationName();
		CProjTempConfigChange projTempConfigChange(pProject);
		projTempConfigChange.ChangeConfig(strConfigName);

        // Get the current name
        CString strFrom;
		pProject->GetStrProp(P_ProjConfiguration, strFrom);

		// Form the whole configuration name
		int nFirst = strConfigName.Find(_T('-'));
		ASSERT(nFirst != -1);
 		strNewName = strConfigName.Left(nFirst);

        CProjItem * pItem = m_pTargetSlob->GetProjItem();
		const CPlatform * pPlatform = pItem->GetProjType()->GetPlatform();
        CString strPlatform = *(pPlatform->GetUIDescription());
		strNewName += _T("- ") + strPlatform + _T(" ") + val;
	
        // Check if we already have a target with the new name
        CTargetItem * pTarget = pProject->GetTarget(strNewName);
//        HBUILDER hBld = g_BldSysIFace.GetActiveBuilder();
        if (pTarget != NULL) // &&
//            g_BldSysIFace.CnvHTarget(hBld, m_hTarget) != pTarget)
        {
            MsgBox(Information, IDS_BUILD_DUPTARGET);
            bRetVal = FALSE;
        }   
  		else
			bRetVal = pProject->RenameTarget(strFrom, strNewName);
 
  		theUndoSlob.Resume();

		return bRetVal;
	}

	ASSERT(m_pTargetSlob);
	return m_pTargetSlob->SetStrProp(idProp, strNewName);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::GetIntProp, thunk nearly everything through to the CBuildSlob

GPT CBuildViewSlob::GetIntProp(UINT idProp, int &val)
{
	GPT gpt;

	switch (idProp)
	{
		case P_BoldWorkspaceText:
		{
			// Check to see if we are representing the active target
			HBLDTARGET hTarget = GetBuildNode()->GetActiveTarget();
			if(hTarget != NO_TARGET)
				val = (hTarget == GetTarget());
			else
				val = FALSE;
 			
  			gpt = valid;
			break;
		}

		case P_IsExpanded:
		case P_IsInvisibleNode:
			gpt = CSlob::GetIntProp(idProp, val);
			break;

        case P_IsExpandable:
            gpt = invalid;
            break;
		default:
			ASSERT(m_pTargetSlob);
			gpt = m_pTargetSlob->GetIntProp(idProp, val);
			break;
	}

	return gpt;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::GetStrProp, thunk everything through to the CBuildSlob

GPT CBuildViewSlob::GetStrProp(UINT idProp, CString &val)
{
	// May get asked when node is being deleted so we
	// just return invalid if we don't really have a CBuildSlob.
	if (m_pTargetSlob == NULL)
		return invalid;

	if (idProp == P_ConfigName)
	{
		GPT gpt;
		CString strConfig;
 		gpt = m_pTargetSlob->GetStrProp(P_ProjConfiguration, strConfig);

		if (g_BldSysIFace.GetFlavourFromConfigName(strConfig, val))
			return valid;
		else
			// Failed to find a matching platform
			return invalid;
 	}

	return m_pTargetSlob->GetStrProp(idProp, val);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::CanAdd, we determine here whether we have any buildslobs to
// paste into this target. Only CBuildSlobs can be pasted in targets

BOOL CBuildViewSlob::CanAdd(CSlob * pAddSlob)
{
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

			if (g_Spawner.SpawnActive() ||
			(!((CBuildSlob *)pSlob)->GetProjItem()->IsKindOf(RUNTIME_CLASS(CFileItem)) && 
			 !((CBuildSlob *)pSlob)->GetProjItem()->IsKindOf(RUNTIME_CLASS(CDependencyFile)) &&
			 !((CBuildSlob *)pSlob)->GetProjItem()->IsKindOf(RUNTIME_CLASS(CProjGroup))))
				return FALSE;
		}

		return TRUE;
	}
	// We can only add CBuildSlobs
	else if (pAddSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)) &&
			 (((CBuildSlob *)pAddSlob)->GetProjItem()->IsKindOf(RUNTIME_CLASS(CFileItem)) || 
			 ((CBuildSlob *)pAddSlob)->GetProjItem()->IsKindOf(RUNTIME_CLASS(CDependencyFile)) || 
			 ((CBuildSlob *)pAddSlob)->GetProjItem()->IsKindOf(RUNTIME_CLASS(CProjGroup))) && 
			 !g_Spawner.SpawnActive())
		return TRUE;

	// No CBuildSlobs so nothing to add
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::PrepareAddList, this just thunks through to the CBuildSlob.
// It filters the list of things we are about to paste into the target, such
// as removing files that are already in this target

void CBuildViewSlob::PrepareAddList(CMultiSlob* pAddList, BOOL fPasting)
{
	CBuildSlob * pBldSlob = GetBuildSlob();

	pBldSlob->PrepareAddList(pAddList, fPasting);
}

void CBuildViewSlob::PrepareDrop(CDragNDrop *pInfo)
{
	CBuildSlob * pBldSlob = GetBuildSlob();

	pBldSlob->PrepareDrop(pInfo);
}

void CBuildViewSlob::OnClean()
{
	ASSERT(m_pTargetSlob);
	m_pTargetSlob->OnClean();
}

#ifdef VB_MAKEFILES
void CBuildViewSlob::OnLaunchVB()
{
	ASSERT(m_pTargetSlob);
	
	CProject * pProject = m_pTargetSlob->GetProjItem()->GetProject();
	ShellExecute(NULL,NULL,pProject->m_strVBProjFile,NULL,NULL,SW_SHOW);
}
#endif

void CBuildViewSlob::OnAddGroup()
{
	ASSERT(m_pTargetSlob);
	m_pTargetSlob->OnAddGroup();
}

void CBuildViewSlob::OnUpdateAddGroup(CCmdUI *pCmdUI)
{
	ASSERT(m_pTargetSlob);
	m_pTargetSlob->OnUpdateAddGroup(pCmdUI);
}

void CBuildViewSlob::OnPopupFilesIntoProject()
{
	ASSERT(m_pTargetSlob);
	m_pTargetSlob->OnPopupFilesIntoProject();
}

void CBuildViewSlob::OnUpdateFilesIntoProject(CCmdUI * pCmdUI)
{
	ASSERT(m_pTargetSlob);
	m_pTargetSlob->OnUpdateFilesIntoProject(pCmdUI);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::OnBuild, support for context sensitive building, we pass
// this on to the CBuildSlob, which will handle it like any other verb.

void CBuildViewSlob::OnBuild()
{
	ASSERT(m_pTargetSlob);
	m_pTargetSlob->OnBuild();
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::OnBuildNoDeps, support for context sensitive building, we pass
// this on to the CBuildSlob, which will handle it like any other verb.

void CBuildViewSlob::OnBuildNoDeps()
{
	ASSERT(m_pTargetSlob);
	m_pTargetSlob->OnBuildNoDeps();
}

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob::OnSetDefault, set the selected target as the active target

void CBuildViewSlob::OnSetDefault()
{
	g_BldSysIFace.SetActiveTarget(m_hTarget, m_hBld);
}


void CBuildViewSlob::GetGlyphTipText(CString & str)
{
	str.Empty();
	const CPath * pPath = m_pTargetSlob->GetProjItem()->GetProject()->GetFilePath();
	if ((pPath != NULL) && (g_pSccManager->IsActive() == S_OK))
	{
		VERIFY(SUCCEEDED(g_pSccManager->GetStatusText(*pPath, str)));
	}
}

