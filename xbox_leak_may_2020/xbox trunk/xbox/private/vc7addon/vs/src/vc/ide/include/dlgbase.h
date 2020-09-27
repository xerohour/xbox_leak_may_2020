///////////////////////////////////////////////////////////////////////////////
//	DLGBASE.H
//		Declarations for the sushi dialog base classes.
//
#ifndef __DLGBASE_H__
#define __DLGBASE_H__

#ifndef __PATH_H__
#include "path.h"
#endif

#include "stdfont.h"

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA
#ifndef SLOBAPI
#define SLOBAPI __declspec()
#endif

// Set standard fonts in dialog template
void SetStdFont(C3dDialogTemplate & dt);

// All Sushi dialog boxes should derive from this class...
//
class SLOBAPI C3dDialog : public CDialog
{
public:
	C3dDialog() {m_pLocInfo = NULL;};
	C3dDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL, CLocaleInfo  *pLocInfo = NULL);
	C3dDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL, CLocaleInfo  *pLocInfo = NULL);

	virtual INT_PTR DoModal();
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	
	afx_msg void OnRobustOK();
	afx_msg LRESULT OnTestMenu(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	CLocaleInfo  *m_pLocInfo;
};

extern BOOL FindAccel( LPCTSTR lpstr, MSG *pMsg );

class SLOBAPI C3dFileDialog : public CFileDialog
{
public:
	C3dFileDialog(BOOL bOpenFileDialog, // TRUE for Open, FALSE for SaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		UINT nHelpID = 0, UINT nExplorerDlg = (UINT)-1);
	~C3dFileDialog();

	LPCTSTR GetFilterExtension(int n);
	void UpdateType(LPCTSTR lpszExt = NULL);
	void ApplyDefaultExtension(void);
	void SetOkButtonText(UINT ids);

	virtual INT_PTR DoModal();

	afx_msg void OnRobustOK();
	afx_msg void OnNameKillFocus();
	afx_msg void OnFileListSelChange();
	afx_msg void OnScc();

	void UpdateMultiSelectOnNT(void);

	virtual void OnOK();
	virtual void OnCancel ();
	virtual BOOL OnFileNameOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();

	BOOL IsExplorer()
		{ return m_ofn.Flags & OFN_EXPLORER; }

	// essentially overrides for MFC's GetStartPosition and GetNextPathName,
	// but which help us deal with limitations on multiple-selection handling.
	POSITION GetFullFileStartPosition();
	CString GetNextFullFileName(POSITION pos);

	DECLARE_MESSAGE_MAP()

protected:
	CDir m_dirCurBefore;	// Current dir before dialog goes up
	CDir m_dirCurAfter;		// Current dir after dismissal

public:
	CStringArray* m_psaFileNames;
	UINT m_iddWin95;
	UINT m_iddWinNT;
	BOOL m_bMultiSelectOnNT;
	BOOL m_bSccEnabled;
};

extern void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	const CString& strFilterName, BOOL bSetDefExt = FALSE);

extern void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	UINT idFilter, BOOL bSetDefExt = FALSE);

extern BOOL AddRegistryFilters(CString& strFilter, int nDesiredType, OPENFILENAME& ofn);

extern void AddWildFilter(CString& strFilter, OPENFILENAME& ofn);

extern void SetFileFilters(CString& strFilter, OPENFILENAME& ofn, int nDesiredType, 
	UINT* rgidDefaults, int nDefaults, const TCHAR* szCommon = NULL);

#define FLT_OPEN			1	// The File Open dialog
#define FLT_PROJECTFILES	2	// The Project Files dialog
#define FLT_TEXTFILES		4	// The Find in Files dialog

#endif	// __DLGBASE_H__