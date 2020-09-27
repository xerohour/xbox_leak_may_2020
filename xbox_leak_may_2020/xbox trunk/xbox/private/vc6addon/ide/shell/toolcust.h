// toolcust.h : header file for CToolsGrid, CCustomizeToolsDialog
//

#ifndef __TOOLCUST_H__
#define __TOOLCUST_H__

#include "tools.h"
#include "resource.h"
#include "utilctrl.h"

class CCustomizeToolsDialog;

/////////////////////////////////////////////////////////////////////////////
// CToolsGrid

class CToolsGrid : public CStringListGridWnd
{
// Construction
public:
	CToolsGrid();
	~CToolsGrid();

// Attributes
public:
	class CCustomizeToolsDialog* m_pDlg;

// Overrides
protected:
	virtual inline BOOL OnAddString(int nIndex);
	virtual inline BOOL OnDeleteString(int nIndex);
	virtual inline BOOL OnChange(int nIndex);
	virtual inline BOOL OnMove(int nSrcIndex, int nDestIndex);

	void DoCut();
	void DoCopy();
	void DoPaste();

	virtual void DoMove(int nToIndex);

// Implementation
public:
	virtual BOOL ProcessKeyboard(MSG* pMsg, BOOL bPreTrans = FALSE);
	virtual BOOL BeginDrag(UINT nFlags, const CPoint& point);

	virtual void AddNewRow(BOOL bSelect = TRUE);

protected:
	afx_msg void OnGridNew();			//  New button pressed 

private:
	CTool *m_pClipboardTool;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCustomizeToolsDialog

class CCustomizeToolsDialog : public CDlgTab
{
// Construction
public:
	
// Dialog data
	//{{AFX_DATA(CCustomizeToolsDialog)
	enum { IDD = IDDP_CUSTOMIZE_TOOLSMENU };
	//}}AFX_DATA

	CCustomizeToolsDialog();

	BOOL OnAddTool(int nIndex);
	BOOL OnDelTool(int nIndex);
	BOOL OnChangeTool(int nIndex);
	BOOL OnMoveTool(int nSrcIndex, int nDestIndex);

protected:
	//{{AFX_MSG(CCustomizeToolsDialog)
	virtual BOOL	OnInitDialog();
	virtual BOOL	ValidateTab();
	virtual void	CancelTab();
	afx_msg void	OnPathNameKillFocus();
	afx_msg void	OnBrowse();
	afx_msg void	OnRedirect();
	afx_msg void	OnMacroItem(UINT nId);
	afx_msg void	OnGridSelChange();
	afx_msg void	OnGridAddString();
	afx_msg void	OnGridDeleteString();
	afx_msg void	OnGridSetFocus();
	afx_msg void	OnGridKillFocus();
	afx_msg void	OnGridActivate();
	afx_msg void	OnGridAccept();
	//}}AFX_MSG	

	// our message map functions
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CCustomizeToolsDialog)

public:
	CToolsGrid m_ToolGrid;
	CToolList  m_ToolList;

private:
	CMenuBtn m_mbtnArgMacros;
	CMenuBtn m_mbtnDirMacros;
	BOOL     m_bOutputWindowAvailable;

	int  m_nDeletedOutputWindows;
	UINT m_rgnDeletedOutputWindows[MAXTOOLIST];

public:
	void ShowToolInfo();
	BOOL GetToolInfo();
	void FillGrid(int nSelectRow = 0);
	void EnableControls();
	void EnableRedirection();
};

/////////////////////////////////////////////////////////////////////////////
// CToolsGrid inlines

inline BOOL CToolsGrid::OnAddString(int nIndex)
{
	return(m_pDlg->OnAddTool(nIndex));
}

inline BOOL CToolsGrid::OnDeleteString(int nIndex)
{
	return(m_pDlg->OnDelTool(nIndex));
}

inline BOOL CToolsGrid::OnChange(int nIndex)
{
	return(m_pDlg->OnChangeTool(nIndex));
}

inline BOOL CToolsGrid::OnMove(int nSrcIndex, int nDestIndex)
{
	return(m_pDlg->OnMoveTool(nSrcIndex, nDestIndex));
}

/////////////////////////////////////////////////////////////////////////////

#endif	// __TOOLCUST_H__
