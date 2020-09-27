#if !defined(AFX_VIBRATOLFODIALOG_H__EAEB7193_15B2_44DC_AF35_285187CC357F__INCLUDED_)
#define AFX_VIBRATOLFODIALOG_H__EAEB7193_15B2_44DC_AF35_285187CC357F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// vibratolfodialog.h : header file
//

#include "resource.h"
#include "myslider.h"
#include "DLSLoadSaveUtils.h"

/////////////////////////////////////////////////////////////////////////////
// CVibratoLFODialog dialog

class CArticulation;
class CCollection;

class CVibratoLFODialog : public CDialog, CSliderCollection
{
// Construction
public:
	CVibratoLFODialog(CArticulation* pArticulation);   

public:
	void UpdateArticulation(CArticulation* pNewArt);

// Dialog Data
	//{{AFX_DATA(CVibratoLFODialog)
	enum { IDD = IDD_VIBLFO_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVibratoLFODialog)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVibratoLFODialog)
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
    
	MySlider*		m_pmsLFOMWToPitch;
	MySlider*		m_pmsLFOFrequency;
    MySlider*		m_pmsLFODelay;
    MySlider*		m_pmsLFOPitchScale;
	MySlider*		m_pmsLFOChanPressToPitch;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIBRATOLFODIALOG_H__EAEB7193_15B2_44DC_AF35_285187CC357F__INCLUDED_)
