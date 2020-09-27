
-------------------------------------------------
IDMUSProdEditor AutoDocs for DirectMusic Producer
-------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODEDITOR
========================================================================================
@interface IDMUSProdEditor | 
	This interface defines interaction between the <o Framework> and the <o Editor> that opens
	when a node in the Project Tree is double-clicked.

@comm
	DirectMusic Producer Editors are standard registered COM objects.  In addition to
	<i IDMUSProdEditor>, these objects must implement <i IOleObject>, <i IViewObjectEx>,
	<i IOleControl>, <i IOleInPlaceObject>, and <i IOleInPlaceActiveObject>. 	

@base public | IUnknown

@xref <i IDMUSProdNode>, <om IDMUSProdFramework.OpenEditor>, <om IDMUSProdFramework.CloseEditor>

@meth HRESULT | AttachObjects | Hands corresponding IDMUSProdNode interface pointer to
		the Editor.
@meth HRESULT | OnInitMenuFilePrint | Initializes state of Framework's File Print menu
		item.
@meth HRESULT | OnInitMenuFilePrintPreview | Initializes state of Framework's File
		Print Preview menu item.
@meth HRESULT | OnFilePrint | Performs action associated with Framework's File Print
		menu item.
@meth HRESULT | OnFilePrintPreview | Performs action associated with Framework's File
		Print Preview menu item.
@meth HRESULT | OnViewProperties | Performs action associated with Framework's View 
		Properties menu item.
@meth HRESULT | OnF1Help | Performs context help action associated with F1.
--------------------------------------------------------------------------------------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER EDITOR OBJECT
========================================================================================
@object Editor | Represents a <o Node>'s Editor.  Normally, the node will appear in the
		Project Tree but this is not necessary.  A Band in a Segment's Band track is
		an example of a node that does not display in the Project Tree.  Editors are
		opened one of two ways:  double-clicking its corresponding node in the Project 
		Tree, or via the <om IDMUSProdFramework.OpenEditor> method.

@supint IDMUSProdEditor | Defines interaction with <o Framework>.  Implementation is required.
@supint IOleObject | Implementation is required.
@supint IViewObjectEx | Implementation is required.
@supint IOleControl | Implementation is required.
@supint IOleInPlaceObject | Implementation is required.
@supint IOleInPlaceActiveObject | Implementation is required.

@comm
	DirectMusic Producer Editors are standard registered COM objects.  These objects must
	implement the <i IDMUSProdEditor>, <i IOleObject>, <i IViewObjectEx>, <i IOleControl>,
	<i IOleInPlaceObject> and <i IOleInPlaceActiveObject> interfaces.

	The following methods are called when opening an editor for a node:
	 
		1. <om IDMUSProdNode::GetEditorClsId> - returns the CLSID of the node's editor

		2. <i OleCreate> - creates an embedded Editor object

		3. <om IDMUSProdEditor::AttachObjects> - hands IDMUSProdNode to the Editor object

		4. <om IOleObject::DoVerb> - displays Editor object (OLEIVERB_SHOW)

	The DirectMusic Producer Editor object is destroyed when its parent window closes.

@xref	<i IDMUSProdNode>, <om IDMUSProdFramework.OpenEditor>, <om IDMUSProdFramework.CloseEditor>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODEDITOR::ATTACHOBJECTS
========================================================================================
@method HRESULT | IDMUSProdEditor | AttachObjects | Hands corresponding <i IDMUSProdNode>
		interface pointer to the <o Editor>.
 
@comm
	The Framework calls AttachObjects immediately after contructing the Editor OLE Control
	so that the pointers are avaliable for the Editor's WM_CREATE handler.

@parm IDMUSProdNode* | pINode | [in] Pointer to Editor's corresponding <i IDMUSProdNode> interface.

@rvalue S_OK | Pointer was stored successfully.
@rvalue E_INVALIDARG | <p pINode> is not valid.  For example, the address may be NULL.

@ex The following example stores pointers supplied by AttachObjects: |

HRESULT CMyEditor::AttachObjects( IDMUSProdNode* pINode )
{
	if( pINode == NULL )
	{
		return E_INVALIDARG;
	} 

	m_pStyle = (CStyle *)pINode;
	m_pStyle->AddRef();

	return S_OK;
}

@ex The following code excerpt shows the Framework calling AttachObjects: |

	...
	...
	...

	pCntrItem = new CComponentCntrItem( this );
	ASSERT_VALID( pCntrItem );

	if( !pCntrItem->CreateNewItem( clsidEditor ) )
	{
		AfxThrowMemoryException();		// any exception will do
	}
	
	// Attach object to the control 
	ASSERT( pCntrItem->m_lpObject != NULL );
	if( SUCCEEDED ( pCntrItem->m_lpObject->QueryInterface( IID_IDMUSProdEditor, (void**)&pIEditor ) ) )
	{
		pView->m_pIEditor = pIEditor;
		pIEditor->AttachObjects( pINode, NULL );
	}
	else
	{
		AfxThrowMemoryException();		// any exception will do
	}

	pCntrItem->DoVerb( OLEIVERB_SHOW, static_cast<CComponentView*>( pView ) );

	...
	...
	...
	
@xref <i IDMUSProdEditor>, <om IDMUSProdNode.GetEditorClsId>, <om IDMUSProdFramework.OpenEditor>, <om IDMUSProdFramework.CloseEditor>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODEDITOR::ONINITMENUFILEPRINT
========================================================================================
@method HRESULT | IDMUSProdEditor | OnInitMenuFilePrint | Initializes state of Framework's File
		Print menu item.
 
@parm HMENU | hMenu | [in] Handle to Framework's File menu.
@parm UINT | nMenuId | [in] Command ID of Framework's File Print menu item.

@rvalue S_OK | Always succeeds.  

@ex The following example enables the Framework's File Print menu item: |

HRESULT CMyEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	::EnableMenuItem( hMenu, nMenuId, (MF_ENABLED | MF_BYCOMMAND) );

	return S_OK;
}
	
@xref <i IDMUSProdEditor>, <om IDMUSProdEditor.OnFilePrint>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODEDITOR::ONINITMENUFILEPRINTPREVIEW
========================================================================================
@method HRESULT | IDMUSProdEditor | OnInitMenuFilePrintPreview | Initializes state of Framework's
		File Print Preview menu item.
 
@parm HMENU | hMenu | [in] Handle to Framework's File menu.
@parm UINT | nMenuId | [in] Command ID of Framework's File Print Preview menu item.

@rvalue S_OK | Always succeeds. 

@ex The following example disables the Framework's File Print Preview menu item: |

HRESULT CMyEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}
	
@xref <i IDMUSProdEditor>, <om IDMUSProdEditor.OnFilePrintPreview>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODEDITOR::ONFILEPRINT
========================================================================================
@method HRESULT | IDMUSProdEditor | OnFilePrint | Performs action associated with Framework's
		File Print menu item. 

@rvalue S_OK | Print was successful.
@rvalue E_FAIL | Print encountered a problem.
@rvalue E_NOTIMPL | This <o Editor> does not support Print.
	
@xref <i IDMUSProdEditor>, <om IDMUSProdEditor.OnInitMenuFilePrint>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODEDITOR::ONFILEPRINTPREVIEW
========================================================================================
@method HRESULT | IDMUSProdEditor | OnFilePrintPreview | Performs action associated with
		Framework's File Print Preview menu item. 

@rvalue S_OK | Print Preview was successful.
@rvalue E_FAIL | Print Preview encountered a problem.
@rvalue E_NOTIMPL | This <o Editor> does not support Print Preview.

@xref <i IDMUSProdEditor>, <om IDMUSProdEditor.OnInitMenuFilePrintPreview>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODEDITOR::ONVIEWPROPERTIES
========================================================================================
@method HRESULT | IDMUSProdEditor | OnViewProperties | Performs action associated with
		Framework's View Properties menu item. 
 
@comm
	Called when an Editor is active and the user selects the Framework's View Properties
	menu item.
	
	Properties should be displayed for the control which has focus. 
	
@rvalue S_OK | Display of properties was successful.
@rvalue E_FAIL | An error occurred and properties could not be displayed.
@rvalue E_NOTIMPL | This <o Editor> does not support properties.
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdEditor>, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODEDITOR::ONF1HELP
========================================================================================
@method HRESULT | IDMUSProdEditor | OnF1Help | Performs the context help action associated
		with F1.
 
@rvalue S_OK | Context help was displayed successfully.
@rvalue E_FAIL | An error occurred and context help could not be displayed.
@rvalue E_NOTIMPL | This <o Editor> does not support context help.
	
@xref <i IDMUSProdEditor>
--------------------------------------------------------------------------------------*/
