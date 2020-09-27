///////////////////////////////////////////////////////////////////////////////
//	subprj1.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CSubprojectsTest1 class

#include "stdafx.h"
#include "subprj1.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CSubprojectsTest1, CProjectTestSet, "Doing....", -1, CSubprojectsSubSuite)

void CSubprojectsTest1::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//CSubprojectsTest1

void CSubprojectsTest1::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( Test1() );
	XSAFETY;

}


BOOL CSubprojectsTest1::Test1( void )
{
	LogTestHeader("Test1");

// Write the test

	return TRUE;
}


