#if !defined(AFX_PROPPAGENOTE_H__8B971CDC_E747_11D0_BC07_00A0C922E6EB__INCLUDED_)
#define AFX_PROPPAGENOTE_H__8B971CDC_E747_11D0_BC07_00A0C922E6EB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <staticproppagemanager.h>
#include "PropNote.h"
#include "PropPageMgr.h"

// PropPageNote.h : header file
//

#define DIALOG_LEN				20

/*
//#define CH_OFFSET		(1 << 0)
//#define CH_GRIDSTART	(1 << 1)
#define CH_SCALEVALUE	(1 << 2)
#define CH_OCTAVE		(1 << 3)
#define CH_VALUE		(1 << 4)
#define CH_VELOCITY		(1 << 5)
//#define CH_MUSICVALUE	(1 << 6)
//#define CH_DURATION	(1 << 7)
#define CH_PLAYMODE		(1 << 8)
#define CH_TIMERANGE	(1 << 9)
#define CH_DURRANGE		(1 << 10)
#define CH_VELRANGE		(1 << 11)
#define CH_STARTBAR		(1 << 12)
#define CH_STARTBEAT	(1 << 13)
#define CH_STARTGRID	(1 << 14)
#define CH_STARTTICK	(1 << 15)
#define CH_ENDBAR		(1 << 16)
#define CH_ENDBEAT		(1 << 17)
#define CH_ENDGRID		(1 << 18)
#define CH_ENDTICK		(1 << 19)
#define CH_DURBAR		(1 << 20)
#define CH_DURBEAT		(1 << 21)
#define CH_DURGRID		(1 << 22)
#define CH_DURTICK		(1 << 23)
#define CH_CHORDBAR		(1 << 24)
#define CH_CHORDBEAT	(1 << 25)

#define CH_STARTBARBEATGRIDTICK (CH_STARTBAR | CH_STARTBEAT | CH_STARTGRID | CH_STARTTICK)
#define CH_ENDBARBEATGRIDTICK (CH_ENDBAR | CH_ENDBEAT | CH_ENDGRID | CH_ENDTICK)
#define CH_DURBARBEATGRIDTICK (CH_DURBAR | CH_DURBEAT | CH_DURGRID | CH_DURTICK)
*/

class CNotePropPageMgr;

class CMyEdit :
	public CEdit
{
	DECLARE_DYNCREATE(CMyEdit)
public:
	CMyEdit();

	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
};

/////////////////////////////////////////////////////////////////////////////
// PropPageNote dialog

class PropPageNote : public CPropertyPage
{
// Construction
public:
	PropPageNote( CNotePropPageMgr*	pPageManager );
	~PropPageNote();

// Dialog Data
	//{{AFX_DATA(PropPageNote)
	enum { IDD = IDD_PROPPAGE_NOTE };
	CEdit	m_editDiatonicOffset;
	CSpinButtonCtrl	m_spinDiatonicOffset;
	CEdit	m_editVelocity;
	CMyEdit	m_editStartTick;
	CEdit	m_editStartGrid;
	CEdit	m_editStartBeat;
	CEdit	m_editStartBar;
	CEdit	m_editScaleFunc;
	CEdit	m_editOctave;
	CEdit	m_editMidiNote;
	CEdit	m_editHumVel;
	CEdit	m_editHumStart;
	CEdit	m_editHumDuration;
	CMyEdit	m_editEndTick;
	CEdit	m_editEndGrid;
	CEdit	m_editEndBeat;
	CEdit	m_editEndBar;
	CMyEdit	m_editDurTick;
	CEdit	m_editDurGrid;
	CEdit	m_editDurBeat;
	CEdit	m_editDurBar;
	CEdit	m_editChordBeat;
	CEdit	m_editChordBar;
	CEdit	m_editAccidental;
	CSpinButtonCtrl	m_spinAccidental;
	CSpinButtonCtrl	m_spinScaleFunc;
	CComboBox	m_comboInversionId;
	CComboBox	m_comboPlayMode;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinEndGrid;
	CSpinButtonCtrl	m_spinDurGrid;
	CSpinButtonCtrl	m_spinDurTick;
	CSpinButtonCtrl	m_spinDurBeat;
	CSpinButtonCtrl	m_spinDurBar;
	CSpinButtonCtrl	m_spinVelocity;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartBar;
	CSpinButtonCtrl	m_spinOctave;
	CSpinButtonCtrl	m_spinMidiNote;
	CSpinButtonCtrl	m_spinHumVelocity;
	CSpinButtonCtrl	m_spinHumStart;
	CSpinButtonCtrl	m_spinHumDuration;
	CSpinButtonCtrl	m_spinEndTick;
	CSpinButtonCtrl	m_spinEndBeat;
	CSpinButtonCtrl	m_spinEndBar;
	CSpinButtonCtrl	m_spinChordBeat;
	CSpinButtonCtrl	m_spinChordBar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageNote)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropPageNote)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposNoteSpinMidinote(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeNoteEditDurtick();
	afx_msg void OnChangeNoteEditDurbeat();
	afx_msg void OnChangeNoteEditDurbar();
	afx_msg void OnChangeNoteEditHumduration();
	afx_msg void OnChangeNoteEditHumstart();
	afx_msg void OnChangeNoteEditHumvelocity();
	afx_msg void OnChangeNoteEditVelocity();
	afx_msg void OnChangeNoteEditOctave();
	afx_msg void OnChangeNoteEditDurgrid();
	afx_msg void OnChangeNoteEditChordbar();
	afx_msg void OnChangeNoteEditChordbeat();
	afx_msg void OnChangeNoteEditStartbar();
	afx_msg void OnChangeNoteEditStartbeat();
	afx_msg void OnChangeNoteEditStartgrid();
	afx_msg void OnChangeNoteEditStarttick();
	afx_msg void OnChangeNoteEditEndbar();
	afx_msg void OnChangeNoteEditEndbeat();
	afx_msg void OnChangeNoteEditEndgrid();
	afx_msg void OnChangeNoteEditEndtick();
	afx_msg void OnKillfocusNoteEditDurtick();
	afx_msg void OnKillfocusNoteEditDurgrid();
	afx_msg void OnKillfocusNoteEditDurbeat();
	afx_msg void OnKillfocusNoteEditDurbar();
	afx_msg void OnKillfocusNoteEditHumduration();
	afx_msg void OnKillfocusNoteEditHumstart();
	afx_msg void OnKillfocusNoteEditHumvelocity();
	afx_msg void OnKillfocusNoteEditVelocity();
	afx_msg void OnKillfocusNoteEditOctave();
	afx_msg void OnKillfocusNoteEditChordbar();
	afx_msg void OnKillfocusNoteEditChordbeat();
	afx_msg void OnKillfocusNoteEditStartbar();
	afx_msg void OnKillfocusNoteEditStartbeat();
	afx_msg void OnKillfocusNoteEditStartgrid();
	afx_msg void OnKillfocusNoteEditStarttick();
	afx_msg void OnKillfocusNoteEditEndbar();
	afx_msg void OnKillfocusNoteEditEndbeat();
	afx_msg void OnKillfocusNoteEditEndgrid();
	afx_msg void OnKillfocusNoteEditEndtick();
	afx_msg void OnSelchangeComboPlaymode();
	afx_msg void OnDeltaposNoteSpinHumduration(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinHumstart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboInversionId();
	afx_msg void OnChangeNoteEditScalefunc();
	afx_msg void OnDeltaposNoteSpinAccidental(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusNoteEditScalefunc();
	afx_msg void OnDeltaposNoteSpinChordbeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinDurbeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinDurgrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinDurtick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinEndbeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinEndgrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinEndtick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinStartbeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinStartgrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinStarttick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNoteSpinDiatonicOffset(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetNote( CPropNote* pPropNote );
	void GetNote( CPropNote* pNote );
	void SetParameters( struct PropPageNoteParams* pParam );
	void SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
	virtual BOOL OnKillActive();

protected:
	long	m_lMaxMeasures;		// maximum number of measures

	long	m_lMaxStartBeats;	// maximum number of beats in a mesaures
	long	m_lMaxStartGrids;	// maximum number of grids in a beat
	long	m_lMinStartClocks;	// maximum value for clocks parameter
	long	m_lMaxStartClocks;	// minimum value for clocks parameter

	long	m_lMaxEndBeats;		// maximum number of beats in a mesaures
	long	m_lMaxEndGrids;		// maximum number of grids in a beat
	long	m_lMinEndClocks;	// maximum value for clocks parameter
	long	m_lMaxEndClocks;	// minimum value for clocks parameter

	long	m_lMaxChordBeats;	// maximum number of beats in a mesaures

	DWORD	m_dwChanging;
	BOOL	m_fValidPropNote;
	BOOL	m_fAllDisabled;
	BOOL	m_fNeedToDetach;
	BOOL	m_fInUpdateControls;

	/*
	int		m_nMeasureWidth;	// width of the measure field in characters
	int		m_nBeatWidth;		// width of the beat field in characters
	int		m_nGridWidth;		// width of the grid field in characters
	int		m_nTickWidth;		// width of the tick field in characters
	*/

	CPropNote					m_PropNote;
	CNotePropPageMgr*			m_pPageManager;
	IDMUSProdPropPageObject*	m_pIPPO;

	void UpdatePPO( void ); // update the note's properties in m_pIPPO
	BOOL IsEditEmpty(CEdit* pEdit);
	void EnableItem(int nItem, BOOL fEnable);
	void EnableItem(int nItem, BOOL fEnable, long lValue);
	void DisableItem(int nItem);
	void SetItemWidth( int nID, int nWidth);
	long RangeTextToPos( LPCTSTR szText );
	void PosToRangeText( long lSpinPos, CString& strText );
	void FillInversionIdComoboBox( void );

	void ValidateHumDuration( LPCTSTR lpszTmp );
	void ValidateHumStart( LPCTSTR lpszTmp );
	void ValidateOctave( LPCTSTR lpszTmp );

	void HandleEditChange(CSpinButtonCtrl& spin, DWORD dwChg, long& lUpdateVal);
	void HandleKillFocus(CSpinButtonCtrl& spin,	DWORD dwChg, long& lUpdateVal);

	void HandleEditChangeByte(CSpinButtonCtrl& spin, DWORD dwChg, BYTE& bUpdateVal);
	void HandleKillFocusByte(CSpinButtonCtrl& spin,	DWORD dwChg, BYTE& bUpdateVal);

	void HandleDeltaChange(NMHDR* pNMHDR, LRESULT* pResult, DWORD dwChg, long& lUpdateVal );
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGENOTE_H__8B971CDC_E747_11D0_BC07_00A0C922E6EB__INCLUDED_)
