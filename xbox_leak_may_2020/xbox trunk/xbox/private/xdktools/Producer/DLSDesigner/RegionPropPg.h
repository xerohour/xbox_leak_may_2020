#if !defined(AFX_REGIONPROPPG_H__8C0AA7C6_E6FC_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_REGIONPROPPG_H__8C0AA7C6_E6FC_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
// RegionPropPg.h : header file
//

#include "resource.h"
#include "myslider.h"
#include "region.h"
#include "instrumentfveditor.h"

#define	MAX_WAVELINK_CHANNELS	18

class CRegion;

/////////////////////////////////////////////////////////////////////////////
// CRegionPropPg dialog

class CRegionPropPg : public CPropertyPage, CSliderCollection
{
    friend class CInstrumentFVEditor;

	DECLARE_DYNCREATE(CRegionPropPg)

// Construction
public:
	CRegionPropPg();
	~CRegionPropPg();

	void SetObject(CRegion* pRegion) {m_pRegion = pRegion;}
	
	void EnableControls(BOOL fEnable);
	void EnablePhaseGroupControls(bool bNewState);
	void EnablePhaseGroupEdit(bool bNewState);
	void EnableChannelCombo(bool bNewState);
	void EnableFineTune(bool bNewState);
	void EnableAttenuation(bool bNewState);
	void EnableRootNote(bool bNewState);
    void RevertToWaveRootNote(void);
    void RevertToWaveAttenuation(void);
    void RevertToWaveFineTune(void);
	void UpdateRootNote(USHORT usNewNote);
	void UpdateRootNoteText(int note);
	USHORT GetNoteInEditBox();
	void SetRootNote(USHORT usNewNote);


// Dialog Data
	//{{AFX_DATA(CRegionPropPg)
	enum { IDD = IDD_REGION_PROP_PAGE };
	CButton	m_MultichannelCheck;
	CComboBox	m_ChannelCombo;
	CSpinButtonCtrl	m_PhaseGroupSpin;
	CEdit	m_PhaseGroupEdit;
	CButton	m_PhaseGroupCheck;
	CButton	m_MasterCheck;
	CEdit	m_RootNoteEdit;
	CSpinButtonCtrl	m_RootNoteSpin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegionPropPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRegionPropPg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnOwsActiveAttenuation();
	afx_msg void OnOwsActiveFineTune();
	afx_msg void OnOwsActiveRootNote();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnKillfocusOwsDunitynote();
	afx_msg void OnDeltaposOwsRootNodeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPhaseGroupCheck();
	afx_msg void OnKillfocusPhaseGroupEdit();
	afx_msg void OnDeltaposPhaseGroupSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMasterCheck();
	afx_msg void OnSelchangeChannelCombo();
	afx_msg void OnMultichannelCheck();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void	InitPhaseGroupControls();
	void	InitChannelCombo();
	void	SetRegionWaveLink(CRegion* pRegion, WAVELINK waveLink);
	void	SaveUndoState(UINT uUndoID);
	void	UpdateOwsDattenuation();
	void	UpdateOwsDtune();

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);

private:
    long		m_lFineTune;
    long		m_lAttenuation;

	MySlider*	m_pmsTune;
    MySlider*	m_pmsAttenuation;

	CRegion*	m_pRegion;

	bool		m_fInOnInitialUpdate;	
	BOOL		m_fActivateFineTune;
	BOOL		m_fActivateAttenuation;
	BOOL		m_fActivateRootNote;
	CMenu*		m_pContextMenu;
	BOOL		m_fNeedToDetach;

	static UINT	m_arrChannelPositions[MAX_WAVELINK_CHANNELS];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONPROPPG_H__8C0AA7C6_E6FC_11D0_876A_00AA00C08146__INCLUDED_)
