//
// CDebugJavaGeneralPage
//		The general debugger options page for Java projects. This page
//		lets the user enter the class file to debug, the HTML page
//		containing the applet, and/or the parameters to the applet/application.
//
// CParamGridWnd
//		The grid for entering applet parameters.
//				   

#ifndef __JAVADBG_H__
#define __JAVADBG_H__

#include "prjoptn.h"
#include "optnui.h"		// CDebugPageTab

#include "resource.h"	// our resource IDs

// Classes defined in this file
class CDebugJavaGeneralPage;
class CParamGridWnd;
class CParamGridRow;
class CAddlClassesGridWnd;

//----------------------------------------------------------------
// CDebugJavaGeneralPage: the page to handle the general options
//----------------------------------------------------------------

class CDebugJavaGeneralPage: public CDebugPageTab
{
	DECLARE_IDE_CONTROL_MAP()
	DECLARE_MESSAGE_MAP()

// page operations
public:
	virtual void InitPage();
	virtual void CommitPage();
	virtual BOOL Validate();

public:
	virtual BOOL OnInitDialog();
	BOOL OnPropChange(UINT idProp);

// internal functions
private:
	void OnChangeClassFileName();
	void OnChangeDebugUsing();
};

//----------------------------------------------------------------
// CDebugJavaBrowserPage: the page to handle the browser options
//----------------------------------------------------------------

// private messages handled by the Java general debug page
#define GN_CHANGE WM_USER + 0xbc

class CDebugJavaBrowserPage: public CDebugPageTab
{
	DECLARE_IDE_CONTROL_MAP()
	DECLARE_MESSAGE_MAP()

// page operations
public:
	virtual void InitPage();
	virtual void CommitPage();
	virtual BOOL Validate();

public:
	virtual BOOL OnInitDialog();
	BOOL OnPropChange(UINT idProp);
	void OnDestroy();

// message handlers
protected:
	afx_msg void OnChangeParamGrid();

// internal functions
private:
	void OnChangeBrowser();
	void OnChangeParamSource();
	void UpdateParamGrid();

// internal data
private:
	// param grid
	CParamGridWnd *m_pgridParams;
};

//----------------------------------------------------------------
// CDebugJavaStandalonePage: the page to handle the stand-alone options
//----------------------------------------------------------------

class CDebugJavaStandalonePage: public CDebugPageTab
{
	DECLARE_IDE_CONTROL_MAP()
	DECLARE_MESSAGE_MAP()

// page operations
public:
	virtual void InitPage();
	virtual void CommitPage();
	virtual BOOL Validate();

public:
	virtual BOOL OnInitDialog();
	BOOL OnPropChange(UINT idProp);

// internal functions
private:
	void OnChangeStandalone();
};

//----------------------------------------------------------------
// CDebugJavaAddlClassesPage: the page to handle additional classes
//----------------------------------------------------------------

class CDebugJavaAddlClassesPage: public CDebugAdditionalDllPage
{
	DECLARE_IDE_CONTROL_MAP()

public:
	virtual void InitPage();
	virtual BOOL OnInitDialog();
	virtual void InitGrids();
};

//----------------------------------------------------------------
// CParamGridRow: row for handling parameters
//----------------------------------------------------------------

class CParamGridRow: public CGridControlRow
{
// columns
public:
	enum { GRID_COL_NAME, GRID_COL_VALUE, NUM_GRID_COLS /* should be last */ };

// ctor/dtor
public:
	CParamGridRow(CParamGridWnd* pGrid);
	~CParamGridRow();

// row operations
public:
	void DrawCell(CDC *pDC, const CRect& cellRect, int nColumn);
	BOOL IsNewRow();

// column operations
public:
	void OnActivate(BOOL bActivate, int nColumn);
	virtual BOOL OnAccept(CWnd* pControlWnd);
	virtual void GetColumnText(int nColumn, CString& str);
	void SetColumnText(int nColumn, LPCSTR pszValue);
	void ResetSize(CDC* pDC);

// grid access
public:
	CParamGridWnd* GetParamGrid();

// internal functions
private:
	void QuoteString(CString& str);

// internal data
private:
	CString m_strName;
	CString m_strValue;
};

//----------------------------------------------------------------
// CParamGridWnd: grid control window for handling parameters
//----------------------------------------------------------------

class CParamGridWnd: public CGridControlWnd
{
	DECLARE_DYNAMIC(CParamGridWnd)

// ctor/dtor
public:
	CParamGridWnd(int nColumns = CParamGridRow::NUM_GRID_COLS, BOOL bDisplayCaption = TRUE);
	~CParamGridWnd();
	
// row access
public:
	int GetCurSel();
	CParamGridRow* GetTailRow() const;
	POSITION GetTailRowPosition() const;
	POSITION GetTailSelPosition() const;

// row operations
public:
	void DoRowDelete();
	void DoRowMove(int nToIndex);
	BOOL OnRowMove(int nSrcIndex, int nDestIndex);
	int GetRowIndex(const CParamGridRow* pRow) const;

// toolbar button handlers
protected:
	afx_msg void OnGridNew();			//  New button pressed 
	afx_msg void OnGridDelete();		//  Delete button pressed 
	afx_msg void OnGridMoveUp();		//  Move Up button pressed 
	afx_msg void OnGridMoveDown();		//  Move Down button pressed 

// overrides
protected:
	virtual BOOL ProcessKeyboard(MSG* pMsg, BOOL bPreTrans = FALSE);
	virtual void DoDragDrop(CPoint point);
	afx_msg UINT OnGetDlgCode();

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

// attributes
public:
	void SetReadOnly(BOOL bReadOnly);
	BOOL IsReadOnly();

// param string access
public:
	void SetParamsFromHTML(LPCSTR pszParams);
	CString GetParamsAsHTML();

	CString GetParamsAsCmdLine();

// message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);
	afx_msg void OnDestroy();
	void OnPaint();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()

// internal functions
private:
	void DeleteAllRows();
	void SetGridFromHTML(LPCSTR pszParams);
	CString GetHTMLFromGrid();
	BOOL GetParamLine(CString& strParams, CString& strName, CString& strValue);
	BOOL SkipTokens(CString& strString, TCHAR* pszSkipTokens[]);
	BOOL GetQuotedString(CString& strString, CString& strValue);
	CString GetCmdLineFromGrid();
	void GridChange();

// internal data
private:
	// string of params to display in grid
	CString m_strHTMLParams;
	// flag for read only grid
	BOOL m_bReadOnly;
};

//----------------------------------------------------------------
// CAddlClassesGridRow: grid row for handling addl classes
//----------------------------------------------------------------
class CAddlClassesGridRow: public CDLLGridRow
{
// ctor/dtor
public:
	CAddlClassesGridRow(CAddlClassesGridWnd* pGridWnd, CAddlClassesGridRow* pParent = NULL);

// attributes
public:
	virtual void GetColumnText(int nColumn, CString& rStr);
	virtual void OnActivate(BOOL bActivate, int nColumn);
	virtual BOOL OnAccept(CWnd *pControlWnd);
};

//----------------------------------------------------------------
// CAddlClassesGridWnd: grid control window for handling addl classes
//----------------------------------------------------------------
class CAddlClassesGridWnd: public CDLLGridWnd
{
// ctor/dtor
public:
	CAddlClassesGridWnd(int nColumns = 3, BOOL bDisplayCaption = TRUE);
};

#endif // __JAVADBG_H__
