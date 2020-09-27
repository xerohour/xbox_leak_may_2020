
---------------------------------------------------
IDMUSProdSortNode AutoDocs for DirectMusic Producer
---------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODSORTNODE
========================================================================================
@interface IDMUSProdSortNode | 
	The <i IDMUSProdSortNode> interface attaches custom sorting algorithms to <o Node>
	objects displayed in the <o Framework>'s Project Tree.

@base public | IUnknown

@xref <o Node> Object, <o Framework> Object

@meth HRESULT | CompareNodes | Provides the means to override alphabetical sorting of nodes
	in the Project Tree.
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODMENU::COMPARENODES
========================================================================================
@method HRESULT | IDMUSProdSortNode | CompareNodes | Provides the means to override alphabetical
		sorting of <o Node>s in the Project Tree.
 
@comm
	<i IDMUSProdSort> should be implemented by parent nodes that do not want their immediate
	child nodes to be sorted alphabetically.  

@parm IDMUSProdNode* | pINode1 | [in] Pointer to the first child node to be sorted.
@parm IDMUSProdNode* | pINode2 | [in] Pointer to the second child node to be sorted.
@parm int* | pnResult | [out,retval] Set to a negative value if <p pINode1> should precede
		<p pINode2>, a positive value if <p pINode1> should follow <p pINode2>, or zero
		if the two nodes are equivalent. 

@rvalue S_OK | The nodes were sorted successfully.
@rvalue E_FAIL | An error occurred and the nodes could not be sorted.

@ex The following example sorts two nodes: |

HRESULT CMyParentNode::CompareNodes( IDMUSProdNode* pINode1, IDMUSProdNode* pINode2, int* pnResult )
{
	if( pINode1 == NULL
	||  pINode2 == NULL )
	{
		return E_INVALIDARG;
	}

	if( pnResult == NULL )
	{
		return E_POINTER;
	}

	CMyNode* pNode1 = (CMyNode *)pINode1;
	CMyNode* pNode2 = (CMyNode *)pINode2;

	if( pNode1->m_nID < pNode2->m_nID )
	{
		*pnResult = -1;
	}
	else if( pNode1->m_nID > pNode2->m_nID )
	{
		*pnResult = 1;
	}
	else
	{
		*pnResult = 0;
	}

	return S_OK;
}
	
@xref <i IDMUSProdSortNode>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/
