
----------------------------------------------------
IDMUSProdComponent AutoDocs for DirectMusic Producer
----------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODCOMPONENT
========================================================================================
@interface IDMUSProdComponent | 
	DirectMusic Producer Components extend the functionality of DirectMusic Producer by
	way of a module mechanism.  They are standard registered COM objects that implement
	<i IDMUSProdComponent>.  The vast majority of Producer's functionality resides in
	<o Component> objects.  The <i IDMUSProdComponent> interface contains the methods
	to initialize and free a DirectMusic Producer Component.

@comm
	Implementation of <i IDMUSProdComponent> is mandatory.  The <o Framework> object obtains
	a pointer to a Component's <i IDMUSProdComponent> interface by passing the Component's
	CLSID to CoCreateInstance.  DirectMusic Producer calls CoCreateInstance for each of the
	Component CLSID's registered under:

	[HKEY_LOCAL_MACHINE\Software\Microsoft\DMUSProducer\Components].

@base public | IUnknown

@xref <o Component> Object, <o Framework> Object, <i IDMUSProdFramework>

@meth HRESULT | Initialize | Initializes the Component. Called during DirectMusic Producer launch.
@meth HRESULT | CleanUp | Counterpart to Initialize method. Called during DirectMusic Producer exit.
@meth HRESULT | GetName | Returns name of Component.
@meth HRESULT | AllocReferenceNode | Allocates a reference Node of type <p guidRefNodeId>.
@meth HRESULT | OnActivateApp | Notifies a Component that DirectMusic Producer is being
		activated or deactivated.
--------------------------------------------------------------------------------------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER COMPONENT OBJECT
========================================================================================
@object Component | Represents a DirectMusic Producer plug-in module.

@supint IDMUSProdComponent | Defines the startup and shut down behavior for a DirectMusic
		Producer Component. Implementation is required.
@supint IDMUSProdRIFFExt | Components implementing IDMUSProdRIFFExt agree to load RIFF 
		chunks and participate in the management of <o Node>s for other Components.  For
		example, the Band Component implements IDMUSProdRIFFExt and provides file I/O and
		an editor for Bands in existing in Styles and Segment Band tracks. Implementation
		is optional.

@comm
	DirectMusic Producer Components are standard registered COM objects.  These objects
	must implement the <i IDMUSProdComponent> interface and understand the
	<i IDMUSProdFramework> interface.

	During DirectMusic Producer launch, the <o Framework> object calls CoCreateInstance
	to obtain an <i IDMUSProdComponent> interface pointer for each of the DirectMusic
	Producer Component CLSID's registered under:

	[HKEY_LOCAL_MACHINE\Software\Microsoft\DMUSProducer\Components]

	DirectMusic Producer Component objects exist until the DirectMusic Producer application
	shuts down.

	After all registered DirectMusic Producer Components have been instantiated, the
	Framework turns around and calls each Component's <om IDMUSProdComponent::Initialize>
	method.  This method receives a pointer to the Framework's <i IDMUSProdFramework>
	interface.
	
	DirectMusic Producer Components are not required to create any other "standard"
	DirectMusic Producer objects. The purpose of a Component may simply be to provide
	functionality to other DirectMusic Producer Components through custom interfaces.
	However, <om IDMUSProdComponent::Initialize> will more than likely create one or more
	of the following DirectMusic Producer objects:  <o Menu> object, <o Toolbar> object,
	<o DocType> object. 
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODCOMPONENT::INITIALIZE
========================================================================================
@method HRESULT | IDMUSProdComponent | Initialize | 
	Initializes a DirectMusic Producer <o Component> object.

@comm
	The <o Framework> calls this method once during DirectMusic Producer startup.   

	In addition to allocating necessary resources, a Component may use the following
	<i IDMUSProdFramework> methods to further integrate itself into DirectMusic Producer:
	<om IDMUSProdFramework.AddDocType>, <om IDMUSProdFramework.AddClipFormatForFile>,
	<om IDMUSProdFramework.AddNodeImageList>, <om IDMUSProdFramework.AddMenuItem>,
	<om IDMUSProdFramework.AddToolBar>.

	Components which depend on other DirectMusic Producer Components should call
	<om IDMUSProdFramework.FindComponent> from within their <om IDMUSProdComponent.Initialize>
	method to ensure the needed Components are available.

@parm IDMUSProdFramework* | pIFramework | [in] Pointer to the <o Framework> object's <i IDMUSProdFramework>
		interface.
@parm BSTR* | pbstrErrMsg | [out] Pointer to a brief string that describes the error.
		Only used when an error has occurred.

@rvalue S_OK | The Component was successfully initialized.
@rvalue E_INVALIDARG | <p pIFramework> or <p pbstrErrMsg> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | The Component could not be initialized due to lack of memory.
@rvalue E_FAIL | The Component could not be initialized for some reason other than
		lack of memory or invalid arguments.  <p pbstrErrMsg> must briefly decribe the error.

@ex The following example initializes a DirectMusic Producer Component: |

HRESULT CMyComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	TCHAR achErrMsg[MID_BUFFER];
	CString strErrMsg;

	if( m_pIFramework )		// already initialized
	{
		return S_OK;
	}

	if( pIFramework == NULL
	||  pbstrErrMsg == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	// Make sure all dependent DirectMusic Producer Components are available
	// Get the IDMUSProdComponent interface for the Band component 
	if( !SUCCEEDED ( pIFramework->FindComponent( CLSID_BandComponent,  &m_pIBandComponent ) ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_BAND, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Add applicable images to the Project Tree control's image list 
	if( !SUCCEEDED ( AddNodeImageLists() ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_IMAGELIST, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Add menu item to Framework's Add-Ins menu 
	if( !SUCCEEDED ( pIFramework->AddMenuItem( (IDMUSProdMenu *)this ) ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_MENUITEM, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	else
	{
		m_fMenuWasAdded = TRUE;
	}

	// Register applicable doc types with the Framework
	// Register doc types after all other initialization has completed successfully
	m_pIDocType = new CStyleDocType( this );
    if( m_pIDocType == NULL )
    {
		CleanUp();
        return E_OUTOFMEMORY;
    }

	m_pIDocType->AddRef();

	if( !SUCCEEDED ( pIFramework->AddDocType(m_pIDocType) ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CMyComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, TRUE, 2, 0 );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_STYLE_DOCTYPE) );
	lstImages.Add( hIcon );
	::DeleteObject( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_STYLE_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DeleteObject( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstStyleImage ) ) )
	{
		return E_FAIL;
	}
	
	lstImages.Create( 16, 16, TRUE, 2, 0 );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_PERSONALITY) );
	lstImages.Add( hIcon );
	::DeleteObject( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_PERSONALITY_SEL) );
	lstImages.Add( hIcon );
	::DeleteObject( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstPersonalityImage ) ) )
	{
		return E_FAIL;
	}
	
	lstImages.Create( 16, 16, TRUE, 2, 0 );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER) );
	lstImages.Add( hIcon );
	::DeleteObject( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_SEL) );
	lstImages.Add( hIcon );
	::DeleteObject( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstFolderImage ) ) )
	{
		return E_FAIL;
	}
	
	return S_OK;
}
	
@xref <o Component> Object, <o Framework> Object, <i IDMUSProdComponent>, <om IDMUSProdComponent.CleanUp>, <i IDMUSProdFramework>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODCOMPONENT::CLEANUP
========================================================================================
@method HRESULT | IDMUSProdComponent | CleanUp | 
	Notifies a DirectMusic Producer <o Component> that the application is closing.

@comm 
	Counterpart to <om IDMUSProdComponent.Initialize>.  The <o Framework> calls this method
	once when DirectMusic Producer exits.

	Provides an opportunity for a DirectMusic Producer Component to release the objects
	it manages that contain references to itself.  
	
	In addition, <om IDMUSProdComponent.CleanUp> must also remove all <o Menu>s and
	<o ToolBar>s that the Component added to the Framework.

@rvalue S_OK | Clean up processing completed successfully.

@ex The following example contains clean up processing for a DirectMusic Producer Component: |

HRESULT CMyComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CStyle *pStyle;

	while( !m_lstStyles.IsEmpty() )
	{
		pStyle = static_cast<CStyle*>( m_lstStyles.RemoveHead() );
		pStyle->Release();
	}
	
	return S_OK;
}
	
@xref <i IDMUSProdComponent>, <om IDMUSProdComponent.Initialize>, <om IDMUSProdFramework.RemoveMenuItem>, <om IDMUSProdFramework.RemoveToolBar>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODCOMPONENT::GETNAME
========================================================================================
@method HRESULT | IDMUSProdComponent | GetName | 
	Returns the name of a DirectMusic Producer Component.

@comm 
	The Framework uses this text, for example, when reporting errors during <o Component>
	initialization.

@parm BSTR*| pbstrName | [out, retval] Pointer to the caller-allocated variable that
	receives the copy of the name.  The caller must free <p pbstrName> with SysFreeString
	when it is no longer needed. 

@rvalue S_OK | The name was returned successfully.
@rvalue E_FAIL | An error occurred and the name was not returned.

@ex The following example retrieves the name of a DirectMusic Producer Component: |

	IDMUSProdComponent* pIComponent;
	CString strName;
	BSTR bstrName;
	
	pIComponent->GetName( &bstrName );
	strName = bstrName;
	::SysFreeString( bstrName );
	
@xref <i IDMUSProdComponent>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODCOMPONENT::ALLOCREFERENCENODE
========================================================================================
@method HRESULT | IDMUSProdComponent | AllocReferenceNode | 
	Allocates a reference <o Node> of type <p guidRefNodeId>.

@comm
	Use <om IDMUSProdReferenceNode.SetReferencedFile> to link the reference Node to a file.

@parm GUID | guidRefNodeId | [in] Reference Node ID.
@parm IDMUSProdNode** | ppIRefNode | [out,retval] Address of a variable to receive the
		requested <i IDMUSProdNode> interface. 	If an error occurs, the implementation sets
		<p ppIRefNode> to NULL.  On success, the caller is responsible for calling
		<om IDMUSProdNode.Release>.

@rvalue S_OK | The reference Node was returned in <p ppIRefNode>.
@rvalue E_POINTER | The address in <p ppIRefNode> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | <p guidRefNodeId> is not valid.  For example, the Component may not
		know how to create a reference for that particular type of Node.
@rvalue E_OUTOFMEMORY | Out of memory.
@rvalue E_FAIL | An error occurred and the reference Node was not returned.

@ex The following example allocates a reference Node: |

HRESULT CMyComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_StyleRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create StyleRefNode
	CStyleRef* pStyleRef = new CStyleRef( this );
	if( pStyleRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pStyleRef;

	return S_OK;
}
	
@xref <o Node> Object, <i IDMUSProdComponent>, <i IDMUSProdFileRefChunk>, <i IDMUSProdNode>, <i IDMUSProdReferenceNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODCOMPONENT::ONACTIVATEAPP
========================================================================================
@method HRESULT | IDMUSProdComponent | OnActivateApp | 
	Notifies a <o Component> that DirectMusic Producer is being activated or deactivated.

@comm 
	The Framework calls this method when it receives a WM_ACTIVATEAPP message.

@parm BOOL | fActivate | [in] Specifies whether DirectMusic Producer is being activated
		or deactivated. This parameter is TRUE if the application is being activated; it
		is FALSE if the application is being deactivated.  

@rvalue S_OK | This method completed successfully.
@rvalue E_NOTIMPL | Not implemented.
@rvalue E_FAIL | An error occurred.

@xref <i IDMUSProdComponent>
--------------------------------------------------------------------------------------*/

