#include "stdafx.h"
#include "RegionKeyboardCtl.h"
#include "Region.h"


int CRegion::m_arrKeyboardMap[12] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};	// 0 is for a White key; 1 for the black key
int CRegion::m_arrKeySizes[2] = {WHITE_KEY_WIDTH, BLACK_KEY_WIDTH};			// White key is 5 pixels; Black is 4 pixels wide


CRegion::CRegion(int nLayer, 
				 int nStartNote, int nEndNote,
				 int nStartVelocity, int nEndVelocity, 
				 int nRootNote, const CString& strWaveName) :
				 				  m_nLayer(nLayer), m_nStartNote(nStartNote), m_nEndNote(nEndNote), 
								  m_nStartVelocity(nStartVelocity), m_nEndVelocity(nEndVelocity),
								  m_nRootNote(nRootNote), m_bSelected(FALSE), m_pKeyboard(NULL), 
								  m_bPlaying(FALSE), m_bEnabled(TRUE), m_strWaveName(strWaveName)
{
	ASSERT(m_nEndNote >= m_nStartNote);
	ASSERT(m_nEndVelocity >= m_nStartVelocity);
	m_DrawColor = PALETTERGB(255, 0, 0);
	//m_Layer0DrawColor = PALETTERGB(7, 39, 141);
	m_Layer0DrawColor = m_DrawColor;
	m_NoteOnColor = PALETTERGB(0, 128, 255);
	m_BkColor = PALETTERGB(255, 255, 255);
}

void CRegion::SetParentControl(CRegionKeyboardCtrl* pParent)
{
	ASSERT(pParent);
	m_pKeyboard = pParent;
}

void CRegion::SetLayer(int nLayer)
{
	m_nLayer = nLayer;
}

int CRegion::GetLayer()
{
	return m_nLayer;
}

int CRegion::GetStartNote()
{
	return m_nStartNote;
}

int	CRegion::GetEndNote()
{
	return m_nEndNote;
}

void CRegion::GetNoteRange(int& nStartNote, int& nEndNote)
{
	nStartNote = m_nStartNote;
	nEndNote = m_nEndNote;
}

void CRegion::SetNoteRange(int nStartNote, int nEndNote)
{
	m_nStartNote = nStartNote;
	m_nEndNote = nEndNote;

	ValidateAndSetRanges();
}


void CRegion::GetVelocityRange(int& nStartVelocity, int& nEndVelocity)
{
	nStartVelocity = m_nStartVelocity;
	nEndVelocity = m_nEndVelocity;
}

void CRegion::SetVelocityRange(int nStartVelocity, int nEndVelocity)
{
	m_nStartVelocity = nStartVelocity;
	m_nEndVelocity = nEndVelocity;

	ValidateAndSetRanges();
}

void CRegion::SetWaveName(LPCTSTR pszWaveName)
{
	m_strWaveName = pszWaveName;
}

void CRegion::SetRegionColor(COLORREF clrRegion)
{
	m_DrawColor = clrRegion;
}

void CRegion::SetBackgroundColor(COLORREF clrBkgnd)
{
	m_BkColor = clrBkgnd;
}

void CRegion::Draw(CDC* pDC, int nLayerAreaHeight, int nFirstVisibleLayer)
{
	ASSERT(m_pKeyboard);
	ASSERT(pDC);

	if(m_nLayer < nFirstVisibleLayer || m_nLayer > nFirstVisibleLayer + (NUMBER_OF_VISIBLE_LAYERS - 1))
		return;

	int nStartX = GetPointFromNote(true, m_nStartNote);
	int nEndX = GetPointFromNote(false, m_nEndNote);

	int nStartY = nLayerAreaHeight - (((m_nLayer - nFirstVisibleLayer) + 1) * REGION_LAYER_HEIGHT);
	
	if(nStartY < 0)
		nStartY = 0;

	int nEndY = nStartY + REGION_LAYER_HEIGHT;
	CRect rectRegion(nStartX, nStartY, nEndX, nEndY);

	m_RegionRect = rectRegion;
	CRect rcInner = m_RegionRect;
	rcInner.InflateRect(-1, -1);

	// figure out the inner color
	COLORREF clrDraw;
	if(m_bSelected && m_pKeyboard->GetFocus() == m_pKeyboard || m_bPlaying)
	{
		clrDraw = m_DrawColor;
		if(m_nLayer == 0)
		{
			clrDraw = m_Layer0DrawColor;
		}

		if(m_bPlaying && m_bEnabled)
		{
			clrDraw = m_NoteOnColor;
		}
	}
	else if(m_bSelected && m_pKeyboard->GetFocus() != m_pKeyboard)
	{
		clrDraw = ::GetSysColor(COLOR_INACTIVECAPTION);
	}
	else
	{
		clrDraw = COLOR_LAYER_CANVAS;
	}

	// font is expected to be set & restored, and background restored, by caller (CRegionKeyboardCtrl::DrawRegionRects)
	pDC->SetBkColor(clrDraw);

	// draw
	pDC->Rectangle(&m_RegionRect);
	CString strWaveName = m_strWaveName;
	strWaveName.MakeUpper();
	pDC->ExtTextOut(rcInner.left+1, m_RegionRect.top, ETO_OPAQUE | ETO_CLIPPED, rcInner, strWaveName, strWaveName.GetLength(), NULL);
}


BOOL CRegion::IsPointInRegion(CPoint point)
{
	return m_RegionRect.PtInRect(point);
}

BOOL CRegion::IsPointInRegion(CPoint point, bool bCheckForKeyboard)
{
	if(bCheckForKeyboard)
	{
		CPoint ptTopLeft = m_RegionRect.TopLeft();
		CPoint ptBottomRight = m_RegionRect.BottomRight();

		return (ptTopLeft.x <= point.x && ptBottomRight.x >= point.x);
	}
	else
		return IsPointInRegion(point);
}

BOOL CRegion::IsNoteInRegion(int nNote)
{
	if(nNote >= m_nStartNote && nNote <= m_nEndNote)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRegion::IsNoteInRegion(int nLayer, int nNote)
{
	if(m_nLayer == nLayer && nNote >= m_nStartNote && nNote <= m_nEndNote)
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CRegion::IsNoteInRegion(int nLayer, int nNote, int nVelocity, bool bIgnoreLayer)
{
	if(!bIgnoreLayer)
	{
		if(m_nLayer == nLayer && nNote >= m_nStartNote && nNote <= m_nEndNote &&
			nVelocity >= m_nStartVelocity && nVelocity <= m_nEndVelocity)
		{
			return TRUE;
		}
	}
	else
	{
		if(nNote >= m_nStartNote && nNote <= m_nEndNote &&
			nVelocity >= m_nStartVelocity && nVelocity <= m_nEndVelocity)
		{
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CRegion::IsOverlappingRegion(int nLayer, int nStartNote, int nEndNote)
{
	if(nLayer != m_nLayer)
		return FALSE;

	if(nStartNote >= m_nStartNote && nStartNote <= m_nEndNote)
		return TRUE;

	if(nEndNote >= m_nStartNote && nEndNote <= m_nEndNote)
		return TRUE;

	if(m_nStartNote >= nStartNote && m_nStartNote <= nEndNote)
		return TRUE;

	if(m_nEndNote >= nStartNote && m_nEndNote <= nEndNote)
		return TRUE;

	return FALSE;
}


int CRegion::GetPointFromNote(bool bStartPoint, int nNote)
{

	int nOctaves = 0;
	int nNoteInOctave = nNote;
	
	if(nNote > 0)
	{
		nOctaves = nNote / 12;					// Number of octaves up
		nNoteInOctave = nNote - nOctaves * 12;	// The note in the octave
	}

	if(nNoteInOctave < 0)
		nNoteInOctave = 0;

	int nPixelsOffsetInOctave = 0;

	for(int nNoteIndex = 0; nNoteIndex <= nNoteInOctave; nNoteIndex++)
	{
		if(m_arrKeyboardMap[nNoteIndex] == 0)
			nPixelsOffsetInOctave += WHITE_KEY_WIDTH;
	}

	if(m_arrKeyboardMap[nNoteInOctave] == 1)				// Black key
	{
		if(bStartPoint)
			nPixelsOffsetInOctave -= BLACK_KEY_WIDTH / 2;
		else
			nPixelsOffsetInOctave += BLACK_KEY_WIDTH / 2;
	}
	else if(m_arrKeyboardMap[nNoteInOctave] == 0)			// White key
	{
		if(bStartPoint)
		{
			if(nNoteInOctave == 0 || (nNoteInOctave > 0 && m_arrKeyboardMap[nNoteInOctave - 1] == 0)) // Previous key is white
				nPixelsOffsetInOctave -= WHITE_KEY_WIDTH;
			else
				nPixelsOffsetInOctave -= WHITE_KEY_WIDTH - BLACK_KEY_WIDTH/2 ;
		}
		else if(nNote < 126 && m_arrKeyboardMap[nNoteInOctave + 1] == 1) // The next key is a black key
			nPixelsOffsetInOctave -= WHITE_KEY_WIDTH - BLACK_KEY_WIDTH - 2;

	}
	
	return (nOctaves * OCTAVE_WIDTH) + nPixelsOffsetInOctave;
}


BOOL CRegion::IsSelected()
{
	return m_bSelected;
}

void CRegion::SetSelected(BOOL bSelected)
{
	m_bSelected = bSelected;
}

void CRegion::SetStartNote(int nNote)
{
	if(nNote < 0)
		nNote = 0;
	
	if(nNote <= m_nEndNote)
		m_nStartNote = nNote;
}

void CRegion::SetEndNote(int nNote)
{
	if(nNote < 0)
		nNote = 0;
	
	if(nNote >= m_nStartNote)
		m_nEndNote = nNote;
}

void CRegion::ValidateAndSetRanges()
{
	// Check if notes are in range
	if(m_nStartNote < 0)
		m_nStartNote = 0;
	if(m_nStartNote > 127)
		m_nStartNote = 127;
	if(m_nEndNote <	0)
		m_nEndNote = 0;
	if(m_nEndNote > 127)
		m_nEndNote = 127;

	// Swap the notes if required
	int nTemp = m_nStartNote;
	if(m_nStartNote > m_nEndNote)
	{
		m_nStartNote = m_nEndNote;
		m_nEndNote = nTemp;
	}

	// Check if the velocities are in range
	if(m_nStartVelocity < 0)
		m_nStartVelocity = 0;
	if(m_nStartVelocity > 127)
		m_nStartVelocity = 127;
	if(m_nEndVelocity < 0)
		m_nEndVelocity = 0;
	if(m_nEndVelocity > 127)
		m_nEndVelocity = 127;

	// Swap the velocities if required
	nTemp = m_nStartVelocity;
	if(m_nStartVelocity > m_nEndVelocity)
	{
		m_nStartVelocity = m_nEndVelocity;
		m_nEndVelocity = nTemp;
	}
}


BOOL CRegion::IsPointOnStartBoundary(CPoint point)
{
	int nDragZoneWidth = DRAG_ZONE_WIDTH;
	if(abs(m_nStartNote - m_nEndNote) < 4)
		nDragZoneWidth = 1;

	CPoint ptTopLeft = m_RegionRect.TopLeft();
	if(point.x - ptTopLeft.x  >= 0 &&  abs(ptTopLeft.x - point.x) <= nDragZoneWidth)
		return TRUE;

	return FALSE;
}

BOOL CRegion::IsPointOnEndBoundary(CPoint point)
{
	int nDragZoneWidth = DRAG_ZONE_WIDTH;
	if(abs(m_nStartNote - m_nEndNote) < 4)
		nDragZoneWidth = 1;

	CPoint ptBottomRight = m_RegionRect.BottomRight();
	if(ptBottomRight.x - point.x >= 0 && abs(ptBottomRight.x - point.x) <= nDragZoneWidth)
		return TRUE;

	return FALSE;

}

void CRegion::SetRootNote(int nNote)
{
	ASSERT(nNote >= 0 && nNote <= 127);
	
	if(nNote >= 0 && nNote <= 127)
		m_nRootNote = nNote;
}

int CRegion::GetRootNote()
{
	return m_nRootNote;
}

void CRegion::SetPlayState(BOOL bOn)
{
	m_bPlaying = bOn;
}

void CRegion::Enable(BOOL bEnable)
{
	m_bEnabled = bEnable;
}

CRect CRegion::GetRect()
{
	return m_RegionRect;
}
