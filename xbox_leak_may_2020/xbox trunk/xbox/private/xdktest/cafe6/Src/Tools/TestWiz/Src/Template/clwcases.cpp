///////////////////////////////////////////////////////////////////////////////
//	$$cases_cpp$$.CPP
//											 
//	Created by :			
//		$$TestOwner$$		
//
//	Description :								 
//		$$TestTitle$$		 

#include "stdafx.h"
#include "$$cases_cpp$$.h"					 	 
											   
#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

IMPLEMENT_TEST($$TestClass$$, CTest, "$$TestTitle$$", -1, $$DriverClass$$)
												 
void $$TestClass$$::Run(void)
{
$$TestFunctions$$

}

$$Script_Text$$




