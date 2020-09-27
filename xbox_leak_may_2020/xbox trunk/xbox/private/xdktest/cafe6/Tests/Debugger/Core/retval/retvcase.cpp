///////////////////////////////////////////////////////////////////////////////
//	RetVcase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "RetVcase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CRetValIDETest, CDebugTestSet, "Return Value", -1, CRetValSubSuite)
									   
												 
void CRetValIDETest::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CRetValIDETest::Run(void)
	
	{
	/******************
	 * INITIALIZATION *
	 ******************/
	if(!InitProject("retval\\testdll", PROJECT_DLL))
//	if(!SetProject("retval\\testdll", PROJECT_DLL))
		{
		m_pLog->RecordInfo("ERROR: could not init testdll project");
		return;
		}

	if(!InitProject("retval\\retval"))
//	if(!SetProject("retval\\retval"))
		{
		m_pLog->RecordInfo("ERROR: could not init retval project");
		return;
		}

	// this is the initial state each test expects.
	//	  - step passed prolog of WinMain().
	//	  - select Auto pane in variables wnd.
	//    - restore focus to source wnd.
	if(!dbg.StepOver(2))
		{
		m_pLog->RecordInfo("ERROR: could not step passed WinMain()'s prolog");
		return;
		}

	uivar.Activate();

	if(!uivar.SetPane(PANE_AUTO) == ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR: could not activate Auto pane");
		return;
		}

	RestoreFocusToSrcWnd();
		
	/*********
	 * TESTS *
	 *********/
	// can't step into system code on win95.
	//if(GetSystem() != SYSTEM_CHICAGO)
		// TODO (michma): need to get passed compiler problem with EnumWindowsProc
		// before we can finish implementing this test.
		//StepOutOfCallbackFunction();
		
	StepOverFunctionsReturningNativeTypes();
	StepOverFunctionReturningClassObject();
	StepOverFunctionReturningPointerToClassObject();
// Orion 12285 is postponed:	StepOverFunctionThrowingExceptionToParent();
	StepOutOfWinMain();
	StepOverDllFunction();
	StepOverConstructorAndVirtualMemberFunction();
	StepOverFunctionWithOnlyLineNumberDebugInfo();
	StepOverFunctionWithNoDebugInfo();
	// TODO (michma): need to change retval.mak so we actually statically link to retail crt.
	// test will fail until then.
	//StepOverStaticallyLinkedNonDebugCrtFunction();
	
	StepOverWin32ApiFunction();
	StepOutOfFunctionWithF8();
	StepOutOfFunctionWithF10();
	StepOutOfFunctionWithShiftF7();
	StepOverLineWithNoFunctionCall();
	VerifyNoReturnValueDisplayedAfterGo();
	VerifyReturnValueDisappearsWhenAssigningValue();
	VerifyReturnValueDisappearsWhenNotAssigningValue();
	VerifyReturnValueStaysAfterResettingNextStatement();
	VerifyReturnValueStaysAfterStackNavigation();
	VerifyReturnValueForFastcallCallingConvention();
	StepOutOfMultipleLevelsOfRecursion();
	VerifyReturnValueFieldCannotBeEdited();
	ToggleReturnValueOffAndOn();
	VerifyNoReturnValueDisplayedAfterRestart(); 
// TODO(michma): re-enable when orion: 9820.
	//VerifyNoReturnValueDisplayedAfterStopAndRestart(); 
	} 
	     

/*****************************************************************************
 * step over function returning each native type (void, char, unsigned char, *
 * int, unsigned int, long, unsigned long, float, double, long double).      *
 *****************************************************************************/
/***************************************************************
 * verify return value for cdecl (default) calling convention. *
 ***************************************************************/
void CRetValIDETest::StepOverFunctionsReturningNativeTypes(void)

	{
	// void
	LogTestHeader("StepOverFunctionsReturningNativeTypes - void");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
#ifndef _M_ALPHA
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
#else
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "0 '\\x00'");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
#endif
	RestoreFocusToSrcWnd();

	// char
	LogTestHeader("StepOverFunctionsReturningNativeTypes - char");
	EXPECT_TRUE(src.Find("char ch = func_char();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "127 '\x7f'");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "unsigned char", "uch", "UNKNOWN");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "char", "func_char returned", "127 '\x7f'");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();

	// unsigned char
	LogTestHeader("StepOverFunctionsReturningNativeTypes - unsigned char");
	EXPECT_TRUE(src.Find("func_uchar();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "UNKNOWN");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "unsigned char", "uch", "255 '\xff'");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "unsigned char", "func_uchar returned", "255 '\xff'");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();

	// int
	LogTestHeader("StepOverFunctionsReturningNativeTypes - int");
	EXPECT_TRUE(src.Find("func_int();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "2147483647");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "unsigned int", "ui", "UNKNOWN");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "int", "func_int returned", "2147483647");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();

	// unsigned int
	LogTestHeader("StepOverFunctionsReturningNativeTypes - unsigned int");
	EXPECT_TRUE(src.Find("func_uint();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "long", "l", "UNKNOWN");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "unsigned int", "ui", "4294967295");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "unsigned int", "func_uint returned", "4294967295");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();

	// long
	LogTestHeader("StepOverFunctionsReturningNativeTypes - long");
	EXPECT_TRUE(src.Find("func_long();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "long", "l", "2147483647");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "unsigned long", "ul", "UNKNOWN");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "long", "func_long returned", "2147483647");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();

	// unsigned long
	LogTestHeader("StepOverFunctionsReturningNativeTypes - unsigned long");
	EXPECT_TRUE(src.Find("func_ulong();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "float", "flt", "UNKNOWN");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "unsigned long", "ul", "4294967295");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "unsigned long", "func_ulong returned", "4294967295");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();

	// float
	LogTestHeader("StepOverFunctionsReturningNativeTypes - float");
	EXPECT_TRUE(src.Find("func_float();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "double", "dbl", "UNKNOWN");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "float", "flt", "3.40282e+038");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "float", "func_float returned", "3.40282e+038");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();

	// double
	LogTestHeader("StepOverFunctionsReturningNativeTypes - double");
	EXPECT_TRUE(src.Find("func_double();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "double", "dbl", "1.7976931348623e+308");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "double", "ldbl", "UNKNOWN");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "double", "func_double returned", "1.7976931348623e+308");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();

	// long double
	LogTestHeader("StepOverFunctionsReturningNativeTypes - long double");
	EXPECT_TRUE(src.Find("func_ldouble();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base", "b", "{...}");

	// 68k uses long doubles, other platforms map them to doubles.
	CString ldbl_val_str = "1.7976931348623e+308";

	CString ldbl_typ_str = "double";

	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, ldbl_typ_str, "ldbl", ldbl_val_str);
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, ldbl_typ_str, "func_ldouble returned", ldbl_val_str);
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();
	}


/**********************************************
 * step over function returning class object. *
 **********************************************/
void CRetValIDETest::StepOverFunctionReturningClassObject(void)
	{
	LogTestHeader("StepOverFunctionReturningClassObject");
	EXPECT_TRUE(src.Find("func_class();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base", "b", "{...}");
	FillExprInfo(expr_info_expected[1], COLLAPSED, "base *", "pb", "ADDRESS");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "base", "func_class returned", "{...}");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));
	RestoreFocusToSrcWnd();
	}


/*********************************************************
 * step over function returning pointer to class object. *
 *********************************************************/
void CRetValIDETest::StepOverFunctionReturningPointerToClassObject(void)
	{
	LogTestHeader("StepOverFunctionReturningPointerToClassObject");
	EXPECT_TRUE(src.Find("func_pclass();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base *", "pb", "ADDRESS");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "base *", "func_pclass returned", "ADDRESS");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	// verify that address values displayed for both items are the same.
	EXPECT_TRUE(expr_info_actual[0].value == expr_info_actual[1].value);
	RestoreFocusToSrcWnd();
	}


/************************************************************
 * step over function that does a throw and lands execution *
 * back in the parent function, and step out of the catch.  *
 ************************************************************/
void CRetValIDETest::StepOverFunctionThrowingExceptionToParent(void)

	{
	LogTestHeader("StepOverFunctionThrowingExceptionToParent");
	EXPECT_TRUE(src.Find("try"));
	EXPECT_TRUE(dbg.SetNextStatement());
	
	// step over function that does a throw.
	EXPECT_TRUE(dbg.StepOver(2));

	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	
	// step out of the catch.
	EXPECT_TRUE(dbg.StepInto());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	
	RestoreFocusToSrcWnd();
	}


/************************
 * step out of WinMain. *
 ************************/
/****************************************************************
 * verify return value for stdcall (pascal) calling convention. *
 ****************************************************************/
void CRetValIDETest::StepOutOfWinMain(void)
	{
	LogTestHeader("StepOutOfWinMain");
	// start from WinMain()'s return statement.
	EXPECT_TRUE(src.Find("return 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver(2));
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, ROW_LAST));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "WinMain returned", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	// restore execution to initial state (anywhere after WinMain prolog).
	RestoreFocusToSrcWnd();
	dbg.Restart();
	dbg.SetSteppingMode(SRC);
	dbg.StepOver();
	}


/***************************
 * step over dll function. *
 ***************************/
void CRetValIDETest::StepOverDllFunction(void)
	{
	LogTestHeader("StepOverDllFunction");
	EXPECT_TRUE(src.Find("dll_func();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "dll_func returned", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/********************************************************************************
 * step over implicit constructor call (new) and virtual class member function. *
 ********************************************************************************/
void CRetValIDETest::StepOverConstructorAndVirtualMemberFunction(void)

	{	
	LogTestHeader("StepOverConstructorAndVirtualMemberFunction");
	
	// step over the constructor.
	EXPECT_TRUE(src.Find("base *pd = new derived;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	// TODO (michma): need to change retval.mak so we statically link to retail crt.
	// (new's return value willl not be displayed).	don't forget to change ExprInfoIs' third param to 4.
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base *", "pd", "ADDRESS");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "pd->m_var", "UNKNOWN");
	//FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "void *", "operator new returned", "ADDRESS");
	FillExprInfo(expr_info_expected[2], NOT_EXPANDABLE, "void", "derived::derived returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 3));

	// step over the member function.
	EXPECT_TRUE(dbg.StepOver(2));
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base *", "pd", "ADDRESS");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "derived::m_func returned", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));

	RestoreFocusToSrcWnd();
	}


/********************************************************
 * step over function with only line number debug info. *
 ********************************************************/ 
void CRetValIDETest::StepOverFunctionWithOnlyLineNumberDebugInfo(void)
	{
	LogTestHeader("StepOverFunctionWithOnlyLineNumberDebugInfo");
	EXPECT_TRUE(src.Find("line_numbers_only();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/******************************************
 * step over function with no debug info. *
 ******************************************/
void CRetValIDETest::StepOverFunctionWithNoDebugInfo(void)
	{
	LogTestHeader("StepOverFunctionWithNoDebugInfo");
	EXPECT_TRUE(src.Find("no_debug_info();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/*********************************************************
 * step over statically linked, non-debug, crt function. *
 *********************************************************/
void CRetValIDETest::StepOverStaticallyLinkedNonDebugCrtFunction(void)
	{
	LogTestHeader("StepOverStaticallyLinkedNonDebugCrtFunction");
	EXPECT_TRUE(src.Find("abs(-1);"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
#ifndef _M_ALPHA
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "abs returned", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
#else
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base *", "pd", "ADDRESS");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "abs returned", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
#endif
	RestoreFocusToSrcWnd();
	}


/*****************************************************************************
 * step over Win32 API function.
 *****************************************************************************/
void CRetValIDETest::StepOverWin32ApiFunction(void)
	{
	LogTestHeader("StepOverWin32ApiFunction");
	EXPECT_TRUE(src.Find("GetLastError();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/*********************************
 * step out of function with F8. * 
 *********************************/
void CRetValIDETest::StepOutOfFunctionWithF8(void)
	
	{
	LogTestHeader("StepOutOfFunctionWithF8");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	if( WaitMsgBoxText("This operation will move the current location", 10) )
	{
		MST.WButtonClick( "OK" ); 
	}
	
	// TODO (michma): x86 acts like mips and alpha until orion: 4423 is fixed.
	//if (m_platform == PLATFORM_WIN32_ALPHA)
	//	{EXPECT_TRUE(dbg.StepInto(2));}
	//else
		{EXPECT_TRUE(dbg.StepInto(3));}
	
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));

	RestoreFocusToSrcWnd();
	}


/**********************************
 * step out of function with F10. *
 **********************************/
void CRetValIDETest::StepOutOfFunctionWithF10(void)
	
	{
	LogTestHeader("StepOutOfFunctionWithF10");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	if( WaitMsgBoxText("This operation will move the current location", 10) )
	{
		MST.WButtonClick( "OK" ); 
	}
	EXPECT_TRUE(dbg.StepInto());

	// TODO (michma): x86 acts like mips and alpha until orion: 4423 is fixed.
	//if (m_platform == PLATFORM_WIN32_ALPHA)
	//	{EXPECT_TRUE(dbg.StepOver());}
	//else
		{EXPECT_TRUE(dbg.StepOver(2));}

	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));

	RestoreFocusToSrcWnd();
	}


/***************************************
 * step out of function with shift+F7. *
 ***************************************/
void CRetValIDETest::StepOutOfFunctionWithShiftF7(void)
	{
	LogTestHeader("StepOutOfFunctionWithShiftF7");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.StepOut());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
#ifndef _M_ALPHA
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
#else
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "0 '\\x00'");
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
#endif
	RestoreFocusToSrcWnd();
	}


/*****************************************
 * step over line with no function call. *
 *****************************************/
void CRetValIDETest::StepOverLineWithNoFunctionCall(void)
	{
	LogTestHeader("StepOverLineWithNoFunctionCall");
	EXPECT_TRUE(src.Find("for(0;0;0);"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
#ifndef _M_ALPHA
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
#else
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "127 '\x7f'");
#endif
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/********************************************************************
 * go and verify that previous function return value not displayed. *
 ********************************************************************/
void CRetValIDETest::VerifyNoReturnValueDisplayedAfterGo(void)
	{
	LogTestHeader("VerifyNoReturnValueDisplayedAfterGo");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(src.Find("for(0;0;0);"));
	EXPECT_TRUE(dbg.StepToCursor());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
#ifndef _M_ALPHA
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
#else
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "127 '\x7f'");
#endif
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/***********************************************************************************************
 * verify that function return item disappears when stepping to next line and assigning value. *
 ***********************************************************************************************/
void CRetValIDETest::VerifyReturnValueDisappearsWhenAssigningValue(void)

	{
	LogTestHeader("VerifyReturnValueDisappearsWhenAssigningValue");
	EXPECT_TRUE(src.Find("char ch = func_char();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.StepOut());
	
	// just stepped out of function so value hasn't been asigned yet but
	// return value should be displayed.
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "UNKNOWN");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "char", "func_char returned", "127 '\x7f'");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	
	// return value should disappear now.
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "127 '\x7f'");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "unsigned char", "uch", "UNKNOWN");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));

	RestoreFocusToSrcWnd();
	}


/**************************************************************
 * verify that function return item disappears when stepping  *
 * to line after next and not assigning value.                *
 **************************************************************/
void CRetValIDETest::VerifyReturnValueDisappearsWhenNotAssigningValue(void)

	{
	LogTestHeader("VerifyReturnValueDisappearsWhenNotAssigningValue");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.StepOut());
	
	// after stepping out of function, execution resumes at next line since no value to assign.
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
#ifndef _M_ALPHA
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
#else
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "127 '\x7f'");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
#endif
	
	// return value should disappear now.
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
#ifndef _M_ALPHA
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
#else
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "127 '\x7f'");
#endif
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));

	RestoreFocusToSrcWnd();
	}


/************************************************************************
 * verify that function return item stays after setting next statement. *
 ************************************************************************/
void CRetValIDETest::VerifyReturnValueStaysAfterResettingNextStatement(void)
	{
	LogTestHeader("VerifyReturnValueStaysAfterResettingNextStatement");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(src.Find("while(0);"));
	EXPECT_TRUE(dbg.SetNextStatement());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
#ifndef _M_ALPHA
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
#else
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "ch", "127 '\x7f'");
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
#endif
	RestoreFocusToSrcWnd();
	}


/**********************************************************************
 * verify that function return item stays after callstack navigation. *
 **********************************************************************/
void CRetValIDETest::VerifyReturnValueStaysAfterStackNavigation(void)
	{
	LogTestHeader("VerifyReturnValueStaysAfterStackNavigation");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(stk.NavigateStack(1)!=0); //may hit the Kernel module, in which case -1
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, ROW_LAST));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	// restore stack to initial state. 
	EXPECT_TRUE(stk.NavigateStack(0)>0);
	RestoreFocusToSrcWnd();
	}


/*****************************************************************
 * step over function that uses the fastcall calling convention. * 
 *****************************************************************/
void CRetValIDETest::VerifyReturnValueForFastcallCallingConvention(void)
	{
	LogTestHeader("VerifyReturnValueForFastcallCallingConvention");
	EXPECT_TRUE(src.Find("fastcall_func();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "fastcall_func returned", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/******************************************************
 * step out of multiple levels of recursive function. *
 ******************************************************/
void CRetValIDETest::StepOutOfMultipleLevelsOfRecursion(void)
	
	{
	LogTestHeader("StepOutOfMultipleLevelsOfRecursion");
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("return i; // most nested return"));
	EXPECT_TRUE(dbg.StepToCursor());
	char ret_val_str[2];

	// step out of each call (4 times).
	// YS:actually it Steps out 5 times and now it shows 2 lines for the first 4 StepOuts
	for(int ret_val_dec = 5; ret_val_dec > 0; ret_val_dec--)
	{
		EXPECT_TRUE(dbg.StepOut());
		uivar.Activate();
	 	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
		if (ret_val_dec > 1)
		{
			FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", itoa(ret_val_dec - 1, ret_val_str, 10));
			FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "recursive_func returned", itoa(ret_val_dec, ret_val_str, 10));
		}
		else
			FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "recursive_func returned", itoa(ret_val_dec, ret_val_str, 10));
		EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	}

	RestoreFocusToSrcWnd();
	}


/**********************************
 * step out of callback function. *
 **********************************/
void CRetValIDETest::StepOutOfCallbackFunction(void)
	{
	// TODO (michma): need to get passed compiler problem with EnumWindowsProc
	// before we can finish implementing this test.
	LogTestHeader("StepOutOfCallbackFunction");
	}


/*********************************************
 * verify that Value field cannot be edited. *
 *********************************************/
void CRetValIDETest::VerifyReturnValueFieldCannotBeEdited(void)
	{
	LogTestHeader("VerifyReturnValueFieldCannotBeEdited");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	// attempt to set the value in the first row (containing return value) to 0.
	EXPECT_TRUE(uivar.SetValue(0, 1) == ERROR_ERROR);
	RestoreFocusToSrcWnd();
	}


/**************************************************************************************
 * restart debugging and verify that previous function return value removed at break. *
 **************************************************************************************/
void CRetValIDETest::VerifyNoReturnValueDisplayedAfterRestart(void)
	{
	LogTestHeader("VerifyNoReturnValueDisplayedAfterRestart");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.Restart());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, ROW_LAST));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "nCmdShow", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	// restore execution to inital state (anywhere after WinMain() prolog).
	EXPECT_TRUE(dbg.StepOver());
	RestoreFocusToSrcWnd();
	}


/*******************************************************
 * stop and restart debugging and verify that previous *
 * function return value removed at break.             *
 *******************************************************/
void CRetValIDETest::VerifyNoReturnValueDisplayedAfterStopAndRestart(void) 
	{
	LogTestHeader("VerifyNoReturnValueDisplayedAfterStopAndRestart");
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.Restart());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, ROW_LAST));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "nCmdShow", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	// restore execution to inital state (anywhere after WinMain() prolog).
	EXPECT_TRUE(dbg.StepOver());
	RestoreFocusToSrcWnd();
	}


/************************************************************
 * toggle return value off and an via Tools.Options.Debug. *
 ************************************************************/
void CRetValIDETest::ToggleReturnValueOffAndOn(void)
	
	{
	LogTestHeader("ToggleReturnValueOffAndOn");

	dbg.ToggleReturnValue(TOGGLE_OFF);
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();

	dbg.ToggleReturnValue(TOGGLE_ON);
	EXPECT_TRUE(src.Find("func_void();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "func_void returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}

