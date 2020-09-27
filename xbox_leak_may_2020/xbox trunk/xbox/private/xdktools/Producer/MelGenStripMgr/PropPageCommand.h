#if !defined(AFX_PROPPAGECOMMAND_H__77240082_BACB_11D2_9801_00C04FA36E58__INCLUDED_)
#define AFX_PROPPAGECOMMAND_H__77240082_BACB_11D2_9801_00C04FA36E58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <afxtempl.h>

// PropPageCommand.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageCommand dialog
class CMelGenPropPageMgr;

class PropPageCommand : public CPropertyPage
{
	friend CMelGenPropPageMgr;
	DECLARE_DYNCREATE(PropPageCommand)

// Construction
public:
	PropPageCommand();
	~PropPageCommand();

// Dialog Data
	//{{AFX_DATA(PropPageCommand)
	enum { IDD = IDD_DIALOG_COMMAND_PROPPAGE };
	CSpinButtonCtrl	m_spinEmbCustom;
	CEdit	m_editEmbCustom;
	CSpinButtonCtrl	m_spinRange;
	CSpinButtonCtrl	m_spinLevel;
	CListBox	m_listEmbellishment;
	CEdit	m_editRange;
	CEdit	m_editLevel;
	CButton	m_checkNoLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageCommand)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropPageCommand)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckNoLevel();
	afx_msg void OnSelchangeListEmbellishment();
	afx_msg void OnChangeEditEmbCustom();
	afx_msg void OnChangeEditLevel();
	afx_msg void OnChangeEditRange();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyDataToMelGen( CPropMelGen* pMelGen );
	void GetDataFromMelGen( CPropMelGen* pMelGen );

protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();
    void CheckNoLevel();
    void UnCheckNoLevel();

private:
	CPropMelGen*			m_pMelGen;
	CMelGenPropPageMgr*	m_pPropPageMgr;
	IDMUSProdFramework*		m_pIFramework;
	BOOL					m_fHaveData;
	BOOL					m_fMultipleMelGensSelected;
	BOOL					m_fNeedToDetach;
    BYTE                    m_bEmbCustom;
    BYTE                    m_bLevel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGECOMMAND_H__77240082_BACB_11D2_9801_00C04FA36E58__INCLUDED_)
