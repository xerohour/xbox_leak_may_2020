#ifndef __SONGCOMPONENT_H__
#define __SONGCOMPONENT_H__

// SongComponent.h : header file
//

#include <afxtempl.h>
#include <Conductor.h>
#include <PrivateUnpackingFiles.h>

class CDirectMusicSong;


#pragma pack(2)

typedef struct RegisteredTrack
{
	RegisteredTrack()
	{
		memset( &clsidTrack, 0, sizeof(CLSID) );
	}

	CLSID				clsidTrack;
	CString				strName;
} RegisteredTrack;

typedef struct RegisteredObject
{
	RegisteredObject()
	{
		memset( &guidNodeId, 0, sizeof(GUID) );
		memset( &guidRefNodeId, 0, sizeof(GUID) );
		memset( &clsidComponent, 0, sizeof(CLSID) );
		memset( &clsidDMObject, 0, sizeof(CLSID) );
		dwRIFFckid = 0;
		dwRIFFfccType = 0;
	}

	GUID    guidNodeId;
	GUID    guidRefNodeId;
	CLSID	clsidComponent;
	CLSID	clsidDMObject;
	CString strObjectType;
	DWORD	dwRIFFckid;
	DWORD	dwRIFFfccType;
} RegisteredObject;

typedef struct EmbeddedFile
{
	EmbeddedFile()
	{
		dwStreamPos = 0;
		memset( &guidDMClass, 0, sizeof(GUID) );
		memset( &guidObject, 0, sizeof(GUID) );
		memset( &guidFile, 0, sizeof(GUID) );
		CoCreateGuid( &guidNotification ); 
		fBeingLoaded = false;
	}

	~EmbeddedFile()
	{
 		IDMUSProdNotifySink* pINotifySink;
		while( !m_lstNotifyWhenLoadFinished.IsEmpty() )
		{
			pINotifySink = static_cast<IDMUSProdNotifySink*>( m_lstNotifyWhenLoadFinished.RemoveHead() );
			RELEASE( pINotifySink );
		}
	}

	DWORD		dwStreamPos;		// Stream position
    GUID        guidDMClass;		// DirectMusic GUID for the class of object
    GUID        guidObject;			// Unique GUID for this object
    CString		strObjectName;		// Name of object

	// Filled in when file is actually loaded
	GUID        guidFile;			// Producer generated GUID
	GUID		guidNotification;	// Used for FRAMEWORK_FileLoadFinished notifications
	bool		fBeingLoaded;		// Indicates file in process of loading
	CTypedPtrList<CPtrList, IDMUSProdNotifySink*> m_lstNotifyWhenLoadFinished;
} EmbeddedFile;

#pragma pack()


////////////////////////////////////////////////////////////////////////////////
class CSongComponent : public IDMUSProdComponent, public IDMUSProdRIFFExt, public IDMUSProdUnpackingFiles
{
public:
    CSongComponent();
	~CSongComponent();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdComponent functions
    HRESULT STDMETHODCALLTYPE Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg );
    HRESULT STDMETHODCALLTYPE CleanUp( void );
    HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName );
	HRESULT STDMETHODCALLTYPE AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode );
	HRESULT STDMETHODCALLTYPE OnActivateApp( BOOL fActivate );

    // IDMUSProdRIFFExt functions
    HRESULT STDMETHODCALLTYPE LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode );

	// IDMUSProdUnpackingFiles functions
    HRESULT STDMETHODCALLTYPE GetDocRootOfEmbeddedFile( IUnknown* pIDocType, BSTR bstrObjectName, IUnknown** ppIDocRootNode );
	HRESULT STDMETHODCALLTYPE AddToNotifyWhenLoadFinished( IUnknown* pIDocType, BSTR bstrObjectName, IUnknown* punkNotifySink, GUID* pguidFile );

    // Additional functions
private:
    HRESULT STDMETHODCALLTYPE AddNodeImageLists();
    HRESULT LoadRegisteredTracks();
	BOOL RegisterClipboardFormats();
	void ReleaseAll();
	BOOL LoadRegisteredObjects();

public:
    HRESULT STDMETHODCALLTYPE GetSongImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetSongRefImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetFolderImageIndex( short* pnFirstImage );
	void GetRegisteredTrackName( CLSID clsidTrack, CString& strName );

	int DetermineAction( IDMUSProdNode* pIDocRootNode, IStream* pIStream, IDMUSProdNode** ppIDupeDocRootNode );
	IDMUSProdNode* LoadEmbeddedSegment( IStream* pIStream );

	void AddToSongFileList( CDirectMusicSong* pSong );
	void RemoveFromSongFileList( CDirectMusicSong* pSong );

	// RegisteredObject list methods
	RegisteredObject* FindRegisteredObjectByCLSID( CLSID clsidDMObject );
	RegisteredObject* FindRegisteredObjectByRIFFIds( DWORD dwRIFFckid, DWORD dwRIFFfccType );

	// EmbeddedFile list methods
	HRESULT CreateEmbeddedFileList( IStream* pIStream );
	void ReleaseEmbeddedFileList();
	HRESULT ParseSongForEmbeddedSegments( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT ParseContainerForEmbeddedSongs( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT AddEmbeddedFileListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	EmbeddedFile* FindEmbeddedFile( IDMUSProdDocType* pIDocType, CString& strObjectName );

public:
	IDMUSProdFramework8*		m_pIFramework8;
	IDirectMusicPerformance8*	m_pIDMPerformance8;
	IDMUSProdConductor*			m_pIConductor;
	IDMUSProdComponent*			m_pISegmentComponent;
	IDMUSProdComponent*			m_pIContainerComponent;
	IDMUSProdComponent*			m_pIAudioPathComponent;
	IDMUSProdComponent*			m_pIToolGraphComponent;

	short						m_nNextSong;			// appended to name of new Song
	UINT						m_cfProducerFile;		//CF_DMUSPROD_FILE clipboard format
	UINT						m_cfSong;				// CF_SONG clipboard format
	UINT						m_cfSongList;			// CF_SONGLIST clipboard format
	UINT						m_cfSegment;			// CF_SEGMENT clipboard format
	UINT						m_cfContainer;			// CF_CONTAINER clipboard format
	UINT						m_cfAudioPath;			// CF_AUDIOPATH clipboard format
	UINT						m_cfGraph;				// CF_GRAPH clipboard format
	UINT						m_cfVirtualSegmentList;	// CF_VIRTUAL_SEGMENT_LIST clipboard format
	UINT						m_cfTrackList;			// CF_TRACK_LIST clipboard format
	UINT						m_cfTransitionList;		// CF_TRANSITION_LIST clipboard format
	
	// Used when unpacking files from runtime Song
    DWORD						m_dwEmbeddedFileListUseCount;
    IStream*					m_pIEmbeddedFileStream;
	EmbeddedFile*				m_pEmbeddedFileRootFile;
	CTypedPtrList<CPtrList, EmbeddedFile*> m_lstEmbeddedFiles;

	// Used when unpacking duplicate files from runtime Song
	IDMUSProdNode*				m_pSongBeingLoaded;
	IDMUSProdProject*			m_pIDupeFileTargetProject;
	int							m_nDupeFileDlgReturnCode;

private:
    DWORD						m_dwRef;
	IDMUSProdDocType8*			m_pISongDocType8;
	short						m_nFirstImage;

	CTypedPtrList<CPtrList, CDirectMusicSong*> m_lstSongs;
	CTypedPtrList<CPtrList, RegisteredTrack*> m_lstRegisteredTracks;
	CTypedPtrList<CPtrList, RegisteredObject*> m_lstRegisteredObjects;
};

#endif // __SONGCOMPONENT_H__
