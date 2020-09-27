//
// CSimpleBldNode
//
// Build Node
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "resource.h"

#include <prjapi.h>
#include <prjguid.h>

#include "bldslob.h"
#include "bldnode.h"	// our local header
#include "vwslob.h"
#include "exevw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Image wells for the glyphs we use in the build nodes

CImageWell g_imageWell;

/////////////////////////////////////////////////////////////////////////////
// Help functions for getting a pointer to the build node

CBuildNode* g_pBuildNode = 0;
CBuildNode* GetBuildNode()
{
    // return the build node pointer
	return g_pBuildNode;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode construction/destruction

CBuildNode::CBuildNode()
{
	// Initialize our build node pointer
	g_pBuildNode = this;

	// We are not removing any nodes yet!!
	m_bRemovingAllNodes = FALSE;

	// Set up the image wells
    if (!g_imageWell.IsOpen())
    {
 	    VERIFY(g_imageWell.Load(IDB_SINGLE_ITEM, CSize(16, 16)));
    	VERIFY(g_imageWell.Open());
	    VERIFY(g_imageWell.CalculateMask());
    }
}

CBuildNode::~CBuildNode()
{
	// Null out our build node pointer
	g_pBuildNode = NULL;
}

const TCHAR * CBuildNode::GetNodeName()
{
	// This is the name of our node type
	// FUTURE: Move to .rc. This is unused right now.
	static TCHAR szName[] = "Target";	
	return szName;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode, node creation for the active target/exe projects

CSlob * CBuildNode::CreateNode()
{
	// Are we an exe project
	if (g_BldSysIFace.GetBuilderType() == ExeBuilder)
	{
		// Exe project
		CExeViewSlob * pExeSlob = new CExeViewSlob();
		m_lstNodes.AddTail(pExeSlob);

		// return the exe project slob as our node
		return pExeSlob;
	}
	else
	{
		// We should have an active target
		HBLDTARGET hTarget = g_BldSysIFace.GetActiveTarget();
		ASSERT(hTarget != NO_TARGET);
		HBUILDER hBld = g_BldSysIFace.GetActiveBuilder();

		// Create the new node and add it to our list of top level nodes
		CBuildViewSlob * pViewSlob = new CBuildViewSlob(hTarget, hBld);
		m_lstNodes.AddTail(pViewSlob);
		
		// return the new node
		return pViewSlob;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode, node creation for a particular target. NOTE this will not
// work for exe projects

CSlob * CBuildNode::CreateNode(HBLDTARGET hTarget, HBUILDER hBld)
{
	// Should only be calling this for internal projects
	ASSERT(g_BldSysIFace.GetBuilderType() == InternalBuilder);

	// The specified target must be a target
	ASSERT(hTarget != NO_TARGET);

	// Create the node and add it to our list of top level nodes
	// ASSERT(hBld==ACTIVE_BUILDER || hBld == g_BldSysIFace.GetBuilder(hTarget));
	CBuildViewSlob * pViewSlob = new CBuildViewSlob(hTarget, hBld);
	m_lstNodes.AddTail(pViewSlob);

	// return the new node
	return pViewSlob;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::AddNode, this is used when we are serializing to add in the
// view nodes to our list of view nodes.

void CBuildNode::AddNode(CSlob * pViewSlob)
{
	// This is used when we load in our serialized
	// data, as we don't creat the slobs, but we need
	// to set up the list of top level nodes.
	m_lstNodes.AddTail(pViewSlob);
}

void CBuildNode::RemoveNode(CSlob * pViewSlob)
{
	POSITION pos = m_lstNodes.Find(pViewSlob);
	if (pos != NULL)
		m_lstNodes.RemoveAt(pos);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::SetNodeFilter, this is called just after creation and will
// display the property page for the new node, which gives the user a list
// of possible targets they can select from.

void CBuildNode::SetNodeFilter()
{
	// Show the property page
	ShowPropertyBrowser();

	// Activate the filter field
	UpdatePropertyBrowser();
	BeginPropertyBrowserEdit(P_Title);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::TargetNotify, we get here when a target was either deleted or
// added

void CBuildNode::TargetNotify(BOOL bDelete, HBLDTARGET hTarget)
{
	if (bDelete)
	{
		// A target has been deleted so we'd better remove
		// any top level nodes that reference it
        RemoveTargetSlob(hTarget);
 	}
	else
	{
		// Process target additions here
		// Ok add a new node automagically!
		HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);

		CTargetItem * pTarget = g_BldSysIFace.CnvHTarget(hBld, hTarget);

		CBuildViewSlob * pTargetSlob;
		HBLDTARGET hCurrTarg;	CTargetItem * pCurrTarg;
		POSITION pos = m_lstNodes.GetHeadPosition();
		while (pos != NULL)
		{
			pTargetSlob = (CBuildViewSlob *)m_lstNodes.GetNext(pos);
			hCurrTarg = pTargetSlob->GetTarget();

			pCurrTarg = g_BldSysIFace.CnvHTarget(hBld, hCurrTarg);
			if (pCurrTarg == pTarget)
			{
				HBLDTARGET hNewTarg;
				HFILESET hFileSet = g_BldSysIFace.GetFileSet(hBld, hCurrTarg);
				VERIFY(g_BldSysIFace.GetTargetFromFileSet(hFileSet, hNewTarg, hBld));
				if (hNewTarg == hTarget)
				{
					// we need to change the target that the node on display
					// represents
					pTargetSlob->SetFilterTarget(hTarget);
				}
				return;
			}
		}

		CDefAddBuildNode defNewNode(hTarget, hBld);

		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
		VERIFY(SUCCEEDED(pProjSysIFace->AddRootProvidedNode(&defNewNode, NULL)));
	}
}

void CBuildNode::ConfigChange()
{
    HoldUpdates();

	HBUILDER hBld = g_BldSysIFace.GetActiveBuilder();
	
	m_hActiveTarget = g_BldSysIFace.GetActiveTarget(hBld);

	HBUILDER hCurrBld;
	HBLDTARGET hCurrTarg;
    HFILESET hFileSet;
	CBuildViewSlob * pTargetSlob;
	POSITION pos = m_lstNodes.GetHeadPosition();
	while (pos != NULL)
	{
		pTargetSlob = (CBuildViewSlob *)m_lstNodes.GetNext(pos);
		hCurrTarg = pTargetSlob->GetTarget();
		hCurrBld = g_BldSysIFace.GetBuilder(hCurrTarg);
		if(hCurrBld)
		{
			hFileSet = g_BldSysIFace.GetFileSet(hCurrBld, hCurrTarg);

			// we could get null file set if hCurrTarg is to be deleted
			if (hFileSet != (HFILESET)NULL)
			{
				HBLDTARGET hTarg;
				g_BldSysIFace.GetTargetFromFileSet(hFileSet, hTarg, hBld /* REVIEW: hCurrBld? */);

				// Check whether hTarg is in the middle of being deleted; if so we must
				// avoid setting the CBuildViewSlob to display it.  This could happen
				// if hTarg was the active config, but was deleted, so we are now
				// changing the active config to a target with the same platform as hTarg
				// (but different fileset).  [olympus:1022]
				CString strTmp;
				if (g_BldSysIFace.GetTargetName(hTarg, strTmp, hCurrBld))
				{
					// It is OK to have this viewslob display its fileset's current
					// platform.
					pTargetSlob->SetFilterTarget(hTarg);
				}

				// case where non build system project is active ?
				if( hBld == NO_BUILDER ){
					// Force a repaint of our target node
					pTargetSlob->InformDependants(SN_ALL);
				}
			}
		}
	}	
	
	// make sure that active target node is visible
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	pos = m_lstNodes.GetHeadPosition();
	while (pos != NULL)
	{
		pTargetSlob = (CBuildViewSlob *)m_lstNodes.GetNext(pos);
		hCurrTarg = pTargetSlob->GetTarget();
		if (hCurrTarg == m_hActiveTarget)
		{
			pProjSysIFace->ScrollSlobIntoView(pTargetSlob, TRUE);
			break;
		}
	}

    EnableUpdates();
}

/////////////////////////////////////////////////////////////////////////////
// Remove all the nodes in the build pane for a particular project
void CBuildNode::RemoveNodes(HBUILDER hBld)
{
	// Get a pointer to the project workspace window interface
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	ASSERT(hBld);
	pProjSysIFace->FlushUndo();

	// Loop through all nodes that match hBld
	CBuildViewSlob * pTargetSlob;
	POSITION pos, posThis;
	pos = m_lstNodes.GetHeadPosition();
	while (pos != NULL)
	{
		// Get the next node
		posThis = pos;
		pTargetSlob = (CBuildViewSlob *)m_lstNodes.GetNext(pos);

		// Get the target corresponding to the target
		HBLDTARGET hTarget = pTargetSlob->GetTarget();

		// only interested in ones that match this builder
		HBUILDER hCmpBld = g_BldSysIFace.GetBuilder(hTarget);
		
		// ASSERT(hCmpBld!=NOBUILDER); // REVIEW: currently fails if exe project!
		if ((hCmpBld!=NO_BUILDER) && (hCmpBld!=hBld))
			continue;

		// attempt to remove this node--this will only succeed if the node
		// is visible in the FileView tree, and is unnecessary otherwise.
		pProjSysIFace->RemoveSlob(pTargetSlob, FALSE);
 	
		CBuildSlob * pBuildSlob;
		// Ok now delete the wrapper slobs
		if ((m_mapTargets.Lookup((void *&)hTarget, (void *&)pBuildSlob)))
		{
			delete pBuildSlob;

			// Remove the CBuildSlob entry in our map
			m_mapTargets.RemoveKey((void *&)hTarget);
		}
 	}
}

/////////////////////////////////////////////////////////////////////////////
// Remove all the nodes in the build pane

void CBuildNode::RemoveAllNodes()
{
	// We are removing all nodes (this is used to ignore informs)
	m_bRemovingAllNodes = TRUE;

	// Get a pointer to the project workspace window interface
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();

	// Loop through all nodes that are being shown
	CBuildViewSlob * pTargetSlob;
	POSITION pos, posThis;
	pos = m_lstNodes.GetHeadPosition();
	while (pos != NULL)
	{
		// Get the next node
		posThis = pos;
		pTargetSlob = (CBuildViewSlob *)m_lstNodes.GetNext(pos);

		// Get the target corresponding to the target
		HBLDTARGET hTarget = pTargetSlob->GetTarget();

		// Remove this node (this will cause it to be deleted)
		VERIFY(SUCCEEDED(pProjSysIFace->RemoveSlob(pTargetSlob, FALSE)));
 	
		CBuildSlob * pBuildSlob;
		// Ok now delete the wrapper slobs
		if ((m_mapTargets.Lookup((void *&)hTarget, (void *&)pBuildSlob)))
		{
			delete pBuildSlob;

			// Remove the CBuildSlob entry in our map
			m_mapTargets.RemoveKey((void *&)hTarget);
		}
 	}

	// Ok we have finished removing all the nodes
	m_bRemovingAllNodes = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::GetTargetSlob, this does a lookup to see if we have CBuildSlobs
// already for the given target, if not we create them

CBuildSlob * CBuildNode::GetTargetSlob(HBLDTARGET hTarget)
{
	HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
	if (hBld == NO_BUILDER)
		return NULL;

	// Should only be calling this for internal projects
	ASSERT(g_BldSysIFace.GetBuilderType(hBld) == InternalBuilder);

	// Find the wrapper slob for the given target, if it
	// doesn't exist then we attempt to create it
	CBuildSlob * pBuildSlob = NULL;
	if (!m_mapTargets.Lookup((void *&)hTarget, (void *&)pBuildSlob))
		pBuildSlob = CreateTargetSlob(hTarget);
	
	// Return the CBuildSlob
	return pBuildSlob;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::RemoveBuildSlob, this removes a build slob from our map. This
// is needed when a CBuildSlob is deleted because the underlying projitem has
// been deleted. In this case the CBuildSlob self destructs and so we need a 
// way to remove the entry in our map. Just before the CBuildSlob destructs
// we get called here, and so have a chance to remove the entry in the map


// FUTURE: Possible performance problem, as for every CBuildSlob that
// gets deleted this function will be called, and we do a search for it in our
// map. This could get to be a problem if we have a lot of targets.

void CBuildNode::RemoveBuildSlob(CBuildSlob * pSlob)
{
    // Ok now delete the wrapper slobs
    CBuildSlob * pBuildSlob;
    HBLDTARGET hTarget;
	POSITION pos = m_mapTargets.GetStartPosition();
	while (pos != NULL)
	{
		m_mapTargets.GetNextAssoc(pos, (void *&)hTarget, (void *&)pBuildSlob);
		if (pBuildSlob == pSlob)
		{
			// Found the target corresponding to the CBuildSlob so remove it
			m_mapTargets.RemoveKey((void *&)hTarget);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::RemoveTargetSlob, this really removes any CBuildViewSlobs currently
// representing the target being deleted. The CBuildSlob hierachy is also deleted

void CBuildNode::RemoveTargetSlob(HBLDTARGET hTarget)
{
	// Should only be calling this for internal projects
	ASSERT(g_BldSysIFace.GetBuilderType() == InternalBuilder);

	// A Target is being deleted
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();

	CBuildViewSlob * pTargetSlob;
	POSITION pos, posThis;
	pos = m_lstNodes.GetHeadPosition();
	while (pos != NULL)
	{
		posThis = pos;
		pTargetSlob = (CBuildViewSlob *)m_lstNodes.GetNext(pos);
		if (pTargetSlob->GetTarget() == hTarget)
		{
			// Ok we got the slob, so now tell the project window to remove it
			VERIFY(SUCCEEDED(pProjSysIFace->RemoveSlob(pTargetSlob, FALSE)));
		}
	}

    CBuildSlob * pBuildSlob;
    // Ok now delete the wrapper slobs
    
	if (m_mapTargets.Lookup((void *&)hTarget, (void *&)pBuildSlob))
    {
		delete pBuildSlob;

		// Remove the CBuildSlob entry in our map
		m_mapTargets.RemoveKey((void *&)hTarget);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::CreateSlobs, this function creates the wrapping CBuildSlobs for
// some CProjItems

CBuildSlob * CBuildNode::CreateSlobs(CProjItem * pItem, ConfigurationRecord * pcrBase, CBuildSlob * pParent, BOOL bClone /* = FALSE */)
{
	// Should only be calling this for internal projects
	ASSERT(g_BldSysIFace.GetBuilderType() == InternalBuilder);

	// Get the matching configuration for this item
	ConfigurationRecord * pcr = NULL;
	if (pcrBase != NULL)
	{
		pcr = pItem->ConfigRecordFromBaseConfig(pcrBase, TRUE);
	}
	else
	{
		ASSERT(bClone);
	}

	// Check that this dep cntr is in the same config as the one we are
	// intereested in.
	if (pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
	{
 		BOOL bOk = ((pcrBase != NULL) && (((CDependencyContainer *)pItem)->IsValid(pcrBase)));
		if (!bOk)
			return NULL;
	}

	// Create a wrapper for this proj item
	CBuildSlob * pSlob = new CBuildSlob(pItem, pcr);

	// Add the wrapper as a dependent of the real item
	pItem->AddDependant(pSlob);

	// Move this slob into its parent
	if (pParent != NULL)
	{
		if (bClone)
		{
			// fake MoveInto that isn't recorded
			pSlob->SetContainer(pParent);
			pParent->Add(pSlob);
		}
		else
		{
			pSlob->MoveInto(pParent);
		}
	}
 
	// Now create any wrappers for any children of this proj item
	CObList * pContentList = pItem->GetContentList();
	if (pContentList)
	{
		if (pParent == NULL)
			pSlob->UnThunkMoves();

		POSITION pos = pContentList->GetHeadPosition();
		while (pos != NULL)
		{
			CProjItem * pItem = (CProjItem *)pContentList->GetNext(pos);
			CreateSlobs(pItem, pcrBase, pSlob, bClone);
		}

		if (pParent == NULL)
			pSlob->ThunkMoves();
	}

	return pSlob;
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::InformBldSlobs, this passes informs onto any bldslob that added
// itself to our inform list. Bulk of the work is done in CBuildViewSlob, this
// really just thunks through to that implementation

void CBuildNode::InformBldSlobs(CSlob * pChangedSlob, UINT idChange, DWORD dwHint)
{ 
	// We must have a target node to do this
	if (m_mapTargets.GetCount() == 0)
		return;

	// Ok just get any random target node, in our case we get the first one
	HBLDTARGET hTarget;
	CBuildSlob * pBuildSlob;
	POSITION pos = m_mapTargets.GetStartPosition();
	m_mapTargets.GetNextAssoc(pos, (void* &)hTarget, (void *&)pBuildSlob);
	pBuildSlob->InformBldSlobs(pChangedSlob, idChange, dwHint);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildNode::CreateTargetSlob, this creates all the wrapping CBuildSlobs for
// a given target

CBuildSlob * CBuildNode::CreateTargetSlob(HBLDTARGET hTarget)
{
	// Get the active builder and the project
 	// HBUILDER hBld = g_BldSysIFace.GetActiveBuilder();
	HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
	CProjItem * pItem = g_BldSysIFace.CnvHTarget(hBld, hTarget);
	if (pItem == NULL)
		return NULL;	// could be NULL if unsupported platform

	// Get the configuration name
	CString strConfigName;
	VERIFY(g_BldSysIFace.GetTargetName(hTarget, strConfigName, hBld));

	// Get the configuration record pointer for this 
	ConfigurationRecord * pcr = pItem->ConfigRecordFromConfigName(strConfigName);
	pcr = (ConfigurationRecord *) pcr->m_pBaseRecord;

	// Create the wrapper slobs
	CBuildSlob * pTargetSlob = CreateSlobs(pItem, pcr, NULL);

	// Add this wrapped target to our list of wrapped targets
	m_mapTargets.SetAt((void *&)hTarget, pTargetSlob);

	// Return a pointer to the newly created wrapper for the target itself
	return pTargetSlob;
}

CSlob * CBuildNode::GetFirstTarget()
{
	if (!m_lstNodes.IsEmpty())
		// Get a build view slob that is on display
		return (CSlob *)m_lstNodes.GetHead();
    else
        return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ShowBuildPane makes the build pane visible

void CBuildNode::ShowBuildPane(BOOL bSetFocus /* = FALSE */)
{
	// We must have some nodes to make this visible
    CSlob * pTargetSlob = GetFirstTarget();
    if (pTargetSlob)
	{
		// Activate the pane containing the build view slob, which should be
		// the build pane.
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
		VERIFY(SUCCEEDED(pProjSysIFace->ActivateContainingPane(pTargetSlob, TRUE, bSetFocus)));
	}
}

/////////////////////////////////////////////////////////////////////////////
// HoldUpdates stops the workspace window from visually updating

void CBuildNode::HoldUpdates()
{
    CSlob * pTargetSlob = GetFirstTarget();
    if (pTargetSlob)
	{
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
        VERIFY(SUCCEEDED(pProjSysIFace->FreezeContainingPane(pTargetSlob)));
	}
}

/////////////////////////////////////////////////////////////////////////////
// EnableUpdates allows the workspace window to visually update

void CBuildNode::EnableUpdates()
{
    CSlob * pTargetSlob = GetFirstTarget();
    if (pTargetSlob)
	{
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
        VERIFY(SUCCEEDED(pProjSysIFace->ThawContainingPane(pTargetSlob)));
	}
}

void CBuildNode::AddDefaultNodes()
{
	// Do we need to add any default nodes
	if (m_lstNodes.IsEmpty())
	{
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
		CDefBuildNode DefNode;
		VERIFY(SUCCEEDED(pProjSysIFace->AddRootProvidedNode(&DefNode, NULL)));
 	}
}

/////////////////////////////////////////////////////////////////////////////
// CDefBuildNode construction/destruction

CDefBuildNode::CDefBuildNode()
{
	m_pTargets = NULL;
}

CDefBuildNode::~CDefBuildNode()
{
    if (m_pTargets)
        delete m_pTargets;
}

/////////////////////////////////////////////////////////////////////////////
// CDefBuildNode::CreateNode, this creates the default nodes for a builder

CSlob * CDefBuildNode::CreateNode()
{
    if (m_pTargets)
        delete m_pTargets;

	// We create a node for each target so we must use a MultiSlob
	m_pTargets = new CMultiSlob;

	// Are we dealing with an exe project
    if (g_BldSysIFace.GetBuilderType() == ExeBuilder)
    {
		// We only add one node for an exe project
        m_pTargets->Add(GetBuildNode()->CreateNode());
    }
    else
    {
    	// Enumerate all targets and add a node for each one
		CString strProject;
    	HBLDTARGET hTarget;
		HFILESET hFileSet;
		HBUILDER hBld;
		g_BldSysIFace.InitBuilderEnum();
		while ((hBld = g_BldSysIFace.GetNextBuilder(strProject, FALSE)) != NO_BUILDER)
		{
			g_BldSysIFace.InitFileSetEnum(hBld);
			while ((hFileSet = g_BldSysIFace.GetNextFileSet(hBld)) != NO_FILESET)
  			{
		    	g_BldSysIFace.GetTargetFromFileSet(hFileSet, hTarget, hBld);
				if (hTarget != NO_TARGET)
				{
					m_pTargets->Add(GetBuildNode()->CreateNode(hTarget, hBld));
				}
		    }
		}
    }

	// Return our nodes
	return m_pTargets;
}

void CDefBuildNode::Add(CSlob * pNewNode)
{
	if (m_pTargets==NULL)
		m_pTargets = new CMultiSlob;
	m_pTargets->Add(pNewNode);
}

/////////////////////////////////////////////////////////////////////////////
// CDefAddBuildNode construction/destruction

CDefAddBuildNode::CDefAddBuildNode(HBLDTARGET hTarget, HBUILDER hBld)
{
	m_hTarget = hTarget;
	m_hBld = hBld;
}

CDefAddBuildNode::~CDefAddBuildNode()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDefAddBuildNode::CreateNode, this class is used to add a new node to the
// build pane

CSlob * CDefAddBuildNode::CreateNode()
{
 	return (GetBuildNode()->CreateNode(m_hTarget, m_hBld));
}

#ifdef CUSTOM_BLDNODES

/////////////////////////////////////////////////////////////////////////////
// Target Filter property page

IMPLEMENT_DYNAMIC(CBuildGeneralPage, CSlobPage)

BEGIN_IDE_CONTROL_MAP(CBuildGeneralPage, IDDP_PROPERTIES_TARGETS, IDS_PROPERTIES_TARGETS)
	MAP_EDIT(IDC_TARGET, P_Title)
END_IDE_CONTROL_MAP()

CBuildGeneralPage g_BuildGeneralPage;

/////////////////////////////////////////////////////////////////////////////
// CBuildGeneralPage::InitializePage, fill the Target Filter combo with the
// available targets

void CBuildGeneralPage::InitializePage()
{
	// Base class initialization
	CSlobPage::InitializePage();

	// Clear out the combos contents
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_TARGET);
	pCombo->ResetContent();
	
	// Prepare to enumerate the targets
	HBUILDER hBld = g_BldSysIFace.GetActiveBuilder();
	g_BldSysIFace.InitTargetEnum(hBld);

	// Enumerate all the targets
	CString strTarget;
	HBLDTARGET hTarget;
	int nIndex;
	hTarget = g_BldSysIFace.GetNextTarget(strTarget, hBld);
	while (hTarget != NO_TARGET)
	{
		// Add the target to the combo box
		nIndex = pCombo->AddString(strTarget);

		// Select this if it is the currently active target for
		// the selected CBuildViewSlob
		if (hTarget == ((CBuildViewSlob *)m_pSlob)->GetTarget())
			pCombo->SetCurSel(nIndex);

		// Continue enumeration
		hTarget = g_BldSysIFace.GetNextTarget(strTarget, hBld);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBuildGeneralPage::Validate, this is where we change the target that the
// CBuildViewSlob represents

BOOL CBuildGeneralPage::Validate()
{
	// Get the index of the selected item
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_TARGET);
	int nIndex = pCombo->GetCurSel();

	// If we have an item then try to change the target
	if (nIndex != LB_ERR)
	{
		// Get the text currently selected
		CString strTarget;
		pCombo->GetLBText(nIndex, strTarget);

		// If we have a target name then look it up and set it
		// as our filter
		if (!strTarget.IsEmpty())
		{
			HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strTarget);
			((CBuildViewSlob *)m_pSlob)->SetFilterTarget(hTarget);
		}
		else
			return FALSE;
	}
	return CSlobPage::Validate();
}

#endif
