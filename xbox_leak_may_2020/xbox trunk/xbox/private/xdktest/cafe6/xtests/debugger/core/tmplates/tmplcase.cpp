///////////////////////////////////////////////////////////////////////////////
//	TMPLCASE.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		Implementation of the CTemplatesIDETest Class

#include "stdafx.h"
#include "tmplcase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CTemplatesIDETest, CDbgTestBase, "Templates", -1, CTemplatesSubSuite)
									   
												 
void CTemplatesIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


void CTemplatesIDETest::Run(void)
	
{
	/******************
	 * INITIALIZATION *
	 ******************/

	if(!InitProject("tmplates\\tmplates", PROJECT_XBE))
	{
		m_pLog->RecordInfo("ERROR: could not init tmplates project");
		return;
	}

	EXPECT_TRUE(dbg.StepInto(2));

	/*********
	 * TESTS *
	 *********/
	StepIntoAndOutOfTemplateFunction();
	StepOverTemplateFunction();
	StepToCursorInTemplateFunction();
	SetBreakpointOnLineInTemplateFunctionWhileDebugging();
	SetBreakpointOnLineInTemplateFunctionWhileNotDebugging();
	VerifyTemplateParameterOnStack();
	GoToDisassemblyFromWithinTemplateFunction();
	StepIntoSpecificTemplateFunction();
	SetNextStatementInTemplateFunction();
	StepToCursorInTemplateFunctionInStack();
	StepToCursorInTemplateFunctionToStartDebugging();	
	HitBreakpointOnNonAmbiguousTemplateFunction();
	HitBreakpointOnAmbiguousTemplateFunction();
	DisableBreakpointInTemplateFunctionAndVerifyNotHit();
	ToggleOffBreakpointInTemplateFunctionAndVerifyNotHit();
	EvaluateSTLObjectWithLongName();
//end of tests
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, ASSUME_NORMAL_TERMINATION));
}


void CTemplatesIDETest::StepIntoAndOutOfTemplateFunction(void)
{
	
	LogTestHeader("StepIntoAndOutOfTemplateFunction");
	EXPECT_TRUE(src.Find("intObject.func(1);"));
	EXPECT_TRUE(dbg.SetNextStatement());

	// step into and verify source, stack.
	EXPECT_TRUE(dbg.StepInto(1, NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<int>::func(int 1)"));
	
	// step out of and verify source, stack.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //1", "WinMain("));
}


void CTemplatesIDETest::StepOverTemplateFunction(void)
{
	LogTestHeader("StepOverTemplateFunction");
	EXPECT_TRUE(src.Find("intObject.func(2);"));
	EXPECT_TRUE(dbg.SetNextStatement());
	// step over and verify source, stack
	EXPECT_TRUE(dbg.StepOver(1, NULL, "while(0); //2", "WinMain("));
}


void CTemplatesIDETest::StepToCursorInTemplateFunction(void)
{
	LogTestHeader("StepToCursorInTemplateFunction");
	EXPECT_TRUE(src.Find("charObject.func('b');"));
	EXPECT_TRUE(dbg.SetNextStatement());
	// step to cursor and resolve source line ambiguity
	EXPECT_TRUE(src.Find("// first line of template function 'func'"));
	EXPECT_TRUE(dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT));
	// choose the second (<char>) entry to resolve the ambiguity 
	EXPECT_TRUE(dbg.ResolveSourceLineAmbiguity(2));
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	// verity the line and function
	EXPECT_TRUE(dbg.CurrentLineIs("// first line of template function 'func'"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SomeReallyLongSymbolName<char>::func(char 98)"));
	// step out to WinMain to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //b", "WinMain("));
}


void CTemplatesIDETest::SetBreakpointOnLineInTemplateFunctionWhileDebugging(void)

{
	LogTestHeader("SetBreakpointOnLineInTemplateFunctionWhileDebugging");
	// set a bp within a template function
	EXPECT_TRUE(src.Find("// first line of template function 'func'"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	// set ip to function a place before all instances of the template.
	EXPECT_TRUE(src.Find("intObject.func(1);"));
	EXPECT_TRUE(dbg.SetNextStatement());

	// verify that the bp is hit for each instance of the template
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<int>::func(int 1)"));
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<int>::func(int 2)"));
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<char>::func(char 98)"));
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<char>::func(char 99)"));
	
	// step out to WinMain and clear breakpoint to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //c", "WinMain("));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


void CTemplatesIDETest::SetBreakpointOnLineInTemplateFunctionWhileNotDebugging(void)

{
	LogTestHeader("SetBreakpointOnLineInTemplateFunctionWhileNotDebugging");
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(src.Find("// first line of template function 'func'"));
	// set a bp within a template function
	EXPECT_VALIDBP(bps.SetBreakpoint());
	
	// verify that the bp is hit for each instance of the template
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<int>::func(int 1)"));
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<int>::func(int 2)"));
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<char>::func(char 98)"));
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<char>::func(char 99)"));
	
	// step out to WinMain and clear breakpoint to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //c", "WinMain("));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


void CTemplatesIDETest::VerifyTemplateParameterOnStack(void)

{
	LogTestHeader("VerifyTemplateParameterOnStack");
	EXPECT_TRUE(src.Find("FuncWithTemplateClassParam(intObject);"));
	EXPECT_TRUE(dbg.SetNextStatement());
	
	// step into and verify stack.
	EXPECT_TRUE(dbg.StepInto(1, NULL, 
		"// first line of FuncWithTemplateClassParam",
		"FuncWithTemplateClassParam(SomeReallyLongSymbolName<int> {...})"));
	
	// step out to WinMain to set up next test.
	CString strline;
	Frame frm( stk.GetFunction(1) );
	LOG->Comment("Frame no 1: %s",frm);
	frm.GetLineNumber(strline);
	strline = "@" + strline;
	LOG->Comment("Line to go from frame no 1: %s",strline);
	EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, "WinMain("));
	//EXPECT_TRUE(dbg.AtLine("while(0); //intObject") );
	EXPECT_TRUE(dbg.VerifyCurrentLine(strline) );
}


void CTemplatesIDETest::GoToDisassemblyFromWithinTemplateFunction(void)

{
	LogTestHeader("GoToDisassemblyFromWithinTemplateFunction");
	EXPECT_TRUE(src.Find("intObject.func(1);"));
	src.AttachActiveEditor();
	EXPECT_TRUE(dbg.SetNextStatement());

	// step into template function and go to disassembly (will need to resolve ambiguity).
	EXPECT_TRUE(dbg.StepInto(1, NULL,
		"// first line of template function 'func'",
		"SomeReallyLongSymbolName<int>::func(int 1)"));
//	EXPECT_TRUE(dbg.SetSteppingMode(ASM)); //Goto DAM from menu works in IP context - not ambiguous
	src.TypeTextAtCursor("{HOME}+{F10}",FALSE);
	Sleep(2000);
	MST.WMenu("Go To Disassembly"); //only Goto DAM from source context is ambiguous
	// choose the first (<int>) entry to resolve the ambiguity. 
	EXPECT_TRUE(dbg.ResolveSourceLineAmbiguity(1));
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	// restore source mode
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	// step out to WinMain to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //1", "WinMain("));
}



void CTemplatesIDETest::StepIntoSpecificTemplateFunction(void)
{
	int nline;
	LogTestHeader("StepIntoSpecific Member of TemplateClass");
	
	EXPECT_TRUE(src.Find("charObject.func('c');"));
	src.AttachActiveEditor();
	EXPECT_TRUE(dbg.SetNextStatement());
	src.TypeTextAtCursor("{Home}",FALSE);
	nline = UIWB.GetEditorCurPos(GECP_LINE); /*GetLineNumber()*/
	EXPECT_TRUE(src.Find("func")); //only function name shld be highlighted
	EXPECT_TRUE(nline == UIWB.GetEditorCurPos(GECP_LINE) ); //line # didn't change
	src.TypeTextAtCursor("+{F10}", FALSE);Sleep(2000);
	MST.WMenu("Ste&p into SomeReallyLongSymbolName<char>::func");
	//no ambiguity here
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.CurrentLineIs("{ // first line of template function 'func'") );
	// step out to WinMain to set up next test.
	EXPECT_TRUE( (nline=stk.NavigateStack(1)) > 0); //check where we are going to step out
	CString strnumber;strnumber.Format("@%d",nline);
	EXPECT_TRUE(dbg.StepOut(1, NULL, strnumber));


	LogTestHeader("StepIntoSpecific Specialization of TemplateFunction");
	
	EXPECT_TRUE(src.Find("globaltemplatefunc(global_float"));
	EXPECT_TRUE(dbg.SetNextStatement());
	src.TypeTextAtCursor("{Home}",FALSE);
	nline = UIWB.GetEditorCurPos(GECP_LINE); /*GetLineNumber()*/
	EXPECT_TRUE(src.Find("globaltemplatefunc")); //only function name shld be highlighted
	EXPECT_TRUE(nline == UIWB.GetEditorCurPos(GECP_LINE) ); //line # didn't change
	src.TypeTextAtCursor("+{F10}", FALSE);Sleep(2000);
	MST.WMenu("Ste&p into globaltemplatefunc");
	//no ambiguity here
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.CurrentLineIs("{ // first line of float specialization of globaltemplatefunc") );
	// step out to WinMain to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "globaltemplatefunc(global_float"));
}

void CTemplatesIDETest::SetNextStatementInTemplateFunction(void)

{
	LogTestHeader("SetNextStatementInTemplateFunction");
	EXPECT_TRUE(src.Find("intObject.func(1);"));
	EXPECT_TRUE(dbg.SetNextStatement());
	
	// step into and verify source, stack.
	EXPECT_TRUE(dbg.StepInto(2, NULL,
		"m_Type = var;", 
		"SomeReallyLongSymbolName<int>::func(int 1)"));

	// set next statement and verify source and stack
	EXPECT_TRUE(src.Find("in template function 'func'"));
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.ResolveSourceLineAmbiguity(1));
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.CurrentLineIs("// in template function 'func'"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SomeReallyLongSymbolName<int>::func(int 1)"));
	// step out to WinMain to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //1", "WinMain("));
}


void CTemplatesIDETest::StepToCursorInTemplateFunctionInStack(void)
{
	LogTestHeader("StepToCursorInTemplateFunctionInStack");
	EXPECT_TRUE(src.Find("intObject.func(2);"));
	EXPECT_TRUE(dbg.SetNextStatement());

	// run to a function that was called by a template function
	EXPECT_TRUE(src.Find("void globalfunc(void){}"));
	EXPECT_TRUE(dbg.StepToCursor());
	// navigate to the template function
	EXPECT_TRUE(stk.NavigateStack("SomeReallyLongSymbolName<int>::func(int 2)")>0);
	EXPECT_TRUE(stk.NavigateStack(0)>0); //return to EIP
	// run to the template function in the stack, verify source and stack.
	// no ambiguity here
	EXPECT_TRUE(stk.RunToFrame("SomeReallyLongSymbolName<int>::func(int 2)", 0));
	EXPECT_TRUE(dbg.CurrentLineIs("// in template function 'func'"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SomeReallyLongSymbolName<int>::func(int 2)"));
	// step out to WinMain to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //2", "WinMain("));
}


void CTemplatesIDETest::StepToCursorInTemplateFunctionToStartDebugging(void)
{
	LogTestHeader("StepToCursorInTemplateFunctionToStartDebugging");
	EXPECT_TRUE(dbg.StopDebugging());
	// step to cursor into the template function to start debugging..
	EXPECT_TRUE(src.Find("// first line of template function 'func'"));
	EXPECT_TRUE(dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT));
	// choose the second (<char>) entry to resolve the ambiguity 
	EXPECT_TRUE(dbg.ResolveSourceLineAmbiguity(2));
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	// verity the line and function
	EXPECT_TRUE(dbg.CurrentLineIs("// first line of template function 'func'"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SomeReallyLongSymbolName<char>::func(char 98)"));
	// step out to WinMain to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //b", "WinMain("));
}


void CTemplatesIDETest::HitBreakpointOnNonAmbiguousTemplateFunction(void)

{
	LogTestHeader("HitBreakpointOnNonAmbiguousTemplateFunction");
	// set a bp on a template function
	EXPECT_VALIDBP(bps.SetBreakpoint("globaltemplatefunc(int)"));
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of globaltemplatefunc", 
		"globaltemplatefunc(int 1)"));
	
	// step out to WinMain to set up next test.
	CString strline;
	Frame frm( stk.GetFunction(1) );
	LOG->Comment("Frame no 1: %s",frm);
	frm.GetLineNumber(strline);
	strline = "@" + strline;
	LOG->Comment("Line to go from frame no 1: %s",strline);
	EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, "WinMain("));
	//EXPECT_TRUE(dbg.AtLine("while(0); //::1") );
	EXPECT_TRUE(dbg.VerifyCurrentLine(strline) );

	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


void CTemplatesIDETest::HitBreakpointOnAmbiguousTemplateFunction(void)

{
	LogTestHeader("HitBreakpointOnAmbiguousTemplateFunction");
	// set a bp on a template function (char)
	EXPECT_VALIDBP(bps.SetAmbiguousBreakpoint("globaltemplatefunc", 4)); //TODO: choose it from string (char) rather than position in the list
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of globaltemplatefunc", 
		"globaltemplatefunc(char 97)"));
	
	// step out to WinMain to set up next test.
	CString strline;
	Frame frm( stk.GetFunction(1) );
	LOG->Comment("Frame no 1: %s",frm);
	frm.GetLineNumber(strline);
	strline = "@" + strline;
	LOG->Comment("Line to go from frame no 1: %s",strline);
	EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, "WinMain("));
	//EXPECT_TRUE(dbg.AtLine("while(0); //::a") );
	EXPECT_TRUE(dbg.VerifyCurrentLine(strline) );

	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


void CTemplatesIDETest::DisableBreakpointInTemplateFunctionAndVerifyNotHit(void)

{
	LogTestHeader("DisableBreakpointInTemplateFunctionAndVerifyNotHit");
	// set a bp within a template function
	EXPECT_TRUE(src.Find("// first line of template function 'func'"));
	bp *pbp = bps.SetBreakpoint();
	// two bp's were set, disable one of them (<int>)
	// TODO(michma): two bps were set but we only get one bp object.
	// the first one gets disabled because of the way cafe indexes bp
	// objects. we need a disable breakpoint function that takes indexes.
	EXPECT_TRUE(bps.DisableBreakpoint(pbp));
	// set ip to function a place before the breakpoints.
	EXPECT_TRUE(src.Find("intObject.func(1);"));
	EXPECT_TRUE(dbg.SetNextStatement());

	// verify that the bp is hit for each instance of the template not disabled (<char>)
	// the other earlier ones (<int>) should get passed up.
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<char>::func(char 98)"));
	EXPECT_TRUE(dbg.Go(NULL, 
		"// first line of template function 'func'", 
		"SomeReallyLongSymbolName<char>::func(char 99)"));
	
	// step out to WinMain and clear breakpoint to set up next test.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "while(0); //c", "WinMain("));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


void CTemplatesIDETest::ToggleOffBreakpointInTemplateFunctionAndVerifyNotHit(void)

{
	LogTestHeader("ToggleOffBreakpointInTemplateFunctionAndVerifyNotHit");
	// toggle on a bp within a template function
	EXPECT_TRUE(src.Find("// first line of template function 'func'"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	// toggle it off.
	// TODO(michma): SetBreakpoint isn't smart enough to know when we're
	// toggling a bp off. we shouldn't be getting back a valid bp.
	EXPECT_VALIDBP(bps.SetBreakpoint());
	// go and make sure we don't hit the breakpoints we just toggled off.
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION));
	// step past prolog of WinMain to set up next test.
	EXPECT_TRUE(dbg.StepInto(2));
}


void CTemplatesIDETest::EvaluateSTLObjectWithLongName(void)
{
	LogTestHeader("EvaluateSTLObjectWithLongName");
	EXPECT_TRUE(cxx.ExpressionValueIs("STLObjectWithLongName._Tr", "{...}"));
}


