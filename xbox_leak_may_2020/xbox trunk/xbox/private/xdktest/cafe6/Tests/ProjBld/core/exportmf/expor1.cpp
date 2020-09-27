///////////////////////////////////////////////////////////////////////////////
//	expor1.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CExportMakefileTest1 class

#include "stdafx.h"
#include "expor1.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CExportMakefileTest1, CProjectTestSet, "Doing....", -1, CExportMakefileSubSuite)

void CExportMakefileTest1::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CExportMakefileTest1

void CExportMakefileTest1::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( Test1() );
	XSAFETY;

}


BOOL CExportMakefileTest1::Test1( void )
{
	LogTestHeader("Test1");

// Write the test

	return TRUE;
}


