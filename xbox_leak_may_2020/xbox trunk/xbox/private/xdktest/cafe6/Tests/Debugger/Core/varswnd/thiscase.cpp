///////////////////////////////////////////////////////////////////////////////
//	THISCASE.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script.	 

#include "stdafx.h"
#include "thiscase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
										   
IMPLEMENT_TEST(CThisPaneIDETest, CDbgTestBase, "This Pane", -1, CVarsWndSubSuite)

												 
void CThisPaneIDETest::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CThisPaneIDETest::Run(void)
	
	{
	/******************
	 * INITIALIZATION *
	 ******************/
	if(!InitProject("thispane\\thispane"))
		{
		m_pLog->RecordInfo("ERROR: could not init thispane project");
		return;
		}

	// step passed prolog of WinMain().
	// this is the initial state each test expects.
	if(!dbg.StepOver(2))
		{
		m_pLog->RecordInfo("ERROR: could not step passed WinMain()'s prolog");
		return;
		}

	/*********
	 * TESTS *
	 *********/
	} 
