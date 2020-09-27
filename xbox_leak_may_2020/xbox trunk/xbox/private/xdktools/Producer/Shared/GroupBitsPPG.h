/************************************************************************
*                                                                       *
*   Copyright (c) 1998-2001 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#if !defined(AFX_GROUPBITSPPG_H__9D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_)
#define AFX_GROUPBITSPPG_H__9D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <staticproppagemanager.h>
#include <afxdlgs.h>
#include "GroupBitsPPGresource.h"

extern const GUID GUID_GroupBitsPPGMgr;


// GroupBitsPPG.h : header file
//

#define GROUPBITSPPG_INDEX 0

#define GROUPBITSPPG_GROUPBITS_RO 0x1

typedef struct _PPGTrackParams
{
	DWORD	dwPageIndex;
	DWORD	dwGroupBits;
	DWORD	dwPropPageFlags;
} PPGTrackParams;

/////////////////////////////////////////////////////////////////////////////
// CGroupBitsPropPageMgr

class CGroupBitsPropPageMgr : public CStaticPropPageManager
{
public:
	CGroupBitsPropPageMgr();
	~CGroupBitsPropPageMgr();
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
													 LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

protected:
	class CGroupBitsPPG*	m_pGroupBitsPPG;
	class CTrackFlagsPPG*	m_pTrackFlagsPPG;

public:
	static short			sm_nActiveTab;
};

/////////////////////////////////////////////////////////////////////////////
// CGroupBitsPPG dialog

class CGroupBitsPPG : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupBitsPPG)

// Construction
public:
	CGroupBitsPPG();
	~CGroupBitsPPG();

// Dialog Data
	//{{AFX_DATA(CGroupBitsPPG)
	enum { IDD = IDD_PROPPAGE_GROUP_BITS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupBitsPPG)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupBitsPPG)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdatePPO();
	void EnableItem(int nItem, BOOL fEnable);

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	// Variables for keeping track of the active property tab (if there is more than one)
	static short*				sm_pnActiveTab;
	IDMUSProdPropSheet*			m_pIPropSheet;

protected:
	IDMUSProdPropPageObject*	m_pPPO;
	PPGTrackParams				m_PPGTrackParams;
	BOOL						m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPBITSPPG_H__9D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_)
