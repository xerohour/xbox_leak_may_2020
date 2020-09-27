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
class CPropTrackItem;
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
	CButton	m_btnLockLength;
	CButton	m_btnLockEnd;
	CComboBox	m_comboFile;
	CStatic	m_staticPromptGrid;
	CStatic	m_staticPromptTick;
	CStatic	m_staticPromptBeat;
	CStatic	m_staticPromptBar;
	CEdit	m_editStartTick;
	CEdit	m_editStartGrid;
	CEdit	m_editStartBeat;
	CEdit	m_editStartMeasure;
	CEdit	m_editEndTick;
	CEdit	m_editEndGrid;
	CEdit	m_editEndBeat;
	CEdit	m_editEndMeasure;
	CEdit	m_editLengthTick;
	CEdit	m_editLengthGrid;
	CEdit	m_editLengthBeat;
	CEdit	m_editLengthMeasure;
	CStatic	m_staticLengthNbrSamples;
	CEdit	m_editOffsetTick;
	CEdit	m_editOffsetGrid;
	CEdit	m_editOffsetBeat;
	CEdit	m_editOffsetMeasure;
	CStatic	m_staticOffsetNbrSamples;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartMeasure;
	CSpinButtonCtrl	m_spinEndTick;
	CSpinButtonCtrl	m_spinEndGrid;
	CSpinButtonCtrl	m_spinEndBeat;
	CSpinButtonCtrl	m_spinEndMeasure;
	CSpinButtonCtrl	m_spinLengthTick;
	CSpinButtonCtrl	m_spinLengthGrid;
	CSpinButtonCtrl	m_spinLengthBeat;
	CSpinButtonCtrl	m_spinLengthMeasure;
	CSpinButtonCtrl	m_spinOffsetTick;
	CSpinButtonCtrl	m_spinOffsetGrid;
	CSpinButtonCtrl	m_spinOffsetBeat;
	CSpinButtonCtrl	m_spinOffsetMeasure;
	CStatic	m_staticSourceEndTick;
	CStatic	m_staticSourceEndGrid;
	CStatic	m_staticSourceEndBeat;
	CStatic	m_staticSourceEndMeasure;
	CStatic	m_staticSourceLengthTick;
	CStatic	m_staticSourceLengthGrid;
	CStatic	m_staticSourceLengthBeat;
	CStatic	m_staticSourceLengthMeasure;
	CStatic	m_staticSourceLengthNbrSamples;
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
	afx_msg void OnSpinStartTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinOffsetTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinOffsetGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinOffsetBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinOffsetMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusStartTick();
	afx_msg void OnKillFocusStartGrid();
	afx_msg void OnKillFocusStartBeat();
	afx_msg void OnKillFocusStartMeasure();
	afx_msg void OnKillFocusEndTick();
	afx_msg void OnKillFocusEndGrid();
	afx_msg void OnKillFocusEndBeat();
	afx_msg void OnKillFocusEndMeasure();
	afx_msg void OnKillFocusLengthTick();
	afx_msg void OnKillFocusLengthGrid();
	afx_msg void OnKillFocusLengthBeat();
	afx_msg void OnKillFocusLengthMeasure();
	afx_msg void OnKillFocusOffsetTick();
	afx_msg void OnKillFocusOffsetGrid();
	afx_msg void OnKillFocusOffsetBeat();
	afx_msg void OnKillFocusOffsetMeasure();
	virtual void OnOK();
	afx_msg void OnDropDownComboFile();
	afx_msg void OnSelChangeComboFile();
	afx_msg void OnLockEnd();
	afx_msg void OnDoubleClickedLockEnd();
	afx_msg void OnLockLength();
	afx_msg void OnDoubleClickedLockLength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetPropTrackItem( const CPropTrackItem* pPropItem );

protected:
	// File combo box methods
	void GetFileComboBoxText( const FileListInfo* pFileListInfo, CString& strText );
	void InsertFileInfoListInComboBox();
	void InsertFileInfo( FileListInfo* pFileListInfo );
	void BuildFileInfoList();
	void SetFileComboBoxSelection();

	void UpdateObject();
	void EnableControls( BOOL fEnable );
	bool HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );
	bool HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset );
	void SetEditControl( CEdit& edit, long lValue, int nOffset );
	void SetStaticControl( CStatic& stat, long lValue, int nOffset );

	void SetStart( REFERENCE_TIME rtTime );
	void SetEnd( REFERENCE_TIME rtNewEnd );
	void SetLength( REFERENCE_TIME rtNewLength );
	void SetOffset( REFERENCE_TIME rtNewOffset );

	REFERENCE_TIME StartToUnknownTime();
	REFERENCE_TIME EndToUnknownTime();
	REFERENCE_TIME LengthToUnknownTime();
	REFERENCE_TIME OffsetToRefTime();

	REFERENCE_TIME MinTimeToRefTime();
	REFERENCE_TIME MinTimeToUnknownTime();

	REFERENCE_TIME RemoveFineTune( REFERENCE_TIME rtTime );
	
	void RecomputeTimes();
	void ComputeMinSecGridMs();
	void ComputeBarBeatGridTick();
	
	void SetControlRanges();
	void SetMusicTimeRanges( DWORD dwGroupBits );
	void SetRefTimeRanges();

	CPropPageMgrItem*			m_pPropPageMgr;
	IDMUSProdWaveTimelineDraw*	m_pIWaveTimelineDraw;

	CPropTrackItem				m_PropItem;

	BOOL		m_fNeedToDetach;
	BOOL		m_fValidTrackItem;
	BOOL		m_fIgnoreSelChange;

	long		m_lStartMeasure;
	long		m_lStartBeat;
	long		m_lStartGrid;
	long		m_lStartTick;

	long		m_lEndMeasure;
	long		m_lEndBeat;
	long		m_lEndGrid;
	long		m_lEndTick;

	long		m_lLengthMeasure;
	long		m_lLengthBeat;
	long		m_lLengthGrid;
	long		m_lLengthTick;
	DWORD		m_dwLengthNbrSamples;

	long		m_lOffsetMeasure;
	long		m_lOffsetBeat;
	long		m_lOffsetGrid;
	long		m_lOffsetTick;
	DWORD		m_dwOffsetNbrSamples;

	long		m_lSourceEndMeasure;
	long		m_lSourceEndBeat;
	long		m_lSourceEndGrid;
	long		m_lSourceEndTick;

	long		m_lSourceLengthMeasure;
	long		m_lSourceLengthBeat;
	long		m_lSourceLengthGrid;
	long		m_lSourceLengthTick;

    CTypedPtrList<CPtrList, FileListInfo*> m_lstFileListInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFILEREF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
