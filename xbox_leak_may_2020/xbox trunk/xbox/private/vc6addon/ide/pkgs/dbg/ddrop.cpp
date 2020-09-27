// ddrop.cpp
// OLE Drag/Drop

#include "stdafx.h"
#pragma hdrstop

#include <shlmenu.h>
//#include "inc\dbggrid.h"
//#include "inc\varsview.h"
//#include "inc\extern.h"

UINT g_cfTMText = ::RegisterClipboardFormat(_T("MSDEVDbgTMText"));
UINT g_cfColSelect = ::RegisterClipboardFormat(_T("MSDEVColumnSelect"));

//m_pDragInfo
class CDragInfo
{
public:
	CDragInfo();
	~CDragInfo();
	CDragInfo(CBaseVarsView * pVarView, DROPEFFECT effect);
	CDragInfo(CMultiEdit * pME,  DROPEFFECT effect);

	// do we activate the window?
	//
	BOOL		FTriggerEntryDelay();
	BOOL		IsEntryDelayElapsed();
	void		ResetEntry() { if( 0 == (m_dwDragEnterTime = ::GetTickCount())) m_dwDragEnterTime = 1; }

	// other activation (e.g. tabs)
	BOOL		IsDelayElapsed();
	BOOL        IsDelayRunning() { return m_dwDelayTime != 0; }
	void        ClearDelay() { m_dwDelayTime = 0; }
	void		StartDelay(DWORD dwExtra = 0) { if (0 == (m_dwDelayTime = ::GetTickCount())) m_dwDelayTime = 1; m_dwDelayExtra = dwExtra; }
	void        SetDelayInterval(DWORD dwInterval) { m_dwDelayInterval = dwInterval; }
	
	DWORD       DelayExtra() { return m_dwDelayExtra; }
	void        SetDelayExtra( DWORD dw ) { m_dwDelayExtra = dw; }

	// drop effect
	//
	DROPEFFECT	Effect() { return m_LastDropEffect; }
	void		SetEffect(DROPEFFECT effect) { m_LastDropEffect = effect; }
	void		AddEffect(DROPEFFECT effect) { m_LastDropEffect |= effect; }
	void		SetEffectFromKeys(DWORD dwKeyState);

	// total duration of drag/drop
	DWORD Duration() { return ::GetTickCount() - m_dwStartTime; }

	// context (right mouse) menu
	BOOL		IsContext()                { return m_bContext; }
	void		SetContext(BOOL bContext)  { m_bContext = bContext; }

	// IsFromSelf()
	//
	// Pass GetDocument() to see if you're dragging from this DOC
	// Pass NULL (no arg) to see if you're dragging from this VIEW
	//
	BOOL		IsFromSelf();
	void		ClearSelf();
	CDbgGridWnd *	GridFrom() { return m_pGridFrom; }

private:
	DROPEFFECT      m_LastDropEffect;   // last drop effect
	BOOL			m_bDraggingFrom;    // dragging from this view
	CDbgGridWnd *	m_pGridFrom;		// grid this drag came from

	DWORD			m_dwDragEnterTime;  // time we started drag
	DWORD			m_dwDelayTime;      // time we started this delay
	DWORD           m_dwDelayExtra;     // extra delay info for client
	DWORD			m_dwDelayInterval;  // interval for delay

	DWORD			m_dwStartTime;     // time we started everything

	BOOL			m_bContext;         // show context menu at drop
};


CDragInfo::~CDragInfo()
{
//TRACE("Destroy CDragInfo\n");
	if (m_bDraggingFrom)
	{
		ClearSelf();
	}
}

#if 0
#define DEF_DELAY  (500) // somewhat large for testing delay code
#else
#define DEF_DELAY  (g_dwDragDelay)
#endif

///////////////////////////////////////
// Enforce a minimum total drag duration to sanity-check the whole process:
//
// if (m_pDragInfo->Duration() < g_dwDragDelay + MIN_DRAG_DURATION)
// {
//     // It's lasted long enough that the user may actually
//     // have wanted a real drag-drop op.
// }
//
#ifdef _DEBUG
// not const for debug-time tuning without recompilation
      DWORD MIN_DRAG_DURATION = 50;
#else
const DWORD MIN_DRAG_DURATION = 50;
#endif

////////////////////////////////////////////////////////////////////////

CDragInfo::CDragInfo()
{
	m_LastDropEffect = DROPEFFECT_NONE;
	ResetEntry();
	m_dwDelayTime = 0;
	m_dwDelayInterval = DEF_DELAY;
	m_dwDelayExtra = (DWORD)-1;
	m_bDraggingFrom = FALSE;
	m_pGridFrom = NULL;
	m_bContext = FALSE;
//TRACE("Create Default CDragInfo\n");
	m_dwStartTime = ::GetTickCount();
}

CDragInfo::CDragInfo(CBaseVarsView * pVarView, DROPEFFECT effect)
{
	m_LastDropEffect = DROPEFFECT_NONE;
	ResetEntry();
	m_dwDelayTime = 0;
	m_dwDelayInterval = DEF_DELAY;
	m_dwDelayExtra = (DWORD)-1;
	m_bDraggingFrom = TRUE;
	m_pGridFrom = pVarView->GetActiveGrid();
	m_bContext = FALSE;
//TRACE("Create CBaseVarsView CDragInfo\n");
	m_dwStartTime = ::GetTickCount();
}

CDragInfo::CDragInfo(CMultiEdit * pME,  DROPEFFECT effect)
{
	m_LastDropEffect = DROPEFFECT_NONE;
	ResetEntry();
	m_dwDelayTime = 0;
	m_dwDelayInterval = DEF_DELAY;
	m_dwDelayExtra = (DWORD)-1;
	m_bDraggingFrom = TRUE;
	m_bContext = FALSE;
	m_dwStartTime = ::GetTickCount();
}


BOOL CDragInfo::IsFromSelf()
{ 
	return m_bDraggingFrom;
}

void CDragInfo::ClearSelf()
{
	m_bDraggingFrom = FALSE;
}

void CDragInfo::SetEffectFromKeys(DWORD dwKeyState)
{
	if (dwKeyState & MK_CONTROL)
		SetEffect(DROPEFFECT_COPY);
	else
		SetEffect(DROPEFFECT_MOVE);
}

BOOL CDragInfo::FTriggerEntryDelay()
{
	if (0 == m_dwDragEnterTime)
		return FALSE;
	else if (::GetTickCount() - m_dwDragEnterTime > DEF_DELAY)
	{
//TRACE("Entry delay triggered.\n");
		m_dwDragEnterTime = 0;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CDragInfo::IsEntryDelayElapsed()
{
	if (0 == m_dwDragEnterTime)
		return TRUE;
	else if (::GetTickCount() - m_dwDragEnterTime > DEF_DELAY)
	{
//TRACE("Non-pulsed entry delay triggered.\n");
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CDragInfo::IsDelayElapsed()
{
	if (0 == m_dwDelayTime)
		return FALSE;
	else if (::GetTickCount() - m_dwDelayTime > m_dwDelayInterval)
	{
		return TRUE;
	}
	else
		return FALSE;
}

////////////////////////////////////////////////////////////////
// MFC OLE Drag/Drop overrides

DROPEFFECT
CBaseVarsView::OnDragEnter( COleDataObject* pData, DWORD grfKeyState, CPoint point )
{
	CDbgGridWnd *pGrid = GetActiveGrid();
	if (NULL == pGrid)
		return DROPEFFECT_NONE;

	{
		CWnd * pWndFocus = GetFocus();
		if ((NULL != m_pDragInfo) && 
			(pWndFocus != this) && !IsChild(pWndFocus))
			m_pDragInfo->ResetEntry();
	}
	
	if (pGrid->CanDropText())
	{
		if ((NULL != m_pDragInfo && m_pDragInfo->Effect() != DROPEFFECT_NONE) || 
			pData->IsDataAvailable(CF_TEXT))
		{
			// create the drag info thing
			if (NULL == m_pDragInfo)
			{
				m_pDragInfo = new CDragInfo;
				if (NULL == m_pDragInfo)
					return DROPEFFECT_NONE;
			}

			// we only copy -- no move or link
			m_pDragInfo->SetEffect(DROPEFFECT_COPY);
			m_pDragInfo->SetContext(grfKeyState & MK_RBUTTON);
			return m_pDragInfo->Effect();
		}
	}
	return DROPEFFECT_NONE;
}


DROPEFFECT
CBaseVarsView::OnDragOver( COleDataObject* pData, DWORD grfKeyState, CPoint point )
{
	if (NULL == m_pDragInfo)
		return DROPEFFECT_NONE;

	CDbgGridWnd *pGrid = GetActiveGrid();
	if (NULL == pGrid)
		return DROPEFFECT_NONE;

	// default to copy -- no move or link
	DROPEFFECT deRet = pGrid->CanDropText() ? DROPEFFECT_COPY : DROPEFFECT_NONE;

	if (m_pDragInfo->Effect() != DROPEFFECT_NONE)
	{
		if (m_pDragInfo->IsFromSelf())
		{
			// check if over current row
			CDbgGridWnd *pGridFrom = m_pDragInfo->GridFrom();
			if (pGridFrom != NULL && pGridFrom == pGrid)
			{
				CTreeGridRow *pRow = pGrid->GetCurSel();
				if (pRow != NULL)
				{
					CRect rc;
					pRow->GetRect(rc);
					if (rc.PtInRect(point))
					{
						// we're over the row the drag came from
						deRet = DROPEFFECT_NONE;
					}
				}
			}
		}
		// we only copy -- no move or link
		m_pDragInfo->SetEffect(DROPEFFECT_COPY);

		// if we're not the active window and the entry delay is elapsed, activate
		if (m_pDragInfo->FTriggerEntryDelay())
		{
			CWnd * pWndFocus = GetFocus();
			if ((pWndFocus != this) && !IsChild(pWndFocus))
			{
				theApp.SetActiveView(GetSafeHwnd());
			}
		}
		else if (m_pDragInfo->IsEntryDelayElapsed())
		{
			// activate another tab?
			CRect rc;
			m_widgetScroller.GetWindowRect(&rc);
			ScreenToClient(&rc);
			if (rc.PtInRect(point))
			{
				for (int i = 0; i < m_arrChildWnd.GetSize(); i++)
				{
					m_widgetScroller.GetTabRect(i, rc);
					// compensate for overlap so we don't flash when you're between tabs
					rc.InflateRect(-9,-1);
					// convert the tab's coords to screen coords
					m_widgetScroller.ClientToScreen(&rc);
					// convert tab's screen coords to this window's client coords
					ScreenToClient(&rc);
					// hit on the tab?
					if (rc.PtInRect(point))
					{
						// don't reactivate the current tab
						if (i != m_curIndex)
						{
							if (m_pDragInfo->IsDelayElapsed() &&
								m_pDragInfo->DelayExtra() == (DWORD)i
								)
							{
								m_widgetScroller.ActivateTab(i);
								m_pDragInfo->ClearDelay();
							}
							else if (!m_pDragInfo->IsDelayRunning() || 
								m_pDragInfo->DelayExtra() != (DWORD)i)
							{
								// start activation delay for this tab
								m_pDragInfo->StartDelay((DWORD)i);
							}
						}
						//else
						//	m_pDragInfo->ClearDelay(); // over the current tab
						break;
					}
				}
			}
			else
				m_pDragInfo->ClearDelay(); // not over a tab
		}
	}
	return deRet;
}

DROPEFFECT
CBaseVarsView::OnDropEx( COleDataObject* pData, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
{
	if (NULL == m_pDragInfo)
		return DROPEFFECT_NONE;
	if ((m_pDragInfo->Effect() == DROPEFFECT_NONE) ||
		(m_pDragInfo->Duration() < g_dwDragDelay + MIN_DRAG_DURATION)
		)
	{
		OnDragLeave();
		return DROPEFFECT_NONE;
	}

//	BOOL bMoveInSelf = m_pDragInfo->IsFromSelf() && 
//		(dropDefault & DROPEFFECT_MOVE);
	
	if (m_pDragInfo->IsContext())
	{
		BEGIN_POPUP_MENU(DragDrop)
			MENU_ITEM_(ID_DRAG_MOVE, IDS_DRAG_MOVE)
			MENU_ITEM_(ID_DRAG_COPY, IDS_DRAG_COPY)
			MENU_SEPARATOR()
			MENU_ITEM_(ID_DRAG_CANCEL, IDS_DRAG_CANCEL)
		END_POPUP_MENU()

		CPoint ptScreen (point);
		ClientToScreen (&ptScreen);

		CBContextPopupMenu menu;
		menu.Create(MENU_CONTEXT_POPUP(DragDrop));
		BOOL bEnableMove = (dropDefault & DROPEFFECT_MOVE) && (dropList & DROPEFFECT_MOVE);
		menu.EnableMenuItem(ID_DRAG_MOVE, bEnableMove ? MF_ENABLED : MF_DISABLED);

	    TRACE2("CBShow: DragDrop (%s, %d)\r\n",__FILE__, __LINE__);

		UINT nCmd = menu.TrackPopupMenu( TPM_RIGHTBUTTON | TPM_RETURNCMD, ptScreen.x, ptScreen.y, AfxGetMainWnd() );
		switch (nCmd)
		{
		case ID_DRAG_MOVE: 
			m_pDragInfo->SetEffect(DROPEFFECT_MOVE); 
			break;
		
		case ID_DRAG_COPY:
			m_pDragInfo->SetEffect(DROPEFFECT_COPY); 
			break;
		
		case ID_DRAG_CANCEL:
		default:
			m_pDragInfo->SetEffect(DROPEFFECT_NONE);
			break;
		}
	}
	else
		m_pDragInfo->SetEffect(DROPEFFECT_COPY);

	if (m_pDragInfo->Effect() != DROPEFFECT_NONE)
	{
		if (!InsertFromDataObject(pData, point))
			m_pDragInfo->SetEffect(DROPEFFECT_NONE);
	}

	DROPEFFECT deRet = m_pDragInfo->Effect();
	OnDragLeave();

	AfxGetMainWnd()->SetForegroundWindow();
	SetFocus();	
	return deRet;
}

void
CBaseVarsView::OnDragLeave()
{
	// m_pDragInfo destruction in drag from self case is handled in DoDrag
	if (NULL != m_pDragInfo && !m_pDragInfo->IsFromSelf())
	{
		delete m_pDragInfo;
		m_pDragInfo = NULL;
	}
}

////////////////////////////////////////////////////////////////
// Drag/Drop utilities
BOOL CBaseVarsView::CanMoveText() 
{
	CDbgGridWnd *pGrid = GetActiveGrid();
	return pGrid->CanMoveText();
}

BOOL CBaseVarsView::CanDropText()
{
	CDbgGridWnd *pGrid = GetActiveGrid();
	return pGrid->CanDropText();
}

BOOL CBaseVarsView::DoDrag(CPoint point)
{
	{
		CWnd * pWndFocus = GetFocus();
		if ((pWndFocus != this) && !IsChild(pWndFocus))
			return FALSE;
	}

	CRect rcSlop;

	{
		CDbgGridWnd *pGrid = GetActiveGrid();
		if (NULL == pGrid)
		{
			ASSERT(0);
			return FALSE;
		}
		CGridElement * pElement = pGrid->ElementFromPoint(point);
		if (NULL == pElement)
		{
			ASSERT(0);
			return FALSE;
		}
		CGridRow * pGRow = pElement->GetRow();
		if (pGRow == pGrid->GetCaptionRow())
		{
			// Don't drag caption
			return FALSE;
		}
		if (static_cast<CTreeGridRow *>(pGRow)->IsNewRow())
		{
			// Don't drag New row
			return FALSE;
		}
		// Don't drag +/- glyph
		CWatchRow * pWRow = static_cast<CWatchRow *>(pGRow);
		if (WRH_GLYPH == pWRow->HitTest(point))
			return FALSE;

		// Get the drag slop rect from the row
		pGrid->GetDragSlopRect(point, rcSlop);
	}

	// drop effects
	DROPEFFECT dropeffect = DROPEFFECT_COPY;
	if (CanMoveText())
		dropeffect |= DROPEFFECT_MOVE;

	// save current selection
	ASSERT(NULL == m_pDragInfo);
	m_pDragInfo = new CDragInfo(this, dropeffect);
	if (NULL == m_pDragInfo)
		return FALSE;

	COleDataSource data;
	if (!InitDataSource(data, point))
	{
		delete m_pDragInfo;
		m_pDragInfo = NULL;
		return FALSE;
	}

	dropeffect = data.DoDragDrop(dropeffect, &rcSlop);

//	// move in self handled by destination in OnDropEx
//	if (dropeffect == DROPEFFECT_MOVE && CanMoveText() && 
//		m_pDragInfo != NULL && m_pDragInfo->IsFromSelf()
//		)
//	{
//		// move from here to somewhere else -- delete here
//		// TODO:
//	}

//  cross-app activation, which we aren't doing from the debugger right now
//	if (DROPEFFECT_NONE == dropeffect)
//	{
//		AfxGetMainWnd()->SetForegroundWindow();
//		theApp.SetActiveView(GetSafeHwnd());
//	}

	if (m_pDragInfo)
	{
		if (m_pDragInfo->Duration() < g_dwDragDelay + MIN_DRAG_DURATION)
		{
			// "It all happened so fast..., it can't be real."
	//		TRACE("Firing debug drag cancel timing heuristic\n");
			dropeffect = DROPEFFECT_NONE;
		}
		delete m_pDragInfo;
		m_pDragInfo = NULL;
	}
	return dropeffect != DROPEFFECT_NONE;
}


BOOL CBaseVarsView::InitDataSource(COleDataSource& data, const CPoint & point)
{
	BOOL bRet = FALSE;
	HANDLE hData = NULL;

//	data.Empty();

	//
	// get active grid
	//
	CDbgGridWnd *pGrid = GetActiveGrid();
	if (NULL == pGrid)
		return FALSE;

	//
	// get TM text
	//
	{
		// Don't allow drag/drop if the user is currently in the midst of editing
		// a name or value.	
//		if (pGrid->m_pControlWnd != NULL)
//			return FALSE;

		UINT uHit = WRH_NONE;
		CGridElement * pElement = pGrid->ElementFromPoint(point);
		if (NULL == pElement)
		{
			ASSERT(0);
			bRet = FALSE;
			goto L_Exit;;
		}
		
		CGridRow * pGRow = pElement->GetRow();
		// Don't drag caption
		if (pGRow == pGrid->GetCaptionRow())
		{
			bRet = FALSE;
			goto L_Exit;
		}

		// Don't drag New row
		if (static_cast<CTreeGridRow *>(pGRow)->IsNewRow())
		{
			bRet = FALSE;
			goto L_Exit;
		}

		// Don't drag +/- glyph
		CWatchRow * pWRow = static_cast<CWatchRow *>(pGRow);
		uHit = pWRow->HitTest(point);
		if (WRH_GLYPH == uHit)
		{
			bRet = FALSE;
			goto L_Exit;
		}

		CString str;
		CTM * pTM =  pGrid->GetSelectedTM( );
		if (pTM != NULL)
		{
			if (WRH_VALUE == uHit)
				str = pTM->GetEditableValue();
			else
			{
				str = pTM->GetExpr();
				if (str.IsEmpty())
				{
					// If the expression is not evaluatable (a var is currently 
					// out of scope, for example), GetExpr() returns an empty string.
					// In this case, get whatever text is in the column.
					static_cast<CTreeGridRow *>(pGRow)->GetColumnText(COL_NAME, str);
				}
			}
			if (!str.IsEmpty())
			{
				// convert string to handle
				hData = HandleFromString(str);
				data.CacheGlobalData(g_cfTMText, hData);
				//
				// get plain text, too, for dragging to the editor
				//
				hData = pGrid->CFTextHandle();
				if (hData != NULL)
				{
					data.CacheGlobalData(CF_TEXT, hData);
				}
				bRet = TRUE;
			}
		}
	}
L_Exit:
	if (bRet)
		return TRUE;
	else
	{
		data.Empty();
		return FALSE;
	}
}

// Insert text from an HGLOBAL
static BOOL InsertFromHandle(CDbgGridWnd *pGrid, HGLOBAL hData)
{
	int size;
	LPSTR pch;

	if (size = RealGlobalSize(hData))
	{
		// Get a pointer to the data.  Leave if we can't do it.
		VERIFY((pch = (LPSTR)RealGlobalLock(hData)));
		if (!pch)
			return FALSE;
		pGrid->ImportWatchFromBuffer(pch, strlen(pch));
		RealGlobalUnlock(hData);
	}
	return TRUE;
}

// Insert text from a COleDataObject
BOOL CBaseVarsView::InsertFromDataObject(COleDataObject * data, const CPoint & point)
{
//TRACE("Elapsed drag time: %d\n", m_pDragInfo ? m_pDragInfo->Duration() : 0);
	CDbgGridWnd *pGrid = GetActiveGrid();
	if (NULL == pGrid)
		return FALSE;

	if (data->IsDataAvailable(g_cfTMText))
	{
		return InsertFromHandle(pGrid, data->GetGlobalData(g_cfTMText));
	}
	else if (data->IsDataAvailable(CF_TEXT))
	{
		return InsertFromHandle(pGrid, data->GetGlobalData(CF_TEXT));
	}
	else
	{
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////

DROPEFFECT CMultiEdit::OnDragEnter( COleDataObject* pData, DWORD grfKeyState, CPoint point )
{
	if (NULL != m_pDragInfo && GetFocus() != this)
		m_pDragInfo->ResetEntry();
	if (CanDropText())
	{
		if ((NULL != m_pDragInfo && m_pDragInfo->Effect() != DROPEFFECT_NONE) || 
			pData->IsDataAvailable(CF_TEXT))
		{
			// create the drag info thing
			if (NULL == m_pDragInfo)
			{
				m_pDragInfo = new CDragInfo;
				if (NULL == m_pDragInfo)
					return DROPEFFECT_NONE;
			}
			// we only copy -- no move or link
			m_pDragInfo->SetEffect(DROPEFFECT_COPY);

			OnDragEnterFeedback(grfKeyState, point);
			m_pDragInfo->SetEffect(m_pDragInfo->Effect() & ~(DROPEFFECT_MOVE|DROPEFFECT_LINK));
			m_pDragInfo->SetContext(grfKeyState & MK_RBUTTON);
			return m_pDragInfo->Effect();
		}
	}
	return DROPEFFECT_NONE;
}



DROPEFFECT CMultiEdit::OnDragOver( COleDataObject* pDataObject, DWORD grfKeyState, CPoint point )
{
	if (NULL == m_pDragInfo)
		return DROPEFFECT_NONE;
	if (m_pDragInfo->Effect() != DROPEFFECT_NONE)
	{
		// we only copy -- no move or link
		m_pDragInfo->SetEffect(DROPEFFECT_COPY);

		// if we're not the active window and the entry delay is elapsed, activate
		if (m_pDragInfo->FTriggerEntryDelay())
		{
			if (GetFocus() != this)
			{
				theApp.SetActiveView(GetSafeHwnd());
			}
		}
		else if (m_pDragInfo->IsEntryDelayElapsed())
		{
			OnDragOverFeedback(grfKeyState, point);
		}
	}
	return m_pDragInfo->Effect();
}


void CMultiEdit::OnDragLeave()
{
	OnDragLeaveFeedback();
	// m_pDragInfo destruction in drag from self case is handled in DoDrag
	if (NULL != m_pDragInfo && !m_pDragInfo->IsFromSelf())
	{
		delete m_pDragInfo;
		m_pDragInfo = NULL;
	}
}


DROPEFFECT CMultiEdit::OnDropEx( COleDataObject* pData, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point )
{
	if (NULL == m_pDragInfo)
		return DROPEFFECT_NONE;
	if (m_pDragInfo->Effect() == DROPEFFECT_NONE)
	{
		OnDragLeave();
		return DROPEFFECT_NONE;
	}

//	BOOL bMoveInSelf = m_pDragInfo->IsFromSelf() && 
//		(dropDefault & DROPEFFECT_MOVE);
	
	if (m_pDragInfo->IsContext())
	{
		BEGIN_POPUP_MENU(DragDrop)
			MENU_ITEM_(ID_DRAG_MOVE, IDS_DRAG_MOVE)
			MENU_ITEM_(ID_DRAG_COPY, IDS_DRAG_COPY)
			MENU_SEPARATOR()
			MENU_ITEM_(ID_DRAG_CANCEL, IDS_DRAG_CANCEL)
		END_POPUP_MENU()

		CPoint ptScreen (point);
		ClientToScreen (&ptScreen);

		CBContextPopupMenu menu;
		menu.Create(MENU_CONTEXT_POPUP(DragDrop));
		BOOL bEnableMove = (dropDefault & DROPEFFECT_MOVE) && (dropList & DROPEFFECT_MOVE);
		menu.EnableMenuItem(ID_DRAG_MOVE, bEnableMove ? MF_DISABLED : MF_ENABLED);

	    TRACE2("CBShow: DragDrop (%s, %d)\r\n",__FILE__, __LINE__);

		UINT nCmd = menu.TrackPopupMenu( TPM_RIGHTBUTTON | TPM_RETURNCMD, ptScreen.x, ptScreen.y, AfxGetMainWnd() );
		switch (nCmd)
		{
		case ID_DRAG_MOVE: 
			m_pDragInfo->SetEffect(DROPEFFECT_MOVE); 
			break;
		
		case ID_DRAG_COPY:
			m_pDragInfo->SetEffect(DROPEFFECT_COPY); 
			break;
		
		case ID_DRAG_CANCEL:
		default:
			m_pDragInfo->SetEffect(DROPEFFECT_NONE);
			break;
		}
	}
	else
		m_pDragInfo->SetEffect(DROPEFFECT_COPY);

	if (m_pDragInfo->Effect() != DROPEFFECT_NONE)
	{
		if (!InsertFromDataObject(pData, point))
			m_pDragInfo->SetEffect(DROPEFFECT_NONE);
	}

	DROPEFFECT deRet = m_pDragInfo->Effect();

	OnDragLeave();

	AfxGetMainWnd()->SetForegroundWindow();
	SetFocus();	
	return deRet;
}


BOOL CMultiEdit::DoDrag(CPoint point)
{
	// drop effects
	DROPEFFECT dropeffect = DROPEFFECT_COPY;
	if (CanMoveText())
		dropeffect |= DROPEFFECT_MOVE;

	// slop rect for starting drag
	CRect rect;
	rect.TopLeft() = point;
	rect.BottomRight() = point;
	rect.InflateRect(g_dwDragMinRadius, g_dwDragMinRadius);

	// save current selection
	ASSERT(NULL == m_pDragInfo);
	m_pDragInfo = new CDragInfo(this, dropeffect);
	if (NULL == m_pDragInfo)
		return FALSE;

	COleDataSource data;
	if (!InitDataSource(data, point))
		return FALSE;

	dropeffect = data.DoDragDrop(dropeffect, &rect);

//	// move in self handled by destination in OnDropEx
//	if (dropeffect == DROPEFFECT_MOVE && CanMoveText() && 
//		m_pDragInfo != NULL && m_pDragInfo->IsFromSelf()
//		)
//	{
//		// move from here to somewhere else -- delete here
//		// TODO:
//	}

	if (DROPEFFECT_NONE == dropeffect)
	{
		AfxGetMainWnd()->SetForegroundWindow();
		theApp.SetActiveView(GetSafeHwnd());
	}

	if (m_pDragInfo)
	{
		delete m_pDragInfo;
		m_pDragInfo = NULL;
	}
	return dropeffect != DROPEFFECT_NONE;
}


BOOL CMultiEdit::InitDataSource(COleDataSource& data, const CPoint & point)
{
	HANDLE hData = NULL;
	
	hData = CopySelection(0, (size_t)-1);
	if (hData != NULL)
	{
		data.CacheGlobalData(CF_TEXT, hData);
			if (m_fColumnSelect)
			data.CacheGlobalData(g_cfColSelect, HandleFromString(_T("C")));
		return TRUE;
	}
	else
		return FALSE;
}


BOOL CMultiEdit::InsertFromDataObject(COleDataObject * data, const CPoint & point)
{
	return FALSE;
}

