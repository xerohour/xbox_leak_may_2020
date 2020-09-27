#if !defined(AFX_PROPPAGEPLAY_H__D2E3AF92_F1C2_11D2_813F_00C04FA36E58__INCLUDED_)
#define AFX_PROPPAGEPLAY_H__D2E3AF92_F1C2_11D2_813F_00C04FA36E58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <afxtempl.h>

// PropPagePlay.h : header file
//


#define MELGEN_PLAY_MASK (DMUS_FRAGMENTF_ANTICIPATE | DMUS_FRAGMENTF_INVERT | DMUS_FRAGMENTF_REVERSE | DMUS_FRAGMENTF_SCALE | DMUS_FRAGMENTF_CHORD | DMUS_FRAGMENTF_USE_PLAYMODE)

/////////////////////////////////////////////////////////////////////////////
// PropPagePlay dialog
class CMelGenPropPageMgr;

class PropPagePlay : public CPropertyPage
{
	friend CMelGenPropPageMgr;
	DECLARE_DYNCREATE(PropPagePlay)

// Construction
public:
	PropPagePlay();
	~PropPagePlay();

// Dialog Data
	//{{AFX_DATA(PropPagePlay)
	enum { IDD = IDD_PLAY_OPTIONS_PROPPAGE };
	CListBox	m_listTransposeIntervals;
	CListBox	m_listPlayOptions;
	CComboBox	m_comboRepeatOptions;
	CComboBox	m_comboPlaymode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPagePlay)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropPagePlay)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPlaymode2();
	afx_msg void OnSelchangeComboRepeatOptions();
	afx_msg void OnSelchangeListPlayOptions();
	afx_msg void OnSelchangeListTranspose();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyDataToMelGen( CPropMelGen* pMelGen );
	void GetDataFromMelGen( CPropMelGen* pMelGen );

protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();

private:
	CPropMelGen*			m_pMelGen;
	CMelGenPropPageMgr*	m_pPropPageMgr;
	IDMUSProdFramework*		m_pIFramework;
	BOOL					m_fHaveData;
	BOOL					m_fMultipleMelGensSelected;
	BOOL					m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEPLAY_H__D2E3AF92_F1C2_11D2_813F_00C04FA36E58__INCLUDED_)
