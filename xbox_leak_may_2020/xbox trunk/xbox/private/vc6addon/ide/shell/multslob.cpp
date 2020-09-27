#include "stdafx.h"

#include "util.h"
#include "slob.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// IMPLEMENT_DYNCREATE needs real new.
#undef new
#endif

IMPLEMENT_DYNCREATE(CMultiSlob, CSlob)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMultiSlob::CMultiSlob()
{
    m_pSlobWnd = NULL;
    m_bSolo = FALSE;
    m_bAllowEmpty = TRUE;
	m_bAllowMultipleContainers = FALSE;
}


CMultiSlob::~CMultiSlob()
{
    POSITION pos = GetHeadPosition();
    while (pos != NULL)
		GetNext(pos)->RemoveDependant(this);
}


CObList* CMultiSlob::GetContentList()
{
	// This is not really a 'content' list since it contains
	// references to slobs that are probably really contained
	// some other slob.  This does make a lot of things very
	// convenient though...
	
    return &m_objects;
}


CSlobWnd* CMultiSlob::GetSlobWnd() const
{
    if (m_pSlobWnd != NULL)
        return m_pSlobWnd;

	// NOTE: This makes the assumption that all slobs in a selection
	// are displayed in the same CSlobWnd.
	
    if (!m_objects.IsEmpty())
        return GetDominant()->GetSlobWnd();

    return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Drag and Drop Support

// Return an index into the cursor table for dragging this slob.
//
int CMultiSlob::GetCursorIndex()
{
    return GetDominant()->GetCursorIndex();
}

// Return a cursor for dragging this slob.
//
HCURSOR CMultiSlob::GetCursor(int ddcCursorStyle)
{
	return GetDominant()->GetCursor(ddcCursorStyle);
}


/////////////////////////////////////////////////////////////////////////////
// Selection Management


BOOL CMultiSlob::IsSelected(CSlob* pSlob) const
{
	return m_objects.Find(pSlob) != NULL;
}

BOOL CMultiSlob::IsOnlySelected(CSlob* pSlob) const
{
	return m_objects.GetCount() == 1 && m_objects.GetHead() == pSlob;
}

BOOL CMultiSlob::IsDominantSelected(CSlob* pSlob) const
{
	if (m_objects.IsEmpty())
		return FALSE;
	
	return m_objects.GetHead() == pSlob;
}

CSlob* CMultiSlob::GetDominant() const
{
	if (m_objects.IsEmpty())
		return NULL;
	
	return (CSlob*)m_objects.GetHead();
}


// This function selects a single slob, deselecting any previously
// selected slobs.  If bSolo is set, the slob will be deslected if
// any other slobs are 'added' to the selection (it's a loner and
// doesn't want to be part of a group selection).
//
void CMultiSlob::Single(CSlob* pSlob, BOOL bSolo /*=FALSE*/ )
{
    ASSERT(pSlob != NULL);

    if (IsOnlySelected(pSlob))
        return;

    Clear();
	Add(pSlob);
	m_bSolo = bSolo;
}


// This function adds a slob to the current selection.  The new
// slob will be the dominant member of the selection.
//
// NOTE:    This function always adds it's argument to the BEGINNING of
//          the multislob!  This makes cloning in forward order work
//          properly.  That's also how we know it's dominant...
//
void CMultiSlob::Add(CSlob* pSlob)
{
    if (m_bSolo)
    {
		// If the current selection is an object which insists on having
		// a solo selection (cannot be part of a multiple selection) then
		// we must deselect it.
		
		Clear();
		m_bSolo = FALSE;
    }

	if (m_pSlobWnd != NULL)
		m_pSlobWnd->HideSprites();

    if (!m_bAllowMultipleContainers && !m_objects.IsEmpty())
    {
		CSlob* pPrevDom = (CSlob*) m_objects.GetHead();

        // Only allow mulitple selections within a single container!
        if (pSlob->GetContainer() != pPrevDom->GetContainer())
			Clear();
    }
	
	// Check to see if it is already selected.
	POSITION pos = m_objects.Find(pSlob);
	if (pos != NULL)
		m_objects.RemoveAt(pos);

    m_objects.AddHead(pSlob);
	
	if (pos == NULL)
		pSlob->AddDependant(this);
	
    if (m_pSlobWnd != NULL)
	{
		m_pSlobWnd->ShowSprites();
        m_pSlobWnd->OnSelChange();
	}
	
    InformDependants(SN_ALL);
}


// This function removes a slob from the selection.
//
void CMultiSlob::Remove(CSlob* pSlob)
{
    POSITION pos = m_objects.Find(pSlob);
    ASSERT(pos != NULL);
	
    if (theUndoSlob.IsRecording())
        theUndoSlob.OnMultiRemove(this, pSlob);
	
	if (m_pSlobWnd != NULL)
		m_pSlobWnd->HideSprites();
	
    m_objects.RemoveAt(pos);
    pSlob->RemoveDependant(this);

	// Now we have to fix the state of the remaining slobs in the
    // selection.
	
    int count = m_objects.GetCount();
    if (count == 0 && !m_bAllowEmpty)
	{
		CSlob* pContainer = pSlob->GetContainer();
		if (pContainer != NULL)
		{
			ASSERT(pContainer != NULL);
			Single(pContainer);
			return;
		}
    }
	
    if (m_pSlobWnd != NULL)
	{
		m_pSlobWnd->ShowSprites();
        m_pSlobWnd->OnSelChange();
	}

    InformDependants(SN_ALL);
}


// Remove all items from the current selection, leaving it empty.
//
void CMultiSlob::Clear()
{
	if (m_pSlobWnd != NULL)
		m_pSlobWnd->HideSprites();
	
    while (!m_objects.IsEmpty())
    {
        CSlob* pSlob = (CSlob*)m_objects.RemoveHead();
        pSlob->RemoveDependant(this);
    }
	
	if (m_pSlobWnd != NULL)
		m_pSlobWnd->ShowSprites();
	
    InformDependants(SN_ALL);
}


/////////////////////////////////////////////////////////////////////////////
//

// CMultiSlob::Delete
//
// This handles the delete and cut commands for all slob-based editors.
// The bClip parameter set to TRUE indicates this is a Cut and things
// should be placed in the clipboard.  If any slobs refuse to go away, 
// they are left selected. Return value is TRUE iff all of the selection 
// is deleted.  If ppNextSlob != NULL, a pointer to the first item after 
// the last selected item is stuck there (or the first item if the last 
// was deleted, or the container if all are gone).  This may be used to 
// set the selection at the end of the delete.
//
BOOL CMultiSlob::Delete(BOOL bClip, CSlob** ppNextSlob)
{
    CWaitCursor waitCursor;

    CSlob* pContainer = ((CSlob*)m_objects.GetHead())->GetContainer();
    CObList* conList = pContainer->GetContentList();
    ASSERT(conList != NULL);

    theUndoSlob.BeginUndo(bClip ? IDS_UNDO_CUT : IDS_UNDO_DELETE );
    
    if (bClip)
        theClipboardSlob.ClearContents();

    CObList deathRow;
    deathRow.AddHead(&m_objects);

    // Clear out the selection before deleting things to prevent an
    // illegal state...
    Clear();

    // Then delete the ones that were selected...
    POSITION nextPos = NULL;
    BOOL bVeto = FALSE;
    POSITION pos = deathRow.GetHeadPosition();
    while (pos != NULL)
    {
        if (InEmergencyState())
        {
            bVeto = TRUE;
            break;
        }

        CSlob* pSlob = (CSlob*)deathRow.GetNext(pos);
        if (pSlob->CanAct(bClip ? act_cut : act_delete))
        {
            if (ppNextSlob != NULL)
            {
                // don't need to do this if we aren't 
                // interested in the selection at the end
                POSITION slobPos = conList->Find(pSlob);
                if (slobPos != NULL)
                {
                    nextPos = slobPos;
                    conList->GetNext(nextPos);
                }
            }
            
            if (bClip)
            {
                CSlob* pClone = pSlob->Clone();
                if (pClone != NULL)
                    pClone->MoveInto(&theClipboardSlob);
            }
            
            pSlob->MoveInto(NULL); // Move the slob into oblivion
            pSlob = NULL;
        }
        else
        {
            // If one chooses not to die, reselect it...
            if (bVeto)
            {
                Add(pSlob);
            }
            else
            {
                Single(pSlob);
                bVeto = TRUE;
            }
        }
    }
    
    // Let the caller know what slob they might want to select after
    // the delete takes place...
    if (ppNextSlob != NULL)
    {
        if (conList->IsEmpty())
            *ppNextSlob = pContainer;
        else if (nextPos == NULL)
            *ppNextSlob = (CSlob*)conList->GetHead();
        else
            *ppNextSlob = (CSlob*)conList->GetAt(nextPos);
    }

    theUndoSlob.EndUndo();
    
    return !bVeto;
}


// If any slob cannot added to, none can!
// Caution: use this with care (you may want to ignore CanAdd() for multislobs)
BOOL CMultiSlob::CanAdd(CSlob *pAddSlob)
{
    POSITION pos = GetHeadPosition();
	if (!pos)
		return FALSE;	// must be something to consider adding

    while (pos != NULL)
    {
        CSlob* pSlob = GetNext(pos);
        if (!pSlob->CanAdd(pAddSlob))
            return FALSE;
    }
    
    return TRUE;
}

BOOL CMultiSlob::CanAct(ACTION_TYPE action)
{
	if (action != act_delete && action != act_cut &&
		action != act_copy && action != act_paste)
		return CSlob::CanAct(action);

    POSITION pos = GetHeadPosition();
	if (!pos)
		return FALSE;

	BOOL bActIfAny = (action == act_paste);

    while (pos != NULL)
    {
        CSlob* pSlob = GetNext(pos);
        if ((bActIfAny && pSlob->CanAct(action)) ||
			(!bActIfAny && !pSlob->CanAct(action)))
            return bActIfAny;
    }
    
    return !bActIfAny;
}

// Give slobs one last chance to refuse the action.
BOOL CMultiSlob::PreAct(ACTION_TYPE action)
{
    POSITION pos = GetHeadPosition();
    while (pos != NULL)
    {
        CSlob* pSlob = GetNext(pos);
        if (!pSlob->PreAct(action))
            return FALSE;
    }
    
    return TRUE;
}

void CMultiSlob::EnsureValidSelection()
{
    // Default is a no-op...
	if (m_pSlobWnd != NULL)
	{
		m_pSlobWnd->EnsureValidSelection();
	}
}

void CMultiSlob::UndoAction(CSlobSequence* pSeq, UINT idAction)
{
    switch (idAction)
    {
    default:
        CSlob::UndoAction(pSeq, idAction);
        break;

    case A_PostSel:
        {
            CObList items;
            
            int cb;
            pSeq->RetrieveInt(cb);
            
            int nPtrs = cb / sizeof (CSlob*);
            while (nPtrs-- > 0)
            {
                CSlob* pSlob;
                pSeq->RetrievePtr(pSlob);
                items.AddHead(pSlob);
            }
            
            theUndoSlob.InsertSel(A_PreSel, this, &items);
        }
        break;
        
    case A_PreSel:
        {
            theUndoSlob.Pause();
            
            Clear();

            int cb;
            pSeq->RetrieveInt(cb);
            
            int nPtrs = cb / sizeof (CSlob*);
            while (nPtrs-- > 0)
            {
                CSlob* pSlob;
                pSeq->RetrievePtr(pSlob);
                Add(pSlob);
            }

            EnsureValidSelection();

            theUndoSlob.Resume();
            
            theUndoSlob.OnPostSel(this);
        }
        break;
    }
}


void CMultiSlob::OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint)
{
    if (idChange == SN_DESTROY)
    {
		// Remove slobs that are destroyed from this list
		
        POSITION pos = m_objects.Find(pChangedSlob);
        if (pos != NULL)
            m_objects.RemoveAt(pos);
    }
    else
    {
		// All other changes are propegated to dependants of this
		
        InformDependants(idChange, dwHint);
    }

    CSlob::OnInform(pChangedSlob, idChange, dwHint);
}


/////////////////////////////////////////////////////////////////////////////
// Propery Modification Functions

BOOL CMultiSlob::SetDoubleProp(UINT idProp, double val)
{
    BOOL bValid = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while( bValid && (pos != NULL) )
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);

		double inv;
		GPT gpt = pSlob->GetDoubleProp(idProp, inv);
		if( (gpt == avoid) || (gpt == invisible) )
			continue;

        if (!pSlob->SetDoubleProp(idProp, val))
            bValid = FALSE;		// abort if we are having trouble setting this property
    }

    return bValid;
}


BOOL CMultiSlob::SetIntProp(UINT idProp, int val)
{
    BOOL bValid = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while( bValid && (pos != NULL) )
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);

		int inv;
		GPT gpt = pSlob->GetIntProp(idProp, inv);
		if( (gpt == avoid) || (gpt == invisible) )
			continue;

        if (!pSlob->SetIntProp(idProp, val))
            bValid = FALSE;
    }

    return bValid;
}


BOOL CMultiSlob::SetLongProp(UINT idProp, long val)
{
    BOOL bValid = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while( bValid && (pos != NULL) )
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);

		long inv;
		GPT gpt = pSlob->GetLongProp(idProp, inv);
		if( (gpt == avoid) || (gpt == invisible) )
			continue;

        if (!pSlob->SetLongProp(idProp, val))
            bValid = FALSE;
    }

    return bValid;
}


BOOL CMultiSlob::SetStrProp(UINT idProp, const CString& val)
{
    BOOL bValid = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while( bValid && (pos != NULL) )
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);

		CString inv;
		GPT gpt = pSlob->GetStrProp(idProp, inv);
		if( (gpt == avoid) || (gpt == invisible) )
			continue;

        if (!pSlob->SetStrProp(idProp, val))
            bValid = FALSE;
    }

    return bValid;
}


BOOL CMultiSlob::SetSlobProp(UINT idProp, CSlob* val)
{
    BOOL bValid = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while( bValid && (pos != NULL) )
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);

		CSlob* inv;
		GPT gpt = pSlob->GetSlobProp(idProp, inv);
		if( (gpt == avoid) || (gpt == invisible) )
			continue;

        if (!pSlob->SetSlobProp(idProp, val))
            bValid = FALSE;
    }

    return bValid;
}


BOOL CMultiSlob::SetRectProp(UINT idProp, const CRect& val)
{
    BOOL bValid = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while( bValid && (pos != NULL) )
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);

		CRect inv;
		GPT gpt = pSlob->GetRectProp(idProp, inv);
		if( (gpt == avoid) || (gpt == invisible) )
			continue;

        if (!pSlob->SetRectProp(idProp, val))
            bValid = FALSE;
    }

    return bValid;
}


BOOL CMultiSlob::SetPointProp(UINT idProp, const CPoint& val)
{
    BOOL bValid = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while( bValid && (pos != NULL) )
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);

		CPoint inv;
		GPT gpt = pSlob->GetPointProp(idProp, inv);
		if( (gpt == avoid) || (gpt == invisible) )
			continue;

        if (!pSlob->SetPointProp(idProp, val))
            bValid = FALSE;
    }

    return bValid;
}


/////////////////////////////////////////////////////////////////////////////
// Property Retrieval Functions

// It is quite likely in a multiple selection that the GPT return codes
// will not match up.  We should take the worst case, and return that.
// The worst case is 'avoid' -- some control insists on outright hiding
// the property.  Next is 'invalid', which renders the property invalid
// for all.  Next is 'ambiguous', which usually means different slobs
// returned valid, but with different values.  Best is 'valid', which we only
// return if all contained slobs have the same valid value.  Finally,
// there is 'invisible', which hides the property like 'avoid' does, but
// if some other item in the selection wants it shown, then that's fine.

GPT CMultiSlob::GetDoubleProp(UINT idProp, double& valReturn)
{
    GPT gptRet = invisible;
	double val;
    BOOL bFirstTime = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);
        GPT gpt = pSlob->GetDoubleProp(idProp, valReturn);
        gptRet = min(gpt, gptRet);

        if( gpt == invisible )
			continue;
        if (bFirstTime)
        {
            val = valReturn;
            bFirstTime = FALSE;
        }
        else if (val != valReturn)
        {
            gptRet = min(gptRet, ambiguous);
        }
    }

    return gptRet;
}


GPT CMultiSlob::GetIntProp(UINT idProp, int& valReturn)
{
    GPT gptRet = invisible;
    int val;
    BOOL bFirstTime = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);
        GPT gpt = pSlob->GetIntProp(idProp, valReturn);
        gptRet = min(gpt, gptRet);

        if( gpt == invisible )
			continue;
        if (bFirstTime)
        {
            val = valReturn;
            bFirstTime = FALSE;
        }
        else if (val != valReturn)
        {
            gptRet = min(gptRet, ambiguous);
        }
    }

    return gptRet;
}


GPT CMultiSlob::GetLongProp(UINT idProp, long& valReturn)
{
    GPT gptRet = invisible;
    long val;
    BOOL bFirstTime = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);
        GPT gpt = pSlob->GetLongProp(idProp, valReturn);
        gptRet = min(gpt, gptRet);

        if( gpt == invisible )
			continue;
        if (bFirstTime)
        {
            val = valReturn;
            bFirstTime = FALSE;
        }
        else if (val != valReturn)
        {
            gptRet = min(gptRet, ambiguous);
        }
    }

    return gptRet;
}


GPT CMultiSlob::GetStrProp(UINT idProp, CString& valReturn)
{
    GPT gptRet = invisible;
    CString val;
    BOOL bFirstTime = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);
        GPT gpt = pSlob->GetStrProp(idProp, valReturn);
        gptRet = min(gpt, gptRet);

        if( gpt == invisible )
			continue;
        if (bFirstTime)
        {
            val = valReturn;
            bFirstTime = FALSE;
        }
        else if (val != valReturn)
        {
            gptRet = min(gptRet, ambiguous);
        }
    }

    return gptRet;
}


GPT CMultiSlob::GetSlobProp(UINT idProp, CSlob*& valReturn)
{
    GPT gptRet = invisible;
    CSlob* val;
    BOOL bFirstTime = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);
        GPT gpt = pSlob->GetSlobProp(idProp, valReturn);
        gptRet = min(gpt, gptRet);

        if( gpt == invisible )
			continue;
        if (bFirstTime)
        {
            val = valReturn;
            bFirstTime = FALSE;
        }
        else if (val != valReturn)
        {
            gptRet = min(gptRet, ambiguous);
        }
    }

    return gptRet;
}


GPT CMultiSlob::GetRectProp(UINT idProp, CRect& valReturn)
{
    GPT gptRet = invisible;
    CRect val;
    BOOL bFirstTime = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);
        GPT gpt = pSlob->GetRectProp(idProp, valReturn);
        gptRet = min(gpt, gptRet);

        if( gpt == invisible )
			continue;
        if (bFirstTime)
        {
            val = valReturn;
            bFirstTime = FALSE;
        }
        else if (val != valReturn)
        {
            gptRet = min(gptRet, ambiguous);
        }
    }

    return gptRet;
}


GPT CMultiSlob::GetPointProp(UINT idProp, CPoint& valReturn)
{
    GPT gptRet = invisible;
    CPoint val;
    BOOL bFirstTime = TRUE;
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);
        GPT gpt = pSlob->GetPointProp(idProp, valReturn);
        gptRet = min(gpt, gptRet);

        if( gpt == invisible )
			continue;
        if (bFirstTime)
        {
            val = valReturn;
            bFirstTime = FALSE;
        }
        else if (val != valReturn)
        {
            gptRet = min(gptRet, ambiguous);
        }
    }

    return gptRet;
}

BOOL CMultiSlob::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	CSheetWnd* pPropSheet = GetPropertyBrowser();
	InhibitExtraPropertyPages();
	StartNewPropertyPageSet();

    BOOL bFirstSlob = TRUE;
	BOOL bRet = TRUE;
	POSITION pos = m_objects.GetHeadPosition();
    while( pos != NULL )
    {
        CSlob* pSlob = (CSlob*)m_objects.GetNext(pos);

		// prepare to merge this slob's pages with the previous ones
		if( !bFirstSlob )
			StartNewPropertyPageSet();

		// add the pages for this sheet
		// REVIEW(davidga): what if SetupPropertyPages returns FALSE?
		pSlob->SetupPropertyPages(pNewSel, FALSE);

		// merge the pages
		if( !bFirstSlob )
			MergePropertyPageSets();
		else
			bFirstSlob = FALSE;

		// I can't do the following, because I don't have access to protected
		// members of CSheetWnd.
		// So instead, this is done in CSheetWnd::AddPropertyPage.
//		POSITION posPage = pPropSheet->m_listNewPages.GetHeadPosition();
//		while( posPage != NULL )
//		{
//			C3dPropertyPage* pPage = (C3dPropertyPage*)pPropSheet->
//				m_listNewPages.GetNext(posPage);
//			if( pPage->m_pSlob == pSlob )
//				pPage->m_pSlob = this;
//		}
    }

	if( bSetCaption && (m_objects.GetCount() > 0) )
	{
		// REVIEW(davidga): Is there anything better I should do for a caption?
		CString strCaption(MAKEINTRESOURCE(IDS_MULTISEL));
		SetPropertyCaption(strCaption);
	}

	InhibitExtraPropertyPages(FALSE);	// release the inhibition

	return TRUE;
}
