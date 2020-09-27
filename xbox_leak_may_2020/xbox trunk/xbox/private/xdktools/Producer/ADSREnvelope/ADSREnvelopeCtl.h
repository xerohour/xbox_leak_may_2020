#if !defined(AFX_ADSRENVELOPECTL_H__71AE3634_A9BD_11D0_BCBA_00AA00C08146__INCLUDED_)
#define AFX_ADSRENVELOPECTL_H__71AE3634_A9BD_11D0_BCBA_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "Utilities.h"

#define SEGMENTS		6				// Delay, Attack, Hold, Decay, Sustain and Release
#define SEGMENT_THUMBS	SEGMENTS - 1	// Boxes to drag the segments

#define DELAY_COLOR		RGB(0, 0, 0)		// Delay segment color
#define ATTACK_COLOR	RGB(255, 0, 0)		// Attack segment color
#define HOLD_COLOR		RGB(255, 0, 255)	// Hold segment color
#define DECAY_COLOR		RGB(0, 128, 64)		// Decay segment color
#define	SUSTAIN_COLOR	RGB(0, 0, 0)		// Sustain segment color same as the delay color
#define RELEASE_COLOR	RGB(0, 0, 255)		// Release segment color

// ADSREnvelopeCtl.h : Declaration of the CADSREnvelopeCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl : See ADSREnvelopeCtl.cpp for implementation.

class CADSREnvelopeCtrl : public COleControl
{
	DECLARE_DYNCREATE(CADSREnvelopeCtrl)

// Constructor
public:
	CADSREnvelopeCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CADSREnvelopeCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CADSREnvelopeCtrl();

	void UpdateADSREnvelope(CDC *pDC, const CRect* rcBounds = NULL);	
	
	DECLARE_OLECREATE_EX(CADSREnvelopeCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CADSREnvelopeCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CADSREnvelopeCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CADSREnvelopeCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CADSREnvelopeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CADSREnvelopeCtrl)`
	afx_msg long GetReleaseTime();
	afx_msg void SetReleaseTime(long nNewValue);
	afx_msg long GetAttackTime();
	afx_msg void SetAttackTime(long nNewValue);
	afx_msg long GetDecayTime();
	afx_msg void SetDecayTime(long nNewValue);
	afx_msg long GetSustainLevel();
	afx_msg void SetSustainLevel(long nNewValue);
	afx_msg long GetDelayTime();
	afx_msg void SetDelayTime(long nNewValue);
	afx_msg long GetHoldTime();
	afx_msg void SetHoldTime(long nNewValue);
	afx_msg void SetDLS1(BOOL bDLS1);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CADSREnvelopeCtrl)
	void FireADSREDecayTimeChanged(long NewDecayTime)
		{FireEvent(eventidADSREDecayTimeChanged,EVENT_PARAM(VTS_I4), NewDecayTime);}
	void FireADSREAttackTimeChanged(long NewAttackTime)
		{FireEvent(eventidADSREAttackTimeChanged,EVENT_PARAM(VTS_I4), NewAttackTime);}
	void FireADSREReleaseTimeChanged(long NewReleaseTime)
		{FireEvent(eventidADSREReleaseTimeChanged,EVENT_PARAM(VTS_I4), NewReleaseTime);}
	void FireADSRESustainLevelChanged(long NewSustainLevel)
		{FireEvent(eventidADSRESustainLevelChanged,EVENT_PARAM(VTS_I4), NewSustainLevel);}
	void FireRealDecayChanged(long lNewRealDecay)
		{FireEvent(eventidRealDecayChanged,EVENT_PARAM(VTS_I4), lNewRealDecay);}
	void FireRealReleaseChanged(long lNewRealRelease)
		{FireEvent(eventidRealReleaseChanged,EVENT_PARAM(VTS_I4), lNewRealRelease);}
	void FireADSRMouseMoveStart()
		{FireEvent(eventidADSRMouseMoveStart,EVENT_PARAM(VTS_NONE));}
	void FireADSRHoldTimeChanged(long lNewHoldTime)
		{FireEvent(eventidADSRHoldTimeChanged,EVENT_PARAM(VTS_I4), lNewHoldTime);}
	void FireADSREDelayTimeChanged(long lNewDelayTime)
		{FireEvent(eventidADSREDelayTimeChanged,EVENT_PARAM(VTS_I4), lNewDelayTime);}
	void FireADSREHoldTimeChanged(long lNewHoldTime)
		{FireEvent(eventidADSREHoldTimeChanged,EVENT_PARAM(VTS_I4), lNewHoldTime);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CADSREnvelopeCtrl)
	dispidReleaseTime = 1L,
	dispidAttackTime = 2L,
	dispidDecayTime = 3L,
	dispidSustainLevel = 4L,
	dispidDelayTime = 5L,
	dispidHoldTime = 6L,
	dispidSetDLS1 = 7L,
	eventidADSREDecayTimeChanged = 1L,
	eventidADSREAttackTimeChanged = 2L,
	eventidADSREReleaseTimeChanged = 3L,
	eventidADSRESustainLevelChanged = 4L,
	eventidRealDecayChanged = 5L,
	eventidRealReleaseChanged = 6L,
	eventidADSRMouseMoveStart = 7L,
	eventidADSRHoldTimeChanged = 8L,
	eventidADSREDelayTimeChanged = 9L,
	eventidADSREHoldTimeChanged = 10L,
	//}}AFX_DISP_ID
	};

private:
	void UpdateRealReleaseTime() { FireRealReleaseChanged(TimeCents2Mils(m_lRTime) * m_lSLevel / 1000); }
	void UpdateRealDecayTime() { FireRealDecayChanged(TimeCents2Mils(m_lDTime) * (1000 - m_lSLevel) / 1000); }
	enum { START, DELAY, ATTACK, HOLD, DECAY, RELEASE, END, TIMECENTS_MIN = -11960, TIMECENTS_RANGE =  18347};

	/* returns whether the given handle (DELAY, ATTACK, etc) is visible given the DLS1 setting. */
	bool ADRVisible(BYTE btLevel);
	
	bool	m_bHasCapture;
	POINT	m_ptADRPts[SEGMENTS + 1];		
	CRect	m_rcADRPosBox[SEGMENT_THUMBS];
	int		m_nSelPB;			// Currently selected PosBox
	int		m_nOnTop;
	CPoint	m_ptPrevMousePos;
	bool	m_bFireUndoNotify;


	long m_lDelayTime;		// Delay Time in TIMECENTS
	long m_lATime;			// Attack Time in TIMECENTS
	long m_lHTime;			// Hold Time in TIMECENTS
	long m_lDTime;			// Decay Time in TIMECENTS
	long m_lRTime;			// Release Time in TIMECENTS
	long m_lSLevel;			// Sustain Level in tenths of a percent

	int m_nDelayPixels;		// Delay Position calculated from Delay Time
	int	m_nAPixels;			// Attack Position calculated from Attack Time
	int m_nHPixels;			// Hole Position calculated from Hold Time
	int m_nDPixels;			// Decay Position calculated from Decay Time
	int m_nRPixels;			// Release Position calculated from Release Time

	int	m_nWidth;
	int m_nHeight;
	int	m_nMaxPixels;		
	int m_nSegmentLength;	// Each segment can have this max length

	BOOL m_bDLS1;			// DLS1 envelope

	double m_TimeCentsPerPixel;
	double m_PixelsPerTenthPercent;

};	

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADSRENVELOPECTL_H__71AE3634_A9BD_11D0_BCBA_00AA00C08146__INCLUDED)
