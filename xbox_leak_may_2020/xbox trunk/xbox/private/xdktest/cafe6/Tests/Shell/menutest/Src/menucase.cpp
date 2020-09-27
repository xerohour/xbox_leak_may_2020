///////////////////////////////////////////////////////////////////////////////
//	Menucase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		Cycles Through Menus		 

#include "stdafx.h"
#include "Menucase.h"					 	 
											   
#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMenuTesterIDETest, CTest, NULL, -1, CMenuTesterSubSuite)
												 
#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

void CMenuTesterIDETest::Run(void)
{

TestCase1( );

}


void CMenuTesterIDETest::TestCase1( )
{

	if(GetSubSuite()->brunmenus==FALSE)
	{
		m_pLog->RecordInfo("No menu exercises");
		return;
	}
	m_pLog->RecordInfo("Starting menu exerciser");

	//Put focus on menu
	MST.DoKeys("%F{ESC}");

	//Drop down first menu
	MST.DoKeys("{DOWN}");

	//Cycle through 13 menus 20 times 
	for (int nCycle = 0; nCycle < 260; nCycle++)
		MST.DoKeys("{RIGHT}");

	//Take focus off menu
	MST.DoKeys("{ALT}");

	m_pLog->RecordInfo("Done with menu exerciser");

	return;
}





