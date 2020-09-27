///////////////////////////////////////////////////////////////////////////////
//      PERCASES.CPP
//
//      Created by :                    Date :
//              WayneBr                         9/28/94
//                                      
//      Description :
//              Portable Edit'n Go performance sniff tests

#include "stdafx.h"
#include "fstream.h"
#include "percases.h"
#include "debugger.h"
//
// Daytona only api handling
//
#include "DllApi.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// OS hack function
CString GetOSDescription(void);  // e.g. "Windows NT 3.5, build 807"

inline BOOL MyWaitForBreak (HWND hwnd);
inline void FlushMem(void);

// Timer Globals
DWORD dwStartTime;
DWORD dwEndTime;
DWORD dwElapsedTime;
DWORD dwSleep;

// Switch Globals
BOOL fFlush;
BOOL fCleanUp;
DWORD   FullWait;
DWORD   FullDepWait;
DWORD   MajorWait;
DWORD   MinorWait;
DWORD   StartDBGWait;
DWORD   RestartWait;
CString ProductVersion;
CString OSVersion;

// Globals for MyWaitForBreak
HWND    hwnd;
char    Buffer[128];

// Source file pointers / allocation map
COSource *pSrcFCN, *pSrcDef, *pSrcRef;
BOOL bSRC_USED[MAX_SRC]= {FALSE, FALSE, FALSE};

#if defined(LEGO)    // Lego meeds minimal case, ie no timing info needed
#define GetTickCount()  0
#endif

IMPLEMENT_TEST(CPerformanceCases, CTest, "Performance Tests", -1, CSniffDriver)

CPerformanceCases::CPerformanceCases(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CTest(pSubSuite, szName, nExpectedCompares, szListFilename)
{
	// Function to add and Modifications to function for later
	FCN="~void Pretend_Dump{(}{)} {{}{TAB}//New fuction to test incremental builds~{TAB}int x=1; //Delete_This_Later~int y;~int z; //Delete_This_Later~~y=2;~z=3.7;~z=y/z;~OutputDebugString{(}\"Pretend dump\"{)};~+{TAB}{}};~";
	MOD="float z; ~char *sz=\"Sample String\";~~";
}

void CPerformanceCases::PreRun(void)
{
	#define ALLBUILDTIME_FLAG       " /Y3"
	#define DEBUGTIME_FLAG			" /DBGLOG"
	
	CSettings       *pSettings=             GetSubSuite()->GetIDE()->GetSettings();
	CString         strCommandLine= pSettings->GetTextValue(settingCmdLine);
	strCommandLine = strCommandLine + ALLBUILDTIME_FLAG + DEBUGTIME_FLAG;
	pSettings->SetTextValue(settingCmdLine,strCommandLine);
	FlushMem();  // Flush working set on NT iff -Flush flag is set
	CTest::PreRun();
}

BOOL CPerformanceCases::Setup(Project_Info* PI)
{

#if defined(LEGO) // Lego meeds minimal case, no setup (do in batch file before running)
	return TRUE;
#endif
	BOOL bResult=TRUE;
	CString s,DBG_Wait,BLD_Wait;
	CString SrcProjDir_FullPath= FullPath(PI->SrcProjDir);
	CString ProjDir_FullPath= FullPath(PI->ProjDir);

	// Initialize Switch Globals            
		fFlush=(CMDLINE->GetBooleanValue("flush",FALSE) ? TRUE:FALSE);
		fCleanUp=(CMDLINE->GetBooleanValue("cleanup",TRUE) ? TRUE:FALSE);
		
		//  -Build_Wait:"60,60,20,20"   multiply by 1000 to convert seconds to ms
		BLD_Wait=(CMDLINE->GetTextValue("Build_Wait"));
		if (! BLD_Wait.IsEmpty())
		{
			s=BLD_Wait;
			FullWait=       1000 * atoi(s.Left(     s.Find(',')));
			s=s.Mid((s.Find(',')+1));
			FullDepWait=1000 * atoi(s.Left( s.Find(',')));
			s=s.Mid((s.Find(',')+1));
			MajorWait=      1000 * atoi(s.Left(     s.Find(',')));
			MinorWait=      1000 * atoi(s.Mid((s.Find(',')+1)));
		} else {
			FullWait=        1000 ;
			FullDepWait= 1000 ;
			MajorWait=       1000 ;
			MinorWait=       1000 ;
		};
	
		//  -DBG_Wait:"20,5"                    multiply by 1000 to convert seconds to ms
		DBG_Wait=(CMDLINE->GetTextValue("DBG_Wait"));
		if (! DBG_Wait.IsEmpty())
		{
			StartDBGWait=1000 * atoi(DBG_Wait.Left( DBG_Wait.Find(',')));
			RestartWait= 1000 * atoi(DBG_Wait.Mid((DBG_Wait.Find(',')+1)));
		} else {
			StartDBGWait= 1000;
			RestartWait=  1000;
		};


		ProductVersion=(CMDLINE->GetTextValue("VC_VER"));
		OSVersion=(CMDLINE->GetTextValue("OS_VER"));

		WRITELOG(TRUE,"=======================  Prepare for Testing  ========================");                        
		WRITELOG(TRUE,"Copy appwiz.v2 tree to appwiz directory" );
		KillAllFiles( ProjDir_FullPath);
		CreateDirectory(ProjDir_FullPath, NULL);
		//Copy appwiz.v2 tree to a working directory
		CopyTree ( SrcProjDir_FullPath, ProjDir_FullPath);
		// save diskfootprint
		MeasureTree(PI->ProjDir,&(PI->Disk_Before));
		WRITELOG(TRUE,"Measured initial disk footprint");
	return bResult;
};



BOOL CPerformanceCases::Shutdown(Project_Info* pProjInfo, BOOL bResult)
{
	// Close project and files
	WRITELOG(TRUE,"=======================  Clean up after Testing  ========================");                     
	CloseALLSources();
	m_prj.Close();
	UIWB.CloseAllWindows();  // incase any other windows are open (winmain.cpp etc..)
	WRITELOG(bResult,"Close project, files, windows");

#if defined(LEGO) // Lego meeds minimal case only no cleanup 
	m_pLog->RecordInfo(pProjInfo->Project+" Testing Completed");
	return bResult;
#endif

	// save diskfootprint
	MeasureTree(pProjInfo->ProjDir,&(pProjInfo->Disk_After));               
	WRITELOG(bResult,"Measured final disk footprint");

	// Clean up files if tests passed.
	if ((bResult) && (fCleanUp)) {
		KillAllFiles( FullPath(pProjInfo->ProjDir));
		WRITELOG(TRUE,"Clean project after all tests PASS");                    
	};
	// Log result to data file and CAFE Log 
	if (bResult) {
		bResult=bResult && LogData( pProjInfo );        // Log result to datfile and CAFE Log
		WRITELOG(TRUE,"Log result to data file and CAFE Log");                  
	}
	// Testing Done
	m_pLog->RecordInfo(pProjInfo->Project+" Testing Completed");

	return bResult;
};


void CPerformanceCases::Run(Project_Info* pProjInfo)
{

	BOOL bResult=Setup( pProjInfo );
	// Users Scenarios
	if (bResult)
		bResult=bResult && CleanBuild ( pProjInfo );    // Clean build User scenario
	if (bResult)
		bResult=bResult && MajorChange( pProjInfo );    // Major change ie adding function(s) User scenario
	if (bResult)
		bResult=bResult && MinorChange( pProjInfo );    // Minor change ie changes within function User scenario
	Shutdown( pProjInfo, bResult);
	
};


BOOL CPerformanceCases::CleanBuild(Project_Info * PI) {

		BOOL bResult=TRUE;
		WRITELOG(TRUE,"====================  Full Clean Build Scenario  ====================");                 

// **** If dependant make file run that first *****     
		if (PI->Dep_Make != "") {
	// open project
			m_prj_dep.Open(FullPath(PI->Dep_Make));
	// clean full build
			dwSleep=FullDepWait;
			FlushMem();  // Flush working set on NT iff -Flush flag is set
			m_prj_dep.RebuildAll(FullDepWait);			
			FlushMem();  // Flush working set on NT iff -Flush flag is set
			// save build timing info
			PI->Time_Full_Build=GetBuildTime();
			VCOMMENT("Time required to for clean full build of dependant stuff (milliseconds): %d", PI->Time_Full_Build);
			BOOL bBuilt= (m_prj_dep.VerifyBuild() == ERROR_SUCCESS);
			bResult=bResult && bBuilt;
			WRITELOG(bBuilt,"Clean Full Depandant Build");
	// close project
	//              m_prj_dep.Close();
		}
		else
		{
	// clear timing info
			PI->Time_Full_Build=0;
		}

// **** Main makefile/project *****     
	// open project
		m_prj.Open(FullPath(PI->Makefile));
	// get/set? target and options if needed
		PI->Target=( (CString)(m_prj.GetTarget()) );
	// clean full build
		dwSleep=FullWait;
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		m_prj.RebuildAll(FullDepWait);			
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		DWORD dwTempTime=GetBuildTime();
		// save build timing info
		PI->Time_Full_Build=PI->Time_Full_Build+dwTempTime;
		VCOMMENT("Time required to for complete clean full build (in milliseconds): %d", PI->Time_Full_Build);
		BOOL bBuilt= (m_prj.VerifyBuild() == ERROR_SUCCESS);
		bResult=bResult && bBuilt;
		WRITELOG(bBuilt,"Clean Full Build");
	// Start debugging (F8)
		dwSleep=StartDBGWait;
		hwnd=MST.WGetActWnd(0);
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		// Measure Start time
		UIWB.DoCommand(IDM_RUN_STEPOVER, DC_MESSAGE);
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		::Sleep(dwSleep);
		BOOL bStarted=MyWaitForBreak(hwnd);
		dwElapsedTime=GetDBGTime(DBG_START);
		PI->Time_Start_Debug=dwElapsedTime;
		VCOMMENT("Time required to start debugging (in milliseconds): %d", dwElapsedTime);
		bResult=bResult && bStarted;
		WRITELOG(bStarted,"Start debugging via F8");
	// Stop debugging
		UIWB.WaitForBreak();  // add to compensate for use of MyWaitForBreak when start debugging
		bResult=bResult && m_dbg.StopDebugging( ASSUME_NORMAL_TERMINATION );
		WRITELOG(bResult, "Stop debugging the TEST app" );
		return bResult;
}

BOOL CPerformanceCases::MajorChange(Project_Info * PI) {

		BOOL bResult=TRUE;
		WRITELOG(TRUE,"====================  Major Change Scenario  ====================");                     
	// ADD definition
		pSrcDef=OpenSource(FullPath(PI->ProjDir+"\\"+PI->AddDef_File));
		pSrcDef->InsertText(PI->AddDef, PI->AddDef_Line, 1);
	// ADD refernece
		pSrcRef=OpenSource(FullPath(PI->ProjDir+"\\"+PI->AddRef_File));
		pSrcRef->InsertText(PI->AddRef, PI->AddRef_Line, 1);            
	// ADD function
		pSrcFCN=OpenSource(FullPath(PI->ProjDir+"\\"+PI->AddFcn_File));
		pSrcFCN->InsertText(PI->AddFcn, PI->AddFcn_Line, 1);    
			
		WRITELOG(TRUE, "Add function (Major Change)");
	// verify navigation via. no compile browser
		// ********** TODO enable when browser feature is on-line ************
		//bResult=Result && m_brz.GotoNthDefinition( 1, "Pretend_Dump" );
		//WRITELOG(bResult, "Go to the definition of \"Pretend_Dump\"" );
	// incremental build
		dwSleep=MajorWait;
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		m_prj.Build(MajorWait);			
		FlushMem();  // Flush working set on NT iff -Flush flag is set
	// save build timing info
		PI->Time_Major_Build=GetBuildTime();
		VCOMMENT("Time required to for incremental build (in milliseconds): %d",PI->Time_Major_Build);
		BOOL bBuilt= (m_prj.VerifyBuild() == ERROR_SUCCESS);
		bResult=bResult && bBuilt;
		WRITELOG(bBuilt,"Incremental Build after adding a function");
	// Start debugging (F8)
		dwSleep=StartDBGWait;
		hwnd=MST.WGetActWnd(0);
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		// Measure Start time
		UIWB.DoCommand(IDM_RUN_STEPOVER, DC_MESSAGE);
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		::Sleep(dwSleep);
		BOOL bStarted=MyWaitForBreak(hwnd);
		dwElapsedTime=GetDBGTime(DBG_START);
		// save start debugging info
		PI->Time_Start_Debug+=dwElapsedTime;
		VCOMMENT("Time required to start debugging (in milliseconds): %d", dwElapsedTime);
		bResult=bResult && bStarted;
		WRITELOG(bStarted,"Start debugging via F8");
	// Stop debugging
		UIWB.WaitForBreak();  // add to compensate for use of MyWaitForBreak when start debugging
		bResult=bResult && m_dbg.StopDebugging( ASSUME_NORMAL_TERMINATION );
		WRITELOG(bResult, "Stop debugging the TEST app" );
		return bResult;
};

BOOL CPerformanceCases::MinorChange(Project_Info * PI) {
	BOOL bResult=TRUE;
	WRITELOG(TRUE,"====================  Minor Change Scenario  ====================");                     
	// Add changes
		//m_src.Open(FullPath(PI->AddFcn_File));
		//m_src->Open(FullPath(PI->ProjDir+"\\"+PI->AddFcn_File));
		COFile file;
		file.Open(FullPath(PI->ProjDir+"\\"+PI->AddFcn_File));
#if !defined(LEGO)  // LEGO needs minimal case not searches
		pSrcFCN->Find("int x=1; //Delete_This_Later");
		pSrcFCN->TypeTextAtCursor("////");
#endif  // LEGO
		pSrcFCN->Find("int z; //Delete_This_Later");
		pSrcFCN->TypeTextAtCursor(PI->ModCode);
		WRITELOG(TRUE, "Modify function (Minor Change)");                       
	// set BP on function
#if !defined(LEGO)  // LEGO needs minimal case not extra debugging
#if defined(_M_MRX000) || defined(_M_ALPHA)
		bResult=bResult && (BOOL)m_bp.SetBreakpoint(PI->AddFcn_Line+5);
#else
		bResult=bResult && (BOOL)m_bp.SetBreakpoint(PI->AddFcn_Line+1);
#endif
#endif  // LEGO
		WRITELOG( bResult,"Set a breakpoint on \"Pretend_Dump\"" );
	// incremental build
		dwSleep=MinorWait;
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		m_prj.Build(dwSleep);			
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		// save build timing info                                  
		PI->Time_Minor_Build=GetBuildTime();
		VCOMMENT("Time required to for incremental build (in milliseconds): %d", PI->Time_Minor_Build);
		BOOL bBuilt= (m_prj.VerifyBuild() == ERROR_SUCCESS);
		bResult=bResult && bBuilt;
		WRITELOG(bBuilt,"Incremental Build editing within a function");
	// Start debugging (F8)
		dwSleep=StartDBGWait;
		hwnd=MST.WGetActWnd(0);
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		// Measure Start time
		UIWB.DoCommand(IDM_RUN_STEPOVER, DC_MESSAGE);  // message is low overhead/faster dokeys is blocking
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		::Sleep(dwSleep);
		BOOL bStarted=MyWaitForBreak(hwnd);
		dwElapsedTime=GetDBGTime(DBG_START);
		// save start debugging info
		PI->Time_Start_Debug+=dwElapsedTime;
		VCOMMENT("Time required to start debugging (in milliseconds): %d", dwElapsedTime);
		bResult=bResult && bStarted;
		WRITELOG(bStarted,"Start debugging via F8");
#if !defined(LEGO) // LEGO needs minimal case not extra debugging
	// Go to BP
		UIWB.WaitForBreak();  // add to compensate for use of MyWaitForBreak when start debugging
		bResult=bResult && m_dbg.Go( NULL, NULL, "Pretend_Dump" );
		WRITELOG(bResult,,"Run until the breakpoint is hit" );
	// Step to right place
		BOOL bLineInfo=TRUE;
#if !defined(_M_MRX000) && !defined(_M_ALPHA)
		bLineInfo=bLineInfo && m_dbg.StepInto(1,NULL,"Sample String",NULL);
#endif
		bLineInfo=bLineInfo && m_dbg.StepInto(1,NULL,"y=2",NULL);
		bLineInfo=bLineInfo && m_dbg.StepInto(1,NULL,"z=3.7",NULL);
		bLineInfo=bLineInfo && m_dbg.StepInto(1,NULL,"z=y/z",NULL);
		bResult=bResult && bLineInfo;
		WRITELOG(bLineInfo,"Verify Line Number via Four Stepinto's");
	// verify debug info
	 //             note can't use EXPECT_TRUE( macro if \"  in string!!!
	 //             note add home to deselect
		//TODO: set bResult for EE stuff below
		MST.DoKeys("{HOME}");
		LogResult(m_cxx.ExpressionErrorIs("x",GetLocString(IDSS_ERR_CHECK_CXX0017)), "ExpressionErrorIs(\"x\",\"CXX0017: Error: symbol \"x\" not found\")");        // x was removed
		EXPECT_TRUE(m_cxx.ExpressionValueIs("y",2));    // y was unchanged
		EXPECT_TRUE(m_cxx.ExpressionValueIs("z",3.7));  // z changed type from int to float
		LogResult(m_cxx.ExpressionValueIs("sz,s" , "\"Sample String\"" ),"ExpressionValueIs(\"sz,s\",\"Sample String\")");      // added new variable
#endif  // LEGO

	// Restart
		dwSleep=RestartWait;
		hwnd=MST.WGetActWnd(0);
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		// Measure restart time
		UIWB.DoCommand(IDM_RUN_RESTART, DC_MESSAGE);  // message is low overhead/faster dokeys is blocking
		FlushMem();  // Flush working set on NT iff -Flush flag is set
		::Sleep(dwSleep);
		bStarted=MyWaitForBreak(hwnd);
		dwElapsedTime=GetDBGTime(DBG_RESTART);
		// save restart debugging timing info
		PI->Time_Restart_Debug=dwElapsedTime;
		VCOMMENT("Time required to restart debugging (in milliseconds): %d", dwElapsedTime);
		bResult=bResult && bStarted;
		WRITELOG(bStarted,"Restart the TEST app" );
	// Stop debugging
		UIWB.WaitForBreak();  // add to compensate for use of MyWaitForBreak when start debugging
		bResult=bResult && m_dbg.StopDebugging( ASSUME_NORMAL_TERMINATION );
		WRITELOG(bResult, "Stop debugging the TEST app" );
	
		return bResult;
};

BOOL CPerformanceCases::LogData(Project_Info * PI) {
		// Create Record        
		CString DataRecord1;
		CTime Time;
		Time=Time.GetCurrentTime();

		if (ProductVersion=="") ProductVersion=(CString)GetSubSuite()->GetIDE()->GetVersion();
		if (OSVersion=="") OSVersion=(CString)GetOSDescription();

		DataRecord1=(CString) Time.Format( "%x" )               +","+
					(CString) Time.Format( "%X" )           +","+
					(CString) OSVersion                                     +","+
					(CString) ProductVersion            +","+
					(CString)PI->Project                            +","+
					(CString)PI->Target                                     +","+
					ConvertTime(PI->Time_Start_IDE)         +","+
					ConvertTime(PI->Time_Start_Debug/3)     +","+
					ConvertTime(PI->Time_Restart_Debug)     +","+
					ConvertTime(PI->Time_Full_Build)        +","+
					ConvertTime(PI->Time_Major_Build)       +","+
					ConvertTime(PI->Time_Minor_Build)       +","+
					ConvertTime((unsigned long)((PI->Disk_After) - (PI->Disk_Before)));
												
		// LOG DATA TO VIEWPORT
		COMMENT(("Data:"+DataRecord1));
	
		// LOG DATA TO FILE
		ofstream DataFile("VC_Data.Dat", ios::app);
		DataFile << DataRecord1<<endl;
		DataFile.close();
		
		// Write Build Record   
		ofstream DataFile2("VCbldLog.txt", ios::app);
		DataFile2 << DataRecord1 << endl;
		DataFile2.close();

		// Write Build Record   
		ofstream DataFile3("VCdisk.txt", ios::app);
		DataFile3 << DataRecord1 << endl;
		DataFile3.close();

		return TRUE;
};

///////////////////////////////////////////////////////////////////////////////
//      Test Utilities

void CPerformanceCases::RemoveTree( CString strRootDir )
{
	if ( strRootDir.Right( 1 ) != '\\' )
		strRootDir += '\\';

	WIN32_FIND_DATA findData;
	HANDLE hSearch = FindFirstFile( strRootDir + "*.*", &findData );
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( findData.cFileName[0] != '.' )
				{
					CString strSubDir = strRootDir + findData.cFileName;
					RemoveTree( strSubDir + '\\' );
				}
			}
			else
				DeleteFile( strRootDir + findData.cFileName );
		}
	    while ( FindNextFile( hSearch, &findData ) );

		FindClose( hSearch );
		// Don't need to remove directory too.
		//RemoveDirectory( strRootDir );
	}
}

void CPerformanceCases::MeasureTree( CString strRootDir, _int64 *pDirSize)              
{
	// Open data file 
	ofstream DataFile("VCdisk.txt", ios::app);

	if ( strRootDir.Right( 1 ) != '\\' )
		strRootDir += '\\';

	WIN32_FIND_DATA findData;
	HANDLE hSearch = FindFirstFile( strRootDir + "*.*", &findData );
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( findData.cFileName[0] != '.' )
				{
					CString strSubDir = strRootDir + findData.cFileName;
					MeasureTree( strSubDir + '\\', pDirSize);
				}
			}
			else
			{
				// TODO replace 0 with max of DWORD
				_int64 FileSize = ((findData.nFileSizeHigh * 0) + findData.nFileSizeLow);
				*pDirSize = *pDirSize + FileSize;
				if ( !(findData.dwFileAttributes & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_DIRECTORY)) )
					 DataFile << findData.cFileName << "\t" << ConvertTime((unsigned long)(FileSize)) << endl;
			}
		}
	    while ( FindNextFile( hSearch, &findData ) );
		FindClose( hSearch );
	}
	// Close Data File
	DataFile.close();
}

CString CPerformanceCases::ConvertTime(unsigned long Time){
	char buffer[255];
	char *pBuf=buffer;
	pBuf=ultoa(Time,pBuf,10);
	return (CString)(pBuf);

};


COSource *CPerformanceCases::OpenSource(LPCSTR filename) {
	int count = 0;
	while  ( bSRC_USED[count] && count < MAX_SRC ) {
		if ( ((CString)(m_src[count].GetFileName())).Find((CString)filename) !=-1 ){
			return &m_src[count];
		}
		count++;
	}
	if (count < MAX_SRC) {
		m_src[count].Open(filename);
		bSRC_USED[count]=TRUE;
		return &m_src[count];
	}

	CString msg = "OpenSource: Ran out of resources";
	COMMENT((const char *) msg) ;
	return NULL;
};

void CPerformanceCases::CloseALLSources() {
	int count = 0;
	while  ( bSRC_USED[count] && count < MAX_SRC)  {
		m_src[count].Close();
		bSRC_USED[count]=FALSE;
		count++;
	}
};
	

DWORD   CPerformanceCases::GetBuildTime() {

#if defined(LEGO) // LEGO needs minimal case not timing info needed
	return 0;
#endif

	CString BuildRecord;
	CString str;
	
	// Get build data
	UIWB.DoCommand(IDM_WINDOW_ERRORS,DC_MNEMONIC);  // goto output window
	MST.DoKeys("^{END}");			// goto end
	MST.DoKeys("+^{HOME}");			// select whole build record
	MST.DoKeys("^c");				// copy it
	GetClipText(BuildRecord);       // get text from clipboard
	MST.DoKeys("^{END}");			// goto end
	MST.DoKeys("+{up}");			// select last line 
	MST.DoKeys("^c");				// copy it
	GetClipText(str);               // get text from clipboard
	MST.DoKeys("{ESC}");            // return focus from output window

	// Write Build Record   
	ofstream DataFile("VCbldLog.txt", ios::app);
	DataFile << BuildRecord << endl;
	DataFile.close();

	// Look for the NEW time instrumented output
	//Build Time 0:18.5      
	if(str.Find("Build Time") >= 0) {		// TODO: Needs to be localized???
		// get time
		long Tenths =atol(str.Mid(str.ReverseFind('.')+1));
		long Seconds=atol(str.Mid(str.ReverseFind(':')+1,2));
		CString temp=str.Mid(0,str.ReverseFind(':')+1);
		long Minutes=atol(temp.Mid(temp.ReverseFind(' ')+1));
		DWORD Time= (Minutes*60*1000) + (Seconds*1000) + (Tenths*100);
		//VCOMMENT("Build Time (milliseconds): %s (%s)", str, ConvertTime(Time));
		return Time;
	}
	// couldn't find "Build Time"
	return 0;
};

DWORD   CPerformanceCases::GetDBGTime(int DBG_Type /* DBG_START */) {

#if defined(LEGO) // LEGO needs minimal case not timing info needed
	return 0;
#endif

	CString DBG_record;
	CString str, fnd_str;
	
	// Get output data
	UIWB.DoCommand(IDM_WINDOW_ERRORS,DC_MNEMONIC);  // goto output window
	MST.DoKeys("^{END}");			// goto end
	MST.DoKeys("+^{HOME}");			// select whole build record
	MST.DoKeys("^c");				// copy it
	GetClipText(DBG_record);		// get text from clipboard
	MST.DoKeys("^{END}");			// goto end
	MST.DoKeys("+{up}");			// select last line 
	MST.DoKeys("^c");				// copy it
	GetClipText(str);               // get text from clipboard
	MST.DoKeys("{ESC}");            // return focus from output window

	// Write Debugger Record   
	ofstream DataFile("VCdbgLog.txt", ios::app);
	DataFile << DBG_record << endl;
	DataFile.close();

	// Look for the NEW time instrumented output
	// Debug start time: 0:10:6
	// Debug restart time: 0:02:4
	if (DBG_Type == DBG_RESTART)
		fnd_str="Debug restart time";		// TODO: Needs to be localized???
	else 
		fnd_str="Debug start time";			// TODO: Needs to be localized???
	if(str.Find(fnd_str) >= 0) {
		// get time
		long Tenths =atol(str.Mid(str.ReverseFind(':')+1));
		CString temp=str.Mid(0,str.ReverseFind(':'));
		long Seconds=atol(temp.Mid(temp.ReverseFind(':')+1,2));
		temp=temp.Mid(0,temp.ReverseFind(':'));
		long Minutes=atol(temp.Mid(temp.ReverseFind(' ')+1));
		DWORD Time= (Minutes*60*1000) + (Seconds*1000) + (Tenths*100);
		//VCOMMENT("Debug start/restart Time (ms): %s (%s)", str, ConvertTime(Time));
		return Time;
	}
	// couldn't find Debug Time string
	return 0;
};



// from log.cpp remove when this function is added to new OS class 
CString GetOSDescription(void)  // e.g. "Windows NT 3.5, build 807"
{
  static LPCSTR szErr = "[Error calling GetVersionEx()]";

  OSVERSIONINFO osvi;

  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);  // required before calling GetVersionEx()

  if (!GetVersionEx(&osvi))
    return szErr;

  CString strD;

  switch (osvi.dwPlatformId)  // find the name of the OS
  {
    case VER_PLATFORM_WIN32_NT:
    {
      strD = "Windows NT";
      break;
    }
    case 1 /*VER_PLATFORM_WIN32_WINDOWS*/:  // see Olympus bug 3496; I'm guessing that this will be 1.
    {
      strD = "Windows";
      break;
    }
    case VER_PLATFORM_WIN32s:
    {
      strD = "Win32s";
      break;
    }
    default:
    {
      return szErr;
    }
  }

  strD += ' ';  // add a space after the name

  CString strV;

  strV.Format("%u.%u build %u", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);

  return strD + strV;
};


CDllApi::CDllApi ( const _TCHAR * szDll, MPSZPFN * mpszpfn ) {
    _mpszpfn = mpszpfn;
	_tcscpy ( _szDll, szDll );
	_fRoutinesOK = FALSE;
	_cRoutines = 0;

	HINSTANCE       hInstance = LoadLibrary ( szDll );

	if ( hInstance ) {
		
		_hInstance = hInstance;

		MPSZPFN *       pmpszpfn;
		BOOL            fRoutinesOK = TRUE;

		for ( pmpszpfn = _mpszpfn; pmpszpfn->szRoutine; pmpszpfn++ ) {
			fRoutinesOK = 
				fRoutinesOK && 
				(pmpszpfn->pfn = GetProcAddress ( hInstance, pmpszpfn->szRoutine ));
			_cRoutines++;
			}
		_fRoutinesOK = fRoutinesOK;
		}
	
    }
CDllApi::~CDllApi() {
	if ( _hInstance ) {
		FreeLibrary ( _hInstance );
		}
	_hInstance = 0;
	_fRoutinesOK = FALSE;
	}


BEGIN_MPSZPFN(Daytona)
MPSZPFN_FCN(SetProcessWorkingSetSize)
MPSZPFN_FCN(GetProcessWorkingSetSize)
END_MPSZPFN(Daytona)

#define ipfnSetWSSize  0
#define cbTrimWorkingSet        0xffffffff


typedef BOOL (WINAPI * PfnSetWSSize)(HANDLE, DWORD, DWORD);

#define cbTrimWorkingSet        0xffffffff

DECLARE_DLLAPI(dllApiDaytona, Daytona, kernel32.dll);

inline BOOL
SetWorkingSetSize ( DWORD cbMinWS, DWORD cbMaxWS ) {
	if ( dllApiDaytona.FRoutinesLoaded() ) {
		return PfnSetWSSize( dllApiDaytona ( ipfnSetWSSize ) ) (
			GetCurrentProcess(),
			cbMinWS,
			cbMaxWS
			);
		}
	return FALSE;
	}


inline void FlushMem(void) {
#if !defined(LEGO)  // LEGO needs minimal case not searches
	if (fFlush) SetWorkingSetSize(cbTrimWorkingSet,cbTrimWorkingSet);
#endif  // LEGO
};

inline BOOL CPerformanceCases::MyWaitForBreak(HWND hwnd) {
	CString lpzTitleBreak= "["+GetLocString(IDSS_DBG_BREAK)+"]";
	DWORD count=0;
	COMMENT("Waiting for '"+lpzTitleBreak+"' in title bar.");
	while( count<10 )  //1 minute limit -- WinslowF Add two morw zeros to handle the slownes on NT
	{
		//((GetText().Find(strTitleBreak) < 0 ) ? Sleep(500) : ct++);
		::Sleep(6000);
		GetWindowText(hwnd, Buffer, 128);
		if ( strstr(Buffer, lpzTitleBreak) )
			return TRUE;
		else
		{
			count+=1;
			COMMENT("MyWaitForBreak: Still looking for '"+lpzTitleBreak+"' in Title Bar");
		}
	};
	COMMENT("MyWaitForBreak: Time out after 1 min, found '"+(CString)Buffer+"' looking for '"+(CString)lpzTitleBreak+"'");
	return FALSE;
};
