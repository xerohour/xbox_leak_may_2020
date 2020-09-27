#if !defined(AFX_SIGNPOSTSTRIPPPG_H__8D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_)
#define AFX_SIGNPOSTSTRIPPPG_H__8D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <staticproppagemanager.h>
#include <afxdlgs.h>
#include "resource.h"

struct ioSignPostStripPPG
{
	DWORD	dwPageIndex;
	DWORD	dwActivityLevel;
};

// SignPostStripPPG.h : header file
//

class CKeyRootEdit :
	public CEdit
{
	DECLARE_DYNCREATE(CKeyRootEdit)
public:
	CKeyRootEdit();

	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
};

/////////////////////////////////////////////////////////////////////////////
// CSignPostStripPropPageMgr

class CSignPostStripPropPageMgr : public CStaticPropPageManager
{
friend class CSignPostStripPPG;
public:
	CSignPostStripPropPageMgr();
	~CSignPostStripPropPageMgr();
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
													 LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

	BOOL					m_fShowGroupBits;
	static short			sm_nActiveTab;

protected:
	class CSignPostStripPPG*	m_pSignPostStripPPG;
	class CGroupBitsPPG*	m_pGroupBitsPPG;
	class CTrackFlagsPPG*	m_pTrackFlagsPPG;
};

/////////////////////////////////////////////////////////////////////////////
// CSignPostStripPPG dialog

class CSignPostStripPPG : public CPropertyPage
{
	DECLARE_DYNCREATE(CSignPostStripPPG)

// Construction
public:
	CSignPostStripPPG();
	~CSignPostStripPPG();

// Dialog Data
	//{{AFX_DATA(CSignPostStripPPG)
	enum { IDD = IDD_PROPPAGE_AUDITION };
	CComboBox	m_comboActivityLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSignPostStripPPG)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSignPostStripPPG)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeActivitylevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdatePPO();

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	CSignPostStripPropPageMgr*		m_pPropPageMgr;

protected:
	IDMUSProdPropPageObject*	m_pPPO;
	BOOL						m_fNeedToDetach;

	// Audition data
	DWORD						m_dwActivityLevel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGNPOSTSTRIPPPG_H__8D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_)
