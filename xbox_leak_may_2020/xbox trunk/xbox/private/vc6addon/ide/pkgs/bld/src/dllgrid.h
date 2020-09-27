#ifndef __DLLGRID_H__
#define __DLLGRID_H__

#ifndef __UTILCTRL_H__
#include "utilctrl.h"
#endif

class CDLLGridRow;
class CDLLGridWnd;

enum {COL_PRELOAD , COL_LOCALNAME,  COL_REMOTENAME, WATCH_NUM_COLUMNS /* should be last */};

#ifndef EXPORTED
#define EXPORTED virtual
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLLGridRow 
// A specialized grid row which can live in a DLL like Grid control.

class CDLLGridRow : public CGridControlRow 
{
public:
	inline CDLLGridRow(CDLLGridWnd *pGridWnd, CDLLGridRow *pParent = NULL )
		: CGridControlRow((CGridWnd *)pGridWnd)
	{
		CRect rect(0,0,0,0);
		m_fPreload = TRUE;	m_fDelete = FALSE;
	}

	typedef CGridControlRow  CBaseClass;

// Attributes	
public:
	virtual void GetColumnText(int nColumn, CString& rStr);
	virtual void OnActivate(BOOL bActivate, int nColumn);
	virtual BOOL OnAccept(CWnd *pControlWnd);
	virtual int GetNextControlColumn(int nColumn);
	virtual int GetPrevControlColumn(int nColumn);
	virtual void DrawCell(CDC *pDC, const CRect& cellRect, int nColumn);
	virtual void AdjustForMargins(CRect& rect, int nColumn);
	virtual void ResetSize(CDC *pDC);
	virtual CSize GetCellSize(CDC *pDC, int nColumn) const;
	inline CDLLGridWnd * GetDLLGrid()
	{	return (CDLLGridWnd *)GetGrid( ); }

	inline int GetCheck() { return m_fPreload ? 1 : 0; }
	void SetCheck(int nCheck);

	// Place holder for function to call if some external
	// state change should force the GridRow to update itself.
	virtual void Update( );
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);

	BOOL m_fPreload;
	CString m_strLocalName;
	CString m_strRemoteName;
	BOOL m_fDelete;
};


/////////////////////////////////////////////////////////////////////////////
// CDLLGridWnd window
// A Grid Control with a DLL like hierarcichal structure.

class CDLLGridWnd : public CGridControlWnd
{
// Construction
public:
	CDLLGridWnd (int nColumns = 3, BOOL bDisplayCaption = TRUE);
	~CDLLGridWnd();
	typedef CGridControlWnd CBaseClass;

// Attributes
public:
	BOOL m_fEnabled;
	BOOL m_fShowRemoteCol;

// 
// Operations
public:
	long GetMinColumnWidth(CString strCaption);
	EXPORTED CDLLGridRow * GetRow(int nIndex = -1);

	inline CDLLGridRow *GetRowAt(POSITION pos)
	{	return (CDLLGridRow *)CGridControlWnd::GetRowAt(pos); }
	inline CDLLGridRow *GetLastRow( )	// returns the last row (the new row that can't be deleted)
	{	return GetRowAt(m_rows.GetTailPosition()); }

	CDLLGridRow *GetCurSel( );	// returns the row iff there is exactly 1 item selected, NULL otherwiase
	BOOL SetCurSel(int nSelect);

	virtual BOOL ProcessKeyboard(MSG* pMsg, BOOL bPreTrans = FALSE);
	void DoDelete();
	void DoToggleCheck();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLLGridWnd)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg UINT OnGetDlgCode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_VIRTUAL

protected:
	BOOL m_bNewRow:1;

protected:

	// True if some row in the grid is selected.
	BOOL HasASelection() const
	{	return (GetHeadSelPosition() != NULL);	}

public:	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg void OnGridNew();			//  New button pressed 
	afx_msg void OnGridDelete();		//  Delete button pressed 
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CDLLGridWnd)mber functions here.
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

#endif // __DLLGRID_H__
