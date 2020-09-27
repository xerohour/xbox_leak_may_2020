
#ifndef __TREEGRID_H__
#define __TREEGRID_H__

// varswnd.h : header file
//

#include "utilctrl.h"

class CTreeGridRow;

class CTreeGridWnd;

typedef CTypedPtrArray<CPtrArray, CTreeGridRow *> CTreeGridRowArray;

// Private messages 
#define WM_DISMISS_CONTROL  (WM_USER + 10)	 // send to dismiss control
#define WM_WARN_AND_EXPAND	(WM_USER + 11)	 // warn about expansion time and then expand.

/////////////////////////////////////////////////////////////////////////////
// CTreeGridRow 
// A specialized grid row which can live in a Tree like Grid control.



class CTreeGridRow : public CGridControlRow 
{
public:
	CTreeGridRow(CTreeGridWnd *, CTreeGridRow * = NULL );
	virtual ~CTreeGridRow();
	typedef CGridControlRow  CBaseClass;

// Attributes	
public:
	virtual BOOL IsExpandable( ) const = 0;
	virtual BOOL GetChildren(CTreeGridRowArray& ) = 0;
	virtual DWORD GetChildCount() = 0;

	// Only the special row for adding new entries should 
	// return TRUE on this function call.
	virtual BOOL IsNewRow( ) const; 

	// Place holders for function to call if some external
	// state change should force the GridRow to update itself.
	virtual void Update( );
	virtual void Restart( );

	CTreeGridRow * m_pParent;
	int m_nLevels;
	int m_nFlags;
};

class CTGNewRow : public CTreeGridRow 
{
public:
	// Indicates what column should have the fuzzy border and allow
	// editing. Currently only supports one column editing.
	CTGNewRow(CTreeGridWnd *, int editColumn);
	~CTGNewRow( );
	typedef CTreeGridRow CBaseClass;
public:
	virtual BOOL IsExpandable() const;
	virtual BOOL GetChildren(CTreeGridRowArray&);
	virtual DWORD GetChildCount();
		
	virtual BOOL IsNewRow() const;

	virtual void GetColumnText(int nColumn, CString& rStr);
	virtual void OnActivate(BOOL bActivate, int nColumn);
	virtual BOOL OnAccept(CWnd *pControlWnd);
	virtual void DrawCell(CDC *pDC, const CRect& cellRect, int nColumn);
	virtual void ResetSize(CDC *pDC);
	virtual CSize GetCellSize(CDC *pDC, int nColumn) const;
	virtual int GetNextControlColumn(int nColumn);
	virtual int GetPrevControlColumn(int nColumn);
protected:
	int m_editColumn;
};
	
			
// Flags used by the treegrid control.
// plagiarized from utilctrl.h
#define TGF_NONE				0x0000
#define TGF_LASTLEVELENTRY		0x0001
#define TGF_EXPANDED				0x0002
#define TGF_DISABLED				0x0004
#define TGF_EXPANDABLE			0x0008
#define TGF_NOT_EXPANDABLE		0x0020
#define TGF_DIRTY_CONTENT		0x0040
#define TGF_DIRTY_ITEM			0x0010
#define TGF_DIRTY_DELETE		0x0080
#define TGF_DROPTARGET           0x0100
#define TGF_DRAGSOURCE           0x0200
#define TGF_MARK_FOR_DELETE		 0x0400

/////////////////////////////////////////////////////////////////////////////
// CTreeGridWnd window
// A Grid Control with a tree like hierarcichal structure.

class CTreeGridWnd : public CGridControlWnd
{
	DECLARE_DYNAMIC(CTreeGridWnd);

// Construction
public:
	CTreeGridWnd (int nColumns, BOOL bDisplayCaption = TRUE);
	virtual ~CTreeGridWnd();
	typedef CGridControlWnd CBaseClass;

// Attributes
public:
	// is a particular row expanded.    
	BOOL IsExpanded(CTreeGridRow *);

// 
// Operations
public:
	virtual void Expand(CTreeGridRow *, BOOL bRedraw = TRUE);		
	virtual void Collapse(CTreeGridRow *, BOOL bRedraw = TRUE);
	virtual BOOL ToggleExpansion(CTreeGridRow *pRow);

	// Control handling
	virtual void ActivatePrevControl();
	virtual void ActivateNextControl();
	virtual BOOL ProcessKeyboard(MSG *pMsg, BOOL bPreTrans = FALSE);

	virtual BOOL Refresh(CTreeGridRow *, int&);	 
	virtual void RefreshAllRoots( );


	inline CTreeGridRow *GetRowAt(POSITION pos) const
	{	return (CTreeGridRow *)CBaseClass::GetRowAt(pos); }

	CTreeGridRow *GetCurSel( ) const;	// returns the row iff there is exactly 
					    				// 1 item selected, NULL otherwiase

	// The New Row is a special row, which allows to user to enter
	// new values into the grid. Functions related to the new row.
	virtual CTGNewRow * CreateNewRow( );
	virtual void OnEnterNew (CString str);
	virtual void EnableNewRow(BOOL bEnable = TRUE);

	CTreeGridRow * GetRow(int nIndex = -1);
	CTreeGridRow * GetChildRow(CTreeGridRow *, int nIndex);

	virtual void DirtyRow(CTreeGridRow *, int tf);

	// Warn if # if children is greater than count before expanding.
	virtual BOOL GetExpansionWarnCount() { return 1000; }
	// Call if you want to expand a given row.
	virtual BOOL InsertLevel(CTreeGridRow *pParent, BOOL bRedraw = TRUE);

	// Call  if you want to insert a row at level 1.
	virtual BOOL InsertTopLevelRow(CTreeGridRow *pRow, BOOL bRedraw = TRUE);
	virtual BOOL DeleteTopLevelRow(CTreeGridRow *pRow, BOOL bRedraw = TRUE);
	virtual BOOL DeleteSelectedRows( );

	virtual void FreeAllData( );
	virtual void InvalidateGrid();
			
	void InsertRowAfter(CTreeGridRow *pRow, CTreeGridRow *pRowAfter,
			 BOOL bRedraw = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeGridWnd)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

public:
	BOOL m_bNewRow:1;
	BOOL m_bForceAccept:1; 

protected:

	// These are really grid primitives that are implemented here.
	void InsertMultipleRows(CTreeGridRowArray *rgpRow, CTreeGridRow *pRowAfter,
			BOOL bRedraw = TRUE);
	// True if some row in the grid is selected.
	BOOL HasASelection() const
	{	return (GetHeadSelPosition() != NULL);	}

	void ExpandHelper(CTreeGridRow *pParent, BOOL bRedraw, BOOL bWarn);

public:	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeGridWnd)mber functions here.
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnDismissControl(WPARAM, LPARAM);
	afx_msg LRESULT OnWarnAndExpand(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

	
/////////////////////////////////////////////////////////////////////////////

#endif // __TREEGRID_H__
