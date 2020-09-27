#if !defined(AFX_TABROUTINE_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_TABROUTINE_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

// TabRoutine.h : header file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>

class CPropPageMgrItem;
class CTabOnReturnEdit;

/////////////////////////////////////////////////////////////////////////////
// CTabRoutine dialog

class CTabRoutine : public CPropertyPage
{
	friend CPropPageMgrItem;
	DECLARE_DYNCREATE(CTabRoutine)

// Construction
public:
	CTabRoutine();
	~CTabRoutine();

// Dialog Data
	//{{AFX_DATA(CTabRoutine)
	enum { IDD = IDD_PROPPAGE_ROUTINE };
	CSpinButtonCtrl	m_spinLogicalMeasure;
	CSpinButtonCtrl	m_spinLogicalBeat;
	CEdit	m_editLogicalMeasure;
	CEdit	m_editLogicalBeat;
	CStatic	m_staticPromptTick;
	CStatic	m_staticPromptMeasure;
	CStatic	m_staticPromptBeat;
	CComboBox	m_comboScript;
	CComboBox	m_comboRoutine;
	CButton	m_radioTimingQuick;
	CButton	m_radioTimingBeforeTime;
	CButton	m_radioTimingAtTime;
	CSpinButtonCtrl	m_spinTick;
	CEdit	m_editTick;
	CEdit	m_editBeat;
	CEdit	m_editMeasure;
	CSpinButtonCtrl	m_spinMeasure;
	CSpinButtonCtrl	m_spinBeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabRoutine)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabRoutine)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditTick();
	afx_msg void OnKillfocusEditBeat();
	afx_msg void OnKillfocusEditRoutine();
	afx_msg void OnKillfocusEditMeasure();
	virtual void OnOK();
	afx_msg void OnDropDownComboScript();
	afx_msg void OnSelChangeComboScript();
	afx_msg void OnDropDownComboRoutine();
	afx_msg void OnSelChangeComboRoutine();
	afx_msg void OnTimingQuick();
	afx_msg void OnDoubleClickedTimingQuick();
	afx_msg void OnTimingBeforeTime();
	afx_msg void OnDoubleClickedTimingBeforeTime();
	afx_msg void OnTimingAtTime();
	afx_msg void OnDoubleClickedTimingAtTime();
	afx_msg void OnKillFocusComboRoutine();
	afx_msg void OnKillFocusEditLogicalBeat();
	afx_msg void OnKillFocusEditLogicalMeasure();
	afx_msg void OnDeltaPosSpinLogicalBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLogicalMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetItem( const CTrackItem * pItem );

protected:
	// Script combo box methods
	void GetScriptComboBoxText( const FileListInfo* pFileListInfo, CString& strText );
	void InsertScriptInfoListInComboBox();
	void InsertScriptInfo( FileListInfo* pFileListInfo );
	void BuildScriptInfoList();
	void SetScriptComboBoxSelection();

	// Routine combo box methods
	void FillRoutineComboBox();
	void SetRoutineComboBoxSelection();

	void UpdateObject( void );
	void EnableControls( BOOL fEnable );
	bool HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );
	bool HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset );
	void SetEditControl( CEdit& edit, long lValue, int nOffset );
	
	void SetControlRanges();
	void SetMusicTimeRanges( DWORD dwGroupBits );
	void SetRefTimeRanges();

	void SetStart( REFERENCE_TIME rtTime );
	REFERENCE_TIME StartToUnknownTime();
	REFERENCE_TIME MinTimeToUnknownTime();

	IDMUSProdTimeline*		m_pTimeline;		// Weak timeline reference
	CPropPageMgrItem*		m_pPropPageMgr;

	CTrackItem				m_TrackItem;

	BOOL					m_fNeedToDetach;
	BOOL					m_fValidRoutine;
	BOOL					m_fIgnoreSelChange;

    CTypedPtrList<CPtrList, FileListInfo*> m_lstFileListInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABROUTINE_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
