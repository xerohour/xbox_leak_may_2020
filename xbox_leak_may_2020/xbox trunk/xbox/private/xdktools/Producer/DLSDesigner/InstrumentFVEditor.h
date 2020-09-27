//{{AFX_INCLUDES()
#include "regionkeyboard.h"
//}}AFX_INCLUDES
#if !defined(AFX_INSTRUMENTFVEDITOR_H__8C0AA7C7_E6FC_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_INSTRUMENTFVEDITOR_H__8C0AA7C7_E6FC_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InstrumentFVEditor.h : header file
//

#include "resource.h"
#include "myslider.h"
#include "Conductor.h"
#include "RegionNote.h"

#define CF_DLS_ARTICULATION "DLS Articulation"
#define CF_DLS_REGION "DLS Region"

class CDLSStatic;

/////////////////////////////////////////////////////////////////////////////
// CInstrumentFVEditor form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#define DM_VALIDATE			(WM_USER + 1000)
#define DM_BRINGTOTOP		(DM_VALIDATE + 1)
#define DM_UPDATE_VALUES	(DM_BRINGTOTOP + 1)
#define	DM_REGION_SELECT	(DM_UPDATE_VALUES + 1)

#define AUDITION_SOLO		0
#define AUDITION_MULTIPLE	1

class CCollection;
class CCollectionWaves;
class CArticulation;
class CInstrument;
class CRegion;
class CWave;
class CInstrumentCtrl;
class CLFODialog;
class CPitchDialog;
class CVolDialog;
class CDLSEdit;

class CInstrumentFVEditor : public CFormView, public IPersistStream, public IDMUSProdMidiInCPt
{
friend class CArticulation;
friend class CRegion;
friend class CInstrument;
friend class CInstrumentCtrl;
friend class CInstrumentRegions;
friend class CLFODialog;
friend class CVibratoLFODialog;
friend class CPitchDialog;
friend class CVolDialog;
friend class CFilterDialog;

protected:
	CInstrumentFVEditor(CInstrumentCtrl *parent = NULL);	// protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInstrumentFVEditor)

// Form Data
public:
	//{{AFX_DATA(CInstrumentFVEditor)
	enum { IDD = IDD_INSTRUMENT };
	CButton	m_SoloLayerButton;
	CButton	m_FilterButton;
	CButton	m_VibLFOButton;
	CButton	m_ArtDLS1Check;
	CComboBox	m_RegionConditionCombo;
	CScrollBar	m_LayerScrollBar;
	CSpinButtonCtrl	m_ClickVelocitySpin;
	CEdit	m_ClickVelocityEdit;
	CSpinButtonCtrl	m_VelocityHighRangeSpin;
	CSpinButtonCtrl	m_VelocityLowRangeSpin;
	CEdit	m_VelocityHighRangeEdit;
	CEdit	m_VelocityLowRangeEdit;
	CButton	m_VolumeButton;
	CButton	m_LFOButton;
	CButton	m_PitchButton;
	CEdit	m_RootNoteEdit;
	CEdit	m_HighRangeEdit;
	CEdit	m_LowRangeEdit;
	CSpinButtonCtrl	m_ThruSpin;
	CSpinButtonCtrl	m_RangeSpin;
	CSpinButtonCtrl	m_RootNoteSpin;
	CSpinButtonCtrl	m_PatchSpin;
	CSpinButtonCtrl	m_MSBSpin;
	CSpinButtonCtrl	m_LSBSpin;
	UINT	m_wBank;
	UINT	m_wBank2;
	UINT	m_wPatch;
	BOOL	m_fIsDrumKit;
	CRegionKeyboard	m_RegionKeyBoard;
	BOOL	m_fAllowOverlap;
	BOOL	m_fUseInstArt;
	//}}AFX_DATA
	CDLSEdit*	m_pCDLSEditUpperRange;
	CDLSEdit*	m_pCDLSEditLowerRange;
	CDLSEdit*	m_pCDLSEditRootNote;
	CDLSEdit*	m_pCDLSEditMSB;
	CDLSEdit*	m_pCDLSEditLSB;
	CDLSEdit*	m_pCDLSEditPatch;
// Attributes
public:

// Operations
public:

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// IDMUSProdMidiInCPt functions
	HRESULT STDMETHODCALLTYPE OnMidiMsg(REFERENCE_TIME dwTime, 
										BYTE bStatus, 
										BYTE bData1, 
										BYTE bData2);

    // IPersist functions
    STDMETHOD(GetClassID)(CLSID* pClsId);

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentFVEditor)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL


// Implementation
protected:
	virtual ~CInstrumentFVEditor();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void RegisterMidi();
	void UnRegisterMidi();
	bool IsMidiRegistered();
	void RegionWaveChanged();
	void DeleteAndSetupWavesForCurrentRegion();
	void TurnOffMidiNotes();
	void UpdateRootNote(int nRootNote);
	void UpdateNoteRange(DWORD dwLowRange, DWORD dwHighRange, bool bSettingLowRange);
	void UpdateVelocityRange(DWORD dwLowRange, DWORD dwHighRange, bool bSettingLowRange);
	BOOL SendKeyToChildControl(UINT nChar, UINT nRepCnt, UINT nFlags);

	USHORT GetActiveLayer();
	CRegion* FindConflictingDLS1Region(USHORT usStartNote, USHORT usEndNote);

	CWnd* GetRegionKeyboardWnd();

private:
	/* adds the given wave as an entry in the region wavelink dropdown */
	void AddWaveToWaveLinkDropdown(CComboBox *pCombo, CWave *pWave);

	void SetupInstrument();
	void SetupRegion();
	void SetupArticulation();
	void CleanupArticluation();
	void SetCurRegion(CRegion* pRegion);
	void SetCurArticulation(CArticulation* pArticulation);
	void InitRegionConditionsCombo(CRegion* pRegion);
	void RefreshRegion();
	void SetRKBMap();
	CRegion* FindRegionFromMap(short nLayer, short nStartNote);
	CRegion* FindRegionFromMap(int nNote);
	void SendRegionChange();
    BOOL IsValidMidiNoteText(char * pszNote);
	void ValidatePatch();
	void ValidateBank();
	void ValidateBank2();
	void SetLayerScrollInfo(int nFirstVisibleLayer);

	void RegionChangeCommonTasks(bool bUpdateFramework=true);
	void ValidateFullPatch(UINT* pwValidationMember, UINT wOldValue, UINT uControlID);
	void Update_and_Download(UINT uCause);
	WORD CalculateAuditionNote();
	void SetAttachedNode();
	void UpdateUseInstArt();
	void SetArticulationStaticIcon(bool bInstrumentLevel);

	/* updates both shared and articulation dialog-specific controls to match m_pCurArticulation */
	void UpdateArticulationControls();

	HRESULT GetArticulationBounds(CRect& rcBounds);
	HRESULT GetControlPosition(UINT nID, CRect& rcPosition);

	void CollectStatics();
	CDLSStatic* GetStaticControl(UINT nID);

	void AddStringToCombo(CComboBox& combo, CString& sString);

	void SetAuditionRadioMode();

	/* refreshes the property page for the current region, if selected and displayed */
	void RefreshCurrentRegionPropertyPage();
	
	CPtrList m_lstStatics;	// List of static controls in the editor

	UINT		m_nCurrentArticulationDlg;
	CDialog*	m_pArtDialog;
	CRect		m_rcArtBounds;
	
	//enum { TAB_COUNT = 3, TAB_X_POS = 18 + 4, INST_SETTINGS_MAX_TEXT = 64, TAB_Y_POS = (235 + 21)};
	// Fixed Large Font problem of placing VolDialog etc. The tab control they are inserted into is
	// located at dialog unit coordinates (7,143). Changed TAB_X_POS and TAB_Y_POS
	// Added some constants for the rects of the 3 groups - instrument, region and articulation - for
	// right click tests

	enum { TAB_COUNT = 3, TAB_X_POS = 8, INST_SETTINGS_MAX_TEXT = 64, TAB_Y_POS = (150 + 25),
			INST_GROUP_X = 278, INST_GROUP_Y = 75, INST_GROUP_WIDTH = 125,INST_GROUP_HEIGHT = 70,
			RGN_GROUP_X = 2, RGN_GROUP_Y = 10,RGN_GROUP_WIDTH = 410, RGN_GROUP_HEIGHT = 93,
			ART_GROUP_X = 2, ART_GROUP_Y = 136, ART_GROUP_WIDTH = 314, ART_GROUP_HEIGHT = 176};

	CInstrumentCtrl*		m_parent;
	CInstrument*			m_pInstrument;
	CCollection*			m_pCollection;
	CDLSComponent*			m_pComponent;
	//CDialog*				m_Tabs[TAB_COUNT];
	int						m_currSelTab;
	bool					m_fInOnInitialUpdate;

	DWORD					m_dwLowerNoteRange;		// Start note for the current region
	DWORD					m_dwUpperNoteRange;		// End note for the current region
	DWORD					m_dwLowerVelocityRange;	// Lower limit of the velocity range for the current region
	DWORD					m_dwUpperVelocityRange; // Upper limit of the velocity range for the current region
	long					m_lUnityNote;	// Root Note

	CWave*					m_pWave;
	CCollectionWaves*		m_pWaves;
	CRegion*				m_pCurRegion;
	CArticulation*			m_pCurArticulation;
    DWORD					m_dwGroup;
	DWORD					m_dwCookie;
	UINT					m_nClickVelocity;

	USHORT					m_usActiveLayer;		// Keeps the active layer for region edits and auditions

	static int				m_nInstrumentEditors;
	static HICON			m_hInstrumentIcon;
	static HICON			m_hRegionIcon;
	static CMenu*			m_pContextInstMenu;
	static CMenu*			m_pContextRegionMenu;
	static CMenu*			m_pContextArtMenu;
	
	static CBitmap			m_bmpAttack;
	static CBitmap			m_bmpDecay;
	static CBitmap			m_bmpDecay2;
	static CBitmap			m_bmpSustain;
	static CBitmap			m_bmpRelease;
	static CBitmap			m_bmpRelease2;

	static HANDLE			m_hAttack;
	static HANDLE			m_hDecay ;
	static HANDLE			m_hDecay2;
	static HANDLE			m_hSustain;
	static HANDLE			m_hRelease;
	static HANDLE			m_hRelease2;


	UINT					m_cfFormatRegion;
	UINT					m_cfFormatArt;
	int						m_nStartNoteForCurRegion;
	int						m_nHasFocus;
	enum {fBANK = 0x0001, fBANK2 = 0x0002, fPATCH = 0x0004, fLOWER_RANGE = 0x0008,
		fUPPER_RANGE = 0x0010, fROOT_NOTE = 0x0020};
	DWORD					m_bfTouched;
	bool					m_bTouchedByProgram;
	bool					m_bTouchedBySpinner;
	DWORD					m_dwIgnorePatchConflicts;
		
	// Generated message map functions
	//{{AFX_MSG(CInstrumentFVEditor)
	afx_msg void OnDrums();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRegionSelectedChangedRegionRegionkeyboard(short nLayer, short nStartNote);
	afx_msg void OnRegionOverlap();
	afx_msg void OnRegionUia();
	afx_msg void OnSelchangeRegionWavelink();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnArticulationCopy();
	afx_msg void OnArticulationPaste();
	afx_msg void OnArticulationDelete();
	afx_msg void OnRegionDelete();
	afx_msg void OnInstrumentProperties();
	afx_msg void OnRegionProperties();
	afx_msg void OnInstEdRegionNewRegion();
	afx_msg void OnNewRegionRegionRegionkeyboard(short nLayer, long lower, long upper);
	afx_msg void OnRangeChangedRegionRegionkeyboard(short nLayer, long lower, long upper);
	afx_msg void OnKillfocusPatch();
	afx_msg void OnKillfocusBank();
	afx_msg void OnKillfocusBank2();
	afx_msg void OnSelchangeRegionGroup();
	afx_msg void OnUpdatePatch();
	afx_msg void OnDropdownRegionWavelink();
	afx_msg LRESULT OnValidate(UINT wParam,LONG lParam);
	afx_msg LRESULT OnBringToTop(UINT wParam,LONG lParam);
	afx_msg LRESULT OnUpdateMIDIValues(UINT wParam, LONG lPARAM);
	afx_msg LRESULT OnMIDIRegionSelect(UINT wParam, LONG lParam);
	afx_msg void OnChangeBank();
	afx_msg void OnChangeBank2();
	afx_msg void OnChangePatch();
	afx_msg void OnChangeLowerRange();
	afx_msg void OnChangeUpperRange();
	afx_msg void OnChangeRootNote();
	afx_msg void OnUpdateBank();
	afx_msg void OnUpdateBank2();
	afx_msg void OnDeltaPosMSBSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosLSBSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosPatchSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNotePlayedRegionRegionkeyboard(long nNote, BOOL bType);
	afx_msg void OnRegionMovedRegionRegionkeyboard(short nOldLayer, short nOldStartNote, short nMovedLayer, short nMovedStartNote, short nMovedEndNote);
	afx_msg void OnRegionDeletedRegionkeyboard(short nLayer, short nStartnote);
	afx_msg void OnKillfocusRegionElrange();
	afx_msg void OnKillfocusRegionEurange();
	afx_msg void OnKillfocusRegionRootNote();
	afx_msg void OnDeltaposRegionRootNoteSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposRegionThruSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposRegionRangeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusRegionElrange();
	afx_msg void OnSetfocusRegionEurange();
	afx_msg void OnSetfocusRegionRootNote();
	afx_msg void OnPitchButton();
	afx_msg void OnLfoButton();
	afx_msg void OnVolumeButton();
	afx_msg void OnKillfocusRegionVelocityElrange();
	afx_msg void OnKillfocusRegionVelocityEurange();
	afx_msg void OnKillfocusClickVelocity();
	afx_msg void OnDeltaposRegionVelocityRangeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposRegionVelocityThruSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposClickVelocitySpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnActiveLayerChangedRegionRegionkeyboard(short nLayer);
	afx_msg void OnConditionEditButton();
	afx_msg void OnSelchangeRegionConditionCombo();
	afx_msg void OnDropdownRegionConditionCombo();
	afx_msg void OnArtDls1Check();
	afx_msg void OnViblfoButton();
	afx_msg void OnFilterButton();
	afx_msg void OnInstEdRegionNewLayer();
	afx_msg void OnInstEdLayerDelete();
	afx_msg void OnRadioMultipleLayers();
	afx_msg void OnRadioSoloLayer();
	afx_msg void OnCopyRegionRegionkeyboard(short nSourceLayer, short nSourceStartNote, short nCopyLayer, short nCopyStartNote);
	afx_msg void OnWaveEditButton();
	afx_msg void OnScrollLayersRegionRegionkeyboard(BOOL bUp);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTRUMENTFVEDITOR_H__8C0AA7C7_E6FC_11D0_876A_00AA00C08146__INCLUDED_)
