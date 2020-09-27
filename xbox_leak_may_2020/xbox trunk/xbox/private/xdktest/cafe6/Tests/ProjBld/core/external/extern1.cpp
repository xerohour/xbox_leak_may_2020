///////////////////////////////////////////////////////////////////////////////
//	extern1.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CExternalMakefileTest1 class

#include "stdafx.h"
#include "extern1.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CExternalMakefileTest1, CProjectTestSet, "Doing....", -1, CExternalMakefileSubSuite)

void CExternalMakefileTest1::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CExternalMakefileTest1

void CExternalMakefileTest1::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( Test1() );
	XSAFETY;

}


BOOL CExternalMakefileTest1::Test1( void )
{
	LogTestHeader("Test1");

// Write the test

	return TRUE;
}


