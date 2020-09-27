// BLDDLG.H
// ---------
// Exports from BLDDLG.CPP.
//
// History
// =======
// 20-Feb-96	karlsi		Created

#ifndef __BLDDLG_H__
#define __BLDDLG_H__

#ifndef __DLGBASE_H__
#include "dlgbase.h"
#endif

#if 0  // We might need a grid control 
#ifndef __UTILCTRL_H__
#include "utilctrl.h"
#endif

////////////////////////////////////////////////////////////
// CBldGrid
class CBldGrid : public CStringListGridWnd
{
// Construction
public:
	CBldGrid();

// Attributes
public:
	class CBldOptDlg* m_pDlg;

// Overrides
protected:
	virtual inline BOOL OnChange(int nIndex);
	virtual inline BOOL OnAddString(int nIndex);
	virtual inline BOOL OnDeleteString(int nIndex);
	virtual inline BOOL OnMove(int nSrcIndex, int nDestIndex);
};
#endif

////////////////////////////////////////////////////////////
// CBldOptDlg

class CBldOptDlg : public CDlgTab
{
protected:

			BOOL m_bAlreadyWarnedOfBuild;
			BOOL m_bExportDeps;
			BOOL m_bAlwaysExportMakefile;
			BOOL m_bWriteBuildLog;
			// Have we already warned that a build is going on.

			// CBldGrid m_BldGrid;

			VOID	CheckForBuildAndWarn ();
				// Warn the use if there's a build going on that changes
				//  won't take effect.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBldOptDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
					CBldOptDlg();
					~CBldOptDlg();

	virtual	BOOL	OnInitDialog();
	virtual	void	CommitTab();
	virtual BOOL	Activate(CTabbedDialog *, CPoint);
	virtual BOOL	ValidateTab();
	virtual void	OnClick();


#if 0
	BOOL OnAdd(int nIndex);
	BOOL OnDel(int nIndex);
	BOOL OnMove(int nSrcIndex, int nDestIndex);
	BOOL OnChange(int nIndex);
#endif

	//{{AFX_MSG(CBldOptDlg)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


////////////////////////////////////////////////////////////

#if 0
// CBldGrid inlines
inline BOOL CBldGrid::OnChange(int nIndex)
{
	return m_pDlg->OnChange(nIndex);
}

inline BOOL CBldGrid::OnAddString(int nIndex)
{
	return m_pDlg->OnAdd(nIndex);
}

inline BOOL CBldGrid::OnDeleteString(int nIndex)
{
	return m_pDlg->OnDel(nIndex);
}

inline BOOL CBldGrid::OnMove(int nSrcIndex, int nDestIndex)
{
	return m_pDlg->OnMove(nSrcIndex, nDestIndex);
}
#endif


#endif // __BLDDLG_H__
