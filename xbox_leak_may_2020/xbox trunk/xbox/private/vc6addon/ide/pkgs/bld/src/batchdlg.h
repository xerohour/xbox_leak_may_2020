//
// batchdlg.h
//
// Defines CBatchBldDlg class, batch build dialog for building multiple
// project configurations in one go.
//
// Implementation is in : batchdlg.cpp
// 
// History:
// Date				Who			What
// 01/17/94			colint			created
//////////////////////////////////////////////////////////////////

#ifndef _BATCHDLG_H_
#define _BATCHDLG_H_

#include "resource.h"
#include "prjconfg.h"
#include "utilctrl.h"
#include <dlgbase.h>	// C3dDialog

/////////////////////////////////////////////////////////////////
// CBatchDlg class
// definition for batch build Dialog
//////////////////////////////////////////////////////////////////

class CBatchBldDlg: public C3dDialog
{
//  Construction 
public:
 	CBatchBldDlg ( CWnd * pParent = NULL );

// Dialog Data
	//{{AFC_DATA(CBatchBldDlg)
	enum { IDD = IDD_PROJECT_BATCH_BUILD } ;
	BOOL 		m_bRebuild;
	BOOL 		m_bClean;
	static BOOL	m_bSelectionOnly;
	//}}AFX_DATA

	CCheckList m_lbConfigs;
	CStringList * m_pBuildConfigs;
	static CMapStringToPtr m_mapSelected;

	static BOOL LoadBatchBldOpt(CArchive & archive);
	static BOOL SaveBatchBldOpt(CArchive & archive);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	void GetConfigs();

public:
	//{{AFX_MSG(CBatchBldDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRebuild();
	afx_msg void OnClean();
	//}}AFX_MSG

// private data
private:
} ;

/////////////////////////////////////////////////////////////////
// CProjectsDlg class
// definition for select projects Dialog
//////////////////////////////////////////////////////////////////

class CProjectsDlg: public C3dDialog
{
//  Construction 
public:
 	CProjectsDlg ( CWnd * pParent = NULL );

// Dialog Data
	//{{AFC_DATA(CProjectsDlg)
	enum { IDD = IDD_SELECT_PROJECTS } ;
	BOOL 		m_bExport;
	BOOL 		m_bExportDeps;
	//}}AFX_DATA

	CCheckList m_lbProjects;
	static CMapStringToPtr m_mapSelectedProjects;

	// FUTURE: use these to persist selection
	static BOOL LoadProjectsSelOpt(CArchive & archive);
	static BOOL SaveProjectsSelOpt(CArchive & archive);

// Implementation
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	DECLARE_MESSAGE_MAP()

	void GetProjects();

public:
	//{{AFX_MSG(CProjectsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void OnSelectAll();
	//}}AFX_MSG

// private data
private:
	CMapStringToPtr m_mapProjects;
} ;
#endif // _BATCHDLG_H_
