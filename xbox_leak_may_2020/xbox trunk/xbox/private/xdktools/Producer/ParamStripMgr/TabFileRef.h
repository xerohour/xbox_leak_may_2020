#if !defined(AFX_TABFILEREF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_TABFILEREF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

// TabFileRef.h : header file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>

class CPropPageMgrItem;
class CTabOnReturnEdit;


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef dialog

class CTabFileRef : public CPropertyPage
{
	friend CPropPageMgrItem;
	DECLARE_DYNCREATE(CTabFileRef)

// Construction
public:
	CTabFileRef();
	~CTabFileRef();

// Dialog Data
	//{{AFX_DATA(CTabFileRef)
	enum { IDD = IDD_PROPPAGE_WAVE_FILEREF };
	CComboBox	m_comboFile;
	CEdit	m_editTick;
	CEdit	m_editBeat;
	CEdit	m_editMeasure;
	CSpinButtonCtrl	m_spinTick;
	CSpinButtonCtrl	m_spinBeat;
	CSpinButtonCtrl	m_spinMeasure;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabFileRef)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabFileRef)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditTick();
	afx_msg void OnKillfocusEditBeat();
	afx_msg void OnKillfocusEditMeasure();
	virtual void OnOK();
	afx_msg void OnDropDownComboFile();
	afx_msg void OnSelChangeComboFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetTrackItem( const CTrackItem * pItem );

protected:
	// File combo box methods
//	void GetFileComboBoxText( const FileListInfo* pFileListInfo, CString& strText );
//	void InsertFileInfoListInComboBox();
//	void InsertFileInfo( FileListInfo* pFileListInfo );
//	void BuildFileInfoList();
//	void SetFileComboBoxSelection();

	void UpdateObject();
	void EnableControls( BOOL fEnable );
	void HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal );
	void HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );

	IDMUSProdTimeline*		m_pTimeline;		// Weak timeline reference
	CPropPageMgrItem*		m_pPropPageMgr;

	CTrackItem				m_TrackItem;

	BOOL					m_fNeedToDetach;
	BOOL					m_fValidTrackItem;
	BOOL					m_fIgnoreSelChange;

//    CTypedPtrList<CPtrList, FileListInfo*> m_lstFileListInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFILEREF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
