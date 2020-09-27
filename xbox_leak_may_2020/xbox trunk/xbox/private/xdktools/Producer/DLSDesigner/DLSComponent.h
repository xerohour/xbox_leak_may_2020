#ifndef __DLSCOMPONENT_H__
#define __DLSCOMPONENT_H__

// DLSComponent.h : header file
//

#include "DLSDesigner.h"
#include "alist.h"
#include "dmusicc.h"
#include "dmusici.h"
#include "Conductor.h"
#include <afxtempl.h>

#define PCHANNEL_GROUP_DLS	137
#define PCHANNEL_DLS		(PCHANNEL_GROUP_DLS * 16)
#define PCHANNEL_DLS_DRUM	(PCHANNEL_GROUP_DLS * 16 + 9)

class CCollection;
class CJazzDataObject;
class CInstrument;
class CWaveNode;
class CConditionConfigToolbar;


/////////////////////////////////////////////////////////////////////////////
// Defined DLS Synth supported qeries that can be used to setup conditions in 
// the conditional chunks for regions and articulations

class CDLSQuery
{
public:
	CDLSQuery(GUID guidDLSQuery, CString sName)
	{
		m_Guid = guidDLSQuery;
		m_sName = sName;
	};

	CString GetName()
	{
		return m_sName;
	}

	GUID GetGUID()
	{
		return m_Guid;
	}

private:
	GUID	m_Guid;
	CString m_sName;

};


//////////////////////////////////////////////////////////////////////
// This class keeps the value for a DLS query as defined by the author
class CQueryValuePair
{

public:

	CQueryValuePair(CDLSQuery* pDLSQuery, DWORD dwValue);
	~CQueryValuePair();

public:
	
	CDLSQuery*	GetQuery();
	DWORD		GetValue();
	void		SetValue(DWORD dwValue);

private:
	
	CDLSQuery*	m_pDLSQuery;	// The query for which a value has been set
	DWORD		m_dwValue;		// The value for a query; any non-zero value is TRUE for boolean queries
};


//////////////////////////////////////////////////////////
// Named set of values mapping to queries....
// 
class CSystemConfiguration
{

public:

	CSystemConfiguration(bool bDefaultConfig = false);
	~CSystemConfiguration();

public:

	CString GetName();
	void	SetName(CString sConfigName);
	bool	IsDefault();

	// Operations on list of QueryValue list
	HRESULT	GetValueForQuery(CDLSQuery* pDLSQuery, DWORD& dwValue);
	HRESULT	GetValueForQuery(GUID guidQuery, DWORD& dwValue);
	HRESULT SetValueForQuery(CDLSQuery* pDLSQuery, DWORD dwValue);
	HRESULT AddQueryValue(CQueryValuePair* pQueryValuePair);
	HRESULT RemoveQueryValuePair(CQueryValuePair* pQueryValuePair);
	HRESULT RemoveQueryValuePair(GUID guidQuery);

	CPtrList& GetQueryValueList();

private:
	CQueryValuePair* GetQueryValuePair(CDLSQuery* pDLSQuery);

private:
	bool m_bDefaultConfig;
	CString		m_sName;		// Name for the configuration, this is used in the dropdown in the toolbar
	CTypedPtrList<CPtrList, CQueryValuePair*> m_lstQueryValues;
	
};

/////////////////////////////////////////////////////////////////////
// Keeps the guids for a collection so failure on load can be checked
class CCollectionGUIDItem
{
public:
	// Constructor
	CCollectionGUIDItem(GUID collectionGUID)
	{
		m_CollectionGUID = collectionGUID;
	};


	GUID m_CollectionGUID;
};

/////////////////////////////////////////////////////////////////////
// List of collections
class CCollectionList : public AList
{
	friend class CDLSComponent;
	friend class CDLSDocType;
	friend class CCollection;

	
	//	AList overrides
    CCollection *GetHead();
	CCollection *GetItem(LONG lIndex);
    CCollection *RemoveHead();
	void Remove(CCollection *pINode);
	void AddTail(CCollection *pINode);
};

/////////////////////////////////////////////////////////////////////////
// DLS component class

class CDLSComponent : public IDMUSProdComponent, IDMUSProdPortNotify, IDMUSProdDLSNotify, IDMUSProdNotifySink, IDLSReferenceLoadNotify, IDMUSProdRIFFExt
{
public:
    CDLSComponent();
	~CDLSComponent();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID *ppv);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdComponent functions
    HRESULT STDMETHODCALLTYPE Initialize(IDMUSProdFramework* pIFramework, 
										 BSTR* pbstrErrMsg);

	HRESULT STDMETHODCALLTYPE AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode);
	HRESULT STDMETHODCALLTYPE CleanUp(void);
    HRESULT STDMETHODCALLTYPE GetName(BSTR* pbstrName);
	HRESULT STDMETHODCALLTYPE OnActivateApp(BOOL fActivate);

	// IDMUSProdPortNotify methods
	HRESULT STDMETHODCALLTYPE OnOutputPortsChanged(void);
	HRESULT STDMETHODCALLTYPE OnOutputPortsRemoved(void);

	// IDMUSProdDLSNotify methods
	HRESULT STDMETHODCALLTYPE OnDownloadCustomDLS(BOOL fDownloadCustomDLS);
	HRESULT STDMETHODCALLTYPE OnDownloadGM(BOOL fDownloadGM);

	// IDMUSProdNotifySink methods
	HRESULT STDMETHODCALLTYPE OnUpdate(IDMUSProdNode *pIDocRootNode, GUID guidUpdateType, void *pData);

	// IDLSReferenceLoadNotify methods
	HRESULT STDMETHODCALLTYPE MarkLoadStart(void);
	HRESULT STDMETHODCALLTYPE MarkLoadEnd(void);

	// IDMUSProdRIFFExt methods
	HRESULT STDMETHODCALLTYPE LoadRIFFChunk(IStream* pIStream, IDMUSProdNode** ppITopNode);



    // Additional functions
public:
    HRESULT STDMETHODCALLTYPE GetCollectionImageIndex(short* pnNbrFirstImage);
	HRESULT STDMETHODCALLTYPE GetCollectionRefImageIndex(short* pnNbrFirstImage);
    HRESULT STDMETHODCALLTYPE GetInstrumentImageIndex(short* pnNbrFirstImage);
    HRESULT STDMETHODCALLTYPE GetArticulationImageIndex(short* pnNbrFirstImage);
	HRESULT STDMETHODCALLTYPE GetArticulationGrayedImageIndex( short* pnFirstImage);
    HRESULT STDMETHODCALLTYPE GetRegionImageIndex(short* pnNbrFirstImage);
	HRESULT STDMETHODCALLTYPE GetRegionGrayedImageIndex( short* pnFirstImage);
    HRESULT STDMETHODCALLTYPE GetWaveImageIndex(short* pnNbrFirstImage);
	HRESULT STDMETHODCALLTYPE GetStereoWaveImageIndex(short* pnNbrFirstImage);
	HRESULT STDMETHODCALLTYPE GetWaveRefImageIndex( short* pnNbrFirstImage );
	HRESULT STDMETHODCALLTYPE GetStereoWaveRefImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetFolderImageIndex(short* pnNbrFirstImage);
	HRESULT STDMETHODCALLTYPE GetFolderGrayedImageIndex(short* pnNbrFirstImage);


private:
    HRESULT STDMETHODCALLTYPE AddNodeImageLists(void);
	DWORD	GetCollectionWithName(char *nName);
	void	ReleaseAll();

public:
	HRESULT	GetAppendValueForPort(IDirectMusicPortDownload* pPortDownload, DWORD* pdwAppendValue);
	HRESULT IsValidDummyPatch(CInstrument* pValidateInstrument, ULONG ulBank, ULONG ulInstrument);
	HRESULT GetNextValidDummyPatch(CInstrument* pDummy, BYTE* pWMSB, BYTE* pWLSB, BYTE* pWPatch);
	void	NotifyDummyInstruments();
	bool	IsGM(DWORD dwBank, DWORD dwInstrument);

	HRESULT GetNextValidPatch(DWORD dwDrum, BYTE* pWMSB, BYTE* pWLSB, BYTE* pWPatch);
	HRESULT IsValidPatch(CInstrument* pValidateInstrument, ULONG ulBank, ULONG ulInstrument, CCollection* pLoadingCollection, CInstrument** ppInstrumentCollision);
	HRESULT PlayMIDIEvent(BYTE bStatus, BYTE bData1, BYTE bData2, DWORD dwTime, bool bIsDrum);

	void	SetSnapToZero(BOOL bSnapToZero);	// Sets the snaptozero option in the registry
	BOOL	IsSnapToZero();						// Is SnapToZero Set?
	BOOL	IsInReferenceLoad();				// Is the framework trying to load any file?
	void	GetPortName(IDirectMusicPortDownload* pIDMDownloadPort, CString& sPortName); 

	// Methods to deal with downloading/unloading collections and waves
	void	UnloadAllCollections();
	void	DownloadAllCollections();
	void	UnloadAllWaves();
	void	UnloadWave(CWaveNode* pWaveNode);
	void	DownloadAllWaves();
	BOOL	IsAutoDownloadOkay();
	BOOL	IsCollectionDownloadOK();


	// Methods to handle the FAILED collection GUIDs list
	void				 AddToFailedCollectionGUIDs(GUID collectionGUID);		// Adds to the FAILED list
	void				 DeleteFromFailedCollectionGUIDs(GUID collectionGUID);	// Deletes from the FAILED list
	CCollectionGUIDItem* FindCollectionGUID(GUID srearchGUID);					// Looks for a GUID and returns the item if found
	void				 DeleteGUIDList();										// Delete the list
	
	// Methods to deal with collection nodes
	void	AddToCollectionList(CCollection* pCollection);
	void	DeleteFromCollectionList(CCollection* pCollection);
	bool	IsInCollectionList(CCollection* pCollection);

	// Methods to deal with wavenodes
	void	AddToWaveNodesList(CWaveNode* pWaveNode);
	void	DeleteFromWaveNodesList(CWaveNode* pWaveNode);
	bool	IsInWaveNodesList(CWaveNode* pWaveNode);
	bool	IsInWaveNodesList(GUID guidFile);
	HRESULT GetListOfWaves(CPtrList* pWaveList);
	BOOL	IsLongLoopsOK();
	void	SetLongLoopOK(BOOL bLongLoopOK);

	// Methods to deal with Queries
	void	DeleteQueryList();					// Delete the list of preset queries
	BOOL	IsAPresetQuery(const GUID guidQuery, CString& sQueryname);
	BOOL	IsAPresetQuery(const CString sQueryName, GUID& guidQuery);
	const	CPtrList& GetListOfQueries();
	CDLSQuery* GetDLSQuery(const GUID& guidQuery);
	void	WriteQueryToRegistryAndAddToList(CDLSQuery* pDLSQuery);
	
	// Methods to deal with configurations
	void	DeleteConfigList();
	void	RefreshConfigToolbar();
	void	OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode = true);
	void	DeleteConfiguration(CSystemConfiguration* pConfig);
	void	AddConfiguration(CSystemConfiguration* pConfig);
	void	WriteConfigToRegistry(CSystemConfiguration* pConfig);
	void	UpdateConfigsInRegistry();
	void	LoadConfigurations();
	void	ExportConfigurations(CString sPath); 
	void	ImportConfigurations(CString sPath);

	// Methods to deal with failed codecs
	void	AddToFailedCodecs(DWORD dwCodec);
	BOOL	IsInFailedCodecs(DWORD dwCodec);
	void	RemoveFromFailedCodecs(DWORD dwCodec);
	void	ClearFailedCodecs();

	// Accessors/Mutators for configurations
	CSystemConfiguration*	GetCurrentConfig();
	CSystemConfiguration*	GetConfiguration(CString sConfigName);		

	// Accessors/mutators for patch conflict dialog show/no-show flags
	void	SetCollectionPatchConflict(bool bShow);
	bool	IsNoShowCollectionPatchConflict();

	void	SetUserPatchConflict(bool bShow);
	bool	IsNoShowUserPatchConflict();

#ifdef DMP_XBOX
	void	SetBadXboxLoopWarning(bool bShow);
	bool	IsNoShowBadXboxLoopWarning();
#endif // DMP_XBOX

	IDirectMusic*					m_pIDirectMusic;
	IDirectMusicPerformance8*		m_pIPerformance;
	IDirectMusicPort*				m_pISynthPort;
	IDirectMusicAudioPath*			m_pIAudioPath;
	
	CTypedPtrList<CPtrList, IDirectMusicPortDownload *> m_lstDLSPortDownload;
	
	GUID							m_SynthGUID;
	IDMUSProdConductor*				m_pIConductor;	
	IDMUSProdFramework*				m_pIFramework;
	CJazzDataObject*				m_pCopyDataObject;

	short			m_nNextDLS;			// Appended to name of new Style
	short			m_nNextWave;
    
	// Clipboard formats
	UINT			m_cfProducerFile;	// CF_DMUSPROD_FILE clipboard format
    UINT			m_cfCollection;		// CF_COLLECTION
    UINT			m_cfWave;			// CF_WAVE
	UINT			m_cfInstrument;		// CF_INSTRUMENT

private:
	HRESULT	OpenPort();
	HRESULT	UpdateDLSPortList();
	HRESULT UpdateDLSDownloads();
	void	InitSnapToZero();
	void	InitQueryList();
	void	WriteQueryToRegistry(const GUID& guidQuery, CString sQueryName);
	void	WriteQueryToRegistry(const GUID& guidQuery, UINT nNameID);
	void	WriteToRegistry(TCHAR* pszValueName, UINT nValueType,					// Writes the passed DWORD under the  szValueName subkey in the registry 	 
							BYTE* pbValue, DWORD cbData, TCHAR* pszSubPath = NULL); // (Uses pszSubPath to create additional folders if necessary)

	void	CreateDefaultConfigs();
	
	CSystemConfiguration* GetConfigForPort(IDirectMusicPortDownload* pPort);
	HRESULT	ReadFromRegistry(TCHAR* pszValueName, DWORD* pdwType, DWORD* pdwDataSize, BYTE* pbData, TCHAR* pszSubPath = NULL);  
	BOOL	SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

	HRESULT ValidateWaveDummyInstruments(CPtrList* pWaveList, CInstrument* pValidateInstrument, ULONG ulBank, ULONG ulInstrument);
								

    DWORD			m_dwRef;

	CConditionConfigToolbar* m_pConditionConfigToolbar;

	IDMUSProdDocType8*  m_pIDLSDocType8;
	IDMUSProdDocType8*  m_pIWaveDocType8;
	CCollectionList		m_CollectionList;

	BOOL	m_bInCleanup;
	BOOL	m_bInReferencedLoad;
	BOOL	m_bAutoDownloadDLS;
	short	m_nFirstCollectionImage;
	short	m_nFirstCollectionRefImage;
	short	m_nFirstInstrumentImage;
	short	m_nFirstArticulationImage;
	short	m_nFirstArticulationGrayedImage;
	short	m_nFirstRegionImage;
	short	m_nFirstRegionGrayedImage;
	short	m_nFirstWaveImage;
	short	m_nFirstStereoWaveImage;
	short	m_nFirstWaveRefImage;
	short	m_nFirstStereoWaveRefImage;
	short	m_nFirstFolderImage;
	short	m_nFirstFolderGrayedImage ;
	BYTE	m_bDummyMSB;
	BYTE	m_bDummyLSB;
	BYTE	m_bDummyPatch;
	BOOL	m_bSnapToZero;
	bool	m_bShowGetFormatErrorMessage;
	bool	m_bNoShowCollectionPatchConflict;
	bool	m_bNoShowUserPatchConflict;
#ifdef DMP_XBOX
	bool	m_bNoShowBadXboxLoopWarning;
#endif // DMP_XBOX
	BOOL	m_bLongLoopOK;
	BOOL	m_bDownloadCollection;


	//	The next variable is used to maintain the IDLSQueryInstruments interface...
	DWORD	m_dwInstrumentIndex;

	// The list of GUIDs of collections that failed to load in referenced load condition
	// The referenced load is marked and unmarked by the framework when a file is opened
	// The referenced load will be marked for simple collection open as well............
	CTypedPtrList<CPtrList, CCollectionGUIDItem*>	m_lstFailedCollectionGUIDs;

	CList<DWORD, DWORD>		m_lstFailedCodecs;

	// This is the list of wavenodes that are NOT in a collection
	CTypedPtrList<CPtrList, CWaveNode*>	m_lstWaveNodes;

	// This is the list of preset queries that we support for conditional chunks
	CTypedPtrList<CPtrList, CDLSQuery*> m_lstDLSQueries;

	// This is the list of defined configurations for conditional testing
	CTypedPtrList<CPtrList, CSystemConfiguration*> m_lstConfigurations;

	// This is the list of default configurations...<none> and ActivePorts
	CTypedPtrList<CPtrList, CSystemConfiguration*> m_lstDefaultConfigs;
};

#endif // __DLSCOMPONENT_H__