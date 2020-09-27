#if !defined(AFX_CHORDSTRIPPPG_H__8D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_)
#define AFX_CHORDSTRIPPPG_H__8D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <staticproppagemanager.h>
#include <afxdlgs.h>
#include "resource.h"

struct ioChordStripPPG
{
	DWORD	dwPageIndex;
	BOOL	fUseFlats;
	int		nKeyRoot;
	int		nNumAccidentals;
};

// ChordStripPPG.h : header file
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
// CChordStripPropPageMgr

class CChordStripPropPageMgr : public CStaticPropPageManager
{
friend class CChordStripPPG;
public:
	CChordStripPropPageMgr();
	~CChordStripPropPageMgr();
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
	class CChordStripPPG*	m_pChordStripPPG;
	class CGroupBitsPPG*	m_pGroupBitsPPG;
	class CTrackFlagsPPG*	m_pTrackFlagsPPG;
};

/////////////////////////////////////////////////////////////////////////////
// CChordStripPPG dialog

class CChordStripPPG : public CPropertyPage
{
	DECLARE_DYNCREATE(CChordStripPPG)

// Construction
public:
	CChordStripPPG();
	~CChordStripPPG();

// Dialog Data
	//{{AFX_DATA(CChordStripPPG)
	enum { IDD = IDD_PROPPAGE_CHORDTRACK_KEY };
	CSpinButtonCtrl	m_spinKeyRoot;
	CSpinButtonCtrl	m_spinAccidentals;
	CEdit	m_editAccidentals;
	CKeyRootEdit	m_editKeyRoot;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CChordStripPPG)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CChordStripPPG)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioFlat();
	afx_msg void OnRadioSharp();
	afx_msg void OnChangeEditAccidentals();
	afx_msg void OnDeltaposSpinKeyRoot(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditKeyRoot();
	afx_msg void OnKillfocusEditAccidentals();
	afx_msg void OnChangeEditKeyRoot();
	afx_msg void OnDeltaposSpinAccidentals(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableItem( int nID, BOOL fEnable );
	void ShowItem( int nID, BOOL fEnable );
	void EnableControls( BOOL fEnable );
	void UpdatePPO();

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	CChordStripPropPageMgr*		m_pPropPageMgr;

protected:
	IDMUSProdPropPageObject*	m_pPPO;
	BOOL						m_fNeedToDetach;

	// Key Signature data
	BOOL						m_fUseFlats;
	int							m_nKeyRoot;
	int							m_nNumAccidentals;
	BOOL						m_fChangedKeyRoot;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDSTRIPPPG_H__8D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_)
