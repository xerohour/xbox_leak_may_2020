#ifndef __NOTETRACKER_H__
#define __NOTETRACKER_H__

// NoteTracker.h : header file
//

#define NTRK_NOTHING		0
#define NTRK_SELECTING		1

interface IDMUSProdTimeline;
class CPianoRollStrip;

/////////////////////////////////////////////////////////////////////////////
// CNoteTracker class

class CNoteTracker : public CRectTracker
{
public:
   CNoteTracker( CWnd* pWnd, IDMUSProdTimeline* pITimeline, CPianoRollStrip* pPianoRollStrip, short nAction,
	   CRect* initialRect = NULL);
   CNoteTracker( CRect* initialRect );
   virtual ~CNoteTracker();
   
protected:
	WORD DoAction();
	void ScrollPianoRoll();
	void SetStartEndPoints();

// Overrides
public:
	virtual void AdjustRect( int nHandle, RECT* pRect );
	virtual void DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd );

// public members
public:

// private members
protected:
	CWnd*			m_pWnd;
	IDMUSProdTimeline*	m_pITimeline;
	CPianoRollStrip*	m_pPianoRollStrip;

	CPoint			m_ptStart;	
	CPoint			m_ptEnd;	
	BOOL			m_fSetStartPoint;
	
	DWORD			m_dwScrollTick;
	
};

#endif // __NOTETRACKER_H__
