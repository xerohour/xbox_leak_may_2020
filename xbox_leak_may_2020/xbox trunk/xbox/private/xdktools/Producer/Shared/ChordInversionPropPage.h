#if !defined(AFX_CHORDINVERSIONPROPPAGE_H__E4A7E136_B485_11D1_9875_00805FA67D16__INCLUDED_)
#define AFX_CHORDINVERSIONPROPPAGE_H__E4A7E136_B485_11D1_9875_00805FA67D16__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include "conductor.h"
#include "..\shared\keyboard.h"
#include "..\shared\invertvector.h"

// ChordInversionPropPage.h : header file
//

class BitFlag
{
	unsigned long	dw;
public:
	BitFlag() { dw = 0; }
	BitFlag(unsigned long l) { dw = l; }
	BitFlag& operator = (unsigned long l)
	{
		dw = l;
		return *this;
	}
	unsigned long GetBit(int bit)
	{
		return dw & (1 << bit);
	}
	void SetBit(int bit)
	{
		dw |= (1 << bit);
	}
	void ClearBit(int bit)
	{
		dw &= ~(1<<bit);
	}
	operator unsigned long()
	{
		return dw;
	}
};

class InversionState
{
	int m_state;
public:
	enum { Root=0, First, Second, Third, NStates };
	InversionState()
	{
		m_state = Root;
	}
	void Reset()
	{
		m_state = Root;
	}
	int Get() const { return m_state; }
	void Set(int m)
	{
		ASSERT(Root <= m && m <= Third);
		if(Root <=m && m <= Third)
		{
			m_state = m;
		}
	}
	int Rotate(int highestInvert = -1)
	{
		//highestInvert == # notes in chord
		//highestInvert == -1 -> no inversion limit
		//highestInvert == 0 -> only a single note--nothing to invert
		if(highestInvert > NStates)
			highestInvert = NStates;	// 3 inversions is limit
		if(highestInvert != 0)
		{
			m_state = (m_state + 1) % ((highestInvert==-1) ? NStates : highestInvert );
		}
		return m_state;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CChordInversionPropPage dialog
class CChordPropPageMgr;

class CChordInversionPropPage : public CPropertyPage
{
	friend class CChordPropPageMgr;
	DECLARE_DYNCREATE(CChordInversionPropPage)

// Construction
public:
	CChordInversionPropPage();
	~CChordInversionPropPage();
	enum { MAX_POLY = 4 , CHORDNOTE_DUR = 250, SCALENOTE_DUR = 250, CHORDNOTE_DELAY = 125, SCALENOTE_DELAY=125};

// Dialog Data
	//{{AFX_DATA(CChordInversionPropPage)
	enum { IDD = IDD_CHORDINVERSION_PROPPAGE };
	CKeyboard	m_keysinv4;
	CKeyboard	m_keysinv3;
	CKeyboard	m_keysinv2;
	CKeyboard	m_keysinv1;
	CKeyboard	m_keysbase4;
	CKeyboard	m_keysbase3;
	CKeyboard	m_keysbase2;
	CKeyboard	m_keysbase1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CChordInversionPropPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CChordInversionPropPage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonInvert1();
	afx_msg void OnButtonInvert2();
	afx_msg void OnButtonInvert3();
	afx_msg void OnButtonInvert4();
	afx_msg void OnButtonIplay1();
	afx_msg void OnButtonIplay2();
	afx_msg void OnButtonIplay3();
	afx_msg void OnButtonIplay4();
	afx_msg void OnCheckLegal1();
	afx_msg void OnCheckLegal2();
	afx_msg void OnCheckLegal3();
	afx_msg void OnCheckLegal4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void PlayChordInversion(int index);
	int GetInversionKeyboardIndex(const CKeyboard* pKeyboard);
	CKeyboard* GetInversionKeyboard(int index);
	int GetChordKeyboardIndex(const CKeyboard* pKeyboard);
	CKeyboard* GetChordKeyboard(int index);
	void SetChord(CPropChord* pChord, int SubChord = -1 /* default = all subchords */);
	void DrawInversionNote(CPropChord* pChord, DWORD dwChordInversion, int index, short nX);
	// IUnknown functions
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

	// data transfer
	void CopyDataToTab( CPropChord*  TabData );
	void GetDataFromTab( const CPropChord* TabData );


private:
	CBitmapButton	m_nextinvert4;
	CBitmapButton	m_nextinvert3;
	CBitmapButton	m_nextinvert2;
	CBitmapButton	m_nextinvert1;
	
	CBitmapButton	m_play4;
	CBitmapButton	m_play3;
	CBitmapButton	m_play2;
	CBitmapButton	m_play1;

	InversionState	m_invertStates[CPropChord::MAX_POLY];
	InvertVector	m_invertVector[CPropChord::MAX_POLY];
	
	DWORD			m_cRef;
	CPropChord		m_Chord;
	CChordPropPageMgr*	m_pPropPageMgr;
	IDMUSProdConductor*	m_pIConductor;
	bool	m_bValidChord;
	BOOL	m_fNeedToDetach;};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDINVERSIONPROPPAGE_H__E4A7E136_B485_11D1_9875_00805FA67D16__INCLUDED_)
