#include "stdafx.h"
#include "slob.h"
#include "util.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CUndoSlob AFX_DATA theUndoSlob;


/////////////////////////////////////////////////////////////////////////////
//
// A CSlobSequence is a packed array of slob property changes or custom
// actions.  Each record contains a property or action id, a pointer to
// a slob, a property type, and a value (depending on the type).
//
// These sequences are used to store undo/redo information in theUndoSlob.
// Each undo/redo-able thing is contained in one CSlobSequence.
//


CSlobSequence::CSlobSequence() : CByteArray(), m_strDescription()
{
	SetSize(0, 100); // increase growth rate
	m_nCursor = 0;
}


CSlobSequence::~CSlobSequence()
{
	Cleanup();
}


// Pull an array of bytes out of the sequence.
//
void CSlobSequence::Retrieve(BYTE* rgb, int cb)
{
	for (int ib = 0; ib < cb; ib += 1)
		*rgb++ = GetAt(m_nCursor++);
}


// Pull a string out the sequence.
//
void CSlobSequence::RetrieveStr(CString& str)
{
	int nStrLen;
	RetrieveInt(nStrLen);
	if (nStrLen == 0)
	{
		str.Empty();
	}
	else
	{
		BYTE* pb = (BYTE*)str.GetBufferSetLength(nStrLen);
		for (int nByte = 0; nByte < nStrLen; nByte += 1)
			*pb++ = GetAt(m_nCursor++);
		str.ReleaseBuffer(nStrLen);
	}
}


// Traverse the sequence and remove any slobs that are contained within.
//
void CSlobSequence::Cleanup()
{
	m_nCursor = 0;
	while (m_nCursor < GetSize())
	{
		CUndoRecord rec;
		RetrieveRecord(rec);

		switch (rec.m_op)
		{
		default:
			TRACE1("Illegal undo opcode (%d)\n", rec.m_op);
			ASSERT(FALSE);

		case CUndoSlob::opAction:
			{
				int cbUndoRecord;
				RetrieveInt(cbUndoRecord);
				int ib = m_nCursor;
				rec.m_pSlob->DeleteUndoAction(this, rec.m_nPropID);
				m_nCursor = ib + cbUndoRecord;
			}
			break;

		case CUndoSlob::opIntProp:
		case CUndoSlob::opBoolProp:
			{
				int val;
				RetrieveInt(val);
			}
			break;

		case CUndoSlob::opLongProp:
			{
				long val;
				RetrieveLong(val);
			}
			break;

		case CUndoSlob::opDoubleProp:
			{
				double num;
				RetrieveNum(num);
			}
			break;

		case CUndoSlob::opStrProp:
			{
				CString str;
				RetrieveStr(str);
			}
			break;

		case CUndoSlob::opSlobProp:
			{
				CSlob* pSlobVal;
				RetrievePtr(pSlobVal);
			}
			break;

		case CUndoSlob::opRectProp:
			{
				CRect rcVal;
				RetrieveRect(rcVal);
			}
			break;

		case CUndoSlob::opPointProp:
			{
				CPoint ptVal;
				RetrievePoint(ptVal);
			}
			break;
		}
	}
}


// Start looking right after the begin op for ops we really need to keep.
// If none are found, the entire record is discarded below.  (For now, we
// only throw away records that are empty or consist only of selection
// change ops.)
//
BOOL CSlobSequence::IsUseful(CSlob*& pLastSlob, int& nLastPropID)
{
	CUndoRecord rec;

	m_nCursor = 0;
	while (m_nCursor < GetSize())
	{
		RetrieveRecord(rec);
		if (rec.m_op != CUndoSlob::opAction)
			break;

		int cbActionRecord;
		RetrieveInt(cbActionRecord);

		if (rec.m_nPropID != A_PreSel && rec.m_nPropID != A_PostSel)
		{
			// Back cursor up to the opcode...
			m_nCursor -= sizeof (int) + sizeof (CUndoRecord);
			break;
		}

		m_nCursor += cbActionRecord;
	}

	if (m_nCursor == GetSize())
		return FALSE; // sequnce consists only of selection changes


	// Now check if we should throw this away because it's just
	// modifying the same string or rectangle property as the last
	// undoable operation...  This is an incredible hack to implement
	// a "poor man's" Multiple-Consecutive-Changes-to-a-Property-as-
	// One-Operation feature.

	if (rec.m_op == CUndoSlob::opStrProp || rec.m_op == CUndoSlob::opRectProp)
	{
		if (rec.m_nPropID == (WORD) nLastPropID && rec.m_pSlob == pLastSlob &&
			rec.m_nPropID != P_ID && rec.m_nPropID != P_Value)
		{
			return FALSE; // not useful
		}

		nLastPropID = rec.m_nPropID;
		pLastSlob = rec.m_pSlob;
	}

	m_nCursor = 0;
	return TRUE;
}


// Perform the property changes and actions listed in the sequence.
//
void CSlobSequence::Apply()
{
	m_nCursor = 0;
	while (m_nCursor < GetSize())
	{
		CUndoRecord rec;
		RetrieveRecord(rec);

		switch (rec.m_op)
		{
		default:
			TRACE1("Illegal undo opcode (%d)\n", rec.m_op);
			ASSERT(FALSE);

		case CUndoSlob::opAction:
			rec.m_pSlob->UndoAction(this, rec.m_nPropID);
			break;

		case CUndoSlob::opIntProp:
		case CUndoSlob::opBoolProp:
#ifdef _WIN32
		case CUndoSlob::opLongProp:
#endif
			{
				int val;
				RetrieveInt(val);
				if (rec.m_pBag != NULL)
				{
					rec.m_pBag->SetIntProp(rec.m_pSlob, rec.m_nPropID, val);
					rec.m_pSlob->InformDependants(rec.m_nPropID);
				}
				else
				{
					rec.m_pSlob->SetIntProp(rec.m_nPropID, val);
				}
			}
			break;

#ifndef _WIN32
		case CUndoSlob::opLongProp:
			{
				long val;
				RetrieveLong(val);
				rec.m_pSlob->SetLongProp(rec.m_nPropID, val);
			}
			break;
#endif

		case CUndoSlob::opDoubleProp:
			{
				double num;
				RetrieveNum(num);
				rec.m_pSlob->SetDoubleProp(rec.m_nPropID, num);
			}
			break;

		case CUndoSlob::opStrProp:
			{
				CString str;
				RetrieveStr(str);
				rec.m_pSlob->SetStrProp(rec.m_nPropID, str);
			}
			break;

		case CUndoSlob::opSlobProp:
			{
				CSlob* pSlobVal;
				RetrievePtr(pSlobVal);
				rec.m_pSlob->SetSlobProp(rec.m_nPropID, pSlobVal);
			}
			break;

		case CUndoSlob::opRectProp:
			{
				CRect rcVal;
				RetrieveRect(rcVal);
				rec.m_pSlob->SetRectProp(rec.m_nPropID, rcVal);
			}
			break;

		case CUndoSlob::opPointProp:
			{
				CPoint ptVal;
				RetrievePoint(ptVal);
				rec.m_pSlob->SetPointProp(rec.m_nPropID, ptVal);
			}
			break;
		}
	}
}



/////////////////////////////////////////////////////////////////////////////

CUndoStack::CUndoStack()
{
	m_nRedoSeqs = 0;
	m_nMaxLevels = theUndoSlob.GetMaxLevels();
}

CUndoStack::~CUndoStack()
{
	Flush();
	
	if (theUndoSlob.m_pStack == this)
		theUndoSlob.m_pStack = NULL;
}

void CUndoStack::Flush()
{
	POSITION pos = m_seqs.GetHeadPosition();
	while (pos != NULL)
		delete m_seqs.GetNext(pos);
	m_seqs.RemoveAll();
	
	pos = m_objects.GetHeadPosition();
	while (pos != NULL)
		delete m_objects.GetNext(pos);
	m_objects.RemoveAll();
	
	m_nRedoSeqs = 0;
}

void CUndoStack::FlushRedo()
{
	while (m_nRedoSeqs > 0)
	{
		delete m_seqs.RemoveHead();
		m_nRedoSeqs -= 1;
	}
}

void CUndoStack::Truncate()
{
	POSITION pos = m_seqs.FindIndex(m_nRedoSeqs + m_nMaxLevels);
	while (pos != NULL)
	{
#ifdef _DEBUG
	if (GetLogUndoFlag())
		TRACE("Undo record fell off the edge...\n");
#endif
		POSITION posRemove = pos;
		delete m_seqs.GetNext(pos);
		m_seqs.RemoveAt(posRemove);
	}
}

/////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CUndoSlob, CSlob)


CUndoSlob::CUndoSlob() : CSlob()
{
	ASSERT(this == &theUndoSlob); // only one of these is allowed!

	m_nRecording = 0;
	m_nMaxLevels = DEFAULT_UNDO_LEVELS;
	m_pLastSlob = NULL;
	m_nLastPropID = 0;
	m_nPauseLevel = 0;
	m_pSel = NULL;
	m_nSerial = 0;
	m_bFlushAtEnd = FALSE;
}


CUndoSlob::~CUndoSlob()
{
}


CObList* CUndoSlob::GetContentList()
{
	if (m_pStack == NULL)
		return NULL;
	
	return &m_pStack->m_objects;
}


// Is there something to undo in the active CSlobWnd?
//
BOOL CUndoSlob::CanUndo() const
{
	CSlobWnd* pWnd = GetActiveSlobWnd();
	if (pWnd == NULL)
		return FALSE;
	
	return pWnd->CanUndo();
}


// Is there something to redo in the active CSlobWnd?
//
BOOL CUndoSlob::CanRedo() const
{
	CSlobWnd* pWnd = GetActiveSlobWnd();
	if (pWnd == NULL)
		return FALSE;
	
	return pWnd->CanRedo();
}


// Set the maximum number of sequences that can be held at once.
//
void CUndoSlob::SetMaxLevels(int nLevels)
{
	if (nLevels < 1)
		return;

	m_nMaxLevels = nLevels;
	
	// ISSUE: Should this trim the stacks in open windows?
}


// Returns the maximum number of sequences that can be held at once.
//
int CUndoSlob::GetMaxLevels() const
{
	return m_nMaxLevels;
}


// Call this to after a sequence is recorded to prevent the next
// sequence from being coalesced with it.
//
void CUndoSlob::FlushLast()
{
	m_pLastSlob = NULL;
	m_nLastPropID = 0;
}


// Call this at the start of an undoable user action.  Calls may be nested
// as long as each call to BeginUndo is balanced with a call to EndUndo.
// Only the "outermost" calls actually have any affect on the undo buffer.
//
// The szCmd parameter should contain the text that you want to appear
// after "Undo" in the Edit menu.
//
// The pSelection parameter should point to the selection (CMultiSlob) that
// the operation will affect (pass the "source" selection when two selections
// are involved, or NULL if no selection changes should be recorded).
//
// The bResetCursor parameter is only used internally to modify behaviour
// when recording redo sequences and you should NOT pass anything for this
// parameter.
//
void CUndoSlob::BeginUndo(const char* szCmd,
	CSlobWnd* pSlobWnd /* = NULL */, BOOL bResetCursor /* = TRUE*/)
{
#ifdef _DEBUG
	if (GetLogUndoFlag())
		TRACE2("BeginUndo: %s (%d)\n", szCmd, m_nRecording);
#endif

	if (pSlobWnd == NULL)
		pSlobWnd = GetActiveSlobWnd();

	if ((pSlobWnd == NULL) || (!pSlobWnd->EnableUndo()))
		return;
	
	// Handle nesting
	m_nRecording += 1;
	if (m_nRecording != 1)
		return;
	
	ASSERT(m_pStack == NULL || m_pStack == pSlobWnd->m_pUndoStack);
	m_pStack = pSlobWnd->m_pUndoStack;

	if (bResetCursor) // this is the default case
	{
		// Disable Redo for non-Undo/Redo commands...
		m_pStack->FlushRedo();
	}

	m_pCurSeq = new CSlobSequence;
	m_pCurSeq->m_strDescription = szCmd;
	m_pCurSeq->m_nSerial = m_nSerial++;

	m_pSel = NULL;

	if (pSlobWnd != NULL && !InUndoRedo())
	{
		CMultiSlob* pSelection = (CMultiSlob*)pSlobWnd->GetSelection();
		if (pSelection != NULL &&
			pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		{
			OnPreSel(pSelection);
			m_pSel = pSelection;
		}
	}

	m_pDragDropObject = NULL;
	
	m_bFlushAtEnd = FALSE;
}

void CUndoSlob::BeginUndo(UINT nID,
	CSlobWnd* pSlobWnd /* = NULL */, BOOL bResetCursor /* = TRUE*/)
{
	CString str;

	VERIFY( str.LoadString( nID ) );
	BeginUndo( str, pSlobWnd, bResetCursor );
}

// Call this at the end of an undoable user action to cause the sequence
// since the BeginUndo to be stored in the undo buffer.
//
void CUndoSlob::EndUndo(BOOL bAbort /*=FALSE*/)
{
#ifdef _DEBUG
	if (GetLogUndoFlag())
		TRACE1("EndUndo: %d\n", m_nRecording - 1);
#endif

	if (m_nRecording == 0)
		return;

	if (bAbort)
		m_nRecording = 0;
	// Handle nesting
	else if (--m_nRecording != 0)
		return;
	
	if (bAbort || m_pCurSeq == NULL ||
		!m_pCurSeq->IsUseful(m_pLastSlob, m_nLastPropID))
	{
		// Remove empty or otherwise useless undo records!
		delete m_pCurSeq;
		m_pCurSeq = NULL;
		m_pStack = NULL;
		return;
	}

	if (m_pSel != NULL && !InUndoRedo())
		OnPostSel(m_pSel);

	// We'll keep it, add it to the list...
	if (m_pStack->m_nRedoSeqs > 0)
	{
		// Add AFTER any redo sequences we have but before any undo's
		POSITION pos = m_pStack->m_seqs.FindIndex(m_pStack->m_nRedoSeqs - 1);
		ASSERT(pos != NULL);
		m_pStack->m_seqs.InsertAfter(pos, m_pCurSeq);
	}
	else
	{
		// Just add before any other undo sequences
		m_pStack->m_seqs.AddHead(m_pCurSeq);
	}
	
	m_pCurSeq = NULL;
	
	m_pStack->Truncate(); // Make sure the undo buffer doesn't get too big!
	m_pStack = NULL;
	
	if (m_bFlushAtEnd)
		Flush();
	
	if (m_pDragDropObject != NULL)
	{
		CSlobWnd* pSourceWnd = m_pDragDropObject->GetSlobWnd();
		m_nRecording = 1;
		m_pStack = pSourceWnd->m_pUndoStack;
		
		m_pStack->FlushRedo();
		
		m_pCurSeq = new CSlobSequence;
		VERIFY( m_pCurSeq->m_strDescription.LoadString( IDS_MOVETO ) );
		m_pCurSeq->m_nSerial = m_nSerial++;
		
		m_pSel = NULL;
		m_bFlushAtEnd = FALSE;
		
		m_pDragDropObject = NULL;
	}
}


// Core of both the undo and the redo commands.  Returns TRUE if the
// undo stack is still around so a reversing record was generated.
//
BOOL CUndoSlob::DoUndoRedo(CSlobWnd* pSlobWnd)
{
	CWaitCursor waitCursor;

	ASSERT(m_pStack == NULL || m_pStack == pSlobWnd->m_pUndoStack);
	m_pStack = pSlobWnd->m_pUndoStack;
	
	if (m_pStack->m_nRedoSeqs == m_pStack->m_seqs.GetCount())
	{
		m_pStack = NULL;
		return FALSE; // nothing to undo!
	}

	m_bPerformingUndoRedo = TRUE;

	POSITION pos = m_pStack->m_seqs.FindIndex(m_pStack->m_nRedoSeqs);
	ASSERT(pos != NULL);
	CSlobSequence* pSeq = (CSlobSequence*)m_pStack->m_seqs.GetAt(pos);

	BeginUndo(pSeq->m_strDescription, pSlobWnd, FALSE); // Setup Redo

	// Remove this sequence after BeginUndo so the one inserted
	// there goes to the right place...
	m_pStack->m_seqs.RemoveAt(pos);
	
	pSeq->Apply(); // this may delete m_pStack and set it to NULL
	
	CUndoStack* pStack = m_pStack; // will get set to NULL in EndUndo
	FlushLast();
	EndUndo();
	FlushLast();
	m_pStack = pStack; // caller still needs this

	m_bPerformingUndoRedo = FALSE;

	delete pSeq;
	
	return pStack != NULL;
}


// Call this to perform an undo command.
//
void CUndoSlob::DoUndo(CSlobWnd* pWnd)
{
	ASSERT(pWnd != NULL);
	
	CUndoStack* pStack = pWnd->m_pUndoStack;
	if (pStack->m_nRedoSeqs == pStack->m_seqs.GetCount())
		return;
	
	POSITION pos = pStack->m_seqs.FindIndex(pStack->m_nRedoSeqs);
	long nSerial = ((CSlobSequence*)pStack->m_seqs.GetAt(pos))->m_nSerial;

	pWnd->EnableUpdates(FALSE);
	BOOL bSuccess = DoUndoRedo(pWnd);
	pWnd->EnableUpdates(TRUE);

	if (bSuccess)
	{
		// Do not bump the redo count if the undo flushed the buffer!  (This
		// happens when a resource is pasted/dropped, then opened, then a
		// property in it changes, and the user undoes back to before the
		// paste.)
		
		ASSERT(m_pStack != NULL);
		if (m_pStack->m_seqs.GetCount() != 0)
			m_pStack->m_nRedoSeqs += 1;
	
		m_pStack = NULL;
	}
	
	ASSERT(m_pStack == NULL);
	
	
	// Handle undo in linked windows...
	pos = CSlobWnd::GetHeadSlobWndPosition();
	while (pos != NULL)
	{
        CSlobWnd* pSlobWnd = CSlobWnd::GetNextSlobWnd(pos);
		
		pStack = pSlobWnd->m_pUndoStack;
		POSITION pos2 = pStack->m_seqs.FindIndex(pStack->m_nRedoSeqs);
		if (pos2 != NULL &&
			nSerial == ((CSlobSequence*)pStack->m_seqs.GetAt(pos2))->m_nSerial)
		{
			Link();
			DoUndo(pSlobWnd);
		}
	}
}


// Call this to perform a redo command.
//
void CUndoSlob::DoRedo(CSlobWnd* pWnd)
{
	ASSERT(pWnd != NULL);
	
	CUndoStack* pStack = pWnd->m_pUndoStack;
	
	if (pStack->m_nRedoSeqs == 0)
		return; // nothing in redo buffer
	
	POSITION pos = pStack->m_seqs.FindIndex(pStack->m_nRedoSeqs - 1);
	long nSerial = ((CSlobSequence*)pStack->m_seqs.GetAt(pos))->m_nSerial;
TRACE1("Redoing action %ld\n", nSerial);
	
	pStack->m_nRedoSeqs -= 1;
	
	pWnd->EnableUpdates(FALSE);
	DoUndoRedo(pWnd);
	pWnd->EnableUpdates(TRUE);
	
	m_pStack = NULL;
	
	
	// Handle redo in linked windows...
	pos = CSlobWnd::GetHeadSlobWndPosition();
	while (pos != NULL)
	{
        CSlobWnd* pSlobWnd = CSlobWnd::GetNextSlobWnd(pos);
		
		pStack = pSlobWnd->m_pUndoStack;
		if (pStack->m_nRedoSeqs < 1)
			continue;
		POSITION pos2 = pStack->m_seqs.FindIndex(pStack->m_nRedoSeqs - 1);
		if (pos2 != NULL &&
			nSerial == ((CSlobSequence*)pStack->m_seqs.GetAt(pos2))->m_nSerial)
		{
			Link();
			DoRedo(pSlobWnd);
		}
	}
}


// Generate a string appropriate for the undo menu command.
//
void CUndoSlob::GetUndoString(CString& strUndo, int nLevel)
{
	CSlobWnd* pWnd = GetActiveSlobWnd();
	ASSERT(pWnd != NULL);
	CUndoStack* pStack = pWnd->m_pUndoStack;
	
	if (pStack->m_nRedoSeqs + nLevel >= pStack->m_seqs.GetCount())
	{
		strUndo.Empty();
		return;
	}
	
	POSITION pos = pStack->m_seqs.FindIndex(pStack->m_nRedoSeqs + nLevel);
	strUndo = ((CSlobSequence*)pStack->m_seqs.GetAt(pos))->m_strDescription;
	
#if 0 // FUTURE: move undo formatting to shell
	// FUTURE: NYI: if nLevel > 0, return string for undo control
	
	static CString NEAR strUndoTemplate;
	
	if (strUndoTemplate.IsEmpty())
		VERIFY(strUndoTemplate.LoadString(IDS_UNDO));
	
	CString strUndoCmd;
	
	if (CanUndo())
	{
		CSlobWnd* pWnd = GetActiveSlobWnd();
		ASSERT(pWnd != NULL);
		CUndoStack* pStack = pWnd->m_pUndoStack;

		POSITION pos = pStack->m_seqs.FindIndex(pStack->m_nRedoSeqs);
		strUndoCmd = ((CSlobSequence*)
			pStack->m_seqs.GetAt(pos))->m_strDescription;
	}

	int cchUndo = strUndoTemplate.GetLength() - 2; // less 2 for "%s"
	sprintf(strUndo.GetBufferSetLength(cchUndo + strUndoCmd.GetLength()),
		strUndoTemplate, (const char*)strUndoCmd);
	strUndo.ReleaseBuffer();
#endif
}


// Generate a string appropriate for the redo menu command.
//
void CUndoSlob::GetRedoString(CString& strRedo, int nLevel)
{
	CSlobWnd* pWnd = GetActiveSlobWnd();
	ASSERT(pWnd != NULL);
	CUndoStack* pStack = pWnd->m_pUndoStack;
	
	if (nLevel >= pStack->m_nRedoSeqs)
	{
		strRedo.Empty();
		return;
	}
	
	POSITION pos = pStack->m_seqs.FindIndex(pStack->m_nRedoSeqs - 1 - nLevel);
	strRedo = ((CSlobSequence*)pStack->m_seqs.GetAt(pos))->m_strDescription;
	
#if 0 // FUTURE: move undo formatting to shell
	static CString NEAR strRedoTemplate;

	if (strRedoTemplate.IsEmpty())
		VERIFY(strRedoTemplate.LoadString(IDS_REDO));

	CString strRedoCmd;

	if (CanRedo())
	{
		CSlobWnd* pWnd = GetActiveSlobWnd();
		ASSERT(pWnd != NULL);
		CUndoStack* pStack = pWnd->m_pUndoStack;

		POSITION pos = pStack->m_seqs.FindIndex(pStack->m_nRedoSeqs - 1);
		strRedoCmd = ((CSlobSequence*)
			pStack->m_seqs.GetAt(pos))->m_strDescription;
	}

	int cchRedo = strRedoTemplate.GetLength() - 2; // less 2 for "%s"
	sprintf(strRedo.GetBufferSetLength(cchRedo + strRedoCmd.GetLength()),
		strRedoTemplate, (const char*)strRedoCmd);
	strUndo.ReleaseBuffer();
#endif
}


// Call this to completely empty the undo buffer.
//
void CUndoSlob::Flush()
{
	CSlobWnd* pWnd = GetActiveSlobWnd();
	ASSERT(pWnd != NULL);
	
	pWnd->EndUndo(TRUE);	// Abort any recording.
	pWnd->m_pUndoStack->Flush();
	
	m_bFlushAtEnd = TRUE;
}


//
// The following functions are used by the CSlob code to insert commands
// into the undo/redo sequence currently being recorded.  All of the On...
// functions are used to record changes to the various types of properties
// and are called by the CSlob::Set...Prop functions exclusively.
//


// Insert an array of bytes.
//
UINT CUndoSlob::Insert(const void* pv, int cb)
{
	ASSERT(m_pCurSeq != NULL);

	BYTE* rgb = (BYTE*)pv;

	m_pCurSeq->InsertAt(0, 0, cb);

	for (int ib = 0; ib < cb; ib += 1)
		m_pCurSeq->SetAt(ib, *rgb++);

	return cb;
}


// Insert a string.
//
UINT CUndoSlob::InsertStr(const char* sz)
{
	ASSERT(m_pCurSeq != NULL);

	BYTE* pb = (BYTE*)sz;
	int nStrLen = lstrlen(sz);

	InsertInt(nStrLen);
	if (nStrLen > 0)
	{
		m_pCurSeq->InsertAt(sizeof (int), 0, nStrLen);
		for (int nByte = 0; nByte < nStrLen; nByte += 1)
			m_pCurSeq->SetAt(sizeof (int) + nByte, *pb++);
	}
	return nStrLen + sizeof (int);
}


void CUndoSlob::OnSetIntProp(CSlob* pChangedSlob, UINT nPropID, UINT nOldVal, CPropBag* pBag)
{
	ASSERT(m_nRecording != 0);

	CIntUndoRecord undoRecord;
	undoRecord.m_op = opIntProp;
	undoRecord.m_nPropID = (WORD)nPropID;
	undoRecord.m_pSlob = pChangedSlob;
	undoRecord.m_pBag = pBag;
	undoRecord.m_nOldVal = nOldVal;
	Insert(&undoRecord, sizeof (undoRecord));
	pChangedSlob->AddDependant(this);

#ifdef _DEBUG
	if (GetLogUndoFlag())
	{
		CString strName;
		pChangedSlob->GetSlobName(strName);
		TRACE("Undo: SetIntProp(%s, %d, %d, 0x%08lx)\n",
			(const char*)strName, nPropID, nOldVal, pBag);
	}
#endif
}


void CUndoSlob::OnSetLongProp(CSlob* pChangedSlob, UINT nPropID, long nOldVal, CPropBag* pBag)
{
	ASSERT(m_nRecording != 0);

	CLongUndoRecord undoRecord;
	undoRecord.m_op = opLongProp;
	undoRecord.m_pSlob = pChangedSlob;
	undoRecord.m_pBag = pBag;
	undoRecord.m_nPropID = (WORD)nPropID;
	undoRecord.m_nOldVal = nOldVal;
	Insert(&undoRecord, sizeof (undoRecord));
	pChangedSlob->AddDependant(this);

#ifdef _DEBUG
	if (GetLogUndoFlag())
	{
		CString strName;
		pChangedSlob->GetSlobName(strName);
		TRACE3("Undo: SetLongProp(%s, %d, %ld)\n",
			(const char*)strName, nPropID, nOldVal);
	}
#endif
}


void CUndoSlob::OnSetDoubleProp(CSlob* pChangedSlob, UINT nPropID,
	double numOldVal, CPropBag* pBag)
{
	ASSERT(m_nRecording != 0);

	CDoubleUndoRecord undoRecord;
	undoRecord.m_op = opDoubleProp;
	undoRecord.m_pSlob = pChangedSlob;
	undoRecord.m_pBag = pBag;
	undoRecord.m_nPropID = (WORD)nPropID;
	undoRecord.m_numOldVal = numOldVal;
	Insert(&undoRecord, sizeof (undoRecord));
	pChangedSlob->AddDependant(this);

#ifdef _DEBUG
	if (GetLogUndoFlag())
	{
		CString strName;
		pChangedSlob->GetSlobName(strName);
		TRACE3("Undo: SetDoubleProp(%s, %d, %f)\n",
			(const char*)strName, nPropID, numOldVal);
	}
#endif
}


void CUndoSlob::OnSetStrProp(CSlob* pChangedSlob, UINT nPropID,
	const CString* pStrOldVal, CPropBag* pBag)
{
	ASSERT(m_nRecording != 0);

	InsertStr(((CString*)pStrOldVal)->GetBuffer(0));

	CUndoRecord undoRecord;
	undoRecord.m_op = opStrProp;
	undoRecord.m_pSlob = pChangedSlob;
	undoRecord.m_pBag = pBag;
	undoRecord.m_nPropID = (WORD)nPropID;
	Insert(&undoRecord, sizeof (undoRecord));
	pChangedSlob->AddDependant(this);

#ifdef _DEBUG
	if (GetLogUndoFlag())
	{
		CString strName;
		pChangedSlob->GetSlobName(strName);
		if (strName.GetLength() > 80)
		{
			strName = strName.Left(80);
			strName += "...";
		}
		CString strOldName = *pStrOldVal;
		if (strOldName.GetLength() > 80)
		{
			strOldName = strOldName.Left(80);
			strOldName += "...";
		}
		TRACE3("Undo: SetStrProp(%s, %d, %s)\n",
			(const char*)strName, nPropID, (const char*)strOldName);
	}
#endif
}


void CUndoSlob::OnSetSlobProp(CSlob* pChangedSlob, UINT nPropID,
	const CSlob* pOldSlob, CPropBag* pBag)
{
	ASSERT(m_nRecording != 0);

	CSlobUndoRecord undoRecord;
	undoRecord.m_op = opSlobProp;
	undoRecord.m_pSlob = pChangedSlob;
	undoRecord.m_pBag = pBag;
	undoRecord.m_nPropID = (WORD)nPropID;
	undoRecord.m_pOldVal = pOldSlob;
	Insert(&undoRecord, sizeof (undoRecord));
	pChangedSlob->AddDependant(this);

#ifdef _DEBUG
	if (GetLogUndoFlag())
	{
		CString strName;
		CString strOldName;
		pChangedSlob->GetSlobName(strName);
		if (pOldSlob == NULL)
			strOldName = "NULL";
		else
			pOldSlob->GetSlobName(strOldName);
		TRACE3("Undo: SetSlobProp(%s, %d, %s)\n",
			(const char*)strName, nPropID, (const char*)strOldName);
	}
#endif
}


void CUndoSlob::OnSetRectProp(CSlob* pChangedSlob, UINT nPropID,
	const CRect& rcOldVal, CPropBag* pBag)
{
	ASSERT(m_nRecording != 0);

	CRectUndoRecord undoRecord;
	undoRecord.m_op = opRectProp;
	undoRecord.m_pSlob = pChangedSlob;
	undoRecord.m_pBag = pBag;
	undoRecord.m_nPropID = (WORD)nPropID;
	undoRecord.m_rectOldVal = rcOldVal;
	Insert(&undoRecord, sizeof (undoRecord));
	pChangedSlob->AddDependant(this);

#ifdef _DEBUG
	if (GetLogUndoFlag())
	{
		CString strName;
		pChangedSlob->GetSlobName(strName);
		TRACE3("Undo: SetRectProp(%s, %d, (%d,%d,%d,%d))\n",
			(const char*)strName, nPropID, rcOldVal);
	}
#endif
}


void CUndoSlob::OnSetPointProp(CSlob* pChangedSlob, UINT nPropID,
	const CPoint& ptOldVal, CPropBag* pBag)
{
	ASSERT(m_nRecording != 0);

	CPointUndoRecord undoRecord;
	undoRecord.m_op = opPointProp;
	undoRecord.m_pSlob = pChangedSlob;
	undoRecord.m_pBag = pBag;
	undoRecord.m_nPropID = (WORD)nPropID;
	undoRecord.m_ptOldVal = ptOldVal;
	Insert(&undoRecord, sizeof (undoRecord));
	pChangedSlob->AddDependant(this);

#ifdef _DEBUG
	if (GetLogUndoFlag())
	{
		CString strName;
		pChangedSlob->GetSlobName(strName);
		TRACE3("Undo: SetPointProp(%s, %d, (%d,%d,%d,%d))\n",
			(const char*)strName, nPropID, ptOldVal);
	}
#endif
}


void CUndoSlob::OnMultiRemove(CMultiSlob* pMultiSlob, CSlob* pRemoveSlob)
{
	InsertPtr(pRemoveSlob);
	InsertInt(sizeof (CSlob*));

	CUndoRecord rec;
	rec.m_op = opAction;
	rec.m_nPropID = A_PreSel;
	rec.m_pSlob = pMultiSlob;
	rec.m_pBag = NULL;
	Insert(&rec, sizeof (CUndoRecord));

#ifdef _DEBUG
	if (GetLogUndoFlag())
		TRACE1("Undo: Remove from selection: 0x%08lx\n", pRemoveSlob);
#endif
}

void CUndoSlob::OnAddProp(CSlob* pSlob, CPropBag* pBag, UINT nPropID)
{
	// NYI
	TRACE("NYI CUndoSlob::OnAddProp() - undo for prop bags");
}

void CUndoSlob::OnRemoveIntProp(CSlob* pSlob, CPropBag* pBag,
	UINT nPropID, int nOldVal)
{
	// NYI
	ASSERT(FALSE);
}


void CUndoSlob::InsertSel(UINT nAction, CMultiSlob* pMultiSlob,
	CObList* pItems)
{
	ASSERT(m_pCurSeq != NULL);
	if (m_pCurSeq->GetSize() > 0 && m_pCurSeq->GetAt(0) == opAction &&
		*(UNALIGNED UINT*)&m_pCurSeq->ElementAt(1 + sizeof (CSlob*)) == nAction)
	{
#ifdef _DEBUG
		if (GetLogUndoFlag())
			TRACE("Ignoring duplicate OnPreSel...\n");
#endif
		return;
	}

	if (pMultiSlob->m_objects.IsEmpty())
		return;

	if (pItems == NULL)
		pItems = pMultiSlob->GetContentList();
	ASSERT(pItems != NULL);

	int nPtrs = 0;
	POSITION pos = pItems->GetHeadPosition();
	while (pos != NULL)
	{
		CSlob *pSlob = (CSlob *)pItems->GetNext (pos);
		ASSERT (pSlob->IsKindOf (RUNTIME_CLASS (CSlob)));

		// Ask the slob if it likes being inserted
		// into the undo slob. Slobs that can go away
		// spontaneously (dependency container's build slobs)
		// should refuse this.

		if (pSlob->CanAct (act_insert_into_undo_slob))
		{
			InsertPtr (pSlob);
			nPtrs += 1;
		}
	}
	InsertInt(sizeof (CSlob*) * nPtrs);

	CUndoRecord rec;
	rec.m_op = opAction;
	rec.m_nPropID = (WORD)nAction;
	rec.m_pSlob = pMultiSlob;
	rec.m_pBag = NULL;
	Insert(&rec, sizeof (CUndoRecord));
}


void CUndoSlob::OnPreSel(CMultiSlob* pMultiSlob)
{
	InsertSel(A_PreSel, pMultiSlob);

#ifdef _DEBUG
	if (GetLogUndoFlag())
		TRACE("Undo: OnPreSel\n");
#endif
}


void CUndoSlob::OnPostSel(CMultiSlob* pMultiSlob)
{
	InsertSel(A_PostSel, pMultiSlob);
}


#ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
//
// Undo related debugging aids
//

void CSlobSequence::Dump()
{
	m_nCursor = 0;
	while (m_nCursor < GetSize())
	{
		CUndoRecord rec;

		RetrieveRecord(rec);

		switch (rec.m_op)
		{
		default:
			TRACE1("Illegal undo opcode (%d)\n", rec.m_op);
			ASSERT(FALSE);

		case CUndoSlob::opAction:
			{
				int cbUndoRecord;
				RetrieveInt(cbUndoRecord);
				m_nCursor += cbUndoRecord;

				TRACE3("opAction: pSlob = 0x%08lx, nActionID = %d, "
					"nBytes = %d\n", rec.m_pSlob, rec.m_nPropID, cbUndoRecord);
			}
			break;

		case CUndoSlob::opIntProp:
		case CUndoSlob::opBoolProp:
			{
				int val;
				RetrieveInt(val);
				TRACE3("opInt: pSlob = 0x%08lx, nPropID = %d, val = %d\n",
					rec.m_pSlob, rec.m_nPropID, val);
			}
			break;

		case CUndoSlob::opLongProp:
			{
				long val;
				RetrieveLong(val);
				TRACE3("opInt: pSlob = 0x%08lx, nPropID = %d, val = %ld\n",
					rec.m_pSlob, rec.m_nPropID, val);
			}
			break;

		case CUndoSlob::opDoubleProp:
			{
				double num;
				RetrieveNum(num);
				TRACE3("opInt: pSlob = 0x%08lx, nPropID = %d, val = %f\n",
					rec.m_pSlob, rec.m_nPropID, num);
			}
			break;

		case CUndoSlob::opStrProp:
			{
				CString str;
				RetrieveStr(str);
				if (str.GetLength() > 80)
				{
					str = str.Left(80);
					str += "...";
				}
				TRACE3("opStr: pSlob = 0x%08lx, nPropID = %d, val = %s\n",
					rec.m_pSlob, rec.m_nPropID, (const char*)str);
			}
			break;

		case CUndoSlob::opSlobProp:
			{
				CSlob* pSlobVal;
				RetrievePtr(pSlobVal);
				TRACE3("opInt: pSlob = 0x%08lx, nPropID = %d, "
					"val = 0x%08lx\n", rec.m_pSlob, rec.m_nPropID, pSlobVal);
			}
			break;

		case CUndoSlob::opRectProp:
			{
				CRect rcVal;
				RetrieveRect(rcVal);
				TRACE3("opRect: pSlob = 0x%08lx, nPropID = %d, "
					"val = %d,%d,%d,%d\n", rec.m_pSlob, rec.m_nPropID, rcVal);
			}
			break;

		case CUndoSlob::opPointProp:
			{
				CPoint ptVal;
				RetrievePoint(ptVal);
				TRACE3("opPoint: pSlob = 0x%08lx, nPropID = %d, "
					"val = %d,%d,%d,%d\n", rec.m_pSlob, rec.m_nPropID, ptVal);
			}
			break;
		}
	}
}


void CUndoSlob::Dump()
{
	CSlobWnd* pWnd = GetActiveSlobWnd();
	if (pWnd == NULL)
	{
		TRACE("No undo buffer to dump!\n");
		return;
	}
	
	CUndoStack* pStack = pWnd->m_pUndoStack;
	
	int nRecord = 0;
	POSITION pos = pStack->m_seqs.GetHeadPosition();
	while (pos != NULL)
	{
		CSlobSequence* pSeq = (CSlobSequence*)pStack->m_seqs.GetNext(pos);
		TRACE2("Record (%d) %s:\n", nRecord,
			nRecord < pStack->m_nRedoSeqs ? "redo" : "undo");
		pSeq->Dump();
		nRecord += 1;
	}
}


extern "C" void DumpUndo()
{
	theUndoSlob.Dump();
}

#endif
