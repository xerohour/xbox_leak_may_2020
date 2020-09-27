#if !defined(AFX_CHORDSCALEPROPPAGE_H__E4A7E135_B485_11D1_9875_00805FA67D16__INCLUDED_)
#define AFX_CHORDSCALEPROPPAGE_H__E4A7E135_B485_11D1_9875_00805FA67D16__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxext.h>
#include "resource.h"
#include "dmusici.h"
#include "conductor.h"
#include "..\shared\keyboard.h"
// ChordScalePropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChordScalePropPageData (used to transfer data to/from property tab)

class CChordScalePropPageData
{
public:
	CChordScalePropPageData()
	{
		m_pPropChord = NULL;
		m_fLockAllScales = TRUE;
		m_fSyncLevelOneToAll = TRUE;
	}
	
	~CChordScalePropPageData()
	{
	}

	CPropChord*			m_pPropChord;
	BOOL				m_fLockAllScales;
	BOOL				m_fSyncLevelOneToAll;
};


/////////////////////////////////////////////////////////////////////////////
// CChordScalePropPage dialog
class CChordPropPageMgr;

class CChordScalePropPage : public CPropertyPage, public IDMUSProdMidiInCPt
{
	friend class CChordPropPageMgr;
	DECLARE_DYNCREATE(CChordScalePropPage)

// Construction
public:
	enum { MAX_POLY = 4 , CHORDNOTE_DUR = 250, SCALENOTE_DUR = 250, CHORDNOTE_DELAY = 125, SCALENOTE_DELAY=25};
	CChordScalePropPage();
	~CChordScalePropPage();
// dispatch callbacks from keyboard controls
	void DispatchKeyboardChange(CKeyboard* pKeyboard, short nKey);

// Dialog Data
	//{{AFX_DATA(CChordScalePropPage)
	enum { IDD = IDD_CHORDSCALE_PROPPAGE };
	CSpinButtonCtrl	m_spinRoot;
	CEdit	m_chordname;
	CKeyboard	m_keysscale4;
	CKeyboard	m_keysscale3;
	CKeyboard	m_keysscale2;
	CKeyboard	m_keysscale1;
	CKeyboard	m_keyschord4;
	CKeyboard	m_keyschord3;
	CKeyboard	m_keyschord2;
	CKeyboard	m_keyschord1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CChordScalePropPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CChordScalePropPage)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnButtonMidichord4();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnButtonMidichord1();
	afx_msg void OnButtonMidichord2();
	afx_msg void OnButtonMidichord3();
	afx_msg void OnButtonMidiroot();
	afx_msg void OnButtonMidiscale1();
	afx_msg void OnButtonMidiscale2();
	afx_msg void OnButtonMidiscale3();
	afx_msg void OnButtonMidiscale4();
	afx_msg void OnButtonPlay1();
	afx_msg void OnButtonPlay2();
	afx_msg void OnButtonPlay3();
	afx_msg void OnButtonPlay4();
	afx_msg void OnButtonPlayall();
	afx_msg void OnChangeEditName();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnUseflats();
	afx_msg void OnUsesharps();
	afx_msg void OnDeltaposSpinRoot(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void WarnUser();
	void DispatchRightMouseKeyboard(CKeyboard* pKeyboard, short nKey, CPoint& ptMouse);
	HRESULT ChangePatch(BYTE ch, BYTE inst, BYTE msb, BYTE lsb);
	HRESULT PlayNote(BYTE ch, BYTE key, BYTE vel, DWORD startTime, DWORD dur);
	void PlayAllChords();
	void PlaySubChord(int index);
	void SetMidiIn(int keyboard);
	void SetChord(CPropChord* pChord);
	void SetChordData(CChordScalePropPageData* pChordScalePropPageData);
	void UpdateChordData();

	// IUnknown functions
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

	// IDMUSProdMidiInCPt functions
	HRESULT STDMETHODCALLTYPE OnMidiMsg(REFERENCE_TIME rtTime, 
										BYTE bStatus, 
										BYTE bData1, 
										BYTE bData2);

private:
	BOOL m_bNotWarn;
	enum {All = -1, Nop = 1000, CopyChord, CopyScale, CopyChordScale, ClearChordScale};	// chord to chord copy ops
	void ComputeCopyOp(WORD menuid, int& from, int& to, int& what);
	CButton* GetButton(int id);
	enum {None = -1, Chord1 = 0, Chord2 = 1, Chord3 = 2, Chord4 = 3, Scale1 = 4, Scale2 = 5, Scale3 = 6, Scale4 = 7, Root = 8};
	UINT	m_MidiInIds[MAX_POLY*2 + 1];
	int m_fMidiInputTarget;
	void OnChangeRoot( bool fUpdateChordData );
	void SetCheckBox(CPropChord* pChord, DWORD dwUDFlag, UINT nDlgID, UINT nState);
	void OnNoteDownChordKeys(CKeyboard* pKeyboard, int index, short nKey);
	void OnNoteDownScaleKeys(CKeyboard* pKeyboard, int index, short nKey);
//	void DrawScaleNote(CPropChord* pChord, int index, short nX);
	void DrawScale(CPropChord* pChord, int index, int note = -1 /* set this to do only one note */);
	int	GetChordKeyboardIndex(const CKeyboard* pKeyboard);
	int GetScaleKeyboardIndex(const CKeyboard* pKeyboard);
	int m_nLastKeyboardLevel;	// keyboard last rightclicked on (for copy ops)
	CKeyboard*		GetChordKeyboard(int index)
	{
		switch(index)
		{
		case 0:
			return &m_keyschord1;			
			break;
		case 1:
			return &m_keyschord2;
			break;
		case 2:
			return &m_keyschord3;
			break;
		case 3:
			return &m_keyschord4;
			break;
		default:
			return 0;
			break;
		}
	}
	CKeyboard*		GetScaleKeyboard(int index)
	{
		switch(index)
		{
		case 0:
			return &m_keysscale1;
			break;
		case 1:
			return &m_keysscale2;
			break;
		case 2:
			return &m_keysscale3;
			break;
		case 3:
			return &m_keysscale4;
			break;
		default:
			return 0;
			break;
		}
	}

	CBitmapButton	m_midichord4;
	CBitmapButton	m_midichord3;
	CBitmapButton	m_midichord2;
	CBitmapButton	m_midichord1;
	CBitmapButton* GetMidiChordButton(int index)
	{
		switch(index)
		{
		case 0:
			return &m_midichord1;			
			break;
		case 1:
			return &m_midichord2;
			break;
		case 2:
			return &m_midichord3;
			break;
		case 3:
			return &m_midichord4;
			break;
		default:
			return 0;
			break;
		}
	}

	CBitmapButton	m_midiscale4;
	CBitmapButton	m_midiscale3;
	CBitmapButton	m_midiscale2;
	CBitmapButton	m_midiscale1;
	CBitmapButton*	GetMidiScaleButton(int index)
	{
		switch(index)
		{
		case 0:
			return &m_midiscale1;
			break;
		case 1:
			return &m_midiscale2;
			break;
		case 2:
			return &m_midiscale3;
			break;
		case 3:
			return &m_midiscale4;
			break;
		default:
			return 0;
			break;
		}
	}

	
	CBitmapButton	m_midiroot;
	
	CBitmapButton	m_shiftl4;
	CBitmapButton	m_shiftl3;
	CBitmapButton	m_shiftl2;
	CBitmapButton	m_shiftl1;
	CBitmapButton* GetShiftLButton(int index)
	{
		switch(index)
		{
		case 0:
			return &m_shiftl1;
			break;
		case 1:
			return &m_shiftl2;
			break;
		case 2:
			return &m_shiftl3;
			break;
		case 3:
			return &m_shiftl4;
			break;
		default:
			return 0;
			break;
		}
	}

	CBitmapButton	m_shiftr4;
	CBitmapButton	m_shiftr3;
	CBitmapButton	m_shiftr2;
	CBitmapButton	m_shiftr1;
	CBitmapButton* GetShiftRButton(int index)
	{
		switch(index)
		{
		case 0:
			return &m_shiftr1;
			break;
		case 1:
			return &m_shiftr2;
			break;
		case 2:
			return &m_shiftr3;
			break;
		case 3:
			return &m_shiftr4;
			break;
		default:
			return 0;
			break;
		}
	}
	
	CBitmapButton	m_play4;
	CBitmapButton	m_play3;
	CBitmapButton	m_play2;
	CBitmapButton	m_play1;
	CBitmapButton* GetPlayButton(int index)
	{
		switch(index)
		{
		case 0:
			return &m_play1;
			break;
		case 1:
			return &m_play2;
			break;
		case 2:
			return &m_play3;
			break;
		case 3:
			return &m_play4;
			break;
		default:
			return 0;
			break;
		}
	}
		
void EnableControls(BOOL bEnable);

	CBitmapButton	m_playall;
	
	BOOL			m_fNeedToDetach;
	bool			m_bValidChord;
	
	CPropChord		m_Chord;
	BOOL			m_fLockAllScales;
	BOOL			m_fSyncLevelOneToAll;

	DWORD			m_cRef;
	CChordPropPageMgr*	m_pPropPageMgr;
	IDMUSProdConductor* m_pIConductor;
	IDirectMusicPerformance*	m_pIPerformance;
	void RegisterMidi();
	void UnRegisterMidi();
	DWORD m_dwCookie;		// may be used to distinguish what chord is receiving input (if necessary)
	DWORD m_cRegCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDSCALEPROPPAGE_H__E4A7E135_B485_11D1_9875_00805FA67D16__INCLUDED_)
