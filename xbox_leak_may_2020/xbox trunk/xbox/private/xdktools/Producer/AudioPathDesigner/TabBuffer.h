#if !defined(AFX_TABBUFFER_H__527B1967_EB6A_435C_9C70_7559BF058E07__INCLUDED_)
#define AFX_TABBUFFER_H__527B1967_EB6A_435C_9C70_7559BF058E07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabBuffer.h : header file
//

#include "resource.h"
#include "BufferPPGMgr.h"
#include "Dlg3DParam.h"

interface IDMUSProdPropPageObject;

/////////////////////////////////////////////////////////////////////////////
// CTabBuffer dialog

class CTabBuffer : public CPropertyPage
{
friend CBufferPPGMgr;

	DECLARE_DYNCREATE(CTabBuffer)

// Construction
public:
	CTabBuffer();
	~CTabBuffer();

// Dialog Data
	//{{AFX_DATA(CTabBuffer)
	enum { IDD = IDD_TAB_BUFFER };
	CSpinButtonCtrl	m_spinVolume;
	CSpinButtonCtrl	m_spinPan;
	C3DEdit	m_editVolume;
	CEdit	m_editPan;
	CComboBox	m_comboStandardBuffer;
	CButton	m_checkShared;
	CButton	m_checkPredefined;
	CButton	m_btnEditGuid;
	CEdit	m_editChannels;
	CSpinButtonCtrl	m_spinChannels;
	CComboBox	m_combo3DAlg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabBuffer)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Access functions
public:
	void SetBuffer( BufferInfoForPPG* pBufferInfoForPPG, IDMUSProdPropPageObject* pINewPropPageObject );

// Attributes
protected:
	BufferInfoForPPG	m_BufferInfoForPPG;
	IDMUSProdPropPageObject* m_pIPropPageObject;
	CBufferPPGMgr	*m_pBufferPPGMgr;
	bool			m_fNeedToDetach;
	bool			m_fWasShared;

// Implementation
protected:
	void UpdateControls();
	void EnableControls( bool fEnable );
	void EnableUserControls( bool fEnable );
	void EnableDlgItem( int nItem, BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CTabBuffer)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckPredefined();
	afx_msg void OnSelchangeComboStandardBuffer();
	afx_msg void OnCheckShare();
	afx_msg void OnButtonGuid();
	afx_msg void OnSelchangeCOMBO3DAlg();
	afx_msg void OnDeltaposSpinChannels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditChannels();
	afx_msg void OnButton3dPos();
	afx_msg void OnKillfocusEditPan();
	afx_msg void OnKillfocusEditVolume();
	afx_msg void OnDeltaposSpinPan(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBUFFER_H__527B1967_EB6A_435C_9C70_7559BF058E07__INCLUDED_)
