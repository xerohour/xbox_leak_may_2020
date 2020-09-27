#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "DLSComponent.h"
#include "dls2.h"
#include "ConditionalChunk.h"
#include "NameUnknownQueryDialog.h"

Opcode CConditionalChunk::m_arrOpcodes[MAX_OPERATORS] = 
					{Opcode(DLS_CDL_AND, BITWISE_AND), Opcode(DLS_CDL_OR, BITWISE_OR), Opcode(DLS_CDL_XOR, BITWISE_XOR), Opcode(DLS_CDL_ADD, ADDITIVE), 
					Opcode(DLS_CDL_SUBTRACT, ADDITIVE), Opcode(DLS_CDL_MULTIPLY, MULTIPLICATIVE), Opcode(DLS_CDL_DIVIDE, MULTIPLICATIVE), Opcode(DLS_CDL_LOGICAL_AND, LOGICAL_AND), 
					Opcode(DLS_CDL_LOGICAL_OR, LOGICAL_OR), Opcode(DLS_CDL_LT, RELATIONAL), Opcode(DLS_CDL_LE, RELATIONAL), Opcode(DLS_CDL_GT, RELATIONAL), 
					Opcode(DLS_CDL_GE, RELATIONAL), Opcode(DLS_CDL_EQ, EQUALITY), Opcode(DLS_CDL_NOT, UNARY, TRUE), Opcode(DLS_CDL_QUERYSUPPORTED, UNARY, TRUE)};


CConditionalChunk::CConditionalChunk(CDLSComponent* pComponent): m_pComponent(pComponent), 
m_pExpressionTreeRoot(NULL), 
m_dwRef(0),
m_dwQueriesInParseTree(0),
m_dwQueriesInExpression(0),
m_dwOpsInExpression(0),
m_dwOpsInParseTree(0),
m_bIsDirty(FALSE)
{
	ASSERT(pComponent);
	InitOpcodeStrings();
}

void CConditionalChunk::InitOpcodeStrings()
{
	for(int nIndex = 0; nIndex < MAX_OPERATORS; nIndex++)
	{
		m_arrOpcodes[nIndex].m_sSymbol.LoadString(IDS_OPCODE_BITWISE_AND + nIndex);
		m_arrOpcodes[nIndex].m_sHelpString.LoadString(IDS_OPCODE_HELP_BITWISE_AND + nIndex);
	}
}


CConditionalChunk::~CConditionalChunk()
{
	DeleteExpressionTree();
	DeleteOperationsList();
}


HRESULT CConditionalChunk::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
		return E_FAIL;

	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
		return E_POINTER;

	ASSERT(pckMain);
	if(pckMain == NULL)
		return E_POINTER;

	// Delete the previous list
	DeleteOperationsList();

	IStream* pIStream = pIRiffStream->GetStream();
	if(pIStream == NULL)
		return E_OUTOFMEMORY;

	HRESULT hr = E_FAIL;

	DWORD dwBytesRead = 0;
	while(dwBytesRead < pckMain->cksize)
	{
		Operation* pOperation = NULL;
		// Read the Operation.opcode
		DWORD cb = 0;
		USHORT usOpcode;
		hr = pIStream->Read(&usOpcode, sizeof(USHORT), &cb);
		if(FAILED(hr) || cb != sizeof(USHORT))
		{
			pIStream->Release();
			return hr;
		}
		
		dwBytesRead += cb;

		pOperation = new Operation;
		if(pOperation == NULL)
		{
			return E_OUTOFMEMORY;
		}
		pOperation->m_usOpcode = usOpcode;

		if(usOpcode == DLS_CDL_QUERY || usOpcode == DLS_CDL_QUERYSUPPORTED)
		{
			GUID guidQuery;
			hr = pIStream->Read(&guidQuery, sizeof(GUID), &cb);
			if(FAILED(hr) || cb != sizeof(GUID))
			{
				pIStream->Release();
				return hr;
			}

			CString sQueryName;
			if(m_pComponent->IsAPresetQuery(guidQuery, sQueryName) == FALSE)
			{
				CNameUnknownQueryDialog nameQueryDlg(NULL, m_pComponent);
				nameQueryDlg.SetQueryGUID(guidQuery);

				while(nameQueryDlg.DoModal() != IDOK);
				CString sQueryName = nameQueryDlg.GetQueryName();
				CDLSQuery* pDLSQuery = new CDLSQuery(guidQuery, sQueryName);
				ASSERT(pDLSQuery);
				m_pComponent->WriteQueryToRegistryAndAddToList(pDLSQuery);
			}
			
			dwBytesRead += cb;
			pOperation->m_guidDLSQuery = guidQuery;
		}
		else if(usOpcode == DLS_CDL_CONST)
		{
			ULONG ulConstant = 0;
			hr = pIStream->Read(&ulConstant, sizeof(ULONG), &cb);
			if(FAILED(hr) || cb != sizeof(ULONG))
			{
				pIStream->Release();
				return hr;
			}
			
			dwBytesRead += cb;
			pOperation->m_ulConstant = ulConstant;
		}

		m_lstOperations.AddTail(pOperation);
	}

	pIStream->Release();

	// Build the expression tree and get the regular expression form
	m_sRegularExpression = RPNToRegular();

	return hr;
}

HRESULT CConditionalChunk::Save(IDMUSProdRIFFStream* pIRiffStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
		return E_FAIL;

	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
		return E_POINTER;

	if(m_lstOperations.GetCount() == 0)
		return S_OK;

	IStream* pIStream = pIRiffStream->GetStream();
	if(pIStream == NULL)
		return E_OUTOFMEMORY;

	HRESULT hr = E_FAIL;
	MMCKINFO ck;
	ck.ckid = FOURCC_CDL;
	ck.cksize = GetChunkSize();
	hr = pIRiffStream->CreateChunk(&ck, 0);
	if(FAILED(hr))
	{
		pIStream->Release();
		return E_FAIL;
	}

	POSITION position = m_lstOperations.GetHeadPosition();
	while(position)
	{
		Operation* pOperation = (Operation*) m_lstOperations.GetNext(position);
		ASSERT(pOperation);
		if(pOperation)
		{
			DWORD cb = 0;
			USHORT usOpcode = pOperation->m_usOpcode;
			hr = pIStream->Write((LPSTR)&usOpcode, sizeof(USHORT), &cb);
			if(FAILED(hr) || cb != sizeof(USHORT))
			{
				pIStream->Release();
				return E_FAIL;
			}

			if(usOpcode == DLS_CDL_QUERY || usOpcode == DLS_CDL_QUERYSUPPORTED)
			{
				hr = pIStream->Write((LPSTR)&(pOperation->m_guidDLSQuery), sizeof(GUID), &cb);
				if(FAILED(hr) || cb != sizeof(GUID))
				{
					pIStream->Release();
					return E_FAIL;
				}
			}
			else if(usOpcode == DLS_CDL_CONST)
			{
				hr = pIStream->Write((LPSTR)&(pOperation->m_ulConstant), sizeof(ULONG), &cb);
				if(FAILED(hr) || cb != sizeof(ULONG))
				{
					pIStream->Release();
					return E_FAIL;
				}
			}
		}
	}

	pIStream->Release();

	return S_OK;
}

void CConditionalChunk::DeleteExpressionTree()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pExpressionTreeRoot == NULL)
		return;
	
	DeleteTreeNode(m_pExpressionTreeRoot);
	m_pExpressionTreeRoot = NULL;
}

void CConditionalChunk::DeleteTreeNode(CExpressionTreeNode* pNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(pNode == NULL)
		return;

	DeleteTreeNode(pNode->m_pLeft);
	pNode->m_pLeft = NULL;

	DeleteTreeNode(pNode->m_pRight);
	pNode->m_pRight = NULL;

	if(pNode->m_pLeft == NULL && pNode->m_pRight == NULL)
	{
		delete pNode;
		pNode = NULL;
	}

}



void CConditionalChunk::DeleteOperationsList()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	while(!m_lstOperations.IsEmpty())
	{
		Operation* pOperation = (Operation*) m_lstOperations.RemoveHead();
		if(pOperation)
		{
			delete pOperation;
		}
	}
}


HRESULT CConditionalChunk::RegularToRPN(CString sRegularExpression)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = E_FAIL;
	
	CString sNone;
	sNone.LoadString(IDS_NONE);

	// No condition?
	if(sRegularExpression.IsEmpty() || sRegularExpression == sNone)
	{
		DeleteOperationsList();
		DeleteExpressionTree();
		m_sRegularExpression = "";
		return S_OK;
	}

	// Remove all unwanted characters from both ends of the string
	sRegularExpression.TrimLeft();
	sRegularExpression.TrimRight();

	if(SUCCEEDED(hr = CheckForSyntax(sRegularExpression)))
	{
		m_sRegularExpression = sRegularExpression;
		hr = ConvertToRPN(sRegularExpression);
		if(FAILED(hr) || m_dwQueriesInParseTree != m_dwQueriesInExpression || m_dwOpsInExpression != m_dwOpsInParseTree)
		{
			DeleteOperationsList();
			DeleteExpressionTree();

			CString sErrorMessage;
			sErrorMessage.LoadString(IDS_MALFORMED_EXPRESSION);
			AfxMessageBox(sErrorMessage);
			return E_FAIL;
		}

	}

	return hr;
}

HRESULT	CConditionalChunk::ConvertToRPN(CString& sRegularExpression)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(sRegularExpression.IsEmpty())
		return E_FAIL;

	if(m_pExpressionTreeRoot)
	{
		DeleteExpressionTree();
	}

	m_pExpressionTreeRoot = BuildExpressionTree(sRegularExpression);
	
	// Do the postorder traversal and get the RPN form
	if(m_pExpressionTreeRoot)
	{
		CString sRPNForm;
		DeleteOperationsList();
		BOOL bError = FALSE;
		m_dwQueriesInParseTree = 0;
		m_dwOpsInParseTree = 0;
		TraversePostorder(m_pExpressionTreeRoot, sRPNForm, bError);
		if(bError == TRUE)
		{
			DeleteOperationsList();
			DeleteExpressionTree();

			/*CString sErrorMessage;
			sErrorMessage.LoadString(IDS_MALFORMED_EXPRESSION);
			AfxMessageBox(sErrorMessage);*/
			return E_FAIL;
		}


		CString sRegularForm;
		TraverseInorder(m_pExpressionTreeRoot, sRegularForm);
		m_sRegularExpression = sRegularForm;
		
		return S_OK;
	}
	
	return E_FAIL;
}

// Traverses the expression tree in postorder adding the tokens to the list of operations
void CConditionalChunk::TraversePostorder(CExpressionTreeNode* pNode, CString& sRPNForm, BOOL& bError)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(bError != FALSE)
		return;

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
		return;

	if(pNode == NULL)
		return;

	TraversePostorder(pNode->m_pLeft, sRPNForm, bError);
	TraversePostorder(pNode->m_pRight, sRPNForm, bError);
	
	if(!bError)
	{
		if(pNode->m_bNodeType == QUERY)
		{
			CString sQueryName;
			if(m_pComponent->IsAPresetQuery(pNode->m_guidQuery, sQueryName))
			{
				sRPNForm += sQueryName;
				m_dwQueriesInParseTree++;
			}
		}
		else if(pNode->m_bNodeType == CONSTANT)
		{
			CString sConstant;
			sConstant.Format("%d", pNode->m_ulConstant);
			sRPNForm += sConstant;
			m_dwOpsInParseTree++;
		}
		else if(pNode->m_bNodeType == OPCODE)
		{
			sRPNForm += pNode->m_Opcode.m_sSymbol;

			if(pNode->m_pRight == NULL)
			{
				bError = TRUE;
			}
			if(pNode->m_Opcode.m_bUnary && pNode->m_pLeft != NULL)
			{
				bError = TRUE;
			}
			else if(!pNode->m_Opcode.m_bUnary && pNode->m_pLeft == NULL)
			{
				bError = TRUE;
			}

			m_dwOpsInParseTree++;
		}
		else
		{
			bError = TRUE;
		}

		AddToOperationsList(pNode);
	}
}


void CConditionalChunk::TraverseInorder(CExpressionTreeNode* pNode, CString& sRegularForm)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
		return;

	if(pNode == NULL)
		return;

	if(pNode->m_bNodeType == OPCODE)
	{
		sRegularForm += "(";
	}

	TraverseInorder(pNode->m_pLeft, sRegularForm);
	
	if(pNode->m_bNodeType == QUERY)
	{
		CString sQueryName;
		if(m_pComponent->IsAPresetQuery(pNode->m_guidQuery, sQueryName))
		{
			sRegularForm += sQueryName;
		}
	}

	if(pNode->m_bNodeType == CONSTANT)
	{
		CString sConstant;
		sConstant.Format("%d", pNode->m_ulConstant);
		sRegularForm += sConstant;
	}

	if(pNode->m_bNodeType == OPCODE)
	{
		CString sOperator = " " + pNode->m_Opcode.m_sSymbol;
		if(!(pNode->m_Opcode.m_bUnary))
		{
			sOperator += " ";
		}

		sRegularForm += sOperator;
	}

	TraverseInorder(pNode->m_pRight, sRegularForm);

	if(pNode->m_bNodeType == OPCODE)
	{
		sRegularForm = sRegularForm + ")";
	}
}


void CConditionalChunk::AddToOperationsList(CExpressionTreeNode* pNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(pNode == NULL)
		return;

	Operation* pOperation = new Operation;
	
	if(pNode->m_bNodeType == OPCODE)
	{
		pOperation->m_usOpcode = USHORT(pNode->m_Opcode.m_nOpcode);
	}
	if(pNode->m_bNodeType == CONSTANT)
	{
		pOperation->m_usOpcode = DLS_CDL_CONST;
		pOperation->m_ulConstant = pNode->m_ulConstant;
	}
	if(pNode->m_bNodeType == QUERY)
	{
		pOperation->m_usOpcode = DLS_CDL_QUERY;
		pOperation->m_guidDLSQuery = pNode->m_guidQuery;
	}

	m_lstOperations.AddTail(pOperation);
}

CExpressionTreeNode* CConditionalChunk::BuildExpressionTree(CString sRegularExpression)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(sRegularExpression.IsEmpty())
		return NULL;

	// Trim the whitespaces from both ends of the expression
	sRegularExpression.TrimLeft();
	sRegularExpression.TrimRight();

	// Find the operator of the lowest precedence
	int nStartIndex = 0;
	int nEndIndex = 0;
	int nDepthLow = 0;
	CString	sOperator = GetNextOperator(sRegularExpression, nStartIndex, nEndIndex, nDepthLow);
	int nOffset = nEndIndex;
	Opcode opcodeLow;
	if(IsAnOpcode(sOperator, opcodeLow))
	{
		while(!sOperator.IsEmpty())
		{
			nStartIndex = nEndIndex;
			int nDepth = 0;
			sOperator = GetNextOperator(sRegularExpression, nStartIndex, nEndIndex, nDepth);

			Opcode opcode;
			if(IsAnOpcode(sOperator, opcode))
			{
				if(((opcode.m_nPrecedence < opcodeLow.m_nPrecedence) && nDepth <= nDepthLow) || nDepth < nDepthLow)
				{
					nDepthLow = nDepth;
					opcodeLow = opcode;
					nOffset = nEndIndex;
				}
			}
		}
		
		CExpressionTreeNode* pNode = new CExpressionTreeNode(opcodeLow);
		
		// Parse the left and right sides of the expression
		if(opcodeLow.m_bUnary)
		{
			pNode->m_pLeft = NULL;
		}
		else
		{
			pNode->m_pLeft = BuildExpressionTree(sRegularExpression.Left(nOffset - opcodeLow.m_sSymbol.GetLength()));
		}
		
		pNode->m_pRight = BuildExpressionTree(sRegularExpression.Right(sRegularExpression.GetLength() - nOffset));
		return pNode;
	}

	nStartIndex = 0;
	nEndIndex = 0;
	ULONG ulConstant = 0;
	CString sOperand = GetNextOperand(sRegularExpression, nStartIndex, nEndIndex);
	GUID guidQuery;
	if(m_pComponent->IsAPresetQuery(sOperand, guidQuery))
	{
		CExpressionTreeNode* pNode = new CExpressionTreeNode(guidQuery);
		return pNode;
	}
	else if(IsConstant(sOperand, ulConstant))
	{
		CExpressionTreeNode* pNode = new CExpressionTreeNode(ulConstant);
		return pNode;
	}

	return NULL;
}


HRESULT CConditionalChunk::CheckForSyntax(CString sRegularExpression)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = E_FAIL;
	
    // Reset the count for queries and opcodes
    m_dwOpsInExpression = 0;
    m_dwQueriesInExpression = 0;

	if(FAILED(hr = MatchBrackets(sRegularExpression)))
		return hr;

	if(FAILED(hr = VerifyAllQueries(sRegularExpression)))
		return hr;

	if(FAILED(hr = VerifyAllOpcodes(sRegularExpression)))
		return hr;

	return hr;
}

HRESULT CConditionalChunk::MatchBrackets(CString sRegularExpression)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	int nCount = 0;
	int nLength = sRegularExpression.GetLength();
	for(int nIndex = 0; nIndex < nLength; nIndex++)
	{
		if(sRegularExpression.GetAt(nIndex) == '(')
			nCount++;
		else if(sRegularExpression.GetAt(nIndex) == ')')
			nCount--;
	}

	if(nCount != 0)
	{
		CString sMissingParen = ")";
		if(nCount < 0)
		{
			sMissingParen = "(";
		}

		CString sErrorMessage;
		sErrorMessage.Format(IDS_ERR_MATCHING_PARENTHESIS_FORMAT, sMissingParen);
		AfxMessageBox(sErrorMessage);
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CConditionalChunk::VerifyAllQueries(CString sRegularExpression)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

    // Go through the expression and check for invalid Queries
	int nTokenStartIndex = 0;
	int nTokenEndIndex = 0;

	int nLength = sRegularExpression.GetLength();
	for(int nIndex = nTokenEndIndex; nIndex < nLength; nIndex++)
	{
		char cCheckedChar = sRegularExpression.GetAt(nIndex);
		if(isalnum(cCheckedChar) && nTokenStartIndex == nTokenEndIndex)
		{
			nTokenStartIndex = nIndex;
			nTokenEndIndex = nTokenStartIndex + 1; 
		}

		if((!isalnum(cCheckedChar) && nTokenStartIndex != nTokenEndIndex) || nIndex + 1 == nLength)
		{
			if(nIndex + 1 == nLength && isalnum(cCheckedChar))
			{
				nIndex = nLength;
			}

			nTokenEndIndex = nIndex;
			
			CString sToken = sRegularExpression.Mid(nTokenStartIndex, abs(nTokenEndIndex - nTokenStartIndex));
			sToken.TrimLeft();
			sToken.TrimRight();
			GUID guidQuery;
			if(sToken != ")" && sToken != "(")
			{
				if(m_pComponent->IsAPresetQuery(sToken, guidQuery) == FALSE)
				{
					ULONG ulConstant = 0;
					Opcode opcodeType;
					if(IsConstant(sToken, ulConstant) == FALSE && IsAnOpcode(sToken, opcodeType) == FALSE)
					{
						CString sErrorMessage;
						sErrorMessage.Format(IDS_ERR_UNKNOWN_QUERY, sToken);
						AfxMessageBox(sErrorMessage);
						return E_FAIL;
					}
					else
					{
						m_dwOpsInExpression++;
					}
				}
				else
				{
					m_dwQueriesInExpression++;
				}
			}
			
			nTokenStartIndex = nTokenEndIndex;
			nIndex = nTokenStartIndex;
		}
	}

	return S_OK;
}



HRESULT CConditionalChunk::VerifyAllOpcodes(CString sRegularExpression)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Go through the expression and check for invalid Queries
	int nTokenStartIndex = 0;
	int nTokenEndIndex = 0;

	int nLength = sRegularExpression.GetLength();
	for(int nIndex = nTokenEndIndex; nIndex < nLength; nIndex++)
	{
		char cTemp = sRegularExpression.GetAt(nIndex);
		if(!isalnum(cTemp) && (cTemp != ')' && cTemp != '(' && cTemp != ' ') && nTokenStartIndex == nTokenEndIndex)
		{
			nTokenStartIndex = nIndex;
		}

		if((isalnum(cTemp) || cTemp == ')' || cTemp == '(' || cTemp == ' ') && nTokenStartIndex != nTokenEndIndex)
		{
			nTokenEndIndex = nIndex;
			CString sToken = sRegularExpression.Mid(nTokenStartIndex, abs(nTokenEndIndex - nTokenStartIndex));
			Opcode opcodeType;
			if(IsAnOpcode(sToken, opcodeType) == FALSE)
			{
				CString sErrorMessage;
				sErrorMessage.Format(IDS_ERR_UNKNOWN_OPERATOR, sToken);
				AfxMessageBox(sErrorMessage);
				return E_FAIL;
			}
			else
			{
				m_dwOpsInExpression++;
			}
			
			nTokenStartIndex = ++nTokenEndIndex;
			nIndex = nTokenStartIndex;
		}
	}

	return S_OK;

}



BOOL CConditionalChunk::IsAnOpcode(const CString sToken, Opcode& opcodeType)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	for(int nIndex = 0; nIndex < MAX_OPERATORS; nIndex++)
	{
		if(m_arrOpcodes[nIndex].m_sSymbol == sToken)
		{
			opcodeType = m_arrOpcodes[nIndex];
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CConditionalChunk::IsAnOpcode(const Operation* pOperation, Opcode& opcodeType)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(pOperation == NULL)
	{
		return FALSE;
	}

	if(pOperation->m_usOpcode == DLS_CDL_QUERY || pOperation->m_usOpcode == DLS_CDL_CONST)  
	{
		return FALSE;
	}

	for(int nIndex = 0; nIndex < MAX_OPERATORS; nIndex++)
	{
		if(m_arrOpcodes[nIndex].m_nOpcode == pOperation->m_usOpcode)
		{
			opcodeType = m_arrOpcodes[nIndex];
			return TRUE;
		}
	}

	return FALSE;
}

CString CConditionalChunk::RPNToRegular()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pExpressionTreeRoot)
	{
		DeleteExpressionTree();
	}

	if(m_lstOperations.GetCount() == 0)
		return "";

	POSITION position = m_lstOperations.GetTailPosition();
	Operation* pOperation = (Operation*) m_lstOperations.GetPrev(position);;
	m_pExpressionTreeRoot = BuildExpressionTree(pOperation, position);

	if(m_pExpressionTreeRoot)
	{
		CString sRegularForm;
		TraverseInorder(m_pExpressionTreeRoot, sRegularForm);
		return sRegularForm;
	}

	return "";
}


CExpressionTreeNode* CConditionalChunk::BuildExpressionTree(Operation* pOperation, POSITION& position)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(pOperation == NULL)
	{
		return NULL;
	}

	// If this is an operator
	Opcode opcode;
	if(IsAnOpcode(pOperation, opcode))
	{
		CExpressionTreeNode* pNode = new CExpressionTreeNode(opcode);
		
		// Special case for query supported operator
		if(opcode.m_nOpcode == DLS_CDL_QUERYSUPPORTED)
		{
			pNode->m_pLeft = NULL;
			CExpressionTreeNode* pQueryNode = new CExpressionTreeNode(pOperation->m_guidDLSQuery);
			pQueryNode->m_pLeft = NULL;
			pQueryNode->m_pRight = NULL;

			pNode->m_pRight = pQueryNode;

			return pNode;
		}

		if(position)
		{
			Operation* pPrevOperation = (Operation*) m_lstOperations.GetPrev(position);
			pNode->m_pRight = BuildExpressionTree(pPrevOperation, position);
			if(position)
			{
				pNode->m_pLeft = NULL;
				if(!opcode.m_bUnary)
				{
					pPrevOperation = (Operation*) m_lstOperations.GetPrev(position);
					pNode->m_pLeft = BuildExpressionTree(pPrevOperation, position);
				}
			}
		}

		return pNode;
	}

	if(pOperation->m_usOpcode == DLS_CDL_QUERY)
	{
		CExpressionTreeNode* pNode = new CExpressionTreeNode(pOperation->m_guidDLSQuery);
		return pNode;
	}

	if(pOperation->m_usOpcode == DLS_CDL_CONST)
	{
		CExpressionTreeNode* pNode = new CExpressionTreeNode(pOperation->m_ulConstant);
		return pNode;
	}

	return NULL;
}


BOOL CConditionalChunk::IsConstant(CString sToken, ULONG& ulValue)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	int nLength = sToken.GetLength();
	for(int nIndex = 0; nIndex < nLength; nIndex++)
	{
		if(!isalnum(sToken[nIndex]) || isalpha(sToken[nIndex]))
		{
			ulValue = 0;
			return FALSE;
		}
	}

	ulValue = atoi((LPCSTR)sToken);
	return TRUE;
}

CString CConditionalChunk::GetNextToken(CString sExpression, int nStartIndex, int& nEndIndex)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(sExpression.IsEmpty())
	{
		return sExpression;
	}
	
	// Trim the whitespaces
	sExpression.TrimLeft();
	sExpression.TrimRight();

	int nLength = sExpression.GetLength();

	// skip white spaces and brackets
	while(nStartIndex < nLength && (sExpression[nStartIndex] == ' ' || sExpression[nStartIndex] == '(' || sExpression[nStartIndex] == ')'))
	{
		nStartIndex++;
	}

	if(nStartIndex >= nLength)
	{
		return "";
	}

	// If the first character is not alphanumeric then it must be an operator?
	if(!isalnum(sExpression[nStartIndex]))
	{
		for(int nIndex = nStartIndex; nIndex < nLength; nIndex++)
		{
			if(isalnum(sExpression[nIndex]) || sExpression[nIndex] == ' ' || sExpression[nIndex] == '(' || sExpression[nIndex] == ')')
			{
				break;
			}
		}
		
		nEndIndex = nIndex;

		return sExpression.Mid(nStartIndex, abs(nEndIndex - nStartIndex));
	}
	
	// This could either be a query or a constant 
	for(int nIndex = nStartIndex; nIndex < nLength; nIndex++)
	{
		if(!isalnum(sExpression[nIndex]) || sExpression[nIndex] == ' ' || sExpression[nIndex] == '(' || sExpression[nIndex] == ')')
		{
			break;
		}
	}
	
	nEndIndex = nIndex;

	return sExpression.Mid(nStartIndex, abs(nEndIndex - nStartIndex));
}


CString CConditionalChunk::GetNextOperator(CString sExpression, int nStartIndex, int& nEndIndex, int& nDepth)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString sToken = GetNextToken(sExpression, nStartIndex, nEndIndex);
	while(!sToken.IsEmpty())
	{
		nStartIndex = nEndIndex;
		Opcode opcode;
		if(IsAnOpcode(sToken, opcode))
		{
			// Calculate depth
			int nLength = sExpression.GetLength();
			for(int nIndex = nEndIndex - 1; (nIndex >= 0 && nIndex < nLength); nIndex--)
			{
				if(sExpression[nIndex] == '(')
				{
					nDepth++;
				}
				if(sExpression[nIndex] == ')')
				{
					nDepth--;
				}
			}

			return sToken;
		}

		sToken = GetNextToken(sExpression, nStartIndex, nEndIndex);
	}

	return sToken;
}

CString CConditionalChunk::GetNextOperand(CString sExpression, int nStartIndex, int& nEndIndex)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
		return "";

	CString sToken = GetNextToken(sExpression, nStartIndex, nEndIndex);
	while(!sToken.IsEmpty())
	{
		nStartIndex = nEndIndex;
		GUID guidQuery;
		ULONG ulConstant = 0;
		if(m_pComponent->IsAPresetQuery(sToken, guidQuery) || IsConstant(sToken, ulConstant))
			return sToken;

		sToken = GetNextToken(sExpression, nStartIndex, nEndIndex);
	}

	return sToken;
}


CString CConditionalChunk::GetCondition()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return m_sRegularExpression;
}

DWORD CConditionalChunk::GetChunkSize()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	DWORD dwSize = 0;
	POSITION position = m_lstOperations.GetHeadPosition();
	while(position)
	{
		Operation* pOperation = (Operation*) m_lstOperations.GetNext(position);
		ASSERT(pOperation);
		if(pOperation)
		{
			USHORT usOpcode = pOperation->m_usOpcode; 
			dwSize += sizeof(usOpcode);
			
			if(usOpcode == DLS_CDL_QUERY || usOpcode == DLS_CDL_QUERYSUPPORTED)
			{
				dwSize += sizeof(GUID);
			}
			else if(usOpcode == DLS_CDL_CONST)
			{
				dwSize += sizeof(ULONG);
			}
		}
	}

	return dwSize;
}

CString CConditionalChunk::GetName()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString sNone;
	sNone.LoadString(IDS_NONE);

	if(m_sRegularExpression.IsEmpty())
		return sNone;

	return m_sRegularExpression;
}


BOOL CConditionalChunk::Evaluate(CSystemConfiguration* pConfig)
{
	ASSERT(pConfig);
	if(pConfig == NULL)
	{
		return FALSE;
	}

	// There's no condition set
	if(m_sRegularExpression.IsEmpty())
	{
		return TRUE;
	}

	BOOL bResult = (BOOL) EvaluateCondition(m_pExpressionTreeRoot, pConfig);
	return bResult;
}

DWORD CConditionalChunk::EvaluateCondition(CExpressionTreeNode* pNode, CSystemConfiguration* pConfig)
{
	if(pNode == NULL)
	{
		return 0;
	}

	if(pConfig == NULL)
	{
		return 0;
	}

	DWORD dwLeft = FALSE;
	DWORD dwRight = FALSE;

	switch(pNode->m_bNodeType)
	{
		case CONSTANT:
		{
			return pNode->m_ulConstant;
		}
		
		case QUERY:
		{
			DWORD dwValue = 0;
			pConfig->GetValueForQuery(pNode->m_guidQuery, dwValue);
			return dwValue;
		}
		case OPCODE:
		{
			// Are we trying to see if this query is supported?
			if(pNode->m_Opcode.m_nOpcode == DLS_CDL_QUERYSUPPORTED)
			{	
				DWORD dwValue = 0;
				GUID guidQuery = pNode->m_pRight->m_guidQuery;
				HRESULT hr = pConfig->GetValueForQuery(guidQuery, dwValue);
				if(FAILED(hr))
				{
					return FALSE;
				}
				else
				{
					return TRUE;
				}
			}
			// Else evaluate both sides for a binary operator
			else
			{
				if(!pNode->m_Opcode.m_bUnary)
				{
					
					dwLeft = EvaluateCondition(pNode->m_pLeft, pConfig);
				}
				dwRight = EvaluateCondition(pNode->m_pRight, pConfig);
			}

			return Operate(pNode->m_Opcode, dwLeft, dwRight);
		}
	}

	return 0;
}

DWORD CConditionalChunk::Operate(Opcode opcode, DWORD dwLeft, DWORD dwRight)
{
	switch(opcode.m_nOpcode)
	{
		// Bitwise AND
		case DLS_CDL_AND:
		{
			return (dwLeft & dwRight);
		}

		// Bitwise OR
		case DLS_CDL_OR:
		{
			return (dwLeft | dwRight);
		}

		case DLS_CDL_XOR:
		{
			return (dwLeft ^ dwRight);
		}

		case DLS_CDL_ADD:
		{
			return (dwLeft + dwRight);
		}   	  

		case DLS_CDL_SUBTRACT:
		{
			return (dwLeft - dwRight);
		}   	

		case DLS_CDL_MULTIPLY:
		{
			return (dwLeft * dwRight);
		}

		case DLS_CDL_DIVIDE:
		{
			if(dwRight == 0)
			{
				// Divide by Zero Error!!
				return 0;
			}
			
			return (dwLeft / dwRight);
		}		  

		case DLS_CDL_LOGICAL_AND:
		{
			return (dwLeft && dwRight);
		}	  

		case DLS_CDL_LOGICAL_OR:
		{
			return (dwLeft || dwRight);
		}	

		case DLS_CDL_LT:
		{
			return (dwLeft < dwRight);
		}			  

		case DLS_CDL_LE:
		{
			return (dwLeft <= dwRight);
		}
		
		case DLS_CDL_GT:
		{
			return (dwLeft > dwRight);
		}	    	

		case DLS_CDL_GE:
		{
			return (dwLeft >= dwRight);
		}

		case DLS_CDL_EQ:
		{
			return (dwLeft == dwRight);
		}

		case DLS_CDL_NOT:
		{
			// Any non-zero value is TRUE so return 0 for it's negation
			return ((dwLeft == 0) ? 1 : 0);
		}

		default:
		{
			// Default is always failure
			return 0;
		}
	}
}