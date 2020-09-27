// Project.h: interface for the CProject class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROJECT_H__
#define __PROJECT_H__

/*-----------
@doc DMUSPROD
-----------*/

#include <PChannelName.h>

class CProject;
class CJazzDoc;
class CJzRuntimeFolder;
class CProjectPropTabGeneral;
class CProjectPropTabFolders;


/////////////////////////////////////////////////////////////////////////////
// PChannelName

struct PChannelName
{
	DWORD	m_dwPChannel;
	CString	m_strName;
};

/////////////////////////////////////////////////////////////////////////////
// CJzRuntimeFolder

class CJzRuntimeFolder
{
// Constructor
public:
	CJzRuntimeFolder();
	virtual ~CJzRuntimeFolder();

// Attributes
public:
	CJazzDocTemplate*	m_pDocTemplate;
	CString				m_strFilterExt;
	CString				m_strRuntimeFolder;		// Default "Runtime" folder
};


//////////////////////////////////////////////////////////////////////
//  CProjectPropPageManager

class CProjectPropPageManager : public IDMUSProdPropPageManager 
{
friend class CProjectPropTabGeneral;
friend class CProjectPropTabFolders;

public:
	CProjectPropPageManager();
	virtual ~CProjectPropPageManager();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();

    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
    HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject );

    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE IsEqualPageManagerGUID( REFGUID rguidPageManager );

	//Additional functions
private:
	void RemoveCurrentObject( void );

	// Member variables
private:
    DWORD						m_dwRef;
	IDMUSProdPropSheet*			m_pIPropSheet;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	
	CProjectPropTabGeneral*		m_pTabGeneral;
	CProjectPropTabFolders*		m_pTabFolders;

public:
	static short				sm_nActiveTab;
};



/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER PROJECT OBJECT
========================================================================================
@object Project | Represents a collection of files.

@supint IDMUSProdProject | Allows DirectMusic Producer <o Component>s to interact with
		other files and/or <o Node>s in a specific Project.  Implemented within the Framework.
@supint IDMUSProdPChannelName | Allows PChannel names to be assigned on a Project by
		Project basis.  Implemented within the Framework.
@comm
	Every DirectMusic Producer Project corresponds to a directory residing on disk and
	contains all files located in that entire directory tree.  Adding and removing files
	from a Project affects the contents of the directory tree because the contents of a
	DirectMusic Producer Project mirrors the files on disk.
	
	If desired, Explorer can be used to place files into the directory tree of a DirectMusic
	Producer Project.  Both Producer files and non-Producer files are displayed in the
	application's Project Tree.  Double-clicking on a Producer file in the Project Tree
	will invoke the Node's <o Editor>.  Double-clicking on a non-Producer file will open
	the application associated with its file extension.
	
	The <o Framework> handles creation and management of Projects.

--------------------------------------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////
//  CProject

class CProject : public CDirectoryNode, public IDMUSProdProject, public IDMUSProdPropPageObject, public IDMUSProdPChannelName
{
public:
	CProject();
	virtual ~CProject();

    // IUnknown overrides
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode overrides
	virtual HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnFirstImage );

    virtual HRESULT STDMETHODCALLTYPE GetDocRootNode( IDMUSProdNode** ppIDocRootNode );
    virtual HRESULT STDMETHODCALLTYPE SetDocRootNode( IDMUSProdNode* pIDocRootNode );
    virtual HRESULT STDMETHODCALLTYPE GetParentNode( IDMUSProdNode** ppIParentNode );
    virtual HRESULT STDMETHODCALLTYPE SetParentNode( IDMUSProdNode* pIParentNode );

    virtual HRESULT STDMETHODCALLTYPE GetNodeId( GUID* pguid );
    virtual HRESULT STDMETHODCALLTYPE GetNodeNameMaxLength( short* pnMaxLength );
    virtual HRESULT STDMETHODCALLTYPE ValidateNodeName( BSTR bstrName );
    virtual HRESULT STDMETHODCALLTYPE SetNodeName( BSTR bstrName );

    virtual HRESULT STDMETHODCALLTYPE GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnResourceId );
    virtual HRESULT STDMETHODCALLTYPE OnRightClickMenuInit( HMENU hMenu );
    virtual HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect( long lCommandId );

    virtual HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	virtual HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	virtual HRESULT STDMETHODCALLTYPE CanCut();
	virtual HRESULT STDMETHODCALLTYPE CanCopy();
	virtual HRESULT STDMETHODCALLTYPE CanDelete();
	virtual HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );

/* --------------------------------------------------------------------------
@interface IDMUSProdProject | 
	Allows DirectMusic Producer <o Component>s to interact with other files and/or <o Node>s
	in a specific <o Project>.

@comm
	<om IDMUSProdFramework.GetFirstProject> and <om IDMUSProdFramework.GetNextProject> enumerate
	the currently loaded Projects.


@base public | IUnknown

@xref  <o Project> Object, <om IDMUSProdFramework.FindProject>, <om IDMUSProdFramework.GetFirstProject>, <om IDMUSProdFramework.GetNextProject>
-------------------------------------------------------------------------- */

    // IDMUSProdProject functions
// @meth HRESULT | GetName|  Returns the Project name. 
    HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName );
// @meth HRESULT | GetGUID|  Returns the Project GUID. 
    HRESULT STDMETHODCALLTYPE GetGUID( GUID* pguid );
// @meth HRESULT | GetFirstFileByDocType| Returns an IDMUSProdNode interface pointer for the first File node
//			in the Project Tree whose corresponding DocType matches <p pIDocType>. 
    HRESULT STDMETHODCALLTYPE GetFirstFileByDocType( IDMUSProdDocType* pIDocType, IDMUSProdNode** ppIFirstFileNode );
// @meth HRESULT | GetNextFileByDocType| Returns an IDMUSProdNode interface pointer for the next File node
//			in the Project Tree whose corresponding DocType matches that of <p pIFileNode>.
    HRESULT STDMETHODCALLTYPE GetNextFileByDocType( IDMUSProdNode* pIFileNode, IDMUSProdNode** ppINextFileNode );
// @meth HRESULT | GetFirstFile| Returns an IDMUSProdNode interface pointer for the first File node
//			in the Project Tree.
    HRESULT STDMETHODCALLTYPE GetFirstFile( IDMUSProdNode** ppIFirstFileNode );
// @meth HRESULT | GetNextFile| Returns an IDMUSProdNode interface pointer for the next File node
//			in the Project Tree.
    HRESULT STDMETHODCALLTYPE GetNextFile( IDMUSProdNode* pIFileNode, IDMUSProdNode** ppINextFileNode );

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();
	HRESULT STDMETHODCALLTYPE OnShowProperties();

	// IDMUSProdPChannelName
	STDMETHOD(GetPChannelName)( DWORD dwPChannel, WCHAR* pwszName );
	STDMETHOD(SetPChannelName)( DWORD dwPChannel, WCHAR* pwszName );

	//Additional functions
protected:
	HRESULT LoadDefaultRuntimeFolders( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT SaveProjectChunk( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveProjectInfo( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveDefaultRuntimeFolders( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SavePChannelNames( IDMUSProdRIFFStream* pIRiffStream );

public:
	HRESULT LoadTheProject( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain ); 
	HRESULT SaveTheProject( IDMUSProdRIFFStream* pIRiffStream ); 

	BOOL Duplicate( LPCTSTR pszTargetDir );
	void DetermineDefaultRuntimeFolders();
	void RuntimeSaveAllFiles();
	void SyncFilesWithDisk( IDMUSProdNode* pIParentNode, CString& strPath );
	void FindNestedProjects( CString& strPath, CString& strNestedProjects );
	void DeleteNestedProjectFiles( CString& strPath );
	void DeleteNonExistentFiles();
	void SyncListInfo();
	BOOL DeleteAllFilesToRecycleBin( short nWhichFiles, BOOL fRuntimePrompt );
	void AddFile( CFileNode* pFileNode );
	void RemoveFile( CFileNode* pFileNode );
	void RenameOpenDocuments();
	int GetNbrFiles( DWORD dwFlags );
	void CloseAllFiles();
	BOOL SaveAllFiles();
	void CleanUpNotifyLists( CFileNode* pFileNode, BOOL fFileClosed  );
	CFileNode* CreateFileNode( LPCTSTR szFileName );
	CFileNode* FindFileNode( LPCTSTR szPathName );
	CFileNode* GetFileByGUID( GUID guidFile );
	CFileNode* GetFileByDocRootNode( IDMUSProdNode* pIDocRootNode );
	CFileNode* GetFileByText( IDMUSProdDocType* pIDocType, LPCTSTR szNodeName, LPCTSTR szNodeDescriptor );
	CFileNode* GetFileByRuntimeFileName( LPCTSTR szRuntimeFileName, CFileNode* pBypassFileNode, BOOL fUseDefaultName );
    CFileNode* GetBestGuessDocRootNode( IDMUSProdDocType* pIDocType, LPCTSTR szNodeName, IDMUSProdNode* pITreePositionNode );
	void GetDefaultRuntimeFolderByExt( LPCTSTR szExt, CString& strDefaultRuntimeFolder );
	void GetUniqueFileName( CString& strFileName );
	BOOL HandleRuntimeDuplicates();
	void AdjustProjectFolders( LPCTSTR szOrigDir, LPCTSTR szNewDir );
	void GenerateNewGUIDs();
	BOOL CanClose();

	// Member variables
public:
    CTypedPtrList<CPtrList, CFileNode*> m_lstFiles;
    CTypedPtrList<CPtrList, CJzRuntimeFolder*> m_lstRuntimeFolders;	// defaults per extension

    GUID		m_guid;
	CString		m_strDescription;		// Project Description
	CString		m_strLastModified;		// Project Last Modified
	CString		m_strLastModifiedBy;	// Project Last Modified By
	CString		m_strRuntimeDir;		// Default directory for Runtime files
	CString		m_strProjectDir;		// Default directory for Project files
	CJazzDoc*	m_pProjectDoc;
	CBookmark*	m_pBookmark;			// Used when loading Project	
	CTypedPtrList<CPtrList, PChannelName*> m_lstPChannelNames;// For PChannels > 31
	CString		m_aPChannelNames[32];	// For speedy access to PChannels 0-31
};

#endif //__PROJECT_H__
