
--------------------------------------------------
IDMUSProdDocType AutoDocs for DirectMusic Producer
--------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODDOCTYPE
========================================================================================
@interface IDMUSProdDocType |
	Additional document types can be added to the Framework's File New, File Open and File
	Save dialogs through creation of one or more <o DocType> objects.  One DocType object must
	be created for each type of file that a DirectMusic Producer Component wants to process.
	DocType objects must implement the <i IDMUSProdDocType> interface.

@comm
	A Component's <om IDMUSProdComponent.Initialize> method should register its DocType
	object(s) through call(s) to <om IDMUSProdFramework.AddDocType>.

@base public | IUnknown

@xref <o DocType> Objects, <om IDMUSProdFramework.AddDocType>, <i IDMUSProdNode>

@meth HRESULT | GetResourceId | Returns HINSTANCE and resource ID for DocType's icon,
		accelerator table and string resource.
@meth HRESULT | DoesExtensionMatch | Returns S_OK if specified file extension matches
		this DocType's extension.
@meth HRESULT | DoesIdMatch | Returns S_OK if specified node ID is associated with this
		DocType.
@meth HRESULT | AllocNode | Creates a new Node object and returns a pointer to its
		IDMUSProdNode interface.
@meth HRESULT | OnFileNew | Creates a new file and adds the file's corresponding Node
		object(s) to the Project Tree.
@meth HRESULT | OnFileOpen | Loads the specified stream and adds the corresponding
		Node object(s) to the Project Tree.
@meth HRESULT | OnFileSave | Saves the file to the specified stream.
@meth HRESULT | GetListInfo | Fills a DMUSProdListInfo structure with information describing
		the object in <p pIStream>.
@meth HRESULT | IsFileTypeExtension | Determines whether the specified extension 
		is valid for the specified file type.
--------------------------------------------------------------------------------------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER DOCTYPE OBJECT
========================================================================================
@object DocType | Represents a type of document, or file.

@supint IDMUSProdDocType | Primary interface.  Implementation is required.

@comm
	A DirectMusic Producer <o Component> can add additional document types to the
	<o Framework>'s File New, File Open and File Save dialogs.  One DocType object must
	be created for each type of file that the Component wants to process.  These objects
	must implement <i IDMUSProdDocType>.

	After creation, the object's <i IDMUSProdDocType> interface gets handed to the Framework
	via the Framework's <om IDMUSProdFramework::AddDocType> method.  This method registers
	the DocType with DirectMusic Producer so that it will appear in the Framework's File New,
	File Open and File Save dialogs.

	DirectMusic Producer DocType objects must be created and registered with the Framework
	from within their Component's <om IDMUSProdComponent::Initialize> method.  These objects
	exist throughout the entire DirectMusic Producer work session.
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::GETRESOURCEID
========================================================================================
@method HRESULT | IDMUSProdDocType | GetResourceId | A Component can call
		<om IDMUSProdFramework.AddDocType> to add an additional <o DocType> to the Framework's
		File New, FileOpen and File Save dialogs.  GetResourceId returns the HINSTANCE and
		resource ID storing the DocType's icon, accelerator table (if needed) and
		string resource.
 
@comm
	The string resource consists of up to seven substrings separated by the '\n' character
	(the '\n' character is needed as a place holder if a substring is not included; however,
	trailing '\n' characters are not necessary); these substrings describe the DocType:

	1. <p windowTitle>   Name that appears in the application window's title bar (for example,
		"Style Editor").  Only used in SDI applications.
	
	2. <p docName>   Root for the default document name (for example, "Style"). This root,
		plus a number, is used for the default name of a new document of this type whenever the
		user chooses the New command from the File menu (for example, "Style1" or "Style2").
		If not specified, "Untitled" is used as the default.
	
	3. <p fileName>	  Name of this DocType. If the application supports more than one type
		of document, this string is displayed in the File New dialog box (for example, "Style").
		If not specified, the DocType is inaccessible using the File New command.	
	
	4. <p filterName>   Description of the DocType and a wildcard filter matching documents
		of this type. This string is displayed in the List Files Of Type drop-down list in the
		File Open dialog box (for example, "Style Files (*.sty;*.stp)").  If not specified, the
		DocType  is inaccessible using the File Open command.  Filters should include both the
		runtime and design-time extensions.
	
	5. <p filterExt>  Extension for documents of this type (for example, ".sty;.stp"). If not
		specified, the DocType is inaccessible using the File Open command.  Filters should 
		nclude both the runtime and design-time extensions. 
	
	6. <p regTypeId>   Identifier for the DocType to be stored in the registration database
		maintained by Windows.  This string is for internal use only (for example, "DirectMusic.Style").
		If not specified, the DocType cannot be registered with the Windows File Manager.
	
	7. <p regTypeName>   Name of the DocType to be stored in the registration database.
		This string may be displayed in dialog boxes of applications that access the registration
		database (for example, "DirectMusic Style").

@parm HINSTANCE* | phInstance | [out] Pointer to handle of the module containing the resources
		for this DocType.
@parm UINT* | pnResourceId | [out] Pointer to ID of the resources used for this DocType.

@rvalue S_OK | HINSTANCE and resource ID were returned in <p phInstance> and <p pnResourceId>. 
@rvalue E_POINTER | The address in either <p phInstance> or <p pnResourceId> is not valid.
		For example, it may be NULL.

@ex The resources are found in the Component DLL's resource file.  For example: |

	IDR_STYLE_DOCTYPE ICON	DISCARDABLE     "res\\Style.ico"				

	STRINGTABLE DISCARDABLE 
	BEGIN
		IDR_STYLE_DOCTYPE	"\nStyle\nStyle\nStyle Files (*.sty;*.stp)\n.sty;.stp\nDirectMusic.Style\nDirectMusic Style"
	END

@ex	Note that the string begins with a '\n' character; this is because the first substring is not
	used for MDI applications and so is not included.  You can edit this string using the string
	editor; the entire string appears as a single entry in the String Editor, not as seven separate
	entries. |

@ex The following example returns the HINSTANCE and resource ID for the DocType: |

HRESULT CStyleDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( phInstance == NULL
	||  pnResourceId == NULL )
	{
		return E_POINTER;
	}

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_STYLE_DOCTYPE;

	return S_OK;
}

@xref <i IDMUSProdDocType>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::DOESEXTENSIONMATCH
========================================================================================
@method HRESULT | IDMUSProdDocType | DoesExtensionMatch | Returns S_OK if <p bstrExt> matches
	this <o DocType>'s file extension.
		
@comm 
	The <i IDMUSProdDocType> interface provides methods for handling File New and File Open
	commands.  After the user has invoked File New or File Open, the Framework enumerates
	the registered <i IDMUSProdDocType> interfaces calling <om IDMUSProdDocType.DoesExtensionMatch>
	to find the appropriate <i IDMUSProdDocType> interface for the requested operation.  Once
	found, either <om IDMUSProdDocType.OnFileNew> or <om IDMUSProdDocType.OnFileOpen> are called to
	complete the user's request.

@parm BSTR | bstrExt | [in] File extension (i.e. ".sty").  This method must free
		<p bstrExt> with SysFreeString when it is no longer needed.

@rvalue S_OK | <p bstrExt> matches the file extension of this DocType. 
@rvalue S_FALSE | <p bstrExt> does not match the file extension of this DocType.
@rvalue E_FAIL | An error occurred, and <p bstrExt> could not be checked against this DocType.

@ex The following example determines whether or not the specified file extension matches
	this DocType: |

HRESULT CStyleDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_STYLE_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
	{
		return E_FAIL;
	}

	if( AfxExtractSubString(strDocTypeExt, achBuffer, CDocTemplate::filterExt) )
	{
		ASSERT( strDocTypeExt[0] == '.' );

		BOOL fContinue = TRUE;
		CString strDocExt;
		int nFindPos;

		nFindPos = strDocTypeExt.Find( _T(";") );
		while( fContinue )
		{
			if( nFindPos == -1 )
			{
				fContinue = FALSE;

				nFindPos = strDocTypeExt.Find( _T(".") );
				if( nFindPos != 0 )
				{
					break;
				}
				strDocExt = strDocTypeExt;
			}
			else
			{
				strDocExt = strDocTypeExt.Left( nFindPos );
				strDocTypeExt = strDocTypeExt.Right( strDocTypeExt.GetLength() - (nFindPos + 1) ); 
			}

			if( _tcsicmp(strExt, strDocExt) == 0 )
			{
				return S_OK;	// extension matches 
			}

			nFindPos = strDocTypeExt.Find( _T(";") );
		}
	}

	return S_FALSE;
}
	
@xref <i IDMUSProdDocType>, <om IDMUSProdFramework.FindDocTypeByExtension>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::DOESIDMATCH
========================================================================================
@method HRESULT | IDMUSProdDocType | DoesIdMatch | Returns S_OK if this <o DocType> knows how
		to manage/allocate <o Node>s of type <p rguid>.

@comm
	A GUID identifies a type of Node.  For example, all Style Nodes have the same
	GUID, or node ID.  

	A DocType may be associated with more than one node ID, meaning that it knows how to
	process more than one type of Node object.  

@parm REFGUID | rguid | [in] Node ID.  

@rvalue S_OK | <p rguid> matches one of the node ID's supported by this DocType. 
@rvalue S_FALSE | <p rguid> is not supported by this DocType.
@rvalue E_FAIL | An error occurred, and <p rguid> could not be checked against this DocType.

@ex The following <om IDMUSProdDocType.DoesIdMatch> method determines whether or not the specified
	ID is supported by the DocType object: |

HRESULT CBandDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_BandNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}
	
@xref <i IDMUSProdDocType>, <om IDMUSProdFramework.FindDocTypeByNodeId>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::ALLOCNODE
========================================================================================
@method HRESULT | IDMUSProdDocType | AllocNode | Creates a new <o Node> object and returns a pointer
		to its <i IDMUSProdNode> interface. 

@comm
	The <om IDMUSProdDocType.AllocNode> method knows how to create new instances of one or more
	types of Node objects.  This method differs from <om IDMUSProdDoctype.OnFileNew> in that it
	does not create a new file.  It simply provides an <i IDMUSProdNode> interface to a newly
	created Node that can then be inserted within a file.  The newly created Node may have
	children.

	This method must call <om IDMUSProdNode.AddRef> on <p ppINode> before returning.

@parm REFGUID | rguid | [in] Identifies type of Node object to create. 
@parm IDMUSProdNode** | ppINode | [out,retval] Address of a variable to receive the requested
		  <i IDMUSProdNode> interface.  If an error occurs, the implementation sets <p ppINode>
		  to NULL.  On success, the caller is responsible for calling <om IDMUSProdNode.Release>
		  when this pointer is no longer needed.

@rvalue S_OK | The newly created object's <i IDMUSProdNode> was returned in <p ppINode>.
@rvalue E_POINTER | The address in <p ppINode> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | Cannot create Node object of type <p rguid>.
@rvalue E_OUTOFMEMORY | Out of memory.

@ex The following example creates a Band Node: |

HRESULT STDMETHODCALLTYPE CBandDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pBandComponent != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_BandNode) == FALSE )
	{
		return E_INVALIDARG;
	}

	// Create a new Band 
	CBand* pBand = new CBand( m_pBandComponent );
	if( pBand == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	pBand->AddRef();
	*ppINode = (IDMUSProdNode *)pBand;

	return S_OK;
}

@ex The <om IDMUSProdDocType.AllocNode> method knows how to create new instances of Node objects.
	This method differs from <om IDMUSProdDoctype.OnFileNew> in that it does not create a new
	file.  It simply provides an <i IDMUSProdNode> interface to a newly created Project Tree
	Node that can then be inserted within another file.
		
	A Style file contains Band Nodes.  The Style file's Band Nodes are actually created and
	managed by a different DirectMusic Producer Component.  When the Style Editor Component
	recognizes that the user wants to insert a new Band into a Style, it must locate the
	<i IDMUSProdDocType> interface whose <om IDMUSProdDocType.AllocNode> method knows how to
	create Band Nodes. This is accomplished by calling <om IDMUSProdFramework.FindDocTypeByNodeId>.
	<om IDMUSProdFramework.FindDocTypeByNodeId> enumerates the registered <i IDMUSProdDocType> interfaces
	calling <om IDMUSProdDocType.DoesIdMatch> to find the requested <i IDMUSProdDocType> interface.
	
	The following code inserts a new Band into the 'Bands' folder of a Style: |

HRESULT CStyleBands::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdDocType* pIDocType;

	ASSERT( m_pStyleComponent != NULL );
	ASSERT( m_pStyleComponent->m_pIFramework != NULL );
	ASSERT( m_pStyle != NULL );

	if( pIChildNode == NULL )	// Create a new Band
	{
		HRESULT hr;

		hr = m_pStyleComponent->m_pIFramework->FindDocTypeByNodeId( GUID_BandNode, &pIDocType );
		if( SUCCEEDED ( hr ) ) 
		{
			// Create a new Band 
			hr = pIDocType->AllocNode( GUID_BandNode, &pIChildNode );
			if( !SUCCEEDED ( hr ) )
			{
				pIChildNode = NULL;
			}

			pIDocType->Release();
		}

		if( pIChildNode == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		pIChildNode->AddRef();
	}

	// add to CStyle Band list
	m_lstBands.AddTail( pIChildNode );

	// Set DocRoot node and parent Node of ALL children
	theApp.SetNodePointers( pIChildNode, (IDMUSProdNode *)m_pStyle, (IDMUSProdNode *)this );

	// Add Node to Project Tree
	if( !SUCCEEDED ( m_pStyleComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( pIChildNode );
		return E_FAIL;
	}

	m_pStyleComponent->m_pIFramework->SetModifiedFlag( (IDMUSProdNode *)this, TRUE );

	return S_OK;
}
	
@xref <i IDMUSProdDocType>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::ONFILENEW
========================================================================================
@method HRESULT | IDMUSProdDocType | OnFileNew | Creates a new file and adds the corresponding
		<o Node> object(s) to the Project Tree. 

@comm
	This method gets called in response to a File New command.

	A file may consist of one or more <o Node> objects.  The top Node is the Node that must
	be returned in <p ppIDocRootNode>.  The Framework associates <p ppIDocRootNode> with
	its own representation of a file object.

	This method must call <om IDMUSProdNode.AddRef> on <p ppIDocRootNode> before returning.

@parm IDMUSProdProject* | pITargetProject | [in] <i IDMUSProdProject> interface pointer to the
		<o Project> that will contain this file.
@parm IDMUSProdNode* | pITargetDirectoryNode | [in] <i IDMUSProdNode> interface pointer to the
		Project Tree Directory <o Node> that will contain this file. 
@parm IDMUSProdNode** | ppIDocRootNode | [out,retval] Address of a variable to receive the
		requested <i IDMUSProdNode> interface.  If an error occurs, the implementation sets
		<p ppIDocRootNode> to NULL.  On success, the caller is responsible for calling
		<om IDMUSProdNode.Release> when this pointer is no longer needed.

@rvalue S_OK | The newly created object's top <i IDMUSProdNode> was returned in
		<p ppIDocRootNode>.
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may
		be NULL.
@rvalue E_OUTOFMEMORY | Out of memory.

@ex The following example creates a new Band file and places its DocRoot node into the
	active folder of the Project Tree.  Notice that the <om IDMUSProdDocType.AllocNode> method
	is actually responsible for creating the Band object: |

HRESULT CBandDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIBandNode;
	HRESULT hr;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Band 
	hr = AllocNode( GUID_BandNode, &pIBandNode );	// performs AddRef()
	if( SUCCEEDED ( hr ) )
	{
		// Insert the Band Node into the Project Tree
		hr = InsertChildNode( pIBandNode );
		if( SUCCEEDED ( hr ) )
		{
			*ppIDocRootNode = pIBandNode;
			m_pComponent->m_pIFramework->SetModifiedFlag( pIBandNode, TRUE );
			hr = S_OK;
		}
		else
		{
			pIBandNode->Release();
		}
	}

	return hr;
}
	
@xref <o Node> Object, <i IDMUSProdDocType>, <om IDMUSProdDocType.OnFileOpen>, <om IDMUSProdDocType.OnFileSave>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::ONFILEOPEN
========================================================================================
@method HRESULT | IDMUSProdDocType | OnFileOpen | Loads the specified stream and adds the
		corresponding <o Node> object(s) to the Project Tree. 

@comm
	This method gets called in response to a File Open command.

	A file may consist of one or more <o Node> objects.  The top Node is the Node that
	must be returned in <p ppIDocRootNode>.  The Framework associates <p ppIDocRootNode>
	with its own representation of a file object.

	This method must call <om IDMUSProdNode.AddRef> on <p ppIDocRootNode> before returning.
	
@parm IStream* | pIStream | [in] Pointer to the stream from which the object should be
		loaded.
@parm IDMUSProdProject* | pITargetProject | [in] <i IDMUSProdProject> interface pointer to the
		<o Project> that will contain this file.
@parm IDMUSProdNode* | pITargetDirectoryNode | [in] <i IDMUSProdNode> interface pointer to the
		Project Tree Directory <o Node> that will contain this file. 
@parm IDMUSProdNode** | ppIDocRootNode | [out,retval] Address of a variable to receive the
		requested <i IDMUSProdNode> interface.  If an error occurs, the implementation sets
		<p ppIDocRootNode> to NULL.  On success, the caller is responsible for calling
		<om IDMUSProdNode.Release> when this pointer is no longer needed.

@rvalue S_OK | The newly created object's top <i IDMUSProdNode> was returned in
		<p ppIDocRootNode>.
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example,
		it may be NULL.
@rvalue E_OUTOFMEMORY | Out of memory.

@ex The following example loads a Band file and places its DocRoot node into the Project
	Tree's active folder.  Notice that the Band Editor Component's
	<om IDMUSProdComponent.LoadRIFFChunk> method is actually responsible for loading the Band
	object: |

HRESULT CBandDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pINode;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pComponent != NULL );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( SUCCEEDED ( m_pComponent->LoadRIFFChunk( pIStream, &pINode ) ) )	// performs AddRef()
	{
		// Insert a Node into the Project Tree
		hr = InsertChildNode( pINode );
		if( !SUCCEEDED ( hr ) )
		{
			pINode->Release();
			return hr;
		}
	}

	*ppIDocRootNode = pINode;
	return S_OK;
}
	
@xref <o Node> Object, <i IDMUSProdDocType>, <om IDMUSProdDocType.OnFileNew>, <om IDMUSProdDocType.OnFileSave>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::ONFILESAVE
========================================================================================
@method HRESULT | IDMUSProdDocType | OnFileSave | Saves the file to the specified stream.

@comm
	This method gets called in response to a File Save or File Save As command.

	A file may consist of one or more <o Node> objects.  The top Node of the file is the
	Node specified in <p pIDocRootNode>.  The Framework associates <p pIDocRootNode>
	with its own representation of a file object.
	
@parm IStream* | pIStream | [in] Pointer to the stream into which the object should be
		saved.
@parm IDMUSProdNode* | pIDocRootNode | [in] Pointer to the file's DocRoot <i IDMUSProdNode>
		interface.

@rvalue S_OK | The file specified in <p pIDocRootNode> was saved successfully.
@rvalue E_FAIL | An error occurred, and the file was not saved.
	
@xref <o Node> Object, <i IDMUSProdDocType>, <om IDMUSProdDocType.OnFileNew>, <om IDMUSProdDocType.OnFileOpen>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::GETLISTINFO
========================================================================================
@method HRESULT | IDMUSProdDocType | GetListInfo | Fills a <t DMUSProdListInfo> structure with information
	describing the object in <p pIStream>. 

@parm IStream* | pIStream | [in] Pointer to the stream from which the object should be
		loaded.
@parm DMUSProdListInfo* | pListInfo  | [in] Pointer to a DMUSProdListInfo structure.  The caller must free
	pListInfo->bstrDescriptor and pListInfo->bstrName with SysFreeString when they are no longer
	needed.

@comm
	The <o Framework> calls <om IDMUSProdDocType::GetListInfo> to retrieve information later returned
	by <om IDMUSProdNode.GetNodeListInfo> for files that exist in the Project Tree but have not yet
	been loaded.

	Lists of specific types of Nodes can be built by calling <om IDMUSProdProject.GetFirstFileByDocType>
	and <om IDMUSProdProject.GetNextFileByDocType>.  After acquiring each <i IDMUSProdNode> pointer,
	<om IDMUSProdNode.GetNodeListInfo> can be called to obtain text for the combo box item.
	After a selection is made, <om IDMUSProdFramework.FindDocRootNode> can be called to obtain an
	IDMUSProdNode pointer for the selected item.

	The caller must set DMUSProdListInfo.wSize to the size of the DMUSProdListInfo structure.

@rvalue S_OK | Information was successfully placed in the DMUSProdListInfo structure.
@rvalue E_INVALIDARG | The address in either <p pIStream> or <p pListInfo> is not valid.
		For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the information was not returned.

@ex The following example fills a DMUSProdListInfo structure for a Style file: |

HRESULT CStyleDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
    HRESULT hr;

    ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

    hr = E_FAIL;

    if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
    {
        ckMain.fccType = FOURCC_STYLE_FORM;

        if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
        {
	        ck.ckid = FOURCC_STYLE;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
			    ioStyle iStyle;
				DWORD dwSize;
				DWORD dwByteCount;

				dwSize = min( ck.cksize, sizeof( iStyle ) );
				hr = pIStream->Read( &iStyle, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
				}
				else
				{
					CString strName;
					CString strDescriptor;

					strName = iStyle.wstrName;
					pListInfo->bstrName = strName.AllocSysString();

					WORD wClicksPerBeat = iStyle.wClocksPerBeat / iStyle.wClocksPerClick;
					strDescriptor.Format( "%d/%d", iStyle.wBPM, iStyle.wBeat );
					if( !(wClicksPerBeat % 3) )
					{
						strDescriptor += _T("  3");
					}
					pListInfo->bstrDescriptor = strDescriptor.AllocSysString();

					hr = S_OK;
				}
			}
        }

        pIRiffStream->Release();
    }

	return hr;
}
	
@xref <o Node> Object, <i IDMUSProdDocType>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODDOCTYPE::ISFILETYPEEXTENSION
========================================================================================
@method HRESULT | IDMUSProdDocType | IsFileTypeExtension | Determines whether <p bstrExt> is a 
		valid extension for files with a file type of <p ftFileType>.

@comm
	DirectMusic Producer allows users to save files in either design-time or runtime format.
	This method associates <p bstrExt> with a specific file type.

	The following file types are valid for use in <p ftFileType>:
	
	FT_DESIGN        Design-time file.  File contains UI state information for use by its editor(s). 
	
	FT_RUNTIME      Runtime file.  File stores content in its most compact form.
	
@parm FileType | ftFileType | [in] Type of file.  <p ftFileType> must be FT_DESIGN or
		FT_RUNTIME.
@parm BSTR | bstrExt | [in] File extension (i.e. ".sty").  This method must free
		<p bstrExt> with SysFreeString when it is no longer needed.

@rvalue S_OK | <p bstrExt> is a valid extension for files with a FileType of <p ftFileType>. 
@rvalue S_FALSE | <p bstrExt> is not a valid extension for files with a FileType of
		<p ftFileType>. 
@rvalue E_FAIL | An error occurred, and <p bstrExt> could not be checked against <p ftFileType>.

@ex The following example determines whether or not <p bstrExt> is a valid extension for
		files with a type of <p ftFileType>: |

HRESULT CStyleDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".stp") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".sty") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}
	
@xref <i IDMUSProdDocType>, <i IDMUSProdPersistInfo>, <om IDMUSProdPersistInfo.GetStreamInfo>
--------------------------------------------------------------------------------------*/
