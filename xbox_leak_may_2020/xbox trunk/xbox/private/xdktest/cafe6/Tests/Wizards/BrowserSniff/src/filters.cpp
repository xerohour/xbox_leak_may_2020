///////////////////////////////////////////////////////////////////////////////
//	FILTERS.CPP
//
//	Created by :			Date :
//		TomSe					1/31/94
//
//	Description :
//		Implementation of the CFilters class
//

#include "stdafx.h"
#include "sniff.h"
#include "filters.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// full info
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

static char* cppClasses[] = {
	"A (class)",
	"B (class)",
	"MI (class)",
	NULL,
};

static char* cppFunctions[] = {
	"MI::InlineFoo(void)",
	NULL,
};

static char* cppData[] = {
	"A::m_i",
	"B::m_i",
	"MI::m_i",
	"int  MI::m_si",	// Why are two spaces generated?
	NULL,
};

static char* cppMacros[] = {
	"FOOMAX (macro)",
	NULL,
};

static char* cppTypes[] = {
	"FOOTYPE (typedef)",
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

static char* MINoBranches[] = {
	NULL,
};

// full info
static char* MIMembers[] = {
	"MI::InlineFoo(void)",
	"MI::StaticFoo(class MI,class MI)",
	"MI::vfoo2(void)",
	"MI::m_i",
	"int  MI::m_si",
	NULL,
};

static char* MIAllFun[] = {
	"MI::InlineFoo(void)",
	"MI::StaticFoo(class MI,class MI)",
	"MI::vfoo2(void)",
	NULL,
};

static char* MIVirtualFn[] = {
	"MI::vfoo2(void)",
	NULL,
};

static char* MIStaticFn[] = {
	"MI::StaticFoo(class MI,class MI)",
	NULL,
};

static char* MINonStatNonVirtFn[] = {
	"MI::InlineFoo(void)",
	NULL,
};

static char* MINonVirtFn[] = {
	"MI::InlineFoo(void)",
	"MI::StaticFoo(class MI,class MI)",
	NULL,
};

static char* MINonStatFn[] = {
	"MI::InlineFoo(void)",
	"MI::vfoo2(void)",
	NULL,
};

static char* MINone[] = {
	NULL,
};

static char* MIAllData[] = {
	"MI::m_i",
	"int  MI::m_si",
	NULL,
};

static char* MIStaticData[] = {
	"int  MI::m_si",
	NULL,
};

static char* MINonStatData[] = {
	"MI::m_i",
	NULL,
};

static LPCSTR szTestName = "Filters";

static QueryInfo qryTests[] = {
	{	szTestName, "FileOutline w/all symbols", FileOutline, "cpptest.h", cppOutline, NULL, NULL },
	{	szTestName, "FileOutline w/Classes", FileOutline, "cpptest.h", cppClasses, NULL, NULL, CLASSES },
	{	szTestName, "FileOutline w/Functions", FileOutline, "cpptest.h", cppFunctions, NULL, NULL, FUNCTIONS },
	{	szTestName, "FileOutline w/Data", FileOutline, "cpptest.h", cppData, NULL, NULL, DATA },
	{	szTestName, "FileOutline w/Macros", FileOutline, "cpptest.h", cppMacros, NULL, NULL, MACROS },
	{	szTestName, "FileOutline w/Types", FileOutline, "cpptest.h", cppTypes, NULL, NULL, TYPES },
	{	szTestName, "Base Class: All", BaseClass,	"MI", MIMatches, MIBranches, MIMembers, AllFNCs|AllData },
	{	szTestName, "Base Class: All Functions", BaseClass,	"MI", MIMatches, MIBranches, MIAllFun, AllFNCs|NoData },
	{	szTestName, "Base Class: Virtual Functions", BaseClass,	"MI", MIMatches, MIBranches, MIVirtualFn, Virtual|NoData },
	{	szTestName, "Base Class: Static Functions", BaseClass,	"MI", MIMatches, MIBranches, MIStaticFn, Static|NoData },
	{	szTestName, "Base Class: Non-virtual Functions", BaseClass,	"MI", MIMatches, MIBranches, MINonVirtFn, NonVirtual|NoData },
	{	szTestName, "Base Class: Non-static Functions", BaseClass,	"MI", MIMatches, MIBranches, MINonStatFn, NonStatic|NoData },
	{	szTestName, "Base Class: Non-virtual Non-static Functions", BaseClass,	"MI", MIMatches, MIBranches, MINonStatNonVirtFn, NSNV|NoData },
	{	szTestName, "Base Class: No Functions, No Data", BaseClass,	"MI", MIMatches, MIBranches, MINone, NoFCNs|NoData },
	{	szTestName, "Base Class: All Data", BaseClass,	"MI", MIMatches, MIBranches, MIAllData, NoFCNs|AllData },
	{	szTestName, "Base Class: Static Data", BaseClass,	"MI", MIMatches, MIBranches, MIStaticData, NoFCNs|StaticData },
	{	szTestName, "Base Class: Non-static Data", BaseClass,	"MI", MIMatches, MIBranches, MINonStatData, NoFCNs|NonStaticData },
#if 0	// Assume that derived tests work if base class work
	{	szTestName, "Derived Class: All", DerivedClass,	"MI", MIMatches, MINoBranches, MIMembers, AllFNCs|AllData },
	{	szTestName, "Derived Class: All Functions", DerivedClass,	"MI", MIMatches, MINoBranches, MIAllFun, AllFNCs|NoData },
	{	szTestName, "Derived Class: Virtual Functions"", DerivedClass,	"MI", MIMatches, MINoBranches, MIVirtualFn, Virtual|NoData },
	{	szTestName, "Derived Class: Static Functions", DerivedClass,	"MI", MIMatches, MINoBranches, MIStaticFn, Static|NoData },
	{	szTestName, "Derived Class: Non-virtual Functions"", DerivedClass,	"MI", MIMatches, MINoBranches, MINonVirtFn, NonVirtual|NoData },
	{	szTestName, "Derived Class: Non-static Functions", DerivedClass,	"MI", MIMatches, MINoBranches, MINonStatFn, NonStatic|NoData },
	{	szTestName, "Derived Class: Non-virtual Non-static Functions", DerivedClass,	"MI", MIMatches, MINoBranches, MINonStatNonVirtFn, NSNV|NoData },
	{	szTestName, "Derived Class: No Functions, No Data", DerivedClass,	"MI", MIMatches, MINoBranches, MINone, NoFCNs|NoData },
	{	szTestName, "Derived Class: All Data", DerivedClass,	"MI", MIMatches, MINoBranches, MIAllData, NoFCNs|AllData },
	{	szTestName, "Derived Class: Static Data", DerivedClass,	"MI", MIMatches, MINoBranches, MIStaticData, NoFCNs|StaticData },
	{	szTestName, "Derived Class: Non-static Data", DerivedClass,	"MI", MIMatches, MINoBranches, MINonStatData, NoFCNs|NonStaticData },
#endif
};

const int numTests = ( sizeof( qryTests ) ) / sizeof( QueryInfo );

IMPLEMENT_TEST(CFilters, CQueries, "Filters Tests", -1, CSniffDriver)

void CFilters::Run(void)
{
	if(GetSubSuite()->InitProject() == ERROR_SUCCESS)
		{
		for( int i = 0; i < numTests; i++ )
			TestQuery( qryTests[i] );
		}
	else
		m_pLog->RecordFailure("src\\browse.mak failed to open or build");
}
