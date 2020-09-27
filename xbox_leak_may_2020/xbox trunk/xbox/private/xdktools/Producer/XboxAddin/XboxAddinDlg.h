// XboxAddinDlg.h : header file
//

#if !defined(AFX_XBOXADDINDLG_H__E04930FE_9CAB_489E_875F_CFCB21E9E933__INCLUDED_)
#define AFX_XBOXADDINDLG_H__E04930FE_9CAB_489E_875F_CFCB21E9E933__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vsplitter.h"
#include "XboxAddin.h"

class CFileItem;
class CLeftPaneDlg;
class CRightPaneDlg;
interface IDMUSProdNode;

/////////////////////////////////////////////////////////////////////////////
// CXboxAddinDlg dialog

class CXboxAddinDlg : public CDialog
{
// Construction
public:
	CXboxAddinDlg(CWnd* pParent = NULL);	// standard constructor
	~CXboxAddinDlg();

    void EndTrack( long lNewPos );
	bool IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode );
	HRESULT AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode );
	void HandleNotification( NOTIFICATION_TYPE notificationType, DWORD dwSegmentID, DWORD dwData1 );
	void CleanUpDisplay( void );
	bool IsFileInUse( CFileItem *pFileItem );
	virtual BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CXboxAddinDlg)
	enum { IDD = IDD_XBOXADDIN_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXboxAddinDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	CLeftPaneDlg		*m_pLeftPaneDlg;
    CRightPaneDlg       *m_pRightPaneDlg;

protected:
	HICON m_hIcon;
	CVSplitter			m_wndVSplitter;

    WORD                m_wSplitterPos;

    void UpdateListBoxPositions( void );
	void OnConnectionStateChanged( void );

	// Generated message map functions
	//{{AFX_MSG(CXboxAddinDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnFileClose();
	afx_msg void OnSetTargetDirectory();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnSynchronizeAll();
	afx_msg void OnSynchronizeAuto();
	afx_msg void OnAppAbout();
	afx_msg void OnXboxPanic();
	afx_msg void OnSetXboxName();
	//}}AFX_MSG
	afx_msg void OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu );
	LRESULT OnApp( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XBOXADDINDLG_H__E04930FE_9CAB_489E_875F_CFCB21E9E933__INCLUDED_)
