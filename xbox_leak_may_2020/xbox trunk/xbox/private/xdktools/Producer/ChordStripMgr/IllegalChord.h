#if !defined(AFX_ILLEGALCHORD_H__9036C8B9_5315_11D2_BC79_00C04FA3726E__INCLUDED_)
#define AFX_ILLEGALCHORD_H__9036C8B9_5315_11D2_BC79_00C04FA3726E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IllegalChord.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIllegalChord dialog

class CIllegalChord : public CDialog
{
// Construction
public:
	CIllegalChord(CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CIllegalChord)
	enum { IDD = IDD_ILLEGALCHORD };
	CStatic	m_infotext;
	BOOL	m_bDontWarn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIllegalChord)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIllegalChord)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ILLEGALCHORD_H__9036C8B9_5315_11D2_BC79_00C04FA3726E__INCLUDED_)
