#ifndef __BAND_H__
#define __BAND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Band.h : header file
//

#include "BandComponent.h"
#include "BandDocType.h"
#include "TabBand.h"
#include "RiffStrm.h"
#include "dmusici.h"
#include "dmusicf.h"

#define DMUS_PATCH (DMUS_IO_INST_BANKSELECT | DMUS_IO_INST_PATCH)

#define DM_PATCH_CHANGE		DMUS_PMSGT_PATCH
#define DM_TRANSPOSE		DMUS_PMSGT_TRANSPOSE
#define DM_PITCHBEND_RANGE	DMUS_PMSGT_CURVE
#define DM_VOLUME			(DM_TRANSPOSE + 1)
#define DM_PAN				(DM_VOLUME + 1)

#define MIDI_PROGRAM_CHANGE 0xc0
#define MIDI_CONTROL_CHANGE 0xb0
#define MIDI_CC_BS_MSB		0x00
#define MIDI_CC_BS_LSB		0x20
#define MIDI_CC_VOLUME		0x07
#define MIDI_CC_PAN 		0x0a

#define DRUM_TRACK			10
#define DRUM_PCHANNEL		9


static DWORD dwaPriorities[NBR_IMA_INSTRUMENTS] = { DAUD_CHAN1_DEF_VOICE_PRIORITY, DAUD_CHAN2_DEF_VOICE_PRIORITY,
													DAUD_CHAN3_DEF_VOICE_PRIORITY, DAUD_CHAN4_DEF_VOICE_PRIORITY, 
													DAUD_CHAN5_DEF_VOICE_PRIORITY, DAUD_CHAN6_DEF_VOICE_PRIORITY,
													DAUD_CHAN7_DEF_VOICE_PRIORITY, DAUD_CHAN8_DEF_VOICE_PRIORITY,
													DAUD_CHAN9_DEF_VOICE_PRIORITY, DAUD_CHAN10_DEF_VOICE_PRIORITY,
													DAUD_CHAN11_DEF_VOICE_PRIORITY, DAUD_CHAN12_DEF_VOICE_PRIORITY,
													DAUD_CHAN13_DEF_VOICE_PRIORITY, DAUD_CHAN14_DEF_VOICE_PRIORITY,
													DAUD_CHAN15_DEF_VOICE_PRIORITY, DAUD_CHAN16_DEF_VOICE_PRIORITY};

static DWORD dwaPriorityLevels[5] = { DAUD_CRITICAL_VOICE_PRIORITY, DAUD_HIGH_VOICE_PRIORITY,     
									  DAUD_STANDARD_VOICE_PRIORITY, DAUD_LOW_VOICE_PRIORITY };  


/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler window

class CContextMenuHandler : public CWnd
{
// Construction
public:
	CContextMenuHandler();

// Attributes
public:
	class CBand *m_pBand;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContextMenuHandler)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CContextMenuHandler();

	// Generated message map functions
protected:
	//{{AFX_MSG(CContextMenuHandler)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CLinkedDLSCollection

class CLinkedDLSCollection
{
// Constructor
public:
	CLinkedDLSCollection( IDMUSProdNode* pIDocRootNode );
	virtual ~CLinkedDLSCollection();

// Attributes
public:
	IDMUSProdNode*	m_pIDocRootNode;
	DWORD			m_dwUseCount;
};


//////////////////////////////////////////////////////////////////////
//	CBandPropPageManager

class CBandPropPageManager : public CDllBasePropPageManager 
{
friend class CTabBand;

public:
	CBandPropPageManager();
	virtual ~CBandPropPageManager();

	// IDMUSProdPropPageManager functions
	HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
	HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
	HRESULT STDMETHODCALLTYPE RefreshData();

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
	CTabBand*			m_pTabBand;
};


class CDMInstrument : public DMUS_IO_INSTRUMENT
{
friend class CBand;
friend class CBandDlg;
friend class CInstrumentListItem;
friend class CCommonInstrumentPropertyObject;

protected:
	CString 					m_csCollection;
private:
	CBand*						m_pBand;
	CBandComponent* 			m_pComponent;
	IDMUSProdReferenceNode* 	m_pDMReference;
	bool						m_fSelected;		// This flag is used to save the selection info in DesignTime save

	static BOOL					m_bSendBandUpdate;

public:
	CDMInstrument();
	~CDMInstrument();
	CDMInstrument&	operator = (const CDMInstrument& dmInstrument);

	void						ResetInstruemnt();
	bool						IsDrums();
	HRESULT 					Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT 					Save(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT 					SaveFileRef(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT 					SaveDMRef(IDMUSProdRIFFStream* pIRiffStream, WhichLoader whichLoader );
	void						SetComponent(CBandComponent* pComponent) {m_pComponent = pComponent;}
	CBandComponent* 			GetComponent() {return m_pComponent;}
	void						SetBand(CBand* pBand) {m_pBand = pBand;}
	CBand*						GetBand() {return m_pBand;}
	BOOL						SendBandUpdate(IDMUSProdNode* pIBandnode);
	HRESULT 					Send(IDMUSProdNode* pIBandNode, UINT uMsgType);

	void						SetDLSCollection( IDMUSProdReferenceNode* pDMReferenceNode );
};


class CInstrumentListItem : public IDMUSProdBandPChannel
{
public:
	CInstrumentListItem();
	CInstrumentListItem(CDMInstrument* pInstrument);
	
	~CInstrumentListItem();

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	// IDMUSProdBandPChannel methods
	HRESULT STDMETHODCALLTYPE GetPChannelNumber(int* pnNumber);
	HRESULT STDMETHODCALLTYPE GetVolume(int* pnVolume);
	HRESULT STDMETHODCALLTYPE GetPan(int* pnPan);

	HRESULT STDMETHODCALLTYPE SetVolume(int nVolume);
	HRESULT STDMETHODCALLTYPE SetPan(int nPan);
	
	HRESULT STDMETHODCALLTYPE IsSelected(BOOL* pfSelected);
	HRESULT STDMETHODCALLTYPE SetSelected(BOOL fSelection);

	HRESULT STDMETHODCALLTYPE IsEnabled(BOOL* pbEnabled);

	HRESULT STDMETHODCALLTYPE SyncChanges(BOOL bSendBandUpdate);


	// Other methoda
	void SetInstrument(CDMInstrument* pInstrument);
	CDMInstrument* GetInstrument();

	void SetSelected(bool bSelectionFlag);
	void SetSelected();
	bool IsSelected();

	static CCommonInstrumentPropertyObject m_CommonPropertyObject;

private:
	DWORD			m_dwRef;
	bool			m_fSelected;
	CDMInstrument*	m_pInstrument;
};



 

class CCommonInstrumentPropertyObject : public IDMUSProdPropPageObject
{
	friend class CInstrumentListItem;
	friend class CPChannelPropPage;
public:

	CCommonInstrumentPropertyObject();
	~CCommonInstrumentPropertyObject();
	
	// IUnknown functions
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void** ppData );
	HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();


	void		SetReset(bool bReset);
	void		SetProperties(CBandDlg* pBandDlg, CDMInstrument* pInstrument);
	void		SetSelectedChannels(int nSelectedChannels);
	void		SetBandDialog(CBandDlg* pBandDlg);
	CBandDlg*	GetBandDialog();

	// Applies and Syncs the changes for all the selected PChannels and refreshes the dialog
	void ApplyChangesToSelectedChannelsAndRefreshUI();
	void ApplyChanges(CDMInstrument* pInstrument);

	void SyncPChannelChanges(int nChannels, CInstrumentListItem** ppInstrumentArray);
	void SyncChanges(CDMInstrument* pInstrument, CBand *pBand = NULL);

	void ApplyOctaveChange(CDMInstrument* pInstrument);
	void ApplyPanChange(CDMInstrument* pInstrument);
	void ApplyVolumeChange(CDMInstrument* pInstrument);
	void ApplyInstrumentChange(CDMInstrument* pInstrument, const CString *pcsCurrentCollection = NULL);
	void ApplyPChannelChange(CDMInstrument* pInstrument);
	void ApplyNoteRangeChange(CDMInstrument* pInstrument);
	void ApplyPriorityChange(CDMInstrument* pInstrument);
	void ApplySoundCanvasChange(CDMInstrument* pInstrument);
	void ApplyPitchBendRangeChange(CDMInstrument* pInstrument);

	void SyncOctaveChange(CDMInstrument* pInstrument);
	void SyncPanChange(CDMInstrument* pInstrument);
	void SyncVolumeChange(CDMInstrument* pInstrument);
	void SyncInstrumentChange(CDMInstrument* pInstrument);
	void SyncPChannelChange(CDMInstrument* pInstrument);
	void SyncNoteRangeChange(CDMInstrument* pInstrument);
	void SyncPriorityChange(CDMInstrument* pInstrument);
	void SyncSoundCanvasChange(CDMInstrument* pInstrument);
	void SyncPitchBendRangeChange(CDMInstrument* pInstrument);

	// Set all the ignores
	void SetIgnoreInstrument(int nIgnore);
	void SetIgnoreOctaveTranspose(int nIgnore);
	void SetIgnoreVolume(int nIgnore);
	void SetIgnorePan(int nIgnore);
	void SetIgnorePriority(int nIgnore);
	void SetIgnoreNoteRange(int nIgnore);
	void SetIgnorePitchBendRange(int nIgnore);

	// Applies all the ignores
	void ApplyIgnores(CDMInstrument* pInstrument);
	void SyncIgnores(CDMInstrument* pInstrument);

	void ApplyInstrumentIgnore(CDMInstrument* pInstrument);
	void ApplyOctaveTransposeIgnore(CDMInstrument* pInstrument);
	void ApplyVolumeIgnore(CDMInstrument* pInstrument);
	void ApplyPanIgnore(CDMInstrument* pInstrument);
	void ApplyPriorityIgnore(CDMInstrument* pInstrument);
	void ApplyNoteRangeIgnore(CDMInstrument* pInstrument);
	void ApplyPitchBendRangeIgnore(CDMInstrument* pInstrument);

	void SyncInstrumentIgnore(CDMInstrument* pInstrument);
	void SyncOctaveTransposeIgnore(CDMInstrument* pInstrument);
	void SyncVolumeIgnore(CDMInstrument* pInstrument);
	void SyncPanIgnore(CDMInstrument* pInstrument);
	void SyncPriorityIgnore(CDMInstrument* pInstrument);
	void SyncPitchBendRangeIgnore(CDMInstrument* pInstrument);

	// Gets the number of selected channels
	int GetNumberOfChannels();

	void SetPChannelNumber(int nNumber);
	void SetPChannelName(CString sName);
	void SetInstrument(CDMInstrument* pInstrument);
	void SetPatch(DWORD dwPatch);
	void SetInstrument(CString sInstrumentName);
	void SetOctave(short nOctave);
	void SetTranspose(short nTranspose);
	void SetOctaveTranspose(short nOctaveTranspose);
	void SetVolume(int nVolume);
	void SetPan(int nPan);
	void SetPriority(DWORD lPriority, bool bPreserveOffset = false);
	void SetLowNote(short nLowNote);
	void SetHighNote(short nHighNote);
	void SetSoundCanvas(int nState);
	void SetSoundCanvas(CDMInstrument* pInstrument);
	void SetPitchBendOctave(short nPitchBendRange);
	void SetPitchBendTranspose(short nPitchBendRange);
	void SetPitchBendRange(short nPitchBendRange);
	void SetNoteRange(int nHighNote, int LowNote);

	int 	GetPChannelNumber();
	CString GetPChannelName();
	CString GetInstrument();
	short	GetOctave();
	short	GetTranspose();
	short	GetOctaveTranspose();
	int 	GetVolume();
	int 	GetPan();
	void	GetPriority(DWORD& dwPriorityLevel, DWORD& dwPriorityOffset);
	short	GetLowNote();
	short	GetHighNote();
	int		GetSoundCanvas();
	short	GetPitchBendOctave();
	short	GetPitchBendTranspose();
	short	GetPitchBendRange();

// Attributes
public:
		// The flag to check if something has changed
	DWORD m_dwChanged;
	DWORD m_dwIgnoresChanged;
	DWORD  m_dwInstrumentChangeID;
	int m_nABIIndex;


//Attributes
private:

	DWORD		m_dwRef; 

	bool		m_bReset;					// The reset will make the set methods ignore 
											// the unknown val if previouosly set

	CBandDlg*	m_pBandDlg; 				// The Band Dialog for the selected PChannels
	int 		m_nChannels;				// Number of selected PChannels

	int			m_nLastSelectedCollection; 	// We need to remember which dLS collection we used the last time
	CString		m_csCurrentCollection;		// so we can set it in the "Other DLS.." dialog 

	CString		m_sInstrumentInfo;			// This keeps the Collection, MSB, LSB, Patch info to show on the prop page
	CString		m_sInstrumentCollectionInfo;// Keeps the collection name for the instruments
	DWORD		m_dwInstrumentPatchInfo;	// Keeps the MSB LSB, Patch info 


	bool		m_bPreservePriorityOffset;  // Used to change only the priority level maintaining the offset

	// The common fields to be displayed in the property sheet

	int 		m_nPChannelNumber;		// PChannels number
	CString 	m_sPChannelName;		// PChannel Name
	DWORD		m_dwPatch;				// Patch Number
	CString 	m_sPatch;				// The instrument name
	short		m_nOctave;				// Octave
	short		m_nTranspose;			// Transpose
	short		m_nOctaveTranspose;		// Octave and Transpose added
	int 		m_nVolume;				// Volume
	int 		m_nPan; 				// Pan
	DWORD		m_dwPriority;			// Priority
	DWORD		m_dwPriorityLevel;		// Priority Level that maps to standard offsets (Low, Standard, High, critical)
	DWORD		m_dwPriorityOffset;		// Offset difference from the level
	short		m_nLowNote; 			// Low note of range
	short		m_nHighNote;			// High note of range
	int			m_nSoundCanvas;			// Use Roland SoundCanvas (TM) : 0 - Don't, 1 - Use it, 2 - Don't Know! Different Instrument, different settings!
	short		m_nPitchBendOctave;		// Octave for the Pitch Bend Range
	short		m_nPitchBendTranspose;	// Transpose for the Pitch Bend Range
	short		m_nPitchBendRange;		// The pitch bend range for the instrument

	// All the ignores....

	int 	m_nIgnoreVolume;
	int 	m_nIgnoreOctaveTranspose;
	int 	m_nIgnoreInstrument;
	int 	m_nIgnorePan;
	int 	m_nIgnorePriority;
	int 	m_nIgnoreNoteRange;
	int		m_nIgnorePitchBendRange;

//Operations
private:
	void CheckAllIgnores(CDMInstrument* pInstrument);
	void CheckInstrumentIgnore(CDMInstrument* pInstrument);
	void CheckOctaveTransposeIgnore(CDMInstrument* pInstrument);
	void CheckVolumeIgnore(CDMInstrument* pInstrument);
	void CheckPanIgnore(CDMInstrument* pInstrument);
	void CheckPriorityIgnore(CDMInstrument* pInstrument);
	void CheckNoteRangeIgnore(CDMInstrument* pInstrument);
	void CheckPitchBendIgnore(CDMInstrument* pInstrument);
};

#define PCHANNEL_CHANGED	(1 << 0)
#define INSTRUMENT_CHANGED	(1 << 1)
#define OCTAVE_CHANGED		(1 << 2)
#define TRANSPOSE_CHANGED	(1 << 3)
#define VOLUME_CHANGED		(1 << 4)
#define PAN_CHANGED 		(1 << 5)
#define NOTERANGE_CHANGED	(1 << 6)
#define PRIORITY_CHANGED	(1 << 7)
#define SOUNDCANVAS_CHANGED (1 << 8)
#define PITCHBEND_CHANGED	(1 << 9)

#define IGNORE_INSTRUMENT_CHANGED		(1 << 10)
#define IGNORE_VOLUME_CHANGED			(1 << 11)
#define IGNORE_PAN_CHANGED				(1 << 12)
#define IGNORE_OCTAVETRANSPOSE_CHANGED	(1 << 13)
#define IGNORE_PRIORITY_CHANGED 		(1 << 14)
#define IGNORE_NOTERANGE_CHANGED		(1 << 15)
#define IGNORE_PITCHBEND_CHANGED		(1 << 16)

#define IGNORES_CHANGED ( IGNORE_INSTRUMENT_CHANGED | IGNORE_VOLUME_CHANGED | IGNORE_PAN_CHANGED \
						| IGNORE_OCTAVETRANSPOSE_CHANGED | IGNORE_PRIORITY_CHANGED | IGNORE_NOTERANGE_CHANGED \
						| IGNORE_PITCHBEND_CHANGED )

#define DLS_REQUEST 		1
#define DRUMS_REQUEST		2

#define IGNORE_OFF			1			
#define IGNORE_ON			0
#define IGNORE_UNDT 		2


class CBand : public IDMUSProdNode, public IDMUSProdBandEdit8a, public IPersistStream,
			  public IDMUSProdNotifySink, public IDMUSProdPropPageObject, public IDMUSProdBandMgrEdit,
			  public IDMUSProdGetReferencedNodes
{
friend class CBandCtrl;
friend class CBandDlg;
friend class CBandDocType;
friend class CBandPropPageManager;
friend class CCommonInstrumentPropertyObject;
friend class CTabBand;

public:
	CBand( CBandComponent* pComponent );
	~CBand();

	// IUnknown functions
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	// IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnFirstImage );
	HRESULT STDMETHODCALLTYPE GetFirstChild( IDMUSProdNode** ppIFirstChildNode );
	HRESULT STDMETHODCALLTYPE GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode );

	HRESULT STDMETHODCALLTYPE GetComponent( IDMUSProdComponent** ppIComponent );
	HRESULT STDMETHODCALLTYPE GetDocRootNode( IDMUSProdNode** ppIDocRootNode );
	HRESULT STDMETHODCALLTYPE SetDocRootNode( IDMUSProdNode* pIDocRootNode );
	HRESULT STDMETHODCALLTYPE GetParentNode( IDMUSProdNode** ppIParentNode );
	HRESULT STDMETHODCALLTYPE SetParentNode( IDMUSProdNode* pIParentNode );
	HRESULT STDMETHODCALLTYPE GetNodeId( GUID* pguid );
	HRESULT STDMETHODCALLTYPE GetNodeName( BSTR* pbstrName );
	HRESULT STDMETHODCALLTYPE GetNodeNameMaxLength( short* pnMaxLength );
	HRESULT STDMETHODCALLTYPE ValidateNodeName( BSTR bstrName );
	HRESULT STDMETHODCALLTYPE SetNodeName( BSTR bstrName );
	HRESULT STDMETHODCALLTYPE GetNodeListInfo( DMUSProdListInfo* pListInfo );

	HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
	HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
	HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
	HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

	HRESULT STDMETHODCALLTYPE GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnResourceId );
	HRESULT STDMETHODCALLTYPE OnRightClickMenuInit( HMENU hMenu );
	HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect( long lCommandId );

	HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
	HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );

	HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	HRESULT STDMETHODCALLTYPE CanCut();
	HRESULT STDMETHODCALLTYPE CanCopy();
	HRESULT STDMETHODCALLTYPE CanDelete();
	HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject );

	// IDMUSProdBandEdit functions
	HRESULT STDMETHODCALLTYPE GetDefaultFlag( BOOL* pfDefaultFlag);
	HRESULT STDMETHODCALLTYPE SetDefaultFlag( BOOL fDefaultFlag );
    HRESULT STDMETHODCALLTYPE GetDLSRegionName( DWORD dwPChannel, BYTE bMIDINote, BSTR* pbstrName );

	// IDMUSProdBandEdit8 functions
	HRESULT STDMETHODCALLTYPE GetPatchForPChannel( DWORD dwPChannel, DWORD *pdwPatch );

 	// IDMUSProdBandEdit8a functions
    HRESULT STDMETHODCALLTYPE GetInstNameForPChannel( DWORD dwPChannel, BSTR* pbstrName );
    HRESULT STDMETHODCALLTYPE DisplayInstrumentButton( DWORD dwPChannel, LONG lXPos, LONG lYPos );
	HRESULT STDMETHODCALLTYPE InsertPChannel( DWORD dwPChannel, BOOL fClearBandFirst );
	HRESULT STDMETHODCALLTYPE SetAudiopath( IUnknown* punkAudiopath );

	// IDMUSProdBandMgrEdit functions
    HRESULT STDMETHODCALLTYPE DisplayEditBandButton( DWORD dwPChannel, LONG lXPos, LONG lYPos );

	// IPersist functions
	STDMETHOD(GetClassID)( CLSID* pClsId );

	// IPersistStream functions
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
	STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	// IDMUSProdNotifySink method
	STDMETHOD(OnUpdate)(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData);

	// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void** ppData );
	HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// Additional functions
	HRESULT OnViewProperties();

    // IDMUSProdGetReferencedNodes
    HRESULT STDMETHODCALLTYPE GetReferencedNodes( DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode, DWORD dwErrorLength, WCHAR *wcstrErrorText );

public:
	HRESULT Load_IMA_Band( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT LoadDMBand( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo );
	
	void	SetModifiedFlag( BOOL fNeedsSaved );
	void	Activate();
	void	EnablePatch( int nIndex );
	void	RemoveFromNotifyList( IDMUSProdReferenceNode* pDMReferenceNode );
	void	AddToNotifyList( IDMUSProdReferenceNode* pDMReferenceNode );
	HRESULT NotifyEveryone( GUID guidNotification);

	void	GetGUID( GUID* pguidBand );
	void	SetGUID( GUID guidBand );

	HRESULT OnContextCommand( WPARAM wParam, LPARAM lParam );

	// Operations on the Instrument List

	CDMInstrument*			GetInstrumentFromList(long PChannel);
	CDMInstrument*			GetInstrumentByIndex(int nIndex);
	
	CInstrumentListItem*	GetInstrumentItemFromList(long lPChannel);
	CInstrumentListItem*	GetInstrumentItemByIndex(int nCount);

	int						GetInstrumentCount();
	int						GetNumberOfSelectedInstruments();

	int						GetSelectedInstruments(CInstrumentListItem** ppInstrumentArray);
	void					UnselectAllInstruments();
	void					SelectAllInstruments();
	
	void*					GetInstrumentList();
	void					DeleteInstrumentList();
	
	void					InsertIntoInstrumentList(CInstrumentListItem* pInstrumentItem);
	CInstrumentListItem*	InsertIntoInstrumentList();
	
	int						DeleteFromInstrumentList(DWORD nPChannel);
	int						DeleteFromInstrumentList(CInstrumentListItem* pInstrumentItem);
	
	DWORD					GetNextAvailablePChannel();
	DWORD					GetNextAvailablePChannel(DWORD dwStartChannel);
	DWORD					GetLastAvailablePChannel(DWORD nStartIndex);

	bool					IsPChannelFree(DWORD dwPChannel);
	CInstrumentListItem*	ChangePChannelNumber(DWORD oldPChannel, DWORD newPChannel);
	void					SendBand();
	bool					m_bInUndoRedo;

protected:
	HRESULT SaveBandChunk( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveBandUnfoList( IDMUSProdRIFFStream* pIRiffStream );

private:
	void InitInstrumentList();
	HRESULT LoadDMInstruments( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT SyncAllInstruments(DMUS_PMSGT_TYPES pmsgType);


public:
	CBandComponent* 	m_pComponent;
	CBandCtrl*			m_pBandCtrl;
	IDirectMusicAudioPath *m_pAudiopath;

private:
	DWORD				m_dwRef;
	IDMUSProdNode*		m_pIDocRootNode;
	IDMUSProdNode*		m_pIParentNode;

	HWND				m_hWndEditor;
	CContextMenuHandler	m_ContextMenuHandler;
	DWORD				m_dwPChannelForContextMenu;
	CDMInstrument*		m_pDMInstrumentForContextMenu;

	// Band data for Music Engine
	BOOL				m_fDefault;

	//patch/program change and DLS info
	CTypedPtrList<CPtrList, CLinkedDLSCollection*>	 m_lstDLSCollections;
	CTypedPtrList<CPtrList, CInstrumentListItem*>	 m_lstInstruments;

	GUID				m_guidBand;
	CString 			m_csName;
	CString 			m_strDefaultCollection;
	DMUS_IO_VERSION 	m_Version;
	BOOL				m_fDirty;

	// resource reference count
	static long			m_slResourceRefCount;
};

#define FOURCC_COLLECTION_REF	mmioFOURCC('c','o','l','r')
#define FOURCC_INST_DESIGNTIME	mmioFOURCC('i','n','s','p')

// 5/11/98 JHD: Added next 3 lines to reflect the IMA -> DMusic mapping
// This expects a voiceid from 1-16
#define IMA25_VOICEID_TO_PCHANNEL( id )  ( ( id + 4 ) & 0xf )
#define PCHANNEL_TO_IMA25_VOICEID( id )  ( ( id + 12 ) & 0xf )

#endif // __BAND_H__
