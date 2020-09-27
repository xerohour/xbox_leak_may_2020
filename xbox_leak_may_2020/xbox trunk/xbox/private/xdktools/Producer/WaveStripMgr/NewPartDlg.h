#if !defined(AFX_NEWPARTDLG_H__8E79CB36_EB82_11D2_A6E5_00105A26620B__INCLUDED_)
#define AFX_NEWPARTDLG_H__8E79CB36_EB82_11D2_A6E5_00105A26620B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewPartDlg.h : header file
//

#include "resource.h"

// Also defined in TabWaveTrack.h
#define MIN_PCHANNEL	1
#define MAX_PCHANNEL	999

class CTrackMgr;

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
// CNewPartDlg dialog

class CNewPartDlg : public CDialog
{
// Construction
public:
	CNewPartDlg(CWnd* pParent = NULL);   // standard constructor

	void	SetTrack( int nTrack ); // Set the initial track number
	void	SetTrackMgr( CTrackMgr *pTrackMgr );

// Dialog Data
	//{{AFX_DATA(CNewPartDlg)
	enum { IDD = IDD_NEW_PART };
	CStatic	m_staticStatus;
	CButton	m_btnOK;
	CNewPartEdit	m_editPChannel;
	CSpinButtonCtrl	m_spinPChannel;
	CEdit	m_editPChannelName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewPartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewPartDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNewpartEditPChannel();
	afx_msg void OnKillfocusNewpartEditPChannel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	long			m_lPChannel;
	CTrackMgr*		m_pTrackMgr;
	CString			m_strEmptyPChannel;
	CString			m_strUsedPChannel;
	CString			m_strInvalidPChannel;

	void UpdatePChannelName( void );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPARTDLG_H__8E79CB36_EB82_11D2_A6E5_00105A26620B__INCLUDED_)
