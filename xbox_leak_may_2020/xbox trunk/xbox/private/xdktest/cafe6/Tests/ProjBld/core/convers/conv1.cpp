///////////////////////////////////////////////////////////////////////////////
//	conv1.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CConversionTest1 class

#include "stdafx.h"
#include "conv1.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CConversionTest1, CProjectTestSet, "Doing....", -1, CConversionSubSuite)

void CConversionTest1::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CSuite1Test

void CConversionTest1::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( Test1() );
	XSAFETY;

}


BOOL CConversionTest1::Test1( void )
{
	LogTestHeader("Test1");

// Write the test

	return TRUE;
}


