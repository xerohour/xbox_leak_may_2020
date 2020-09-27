#if !defined(AFX_RHYTHMDLG_H__DDA3C785_3389_11D2_89B4_00C04FD912C8__INCLUDED_)
#define AFX_RHYTHMDLG_H__DDA3C785_3389_11D2_89B4_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RhythmDlg.h : header file
//


#define MEASURE_LINE_COLOR		RGB(0,0,255)
#define BEAT_LINE_COLOR			RGB(255,0,255)


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid window

class CRhythmGrid : public CButton
{
friend class CRhythmDlg;

// Construction
public:
	CRhythmGrid();

// Attributes
public:
	CRhythmDlg*	m_pRhythmDlg;

// Operations
protected:
	void DrawOneBeat( CDC* pDC, long lBeat, BOOL fDrawOne );
	void DrawBeats( CDC* pDC, long lStartBeat );
	void DrawMeasureBeatLines( CDC* pDC, long lStartBeat );
	void DrawEntireGrid( CDC* pDC );
	int GetBeatsPerRhythmGrid();
	void SetClipRect( CDC* pDC );
	void GetGridRect( CRect* pRectGrid );
	long FindBeat( int nXPos, int nYPos );
	BOOL IsBeatChecked( long lBeat );
	void SetBeat( long lBeat, BOOL fRedraw );
	void ShiftClick( long lBeat );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRhythmGrid)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRhythmGrid();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRhythmGrid)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg dialog

class CRhythmDlg : public CDialog
{
friend class CRhythmGrid;

// Construction
public:
	CRhythmDlg( CWnd* pParent = NULL );
	virtual ~CRhythmDlg();

// Dialog Data
	//{{AFX_DATA(CRhythmDlg)
	enum { IDD = IDD_CUSTOM_RHYTHM };
	CScrollBar	m_scrollGrid;
	//}}AFX_DATA

// Member variables
private:
	CFont*					m_pFont;
	CRhythmGrid				m_btnGrid;			// Subclasses IDC_GRID

public:
	DWORD*					m_pRhythmMap;		// Variable array of rhythms for chord matching
	DirectMusicTimeSig		m_TimeSignature;	// Pattern time signature
	WORD					m_wNbrMeasures;		// Pattern length in measures

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRhythmDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRhythmDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClear();
	afx_msg void OnHalf();
	afx_msg void OnQuarter();
	afx_msg void OnWhole();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RHYTHMDLG_H__DDA3C785_3389_11D2_89B4_00C04FD912C8__INCLUDED_)
