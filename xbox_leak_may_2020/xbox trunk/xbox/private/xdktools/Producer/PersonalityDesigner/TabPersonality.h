#if !defined(AFX_TABPERSONALITY_H__1E9289E2_F7AC_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABPERSONALITY_H__1E9289E2_F7AC_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabPersonality.h : header file
//

#include "Personality.h"

/////////////////////////////////////////////////////////////////////////////
// tabPersonality struct

#pragma pack(2)

typedef struct tabPersonality		// Used to pass data from Personality to page
{									// Used to pass data from page to Personality
	GUID		guidProject;
	CString		strName;
//	CString		strRefName;			// removed: reference name is now fixed.
	CString		strDescription;
	BOOL		fUseTimeSignature;
	BOOL		fVariableNotFixed;
	short		nBPM;
	short		nBeat;
	DWORD	dwKey;
	IDMUSProdNode*	pIStyleDocRootNode;
	DWORD		dwGroove;
	short		nChordMapLength;			// length of chord map
	long		lScalePattern;
	CPersonality*	pPersonality;
} tabPersonality;

#pragma pack()


class CPersonalityPageManager;

/////////////////////////////////////////////////////////////////////////////
// CTabPersonality dialog

class CTabPersonality : public CPropertyPage
{
// Construction
public:
	CTabPersonality( CPersonalityPageManager* pPageManager );
	~CTabPersonality();

// Dialog Data
	//{{AFX_DATA(CTabPersonality)
	enum { IDD = IDD_TAB_PERSONALITY };
	CStatic	m_staticTimeSig;
	CSpinButtonCtrl	m_spinkey;
	CSpinButtonCtrl	m_spinBPM;
	CEdit	m_editName;
	CEdit	m_editDescription;
	CComboBox	m_comboBeat;
	CEdit	m_editBPM;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabPersonality)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Member variables
protected:
	CPersonalityPageManager* m_pPageManager;

	BOOL		m_fNeedToDetach;
	BOOL		m_fHaveData;
	BOOL		m_fInOnSetActive;
	CString		m_strName;
//	CString		m_strRefName;
	CString		m_strDescription;
	BOOL		m_fUseTimeSignature;
	BOOL		m_fVariableNotFixed;
	short		m_nBPM;
	short		m_nBeat;
	short		m_nChordMapLength;
	DWORD	m_dwKey;
	long		m_lScalePattern;
// Implementation
protected:
	void SetTimeSignature();

public:
	void OnChangeRoot();
	void CopyDataToTab( tabPersonality* pTabData );
	void GetDataFromTab( tabPersonality* pTabData );

protected:
	void EnableControls( BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CTabPersonality)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeName();
	afx_msg void OnChangeRefName();
	afx_msg void OnChangeDescription();
	afx_msg void OnSelchangeComboBeat();
	afx_msg void OnChangeBPM();
	afx_msg void OnChangeLength();
	afx_msg void OnKillfocusName();
	afx_msg void OnDeltaposSpinkey(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUseflats();
	afx_msg void OnUsesharps();
	afx_msg void OnRadiofix();
	afx_msg void OnRadiovar();
	afx_msg void OnKillfocusRefName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABPERSONALITY_H__1E9289E2_F7AC_11D0_89AE_00A0C9054129__INCLUDED_)
