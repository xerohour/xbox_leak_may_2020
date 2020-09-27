#if !defined(AFX_PCHANNELLIST_H__D9BCBB12_2834_11D2_9A65_006097B01078__INCLUDED_)
#define AFX_PCHANNELLIST_H__D9BCBB12_2834_11D2_9A65_006097B01078__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PChannelList.h : header file
//


class CBandDlg;
class CDMInstrument;
/////////////////////////////////////////////////////////////////////////////
// CPChannelList window

class CPChannelList : public CListBox
{
// Construction
public:
	CPChannelList();

// Attributes
public:
	CBandDlg* m_pBandDlg;

// Operations
public:
	void SetBandDialog(CBandDlg* pBandDlg);
	
	CString GetPChannelName(CDMInstrument* pInstrument);
	
	bool TestIfIgnore(CDMInstrument* pInstrument, DWORD dwTestFlag);

	void SelectPChannelFromPoint(CPoint point, bool bSelect);

	void DrawListItemText(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct, 
						  int nLeftOffset, int nRightOffset, 
						  CRect& originalRect, CString& sText, 
						  bool bDisabled, CFont* pNormalFont, 
						  CFont* pBoldFont);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPChannelList)
	public:
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPChannelList();

	

	// Generated message map functions
protected:
	//{{AFX_MSG(CPChannelList)
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCHANNELLIST_H__D9BCBB12_2834_11D2_9A65_006097B01078__INCLUDED_)
