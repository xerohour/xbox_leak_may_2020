#include "stdafx.h"

#include "slob.h"
#include "util.h"
#include "utilauto.h"
#include "prjapi.h"
#include "prjguid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// IMPLEMENT_DYNCREATE needs real new.
#undef new
#endif

IMPLEMENT_SERIAL(CSlob, CObject, 1)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define theClass CSlob
PRD BASED_CODE CSlob::m_rgprd [] =
{
	{ P_BEGIN_MAP, NULL, PRD::null, "CSlob" },
	SLOB_PROP(Container)
	{ P_END_MAP, NULL, PRD::null, NULL }
};
#undef theClass

const PRD* PrdFindProp(const PRD* propMap, UINT idProp)
{
	for ( ; propMap != NULL; propMap = (PRD*)propMap[0].pbOffset)
	{
		for (int iProp = 1; propMap[iProp].nProp != P_END_MAP; iProp += 1)
		{
			if (propMap[iProp].nProp == idProp)
				return &propMap[iProp];
		}
	}

	return NULL;
}

CSlob::CSlob()
{
	m_pContainer = NULL;
	m_cDep = 0;
}

CSlob::~CSlob()
{
	InformDependants(SN_DESTROY);

	ASSERT(m_cDep >= 0 && m_cDep <= 3);	// 3 means we are using a CObList.
	if (m_cDep == 3)
		delete (CObList*) m_aDep[0];
}


// Return a list containing any contained (child) slobs.  The default
// is for slobs to not be containers.  Override for any slob that may
// contain other slobs.
//
CObList* CSlob::GetContentList()
{
	return NULL;
}


// Create a "clone" of this.  The clone should not have a container and
// some other information may not be copied (such as a symbol).  This
// is used by the Cut, Copy and Paste commands, Ctrl+Drag, and any other
// user actions that need to copy slobs...
//
// The default coppies all properties except P_Container and P_ID as well
// as all decendants of the slob.  If the slob is a CMultiSlob, all the
// slobs it references as well as their decendants are clones.
//
// Override this for slobs that have other cloning needs, but be sure to
// call this function first thing...
//
CSlob* CSlob::Clone()
{
	CRuntimeClass* pClass = GetRuntimeClass();
	CSlob* pClone = (CSlob*)pClass->CreateObject();
	ASSERT(pClone != NULL);

	CopyProps(pClone);

	// Now clone any contained slobs as well.
	//
//
// FUTURE:	This function does some really messy hacks at the end to make
//			Clone() work properly for both Slobs and MultiSlobs, cloning
//			them in the proper order such that the clone is ordered
//			identically do the original.  See the HACK comment below to
//			understand why this is needed.  It should eventually be
//			cleaned	up -- ggh 02-Sep-92
//
	// HACK:	Note that we have two different algorithms for cloning a
	// 			slob, depending on whether this is a multislob or not.
	//			This is because a MULTISLOB currently (as of v1 alpha)
	//			behaves by adding to the head instead of the tail, which
	//			would cause Clone() to reverse the list, were we not
	//			careful.  The proper long-term fix is to change the places
	//			that depend on Add doing an AddHead to expect Add to do an
	//			AddTail.  The AddHead hack is used to make selections easy
	//			to deal with (making something the head of the multislob
	//			makes it the dominant selection).	ggh 02-Sep-92

	if (GetContentList() != NULL)
	{
		if (IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		{
			CObList *plist = GetContentList();
			POSITION pos = plist->GetTailPosition();
			while (pos != NULL)
			{
				CSlob* pContentClone = ((CSlob *) plist->GetPrev(pos))->Clone();
				if (pContentClone != NULL)
					pClone->Add(pContentClone);
			}
		}
		else
	    {
			POSITION pos = GetHeadPosition();
			while (pos != NULL)
			{
				CSlob* pContentClone = GetNext(pos)->Clone();
				if (pContentClone != NULL)
				{
					pContentClone->m_pContainer = pClone;
					pClone->Add(pContentClone);
				}
			}
	    }
	}

	return pClone;
}

///////////////////////////////////////////////////////////////////////////////
//	Copy the properties for this slob, both map and bag
void CSlob::CopyProps(CSlob * pClone)
{
	ASSERT(pClone != (CSlob *)NULL);

	// Copy most of the properties...
	const PRD* rgprd = GetPropMap();
	for ( ; rgprd != NULL; rgprd = (const PRD*)rgprd[0].pbOffset)
	{
		for (int iprd = 1; rgprd[iprd].nProp != P_END_MAP; iprd += 1)
		{
			if (rgprd[iprd].pbOffset == 0)
				continue;

			switch (rgprd[iprd].nProp)
			{
			case P_Container:
			case P_ID:
				// Don't ever copy these properties!
				continue;
			}

			BYTE* pbProp = (BYTE*)this + (DWORD)rgprd[iprd].pbOffset;
			BYTE* pbCloneProp = (BYTE*)pClone + (DWORD)rgprd[iprd].pbOffset;

			switch (rgprd[iprd].nType)
			{
			case PRD::null:
			case PRD::action:
				break;

			case PRD::booln:
			case PRD::integer:
				*(int*)pbCloneProp = *(int*)pbProp;
				break;

			case PRD::longint:
				*(long*)pbCloneProp = *(long*)pbProp;
				break;

			case PRD::number:
				*(double*)pbCloneProp = *(double*)pbProp;
				break;

			case PRD::string:
				*(CString*)pbCloneProp = *(CString*)pbProp;
				break;

			case PRD::slob:
				// NOTE:  This only copies a reference to the
				// slob, it doesn't copy the slob itself!
				*(CSlob**)pbCloneProp = *(CSlob**)pbProp;
				break;

			case PRD::rect:
				*(CRect*)pbCloneProp = *(CRect*)pbProp;
				break;

			case PRD::point:
				*(CPoint*)pbCloneProp = *(CPoint*)pbProp;
				break;
			}
		}
	}

	// Clone bagged properties...
	int nBags = GetPropBagCount();
	for (int i = 0; i < nBags; i += 1)
	{
		CPropBag* pBag = GetPropBag(i);
		ASSERT(pBag != NULL);

		pClone->ClonePropBag(pBag, i);
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Clone a the property bag into 'this' CSlob's property bag for
//  nBag (-1 default)
//

void CSlob::ClonePropBag(CPropBag * pBag, int nBag, BOOL fEmpty /*=TRUE*/)
{
	CPropBag * pCloneBag = GetPropBag(nBag);

	pCloneBag->Clone(this, pBag, fEmpty) ;
}

///////////////////////////////////////////////////////////////////////////////
//	Serialize all the properties of a CSlob except P_Container.
//
//	Note that since we go through all the slob's property maps, derived classes
//	will be serialized correctly, and don not need to override Serailize just
//	to get their props saved.

void CSlob::SerializePropMap(CArchive& ar)
{
	const PRD* rgprd = GetPropMap();

	// Loop through slob's prop map and its base classes:
	for ( ; rgprd != NULL; rgprd = (const PRD*)rgprd[0].pbOffset)
	{
		for (int iprd = 1; rgprd[iprd].nProp != P_END_MAP; iprd += 1)
		{
			if (rgprd[iprd].pbOffset == 0)
				continue;

			// do we want to filter out this property id?
			if (!SerializePropMapFilter(rgprd[iprd].nProp))
				continue;

			switch (rgprd[iprd].nProp)
			{
			case P_Container:
				continue;
			}

			BYTE* pbProp = (BYTE*)this + (DWORD)rgprd[iprd].pbOffset;

			if (ar.IsStoring())
			{

				switch (rgprd[iprd].nType)
				{
				case PRD::null:
				case PRD::action:
					break;

				case PRD::booln:
				case PRD::integer:
				case PRD::longint:
					// Archive int's and long's as DWORD to avoid ambiguity:
					ar << ((DWORD) *(int*)pbProp);	
					break;
				case PRD::number:
					ar << *(double*)pbProp;
					break;

				case PRD::string:
					ar << *(CString*)pbProp;
					break;

				case PRD::slob:
					// NOTE:  This only copies a reference to the
					// slob, it doesn't copy the slob itself!
					ar << *(CSlob**)pbProp;
					break;

				case PRD::rect:
					ar << *(CRect*)pbProp;
					break;

				case PRD::point:
					ar << *(CPoint*)pbProp;
					break;
				}	// switch (rgprd[iprd].nType)
			}	// ar.IsStoring()
			else
			{

				switch (rgprd[iprd].nType)
				{
				case PRD::null:
				case PRD::action:
					break;

				case PRD::booln:
				case PRD::integer:
				case PRD::longint:
					// Archive int's and long's as DWORD to avoid ambiguity:
					ar >> ((DWORD&) *(int*)pbProp);	
					break;
				case PRD::number:
					ar >> *(double*)pbProp;
					break;

				case PRD::string:
					ar >> *(CString*)pbProp;
					break;

				case PRD::slob:
					// NOTE:  This only copies a reference to the
					// slob, it doesn't copy the slob itself!
					ar >> *(CSlob**)pbProp;
					break;

				case PRD::rect:
					ar >> *(CRect*)pbProp;
					break;

				case PRD::point:
					ar >> *(CPoint*)pbProp;
					break;
				}	// switch (rgprd[iprd].nType)

			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//
//	MSF Serialization functions:  Prop map + content list:

void CSlob::Serialize(CArchive& ar)
{
	CObList *pObList;
	POSITION pos;


	SerializePropMap ( ar );
		
	// Content list:
	pObList = GetContentList ();
	if (pObList)
	{
		// Call serialize directly since this is really a member:
	 	pObList->Serialize (ar);

		// If we're loading, need to set containment property as well
		// FUTURE: Should we serialize manually so we can call Add for each 
		// object?
		if (ar.IsLoading())
		{
			for (pos = GetHeadPosition (); pos != NULL; )
			{
				GetNext(pos)->m_pContainer = this;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// Return the CSlobWnd that 'this' is displayed in.  Default is to see if
// our container knows.  This must be overridden in slobs that actually are
// in windows, or at least the top-most slob in a window (for example, a
// string table needs to know, but each string doesn't).  If a slob is
// displayed in more than one CSlobWnd, this should return the first in
// a list.
//
CSlobWnd* CSlob::GetSlobWnd() const
{
	CSlob* pSlob = m_pContainer;
	while (pSlob != NULL)
	{
		CSlobWnd* pWnd = pSlob->GetSlobWnd();
		if (pWnd != NULL)
			return pWnd;
		
		pSlob = pSlob->m_pContainer;
	}
	
	return NULL;
}

CSlob* CSlob::GetRootContainer()
{
	if (m_pContainer == NULL)
	{
		return(this);
	}
	else
	{
		CSlob* pRootSlob = this;
		while (pRootSlob->m_pContainer != NULL)
		{
			pRootSlob = pRootSlob->m_pContainer;
		}
		return(pRootSlob);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Drag and Drop Support

CSlob*	CSlob::DropTargetFromPoint(CPoint screenPoint)
{
 	CSlobWnd* pWnd = (CSlobWnd*)CWnd::WindowFromPoint(screenPoint);
	ASSERT(pWnd != NULL);
	
	while (pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CSlobWnd)))
		pWnd = (CSlobWnd*)pWnd->GetParent();
	
	if (pWnd == NULL)
		return NULL;

	CPoint	ptClient = screenPoint;
	pWnd->ScreenToClient(&ptClient);
	CSlob* pSlob = pWnd->DropTargetFromPoint(ptClient);
	if (pSlob != NULL && pSlob->GetContentList() == NULL)
	{
		pSlob = pSlob->GetContainer();
		if (pSlob == NULL)
			return NULL;
		
		ASSERT(pSlob->GetContentList() != NULL);
	}
	
	return pSlob;
}


// Create a CSlobDragger appropriate for dragging 'pDragObject' around over
// 'this'.  'screenPoint' is the mouse cursor position in screen coordinates.
//
CSlobDragger* CSlob::CreateDragger(CSlob *pDragObject, CPoint screenPoint)
{
	return NULL;
}


// Handle a mouse movement during a drag operation.  'pSlob' is being dragged
// over 'this' with the 'pDragger' created when the mouse first entered our
// space.
//
void CSlob::DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint)
{
}


// Called right before the undo for a drop is setup...
//
void CSlob::PrepareDrop(CDragNDrop* pInfo)
{
}


// A slob has been dropped on 'this'.  It's up to derived classes to
// decide what to do...
//
void CSlob::DoDrop(CSlob *pSlob, CSlobDragger *pDragger)
{
	// Delete this.  It won't be used, if this gets called.
	TRACE( "Failed drag-drop.  CSlob deleted.\n" );
	delete pSlob;
}


// Return an index into the cursor table for dragging this slob.
//
int CSlob::GetCursorIndex()
{
	ASSERT(FALSE); // Drag-dropable objects must define this function.
	return -4;
}


// Return a real cursor for dragging this slob.
//
HCURSOR CSlob::GetCursor(int ddcCursorStyle)
{
	TRACE("Missing a drag cursor!\n");
	return LoadCursor(NULL, IDC_ARROW);
}


/////////////////////////////////////////////////////////////////////////////
// Property Page Interface

BOOL CSlob::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Property Bag Interface

// Get the number of property bags this CSlob has.
//
int CSlob::GetPropBagCount() const
{
	return 0;
}


// Return a specific property bag from this CSlob (index based), or 
// the "current" property bag (when nBag is -1).
//
CPropBag* CSlob::GetPropBag(int nBag /* = -1 */)
{
	ASSERT(nBag == -1);
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Custom Undo/Redo Action Handling

void CSlob::UndoAction(CSlobSequence* pSeq, UINT idAction)
{
	switch (idAction)
	{
	default:
		ASSERT(FALSE); // Must handle custom actions in derived class!
		return;

//NYI	case A_ADDPROP:
		// FUTURE: Record a remove prop action...
//NYI		theUndoSlob.OnAddProp(this, pBag, nPropID);
//NYI		break;

//NYI	case A_REMOVEPROP:
		// FUTURE: Record an add prop action...
//NYI		switch (nType)
//NYI		{
//NYI		case integer:
//NYI			theUndoSlob.OnRemoveProp(this, pBag, nPropID, ((CIntProp*)pProp)->m_nValue);
//NYI			break;
//NYI		}
//NYI		break;
	}
}


void CSlob::DeleteUndoAction(CSlobSequence* pSeq, UINT idAction)
{
	// Default is to do nothing.
	//
	// Override this if you have defined a custom undo action and
	// need to perform any cleanup when the action is deleted.
}

void CSlob::BeginUndo(UINT nID)
{
	CSlobWnd *pWnd = GetSlobWnd();
	if(NULL != pWnd)
		pWnd->BeginUndo(nID);
}

void CSlob::EndUndo(BOOL bAbort /*= FALSE*/)
{
	CSlobWnd *pWnd = GetSlobWnd();
	if(NULL != pWnd)
		pWnd->EndUndo(bAbort);
}

/////////////////////////////////////////////////////////////////////////////


BOOL CSlob::CanAdd(CSlob* pAddSlob)
{
	if ((pAddSlob == &theClipboardSlob) ||
		(pAddSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob))) )
	{
		// It is a list of slobs, so make sure that all of them are okay
		POSITION pos = pAddSlob->GetHeadPosition();
		if (pos == NULL)
			return FALSE;

		while (pos != NULL)
		{
			if (!CanAdd(pAddSlob->GetNext(pos)))
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}


void CSlob::PrepareAddList(CMultiSlob* pAddList, BOOL fPasting)
{
	// Default is to do nothing

	// Override this if you need to prepare for having a list added
	// to a container.  (E.g. the dialog editor uses this to setup
	// the m_nOrder properties of the slobs to be added so the tab
	// order is correct after a drop/paste.)
}

//
// CSlob::Add(aSlob)
//
// Adds <aSlob> to the content list of this slob.  In order for this to
// work, this slob must have a content list.
//
// NOTE: 	Add, by convention, always adds to the TAIL of a slob, not
//			to the head.
//

void CSlob::Add(CSlob* pAddSlob)
{
	CObList* pContentList = GetContentList();
	ASSERT(pContentList != NULL);
	ASSERT(pContentList->Find(pAddSlob) == NULL);
	pContentList->AddTail(pAddSlob);

	InformDependants(SN_CONTENT);
	InformDependants(SN_ADD, (DWORD)pAddSlob);
}


// See if a slob can be removed from 'this'.  Return TRUE if it would
// be okay, or FALSE if it would not.  The default says no, unless the
// slob to be removed is a CMultiSlob, in which case each slob referenced
// by the CMultiSlob is checked; if they can all bew removed, TRUE is
// returned.  Note that this must be overridden if anything is ever to
// be removable.
//
// This function is usually used for things like enabling the Delete and
// Cut commands.
//
BOOL CSlob::CanRemove(CSlob* pRemoveSlob)
{
	if (pRemoveSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
	{
		// It is a list of slobs, so make sure that all of them are okay
		POSITION pos = pRemoveSlob->GetHeadPosition();
		if (pos == NULL)
			return FALSE;

		while (pos != NULL)
		{
			if (!CanRemove(pRemoveSlob->GetNext(pos)))
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}


// Removes a slob from 'this'.  Do not call this function!  If you want to
// remove a slob, use MoveInto to move it somewhere else (or NULL to delete
// it).  MoveInto will call this to actually perform the removal.
//
// Override this in slob container classes that must perform any extra
// processing when a slob is removed from it.
//
void CSlob::Remove(CSlob* pRemoveSlob)
{
	CObList* pContentList = GetContentList();
	ASSERT(pContentList != NULL);
	POSITION pos = pContentList->Find(pRemoveSlob);
	ASSERT(pos != NULL);
	pContentList->RemoveAt(pos);
	pRemoveSlob->m_pContainer = NULL;

	InformDependants(SN_CONTENT);
	InformDependants(SN_REMOVE, (DWORD)pRemoveSlob);
}

/////////////////////////////////////////////////////////////////////////////
//	Change Notification Mechanism
//		We optimize for 2 or fewer dependencies, since that all the vast
//		majority of slobs will ever have.

// Add a dependant to 'this'.  The dependant will be notified of any
// property changes or the destruction of 'this'.
//
void CSlob::AddDependant(CSlob* newDependant)
{
	ASSERT(m_cDep >= 0 && m_cDep <= 3);	// 3 means we are using a CObList.
	if (m_cDep < 2)
	{
		if (m_cDep == 0 || m_aDep[0] != newDependant)
			m_aDep[m_cDep++] = newDependant;
	}
	else	// 3 dependants can't fit in the cache.
	{
		if (m_cDep == 2)
		{
			if (m_aDep[0] == newDependant || m_aDep[1] == newDependant)
				return;

			CObList* pExpensiveDependencies = new CObList;
			pExpensiveDependencies->AddTail(m_aDep[0]);
			pExpensiveDependencies->AddTail(m_aDep[1]);
			pExpensiveDependencies->AddTail(newDependant);
			m_aDep[0] = pExpensiveDependencies;
			m_cDep++;
		}
		else
		{
			POSITION pos = ((CObList*) m_aDep[0])->Find(newDependant);
			if (pos == NULL)
				((CObList*) m_aDep[0])->AddTail(newDependant);
		}
	}

	ASSERT(m_cDep >= 0 && m_cDep <= 3);
}


// Remove a dependant of 'this'.
//
void CSlob::RemoveDependant(CSlob* oldDependant)
{
	ASSERT(m_cDep >= 0 && m_cDep <= 3);	// 3 means we are using a CObList.
	if (m_cDep == 1 || m_cDep == 2)
	{
		if (m_aDep[0] == oldDependant)
		{
			if (m_cDep == 2)
				m_aDep[0] = m_aDep[1];

			m_cDep--;
		}
		else if (m_cDep == 2 && m_aDep[1] == oldDependant)
		{
			m_cDep--;
		}
	}
	else if (m_cDep == 3)
	{
		ASSERT(((CObList*) m_aDep[0])->GetCount() > 2);

		POSITION pos = ((CObList*) m_aDep[0])->Find(oldDependant);
		if (pos != NULL)
		{
			((CObList*) m_aDep[0])->RemoveAt(pos);
			if (((CObList*) m_aDep[0])->GetCount() == 2)
			{
				CObList* pExpensiveDependencies = (CObList*) m_aDep[0];
				m_aDep[0] = pExpensiveDependencies->RemoveHead();
				m_aDep[1] = pExpensiveDependencies->RemoveHead();
				delete pExpensiveDependencies;
				m_cDep--;
			}
		}
	}
	ASSERT(m_cDep >= 0 && m_cDep <= 3);
}


// Inform all dependants of 'this' that a change has occured.  Special
// notifications SN_DESTROY and SN_ALL indicate that this slob is being
// destroyed or that all of it's properties may have changed respectively.
// This function is also used when the validity of a property has changed
// so that dependants will find out when a property has been enabled or
// disabled.
//
#define MAX_SAFE_DEPENDANTS 128

void CSlob::InformDependants(UINT idChange, DWORD dwHint)
{
	ASSERT(m_cDep >= 0 && m_cDep <= 3);	// 3 means we are using a CObList.
	if (m_cDep == 0)
		return;
	else if (m_cDep == 1)
		((CSlob*) m_aDep[0])->OnInform(this, idChange, dwHint);
	else if (m_cDep == 2)
	{
		// Snapshot the dependants, since informing may cause changes.
		CSlob* rgSlobs[2];
		rgSlobs[0] = (CSlob*) m_aDep[0];
		rgSlobs[1] = (CSlob*) m_aDep[1];

		rgSlobs[0]->OnInform(this, idChange, dwHint);
		rgSlobs[1]->OnInform(this, idChange, dwHint);
	}
	else
	{
		CObList* pList = (CObList*) m_aDep[0];

		ASSERT(m_cDep == 3);
		ASSERT(pList->GetCount() > 2);

		POSITION pos = pList->GetHeadPosition();
		if (pList->GetCount() > MAX_SAFE_DEPENDANTS)
		{
			// Give up and do it the old less safe way.
			while (pos != NULL)
			{
				CSlob* pSlob = (CSlob*)((CObList*) m_aDep[0])->GetNext(pos);
				pSlob->OnInform(this, idChange, dwHint);
			}
		}
		else
		{
			// Snapshot the dependants, since informing may cause changes.
			int nSlobs = 0;
			CSlob* rgSlobs[MAX_SAFE_DEPENDANTS];

			while (pos != NULL)
				rgSlobs[nSlobs++] = (CSlob*)pList->GetNext(pos);

			for (int i = 0; i < nSlobs; i++)
				rgSlobs[i]->OnInform(this, idChange, dwHint);
		}
	}
}


// This should be overridden for slobs that are dependant on change
// notifcations from other slobs.  When the other slob changes, this
// function is called to indicate what slobs changed and the nature of
// the change.  By the time this is called, the change has already
// occured, so calling Get?Prop on the changed slob will yield the new
// value.
//
void CSlob::OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint)
{
	if (idChange == SN_DESTROY)
	{
		RemoveDependant(pChangedSlob);

		CObList* pContent = GetContentList();
		if (pContent != NULL)
		{
			POSITION pos = pContent->Find(pChangedSlob);
			if (pos != NULL)
				pContent->RemoveAt(pos);
		}
	}

#ifdef VERBOSE
	CString strThisName, strChangedName;
	GetSlobName(strThisName);
	pChangedSlob->GetSlobName(strChangedName);

	TRACE4("%s was informed that property %d of %s changed (hint=%ld)\n",
		(const char*)strThisName, idChange, (const char*)strChangedName,
		dwHint);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// Propery Modification Functions


BOOL CSlob::SetSlobProp(UINT idProp, CSlob* val)
{
	if (idProp == P_Container)
	{
		CSlob* pContainer = GetContainer();
		CSlob* pNewContainer = val;

		// "Delete" really means "move into the undo slob...
		if (pNewContainer == NULL)
			pNewContainer = &theUndoSlob;

		// Moving a multi-slob really means the slobs referenced by it...
		if (IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		{
			BOOL bOkay = TRUE;
			POSITION pos = GetHeadPosition();
			while (pos != NULL)
			{
				if (InEmergencyState())
				{
					bOkay = FALSE;
					break;
				}

				GetNext(pos)->MoveInto(pNewContainer);
			}

			// the items in m_objects are also on
			// our dependancy list so wee need to remove the dependencies
			// individually and then call RemoveAll

			CObList *pList = GetContentList();
			pos = pList->GetHeadPosition();
			while (pos != NULL)
			{
				CSlob *pDepSlob = (CSlob*) pList->GetNext(pos);
				pDepSlob->RemoveDependant(this);
			}

			pList->RemoveAll();

			return bOkay;
		}

		// Moving into the current container is a no-op...
		if (pNewContainer == pContainer)
			return TRUE;

		if (pContainer != NULL)
		{
			// Remove the slob from it's current container...
			pContainer->Remove(this);
		}

		// If we are moving into theUndoSlob, and it is not recording, then
		// just delete the slob.
		//
		if (pNewContainer == &theUndoSlob && !theUndoSlob.IsRecording())
		{
			m_pContainer = NULL;
			InformDependants(P_Container);
			delete this;
			return TRUE;			 // Must exit.
		}

		// Setup the new container so the symbol stuff will work...
		if (theUndoSlob.IsRecording() && pNewContainer != &theClipboardSlob)
			theUndoSlob.OnSetSlobProp(this, P_Container, pContainer);

		m_pContainer = pNewContainer;
		pNewContainer->Add(this);
	}
	else // non-container property
	{
		const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

		if (pprd == NULL)
		{
			// TRACE2("CSlob::SetSlobProp: Unhandled (%d, 0x%x)!\n", idProp, val);
			return FALSE;
		}

		ASSERT(pprd->pbOffset != NULL);
		BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

		if (pprd->nType != PRD::slob)
		{
			TRACE("Bogus SetSlobProp!\n");
			ASSERT(FALSE);
		}

		if (*(CSlob**)pbProp == (CSlob*)val)
			return TRUE;

		if (theUndoSlob.IsRecording())
			theUndoSlob.OnSetSlobProp(this, idProp, *(CSlob**)pbProp);

		*(CSlob**)pbProp = (CSlob*)val;
	}

	InformDependants(idProp);

	return TRUE;
}


BOOL CSlob::SetRectProp(UINT idProp, const CRect& val)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (pprd == NULL)
	{
		TRACE("CSlob::SetRectProp: Unhandled (%d, {%d,%d,%d,%d})!\n", idProp,
			val.left, val.top, val.right, val.bottom);
		return FALSE;
	}

	ASSERT(pprd->pbOffset != NULL);
	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	if (pprd->nType != PRD::rect)
	{
		TRACE("Bogus SetRectProp!\n");
		ASSERT(FALSE);
	}

	if (*(CRect*)pbProp == val)
		return TRUE;

	if (theUndoSlob.IsRecording())
		theUndoSlob.OnSetRectProp(this, idProp, *(CRect*)pbProp);

	*(CRect*)pbProp = val;

	InformDependants(idProp);

	return TRUE;
}


BOOL CSlob::SetPointProp(UINT idProp, const CPoint& val)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (pprd == NULL)
	{
		TRACE3("CSlob::SetPointProp: Unhandled (%d, {%d, %d})!\n", idProp, val.x, val.y);
		return FALSE;
	}

	ASSERT(pprd->pbOffset != NULL);
	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	if (pprd->nType != PRD::point)
	{
		TRACE("Bogus SetPointProp!\n");
		ASSERT(FALSE);
	}

	if (*(CPoint*)pbProp == val)
		return TRUE;

	if (theUndoSlob.IsRecording())
		theUndoSlob.OnSetPointProp(this, idProp, *(CPoint*)pbProp);

	*(CPoint*)pbProp = val;

	InformDependants(idProp);

	return TRUE;
}


BOOL CSlob::SetDoubleProp(UINT idProp, double val)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (pprd == NULL)
	{
		TRACE2("CSlob::SetDoubleProp: Unhandled (%d, %f)!\n", idProp, val);
		return FALSE;
	}

	BOOL bRet = TRUE;
	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	switch (pprd->nType)
	{
	case PRD::action:
		TRACE("Action properties cannot be set!\n");
		ASSERT(FALSE);

	case PRD::booln:
	case PRD::integer:
		bRet = SetIntProp(idProp, (int)val);
		break;

	case PRD::longint:
		bRet = SetLongProp(idProp, (long)val);
		break;

	case PRD::number:
		ASSERT(pprd->pbOffset != NULL);
		if (*(double*)pbProp == val)
			return TRUE;

		if (theUndoSlob.IsRecording())
			theUndoSlob.OnSetDoubleProp(this, idProp, *(double*)pbProp);

		*(double*)pbProp = val;
		break;

	case PRD::string:
		{
			char szBuf [32];
			sprintf(szBuf, "%f", val);
			bRet = SetStrProp(idProp, szBuf);
		}
		break;
	}

	if (bRet)
		InformDependants(idProp);

	return bRet;
}


BOOL CSlob::SetIntProp(UINT idProp, int val)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);
	CPropBag* pBag = NULL;
	int nType;

	if (pprd == NULL)
	{
		pBag = GetPropBag();
		if (pBag == NULL)
		{
			// TRACE2("CSlob::SetIntProp: Unhandled (%d, %d)!\n", idProp, val);
			return FALSE;
		}
		
		CProp* pProp = pBag->FindProp(idProp);
		nType = pProp != NULL ? pProp->m_nType : integer;
	}
	else
	{
		nType = pprd->nType;
	}

#ifdef VERBOSE
	CString strName;
	GetSlobName(strName);
	TRACE3("%s->SetIntProp(P_%s, %d)\n", (const char*)strName, pprd->szName, val);
#endif

	BOOL bRet = TRUE;

	switch (nType)
	{
	default:
		TRACE1("Illegal property type conversion (from %d to INT)\n", pprd->nType);
		ASSERT(FALSE);
		return FALSE;

	case PRD::action:
		TRACE("Action properties cannot be set!\n");
		ASSERT(FALSE);

	case PRD::booln:
	case PRD::integer:
#ifdef _WIN32
	case PRD::longint:
#endif
		if (pprd != NULL)
		{
			ASSERT(pprd->pbOffset != NULL);
			BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

			if (*(int*)pbProp == val)
				return TRUE;

			if (theUndoSlob.IsRecording())
				theUndoSlob.OnSetIntProp(this, idProp, *(int*)pbProp);

			*(int*)pbProp = val;
		}
		else
		{
			pBag->SetIntProp(this, idProp, val);
		}
		break;

	case PRD::number:
		bRet = SetDoubleProp(idProp, (double)val);
		break;

#ifndef _WIN32
	case PRD::longint:
		bRet = SetLongProp(idProp, (long)val);
		break;
#endif

	case PRD::string:
		{
			char szBuf [32];
			sprintf(szBuf, "%d", val);
			bRet = SetStrProp(idProp, szBuf);
		}
		break;
	}

	if (bRet)
		InformDependants(idProp);

	return bRet;
}


BOOL CSlob::SetLongProp(UINT idProp, long val)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (pprd == NULL)
	{
		TRACE2("CSlob::SetLongProp: Unhandled (%d, %ld)!\n", idProp, val);
		return FALSE;
	}

#ifdef VERBOSE
	CString strName;
	GetSlobName(strName);
	TRACE3("%s->SetIntProp(P_%s, %ld)\n", (const char*)strName, pprd->szName, val);
#endif

	BOOL bRet = TRUE;
	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	switch (pprd->nType)
	{
	case PRD::action:
		TRACE("Action properties cannot be set!\n");
		ASSERT(FALSE);

	case PRD::booln:
	case PRD::integer:
		bRet = SetIntProp(idProp, (int)val);
		break;

	case PRD::number:
		bRet = SetDoubleProp(idProp, (double)val);
		break;

	case PRD::longint:
		ASSERT(pprd->pbOffset != NULL);
		if (*(long*)pbProp == val)
			return TRUE;

		if (theUndoSlob.IsRecording())
			theUndoSlob.OnSetLongProp(this, idProp, *(long*)pbProp);

		*(long*)pbProp = val;
		break;

	case PRD::string:
		{
			char szBuf [32];
			sprintf(szBuf, "%d", val);
			bRet = SetStrProp(idProp, szBuf);
		}
		break;
	}

	if (bRet)
		InformDependants(idProp);

	return bRet;
}


BOOL CSlob::SetStrProp(UINT idProp, const CString& val)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);
	CPropBag* pBag = NULL;
	int nType;

	if (pprd == NULL)
	{
		pBag = GetPropBag();
		if (pBag == NULL)
		{
			TRACE2("CSlob::SetStrProp: Unhandled (%d, %d)!\n", idProp, val);
			return FALSE;
		}
		
		CProp* pProp = pBag->FindProp(idProp);
		nType = pProp != NULL ? pProp->m_nType : string;
	}
	else
	{
		nType = pprd->nType;
	}

#ifdef VERBOSE
	CString strName;
	GetSlobName(strName);
	TRACE3("%s->SetStrProp(P_%s, %s)\n", (const char*)strName, pprd->szName,
		(const char*)val);
#endif

	BOOL bRet = TRUE;

	switch (nType)
	{
	case PRD::action:
		TRACE("Action properties cannot be set!\n");
		ASSERT(FALSE);

	case PRD::booln:
	case PRD::integer:
#ifdef _WIN32
	case PRD::longint:
#endif
		bRet = SetIntProp(idProp, atoi(val));
		break;

	case PRD::number:
		bRet = SetDoubleProp(idProp, atof(val));
		break;

#ifndef _WIN32
	case PRD::longint:
		bRet = SetLongProp(idProp, atol(val));
		break;
#endif

	case PRD::string:
		
		if (pprd != NULL)
		{
			ASSERT(pprd->pbOffset != NULL);
			BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

			// P_ID and P_Value are reserved for CSymSlob's only!
			ASSERT(idProp != P_ID && idProp != P_Value);

			if (*(CString*)pbProp == val)
				return TRUE;

			if (theUndoSlob.IsRecording())
				theUndoSlob.OnSetStrProp(this, idProp, (CString*)pbProp);

			*(CString*)pbProp = val;
		}
		else
		{
			pBag->SetStrProp(this, idProp, (CString)val);
		}
		break;
	}

	if (bRet)
		InformDependants(idProp);

	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
// Property Retrieval Functions

GPT CSlob::GetSlobProp(UINT idProp, CSlob*& valReturn)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (pprd == NULL)
	{
		// TRACE1("CSlob::GetSlobProp: Unhandled (%d)!\n", idProp);
		return invalid;
	}

	ASSERT(pprd->pbOffset != NULL);
	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	if (pprd->nType != PRD::slob)
	{
		TRACE("Bogus GetSlobProp!\n");
		ASSERT(FALSE);
	}

	valReturn = *(CSlob**)pbProp;

	return valid;
}


GPT CSlob::GetRectProp(UINT idProp, CRect& valReturn)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (idProp == P_CurrentSlobRect)
	{
		IProjectWorkspaceWindow *pWorkspace = NULL;
		if (FAILED(theApp.FindInterface(IID_IProjectWorkspaceWindow, (void **)&pWorkspace)))
		{
			TRACE("IID_IProjectWorkspaceWindow not found\n");
			return invalid;
		}

		RECT rt;
		pWorkspace->GetCurrentSlobRect(&rt);
		valReturn = rt;
		return valid;
	}

	if (pprd == NULL)
	{
		TRACE1("CSlob::GetRectProp: Unhandled (%d)!\n", idProp);
		return invalid;
	}

	ASSERT(pprd->pbOffset != NULL);
	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	if (pprd->nType != PRD::rect)
	{
		TRACE("Bogus GetRectProp!\n");
		ASSERT(FALSE);
	}

	valReturn = *(CRect*)pbProp;

	return valid;
}


GPT CSlob::GetPointProp(UINT idProp, CPoint& valReturn)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (pprd == NULL)
	{
		TRACE1("CSlob::GetPointProp: Unhandled (%d)!\n", idProp);
		return invalid;
	}

	ASSERT(pprd->pbOffset != NULL);
	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	if (pprd->nType != PRD::point)
	{
		TRACE("Bogus GetPointProp!\n");
		ASSERT(FALSE);
	}

	valReturn = *(CPoint*)pbProp;

	return valid;
}


GPT CSlob::GetDoubleProp(UINT idProp, double& valReturn)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (pprd == NULL)
	{
		TRACE1("CSlob::GetDoubleProp: Unhandled (%d)!\n", idProp);
		return invalid;
	}

	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	switch (pprd->nType)
	{
	case PRD::action:
		TRACE("Action properties cannot be retrieved!\n");
		ASSERT(FALSE);

	case PRD::booln:
	case PRD::integer:
		ASSERT(pprd->pbOffset != NULL);
		valReturn = *(int*)pbProp;
		break;

	case PRD::longint:
		ASSERT(pprd->pbOffset != NULL);
		valReturn = *(long*)pbProp;
		break;

	case PRD::number:
		ASSERT(pprd->pbOffset != NULL);
		valReturn = *(double*)pbProp;
		break;

	case PRD::string:

		ASSERT(pprd->pbOffset != NULL);
		valReturn = atoi(*(CString*)pbProp);
		break;
	}

	return valid;
}


GPT CSlob::GetIntProp(UINT idProp, int& valReturn)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);
	CPropBag* pBag = NULL;

	if (pprd == NULL)
	{
		pBag = GetPropBag();
		if (pBag == NULL)
		{
			// TRACE1("CSlob::GetIntProp: Unhandled (%d)!\n", idProp);
			return invalid;
		}

		CProp* pProp = pBag->FindProp(idProp);
		if (pProp == NULL)
			return invalid;

		switch (pProp->m_nType)
		{
		default:
			TRACE("Illegal conversion\n");
			ASSERT(FALSE);
			return invalid;

		case integer:
		case booln:
#ifdef _WIN32
		case longint:
#endif
			valReturn = ((CIntProp*)pProp)->m_nVal;
			break;

		case string:
			valReturn = atoi(((CStringProp*)pProp)->m_strVal);
			break;
		}
	}
	else
	{
		BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

		switch (pprd->nType)
		{
		case PRD::action:
			TRACE("Action properties cannot be retrieved!\n");
			ASSERT(FALSE);

		case PRD::booln:
		case PRD::integer:
			ASSERT(pprd->pbOffset != NULL);
			valReturn = *(int*)pbProp;
			break;

		case PRD::longint:
			ASSERT(pprd->pbOffset != NULL);
			valReturn = (int) LOWORD(*(long*)pbProp);
			break;

		case PRD::number:
			ASSERT(pprd->pbOffset != NULL);
			valReturn = (int)*(double*)pbProp;
			break;

		case PRD::string:
			ASSERT(idProp != P_Value);
			ASSERT(pprd->pbOffset != NULL);
			valReturn = atoi(*(CString*)pbProp);
			break;
		}
	}

	return valid;
}


GPT CSlob::GetLongProp(UINT idProp, long& valReturn)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);

	if (pprd == NULL)
	{
		TRACE1("CSlob::GetLongProp: Unhandled (%d)!\n", idProp);
		return invalid;
	}

	BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

	switch (pprd->nType)
	{
	case PRD::action:
		TRACE("Action properties cannot be retrieved!\n");
		ASSERT(FALSE);

	case PRD::booln:
	case PRD::integer:
		ASSERT(pprd->pbOffset != NULL);
		valReturn = *(int*)pbProp;
		break;

	case PRD::longint:
		ASSERT(pprd->pbOffset != NULL);
		valReturn = *(long*)pbProp;
		break;

	case PRD::number:
		ASSERT(pprd->pbOffset != NULL);
		valReturn = (int)*(double*)pbProp;
		break;

	case PRD::string:
		ASSERT(idProp != P_Value);
		ASSERT(pprd->pbOffset != NULL);
		valReturn = atoi(*(CString*)pbProp);
		break;
	}

	return valid;
}


GPT CSlob::GetStrProp(UINT idProp, CString& valReturn)
{
	const PRD* pprd = PrdFindProp(GetPropMap(), idProp);
	CPropBag* pBag = NULL;

	if (pprd == NULL)
	{
		pBag = GetPropBag();
		if (pBag == NULL)
		{
			TRACE1("CSlob::GetStrProp: Unhandled (%d)!\n", idProp);
			return invalid;
		}

		CProp * pProp = pBag->FindProp(idProp);
		if (pProp == NULL)
			return invalid;

		switch (pProp->m_nType)
		{
			case integer:
			{
				char szBuf [16];	// a long int is +/- 2,147,483,648  
				valReturn = _itoa(((CBoolProp *)pProp)->m_bVal, szBuf, 10);
				break;
			}

			case booln:
			{
				char szBuf [16];	// a long int is +/- 2,147,483,648  
				valReturn = _itoa(((CIntProp *)pProp)->m_nVal, szBuf, 10);
				break;
			}

			case string:
				valReturn = ((CStringProp *)pProp)->m_strVal;
				break;

			default:
				TRACE("Illegal conversion\n");
				ASSERT(FALSE);
				return invalid;
		}
	}
	else
	{
		BYTE* pbProp = (BYTE*)this + (DWORD)pprd->pbOffset;

		switch (pprd->nType)
		{
		case PRD::action:
			TRACE("Action properties cannot be retrieved!\n");
			ASSERT(FALSE);

		case PRD::booln:
		case PRD::integer:
			{
				int num;
				char szBuf [32];
				GPT gpt = GetIntProp(idProp, num);
				if (gpt != valid)
					return gpt;

				sprintf(szBuf, "%d", num);
				valReturn = szBuf;
			}
			break;

		case PRD::longint:
			{
				long num;
				char szBuf [32];
				GPT gpt = GetLongProp(idProp, num);
				if (gpt != valid)
					return gpt;

				sprintf(szBuf, "%8ld", num);
				valReturn = szBuf;
			}
			break;

		case PRD::number:
			{
				double num;
				char szBuf [32];
				GPT gpt = GetDoubleProp(idProp, num);
				if (gpt != valid)
					return gpt;

				sprintf(szBuf, "%f", num);
				valReturn = szBuf;
			}
			break;

		case PRD::rect:
			{
				CRect rc;
				char szBuf [32];
				GPT gpt = GetRectProp(idProp, rc);
				if(gpt != valid)
					return gpt;

				sprintf(szBuf, "%d,%d,%d,%d", rc.left, rc.top,
					rc.right, rc.bottom);
				valReturn = szBuf;
			}
			break;

		case PRD::point:
			{
				CPoint pt;
				char szBuf [32];
				GPT gpt =GetPointProp(idProp, pt);
				if (gpt != valid)
					return gpt;

				sprintf(szBuf, "%d,%d", pt.x, pt.y);
				valReturn = szBuf;
			}
			break;

		case PRD::string:
			ASSERT(idProp != P_Value);
			ASSERT(pprd->pbOffset != NULL);
			valReturn = *(CString*)pbProp;
			break;
		}
	}

	return valid;
}

// Add any files associated with this slob. By default, this is nothing
// but files associated with contained slobs.
int CSlob::GetAssociatedFiles(CStringArray & saFiles)
{
	int nFiles = 0;
	if (GetContentList() != NULL)
	{
		CObList *plist = GetContentList();
		POSITION pos = plist->GetHeadPosition();
		while (pos != NULL)
		{
			nFiles += ((CSlob *) plist->GetNext(pos))->GetAssociatedFiles(saFiles);
		}
	}
	return nFiles;
}

#ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Debugging Functions

void CSlob::AssertValid() const
{
	CObject::AssertValid();
	
	ASSERT(m_pContainer == NULL || 
		m_pContainer->IsKindOf(RUNTIME_CLASS(CSlob)));
}


// Retrieve the name of a slob.  Default is to just format the address
// as a string.  Override for classes that actually have names, or more
// interesting things to use.
//
void CSlob::GetSlobName(CString& strName) const
{
	char szBuf [32];
	sprintf(szBuf, "0x%08lx", this);
	strName = szBuf;
}

#endif

/////////////////////////////////////////////////////////////////////////////


CClipboardSlob theClipboardSlob;

IMPLEMENT_DYNAMIC(CClipboardSlob, CSlob)

CClipboardSlob::CClipboardSlob() : m_objects()
{
}

CClipboardSlob::~CClipboardSlob()
{
	ClearContents();
}

void CClipboardSlob::Copy(CSlob* pCopySlob)
{
	if (pCopySlob->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
	{
		POSITION pos = pCopySlob->GetHeadPosition();
		while (pos != NULL)
		{
			Copy(pCopySlob->GetNext(pos));

			if (InEmergencyState())
				break;
		}
		return;
	}

	if (pCopySlob->GetContainer()->CanRemove(pCopySlob))
	{
		CSlob* pClone = pCopySlob->Clone();
		if (pClone != NULL)
			pClone->MoveInto(this);
	}
}

BOOL CClipboardSlob::CanAct(ACTION_TYPE action)
{
	// FUTURE: needs to find out what slob will accept the clipboard
	// if a paste were to occur and ask it if it can deal with the stuff
	// in there...
	if (action == act_paste)
		return !m_objects.IsEmpty();

	return CSlob::CanAct(action);
}

void CClipboardSlob::Paste(CSlob* pDestSlob, CMultiSlob* pSelection, BOOL bClearSelection)
{
	// Give the destination slob a final chance to refuse a paste.
	if (!pDestSlob->PreAct(act_paste))
		return;

	CWaitCursor waitCursor;

	CMultiSlob clones;
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CSlob* pSlob = GetNext(pos);

		if (pSlob->CanAct(act_paste))
		{
			CSlob* pClone = pSlob->Clone();
			if (pClone != NULL)
				clones.Add(pClone);
		}
	}

	pDestSlob->PrepareAddList(&clones, TRUE);

	if ((pSelection != NULL) && bClearSelection)
		pSelection->Clear();

	pos = clones.GetHeadPosition();
	while (pos != NULL)
	{
		CSlob* pSlob = clones.GetNext(pos);

		if (pDestSlob->CanAdd(pSlob))
		{
			pSlob->MoveInto(pDestSlob);

			if (InEmergencyState())
				break;

			if (pSelection != NULL)
				pSelection->Add(pSlob);
		}
		else
		{
			// couldn't add the slob, so delete it to prevent leaks
			pSlob->MoveInto(NULL);
		}
	}
}

void CClipboardSlob::ClearContents()
{
	// go through the list removing and deleting slobs
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
		delete GetNext(pos);
	m_objects.RemoveAll();
}


///////////////////////////////////////////////////////////////////////////////
// CProjSlob

//IMPLEMENT_SERIAL(CProjSlob, CSlob, 1)

IMPLEMENT_DYNAMIC(CProjSlob, CSlob)

CProjSlob::CProjSlob()
{
}

CProjSlob::~CProjSlob()
{
}

void CProjSlob::GetGlyphTipText(CString &strTip)
{
	if (GetStrProp(P_Title, strTip) != valid)
		strTip = "";
}

int CProjSlob::CompareSlob(CSlob *pCompareSlob)
{
	// if pCompareSlob < this, return -1.
	// if pCompareSlob == this, return 0.
	// if pCompareSlob > this, return 1.

	// default behavior is to compare title properties.
	// override this behavior if different sorting mechanism is desired.
	CString strThis, strCompare;

	if ((GetStrProp(P_Title, strThis) != valid) ||
		(pCompareSlob->GetStrProp(P_Title, strCompare) != valid))
	{
		// Since one or other slob has no title property, we have no
		// basis for comparison, so just return that they're equal.
		ASSERT(FALSE);	// Really pretty bogus.
		return(0);
	}

	return(strThis.CompareNoCase(strCompare));
}

// Normally we will look in the Property Map to find the Verbs and
// commands supported by a slob.  The arrays contain the Cmd IDS
// of the verbs and cmds to be supported.
void CProjSlob::GetCmdIDs(CWordArray& aVerbs, CWordArray& aCmds)
{
#ifdef _DEBUG
	aVerbs.AssertValid();
	aCmds.AssertValid();
#endif

	const PRD* propMap = GetPropMap();
	for ( ; propMap != NULL; propMap = (PRD*)propMap[0].pbOffset)
	{
		for (int iProp = 1; propMap[iProp].nProp != P_END_MAP; iProp += 1)
		{
			if (propMap[iProp].nType == verb)
				aVerbs.Add((WORD)propMap[iProp].pbOffset);
			else if (propMap[iProp].nType == command)
				aCmds.Add((WORD)propMap[iProp].pbOffset);
		}
	}
}

// Standard Display routine.  Used when various controls try to display
// the slob, for example when the project window displays a slob.
// If FALSE is returned the control will draw its own glyph for the slob.
BOOL CProjSlob::DrawGlyph(CDC* pDC, LPCRECT lpRect)
{
	return FALSE;
}

