#if !defined(AFX_DLGADDFILES_H__A1C4955C_94FC_40B5_833C_0D471C57F42B__INCLUDED_)
#define AFX_DLGADDFILES_H__A1C4955C_94FC_40B5_833C_0D471C57F42B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddFiles.h : header file
//


#define SORT_NAME		1
#define SORT_TYPE		2
#define SORT_SIZE		3


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles dialog

class CDlgAddFiles : public CDialog
{
// Construction
public:
	CDlgAddFiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddFiles)
	enum { IDD = IDD_DLG_ADD_FILES };
	CStatic	m_staticReferencePrompt1;
	CStatic	m_staticReferencePrompt2;
	CButton	m_btnOK;
	CListBox	m_lstbxReference;
	CListBox	m_lstbxAvailable;
	CListBox	m_lstbxEmbed;
	CButton	m_btnReset;
	CButton	m_btnRemoveReference;
	CButton	m_btnAddReference;
	CButton	m_btnRemoveEmbed;
	CButton	m_btnAddEmbed;
	CButton	m_radioSortName;
	CButton	m_radioSortType;
	CButton	m_radioSortSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillAvailableList();
	void FillEmbedList();
	void FillReferenceList();
	void SortAvailableList();
	void SortEmbedList();
	void SortReferenceList();
	void FormatTextUI( IDMUSProdNode* pINode, CString& strText );
	CFont* CreateFont();

	// Generated message map functions
	//{{AFX_MSG(CDlgAddFiles)
	virtual BOOL OnInitDialog();
	afx_msg int OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnRadioSortName();
	afx_msg void OnDoubleClickedRadioSortName();
	afx_msg void OnRadioSortType();
	afx_msg void OnDoubleClickedRadioSortType();
	afx_msg void OnRadioSortSize();
	afx_msg void OnDoubleClickedRadioSortSize();
	virtual void OnOK();
	afx_msg void OnBtnEmbedAdd();
	afx_msg void OnBtnEmbedRemove();
	afx_msg void OnBtnReferenceAdd();
	afx_msg void OnBtnReferenceRemove();
	afx_msg void OnBtnReset();
	afx_msg void OnDblClkListAvailable();
	afx_msg void OnDblClkListEmbed();
	afx_msg void OnDblClkListReference();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	short	m_nSortType;

public:
	bool	m_fDisableReferenced;
	CString m_strTitle;

	CTypedPtrList<CPtrList, IDMUSProdNode*> m_lstAvailable;
	CTypedPtrList<CPtrList, IDMUSProdNode*> m_lstEmbedded;
	CTypedPtrList<CPtrList, IDMUSProdNode*> m_lstReferenced;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDFILES_H__A1C4955C_94FC_40B5_833C_0D471C57F42B__INCLUDED_)
