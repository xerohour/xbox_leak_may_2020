///////////////////////////////////////////////////////////////////////////////
//	Namecase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script.	 

#include "stdafx.h"
#include "Namecase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
										   
IMPLEMENT_TEST(CNameSpceIDETest, CDebugTestSet, "Namespace", -1, CNameSpceSubSuite)

												 
void CNameSpceIDETest::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CNameSpceIDETest::Run(void)
	
	{
	/******************
	 * INITIALIZATION *
	 ******************/
	if(!InitProject("namspace\\namspace"))
	{
		m_pLog->RecordInfo("ERROR: could not init namspace project");
		return;
	}

	// this is the initial state each test expects.
	//	  - step passed prolog of WinMain().
	if(!dbg.StepOver(2))
	{
		m_pLog->RecordInfo("ERROR: could not step passed WinMain()'s prolog");
		return;
	}

	/*********
	 * TESTS *
	 *********/
	EvaluateNamespaceFuncOutsideNamespace();
	EvaluateNamespaceVarOutsideNamespace();
	EvaluateNestedNamespaceVarOutsideNamespace();
	EvaluateNamespaceMemberInsideNamespace();
	GoToLocationBpOnNamespaceFunc();
	GoToDataBpOnNamespaceVar();
	NavigateMemoryWndToNamespaceVar();
	VerifyReturnValueInfoIncludesNamespace();
	NavigateStackOutOfAndBackIntoNamespaceFunc();
	ImplicitlyReferenceNamespaceMemberAfterUsingDirective();
	ImplicitlyReferenceNamespaceMemberInsideNamespace();
	} 


/************************************************************************
 * evaluate explicitly referenced namespace variable outside namespace. *
 ************************************************************************/
void CNameSpceIDETest::EvaluateNamespaceVarOutsideNamespace(void)
	{
	LogTestHeader("EvaluateNamespaceVarOutsideNamespace");
	EXPECT_TRUE(cxx.ExpressionValueIs("n1::i", 1));
	}


/******************************************************
 * evaluate explicitly referenced namespace function. *
 ******************************************************/
void CNameSpceIDETest::EvaluateNamespaceFuncOutsideNamespace(void)
	{
	LogTestHeader("EvaluateNamespaceFuncOutsideNamespace");
	EXPECT_TRUE(cxx.ExpressionValueIs("n1::func()", "<void>"));
	}


/*************************************************************
 * evaluate explicitly referenced nested namespace variable. *
 *************************************************************/
void CNameSpceIDETest::EvaluateNestedNamespaceVarOutsideNamespace(void)
	{
	LogTestHeader("EvaluateNestedNamespaceVarOutsideNamespace");
	EXPECT_TRUE(cxx.ExpressionValueIs("n1::n2::i", 2));
	}


/*********************************************************************
 * evaluate explicitly referenced namespace member inside namespace. *
 *********************************************************************/
void CNameSpceIDETest::EvaluateNamespaceMemberInsideNamespace(void)
	{
	LogTestHeader("EvaluateNamespaceMemberInsideNamespace");
	// step into namespace function (n1::func()).
	EXPECT_TRUE(src.Find("func();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(cxx.ExpressionValueIs("n1::i", 1));
	// restore stack to initial state.
	EXPECT_TRUE(dbg.StepOut());
	}


/********************************************************************************
 * set location bp on explicitly referenced namespace function, go, verify hit. *
 ********************************************************************************/
void CNameSpceIDETest::GoToLocationBpOnNamespaceFunc(void)
	
	{
	LogTestHeader("GoToLocationBpOnNamespaceFunc");
	EXPECT_TRUE(dbg.Restart());
	EXPECT_VALIDBP(bps.SetBreakpoint("n1::func"));

	if(GetUserTargetPlatforms() == PLATFORM_WIN32_MIPS || GetUserTargetPlatforms() == PLATFORM_WIN32_ALPHA)	
		dbg.Go();
	else
		{EXPECT_TRUE(dbg.Go(NULL, "first line of n1::func()"));}
	
	// restore stack and bp list to initial state.
	EXPECT_TRUE(dbg.StepOut());
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	}


/****************************************************************************
 * set data bp on explicitly referenced namespace variable, go, verify hit. *
 ****************************************************************************/
void CNameSpceIDETest::GoToDataBpOnNamespaceVar(void)
	{
	LogTestHeader("GoToDataBpOnNamespaceVar");
	EXPECT_TRUE(dbg.Restart());
	EXPECT_VALIDBP(bps.SetBreakOnExpr("n1::j", COBP_TYPE_IF_EXP_CHANGED));
	EXPECT_TRUE(dbg.Go(NULL, "line after n1::j changed"));
	// restore bp list to initial state.
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	}


/***********************************************************************
 * navigate memory window to explicitly referenced namespace variable. *
 ***********************************************************************/
void CNameSpceIDETest::NavigateMemoryWndToNamespaceVar(void)
	{
	LogTestHeader("NavigateMemoryWndToNamespaceVar");
	EXPECT_TRUE(mem.MemoryDataIs("n1::i", 1));
	RestoreFocusToSrcWnd();
	}


/**************************************************************************************
 * verify return value info includes namespace when stepping over namespace function. *
 **************************************************************************************/
void CNameSpceIDETest::VerifyReturnValueInfoIncludesNamespace(void)
	{
	LogTestHeader("VerifyReturnValueInfoIncludesNamespace");
	// step over namespace function (n1::func()).
	EXPECT_TRUE(src.Find("func();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver());
	uivar.Activate();
	EXPECT_SUCCESS(uivar.SetPane(PANE_AUTO));
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, ROW_LAST, 1));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "void", "n1::func returned", "<void>");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));
	RestoreFocusToSrcWnd();
	}


/******************************************************
 * navigate stack into and out of namespace function. *
 ******************************************************/
/***********************************************************************
 * verify stack window includes namespace when for namespace function. *
 ***********************************************************************/
void CNameSpceIDETest::NavigateStackOutOfAndBackIntoNamespaceFunc(void)
	
	{
	LogTestHeader("NavigateStackOutOfAndBackIntoNamespaceFunc");
	
	// step into namespace function (n1::func()).
	EXPECT_TRUE(src.Find("func();"));
	EXPECT_TRUE(dbg.SetNextStatement());

	if(GetUserTargetPlatforms() == PLATFORM_WIN32_MIPS || GetUserTargetPlatforms() == PLATFORM_WIN32_ALPHA)	
		{EXPECT_TRUE(dbg.StepInto(2));}
	else
		{EXPECT_TRUE(dbg.StepInto(3));}

	// navigate to WinMain() and back to namespace function.
	EXPECT_TRUE(stk.NavigateStack(1)>0);
	EXPECT_TRUE(stk.NavigateStack(0)>0);
	EXPECT_TRUE(stk.CurrentFunctionIs("n1::func()"));

	// verify locals pane updated.
	uivar.Activate();
	EXPECT_SUCCESS(uivar.SetPane(PANE_LOCALS));
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, "int", "local", "3");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1));

	// restore stack to initial state.
	EXPECT_TRUE(dbg.StepOut());
	RestoreFocusToSrcWnd();
	}


/***********************************************************
 * verify debugger doesn’t recognize implicitly referenced *
 * namespace member after using directive.                 *
 ***********************************************************/
void CNameSpceIDETest::ImplicitlyReferenceNamespaceMemberAfterUsingDirective(void)
	{
	LogTestHeader("ImplicitlyReferenceNamespaceMemberAfterUsingDirective");
	// should evaluate ::i instead of n1::i.
	EXPECT_TRUE(cxx.ExpressionValueIs("i", 0));
	}


/**********************************************************************************************
 * verify debugger doesn’t recognize implicitly referenced namespace member inside namespace. *
 **********************************************************************************************/
void CNameSpceIDETest::ImplicitlyReferenceNamespaceMemberInsideNamespace(void)
	{
	LogTestHeader("ImplicitlyReferenceNamespaceMemberInsideNamespace");
	// step into namespace function (n1::func()).	
	EXPECT_TRUE(src.Find("func();"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());
	// should evaluate ::i instead of n1::i.
	EXPECT_TRUE(cxx.ExpressionValueIs("i", 0));
	// restore stack to initial state.
	EXPECT_TRUE(dbg.StepOut());
	}










