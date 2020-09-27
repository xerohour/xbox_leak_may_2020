///////////////////////////////////////////////////////////////////////////////
//	BRZOutln.CPP
//
//	Created by :			Date :
//		TomSe					1/31/94
//
//	Description :
//		Implementation of the CQueries class
//

#include "stdafx.h"
#include "sniff.h"
#include "queries.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

static char* m_iDefRef[] = {
	"m_i (variable)",
	"A::m_i",
	"B::m_i",
	"MI::m_i",
	NULL,
};

static char* cppOutline[] = {
	"A (class)",
	"B (class)",
	"FOOMAX (macro)",
	"FOOTYPE (typedef)",
	"MI::InlineFoo(void)",
	"A::m_i",
	"B::m_i",
	"MI::m_i",
	"int  MI::m_si",	// Why are two spaces generated?
	"MI (class)",
	NULL,
};

static char* MIMatches[] = {
	"MI (class)",
	NULL,
};

static char* MIBranches[] = {
	"A (class)",
	"B (class)",
	NULL,
};

static char* MIMembers[] = {
	"MI::InlineFoo(void)",
	"MI::StaticFoo(class MI,class MI)",
	"MI::vfoo2(void)",
	"MI::m_i",
	"int  MI::m_si",
	NULL,
};

static char* AMatches[] = {
	"A (class)",
	NULL,
};

static char* ABranches[] = {
	"MI (class)",
	NULL,
};

static char* AMembers[] = {
	"A::~A(void)",
	"A::vfoo1(void)",
	"A::vfoo2(void)",
	"A::m_i",
	NULL,
};

static char* vfoo2Matches1[] = {
	"A::vfoo2(void)",
	"B::vfoo2(void)",
	"MI::vfoo2(void)",
	NULL,
};

static char* vfoo2Branches1[] = {
	NULL,
};

static char* vfoo2Matches2[] = {
	"A::vfoo2(void)",
	"B::vfoo2(void)",
	"MI::vfoo2(void)",
	NULL,
};

static char* vfoo2Branches2[] = {
	"Bfoos(class B &)",
	NULL,
};

static char* cpptestMatches[] = {
	"cpptest(void)",
	NULL,
};

static char* cpptestBranches[] = {
	"Afoos(class A &)",
	"Bfoos(class B &)",
	NULL,
};

static char* addtailMatches[] = {
	"addtail (function)",
	NULL,
};

static char* addtailBranches[] = {
	"adddata (function)",
	"addtail (function)",
	NULL
};

static LPCSTR szTestName = "Queries";

static QueryInfo qryTests[] = {
	{	szTestName, "Definitions and References", DefRef, "m_i", m_iDefRef, NULL, NULL },
	{	szTestName, "File Outline", FileOutline, "cpptest.h", cppOutline, NULL, NULL },
	{	szTestName, "Base Class and Members", BaseClass,	"MI", MIMatches, MIBranches, MIMembers },
	{	szTestName, "Derived Class and Members", DerivedClass,	"A", AMatches, ABranches, AMembers },
	{	szTestName, "Call Graph w/ambiguity", CallGraph,	"vfoo2", vfoo2Matches1, vfoo2Branches1, NULL, 0, 2 },	// Should be B::vfoo2
	{	szTestName, "Caller Graph w/ambiguity", CallerGraph,	"vfoo2", vfoo2Matches2, vfoo2Branches2, NULL, 0, 2 },	// Should be B::vfoo2
	{	szTestName, "Call Graph", CallGraph,	"cpptest", cpptestMatches, cpptestBranches, NULL },
	{	szTestName, "Caller Graph w/recursive function", CallerGraph,	"addtail", addtailMatches, addtailBranches, NULL },	// Recursion
};

const int numTests = ( sizeof( qryTests ) ) / sizeof( QueryInfo );

IMPLEMENT_TEST(CQueries, CTest, "Query Tests", -1, CSniffDriver)

// since this class can also be used as a base class for tests, we need to
// provide the appropriate ctor for the derived class to call
CQueries::CQueries(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CTest(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}

void CQueries::Run(void)
{
	COWorkSpace ws;
	if(GetSubSuite()->InitProject() == ERROR_SUCCESS)
		{
		for( int i = 0; i < numTests; i++ )
			TestQuery( qryTests[i] );
		}
	else
		m_pLog->RecordFailure("src\\browse.mak failed to open or build");
    ws.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Outln

//
//
BOOL CQueries::TestQuery( const QueryInfo& test) {
	BOOL allpass = TRUE;
	BOOL result;
	CStringArray strings;
	int index;

	result = brz.GetMatches( strings, test.symbolname, test.type, test.filters );
	for( index=0; index < strings.GetSize() && result; index++ ) {
		if( test.matches[index] == NULL ) {
			m_pLog->RecordInfo( "CQueries::TestQuery - test.matches[%d] == NULL", index );
			result = FALSE;
			break;
		}
		else {
			if( strings[index] != test.matches[index] ) {
				m_pLog->RecordInfo( "CQueries::TestQuery - %s != %s",(LPCTSTR)strings[index], test.matches[index]  );
				result = FALSE;
				break;
			}
		}
	}
	if( result && test.matches[ strings.GetSize() ] != NULL ) {
		m_pLog->RecordInfo( "CQueries::TestQuery - test.matches has more data that wasn't compared" );
		result = FALSE;
	}

	allpass = allpass && result;

	if( test.type != FileOutline && test.type != DefRef ) {
		EXPECT( test.branches );
		result = brz.GetFirstLevelBranches( strings, test.symbolname, test.type, test.filters, test.amb-1 );

		for( index=0; index < strings.GetSize() && result; index++ ) {
			if( test.branches[index] == NULL ) {
				m_pLog->RecordInfo( "CQueries::TestQuery - test.branches[%d] == NULL", index );
				result = FALSE;
				break;
			}
			else {
				if( strings[index] != test.branches[index] ) {
					m_pLog->RecordInfo( "CQueries::TestQuery - %s != %s",(LPCTSTR)strings[index], test.branches[index]  );
					result = FALSE;
					break;
				}
			}
		}
		if( result && test.branches[ strings.GetSize() ] != NULL ) {
			m_pLog->RecordInfo( "CQueries::TestQuery - test.branches has more data that wasn't compared" );
			result = FALSE;
		}

		allpass = allpass && result;
	}
	
	if( test.type == BaseClass || test.type == DerivedClass ) {
		EXPECT( test.members );
		result = brz.GetMembers( strings, test.symbolname, test.type, test.filters, test.amb-1 );
		for( index=0; index < strings.GetSize() && result; index++ ) {
			if( test.members[index] == NULL ) {
				m_pLog->RecordInfo( "CQueries::TestQuery - test.members[%d] == NULL", index );
				result = FALSE;
				break;
			}
			else {
				if( strings[index] != test.members[index] ) {
					m_pLog->RecordInfo( "CQueries::TestQuery - %s != %s",(LPCTSTR)strings[index], test.members[index]  );
					result = FALSE;
					break;
				}
			}
		}
		if( result && test.members[ strings.GetSize() ] != NULL ) {
			m_pLog->RecordInfo( "CQueries::TestQuery - test.members has more data that wasn't compared" );
			result = FALSE;
		}

		allpass = allpass && result;
	}

	if (!allpass) {
		m_pLog->RecordFailure(test.desc);
		return FALSE;
	}
	return TRUE;
}


