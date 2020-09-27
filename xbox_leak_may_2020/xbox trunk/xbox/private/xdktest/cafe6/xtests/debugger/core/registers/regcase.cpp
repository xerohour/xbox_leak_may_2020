///////////////////////////////////////////////////////////////////////////////
//	Regcase.CPP
//											 
//	Created by :			
//		dklem
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "Regcase.h"	
#include "process.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(/*999,*/ CRegistersIDETest, CDbgTestBase, "General", -1, CRegistersSubSuite)

// each function that runs a test has this prototype.
typedef void (CRegistersIDETest::*PTR_TEST_FUNC)(void); 

// Information about a test to be put in the test header
#define LOGTESTHEADER(strTestName) 	m_strLogInfo.Format("# - %d.  %s. Total # - %d, Passed - %d %%", ++m_TestNumber, strTestName, m_TotalNumberOfTests, (m_TestNumber*100)/m_TotalNumberOfTests);  \
									LogTestHeader(m_strLogInfo);

// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.

PTR_TEST_FUNC tests[] = 
{
	&CRegistersIDETest::VerifyEIP,
	&CRegistersIDETest::EIP_ESP_ChangeAfterStep,
	&CRegistersIDETest::VerifyFlags,
	&CRegistersIDETest::VerifyEAX,
	&CRegistersIDETest::EditRegisters,
	&CRegistersIDETest::ToggleFlags,
	&CRegistersIDETest::EditRegisterViaQuickWatch,
	&CRegistersIDETest::Registers_Dam_Watch_Memory,
	&CRegistersIDETest::Registers_Watch_Memory,
	&CRegistersIDETest::Change_EAX_Changes_RetValue,
	&CRegistersIDETest::DisplayIsAlwaysHex,
	&CRegistersIDETest::IntegrityWithSetFocusToThread,
	&CRegistersIDETest::IntegrityWithFunctionEvaluation,
	&CRegistersIDETest::BoundaryConditionsForEditingValues,
	&CRegistersIDETest::TypeInvalidValues,
	&CRegistersIDETest::FloatingPointException,
	&CRegistersIDETest::VerifyDockingView,
};

											 
void CRegistersIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CRegistersIDETest::Run()
{	 
	/******************
	 * INITIALIZATION *
	 ******************/

	if (!CreateXboxProjectFromSource("Registers", "src\\Registers",
				"Registers.cpp"))
			return;

	// all tests expect the ide to be in this initial state.
	EXPECT_TRUE(dbg.StepOver());
	
	// indexes into the test function array.
	int iTest;
	m_TestNumber = 0;
	m_TotalNumberOfTests = sizeof(tests) / sizeof(PTR_TEST_FUNC);
	// we only want to randomly run each test once.
	RandomNumberCheckList rncl(sizeof(tests) / sizeof(PTR_TEST_FUNC));

	// randomly run each test once.
//	while((iTest = rncl.GetNext()) != -1)
	for(iTest = 0; iTest < sizeof(tests) / sizeof(PTR_TEST_FUNC); iTest++)
		(this->*(tests[iTest]))();

	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_SUCCESS(prj.Close());
}	

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

void CRegistersIDETest::HelperFunctionVerifyEIP(char* szTestCaseName, char* szEIP)
{

	CString csDamEIP;

	EXPECT_TRUE(dam.Enable());
	csDamEIP = dam.GetInstruction();
	csDamEIP = csDamEIP.Left(8);
	EXPECT_TRUE(dam.Disable());

	// Resize and Move
	EXPECT_TRUE(regs.Enable());	
	EXPECT_TRUE(regs.RegisterValueIs(CString("EIP"), csDamEIP, -1));

	// Return EIP if requested
	if(szEIP)
		strcpy(szEIP, LPCTSTR(csDamEIP));
}


void CRegistersIDETest::VerifyEIP(void)
{

	LOGTESTHEADER("VerifyEIP");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());

	HelperFunctionVerifyEIP("VerifyEIP", NULL);
}

void CRegistersIDETest::EIP_ESP_ChangeAfterStep(void)
{
	LOGTESTHEADER("EIP_ESP_ChangeAfterStep");

	char *pszFlags[] = {"EIP ", "ESP "};
	CString csRegValues[2];
	int ii;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());

	// Get initial values
	for(ii = 0; ii < sizeof(pszFlags)/sizeof(*pszFlags); ii++)
		csRegValues[ii] = regs.GetRegister(CString(pszFlags[ii]), NULL);

	// Step Into
	EXPECT_TRUE(dbg.StepInto(1));
	EXPECT_TRUE(regs.Enable());

	// Make sure that values changed
	for(ii = 0; ii < sizeof(pszFlags)/sizeof(*pszFlags); ii++)
		EXPECT_TRUE(!regs.RegisterValueIs(CString(pszFlags[ii]), csRegValues[ii], -1));

}

void CRegistersIDETest::VerifyFlags(void)
{

	LOGTESTHEADER("VerifyFlags");

	char *pszFlags[] = {"OV", "UP", "EI", "PL", "ZR", "AC", "PE", "CY"};
	BOOL bSame, bSame1;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());

	// Verify that all flags are in place and their value are either 0 or 1
	for(int ii = 0; ii < sizeof(pszFlags)/sizeof(*pszFlags); ii++)
	{
		bSame = regs.RegisterValueIs(CString(pszFlags[ii]), CString("0"), -1);
		bSame1 = regs.RegisterValueIs(CString(pszFlags[ii]), CString("1"), -1);
		EXPECT_TRUE(bSame || bSame1);
	}

}

void CRegistersIDETest::VerifyEAX(void)
{

	LOGTESTHEADER("VerifyEAX");
	
	CString csRetValue;
	int nRegValue;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	// Step Over the Foo(7) function
	EXPECT_TRUE(dbg.StepOver());

	// Get return value
	EXPECT_TRUE(ee.GetExpressionValue("res", csRetValue));

	EXPECT_TRUE(regs.Enable());

	// Verify that EAX is correct
	regs.GetRegister(CString("EAX"), &nRegValue);

	int x = atoi(LPCTSTR(csRetValue));

	EXPECT_TRUE(nRegValue == atoi(LPCTSTR(csRetValue)));

}

void CRegistersIDETest::EditRegisters(void)
{

	LOGTESTHEADER("EditRegisters");

	char szNewValue[128];
	CString csRegister;
/*	char szTmpStr[128];
	char *ptr;
	int nStrLen;
	BOOL bSign;
*/	
	char *pszCPURegs[] =		{"EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EIP", "ESP", "EBP", "EFL"};
	char *pszCPUSegments[] = 	{"CS", "DS", "ES", "SS", "FS", "GS" }; 
	// TODO (dklem 11/16/00) EDO was removed from the list because you can edit just low 4 bytes.  BUG # 50809 in VS7
	// TODO(michma - 3/15/00): we can't distinguish yet between normal EIP and float EIP.
	char *pszFloatingInt[] =	{"CTRL", "STAT", "TAGS", /* "EIP", */ "CS", "DS", /*"EDO "*/};
	char *pszFloatingFloat[] =	{"ST0", "ST1", "ST2", "ST3", "ST4", "ST5", "ST6", "ST7"};

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());

	// initialize random numbers generator
	srand((unsigned)time( NULL ));


	m_pLog->RecordInfo("Testing cpu registers.");

	// Verify that all CPU registers and segments are in place and are editable
	for(int ii = 0; ii < sizeof(pszCPURegs)/sizeof(*pszCPURegs); ii++)

	{
		m_pLog->RecordInfo("Testing register %s.", pszCPURegs[ii]);

		// The next for cycle is for Multiple Undo/Redo verification
		for(int jj = 0; jj < gl_constNumberUndoRedo; jj++)
		{
			// Create a new value 
			sprintf(szNewValue, "%.8x", (int) (0xFFFFFFFF * (((float)rand()) / ((float)(RAND_MAX)))));
			strupr(szNewValue);

			// Save value for Multiple Undo/Redo verification
			strcpy(m_pszUndoRedoRegisters[jj], szNewValue);

			// Set value
			EXPECT_TRUE(regs.SetRegister(CString(pszCPURegs[ii]), CString(szNewValue), -1));

			if(0 == strcmp(pszCPURegs[ii], "EIP "))
			{
				EXPECT_TRUE(regs.Enable());
			}

			// Verify value
			EXPECT_TRUE(regs.RegisterValueIs(CString(pszCPURegs[ii]), CString(szNewValue), -1));
		}

		// TODO (dklem 11/13/00) Enable when this function available
		// HelperFunctio_MultipleUndoRedo(pszCPURegs[ii]);
	}

	m_pLog->RecordInfo("Testing CPU segments.");

	// Verify that all CPU segments and segments are in place and are editable
	for(ii = 0; ii < sizeof(pszCPUSegments)/sizeof(*pszCPUSegments); ii++)
	
	{
		m_pLog->RecordInfo("Testing register %s.", pszCPUSegments[ii]);

		// The next for cycle is for Multiple Undo/Redo verification
		for(int jj = 0; jj < gl_constNumberUndoRedo; jj++)
		{
			// Create a new value 
			sprintf(szNewValue, "%.4x", (int) (0xFFFF * (((float)rand()) / ((float)(RAND_MAX)))));
			strupr(szNewValue);

			// Save value for Multiple Undo/Redo verification
			strcpy(m_pszUndoRedoRegisters[jj], szNewValue);

			// Set value
			EXPECT_TRUE(regs.SetRegister(CString(pszCPUSegments[ii]), CString(szNewValue), -1));
			// Verify value
			EXPECT_TRUE(regs.RegisterValueIs(CString(pszCPUSegments[ii]), CString(szNewValue), -1));
		}

		// TODO (dklem 11/13/00) Enable when this function available
		// HelperFunctio_MultipleUndoRedo(pszCPUSegments[ii]);
	}

	m_pLog->RecordInfo("Testing floating point int.");

	for(ii = 0; ii < sizeof(pszFloatingInt)/sizeof(*pszFloatingInt); ii++)

	{
		m_pLog->RecordInfo("Testing register %s.", pszFloatingInt[ii]);

		// The next for cycle is for Multiple Undo/Redo verification
		for(int jj = 0; jj < gl_constNumberUndoRedo; jj++)
		{
			// Create a new value 
			if(!strcmp(pszFloatingInt[ii], "EIP ") || !strcmp(pszFloatingInt[ii], "EDO "))
				sprintf(szNewValue, "%.8x", (int) (0xFFFFFFFF * (((float)rand()) / ((float)(RAND_MAX)))));
			else
				sprintf(szNewValue, "%.4x", (int) (0xFFFF * (((float)rand()) / ((float)(RAND_MAX)))));
			strupr(szNewValue);

			// Save value for Multiple Undo/Redo verification
			strcpy(m_pszUndoRedoRegisters[jj], szNewValue);

			// Set value
			EXPECT_TRUE(regs.SetRegister(CString(pszFloatingInt[ii]), CString(szNewValue), -1));
			// Verify value
			EXPECT_TRUE(regs.RegisterValueIs(CString(pszFloatingInt[ii]), CString(szNewValue), -1));
		}

		// TODO (dklem 11/13/00) Enable when this function available
		// HelperFunctio_MultipleUndoRedo(pszFloatingInt[ii]);
	}

	// Verify that there are no IP and DO in floats
	// TODO: (dklem 10/14/99) Uncomment the following 2 lines when #50800 is fixed. FIXED in 8682 (11/16/99).
	csRegister = regs.GetRegister(CString(" IP "), NULL);
	EXPECT_TRUE(csRegister.IsEmpty());
	csRegister = regs.GetRegister(CString(" DO "), NULL);
	EXPECT_TRUE(csRegister.IsEmpty());

	// DON'T FORGET TO ADD MULTIPLE UNDO/REDO VERIFICATION FOR FLOATING POINTS
	// TODO (dklem 11/16/00) The following piece of code was commented because changing exponent changes mantissa.  BUG #63001 in VS7
/*
	// Don't forget to treat + correctly.  Otherwise it will be interpreted as Shift.
	for(ii = 0; ii < sizeof(pszFloatingFloat)/sizeof(*pszFloatingFloat); ii++)
	{
		csRegister = regs.GetRegister(CString(pszFloatingFloat[ii]), NULL);
		lstrcpyn(szNewValue, LPCTSTR(csRegister), sizeof(szNewValue));

		// Change value
		nStrLen = (int) strlen(szNewValue);
		for(int i = 0; i < nStrLen; i++)
		{
			if('0' <= szNewValue[i] && szNewValue[i] <= '9')
				szNewValue[i] = '0' + (int) (10 * (((float)rand()) / ((float)(RAND_MAX)))); 

			// Choose the sign randomly
			if('+' == szNewValue[i] || szNewValue[i] == '-')
			{
				bSign = (int) ((((float)rand()) / ((float)(RAND_MAX))) + 0.5); 

				// Verify that bSign is 1 and 0
				if(bSign)
					while(0);
				else
					while(0);
				

				// we should put {} around +.  Otherwise it would be treated as Shift
				if(bSign)
				{
					memcpy(szNewValue + i + 3, szNewValue + i + 1, nStrLen);
					strncpy(szNewValue + i, "{+}", 3);
					nStrLen +=2;
					i +=2;
					strcpy(szNewValue + nStrLen - 2, "00");
					*(szNewValue + nStrLen) = '\0';
				}
				else
					szNewValue[i] = '-';
			}


		}

		// Since STx are long doubles whose size is 10 bytes and limits are 1.2E +/- 4932 (19 digits), we put these limits
		if(4931 < atoi(szNewValue + nStrLen - 4))
			strcpy(szNewValue + nStrLen - 4, "4931");

		// Set value
		EXPECT_TRUE(regs.SetRegister(CString(pszFloatingFloat[ii]), CString(szNewValue), -1));

		// Remove {} from szNewValue
		ptr = szNewValue;
		while(NULL != (ptr = strstr(ptr, "{+}")))
		{
			strcpy(szTmpStr, ptr + 3);
			strcpy(ptr, "+");
			strcpy(ptr + 1, szTmpStr);
		}

		// Verify value
		EXPECT_TRUE(regs.RegisterValueIs(CString(pszFloatingFloat[ii]), CString(szNewValue), -1));

	}
*/
	// Test the boundary conditions
	EXPECT_TRUE(regs.SetRegister(CString(pszFloatingFloat[0]), CString("{+}1.00000000000000000e{+}0000"), -1));
	EXPECT_TRUE(regs.SetRegister(CString(pszFloatingFloat[0]), CString("{+}1"), -1));
	EXPECT_TRUE(regs.RegisterValueIs(CString(pszFloatingFloat[0]), CString("+1.00000000000000000e+0000"), -1));

	// TODO (dklem 11/17/00) The following piece was commented because of limits for STx seems to be incorrect.  BUG # 62937 in VS7
/*
	EXPECT_TRUE(regs.SetRegister(CString(pszFloatingFloat[0]), CString("{+}1.19000000000000000e{+}4932"), -1));
	EXPECT_TRUE(regs.RegisterValueIs(CString(pszFloatingFloat[0]), CString("+1.19000000000000000e+4932"), -1));

	EXPECT_TRUE(regs.SetRegister(CString(pszFloatingFloat[0]), CString("{+}1.19000000000000000e-4932"), -1));
	EXPECT_TRUE(regs.RegisterValueIs(CString(pszFloatingFloat[0]), CString("+1.19000000000000000e-4932"), -1));
*/
	EXPECT_TRUE(regs.SetRegister(CString(pszFloatingFloat[0]), CString("{+}1.21000000000000000e{+}4932"), -1));
	EXPECT_TRUE(regs.RegisterValueIs(CString(pszFloatingFloat[0]), CString("1#INF"), -1));

// TODO (dklem 11/16/00) The following piece of code was commented because changing exponent changes mantissa.  BUG #63001 in VS7
/*
	EXPECT_TRUE(regs.SetRegister(CString(pszFloatingFloat[0]), CString("{+}1.21000000000000000e-4932"), -1));
	EXPECT_TRUE(regs.RegisterValueIs(CString(pszFloatingFloat[0]), CString("1#INF"), -1));
*/
	// Should disable (close) disassembly window since changing EIP bings it up
	EXPECT_TRUE(dam.Disable());
}

void CRegistersIDETest::ToggleFlags(void)
{

	LOGTESTHEADER("ToggleFlags");

	int nRegValue;
	char szTmpStr[8];

	char *pszFlags[] = {"OV", "UP", "EI", "PL", "ZR", "AC", "PE", "CY"};


	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());

	// Verify that all flags are in place and their value are either 0 or 1
	for(int ii = 0; ii < sizeof(pszFlags)/sizeof(*pszFlags); ii++)
	{
		// The next for cycle is for Multiple Undo/Redo verification
		for(int jj = 0; jj < gl_constNumberUndoRedo; jj++)
		{
			regs.GetRegister(CString(pszFlags[ii]), &nRegValue);

			// Swap value
			nRegValue = !nRegValue;

			// Save value for Multiple Undo/Redo verification
			strcpy(m_pszUndoRedoRegisters[jj], itoa(nRegValue, szTmpStr, 10));

			// Set value
			EXPECT_TRUE(regs.SetRegister(CString(pszFlags[ii]), CString(""), nRegValue, FALSE /* value is given as an integer */));
			EXPECT_TRUE(regs.RegisterValueIs(CString(pszFlags[ii]), CString(""), nRegValue, FALSE /* value is given as an integer */));
		}

		// TODO (dklem 11/13/00) Enable when this function available
		// HelperFunctio_MultipleUndoRedo(pszFlags[ii]);
	}

}

void CRegistersIDETest::EditRegisterViaQuickWatch(void)
{

	LOGTESTHEADER("EditRegisterViaQuickWatch");

	char *pszRegisters[] = {"EAX", "ESP"};
	char szNewValue[128], szSetValue[128];
	int ii;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());

	// Make sure that values changed
	for(ii = 0; ii < sizeof(pszRegisters)/sizeof(*pszRegisters); ii++)
	{
		// Create a new value 
		sprintf(szNewValue, "%.8x", (int) (0xFFFFFFFF * (((float)rand()) / ((float)(RAND_MAX)))));
		strupr(szNewValue);
		sprintf(szSetValue, "0X%s", szNewValue);

		// Change the value via quick watch window
		EXPECT_TRUE(uiqw.Activate());
		EXPECT_TRUE(uiqw.SetExpression(pszRegisters[ii]));
		EXPECT_TRUE(uiqw.Recalc());
		EXPECT_TRUE(uiqw.SetNewValue(szSetValue));
		EXPECT_TRUE(uiqw.Recalc());

		// Verify that the new value has been set correctly
		EXPECT_TRUE(regs.Enable());
		EXPECT_TRUE(regs.RegisterValueIs(CString(pszRegisters[ii]), CString(szNewValue), -1));
	}

}

void CRegistersIDETest::Registers_Dam_Watch_Memory(void)
{

	LOGTESTHEADER("Registers_Dam_Watch_Memory");

	CString csEIP1, csEIP2, csMemoryAddress;
	CString csTmpAddress;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	// Turn Hex mode on
	EXPECT_TRUE(dbg.ToggleHexDisplay(TRUE));

	// Get current EIP and EIP 7 lines down
	EXPECT_TRUE(dam.Enable());
	csEIP1 = dam.GetInstruction();
	csEIP1 = csEIP1 .Left(8);

	// We use UI layer since we CODisassembly doesn't have GetInstructionAt()
	//EXPECT_TRUE(dam.AttachActive());
	EXPECT_TRUE(dam.Enable());
	csEIP2 = uidam.GetInstruction(7, 1, FALSE);
	csEIP2  = csEIP2 .Left(8);

	// Verify EIP in the Registers window
	EXPECT_TRUE(regs.Enable());
	EXPECT_TRUE(regs.RegisterValueIs(CString("EIP"), csEIP1, -1));

	// Verify EIP in the Memory window
	EXPECT_TRUE(mem.Enable());
	csMemoryAddress = mem.GetAddress(CString("EIP"));
	EXPECT_TRUE((0 == csEIP1.CompareNoCase(csMemoryAddress.Right(8))));

	// Verify EIP in the Watch window
	csMemoryAddress.MakeLower();			// Value in the Watch appear with small letters
	csMemoryAddress.Insert(0, "0x");
	EXPECT_TRUE(watch.AddWatch(CString("EIP")));
	EXPECT_TRUE(watch.VerifyWatchInfo(CString("EIP"), csMemoryAddress,  "unsigned long", (EXPR_STATE) 0 /*NOT_EXPANABLE*/));
	EXPECT_TRUE(watch.RemoveWatch(CString("EIP")));
	EXPECT_TRUE(ee.ExpressionValueIs("EIP", csMemoryAddress));
	
	// Set new value in the Registers window
	EXPECT_TRUE(regs.Enable());
	EXPECT_TRUE(regs.SetRegister(CString("EIP"), csEIP2, -1));

	// Make sure that values changed
	EXPECT_TRUE(dam.Enable());
	csTmpAddress = dam.GetInstruction();
	csTmpAddress = csTmpAddress.Left(8);
	EXPECT_TRUE(0 == csEIP2.CompareNoCase(csTmpAddress));
	
	// Verify EIP in the Memory window
	EXPECT_TRUE(mem.Enable());
	csMemoryAddress = mem.GetAddress(CString("EIP"));
	EXPECT_TRUE((0 == csEIP2.CompareNoCase(csMemoryAddress.Right(8))));

	// Verify EIP in the Watch window
	csMemoryAddress.MakeLower();			// Value in the Watch appear with small letters
	csMemoryAddress.Insert(0, "0x");
	EXPECT_TRUE(watch.AddWatch(CString("EIP")));
	EXPECT_TRUE(watch.VerifyWatchInfo(CString("EIP"), csMemoryAddress,  "unsigned long", (EXPR_STATE) 0 /*NOT_EXPANABLE*/));
	EXPECT_TRUE(watch.RemoveWatch(CString("EIP")));
	EXPECT_TRUE(ee.ExpressionValueIs("EIP", csMemoryAddress));

	EXPECT_TRUE(mem.Disable());
	EXPECT_TRUE(dam.Disable());

	// Turn Hex mode off
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));

}

void CRegistersIDETest::Registers_Watch_Memory(void)
{

	LOGTESTHEADER("Registers_Watch_Memory");

	char *pszRegisters[] =		{"EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EIP", "ESP", "EBP", "EFL"};
	CString csReg, csMemoryAddress;
	char szTmpData[1024];

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	// Turn Hex mode on
	EXPECT_TRUE(dbg.ToggleHexDisplay(TRUE));

	for(int ii = 0; ii < sizeof(pszRegisters)/sizeof(*pszRegisters); ii++)
	{

		for(int i = 0; i < 2; i++)
		{
			// Verify Register in the Registers window
			EXPECT_TRUE(regs.Enable());
			csReg = regs.GetRegister(CString(pszRegisters[ii]), NULL);

			// Verify Register in the Memory window
			EXPECT_TRUE(mem.Enable());
			csMemoryAddress = mem.GetAddress(CString(pszRegisters[ii]));
			EXPECT_TRUE((0 == csReg.CompareNoCase(csMemoryAddress.Right(8))));
	
			// Verify Register in the Watch window
			csMemoryAddress.MakeLower();			// Value in the Watch appear with small letters
			csMemoryAddress.Insert(0, "0x");
			EXPECT_TRUE(watch.AddWatch(CString(pszRegisters[ii])));
			EXPECT_TRUE(watch.VerifyWatchInfo(CString(pszRegisters[ii]), csMemoryAddress,  "unsigned long", (EXPR_STATE) 0 /*NOT_EXPANABLE*/));
			EXPECT_TRUE(watch.RemoveWatch(CString(pszRegisters[ii])));

			EXPECT_TRUE(ee.ExpressionValueIs(CString(pszRegisters[ii]), csMemoryAddress));

			// Set new value in the Registers window
			// Create a new value 
			sprintf(szTmpData, "%.8x", (int) (0xFFFFFFFF * (((float)rand()) / ((float)(RAND_MAX)))));
			strupr(szTmpData);
			EXPECT_TRUE(regs.Enable());
			EXPECT_TRUE(regs.SetRegister(CString(pszRegisters[ii]), CString(szTmpData), -1));
		}

		// Make sure that values changed, i = 1;

	}	

	EXPECT_TRUE(mem.Disable());
	// changing the EIP may bring the asm window up.
	EXPECT_TRUE(dam.Disable());

	// Turn Hex mode off
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));
}

void CRegistersIDETest::Change_EAX_Changes_RetValue(void)
{

	LOGTESTHEADER("Change_EAX_Changes_RetValue");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* Return form Foo */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());

	EXPECT_TRUE(regs.SetRegister(CString("EAX"), CString("000000AF"), -1));
	// Step from function
	EXPECT_TRUE(dbg.StepOver(2));

	// Turn Hex mode off
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));

	// Verify res
	EXPECT_TRUE(watch.AddWatch(CString("res")));
	EXPECT_TRUE(watch.VerifyWatchInfo(CString("res"), CString("175"),  "int", (EXPR_STATE) 0 /*NOT_EXPANABLE*/));
	EXPECT_TRUE(watch.RemoveWatch(CString("res")));

}

void CRegistersIDETest::DisplayIsAlwaysHex(void)
{

	LOGTESTHEADER("DisplayIsAlwaysHex");

	char *pszCPURegs[] =		{"EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EIP", "ESP", "EBP", "EFL"};
	CString csCPURegValues[10];  // MUST BE THE SAME SIZE AS NUMBER OF REGS ABOVE
	char *pszCPUSegments[] = 	{"CS", "DS", "ES", "SS", "FS", "GS" }; 
	CString csCPUSegmentsValues[6];  // MUST BE THE SAME SIZE AS NUMBER OF REGS ABOVE
	char *pszFloatingInt[] =	{"CTRL", "STAT", "TAGS", "EIP", "CS", "DS", "EDO"};
	CString csFloatingIntValues[7];  // MUST BE THE SAME SIZE AS NUMBER OF REGS ABOVE

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	// Turn Hex mode on
	EXPECT_TRUE(dbg.ToggleHexDisplay(TRUE));

	EXPECT_TRUE(regs.Enable());
	// Get initial values
	for(int ii = 0; ii < sizeof(pszCPURegs)/sizeof(*pszCPURegs); ii++)
	{
		csCPURegValues[ii] = regs.GetRegister(CString(pszCPURegs[ii]), NULL);
	}

	for(ii = 0; ii < sizeof(pszCPUSegments)/sizeof(*pszCPUSegments); ii++)
	{
		csCPUSegmentsValues[ii] = regs.GetRegister(CString(pszCPUSegments[ii]), NULL);
	}

	for(ii = 0; ii < sizeof(pszFloatingInt)/sizeof(*pszFloatingInt); ii++)
	{
		csFloatingIntValues[ii] = regs.GetRegister(CString(pszFloatingInt[ii]), NULL);
	}

	// Turn Hex mode on
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));

	// Make sure that values has not changed
	for(ii = 0; ii < sizeof(pszCPURegs)/sizeof(*pszCPURegs); ii++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(pszCPURegs[ii], csCPURegValues[ii], -1));
	}

	for(ii = 0; ii < sizeof(pszCPUSegments)/sizeof(*pszCPUSegments); ii++)
	{

		EXPECT_TRUE(regs.RegisterValueIs(pszCPUSegments[ii], csCPUSegmentsValues[ii], -1));
	}

	for(ii = 0; ii < sizeof(pszFloatingInt)/sizeof(*pszFloatingInt); ii++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(pszFloatingInt[ii], csFloatingIntValues[ii], -1));

	}

}

void CRegistersIDETest::IntegrityWithSetFocusToThread(void)
{

	LOGTESTHEADER("IntegrityWithSetFocusToThread");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* Thread proc */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	char szOriginalEIP[512];
	char szEIP[512];

	HelperFunctionVerifyEIP("IntegrityWithSetFocusToThread", szOriginalEIP);

	// Set 'main' or 'WinMainCRTStartup' as an active thread.
	unsigned long ulThreadID;
	EXPECT_TRUE(ee.GetExpressionValue("dwMainThreadID", &ulThreadID));
	EXPECT_TRUE(threads.SetCurrentThread(ulThreadID));

	HelperFunctionVerifyEIP("IntegrityWithSetFocusToThread", szEIP);
	if (!strcmp(szOriginalEIP, szEIP))
		LOG->RecordInfo("szOriginalEIP=[%s], szEIP=[%s]", szOriginalEIP, szEIP);
	EXPECT_TRUE(0 != strcmp(szOriginalEIP, szEIP));

	// if we are deep in thread switch goo, need to navigate stack so
	// we are inside main() and can thus evaluate dwSecondaryThreadID as
	// an expression
	stk.NavigateStack("main");
	// Set 'ThreadRoutine' as an active thread
	EXPECT_TRUE(ee.GetExpressionValue("dwSecondaryThreadID" ,&ulThreadID));
	EXPECT_TRUE(threads.SetCurrentThread(ulThreadID));

	HelperFunctionVerifyEIP("IntegrityWithSetFocusToThread", szEIP);
	if (strcmp(szOriginalEIP, szEIP))
		LOG->RecordInfo("szOriginalEIP=[%s], szEIP=[%s]", szOriginalEIP, szEIP);
	EXPECT_TRUE(0 == strcmp(szOriginalEIP, szEIP));
}

void CRegistersIDETest::IntegrityWithFunctionEvaluation(void)
{

	LOGTESTHEADER("IntegrityWithFunctionEvaluation");

	CString csFooAddress;
	char szEIP[512];

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(dbg.StepInto(1));

	// Get Foo address from the Watch window
	EXPECT_TRUE(ee.GetExpressionValue(CString("Foo"), csFooAddress));
	csFooAddress = csFooAddress.Mid(2, 8);
	csFooAddress.MakeUpper();

	// Get EIP from the Registers and Disassembly window
	HelperFunctionVerifyEIP("IntegrityWithFunctionEvaluation", szEIP);

	// Compare
	EXPECT_TRUE(0 == strcmp(LPCTSTR(csFooAddress), szEIP));
}

void CRegistersIDETest::BoundaryConditionsForEditingValues(void)
{

	LOGTESTHEADER("BoundaryConditionsForEditingValues");

	char szOriginalEIP[512];
	char szEIP[512];

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	// Get EIP from the Registers and Disassembly window
	HelperFunctionVerifyEIP("BoundaryConditionsForEditingValues", szOriginalEIP);

	// Open Disasm and try Boundary conditions
	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(regs.Enable());
	EXPECT_TRUE(regs.SetRegister(CString("EIP"), CString("00000000"), -1));
	EXPECT_TRUE(regs.SetRegister(CString("EIP"), CString("FFFFFFFF"), -1));
	EXPECT_TRUE(regs.SetRegister(CString("EIP"), CString(szOriginalEIP), -1));

	// Close Disasm and compare EIP
	EXPECT_TRUE(dam.Disable());
	HelperFunctionVerifyEIP("BoundaryConditionsForEditingValues", szEIP);
	EXPECT_TRUE(0 == strcmp(szOriginalEIP, szEIP));

}

void CRegistersIDETest::TypeInvalidValues(void)
{

	LOGTESTHEADER("TypeInvalidValues");

	CString csRegValue;
	char *pszCPURegs[] =		{"EAX ", "EBX ", "ECX ", "EDX ", "ESI ", "EDI ", "EIP ", "ESP ", "EBP ", "EFL "};
	char *pszCPUSegments[] = 	{"CS ", "DS ", "ES ", "SS ", "FS ", "GS " }; 
	char *pszFloatingInt[] =	{"CTRL ", "STAT ", "TAGS ", "EIP ", "CS ", "DS ", "EDO "};
	char *pszFlags[] =			{"OV", "UP", "EI", "PL", "ZR", "AC", "PE" , "CY"};


	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());
/*
	// Get initial values
	for(int ii = 0; ii < sizeof(pszCPURegs)/sizeof(*pszCPURegs); ii++)
	{
		csRegValue = regs.GetRegister(CString(pszCPURegs[ii]), NULL);
		EXPECT_TRUE(regs.SetRegister(CString(pszCPURegs[ii]), CString("JK%$#KL{"), -1));
		EXPECT_TRUE(regs.RegisterValueIs(CString(pszCPURegs[ii]), CString(csRegValue), -1));
	}

	for(ii = 0; ii < sizeof(pszCPUSegments)/sizeof(*pszCPUSegments); ii++)
	{
		csRegValue = regs.GetRegister(CString(pszCPUSegments[ii]), NULL);
		EXPECT_TRUE(regs.SetRegister(CString(pszCPUSegments[ii]), CString("JK%$"), -1));
		EXPECT_TRUE(regs.RegisterValueIs(CString(pszCPUSegments[ii]), CString(csRegValue), -1));
	}

	for(ii = 0; ii < sizeof(pszFloatingInt)/sizeof(*pszFloatingInt); ii++)
	{
		csRegValue = regs.GetRegister(CString(pszFloatingInt[ii]), NULL);
		EXPECT_TRUE(regs.SetRegister(CString(pszFloatingInt[ii]), CString("#KL{"), -1));
		EXPECT_TRUE(regs.RegisterValueIs(CString(pszFloatingInt[ii]), CString(csRegValue), -1));
	}
*/
	for(int ii = 0; ii < sizeof(pszFlags)/sizeof(*pszFlags); ii++)
	{
		csRegValue = regs.GetRegister(CString(pszFlags[ii]), NULL);

		// Try to set invalid number (2) for even flags
		if((float) (ii/2) == (float)ii/2.)
		{
			EXPECT_TRUE(regs.SetRegister(CString(pszFlags[ii]), CString("2"), -1));
		}
		// Try to set invalid letter (S) for odd flags
		else
		{
			EXPECT_TRUE(regs.SetRegister(CString(pszFlags[ii]), CString("S"), -1));
		}

		EXPECT_TRUE(regs.RegisterValueIs(CString(pszFlags[ii]), CString(csRegValue), -1));
	}
}

void CRegistersIDETest::FloatingPointException(void)
{

	LOGTESTHEADER("FloatingPointException");

	//	ADDED BY DVERMA 4/19/2000 as exception not thrown on Win ME 
	//	won't fix bug #64854 - changes to CTRL don't persist.
	DWORD sysVal = GetSystem();
	if ((sysVal == SYSTEM_WIN_95) || (sysVal == SYSTEM_WIN_98) || (sysVal == SYSTEM_WIN_MILL))
	{
		LOG->RecordInfo("FloatingPointException() - test won't run on Win 9X OS.");
		return;
	}	

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* Exception should be here for CTRL 277 */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());
	EXPECT_TRUE(regs.SetRegister(CString("CTRL"), CString("0277"), -1));
	EXPECT_TRUE(dbg.StepOver(1, NULL, NULL, NULL, NOWAIT));
	// wait for the exception message to come up.
	while(!MST.WButtonExists(GetLocString(IDSS_OK)));
	MST.DoKeys("{ENTER}");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* Exception should be here for CTRL 27B */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(regs.Enable());
	EXPECT_TRUE(regs.SetRegister(CString("CTRL"), CString("027B"), -1));
	EXPECT_TRUE(dbg.StepOver(1, NULL, NULL, NULL, NOWAIT));
	// wait for the exception message to come up.
	while(!MST.WButtonExists(GetLocString(IDSS_OK)));
	MST.DoKeys("{ENTER}");
}


void CRegistersIDETest::VerifyDockingView(void)
{
	LOGTESTHEADER("VerifyDockingView");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(uidam.EnableDockingView(TRUE));
	EXPECT_TRUE(uidam.IsDockingViewEnabled());
	// TODO: (dklem 09/14/98) Uncomment the following line when #6572 gets fixed
	// The bug #6572 is Memory leak while resizing DAM window. This leak is not that bad in the Release version
	// Based on my tests the Debug vesion consumes ~71,000K (at the peak) while running this test suite, Release version - 23,000K (at the peak) , which is acceptable
	// Comment the next line if the Debug version of MSDEV causes Out of Memory
	// TODO(michma - 3/2/00): this isn't working yet.
	//TestResizeMoveWindow(TRUE, TRUE);	

	EXPECT_TRUE(uidam.EnableDockingView(FALSE));
	EXPECT_TRUE(!uidam.IsDockingViewEnabled());
	// TODO: (dklem 09/14/98) Uncomment the following line when #6572 gets fixed
	// The bug #6572 is Memory leak while resizing DAM window. This leak is not that bad in the Release version
	// Based on my tests the Debug vesion consumes ~71,000K (at the peak) while running this test suite, Release version - 23,000K (at the peak) , which is acceptable
	// Comment the next line if the Debug version of MSDEV causes Out of Memory
	// TODO(michma - 3/2/00): this isn't working yet.
	//TestResizeMoveWindow(FALSE, TRUE);	

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

}


void CRegistersIDETest::MinimalTestResizeMoveWindow()
{

// TODO remove the next line immediately
//	return;
	
	BOOL bInitiallyMaximized;

	uidam.Activate();
	HWND hwnd = uidam.HWnd();

	if(IsZoomed(hwnd))
	{
		bInitiallyMaximized = TRUE;
		ShowWindow(hwnd, SW_RESTORE);
	}
	else 
		bInitiallyMaximized = FALSE;
	
	

	// TODO: (dklem 09/14/98) Uncomment the following 4 lines when #6572 gets fixed
	// The bug #6572 is Memory leak while resizing DAM window. This leak is not that bad in the Release version
	// Based on my tests the Debug vesion consumes ~71,000K (at the peak) while running this test suite, Release version - 23,000K (at the peak) , which is acceptable
	// Comment the next 4 lines if the Debug version of MSDEV causes Out of Memory
	// TODO(michma - 3/2/00): this isn't working yet.
	//TestResizeMoveWindow(FALSE, FALSE);

	// Restore the DAM window before turning Docking View on
	if(bInitiallyMaximized)
		ShowWindow(hwnd, SW_MAXIMIZE);
	
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));

	// TODO(michma - 3/2/00): this isn't working yet.
	//TestResizeMoveWindow(TRUE, FALSE);
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
}

/*

TODO(michma - 3/2/00): this isn't working yet.

void CRegistersIDETest::TestResizeMoveWindow(BOOL bIsDocked, BOOL bFullTest)
{
	int nPositionMoved;
	uiregs.Activate();

	BOOL DragFullWindows = TRUE;
	EXPECT_TRUE(GetDragFullWindows(&DragFullWindows));

	// Try minimum hor and ver sizes
	nPositionMoved = uiregs. ResizeWindow(bIsDocked, SLeft, DRight, -1, DragFullWindows);
	uiregs.ResizeWindow(bIsDocked, SLeft, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uiregs.ResizeWindow(bIsDocked, SBottom, DUp, -1, DragFullWindows);
	uiregs.ResizeWindow(bIsDocked, SBottom, DDown, nPositionMoved, DragFullWindows);

	// Move the window
	nPositionMoved = uiregs.MoveWindow(bIsDocked, DRight, 20, DragFullWindows);
	nPositionMoved = uiregs.MoveWindow(bIsDocked, DDown, 20, DragFullWindows);
	nPositionMoved = uiregs.MoveWindow(bIsDocked, DLeft, 20, DragFullWindows);
	nPositionMoved = uiregs.MoveWindow(bIsDocked, DUp, 20, DragFullWindows);

	if(!bFullTest) return;

	// Try to go beyond screen borders
	nPositionMoved = uiregs.ResizeWindow(bIsDocked, SLeft, DLeft, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
		EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
		uiregs.Activate();
	}
	uiregs.ResizeWindow(bIsDocked, SLeft, DRight, nPositionMoved, DragFullWindows);

	nPositionMoved = uiregs.ResizeWindow(bIsDocked, SRight, DRight, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
		EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
		uiregs.Activate();
	}
	uiregs.ResizeWindow(bIsDocked, SRight, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uiregs.ResizeWindow(bIsDocked, SBottom, DDown, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
		EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
		uiregs.Activate();
	}
	uiregs.ResizeWindow(bIsDocked, SBottom, DUp, nPositionMoved, DragFullWindows);

	nPositionMoved = uiregs.ResizeWindow(bIsDocked, STop, DUp, -1, DragFullWindows);
	// If we move it to far, the title bar is not available, we need to reactivate memory window
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	uiregs.Activate();
	uiregs.ResizeWindow(bIsDocked, STop, DDown, 10, DragFullWindows);

	// I like to move it move it
	nPositionMoved = uiregs.MoveWindow(bIsDocked, DRight, -1, DragFullWindows);
	// If we move it to far, the title bar is not available, we need to reactivate memory window
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	uiregs.Activate();
	uiregs.MoveWindow(bIsDocked, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uiregs.MoveWindow(bIsDocked, DLeft, -1, DragFullWindows);
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	uiregs.Activate();
	uiregs.MoveWindow(bIsDocked, DRight, nPositionMoved, DragFullWindows);

	nPositionMoved = uiregs.MoveWindow(bIsDocked, DDown, -1, DragFullWindows);
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	uiregs.Activate();
	uiregs.MoveWindow(bIsDocked, DUp, nPositionMoved, DragFullWindows);

	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	uiregs.Activate();

	nPositionMoved = uiregs.MoveWindow(bIsDocked, DUp, -1, DragFullWindows);
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uiregs.ChooseContextMenuItem(DOCKING_VIEW));
	uiregs.Activate();
	uiregs.MoveWindow(bIsDocked, DDown, nPositionMoved, DragFullWindows);

}
*/