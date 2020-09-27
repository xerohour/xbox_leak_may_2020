#ifndef __CONTAINERCOMPONENT_H__
#define __CONTAINERCOMPONENT_H__

// ContainerComponent.h : header file
//

#include <afxtempl.h>
#include <Conductor.h>
#include <PrivateUnpackingFiles.h>


class CDirectMusicContainer;
class CContainerObject;


/////////////////////////////////////////////////////////////////////
// Structures
//

#pragma pack(2)

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


/////////////////////////////////////////////////////////////////////
// CContainerComponent class
//

class CContainerComponent : public IDMUSProdComponent, public IDMUSProdRIFFExt, public IDMUSProdUnpackingFiles
{
public:
    CContainerComponent();
	~CContainerComponent();

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
	BOOL RegisterClipboardFormats();
	void ReleaseAll();
	BOOL LoadRegisteredObjects();

public:
    HRESULT STDMETHODCALLTYPE GetContainerImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetContainerRefImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetFolderImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetFolderGrayImageIndex( short* pnNbrFirstImage );
	HRESULT STDMETHODCALLTYPE IsRegisteredObject( IDMUSProdNode* pIDocRootNode );
	HRESULT STDMETHODCALLTYPE CreateRefNode( IDMUSProdNode* pIDocRootNode, IDMUSProdNode** ppIRefNode );
	RegisteredObject* FindRegisteredObjectByCLSID( CLSID clsidDMObject );
	RegisteredObject* FindRegisteredObjectByDocRoot( IDMUSProdNode* pIDocRootNode );
	RegisteredObject* FindRegisteredObjectByRIFFIds( DWORD dwRIFFckid, DWORD dwRIFFfccType );
	HRESULT FindReferencedFile( CContainerObject* pObject, CLSID clsidDMObject, CString strObjectName, IStream* pIStream, IDMUSProdNode** ppIDocRootNode );
	IDMUSProdNode* LoadEmbeddedFile( CLSID clsidDMObject, IStream* pIStream );
	int DetermineAction( IDMUSProdNode* pIDocRootNode, RegisteredObject* pRegisteredObject, IStream* pIStream, IDMUSProdNode** ppIDupeDocRootNode );
	void AddToContainerFileList( CDirectMusicContainer* pContainer );
	void RemoveFromContainerFileList( CDirectMusicContainer* pContainer );

	// EmbeddedFile list methods
	HRESULT CreateEmbeddedFileList( IStream* pIStream );
	void ReleaseEmbeddedFileList();
	HRESULT ParseContainerForEmbeddedFiles( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT AddEmbeddedFileListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	EmbeddedFile* FindEmbeddedFile( IDMUSProdDocType* pIDocType, CString& strObjectName );

public:
	IDMUSProdFramework8*		m_pIFramework8;
	IDirectMusicPerformance*	m_pIDMPerformance;
	IDMUSProdConductor*			m_pIConductor;
	short						m_nNextContainer;		// appended to name of new Container
	UINT						m_cfProducerFile;		// CF_DMUSPROD_FILE clipboard format
	UINT						m_cfContainer;			// CF_CONTAINER clipboard format
	UINT						m_cfContainerList;		// CF_CONTAINERLIST clipboard format
	
	// Used when unpacking files from runtime Container
    DWORD						m_dwEmbeddedFileListUseCount;
    IStream*					m_pIEmbeddedFileStream;
	EmbeddedFile*				m_pEmbeddedFileRootFile;
	CTypedPtrList<CPtrList, EmbeddedFile*> m_lstEmbeddedFiles;
	
	// Used when unpacking duplicate files from runtime Container
	IDMUSProdNode*				m_pContainerBeingLoaded;
	IDMUSProdProject*			m_pIDupeFileTargetProject;
	int							m_nDupeFileDlgReturnCode;

private:
    DWORD						m_dwRef;
	IDMUSProdDocType8*			m_pIContainerDocType8;
	short						m_nFirstImage;

	CTypedPtrList<CPtrList, CDirectMusicContainer*> m_lstContainers;
	CTypedPtrList<CPtrList, RegisteredObject*> m_lstRegisteredObjects;
};

#endif // __CONTAINERCOMPONENT_H__
