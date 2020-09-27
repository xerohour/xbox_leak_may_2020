///////////////////////////////////////////////////////////////////////////////
//	AUTOCASE.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 	   
//		TestWizard Generated script.	 

#include "stdafx.h"
#include "autocase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
										   
IMPLEMENT_TEST(CAutoPaneIDETest, CDbgTestBase, "Auto Pane", -1, CVarsWndSubSuite)
												 
void CAutoPaneIDETest::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CAutoPaneIDETest::Run(void)
	
	{
	/******************
	 * INITIALIZATION *
	 ******************/
	if(!InitProject("autopane\\autopane"))
		{
		m_pLog->RecordInfo("ERROR: could not init autopane project");
		return;
		}

	// this is the initial state each test expects.
	//	  - step passed prolog of WinMain().
	//	  - select Auto pane in variables wnd.
	//    - restore focus to source window.
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
	VerifyUpdateAfterGo();
	VerifyUpdateAfterStepInto();
	VerifyUpdateAfterStepOut();
	VerifyUpdateAfterStepOver();
//	VerifyUpdateAfterRestart();	   //bug orion #15002
	VerifyUpdateAfterBreak();
	VerifyUpdateAfterRunToCursor();
	VerifyUpdateAfterSetNextStatement();
	ChangeValueViaExecution();
//	ChangeValueViaMemWnd();
	ChangeValueViaWatchWnd();
	ChangeValueViaQuickWatchDlg();
	ChangeValueViaAutoPane();
	//VerifyFuncArgsAppearWhenPrologReached();
	//NavigateStackToParentFuncAndBack();
	//ChangeToThisPaneAndBack();
	//ChangeToLocalsPaneAndBack();
	//VerifyOnlyCurrentAndPrevStatementsAreScanned();
	//VerifyMultilineStatementsGetScannedBackTenLines();
	ScanArraySubscript();
	ScanCastsAndOpsInsideArraySubscript();
	ScanSelfSubscriptedArray();
	ScanIndirectAccessToClassVariableAndFunc();
	ScanDirectAccessToClassVariableAndFunc();
	ScanScopeOp();
	ScanAddressOfOp();
	//ScanPointerToMember();
	//VerifyRegistersNotAddedInMixedMode();
	//VerifyNamesCantBeManuallyDeletedAddedOrEdited();
	//VerifyConstantExpressionsNotAdded();
	//VerifyFuncCallsNotAdded();
	//VerifyUnsupportedOpsNotAdded();
	StepIntoContextOfNoExpressions();
	VerifyExpressionsInCommentsNotAdded();
	VerifyDupExpressionsNotAdded();
	//VerifyExpressionsWithErrorValsNotAdded();

	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_SUCCESS(prj.Attach());
	EXPECT_SUCCESS(prj.Close());
	} 


/*******************************************
 * verify variables list updated after go. *
 *******************************************/
void CAutoPaneIDETest::VerifyUpdateAfterGo(void)
	{
	LogTestHeader("VerifyUpdateAfterGo");
	EXPECT_TRUE(dbg.Restart());
//	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(src.Find("*pi = 1;"));
	EXPECT_TRUE(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.Go());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "0");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "*pi", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	bps.ClearAllBreakpoints();
	RestoreFocusToSrcWnd();
	}


/**************************************************
 * verify variables list updated after step into. *
 **************************************************/
void CAutoPaneIDETest::VerifyUpdateAfterStepInto(void)
	{
	LogTestHeader("VerifyUpdateAfterStepInto");
	EXPECT_TRUE(src.Find("i = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "0");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "*pi", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/*************************************************
 * verify variables list updated after step out. *
 *************************************************/
void CAutoPaneIDETest::VerifyUpdateAfterStepOut(void)
	{
	LogTestHeader("VerifyUpdateAfterStepOut");
	EXPECT_TRUE(src.Find("b.func(0);"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.StepOut());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base", "b", "{...}");
	//FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "void", "base::func returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/**************************************************
 * verify variables list updated after step over. *
 **************************************************/
void CAutoPaneIDETest::VerifyUpdateAfterStepOver(void)
	{
	LogTestHeader("VerifyUpdateAfterStepOver");
	EXPECT_TRUE(src.Find("i = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "0");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "*pi", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/************************************************
 * verify variables list updated after restart. *
 ************************************************/
void CAutoPaneIDETest::VerifyUpdateAfterRestart(void)
	{
	LogTestHeader("VerifyUpdateAfterRestart");
	EXPECT_TRUE(dbg.Restart());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "void *", "hinst", "ADDRESS");
	FillExprInfo(expr_info_expected[1], COLLAPSED, "void *", "hinstPrev", "ADDRESS");
	FillExprInfo(expr_info_expected[2], COLLAPSED, "char *", "lszCmdLine", "ADDRESS");
	FillExprInfo(expr_info_expected[3], NOT_EXPANDABLE, "int", "nCmdShow", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 4));
	// restore execution to initial state (anywhere after WinMain's prolog).
	EXPECT_TRUE(dbg.StepOver());
	RestoreFocusToSrcWnd();
	}


/**********************************************
 * verify variables list updated after break. *
 **********************************************/
void CAutoPaneIDETest::VerifyUpdateAfterBreak(void)
	{
	LogTestHeader("VerifyUpdateAfterBreak");
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.Go(NULL, 0, NULL, WAIT_FOR_RUN));
	// wait until infinite loop (while(1);) is executed.
	Sleep(10000);
	EXPECT_TRUE(dbg.Break());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/******************************************************
 * verify variables list updated after run to cursor. *
 ******************************************************/
void CAutoPaneIDETest::VerifyUpdateAfterRunToCursor(void)
	{
	LogTestHeader("VerifyUpdateAfterRunToCursor");
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("*pi = 1;"));
	EXPECT_TRUE(dbg.StepToCursor());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "0");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "*pi", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/************************************************************
 * verify variables list updated after setd next statement. *
 ************************************************************/
void CAutoPaneIDETest::VerifyUpdateAfterSetNextStatement(void)
	{
	LogTestHeader("VerifyUpdateAfterSetNextStatement");
	EXPECT_TRUE(src.Find("i = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(src.Find("*pi = 1;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "0");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "*pi", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/****************************************************************
 * verify auto pane updated when variable changes via execution	*
 ****************************************************************/
void CAutoPaneIDETest::ChangeValueViaExecution(void)
	{
	LogTestHeader("ChangeValueViaExecution");
	EXPECT_TRUE(src.Find("int i2 = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	// step over "int i2 = 0;" and "i2 = 1;".
	EXPECT_TRUE(dbg.StepOver(2));
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i2", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}

/*********************************************************************
 * verify auto pane updated when variable changes via memory window. *
 *********************************************************************/
void CAutoPaneIDETest::ChangeValueViaMemWnd(void)
	{
	LogTestHeader("ChangeValueViaMemWnd");
	EXPECT_TRUE(src.Find("int i2 = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(mem.SetMemoryData("i2", 1));
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i2", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/********************************************************************
 * verify auto pane updated when variable changes via watch window.	*
 ********************************************************************/
void CAutoPaneIDETest::ChangeValueViaWatchWnd(void)
	{
	LogTestHeader("ChangeValueViaWatchWnd");
	EXPECT_TRUE(src.Find("int i2 = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uiwatch.Activate();
	EXPECT_SUCCESS(uiwatch.SetName("i2", 1));
	EXPECT_SUCCESS(uiwatch.SetValue("1", 1));
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i2", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	uiwatch.Activate();
	uiwatch.Delete(1);
	RestoreFocusToSrcWnd();
	}


/************************************************************************
 * verify auto pane updated when variable changes via quickwatch dlg.	*
 ************************************************************************/
void CAutoPaneIDETest::ChangeValueViaQuickWatchDlg(void)
	{
	LogTestHeader("ChangeValueViaQuickWatchDlg");
	EXPECT_TRUE(src.Find("int i2 = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(uiqw.Activate());
	EXPECT_TRUE(uiqw.SetExpression("i2"));
	EXPECT_TRUE(uiqw.Recalc());
	EXPECT_TRUE(uiqw.SetNewValue("1"));
	EXPECT_TRUE(uiqw.Close() == NULL);
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i2", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/******************************************************************************************
 * verify auto pane updated when variable changes via auto pane (ie. two different rows). *
 ******************************************************************************************/
void CAutoPaneIDETest::ChangeValueViaAutoPane(void)
	{
	LogTestHeader("ChangeValueViaAutoPane");
	EXPECT_TRUE(src.Find("i = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.SetValue("1", 1));
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "i", "1");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "*pi", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/**************************************************************
 * verify that function arguments appear when prolog reached. *
 **************************************************************/
void CAutoPaneIDETest::VerifyFuncArgsAppearWhenPrologReached(void)
	{
	}


/****************************************************
 * navigate the stack to another function and back.	*
 ****************************************************/
void CAutoPaneIDETest::NavigateStackToParentFuncAndBack(void)
	{
	}


/********************************************
 * change panes from auto to this and back.	*
 ********************************************/
void CAutoPaneIDETest::ChangeToThisPaneAndBack(void)
	{
	}


/**********************************************
 * change panes from auto to locals and back. *
 **********************************************/
void CAutoPaneIDETest::ChangeToLocalsPaneAndBack(void)
	{
	}


/*****************************************************************************************
 * verify that only expressions from current statement and previous statement are added. *
 *****************************************************************************************/
void CAutoPaneIDETest::VerifyOnlyCurrentAndPrevStatementsAreScanned(void)
	{
	}


/****************************************************************
 * verify that multi-line statements get scanned back 10 lines.	*
 ****************************************************************/
void CAutoPaneIDETest::VerifyMultilineStatementsGetScannedBackTenLines(void)
	{
	}


/******************************************************************************************
 * step into context of "array[i]".  only "array[i]" should show up, not "array" nor "i". *
 ******************************************************************************************/
void CAutoPaneIDETest::ScanArraySubscript(void)
	{
	LogTestHeader("ScanArraySubscript");
	EXPECT_TRUE(src.Find("array[0] = 'x';"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "array[0]", "120 'x'");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/***********************************************************************************
 * step into context of "array[i + (int)*pchar]", entire expression should show    *
 * up even though casts and arithmetic operators are unsupported outside brackets. *	
 ***********************************************************************************/
void CAutoPaneIDETest::ScanCastsAndOpsInsideArraySubscript(void)
	{
	LogTestHeader("ScanCastsAndOpsInsideArraySubscript");
	EXPECT_TRUE(src.Find("array[(5 + 15) / 10 * 2 - (4 + (int)'\x00')] = '1';"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "array[(5+15)/10*2-(4+(int)'\\x00')]", "49 '1'");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/*******************************************
 * step into context of "array[array[i]]". *
 *******************************************/
void CAutoPaneIDETest::ScanSelfSubscriptedArray(void)
	{
	LogTestHeader("ScanSelfSubscriptedArray");
	EXPECT_TRUE(src.Find("array[array[4]] = 'a';"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "char", "array[4]", "UNKNOWN");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "char", "array[array[4]]", "97 'a'");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/*******************************************************************************
 * step into context of indirect access to class member variable and function. *
 *******************************************************************************/
void CAutoPaneIDETest::ScanIndirectAccessToClassVariableAndFunc(void)
	{
	LogTestHeader("ScanIndirectAccessToClassVariableAndFunc");
	EXPECT_TRUE(src.Find("pb->m_public = 0;"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base *", "pb", "ADDRESS");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "pb->m_public", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/*****************************************************************************
 * step into context of direct access to class member variable and function. *
 *****************************************************************************/
void CAutoPaneIDETest::ScanDirectAccessToClassVariableAndFunc(void)
	{
	LogTestHeader("ScanDirectAccessToClassVariableAndFunc");
	EXPECT_TRUE(src.Find("b.m_public = 1;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "base", "b", "{...}");
	FillExprInfo(expr_info_expected[1], NOT_EXPANDABLE, "int", "b.m_public", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/*************************************
 * step into context of :: operator. *
 *************************************/
void CAutoPaneIDETest::ScanScopeOp(void)
	{
	LogTestHeader("ScanScopeOp");
	EXPECT_TRUE(src.Find("::global = 1;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "::global", "1");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/*************************************************
 * step into context of address-of (&) operator. *
 *************************************************/
void CAutoPaneIDETest::ScanAddressOfOp(void)
	{
	LogTestHeader("ScanAddressOfOp");
	EXPECT_TRUE(src.Find("int *pint = &i;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], COLLAPSED, "int *", "&i", "ADDRESS");
	FillExprInfo(expr_info_expected[1], COLLAPSED, "int *", "pint", "ADDRESS");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 2));
	RestoreFocusToSrcWnd();
	}


/*******************************************
 * step into context of pointer to member. *
 *******************************************/
void CAutoPaneIDETest::ScanPointerToMember(void)
	{
	}


/*************************************************
 * step in mixed mode - registers not supported. *
 *************************************************/
void CAutoPaneIDETest::VerifyRegistersNotAddedInMixedMode(void)
	{
	}


/********************************************************************
 * verify that expression names can’t be deleted, added, or edited.	*
 ********************************************************************/
void CAutoPaneIDETest::VerifyNamesCantBeManuallyDeletedAddedOrEdited(void)
	{
	}


/**************************************************
 * verify that no constant expressions are added. *
 **************************************************/
void CAutoPaneIDETest::VerifyConstantExpressionsNotAdded(void)
	{
	}


/********************************************
 * verify that function calls aren’t added.	*
 ********************************************/
void CAutoPaneIDETest::VerifyFuncCallsNotAdded(void)
	{
	}


/****************************************************************************
 * verify that the unsupported operators are not included 				    *
 * (arithmetic, casts, logical, bitwise, increment, decrement, relational).	*
 ****************************************************************************/
void CAutoPaneIDETest::VerifyUnsupportedOpsNotAdded(void)
	{
	}


/****************************************
 * step into context of no expressions.	*
 ****************************************/
void CAutoPaneIDETest::StepIntoContextOfNoExpressions(void)
	{
	LogTestHeader("StepIntoContextOfNoExpressions");
	EXPECT_TRUE(src.Find("while(0); // no expressions"));
	EXPECT_TRUE(dbg.SetNextStatement());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "", "", "");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/*****************************************************
 * verify that no expressions in comments are added. *
 *****************************************************/
void CAutoPaneIDETest::VerifyExpressionsInCommentsNotAdded(void)
	{
	LogTestHeader("VerifyExpressionsInCommentsNotAdded");
	EXPECT_TRUE(src.Find("int before_comments = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	// next few lines are commented symbols, then more code.
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "before_comments", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/****************************************************
 * verify that duplicate expressions are not added. *
 ****************************************************/
void CAutoPaneIDETest::VerifyDupExpressionsNotAdded(void)
	{
	LogTestHeader("VerifyDupExpressionsNotAdded");
	EXPECT_TRUE(src.Find("int before_comments = 0;"));
	EXPECT_TRUE(dbg.SetNextStatement());
	// next few lines are commented symbols, then more code.
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "before_comments", "0");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/***********************************************************
 * verify that no expressions with error values are added. *
 ***********************************************************/
void CAutoPaneIDETest::VerifyExpressionsWithErrorValsNotAdded(void)
	{
	}