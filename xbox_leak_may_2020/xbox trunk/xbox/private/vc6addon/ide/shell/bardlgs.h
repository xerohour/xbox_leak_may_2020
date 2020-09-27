///////////////////////////////////////////////////////////////////////////////
//	BARDLGS.H
//      Contains declarations for the dockable toolbar dialogs.
///////////////////////////////////////////////////////////////////////////////

#ifndef __BARDLGS_H__
#define __BARDLGS_H__

#include "dlgbase.h"
#include "utilctrl.h"
#include "btnctl.h"
#include "resource.h"
#include "customiz.h"

class CDockManager;
class CDockWorker;
class CASBar;

/////////////////////////////////////////////////////////////////////////////
// CToolbarDialog dialog

//	CToolbarCheckList control.
//		Override CCheckList for check notification to the dialog.

class CToolbarCheckList : public CCheckList
{
public:
	virtual void SetCheck(int nItem, BOOL bCheck);
};

class CToolbarDialog : public CDlgTab
{
// Construction
public:
	CToolbarDialog(CDockManager* pManager, CWnd* pParent = NULL);
	~CToolbarDialog();

	virtual BOOL Create();
	
	void LockManager(BOOL bLock);
	void UpdateCheck(UINT nIDWnd);
	BOOL UpdateVisible(int nItem);
	void AddToolbar(CDockWorker* pDocker);
	void RemoveToolbar(CDockWorker* pDocker);
	CDockWorker* GetWorker(int nIndex = LB_ERR);
	BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	void Deactivate(CTabbedDialog* pParentWnd);

// Dialog Data
	//{{AFX_DATA(CToolbarDialog)
	enum { IDD = IDD_TOOLBARS };
	CStatic m_statName;
	CEdit	m_edName;
	CButton	m_btnDelete;
	CButton	m_btnReset;
	CButton	m_btnResetAllBars;
	CButton	m_btnShow;
	CString	m_strName;
	BOOL m_bToolTips;
	BOOL m_bKeys;
	BOOL	m_bLargeToolBars;
	//}}AFX_DATA
	CToolbarCheckList m_lbToolbars;
	
	int m_nSel;
	CDockManager* m_pManager;
	CObArray m_aToolWorkers;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CToolbarDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedDelete();
	afx_msg void OnClickedReset();
	afx_msg void OnClickedResetAllBars();
	afx_msg void OnClickedNew();
	afx_msg void OnClickedToolTips();
	afx_msg void OnClickedKeys();
	afx_msg void OnSelChangeToolbar();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnClickedLargeToolBars();
	//}}AFX_MSG

	// resets the nIndex bar in the list
	void ResetBar(int nIndex);
    
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CToolbarDialog)
};

/////////////////////////////////////////////////////////////////////////////
// CNewBarDialog dialog

class CNewBarDialog : public C3dDialog
{
// Construction
public:
	CNewBarDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNewBarDialog)
	enum { IDD = IDD_NEWTOOLBAR };
	CButton	m_btnOK;
	CString	m_strName;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNewBarDialog)
	afx_msg void OnChangeName();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCustomDialog dialog

class CCustomDialog : public CCommandTab
{
// Construction
public:
	CCustomDialog(CDockManager* pManager, CWnd* pParent = NULL);
	~CCustomDialog();

	virtual BOOL Create();
	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog* pParentWnd);

	void LockManager(BOOL bLock);
	CASBar* GetToolbar(int nIndex = LB_ERR);
	void SetDescriptionText(UINT nID);
	
// Dialog Data
	//{{AFX_DATA(CCustomDialog)
	enum { IDD = IDDP_CUSTOMIZE_TOOLBARS };
	CButtonDragListBox	m_lbTextList;
	CComboBox			m_cbCategories;
	CComboBox			m_cbMenuFilter;
	CStatic				m_statDescription;
	CStatic				m_statTools;
	CButton				m_btnResetAllMenus;
	CMenuBtn			m_btnModifySelection;
	//}}AFX_DATA

	int m_nSel;
	CDockManager* m_pManager;
	CObArray m_aToolbars;
	// ids of categories; used for text categories
	CWordArray m_aIds;
	CAppToolGroups *m_pGroups;
	enum listFill	{	cListFillFirst=-5,
						cListFillCommands=-5,			// all commands
						cListFillDeletedCommands=-4,	// all commands which have been deleted from toolbars or menus
						cListFillMenus=-3,				// all menu bars
						cListFillNewMenu=-2,			// an empty custom menu
						cListFillCount=4,				// number of items
						cListFillNone=-1,
						cListFillNormalFirst=0
	};
	enum { cTextListStringBase=IDS_ALLCOMMANDS }; // the strings for the other
	enum menuFilters {	cMenuFilterCurrent=0,
						cMenuFilterAll=1 };
	enum { cMenuFilterStringBase=IDS_MENUFILTER_CURRENT };

									// possible text list entries should be monotonically
									// increasing from here.
	listFill m_lfCurrent;			// tells what the text list is currently filled with
									// to minimize the number of list re-fills
	int	m_iTextListBase;			// the item number within the categories list box that
									// contains the first text list item.
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void AutoSelectInitialCommand();
	// creates the toolbar set
	void CreateToolbars(void);
	// destroys it
	void DestroyToolbars(void);
	

	// Generated message map functions
	//{{AFX_MSG(CCustomDialog)
	afx_msg void OnSelChangeCategories();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnBarSelectChanging(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelChangeTextList();
	afx_msg void OnSelChangeMenuFilter();
	afx_msg void OnClickedResetAllMenus();
    afx_msg LRESULT OnCommandRemoved(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CCustomDialog)
};

#endif __BARDLGS_H__
