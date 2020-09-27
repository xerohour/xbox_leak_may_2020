#if !defined(AFX_PROPPAGEMELGENFLAGS_H__0EAB587D_2C7E_4CF4_8630_EBFB6444664D__INCLUDED_)
#define AFX_PROPPAGEMELGENFLAGS_H__0EAB587D_2C7E_4CF4_8630_EBFB6444664D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

struct ioMelGenFlagsPPG
{
	DWORD	dwPageIndex;
	DWORD	dwPlayMode;
};

// PropPageMelGenFlags.h : header file
//

interface IDMUSProdPropPageObject;
interface IDMUSProdPropSheet;

/////////////////////////////////////////////////////////////////////////////
// CPropPageMelGenFlags dialog

class CPropPageMelGenFlags : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPageMelGenFlags)

// Construction
public:
	CPropPageMelGenFlags();
	~CPropPageMelGenFlags();

// Dialog Data
	//{{AFX_DATA(CPropPageMelGenFlags)
	enum { IDD = IDD_PROPPAGE_MELGEN_FLAGS };
	CComboBox	m_PlaymodeDropDownList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPageMelGenFlags)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropPageMelGenFlags)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeComboPlaymodeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdatePPO();

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	// Variables for keeping track of the active property tab (if there is more than one)
	static short*				sm_pnActiveTab;
	IDMUSProdPropSheet*			m_pIPropSheet;

protected:
	IDMUSProdPropPageObject*	m_pPPO;
	ioMelGenFlagsPPG			m_PPGTrackParams;
	BOOL						m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEMELGENFLAGS_H__0EAB587D_2C7E_4CF4_8630_EBFB6444664D__INCLUDED_)
