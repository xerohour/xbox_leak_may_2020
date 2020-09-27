#if !defined(AFX_DLGDLS_H__D1868D61_9F84_11D0_8C10_00A0C92E1CAC__INCLUDED_)
#define AFX_DLGDLS_H__D1868D61_9F84_11D0_8C10_00A0C92E1CAC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgDLS.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// Collections List Item Struct
typedef struct _CollectionListItem
{
	CString									csCollectionName;
	interface IDMUSProdReferenceNode*		pRefNode;
} COLLECTION_LISTITEM;

/////////////////////////////////////////////////////////////////////////////
// Instruments List Item struct
typedef struct _InstrumentListItem
{
	DWORD dwParams;
	CString sInstrumentName;
}INSTRUMENT_LISTITEM;

/////////////////////////////////////////////////////////////////////////////
// CDlgDLS dialog
class CDlgDLS : public CDialog
{
// Construction
public:
	CDlgDLS(CWnd* pParent = NULL);   // standard constructor
	~CDlgDLS();
// Dialog Data
	//{{AFX_DATA(CDlgDLS)
	enum { IDD = IDD_DLS };
	CListBox	m_cList;
	CComboBox	m_DLS_Combo;
	int		m_nLSB;
	int		m_nMSB;
	int		m_nPC;
	//}}AFX_DATA

	long					m_nTrack;
	CString					m_csCurrentCollection;
	IDMUSProdReferenceNode*	m_pICurrentRefNode;
	int						m_nLastSelectedCollection;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDLS)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDLS)
	afx_msg void OnDblclkDlslist();
	afx_msg void OnSelchangeDlslist();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCollectionsCombo();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	IDMUSProdReferenceNode* CreateReferenceNode(IDMUSProdNode* pCollectionDocRoot);
	void GetCurrentCollection(); 
	void InitCollectionList();
	void InitCollectionCombo();
	void InitInstrumentList();
	void CleanInstrumentList();
	void SortInstrumentsAndAddToList();
	void QuickSort(POSITION position, int nElems);
	void swap(POSITION& position1, POSITION& position2);

#ifdef _DEBUG
	void _PrintTrace(POSITION startPosition, int nElems); 
#endif

// Attributes
private:
	CTypedPtrList<CPtrList, COLLECTION_LISTITEM*>	m_CollectionList;
	CTypedPtrList<CPtrList, INSTRUMENT_LISTITEM*>	m_InstrumentList;
	IDMUSProdFramework*								m_pIFramework;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDLS_H__D1868D61_9F84_11D0_8C10_00A0C92E1CAC__INCLUDED_)
