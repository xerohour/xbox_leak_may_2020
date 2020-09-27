#if !defined(AFX_DIALOGNEWPART_H__2CC1089D_64F9_11D1_8856_00C04FBF8D15__INCLUDED_)
#define AFX_DIALOGNEWPART_H__2CC1089D_64F9_11D1_8856_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogNewPart.h : header file
//

#include "resource.h"

class CMIDIMgr;

class CNewPartEdit :
	public CEdit
{
	DECLARE_DYNCREATE(CNewPartEdit)
public:
	CNewPartEdit();

	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
};

/////////////////////////////////////////////////////////////////////////////
// CDialogNewPart dialog

class CDialogNewPart : public CDialog
{
// Construction
public:
	CDialogNewPart(CWnd* pParent = NULL);   // standard constructor

	void SetTrack( int nTrack ); // Set the initial track number
	void SetMIDIMgr( CMIDIMgr *pMIDIMgr );

	class CDirectMusicPartRef	*m_pDMPartRef;
	class CDirectMusicPart		*m_pDMPart;

// Dialog Data
	//{{AFX_DATA(CDialogNewPart)
	enum { IDD = IDD_NEW_PART };
	CEdit	m_editPChName;
	CButton	m_btnExisting;
	CStatic	m_staticStatus;
	CNewPartEdit	m_editPChannel;
	CSpinButtonCtrl	m_spinPChannel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogNewPart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogNewPart)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNewpartEditPchannel();
	afx_msg void OnKillfocusNewpartEditPchannel();
	afx_msg void OnButtonExisting();
	afx_msg void OnButtonLink();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinPchannel(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int		m_nTrack;

	CString	m_strEmptyPChannel;
	CString m_strSinglePChannel;
	CString m_strMultiplePChannel;
	CString	m_strInvalidPChannel;

	CString m_strDisplayPartRef;
	CString m_strChoosePartRef;

	CString m_strBroadcastSeg;
	CString m_strBroadcastPerf;
	CString m_strBroadcastAPath;
	CString m_strBroadcastGroup;

	CMIDIMgr *m_pMIDIMgr;

	void UpdatePChannelStatus( void );
	int NumberOfPartRefs( DWORD dwPChannel );
	int GetFirstUsedPChannel( void );
	void DoCreatePart( int nButtonClicked );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGNEWPART_H__2CC1089D_64F9_11D1_8856_00C04FBF8D15__INCLUDED_)
