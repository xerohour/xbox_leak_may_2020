class CPersistWatchTab;

void AFXAPI ConstructElements(CPersistWatchTab*, int nCount);
void AFXAPI DestructElements(CPersistWatchTab *, int nCount);
void AFXAPI SerializeElements(CArchive& ar, CPersistWatchTab *, int nCount);

class CBaseVarsView; // Forward declaration.
class CVarsDialogBar;

class CVarsPropPage : public CSlobPage
{
public:
	DECLARE_IDE_CONTROL_MAP()
};

class CVarsSlob : public CSlob
{
	DECLARE_DYNCREATE(CVarsSlob)
public:
	CVarsSlob();
	~CVarsSlob();

	virtual GPT  GetStrProp(UINT nPropId, CString& val);

	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);
	
	static CString  s_strPropCaption;
	static CVarsPropPage s_varsPropPage;

	CBaseVarsView * m_pView;

};


/////////////////////////////////////////////////////////////////////////////
// CBaseVarsView view
class CBaseVarsView : public CDockablePartView
{
protected:	// so we don't create an object of this class directly.
	CBaseVarsView(UINT, UINT);
	DECLARE_DYNAMIC(CBaseVarsView)
	typedef CDockablePartView CBaseClass; 	// another name for our base class.

// Attributes
public:
	inline CDbgGridWnd * GetActiveGrid() const;
 	UINT m_nIDWnd;
	UINT m_nIDPacket;
	CFontInfo * m_pFontInfo;
	CFont	m_font;

// Operations
public:
	virtual BOOL EnsureWindow(); // Creates window if it doesn't exist currently.
	virtual void UpdateTabs( );
	virtual void ResetWidgetScrollBar( );
	virtual void OnFontColorChange();

	// call this function when the grid rows selection status
	// changes in any fashion.
	virtual void OnGridRowSelectionChange( );

	inline void ShowSelection(BOOL bShow);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseVarsView)
	public:
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

protected:
	BOOL m_bHasVScroll:1;
	BOOL m_bFirstNonZeroSize:1;

	CWidgetScrollBar m_widgetScroller;
	int c_nSplitterPref;
	UINT m_iNameColWidth;

	
	CVarsSlob m_slob;	
	CMultiSlob m_multiSlob;
	// Slob added to the MultiSlob to get the multiple selection dialog.
	CSlob m_fakeSlob;

	// Sub-classes should add their windows to this array.
	CTypedPtrArray <CObArray, CDbgGridWnd *> m_arrChildWnd;
	int m_curIndex ; 	// Which child is currently active.
			
	// OLE Drag/Drop
public:
	virtual BOOL		CanDropText();
	virtual BOOL		CanMoveText();
	virtual DROPEFFECT	OnDropEx( COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point );
	virtual DROPEFFECT	OnDragEnter( COleDataObject* pDataObject, DWORD grfKeyState, CPoint point );
	virtual DROPEFFECT	OnDragOver( COleDataObject* pDataObject, DWORD grfKeyState, CPoint point );
	virtual void		OnDragLeave();
	// utilities
	BOOL				DoDrag(CPoint point);
	BOOL				InitDataSource(COleDataSource& data, const CPoint & point);
	BOOL				InsertFromDataObject(COleDataObject * data, const CPoint & point);
	BOOL				IsDragActive() { return (m_pDragInfo != NULL); }
	// Drag & Drop implementation
private:
	COleDropTarget	m_DropTarget;
	CDragInfo *		m_pDragInfo;

// Implementation
protected:
	virtual ~CBaseVarsView();
	virtual void RecalcLayout();

    virtual CSlob* GetSelection();
    virtual CSlob* GetEditSlob();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBaseVarsView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnChangeTab(WPARAM, LPARAM);
	afx_msg  LRESULT OnUpdateDebug(WPARAM, LPARAM);	
	afx_msg  LRESULT OnClearDebug(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

inline CDbgGridWnd * CBaseVarsView::GetActiveGrid( ) const
{
	if ( m_curIndex == -1 )
		return NULL;
	else
		return (m_arrChildWnd[m_curIndex]);
}

inline void  CBaseVarsView::ShowSelection( BOOL bShow )
{
	// tell each of the child windows to show the selection
	for (int i = 0; i < m_arrChildWnd.GetSize(); i++)
		m_arrChildWnd[i]->ShowSelection(bShow);
}	

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CGridWatchView view

class CGridWatchView : public CBaseVarsView
{
public:
	CGridWatchView();
	virtual ~CGridWatchView();

	DECLARE_DYNCREATE(CGridWatchView)
	typedef CBaseVarsView CBaseClass;
	static BOOL EnsureWatchWindow();

// Attributes
public:


// Operations
public:
	virtual void ShowContextPopupMenu(CPoint pt);
	virtual BOOL AddNewWatch(CString);
	virtual DWORD GetHelpID( )	{ return HID_WND_WATCH ; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridWatchView)
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGridWatchView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	friend class CDebugPackage;
	// Font selection info.
	static UINT g_cRef;		// reference count
	static CFontInfo *g_pFontInfo;	
};




/////////////////////////////////////////////////////////////////////////////

class CVarsCombo : public CDockCombo
{
// Construction
public:
	CVarsCombo();

// Attributes
public:
	CVarsDialogBar *m_pToolBar;	
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarsCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVarsCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVarsCombo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////
// CVarsDialogBar
//
// toolbar wrapper for the variables window...
//

class CVarsDialogBar : public C3dDialogBar
{
public:
	CVarsCombo		m_list;
	BOOL			m_fUpdateListbox;
	BOOL			m_fDoUpdate;
	CSize			m_sizeBar;

	virtual BOOL OnInitDialog();				// dialog is coming up, init items
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

/////////////////////////////////////////////////////////////////////////////
// CVarsView view

class CVarsView : public CBaseVarsView
{
public:
	CVarsView();
	virtual ~CVarsView();
	DECLARE_DYNCREATE(CVarsView)

	typedef CBaseVarsView CBaseClass;

// Attributes
public:
	enum { indexLocals = 0 };

// Operations
public:
	virtual void ShowContextPopupMenu(CPoint pt);
	virtual void RecalcLayout( );
	virtual DWORD GetHelpID( )	{ return HID_WND_VARS ; }

	void ResetCallStack( );
	void UpdateCallStack( );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarsView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	protected:
	virtual BOOL OnNotify(WPARAM, LPARAM, LRESULT *);

// Implementation
protected:
	CVarsDialogBar m_toolbar;

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CVarsView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnToggleVarsBar();
	afx_msg void OnUpdateToggleVarsBar(CCmdUI *);
	afx_msg void OnDropDown();
	afx_msg void OnCloseUp();
	afx_msg  LRESULT OnUpdateDebug(WPARAM, LPARAM);	
	afx_msg  LRESULT OnFreezeData(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	friend class CDebugPackage;
	// Font selection info.
	static UINT g_cRef;		// reference count
	static CFontInfo *g_pFontInfo;	

};

/////////////////////////////////////////////////////////////////////////////

// Perisistence information for the watch window.

class CPersistWatchTab : public CObject
{
	DECLARE_SERIAL(CPersistWatchTab);
public:
	CPersistWatchTab ( );
	CPersistWatchTab (const CString& strTabName);
	const CPersistWatchTab& operator=(const CPersistWatchTab&);

	inline CString& GetTabName();
	void ChangeTabName(const CString& str);
	inline CString& GetWatchName(int index);
	inline int GetWatchCount();
	void AddWatchName(CString& str);
	void ClearAllWatches( );
	void InitDefault( );
			
	CString m_strTabName;
	CStringArray m_arrWatchNames;

	virtual void Serialize(CArchive& ar);
};

CString& CPersistWatchTab::GetTabName( )
{
	return m_strTabName;
}

CString& CPersistWatchTab::GetWatchName(int index)
{
	return m_arrWatchNames[index];
}

int CPersistWatchTab::GetWatchCount( )
{
	return m_arrWatchNames.GetSize();
}

class CPersistWatch : public CObject
{
	DECLARE_SERIAL(CPersistWatch);
	
public:
	CPersistWatch( );

	void InitDefault();
	void ClearAll();

	CPersistWatchTab&  GetWatchTab(int);
	void AddWatchTab(CPersistWatchTab& );
	void AddWatchTab(const CString&);
	int GetTabCount();

	inline void SetActiveTab(int index);
	inline int GetActiveTabIndex();

	int m_curIndex;
	int m_nSplitterPref;
	CArray<CPersistWatchTab, CPersistWatchTab&>	m_arrWatchTab;		

	virtual void Serialize(CArchive& ar);
};


void CPersistWatch::SetActiveTab(int index)
{
	m_curIndex = index;
}

int CPersistWatch::GetActiveTabIndex()
{
	return m_curIndex;
}

/////////////////////////////////////////////////////////////////////////
// Persistence information for the variables window.

class CPersistVarsTab : public CObject
{
	DECLARE_SERIAL(CPersistVarsTab);
public:
	CPersistVarsTab ( );
	void InitDefault();

	virtual void Serialize(CArchive& ar);
};

class CPersistVariables : public CObject
{
	DECLARE_SERIAL(CPersistVariables)
public:
	CPersistVariables();
	void InitDefault();

	CPersistVarsTab m_locals;
	CPersistVarsTab m_auto ;
	CPersistVarsTab m_this ;

	inline void SetActiveTab(int index);
	inline int GetActiveTabIndex();

	int m_curIndex;
	int m_nSplitterPref;

	virtual void Serialize(CArchive& ar);
};


void CPersistVariables::SetActiveTab(int index)
{
	m_curIndex = index;
}

int CPersistVariables::GetActiveTabIndex()
{
	return m_curIndex;
}


extern CGridWatchView * g_pWatchView;
extern CVarsView * g_pVarsView;
extern CPersistWatch g_persistWatch;
extern CPersistVariables g_persistVars;
