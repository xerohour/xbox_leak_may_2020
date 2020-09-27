#if !defined(AFX_DLGADDREMOVEBUSES_H__19553BDD_5245_48A5_97EB_10978E4C04E6__INCLUDED_)
#define AFX_DLGADDREMOVEBUSES_H__19553BDD_5245_48A5_97EB_10978E4C04E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddRemoveBuses.h : header file
//

#include "resource.h"

#define MAX_AUDIO_CHANNELS 2

/////////////////////////////////////////////////////////////////////////////
// CDlgAddRemoveBuses dialog

class CDlgAddRemoveBuses : public CDialog
{
// Construction
public:
	CDlgAddRemoveBuses(CWnd* pParent = NULL);   // standard constructor

	CDWordArray	m_adwBusIDs;

// Dialog Data
	//{{AFX_DATA(CDlgAddRemoveBuses)
	enum { IDD = IDD_DLG_ADD_BUSES };
	CButton	m_btnUp;
	CButton	m_btnDown;
	CButton	m_btnAdd;
	CButton	m_btnRemove;
	CListBox	m_listInUse;
	CListBox	m_listAvailable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddRemoveBuses)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AddStringToAvailableList( int nStringID, DWORD dwBusID );
	void AddBusIDToUsedList( DWORD dwBusID );
	void SortUsedList( void );

	// Generated message map functions
	//{{AFX_MSG(CDlgAddRemoveBuses)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonReset();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	virtual void OnOK();
	afx_msg void OnDblclkListBusAvailble();
	afx_msg void OnDblclkListBusInuse();
	afx_msg void OnSelchangeListBusInuse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDREMOVEBUSES_H__19553BDD_5245_48A5_97EB_10978E4C04E6__INCLUDED_)
