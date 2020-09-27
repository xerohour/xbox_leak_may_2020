#if !defined(AFX_PROPPAGENOTEVAR_H__06A5F6C5_B758_11D1_89AF_00C04FD912C8__INCLUDED_)
#define AFX_PROPPAGENOTEVAR_H__06A5F6C5_B758_11D1_89AF_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropPageNoteVar.h : header file
//

#include "staticproppagemanager.h"
#include "PropNote.h"

class CNotePropPageMgr;

/////////////////////////////////////////////////////////////////////////////
// PropPageNoteVar dialog

class PropPageNoteVar : public CPropertyPage
{
// Construction
public:
	PropPageNoteVar( CNotePropPageMgr*	pPageManager );
	~PropPageNoteVar();

// Dialog Data
	//{{AFX_DATA(PropPageNoteVar)
	enum { IDD = IDD_PROPPAGE_NOTE_VARIATION };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageNoteVar)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropPageNoteVar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetNote( CPropNote* pPropNote );
	void GetNote( CPropNote* pPropNote );
	void SetParameters( struct PropPageNoteParams* pParam );
	void SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

protected:
	void UpdatePPO();
	void EnableItem(int nItem, BOOL fEnable);

protected:
	CPropNote			m_PropNote;
	CNotePropPageMgr*	m_pPageManager;
	IDMUSProdPropPageObject*	m_pIPPO;
	DWORD				m_dwVariation;
	BOOL				m_fAllDisabled;
	BOOL				m_fValidPropNote;
	BOOL				m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGENOTEVAR_H__06A5F6C5_B758_11D1_89AF_00C04FD912C8__INCLUDED_)
