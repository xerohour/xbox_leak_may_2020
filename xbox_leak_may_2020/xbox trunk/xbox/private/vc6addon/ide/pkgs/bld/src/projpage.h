#ifndef __PROJPAGE_H__
#define __PROJPAGE_H__

#include "proppage.h"
#include "tabpage.h"
#include "utilctrl.h"


/////////////////////////////////////////////////////////////////////////////
// CProjSlobPageTab base class for tabs in the project settings dialog.
// We override Validate, to bypass Undo/Redo support

class CProjSlobPageTab : public CSlobPageTab
{
    DECLARE_DYNAMIC(CProjSlobPageTab);

public:
    virtual BOOL Validate();
};

/////////////////////////////////////////////////////////////////////////////
// CProjPage property page base-class for Project Window

#define WM_USER_CONFIG_CHANGE (WM_USER + 4)

class CProjPage : public CSlobPage
{
	DECLARE_DYNAMIC(CProjPage)

public:
	virtual BOOL IsPageDisabled();
	virtual BOOL DisableDuringBuild() { return FALSE; }
};

/////////////////////////////////////////////////////////////////////////////
// No Properties property pages
class CProjNoPropsPage : public CProjPage
{
	DECLARE_IDE_CONTROL_MAP();
};

/////////////////////////////////////////////////////////////////////////////
// General property pages
class CTargetGeneralPage : public CProjPage
{
	DECLARE_IDE_CONTROL_MAP();
};

class CGrpGeneralPage : public CProjPage
{
	DECLARE_IDE_CONTROL_MAP();
public:
    virtual BOOL Validate();
};

class CFileGeneralPage : public CProjPage
{
	DECLARE_IDE_CONTROL_MAP();

public:
    virtual BOOL Validate();
protected:
	BOOL OnPropChange(UINT idProp);
};

class CDepGeneralPage : public CProjPage
{
	DECLARE_IDE_CONTROL_MAP();
};

class COptionTreeCtl;
class CToolGeneralPage : public CProjSlobPageTab
{
	DECLARE_DYNAMIC(CToolGeneralPage)
	DECLARE_IDE_CONTROL_MAP();

public:
	void InitPage();
	void UpdateState();
	virtual BOOL Validate();

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CToolGeneralPage)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnSetDefault();
	//}}AFX_MSG

	// REVIEW:(KPERRY) should explicitly declare scoping after MFC Macros
	BOOL OnPropChange(UINT idProp);
};

/////////////////////////////////////////////////////////////////////////////
// Internal Java project general tab
class CJavaToolGeneralPage : public CProjSlobPageTab
{
	DECLARE_DYNAMIC(CJavaToolGeneralPage)
	DECLARE_IDE_CONTROL_MAP();

public:
	void InitPage();
	void UpdateState();

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CJavaToolGeneralPage)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnSetDefault();
	//}}AFX_MSG

	BOOL OnPropChange(UINT idProp);
};

/////////////////////////////////////////////////////////////////////////////
// External project options property pages
class CExtOptsPage : public CProjSlobPageTab
{
	DECLARE_DYNAMIC(CExtOptsPage)
	DECLARE_IDE_CONTROL_MAP();

public:
	void InitPage();
	virtual BOOL Validate();
	void UpdateState();

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CExtOptsPage)
	afx_msg void OnSetDefault();
	//}}AFX_MSG

	BOOL OnPropChange(UINT idProp);
};

/////////////////////////////////////////////////////////////////////////////
// External Target options property pages
class CExtTargetOptsPage : public CProjSlobPageTab
{
	DECLARE_DYNAMIC(CExtTargetOptsPage)
	DECLARE_IDE_CONTROL_MAP();

public:
	void InitPage();
	virtual BOOL Validate();
	void UpdateState();

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CExtTargetOptsPage)
	afx_msg void OnSetDefault();
	//}}AFX_MSG

	BOOL OnPropChange(UINT idProp);
};

class COptionMiniPage;
class CProxySlob;
class COptsDebugTab : public CProjSlobPageTab
{
	DECLARE_DYNAMIC(COptsDebugTab)
	DECLARE_IDE_CONTROL_MAP();

public:
	COptsDebugTab()
	{
		m_nCurrPage = UINT(-1); /* not active yet */
		m_pcboMiniPage = (CComboBox *)NULL;
		m_pProxySlob = (CProxySlob *)NULL;
	}

	~COptsDebugTab() {RemoveAllPages();}

	// page activation, deactivation and selection
	BOOL ActivatePage(UINT nPage);
	void DeactivatePage(UINT nPage);
	void SelectPage(UINT nPage);

	BOOL OnPropChange(UINT nProp);

	// remembering/setting the current mini-page (category-page)
	BOOL SetCurrentMiniPage(CString & strTitle);
	BOOL GetCurrentMiniPage(CString & strTitle);

	// the COptionsDebugTab will delete these pages, client should create
	BOOL AddOptionPage(COptionMiniPage * pPage);

	void RemoveAllPages();

	__inline COptionMiniPage * GetOptionPage(UINT nPage)
	{
		ASSERT(0 <= (int)nPage && (int)nPage < m_rgPages.GetSize());
		return (COptionMiniPage *)m_rgPages[nPage];
	}

	// return our caption
	__inline CString * GetCaption() {return &m_strCaption;}

protected:
	// make sure the page is a created Windows dialog
	COptionMiniPage * CreatePage(UINT nPage);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual void CommitTab();

	//{{AFX_MSG(COptsDebugTab)
	afx_msg void OnMiniPageSelect();
	//}}AFX_MSG

	// CDlgTab methods
	virtual BOOL Activate(CTabbedDialog * pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog * pParentWnd);
	virtual BOOL ValidateTab();

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
};

/////////////////////////////////////////////////////////////////////////////
// Pre-link, Post-build property page
class CPreLinkPage : public CProjSlobPageTab
{
	DECLARE_DYNAMIC(CPreLinkPage)
	DECLARE_IDE_CONTROL_MAP();

public:
	void InitPage();
	virtual BOOL Validate();
	void UpdateState();

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CPreLinkPage)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnSetDefault();
	//}}AFX_MSG

	BOOL OnPropChange(UINT idProp);

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	// string list grids for build command(s)
	CStringListGridWnd_WithChange m_PreLinkCmds;

	CString m_strPreLinkCmds;
};

class CPostBuildPage : public CProjSlobPageTab
{
	DECLARE_DYNAMIC(CPostBuildPage)
	DECLARE_IDE_CONTROL_MAP();

public:
	void InitPage();
	virtual BOOL Validate();
	void UpdateState();

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CPostBuildPage)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnSetDefault();
	//}}AFX_MSG

	BOOL OnPropChange(UINT idProp);

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	// string list grids for build command(s)
	CStringListGridWnd_WithChange m_PostBuildCmds;

	CString m_strPostBuildCmds;
};

/////////////////////////////////////////////////////////////////////////////
// Debug-only property pages

#ifdef _DEBUG
class CProjInfoPage : public CProjPage
{
	DECLARE_IDE_CONTROL_MAP();

public:
	void	InitPage();
};
#endif

/////////////////////////////////////////////////////////////////////////////
// shared routine to notice changes to Additional DLLs and inform the debugger
BOOL Validate_UpdateAdditionalDLLs(COptionMiniPage * pProjPage, 
		UINT idEditAddLocalDLLs, UINT idEditAddRemoteDLLs, UINT & idMsg);


/////////////////////////////////////////////////////////////////////////////
// the one 'n' only instances of our Project Window property pages
extern CProjNoPropsPage g_NoPropsPage;
extern CTargetGeneralPage g_TargetGeneralPage;
extern CGrpGeneralPage g_GrpGeneralPage;
extern CFileGeneralPage g_FileGeneralPage;
extern CDepGeneralPage g_DepGeneralPage;

#if _DEBUG
extern CProjInfoPage g_ProjInfoPage;
#endif

#endif // __PROJPAGE_H__
