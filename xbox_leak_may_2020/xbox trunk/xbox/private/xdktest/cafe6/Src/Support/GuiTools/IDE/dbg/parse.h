///////////////////////////////////////////////////////////////////////////////
//	PARSE.H
//
//	Created by :			Date :
//		BrianCr				09/24/93
//
//	Description :
//		Declaration of the token classes and parse function
//
//

#ifndef __PARSE_H__
#define __PARSE_H__

#include "dbgxprt.h"

// parsing support

// BEGIN_CLASS_HELP
// ClassName: CToken
// BaseClass: CObject
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS CToken: public CObject {
public:
	CToken(void);
	CToken(CString, CString);
	~CToken();

	CToken &operator=(CToken &);

	CString GetToken(void);
	CString GetLeadingSeps(void);

	void SetToken(CString);
	void SetLeadingSeps(CString);

private:
	CString m_strToken;
	CString m_strLeadingSeps;
};


// BEGIN_CLASS_HELP
// ClassName: CTokenList
// BaseClass: CObList
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS CTokenList: public CObList {
public:
	CTokenList();
	~CTokenList();

	//CToken *&GetHead(void);
	CToken *GetHead(void);

	//CToken *&GetTail(void);
	CToken *GetTail(void);

	POSITION AddHead(CToken *);
	// void AddHead(CTokenList *);

	POSITION AddTail(CToken *);
	// void AddTail(CTokenList *);

//	CToken *&GetNext(POSITION &);
	CToken *GetNext(POSITION &);

//	CToken *&GetPrev(POSITION &);
	CToken *GetPrev(POSITION &);

	//CToken *&GetAt(POSITION);
	CToken *GetAt(POSITION);

	CString GetTokenString(int);

	void RemoveAll(void);
};


void Parse(CString, CString, CTokenList &);

#endif // __PARSE_H__
