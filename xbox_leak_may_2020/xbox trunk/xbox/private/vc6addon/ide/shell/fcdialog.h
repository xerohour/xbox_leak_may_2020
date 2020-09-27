// fcdialog.h
//
// Headers necessary for the font and color dropdown controls used in
// the tabbed options pages.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __FCDIALOG_H__
#define __FCDIALOG_H__

/////////////////////////////////////////////////////////////////////////////
// CTTBitmap

class CTTBitmap : CBitmap
{
	BITMAP		m_bmData;
	CBitmap		m_bmpMask;
	BOOL		m_bValid;

public:
	CTTBitmap ();
	~CTTBitmap ();

	BOOL CreateMask ();

	BOOL DrawImage (CDC * pDC, CRect& rect);
	inline void AdjustDrawRect (CRect& rect);
};

///// AdjustDrawRect - Make room in a rect for the bitmap
//
//	The "rect" argument is assumed to be a place to draw a font name.  This function
//	adjusts the left side of that "rect" to make room for the TrueType bitmap.
///
inline void CTTBitmap::AdjustDrawRect (CRect& rect)
{
	rect.left += m_bmData.bmWidth;
}


/////////////////////////////////////////////////////////////////////////////
// CFontNameCombo

class CFontNameCombo : public CComboBox
{
DECLARE_DYNAMIC (CFontNameCombo)

	CTTBitmap		m_bmpTrueType;
	BOOL			m_bProportional;

	//
	struct FontInfo { 
		_TCHAR *	szName;
		DWORD		dwFontType;
		};

	CPtrArray		m_rgFontInfo;	// Necessary because Windows is retarded.

// Construction
public:
	CFontNameCombo();

// Attributes
public:
	DWORD GetItemData (int index) const;
	int GetLBText (int index, LPTSTR szText) const;
	void GetLBText (int index, CString & strText) const;
	int FindStringExact (int index, LPCTSTR szFind) const;

// Operations
private:
	static int CALLBACK FillCallBack(CONST ENUMLOGFONT *, CONST NEWTEXTMETRIC *, int, LPARAM);

public:
	void AddEntry (const CString& strName, DWORD fontType);
	void Enumerate (BOOL bProportional = TRUE);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontNameCombo)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFontNameCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontNameCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSizeCombo
//
//	Represents the Font Size combo box.  It maintains a list of unique point
//	sizes.

class CSizeCombo : public CComboBox
{
private:
	static DWORD	m_rgTTSizes[];

	int				m_ptPreferredSize;

// Construction
public:
	CSizeCombo();

// Attributes
public:

	int AddSize ( int cPoints );
	void SetStandardSizes ();

	DWORD	GetSize ( int ixSize = -1 );
	int		SetToSize (int nSize, BOOL bMatchClosest);

	inline void	SetPreferredSize (int cPoints);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSizeCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSizeCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSizeCombo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


///// SetPreferredSize - Sets the "center of gravity size"
//
//	The "preferred size" is the size the control will try to display, if possible.
//	The problem being solved is that switching around between fonts with different
//	available sizes tends to make the size drift in one direction or another.  For
//	example, if we start at size 11, then switch to a font with only 10 and 12, we now
//	display 12.  If we go back to the original font we will now being showing 12 instead
//	of 11, although to user didn't make the change.
//	The Preferred size should be set when the category changes (including init time), and
//	whenever the user changes the font size.
///
inline void	CSizeCombo::SetPreferredSize (int cPoints)
{
	m_ptPreferredSize = cPoints;
}


/////////////////////////////////////////////////////////////////////////////
// CColorCombo window
//
//	Displays a list of color swatches.  The elements are references by their
//	RGB values.

class CColorCombo : public CComboBox
{
private:
	static COLORREF		m_rgStandardColors[16];
	static CBrush		m_brushSwatchFrame;
	_TCHAR *			m_szAutomatic;

// Construction
public:
	 CColorCombo();

	int AddColor (COLORREF color);
	void SetStandardColors ();

	inline int SetCurColor (COLORREF rgb) { return SetCurSel (FindStringExact (-1, (LPCTSTR)rgb)); }
	inline COLORREF GetColor (int ix = -1) { return ix == -1 ? GetItemData(GetCurSel()) : GetItemData (ix); }

// Attributes
public:
	enum { rgbAuto = 0x80000000 };	// Represents the "Automatic" selection.	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorCombo)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

	void DrawSelectionChange (CDC * pDC, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawFocusChange (CDC * pDC, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawEntireItem (CDC * pDC, LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CColorCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



// CWindowList window
class CWinList;

class CWindowList : public CListBox
{
// Construction
public:
	CWindowList();

// Attributes
private:
	CWinList *		m_pWinList;

public:
	inline void SetWinList (CWinList * pWinList);


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWindowList();

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) ;

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


inline void CWindowList::SetWinList (CWinList * pWinList)
{
	m_pWinList = pWinList;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFontColorDlg dialog

class CFontColorDlg : public CDlgTab
{
friend	CFCDialogState;
	
DECLARE_DYNAMIC (CFontColorDlg)

	CFCDialogState		m_state;

	CFont				m_SampleFont;
	CRect				m_rcSample;
	BOOL				m_bSampleIsTrueType;
	CString				m_strSample;
	UINT				m_idsSample;

	int					m_nPixPerInchY;

	static int CALLBACK SizeCallBack(CONST ENUMLOGFONT *, CONST NEWTEXTMETRIC *, int, LPARAM);

	void				RegenSizes (BOOL bUpdateState);

// Construction
public:
	CFontColorDlg(CWnd* pParent = NULL);   // standard constructor

	void UpdateSampleFont ();

// Dialog Data
	//{{AFX_DATA(CFontColorDlg)
	enum { IDD = IDD_OPTIONS_FONTCOLOR };
	CWindowList m_lstWindows;
	CStatic		m_stcSample;
	CSizeCombo	m_cmbFontSize;
	CFontNameCombo	m_cmbFontName;
	CListBox	m_lstElements;
	CColorCombo	m_cmbBackColor;
	CColorCombo	m_cmbForeColor;
	//BOOL	m_bProportional;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void CommitTab();


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFontColorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeWindow();
	afx_msg void OnChangeFont();
	afx_msg void OnChangeForeColor();
	afx_msg void OnChangeSize();
	afx_msg void OnChangeElement();
	afx_msg void OnChangeBackColor();
	afx_msg void OnKillFocusSize ();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	afx_msg void OnSysColorChange ();
	//afx_msg void OnCheckProportional();
	afx_msg void OnRestoreDefaults ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __FCDIALOG_H__
