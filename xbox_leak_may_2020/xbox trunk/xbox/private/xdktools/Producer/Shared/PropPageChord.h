#include "..\shared\keyboard.h"

#if !defined(AFX_PROPPAGECHORD_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGECHORD_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include "conductor.h"
// PropPageChord.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageChord dialog
class CChordPropPageMgr;

class PropPageChord : public CPropertyPage, public IDMUSProdMidiInCPt
{
	friend class CChordPropPageMgr;
	DECLARE_DYNCREATE(PropPageChord)

// Construction
public:
	PropPageChord();
	~PropPageChord();
	void DispatchKeyboardChange(CKeyboard* pKeyboard, short nKey);

// Dialog Data
	//{{AFX_DATA(PropPageChord)
	enum { IDD = IDD_CHORD_PROPPAGE };
	CKeyboard	m_cScale;
	CKeyboard	m_cChord;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageChord)
	public:
	virtual BOOL OnSetActive();
	virtual int DoModal();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	afx_msg long OnWMUser(WPARAM, LPARAM);
	// Generated message map functions
	//{{AFX_MSG(PropPageChord)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnNoteDownChordKeys(short nKey);
	afx_msg void OnChordDropoctave();
	afx_msg void OnChordFlat();
	afx_msg void OnChordFournotes();
	afx_msg void OnChordInvert();
	afx_msg void OnChangeChordName();
	afx_msg void OnDeltaposRootSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeRootText();
	afx_msg void OnNoteDownScaleKeys(short nKey);
	afx_msg void OnTest();
	afx_msg void OnChordMidiIn();
	afx_msg void OnScaleMidiIn();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void PreProcessPSP( PROPSHEETPAGE& psp, BOOL bWizard );
	void SetChord( CPropChord * pChord );

// IUnknown
public:
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

	// IDMUSProdMidiInCPt functions
	HRESULT STDMETHODCALLTYPE OnMidiMsg(REFERENCE_TIME rtTime, 
										BYTE bStatus, 
										BYTE bData1, 
										BYTE bData2);

private:
	DWORD	m_cRef;
	void DrawScaleNote(CPropChord* pChord, short nX);
	void SetCheckBox(CPropChord* pChord, DWORD dwUDFlag, UINT nDlgID, UINT nState);
	CPropChord				m_Chord;
	CChordPropPageMgr*		m_pPropPageMgr;
	IDMUSProdConductor*		m_pIConductor;
	// midi chord/scale input members
	void RegisterMidi();
	void UnRegisterMidi();
	DWORD m_dwCookie;		// may be used to distinguish what chord is receiving input (if necessary)
	enum {None, Chord, Scale };
	int	 m_fMidiInputTarget;
	DWORD m_cRegCount;
	BOOL m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGECHORD_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
