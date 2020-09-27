#include "stdafx.h"
#pragma hdrstop

#include "tokens.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CToken - Wrapper for a TOKEN returned from the lexer.


// Coarse level categorization of tokens.
const static TOKCLS rgtokCls[] = 
{
		tokclsError, 
		tokclsKeyWordMin,
		tokclsOpMin,
		tokclsOpSpecMin,
		tokclsIdentMin,
		tokclsIdentUser,
		tokclsConstInteger,
		tokclsConstReal,
		tokclsConstString,
		tokclsCommentMin,
		tokclsUserMax		// REVIEW: what should we do about the user range. 
};
		
const TOKCLS * CToken::s_rgtokCls = rgtokCls;
int CToken::ctokCls = sizeof(rgtokCls)/sizeof(rgtokCls[0]);

CToken::CToken()
{
	m_token = tokclsError;
	m_tokCls = tokclsError;
}

CToken::CToken(const TOKEN& token)
{
	m_token = token;
	m_tokCls = (TOKCLS)-1;	// Just implies we have not initialized this yet.
}

const CToken& CToken::operator= (const CToken& ctoken)
{
	m_token = ctoken.m_token;
	m_tokCls = (TOKCLS)-1;
	return *this;
}					

const CToken& CToken::operator= (const TOKEN& token)
{
	m_token = token;
	m_tokCls = (TOKCLS)-1;
	return *this;
}					

CToken::operator TOKEN() const
{
	return m_token;
}

TOKCLS CToken::GetTokClass() const
{
	if ( m_tokCls == (TOKCLS)-1)	// not yet determined.
	{
		TOKEN tokSearch = m_token & tokclsMask;

		for ( int i = 0 ; i < ctokCls ; i++ )
		{
			if ( s_rgtokCls[i] > tokSearch)
			{
				((CToken *)this)->m_tokCls = s_rgtokCls[i - 1];	 // logically const.
				break;
			}
		}
		ASSERT(m_tokCls != (TOKCLS)-1);
	}

	return m_tokCls;
}

BOOL CToken::IsCxxIdent() const
{
	return IsUserIdent() || m_token ==  tokKEYW_THIS ;
}

// Is this one of the class of assignment operators.
// i.e. =, +=, -=, *= etc etc.
BOOL CToken::IsAssignOp() const
{
	TOKCLS tokCls = GetTokClass();
	if ( tokCls == tokclsOpMin )
	{
		switch (m_token)
		{
			case tokOP_MODEQ:
			case tokOP_ANDEQ:	
			case tokOP_MULEQ:	
			case tokOP_PLUSEQ:	
			case tokOP_MINUSEQ:	
			case tokOP_DIVEQ:	
			case tokOP_LEQ:	
			case tokOP_ASGN:	
			case tokOP_GEQ:	
			case tokOP_XOREQ:
			case tokOP_OREQ:
			case tokOP_RSHIFTEQ:
			case tokOP_LSHIFTEQ:
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CToken::IsIndirectionOp() const
{
	if (m_token == tokOP_AND || m_token == tokOP_MUL )
		return TRUE;
	else
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CTokenStream

CTokenStream::CTokenStream(LPSOURCEEDIT pISourceEdit, ILINE begLine, ILINE endLine)
{
	ASSERT(pISourceEdit != NULL);
	ASSERT(begLine <= endLine);

	// The two conditions below could happen if we found the 
	// incorrect source file or have bad debug info.
	ILINE cLines = 0;
	pISourceEdit->CLines((ULONG*)&cLines);
	if (begLine < 0 || begLine >= cLines)
		return;
	
	if (endLine < 0 || endLine >= cLines)
		return;

	// get the lexer
	CLex *pLex = NULL;
	pISourceEdit->GetLexer((LPVOID*)&pLex);
	if (NULL == pLex)
		return;

	// Make sure we have a "C/C++" lexer, ignore if we have something else
	CString strLang;
	pISourceEdit->GetLanguage(strLang);
	if ((strLang != "C/C++") && (strLang != "Microsoft TSQL") && (strLang != "Microsoft SQL") && (strLang != "Java"))
		return;

	LXS lxs = 0; // O is the ground state for a LXS.
	DWORD dwReserved = 0;

	if ( begLine > 0 )
		pISourceEdit->GetLXS( begLine - 1, &lxs);

	m_rgTokenLine.SetSize(0, endLine - begLine + 1);

	ILINE iLine = begLine;

	int cNonTrivialLines = 0;

	for (int nIndex = 0; iLine <= endLine ; nIndex++, iLine++ )
	{
		PCSTR pcurLine;
		ULONG cb;
		CTokenLine *pTokLine =  new CTokenLine( );
	
		// retrieve the line from the text editor.
		if (!(NO_ERROR == pISourceEdit->GetLineTextSansCrLf(iLine, &pcurLine, &cb)))
			continue;

		// REVIEW: Does the compiler optimize away the redundant CString object in this case. 
		pTokLine->m_strLine = CString(pcurLine, cb);
		
		pLex->CbLexLine(pTokLine->m_strLine, pTokLine->m_strLine.GetLength(), &lxs, pTokLine->m_rgtxtb, dwReserved); 			

		if ( pTokLine->m_rgtxtb.GetSize() != 0 )
		{
			m_rgTokenLine.SetAtGrow(cNonTrivialLines++, pTokLine);
		}
		else 
		{
			delete pTokLine;	// free up the allocated memory.
		}
	}

	m_rgTokenLine.SetSize(cNonTrivialLines);
}

CTokenStream::~CTokenStream( )
{
	for ( int nIndex = 0; nIndex < m_rgTokenLine.GetSize() ; nIndex++ )
		delete m_rgTokenLine[nIndex];
}

CTokenStreamPos CTokenStream::GetHeadPosition() const
{
	
	CTokenStreamPos pos;

	if ( m_rgTokenLine.GetSize() != 0 )
		pos.m_nIndex = 0;
	else
		pos.m_nIndex = -1;

	pos.m_nToken = 0;

	return pos;
}

CTokenStreamPos CTokenStream::GetTailPosition () const
{
	CTokenStreamPos pos;

	if ( m_rgTokenLine.GetSize() == 0 )
	{
		pos.m_nIndex = -1;
		pos.m_nToken = 0;
	}
	else 
	{
		pos.m_nIndex = m_rgTokenLine.GetSize () - 1;
		pos.m_nToken = m_rgTokenLine[pos.m_nIndex]->m_rgtxtb.GetSize () - 1;
	}

	return pos;
}

TXTB* CTokenStream::GetAt (CTokenStreamPos& pos)
{
	ASSERT_VALID(this);

	if (!pos.IsValid ())
	{
		return NULL;
	}
	return &m_rgTokenLine[pos.m_nIndex]->m_rgtxtb[pos.m_nToken];
}

TXTB* CTokenStream::GetNext (CTokenStreamPos& pos)
{
	ASSERT_VALID(this);

	TXTB *ptxtb = GetAt (pos);

	if (ptxtb == NULL)
		return NULL;

	if ( pos.m_nToken + 1 < m_rgTokenLine[pos.m_nIndex]->m_rgtxtb.GetSize() )
	{
		pos.m_nToken++;
	}
	else 
	{
		// Search forward looking for the next valid token.
		int nIndex = pos.m_nIndex;
		while (++nIndex < m_rgTokenLine.GetSize() )
		{
			if ( m_rgTokenLine[nIndex]->m_rgtxtb.GetSize() != 0 )
			{
				pos.m_nToken = 0;
				pos.m_nIndex = nIndex;
				ASSERT_VALID(this);
				return ptxtb;
			}
		}
		pos.m_nIndex = -1;
	}
	return ptxtb;
}

TXTB* CTokenStream::GetPrev (CTokenStreamPos& pos)
{
	ASSERT_VALID(this);

	TXTB *ptxtb = GetAt (pos);

	if (ptxtb == NULL)
		return NULL;

	if ( pos.m_nToken > 0  )
	{
		pos.m_nToken--;
	}
	else 
	{
		// Search backwards looking for the previous valid token.
		int nIndex = pos.m_nIndex;
		while (--nIndex >= 0 )
		{
			if ( m_rgTokenLine[nIndex]->m_rgtxtb.GetSize() != 0 )
			{
				pos.m_nToken = m_rgTokenLine[nIndex]->m_rgtxtb.GetSize() - 1;
				pos.m_nIndex = nIndex;
				ASSERT_VALID(this);
				return ptxtb;
			}
		}
		pos.m_nIndex = -1;
	}

	return ptxtb;
}

CString CTokenStream::GetStringAt (CTokenStreamPos& pos)
{
	ASSERT_VALID(this);

	if (!pos.IsValid ())
	{
		return "";
	}

	TXTB &txtb = m_rgTokenLine[pos.m_nIndex]->m_rgtxtb[pos.m_nToken];
	CString strLine = m_rgTokenLine[pos.m_nIndex]->m_strLine;

	return strLine.Mid(txtb.ibTokMin, txtb.ibTokMac - txtb.ibTokMin);
}

#ifdef _DEBUG

void CTokenStream::AssertValid( ) const
{
}

void CTokenStream::Dump(CDumpContext &dc) const
{
	CObject::Dump(dc);
}

#endif
