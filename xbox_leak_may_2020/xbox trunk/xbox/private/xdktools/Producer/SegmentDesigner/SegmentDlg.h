#if !defined(AFX_SEGMENTDLG_H__A4AB9403_DDF9_11D0_BAC5_00805F493F43__INCLUDED_)
#define AFX_SEGMENTDLG_H__A4AB9403_DDF9_11D0_BAC5_00805F493F43__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SegmentDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg dialog

#include "SegmentCtl.h"
#include "SegmentComponent.h"
#include "resource.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )

class CSegmentCtrl;
class CTrack;
interface IDMUSProdStrip;
interface IDMUSProdTransport;
interface IDMUSProdTimeline;

class CSegmentDlg : public CDialog
{
// Construction
public:
	CSegmentDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSegmentDlg();

// Dialog Data
	//{{AFX_DATA(CSegmentDlg)
	enum { IDD = IDD_SEGMENT_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CSegmentCtrl		*m_pSegmentCtrl;
	IDMUSProdTimeline	*m_pTimeline;
	BOOL				m_fActive;
protected:
	IDMUSProdTransport	*m_pITransport;
	CSegment			*m_pSegment;
	UINT				m_nTimerID;
	IDMUSProdStrip		*m_pActiveStripWhenWindowDeactivated;

// Operations
public:

	void	SetTimelineLength(int iLength);
	void	RefreshTimeline();
	HRESULT	GetTimeline(IDMUSProdTimeline **ppTimeline);
	void	EnableTimer( BOOL fEnable );
	HRESULT AddTrack( CTrack* pTrack );
	HRESULT RemoveTrack( CTrack* pTrack );
	void	UpdateZoomAndScroll( void );

	void	Activate( BOOL fActive );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSegmentDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	LRESULT OnApp(WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTDLG_H__A4AB9403_DDF9_11D0_BAC5_00805F493F43__INCLUDED_)
