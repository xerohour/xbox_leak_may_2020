#if !defined(AFX_SEGMENTLENGTH_H__8813F648_6049_11D1_9844_00805FA67D16__INCLUDED_)
#define AFX_SEGMENTLENGTH_H__8813F648_6049_11D1_9844_00805FA67D16__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SegmentLength.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSegmentLength dialog

#define MIN_EXTRA_BARS		0
#define MAX_EXTRA_BARS		32767

interface IDMUSProdTimeline;

class CSegmentLength : public CDialog
{
// Construction
public:
	CSegmentLength(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSegmentLength)
	enum { IDD = IDD_LENGTH };
	CSpinButtonCtrl	m_spinSecond;
	CSpinButtonCtrl	m_spinMinute;
	CSpinButtonCtrl	m_spinMillisecond;
	CEdit	m_editSecond;
	CEdit	m_editMinute;
	CEdit	m_editMillisecond;
	CButton	m_checkPickup;
	CSpinButtonCtrl	m_spinExtLength;
	CEdit	m_editExtLength;
	CEdit	m_editLength;
	CSpinButtonCtrl	m_spinLength;
	//}}AFX_DATA

	DWORD				m_dwLength;
	REFERENCE_TIME		m_rtLength;
	BOOL				m_fClockTime;
	IDMUSProdTimeline	*m_pTimeline;
	DWORD				m_dwGroupBits;
	DWORD				m_dwNbrExtraBars;
	BOOL				m_fPickupBar;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentLength)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSegmentLength)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusEditlength();
	afx_msg void OnKillfocusExtLength();
	afx_msg void OnKillfocusEditMillisecond();
	afx_msg void OnKillfocusEditMinute();
	afx_msg void OnKillfocusEditSecond();
	afx_msg void OnDeltaposSpinMillisecond(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinMinute(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinSecond(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioClocktime();
	afx_msg void OnRadioMeasures();
	afx_msg void OnChangeEditMillisecond();
	afx_msg void OnChangeEditMinute();
	afx_msg void OnChangeEditSecond();
	afx_msg void OnChangeEditlength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateRefTimeConvenienceText();
	void UpdateMeasureConvenienceText();
	void EnableLengthControls();
	void UpdateReflengthControls();
	void UpdateReflengthValueAndConvenienceText();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTLENGTH_H__8813F648_6049_11D1_9844_00805FA67D16__INCLUDED_)
