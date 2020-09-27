///////////////////////////////////////////////////////////////////////////////
//	testcase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "testcase.h"					 	 
											   
#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS
												 
IMPLEMENT_GET_NAME(TestTestSet, "TestWizard Generated script", NULL)
IMPLEMENT_GET_NUM_TESTS(TestTestSet, 1, 1)

TestTestSet::TestTestSet()
{
}

int TestTestSet::Run(void* pData /*=NULL*/)
{
BEGIN_TESTSET();


TestCase1( );

return EndTestSet();	// returns how many failed in this test set
}


 

BOOL TestTestSet::TestCase1( )
{
	COProject  proje;
	proje.New( szProjName, ProjType );
	proje.GetErrorCount( );
	proje.Save( );
	WriteWarning("");
	 
	return(TRUE);
}




