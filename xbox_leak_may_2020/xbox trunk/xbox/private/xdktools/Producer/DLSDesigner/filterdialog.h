#if !defined(AFX_FILTERDIALOG_H__83A2C55F_3E61_4504_AA8C_7D052F440A6F__INCLUDED_)
#define AFX_FILTERDIALOG_H__83A2C55F_3E61_4504_AA8C_7D052F440A6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// filterdialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilterDialog dialog
#include "resource.h"
#include "myslider.h"
#include "DLSLoadSaveUtils.h"

class CArticulation;
class CCollection;

class CFilterDialog : public CDialog, CSliderCollection
{
// Construction
public:
	
	CFilterDialog(CArticulation* pArticulation);  

public:
	void UpdateArticulation(CArticulation* pNewArt);

// Dialog Data
	//{{AFX_DATA(CFilterDialog)
	enum { IDD = IDD_FILTER_PAGE };
	CButton	m_EnableFilterCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterDialog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnEnableFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HRESULT	SaveUndoState(UINT uStringRes);
	void	CollectStatics();
	void	InitSliders();
	void	EnableSliders(BOOL bEnable = TRUE); 
	void	EnableDLS2Controls(BOOL bEnable = TRUE);	// All controls are actually DLS2 in this case
	void	UpdateInstrument();

	CPtrList		m_lstStatics;	// List of static controls in the dialog

	CCollection*	m_pCollection;	
	ArticParams*	m_pArticParams;
	FilterParams	m_FilterParams;
	CArticulation*	m_pArticulation;

	MySlider*	m_pmsInitialFc;				// also used to test initialization of all sliders
	MySlider*	m_pmsInitialQ;
	MySlider*	m_pmsLFOToFc;
	MySlider*	m_pmsLFOCC1ToFc;
	MySlider*	m_pmsEG2ToFc;
	MySlider*	m_pmsKeyVelToFc;
	MySlider*	m_pmsKeyNumToFc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDIALOG_H__83A2C55F_3E61_4504_AA8C_7D052F440A6F__INCLUDED_)
