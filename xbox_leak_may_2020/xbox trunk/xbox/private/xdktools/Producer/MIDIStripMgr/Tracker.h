#ifndef __CURVETRACKER_H__
#define __CURVETRACKER_H__

// Tracker.h : header file
//

#define CTRK_NOTHING		0
#define CTRK_SELECTING		1
#define CTRK_INSERTING		2
#define CTRK_EDITING		3
#define CTRK_DRAGGING		4
#define CTRK_DRAGGING_EDGE	5

/////////////////////////////////////////////////////////////////////////////
// CCurveTracker class

typedef CList<CDirectMusicStyleCurve*, CDirectMusicStyleCurve*> CCurveList;
class CCurveTracker : public CRectTracker
{
public:
   CCurveTracker( CWnd* pWnd, IDMUSProdTimeline* pITimeline, CCurveStrip* pCurveStrip, short nAction,
	   CRect* initialRect = NULL);
   CCurveTracker( CRect* initialRect );
   virtual ~CCurveTracker();
   
protected:
	WORD DoAction();
	void ScrollPianoRoll();
	void SetStartEndPoints(int nHandle);
	void LimitRects();

public:
	void SetCurve( CDirectMusicStyleCurve* pDMCurve );
	void SetCurveList(CCurveList* pCurveList);
	void SetXOffset(int nXOffset);

// Overrides
public:
	virtual void AdjustRect( int nHandle, RECT* pRect );
	virtual void DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd );

// public members
public:
	// used for dragging
	CSize			m_offset;
	CDirectMusicStyleCurve*	m_pDMCurve;	
	CCurveList*		m_pCurveList;
	int				m_nXOffset;			

// private members
protected:
	CWnd*				m_pWnd;
	IDMUSProdTimeline*	m_pITimeline;
	CCurveStrip*		m_pCurveStrip;
	short				m_nAction;
	long				m_lPartClockLength;

	CPoint			m_ptStart;	
	CPoint			m_ptEnd;
	BOOL			m_fSetStartPoint;
	
	DWORD			m_dwScrollTick;
	BOOL			m_fScrolledPianoRoll;
};

#endif // __CURVETRACKER_H__
