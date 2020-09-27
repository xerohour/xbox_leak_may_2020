// dbggrid.h : header file
//
#ifndef __DBGGRID_H__
#define __DBGGRID_H__

/////////////////////////////////////////////////////////////////////////////
// CDbgGridWnd view

// This class is the debugger specific sub-class of CTreeGrid.
// It handles things like dockableness, refresh, clipboard, which are common
// to all the debugger windows.

class CPersistWatchTab;	 // Forward declaration.
class CPersistVarsTab;	 // Forward declaration.
class CWatchRow;		 // Forward declaration.
class CTM;			// forward declaration.
class CBaseVarsView;

class CDragInfo; //forward decl

class CWRInfo 	// Information about watch rows.
{
public:
	CString strName;
	CString strValue;
	BOOL bExpanded;
};

void AFXAPI ConstructElements(CWRInfo * pWRInfo, int nCount);
void AFXAPI DestructElements(CWRInfo * pWRInfo, int nCount);

typedef CArray<CString, CString&> CStrArray;
typedef CArray<CWRInfo, CWRInfo&> CWRInfoArray;

class CDbgGridWnd : public CTreeGridWnd
{
	DECLARE_DYNAMIC(CDbgGridWnd);

public:
	CDbgGridWnd(CString wndName, CBaseVarsView * pView = NULL, BOOL bEditable = FALSE);
	virtual ~CDbgGridWnd();
	typedef CTreeGridWnd  CBaseClass;

// Attributes
public:
	const CString& GetTabName( ) { return m_strTabName; }

	// Values are always editable, this function only tells you if
	// a) the names can be edited.
	// b) if the rows can be deleted from this window by user action.
	BOOL  IsEditable( ) const { return m_bEditable; }
	CBaseVarsView * GetView() const { return m_pView; }
	int GetNameColWidth() const;

	// Color handling.
	inline void SetChangeHighlightColors(COLORREF crText, COLORREF crBkground)
	{	m_crChangedText = crText; m_crChangedBk = crBkground; }
	inline void GetChangeHighlightColors(COLORREF &crText, COLORREF &crBkground)
	{	crText = m_crChangedText; crBkground = m_crChangedBk; }
	
	BOOL CanDropText() { return m_bEditable && DebuggeeAlive() && !DebuggeeRunning(); }
	BOOL CanMoveText() { return FALSE; }
	void GetDragSlopRect( const CPoint& pt, CRect & rc );

// Operations
public:
	// Persistence related stuff.
	virtual void RememberSettings(CPersistWatchTab&);
	virtual void RememberSettings(CPersistVarsTab&);
	virtual void RestoreSettings(CPersistWatchTab&,int nColumnWidth);
	virtual void RestoreSettings(CPersistVarsTab&,int nColumnWidth);
	
	virtual int  GetDefaultNameWidth();
	virtual void MakeWindowAlive(BOOL bAlive, int nNameColWidth);

	virtual CTGNewRow *CreateNewRow( );
	virtual void OnEnterNew(CString);

	virtual BOOL AddNewWatch(CString);
	virtual BOOL AddNewWatch(CString, CWatchRow*&);

	virtual void Expand(CTreeGridRow *, BOOL bRedraw = TRUE);
	virtual void Collapse(CTreeGridRow *, BOOL bRedraw = TRUE);
	virtual BOOL ProcessKeyboard(MSG *pMsg, BOOL bPreTrans = FALSE);

	virtual BOOL InsertTopLevelRow(CTreeGridRow *, BOOL bRedraw = TRUE);
	virtual BOOL DeleteTopLevelRow(CTreeGridRow *, BOOL bRedraw = TRUE);

	virtual void ClearCurEntries( );
	virtual void DirtyRow(CTreeGridRow *pRow, int tf);	
	// expansion persistence related functions.
	BOOL IsStrInArray(CStrArray& rgStr, const CString&);
	void RememberPrevState(CWRInfoArray& rgWRInfo);
	void RefreshUsingPrevState(CWRInfoArray& rgWRInfo);

		
	// Clipboard related functions.
	virtual HANDLE CFTextHandle( );
	virtual BOOL ImportWatchFromBuffer(LPCSTR p, DWORD size);

public:
	virtual CTM * GetSelectedTM();
	virtual void ShowContextPopupMenu(POPDESC* ppop, CPoint pt);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbgGridWnd)
	protected:
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// CTreeGridWnd override
	virtual BOOL BeginDrag(UINT nFlags, const CPoint& point);

// Implementation
protected:
	CString m_strTabName;
	CBaseVarsView * m_pView; // Can be NULL if grid is not contained within view.
	COLORREF m_crChangedText ; // colors to be used to do change highlighting.
	COLORREF  m_crChangedBk;

	BOOL m_bEditable:1;
	// Only those windows that are alive track the values from the debugger.
	BOOL m_bAlive:1;	
	BOOL m_bDragging:1;
	BOOL m_bDockable:1;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDbgGridWnd)
	afx_msg void OnEditClear( );
	afx_msg void OnEditCopy( );
	afx_msg void OnEditCut( );
	afx_msg void OnEditPaste( );
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	//}}AFX_MSG
	virtual LRESULT OnUpdateDebug(WPARAM, LPARAM);
	virtual LRESULT OnClearDebug(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CLocalsWnd window

class CLocalsWnd : public CDbgGridWnd
{
// Construction
public:
	CLocalsWnd(CBaseVarsView * pView);
	virtual ~CLocalsWnd();
	typedef CDbgGridWnd CBaseClass;
		
// Attributes
public:
		
// Operations
public:
	void UpdateEntries( );
   	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocalsWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bContextValid :1	;
	BOOL m_bProlog:1;

	CXF	 m_lastCxf;

	// Generated message map functions
protected:
	//{{AFX_MSG(CLocalsWnd)
	//}}AFX_MSG
	virtual LRESULT OnUpdateDebug(WPARAM, LPARAM);
	virtual LRESULT OnClearDebug(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CThisWnd window

class CThisWnd : public CDbgGridWnd
{
// Construction
public:
	CThisWnd(CBaseVarsView * pView);
	virtual ~CThisWnd();
	typedef CDbgGridWnd CBaseClass;

// Attributes
public:

// Operations
public:
	void ExpandBaseClasses(CWatchRow *);
	void UpdateThis();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThisWnd)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CThisWnd)
	//}}AFX_MSG
	virtual LRESULT OnUpdateDebug(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // __DBGGRID_H__
