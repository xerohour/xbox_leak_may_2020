//
// COptionPage, COptionPageTab
//
// Option user interface.
//
// [matthewt]
//				   

#ifndef _INCLUDE_OPTIONUI_H										
#define _INCLUDE_OPTIONUI_H

#ifndef _SUSHI_PROJECT
// If you are including this in a file that is being compiled under
// WIN16, either make sure everything compiles under WIN16 and remove
// this error, or include conditional on _SUSHI_PROJECT.
//
#error This code may not be Win 3.1 compatible.
#endif

#include "slob.h"		// CSlob and CPropBag def'n
#include "dlgbase.h"		// CDlgTab
#include "tabpage.h"	// CSlobPageTab
#include "dllgrid.h"	// CDLLGridWnd

#include "resource.h"	// our resource IDs

// our tool options tab for
// use as a frame for tool option property pages
// it'll also handle 
// o selection of pages
// o handling of set default
// o handling of direct editing of options string

class COptionMiniPage;
class CProxySlob;
class BLD_IFACE CToolOptionTab : public CDlgTab
{
	DECLARE_DYNAMIC(CToolOptionTab)

public:
	// supply the title of the tab, ie. 'C/C++ Compiler'
	CToolOptionTab(UINT nIDTitle) :
		CDlgTab(IDD_TOOL_OPTION_TAB, nIDTitle)
	{
		m_nCurrPage = UINT(-1); /* not active yet */
		m_popthdlr = (COptionHandler *)NULL;
		m_pcboMiniPage = (CComboBox *)NULL;
		m_pProxySlob = (CProxySlob *)NULL;
		m_fRefresh = TRUE;
	}

	~CToolOptionTab() {RemoveAllPages();}

	// page activation, deactivation and selection
	BOOL ActivatePage(UINT nPage);
	void DeactivatePage(UINT nPage);
	void SelectPage(UINT nPage);
	void Refresh();

	// remembering/seeting the current mini-page (category-page)
	BOOL SetCurrentMiniPage(CString & strTitle);
	BOOL GetCurrentMiniPage(CString & strTitle);

	// make sure the approp. controls are disabled/enabled etc.	
	void UpdateState();

	// the CToolOptionTab will delete these pages, client should create
	BOOL AddOptionPage(COptionMiniPage * pPage);

	void RemoveAllPages();
	__inline COptionMiniPage * GetOptionPage(UINT nPage)
	{
		ASSERT(0 <= (int)nPage && (int)nPage < m_rgPages.GetSize());
		return (COptionMiniPage *)m_rgPages[nPage];
	}

	// set the option handler to use while
	// this tab is up (doesn't need to be set)
	__inline void SetOptionHandler(COptionHandler * popthdlr) 
	{ m_popthdlr = popthdlr; m_nOrder = popthdlr->GetTabDlgOrder();}

	__inline COptionHandler * GetOptionHandler() {return m_popthdlr;}

	// return our caption
	__inline CString * GetCaption() {return &m_strCaption;}

	BOOL m_fRefresh;

protected:
	// make sure the page is a created Windows dialog
	COptionMiniPage * CreatePage(UINT nPage);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual LRESULT OnOptStrEditPaint(WPARAM, LPARAM);

	//{{AFX_MSG(CToolOptionTab)
	afx_msg void OnDestroy();
	afx_msg void OnMiniPageSelect();
	afx_msg void OnSetDefault();
	afx_msg void Validate_Dest();
	afx_msg void Update();
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	//}}AFX_MSG

	// CDlgTab methods
	virtual BOOL Activate(CTabbedDialog * pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog * pParentWnd);
	virtual BOOL ValidateTab();
	
	// our COptionHandler to use
	COptionHandler * m_popthdlr;
	
private:

	// our current mini page
	UINT m_nCurrPage;
	
	// category to init. the tab category listbox 
	CString m_strInitCategory;
	
	// our list of mini pages
	CObArray m_rgPages;

	// our comobo box control used to select a mini-page
	CComboBox * m_pcboMiniPage;

	// our parent's (a tabbed dialog) CProxySlob
	CProxySlob * m_pProxySlob;

	// top of al pages to be created
	int m_cyPageTop;

	// Refresh 	m_pcboMiniPage value to make sure it is valid.
	BOOL ValidateMiniPageComboBox()
	{ 
		m_pcboMiniPage = (CComboBox *)GetDlgItem(IDC_MINI_PAGE);
		return (NULL!=m_pcboMiniPage);
	}
};

// our tool options property page for
// any tool options, eg. output directories

class COptionWatcher : public CSlob
{
public:
	void OnInform(CSlob *, UINT, DWORD);
	COptionMiniPage * m_pOptionPage;
};

class COptsDebugTab;
class BLD_IFACE COptionMiniPage : public CDialog
{
friend class COptionWatcher;

	DECLARE_DYNAMIC(COptionMiniPage)
	DECLARE_IDE_CONTROL_MAP()
	DECLARE_MESSAGE_MAP()

public:
	COptionMiniPage();
	~COptionMiniPage();

	// mini-page creation
	// o requires parent dialog
	BOOL Create(CToolOptionTab * pParent);
	BOOL Create(COptsDebugTab * pParent);

	// setup our 
	// o CSlob
	// o associated option handler (if we have one) for init. control map
	BOOL SetupPage(CSlob * pSlob, COptionHandler * popthdlr = (COptionHandler *)NULL);

	// get our CSlob for this CSlobPageTab
	__inline CSlob * GetSlob() {return m_pSlob;}

	virtual BOOL OnInitDialog();

	// initialization and termination of the property page
	virtual void InitPage();
	virtual void TermPage();

	// hook into property value changing
	virtual BOOL OnCommand(UINT wParam, LONG lParam);
	virtual BOOL OnPropChange(UINT nProp);

	// contol map dialog id and name extraction
	int GetDlgID() {return GetControlMap()->m_nCtlID;}
	UINT GetNameID();
	void GetName(CString& strName);

	// property mapping
	__inline UINT MapLogical(UINT idProp) {return m_popthdlr->MapLogical(idProp);}
	__inline UINT MapActual(UINT idProp) {return m_popthdlr->MapActual(idProp);}

	// property value validation
	virtual BOOL Validate();
 	virtual BOOL Validate_DestinationProp
				(UINT idProp, UINT idOption, UINT idEditDest, UINT idMsg,
				 const TCHAR * pchExt, BOOL fMustBeDir, BOOL fCantBeDir);	
	virtual BOOL Validate_Destination
				(UINT idEditDest, UINT idMsg, const TCHAR * pchExt, BOOL fMustBeDir, BOOL fCantBeDir);	

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	LONG OnValidateReq(UINT, LONG);
	int m_nValidateID;

	// control map searching
	CControlMap* FindControl(UINT nCtlID);
	CControlMap* FindProp(UINT nProp);

	// currently validating?
	BOOL m_isValidating;

	UINT m_nCurrentEditID;

private:
	virtual void OnOK();
	virtual void OnCancel();

protected:
	// Our Check-List we substitute by sub-classing
	// listboxes requiring a check-list in the prop. page
	CPropCheckList m_PropCheckList;
	
	// Our slob which is used to store/retrieve properties to/from
	// You'll need to handle the commit semantics yourself.
	// ie. On Cancel undo property changes to this CSlob, or
	// on OK commit changes from a tmp. CSlob (this one) into the actual one
	CSlob* m_pSlob;
																						
	// Our option 'watcher' CSlob
	COptionWatcher m_optionWatcher;

	// A cache of Check-List objects
	CPtrList m_listCheckList;

	CToolOptionTab * m_pToolOptionTab;
 	COptionHandler * m_popthdlr;

private:
	BOOL m_bIgnoreChange;
	BOOL m_bGotSlobHook;
};

//----------------------------------------------------------------
// our debug option property pages
//----------------------------------------------------------------
class CDebugPageTab : public COptionMiniPage
{
public:
	CDebugPageTab() { m_fPageInitialized = FALSE; }
	~CDebugPageTab() {}
	virtual BOOL Validate();
	virtual BOOL FCanResetPropsForConfig()	{ ASSERT (FALSE); return FALSE; }
	virtual void ResetPropsForConfig()  { ASSERT (FALSE); }
	virtual void CommitPage() = 0;

protected:
	BOOL m_fPageInitialized;
};

//----------------------------------------------------------------
// our 'General' option property page
//----------------------------------------------------------------

class CDebugGeneralPage : public CDebugPageTab
{
	DECLARE_IDE_CONTROL_MAP()
public:
	void InitPage();
	virtual BOOL Validate();
	virtual void CommitPage();

	virtual BOOL OnInitDialog();

	void OnDebugBrowse();
	void OnUseWebBrowser();
	void OnUseTestContainer();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

private:
	CMenuBtn m_btnExeFinder;
};

//----------------------------------------------------------------
// our 'Additional DLLs' option property page
//----------------------------------------------------------------

class CDebugAdditionalDllPage : public CDebugPageTab
{
	DECLARE_IDE_CONTROL_MAP()
	DECLARE_MESSAGE_MAP()
public:

	// special handling of property changes
	virtual void InitPage();
	virtual void TermPage();
	virtual BOOL OnInitDialog();
	virtual BOOL Validate();
	BOOL Validate_UpdateAdditionalDLLs(UINT &idMsg);
	virtual void CommitPage();
	virtual void OnBrowse();
	virtual void InitGrids();
	virtual void SaveGrids();

protected:
	afx_msg void OnDestroy();

protected:
	CPtrList m_GridList;	// GridList is a list of pointers to GRIDINFOs
	CDLLGridWnd *m_pGridActive;
};

typedef struct {
	HBLDTARGET hTarget;
	CDLLGridWnd *pGrid;
} GRIDINFO;



BOOL Validate_Destination(CDialog * pDlg, UINT idEditDest, UINT idMsg,
						  const TCHAR * pchExt, BOOL fMustBeDir, BOOL fCantBeDir);

#endif // _INCLUDE_OPTIONUI_H
