///////////////////////////////////////////////////////////////////////////////
//	cs_dlls.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "stacsub.h"
#include "cs_dlls.h"	 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
#define cRetailTests 1
#define cDebugTests 1

IMPLEMENT_TEST(Cstack_dllsIDETest, CDebugTestSet, "CallStack DLLs", -1, CstackSubSuite)

void Cstack_dllsIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void Cstack_dllsIDETest::Run()
{
	XSAFETY;

//	EXPECT_EXEC( dbg.SetOwnedStack( &stk ), "Could not setup owned stack" );
//	XSAFETY;
	EXPECT_EXEC( SetProject( "dllapp\\incdll", PROJECT_DLL, fBuildOnly), "Project could not be setup" );
	EXPECT_EXEC( SetProject( "dllapp\\powdll", PROJECT_DLL, fBuildOnly), "Project could not be setup" );
	EXPECT_EXEC( SetProject( "dllapp\\dec2", PROJECT_DLL, fBuildOnly), "Project could not be setup" );
	EXPECT_EXEC( SetProject( "dllapp\\dllappx"), "Project could not be setup" );

	// when makefile is wrapped, wrapper name is dllappx1.mak and
	// exe for debug session becomes dllappx1.exe, so we must
	// change it back since we actually build dllappx.exe.
	proj.SetExeForDebugSession("dllappx.exe");

	// dllappx builds 4 files: incdll.dll, dec2.dll, powdll.dll and dllappx.exe.
	// dllappx.exe is copied to remote by SetProject, but we need to do dlls.
	COConnection *pconnec = GetSubSuite()->GetIDE()->GetConnectionInfo();
	if(pconnec->GetUserConnections() != CONNECTION_LOCAL)
	{
		proj.UpdateRemoteFile("src\\dllapp\\incdll.dll");
		proj.UpdateRemoteFile("src\\dllapp\\dec2.dll");
		proj.UpdateRemoteFile("src\\dllapp\\powdll.dll");
	}

	XSAFETY;
	DllGeneralNavigation();
	XSAFETY;
	DllStepCheckCallStack();
	XSAFETY;

	StopDbgCloseProject();
	WriteLog(PASSED,"Testing of dllapp.mak completed");
}


BOOL Cstack_dllsIDETest::DllGeneralNavigation()
{
	CString fnName;
	UIEditor editor;
	int index;

 	LogTestHeader( "DllGeneralNavigation" );

	bps.ClearAllBreakpoints();

	LogTestHeader( "Initial callstack after first stepin" );
	EXPECT_TRUE( dbg.StepInto( 1, "WinMain" ) );
	EXPECT_TRUE( Frame( stk.GetFunction( 0 ) ).GetFunctionName( fnName ) );
	EXPECT_TRUE( fnName == "WinMain" );

 	LogTestHeader( "General navigation with full debug info" );
	EXPECT_TRUE( NULL != bps.SetBreakpoint("{MainWndProc,,}CALLDLLATSTART") );
	EXPECT_TRUE( dbg.Go() );
	EXPECT_TRUE( dbg.AtSymbol("CALLDLLATSTART"));

	char const * const stk1[] =
	{
		{ "MainWndProc( HWND__ * %, unsigned int %, unsigned int %, long %) line 261" },
		NULL
	};

	// Verify information on stack above callback
	EXPECT_TRUE( (index = stk.Index( "MainWndProc" ) ) != -1 );
	EXPECT_TRUE( stk.Compare( stk1, FrameList::NoFilter, 0, index ) );

	dbg.StopDebugging();

	return (TRUE);
}

BOOL Cstack_dllsIDETest::DllStepCheckCallStack()
{
	CString fnName;
	UIEditor editor;
	int index;

 	LogTestHeader( "DllStepCheckCallStack" );
	bps.ClearAllBreakpoints();

	// Step through some dll's.
 	LogTestHeader( "Step through some dll's; stack check and some navigation" );
	EXPECT_TRUE( bps.SetBreakpoint("{MainWndProc,,}CALLDLLATSTART") );
	EXPECT_TRUE( dbg.Go("CALLDLLATSTART") );
	EXPECT_TRUE( StepInUntilFrameAdded( "Dec(unsigned short *" ) );
	EXPECT_TRUE( src.Find("Inc(wBar);   // call another") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Inc(wBar);   // call another") );
	LOG->Comment("Swithing to ASM mode");
	dbg.SetSteppingMode(ASM);
	LOG->Comment("Stepping until INCDLL!");
	for(int i=0;i<30;i++)
	{  //look for the module name when stepping through thunk jumps
		dbg.StepInto();
		if(stk.CurrentFunctionIs("INCDLL! "))
			break;
	}
	LOG->RecordCompare(i<10, "Stepping into INCDLL module");
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE( stk.CurrentFunctionIs("Inc(unsigned short *") );

	// Do a stack check and some navigation
	char const * const stk2[] =
	{
		{ "Inc( unsigned short * % ) line 36" },
		{ "Dec( unsigned short * % ) line 57" },
		{ "MainWndProc( HWND__ * %, unsigned int %, unsigned int %, long %) line 266" },
//		{ "*" },
		NULL
	};

	// Set stack dirty since StepInUntilFrameAdded used different COStack/CODebug objects
//	stk.SetDirty( TRUE );
	EXPECT_TRUE( (index = stk.Index( "MainWndProc" ) ) != -1 );
	EXPECT_TRUE( stk.Compare( stk2, FrameList::NoFilter, 0, index ) );

	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE( StepInUntilFrameRemoved() );
	EXPECT_TRUE( stk.Compare( stk2+1, FrameList::NoFilter, 0, index-1 ) );

		
	//	dPow is called in an explicitly loaded dll by a function pointer
	EXPECT_TRUE( StepInUntilFrameAdded( "dPow(int 10, int 3)" ) );

	// Do a stack check and some navigation
	const char* frame1;
	frame1 = "dPow( int 10, int 3 ) line 37";
			
	char const * const stk3[] =
	{
		{ frame1 },
		{ "Dec( unsigned short * % ) line 64" },
		{ "MainWndProc( HWND__ * %, unsigned int %, unsigned int %, long %) line 266" },
//		{ "*" },
		NULL
	};

	// Set stack dirty since StepInUntilFrameAdded used different COStack/CODebug objects
	stk.SetDirty( TRUE );
	
	EXPECT_TRUE( (index = stk.Index( "MainWndProc" ) ) != -1 );
	EXPECT_TRUE( stk.Compare( stk3, FrameList::NoFilter, 0, index ) );

	EXPECT_TRUE( StepInUntilFrameRemoved() );
 
	dbg.StopDebugging();

	return (TRUE);
}

