///////////////////////////////////////////////////////////////////////////////
//	PARSE.CPP
//
//	Created by :			Date :
//		BrianCr				09/24/93
//
//	Description :
//		Implementation of the token classes and parse function
//
//

#include "stdafx.h"
#include "parse.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// CToken implementation
CToken::CToken(void)
: m_strToken(""), m_strLeadingSeps("")
{
	NULL;
}

CToken::CToken(CString strToken, CString strLeadingSeps)
: m_strToken(strToken), m_strLeadingSeps(strLeadingSeps)
{
	NULL;
}

CToken::~CToken()
{
	NULL;
}

CToken &CToken::operator=(CToken &token)
{
	m_strToken = token.m_strToken;
	m_strLeadingSeps = token.m_strLeadingSeps;

	return (*this);
}

CString CToken::GetToken(void)
{
	return (m_strToken);
}

CString CToken::GetLeadingSeps(void)
{
	return (m_strLeadingSeps);
}

void CToken::SetToken(CString strToken)
{
	m_strToken = strToken;
}

void CToken::SetLeadingSeps(CString strLeadingSeps)
{
	m_strLeadingSeps = strLeadingSeps;
}


// CTokenList implementation
CTokenList::CTokenList(void)
: CObList()
{
	NULL;
}

CTokenList::~CTokenList()
{
	RemoveAll();
}

/*CToken *&CTokenList::GetHead(void)
{
	return ((CToken *&)CObList::GetHead());
} */

CToken *CTokenList::GetHead(void)
{
	return ((CToken *)CObList::GetHead());
}

/*CToken *&CTokenList::GetTail(void)
{
	return ((CToken *)CObList::GetTail());
} */

CToken *CTokenList::GetTail(void)
{
	return ((CToken *)CObList::GetTail());
}

POSITION CTokenList::AddHead(CToken *pToken)
{
	return (CObList::AddHead((CObject *)pToken));
}

POSITION CTokenList::AddTail(CToken *pToken)
{
	return (CObList::AddTail((CObject *)pToken));
}

/*CToken *&CTokenList::GetNext(POSITION &pos)
{
	return ((CToken *&)CObList::GetNext(pos));
} */

CToken *CTokenList::GetNext(POSITION &pos)
{
	return ((CToken *)CObList::GetNext(pos));
}

/*CToken *&CTokenList::GetPrev(POSITION &pos)
{
	return ((CToken *&)CObList::GetPrev(pos));
} */

CToken *CTokenList::GetPrev(POSITION &pos)
{
	return ((CToken *)CObList::GetPrev(pos));
}

/*CToken *&CTokenList::GetAt(POSITION pos)
{
	return ((CToken *&)CObList::GetAt(pos));
} */

CToken *CTokenList::GetAt(POSITION pos)
{
	return ((CToken *)CObList::GetAt(pos));
}

CString CTokenList::GetTokenString(int nIndex)
{
	return ((GetAt(FindIndex(nIndex)))->GetToken());
}

void CTokenList::RemoveAll(void)
{
	POSITION pos, nextpos;

	for (nextpos = pos = GetHeadPosition(); pos != NULL; pos = nextpos) {
		delete GetNext(nextpos);
		RemoveAt(pos);
	}
}


void Parse(CString strParse, CString strSeps, CTokenList &listToken)
{
	CToken *pToken;
	CString strToken, strLeadingSeps;
	int i, nBeginToken, nEndToken;
	BOOL bDone;

	// if the string to parse is empty, we're done
	if (strParse.IsEmpty())
		return;
	// otherwise, get the first token and call this recursively
	else {
		// strip off preceeding separators into strLeadingSeps
		for (i = 0, bDone = FALSE; !bDone; ) {
			if (i >= strParse.GetLength()) {
				bDone = TRUE;
			}
			else {
				if (strSeps.FindOneOf(CString(strParse[i], 1)) != -1) {
					strLeadingSeps += strParse[i++];
				}
				else {
					bDone = TRUE;
				}
			}
		}
		// strip off token into strToken
		nBeginToken = i;
		// if nBeginToken is past the end of the string, there's no token
		if (nBeginToken <= strParse.GetLength()) {
		// end of string is not a separator; if no separator is found, the
			// end of the token is the end of the string
			if ((nEndToken = (strParse.Mid(nBeginToken)).FindOneOf(strSeps)) == -1) {
				nEndToken = strParse.GetLength();
			}
			// convert relative (to nBeginToken) position to absolute position
			else {
				nEndToken += nBeginToken;
			}
			// extract the token
			strToken = strParse.Mid(nBeginToken, nEndToken - nBeginToken);
		}
		// create a new CToken
		pToken = new CToken(strToken, strLeadingSeps);
		// add this token to the end of the token list
		listToken.AddTail(pToken);
		// call Parse minus the stripped token
		Parse(strParse.Mid(nEndToken), strSeps, listToken);
	}
}

