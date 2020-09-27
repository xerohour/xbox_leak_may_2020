///////////////////////////////////////////////////////////////////////////////
//	DISASMPP.CPP
//											 
//	Created by:			
//		dverma
//
//	Description:								 
//		VC6 Processor Pack testcases.		 

#include "stdafx.h"
#include "DisAsmPP.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(/*999,*/ CDisAsmPP, CDbgTestBase, "Processor Pack", -1, CDisAsmSubSuite)

// each function that runs a test has this prototype.
typedef void (CDisAsmPP::*PTR_TEST_FUNC)(void); 

// Information about a test to be put in the test header
#define LOGTESTHEADER(strTestName) 	m_strLogInfo.Format("# - %d.  %s. Total # - %d, Passed - %d %%", ++m_TestNumber, strTestName, m_TotalNumberOfTests, (m_TestNumber * 100) / m_TotalNumberOfTests);  \
									LogTestHeader(m_strLogInfo);

// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.
PTR_TEST_FUNC tests[] = 
{
	&CDisAsmPP::TestMMX,
	&CDisAsmPP::Test3dNow,
	&CDisAsmPP::Test3dNowEnhanced,
	&CDisAsmPP::TestKatmai,
	&CDisAsmPP::TestWNI
};

												 
void CDisAsmPP::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDisAsmPP::Run()

{	 
	char bPPFlagsPresent = 0;

	//	test if any pp switches present
	if (CMDLINE->GetBooleanValue("MMX",FALSE))  bPPFlagsPresent = 1;
	if (CMDLINE->GetBooleanValue("k6",FALSE))	bPPFlagsPresent = 1;
	if (CMDLINE->GetBooleanValue("k7",FALSE))	bPPFlagsPresent = 1;
	if (CMDLINE->GetBooleanValue("kni",FALSE))  bPPFlagsPresent = 1;
	if (CMDLINE->GetBooleanValue("wni",FALSE))  bPPFlagsPresent = 1;

	if (!bPPFlagsPresent) 
	{
			LOG->RecordInfo("No Processor Pack Flags Specified");
			return;
	}

	/******************
	 * INITIALIZATION *
	 ******************/

	// the base name of the localized directories and files we will use.
	if(GetSystem() & SYSTEM_DBCS)
		m_strProjBase = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü DisAsm PP";
	else
		m_strProjBase = "DisAsm PP";

	// the location of the unlocalized sources, under which the projects we will use will be located.
	m_strSrcDir = GetCWD() + "src\\DisAsm PP";
	// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
	m_strExeDir = m_strSrcDir + "\\" + m_strProjBase + " EXE";

	if (!CMDLINE->GetBooleanValue("noclean",FALSE))
	
	{
		// clean-up the exe project from the last run.
		KillFiles(m_strExeDir + "\\Debug\\", "*.*");
		RemoveDirectory(m_strExeDir + "\\Debug");
		KillFiles(m_strExeDir + "\\", "*.*");
		RemoveDirectory(m_strExeDir);

		// set the options for the exe project we are building for this test.
		CProjWizOptions *pWin32AppWizOpt = new(CWin32AppWizOptions);
		pWin32AppWizOpt->m_strLocation = m_strSrcDir;
		pWin32AppWizOpt->m_strName = m_strProjBase + " EXE";	
		
		// create the exe project.
		EXPECT_SUCCESS(prj.New(pWin32AppWizOpt));
		// create a new localized exe source file in the project dir from the unlocalized exe source file.
		CopyFile(m_strSrcDir + "\\" + "DisAsm PP.cpp", m_strExeDir + "\\" + m_strProjBase + " EXE.cpp", FALSE);
		// make it writable so it can be cleaned up later.
		SetFileAttributes(m_strExeDir + "\\" + m_strProjBase + " EXE.cpp", FILE_ATTRIBUTE_NORMAL);
		// add the source to the project.
		EXPECT_SUCCESS(prj.AddFiles(m_strProjBase + " EXE.cpp"));
		// build the project.
		EXPECT_SUCCESS(prj.Build());
	}
	
	else
	{
		EXPECT_SUCCESS(prj.Open(m_strSrcDir + "\\" + m_strProjBase + " EXE\\" + m_strProjBase + " EXE.dsp"));
	}

	// indexes into the test function array.
	int iTest;
	
	// all tests expect the ide to be in this initial state.
	EXPECT_TRUE(dbg.StepOver());
	
	m_TestNumber = 0;
	m_TotalNumberOfTests = sizeof(tests) / sizeof(PTR_TEST_FUNC);

	// run each test once.
	for(iTest = 0; iTest < m_TotalNumberOfTests; iTest++)
		(this->*(tests[iTest]))();
	
	EXPECT_TRUE(dbg.StopDebugging(ASSUME_NORMAL_TERMINATION));
	EXPECT_SUCCESS(prj.Close());
}	

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////


void CDisAsmPP::TestMMX()
{
	//	MMX switch has already been checked for in Run()
	
	LOGTESTHEADER("TestMMX");
	
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("	Test_MMX();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	//	test all registers for visibility
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm1,mm2"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm2,mm3"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm3,mm4"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm4,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm5,mm6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm6,mm7"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm7,mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	shift left
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	mov 32 bit
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd mm0,dword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd dword ptr [ebp-10h],mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mmword ptr [ebp-8],mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd mm0,eax"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd eax,mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Pack with Signed Saturation
	EXPECT_TRUE(dam.VerifyCurrentInstruction("packsswb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packssdw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packsswb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packssdw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Pack with Unsigned Saturation	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("packuswb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packuswb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Add

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Add with Saturation
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Add Unsigned with Saturation 
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Bitwise Logical And 
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pand mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pand mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Bitwise Logical And Not 
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pandn mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pandn mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Bitwise Logical Or
	EXPECT_TRUE(dam.VerifyCurrentInstruction("por mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("por mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Compare for Equal 
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Compare for Greater Than

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Multiply and Add 

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaddwd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaddwd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Multiply High

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Multiply Low

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmullw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmullw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Shift Left Logical

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pslld mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllq mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pslld mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw mm0,7"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pslld mm0,7"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllq mm0,7"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Shift Right Arithmetic

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psraw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrad mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psraw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrad mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psraw mm0,5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrad mm0,5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());	

	//	Packed Shift Right Logical
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrld mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlq mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrld mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlw mm0,6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrld mm0,6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlq mm0,6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Subtract

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Subtract with Saturation

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Subtract Unsigned with Saturation

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubusb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubusb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	Unpack High Packed Data

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhbw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhwd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhdq mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhbw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhwd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhdq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	Unpack Low Packed Data

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklbw mm0,dword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklwd mm0,dword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckldq mm0,dword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklbw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklwd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckldq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Bitwise Logical Exclusive OR

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pxor mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pxor mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

}

void CDisAsmPP::Test3dNow()
{
	if (!(CMDLINE->GetBooleanValue("k6",FALSE)||CMDLINE->GetBooleanValue("k7",FALSE))) 
		return;	//	exit if 3dnow switch is not set	
	
	LOGTESTHEADER("Test3dNow");
	
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("	Test_3dNow();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	//	average of unsigned int bytes
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgusb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgusb mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	float to 32 bit signed int
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pf2id mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pf2id mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	accumulator
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfacc mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfacc mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	add
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfadd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfadd mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	compare for equal
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfcmpeq mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfcmpeq mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	compare for greater than or equal to
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfcmpge mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfcmpge mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	compare for greater than
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfcmpgt mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfcmpgt mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	maximum
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfmax mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfmax mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	minimum
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfmin mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfmin mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	multiply
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfmul mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfmul mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	reciprocal
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrcp mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrcp mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	reciprocal, reciprocal intermediate step 1, reciprocal intermediate step 2
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrcpit1 mm1,mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrcpit2 mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrcpit1 mm5,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrcpit2 mm5,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	reciprocal sqrt
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrsqrt mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrsqrt mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	reciprocal sqrt, reciprocal sqrt intermediate step 1	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrsqit1 mm1,mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfrsqit1 mm1,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	subtract
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfsub mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfsub mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	reverse subtraction
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfsubr mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfsubr mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	32 bit signed int to float
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pi2fd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pi2fd mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	multiply high
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhrw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhrw mm0,mm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dbg.StepOver());

	//	load a processor cache into data cache
	EXPECT_TRUE(dam.VerifyCurrentInstruction("prefetch [ebp-9]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	same, sets MES state to modified
	EXPECT_TRUE(dam.VerifyCurrentInstruction("prefetchw [ebp-9]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

}

void CDisAsmPP::Test3dNowEnhanced()
{
	if (!CMDLINE->GetBooleanValue("k7",FALSE)) return;		//	exit if k7 switch is not set	
	
	LOGTESTHEADER("Test3dNowEnhanced");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("	Test_3dNowEnhanced();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	//	packed float to int word with sign extend
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pf2iw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pf2iw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed float negative accumulate
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfnacc mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfnacc mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed float mixed +ve -ve accumulate
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfpnacc mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pfpnacc mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed int word to float
	//	somewhat erroneous values
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pi2fw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pi2fw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed swap double word
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pswapd mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pswapd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	streaming store using byte mask

	EXPECT_TRUE(dam.VerifyCurrentInstruction("maskmovq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	streaming store
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movntq mmword ptr [ebp-8],mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
			
	//	packed avg of unsigned byte	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgb mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed avg of unsigned word	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	extract word into int register
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pextrw eax,mm0,1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	insert word from int register
	//	TO DO : this test has been altered to pass because of POSTPONED VS98 bug 65288

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pinsrw mm0,ax,1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pinsrw mm0,word ptr [ebp-0Eh],1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	packed max signed word
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaxsw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaxsw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed max unsigned byte
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaxub mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaxub mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	packed min signed word
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pminsw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pminsw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed min unsigned byte	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pminub mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pminub mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	move mask to integer register
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmovmskb eax,mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed multiply high unsigned word
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhuw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhuw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	prefetch non-temporal access
	EXPECT_TRUE(dam.VerifyCurrentInstruction("prefetchnta [ebp-9]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	prefetch to all cache levels
	EXPECT_TRUE(dam.VerifyCurrentInstruction("prefetcht0 [ebp-9]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	prefetch to all cache levels except 0
	EXPECT_TRUE(dam.VerifyCurrentInstruction("prefetcht1 [ebp-9]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	prefetch to all cache levels except 0 & 1
	EXPECT_TRUE(dam.VerifyCurrentInstruction("prefetcht2 [ebp-9]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	packed sum of absolute byte differences
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psadbw mm0,mmword ptr [ebp-8]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psadbw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	packed shuffle word
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pshufw mm0,mmword ptr [ebp-8],0E4h"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pshufw mm0,mm1,0E1h"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	// store fence
	EXPECT_TRUE(dam.VerifyCurrentInstruction("sfence"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
}

void CDisAsmPP::TestKatmai()
{
	if (!(CMDLINE->GetBooleanValue("kni",FALSE)||CMDLINE->GetBooleanValue("wni",FALSE))) 
		return;	//	exit if P3 switch is not set	

	LOGTESTHEADER("TestKatmai");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("	Test_Katmai();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	//	test all registers for visibility
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm1,xmm2"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm2,xmm3"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm3,xmm4"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm4,xmm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm5,xmm6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm6,xmm7"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm7,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());


	EXPECT_TRUE(dam.VerifyCurrentInstruction("addps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("addps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("addss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("addss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andnps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andnps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	cmpps series

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpleps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnleps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());


	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpleps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnleps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	cmpss series

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpless xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnless xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpless xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnless xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());


	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpleps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpless xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnleps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnless xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("comiss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("comiss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2ps xmm0,mmword ptr [GlobalInt64 (0042d080)]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2ps xmm0,mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pi mm0,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pi mm0,mmword ptr [GlobalInt64 (0042d080)]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsi2ss xmm0,eax"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsi2ss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtss2si eax,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtss2si eax,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2pi mm0,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2pi mm0,mmword ptr [GlobalInt64 (0042d080)]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttss2si eax,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttss2si eax,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("divps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("divps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("divss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("divss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("minps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("minps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("minss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("minss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movaps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movaps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movaps xmmword ptr [ebp-20h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movhlps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movhps xmm0,qword ptr [GlobalInt64 (0042d080)]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movhps qword ptr [GlobalInt64 (0042d080)],xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movlps xmm0,qword ptr [GlobalInt64 (0042d080)]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movlps qword ptr [GlobalInt64 (0042d080)],xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movlhps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movmskps eax,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movntps xmmword ptr [ebp-20h],xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss dword ptr [ebp-24h],xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movups xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movups xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movups xmmword ptr [ebp-20h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("orps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("orps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("rcpps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("rcpps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("rcpss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("rcpss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("rsqrtps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("rsqrtps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("rsqrtss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("rsqrtss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("sfence"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("shufps xmm0,xmm1,72h"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("shufps xmm0,xmmword ptr [ebp-20h],73h"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("stmxcsr dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("ldmxcsr dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("subps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("subps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("subss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("subss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("ucomiss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("ucomiss xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpckhps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpckhps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpcklps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpcklps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("xorps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("xorps xmm0,xmmword ptr [ebp-20h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

}

void CDisAsmPP::TestWNI()
{
	if (!CMDLINE->GetBooleanValue("wni",FALSE)) return;	//	exit if WNI switch is not set	

	LOGTESTHEADER("TestWNI");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("	Test_WNI();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("emms"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	test all registers for visibility
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm2,xmm3"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm4,xmm5"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm6,xmm7"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm1,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm3,xmm2"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm5,xmm4"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm7,xmm6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());


	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("addpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("addsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("addsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andnpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andnpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("andpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmplepd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmplepd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnlepd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnlepd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmplesd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmplesd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnlesd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnlesd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("comisd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("comisd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtdq2pd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtdq2pd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpd2pi mm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpd2pi mm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpd2dq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpd2dq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpd2ps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpd2ps xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2pd xmm0,mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2pd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsd2si eax,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsd2si eax,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsd2ss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsd2ss xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsi2sd xmm0,eax"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsi2sd xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtss2sd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtss2sd xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttpd2pi mm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttpd2pi mm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttpd2dq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttpd2dq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttsd2si eax,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttsd2si eax,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("divpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("divpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("divsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("divsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("minpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("minpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("minsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("minsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movapd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movapd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movapd xmmword ptr [ebp-10h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movhpd xmm0,qword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movhpd qword ptr [ebp-18h],xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movlpd xmm0,qword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movlpd qword ptr [ebp-18h],xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movmskpd eax,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movsd mmword ptr [ebp-18h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movupd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movupd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movupd xmmword ptr [ebp-10h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("orpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("orpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("shufpd xmm0,xmm1,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("shufpd xmm0,xmmword ptr [ebp-10h],8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("subpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("subpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("subsd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("subsd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("ucomisd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("ucomisd xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpckhpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpckhpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpcklpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpcklpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("xorpd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("xorpd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtdq2ps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtdq2ps xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2dq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2dq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2dq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2dq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2ps xmm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2ps xmm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pi mm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pi mm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2pi mm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2pi mm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd xmm0,eax"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd xmm0,dword ptr [ebp-24h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd eax,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd dword ptr [ebp-24h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movdqa xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movdqa xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movdqa xmmword ptr [ebp-10h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movdqu xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movdqu xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movdqu xmmword ptr [ebp-10h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	//	TO DO : MUST TEST with VCPP build > 8806.
//	EXPECT_TRUE(dam.VerifyCurrentInstruction("movdq2q mm0,xmm1"));
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movdq2q"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq2dq xmm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq xmm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mmword ptr [ebp-18h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packsswb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packsswb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("packssdw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packssdw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("packuswb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packuswb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddq mm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pand xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pand xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pandn xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pandn xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pavgw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pextrw eax,xmm1,6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pinsrw xmm0,word ptr [ebp-1Eh],6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	TO DO : this test has been altered to pass because of POSTPONED VS98 bug 65288
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pinsrw xmm0,ax,6"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaddwd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaddwd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaxsw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaxsw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaxub xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaxub xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pminsw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pminsw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pminub xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pminub xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmovmskb eax,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhuw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhuw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmullw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmullw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmuludq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmuludq mm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmuludq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmuludq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("por xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("por xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psadbw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psadbw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pshufd xmm0,xmm1,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pshufd xmm0,xmmword ptr [ebp-10h],8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pshufhw xmm0,xmm1,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pshufhw xmm0,xmmword ptr [ebp-10h],8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pshuflw xmm0,xmm1,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pshuflw xmm0,xmmword ptr [ebp-10h],8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pslldq xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pslld xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pslld xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pslld xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllq xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psraw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psraw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psraw xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrad xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrad xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrad xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrldq xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlw xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrld xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrld xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrld xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlq xmm0,8"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubq mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubq mm0,mmword ptr [ebp-18h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubusb xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubusb xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubusw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubusw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhbw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhbw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhwd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhwd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhdq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhdq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhqdq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhqdq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklbw xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklbw xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklwd xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklwd xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckldq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckldq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklqdq xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklqdq xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pxor xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pxor xmm0,xmmword ptr [ebp-10h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("clflush [ebp-19h]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("clflush [eax]"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("lfence"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("maskmovdqu xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
		
	EXPECT_TRUE(dam.VerifyCurrentInstruction("mfence"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movntpd xmmword ptr [ebp-10h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movntdq xmmword ptr [ebp-10h],xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movnti dword ptr [ebp-24h],eax"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pause"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

}


