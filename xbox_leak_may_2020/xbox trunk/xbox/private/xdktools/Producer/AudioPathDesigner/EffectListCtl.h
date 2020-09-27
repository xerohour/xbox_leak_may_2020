#if !defined(AFX_EFFECTLISTCTL_H__0EEF952B_9F3E_490C_AEA4_3B719B1104C5__INCLUDED_)
#define AFX_EFFECTLISTCTL_H__0EEF952B_9F3E_490C_AEA4_3B719B1104C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffectListCtl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEffectListCtl window

class CEffectListCtl : public CListCtrl
{
// Construction
public:
	CEffectListCtl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectListCtl)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEffectListCtl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEffectListCtl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTLISTCTL_H__0EEF952B_9F3E_490C_AEA4_3B719B1104C5__INCLUDED_)
