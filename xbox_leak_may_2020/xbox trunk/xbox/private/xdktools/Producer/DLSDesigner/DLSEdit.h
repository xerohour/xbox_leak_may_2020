#if !defined(AFX_DLSEDIT_H__8A0E5E80_EAB6_11D1_B987_006097B01078__INCLUDED_)
#define AFX_DLSEDIT_H__8A0E5E80_EAB6_11D1_B987_006097B01078__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DLSEdit.h : header file
//

class CInstrumentCtrl;

/////////////////////////////////////////////////////////////////////////////
// CDLSEdit window

class CDLSEdit : public CEdit
{
// Construction
public:
	CDLSEdit(CInstrumentCtrl* pParent);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLSEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDLSEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDLSEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CInstrumentCtrl* m_ParentCtrl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLSEDIT_H__8A0E5E80_EAB6_11D1_B987_006097B01078__INCLUDED_)
