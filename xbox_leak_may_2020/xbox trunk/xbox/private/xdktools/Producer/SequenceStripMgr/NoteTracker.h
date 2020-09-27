#ifndef __NOTETRACKER_H__
#define __NOTETRACKER_H__

// NoteTracker.h : header file
//
#include <afxext.h>

interface IDMUSProdTimeline;

#define NTRK_NOTHING		0
#define NTRK_SELECTING		1

/////////////////////////////////////////////////////////////////////////////
// CNoteTracker class

class CNoteTracker : public CRectTracker
{
public:
   CNoteTracker( CWnd* pWnd, IDMUSProdTimeline* pITimeline, CSequenceStrip* pSequenceStrip, short nAction,
	   CRect* initialRect = NULL);
   CNoteTracker( CRect* initialRect );
   virtual ~CNoteTracker();
   
protected:
	WORD DoAction();
	void ScrollSequenceStrip();
	void SetStartEndPoints();

// Overrides
public:
	virtual void AdjustRect( int nHandle, RECT* pRect );
	virtual void DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd );

// public members
public:

// private members
protected:
	CWnd*				m_pWnd;
	IDMUSProdTimeline*	m_pITimeline;
	CSequenceStrip*		m_pSequenceStrip;

	CPoint				m_ptStart;	
	CPoint				m_ptEnd;	
	BOOL				m_fSetStartPoint;
	
	DWORD				m_dwScrollTick;
	
};

#endif // __NOTETRACKER_H__
