///////////////////////////////////////////////////////////////////////////////
//	memcase.CPP
//											 
//	Created by :			
//		DKLEM
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "memcase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
//#define VERIFY_TEST_SUCCESS(TestCase)\
//	TestCase == ERROR_SUCCESS

//IMPLEMENT_TEST(999, CmemIDETest, CDbgTestBase, "Memory", -1, CmemSubSuite)
IMPLEMENT_TEST(CmemIDETest, CDbgTestBase, "Memory", -1, CmemSubSuite)
//IMPLEMENT_TEST(CmemIDETest, CDebugTestSet, "Memory", -1, CmemSubSuite)


// Information about a test to be put in the test header
#define LOGTESTHEADER(strTestName) 	m_strLogInfo.Format("# - %d.  %s. Total # - %d, Passed - %d %%", ++m_TestNumber, strTestName, m_TotalNumberOfTests, (m_TestNumber*100)/m_TotalNumberOfTests);  \
									LogTestHeader(m_strLogInfo);


// Globals that are used in the Memory Tests
	char			chJustChar; 
	char			sJustString [20]; 
	unsigned char	uchJustUnsignedChar;
	short int		nsJustShort;
	unsigned short	unsJustUnsignedShort;
	int				nJustInt;
	unsigned int	unJustUnsignedInt;
	long int		lJustLongInt;
	unsigned long	ulJustUnsignedLongInt;	
	float			fJustFloat;
	double			dJustDouble;
	long double		dlJustLongDouble;
	__int64			sfJustSixtyFourInt;
	unsigned __int64 sfJustSixtyFourUnsignedInt;

	// Arrays for the Column test
	const int number_of_columns_to_be_tested = 3;

	char			asJustString [number_of_columns_to_be_tested + 2] = "Mamb";
	char			asLongString[8*number_of_columns_to_be_tested + 2] = "Mamba-Tumba-Rumba-Shurum";
	short int		ansJustShort[number_of_columns_to_be_tested + 1] = { -123, 12, -34, 45};
	unsigned short	aunsJustUnsignedShort[number_of_columns_to_be_tested + 1] = { 22987, 126, 334, 645};
	int				anJustInt[number_of_columns_to_be_tested + 1] = { -65555, 112, -354, 4451};
	unsigned int	aunJustUnsignedInt[number_of_columns_to_be_tested + 1] = { 655551, 2112, 9834, 45008};
	long int		alJustLongInt[number_of_columns_to_be_tested + 1] = { -77777, 120087, 3477, 99045};
	unsigned long	aulJustUnsignedLongInt[number_of_columns_to_be_tested + 1]  = { 999999, 77712, 36544, 22245};
	float			afJustFloat[number_of_columns_to_be_tested + 1] = { (float) 123.14, (float) 23.16, (float) 60.70, (float) 2.93};
	double			adJustDouble[number_of_columns_to_be_tested + 1] = { -777e50,  77e22,  12e5,  54e10};
	long double		adlJustLongDouble[number_of_columns_to_be_tested + 1] = { -999e70, 89e3,  18e2,  -4e17};
	__int64			asfJustSixtyFourInt[number_of_columns_to_be_tested + 1] = { -1234567890987654909, 1, -2, -3 };
	unsigned __int64 asfJustSixtyFourUnsignedInt[number_of_columns_to_be_tested + 1] = { 9876543212345678123, 12, 45, 1};

void CmemIDETest::InitializeGlobalsThatAreUsedInTheMemoryTests(void)
{
	chJustChar = 'M'; 
	strcpy(sJustString, "Mamba-hara-mambu-ru"); /* 19 chars in the string */
	uchJustUnsignedChar = 'u';
	nsJustShort = -123;
	unsJustUnsignedShort = 22987;
	nJustInt = -65555;
	unJustUnsignedInt = 655551;
	lJustLongInt = -77777;
	ulJustUnsignedLongInt  = 999999;	
	fJustFloat = (float) 123.14;
	dJustDouble = -777e50;
	dlJustLongDouble = -999e70;
	sfJustSixtyFourInt = -1234567890987654909;
	sfJustSixtyFourUnsignedInt = 9876543212345678123;
}


// each function that runs a test has this prototype.
typedef void (CmemIDETest::*PTR_TEST_FUNC)(void); 

// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.

PTR_TEST_FUNC tests[] = {
	&CmemIDETest::GoToSymbol,	
	&CmemIDETest::EditInWatchVerifyInMemoryWindow,
	&CmemIDETest::VerifyEditAllFormats,
// TODO: (dklem 08/12/98) Uncomment the following 1 line when #4360 and #7452 and #7594 gets fixed. Fixed in 8307.0
// There is no Columns option in 60
//*	&CmemIDETest::VerifyColumns,
	&CmemIDETest::ReEvaluateExpression,
	&CmemIDETest::GoToOverloadedSymbol,
	&CmemIDETest::RestoringMWTroughtDebuggingSessions,
	&CmemIDETest::GoToMaxMinAddress,
	&CmemIDETest::GoToInvalidAddress,
// TODO: (dklem 08/12/98) Uncomment the following 1 line when #5190 gets fixed
	&CmemIDETest::EditEIP,
// TODO: (dklem 08/12/98) Uncomment the following 2 lines when #4360 and #7452 and #7594 gets fixed. Fixed in 8307.0
	&CmemIDETest::NavigatingPointer,
	&CmemIDETest::ContextMenuOnToolBar,
// TODO: (dklem 08/12/98) Uncomment the following 1 line when #5454 gets fixed. Fixed in 8239.4
	&CmemIDETest::VerifyMultipleUndoRedo,
//////////////////////////////////////////////////////////////////
// TODO: (dklem 08/12/98) Uncomment the following 1 line when #4360 and #7452 and #7594 gets fixed. Fixed in 8307.0
//*	&CmemIDETest::CycleThroughMemoryFormats,
// TODO: (dklem 08/12/98) Uncomment the following 1 line when #5555 and #7456 get fixed. Fixed in 8307.0
// TODO: (dklem 11/10/98) Uncomment the following 1 line when #9430 is fixed.
//*	&CmemIDETest::PgUpDownCtrlHomeEnd,
	&CmemIDETest::VerifyEditing,

	//	added by dverma
	&CmemIDETest::VerifyVCPPReg
};


void CmemIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CmemIDETest::Run()
{

	/******************
	 * INITIALIZATION *
	 ******************/


	// the base name of the localized directories and files we will use.
	if(GetSystem() & SYSTEM_DBCS)
		m_strProjBase = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü mem";
	else
		m_strProjBase = "mem";

	// the location of the unlocalized sources, under which the projects we will use will be located.
	m_strSrcDir = GetCWD() + "src\\mem";
	// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
	m_strExeDir = m_strSrcDir + "\\" + m_strProjBase + " exe";

	// clean-up the exe project from the last run.
	KillFiles(m_strExeDir + "\\Debug\\", "*.*");
	RemoveDirectory(m_strExeDir + "\\Debug");
	KillFiles(m_strExeDir + "\\", "*.*");
	RemoveDirectory(m_strExeDir);

	// set the options for the exe project we are building for this test.
	CProjWizOptions *pWin32AppWizOpt = new(CWin32AppWizOptions);
	pWin32AppWizOpt->m_strLocation = m_strSrcDir;
	pWin32AppWizOpt->m_strName = m_strProjBase + " exe";	
	
	// create the exe project.
	EXPECT_SUCCESS(prj.New(pWin32AppWizOpt));
	// create a new localized exe source file in the project dir from the unlocalized exe source file.
	CopyFile(m_strSrcDir + "\\" + "mem.cpp", m_strExeDir + "\\" + m_strProjBase + " exe.cpp", FALSE);
	// make it writable so it can be cleaned up later.
	SetFileAttributes(m_strExeDir + "\\" + m_strProjBase + " exe.cpp", FILE_ATTRIBUTE_NORMAL);
	// add the source to the project.
	EXPECT_SUCCESS(prj.AddFiles(m_strProjBase + " exe.cpp"));
	// build the project.
	EXPECT_SUCCESS(prj.Build());

	// all tests expect the ide to be in this initial state.
	EXPECT_TRUE(dbg.StepOver());

	EXPECT_TRUE(mem.Enable());

	// In the tests it is assumed that the initial memory window has Docking View turned on
	EXPECT_TRUE(uimem.EnableDockingView(TRUE));


	m_TestNumber = 0;
	m_TotalNumberOfTests = sizeof(tests) / sizeof(PTR_TEST_FUNC);
	
	// we only want to randomly run each test once.
	RandomNumberCheckList rncl(sizeof(tests) / sizeof(PTR_TEST_FUNC));
	// indexes into the test function array.
	int iTest;
	
	// randomly run each test once.
	while((iTest = rncl.GetNext()) != -1)
//	for(iTest = 0; iTest < sizeof(tests) / sizeof(PTR_TEST_FUNC); iTest++)
		(this->*(tests[iTest]))();

	VerifyDockingView();
	dbg.StopDebugging(ASSUME_NORMAL_TERMINATION) ;	

}


void CmemIDETest::GoToSymbol(void)
{
	LOGTESTHEADER("GoToSymbol");
	CString address;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));
	// Get current address (which is addres for nJustInt)
	EXPECT_TRUE(ee.GetExpressionValue("&nJustInt", address));
	// Change the address and check that it contains correct data
	EXPECT_TRUE(mem.MemoryDataIs("dJustDouble", dJustDouble));
	uimem.EnableToolBar(FALSE);
	EXPECT_TRUE(mem.MemoryDataIs(address, nJustInt));	

	EXPECT_TRUE(dbg.ToggleHexDisplay(TRUE));

	// Toggle Hex mode and do the previous tests to verify that hex mode doesn't affect them
	
	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));
	// Get current address (which is addres for nJustInt)
	EXPECT_TRUE(ee.GetExpressionValue("&nJustInt", address));
	// Change the address and check that it contains correct data
	EXPECT_TRUE(mem.MemoryDataIs("dJustDouble", dJustDouble));
	uimem.EnableToolBar(TRUE);
	EXPECT_TRUE(mem.MemoryDataIs(address, nJustInt));	

	// Toggle Hex mode back
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));

	// Resize and Move
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}

void CmemIDETest::VerifyEditAllFormats(void)
{

	LOGTESTHEADER("VerifyEditAllFormats");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

/*
	MEM_FORMAT_WCHAR Memory Format is not tested here.
*/

	//Try all types using the same schema
	// 1. Check data
	// 2. Update data in memory window
	// 3. Check data in memory and via expression

	EXPECT_TRUE(mem.MemoryDataIs("chJustChar", chJustChar));	
	chJustChar += 2;
	EXPECT_TRUE(mem.SetMemoryData("chJustChar", chJustChar, MEM_FORMAT_ASCII));	
	EXPECT_TRUE(mem.MemoryDataIs("chJustChar", chJustChar));	
	EXPECT_TRUE(cxx.ExpressionValueIs("chJustChar", chJustChar));	

	EXPECT_TRUE(mem.MemoryDataIs("uchJustUnsignedChar", uchJustUnsignedChar));	
	uchJustUnsignedChar += 2;
	EXPECT_TRUE(mem.SetMemoryData("uchJustUnsignedChar", uchJustUnsignedChar, MEM_FORMAT_ASCII));	
	EXPECT_TRUE(mem.MemoryDataIs("uchJustUnsignedChar", uchJustUnsignedChar));	
	EXPECT_TRUE(cxx.ExpressionValueIs("uchJustUnsignedChar", uchJustUnsignedChar));	

	// Check the Byte Memory Format
	EXPECT_TRUE(mem.SetMemoryData("uchJustUnsignedChar", "65", MEM_FORMAT_BYTE));
	uchJustUnsignedChar = 'e';  // Correspondes to 0x65
	EXPECT_TRUE(mem.MemoryDataIs("uchJustUnsignedChar", uchJustUnsignedChar));	


	EXPECT_TRUE(mem.MemoryDataIs("sJustString", sJustString, MEM_FORMAT_ASCII, strlen(sJustString)));	
	strcpy(sJustString, _strrev(sJustString));
	EXPECT_TRUE(mem.SetMemoryData("sJustString", sJustString, MEM_FORMAT_ASCII));	
	EXPECT_TRUE(mem.MemoryDataIs("sJustString", sJustString));	
	CString csExpectedString("ADDRESS ");
	csExpectedString = csExpectedString + "\"" + sJustString + "\"";
	EXPECT_TRUE(cxx.ExpressionValueIs("sJustString", csExpectedString));
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();
	

	EXPECT_TRUE(mem.MemoryDataIs("nsJustShort", nsJustShort));	
	nsJustShort *= 2;
// TODO: (dklem 08/12/98) Uncomment the following 3 lines when #4734 gets fixed
//	EXPECT_TRUE(mem.SetMemoryData("nsJustShort", nsJustShort, MEM_FORMAT_SHORT));	
//	EXPECT_TRUE(mem.MemoryDataIs("nsJustShort", nsJustShort));	
//	EXPECT_TRUE(cxx.ExpressionValueIs("nsJustShort", nsJustShort));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #4734 and #8915 are fixed. 
	 MinimalTestResizeMoveWindow();


	EXPECT_TRUE(mem.MemoryDataIs("unsJustUnsignedShort", unsJustUnsignedShort));	
	unsJustUnsignedShort *= 2;
	EXPECT_TRUE(mem.SetMemoryData("unsJustUnsignedShort", unsJustUnsignedShort, MEM_FORMAT_SHORT_UNSIGNED));	
	EXPECT_TRUE(mem.MemoryDataIs("unsJustUnsignedShort", unsJustUnsignedShort));	
	EXPECT_TRUE(cxx.ExpressionValueIs("unsJustUnsignedShort", unsJustUnsignedShort));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();

	// Check the Short Hex Memory Format
	// TODO: (dklem 10/20/98) Uncomment the following 3 lines when #8198 gets fixed.  Fixed int 8307.0
	EXPECT_TRUE(mem.SetMemoryData("unsJustUnsignedShort", "0113", MEM_FORMAT_SHORT_HEX));	
	unsJustUnsignedShort = 275;
	EXPECT_TRUE(mem.MemoryDataIs("unsJustUnsignedShort", unsJustUnsignedShort));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));	
	nJustInt += 1002;
// TODO: (dklem 08/12/98) Uncomment the following 3 lines when #4734 gets fixed
//	EXPECT_TRUE(mem.SetMemoryData("nJustInt", nJustInt, MEM_FORMAT_LONG));	
//	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));	
//	EXPECT_TRUE(cxx.ExpressionValueIs("nJustInt", nJustInt));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 and #4734 are fixed. 
	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedInt));	
	unJustUnsignedInt -= 157;
	EXPECT_TRUE(mem.SetMemoryData("unJustUnsignedInt", unJustUnsignedInt, MEM_FORMAT_LONG_UNSIGNED));	
	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedInt));	
	EXPECT_TRUE(cxx.ExpressionValueIs("unJustUnsignedInt", unJustUnsignedInt));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(mem.MemoryDataIs("lJustLongInt", lJustLongInt));	
	lJustLongInt += 12345;
// TODO: (dklem 08/12/98) Uncomment the following 3 lines when #4734 gets fixed
//	EXPECT_TRUE(mem.SetMemoryData("lJustLongInt", lJustLongInt, MEM_FORMAT_LONG));	
//	EXPECT_TRUE(mem.MemoryDataIs("lJustLongInt", lJustLongInt));	
//	EXPECT_TRUE(cxx.ExpressionValueIs("lJustLongInt", lJustLongInt));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 and #4734 are fixed. 
	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(mem.MemoryDataIs("ulJustUnsignedLongInt", ulJustUnsignedLongInt));	
	ulJustUnsignedLongInt -= 76543;
	EXPECT_TRUE(mem.SetMemoryData("ulJustUnsignedLongInt", ulJustUnsignedLongInt, MEM_FORMAT_LONG_UNSIGNED));	
	EXPECT_TRUE(mem.MemoryDataIs("ulJustUnsignedLongInt", ulJustUnsignedLongInt));	
	EXPECT_TRUE(cxx.ExpressionValueIs("ulJustUnsignedLongInt", ulJustUnsignedLongInt));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();

	// Check the Long Hex Memory Format
	EXPECT_TRUE(mem.SetMemoryData("ulJustUnsignedLongInt", "000be293", MEM_FORMAT_LONG_HEX));	
	ulJustUnsignedLongInt = 778899;
	EXPECT_TRUE(mem.MemoryDataIs("ulJustUnsignedLongInt", ulJustUnsignedLongInt));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();

// TODO: (dklem 08/12/98) Uncomment the following 15 lines when #4727 gets fixed.
//	Note: the code in Comem for the float, double and long double is NYI because memory window behavior when 
//	changing (typing) these types is not defined yet.
	EXPECT_TRUE(mem.MemoryDataIs("fJustFloat", fJustFloat));	
	fJustFloat *= (float) 1.981;
	EXPECT_TRUE(mem.SetMemoryData("fJustFloat", fJustFloat, MEM_FORMAT_REAL));	
	EXPECT_TRUE(mem.MemoryDataIs("fJustFloat", fJustFloat));	
	EXPECT_TRUE(cxx.ExpressionValueIs("fJustFloat", fJustFloat));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(mem.MemoryDataIs("dJustDouble", dJustDouble));	
	dJustDouble /= 3.923;
//  TODO: (dklem 09/02/98) Uncomment the following 2 lines when #6156 gets fixed
	EXPECT_TRUE(mem.SetMemoryData("dJustDouble", dJustDouble, MEM_FORMAT_REAL_LONG));	
	EXPECT_TRUE(mem.MemoryDataIs("dJustDouble", dJustDouble));	
// TODO: (dklem 09/02/98) Uncomment the following 1 line when #6097 gets fixed.	
//	EXPECT_TRUE(cxx.ExpressionValueIs("dJustDouble", dJustDouble));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(mem.MemoryDataIs("dlJustLongDouble", dlJustLongDouble));	
	dlJustLongDouble /= (-103.923);
//  TODO: (dklem 09/02/98) Uncomment the following 2 lines when #6156 gets fixed
	EXPECT_TRUE(mem.SetMemoryData("dlJustLongDouble", dlJustLongDouble, MEM_FORMAT_REAL_LONG));	
	EXPECT_TRUE(mem.MemoryDataIs("dlJustLongDouble", dlJustLongDouble));	

	// Check that E-000 can not be set
	CString csBytes = "{+}1.11111E-000";
	EXPECT_TRUE(mem.SetMemoryBytes("fJustFloat", csBytes, MEM_FORMAT_REAL));
	csBytes	= mem.ExtractMemoryData(MEM_FORMAT_REAL, "fJustFloat", 1);
	csBytes.TrimRight(" ");
	EXPECT_TRUE("+1.11111E+000" == csBytes);

	csBytes = "{+}1.11111E-";
	EXPECT_TRUE(mem.SetMemoryBytes("fJustFloat", csBytes, MEM_FORMAT_REAL));
	csBytes	= mem.ExtractMemoryData(MEM_FORMAT_REAL, "fJustFloat", 1);
	csBytes.TrimRight(" ");
	EXPECT_TRUE("+1.11111E+000" == csBytes);

//  TODO: (dklem 09/02/98) Uncomment the following 10 lines when #6156 gets fixed
	csBytes = "{+}2.22222222222222E-000";
	EXPECT_TRUE(mem.SetMemoryBytes("dJustDouble", csBytes, MEM_FORMAT_REAL_LONG));
	csBytes	= mem.ExtractMemoryData(MEM_FORMAT_REAL_LONG, "dJustDouble", 1);
	csBytes.TrimRight(" ");
	EXPECT_TRUE("+2.22222222222222E+000" == csBytes);

	csBytes = "{+}2.22222222222222E-";
	EXPECT_TRUE(mem.SetMemoryBytes("dJustDouble", csBytes, MEM_FORMAT_REAL_LONG));
	csBytes	= mem.ExtractMemoryData(MEM_FORMAT_REAL_LONG, "dJustDouble", 1);
	csBytes.TrimRight(" ");
	EXPECT_TRUE("+2.22222222222222E+000" == csBytes);


// TODO: (dklem 09/02/98) Uncomment the following 1 line when #6097 gets fixed.	
//	EXPECT_TRUE(cxx.ExpressionValueIs("dlJustLongDouble", dlJustLongDouble));	

	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourInt", sfJustSixtyFourInt));	
	sfJustSixtyFourInt += 12345987987;
// TODO: (dklem 08/12/98) Uncomment the following 3 lines when #4734 gets fixed
//	EXPECT_TRUE(mem.SetMemoryData("sfJustSixtyFourInt", sfJustSixtyFourInt, MEM_FORMAT_INT64));	
//	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourInt", sfJustSixtyFourInt));	
// TODO: (dklem 08/12/98) ExpressionValueIs NYI for __in64
//	EXPECT_TRUE(cxx.ExpressionValueIs("sfJustSixtyFourInt", sfJustSixtyFourInt));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 and #4734 are fixed. 
	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	
	sfJustSixtyFourUnsignedInt -= 765430009987;

// TODO: (dklem 08/12/98) Uncomment the following 2 lines when #4780 gets fixed
//	EXPECT_TRUE(mem.SetMemoryData("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt, MEM_FORMAT_INT64_UNSIGNED));	
//	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	
// TODO: (dklem 08/12/98) ExpressionValueIs NYI for __in64
//	EXPECT_TRUE(cxx.ExpressionValueIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	
	// Since there are too many crashes after editing we want to resize after editing each format
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 and #4780 fixed. 
	MinimalTestResizeMoveWindow();

	// Check the Long Hex Memory Format
//  TODO: (dklem 09/02/98) Uncomment the following 3 lines when #6156 gets fixed
	EXPECT_TRUE(mem.SetMemoryData("sfJustSixtyFourUnsignedInt", "002386d8905a5967", MEM_FORMAT_INT64_HEX));	
	sfJustSixtyFourUnsignedInt = 9999888877771111;
	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	

	// Resize and Move
	// TODO: (dklem 11/03/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}


void CmemIDETest::EditInWatchVerifyInMemoryWindow(void)
{

	LOGTESTHEADER("EditInWatchVerifyInMemoryWindow");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	chJustChar += 2;
	EXPECT_TRUE(cxx.SetExpressionValue("chJustChar", (int) chJustChar));	
	EXPECT_TRUE(mem.MemoryDataIs("chJustChar", chJustChar));	

	uchJustUnsignedChar += 2;
	EXPECT_TRUE(cxx.SetExpressionValue("uchJustUnsignedChar", (int) uchJustUnsignedChar));	
	EXPECT_TRUE(mem.MemoryDataIs("uchJustUnsignedChar", uchJustUnsignedChar));	

	nsJustShort *= 2;
	EXPECT_TRUE(cxx.SetExpressionValue("nsJustShort", nsJustShort));	
	EXPECT_TRUE(mem.MemoryDataIs("nsJustShort", nsJustShort));	

	unsJustUnsignedShort *= 2;
	EXPECT_TRUE(cxx.SetExpressionValue("unsJustUnsignedShort", unsJustUnsignedShort));	
	EXPECT_TRUE(mem.MemoryDataIs("unsJustUnsignedShort", unsJustUnsignedShort));	
	
	nJustInt += 1002;
	EXPECT_TRUE(cxx.SetExpressionValue("nJustInt", nJustInt));	
	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));	

	unJustUnsignedInt -= 157;
	EXPECT_TRUE(cxx.SetExpressionValue("unJustUnsignedInt", unJustUnsignedInt));	
	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedInt));	

	lJustLongInt += 12345;
	EXPECT_TRUE(cxx.SetExpressionValue("lJustLongInt", lJustLongInt));	
	EXPECT_TRUE(mem.MemoryDataIs("lJustLongInt", lJustLongInt));	

	ulJustUnsignedLongInt -= 76543;
	EXPECT_TRUE(cxx.SetExpressionValue("ulJustUnsignedLongInt", ulJustUnsignedLongInt));	
	EXPECT_TRUE(mem.MemoryDataIs("ulJustUnsignedLongInt", ulJustUnsignedLongInt));	

	fJustFloat = (float) 438.981;
	EXPECT_TRUE(cxx.SetExpressionValue("fJustFloat", fJustFloat));	
	EXPECT_TRUE(mem.MemoryDataIs("fJustFloat", fJustFloat));	

	dJustDouble = 3.923e22;
	EXPECT_TRUE(cxx.SetExpressionValue("dJustDouble", dJustDouble));	
	EXPECT_TRUE(mem.MemoryDataIs("dJustDouble", dJustDouble));	

	dlJustLongDouble = 1.237e71;
	EXPECT_TRUE(cxx.SetExpressionValue("dlJustLongDouble", dlJustLongDouble));	
	EXPECT_TRUE(mem.MemoryDataIs("dlJustLongDouble", dlJustLongDouble));	

	sfJustSixtyFourInt += 12345987987;
	EXPECT_TRUE(cxx.SetExpressionValue("sfJustSixtyFourInt", sfJustSixtyFourInt));	
	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourInt", sfJustSixtyFourInt));	

	sfJustSixtyFourUnsignedInt -= 765430009987;
	EXPECT_TRUE(cxx.SetExpressionValue("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	
	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	

	// Resize and Move
	// TODO: (dklem 11/03/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());
}


void CmemIDETest::VerifyColumns(void)
{
	LOGTESTHEADER("VerifyColumns");

	char str[64];

	// Make the last element in each array equal to 0 in order to make comparison.  
	// This means that GetMemoryData(,,, count = number_of_columns_to_be_tested + 1) should return 0 for the last element
	// We do not do this while initilazing arrays in order to easily copy/paste this part from debugee
	// if data change
	asJustString [number_of_columns_to_be_tested] = '\0';
	ansJustShort[number_of_columns_to_be_tested] = 0;
	aunsJustUnsignedShort[number_of_columns_to_be_tested] =  0;
	anJustInt[number_of_columns_to_be_tested] = 0;
	aunJustUnsignedInt[number_of_columns_to_be_tested] = 0;
	alJustLongInt[number_of_columns_to_be_tested] = 0;
	aulJustUnsignedLongInt[number_of_columns_to_be_tested] = 0;
	afJustFloat[number_of_columns_to_be_tested] = (float) 0.;
	adJustDouble[number_of_columns_to_be_tested] = 0.;
	adlJustLongDouble[number_of_columns_to_be_tested] = 0.;
	asfJustSixtyFourInt[number_of_columns_to_be_tested] = 0;
	asfJustSixtyFourUnsignedInt[number_of_columns_to_be_tested] = 0;
	
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(mem.Enable());
	EXPECT_TRUE(mem.SetColumns(itoa(number_of_columns_to_be_tested, str, 10)));

/*
	MEM_FORMAT_WCHAR Memory Format is not tested here.
*/

// TODO: (dklem 08/12/98) Uncomment the following 1 line when #4361 gets fixed. Fixed in 8239.4
// TODO: (dklem 10/07/98) Uncomment the following 1 line when #7580 gets fixed
//	EXPECT_TRUE(mem.MemoryDataIs("asJustString", asJustString, MEM_FORMAT_ASCII, number_of_columns_to_be_tested + 1));	

	EXPECT_TRUE(mem.MemoryDataIs("ansJustShort", ansJustShort, MEM_FORMAT_SHORT, number_of_columns_to_be_tested + 1));
	EXPECT_TRUE(mem.MemoryDataIs("aunsJustUnsignedShort", aunsJustUnsignedShort, MEM_FORMAT_SHORT_UNSIGNED, number_of_columns_to_be_tested + 1));
	EXPECT_TRUE(mem.MemoryDataIs("anJustInt", anJustInt, MEM_FORMAT_LONG, number_of_columns_to_be_tested + 1));
	EXPECT_TRUE(mem.MemoryDataIs("aunJustUnsignedInt", aunJustUnsignedInt, MEM_FORMAT_LONG_UNSIGNED, number_of_columns_to_be_tested + 1));
	EXPECT_TRUE(mem.MemoryDataIs("alJustLongInt", alJustLongInt, MEM_FORMAT_LONG, number_of_columns_to_be_tested + 1));
	EXPECT_TRUE(mem.MemoryDataIs("aulJustUnsignedLongInt", aulJustUnsignedLongInt, MEM_FORMAT_LONG_UNSIGNED, number_of_columns_to_be_tested + 1));
	EXPECT_TRUE(mem.MemoryDataIs("afJustFloat", afJustFloat, MEM_FORMAT_REAL, number_of_columns_to_be_tested + 1));
	EXPECT_TRUE(mem.MemoryDataIs("adJustDouble", adJustDouble, MEM_FORMAT_REAL_LONG, number_of_columns_to_be_tested + 1));
	EXPECT_TRUE(mem.MemoryDataIs("adlJustLongDouble", adlJustLongDouble, MEM_FORMAT_REAL_LONG, number_of_columns_to_be_tested + 1));	
	EXPECT_TRUE(mem.MemoryDataIs("asfJustSixtyFourInt", asfJustSixtyFourInt, MEM_FORMAT_INT64, number_of_columns_to_be_tested + 1));	
	// TODO: (dklem 11/04/98) Uncomment the following line when #7580 is fixed
	//EXPECT_TRUE(mem.MemoryDataIs("asfJustSixtyFourUnsignedInt", asfJustSixtyFourUnsignedInt, MEM_FORMAT_INT64_UNSIGNED, number_of_columns_to_be_tested + 1));	

//	Test MEM_FORMAT_BYTE format
	EXPECT_TRUE(mem.MemoryDataIs("asJustString", "4d 61 6d  Mam", MEM_FORMAT_BYTE, number_of_columns_to_be_tested * 4 + 1));
//	Test HEX formats
	EXPECT_TRUE(mem.MemoryDataIs("asLongString", "614d  626d  2d61        ", MEM_FORMAT_SHORT_HEX, (number_of_columns_to_be_tested + 1)));
	EXPECT_TRUE(mem.MemoryDataIs("asLongString", "626d614d  75542d61  2d61626d            ", MEM_FORMAT_LONG_HEX, (number_of_columns_to_be_tested + 1)));
	EXPECT_TRUE(mem.MemoryDataIs("asLongString", "75542d61626d614d  626d75522d61626d  6d75727568532d61                    ", MEM_FORMAT_INT64_HEX, (number_of_columns_to_be_tested + 1)));

	// Resize and Move
	// TODO: (dklem 11/03/98) Uncomment the following line when #7985 is fixed
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}


void CmemIDETest::ReEvaluateExpression(void)
{

	LOGTESTHEADER("ReEvaluateExpression");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));
	// TODO fro 60.  Toggle evaluation doesn't work in 60
	//EXPECT_TRUE(0 == uimem.GetCurrentAddress().CompareNoCase( uimem.GetCurrentSymbol().Right(8) ) );
	EXPECT_TRUE(uimem.ToogleReEvaluation());
	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));
	EXPECT_TRUE("nJustInt" == uimem.GetCurrentSymbol());
	EXPECT_TRUE(uimem.ToogleReEvaluation());

	// Resize and Move
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}



void CmemIDETest::GoToOverloadedSymbol(void)
{

	LOGTESTHEADER("GoToOverloadedSymbol");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(mem.Enable());
	CString address = mem.GetAddress("FuncWithArg");
	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(dam.GotoSymbol(address));

	// TODO: (dklem 10/06/98) Change uidam.GetInstruction() to dam.GetInstruction() when #6013 gets fixed
	CString Instruction = uidam.GetInstruction(-1, 1, FALSE);
	//CString csInstruction = dam.GetInstruction();

	EXPECT_TRUE(-1 != Instruction.Find("int FuncWithArg(int nArg)"));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

	// Resize and Move
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}

void CmemIDETest::RestoringMWTroughtDebuggingSessions(void)
{

	LOGTESTHEADER("RestoringMWTroughtDebuggingSessions");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedInt));
	CString address = uimem.GetCurrentAddress();

	EXPECT_TRUE(dbg.Restart());
	// TODO (dklem 01/29/99) Restarting doesn't preserve "unJustUnsignedInt" in the address box
	// So the next line is commented.
	// EXPECT_TRUE(0 == address.CompareNoCase(uimem.GetCurrentSymbol().Right(8)));

	// Resize and Move
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());
}


void CmemIDETest::GoToMaxMinAddress(void)
{

	LOGTESTHEADER("GoToMaxMinAddress");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	CString csAddress, csSymbol;

	EXPECT_TRUE("00000000" == (csAddress = mem.GetAddress("00000000")));
	EXPECT_TRUE("00000000" == (csSymbol = uimem.GetCurrentSymbol()));

	LOG->RecordInfo("CurrentAddress is %s", csAddress);
	LOG->RecordInfo("CurrentSymbol is %s", csSymbol);

	csAddress = mem.GetAddress("FFFFFFFF");
	csSymbol = uimem.GetCurrentSymbol();
	EXPECT_TRUE(0 == csAddress.CompareNoCase("FFFFFFFF"));
	EXPECT_TRUE(0 == csSymbol.CompareNoCase("FFFFFFFF"));

	LOG->RecordInfo("CurrentAddress is %s", csAddress);
	LOG->RecordInfo("CurrentSymbol is %s", csSymbol);

	// Resize and Move
	// TODO: (dklem 11/03/98) Uncomment the following line when #7985 is fixed
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());
}

void CmemIDETest::GoToInvalidAddress(void)
{

	LOGTESTHEADER("GoToInvalidAddress");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));
	
	EXPECT_TRUE(0 == mem.GetAddress("FFFFFFFFF5").CompareNoCase("FFFFFFF5"));
	EXPECT_TRUE(0 == uimem.GetCurrentSymbol().CompareNoCase("FFFFFFFFF5"));

	EXPECT_TRUE(0 == mem.GetAddress("PRIVET").CompareNoCase("FFFFFFF5"));
	EXPECT_TRUE(0 == uimem.GetCurrentSymbol().CompareNoCase("PRIVET"));

	uimem.EnableToolBar(FALSE);
	EXPECT_TRUE(ERROR_SUCCESS == uimem.GoToAddress("PRIVET"));

	// TODO: (dklem 08/12/98) Write a code for cheking  if "Invalid address" dialog comes up - see next 1 line - when #4379 gets fixed
	//EXPECT_TRUE(MST.WFndWnd("Add Dialog Caption", FW_NOCASE | FW_EXIST));

	EXPECT_TRUE(0 == uimem.GetCurrentAddress().CompareNoCase("FFFFFFF5"));


	uimem.EnableToolBar(TRUE);

	// Resize and Move
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());
}



void CmemIDETest::EditEIP(void)
{

	LOGTESTHEADER("EditEIP");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(dam.Enable());
	CString Instruction = uidam.GetInstruction(2, 1);
	if(Instruction.GetAt(0) == 'F')
	{
		EXPECT_TRUE(mem.SetMemoryData("EIP", "AF"/*Instruction*/, MEM_FORMAT_BYTE));	
	}
	else
	{
		EXPECT_TRUE(mem.SetMemoryData("EIP", "FF"/*Instruction*/, MEM_FORMAT_BYTE));	
	}

	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(Instruction != uidam.GetInstruction(0, 1));

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

	// Resize and Move
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());
}

void CmemIDETest::NavigatingPointer(void)
{
	LOGTESTHEADER("NavigatingPointer");
	
	//char str[64];
	//int nRegEIP;
	int number_of_columns_to_be_tested_for60 = 1;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(uimem.ToogleReEvaluation());

	EXPECT_TRUE(mem.Enable());
	// Get the address
	CString cstr = mem.ExtractMemoryData(MEM_FORMAT_LONG_HEX, "pnJustIntPtr", 1);
	// Navigate to address
	EXPECT_TRUE(mem.MemoryDataIs(cstr, nJustInt));


	// Get the address
	cstr = mem.ExtractMemoryData(MEM_FORMAT_LONG_HEX, "pvJustVoidPtrStr", 1);
	EXPECT_TRUE(mem.MemoryDataIs(cstr, sJustString, MEM_FORMAT_ASCII, strlen(sJustString)));
	// There is no columns feature in 60
	//EXPECT_TRUE(mem.SetColumns(itoa(number_of_columns_to_be_tested_for60, str, 10)));

	cstr = mem.ExtractMemoryData(MEM_FORMAT_LONG_HEX, "pvJustVoidPtrShort", 1);
	EXPECT_TRUE(mem.MemoryDataIs(cstr, ansJustShort, MEM_FORMAT_SHORT, number_of_columns_to_be_tested_for60));

	cstr = mem.ExtractMemoryData(MEM_FORMAT_LONG_HEX, "pvJustVoidPtrInt", 1);
	EXPECT_TRUE(mem.MemoryDataIs(cstr, anJustInt, MEM_FORMAT_LONG, number_of_columns_to_be_tested_for60));
	
	cstr = mem.ExtractMemoryData(MEM_FORMAT_LONG_HEX, "pvJustVoidPtrFloat", 1);
	EXPECT_TRUE(mem.MemoryDataIs(cstr, afJustFloat, MEM_FORMAT_REAL, number_of_columns_to_be_tested_for60));

	cstr = mem.ExtractMemoryData(MEM_FORMAT_LONG_HEX, "pvJustVoidPtrDouble", 1);
	EXPECT_TRUE(mem.MemoryDataIs(cstr, adJustDouble, MEM_FORMAT_REAL_LONG, number_of_columns_to_be_tested_for60));
	
	cstr = mem.ExtractMemoryData(MEM_FORMAT_LONG_HEX, "pvJustVoidPtrInt64", 1);
	EXPECT_TRUE(mem.MemoryDataIs(cstr, asfJustSixtyFourUnsignedInt, MEM_FORMAT_INT64, number_of_columns_to_be_tested_for60));

	EXPECT_TRUE(uimem.ToogleReEvaluation());

	// Check that setting EIP = 0 in the address edit box doesn't have side effects.  Related to bug 8940.  Fixed in 8303.1
	EXPECT_TRUE(dbg.Restart());

	// TODO (dklem 02/01/99) ERROR in 60 - Putting EIP = 0 in memory window stops debugee
	// Moreover EIP = 0 is memorized so there is no way to debug the app other than delet an opt file.
	/*
	EXPECT_TRUE(uimem.ToogleReEvaluation());
	nRegEIP = regs.GetRegister(EIP);
	itoa(nRegEIP, str, 10);
	uimem.Activate();
	EXPECT_TRUE(ERROR_SUCCESS == uimem.GoToAddress("EIP = 0"));
	//EXPECT_TRUE(mem.GetMemoryData((LPCSTR), tmp_str, MEM_FORMAT_ASCII)); 

	COApplication appDialog;
	if(!appDialog.Attach("Microsoft Visual C++", 10))
	{
		LOG->RecordFailure("The dialog \"Microsoft Visual C++ (invalid expression)\" did not come up");
		return;
	}
	else
		// Hit OK to close dialog
		appDialog.SendKeys("ENTER");

	
	// Check that this hasn't changed EIP
	EXPECT_TRUE(cxx.ExpressionValueIs("EIP", str));

	EXPECT_TRUE(uimem.ToogleReEvaluation());
	*/

	// Resize and Move
	// TODO: (dklem 11/03/98) Uncomment the following line when #7985 is fixed
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}


void CmemIDETest::ContextMenuOnToolBar(void)
{
	LOGTESTHEADER("ContextMenuOnToolBar");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(mem.Enable());
	CString csData = mem.ExtractMemoryData(MEM_FORMAT_SHORT_HEX, "asLongString", 1);
	EXPECT_TRUE(0 == csData.CompareNoCase("614d  ")); 
	int asa = csData.CompareNoCase("614d  ");

	csData = mem.ExtractMemoryData(MEM_FORMAT_LONG_HEX, "asLongString", 1);
	EXPECT_TRUE(0 == csData.CompareNoCase("626d614d  ")); 

	// Resize and Move
	// TODO: (dklem 11/03/98) Uncomment the following line when #7985 is fixed
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}


void CmemIDETest::VerifyMultipleUndoRedo(void)
{
	LOGTESTHEADER("VerifyMultipleUndoRedo");

	char str[64];

	unsigned int unJustUnsignedIntegers[2];
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedInt));	
	
	unJustUnsignedIntegers[0] = unJustUnsignedInt - 157;
	EXPECT_TRUE(mem.SetMemoryData("unJustUnsignedInt", unJustUnsignedIntegers[0], MEM_FORMAT_LONG_UNSIGNED));	
	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedIntegers[0]));	

	// 60 doesn't support Multiple Redo/Undo
/*
	unJustUnsignedIntegers[1] = unJustUnsignedIntegers[0] * 2;
	EXPECT_TRUE(mem.SetMemoryData("unJustUnsignedInt", unJustUnsignedIntegers[1], MEM_FORMAT_LONG_UNSIGNED));	
	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedIntegers[1]));
	
	EditUndo(strlen(itoa(unJustUnsignedIntegers[1], str, 10)));
	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedIntegers[0]));	
*/
	EditUndo(strlen(itoa(unJustUnsignedIntegers[0], str, 10)));
	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedInt));	

	EditRedo(strlen(itoa(unJustUnsignedIntegers[0], str, 10)));
	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedIntegers[0]));	
/*
	EditRedo(strlen(itoa(unJustUnsignedIntegers[1], str, 10)));
	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedIntegers[1]));	
*/
	// Resize and Move
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}

void CmemIDETest::CycleThroughMemoryFormats(void)
{
	LOGTESTHEADER("CycleThroughMemoryFormats");

	char str[64];

	// Check that nothing bad happens when cycling and Memory window is closed
	// TODO: (dklem 08/21/98) Uncomment the following 2 lines when #5485 gets fixed. Fixed in 8239.4
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(mem.CycleMemoryFormat(1));

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

/*
	MEM_FORMAT_WCHAR Memory Format is not tested here.
*/
	
	EXPECT_TRUE(mem.MemoryDataIs("asLongString", asLongString, MEM_FORMAT_ASCII, strlen(asLongString)));

	EXPECT_TRUE(mem.SetColumns(itoa(number_of_columns_to_be_tested, str, 10)));

	uimem.Activate();

	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_INT64_UNSIGNED;
	EXPECT_TRUE(" 8454432296767676749   7092453983987524205   7887336170996313441  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_INT64_HEX;
	EXPECT_TRUE("75542d61626d614d  626d75522d61626d  6d75727568532d61  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_INT64;
	EXPECT_TRUE(" 8454432296767676749   7092453983987524205   7887336170996313441  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_REAL_LONG;
	EXPECT_TRUE("+1.51481173207894E+257  +1.35711012392993E+166  +1.89271628313060E+219  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_REAL;
	EXPECT_TRUE("+1.09472E+021  +2.68967E+032  +1.28116E-011  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_LONG_UNSIGNED;
	EXPECT_TRUE("1651335501  1968450913   761356909  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_LONG_HEX;
	EXPECT_TRUE("626d614d  75542d61  2d61626d  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_LONG;
	EXPECT_TRUE(" 1651335501   1968450913    761356909  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_SHORT_UNSIGNED;
	EXPECT_TRUE("24909  25197  11617  " == uimem.GetCurrentData(number_of_columns_to_be_tested));
	
	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_SHORT_HEX;
	EXPECT_TRUE("614d  626d  2d61  " == uimem.GetCurrentData(number_of_columns_to_be_tested));

	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_SHORT;
	EXPECT_TRUE(" 24909   25197   11617  " == uimem.GetCurrentData(number_of_columns_to_be_tested));

	EXPECT_TRUE(mem.CycleMemoryFormat(1));
	uimem.m_MemoryFormat = MEM_FORMAT_BYTE;
	EXPECT_TRUE(0 == stricmp("4d 61 6d  Ma",uimem.GetCurrentData(number_of_columns_to_be_tested + 1)));

	// Resize and Move
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}


void CmemIDETest::PgUpDownCtrlHomeEnd(void)
{
	LOGTESTHEADER("PgUpDownCtrlHomeEnd");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();
	
	EXPECT_TRUE(mem.Enable());
	EXPECT_TRUE(mem.MemoryDataIs("chJustChar", chJustChar));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("uchJustUnsignedChar", uchJustUnsignedChar));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("nsJustShort", nsJustShort));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("unsJustUnsignedShort", unsJustUnsignedShort));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("nJustInt", nJustInt));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("unJustUnsignedInt", unJustUnsignedInt));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("lJustLongInt", lJustLongInt));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("ulJustUnsignedLongInt", ulJustUnsignedLongInt));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("fJustFloat", fJustFloat));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("dJustDouble", dJustDouble));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("dlJustLongDouble", dlJustLongDouble));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourInt", sfJustSixtyFourInt));	
	DoDifferentKeysInMemoryWindow();

	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	
	DoDifferentKeysInMemoryWindow();

	// Resize and Move
	// TODO: (dklem 11/03/98) Uncomment the following line when #7985 is fixed
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}

void CmemIDETest::DoDifferentKeysInMemoryWindow(void)
{
	CString csInitialAddress, csNewAddress, csNewAddress1;

	csInitialAddress = uimem.GetCurrentAddress();
	
	// Check PhUp and PgDown
	MST.DoKeys("{PGUP}");
	csNewAddress = uimem.GetCurrentAddress();
	EXPECT_TRUE(csNewAddress < csInitialAddress);

	MST.DoKeys("{PGDN}");
	csNewAddress = uimem.GetCurrentAddress();
	EXPECT_TRUE(csNewAddress == csInitialAddress);

	MST.DoKeys("{PGDN}");
	csNewAddress = uimem.GetCurrentAddress();
	EXPECT_TRUE(csNewAddress > csInitialAddress);

	MST.DoKeys("{PGUP}");
	csNewAddress = uimem.GetCurrentAddress();
	EXPECT_TRUE(csNewAddress == csInitialAddress);

	// Check Ctrl+End and Ctrl+Home
	MST.DoKeys("^{END}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress);
	EXPECT_TRUE(csNewAddress > csInitialAddress);

	MST.DoKeys("^{HOME}");
	csNewAddress = uimem.GetCurrentAddress();
	EXPECT_TRUE(csNewAddress == csInitialAddress);

	// Check Arrows Up and Down
	MST.DoKeys("{HOME}{UP}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress);
	EXPECT_TRUE(csNewAddress < csInitialAddress);

	MST.DoKeys("{HOME}{UP}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 < csNewAddress);

	MST.DoKeys("{HOME}{DOWN}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 == csNewAddress);

	MST.DoKeys("{HOME}{DOWN}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 == csInitialAddress);

	MST.DoKeys("{HOME}{DOWN}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress);
	EXPECT_TRUE(csNewAddress > csInitialAddress);

	MST.DoKeys("{HOME}{DOWN}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 > csNewAddress);

	MST.DoKeys("{HOME}{UP}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 == csNewAddress);

	MST.DoKeys("{HOME}{UP}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 == csInitialAddress);

	// Resize and Move
	// TODO: (dklem 11/03/98) Uncomment the following line when #7985 is fixed
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}

void CmemIDETest::VerifyDockingView(void)
{
	LOGTESTHEADER("VerifyDockingView");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	EXPECT_TRUE(uimem.EnableDockingView(FALSE));
	EXPECT_TRUE(!uimem.IsDockingViewEnabled());
	// TODO: (dklem 09/08/98) Uncomment the following 1 line when #6346 gets fixed
	// The bug #6346 is Memory leak while resizing Memory window.  This leak is not that bad in the Release version
	// Try to uncomment the next line and run the tests againts the Retail version when #7982 and #7985 are fixed. #7982 Fixed int 8307.0
	TestResizeMoveWindow(FALSE, TRUE);	

	EXPECT_TRUE(uimem.EnableDockingView(TRUE));
	EXPECT_TRUE(uimem.IsDockingViewEnabled());
	// TODO: (dklem 09/08/98) Uncomment the following 1 line when #6346 gets fixed
	// The bug #6346 is Memory leak while resizing Memory window.  This leak is not that bad in the Release version
	// Try to uncomment the next line and run the tests againts the Retail version when #7982 and #7985 are fixed. #7982 Fixed int 8307.0
	TestResizeMoveWindow(TRUE, TRUE);	

}

void CmemIDETest::VerifyEditing(void)
{
	LOGTESTHEADER("VerifyEditing");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	InitializeGlobalsThatAreUsedInTheMemoryTests();

	uimem.Activate();

	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	
	CString csInitialAddress = uimem.GetCurrentAddress();

// TODO: (dklem 09/02/98) Uncomment the following commented portion when #5440 gets fixed. Fixed in 8323.0

	// Check deleting address
	//MST.DoKeys("^{HOME}");
	MST.DoKeys("{HOME}");
	MST.DoKeys("{DEL 2}");
	MST.DoKeys("^+{RIGHT}");
	MST.DoKeys("{DEL}");

	// Check deleting data
	//MST.DoKeys("^{HOME}");
	MST.DoKeys("{HOME}");
	MST.DoKeys("{TAB}");
	MST.DoKeys("{DEL}");
	MST.DoKeys("^+{RIGHT}");
	MST.DoKeys("{DEL}");

	//MST.DoKeys("^{HOME}");
	MST.DoKeys("{HOME}");
	MST.DoKeys("{TAB}");
	MST.DoKeys("^+{RIGHT}");
	MST.DoKeys("{DEL}");

	// Data and address should not change
	EXPECT_TRUE(csInitialAddress == uimem.GetCurrentAddress());
	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	

	if(csInitialAddress != "12345678")
		clipboard.SetText("12345678");
	else
		clipboard.SetText("32154678");

	// Check pasting address
	//MST.DoKeys("^{HOME}");
	MST.DoKeys("{HOME}");
	MST.DoKeys("^+{RIGHT}");
	MST.DoKeys("^V");

	// Check pasting data
	//MST.DoKeys("^{HOME}");
	MST.DoKeys("{HOME}");
	MST.DoKeys("{TAB}");
	MST.DoKeys("^+{RIGHT}");
	MST.DoKeys("^V");

	// Data and address should not change
	EXPECT_TRUE(csInitialAddress == uimem.GetCurrentAddress());
	EXPECT_TRUE(mem.MemoryDataIs("sfJustSixtyFourUnsignedInt", sfJustSixtyFourUnsignedInt));	

	// Resize and Move
	// TODO: (dklem 11/04/98) Uncomment the following line when #8915 is fixed. 
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(mem.Disable());

}

void CmemIDETest::MinimalTestResizeMoveWindow()
{

	BOOL bInitiallyMaximized;
	// TODO: (dklem 11/11/98) enable this test by removing the following 1 line when #6346 is fixed
	// Set HKLM\Software\Microsoft\DevStudio\6.1\NoSafeDup to DWORD(1) to turn it off.  (Debug only).

	// TODO: (dklem 09/08/98) Uncomment the following 4 lines when #6346 gets fixed
	// The bug #6346 is Memory leak while resizing Memory window.  This leak is not that bad in the Release version
	// Try to uncomment the next 4 lines and run the tests againts the Retail version when #7982 and #7985 are fixed. #7982 Fixed int 8307.0
	TestResizeMoveWindow(TRUE, FALSE);
	uimem.ToggleDockingView();

	uimem.Activate();
	HWND hwnd = uimem.HWnd();

	if(IsZoomed(hwnd))
	{
		bInitiallyMaximized = TRUE;
		ShowWindow(hwnd, SW_RESTORE);
	}
	else 
		bInitiallyMaximized = FALSE;

	TestResizeMoveWindow(FALSE, FALSE);
	uimem.ToggleDockingView();

	// Restore the MEM window before turning Docking View on
	if(bInitiallyMaximized)
		ShowWindow(hwnd, SW_MAXIMIZE);
}

void CmemIDETest::TestResizeMoveWindow(BOOL bIsDocked, BOOL bFullTest)
{
	int nPositionMoved;
	uimem.Activate();

	BOOL DragFullWindows = TRUE;
	EXPECT_TRUE(GetDragFullWindows(&DragFullWindows));

	// Try minimum hor and ver sizes
	// TODO: (dklem 09/04/98) Uncomment the following 2 lines when #6246 gets fixed
	nPositionMoved = uimem. ResizeWindow(bIsDocked, SLeft, DRight, -1, DragFullWindows);
	uimem.ResizeWindow(bIsDocked, SLeft, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uimem.ResizeWindow(bIsDocked, SBottom, DUp, -1, DragFullWindows);
	uimem.ResizeWindow(bIsDocked, SBottom, DDown, nPositionMoved, DragFullWindows);

	// Move the window
	nPositionMoved = uimem.MoveWindow(bIsDocked, DRight, 20, DragFullWindows);
	nPositionMoved = uimem.MoveWindow(bIsDocked, DDown, 20, DragFullWindows);
	nPositionMoved = uimem.MoveWindow(bIsDocked, DLeft, 20, DragFullWindows);
	nPositionMoved = uimem.MoveWindow(bIsDocked, DUp, 20, DragFullWindows);

	if(!bFullTest) return;

	// Try to go beyond screen borders
	nPositionMoved = uimem.ResizeWindow(bIsDocked, SLeft, DLeft, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uimem.ToggleDockingView());
		EXPECT_TRUE(uimem.ToggleDockingView());
		uimem.Activate();
	}
	uimem.ResizeWindow(bIsDocked, SLeft, DRight, nPositionMoved, DragFullWindows);

	nPositionMoved = uimem.ResizeWindow(bIsDocked, SRight, DRight, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uimem.ToggleDockingView());
		EXPECT_TRUE(uimem.ToggleDockingView());
		uimem.Activate();
	}
	uimem.ResizeWindow(bIsDocked, SRight, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uimem.ResizeWindow(bIsDocked, SBottom, DDown, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uimem.ToggleDockingView());
		EXPECT_TRUE(uimem.ToggleDockingView());
		uimem.Activate();
	}
	uimem.ResizeWindow(bIsDocked, SBottom, DUp, nPositionMoved, DragFullWindows);

	// TODO: (dklem 09/04/98) Uncomment the following 2 lines when #6245 gets fixed. Fixed int 8307.0
	nPositionMoved = uimem.ResizeWindow(bIsDocked, STop, DUp, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uimem.ToggleDockingView());
		EXPECT_TRUE(uimem.ToggleDockingView());
		uimem.Activate();
	}
	uimem.ResizeWindow(bIsDocked, STop, DDown, nPositionMoved, DragFullWindows);

	// I like to move it move it
	nPositionMoved = uimem.MoveWindow(bIsDocked, DRight, -1, DragFullWindows);
	// If we move it to far, the title bar is not available, we need to reactivate memory window
	uimem.ToggleDockingView();
	uimem.ToggleDockingView();
	uimem.Activate();
	uimem.MoveWindow(bIsDocked, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uimem.MoveWindow(bIsDocked, DLeft, -1, DragFullWindows);
	uimem.ToggleDockingView();
	uimem.ToggleDockingView();
	uimem.Activate();
	uimem.MoveWindow(bIsDocked, DRight, nPositionMoved, DragFullWindows);

	nPositionMoved = uimem.MoveWindow(bIsDocked, DDown, -1, DragFullWindows);
	uimem.ToggleDockingView();
	uimem.ToggleDockingView();
	uimem.Activate();
	uimem.MoveWindow(bIsDocked, DUp, nPositionMoved, DragFullWindows);

	// TODO: (dklem 09/04/98) Uncomment the following 5 lines when #6261 gets fixed
	nPositionMoved = uimem.MoveWindow(bIsDocked, DUp, -1, DragFullWindows);
	uimem.ToggleDockingView();
	uimem.ToggleDockingView();
	uimem.Activate();
	uimem.MoveWindow(bIsDocked, DDown, nPositionMoved, DragFullWindows);
}


//	added by dverma for VCPP
void CmemIDETest::VerifyRegArray(CString Arr[],int Num_items)
{
	unsigned __int64 *data = new unsigned __int64;

	mem.Enable();

	for (int count = 0; count< Num_items; count++)
		mem.GetMemoryData(Arr[count], data, MEM_FORMAT_INT64_UNSIGNED, 1);

	mem.Disable();
}

//	added by dverma for VCPP
void CmemIDETest::VerifyVCPPReg(void)
{
	LOG->RecordInfo("Verifying Processor Pack Registers");
	
	CString MMXRegs[] = {"MM0","MM1","MM2","MM3","MM4","MM5","MM6","MM7"};
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

	CString XMM0IRegs[] = {	"XMM0IL","XMM0IH","XMM1IL","XMM1IH","XMM2IL","XMM2IH",
							"XMM3IL","XMM3IH","XMM4IL","XMM4IH","XMM5IL","XMM5IH",
							"XMM6IL","XMM6IH","XMM7IL","XMM7IH" };

	
	VerifyRegArray(MMXRegs,8);
		
	//	3dnow system
	if (CMDLINE->GetBooleanValue("k6",FALSE)||CMDLINE->GetBooleanValue("k7",FALSE))
	{
		VerifyRegArray(MMXijRegs,16);
	}

	//	P3 system
	if (CMDLINE->GetBooleanValue("kni",FALSE)||CMDLINE->GetBooleanValue("wni",FALSE))
	{
		VerifyRegArray(XMMRegs,8);
		VerifyRegArray(XMMijRegs,32);
		VerifyRegArray(XMM0DRegs,16);
		VerifyRegArray(XMM0IRegs,16);
	}

	LOG->RecordInfo("Processor Pack Verification Successful");
}
