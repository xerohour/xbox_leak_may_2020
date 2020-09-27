///////////////////////////////////////////////////////////////////////////////
//	opt1.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CBuildOptionsTest1 class

#include "stdafx.h"
#include "opt1.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBuildOptionsTest1, CProjectTestSet, "Doing....", -1, CBuildOptionsSubSuite)

void CBuildOptionsTest1::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CBuildOptionsTest1

void CBuildOptionsTest1::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( Test1() );
	XSAFETY;

}


BOOL CBuildOptionsTest1::Test1( void )
{
	LogTestHeader("Test1");

// Write the test

	return TRUE;
}


