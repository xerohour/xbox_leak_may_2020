#if !defined(AFX_TIMESIGNATUREDLG_H__B2B7FA23_73ED_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TIMESIGNATUREDLG_H__B2B7FA23_73ED_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimeSignatureDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CTimeSignatureDlg dialog

class CTimeSignatureDlg : public CDialog
{
// Construction
public:
	CTimeSignatureDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimeSignatureDlg)
	enum { IDD = IDD_TIME_SIGNATURE };
	CButton	m_btnOK;
	CComboBox	m_cmbxGrid;
	CSpinButtonCtrl	m_spinBPM;
	CEdit	m_editBPM;
	CComboBox	m_cmbxBeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeSignatureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	DirectMusicTimeSig	m_TimeSignature;
	UINT				m_nContext;

protected:
	BOOL ValidateBeatsPerMeasure();
	BOOL ValidateBeat();
	BOOL ValidateGridsPerBeat();

	// Generated message map functions
	//{{AFX_MSG(CTimeSignatureDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelChangeBeat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMESIGNATUREDLG_H__B2B7FA23_73ED_11D1_89AE_00A0C9054129__INCLUDED_)
