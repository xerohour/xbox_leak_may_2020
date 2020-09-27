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
	&CDisAsmPP::TestKatmai
};

												 
void CDisAsmPP::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDisAsmPP::Run()

{	
	/******************
	 * INITIALIZATION *
	 ******************/

	if (!CreateXboxProjectFromSource("DisAsm PP", "src\\Disasm PP",
				"Disasm PP.cpp"))
			return;

	// all tests expect the ide to be in this initial state.
	if(!XboxDebuggerConnect())
	{
		prj.Close();
		return;
	}
	
	// indexes into the test function array.
	int iTest;
	
	m_TestNumber = 0;
	m_TotalNumberOfTests = sizeof(tests) / sizeof(PTR_TEST_FUNC);

	// run each test once.
	for(iTest = 0; iTest < m_TotalNumberOfTests; iTest++)
	{
		(this->*(tests[iTest]))();
	}
	
	EXPECT_TRUE(dbg.StopDebugging(NOWAIT));
	EXPECT_SUCCESS(prj.Close());
}	

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////


#if 0
#define EBP8	"[ebp-8]"
#define EBP10H	"[ebp-10h]"
#else
#define EBP8	"[count]"
#define EBP10H	"[iVar32]"
#endif

void CDisAsmPP::TestMMX()
{
	//	MMX switch has already been checked for in Run()
	
	LOGTESTHEADER("TestMMX");
	
	// Find the function we want to step into from main.cpp
	EXPECT_TRUE(dbg.Break());
	EXPECT_SUCCESS(src.Open(m_strProjectDir + "\\" + m_strProjectName + ".cpp")); 
	EXPECT_TRUE(src.Find("Test_MMX(); // src.Find()")); 

	EXPECT_TRUE(dbg.StepToCursor());
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
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	mov 32 bit
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd mm0,dword ptr " EBP10H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movd dword ptr " EBP10H ",mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movq mmword ptr " EBP8 ",mm0"));
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
	EXPECT_TRUE(dam.VerifyCurrentInstruction("packsswb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packssdw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packsswb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packssdw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Pack with Unsigned Saturation	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("packuswb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("packuswb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Add

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddd mm0,mmword ptr " EBP8));
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
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddsw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Add Unsigned with Saturation 
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("paddusw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Bitwise Logical And 
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pand mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pand mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Bitwise Logical And Not 
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pandn mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pandn mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Bitwise Logical Or
	EXPECT_TRUE(dam.VerifyCurrentInstruction("por mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("por mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Compare for Equal 
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpeqd mm0,mmword ptr " EBP8));
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

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pcmpgtd mm0,mmword ptr " EBP8));
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

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaddwd mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmaddwd mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Multiply High

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmulhw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Multiply Low

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmullw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pmullw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Shift Left Logical

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pslld mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psllq mm0,mmword ptr " EBP8));
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

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psraw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrad mm0,mmword ptr " EBP8));
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
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrld mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psrlq mm0,mmword ptr " EBP8));
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

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubd mm0,mmword ptr " EBP8));
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

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//	Packed Subtract Unsigned with Saturation

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubusb mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubusb mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("psubsw mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	//	Unpack High Packed Data

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhbw mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhwd mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckhdq mm0,mmword ptr " EBP8));
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

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklbw mm0,dword ptr " EBP10H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpcklwd mm0,dword ptr " EBP10H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("punpckldq mm0,dword ptr " EBP10H));
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

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pxor mm0,mmword ptr " EBP8));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("pxor mm0,mm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dbg.Go(0, 0, 0, WAIT_FOR_RUN));
}

void CDisAsmPP::TestKatmai()
{
	LOGTESTHEADER("TestKatmai");

	EXPECT_TRUE(dbg.Break());
	EXPECT_SUCCESS(src.Open(m_strProjectDir + "\\" + m_strProjectName + ".cpp")); 
	EXPECT_TRUE(src.Find("Test_Katmai(); // src.Find()")); 

	EXPECT_TRUE(dbg.StepToCursor());
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

#if 0
#define EBP20H "[ebp-20h]"
#define EBP24H "[ebp-24h]"
#define GLOBALINT64 "0042d080"
#else
#define EBP20H "[mVar128]"
#define EBP24H "[i32Val]"
//#define GLOBALINT64 "004194f0"
#endif

	EXPECT_TRUE(dam.VerifyCurrentInstruction("addps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("addss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("addss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andnps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andnps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("andps xmm0,xmmword ptr " EBP20H));
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


	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpleps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnleps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordps xmm0,xmmword ptr " EBP20H));
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

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpeqss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpltss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpless xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpunordss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpneqss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnltss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpnless xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cmpordss xmm0,dword ptr " EBP24H));
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
	EXPECT_TRUE(dam.VerifyCurrentInstruction("comiss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2ps xmm0,mmword ptr [GlobalInt64 (" GLOBALINT64 ")]"));
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2ps xmm0,mmword ptr [GlobalInt64 ("));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtpi2ps xmm0,mm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pi mm0,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	//EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pi mm0,mmword ptr [GlobalInt64 (" GLOBALINT64 ")]"));
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtps2pi mm0,mmword ptr [GlobalInt64 ("));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsi2ss xmm0,eax"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtsi2ss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtss2si eax,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvtss2si eax,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2pi mm0,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	//EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2pi mm0,mmword ptr [GlobalInt64 (" GLOBALINT64 ")]"));
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttps2pi mm0,mmword ptr [GlobalInt64 ("));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttss2si eax,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("cvttss2si eax,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("divps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("divps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("divss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("divss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("maxss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("minps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("minps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("minss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("minss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movaps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movaps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movaps xmmword ptr " EBP20H ",xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movhlps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//EXPECT_TRUE(dam.VerifyCurrentInstruction("movhps xmm0,qword ptr [GlobalInt64 (" GLOBALINT64 ")]"));
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movhps xmm0,qword ptr [GlobalInt64 ("));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	//EXPECT_TRUE(dam.VerifyCurrentInstruction("movhps qword ptr [GlobalInt64 (" GLOBALINT64 ")],xmm0"));
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movhps qword ptr [GlobalInt64 ("));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	//EXPECT_TRUE(dam.VerifyCurrentInstruction("movlps xmm0,qword ptr [GlobalInt64 (" GLOBALINT64 ")]"));
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movlps xmm0,qword ptr [GlobalInt64 ("));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	//EXPECT_TRUE(dam.VerifyCurrentInstruction("movlps qword ptr [GlobalInt64 (" GLOBALINT64 ")],xmm0"));
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movlps qword ptr [GlobalInt64 ("));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movlhps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movmskps eax,xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movntps xmmword ptr " EBP20H ",xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss dword ptr " EBP24H ",xmm0"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("movups xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movups xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("movups xmmword ptr " EBP20H ",xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("mulss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("orps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("orps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("rcpps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("rcpps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("rcpss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("rcpss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("rsqrtps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("rsqrtps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("rsqrtss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("rsqrtss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("sfence"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("shufps xmm0,xmm1,72h"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("shufps xmm0,xmmword ptr " EBP20H ",73h"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("sqrtss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("stmxcsr dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("ldmxcsr dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	
	EXPECT_TRUE(dam.VerifyCurrentInstruction("subps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("subps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("subss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("subss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("ucomiss xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("ucomiss xmm0,dword ptr " EBP24H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpckhps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpckhps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpcklps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("unpcklps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dam.VerifyCurrentInstruction("xorps xmm0,xmm1"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.VerifyCurrentInstruction("xorps xmm0,xmmword ptr " EBP20H));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(dbg.Go(0, 0, 0, WAIT_FOR_RUN));
}
