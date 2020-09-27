//{{AFX_INCLUDES()
#include "adsrenvelope.h"
//}}AFX_INCLUDES
#if !defined(AFX_ARTICULATIONEDITOR_H__D0620364_CB9E_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_ARTICULATIONEDITOR_H__D0620364_CB9E_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ArticulationEditor.h : header file
//

const int MAXDSRTIME = 40000;

/////////////////////////////////////////////////////////////////////////////
// CArticulationEditor form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "myslider.h"
#include "DLSLoadSaveUtils.h"
#include "collection.h"

class CArticulationEditor : public CFormView
{
friend class CArticulationCtrl;

protected:
	CArticulationEditor(CArticulationCtrl* parent = NULL);           // protected constructor used by dynamic creation

	DECLARE_DYNCREATE(CArticulationEditor)

// Form Data
private:
	CArticulationCtrl*		m_parent;
	ArticParams				m_ArticParams;
	LFOParams				m_LFOParams;
    MySlider			    m_msLFOFrequency;
    MySlider			    m_msLFODelay;
    MySlider				m_msLFOVolumeScale;
    MySlider				m_msLFOPitchScale;
    MySlider				m_msLFOMWToVolume;
    MySlider				m_msLFOMWToPitch;
	
	MSCParams   m_MSCParams;
    MySlider    m_msMSCDefaultPan;

	PEGParams   m_PEGParams;
    MySlider    m_msPEGVel2Attack;
    MySlider    m_msPEGKey2Decay;
	MySlider    m_msPEGRange;
	
	VEGParams   m_VEGParams;
    MySlider    m_msVEGVel2Attack;
    MySlider    m_msVEGKey2Decay;
    
	CCollection*	m_pCollection;

	//{{AFX_DATA(CArticulationEditor)
	enum { IDD = IDD_ARTICULATION };
	CADSREnvelope	m_VEGASDREnvelope;
	CADSREnvelope	m_PEGADSREnvelope;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArticulationEditor)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CArticulationEditor();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CArticulationEditor)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnADSREAttackTimeChangedPegAdsrenvelopectrl(long NewAttackTime);
	afx_msg void OnADSREReleaseTimeChangedPegAdsrenvelopectrl(long NewReleaseTime);
	afx_msg void OnADSREDecayTimeChangedPegAdsrenvelopectrl(long NewDecayTime);
	afx_msg void OnADSRESustainLevelChangedPegAdsrenvelopectrl(long NewSustainLevel);
	afx_msg void OnADSRESustainLevelChangedVegAdsrenvelopectrl(long NewSustainLevel);
	afx_msg void OnADSREReleaseTimeChangedVegAdsrenvelopectrl(long NewReleaseTime);
	afx_msg void OnADSREAttackTimeChangedVegAdsrenvelopectrl(long NewAttackTime);
	afx_msg void OnADSREDecayTimeChangedVegAdsrenvelopectrl(long NewDecayTime);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARTICULATIONEDITOR_H__D0620364_CB9E_11D0_876A_00AA00C08146__INCLUDED_)
