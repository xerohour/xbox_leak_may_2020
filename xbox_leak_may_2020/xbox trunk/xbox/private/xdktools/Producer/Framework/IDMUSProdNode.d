
-----------------------------------------------
IDMUSProdNode AutoDocs for DirectMusic Producer
-----------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
STRUCT:  DMUSPRODLISTINFO
========================================================================================
@struct DMUSProdListInfo | Contains information used to describe a <o Node>.  Used when
		constructing lists to show available Nodes.
 

@field WORD | wSize | Size of this structure.  Must be filled prior to calling a
		method that populates the remaining DMUSProdListInfo fields. 
@field BSTR | bstrName | Node's name.  For example, the user name assigned to a Style.
@field BSTR | bstrDescriptor | Additional text to further describe this Node.  For example,
		a Style's time signature.
@field GUID | guidObject | GUID associated with the object represented by this Node.  For
		example, the Component generated GUID stored in a Style file.
--------------------------------------------------------------------------------------*/


/*======================================================================================
INTERFACE:  IDMUSPRODNODE
========================================================================================
@interface IDMUSProdNode | 
	This interface provides the functionality for a <o Node> in the Framework's Project Tree.

@base public | IUnknown

@xref <o Node> Object, <i IDMUSProdComponent>, <i IDMUSProdFramework>, 

@meth HRESULT | GetNodeImageIndex | Returns index into Project Tree's image list.
@meth HRESULT | UseOpenCloseImages | Determines whether Project Tree images follow open/close
		paradigm or select/non-select paradigm.
@meth HRESULT | GetFirstChild| Returns an IDMUSProdNode interface pointer for the first child Node
		in this Node's list of children.
@meth HRESULT | GetNextChild| Returns an IDMUSProdNode interface pointer for the next child Node
		in this Node's list of children.
@meth HRESULT | GetComponent | Returns pointer to the IDMUSProdComponent interface responsible
		for this Node.
@meth HRESULT | GetDocRootNode | Returns pointer to this Node's DocRoot node. 
@meth HRESULT | SetDocRootNode | Sets this Node's DocRoot node.
@meth HRESULT | GetParentNode | Returns pointer to this Node's parent node.
@meth HRESULT | SetParentNode | Sets this Node's parent node.
@meth HRESULT | GetNodeId | Returns GUID used to determine type of Node.
@meth HRESULT | GetNodeName | Returns text used for this Node's label.
@meth HRESULT | GetNodeNameMaxLength | Returns max length of Node's label.
@meth HRESULT | ValidateNodeName | Validates new text for Node's label.
@meth HRESULT | SetNodeName | Sets text used for Node's label.
@meth HRESULT | GetNodeListInfo | Fills a DMUSProdListInfo structure with information describing
		this Node.
@meth HRESULT | GetEditorClsId | Returns CLSID of Node's editor.
@meth HRESULT | GetEditorTitle | Returns title for Node's editor window.
@meth HRESULT | GetEditorWindow | Returns HWND of Node's editor.
@meth HRESULT | SetEditorWindow | Sets HWND of Node's editor.
@meth HRESULT | GetRightClickMenuId | Returns HINSTANCE and resource ID of Node's right-click
		context menu.
@meth HRESULT | OnRightClickMenuInit | Initializes state of menu items contained in Node's
		right-click context menu.
@meth HRESULT | OnRightClickMenuSelect | Performs actions associated with Node's right-click
		context menu.
@meth HRESULT | DeleteChildNode | Instructs parent node to delete specified Node.
@meth HRESULT | InsertChildNode | Instructs parent node to insert specified Node.
@meth HRESULT | DeleteNode | Deletes this Node.
@meth HRESULT | OnNodeSelChanged | Informs Node of changes to its selection state.
@meth HRESULT | CreateDataObject | Returns a pointer to an IDataObject interface containing
		the current contents of this Node.
@meth HRESULT | CanCut | Determines whether this Node can be cut.
@meth HRESULT | CanCopy | Determines whether this Node can be copied.
@meth HRESULT | CanDelete | Determines whether this Node can be deleted.
@meth HRESULT | CanDeleteChildNode | Determines whether <p pIChildNode> can be deleted.
@meth HRESULT | CanPasteFromData | Determines whether <p pIDataObject> contains data that can
		be pasted into this Node.
@meth HRESULT | PasteFromData | Inserts data from <p pIDataObject> into this Node.
@meth HRESULT | CanChildPasteFromData | Determines whether the <p pIDataObject> dropped or
		pasted onto <p pIChildNode> contains data that can be pasted into this Node.
@meth HRESULT | ChildPasteFromData | Inserts data from the <p pIDataObject> dropped or
		pasted onto <p pIChildNode> into this Node.
@meth HRESULT | GetObject | Retrieves a pointer to the requested interface for the object
		identified by <p rclsid>.
--------------------------------------------------------------------------------------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER NODE OBJECT
========================================================================================
@object Node | Represents a unit of data that can be edited and moved around within Producer.
		Originally, there was a one to one correspondence between Node objects and nodes in
		the Framework's Project Tree.  However, this is no longer the case.  A Band in a
		Segment's Band track is an example of a node that does not display in the Project Tree. 

@supint IDMUSProdNode | Defines behavior of Node object.  Implementation is required.
@supint IDMUSProdPropPageObject | Defines behavior of an object displayed in Framework's global
		property sheet.  Implementation is optional.
@supint IDMUSProdSortNode | Overrides alphabetical sorting of Nodes in the Framework's Project
		Tree.  Implementation is optional.
@supint IDMUSProdReferenceNode | Allows for file linking by providing the means to attach files
		to Nodes.  Implementation is required by DocRoot nodes representing files that can be
		referenced in other files; i.e. Styles, ChordMaps.
@supint IDMUSProdNotifySink | Callback to receive various notifications from linked files.
		Implementation is required by DocRoot nodes representing files that can contain
		links to other files; i.e. Segments, which may contain references to Styles, ChordMaps,
		and/or DLS Collections.
@supint IDMUSProdNodeDrop | Allows files dragged from Explorer to be dropped on a Node in the
		Project Tree.  Implementation is optional.
@supint IDMUSProdNotifyCPt | Callback to receive performance notification events from Conductor.
		Implementation is optional.
@supint IDMUSProdTimelineCallback | Callback to receive data changed notifications from Timeline.
		Implementation is required when a Node's editor uses the Timeline object.
@supint IDMUSProdConductorTempo | Allows Node to use the Tempo edit control in the Transport Controls
		toolbar.  Implementation is optional.
@supint IDMUSProdTransport | Allows Node to use the transport controls in the Transport Controls
		toolbar.  Implementation is optional.
@supint IDMUSProdSecondaryTransport | Places Node in the drop down list(s) found in the Secondary
		Segment toolbar.  Implementation is optional.
@supint IPersistStream | Saves/loads file.  Implementation by DocRoot nodes is required.

@comm
	Node objects must implement <i IDMUSProdNode>.  Also, since DirectMusic Producer does
	not understand the relationships between nodes, every Node object must take on the
	responsibility of managing its own children.  This means that every potential parent
	node must implement <om IDMUSProdNode.GetFirstChild> and <om IDMUSProdNode.GetNextChild>
	for the purpose of enumerating their children.  
	
	<i IDMUSProdSortNode> can be implemented to override alphabetical sorting of Node objects in
	the Project Tree.  When required, the implementation of IDMUSProdSortNode must take place in
	the parent node whose children require a custom sort algorithm.

	<i IDMUSProdNotifyCPt> receives notifications requested via <om IDMUSProdConductor.RegisterNotify>.
		
	There are seven (7) types of Node objects:  <I Project Folder>, <I Directory Folder>, <I File>,
	<I DocRoot>, <I Folder>, <I Data>, and <I Reference>.
	
	<I Project Folder>:  Project Folders are the top-most nodes in the Project Tree.  They represent
	an entire DirectMusic Producer <o Project>.  Project Folders exist as folders in Explorer, and 
	the Project Tree provides an Explorer-like view of their sub-directories and files.  The Framework
	creates and manages Project Folder nodes.

	<I Directory Folder>:  Project Folders may contain one or more Directory Folders, which are nothing
	more than sub-folders created for the purpose of organizing a Project.  Directory Folders exist as
	folders in Explorer, and the Project Tree provides an Explorer-like view of their sub-directories
	and files.  The Framework creates and manages Directory Folder nodes. 

	<I File>:  There is a one-to-one correspondence between files in a Project and File nodes.  The
	Framework creates and manages File nodes.  

	<I DocRoot>:  A DocRoot node is the one and only child node of a File node.  DocRoot nodes
	mark the place in the Project Tree hierarchy where responsibility shifts from the <o Framework>
	to the <o Component>.  DocRoot nodes must also implement <i IPersistStream>.

	DocRoot nodes are important to the Framework's own internal representation of a file contained
	within a DirectMusic Producer Project for a couple of reasons: they provide the pathway to the
	actual data in a file, and they are used by the mechanism which sets up references for the purpose
	of linking files.  <om IDMUSProdDocType.OnFileNew> and <om IDMUSProdDocType.OnFileOpen> hand the
	Framework a pointer to the DocRoot node (file) just inserted into the Project Tree.  The Framework
	expects the DocRoot node to provide an <i IPersisStream> interface so that it can handle file I/O
	appropriately.    
 
	<I Folder>:  A file in a DirectMusic Producer project may contain lists of objects.  For
	example, a Style file contains lists of Bands, Motifs, and Patterns.  These lists are
	placed in Folder nodes.  Folder node objects do not contain file data.  They simply represent
	lists of objects.  A Style file contains the following three (3) Folder nodes:  'Bands', Motifs',
	and 'Patterns'.  Folder nodes exist as children under DocRoot nodes, other Folder nodes, or Data
	nodes.
	
	<I Data>:  Data nodes represent chunks of data within a file.  They may or may not have children.
	Data nodes exist as children under DocRoot nodes, Folder nodes, or other Data nodes.

	<I Reference>:  Files that may be referenced from within other files must be able to create a set
	of DocRoot/Folder/Data nodes to represent themselves in the Project Tree as well as create Reference
	nodes that other files can use to set up file references.  Reference nodes do not have children.
	They are simply nodes that point to other DocRoot nodes in the Project Tree.  Reference nodes
	must implement both <i IDMUSProdNode> and <i IDMUSProdReferenceNode>.
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETNODEIMAGEINDEX
========================================================================================
@method HRESULT | IDMUSProdNode | GetNodeImageIndex | Returns index into Project Tree's image
		list. 

@comm
	The Project Tree maintains a list of images used when drawing nodes.  As nodes are
	inserted into the Project Tree, the Framework calls <om IDMUSProdNode.GetNodeImageIndex>
	so that it can associate a pair of consecutive images in its list with the node
	being inserted.  The <om IDMUSProdNode.UseOpenCloseImages> method determines whether
	the consecutive images pertain to non-selected/selected states or closed/opened states
	(folders).  

	How does the method know what index to return?  The corresponding Component's 
	<om IDMUSProdComponent.Initialize> method must call <om IDMUSProdFramework.AddNodeImageList>
	to add its images to the Project Tree's image list.  <om IDMUSProdFramework.AddNodeImageList>
	returns an index to the first item in the newly added image list.
	<om IDMUSProdNode.GetNodeImageIndex> should return an index based on the index received from
	<om IDMUSProdFramework.AddNodeImageList>. 
	
@parm short* | pnNbrFirstImage | [out,retval] Pointer to the image list index.

@rvalue S_OK | Image list index was returned in <p pnNbrFirstImage>. 

@ex In the following example, a Motif node asks the Motif Component to return the 
	'Motif node' index into the Project Tree's image list: |

HRESULT CMotif::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return( m_pComponent->GetMotifImageIndex(pnFirstImage) );
}

HRESULT CMotifComponent::GetMotifImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnFirstImage = m_nFirstMotifImage;
	
	return S_OK;
}

@ex Here is the code which actually adds the images to the Project Tree's image list: |

HRESULT CMotifComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, TRUE, 2, 0 );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_MOTIF_DOCTYPE) );
	lstImages.Add( hIcon );
	::DeleteObject( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_MOTIF_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DeleteObject( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstMotifImage ) ) )
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
	
@xref <om IDMUSProdComponent.Initialize>, <om IDMUSProdFramework.AddNodeImageList>, <i IDMUSProdNode>, <om IDMUSProdNode.UseOpenCloseImages>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::USEOPENCLOSEIMAGES
========================================================================================
@method HRESULT | IDMUSProdNode | UseOpenCloseImages | Determines whether <o Node>'s Project Tree
		images follow open/close paradigm (folder) or select/non-select paradigm. 

@comm
	Set <p pfUseOpenCloseImages> to TRUE if the Node is a Folder node, otherwise set
	<p pfUseOpenCloseImages> to FALSE.

@parm BOOL* | pfUseOpenCloseImages | [out,retval] Pointer to value indicating whether to
		follow open/close paradigm when drawing node's Project Tree images.

@rvalue S_OK | Value was returned in <p pfUseOpenCloseImages>. 
@rvalue E_POINTER | The address in <p pfUseOpenCloseImages> is not valid.  For example, it
		may be NULL.

@ex The following example instructs the Project Tree to use a select/non-select paradigm
	when drawing the node's images: |

HRESULT CMotif::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfUseOpenCloseImages == NULL )
	{
		return E_POINTER;
	}

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetNodeImageIndex>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETFIRSTCHILD
========================================================================================
@method HRESULT | IDMUSProdNode | GetFirstChild | Returns an <i IDMUSProdNode> interface
		 pointer for the first child <o Node> in this Node's list of children.

@comm
	Every <o Node> object in the Project Tree takes on the responsibility of managing its
	children. The Framework has no knowledge of objects in the Project Tree and relies
	on <om IDMUSProdNode.GetFirstChild> and <om IDMUSProdNode.GetNextChild> when building the
	Project Tree in its <om IDMUSProdFramework.AddNode> method.

	The implementation sets <p ppIFirstChildNode> to NULL when the list is empty.
	
	A Node object that never has children under any circumstances can return E_NOTIMPL.

@parm IDMUSProdNode** | ppIFirstChildNode | [out,retval]  Address of a variable to receive the
		requested <i IDMUSProdNode> interface.  If an error occurs, the implementation sets
		<p ppIFirstChildNode> to NULL.  On success, the caller is responsible for calling
		<om IDMUSProdNode.Release> when this pointer is no longer needed.

@rvalue S_OK | The first child Node was returned in <p ppIFirstChildNode>.
@rvalue E_POINTER | The address in <p ppIFirstChildNode> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred, and the first child Node could not be returned.
@rvalue E_NOTIMPL | This Node does not support children.  

@ex In the following example, a Node returns an IDMUSProdNode interface to its first child: |

HRESULT CDirectoryNode::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !m_lstNodes.IsEmpty() )
	{
		IDMUSProdNode* pINode = static_cast<IDMUSProdNode*>( m_lstNodes.GetHead() );
		
		if( pINode )
		{
			pINode->AddRef();
			*ppIFirstChildNode = pINode;
		}
	}

    return S_OK;
}
	
@xref <o Node> Object, <om IDMUSProdFramework.AddNode>, <i IDMUSProdNode>, <om IDMUSProdNode.GetNextChild>, 
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETNEXTCHILD
========================================================================================
@method HRESULT | IDMUSProdNode | GetNextChild | Returns an <i IDMUSProdNode> interface
		 pointer for the next child <o Node> in this Node's list of children.

@comm
	Every <o Node> object in the Project Tree takes on the responsibility of managing its
	children. The Framework has no knowledge of objects in the Project Tree and relies
	on <om IDMUSProdNode.GetFirstChild> and <om IDMUSProdNode.GetNextChild> when building the
	Project Tree in its <om IDMUSProdFramework.AddNode> method.

	Returns the Node located after <p pIChildNode> in this Node's list of children.

	The implementation sets <p ppINextChildNode> ito NULL when the end of the list has
	been reached.
	
	A Node object that never has children under any circumstances can return E_NOTIMPL.

@parm IDMUSProdNode* | pIChildNode | [in]  A pointer to the previous child Node in this Node's
		list of children.
@parm IDMUSProdNode** | ppINextChildNode | [out,retval]  Address of a variable to receive the
		requested <i IDMUSProdNode> interface.  If an error occurs, the implementation sets
		<p ppINextChildNode> to NULL.  On success, the caller is responsible for calling
		<om IDMUSProdNode.Release> when this pointer is no longer needed.

@rvalue S_OK | The next child Node was returned in <p ppINextChildNode>.
@rvalue E_POINTER | The address in <p ppINextChildNode> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred, and the next child Node could not be returned.
@rvalue E_NOTIMPL | This Node does not support children.  

@ex In the following example, a Node returns an IDMUSProdNode interface to its next child: |

HRESULT CDirectoryNode::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINextChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppINextChildNode = NULL;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdNode* pINode;

    POSITION pos = m_lstNodes.GetHeadPosition();

    while( pos )
    {
        pINode = m_lstNodes.GetNext( pos );
		if( pINode == pIChildNode )
		{
			if( pos )
			{
				pINode = m_lstNodes.GetNext( pos );

				pINode->AddRef();
				*ppINextChildNode = pINode;
			}
			break;
		}
    }

    return S_OK;
}
	
@xref <o Node> Object, <om IDMUSProdFramework.AddNode>, <i IDMUSProdNode>, <om IDMUSProdNode.GetFirstChild>, 
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETCOMPONENT
========================================================================================
@method HRESULT | IDMUSProdNode | GetComponent | Returns a pointer to the <i IDMUSProdComponent>
		interface responsible for this <o Node>. 

@parm IDMUSProdComponent** | ppIComponent | [out,retval]  Address of a variable to receive the
		requested <i IDMUSProdComponent> interface.  On success, the caller is responsible for
		calling <om IDMUSProdComponent.Release> when this pointer is no longer needed.

@rvalue S_OK | The Node's corresponding <i IDMUSProdComponent> was returned in <p ppIComponent>.
@rvalue E_POINTER | The address in <p ppIComponent> is not valid.  For example, it may be NULL.
	
@xref <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETDOCROOTNODE
========================================================================================
@method HRESULT | IDMUSProdNode | GetDocRootNode | Returns a pointer to this <o Node>'s DocRoot
		node.
		
@comm
	A DocRoot node is the top <o Node> of a file.  For example, the Style node of a DirectMusic
	Style would be considered the DocRoot node of a Style file.  The Band node of a
	DirectMusic Band would be considered the DocRoot node of a Band file.

	This method must call <om IDMUSProdNode.AddRef> on <p ppIDocRootNode> before returning.

@parm IDMUSProdNode** | ppIDocRootNode | [out,retval] Address of a variable to receive the
		requested <i IDMUSProdNode> interface.  On success, the caller is responsible for
		calling <om IDMUSProdNode.Release> when this pointer is no longer needed.

@rvalue S_OK | The DocRoot <i IDMUSProdNode> was returned in <p ppIDocRootNode>.
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may
		be NULL.
@rvalue E_NOTIMPL | Node is not part of a file.  

@ex The following example returns a Node's DocRoot node: |

HRESULT CMotif::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDocRootNode != NULL );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	m_pIDocRootNode->AddRef();
	*ppIDocRootNode = m_pIDocRootNode;

	return S_OK;
}
	
@xref <o Node> Object, <i IDMUSProdNode>, <om IDMUSProdNode.SetDocRootNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::SETDOCROOTNODE
========================================================================================
@method HRESULT | IDMUSProdNode | SetDocRootNode | Sets this <o Node>'s DocRoot node.  

@comm
	A DocRoot node is the top <o Node> of a file.  For example, the Style node of a
	DirectMusic Style would be considered the DocRoot node of a Style file.  The Band node
	of a DirectMusic Band would be considered the DocRoot node of a Band file.

@parm IDMUSProdNode* | pIDocRootNode | [in] Pointer to the new DocRoot <i IDMUSProdNode> interface.

@rvalue S_OK | <p pIDocRootNode> was stored successfully. 
@rvalue E_INVALIDARG | <p pIDocRootNode> is not valid.  For example, it may be NULL.
@rvalue E_NOTIMPL | Node is not part of a file.

@ex The following example sets a Node's DocRoot node: |

HRESULT CMotif::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDocRootNode == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}
	
@xref <o Node> Object, <i IDMUSProdNode>, <om IDMUSProdNode.GetDocRootNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETPARENTNODE
========================================================================================
@method HRESULT | IDMUSProdNode | GetParentNode | Returns a pointer to this <o Node>'s parent
		node. 

@comm
	This method must call <om IDMUSProdNode.AddRef> on <p ppIParentNode> before returning.

@parm IDMUSProdNode** | ppIParentNode | [out,retval] Address of a variable to receive the
		requested <i IDMUSProdNode> interface.  On success, the caller is responsible for
		calling <om IDMUSProdNode.Release> when this pointer is no longer needed.

@rvalue S_OK | The parent <i IDMUSProdNode> was returned in <p ppIParentNode>.
@rvalue E_POINTER | The address in <p ppIParentNode> is not valid.  For example, it may
		be NULL.
@rvalue E_NOTIMPL | DirectMusic Producer Component does not manage this Node's parent.

@ex The following example returns a Node's parent node: |

HRESULT CMotif::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.SetParentNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::SETPARENTNODE
========================================================================================
@method HRESULT | IDMUSProdNode | SetParentNode | Sets this <o Node>'s parent node. 

@parm IDMUSProdNode* | pIParentNode | [in] Pointer to the new parent <i IDMUSProdNode> interface.

@rvalue S_OK | <p pIParentNode> was stored successfully. 
@rvalue E_INVALIDARG | <p pIParentNode> is not valid.  For example, it may be NULL.
@rvalue E_NOTIMPL | DirectMusic Producer Component does not manage this Node's parent.

@ex The following example sets a Node's parent node: |

HRESULT CMotif::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIParentNode == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetParentNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETNODEID
========================================================================================
@method HRESULT | IDMUSProdNode | GetNodeId | Returns a GUID used to determine type of <o Node>. 

@comm
	A GUID identifies the type of Node.  For example, all Style Nodes have the same
	GUID, or node ID.  

	Nodes created through <om IDMUSProdDocType.AllocNode>, <om IDMUSProdDocType.OnFileNew>, or
	<om IDMUSProdDocType.OnFileOpen> must have the same node ID that was passed to the
	<om IDMUSProdDocType.AllocNode> method responsible for creating the Node.

@parm GUID* | pguid | [out,retval] Node ID.

@rvalue S_OK | Node ID was returned in <p pguid>. 
@rvalue E_POINTER | The address in <p pguid> is not valid.  For example, it may be NULL.

@ex The following example returns a Node's ID: |

HRESULT CMotif::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_MotifNode;

	return S_OK;
}
	
@xref <om IDMUSProdDocType.AllocNode>, <om IDMUSProdDocType.OnFileNew>, <om IDMUSProdDocType.OnFileOpen>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETNODENAME
========================================================================================
@method HRESULT | IDMUSProdNode | GetNodeName | Returns text used for this <o Node>'s label. 

@parm BSTR* | pbstrName | [out,retval] Pointer to the caller-allocated variable that
	receives a copy of the name.  The caller must free <p pbstrName> with SysFreeString
	when it is no longer needed.

@rvalue S_OK | The name was returned successfully.
@rvalue E_POINTER | The address in <p pbstrName> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the name was not returned.

@ex The following example returns the name of a Node in the Project Tree: |

HRESULT CMyStyleNode::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetNodeNameMaxLength>, <om IDMUSProdNode.ValidateNodeName>, <om IDMUSProdNode.SetNodeName>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETNODENAMEMAXLENGTH
========================================================================================
@method HRESULT | IDMUSProdNode | GetNodeNameMaxLength | Returns max length of <o Node>'s label. 

@comm
	Max length does not include the NULL terminator.

@parm short* | pnMaxLength | [out,retval] Pointer to max length of Node name.  A value of
		-1 prevents the Project Tree from editing the Node's name.

@rvalue S_OK | Max length was returned in <p pnMaxLength>.  
@rvalue E_POINTER | The address in <p pnMaxLength> is not valid.  For example, it may be NULL.

@ex The following example returns the max length of a Node's label: |

#define MAX_LENGTH_MOTIF_NAME	15	// excluding NULL

HRESULT CMotif::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pnMaxLength == NULL )
	{
		return E_POINTER;
	}

	*pnMaxLength = MAX_LENGTH_MOTIF_NAME;

	return S_OK;
}

@ex Set <p pnMaxLength> to -1 when a Node cannot be renamed: |

HRESULT CMotifDocType::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a Motif List folder

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetNodeName>, <om IDMUSProdNode.ValidateNodeName>, <om IDMUSProdNode.SetNodeName>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::VALIDATENODENAME
========================================================================================
@method HRESULT | IDMUSProdNode | ValidateNodeName | Validates new text for <o Node>'s label. 

@comm
	The Framework strips leading and trailing spaces from the text before calling
	<om IDMUSProdNode.ValidateNodeName>.

	<om IDMUSProdNode.ValidateNodeName> gives a Node the opportunity to reject the new label
	that was entered through the Project Tree control.
	
@parm BSTR | bstrName | [in] New name.  This method must free <p bstrName> with SysFreeString
		when it is no longer needed.

@rvalue S_OK | New text was accepted. 
@rvalue S_FALSE | New text was rejected. 
@rvalue E_FAIL | An error occurred and the new text could not be validated. 

@ex The following example accepts any text for the Node's new label: |

HRESULT CMotif::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	::SysFreeString( bstrName );

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetNodeName>, <om IDMUSProdNode.GetNodeNameMaxLength>, <om IDMUSProdNode.SetNodeName>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::SETNODENAME
========================================================================================
@method HRESULT | IDMUSProdNode | SetNodeName | Sets text used for a <o Node>'s label. 

@comm
	<om IDMUSProdNode.GetNodeNameMaxLength> and <om IDMUSProdNode.ValidateNodeName> are called
	prior to <om IDMUSProdNode.SetNodeName>.

@parm BSTR | bstrName | [in] New name.  This method must free <p bstrName> with SysFreeString
		when it is no longer needed.

@rvalue S_OK | New name was stored successfully. 
@rvalue E_OUTOFMEMORY | Out of memory. 

@ex The following example sets the name of a Node in the Project Tree: |

HRESULT CMotif::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_strName = bstrName;
	::SysFreeString( bstrName );

	SetModifiedFlag( TRUE );

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetNodeName>, <om IDMUSProdNode.GetNodeNameMaxLength>, <om IDMUSProdNode.ValidateNodeName>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETNODELISTINFO
========================================================================================
@method HRESULT | IDMUSProdNode | GetNodeListInfo | Fills a <t DMUSProdListInfo> structure with information
	describing this Node. 

@parm DMUSProdListInfo* | pListInfo  | [in] Pointer to a DMUSProdListInfo structure.  The caller must free
	pListInfo->bstrDescriptor and pListInfo->bstrName with SysFreeString when they are no longer
	needed.

@comm
	Lists of specific types of Nodes can be built by calling <om IDMUSProdProject.GetFirstFileByDocType>
	and <om IDMUSProdProject.GetNextFileByDocType>.  After acquiring each <i IDMUSProdNode> pointer,
	<om IDMUSProdNode.GetNodeListInfo> can be called to obtain text for the combo box item.
	After a selection is made, <om IDMUSProdFramework.FindDocRootNode> can be called to obtain an
	IDMUSProdNode pointer for the selected item.

	The caller must set DMUSProdListInfo.wSize to the size of the DMUSProdListInfo structure.

@rvalue S_OK | Information was successfully placed in the DMUSProdListInfo structure.
@rvalue E_INVALIDARG | The address in <p pListInfo> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the information was not returned.

@ex The following example fills a DMUSProdListInfo structure for a Style node in the Project Tree: |

HRESULT CStyle::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strDescriptor;

	strDescriptor.Format( "%d/%d", m_wBPM, m_wBeat );

	if( !(m_wClicksPerBeat % 3) )
	{
		strDescriptor += _T("  3");
	}

    pListInfo->bstrName = m_strName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();

	return S_OK;
}
	
@xref <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETEDITORCLSID
========================================================================================
@method HRESULT | IDMUSProdNode | GetEditorClsId | Returns CLSID of <o Node>'s editor. 

@comm
	When a Project Tree node is double-clicked, the Framework calls OleCreate() with
	the CLSID returned from <om IDMUSProdNode.GetEditorClsId> to create the Node's editor
	object.

	The Node's editor object must implement the following interfaces:  <i IDMUSProdEditor>,
	<i IOleObject>, <i IViewObjectEx>, <i IOleControl>, <i IOleInPlaceObject>,
	<i IOleInPlaceActiveObject>.
	
	FYI MFC's COleControl class also implements the following interfaces:  <i IPersistStorage>,
	<i IPersistStreamInit>, <i IPersistMemory>, <i IPersistPropertyBag>, <i IDataObject>,
	<i ISpecifyPropertyPages>, <i IPerPropertyBrowsing>, <i IPropertyNotifySink> for font
	updates (not exposed via QueryInterface), <i IOleCache>, <i IProvideClassInfo2>,
	<i IQuickActivate>, and <i IPointerInactive>.  Further research may indicate that
	MFC requires implementation of one or more of these interfaces.

	At the present time, it does not seem to make sense for a DirectMusic Producer editor object
	to implement <i IPersistStorage>, <i IPersistStreamInit>, <i IPersistMemory>,
	<i IPersistPropertyBag>, <i IDataObject>, <i ISpecifyPropertyPages>, <i IPerPropertyBrowsing>,
	and <i IPropertyNotifySink> because the data and properties must exist and persist whether or
	not the editor object exists. 	A Node object represents the life of data and must implement
	these types of interfaces.
	
@parm CLSID* | pclsid | [out,retval] Pointer to CLSID of Node's editor.

@rvalue S_OK | Editor's CLSID was returned in <p pclsid>.
@rvalue E_POINTER | The address in <p pclsid> is not valid.  For example, it may be NULL.

@ex The following example returns the CLSID of a Node's editor: |

HRESULT CMotif::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pClsId == NULL )
	{
		return E_POINTER;
	}

    *pClsId = CLSID_MotifEditor;
	
	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetEditorTitle>, <om IDMUSProdNode.GetEditorWindow>, <om IDMUSProdNode.SetEditorWindow>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETEDITORTITLE
========================================================================================
@method HRESULT | IDMUSProdNode | GetEditorTitle | Returns title for <o Node>'s editor window. 

@parm BSTR* | pbstrTitle | [out,retval] Pointer to the caller-allocated variable that
	receives a copy of the title.  The caller must free <p pbstrTitle> with SysFreeString
	when it is no longer needed.

@rvalue S_OK | The title was returned successfully.
@rvalue E_POINTER | The address in <p pbstrTitle> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the title was not returned.

@ex The following example returns the window title used for a Project Tree node's editor: |

HRESULT CMyStyleNode::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	if( pbstrTitle == NULL )
	{
		return E_POINTER;
	}

	::LoadString( theApp.m_hInstance, IDS_STYLE_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );
	strTitle += m_strName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetEditorClsId>, <om IDMUSProdNode.GetEditorWindow>, <om IDMUSProdNode.SetEditorWindow>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETEDITORWINDOW
========================================================================================
@method HRESULT | IDMUSProdNode | GetEditorWindow | Returns HWND of <o Node>'s editor. 

@comm
	This method refers to the HWND that contains the editor object.
	
	The <o Framework> embeds the editor object in an MDI child window.  It uses the
	<om IDMUSProdNode.SetEditorWindow> to hand the HWND of the MDI child window to the Node
	object.
	
@parm HWND* | phwndEditor | [out,retval] Pointer to HWND of Node's editor.

@rvalue S_OK | Editor's HWND was returned in <p phwndEditor>. 
@rvalue E_POINTER | The address in <p phwndEditor> is not valid.  For example, it may be NULL.

@ex The following example returns the HWND of the Node's editor: |

HRESULT CMotif::GetEditorWindow( HWND* phWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( phWndEditor == NULL )
	{
		return E_POINTER;
	}

	*phWndEditor = m_hWndEditor;

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetEditorClsId>, <om IDMUSProdNode.GetEditorTitle>, <om IDMUSProdNode.SetEditorWindow>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::SETEDITORWINDOW
========================================================================================
@method HRESULT | IDMUSProdNode | SetEditorWindow | Sets HWND of <o Node>'s editor.  

@comm
	This method refers to the HWND that contains the editor object.
	
	The <o Framework> embeds the editor object in an MDI child window.  It uses the
	<om IDMUSProdNode.SetEditorWindow> to hand the HWND of the MDI child window to the Node
	object.  The Framework calls <om IDMUSProdNode.SetEditorWindow> to set the HWND to NULL
	when the MDI child window closes.

@parm HWND | hWndEditor | [in] HWND of Node's editor.

@rvalue S_OK | <p hWndEditor> was stored successfully. 

@ex The following example stores the HWND of the Node's editor: |

HRESULT CMotif::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetEditorClsId>, <om IDMUSProdNode.GetEditorTitle>, <om IDMUSProdNode.GetEditorWindow>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETRIGHTCLICKMENUID
========================================================================================
@method HRESULT | IDMUSProdNode | GetRightClickMenuId | Returns HINSTANCE and resource ID of
		<o Node>'s right-click context menu. 

@parm HINSTANCE* | phInstance | [out] Pointer to handle of the module containing the menu
		resource.
@parm UINT* | pnResourceId | [out] Pointer to ID of the menu resource.

@rvalue S_OK | HINSTANCE and resource ID were returned in <p phInstance> and <p pnResourceId>. 
@rvalue E_POINTER | The address in either <p phInstance> or <p pnResourceId> is not valid.
		For example, it may be NULL.

@ex The menu resource exists in the Component DLL's resource file.  For example: |

	IDM_MOTIF_NODE_RMENU MENU DISCARDABLE 
	BEGIN
		POPUP "Motif"
		BEGIN
			MENUITEM SEPARATOR
			MENUITEM "Delete",                      IDM_DELETE
			MENUITEM "Rename...",                   IDM_RENAME
			MENUITEM SEPARATOR
			MENUITEM "Properties",                  IDM_PROPERTIES
		END
	END

@ex The following example returns the HINSTANCE and resource ID for the Node's right-click
	menu: |

HRESULT CMotif::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( phInstance == NULL
	||  pnResourceId == NULL )
	{
		return E_POINTER;
	}

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_MOTIF_NODE_RMENU;

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.OnRightClickMenuInit>, <om IDMUSProdNode.OnRightClickMenuSelect>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::ONRIGHTCLICKMENUINIT
========================================================================================
@method HRESULT | IDMUSProdNode | OnRightClickMenuInit | Initializes state of menu items
		contained in <o Node>'s right-click context menu. 

@comm
	Menu items can be inserted, removed or modified.

@parm HMENU | hMenu | [in] Handle to Node's right-click context menu.

@rvalue S_OK | Menu was initialized successfully. 
@rvalue E_FAIL | Menu could not be initialized. 

@ex The following example initializes a Node's right-click menu: |

HRESULT CMotif::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CMenu menu;
	CString strMenuText;
	SHFILEINFO shfi;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	IDMUSProdNode* pIThisNode = (IDMUSProdNode *)this;
	IDMUSProdNode* pIDocRootNode;

	if( !SUCCEEDED ( GetDocRootNode(&pIDocRootNode) ) )
	{
		return E_FAIL;
	}

	if( menu.Attach(hMenu) )
	{
		TCHAR achMenuText[MID_BUFFER];

		if( pIThisNode == pIDocRootNode )
		{
			::LoadString( theApp.m_hInstance, IDS_REVERT_TEXT, achMenuText, MID_BUFFER );
			menu.InsertMenu( 0, MF_BYPOSITION | MF_STRING, IDM_REVERT, achMenuText );
			::LoadString( theApp.m_hInstance, IDS_SAVE_AS_TEXT, achMenuText, MID_BUFFER );
			menu.InsertMenu( 0, MF_BYPOSITION | MF_STRING, IDM_SAVEAS, achMenuText );
			::LoadString( theApp.m_hInstance, IDS_SAVE_TEXT, achMenuText, MID_BUFFER );
			menu.InsertMenu( 0, MF_BYPOSITION | MF_STRING, IDM_SAVE, achMenuText );

			CString strFileName;
			BSTR bstrFileName;

			if( SUCCEEDED ( m_pComponent->m_pIFramework->GetNodeFileName((IDMUSProdNode *)this, &bstrFileName) ) )
			{
				strFileName = bstrFileName;
				::SysFreeString( bstrFileName );
			}
			else
			{
				strFileName.Empty();
			}

			if( strFileName.IsEmpty() )
			{
				menu.EnableMenuItem( IDM_SAVE, (MF_GRAYED | MF_BYCOMMAND) );
				menu.EnableMenuItem( IDM_REVERT, (MF_GRAYED | MF_BYCOMMAND) );
			}
			else
			{

				SHGetFileInfo( strFileName, 0,
							   (SHFILEINFO *)&shfi, sizeof(SHFILEINFO),
							   SHGFI_DISPLAYNAME );
				menu.GetMenuString( IDM_SAVE, strMenuText, MF_BYCOMMAND );
				strMenuText += _T(" ");
				strMenuText += shfi.szDisplayName;
				menu.ModifyMenu( IDM_SAVE, (MF_STRING | MF_BYCOMMAND), IDM_SAVE, strMenuText );

				menu.EnableMenuItem( IDM_SAVE, (MF_ENABLED | MF_BYCOMMAND) );
				menu.EnableMenuItem( IDM_REVERT, (MF_ENABLED | MF_BYCOMMAND) );
			}
		}
		else
		{
			::LoadString( theApp.m_hInstance, IDS_CREATE_MOTIF_FILE, achMenuText, MID_BUFFER );
			menu.InsertMenu( 0, MF_BYPOSITION | MF_STRING, IDM_CREATE_FILE, achMenuText );
		}

		menu.Detach();
	}

	pIDocRootNode->Release();

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetRightClickMenuId>, <om IDMUSProdNode.OnRightClickMenuSelect>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::ONRIGHTCLICKMENUSELECT
========================================================================================
@method HRESULT | IDMUSProdNode | OnRightClickMenuSelect | Performs actions associated with
		<o Node>'s right-click context menu. 

@parm long | lCommandId | [in] Command ID of the selected menu item.

@rvalue S_OK | Action associated with <p lCommandId> was completed successfully. 
@rvalue E_FAIL | Action associated with <p lCommandId> failed. 

@ex The following example handles menu selections for a Node's right-click menu: |

HRESULT CMotif::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	IDMUSProdNode* pIDocRootNode;
	IDMUSProdNode* pIParentNode;

	switch( lCommandId )
	{
		case IDM_SAVE:
			if( SUCCEEDED ( GetDocRootNode(&pIDocRootNode) ) )
			{
				if( SUCCEEDED ( m_pComponent->m_pIFramework->SaveNode(pIDocRootNode) ) )
				{
					hr = S_OK;
				}
				pIDocRootNode->Release();
			}
			break;

		case IDM_SAVEAS:
			if( SUCCEEDED ( GetDocRootNode(&pIDocRootNode) ) )
			{
				if( SUCCEEDED ( m_pComponent->m_pIFramework->SaveNodeAsNewFile(pIDocRootNode) ) )
				{
					hr = S_OK;
				}
				pIDocRootNode->Release();
			}
			break;

		case IDM_REVERT:
			...
			...
			...
			hr = S_OK;
			break;

		case IDM_CREATE_FILE:
			...
			...
			...
			hr = S_OK;
			break;

		case IDM_RENAME:
			if( SUCCEEDED ( m_pComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this) ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_DELETE:
			if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
			{
				pIParentNode->DeleteChildNode( (IDMUSProdNode *)this, TRUE );
				pIParentNode->Release();
				hr = S_OK;
			}
			break;
	}

	return hr;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.GetRightClickMenuId>, <om IDMUSProdNode.OnRightClickMenuInit>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::DELETECHILDNODE
========================================================================================
@method HRESULT | IDMUSProdNode | DeleteChildNode | Instructs parent <o Node> to delete the child 
		Node specified in <p pIChildNode>.

@parm IDMUSProdNode* | pIChildNode | [in] Pointer to the <i IDMUSProdNode> interface to be
		deleted.
@parm BOOL fPromptUser | fPromptUser | [in] Determines whether delete prompt is displayed.

@rvalue S_OK | <p pIChildNode> was deleted successfully. 
@rvalue E_INVALIDARG | <p pIChildNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and <p pIChildNode> could not be deleted. 
@rvalue E_NOTIMPL | This Node does not support children. 

@ex The following example removes a child Node: |

HRESULT CMotifDocType::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Remove from Project Tree
	m_pComponent->m_pIFramework->RemoveNode( pIChildNode );

	// Remove from Component Motif list
	POSITION pos = m_pComponent->m_lstMotifs.Find( (CMotif *)pIChildNode );
	if( pos )
	{
		m_pComponent->m_lstMotifs.RemoveAt( pos );
		pIChildNode->Release();
	}

	m_pComponent->m_pIFramework->SetModifiedFlag( (IDMUSProdNode *)this, TRUE );

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.InsertChildNode>, <om IDMUSProdNode.DeleteNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::INSERTCHILDNODE
========================================================================================
@method HRESULT | IDMUSProdNode | InsertChildNode | Instructs parent <o Node> to insert the child 
		Node specified in <p pIChildNode>. 

@comm
	The implementation may choose to accept a <p pIChildNode> value of NULL.  If accepted,
	a value of NULL signifies that a new Node is to be allocated, initialized and
	inserted.

@parm IDMUSProdNode* | pIChildNode | [in] Pointer to the <i IDMUSProdNode> interface to be
		inserted.

@rvalue S_OK | <p pIChildNode> was inserted successfully.  
@rvalue E_INVALIDARG | <p pIChildNode> is not valid.  For example, it may be NULL.  An
		implementation may consider a <p pIChildNode> value of NULL valid.  See comments.
@rvalue E_FAIL | <p pIChildNode> could not be inserted. 
@rvalue E_NOTIMPL | This Node does not support children. 

@ex The following example inserts a child Node: |

HRESULT CMotifDocType::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Add Motif to Motif component list 
	pIChildNode->AddRef();
	m_pComponent->m_lstMotifs.AddTail( (CMotif *)pIChildNode );

	// Set DocRoot and parent Node of ALL children
	theApp.SetNodePointers( pIChildNode, pIChildNode, (IDMUSProdNode *)this );

	// Add Node to Project Tree
	if( !SUCCEEDED ( m_pComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	m_pComponent->m_pIFramework->SetModifiedFlag( (IDMUSProdNode *)this, TRUE );

	return S_OK;
}

@ex The following implementation accepts a <p pIChildNode> value of NULL: |

HRESULT CStyleMotifs::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdDocType* pIDocType;

	ASSERT( m_pStyleComponent != NULL );
	ASSERT( m_pStyleComponent->m_pIFramework != NULL );
	ASSERT( m_pStyle != NULL );

	if( pIChildNode == NULL )
	{
		HRESULT hr;

		hr = m_pStyleComponent->m_pIFramework->FindDocTypeByNodeId( GUID_MotifNode, &pIDocType );
		if( SUCCEEDED ( hr ) ) 
		{
			// Create a new Motif 
			hr = pIDocType->AllocNode( GUID_MotifNode, &pIChildNode );
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

	// add to CStyle Motif list
	m_lstMotifs.AddTail( pIChildNode );

	// Set DocRoot and parent Node of ALL children
	theApp.SetNodePointers( pIChildNode, (IDMUSProdNode *)m_pStyle, (IDMUSProdNode *)this );

	// Add Node to Project Tree
	if( !SUCCEEDED ( m_pStyleComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	m_pStyleComponent->m_pIFramework->SetModifiedFlag( (IDMUSProdNode *)this, TRUE );

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.DeleteChildNode>, <om IDMUSProdNode.DeleteNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::DELETENODE
========================================================================================
@method HRESULT | IDMUSProdNode | DeleteNode | Deletes this <o Node>.

@comm
	<om IDMUSProdNode.DeleteNode> also deletes the children belonging to this Node.

@parm BOOL fPromptUser | fPromptUser | [in] Determines whether delete prompt is displayed.

@rvalue S_OK | Node was deleted successfully. 
@rvalue E_FAIL | An error occurred and this Node could not be deleted. 
@rvalue E_NOTIMPL | Node does not support deletion.  For example, a Style's Band folder cannot
		be deleted. 

@ex The following example deletes a Node: |

HRESULT CMotif::DeleteNode( void, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIParentNode;

	if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			pIParentNode->DeleteChildNode( this, fPromptUser );
			pIParentNode->Release();
			return S_OK;
		}
	}

	return E_FAIL;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.InsertChildNode>, <om IDMUSProdNode.DeleteChildNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::ONNODESELCHANGED
========================================================================================
@method HRESULT | IDMUSProdNode | OnNodeSelChanged | Informs <o Node> of changes to its selection
		state.

@parm BOOL | fSelected | [in] Specifies whether the Node is being selected or deselected.
		TRUE means the Node is being selected.  FALSE means the Node is being deselected.

@rvalue S_OK | Always successful. 

@ex The following example changes the state of the 'Transport Controls' toolbar: |

HRESULT CDirectMusicPattern::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( fSelected )
	{
		DWORD dwButtonState = BS_AUTO_UPDATE;

		if( m_pPatternCtrl != NULL
		&&  m_pPatternCtrl->m_pPatternDlg != NULL
		&&	m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr != NULL )
		{
			dwButtonState |= BS_RECORD_ENABLED;
			if( m_fRecordPressed )
			{
				dwButtonState |= BS_RECORD_CHECKED;
			}
		}

		theApp.m_pStyleComponent->m_pIConductor->SetActiveTransport( this, dwButtonState );	
		theApp.m_pStyleComponent->m_pIConductor->SetTempo( this, m_pStyle->m_dblTempo, TRUE );
	}

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <i IDMUSProdConductor>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::CREATEDATAOBJECT
========================================================================================
@method HRESULT | IDMUSProdNode | CreateDataObject | Returns a pointer to an IDataObject
	interface containing the current contents of this <o Node>.

@parm IDataObject** | ppIDataObject | [out,retval] Address of a variable to receive the
		requested IDataObject interface.  On success, the caller is responsible for calling
		IUnknown::Release.

@rvalue S_OK | The data object was created and <p ppIDataObject> was returned successfully.  
@rvalue E_POINTER | <p ppIDataObject> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | Out of memory. 
@rvalue E_NOTIMPL | This Node does not support drag/drop, cut, copy, or paste. 
@rvalue E_FAIL | An error occurred, and the data object could not be created.

@ex The following example creates an IDataObject for this Node: |

HRESULT CBand::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject
	// CDllJazzDataObject implements IDataObject and provides a few convenience functions
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Band into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_BAND into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfBand, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		pIStream->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( theApp.m_pIFramework->SaveClipFormat( m_pComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// There is the potential for a Band node to become a file
			// so we must also place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			pIStream->Release();
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CanCut>, <om IDMUSProdNode.CanCopy>, <om IDMUSProdNode.CanDelete>, <om IDMUSProdNode.CanDeleteChildNode>,
	<om IDMUSProdNode.CanPasteFromData>, <om IDMUSProdNode.PasteFromData>, <om IDMUSProdNode.CanChildPasteFromData>, <om IDMUSProdNode.ChildPasteFromData>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::CANCUT
========================================================================================
@method HRESULT | IDMUSProdNode | CanCut | Determines whether this <o Node> can be cut.

@rvalue S_OK | This Node can be cut.  
@rvalue S_FALSE | This Node cannot be cut.  
@rvalue E_FAIL | An error occurred, and this Node cannot be cut.

@ex The following example returns whether or not this Node can be cut: |

HRESULT CBand::CanCut( void )
{
	return CanDelete();
}

HRESULT CBand::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		// Let parent decide whether or not this Band can be deleted.
		// Styles, for example, do not allow their last Band to be deleted.
		if( m_pIParentNode )
		{
			return m_pIParentNode->CanDeleteChildNode( this );
		}
		else
		{
			return S_FALSE;
		}
	}

	// Band files can always be copied
	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CreateDataObject>, <om IDMUSProdNode.CanCopy>, <om IDMUSProdNode.CanDelete>, <om IDMUSProdNode.CanDeleteChildNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::CANCOPY
========================================================================================
@method HRESULT | IDMUSProdNode | CanCopy | Determines whether this <o Node> can be copied.

@rvalue S_OK | This Node can be copied.  
@rvalue S_FALSE | This Node cannot be copied.  
@rvalue E_FAIL | An error occurred, and this Node cannot be copied.

@ex The following example returns whether or not this Node can be copied: |

HRESULT CBand::CanCopy( void )
{
	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CreateDataObject>, <om IDMUSProdNode.CanCut>, <om IDMUSProdNode.CanDelete>, <om IDMUSProdNode.CanDeleteChildNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::CANDELETE
========================================================================================
@method HRESULT | IDMUSProdNode | CanDelete | Determines whether this <o Node> can be deleted.

@rvalue S_OK | This Node can be deleted.  
@rvalue S_FALSE | This Node cannot be deleted.  
@rvalue E_FAIL | An error occurred, and this Node cannot be deleted.

@ex The following example returns whether or not this Node can be deleted: |

HRESULT CBand::CanDelete( void )
{
	return S_OK;
}

@ex The following example defers the choice of whether or not this Node can be deleted to
	its parent Node: |

HRESULT CBand::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		// Let parent decide whether or not this Band can be deleted.
		// Styles, for example, do not allow their last Band to be deleted.
		if( m_pIParentNode )
		{
			return m_pIParentNode->CanDeleteChildNode( this );
		}
		else
		{
			return S_FALSE;
		}
	}

	// Band files can always be copied
	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CreateDataObject>, <om IDMUSProdNode.CanCut>, <om IDMUSProdNode.CanCopy>, <om IDMUSProdNode.CanDeleteChildNode>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::CANDELETECHILDNODE
========================================================================================
@method HRESULT | IDMUSProdNode | CanDeleteChildNode | Determines whether <p pIChildNode>
	can be deleted.

@parm IDMUSProdNode* | pIChildNode | [in] A pointer to a child <o Node> in this Node's list
		of children.

@rvalue S_OK | <p pIChildNode> can be deleted.  
@rvalue S_FALSE | <p pIChildNode> cannot be deleted.  
@rvalue E_INVALIDARG | <p pIChildNode> is not valid.  For example, it may be NULL.
@rvalue E_NOTIMPL | This Node does not support children. 
@rvalue E_FAIL | An error occurred, and <p pIChildNode> cannot be deleted.

@ex The following example returns whether or not <p pIChildNode> can be deleted: |

HRESULT CStyleBands::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure node is in Band list
	POSITION pos = m_lstBands.Find( pIChildNode );
	if( pos == NULL )
	{
		return E_FAIL;
	}

	// Cannot delete the last Band from a Style
	if( m_lstBands.GetCount() < 2 )
	{
		return S_FALSE;
	}

	return S_OK;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CreateDataObject>, <om IDMUSProdNode.CanCut>, <om IDMUSProdNode.CanCopy>, <om IDMUSProdNode.CanDelete>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::CANPASTEFROMDATA
========================================================================================
@method HRESULT | IDMUSProdNode | CanPasteFromData | Determines whether <p pIDataObject>
	contains data that can be pasted into this <o Node>.

@parm IDataObject* | pIDataObject | [in] A pointer to an IDataObject interface.
@parm BOOL* | pfWillSetReference | [out] Communicates whether paste will create a file
	reference instead of actually copying the data.

@rvalue S_OK | A paste operation can be performed using <p pIDataObject>.  
@rvalue S_FALSE | A paste operation cannot be performed using <p pIDataObject>.  
@rvalue E_INVALIDARG | <p pIDataObject> is not valid.  For example, it may be NULL.
@rvalue E_POINTER | <p pfWillSetReference> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and a paste operation cannot be performed using
		<p pIDataObject>. 

@ex The following example returns whether or not <p pIDataObject> contains data that can
	be pasted into this Node: |

HRESULT CStyleBands::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	// CDllJazzDataObject implements IDataObject and provides a few convenience functions.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) 
	||  SUCCEEDED ( m_pStyle->ContainsClipFormat( pDataObject, pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) ) 
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}

@ex The following example defers the choice of whether or not <p pIDataObject> contains
	data that can be pasted to its parent Node: |

HRESULT CBand::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			// Let Style, for example, decide what can be dropped
			return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
		}
	}

	// Can't drop anything on a Band file in the Project Tree
	return S_FALSE;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CreateDataObject>, <om IDMUSProdNode.PasteFromData>, <om IDMUSProdNode.CanChildPasteFromData>, <om IDMUSProdNode.ChildPasteFromData>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::PASTEFROMDATA
========================================================================================
@method HRESULT | IDMUSProdNode | PasteFromData | Inserts data from <p pIDataObject>
	into this <o Node>.

@parm IDataObject* | pIDataObject | [in] A pointer to an IDataObject interface.

@rvalue S_OK | The paste operation completed successfully.
@rvalue E_INVALIDARG | <p pIDataObject> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | Out of memory. 
@rvalue E_FAIL | An error occurred, and the paste operation did not take place.

@ex The following example performs a paste operation: |

HRESULT CStyleBands::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Do not continue unless CStyleBands can accept data
	BOOL fWillSetReference;
	if( CanPasteFromData( pIDataObject, &fWillSetReference ) != S_OK )
	{
		return E_FAIL;
	}

	// Let CDirectMusicStyle do the work
	return m_pStyle->PasteFromData( pIDataObject );
}

HRESULT CDirectMusicStyle::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	// CDllJazzDataObject implements IDataObject and provides a few convenience functions.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) ) )
	{
		// Handle CF_BAND format
		hr = PasteCF_BAND( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) )
	{
		// Handle CF_BANDLIST format
		hr = PasteCF_BANDLIST( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
	{
		// Handle CF_BANDTRACK format
		hr = PasteCF_BANDTRACK( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline ) ) )
	{
		// Handle CF_TIMELINE format
		hr = PasteCF_TIMELINE( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) ) )
	{
		// Handle CF_MOTIF format
		hr = PasteCF_MOTIF( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) )
	{
		// Handle CF_MOTIFLIST format
		hr = PasteCF_MOTIFLIST( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) ) )
	{
		// Handle CF_PATTERN format
		hr = PasteCF_PATTERN( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) )
	{
		// Handle CF_PATTERNLIST format
		hr = PasteCF_PATTERNLIST( pDataObject, pIDataObject, NULL );
	}

	RELEASE( pDataObject );
	return hr;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CreateDataObject>, <om IDMUSProdNode.CanPasteFromData>, <om IDMUSProdNode.CanChildPasteFromData>, <om IDMUSProdNode.ChildPasteFromData>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::CANCHILDPASTEFROMDATA
========================================================================================
@method HRESULT | IDMUSProdNode | CanChildPasteFromData | Determines whether the
	<p pIDataObject> dropped or pasted onto <p pIChildNode> contains data that can be
	pasted into this <o Node>.

@parm IDataObject* | pIDataObject | [in] A pointer to an IDataObject interface.
@parm IDMUSProdNode* | pIChildNode | [in] A pointer to a child <o Node> in this Node's list
		of children.
@parm BOOL* | pfWillSetReference | [out] Communicates whether paste will create a file
	reference instead of actually copying the data.

@rvalue S_OK | A paste operation can be performed using <p pIDataObject>.  
@rvalue S_FALSE | A paste operation cannot be performed using <p pIDataObject>.  
@rvalue E_INVALIDARG | Either <p pIDataObject> or <p pIChildNode> is not valid.
		For example, they may be NULL.
@rvalue E_POINTER | <p pfWillSetReference> is not valid.  For example, it may be NULL.
@rvalue E_NOTIMPL | This Node does not support children. 
@rvalue E_FAIL | An error occurred, and a paste operation cannot be performed using
		<p pIDataObject>. 

@ex The following example returns whether or not <p pIDataObject> contains data that can
	be pasted into this Node: |

HRESULT CStyleBands::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										    BOOL *pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	// CDllJazzDataObject implements IDataObject and provides a few convenience functions.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) 
	||  SUCCEEDED ( m_pStyle->ContainsClipFormat( pDataObject, pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CreateDataObject>, <om IDMUSProdNode.ChildPasteFromData>, <om IDMUSProdNode.CanPasteFromData>, <om IDMUSProdNode.PasteFromData>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::CHILDPASTEFROMDATA
========================================================================================
@method HRESULT | IDMUSProdNode | ChildPasteFromData | Inserts data from the
	<p pIDataObject> dropped or pasted onto <p pIChildNode> into this <o Node>.

@parm IDataObject* | pIDataObject | [in] A pointer to an IDataObject interface.
@parm IDMUSProdNode* | pIChildNode | [in] A pointer to a child <o Node> in this Node's list
		of children.

@rvalue S_OK | The paste operation completed successfully.
@rvalue E_INVALIDARG | Either <p pIDataObject> or <p pIChildNode> is not valid.
		For example, they may be NULL.
@rvalue E_OUTOFMEMORY | Out of memory. 
@rvalue E_NOTIMPL | This Node does not support children. 
@rvalue E_FAIL | An error occurred, and the paste operation did not take place.

@ex The following example performs a paste operation: |

HRESULT CStyleBands::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	if( pIDataObject == NULL
	||  pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	// CDllJazzDataObject implements IDataObject and provides a few convenience functions.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) ) )
	{
		// Handle CF_BAND format
		hr = m_pStyle->PasteCF_BAND( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) )
	{
		// Handle CF_BANDLIST format
		hr = m_pStyle->PasteCF_BANDLIST( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
	{
		// Handle CF_BANDTRACK format
		hr = m_pStyle->PasteCF_BANDTRACK( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline ) ) )
	{
		// Handle CF_TIMELINE format
		hr = m_pStyle->PasteCF_TIMELINE ( pDataObject, pIDataObject, NULL );
	}

	RELEASE( pDataObject );
	return hr;
}
	
@xref <i IDMUSProdNode>, <om IDMUSProdNode.CreateDataObject>, <om IDMUSProdNode.CanChildPasteFromData>, <om IDMUSProdNode.CanPasteFromData>, <om IDMUSProdNode.PasteFromData>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODE::GETOBJECT
========================================================================================
@method HRESULT | IDMUSProdNode | GetObject | Retrieves a pointer to the requested interface
	for an object of type <p rclsid>.

@parm REFCLSID | rclsid | [in] CLSID of the requested object.
@parm REFIID | riid | [in] Identifier of the requested interface.
@parm void** | ppvObject | [out,retval] Address of a variable to receive a pointer to the
		requested object.  On success, the caller is responsible for calling IUnknown::Release.

@rvalue S_OK | The interface specified in <p riid> was returned successfully.
@rvalue E_POINTER | The address in <p ppvObject> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the interface specified in <p riid> could not be returned.

@ex The following example returns interfaces to a DirectMusic Style object: |

HRESULT CStyle::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DMStyle object
	if( ::IsEqualCLSID( rclsid, CLSID_DMStyle ) )
	{
		if( m_pIDMStyle )
		{
			return m_pIDMStyle->QueryInterface( riid, ppvObject );
		}
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}
	
@xref <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/
