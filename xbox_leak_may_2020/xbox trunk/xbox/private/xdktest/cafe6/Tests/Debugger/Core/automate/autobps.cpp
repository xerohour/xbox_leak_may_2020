///////////////////////////////////////////////////////////////////////////////
//	autobps.cpp
//											 
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "autobps.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CBreakpointsAutomationIDETest, CDebugTestSet, "Breakpoints Collection", -1, CAutomationSubSuite)
					   
											 
void CBreakpointsAutomationIDETest::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CBreakpointsAutomationIDETest::Run(void)
	
	{
	/******************
	 * INITIALIZATION *
	 ******************/
	if(!(GetSubSuite()->fProjectReady))
		{
		if(!InitProject("automate", PROJECT_EXE))
			{
			m_pLog->RecordInfo("ERROR: could not init automate project");
			return;
			}
		GetSubSuite()->CloseWorkspaceAndCreateOptionsFile();
		GetSubSuite()->fProjectReady = TRUE;
		GetSubSuite()->LoadMacroFile();
		}

	/*********
	 * TESTS *
	 *********/
	// TODO(michma): use cafe macro class (COMacro?) when available.
	GetSubSuite()->RunMacro("BreakpointsCollectionTest");
	// TODO(michma): use cafe output class (COOutput?) when available.
	GetSubSuite()->WriteOutputWindowToLog();
	} 
	     

