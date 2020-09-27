#ifndef __VARCHOICESDLG_H__
#define __VARCHOICESDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// VarChoicesDlg.h : header file
//

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "resource.h"

class CVarChoices;
class CVarChoicesCtrl;


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn window

class CVarChoicesFlagsBtn : public CButton
{
friend class CVarChoicesDlg;

// Construction
public:
	CVarChoicesFlagsBtn();

// Attributes
public:
	CVarChoicesDlg*	m_pVarChoicesDlg;

// Operations
protected:
	void IMA_DrawButtonText( CDC* pDC, CRect* pRect, short nRow, short nColumn );
	void DM_DrawButtonText( CDC* pDC, CRect* pRect, short nRow, short nColumn );
	void DrawOneButton( CDC* pDC, short nRow, short nColumn, BOOL fDrawOne );
	void DrawRowOfButtons( CDC* pDC, short nRow );
	void DrawAllButtons( CDC* pDC );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarChoicesFlagsBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVarChoicesFlagsBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVarChoicesFlagsBtn)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn window

class CVarChoicesRowsBtn : public CButton
{
friend class CVarChoicesDlg;

// Construction
public:
	CVarChoicesRowsBtn();

// Attributes
public:
	CVarChoicesDlg*	m_pVarChoicesDlg;

// Operations
protected:
	void DrawOneButton( CDC* pDC, short nRow );
	void DrawAllButtons( CDC* pDC );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarChoicesRowsBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVarChoicesRowsBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVarChoicesRowsBtn)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg window

class CVarChoicesDlg : public CFormView
{
friend class CVarChoices;
friend class CVarChoicesFlagsBtn;
friend class CVarChoicesRowsBtn;

public:
	CVarChoicesDlg();
	virtual ~CVarChoicesDlg();

	// Additional methods
protected:
    void RefreshRowOfButtons( short nRow );

	BOOL CreateBtnBitmaps();
	void PrepareXPosArray();
	void PrepareTextArrays();
	void PrepareColumnHeadings();

	void SyncPattern();
	short GetRowType( short nRow );
    short GetNbrColumns( short nRow );
    short GetBtnWidth( short nRow, short nColumn );
    short FindRow( int nYPos );
    short FindColumn( short nRow, int nXPos );
    void SetBit( short nRow, short nColumn );
    void SetSelectedBits( short nState );
    void SelectRow( short nRow, BOOL fSelect );
    void UnselectAllButtons( BOOL fRedraw );
	BOOL RowsAreSelected();
	BOOL BtnsAreSelected();
	BOOL IsBtnSelected( short nRow, short nColumn );
	BOOL IsRowSelected( short nRow );
	BOOL IsRowDisabled( short nRow );
	BOOL IsBitOn( short nRow, short nColumn );
	void CaptureMouse( CWnd* pWnd );
	void ReleaseMouse( CWnd* pWnd );

public:
	void RefreshControls();
	BOOL HandleKeyDown( MSG* pMsg );
	void OnUpdateEditCopy( CCmdUI* pCmdUI );
	void OnEditCopy();
	void OnUpdateEditPaste( CCmdUI* pCmdUI );
	void OnEditPaste();
	BOOL OnViewProperties();

protected:
	DECLARE_DYNCREATE(CVarChoicesDlg)

// Form Data
public:
	//{{AFX_DATA(CVarChoicesDlg)
	enum { IDD = IDD_DLG_VARCHOICES };
	//}}AFX_DATA

// Member variables
private:
	BOOL					m_fRowRightMenu;	// May be recipient of right menu command id
	CFont*					m_pFont;
	CFont*					m_pFontBold;
	CFont*					m_pFontItalics;
	CFont*					m_pFontItalicsBold;
	bool					m_fDirty;

	CVarChoicesRowsBtn		m_btnRows;			// Subclasses IDC_SELECT_ROWS
	CVarChoicesFlagsBtn		m_btnFlags;			// Subclasses IDC_FLAGS

public:
	CVarChoicesCtrl*		m_pVarChoicesCtrl;
	CVarChoices*			m_pVarChoices;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarChoicesDlg)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CVarChoicesDlg)
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __VARCHOICESDLG_H__
