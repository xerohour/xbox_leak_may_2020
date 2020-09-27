///////////////////////////////////////////////////////////////////////////////
//	bpcase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "bpcase2.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(Cbp2IDETest, CDbgTestBase, "Breakpoints DLLs", -1, CbpSubSuite)
												 
void Cbp2IDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void Cbp2IDETest::Run()
{
	XSAFETY;

	if (SetProject("dllapp\\incdll", PROJECT_DLL, fBuildOnly) && SetProject("dllapp\\powdll", PROJECT_DLL, fBuildOnly)
		&& SetProject("dllapp\\dec2", PROJECT_DLL, fBuildOnly) && SetProject("dllapp\\dllappx"))
	{	
		XSAFETY;
		// when makefile is wrapped, wrapper name is dllappx1.mak and exe for debug session 
		// becomes dllappx1.exe, so we must	change it back since we actually build dllappx.exe.
		prj.SetExeForDebugSession("dllappx.exe");

		// dllappx builds 4 files: incdll.dll, dec2.dll, powdll.dll and dllappx.exe.
		// dllappx.exe is copied to remote by SetProject, but we need to do dlls.
		COConnection *pconnec = GetSubSuite()->GetIDE()->GetConnectionInfo();
		if(pconnec->GetUserConnections() != CONNECTION_LOCAL)
		{
			prj.UpdateRemoteFile("src\\dllapp\\incdll.dll");
			prj.UpdateRemoteFile("src\\dllapp\\dec2.dll");
			prj.UpdateRemoteFile("src\\dllapp\\powdll.dll");
		}

		StartDebugging();

		EXPECT_TRUE(BreakDLLInit());
		XSAFETY;
		EXPECT_TRUE(BPTableDLL());	   //bug#14945	postponed: one test is disabled
		XSAFETY;
		// TODO: WinslowF - Load additianal dlls does not work at this time. Need to rewrite the func.
		//	EXPECT_TRUE(LoadDLLBreak());
		//	XSAFETY;
		
		StopDbgCloseProject();
	}

	else
		m_pLog->RecordFailure("Could not initiate incdll, powdll, dec2, or dllappx projects.");
}
