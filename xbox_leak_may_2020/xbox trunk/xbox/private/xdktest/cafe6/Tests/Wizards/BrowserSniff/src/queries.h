///////////////////////////////////////////////////////////////////////////////
//	QUERIES.H
//
//	Created by :			Date :
//		TomSe					1/31/94
//
//	Description :
//		Declaration of the CQueries class
//

#ifndef __QUERIES_H__
#define __QUERIES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

struct QueryInfo {
	LPCSTR name;
	LPCSTR desc;
	QueryType type;
	LPCSTR	symbolname;
	char** matches;
	char** branches;	// Only 1st level for class or function trees.
	char** members;	// Only for class queries.
	int filters;
	int amb;
};

///////////////////////////////////////////////////////////////////////////////
//	BrzTestOutln class

class CQueries : public CTest
{
	DECLARE_TEST(CQueries, CSniffDriver)

// ctor needed since this class also operates as a base class
public:
	CQueries(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

// Operations
public:
	virtual void Run(void);

// Data
protected:
	COFile   file;
	COBrowse brz;
	COSource src;


// Test Outln
protected:
	BOOL TestQuery(const QueryInfo&);
};

#endif //__QUERIES_H__
