// msglog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgLog window

typedef struct
{
	int cchText;
	LPSTR pszText;
	MSGSTREAMDATA2 msd2;
} LINE;


class CMsgLog : public CWnd
{
// Construction
public:
	CMsgLog();

// Attributes
public:

// Operations
public:
	void SetMaxLines(int cLinesMax);
	int GetMaxLines()
	{
		return m_cLinesMax;
	}
	int GetNumLines()
	{
		return m_cLines;
	}
	int GetFirstIndex()
	{
		return m_iFirst;
	}
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	void AddLine(LPSTR psz, PMSGSTREAMDATA pmsd);
	void ClearAll();
	void SelectLine(int iSelect);
	void ShowSelection();
	int GetCurSel()
	{
		return m_iSelected;
	}
	void ShowSelItemProperties();
	void ShowViewedItemProperties(CMsgView *pView);
	BOOL IsEmpty();
	PMSGDESC GetPopupMsgDesc()
	{
		return m_pmdPopup;
	}
	LINE *GetLinesArray()
	{
		return(m_aLines);
	}

// Implementation
public:
	virtual ~CMsgLog();

protected:
	void VerticalScroll(int n);
	void HorizontalScroll(int n);
	int GetVisibleChars();
	void UpdateScrollBars(BOOL bRedraw);
	void CalcLineSizes();
	void InvalidateLine(int iLine);

	int m_iFirst;				// Index of first line in queue
	int m_cLines;				// Number of lines in queue
	int m_iTop;					// Index of line at top of window
	int m_iLeft;				// X offset of the window
	int m_cchMax;				// Max String Length
	int m_cLinesMax;			// Max number of LINEs
	int m_cMaxVisibleLines;		// Max whole visible lines possible
	int m_iSelected;			// Index of selected line (-1 for no selection)
	HFONT m_hfont;				// Font to draw with
	int m_Tdx;					// Font width
	int m_Tdy;					// Font height
	LINE* m_aLines;				// Array of m_cLinesMax LINEs
	PMSGDESC m_pmdPopup;		// Last msg clicked on to bring up popup menu
	BOOL m_fFocusLostBetween;	// was focus lost and regained between SelectLine() calls?
	RECT m_rcFocus;				// focus rectangle in message log view -- keep for horz scroll

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgLog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
