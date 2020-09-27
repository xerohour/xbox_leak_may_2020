///////////////////////////////////////////////////////////////////////////////
//	autodbg.cpp
//											 
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "autodbg.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CDebuggerAutomationIDETest, CDebugTestSet, "Debugger Object", -1, CAutomationSubSuite)
					   
											 
void CDebuggerAutomationIDETest::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CDebuggerAutomationIDETest::Run(void)
	
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
	GetSubSuite()->RunMacro("DebuggerObjectTest");
	// TODO(michma): use cafe output class (COOutput?) when available.
	GetSubSuite()->WriteOutputWindowToLog();
	} 
	     

