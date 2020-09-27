#if !defined(AFX_PROPPAGESEQTRACK_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_)
#define AFX_PROPPAGESEQTRACK_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPageSeqTrack.h : header file
//
// Also defined in NewPartDlg.h
#define MIN_PCHANNEL	1
#define MAX_PCHANNEL	999

#include "resource.h"
#include "NewPartDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPropPageSeqTrack dialog

interface IDMUSProdPropPageObject;
interface IDMUSProdPChannelName;
interface IDMUSProdPropSheet;

struct ioSeqTrackPPG
{
	DWORD dwPageIndex;
	DWORD dwPChannel;
	COLORREF crSelectedNoteColor;
	COLORREF crUnselectedNoteColor;
	COLORREF crOverlappingNoteColor;
	COLORREF crAccidentalColor;
	CString strName;
	CString strPChannelName;
	DWORD dwExtraBars;
	BOOL fPickupBar;
};

class CPropPageSeqTrack : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPageSeqTrack)

// Construction
public:
	CPropPageSeqTrack();
	~CPropPageSeqTrack();

// Dialog Data
	//{{AFX_DATA(CPropPageSeqTrack)
	enum { IDD = IDD_PROPPAGE_SEQ_TRACK };
	CStatic	m_staticPickup;
	CStatic	m_staticExtension;
	CEdit	m_editPChannelName;
	CEdit	m_editName;
	CSpinButtonCtrl	m_spinPChannel;
	CNewPartEdit	m_editPChannel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPageSeqTrack)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropPageSeqTrack)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditPchannel();
	afx_msg void OnKillfocusEditPchannel();
	afx_msg void OnButtonNotecolor();
	afx_msg void OnButtonSelectioncolor();
	afx_msg void OnButtonAccidentalcolor();
	afx_msg void OnButtonOverlapcolor();
	afx_msg void OnChangeEditPChannelName();
	afx_msg void OnChangeEditName();
	afx_msg void OnDeltaposSpinPchannel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonExt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableItem( int nID, BOOL fEnable );
	void EnableControls( BOOL fEnable );
	void UpdatePPO();
	CString GetPChannelName( DWORD dwPChannel );
	void SetExtPickupText( void );

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	// Variables for keeping track of the active property tab (if there is more than one)
	static short*				sm_pnActiveTab;
	IDMUSProdPropSheet*			m_pIPropSheet;

protected:
	IDMUSProdPropPageObject*	m_pPPO;
	DWORD						m_dwPChannel;
	BOOL						m_fNeedToDetach;
	COLORREF					m_crSelectedNoteColor;
	COLORREF					m_crUnselectedNoteColor;
	COLORREF					m_crOverlappingNoteColor;
	COLORREF					m_crAccidentalColor;
	COLORREF					m_pcrCustomColors[16];
	CString						m_strName;
	CString						m_strPChannelName;
	IDMUSProdPChannelName		*m_pIPChannelName;
	DWORD						m_dwExtraBars;
	BOOL						m_fPickupBar;

	CString						m_strBroadcastSeg;
	CString						m_strBroadcastPerf;
	CString						m_strBroadcastAPath;
	CString						m_strBroadcastGrp;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGESEQTRACK_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_)
