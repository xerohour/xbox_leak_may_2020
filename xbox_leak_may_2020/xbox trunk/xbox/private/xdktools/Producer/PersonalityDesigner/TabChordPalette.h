#include "..\shared\Keyboard.h"

#if !defined(AFX_TABCHORDPALETTE_H__1DF95105_798A_11D1_984C_00805FA67D16__INCLUDED_)
#define AFX_TABCHORDPALETTE_H__1DF95105_798A_11D1_984C_00805FA67D16__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabChordPalette.h : header file
//

#include "Personality.h"
#include "TabPersonality.h"


/////////////////////////////////////////////////////////////////////////////
// CTabChordPalette dialog

class CTabChordPalette : public CPropertyPage
{

// Construction
public:
	CTabChordPalette(CPersonalityPageManager* pPageManager, IDMUSProdFramework* pIFramework );
	~CTabChordPalette();

public:
	void PreProcessPSP(PROPSHEETPAGE& psp, BOOL bWizard);
	void CopyDataToTab( tabPersonality* pTabData );
	void GetDataFromTab( tabPersonality* pTabData );
	void DispatchKeyboardChange(CKeyboard* pKeyboard, short nKey);

// Dialog Data
	//{{AFX_DATA(CTabChordPalette)
	enum { IDD = IDD_TAB_CHORDPALETTE };
	CKeyboard	m_scalekeys;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabChordPalette)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableControls(BOOL bEnable);
	void SetScale(long pattern);
	IDMUSProdFramework*			 m_pIFramework;
	CPersonalityPageManager* m_pPageManager;
	BOOL		m_fHaveData;
	BOOL		m_fInOnSetActive;
	long		m_lKey;	// read only! Used to set scales correctly
	bool			m_bUseFlats;	// so that m_dwKey is absolute key
	DWORD		m_lScalePattern;
	BOOL		m_fNeedToDetach;

	// Generated message map functions
	//{{AFX_MSG(CTabChordPalette)
	afx_msg void OnNoteDownScaleKeys(short nKey);
	afx_msg void OnNoteUpScaleKeys(short nKey);
	afx_msg void OnClear();
	afx_msg void OnExtendoctave();
	afx_msg void OnHarmonicminor();
	afx_msg void OnMajor();
	afx_msg void OnMelodicminor();
	afx_msg void OnShiftdown();
	afx_msg void OnShiftup();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABCHORDPALETTE_H__1DF95105_798A_11D1_984C_00805FA67D16__INCLUDED_)
