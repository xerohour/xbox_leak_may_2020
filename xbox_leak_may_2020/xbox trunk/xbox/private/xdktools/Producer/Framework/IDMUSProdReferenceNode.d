
--------------------------------------------------------
IDMUSProdReferenceNode AutoDocs for DirectMusic Producer
--------------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODREFERENCENODE
========================================================================================
@interface IDMUSProdReferenceNode | 
	This interface contains methods used to link Reference <o Node>s to actual files.

@comm
	Reference nodes allow DirectMusic Producer files to bind themselves to other Producer
	files.
	
	Reference nodes must implement the <i IDMUSProdReferenceNode> interface.  A pointer to a
	Node's IDMUSProdReferenceNode interface can be obtained via <om IDMUSProdNode::QueryInterface>.

@base public | IUnknown

@xref <o Node> Object, <i IDMUSProdNode>, <i IDMUSProdFileRefChunk>

@meth HRESULT | GetReferencedFile | Returns a pointer to the DocRoot Node of the referenced
	file.
@meth HRESULT | SetReferencedFile | Links a reference Node to a file.
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODREFERENCENODE::GETREFERENCEDFILE
========================================================================================
@method HRESULT | IDMUSProdReferenceNode | GetReferencedFile | Returns a pointer to the DocRoot
		<o Node> of the referenced file.

@comm
	Reference nodes allow DirectMusic Producer files to bind themselves to other Producer
	files.
	
	Reference nodes must implement the <i IDMUSProdReferenceNode> interface.  A pointer to a
	Node's IDMUSProdReferenceNode interface can be obtained via <om IDMUSProdNode::QueryInterface>.

	The implementation should return E_FAIL if the Reference Node is not linked to a DocRoot
	Node.
 
@parm IDMUSProdNode** | ppIDocRootNode | [out,retval] Address of a variable to receive the requested
		<i IDMUSProdNode> interface.  On success, the caller is responsible for calling <om IDMUSProdNode.Release>
		when this pointer is no longer needed.

@rvalue S_OK | The DocRoot's <i IDMUSProdNode> interface was returned in <p ppIDocRootNode>.
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred, and the DocRoot Node could not be returned.

@ex The following example returns the DocRoot Node of the referenced file: |

HRESULT CStyleRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pStyle )
	{
		m_pStyle->AddRef();
		*ppIDocRootNode = m_pStyle;
		return S_OK;
	}

    return E_FAIL;
}
	
@xref <o Node> Object, <i IDMUSProdReferenceNode>, <i IDMUSProdNode>, <i IDMUSProdFileRefChunk>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODREFERENCENODE::SETREFERENCEDFILE
========================================================================================
@method HRESULT | IDMUSProdReferenceNode | SetReferencedFile | Links a file to reference <o Node>.

@comm
	Reference nodes allow DirectMusic Producer files to bind themselves to other Producer
	files.
	
	Reference nodes must implement the <i IDMUSProdReferenceNode> interface.  A pointer to a
	Node's IDMUSProdReferenceNode interface can be obtained via <om IDMUSProdNode::QueryInterface>.

	Use <om IDMUSProdComponent.AllocReferenceNode> to create the reference Node.

	In the case where <p pIDocRootNode> is NULL, the previous link is removed and the reference
	Node no longer points to a file.
 
@parm IDMUSProdNode* | pIDocRootNode | [in] Pointer to an <i IDMUSProdNode> interface.  

@rvalue S_OK | The reference Node link was linked to <p pIDocRootNode> successfully.
@rvalue E_INVALIDARG | <p pIDocRootNode> is not valid.  For example, it may not be a DocRoot node.
@rvalue E_FAIL | An error occurred, and the link could not be established.

@ex The following example links a file to a reference Node: |

HRESULT CStyleRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDocRootNode )
	{
		// Make sure method was passed a DocRootNode
		IDMUSProdNode* pINode;
		if( FAILED ( pIDocRootNode->GetDocRootNode ( &pINode ) ) )
		{
			pINode = NULL;
		}
		if( pIDocRootNode != pINode )
		{
			if( pINode )
			{
				pINode->Release();
			}
			return E_INVALIDARG;
		}
		pINode->Release();

		// Make sure method was passed a Style Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_StyleNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	if( m_pStyle )
	{
		m_pStyle->Release();
		m_pStyle = NULL;
	}

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pStyle = (CStyle *)pIDocRootNode;
		m_pStyle->AddRef();
	}

	return S_OK;
}
	
@xref <o Node> Object, <om IDMUSProdComponent.AllocReferenceNode>, <i IDMUSProdNode>, <i IDMUSProdFileRefChunk>, <i IDMUSProdReferenceNode>
--------------------------------------------------------------------------------------*/
