#include "stdafx.h"
#pragma hdrstop

#include "engine.h"		// local header
#include "targitem.h"
#include "projdep.h"
#include "project.h"	// CProjTempConfigChange

IMPLEMENT_DYNAMIC(CBuildAction, CObject);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
//
// Class that is used to compare the base configuration of 
// a CConfigurationRecord* or CProjItem*.
//
class BaseConfiguration
{
public:
	BaseConfiguration( const CConfigurationRecord* pcr )
	{
		if ( NULL == pcr )
			m_pBaseRecord = NULL;
		else
			m_pBaseRecord = pcr->m_pBaseRecord;
	}
	BaseConfiguration( CProjItem* pItem )
	{
		if ( NULL == pItem )
			m_pBaseRecord = NULL;
		else
			m_pBaseRecord = pItem->GetActiveConfig()->m_pBaseRecord;
	}

	const CConfigurationRecord* m_pBaseRecord;
};

BOOL SameBaseConfig( BaseConfiguration a, BaseConfiguration b)
{
	//
	// return true if either is NULL since this is usually within an assert.
	// NULL conditions for a ProjItem* or CConfigurationRecord* should use a
	// separate assertion.
	//
	if ( NULL == a.m_pBaseRecord || NULL == b.m_pBaseRecord )
		return TRUE;
	else
		return a.m_pBaseRecord == b.m_pBaseRecord;
}
#endif

// Try to find a tool in an action in our list
VCPOSITION CBuildActionList::Find(CBuildTool * pTool)
{
	VCPOSITION pos = GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
	{
		VCPOSITION posCurr = pos;
		CBuildAction * pAction = (CBuildAction *)GetNext(pos);
		if (pTool == pAction->BuildTool())
			return posCurr;
	}

	return (VCPOSITION)NULL;
}

CBuildAction::CBuildAction(CProjItem * pItem, CBuildTool * pTool, BOOL fBuilder, CConfigurationRecord * pcr)
{
	VSASSERT( SameBaseConfig( pcr, pItem ), "Item and config record don't match!" );

	// associate
	m_pItem = pItem;
	m_pcrOur = pcr;

	m_pTool = m_pOldTool = (CBuildTool *)NULL;

	// bind to our tool?
	if (pTool != (CBuildTool *)NULL)
		Bind(pTool);
}

CBuildAction::~CBuildAction()
{
	// unbind from the tool?
	if (m_pTool != (CBuildTool *)NULL)
		UnBind();
}

// Binding and un-binding to tools
void CBuildAction::Bind(CBuildTool * pTool)
{
	// Set our tool
	SetToolI((CBuildTool *)pTool);
}

void CBuildAction::UnBind()
{
	// set our tool					   
	SetToolI((CBuildTool *)NULL);
}

void CBuildAction::SetToolI(CBuildTool * pTool)
{
	COptionHandler * popthdlr;

	if (m_pTool != (CBuildTool *)NULL)
	{
		// remove ourselves as a dependent of
		// the outputs of this tool
		popthdlr = m_pTool->GetOptionHandler();
		while (popthdlr != (COptionHandler *)NULL)
		{
			CBuildTool * pTool = popthdlr->m_pAssociatedBuildTool;
			if (pTool != (CBuildTool *)NULL)
				pTool->RemoveAction(this);
			popthdlr = popthdlr->GetBaseOptionHandler();
		}
	}
								   
	// add our input for this new tool?
	m_pOldTool = m_pTool;
	m_pTool = pTool;
	if( m_pTool != (CBuildTool *)NULL )
	{
		// make ourselves a dependent on
		// the outputs of this tool
		popthdlr = m_pTool->GetOptionHandler();
		while (popthdlr != (COptionHandler *)NULL)
		{
			CBuildTool * pTool = popthdlr->m_pAssociatedBuildTool;
			if (pTool != (CBuildTool *)NULL)
				pTool->AddAction(this);
			popthdlr = popthdlr->GetBaseOptionHandler();
		}
	}
}


void CBuildAction::Serialize(CArchive & ar)
{
	// don't bother with base-class
	 
	// storing?
	if (!ar.IsStoring())
	{
		// unknown tool?
		BOOL fIsSupported = FALSE;
		ar >> ((BYTE &)fIsSupported);

		// skip can we read this?
		if (!fIsSupported)
		{
			CString strDummy;
			BOOL fDummy = FALSE;

			ar >> ((BYTE &)fDummy);
			ar >> strDummy;
		}
		else if (fIsSupported)
		{
			// dirty command-line?
			BOOL fDirtyCommand = FALSE;
						
			ar >> ((BYTE &)fDirtyCommand);

			if (!fDirtyCommand)
			{
				BOOL fPossibleOptionChange = FALSE;
				ar >> ((BYTE &)fPossibleOptionChange);

				CString strDummy;
				ar >> strDummy;
			}
		}
	}
}

void CBuildAction::BindActions
(
	CProjItem * pTheItem,
	CVCPtrList * plstSelectTools /* = NULL */,
	CConfigurationRecord * pcr /* = (CConfigurationRecord *)NULL */,
	BOOL fBindContained /* = TRUE */
)
{
	VSASSERT(pTheItem != NULL, "Binding to NULL item!");
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProjTempConfigChange projTempConfigChange(pTheItem->GetProject());

	if (pcr != (CConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((CConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else {
		pcr = pTheItem->GetProject()->GetActiveConfig();
	}

	// valid?
	if (pTarget == (CTargetItem *)NULL || pTarget->GetActiveConfig()->IsValid())
	{
		CProjType * pProjType = pTarget->GetProjType();

		// Buildable project type?
		BOOL fBuildableProjType = !pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeUnknown));

		// Get the target type tools list
		CVCPtrList * plstTools = plstSelectTools == (CVCPtrList *)NULL ? pProjType->GetToolsList() : plstSelectTools;

		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups));
		plstContent = &lstContent;

		POSITION posItem = fBindContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;
		while (pItem != (CProjItem *)NULL)
		{
			// don't do this for project dependencies!
			if (!pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			{
				BOOL bItemIgnoreDefaultTool = FALSE;
				BOOL bIsFileItem = pItem->IsKindOf(RUNTIME_CLASS(CFileItem)); // includes (pItem->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem)))); // Hmmmm....
				if (bIsFileItem)
				{
					BOOL fIgnoreTool;
					bItemIgnoreDefaultTool = (pItem->GetIntProp(P_ItemIgnoreDefaultTool, fIgnoreTool) == valid && fIgnoreTool);
				}

				const CPath* pPath = pItem->GetFilePath();

				// Get the list of actions
				// Put these actions on the project if it's a target
				CProjItem * pItemActions = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;
				CBuildActionList * pActions = pItemActions->GetActiveConfig()->GetActionList();
				BOOL fActionsChange = FALSE;

				// Go through them and bind any tools to this item that want to be

				CBuildAction * pAction = NULL;

				CBuildTool * pTool;
				VCPOSITION pos = plstTools->GetHeadPosition();
				while (pos != (VCPOSITION)NULL)
				{
					pTool = (CBuildTool *)plstTools->GetNext (pos);

					// Ignore unknown tools
					if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
						continue;
					
					// skip default action if custom build tool specified
					if (bItemIgnoreDefaultTool && (!pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))))
						continue;

					// Allocate a 'building' action to to list?
					if (pTool->AttachToFile(pPath, pItem))
					{
						// Do we already have an 'old' action for this tool in our list?
						VCPOSITION posAction = pActions->GetHeadPosition();
						BOOL fFound = FALSE;
						while (posAction != (VCPOSITION)NULL)
						{
							pAction = (CBuildAction *)pActions->GetNext(posAction);

							if (pTool == pAction->m_pOldTool)
							{
								fFound = TRUE;
								break;
							}
						}

						// Re-bind?
						if (fFound)
						{
							// Set our config.
							VSASSERT(pAction, "No action?!?");
							pAction->m_pcrOur = pcr;
							VSASSERT( pAction->m_pItem == pItemActions, "Action's item doesn't match!" );
							VSASSERT( SameBaseConfig( pcr, pItemActions ), "Mismatch in config record!" );

							// Re-bind
							pAction->Bind(pTool);
						}
						// Assign!
						else
						{
							// check for multiple assignment
							VSASSERT(!pActions->Find(pTool), "Multiple assignment of tools!  NYI.");

							if(pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
								pItem->SetStrProp(P_ItemBuildTool, pTool->GetToolName());
							pAction = new CBuildAction(pItem, pTool, fBuildableProjType, pcr);
							pActions->AddTail(pAction);
							fActionsChange = TRUE;
						}
					}
					else
						continue;	// ignore
				}
				// Delete any actions not re-bound to
				VCPOSITION posAction = pActions->GetHeadPosition();
				while (posAction != (VCPOSITION)NULL)
				{
					VCPOSITION posActionHere = posAction;
					pAction = (CBuildAction *)pActions->GetNext(posAction);

					// Are we bound to a tool?
					if (pAction->m_pTool == (CBuildTool *)NULL)
					{
						// Remove from list and de-allocate
						pActions->RemoveAt(posActionHere);
						fActionsChange = TRUE;
						delete pAction;
					}
				}

				// Actions changed?
				if (fActionsChange)
				{
					// inform item dependents of output change
					pItem->InformDependants(P_ItemTools);
	#ifdef _DEBUG
					if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
					{
						VSASSERT(pActions->GetCount() <= 1, "Multiple actions for a file.  NYI.");
					}
	#endif
				}
			}

			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!

		}
	}
}

void CBuildAction::UnBindActions
(
	CProjItem * pTheItem,
	CVCPtrList * plstSelectTools /* = NULL */,
	CConfigurationRecord * pcr /* = (CConfigurationRecord *)NULL*/,
	BOOL fUnbindContained /* TRUE */  
)
{
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProjTempConfigChange projTempConfigChange(pTheItem->GetProject());

	if (pcr != (CConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((CConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else
	{
		pcr = pTheItem->GetProject()->GetActiveConfig();
	}

	// valid?
	if (pTarget == (CTargetItem *)NULL || pTarget->GetActiveConfig()->IsValid())
	{
		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups));
		plstContent = &lstContent;

		POSITION posItem = fUnbindContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;
		while (pItem != (CProjItem *)NULL)
		{
			// Get the list of actions
			// Put these actions on the project if it's a target
			CProjItem * pItemActions = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;
			CBuildActionList * pActions = pItemActions->GetActiveConfig()->GetActionList();

			// Go through them and unbind them from the tools
			// for their owner target

			CBuildAction * pAction;

			VCPOSITION pos = pActions->GetHeadPosition();
			while (pos != (VCPOSITION)NULL)
			{
				pAction = (CBuildAction *)pActions->GetNext(pos);

				// Un-bind from the tool?
				if (plstSelectTools != (CVCPtrList *)NULL && !plstSelectTools->Find(pAction->m_pTool))
					continue;	// no, it's not in our list

				pAction->UnBind();

				// Clear our config.
				pAction->m_pcrOur = pcr;
				VSASSERT( pAction->m_pItem == pItemActions, "Action list doesn't match!" );
				VSASSERT( SameBaseConfig( pcr, pItemActions ), "Config record doesn't match!" );
			}
			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
 		}
	}
}
	
void CBuildAction::AssignActions
(
	CProjItem * pTheItem,
	CVCPtrList * plstSelectTools /* = NULL */,
	CConfigurationRecord * pcr /* = NULL */,
	BOOL fAssignContained /* = TRUE */,
	BOOL fAssignContainedOnly /* = FALSE */
)
{
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProject * pProject = pTheItem->GetProject();
	CProjTempConfigChange projTempConfigChange(pProject);
	if (pcr != (CConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((CConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else
	{
		pcr = pProject->GetActiveConfig();
	}

	// valid?
	if (pTarget->GetActiveConfig()->IsValid())
	{
		CProjType * pProjType = pTarget->GetProjType();

		// Buildable project type?
		BOOL fBuildableProjType = !pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeUnknown));

		// Get the target type tools list
		CVCPtrList * plstTools = (plstSelectTools == (CVCPtrList *)NULL) ? pProjType->GetToolsList() : plstSelectTools;

		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups | CProjItem::flt_ExcludeProjDeps));
		plstContent = &lstContent;

		POSITION posItem = fAssignContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;

		if (fAssignContainedOnly)
		{
			//
			// Skip parent item and go to first item in its contents or NULL.
			//
			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
		}

		CBuildTool* pCustomBuildTool = NULL;
		while (pItem != (CProjItem *)NULL)
		{
			// don't do this for project dependencies!
			if (!pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			{
				BOOL bItemIgnoreDefaultTool = FALSE;
				BOOL bIsFileItem = pItem->IsKindOf(RUNTIME_CLASS(CFileItem)); // includes (pItem->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem))));

				if (bIsFileItem)
				{
					BOOL fIgnoreTool;
					bItemIgnoreDefaultTool = (pItem->GetIntProp(P_ItemIgnoreDefaultTool, fIgnoreTool) == valid && fIgnoreTool);
				}

				// stored on the project...even though attach to target
				const CPath* pPath = pItem->GetFilePath();

				// Get the list of actions
				// Put these actions on the project if it's a target
				// or default to the project if no target
				BOOL fTarget = pTarget == (CTargetItem *)NULL || pTarget == pItem;

				CProjItem * pItemActions = fTarget ? pItem->GetProject() : pItem;
				CBuildActionList * pActions = pItemActions->GetActiveConfig()->GetActionList();
				if (pItemActions->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem)))
				{
					if (pCustomBuildTool == NULL)
					{
						CVCPtrList* plstTools = pItem->GetProjType()->GetToolsList();
						VCPOSITION pos = plstTools->GetHeadPosition();
						while (pos != NULL && pCustomBuildTool == NULL)
						{
							CBuildTool* pTool = (CBuildTool*)plstTools->GetNext(pos);
							if (pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
								pCustomBuildTool = pTool;
						}
					}
					if (pCustomBuildTool != NULL)
					{
						CBuildAction* pEventAction = new CBuildAction(pItemActions, pCustomBuildTool, 
							TRUE, pItemActions->GetActiveConfig());
						pActions->AddTail(pEventAction);
					}
				}
				BOOL fActionsChange = FALSE;

				// Go through them and attach any to this item that want to be
				CBuildTool * pTool = NULL;
				VCPOSITION pos = plstTools->GetHeadPosition();

				// 	No need to loop through tools.
				if (pItem->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem)) ||
					(NULL==pPath && !fTarget) || (bIsFileItem && (pActions->GetCount() > 0)))
						pos = NULL;

				// Check to see if forced to a different tool than the default.
				CString strTool;
				{
					while (pos != (VCPOSITION)NULL)
					{
						pTool = (CBuildTool *)plstTools->GetNext (pos);
						

						// Ignore unknown tools
						if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
							continue;

						// skip default action if custom build tool specified
						if (bItemIgnoreDefaultTool && (!pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))))
							continue;

						CBuildAction * pAction;
						// allocate a 'building' + 'option' action to to list?

						// We are in bad shape if the active configuration of pItem doesn't match
						// pcr->m_pBaseRecord.
						VSASSERT( SameBaseConfig( pcr, pItem ), "Config record mismatch!  Deep trouble." );
						if (pTool->AttachToFile(pPath, pItem))
						{
							if(pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
								pItem->SetStrProp(P_ItemBuildTool, pTool->GetToolName());
							pAction = new CBuildAction(pItem, pTool, fBuildableProjType, pcr);
						}
						// allocate an 'option' action if the item is a target?
						else if (fTarget)
						{
							pAction = new CBuildAction(pItem, pTool, FALSE, pcr);
						}
						else
							continue;

						// check for multiple assignment
						VSASSERT(!pActions->Find(pTool), "Multiple tool assignment for file!  NYI.");

						pActions->AddTail(pAction);
						fActionsChange = TRUE;

						// cannot have multiple tools per-file for v3.0
						if (!fTarget && !pItem->IsKindOf(RUNTIME_CLASS(CProject)) )
							break;
					}
				}
				// Actions changed?
				if (fActionsChange)
				{
					// inform item dependents of output change
					pItem->InformDependants(P_ItemTools);
#ifdef _DEBUG
					if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem))) // includes (pItem->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem))))
					{
						VSASSERT(pActions->GetCount() <= 1, "Multiple tool assignment for file!  NYI.");
					}
#endif
				}
			}

			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
		}
	}
}

void CBuildAction::UnAssignActions
(
	CProjItem * pTheItem,
	CVCPtrList * plstSelectTools /* = NULL */,
	CConfigurationRecord * pcr /* = NULL */,
	BOOL fUnassignContained /* = TRUE */
)
{
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProjTempConfigChange projTempConfigChange(pTheItem->GetProject());
	if (pcr != (CConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((CConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else
	{
		pcr = pTheItem->GetProject()->GetActiveConfig();
	}

	// valid?
	if (pTarget == (CTargetItem *)NULL || (pTarget->GetActiveConfig() && pTarget->GetActiveConfig()->IsValid()))
	{
		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups | CProjItem::flt_ExcludeProjDeps));
		plstContent = &lstContent;

		POSITION posItem = fUnassignContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;
		while (pItem != (CProjItem *)NULL)
		{
			// don't do this for project dependencies!
			if (!pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			{
				// Get the list of actions
				// Put these actions on the project if it's a target
				CProjItem * pItemActions = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;
				if (pItemActions->GetActiveConfig())
				{
					CBuildActionList * pActions = pItemActions->GetActiveConfig()->GetActionList();
					BOOL fActionsChange = FALSE;

					// Go through them and detach them 

					CBuildAction * pAction;
					VCPOSITION pos = pActions->GetHeadPosition();
					while (pos != (VCPOSITION)NULL)
					{
						VCPOSITION posAction = pos;
						pAction = (CBuildAction *)pActions->GetNext (pos);

						// Remove from list and de-allocate?
						if (plstSelectTools != (CVCPtrList *)NULL && !plstSelectTools->Find(pAction->m_pTool))
							continue;	// no, it's not in our list
						pActions->RemoveAt(posAction);
						fActionsChange = TRUE;
						delete pAction;
					}

					// Actions changed?
					if (fActionsChange)
					{
						// inform item dependents of output change
						pItem->InformDependants(P_ItemTools);
					}
				}
			}

			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
 		}
	}
}	    
