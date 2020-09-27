#if !defined(AFX_PROPPAGEVARSEED_H__CF76B035_AF34_4CC1_99E9_A5753AFFCF0B__INCLUDED_)
#define AFX_PROPPAGEVARSEED_H__CF76B035_AF34_4CC1_99E9_A5753AFFCF0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPageVarSeed.h : header file
//

#include "resource.h"

typedef struct 
{
	DWORD dwPageIndex;
	DWORD dwVariationSeed;
	BOOL fVariationSeedEnabled;
} ioVarSeedPPGData;

/////////////////////////////////////////////////////////////////////////////
// CPropPageVarSeed dialog

interface IDMUSProdPropPageObject;
interface IDMUSProdPropSheet;

class CPropPageVarSeed : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPageVarSeed)

// Construction
public:
	CPropPageVarSeed();
	~CPropPageVarSeed();

// Dialog Data
	//{{AFX_DATA(CPropPageVarSeed)
	enum { IDD = IDD_PROPPAGE_STYLE_SEED };
	CButton	m_checkEnableSeed;
	CButton	m_btnNewSeed;
	CButton	m_btnEditSeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPageVarSeed)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropPageVarSeed)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckEnableSeed();
	afx_msg void OnButtonEditSeed();
	afx_msg void OnButtonNewSeed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdatePPO();

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	// Variables for keeping track of the active property tab
	static short*				sm_pnActiveTab;
	IDMUSProdPropSheet*			m_pIPropSheet;

protected:
	IDMUSProdPropPageObject*	m_pPPO;
	BOOL						m_fSeedVariationsEnabled;
	DWORD						m_dwVariationSeed;
	BOOL						m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEVARSEED_H__CF76B035_AF34_4CC1_99E9_A5753AFFCF0B__INCLUDED_)
