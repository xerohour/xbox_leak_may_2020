///////////////////////////////////////////////////////////////////////////////
//	ISAPIDebuggingCases.cpp
//
//	Created by: MichMa		Date: 11/20/97
//
//	Description:
//		Implementation of CISAPIDebuggingCases

#include "stdafx.h"
#include "ISAPIDebuggingCases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CISAPIDebuggingCases, CTest, "ISAPI Debugging Scenario", -1, CISAPIDebuggingSubsuite)


void CISAPIDebuggingCases::Run(void)

{
	// support layer objects we will use for this scenario.
	COProject prj;
	COHTML html;
	CODebug dbg;
	COBreakpoints bps;
	COStack stk;
	COSource src;
	COExpEval ee;
	COLocals locals;
	CORegisters regs;
	CODAM dam;
	COMemory mem;

	// we need to know the system drive, system root, and computer name to figure out where to put 
	// the isapi extension dll, the html page that calls it, and also how to browse the page.
	char szBuffer[_MAX_PATH];
	GetEnvironmentVariable("SystemDrive", szBuffer, _MAX_PATH);
	CString strSysDrive = szBuffer;
	GetEnvironmentVariable("SystemRoot", szBuffer, _MAX_PATH);
	CString strSysRoot = szBuffer;
	GetEnvironmentVariable("COMPUTERNAME", szBuffer, _MAX_PATH);
	CString strComputerName = szBuffer;

	// set the base name for the project and html page we will be using, dependent on the os language.
	// TODO(michma 11/21/97): does iis/ie support spaces and/or mbcs chars in isapi extensions or html pages?
	CString strProjBase;
	//if(GetSystem() & SYSTEM_DBCS)
	//	strProjBase = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü ISAPI Extension";
	//else
		strProjBase = "ISAPIExtension";

	// clean up the last isapi extension project used (if any).
	KillAllFiles(strProjBase);
	
	// set the name and location of the atl com server project. we also want the project to be added to
	// the current workspace, and made a subproject of the main app project.
	CProjWizOptions *pISAPIExtensionWizOpt = new(CISAPIExtensionWizOptions);
	pISAPIExtensionWizOpt->m_strLocation = GetCWD();
	pISAPIExtensionWizOpt->m_strName = strProjBase;	
	
	// create the isapi extension project.
	if(prj.New(pISAPIExtensionWizOpt) == ERROR_SUCCESS)
		LOG->RecordSuccess("Create ISAPI Extension project named '%s'.", strProjBase);
	else
	{
		LOG->RecordFailure("Create ISAPI Extension project named '%s'.", strProjBase);
		return;
	}
	
	// open the main source file of the isapi extension project.
	if(src.Open(strProjBase + ".cpp") == ERROR_SUCCESS)
		LOG->RecordSuccess("Open source file '%s.cpp'.", strProjBase);
	else
	{
		LOG->RecordFailure("Open source file '%s.cpp'.", strProjBase);
		return;
	}

	// find the Default function of the ISAPI extension so we can add code in and around it.
	if(src.Find("void CISAPIExtensionExtension::Default"))
		LOG->RecordSuccess("Find Default function of ISAPI extension.");
	else
	{
		LOG->RecordFailure("Find Default function of ISAPI extension.");
		return;
	}

	// add code in and around the Default function of the ISAPI extension.
	LOG->RecordInfo("Adding code in and around the Default function of the ISAPI extension.");
	src.TypeTextAtCursor("{HOME}int global = 0;{ENTER 2}");
	src.TypeTextAtCursor("void func(void)", TRUE);
	src.TypeTextAtCursor("{ENTER}");
	src.TypeTextAtCursor("{", TRUE);
	src.TypeTextAtCursor("{TAB}// first line of func.{ENTER}");
	src.TypeTextAtCursor("global");
	src.TypeTextAtCursor("++;", TRUE);
	src.TypeTextAtCursor("{ENTER}");
	src.TypeTextAtCursor("}", TRUE);
	src.TypeTextAtCursor("{TAB}// line after global changes.{ENTER 2}{DOWN 2}");
	src.TypeTextAtCursor("{TAB}int local = 0;{ENTER}{DOWN 2}");
	src.TypeTextAtCursor("func();", TRUE);
	src.TypeTextAtCursor("{TAB}// first call to func.{ENTER}");
	src.TypeTextAtCursor("func();", TRUE);
	src.TypeTextAtCursor("{TAB}// second call to func.{ENTER}");
	src.TypeTextAtCursor("while(0);", TRUE);
	src.TypeTextAtCursor("{TAB}// line after second call to func.{ENTER}{DOWN 4}");
	src.TypeTextAtCursor("local");
	src.TypeTextAtCursor("++;", TRUE);
	src.TypeTextAtCursor("{ENTER}");

	// build the project.
	if(prj.Build() == ERROR_SUCCESS)
		LOG->RecordSuccess("Build '%s' project.", strProjBase);
	else
	{
		LOG->RecordFailure("Build '%s' project.", strProjBase);
		return;
	}
	
	// verify no errors or warnings were generated.
	int iErr, iWarn;
	if(prj.VerifyBuild(TRUE, &iErr, &iWarn, TRUE) == ERROR_SUCCESS)
		LOG->RecordSuccess("Verify build of '%s' project.", strProjBase);
	else
	{
		LOG->RecordFailure("Verify build of '%s' project.", strProjBase);
		return;
	}

	// copy the isapi extension dll to the scripts directory of the server (overwrite if it exists).
	if(CopyFile(strProjBase + "\\Debug\\" + strProjBase + ".dll", 
				strSysDrive + "\\InetPub\\scripts\\" + strProjBase + ".dll", FALSE))
		LOG->RecordSuccess("Copy ISAPI extension '%s' to the scripts directory of the server.", strProjBase + ".dll");
	else
	{
		LOG->RecordFailure("Copy ISAPI extension '%s' to the scripts directory of the server.", strProjBase + ".dll");
		return;
	}

	// create the html page that will call the isapi extension.
	if(html.Create(strProjBase, strProjBase) == ERROR_SUCCESS)
		LOG->RecordSuccess("Create HTML page named '%s.htm'.", strProjBase);
	else
	{
		LOG->RecordFailure("Create HTML page named '%s.htm'.", strProjBase);
		return;
	}

	// add the form that calls the isapi extension to the html page.
	LOG->RecordInfo("Adding the form that calls the ISAPI Extension to the HTML page.");
	html.TypeTextAtCursor("<form METHOD=\"POST\" ACTION=\"\\scripts\\" + strProjBase + ".dll\">{ENTER}");
	html.TypeTextAtCursor("<input type=\"submit\" value=\"Submit\">{ENTER}");
	html.TypeTextAtCursor("</form>{ENTER}");
	
	// save the changes to the html page.
	if(html.Save() == ERROR_SUCCESS)
		LOG->RecordSuccess("Saving changes to '%s.htm' HTML page.", strProjBase);
	else
	{
		LOG->RecordFailure("Saving changes to '%s.htm' HTML page.", strProjBase);
		return;
	}

	// copy the html page to the www root of the server (overwrite if it exists).
	if(CopyFile(strProjBase + "\\" + strProjBase + ".htm",
				strSysDrive + "\\InetPub\\wwwroot\\" + strProjBase + ".htm", FALSE))
		LOG->RecordSuccess("Copy HTML page '%s' to the www root of the server.", strProjBase + ".htm");
	else
	{
		LOG->RecordFailure("Copy HTML page '%s' to the www root of the server.", strProjBase + ".htm");
		return;
	}

	// set the executable for the debug session to be inetinfo.exe.
	if(prj.SetLocalTarget(strSysRoot + "\\system32\\inetsrv\\inetinfo.exe") == ERROR_SUCCESS)
		LOG->RecordSuccess("Set executable for debug session to 'inetinfo.exe'.");
	else
	{
		LOG->RecordFailure("Set executable for debug session to 'inetinfo.exe'.");
		return;
	}

	// set the program arguments for inetinfo.exe.
	if(prj.SetProgramArguments("-e W3Svc") == ERROR_SUCCESS)
		LOG->RecordSuccess("Set program arguments for 'inetinfo.exe' to '-e W3Svc'.");
	else
	{
		LOG->RecordFailure("Set program arguments for 'inetinfo.exe' to '-e W3Svc'.");
		return;
	}

	// set a bp in the Default function of the isapi extension.
	if(bps.SetBreakpoint("C" + strProjBase + "Extension::Default"))
		LOG->RecordSuccess("Set a bp in the 'Default' function of the ISAPI extension.");
	else
	{
		LOG->RecordFailure("Set a bp in the 'Default' function of the ISAPI extension.");
		return;
	}

	// launch inetinfo.exe under the debugger.
	if(dbg.Go(NULL, NULL, NULL, WAIT_FOR_RUN))
		LOG->RecordSuccess("Launch 'inetinfo.exe' under the debugger.");
	else
	{
		LOG->RecordFailure("Launch 'inetinfo.exe' under the debugger.");
		return;
	}

	// give inetinfo.exe a few seconds to publish the server. if we launch internet explorer too quickly after
	// inetinfo.exe, the server won't be published yet and ie won't find the html page.
	Sleep(10000);

	// CreateProcess() params for internet explorer.
   	STARTUPINFO siStartInfo;
	PROCESS_INFORMATION piProcInfo;
	// this is the minium initialization of STARTUPINFO required for CreateProcess() to work.
	memset(&siStartInfo, 0, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.wShowWindow = SW_SHOWDEFAULT;
	// create the command line that will launch internet explorer and browse the html page that calls the isapi extension.
	strcpy(szBuffer, "iexplore.exe http://" + strComputerName + "/" + strProjBase + ".htm");

	// launch internet explorer and browse the html page that calls the isapi extension.
	SetLastError(0);
	
	if(CreateProcess(NULL, szBuffer, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo))
		LOG->RecordSuccess("Launch Internet Explorer and browse HTML page '%s.htm'.", strProjBase);
	else
	{
		LOG->RecordFailure("Launch Internet Explorer and browse HTML page '%s.htm'. Error == %d.", 
			strProjBase, GetLastError());
		return;
	}
	
	// wait around 60 seconds looking for internet explorer with the html page loaded.
	// the default name that visual studio gives new html pages is "Document Title".
	if(MST.WFndWndWait("Document Title", FW_PART, 60))
		LOG->RecordSuccess("Verify that Internet Explorer has loaded html page '%s.htm'.", strProjBase);		
	else
	{
		LOG->RecordFailure("Verify that Internet Explorer has loaded html page '%s.htm'.", strProjBase);		
		return;
	}

	// submit html page form that calls the isapi extension (3 tabs puts focus on the "Submit" button).
	MST.DoKeys("{TAB 3}");
	MST.DoKeys("{ENTER}");

	// wait around 5 seconds in case a security alert dlg comes up.
	if(MST.WFndWndWaitC(GetLocString(IDSS_YES), "Button", FW_DEFAULT, 5))
		MST.WButtonClick(GetLocString(IDSS_YES));

	// wait for the debugger to break in the isapi extension's Default function.
	if(dbg.Wait(WAIT_FOR_BREAK))
		LOG->RecordSuccess("Wait for debugger to break in ISAPI extension's 'Default' function.");		
	else
	{
		LOG->RecordFailure("Wait for debugger to break in ISAPI extension's 'Default' function.");		
		return;
	}

	// verify that the current function is the ISAPI Extension's Default function.
	if(stk.CurrentFunctionIs("C" + strProjBase + "Extension::Default"))
		LOG->RecordSuccess("Verify current function is ISAPI Extension's 'Default' function.");		
	else
	{
		LOG->RecordFailure("Verify current function is ISAPI Extension's 'Default' function.");		
		return;
	}

	// enable disassembly debugging.
	if(dam.Enable())
		LOG->RecordSuccess("Enable disassembly debugging.");
	else
	{
		LOG->RecordFailure("Enable disassembly debugging.");
	}

	// enable access to registers.
	if(regs.Enable())
		LOG->RecordSuccess("Enable access to registers.");
	else
	{
		LOG->RecordFailure("Enable access to registers.");
	}
	
	// get back into source mode.
	dbg.SetSteppingMode(SRC);

	// find the first call to the function we added ("func()").
	if(src.Find("func();"))
		LOG->RecordSuccess("Find first call to function we added (\"func()\").");
	else
	{
		LOG->RecordFailure("Find first call to function we added (\"func()\").");
		return;
	}
	
	// step to cursor.
	if(dbg.StepToCursor())
		LOG->RecordSuccess("Step to cursor.");
	else
	{
		LOG->RecordFailure("Step to cursor.");
		return;
	}

	// verify debugger landed on first call to func().
	if(dbg.CurrentLineIs("first call to func"))
		LOG->RecordSuccess("Verify debugger landed on first call to func().");
	else
	{
		LOG->RecordFailure("Verify debugger landed on first call to func().");
		return;
	}

	// step over a function.
	if(dbg.StepOver())
		LOG->RecordSuccess("Step over a function.");
	else
	{
		LOG->RecordFailure("Step over a function.");
		return;
	}

	// verify debugger landed on second call to func().
	if(dbg.CurrentLineIs("second call to func"))
		LOG->RecordSuccess("Verify debugger landed on second call to func().");
	else
	{
		LOG->RecordFailure("Verify debugger landed on second call to func().");
		return;
	}

	// step into a function.
	if(dbg.StepInto())
		LOG->RecordSuccess("Step into a function.");
	else
	{
		LOG->RecordFailure("Step into a function.");
		return;
	}

	// verify debugger landed on first line of func().
	if(dbg.CurrentLineIs("first line of func"))
		LOG->RecordSuccess("Verify debugger landed on first line of func().");
	else
	{
		LOG->RecordFailure("Verify debugger landed on first line of func().");
		return;
	}

	// verify current function is func().
	if(stk.CurrentFunctionIs("func"))
		LOG->RecordSuccess("Verify current function is func().");		
	else
	{
		LOG->RecordFailure("Verify current function is func().");		
		return;
	}

	// step out of a function.
	if(dbg.StepOut())
		LOG->RecordSuccess("Step out of a function.");
	else
	{
		LOG->RecordFailure("Step out of a function.");
		return;
	}

	// verify debugger landed on line after second call to func().
	if(dbg.CurrentLineIs("line after second call to func"))
		LOG->RecordSuccess("Verify debugger landed on line after second call to func().");
	else
	{
		LOG->RecordFailure("Verify debugger landed on line after second call to func().");
		return;
	}

	// verify that the current function is the ISAPI Extension's Default function.
	if(stk.CurrentFunctionIs("C" + strProjBase + "Extension::Default"))
		LOG->RecordSuccess("Verify current function is ISAPI Extension's 'Default' function.");		
	else
	{
		LOG->RecordFailure("Verify current function is ISAPI Extension's 'Default' function.");		
		return;
	}

	// find the first call to func() again. the search should wrap here.
	if(src.Find("func();"))
		LOG->RecordSuccess("Find first call to func() again.");
	else
	{
		LOG->RecordFailure("Find first call to func() again.");
		return;
	}

	// set next statement.
	if(dbg.SetNextStatement())
		LOG->RecordSuccess("Set next statement.");
	else
	{
		LOG->RecordFailure("Set next statement.");
		return;
	}

	// verify the debugger landed on the first call to func().
	if(dbg.CurrentLineIs("first call to func"))
		LOG->RecordSuccess("Verify debugger landed on first call to func().");
	else
	{
		LOG->RecordFailure("Verify debugger landed on first call to func().");
		return;
	}

	// set a global data breakpoint.
	if(bps.SetBreakOnExpr("global", COBP_TYPE_IF_EXP_CHANGED))
		LOG->RecordSuccess("Set a global data breakpoint.");
	else
	{
		LOG->RecordFailure("Set a global data breakpoint.");
		return;
	}

	// go until the global data breakpoint is hit.
	if(dbg.Go())
		LOG->RecordSuccess("Go until the global data breakpoint is hit.");
	else
	{
		LOG->RecordFailure("Go until the global data breakpoint is hit.");
		return;
	}
	
	// verify the value of the global.
	if(ee.ExpressionValueIs("global", 3))
		LOG->RecordSuccess("Verify that value of global is 3.");		
	else
	{
		LOG->RecordFailure("Verify that value of global is 3.");		
		return;
	}

	// verify the debugger landed on the line after the global changes.
	if(dbg.CurrentLineIs("line after global changes"))
		LOG->RecordSuccess("Verify debugger landed on line after global changes.");
	else
	{
		LOG->RecordFailure("Verify debugger landed on line after global changes.");
		return;
	}

	// verify that the current function is func().
	if(stk.CurrentFunctionIs("func"))
		LOG->RecordSuccess("Verify current function is func().");		
	else
	{
		LOG->RecordFailure("Verify current function is func().");		
		return;
	}

	// navigate the stack back to the Default function.
	if(stk.NavigateStack(1))
		LOG->RecordSuccess("Navigate the stack back to the ISAPI Extension's 'Default' function.");		
	else
	{
		LOG->RecordFailure("Navigate the stack back to the ISAPI Extension's 'Default' function.");		
		return;
	}

	// verify the current line is the second call to func().
	if(dbg.CurrentLineIs("second call to func"))
		LOG->RecordSuccess("Verify current line is the second call to func().");
	else
	{
		LOG->RecordFailure("Verify current line is the second call to func().");
		return;
	}

	// verify that the local variable 'local' exists.
	if(locals.VerifyLocalInfo("local", "0", "int", NOT_EXPANDABLE))
		LOG->RecordSuccess("Verify that the local variable 'local' exists.");
	else
	{
		LOG->RecordFailure("Verify that the local variable 'local' exists.");
		return;
	}

	// navigate the stack back to func().
	if(stk.NavigateStack(0))
		LOG->RecordSuccess("Navigate the stack back to func().");		
	else
	{
		LOG->RecordFailure("Navigate the stack back to func().");		
		return;
	}

	// verify that the current function is func().
	if(stk.CurrentFunctionIs("func"))
		LOG->RecordSuccess("Verify current function is func().");		
	else
	{
		LOG->RecordFailure("Verify current function is func().");		
		return;
	}

	// verify the current line is the line after global changes.
	if(dbg.CurrentLineIs("line after global changes"))
		LOG->RecordSuccess("Verify current line is the line after global changes.");
	else
	{
		LOG->RecordFailure("Verify current line is the line after global changes.");
		return;
	}

	// verify that the local variable 'local' does not exist.
	if(locals.LocalDoesNotExist("local"))
		LOG->RecordSuccess("Verify that the local variable 'local' does not exist.");
	else
	{
		LOG->RecordFailure("Verify that the local variable 'local' does not exist.");
		return;
	}

	// change value of global via expression evaluator.
	if(ee.SetExpressionValue("global", 4))
		LOG->RecordSuccess("Change value of global to 4 via expression evaluator.");
	else
	{
		LOG->RecordFailure("Change value of global to 4 via expression evaluator.");
		return;
	}

	// verify value of global via memory dump.
	if(mem.MemoryDataIs("global", 4))
		LOG->RecordSuccess("Verify that value of global is 4 via memory dump.");
	else
	{
		LOG->RecordFailure("Verify that value of global is 4 via memory dump.");
		return;
	}

	// clear all breakpoints.
	if(bps.ClearAllBreakpoints())
		LOG->RecordSuccess("Clear all breakpoints.");		
	else
	{
		LOG->RecordFailure("Clear all breakpoints.");		
		return;
	}

	// restart inetinfo.exe
	if(dbg.Restart())
		LOG->RecordSuccess("Restart 'inetinfo.exe'.");		
	else
	{
		LOG->RecordFailure("Restart 'inetinfo.exe'.");		
		return;
	}

	// stop debugging inetinfo.exe.
	if(dbg.StopDebugging())
		LOG->RecordSuccess("Stop debugging 'inetinfo.exe'.");		
	else
	{
		LOG->RecordFailure("Stop debugging 'inetinfo.exe'.");		
		return;
	}
	
	// close the isapi extension project.
	if(prj.Close() == ERROR_SUCCESS)
		LOG->RecordSuccess("Close the ISAPI extension project.");		
	else
	{
		LOG->RecordFailure("Close the ISAPI extension project.");		
		return;
	}

	// terminate internet explorer.
	if(TerminateProcess(piProcInfo.hProcess, 0))
		LOG->RecordSuccess("Terminate Internet Explorer.");		
	else
	{
		LOG->RecordFailure("Terminate Internet Explorer.");		
		return;
	}
}