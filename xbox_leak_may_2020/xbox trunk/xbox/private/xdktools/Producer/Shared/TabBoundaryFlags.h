#if !defined(AFX_TABBOUNDARYFLAGS_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABBOUNDARYFLAGS_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TabBoundaryFlags.h : header file
//

#include "DMUSProd.h"
#include "TabBoundaryFlagsResource.h"

#define PROPF_HAVE_VALID_DATA	0x00000001
#define PROPF_OMIT_SEGF_DEFAULT	0x00000002

typedef struct _PPGTabBoundaryFlags
{
	_PPGTabBoundaryFlags( void )
	{
		dwPageIndex = 2;
		dwPlayFlags = 0;
		dwFlagsUI = 0;
	}

	DWORD	dwPageIndex;
	DWORD	dwPlayFlags;
	DWORD	dwFlagsUI;
	CString strPrompt;
} PPGTabBoundaryFlags;


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags dialog

class CTabBoundaryFlags : public CPropertyPage
{
// Construction
public:
	CTabBoundaryFlags( IDMUSProdPropPageManager* pIPageManager );
	~CTabBoundaryFlags();

// Dialog Data
	//{{AFX_DATA(CTabBoundaryFlags)
	enum { IDD = IDD_TAB_BOUNDARY_FLAGS };
	CButton	m_radioInvalidate;
	CButton	m_radioInvalidatePri;
	CButton	m_radioNoInvalidate;
	CStatic	m_staticPrompt;
	CStatic	m_staticNoMarkers;
	CStatic	m_staticAlignPrompt;
	CButton	m_checkSegDefault;
	CButton	m_checkMarker;
	CComboBox	m_comboBoundary;
	CButton	m_radioSwitchOnBoundary;
	CButton	m_radioSwitchAnyTime;
	CButton	m_radioSwitchAnyGrid;
	CButton	m_radioSwitchAnyBeat;
	CButton	m_radioSwitchAnyBar;
	CButton	m_radioTimingQuick;
	CButton	m_radioTimingAfterPrepareTime;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabBoundaryFlags)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTabBoundaryFlags)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCheckMarker();
	afx_msg void OnDoubleClickedCheckMarker();
	afx_msg void OnCheckSegDefault();
	afx_msg void OnDoubleClickedCheckSegDefault();
	afx_msg void OnSelChangeComboBoundary();
	afx_msg void OnRadioSwitchAnyBar();
	afx_msg void OnDoubleClickedRadioSwitchAnyBar();
	afx_msg void OnRadioSwitchAnyBeat();
	afx_msg void OnDoubleClickedRadioSwitchAnyBeat();
	afx_msg void OnRadioSwitchAnyGrid();
	afx_msg void OnDoubleClickedRadioSwitchAnyGrid();
	afx_msg void OnRadioSwitchAnyTime();
	afx_msg void OnDoubleClickedRadioSwitchAnyTime();
	afx_msg void OnRadioSwitchNextBoundary();
	afx_msg void OnDoubleClickedRadioSwitchNextBoundary();
	afx_msg void OnRadioTimingAfterPrepareTime();
	afx_msg void OnDoubleClickedRadioTimingAfterPrepareTime();
	afx_msg void OnRadioTimingQuick();
	afx_msg void OnDoubleClickedRadioTimingQuick();
	afx_msg void OnRadioInvalidate();
	afx_msg void OnRadioInvalidatepri();
	afx_msg void OnRadioNoinvalidate();
	afx_msg void OnDoubleClickedRadioInvalidate();
	afx_msg void OnDoubleClickedRadioInvalidatepri();
	afx_msg void OnDoubleClickedRadioNoinvalidate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdateObject();
	void HandlePlayFlags();

public:
	void UseTransitionPrompts( BOOL fUseTransitionPrompts );
	void SetObject( IDMUSProdPropPageObject* pPropPageObject );
	void RefreshTab( void );

	// Variables for keeping track of the active property tab (if there is more than one)
	static short*				sm_pnActiveTab;
	IDMUSProdPropSheet*			m_rpIPropSheet;

protected:
	IDMUSProdPropPageManager*	m_pIPageManager;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	PPGTabBoundaryFlags			m_PPGTabBoundaryFlags;
	BOOL						m_fNeedToDetach;
	BOOL						m_fUseTransitionPrompts;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBOUNDARYFLAGS_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
