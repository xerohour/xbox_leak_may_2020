#if !defined(AFX_GROUPBITSPPG_H__9D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_)
#define AFX_GROUPBITSPPG_H__9D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <staticproppagemanager.h>
#include "resource.h"

typedef struct 
{
	DWORD	dwPageIndex;
	DWORD	dwGroupBits;
} ioGroupBitsPPGData;

// GroupBitsPPG.h : header file
//

#define ALLEXTRAS_FLAGS (DMUS_TRACKCONFIG_OVERRIDE_ALL | DMUS_TRACKCONFIG_OVERRIDE_PRIMARY | DMUS_TRACKCONFIG_FALLBACK | DMUS_TRACKCONFIG_CONTROL_ENABLED | \
	DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_NOTIFICATION_ENABLED | DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_PLAY_COMPOSE | \
	DMUS_TRACKCONFIG_LOOP_COMPOSE | DMUS_TRACKCONFIG_COMPOSING | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_CONTROL_NOTIFICATION | \
	DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART )

/////////////////////////////////////////////////////////////////////////////
// CGroupBitsPropPageMgr

class CGroupBitsPropPageMgr : CStaticPropPageManager
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
	enum { IDD = IDD_PROPPAGE_TIMESIG_GROUP };
	CComboBox	m_GroupDropDownList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupBitsPPG)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupBitsPPG)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeComboGroupList();
	afx_msg LRESULT OnApp(WPARAM, LPARAM);
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
	ioGroupBitsPPGData			m_PPGTrackParams;
	BOOL						m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPBITSPPG_H__9D655C81_CE44_11D1_88BC_00C04FBF8D15__INCLUDED_)
