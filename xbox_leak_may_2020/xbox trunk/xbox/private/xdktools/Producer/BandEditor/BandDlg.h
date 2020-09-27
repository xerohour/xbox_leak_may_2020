#ifndef __BANDDLG_H__
#define __BANDDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// BandDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBandDlg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "UndoMan.h"
#include "PChannelList.h"
#include "grid.h"

// instrument information structure
struct BInstr  {
	UINT      nStringId;
	BYTE      bPatch;
	BYTE      bMSB;
	BYTE      bLSB;
	CString  *pstrName;
};

class CBandCtrl;
class CDMInstrument;
class CInstrumentListItem;
class CBandComponent;

class CBandDlg : public CFormView, public IDMUSProdMidiInCPt
{
	friend class CBandCtrl;
	friend class CBand;
	friend class CPChannelPropPage;
	friend class CPChannelList;

public:
	CBandDlg();
	virtual ~CBandDlg();

	// IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE OnMidiMsg(REFERENCE_TIME dwTime, BYTE bStatus, BYTE bData1, BYTE bData2);

	void	RegisterMidi();
	void	UnRegisterMidi();
	bool	IsMidiRegistered();
	HRESULT PlayMIDIEvent(BYTE bStatus, BYTE bData1, BYTE bData2);

	void	EnableGridButtonAndUpdateStatus(int nChannel, bool bSelection);
	static CString GetPatchName(CBandComponent* pComponent, CDMInstrument* pInstrument);
	CBand*	GetBand();

	void	RefreshPChannelList();
	void	RefreshPChannelPropertyPage(bool fChangeToBandProperties = true);
	void	UpdatePChannelChanges(bool bResetChannelList = false, bool fChangeToBandProperties = true);
	void	ChangePChannelNumber(CDMInstrument* pInstrument, DWORD newPChannel);
	void	DisplayStatus(long nChannel);

	static bool		GetTransposeValue(CString sTransposeString, LPINT pnTranspose);
	static CString	GetInterval(int nTranspose);

	static HRESULT	GetNoteValue(CString sNoteText, LPINT pnNote);
	static CString	GetNoteText(UINT nNote);
		
protected:
	DECLARE_DYNCREATE(CBandDlg)

// Form Data
public:
	//{{AFX_DATA(CBandDlg)
	enum { IDD = IDD_DLG_BAND };
	CPChannelList	m_PChannelList;
	CGrid	m_MixGrid;
	//}}AFX_DATA

// Attributes
public:
	CBandCtrl* m_pBandCtrl;
	static CString m_arrIntervals[12];
	static CString m_arrNotes[12];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBandDlg)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBandDlg)
	afx_msg void OnDestroy();
	afx_msg void OnCursorOverGrid(long nId);
	afx_msg void OnMoveGrid(long nId, short nPan, short nVolume);
	afx_msg void OnUpdateGrid(LPUNKNOWN pIBandPChannel, BOOL bCTRLDown, BOOL bRefreshUI = TRUE);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelchangePchannelList();
	afx_msg void OnProperties();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnDblClkPChannelList();
	afx_msg void OnSelectPChannelGrid(long nId, BOOL bSelection, BOOL bCtrlDown);
	afx_msg void OnSaveUndoStateGrid();
	afx_msg void OnDisplayStatusGrid(short nId);
	afx_msg void OnRightClickGrid(short nX, short nY);
	afx_msg void OnDeleteChannelGrid();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DWORD		m_dwCookie;
	long		m_nTrackEdit;
	BOOL		m_fEditChange;
	DWORD		m_dwRef;
	CUndoMan	*m_pBandDo;

	void	LoadIntervals();
	void	LoadNotes();
	CString GetOctave(CDMInstrument* pInstrument);
	CString FormatForDisplay(CDMInstrument* pInstrument);

	void RemoveAndAddAllButtonsToGrid();
	void UpdateGridControls();
	void RefreshSelectedItems();
	void UpdateCommonPropertiesObject();

	void PopulatePChannelList();
	void UpdatePChannelList();
	void SetSelectedPChannels();
	void GetItemRect( UINT id, CRect *prectRel, BOOL bInvalidate = FALSE );

	CString GetPanVolumeText(short nPan, short nVolume);
	
	void InsertPChannel();
	void AddPChannel(int nIndex, CInstrumentListItem* pInstrumentItem);
	void OnDeleteCommand();
	void DeleteSelectedPChannels();
	void AddToGrid(CInstrumentListItem* pInstrumentItem);
	void SendAllNotesOffIfNotPlaying( void );

public:
	void RefreshDisplay();
	void SelectAllPChannels();
	// Undo Manager
	void SaveStateForUndo(char* szStateName);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __BANDDLG_H__
