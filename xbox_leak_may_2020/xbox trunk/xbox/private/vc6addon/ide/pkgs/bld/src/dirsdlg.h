// DIRSDLG.H
// ---------
// Exports from DIRSDLG.CPP.
//
// History
// =======
// 26-Aug-93	mattg		Created

#ifndef __DIRSDLG_H__
#define __DIRSDLG_H__

#ifndef __DLGBASE_H__
#include "dlgbase.h"
#endif

#ifndef __DIRMGR_H__
#include "dirmgr.h"
#endif

#ifndef __UTILCTRL_H__
#include "utilctrl.h"
#endif

////////////////////////////////////////////////////////////
// CDirsGrid
class CDirsGrid : public CStringListGridWnd
{
// Construction
public:
	CDirsGrid();

// Attributes
public:
	class CDirsOptDlg* m_pDlg;

// Overrides
protected:
	virtual inline BOOL OnChange(int nIndex);
	virtual inline BOOL OnAddString(int nIndex);
	virtual inline BOOL OnDeleteString(int nIndex);
	virtual inline BOOL OnMove(int nSrcIndex, int nDestIndex);
};

////////////////////////////////////////////////////////////
// CDirsOptDlg

class CDirsOptDlg : public CDlgTab
{
protected:
			BOOL	m_bDeleteDirs;
				// If TRUE, delete m_Dirs at destructor time
				// (else don't).

			INT		m_nToolset;
				// Current 'toolset' selected.

			INT		m_nJavaPlatform;
				// index in platform list of "Java VM" platform (-1 if not there)

			DIRLIST_TYPE m_type;
				// Current DIRLIST_TYPE selected.

			INT		m_nCurTypeSel;
				// Current selection index (Combo item corresponding to m_type)

			CObList* (*m_pDirs)[C_DIRLIST_TYPES];
				// Copy of the arrays of dirlists in the CDirMgr
				// object.

			BOOL m_bAlreadyWarnedOfBuild;
				// Have we already warned that a build is going on.

			CDirsGrid m_DirGrid;

			POSITION PosFromListboxIndex(INT nIndex);
				// Return the POSITION within the current list
				// of the item specified by nIndex.

			VOID	SetListboxSel(INT nSel);
				// Set the selection in the listbox and enable
				// and disable buttons appropriately.

			VOID	CheckForBuildAndWarn ();
				// Warn the use if there's a build going on that changes
				// in directories won't take effect.

			VOID FillOptionsList ();
				// Sets listbox entries to corrospond to the current type and toolset

            BOOL    m_bModified;
public:
					CDirsOptDlg();
					~CDirsOptDlg();

			void	ResetListbox(BOOL bSetSelection = TRUE);
				// Sets listbox entries to correspond to the
				// current type and toolset.

	virtual	BOOL	OnInitDialog();
	virtual	void	CommitTab();
	virtual BOOL	Activate(CTabbedDialog *, CPoint);
	virtual BOOL	ValidateTab();


	BOOL OnAddDir(int nIndex);
	BOOL OnDelDir(int nIndex);
	BOOL OnMoveDir(int nSrcIndex, int nDestIndex);
	BOOL OnChangeDir(int nIndex);

	//{{AFX_MSG(CDirsOptDlg)
//	afx_msg	VOID	OnClickToolset1(); TOOLSETS DISABLED
//	afx_msg	VOID	OnClickToolset2();
//	afx_msg	VOID	OnClickToolset3();
	afx_msg void OnSelChangeDirOptions();
	afx_msg void OnSelChangeDirToolset();
	afx_msg void OnBrowse();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


////////////////////////////////////////////////////////////
// CDirsGrid inlines
inline BOOL CDirsGrid::OnChange(int nIndex)
{
	return m_pDlg->OnChangeDir(nIndex);
}

inline BOOL CDirsGrid::OnAddString(int nIndex)
{
	return m_pDlg->OnAddDir(nIndex);
}

inline BOOL CDirsGrid::OnDeleteString(int nIndex)
{
	return m_pDlg->OnDelDir(nIndex);
}

inline BOOL CDirsGrid::OnMove(int nSrcIndex, int nDestIndex)
{
	return m_pDlg->OnMoveDir(nSrcIndex, nDestIndex);
}


#endif // __DIRSDLG_H__
