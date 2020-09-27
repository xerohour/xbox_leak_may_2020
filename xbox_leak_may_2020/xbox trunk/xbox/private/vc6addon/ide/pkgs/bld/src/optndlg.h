//
// CProjOptionsDlg
//
// Project Options 'mondo' dialog for setting options
// across one or more targets, and none or more files
// in those targets.
//
// [matthewt]
//				   

#ifndef _INCLUDE_OPTIONDLG_H										
#define _INCLUDE_OPTIONDLG_H

#ifndef _SUSHI_PROJECT
// If you are including this in a file that is being compiled under
// WIN16, either make sure everything compiles under WIN16 and remove
// this error, or include conditional on _SUSHI_PROJECT.
//
#error This code may not be Win 3.1 compatible.
#endif

#include "resource.h"	// for the resource IDs
#include "prjoptn.h"	// our CProxySlob etc.
#include "project.h"	// ConfigurationRecord, CProjItem
#include <utilctrl.h>    // tree control
#include <dlgbase.h>	// C3dDialog

// testing hooks to allow a client
// to select items in the project tree control in
// this dialog using a SendMessage where
// WPARAM = type of node to select (enum)
// LPARAM = title of node to select (string atom)
// n.b. the selection is within the oontext
// of the current selection,
// ie. SendMessage(hwnd, WM_SELECT_ITEM, PRJW_FILE, 'foo.c')
// will select foo.c in the currently selected group.
#define WM_SELECT_ITEM	(WM_USER + 0)
#define PRJW_TARGET	0 
#define PRJW_GROUP	1
#define PRJW_FILE	2

class CNodeState
{
public:
	CNodeState() {}
	CNodeState(TCHAR * pchTitle, int nLevel, BOOL fExpanded, BOOL fSelected)
	{
		m_str = pchTitle; m_nLevel = nLevel;
		m_fExpanded = fExpanded; m_fSelected = fSelected;
	}

	CString m_str;
	int m_nLevel;
	BOOL m_fExpanded;
	BOOL m_fSelected;
};

class COptionTreeStore
{
public:
	COptionTreeStore() {};
	~COptionTreeStore() {Clear();}

	// clear the state (destroy the node states)
	void Clear();
	BOOL Serialize(CArchive & ar);
	BOOL Deserialize(CArchive & ar);

	CPtrList m_lstStates;
};

class COptionTreeCtl : public CTreeCtl
{
friend class CProject;

	DECLARE_DYNCREATE(COptionTreeCtl)
	DECLARE_MESSAGE_MAP()

public:
	// our default CTOR
	COptionTreeCtl() : CTreeCtl(TRUE, TRUE)
	{
		// cache a pointer to the current project
		m_pProject = g_pActiveProject; ASSERT(m_pProject != (CProject *)NULL);
		m_fIgnoreSelChange = FALSE; m_rgLastGoodSel = (int *)NULL;
	}

	// our default DTOR
	virtual ~COptionTreeCtl()
	{
		if (m_rgLastGoodSel != (int *)NULL)	delete [] m_rgLastGoodSel;
	}

	// our creation method (this overrides the super-class)
	BOOL Create(CWnd *pParentWnd, CRect rect);

	// process any messages before the tabbed-dialog if we have the focus
	// eg. VK_KEYDOWN should expand/contract the current selection, and
	// not invoke the default button command
 	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// retrieve/save our state
	void RetrieveTreeLevel(POSITION & pos, int iBaseIndex, int iBaseLevel);
	void RetrieveTreeState();
	void SaveTreeState();

	// is this CProjItem valid for display in the options tree-control?
	BOOL IsValidOptTreeItem(CProjItem * pItem, CString & strItemName);

	// a project item's appearance in the 'view' has changed
	__inline BOOL InterestedInPropChange(UINT uHint, CProjItem * pProjItem) {return uHint == P_ItemExcludedFromBuild || uHint == P_TargetName || uHint == P_ItemIgnoreDefaultTool || uHint == P_ItemTools;}
	void ProjItemChanged(CSlob * pProjItem, UINT uHint);

	// refresh so that the tree control shows the 
	// targets in the currently active project as root nodes
	// (can copy the state of another tree N.B. nodes must be identical)
	int InsertTargetNode(CProject * pProject, const TCHAR * szTarget);
	void RefreshTargetNodes();
#if 1
	void CopyTreeCtlState(CSlob * pSelection = NULL);
#else
	void CopyTreeCtlState(CTreeCtl * pTreeCtlToCopy = NULL);
#endif

	void FreeTargetNode(OptTreeNode *);		// free up a particular target node (plus children)
	void FreeAllOptTreeNodes();				// free-up the memory used by all OptTreeNodes

	// Find a node in the settings tree that corresponds to a particular
	// item in the real project tree
	int FindTargetNode(const CString& strTargetName);

	__inline BOOL HasFocus()
	{
		CWnd * pWndFocus = GetFocus();
		return pWndFocus != (CWnd *)NULL && GetListBox()->m_hWnd == pWndFocus->m_hWnd;  
	}

	// tree-state retrieval/store from an archive
	BOOL m_fDoStateStore;
	__inline static BOOL LoadOptTreeStore(CArchive & archive) {return sm_TreeState.Deserialize(archive);}
	__inline static BOOL SaveOptTreeStore(CArchive & archive) {return sm_TreeState.Serialize(archive);}

	// save/restore the tree-control state to memory
	void RememberSel();	// restore
	void NoteSel();		// save

protected:
	//{{AFX_MSG(COptionTreeCtl)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnDestroy();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	//}}AFX_MSG
	afx_msg void OnLbSetFocus();
	afx_msg void OnLbKillFocus();
	afx_msg void OnLbSelChange();
	BOOL m_fIgnoreSelChange;				// ignore the selection change?
	int * m_rgLastGoodSel;					// last known good multiple selection
	int m_cLastGoodSel;						// # in this selection

	void GetTreeCtlSelection(CTreeCtl * pTreeCtl, INT * & rgInt, int & nSelItems);
	
	BOOL NodeIsExpandable(CNode *);			// is this a container node?
	void GetNodeTitle(CNode *, CString &);	// get the title text for this node
	void GetDisplayText(CNode *, CString &);// get the display text for this node
	BOOL InsertLevel(CNode *, int, BOOL);	// impl. of the retrieve nodes fn
	void OnSelect(int);						// item has been selected
	int  OnKey(UINT);						// used to trap user input
											// Overide so we can draw single items:
	void DrawNodeGlyph(CDC* pDC, CNode* pNode, BOOL bHighlight, CPoint pt);
	int CompareData(const CNode* pNode1, const CNode* pNode2);
	void InvalidateNode (int iNode);

	CPtrList m_ptrTrgNodes;					// list of target nodes in memory

	// tree-state store
 	static COptionTreeStore sm_TreeState;	// our 'remembered' tree state
	
private:
	CProject * m_pProject;					// cached pointer to project
};

// sizes of 'mondo' dialog frame controls
#define CX_TREE_BORDER	8
#define CY_TREE_BORDER	8
#define CX_TREECTL	240
#define CP_OUTER_BORDER	4

// the 'mondo' dialog frame
class CNoPropsDlg : public CDialog
{
public:
	BOOL Create(CWnd * pWndParent = (CWnd *)NULL)
	{
		return CDialog::Create(IDD_NO_CMN_OPTIONS, pWndParent);
	}
};

class CProjOptionsDlg : public CTabbedDialog
{
	DECLARE_DYNAMIC(CProjOptionsDlg)

	friend class COptionTreeCtl;

public:
	CProjOptionsDlg(CWnd * pwndParent = NULL, int iSelectTab = -1) :
		CTabbedDialog(IDS_PROJECT_OPTIONS, pwndParent, iSelectTab)
	{
        // FUTURE (colint): Fix to work with new build view
		// m_pProjTreeCtlToCopy = pProjTreeCtlToCopy;

		m_pSelectionSlob = NULL;

		m_fIgnoreSelChange = FALSE;

		// cache a pointer to the current project
		m_pProject = g_pActiveProject; ASSERT(m_pProject != (CProject *)NULL);
		m_idOnOKMsg = (UINT)-1;	// no OnOK() by default 

		m_nCurTargetSel = CB_ERR;
		m_bDirtyTabs = FALSE;
	}

	// handle the commit model for the tool options
	void ExitCleanup();
	void OnOK();
	void OnCancel();

	void SetSelection(CSlob * pSelection) { m_pSelectionSlob = pSelection; }

	// OnOK message hook hack for pages
	__inline void SetOnOKMessage(UINT idMsg) {m_idOnOKMsg = idMsg;}

	// validation
	__inline BOOL ValidateCurrent()
	{	return m_nTabCur == -1 || GetTab(m_nTabCur)->ValidateTab();}
	
	// handle tab selection changes
	void OnSelectTab(int nTab);

	CButton* GetButtonFromIndex(int index);

	// notification tree control selection has changed
	void OnTreeCtlSelChange();

	// retrieve into our cache the current set of option
	// handlers to use
	void CacheOptionHandlers();

	// update our tab pages to to show only tabs
	// relevant to the current selection in the 
	// option tree control
	BOOL UpdateOptionTabs();

	// update our proxy slob (inform of the selection change?)
	BOOL UpdateProxySlob(BOOL fInform = TRUE);
	void ValidateContent(OptTreeNode * pNode);
	void AddContent(OptTreeNode * pNode, CPtrList & lstSelectedNodes);
	BOOL UpdateConfigsFromCombo();
	void UpdateConfigsFromDlg();
	void RefreshTargets();

	// have any of the project props. changed?
	BOOL FProjectChanged(UINT idSrcBag, UINT idDestBag);

	// perform an operation on bags over the whole project (ie. all the items)
	// o do the 'copy clone->current' and 'copy current->clone' snapshots
	// o do 'clear clone' bags
	// o do 'inform of diffs' operation
	typedef enum {BO_Copy = 0x1, BO_Inform = 0x2, BO_Clear = 0x4, BO_Same = 0x8, BO_UseDest = 0x10, BO_PrivateSpecial = 0x20} BagOp;
	BOOL DoBagOpOverProject(CProject * pProject, UINT idSrcBag, UINT idDestBag, UINT bagop = BO_Copy);

	// remember the current state of our tool option tab categories
	void RememberCategories();

	virtual void GetMargins(CRect & rect);
	virtual void GetTabSize(CRect & rect);

	// our proxy slob to handle getting and setting props from and to the 
	// option tree control selection
	CProxySlob m_ProxySlob;

	// our option tree control in this tabbed dialog
	COptionTreeCtl m_OptTreeCtl;

	static CMapStringToPtr m_mapActiveFlavours;
	static BOOL IsActiveFlavour(CString & strFlavour) { void * pVoid; return m_mapActiveFlavours.Lookup(strFlavour, pVoid); }
	static void SetActiveFlavour(CString & strFlavour) { m_mapActiveFlavours.SetAt(strFlavour, NULL); }
	static void ResetActiveFlavours() { m_mapActiveFlavours.RemoveAll(); }
protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CProjOptionsDlg)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC *, CWnd *, UINT);
	afx_msg LRESULT OnSelectItem(WPARAM, LPARAM);
	afx_msg void OnSelChangeTarget();
	afx_msg void OnActivateEditCmds();
	afx_msg void OnActivateEditOutput();
	//}}AFX_MSG

	// afx_msg void OnBtnChangeTarget();

	BOOL InvokeChangeTargetDlg();

private:
	CProject * m_pProject;					// cached pointer to project

	// our map of tab string to category string
	// we use to maintain the state of our CToolOptionTabs
	static CMapStringToString m_mapLastMiniPage;
	
	// the project tree control whose state we want to initialise with
    // FUTURE (colint): Fix to work with the new build view
	// CProjTreeCtl * m_pProjTreeCtlToCopy;

	CSlob * m_pSelectionSlob;

	// our '&Settings For:' static text
	CStatic m_txtSettingsFor;

#if 0
	// Configurations button / text
	CButton m_bnConfigs;
#endif

	// Configurations combo box
	CComboBox m_cbConfigs;
	
	CString m_strAllConfigs;
	CString m_strMultiConfigs;

	// our 'No Common Properties' dialog
	CNoPropsDlg m_dlgNoProps;

	// our current list of option tool handlers available for the selection
	CPtrList m_listHdlrs;

	// our cached (new) list of option handlers available for the new selection
	CPtrList m_listHdlrsCache;

	// our previous tab header
	CString m_strPrevTab;

	// ignore tree-control selection changes
	BOOL m_fIgnoreSelChange;

	// resume recording OnDestroy()?
	BOOL m_fResumeRecording;

	// OnOK messages
	UINT m_idOnOKMsg;

	// previous target selection
	int m_nCurTargetSel;

public:
	BOOL m_bDirtyTabs;
};

/////////////////////////////////////////////////////////////////////////////
// CConfigSelectDlg dialog

class CConfigSelectDlg : public C3dDialog
{
// Construction
public:
	CConfigSelectDlg(CWnd* pParent = NULL);   // standard constructor
	~CConfigSelectDlg(void) { }

// Dialog Data
	//{{AFX_DATA(CConfigSelectDlg)
	enum { IDD = IDD_SELECT_CONFIG };
	CCheckList m_lbConfigs;
	enum { same, single, multi, all } m_SelState;
	//}}AFX_DATA

protected:
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSccDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSccDlg)
	afx_msg void OnSelectAll();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _INCLUDE_OPTIONDLG_H

