///////////////////////////////////////////////////////////////////////////////
//	RegistersPP.CPP
//											 
//	Created by:			
//		dverma
//
//	Description:								 
//		VC6 Processor Pack testcases.		 

#include "stdafx.h"
#include "RegistersPP.h"	
#include <stdlib.h>

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(/*999,*/ CRegistersPP, CDbgTestBase, "Processor Pack", -1, CRegistersSubSuite)

// each function that runs a test has this prototype.
typedef void (CRegistersPP::*PTR_TEST_FUNC)(void); 

// Information about a test to be put in the test header
#define LOGTESTHEADER(strTestName) 	m_strLogInfo.Format("# - %d.  %s. Total # - %d, Passed - %d %%", ++m_TestNumber, strTestName, m_TotalNumberOfTests, (m_TestNumber * 100) / m_TotalNumberOfTests);  \
									LogTestHeader(m_strLogInfo);

// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.
PTR_TEST_FUNC tests[] = 
{
	&CRegistersPP::VerifyUnsupportedRegistersNotPresent,
	&CRegistersPP::TestMMXreg,
	//&CRegistersPP::Test3dNowReg,
	&CRegistersPP::TestXMMreg,
	//&CRegistersPP::TestWNIreg
};

											 
void CRegistersPP::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}


void CRegistersPP::Run()

{	 
	if (!CreateXboxProjectFromSource("Registers PP", "src\\Registers PP",
				"Registers PP.cpp"))
			return;
	
	// all tests expect the ide to be in this initial state.
	EXPECT_TRUE(dbg.StepOver());

	// indexes into the test function array.
	int iTest;
	m_TestNumber = 0;
	m_TotalNumberOfTests = sizeof(tests) / sizeof(PTR_TEST_FUNC);
	
	for(iTest = 0; iTest < m_TotalNumberOfTests; iTest++)
		(this->*(tests[iTest]))();
}	

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

void CRegistersPP::testRegVal(char *varname, char *vartype, char *value, char *message)
{
	EXPR_INFO expr_info;

	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, vartype, varname, value);		
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

}

void CRegistersPP::VerifyRegArray(CString Arr[],int Num_items)
{
	EXPECT_TRUE(regs.Enable());

	for (int count = 0; count< Num_items; count++)
		EXPECT_FALSE(regs.RegisterDisplayed(Arr[count]));	

	EXPECT_TRUE(regs.Disable());
}

void CRegistersPP::VerifyUnsupportedRegistersNotPresent(void)
{
	CString MMXijRegs[] = {	"MM00","MM10","MM20","MM30","MM40","MM50","MM60","MM70",
							"MM01","MM11","MM21","MM31","MM41","MM51","MM61","MM71"};

	CString XMMRegs[] = {"XMM0","XMM1","XMM2","XMM3","XMM4","XMM5","XMM6","XMM7"};
	CString XMMijRegs[] = {	"XMM00","XMM01","XMM02","XMM03", "XMM10","XMM11","XMM12","XMM13",
							"XMM20","XMM21","XMM22","XMM23", "XMM30","XMM31","XMM32","XMM33",
							"XMM40","XMM41","XMM42","XMM43", "XMM50","XMM51","XMM52","XMM53",
							"XMM60","XMM61","XMM62","XMM63", "XMM70","XMM71","XMM72","XMM73" };

	CString XMM0DRegs[] = {	"XMM0DL","XMM0DH","XMM1DL","XMM1DH","XMM2DL","XMM2DH",
							"XMM3DL","XMM3DH","XMM4DL","XMM4DH","XMM5DL","XMM5DH",
							"XMM6DL","XMM6DH","XMM7DL","XMM7DH" };

	//	3dnow system
	if (CMDLINE->GetBooleanValue("k6",FALSE)||CMDLINE->GetBooleanValue("k7",FALSE))
	{
		VerifyRegArray(XMMRegs,8);
		VerifyRegArray(XMMijRegs,32);
		VerifyRegArray(XMM0DRegs,16);
	}

	//	P3 system
	if (CMDLINE->GetBooleanValue("kni",FALSE)||CMDLINE->GetBooleanValue("wni",FALSE))
	{
		VerifyRegArray(MMXijRegs,16);
	}

	//	KNI systems do not have XMMiD regs
	if (CMDLINE->GetBooleanValue("kni",FALSE))
	{
		VerifyRegArray(XMM0DRegs,16);
	}
}

void CRegistersPP::TestMMXreg(void)
{
	//	MMX switch has already been checked for in Run()
	
	LOGTESTHEADER("TestMMXreg");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	MST.DoKeys("{ESC}");
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("TestMMX();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	CString MMXRegs[] = {"MM0","MM1","MM2","MM3","MM4","MM5","MM6","MM7"};
	CString MMXRegValues[] = {	"1111111111111111",
								"2222222222222222",
								"3333333333333333",
								"4444444444444444",
								"5555555555555555",
								"6666666666666666",
								"7777777777777777",
								"8888888888888888"};

	int num_reg = 8;
	int count = 0;

	//	step through source code
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(dbg.StepOver());
		EXPECT_TRUE(dbg.StepOver());
	}

	//	do one stepover
	//xbox if (!bSystemIsWin9X)
	EXPECT_TRUE(dbg.StepOver());

	//	now verify register values	
	EXPECT_TRUE(regs.Enable());

	for (count = 0; count< num_reg; count++)
	{
		//	test in reg window
		EXPECT_TRUE(regs.RegisterValueIs(MMXRegs[count], "" , (count+1)*100, FALSE));	
	}
	
	
	EXPECT_TRUE(cxx.Enable(EE_ALL_INFO));
	
	for (count = 0; count< num_reg; count++)
	{
		//	test in watch window
		char str[32];
		char strReg[8];
		strcpy(strReg,MMXRegs[count]);
		itoa((count+1)*100,str,10);

		testRegVal(strReg, "unsigned __int64", str,"Test MMX in Watch - Code");
	}

	//	modify register MMi in reg window
	EXPECT_TRUE(regs.Enable());
	
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.SetRegister(MMXRegs[count], MMXRegValues[count] , 0, TRUE));
	}
	
	//	do one stepover
	EXPECT_TRUE(regs.Disable());
	
	//xbox if (!bSystemIsWin9X)
	EXPECT_TRUE(dbg.StepOver());

	//	now verify register values

	EXPECT_TRUE(regs.Enable());	
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(MMXRegs[count], MMXRegValues[count] , 0, TRUE));
	}

	EXPECT_TRUE(cxx.Enable(EE_ALL_INFO));
	EXPECT_TRUE(dbg.ToggleHexDisplay(TRUE));
	for (count = 0; count< num_reg; count++)
	{
		//	test in watch window
		char str[32];
		char strReg[8];
		strcpy(strReg,MMXRegs[count]);
		strcpy(str,"0x");
		strcat(str,MMXRegValues[count]);
		testRegVal(strReg, "unsigned __int64", str,"Test MMX in Watch - Reg Window");
	}
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));
}

/*
void CRegistersPP::Test3dNowReg(void)
{
	if (!(CMDLINE->GetBooleanValue("k6",FALSE)||CMDLINE->GetBooleanValue("k7",FALSE))) 
		return;	//	exit if 3dnow switch is not set	
	
	LOGTESTHEADER("Test3dNowReg");

	//	Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	MST.DoKeys("{ESC}");
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("Test3dNow();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());	
	
	CString MMXRegs[] = {"MM0","MM1","MM2","MM3","MM4","MM5","MM6","MM7"};

	CString MMXijRegs[] = {	"MM00","MM10","MM20","MM30","MM40","MM50","MM60","MM70",
							"MM01","MM11","MM21","MM31","MM41","MM51","MM61","MM71"};

	CString MMXijRegValues[] = {	"+1.11111E+001","+2.22222E+001","+3.33333E+001","+4.44444E+001","+5.55555E+001",
									"+6.66666E+001","+7.77777E+001","+8.88888E+001",
									"-1.11111E-001","-2.22222E-001","-3.33333E-001","-4.44444E-001","-5.55555E-001",
									"-6.66666E-001","-7.77777E-001","-8.88888E-001"};


	CString MMXRegVal[] = {	"BDE38E2A4131C711","BE638E2A41B1C711","BEAAAA9F4205554D","BEE38E2A4231C711",
							"BF0E38DA425E38D5","BF2AAA9F4285554D","BF471C65429B8E2F","BF638E2A42B1C711" };

	int num_reg = 8;
	int count = 0;

	//	step through source code
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(dbg.StepOver());
		EXPECT_TRUE(dbg.StepOver());
	}

	//	do one stepover
	if (!bSystemIsWin9X)
	EXPECT_TRUE(dbg.StepOver());

	//	now verify register values
	CString MMXijStepOverValues[] = {	"+1.00000E+000","+3.00000E+000","+5.00000E+000","+7.00000E+000","+9.00000E+000",
										"+1.10000E+001","+1.30000E+001","+1.50000E+001","+2.00000E+000","+4.00000E+000",
										"+6.00000E+000","+8.00000E+000","+1.00000E+001","+1.20000E+001","+1.40000E+001",
										"+1.60000E+001"	};
	
	//	test in reg window
	EXPECT_TRUE(regs.Enable());
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(MMXijRegs[count], MMXijStepOverValues[count], 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(MMXijRegs[count+num_reg], MMXijStepOverValues[count+num_reg], 0, TRUE));	
	}

	//	test in watch window
	EXPECT_TRUE(cxx.Enable(EE_ALL_INFO));
	for (count = 0; count< num_reg; count++)
	{
		char str[32];
		char strReg[8];
		float f = 0.0;
	
		strcpy(strReg,MMXijRegs[count]);		
		f = (float) atof(MMXijStepOverValues[count]);
		
		if (f < 10)
			sprintf(str,"%6.5f",f);
		else
			sprintf(str,"%6.4f",f);
		
		testRegVal(strReg, "float", str,"Test MMXij in Watch - Code");
		
		strcpy(strReg,MMXijRegs[count+num_reg]);		
		f = (float) atof(MMXijStepOverValues[count+num_reg]);

		if (f < 10)
			sprintf(str,"%6.5f",f);
		else
			sprintf(str,"%6.4f",f);
		
		testRegVal(strReg, "float", str,"Test MMXij in Watch - Code");
	}



	EXPECT_TRUE(regs.Enable());

	//	initialize MMi to set MMij correctly
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.SetRegister(MMXRegs[count],"B727C5AC47C35000" , 0, TRUE));
	}

	//	verify that MMij changed correctly
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(MMXijRegs[count], "+1.00000E+005", 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(MMXijRegs[count+num_reg], "-1.00000E-005", 0, TRUE));
	}

	//	modify register MMi0 & MMi1 in reg window
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.SetRegister(MMXijRegs[count], MMXijRegValues[count], 0, TRUE));
		EXPECT_TRUE(regs.SetRegister(MMXijRegs[count+num_reg], MMXijRegValues[count+num_reg], 0, TRUE));
	}

	EXPECT_TRUE(regs.Disable());

	//	do one stepover
	if (!bSystemIsWin9X)
	EXPECT_TRUE(dbg.StepOver());

	//	verify MMij values

	//	test in reg window
	EXPECT_TRUE(regs.Enable());
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(MMXijRegs[count], MMXijRegValues[count], 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(MMXijRegs[count+num_reg], MMXijRegValues[count+num_reg], 0, TRUE));	
	}

	//	test in watch window
	EXPECT_TRUE(cxx.Enable(EE_ALL_INFO));
	for (count = 0; count< num_reg; count++)
	{
		char str[32];
		char strReg[8];
		float f = 0.0;
	
		strcpy(strReg,MMXijRegs[count]);		
		f = (float) atof(MMXijRegValues[count]);
		sprintf(str,"%6.4f",f);
		testRegVal(strReg, "float", str,"Test MMXij in Watch - Code");
		
		strcpy(strReg,MMXijRegs[count+num_reg]);		
		f = (float) atof(MMXijRegValues[count+num_reg]);
		sprintf(str,"%f",f);
		testRegVal(strReg, "float", str,"Test MMXij in Watch - Code");
	}

	//	verify that MMi changed correctly
	EXPECT_TRUE(regs.Enable());
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(MMXRegs[count], MMXRegVal[count], 0, TRUE));
	}

}
*/

void CRegistersPP::TestXMMreg(void)
{
	if (!(CMDLINE->GetBooleanValue("kni",FALSE)||CMDLINE->GetBooleanValue("wni",FALSE))) 
		return;	//	exit if P3 switch is not set	

	LOGTESTHEADER("TestXMMreg");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	MST.DoKeys("{ESC}");
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("TestXMM();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	CString XMMRegs[] = {"XMM0","XMM1","XMM2","XMM3","XMM4","XMM5","XMM6","XMM7"};
	CString XMMRegValues[] = {	"11111111111111111111111111111111",
								"22222222222222222222222222222222",
								"33333333333333333333333333333333",
								"44444444444444444444444444444444",
								"55555555555555555555555555555555",
								"66666666666666666666666666666666",
								"77777777777777777777777777777777",
								"88888888888888888888888888888888"};

	CString XMMijRegs[] = {	"XMM00","XMM01","XMM02","XMM03", "XMM10","XMM11","XMM12","XMM13",
							"XMM20","XMM21","XMM22","XMM23", "XMM30","XMM31","XMM32","XMM33",
							"XMM40","XMM41","XMM42","XMM43", "XMM50","XMM51","XMM52","XMM53",
							"XMM60","XMM61","XMM62","XMM63", "XMM70","XMM71","XMM72","XMM73" };

	CString XMMijRegValues[] = {	"+1.00000E+001","+1.00001E+001","+1.00011E+001","+1.00111E+001",
									"+2.00000E+001","+2.00002E+001","+2.00022E+001","+2.00222E+001",
									"+3.00000E+001","+3.00003E+001","+3.00033E+001","+3.00333E+001",
									"+4.00000E+001","+4.00004E+001","+4.00044E+001","+4.00444E+001",	
									"+5.00000E+001","+5.00005E+001","+5.00055E+001","+5.00555E+001",	
									"+6.00000E+001","+6.00006E+001","+6.00066E+001","+6.00666E+001",	
									"+7.00000E+001","+7.00007E+001","+7.00077E+001","+7.00777E+001",	
									"+8.00000E+001","+8.00008E+001","+8.00088E+001","+8.00888E+001"	};

	int num_reg = 8;
	int count = 0;

	CString XMMRegTestVal[] = {	
							"4080000040400000400000003F800000",		//	4,3,2,1
							"4100000040E0000040C0000040A00000",		//	8,7,6,5
							"41400000413000004120000041100000",		//	12,11,10,9
							"41800000417000004160000041500000",		//	16,15,14,13
							"41A00000419800004190000041880000",		//	20,19,18,17
							"41C0000041B8000041B0000041A80000",		//	24,23,22,21
							"41E0000041D8000041D0000041C80000",		//	28,27,26,25
							"4200000041F8000041F0000041E80000"		//	32,31,30,29
								};

	//	step through source code
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(dbg.StepOver());
	}

	//	do one stepover
	//xbox if (!bSystemIsWin9X)
	EXPECT_TRUE(dbg.StepOver());

	//	now verify XMMi register values
	EXPECT_TRUE(regs.Enable());
	
	for (count = 0; count< num_reg; count++)
	{		
		EXPECT_TRUE(regs.RegisterValueIs(XMMRegs[count], XMMRegTestVal[count] ,0 , TRUE));
	}

	//	verify XMMij
	CString XMMijStepOverValues[] = {	
									"+1.00000E+000","+2.00000E+000","+3.00000E+000","+4.00000E+000",
									"+5.00000E+000","+6.00000E+000","+7.00000E+000","+8.00000E+000",
									"+9.00000E+000","+1.00000E+001","+1.10000E+001","+1.20000E+001",
									"+1.30000E+001","+1.40000E+001","+1.50000E+001","+1.60000E+001",	
									"+1.70000E+001","+1.80000E+001","+1.90000E+001","+2.00000E+001",	
									"+2.10000E+001","+2.20000E+001","+2.30000E+001","+2.40000E+001",	
									"+2.50000E+001","+2.60000E+001","+2.70000E+001","+2.80000E+001",	
									"+2.90000E+001","+3.00000E+001","+3.10000E+001","+3.20000E+001"	
									};

	//	test in reg window
	EXPECT_TRUE(regs.Enable());
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4], XMMijStepOverValues[count*4] , 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 1], XMMijStepOverValues[count*4 + 1] , 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 2], XMMijStepOverValues[count*4 + 2] , 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 3], XMMijStepOverValues[count*4 + 3] , 0, TRUE));
	}

	//	test in watch window
	EXPECT_TRUE(cxx.Enable(EE_ALL_INFO));
	for (count = 0; count< num_reg; count++)
	{
		char str[32];
		char strReg[8];
		float f = 0.0;
	
		strcpy(strReg,XMMijRegs[count*4]);		
		f = (float) atof(XMMijStepOverValues[count*4]);

		if (f < 10)
			sprintf(str,"%6.5f",f);
		else
			sprintf(str,"%6.4f",f);

		testRegVal(strReg, "float", str,"Test XMMij in Watch - Code");
		
		strcpy(strReg,XMMijRegs[count*4 + 1]);		
		f = (float) atof(XMMijStepOverValues[count*4 + 1]);
		
		if (f < 10)
			sprintf(str,"%6.5f",f);
		else
			sprintf(str,"%6.4f",f);
		
		testRegVal(strReg, "float", str,"Test XMMij in Watch - Code");

		strcpy(strReg,XMMijRegs[count*4+ 2]);		
		f = (float) atof(XMMijStepOverValues[count*4 + 2]);
		
		if (f < 10)
			sprintf(str,"%6.5f",f);
		else
			sprintf(str,"%6.4f",f);
		
		testRegVal(strReg, "float", str,"Test XMMij in Watch - Code");

		strcpy(strReg,XMMijRegs[count*4+ 3]);		
		f = (float) atof(XMMijStepOverValues[count*4 + 3]);
		
		if (f < 10)
			sprintf(str,"%6.5f",f);
		else
			sprintf(str,"%6.4f",f);

		testRegVal(strReg, "float", str,"Test XMMij in Watch - Code");
	}
	
	//	modify register XMMi in reg window
	EXPECT_TRUE(regs.Enable());
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.SetRegister(XMMRegs[count], XMMRegValues[count] , 0, TRUE));
	}

	//	do one stepover
	EXPECT_TRUE(regs.Disable());

	//xbox if (!bSystemIsWin9X)
	EXPECT_TRUE(dbg.StepOver());

	//	now verify XMMi register values
	EXPECT_TRUE(regs.Enable());
	
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMMRegs[count], XMMRegValues[count] , 0 , TRUE));
	}

	//	initialize XMMi to set XMMij correctly
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.SetRegister(XMMRegs[count], "47C3500047C3500047C3500047C35000" , 0, TRUE));
	}

	//	verify that XMMij changed correctly
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4], "+1.00000E+005", 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 1], "+1.00000E+005", 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 2], "+1.00000E+005", 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 3], "+1.00000E+005", 0, TRUE));
	}

	//	modify register XMMij in reg window
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.SetRegister(XMMijRegs[count*4], XMMijRegValues[count*4] , 0, TRUE));
		EXPECT_TRUE(regs.SetRegister(XMMijRegs[count*4 + 1], XMMijRegValues[count*4 + 1] , 0, TRUE));
		EXPECT_TRUE(regs.SetRegister(XMMijRegs[count*4 + 2], XMMijRegValues[count*4 + 2] , 0, TRUE));
		EXPECT_TRUE(regs.SetRegister(XMMijRegs[count*4 + 3], XMMijRegValues[count*4 + 3] , 0, TRUE));
	}

	//	do one stepover
	EXPECT_TRUE(regs.Disable());

	//xbox if (!bSystemIsWin9X)
	EXPECT_TRUE(dbg.StepOver());

	//	verify XMMij values

	//	test in reg window
	EXPECT_TRUE(regs.Enable());
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4], XMMijRegValues[count*4] , 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 1], XMMijRegValues[count*4 + 1] , 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 2], XMMijRegValues[count*4 + 2] , 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMMijRegs[count*4 + 3], XMMijRegValues[count*4 + 3] , 0, TRUE));
	}

	//	test in watch window
	EXPECT_TRUE(cxx.Enable(EE_ALL_INFO));
	for (count = 0; count< num_reg; count++)
	{
		char str[32];
		char strReg[8];
		float f = 0.0;
	
		strcpy(strReg,XMMijRegs[count*4]);		
		f = (float) atof(XMMijRegValues[count*4]);
		sprintf(str,"%6.4f",f);
		testRegVal(strReg, "float", str,"Test XMMij in Watch - Code");
		
		strcpy(strReg,XMMijRegs[count*4 + 1]);		
		f = (float) atof(XMMijRegValues[count*4 + 1]);
		sprintf(str,"%6.4f",f);
		testRegVal(strReg, "float", str,"Test XMMij in Watch - Code");

		strcpy(strReg,XMMijRegs[count*4+ 2]);		
		f = (float) atof(XMMijRegValues[count*4 + 2]);
		sprintf(str,"%6.4f",f);
		testRegVal(strReg, "float", str,"Test XMMij in Watch - Code");

		strcpy(strReg,XMMijRegs[count*4+ 3]);		
		f = (float) atof(XMMijRegValues[count*4 + 3]);
		sprintf(str,"%6.4f",f);
		testRegVal(strReg, "float", str,"Test XMMij in Watch - Code");
	}

	//	verify that XMMi changed correctly

	CString XMMNewRegVal[] = {	
							"41202D77412004814120006941200000",
							"41A02D7741A0048141A0006941A00000",
							"41F0443341F006C241F0009D41F00000",
							"42202D77422004814220006942200000",
							"424838D5424805A24248008342480000",
							"42704433427006C24270009D42700000",
							"428C27C8428C03F1428C005C428C0000",
							"42A02D7742A0048142A0006942A00000"
								};	

	EXPECT_TRUE(regs.Enable());
	
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMMRegs[count], XMMNewRegVal[count], 0, TRUE));
	}

}

/*
void CRegistersPP::TestWNIreg(void)
{
	if (!CMDLINE->GetBooleanValue("wni",FALSE)) return;	//	exit if wni switch is not set	

	LOGTESTHEADER("TestWNIreg");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	MST.DoKeys("{ESC}");
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("TestXMM();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());


	CString XMMRegs[] = {"XMM0","XMM1","XMM2","XMM3","XMM4","XMM5","XMM6","XMM7"};
	CString XMMRegValues[] = {	"40000000000000003FF0000000000000",
								"40100000000000004008000000000000",
								"40180000000000004014000000000000",
								"4020000000000000401C000000000000",
								"40240000000000004022000000000000",
								"40280000000000004026000000000000",
								"402C000000000000402A000000000000",
								"4030000000000000402E000000000000"};

	CString XMM0DRegs[] = {	"XMM0DL","XMM0DH","XMM1DL","XMM1DH","XMM2DL","XMM2DH",
							"XMM3DL","XMM3DH","XMM4DL","XMM4DH","XMM5DL","XMM5DH",
							"XMM6DL","XMM6DH","XMM7DL","XMM7DH" };

	CString XMM0DRegValues[] = {	"+1.00000000000000E+000","+2.00000000000000E+000","+3.00000000000000E+000",
									"+4.00000000000000E+000","+5.00000000000000E+000","+6.00000000000000E+000",
									"+7.00000000000000E+000","+8.00000000000000E+000","+9.00000000000000E+000",
									"+1.00000000000000E+001","+1.10000000000000E+001","+1.20000000000000E+001",
									"+1.30000000000000E+001","+1.40000000000000E+001","+1.50000000000000E+001",
									"+1.60000000000000E+001"	};

	CString XMM0IRegs[] = {	"XMM0IL","XMM0IH","XMM1IL","XMM1IH","XMM2IL","XMM2IH",
							"XMM3IL","XMM3IH","XMM4IL","XMM4IH","XMM5IL","XMM5IH",
							"XMM6IL","XMM6IH","XMM7IL","XMM7IH" };

	CString XMM0IRegValues[] = {	"4607182418800017408","4611686018427387904","4613937818241073152",
									"4616189618054758400","4617315517961601024","4618441417868443648",
									"4619567317775286272","4620693217682128896","4621256167635550208",
									"4621819117588971520","4622382067542392832","4622945017495814144",
									"4623507967449235456","4624070917402656768","4624633867356078080",
									"4625196817309499392"	};


	int num_reg = 8;
	int count = 0;


	//	step through source code
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(dbg.StepOver());
	}

	//	do one stepover
	if (!bSystemIsWin9X)
	EXPECT_TRUE(dbg.StepOver());

	//	now verify XMMiD register values
	
	CString XMMiDStepOverValues[] = {	
										"+2.00000047311187E+000","+5.12000122547150E+002",
										"+8.19200197219849E+003","+1.31072031677246E+005",
										"+5.24288127075195E+005","+2.09715250927734E+006",
										"+8.38861004101563E+006","+3.35544401796875E+007",
										"+6.71088803828125E+007","+1.34217760796875E+008",
										"+2.68435521656250E+008","+5.36871043437500E+008",
										"+1.07374208712500E+009","+2.14748417475000E+009",
										"+4.29496835050000E+009","+8.58993670300000E+009"
									};

	//	test in reg window
	EXPECT_TRUE(regs.Enable());
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMM0DRegs[count*2], XMMiDStepOverValues[count*2] , 0, TRUE));
		EXPECT_TRUE(regs.RegisterValueIs(XMM0DRegs[count*2 + 1], XMMiDStepOverValues[count*2 + 1] , 0, TRUE));
	}


	//	verify XMMiI register values
	CString XMMiIStepOverValues[] = {	
										"4611686019492741120","4647714816524288000",
										"4665729215040061440","4683743613553737728",
										"4692750812811624448","4701758012068462592",
										"4710765211325300736","4719772410582138880",
										"4724276010211082240","4728779609839501312",
										"4733283209467920384","4737786809096339456",
										"4742290408724758528","4746794008353177600",
										"4751297607981596672","4755801207610015744"
									};


	//	test in watch window
	EXPECT_TRUE(cxx.Enable(EE_ALL_INFO));
	for (count = 0; count< num_reg; count++)
	{
		char str[32];
		char strReg[8];
	
		strcpy(strReg,XMM0IRegs[count*2]);		
		strcpy(str,XMMiIStepOverValues[count*2]);
		testRegVal(strReg, "unsigned __int64", str,"Test XMMiI in Watch - Code");
		
		strcpy(strReg,XMM0IRegs[count*2 + 1]);		
		strcpy(str,XMMiIStepOverValues[count*2 + 1]);
		testRegVal(strReg, "unsigned __int64", str,"Test XMMiI in Watch - Code");
	}


	//
	//	verify that modifying XMMi changes XMM0D & XMM0I
	//
	EXPECT_TRUE(regs.Enable());
	
	//	modify register XMMi in reg window
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.SetRegister(XMMRegs[count], XMMRegValues[count] , 0, TRUE));
	}

	EXPECT_TRUE(cxx.Enable(EE_ALL_INFO));
	for (count = 0; count< num_reg; count++)
	{
		char str[32];
		char strReg[8];
		double dbl = 0.0;
		char **strptr = NULL;
	
		//	verify XMM0D

		strcpy(strReg,XMM0DRegs[count*2]);		
		strcpy(str,XMM0DRegValues[count*2]);		
		dbl = strtod(str,strptr);
		if (dbl < 10)
			sprintf(str,"%.13f",dbl);
		else
			sprintf(str,"%.12f",dbl);
		testRegVal(strReg, "double", str,"Test XMMD in Watch - Code");
		
		strcpy(strReg,XMM0DRegs[count*2 + 1]);		
		strcpy(str,XMM0DRegValues[count*2 + 1]);		
		dbl = strtod(str,strptr);
		if (dbl < 10)
			sprintf(str,"%.13f",dbl);
		else
			sprintf(str,"%.12f",dbl);
		testRegVal(strReg, "double", str,"Test XMMD in Watch - Code");

		//	verify XMM0I

		strcpy(strReg,XMM0IRegs[count*2]);		
		strcpy(str,XMM0IRegValues[count*2]);		
		testRegVal(strReg, "unsigned __int64", str,"Test XMMI in Watch - Code");

		strcpy(strReg,XMM0IRegs[count*2 + 1]);		
		strcpy(str,XMM0IRegValues[count*2 + 1]);		
		testRegVal(strReg, "unsigned __int64", str,"Test XMMI in Watch - Code");
	}
	
	//	verify that modifying XMM0D changes XMMi

	//	XMMD values to test XMM with
	CString XMM0D_TestValues[] = {	"+5.00000000000000E+001","+5.10000000000000E+001","+5.20000000000000E+001",
									"+5.30000000000000E+001","+5.40000000000000E+001","+5.50000000000000E+001",
									"+5.60000000000000E+001","+5.70000000000000E+001","+5.80000000000000E+001",
									"+5.90000000000000E+001","+6.00000000000000E+001","+6.10000000000000E+001",
									"+6.20000000000000E+001","+6.30000000000000E+001","+6.40000000000000E+001",
									"+6.50000000000000E+001"	};

	//	corresponding XMM values
	CString XMM_D_TestValues[] = {	"40498000000000004049000000000000",
									"404A800000000000404A000000000000",
									"404B800000000000404B000000000000",
									"404C800000000000404C000000000000",
									"404D800000000000404D000000000000",
									"404E800000000000404E000000000000",
									"404F800000000000404F000000000000",
									"40504000000000004050000000000000"};
	
	EXPECT_TRUE(regs.Enable());
	
	//	modify register XMM0D in reg window
	for (count = 0; count< num_reg*2; count++)
	{
		EXPECT_TRUE(regs.SetRegister(XMM0DRegs[count], XMM0D_TestValues[count] , 0, TRUE));
	}

	//	now verify XMMi register values
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMMRegs[count], XMM_D_TestValues[count] , 0, TRUE));
	}	
	
	//	verify that modifying XMM0I changes XMMi



	EXPECT_TRUE(regs.Enable());
	
	//	modify register XMM0I in watch window
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));

	for (count = 0; count <num_reg*2; count++)
	{
		EXPECT_TRUE(cxx.SetExpressionValue(XMM0IRegs[count],(count+1)*101));
	}

	//	cooresponding XMMi reg values
	CString XMM_I_TestValues[] = {	"00000000000000CA0000000000000065",
									"0000000000000194000000000000012F",
									"000000000000025E00000000000001F9",
									"000000000000032800000000000002C3",
									"00000000000003F2000000000000038D",
									"00000000000004BC0000000000000457",
									"00000000000005860000000000000521",
									"000000000000065000000000000005EB"};


	//	now verify XMMi register values
	for (count = 0; count< num_reg; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMMRegs[count], XMM_I_TestValues[count] , 0 , TRUE));
	}	
		
	//	verify that XMM0D modifies correctly in reg & watch window
	//	modify in watch
	cxx.Enable(EE_ALL_INFO);

	//	set value in watch window
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));
	for (count = 0; count <num_reg*2; count++)
	{
		EXPECT_TRUE(cxx.SetExpressionValue(XMM0DRegs[count],count*11));
	}
	
	//	corresponding values in reg window

	CString XMM0DWatchValues[] = {	"+0.00000000000000E+000","+1.10000000000000E+001","+2.20000000000000E+001",
									"+3.30000000000000E+001","+4.40000000000000E+001","+5.50000000000000E+001",
									"+6.60000000000000E+001","+7.70000000000000E+001","+8.80000000000000E+001",
									"+9.90000000000000E+001","+1.10000000000000E+002","+1.21000000000000E+002",
									"+1.32000000000000E+002","+1.43000000000000E+002","+1.54000000000000E+002",
									"+1.65000000000000E+002"	};

	//	verify in reg
	EXPECT_TRUE(regs.Enable());
	
	for (count = 0; count< num_reg*2; count++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(XMM0DRegs[count], XMM0DWatchValues[count] , 0 , TRUE));
	}
}
*/