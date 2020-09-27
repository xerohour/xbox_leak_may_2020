///////////////////////////////////////////////////////////////////////////////
//	autorec.cpp
//											 
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "autorec.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CRecordingAutomationIDETest, CDebugTestSet, "Macro Recording", -1, CAutomationSubSuite)
					   
											 
void CRecordingAutomationIDETest::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CRecordingAutomationIDETest::Run(void)
	
	{
	/******************
	 * INITIALIZATION *
	 ******************/
	if(!InitProject("automate", PROJECT_EXE, fBuildOnly))
		{
		m_pLog->RecordInfo("ERROR: could not init automate project");
		return;
		}

	/*********
	 * TESTS *
	 *********/
	// TODO(michma): use cafe macro class (COMacro?) when available.
	GetSubSuite()->LoadMacroFile();
	GetSubSuite()->RunMacro("MacroRecordingTest");
	// TODO(michma): use cafe output class (COOutput?) when available.
	GetSubSuite()->WriteOutputWindowToLog();
	} 
	     

