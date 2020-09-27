///////////////////////////////////////////////////////////////////////////////
//	StressCase.CPP
//
//	Created by :			Date :
//		MichMa					1/28/98
//
//	Description :
//		Implementation of the CSysStressCase class

#include "stdafx.h"
#include "StressCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CSysStressCase, CTest, "System Stress", -1, CSysStressSubsuite)


void CSysStressCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
	// some required initialization.
	//m_strStressRoot = "\\\\msvc\\vcqa\\stress\\";
	m_strStressRoot = "e:\\cafe6\\tests\\system\\stress\\";
	m_intIterations = 0;
}


void CSysStressCase::Run(void)

{	 
	COProject prj;
	COSource src;
	CODebug dbg;
	COBreakpoints bp;
	CString strProjName;

	// initialize the web that will publish results for this stress run.
	InitiateWeb();

	// internationalize the project name.
	if(GetSystem() & SYSTEM_JAPAN)
		strProjName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü AppWizard Project";
	// TODO (michma - 2/11/98): cafe needs to support SYSTEM_GERMAN.
	//else if(GetSystem() & SYSTEM_GERMAN)
	//	strProjName = "ßüäöÖÄÜµ AppWizard Project";
	else
		strProjName = "AppWizard Project";
		
	// setup the options for the AppWizard project.
	APPWIZ_OPTIONS options;
	options.m_bUseMFCDLL = TRUE;
	options.m_OLECompound = APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER;

	// create a new AppWizard project
	if(!LOG->RecordCompare(prj.NewAppWiz(strProjName, GetCWD(), GetUserTargetPlatforms(), &options) == ERROR_SUCCESS,
		"Creating an AppWizard project named '%s'.", strProjName))
		return;

	// open the AppWizard project's main source file.
	if(!LOG->RecordCompare(src.Open(GetCWD() + strProjName + "\\" + strProjName + ".CPP") == ERROR_SUCCESS, 
		"Opening file '%s.CPP' for editing.", strProjName))
		return;
		
	// find the line where we are supposed to add construction code.
	if(!LOG->RecordCompare(src.Find(GetLocString(IDSS_ADD_CONSTRUCT_CODE_HERE)),
		"Searching file '%s.CPP' for place to insert code", strProjName))
		return;

	// we append a counter to each line added to distinguish them.
	char szLine[12];
	
	// add code, build, and debug in a loop forever.
	while(1)

	{
		// create a string version of the line counter.
		m_intIterations++;
		sprintf(szLine, "%d", m_intIterations);
		
		// insert a line of code.
		src.TypeTextAtCursor("{END}{ENTER}");
		src.TypeTextAtCursor("while(0); // line #" + (CString)szLine, TRUE);
		if(!LOG->RecordCompare(TRUE, "Adding a line of code to file '%s.CPP'.", strProjName))
			break;

		// set a bp on the new line.
		if(!LOG->RecordCompare(bp.SetBreakpoint() != NULL, "Setting a breakpoint on the new line."))
			break;

		// build the project.
		if(!LOG->RecordCompare(prj.Build() == ERROR_SUCCESS, "Building '%s' project.", strProjName))
			break;

		// verify no errors or warnings were generated.
		int iErr, iWarn;
		if(!LOG->RecordCompare(prj.VerifyBuild(TRUE, &iErr, &iWarn, TRUE) == ERROR_SUCCESS,
			"Verifying build of '%s' project.", strProjName))
			break;

		// run the debugger until the breakpoint.
		if(!LOG->RecordCompare(dbg.Go(), "Running the debugger until the new line."))
			break;

		// verify that the current line.
		if(!LOG->RecordCompare(dbg.CurrentLineIs("while(0); // line #" + (CString)szLine), "Verifying the current line."))
			break;

		// toggle the breakpoint off the new line.
		if(!LOG->RecordCompare(bp.SetBreakpoint() != NULL, "Toggle breakpoint off the new line."))
			break;

		// stop debugging.
		if(!LOG->RecordCompare(dbg.StopDebugging(), "Stop the debugger."))
			break;

		// update the web that publishes results for this stress run with an updated iteration count and running time.
		UpdateWeb();
	}
}


void CSysStressCase::InitiateWeb(void)

{
	// create a date string for the current date.
	time_t time_tCurrent = time(NULL);
	tm *pTMCurrent = localtime(&time_tCurrent);
	m_strCurrentDate.Format("%.2d-%.2d-%.2d", pTMCurrent->tm_mon, pTMCurrent->tm_mday, pTMCurrent->tm_year);

	// create a folder and webpage for this date if one doesn't exist already.
	// TODO(michma) what's the crt/win32 api func for that?

	//{
		CreateDirectory(m_strStressRoot + m_strCurrentDate, NULL);
		
		FILE *pfWebPage = fopen(m_strStressRoot + m_strCurrentDate + "\\" + m_strCurrentDate + ".htm", "w");
		fprintf(pfWebPage, "<body bgcolor=#FFFFFF>\n");
		fprintf(pfWebPage, "<hr>\n\n");
		fprintf(pfWebPage, "<table width=100%%>\n\n");
		fprintf(pfWebPage, "<tr>\n");
		fprintf(pfWebPage, "<td><u><b>MACHINE</b></u></td>\n");
		fprintf(pfWebPage, "<td><u><b>BUILD</b></u></td>\n");
		fprintf(pfWebPage, "<td><u><b>ITERATIONS</b></u></td>\n");
		fprintf(pfWebPage, "<td><u><b>TIME</b></u></td>\n");
		fprintf(pfWebPage, "<td><u><b>LOG</b></u></td>\n");
		fprintf(pfWebPage, "</tr>\n\n");
		fprintf(pfWebPage, "<comment>add entries here</comment>\n\n");
		fprintf(pfWebPage, "</table>\n");
		fclose(pfWebPage);


	//}

	// create a web p
}


void CSysStressCase::UpdateWeb(void)
{
}