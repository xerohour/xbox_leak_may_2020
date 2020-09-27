#if !defined(AFX_LFODIALOG_H__044108C2_EBC5_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_LFODIALOG_H__044108C2_EBC5_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LFODialog.h : header file
//

#include "resource.h"
#include "myslider.h"
#include "DLSLoadSaveUtils.h"

/////////////////////////////////////////////////////////////////////////////
// CLFODialog dialog

class CArticulation;
class CCollection;

class CLFODialog : public CDialog, CSliderCollection
{
// Construction
public:
	CLFODialog(CArticulation* p_Articulation);
	void UpdateArticulation(CArticulation* pNewArt);

// Dialog Data
	//{{AFX_DATA(CLFODialog)
	enum { IDD = IDD_LFO_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLFODialog)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLFODialog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HRESULT			SaveUndoState(UINT uStringRes);
	void			CollectStatics();
	void			EnableDLS2Controls(BOOL bEnable = TRUE);
	void			UpdateInstrument();

	CPtrList		m_lstStatics;	// List of static controls in the dialog

	CCollection*	m_pCollection;	
	ArticParams*	m_pArticParams;
	LFOParams		m_LFOParams;
	CArticulation*	m_pArticulation;
    
	MySlider*		m_pmsLFOFrequency;
    MySlider*		m_pmsLFODelay;
    MySlider*		m_pmsLFOVolumeScale;
    MySlider*		m_pmsLFOPitchScale;
    MySlider*		m_pmsLFOMWToVolume;
    MySlider*		m_pmsLFOMWToPitch;
	MySlider*		m_pmsLFOChanPressToPitch;
	MySlider*		m_pmsLFOChanPressToGain;
	MySlider*		m_pmsLFOChanPressToFc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LFODIALOG_H__044108C2_EBC5_11D0_876A_00AA00C08146__INCLUDED_)
