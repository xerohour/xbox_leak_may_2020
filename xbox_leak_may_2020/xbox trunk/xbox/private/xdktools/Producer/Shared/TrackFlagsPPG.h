#if !defined(AFX_TRACKFLAGSPPG_H__EC7D0C20_F414_4138_8083_BB1A4A729ECD__INCLUDED_)
#define AFX_TRACKFLAGSPPG_H__EC7D0C20_F414_4138_8083_BB1A4A729ECD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrackFlagsPPG.h : header file
//

#include <afxdlgs.h>
#include "GroupBitsPPGresource.h"

typedef struct _PPGTrackFlagsParams
{
	DWORD	dwPageIndex;
	DWORD	dwTrackExtrasFlags;
	DWORD	dwTrackExtrasMask;
	DWORD	dwProducerOnlyFlags;
	DWORD	dwProducerOnlyMask;
} PPGTrackFlagsParams;

#define TRACKFLAGSPPG_INDEX 1

#define ALLEXTRAS_FLAGS (DMUS_TRACKCONFIG_OVERRIDE_ALL | DMUS_TRACKCONFIG_OVERRIDE_PRIMARY | DMUS_TRACKCONFIG_FALLBACK | DMUS_TRACKCONFIG_CONTROL_ENABLED | \
	DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_NOTIFICATION_ENABLED | DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_PLAY_COMPOSE | \
	DMUS_TRACKCONFIG_LOOP_COMPOSE | DMUS_TRACKCONFIG_COMPOSING | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_CONTROL_NOTIFICATION | \
	DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART )

interface IDMUSProdPropSheet;
interface IDMUSProdPropPageObject;

/////////////////////////////////////////////////////////////////////////////
// CTrackFlagsPPG dialog

class CTrackFlagsPPG : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrackFlagsPPG)

// Construction
public:
	CTrackFlagsPPG();
	~CTrackFlagsPPG();

// Dialog Data
	//{{AFX_DATA(CTrackFlagsPPG)
	enum { IDD = IDD_PROPPAGE_TRACK_FLAGS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrackFlagsPPG)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrackFlagsPPG)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnCheckAuditionOnly();
	afx_msg void OnCheckTransition();
	afx_msg void OnDoubleclickedCheckTransition();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdatePPO();
	void OnTransHelper( int nButtonID, DWORD dwFlag );
	void OnCheckHelper( int nButtonID, DWORD dwFlag );
	void EnableItem(int nItem, BOOL fEnable);

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	// Variables for keeping track of the active property tab (if there is more than one)
	static short*				sm_pnActiveTab;
	IDMUSProdPropSheet*			m_pIPropSheet;

protected:
	IDMUSProdPropPageObject*	m_pPPO;
	PPGTrackFlagsParams			m_PPGTrackFlagsParams;
	BOOL						m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKFLAGSPPG_H__EC7D0C20_F414_4138_8083_BB1A4A729ECD__INCLUDED_)
