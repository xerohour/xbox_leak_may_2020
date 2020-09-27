///////////////////////////////////////////////////////////////////////////////
//	WCHCASES.CPP
//
//  Created by:			Date:
//			MichMa			10/19/94
//
//  Description:
//		Watch Window Breadth Test

#include "stdafx.h"
#include "wchcases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CWatchCases, CDebugTestSet, "Watch", -1, CCORESubSuite)

void CWatchCases::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CWatchCases::Run()
												
	{
	EXPR_INFO watch_info_actual[10], watch_info_expected[10];
	CString str;

	XSAFETY;

	// get the project into the initial debugging state

	file.Open("test.exe");
	bp.SetBreakpoint("break_here", "test.cpp");
	dbg.Go();
	/*
	// verify that watch window is empty after start debugging new project	

	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// add existing int variable to watch window

	EXPECT_SUCCESS(watch.SetName("i", 1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "i", "1");
	FillExprInfo(watch_info_expected[1], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// change value of int variable to an illegal value
	// verify that ESC cancels edit

	MST.DoKeys("{HOME}{TAB 2}a{ENTER}");
	str = "";
	MST.WEditText("", str);
	EXPECT_TRUE(str == "a");
	MST.DoKeys("{ESC}");
	EXPECT_TRUE(!MST.WEditExists(""));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// change value of int variable to another legal value
	
	EXPECT_SUCCESS(watch.SetValue("2", 1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "i", "2");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// verify watch window updated when int variable changes via execution

	dbg.StepOver();
	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "i", "3");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// verify watch window updated when int variable changes via memory window

	mem.Activate();
	mem.GoToAddress("i");
	mem.SetCurrentData("0400");
 	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "i", "4");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// verify watch window updated when int variable changes via locals window

	var.Activate();
	var.Locals();
	var.SetValue("5", 7);
 	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "i", "5");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// delete int variable

	EXPECT_SUCCESS(watch.Delete(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "", "", "");
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));
	*/
	// add existing CString variable to watch window without /s format specifier
	dbg.StepOver();
	watch.Activate();
	EXPECT_SUCCESS(watch.SetName("cstring", 1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  COLLAPSED, "CString", "cstring", "{\"abc\"}");

	FillExprInfo(watch_info_expected[1], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// verify that value of CString variable root cannot be edited

	MST.DoKeys("{HOME}{TAB 2}");
	MST.WEditText("", str);
	EXPECT_TRUE(str == "");
	MST.DoKeys("{ESC}");

	// expand CString variable

	EXPECT_SUCCESS(watch.Expand(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  EXPANDED, "CString", "cstring", "{\"abc\"}");

	FillExprInfo(watch_info_expected[1], 
				  COLLAPSED, "char *", "m_pchData", "ADDRESS \"abc\"");

	FillExprInfo(watch_info_expected[2], 
				  NOT_EXPANDABLE, "int", "m_nDataLength", "3");

	FillExprInfo(watch_info_expected[3], 
				  NOT_EXPANDABLE, "int", "m_nAllocLength", "3");

	FillExprInfo(watch_info_expected[4], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 5));
	
	// verify that value of m_pchData member displays only address when editing

	MST.DoKeys("{HOME}{DOWN}{TAB}");
	MST.WEditText("", str);
	MST.DoKeys("{ESC}");
	EXPECT_TRUE(ValueIsAddress(str));
	EXPECT_TRUE(str.GetLength() == 10);

	// change value field of m_pchData member to another legal value
	
	EXPECT_SUCCESS(watch.SetValue("tchar_array", 2, NAME_NOT_EDITABLE));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	
	FillExprInfo(watch_info_expected[0], 
				  EXPANDED, "CString", "cstring", "{\"xyz\"}");

	FillExprInfo(watch_info_expected[1], 
				  COLLAPSED, "char *", "m_pchData", "ADDRESS \"xyz\"");

	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 5));
	
	// expand m_pchData member of CString (has unnamed members)

	EXPECT_SUCCESS(watch.Expand(2));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[1], 
				  EXPANDED, "char *", "m_pchData", "ADDRESS \"xyz\"");

	FillExprInfo(watch_info_expected[2], 
				  NOT_EXPANDABLE, "char", "---", "120 'x'");

	FillExprInfo(watch_info_expected[3], 
				  NOT_EXPANDABLE, "int", "m_nDataLength", "3");

	FillExprInfo(watch_info_expected[4], 
				  NOT_EXPANDABLE, "int", "m_nAllocLength", "3");

	FillExprInfo(watch_info_expected[5], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 6));

	// verify all value fields updated when expanded CString variable changes
	// via execution

	dbg.StepOver();
	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  EXPANDED, "CString", "cstring", "\"1234\"");

	FillExprInfo(watch_info_expected[1], 
				  EXPANDED, "char *", "m_pchData", "ADDRESS \"1234\"");

	FillExprInfo(watch_info_expected[2], 
				  NOT_EXPANDABLE, "char", "---", "49 '1'");

	FillExprInfo(watch_info_expected[3], 
				  NOT_EXPANDABLE, "int", "m_nDataLength", "4");

	FillExprInfo(watch_info_expected[4], 
				  NOT_EXPANDABLE, "int", "m_nAllocLength", "4");

	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 6));

	// verify expanded CString variable isn't deleted by trying to delete one
	// of its members

	EXPECT_SUCCESS(watch.Delete(2));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 6));

	// collapse CString variable

	EXPECT_SUCCESS(watch.Collapse(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  COLLAPSED, "CString", "cstring", "{\"1234\"}");

	FillExprInfo(watch_info_expected[1], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// verify value field updated when collapsed CString variable changes
	// via execution

	dbg.StepOver();
	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

 	FillExprInfo(watch_info_expected[0], 
				  COLLAPSED, "CString", "cstring", "{\"5678\"}");

	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// delete collapsed CString variable

	EXPECT_SUCCESS(watch.Delete(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "", "", "");
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// delete expanded, CString variable by deleting only root name 
	// (i.e. top row only)

	EXPECT_SUCCESS(watch.SetName("cstring", 1));
	EXPECT_SUCCESS(watch.Expand(1));
	EXPECT_SUCCESS(watch.Delete(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// delete expanded, CString variable by deleting entire object
	// (i.e. all rows)
	
	EXPECT_SUCCESS(watch.SetName("cstring", 1));
	EXPECT_SUCCESS(watch.Expand(1));
	EXPECT_SUCCESS(watch.Delete(1, 4));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// add existing TCHAR arrray to watch window without /s format specifier

	EXPECT_SUCCESS(watch.SetName("tchar_array", 1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], COLLAPSED, "char *", "tchar_array", "ADDRESS \"xyz\"");
	FillExprInfo(watch_info_expected[1], NOT_EXPANDABLE, "", "", "");
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// verify that value of TCHAR array root level cannot be edited

	MST.DoKeys("{HOME}{TAB 2}");
	str = "{\"xyz\"}";
	EXPECT_TRUE(!MST.WEditExists(str));
	MST.DoKeys("{ESC}");

	// expand TCHAR array
	
	EXPECT_SUCCESS(watch.Expand(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  EXPANDED, "char *", "tchar_array", "ADDRESS {\"xyz\"}");

	FillExprInfo(watch_info_expected[1], 
				  NOT_EXPANDABLE, "char", "[0]", "120 'x'");

	FillExprInfo(watch_info_expected[2], 
				  NOT_EXPANDABLE, "char", "[1]", "121 'y'");

	FillExprInfo(watch_info_expected[3], 
				  NOT_EXPANDABLE, "char", "[2]", "122 'z'");

	FillExprInfo(watch_info_expected[4], 
				  NOT_EXPANDABLE, "char", "[3]", "0 ''");

	FillExprInfo(watch_info_expected[5], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 6));
	
	// verify that value of TCHAR array element displays only int value when editing

	MST.DoKeys("{HOME}{DOWN}{TAB}");
	MST.WEditText("", str);
	MST.DoKeys("{ESC}");
	EXPECT_TRUE(str == "120");

	// change value of TCHAR array element to another legal value
	
	EXPECT_SUCCESS(watch.SetValue("p", 2, NAME_NOT_EDITABLE));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  EXPANDED, "char *", "tchar_array", "ADDRESS {\"pyz\"}");

	FillExprInfo(watch_info_expected[1], 
				  NOT_EXPANDABLE, "char", "[0]", "112 'p'");

	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 6));
	
	// verify all value fields updated when expanded TCHAR array changes via execution

	dbg.StepOver();
	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  EXPANDED, "char *", "tchar_array", "ADDRESS {\"pdz\"}");

	FillExprInfo(watch_info_expected[2], 
				  NOT_EXPANDABLE, "char", "[1]", "100 'd'");

	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 6));

	// verify expanded TCHAR array isn't deleted by trying to delete one
	// of its elements

	EXPECT_SUCCESS(watch.Delete(2));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 6));

	// collapse TCHAR array

	EXPECT_SUCCESS(watch.Collapse(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  COLLAPSED, "char *", "tchar_array", "ADDRESS \"pdz\"");

	FillExprInfo(watch_info_expected[1], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// verify value field updated when collapsed TCHAR array changes via execution

	dbg.StepOver();
	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));

	FillExprInfo(watch_info_expected[0], 
				  COLLAPSED, "char *", "tchar_array", "ADDRESS {\"pdq\"}");

	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// delete collapsed TCHAR array variable

	EXPECT_SUCCESS(watch.Delete(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "", "", "");
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// delete expanded TCHAR array by deleting only root name (i.e. top row only)

	EXPECT_SUCCESS(watch.SetName("tchar_array", 1));
	EXPECT_SUCCESS(watch.Expand(1));
	EXPECT_SUCCESS(watch.Delete(1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// delete expanded TCHAR array by deleting entire object (i.e. all rows)

	EXPECT_SUCCESS(watch.SetName("tchar_array", 1));
	EXPECT_SUCCESS(watch.Expand(1));
	EXPECT_SUCCESS(watch.Delete(1, 5));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// add existing struct variable to watch window

	EXPECT_SUCCESS(watch.SetName("a_struct", 1));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], COLLAPSED, "A_STRUCT", "a_struct", "{...}");
	FillExprInfo(watch_info_expected[1], NOT_EXPANDABLE, "", "", "");
 	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));
	EXPECT_SUCCESS(watch.Delete(1));

	// with local int, local expanded CString, and local expanded TCHAR array, and global
	// float in watch window, step into a function.
	
	EXPECT_SUCCESS(watch.SetName("i", 1));
	EXPECT_SUCCESS(watch.SetName("cstring", 2));
	EXPECT_SUCCESS(watch.SetName("tchar_array", 3));
	EXPECT_SUCCESS(watch.SetName("f", 4));
	EXPECT_SUCCESS(watch.Expand(3));
	EXPECT_SUCCESS(watch.Expand(2));
 	dbg.StepInto();
 	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "---", "i", "---");
	FillExprInfo(watch_info_expected[1], NOT_EXPANDABLE, "---", "cstring", "---");
	FillExprInfo(watch_info_expected[2], NOT_EXPANDABLE, "---", "tchar_array", "---");
	FillExprInfo(watch_info_expected[3], NOT_EXPANDABLE, "float", "f", "1.000000");
	FillExprInfo(watch_info_expected[4], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 5));

	// with out-of-scope int, expanded CString, and expanded TCHAR array and global float 
	// in the watch window, navigate the stack to the parent function, and back

 	stack.NavigateStack("WinMain");
 	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[5], NOT_EXPANDABLE, "int", "i", "5");
	FillExprInfo(watch_info_expected[6], COLLAPSED, "CString", "cstring", "{\"5678\"}");
	FillExprInfo(watch_info_expected[7], COLLAPSED, "char *", "tchar_array", "ADDRESS \"pdq\"");
	FillExprInfo(watch_info_expected[8], NOT_EXPANDABLE, "float", "f", "1.000000");
	FillExprInfo(watch_info_expected[9], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, &watch_info_expected[5], 5));
	stack.NavigateStack("func");
 	EXPECT_TRUE(watch.Activate());
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 5));

	// with global float in watch window, verify all fields updated when int, expanded CString, and expanded TCHAR
	// array come back into scope

 	dbg.StepOut();
 	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, &watch_info_expected[5], 5));
	EXPECT_SUCCESS(watch.Delete(1, 4));

	// with int variable in all panes and unique variable in each pane, 
	// verify all panes updated when all variables change via execution

	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.SetName("i", 1));
	EXPECT_SUCCESS(watch.SetName("cstring", 2));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH2));	
	EXPECT_SUCCESS(watch.SetName("i", 1));
	EXPECT_SUCCESS(watch.SetName("tchar_array", 2));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH3));
	EXPECT_SUCCESS(watch.SetName("i", 1));
	EXPECT_SUCCESS(watch.SetName("float", 2));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH4));
	EXPECT_SUCCESS(watch.SetName("i", 1));
	EXPECT_SUCCESS(watch.SetName("a_struct", 2));
	dbg.StepOver();
	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH1));	
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "i", "1");
	FillExprInfo(watch_info_expected[1], COLLAPSED, "CString", "cstring", "{\"abc\"}");
	FillExprInfo(watch_info_expected[2], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 3));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH2));	
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[3], NOT_EXPANDABLE, "int", "i", "1");
	FillExprInfo(watch_info_expected[4], COLLAPSED, "char *", "tchar_array", "ADDRESS \"xyz\"");
	FillExprInfo(watch_info_expected[5], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, &watch_info_expected[3], 3));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH3));	
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[6], NOT_EXPANDABLE, "int", "i", "1");
	FillExprInfo(watch_info_expected[7], NOT_EXPANDABLE, "float", "f", "2.000000");
	FillExprInfo(watch_info_expected[8], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, &watch_info_expected[6], 3));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH4));	
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[9], NOT_EXPANDABLE, "int", "i", "1");
	FillExprInfo(watch_info_expected[10], COLLAPSED, "A_STRUCT", "a_struct", "{...}");
	FillExprInfo(watch_info_expected[11], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, &watch_info_expected[9], 3));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH1));	
	
	// verify that watch window status of all panes is preserved if project is closed and re-opened

	dbg.StopDebugging();
	file.Close();
	file.Close();
	file.Open("test.exe");
	bp.ClearAllBreakpoints();
	bp.SetBreakpoint("break_here_too", "test.cpp");
	dbg.Go();
	EXPECT_TRUE(watch.Activate());
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH1));	
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 3));
	EXPECT_SUCCESS(watch.Delete(1, 2));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH2));	
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, &watch_info_expected[3], 3));	
	EXPECT_SUCCESS(watch.Delete(1, 2));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH3));	
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, &watch_info_expected[6], 3));
	EXPECT_SUCCESS(watch.Delete(1, 2));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH4));	
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, &watch_info_expected[9], 3));
	EXPECT_SUCCESS(watch.Delete(1, 2));
	EXPECT_SUCCESS(watch.SetPane(PANE_WATCH1));	
	
	// add expression composed of multiple variables to watch window

	EXPECT_SUCCESS(watch.SetName("((i * tchar_array[0]) + cstring.m_pchData - a_struct.i) / f"));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "((i * tchar_array[0]) + cstring.m_pchData - a_struct.i) / f", "100");
	FillExprInfo(watch_info_expected[1], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));
	EXPECT_SUCCESS(watch.Delete(1));	

	// add non-existent variable to watch window	

	EXPECT_SUCCESS(watch.SetName("non-existent var"));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "---", "non-existent var", "---");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));
	EXPECT_SUCCESS(watch.Delete(1));	

	// add function call to watch window

	EXPECT_SUCCESS(watch.SetName("func()"));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "---", "func()", "---");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));
	EXPECT_SUCCESS(watch.Delete(1));	

	// change name field from expanded CString variable to existing int variable

	EXPECT_SUCCESS(watch.SetName("cstring"));
	EXPECT_SUCCESS(watch.Expand(1));
	EXPECT_SUCCESS(watch.SetName("i"));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "i", "1");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// change name field from expanded TCHAR array to existing int variable

	EXPECT_SUCCESS(watch.SetName("tchar_array"));
	EXPECT_SUCCESS(watch.Expand(1));
	EXPECT_SUCCESS(watch.SetName("i"));
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 2));

	// delete int variable, expanded CString variable, and expanded TCHAR array all at once
	// by only selecting root level of each variable

	EXPECT_SUCCESS(watch.SetName("i"));
	EXPECT_SUCCESS(watch.SetName("cstring"));
	EXPECT_SUCCESS(watch.SetName("tchar_array"));
	EXPECT_SUCCESS(watch.Expand(3));
	EXPECT_SUCCESS(watch.Expand(2));
	EXPECT_SUCCESS(watch.SelectRows(1));
	EXPECT_SUCCESS(watch.SelectRows(2));
	EXPECT_SUCCESS(watch.SelectRows(6));
	EXPECT_SUCCESS(watch.Delete());
	EXPECT_SUCCESS(watch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// verify that virtual object is updated when derived type changes
	// verify all keyboard controls
	// change from docked view to mdi view and back

	XSAFETY;
	}
