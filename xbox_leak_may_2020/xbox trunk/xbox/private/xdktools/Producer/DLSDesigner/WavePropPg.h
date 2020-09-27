#if !defined(AFX_WAVEPROPPG_H__59101182_E95D_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_WAVEPROPPG_H__59101182_E95D_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WavePropPg.h : header file
//

#include "resource.h"
#include "myslider.h"
#include "wave.h"

#define DM_VALIDATE (WM_USER + 1000)

class CWave;
class CWavePropPgMgr;

/////////////////////////////////////////////////////////////////////////////
// CWavePropPg dialog

class CWavePropPg : public CPropertyPage, CSliderCollection
{
	DECLARE_DYNCREATE(CWavePropPg)

// Construction
public:
	CWavePropPg();
	~CWavePropPg();

	virtual BOOL OnInitDialog();

	void SetObject(CWave* pWave);
	

	void	SetPropMgr(CWavePropPgMgr* pPropMgr) {m_pPropMgr = pPropMgr;}
	void	EnableControls(void);
    void	EnableLoopControls(BOOL bEnable);
	void	EnableSelectionControls(BOOL bEnable);
    void	InitializeDialogValues();
	HRESULT ValidateAndSetLoopValues(DWORD dwLoopStart, DWORD dwLoopLength, bool bSaveUndoState = true);
	void	SetSelection(int nSelectionStart = 0, int nSelectionEnd = 0, bool bSetEditor = true, bool bDoSnap = true);
	bool	IsSelectionLengthLocked();
	bool	IsLoopLengthLocked();
	void	SetSelectionLengthLock(bool bLock);
	void	SetWaveLoopType(ULONG ulLoopType);
		
// Dialog Data
	//{{AFX_DATA(CWavePropPg)
	enum { IDD = IDD_WAVE_PROP_PAGE };
	CSpinButtonCtrl	m_SelectionStartSpin;
	CSpinButtonCtrl	m_SelectionLengthSpin;
	CButton	m_SelectionLengthLockCheck;
	CSpinButtonCtrl	m_SelectionEndSpin;
	CSpinButtonCtrl	m_LoopStartSpin;
	CSpinButtonCtrl	m_LoopLengthSpin;
	CButton	m_LoopLengthLockCheck;
	CButton	m_LoopCheck;
	CSpinButtonCtrl	m_LoopEndSpin;
	CEdit	m_LoopEndEdit;
	CEdit	m_LoopStartEdit;
	CEdit	m_LoopLengthEdit;
	CEdit	m_RootNoteEdit;
	CSpinButtonCtrl	m_RootNoteSpin;
	BOOL	m_fAllowCompression;
	BOOL	m_fAllowTruncation;
	DWORD	m_dwLength;
	//}}AFX_DATA

	DWORD	m_dwLoopLength;
	DWORD	m_dwLoopStart;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWavePropPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWavePropPg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusRootNote();
	afx_msg void OnKillfocusLoopLength();
	afx_msg void OnKillfocusLoopStart();
	afx_msg void OnSampleTruncation();
	afx_msg void OnSampleCompression();
	afx_msg LRESULT OnValidate(UINT wParam, long lParam);
	afx_msg void OnDeltaposRootNoteSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLoopCheck();
	afx_msg void OnKillfocusLoopEnd();
	afx_msg void OnDeltaposLoopEndSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLoopLengthLockCheck();
	afx_msg void OnDeltaposLoopLengthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposLoopStartSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusSelectionEnd();
	afx_msg void OnDeltaposSelectionEndSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusSelectionLength();
	afx_msg void OnSelectionLengthLockCheck();
	afx_msg void OnDeltaposSelectionLengthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusSelectionStart();
	afx_msg void OnDeltaposSelectionStartSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnForwardLoopRadio();
	afx_msg void OnLoopAndReleaseRadio();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEditChange( UINT nEditControl );
	enum { fLOOP_START = 0x01,	fLOOP_LENGTH = 0x02, fROOT_NOTE = 0x04 };

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);

private:
	// update region properties methods
    void	UpdateAttenuationInRegions(void);
    void	UpdateFineTuneInRegions(void);
    void	UpdateSampleTruncationInRegions(void);
    void	UpdateSampleCompressionInRegions(void);
    void	UpdateRootNoteInRegions(void);
    void	UpdatePlaybackSettingsInRegions(void);
	void	UpdateDbAttenuation();
	void	UpdateFineTune();
	bool	ValidateLoopLength();
	bool	ValidateLoopStart();
	HRESULT	UpdateRootNote(USHORT usNewNote);
	void	UpdateRootNoteText(int note);
	USHORT	GetNoteInEditBox();
	void	PostValidate(UINT nControl);
	void	EnableControl(UINT ID, BOOL bEnable = true);
	void	Swap(int& nSelectionStart, int& nSelectionEnd);
	void	EnableLoopTypeButtons(BOOL bEnable = TRUE);
	BOOL	CheckSliderForDefaultClick(MySlider& msSlider, CPoint point);

	/* ensures the given selection length fits within boundaries */
	void ConstrainSelectionLength(int& nSelLen);

	/* if necessary, snaps the given selection value in the given direction */
	void EnsureSnap(int& nSel, UINT nSnapDirection);

    LONG		m_lFineTune;
    LONG		m_lAttenuation;

	MySlider*	m_pmsFineTune;
    MySlider*	m_pmsAttenuation;

	BOOL	m_fNeedToDetach;
    BOOL	m_fOneShot;
	BOOL	m_fActivateLoop;
    BOOL	m_fInitialUpdate;
	DWORD	m_bfTouched;			// Bit field for "has control been changed by user?"
	bool	m_bTouchedByProgram;	// Did program call SetWindowText() on this control?
	bool	m_bLockLoopLength;		// Lock the loop length?
	bool	m_bLockSelectionLength;	// Lock the selection Length?
	int		m_nSelectionStart;		// Start of the wave selection; reflected on the wave editor, counted from decompressed start
	int		m_nSelectionEnd;		// End of the wave selection, counted from decompressed start
	bool	m_bInLoopUpdate;


    CWave*	m_pWave;

	CWavePropPgMgr*	m_pPropMgr;

	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEPROPPG_H__59101182_E95D_11D0_876A_00AA00C08146__INCLUDED_)
