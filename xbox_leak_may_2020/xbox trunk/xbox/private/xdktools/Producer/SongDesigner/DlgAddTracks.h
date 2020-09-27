#if !defined(AFX_DLGADDTRACKS_H__AF7EBB6B_B222_44FE_AC01_8A63376BCA1C__INCLUDED_)
#define AFX_DLGADDTRACKS_H__AF7EBB6B_B222_44FE_AC01_8A63376BCA1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddTracks.h : header file
//

#include "Track.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks dialog

class CDlgAddTracks : public CDialog
{
// Construction
public:
	CDlgAddTracks(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddTracks();

// Dialog Data
	//{{AFX_DATA(CDlgAddTracks)
	enum { IDD = IDD_DLG_ADD_TRACKS };
	CListBox	m_lstbxSegmentTracks;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddTracks)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void BuildSegmentTrackList();
	void FillSegmentTrackListBox();
	CFont* CreateFont();

public:
	CDirectMusicSong*	m_pSong;
	CTypedPtrList<CPtrList, CTrack*> m_lstTracks;

protected:
	CTypedPtrList<CPtrList, CTrack*> m_lstSegmentTracks;

	// Generated message map functions
	//{{AFX_MSG(CDlgAddTracks)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	afx_msg void OnDblClkListTracks();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDTRACKS_H__AF7EBB6B_B222_44FE_AC01_8A63376BCA1C__INCLUDED_)
