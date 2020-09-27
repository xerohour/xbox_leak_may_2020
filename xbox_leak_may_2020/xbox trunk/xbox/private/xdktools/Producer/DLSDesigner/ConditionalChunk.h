#ifndef __CONDITIONALCHUNK_H__
#define __CONDITIONALCHUNK_H__

#include <afxtempl.h>
#include "DLSComponent.h"
#include "RiffStrm.h"

#define MAX_OPERATORS	16

// Precedences for Opcodes
#define UNARY			0xFF
#define MULTIPLICATIVE	UNARY >> 1
#define ADDITIVE		UNARY >> 2
#define RELATIONAL		UNARY >> 3
#define EQUALITY		UNARY >> 4
#define BITWISE_AND		UNARY >> 5
#define BITWISE_XOR		UNARY >> 6
#define BITWISE_OR		UNARY >> 7
#define LOGICAL_AND		UNARY >> 8
#define LOGICAL_OR		UNARY >> 9

#define	OPCODE			0x01
#define QUERY			OPCODE << 1
#define CONSTANT		OPCODE << 2

class Opcode
{

public:
	Opcode(UINT nOpcode = 0, UINT nPrecedence = 0, BOOL bUnary = FALSE)
	{
		m_nOpcode = nOpcode;
		m_nPrecedence = nPrecedence;
		m_bUnary = bUnary;
		m_bRightToLeft = FALSE;

		// Associativity for Unary operator is from right to left
		if(bUnary)
		{
			m_bRightToLeft = TRUE;
		}
	};


public:

	CString m_sSymbol;			
	CString m_sHelpString;
	UINT	m_nOpcode;
	UINT	m_nPrecedence;
	BOOL	m_bRightToLeft;	// Associativity for the opcode
	BOOL	m_bUnary;
};

class CDLSQuery;

class CExpressionTreeNode
{

public:
	CExpressionTreeNode(Opcode& opcode)
	{
		m_pLeft = NULL;
		m_pRight = NULL;

		m_Opcode = opcode;
		m_bNodeType = OPCODE;
	};

	CExpressionTreeNode(GUID guidQuery)
	{
		m_pLeft = NULL;
		m_pRight = NULL;

		m_guidQuery = guidQuery;
		m_bNodeType = QUERY;
	};

	CExpressionTreeNode(ULONG ulConstant)
	{
		m_pLeft = NULL;
		m_pRight = NULL;
		
		m_ulConstant = ulConstant;
		m_bNodeType= CONSTANT;
	}


	CExpressionTreeNode* m_pLeft;	// LHS of the expression
	CExpressionTreeNode* m_pRight;	// RHS of the expression

	Opcode	m_Opcode;
	GUID	m_guidQuery;
	BOOL	m_bNodeType;
	ULONG	m_ulConstant;
};

struct Operation
{
	USHORT m_usOpcode;
	GUID   m_guidDLSQuery;
	ULONG  m_ulConstant;
};


class CSystemConfiguration;

class CConditionalChunk
{

public:
	CConditionalChunk(CDLSComponent* pComponent);
	~CConditionalChunk();

	HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
    HRESULT Save(IDMUSProdRIFFStream* pIRiffStream);

	HRESULT Load(IStream* pIStream);
	HRESULT	Save(IStream* pIStream);					// Used only for undo/Redo

	void	InitOpcodeStrings();
	HRESULT RegularToRPN(CString sRegularExpression);	// Parses the regular expression and saves it in the list of operations in the RPN format
	CString RPNToRegular();								// Goes through the operations list and constructs a regular expression			
	CString GetCondition();
	CString GetName();

	BOOL	Evaluate(CSystemConfiguration* pConfig);	// Evaluates the expression based on the passed configuration

public:
	static Opcode m_arrOpcodes[MAX_OPERATORS];				// The array of opcodes 
	
// Methods
private:
	
	CString GetNextToken(CString sExpression, int nStartIndex, int& nEndIndex);

	HRESULT CheckForSyntax(CString sRegularExpression);
	HRESULT MatchBrackets(CString sRegularExpression);
	HRESULT VerifyAllQueries(CString sRegularExpression);
	HRESULT VerifyAllOpcodes(CString sRegularExpression);
	BOOL	IsAnOpcode(const CString sToken, Opcode& opcodeType);
	BOOL	IsAnOpcode(const Operation* pOperation, Opcode& opcodeType);
	BOOL	IsConstant(CString sToken, ULONG& nConstant);
	
	HRESULT	ConvertToRPN(CString& sRegularExpression);
	CExpressionTreeNode* BuildExpressionTree(CString sRegularExpression);
	CExpressionTreeNode* BuildExpressionTree(Operation* pOperation, POSITION& position);
	CString GetNextOperator(CString sExpression, int nStartIndex, int& nEndindex, int& nDepth);
	CString GetNextOperand(CString sExpression, int nStartIndex, int& nEndIndex);

	void	TraversePostorder(CExpressionTreeNode* pNode, CString& sRPNForm, BOOL& bError);
	void	TraverseInorder(CExpressionTreeNode* pNode, CString& sRegularForm);
	void	AddToOperationsList(CExpressionTreeNode* pNode);

	void	DeleteOperationsList();
	void	DeleteExpressionTree();
	void	DeleteTreeNode(CExpressionTreeNode* pNode);

	DWORD	GetChunkSize();

	DWORD	EvaluateCondition(CExpressionTreeNode* pNode, CSystemConfiguration* pConfig);
	DWORD	Operate(Opcode opcode, DWORD dwLeft = 0, DWORD dwRight = 0);

	HRESULT	CountQueriesAndOpcodesInTree(DWORD& dwQueries, DWORD& dwOpcodes);

// Attributes
private:
	DWORD			m_dwRef;
	BOOL			m_bIsDirty;
	CDLSComponent*	m_pComponent;
	CString			m_sName;								// Friendly name for the condition
	CString			m_sRegularExpression;

	DWORD			m_dwQueriesInExpression;				// Number of queries in the expression; used in error checking after the parse tree is built
	DWORD			m_dwOpsInExpression;					// Number of operators/constants in the expression; used in error checking after the parse tree is built
	DWORD			m_dwQueriesInParseTree;
	DWORD			m_dwOpsInParseTree;
	
	CExpressionTreeNode* m_pExpressionTreeRoot;				// Pointer to the expression tree
	CTypedPtrList<CPtrList, Operation*> m_lstOperations;	// List of operations in the chunk

};


#endif //__CONDITIONALCHUNK_H__
