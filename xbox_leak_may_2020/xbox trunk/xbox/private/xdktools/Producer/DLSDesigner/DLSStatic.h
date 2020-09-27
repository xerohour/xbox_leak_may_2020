#ifndef __DLSTATIC_H__
#define __DLSTATIC_H__

class CDLSStatic
{

public:
	CDLSStatic(UINT nID, CString csTitle, CRect rcPosition, LOGFONT* plfTextFont);
	~CDLSStatic();
	
	static HRESULT CreateControl(CWnd* pParentWnd, CWnd* pStaticWnd, CDLSStatic** ppDLSStatic);

public:

	void		OnDraw(CDC* pDC);
	
	UINT		GetID();								// The resource ID for this static control
	CRect		GetPosition();							// The position in the container
	CString		GetText();								// Display text
	LOGFONT*	GetLogFont();							// The LOGFONT struct keeps track of how to display the text

	void		SetText(CString sText);					// New text for the control
	void		SetTextColor(COLORREF clrText);			// Color of the text
	void		SetBackgroundColor(COLORREF clrBckgnd); // Background color for the control
	void		SetTextAlign(UINT nTextAlign);			// Text alignment is DT_LEFT by default
	void		UnderlineText(BOOL bUnderline = TRUE);	// Turn underline option on/off
	void		SetSelected(BOOL bSelection = TRUE);	// Marks the static as selected
	BOOL		IsSelected();							// Is this control selected currently?
	BOOL		IsPointInControl(CPoint ptCheck);		// Checks if the given point lies inside the controls boundaries
	void		Invalidate(CDC* pDC);					// Invalidates the control

private:

	UINT		m_nID;
	CString		m_csTitle;
	CRect		m_rcPosition;
	LOGFONT*	m_plfTextFont;
	COLORREF	m_clrText;
	COLORREF	m_clrBackground;
	COLORREF	m_clrSelectedText;
	COLORREF	m_clrSelectedBackground;
	UINT		m_nTextAlignment;
	BOOL		m_bSelected;
	CWnd*		m_pParentWnd;
};

#endif // __DLSTATIC_H__