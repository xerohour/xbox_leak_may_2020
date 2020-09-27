/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       fw.cpp
 *  Content:    DirectSound test case framework file generator
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/10/01    danrose Created to generate dsound test framework
 *
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <direct.h>

/****************************************************************************
 *
 * Globals to hild the Path, Directory and date
 *
 ****************************************************************************/

TCHAR* g_tszDir = NULL;
TCHAR g_tszPath[MAX_PATH];
TCHAR g_tszUser[32];
WORD g_wDate = 0;
WORD g_wMonth = 0;

/****************************************************************************
 *
 *  SetDirectory
 *
 *  Description:
 *      set the local directory and path for the app
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL SetDirectory( void )
{
	//
	// Allocate space for an index into the Path
	//

	DWORD dwIndex = 0;

	//
	// get the current directory, chekc for failure
	//

	if ( ! GetCurrentDirectory( MAX_PATH, g_tszPath ) )
		return FALSE;

	//
	// start index at end of path
	//


	dwIndex = _tcslen( g_tszPath ) - 1;

	//
	// Walk back down the path until just after a '\' is found
	//

	while ( '\\' != g_tszPath[dwIndex--] );

	//
	// Set the Directory to point ot the proper index in the path
	//

	g_tszDir = g_tszPath + dwIndex + 2;

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  SetUser
 *
 *  Description:
 *      set the user of the app
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL SetUser( void )
{
	return GetEnvironmentVariable( "USERNAME", g_tszUser, 32 );
}

/****************************************************************************
 *
 *  SetDate
 *
 *  Description:
 *      set the day of month for the app
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL SetDate( void )
{
	//
	// allocate a system time struct and init it
	//

	SYSTEMTIME sysTime;
	ZeroMemory( &sysTime, sizeof( SYSTEMTIME ) );

	//
	// Get the system time
	//

	GetSystemTime( &sysTime );

	//
	// Save it away
	//

	g_wDate = sysTime.wDay;
	g_wMonth = sysTime.wMonth;

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateLibSourcesFile
 *
 *  Description:
 *      generate the sources file to build the source file generated for lib
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateLibSourcesFile( void )
{
	//
	// allocate the sources file and check to make sure allocation was good
	//

	FILE* pSourcesFile = fopen( "lib\\sources", "wb" );

	if ( NULL == pSourcesFile )
		return FALSE;

	//
	// Output the text for the sources file
	//

	fprintf( pSourcesFile, "TARGETNAME=%s\r\n", g_tszDir );
	fprintf( pSourcesFile, "TARGETPATH=$(BASEDIR)\\private\\test\\lib\r\n" );
	fprintf( pSourcesFile, "TARGETTYPE=LIBRARY\r\n" );
	fprintf( pSourcesFile, "\r\n" );
	fprintf( pSourcesFile, "INCLUDES =          $(PRIVATE_INC_PATH);                               \\\r\n" );
	fprintf( pSourcesFile, "                    $(BASEDIR)\\private\\test\\inc;                       \\\r\n" );
	fprintf( pSourcesFile, "                    $(BASEDIR)\\private\\test\\multimedia\\inc\r\n" );
	fprintf( pSourcesFile, "\r\n" );                                           
	fprintf( pSourcesFile, "SOURCES= 			                                       \\\r\n" );
	fprintf( pSourcesFile, "    ..\\%s.cpp\r\n", g_tszDir );

	//
	// close the file
	//

	if ( NULL != pSourcesFile )
		fclose( pSourcesFile );

	//
	// return Success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateExeSourcesFile
 *
 *  Description:
 *      generate the sources file to build the source file generated for exe
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateExeSourcesFile( void )
{
	//
	// allocate the sources file and check to make sure allocation was good
	//

	FILE* pSourcesFile = fopen( "exe\\sources", "wb" );

	if ( NULL == pSourcesFile )
		return FALSE;

	//
	// Output the text for the sources file
	//

	fprintf( pSourcesFile, "TARGETNAME=%s\r\n", g_tszDir );
	fprintf( pSourcesFile, "TARGETPATH=obj\r\n" );
	fprintf( pSourcesFile, "TARGETTYPE=PROGRAM\r\n" );
	fprintf( pSourcesFile, "UMTYPE=xboxapp\r\n" );
	fprintf( pSourcesFile, "\r\n" );
	fprintf( pSourcesFile, "C_DEFINES=$(C_DEFINES) -DNOLOGGING\r\n" );
	fprintf( pSourcesFile, "\r\n" );
	fprintf( pSourcesFile, "MSC_WARNING_LEVEL=/W3 /WX\r\n" );
	fprintf( pSourcesFile, "MSC_OPTIMIZATION =  $(MSC_OPTIMIZATION) /Od\r\n" );
	fprintf( pSourcesFile, "\r\n" );
	fprintf( pSourcesFile, "XE_FLAGS=/TESTNAME:\"%s\" /TESTID:0xA7049955\r\n", g_tszDir );
	fprintf( pSourcesFile, "\r\n" );
	fprintf( pSourcesFile, "INCLUDES =          $(PRIVATE_INC_PATH);                               \\\r\n" );
	fprintf( pSourcesFile, "                    $(BASEDIR)\\private\\test\\inc;                       \\\r\n" );
	fprintf( pSourcesFile, "                    $(BASEDIR)\\private\\test\\multimedia\\inc\r\n" );
	fprintf( pSourcesFile, "\r\n" );
	fprintf( pSourcesFile, "TARGETLIBS =        $(LIBRARY_PATH)\\xonline$(D).lib\r\n" );
	fprintf( pSourcesFile, "\r\n" );
	fprintf( pSourcesFile, "SOURCES= 			                                       \\\r\n" );
	fprintf( pSourcesFile, "    ..\\%s.cpp\r\n", g_tszDir );

	//
	// close the file
	//

	if ( NULL != pSourcesFile )
		fclose( pSourcesFile );

	//
	// return Success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateSourceCodeFile
 *
 *  Description:
 *      Generate the .cpp file
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateSourceCodeFile( void )
{
	//
	// allocate space for the file name
	//

	TCHAR tszFileName[MAX_PATH];

	//
	// append .cpp to the directory name
	//

	sprintf( tszFileName, "%s.cpp", g_tszDir );

	//
	// create the source file, check for an error
	//

	FILE* pSourceCodeFile = fopen( tszFileName, "wb" );

	if ( NULL == pSourceCodeFile )
		return FALSE;

	//
	// print the basic framework for the source file
	//

	fprintf( pSourceCodeFile, "/***************************************************************************\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  File:       %s.cpp\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *  Content:    %s tests\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *  History:\r\n" );
	fprintf( pSourceCodeFile, " *   Date       By       Reason\r\n" );
	fprintf( pSourceCodeFile, " *   ====       ==       ======\r\n" );
	fprintf( pSourceCodeFile, " *  %2u/%2u/01    %s Created to test Xbox %s\r\n", g_wMonth, g_wDate, g_tszUser, g_tszDir );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " ****************************************************************************/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#include \"%s.h\"\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "/****************************************************************************\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " * The Global Logging Handle\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " ****************************************************************************/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "extern HANDLE g_hLog;\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "/****************************************************************************\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  %s_BasicTest\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Description:\r\n" );
	fprintf( pSourceCodeFile, " *      Basic Test for %s\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Arguments:\r\n" );
	fprintf( pSourceCodeFile, " *      (void)\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Returns:  \r\n" );
	fprintf( pSourceCodeFile, " *      S_OK on success\r\n" );
	fprintf( pSourceCodeFile, " *      E_XX on failure\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " ****************************************************************************/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "HRESULT %s_BasicTest( void )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "{\r\n" );
	fprintf( pSourceCodeFile, "    HRESULT             hr              = S_OK;\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "    // Return the Hresult\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    return hr;\r\n" );
	fprintf( pSourceCodeFile, "}\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "/****************************************************************************\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  %s_StartTest\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Description:\r\n" );
	fprintf( pSourceCodeFile, " *      The Harness Entry into the %s tests\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Arguments:\r\n" );
	fprintf( pSourceCodeFile, " *      LogHandle - a handle to a logging object\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Returns:  \r\n" );
	fprintf( pSourceCodeFile, " *		(void)\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " ****************************************************************************/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "VOID WINAPI %s_StartTest( HANDLE LogHandle )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "{\r\n" );
	fprintf( pSourceCodeFile, "    HRESULT             hr              = S_OK;\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "    // the following tests will Assert (by design) in debug builds\r\n" );
	fprintf( pSourceCodeFile, "    // to turn these tests on (they are off by default) define\r\n" );
	fprintf( pSourceCodeFile, "    // CODE_COVERAGE when this file is compiled\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#ifdef CODE_COVERAGE\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#endif // CODE_COVERAGE\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "    // Test %s in the mannor it was meant to be called\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    if ( SHOULDRUNTEST( \"%s\", \"Basic\" ) )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    {\r\n" );
	fprintf( pSourceCodeFile, "        SETLOG( LogHandle, \"%s\", \"Online\", \"%s\", \"Basic\" );\r\n", g_tszUser, g_tszDir );
	fprintf( pSourceCodeFile, "        EXECUTE( %s_BasicTest() );\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    }\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "}\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "/****************************************************************************\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  %s_EndTest\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Description:\r\n" );
	fprintf( pSourceCodeFile, " *      The Exit function for the test harness\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Arguments:\r\n" );
	fprintf( pSourceCodeFile, " *      (void)\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Returns:  \r\n" );
	fprintf( pSourceCodeFile, " *		(void)\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " ****************************************************************************/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "VOID WINAPI %s_EndTest( VOID )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "{\r\n" );
	fprintf( pSourceCodeFile, "}\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "/****************************************************************************\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  main\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Description:\r\n" );
	fprintf( pSourceCodeFile, " *      the exe entry point\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Arguments:\r\n" );
	fprintf( pSourceCodeFile, " *      (void)\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Returns:  \r\n" );
	fprintf( pSourceCodeFile, " *      (void)\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " ****************************************************************************/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#ifdef NOLOGGING\r\n" );
	fprintf( pSourceCodeFile, "void __cdecl main( void )\r\n" );
	fprintf( pSourceCodeFile, "{\r\n" );
	fprintf( pSourceCodeFile, "    %s_StartTest( NULL );\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    %s_EndTest();\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "}\r\n" );
	fprintf( pSourceCodeFile, "#endif // NOLOGGING\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "/****************************************************************************\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " * Export Function Pointers for StartTest and EndTest\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " ****************************************************************************/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#pragma data_seg( EXPORT_SECTION_NAME )\r\n" );
	fprintf( pSourceCodeFile, "DECLARE_EXPORT_DIRECTORY( %s )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "#pragma data_seg()\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "BEGIN_EXPORT_TABLE( %s )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    EXPORT_TABLE_ENTRY( \"StartTest\", %s_StartTest )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    EXPORT_TABLE_ENTRY( \"EndTest\", %s_EndTest )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "END_EXPORT_TABLE( %s )\r\n", g_tszDir );

	//
	// close the open file
	//

	if ( NULL != pSourceCodeFile )
		fclose( pSourceCodeFile );

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateSourceCodeFileStandard
 *
 *  Description:
 *      Generate the .cpp file (standard comments)
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateSourceCodeFileStandard( void )
{
	//
	// allocate space for the file name
	//

	TCHAR tszFileName[MAX_PATH];

	//
	// append .cpp to the directory name
	//

	sprintf( tszFileName, "%s.cpp", g_tszDir );

	//
	// create the source file, check for an error
	//

	FILE* pSourceCodeFile = fopen( tszFileName, "wb" );

	if ( NULL == pSourceCodeFile )
		return FALSE;

	//
	// print the basic framework for the source file
	//

	fprintf( pSourceCodeFile, "/*++\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Copyright (c) Microsoft Corporation. All rights reserved.\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Module Name:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    %s.cpp\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Abstract:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    %s tests\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "--*/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#include \"%s.h\"\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "//\r\n" );
	fprintf( pSourceCodeFile, "// The Global Logging Handle\r\n" );
	fprintf( pSourceCodeFile, "//\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "extern HANDLE g_hLog;\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "HRESULT\r\n" );
	fprintf( pSourceCodeFile, "%s_BasicTest(\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    void\r\n" );
	fprintf( pSourceCodeFile, "    )\r\n" );
	fprintf( pSourceCodeFile, "/*++\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Routine Description:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    Basic Test for %s\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Arguments:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    None\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Return Value:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    S_OK on success\r\n" );
	fprintf( pSourceCodeFile, "    E_XX on failure\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "--*/\r\n" );
	fprintf( pSourceCodeFile, "{\r\n" );
	fprintf( pSourceCodeFile, "    HRESULT             hr              = S_OK;\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "    // Return the Hresult\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    return hr;\r\n" );
	fprintf( pSourceCodeFile, "}\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "VOID \r\n" );
	fprintf( pSourceCodeFile, "WINAPI \r\n" );
	fprintf( pSourceCodeFile, "%s_StartTest( \r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    HANDLE              LogHandle \r\n" );
	fprintf( pSourceCodeFile, "    )\r\n" );
	fprintf( pSourceCodeFile, "/*++\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Routine Description:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    The harness entry into the %s tests\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Arguments:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    LogHandle - a handle to a logging object\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Return Value:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    None\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "--*/\r\n" );
	fprintf( pSourceCodeFile, "{\r\n" );
	fprintf( pSourceCodeFile, "    HRESULT             hr              = S_OK;\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "    // the following tests will Assert (by design) in debug builds\r\n" );
	fprintf( pSourceCodeFile, "    // to turn these tests on (they are off by default) define\r\n" );
	fprintf( pSourceCodeFile, "    // CODE_COVERAGE when this file is compiled\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#ifdef CODE_COVERAGE\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#endif // CODE_COVERAGE\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "    // Test %s in the mannor it was meant to be called\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    //\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    if ( SHOULDRUNTEST( \"%s\", \"Basic\" ) )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    {\r\n" );
	fprintf( pSourceCodeFile, "        SETLOG( LogHandle, \"%s\", \"Online\", \"%s\", \"Basic\" );\r\n", g_tszUser, g_tszDir );
	fprintf( pSourceCodeFile, "        EXECUTE( %s_BasicTest() );\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    }\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "}\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "VOID \r\n" );
	fprintf( pSourceCodeFile, "WINAPI \r\n" );
	fprintf( pSourceCodeFile, "%s_EndTest( \r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    VOID \r\n" );
	fprintf( pSourceCodeFile, "    )\r\n" );
	fprintf( pSourceCodeFile, "/*++\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Routine Description:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    The exit function for the test harness\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Arguments:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    None\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Return Value:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    None\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "--*/\r\n" );
	fprintf( pSourceCodeFile, "{\r\n" );
	fprintf( pSourceCodeFile, "}\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#ifdef NOLOGGING\r\n" );
	fprintf( pSourceCodeFile, "void \r\n" );
	fprintf( pSourceCodeFile, "__cdecl \r\n" );
	fprintf( pSourceCodeFile, "main( \r\n" );
	fprintf( pSourceCodeFile, "    void \r\n" );
	fprintf( pSourceCodeFile, "    )\r\n" );
	fprintf( pSourceCodeFile, "/*++\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Routine Description:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    the exe entry point\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Arguments:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    None\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Return Value:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    None\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "--*/\r\n" );
	fprintf( pSourceCodeFile, "{\r\n" );
	fprintf( pSourceCodeFile, "    %s_StartTest( NULL );\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    %s_EndTest();\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "}\r\n" );
	fprintf( pSourceCodeFile, "#endif // NOLOGGING\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "//\r\n" );
	fprintf( pSourceCodeFile, "// Export Function Pointers for StartTest and EndTest\r\n" );
	fprintf( pSourceCodeFile, "//\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#pragma data_seg( EXPORT_SECTION_NAME )\r\n" );
	fprintf( pSourceCodeFile, "DECLARE_EXPORT_DIRECTORY( %s )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "#pragma data_seg()\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "BEGIN_EXPORT_TABLE( %s )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    EXPORT_TABLE_ENTRY( \"StartTest\", %s_StartTest )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "    EXPORT_TABLE_ENTRY( \"EndTest\", %s_EndTest )\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "END_EXPORT_TABLE( %s )\r\n", g_tszDir );

	//
	// close the open file
	//

	if ( NULL != pSourceCodeFile )
		fclose( pSourceCodeFile );

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateHeaderFile
 *
 *  Description:
 *      Generate the .h file
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateHeaderFile( void )
{
	//
	// allocate space for the file name
	//

	TCHAR tszFileName[MAX_PATH];

	//
	// append .h to the directory name
	//

	sprintf( tszFileName, "%s.h", g_tszDir );

	//
	// create the header file, check for an error
	//

	FILE* pSourceCodeFile = fopen( tszFileName, "wb" );

	if ( NULL == pSourceCodeFile )
		return FALSE;

	//
	// print the basic framework for the header file
	//

	fprintf( pSourceCodeFile, "/***************************************************************************\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.\r\n" );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " *  File:       %s.h\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *  Content:    %s utility interface\r\n", g_tszDir );
	fprintf( pSourceCodeFile, " *  History:\r\n" );
	fprintf( pSourceCodeFile, " *   Date       By       Reason\r\n" );
	fprintf( pSourceCodeFile, " *   ====       ==       ======\r\n" );
	fprintf( pSourceCodeFile, " *  %2u/%2u/01    %s Created to test Xbox %s\r\n", g_wMonth, g_wDate, g_tszUser, g_tszDir );
	fprintf( pSourceCodeFile, " *\r\n" );
	fprintf( pSourceCodeFile, " ****************************************************************************/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#pragma once\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#include <xtl.h>\r\n" );
	fprintf( pSourceCodeFile, "#include <xdbg.h>\r\n" );
	fprintf( pSourceCodeFile, "#include <xlog.h>\r\n" );
	fprintf( pSourceCodeFile, "#include <macros.h>\r\n" );
	fprintf( pSourceCodeFile, "#include <xtestlib.h>\r\n" );

	//
	// close the open file
	//

	if ( NULL != pSourceCodeFile )
		fclose( pSourceCodeFile );

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateHeaderFileStandard
 *
 *  Description:
 *      Generate the .h file (standard comments)
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateHeaderFileStandard( void )
{
	//
	// allocate space for the file name
	//

	TCHAR tszFileName[MAX_PATH];

	//
	// append .h to the directory name
	//

	sprintf( tszFileName, "%s.h", g_tszDir );

	//
	// create the header file, check for an error
	//

	FILE* pSourceCodeFile = fopen( tszFileName, "wb" );

	if ( NULL == pSourceCodeFile )
		return FALSE;

	//
	// print the basic framework for the header file
	//

	fprintf( pSourceCodeFile, "/*++\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Copyright (c) Microsoft Corporation. All rights reserved.\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Module Name:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    %s.h\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "Abstract:\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "    %s utility interface\r\n", g_tszDir );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "--*/\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#pragma once\r\n" );
	fprintf( pSourceCodeFile, "\r\n" );
	fprintf( pSourceCodeFile, "#include <xtl.h>\r\n" );
	fprintf( pSourceCodeFile, "#include <xdbg.h>\r\n" );
	fprintf( pSourceCodeFile, "#include <xlog.h>\r\n" );
	fprintf( pSourceCodeFile, "#include <macros.h>\r\n" );

	//
	// close the open file
	//

	if ( NULL != pSourceCodeFile )
		fclose( pSourceCodeFile );

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateLibMakeFile
 *
 *  Description:
 *      generate the make file for the lib in this project
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateLibMakeFile( void )
{
	//
	// allocate the make file and check for failure
	//

	FILE* pMakeFile = fopen( "lib\\makefile", "wb" );

	if ( NULL == pMakeFile )
		return FALSE;

	//
	// print out the text ot the makefile
	//

	fprintf( pMakeFile, "#\r\n" );
	fprintf( pMakeFile, "# DO NOT EDIT THIS FILE!!!  Edit .\\sources. if you want to add a new source\r\n" );
	fprintf( pMakeFile, "# file to this component.  This file merely indirects to the real make file\r\n" );
	fprintf( pMakeFile, "# that is shared by all the components of NT OS/2\r\n" );
	fprintf( pMakeFile, "#\r\n" );
	fprintf( pMakeFile, "!INCLUDE $(NTMAKEENV)\\makefile.def\r\n" );

	//
	// close the file handle
	//

	if ( NULL != pMakeFile )
		fclose( pMakeFile );

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateExeMakeFile
 *
 *  Description:
 *      generate the make file for the exe in this project
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateExeMakeFile( void )
{
	//
	// allocate the make file and check for failure
	//

	FILE* pMakeFile = fopen( "exe\\makefile", "wb" );

	if ( NULL == pMakeFile )
		return FALSE;

	//
	// print out the text ot the makefile
	//

	fprintf( pMakeFile, "#\r\n" );
	fprintf( pMakeFile, "# DO NOT EDIT THIS FILE!!!  Edit .\\sources. if you want to add a new source\r\n" );
	fprintf( pMakeFile, "# file to this component.  This file merely indirects to the real make file\r\n" );
	fprintf( pMakeFile, "# that is shared by all the components of NT OS/2\r\n" );
	fprintf( pMakeFile, "#\r\n" );
	fprintf( pMakeFile, "!INCLUDE $(NTMAKEENV)\\makefile.def\r\n" );

	//
	// close the file handle
	//

	if ( NULL != pMakeFile )
		fclose( pMakeFile );

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateDirsFile
 *
 *  Description:
 *      generate the dirs file for this project
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateDirsFile( void )
{
	//
	// allocate the dirsfile and check for failure
	//

	FILE* pFile = fopen( "dirs", "wb" );

	if ( NULL == pFile )
		return FALSE;

	//
	// print out the text ot the makefile
	//

	fprintf( pFile, "DIRS=  exe \\\r\n" );
	fprintf( pFile, "       lib\r\n" );

	//
	// close the file handle
	//

	if ( NULL != pFile )
		fclose( pFile );

	//
	// return success
	//

	return TRUE;
}

/****************************************************************************
 *
 *  GenerateSubDirs
 *
 *  Description:
 *      generate the exe and lib sub dirs
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure
 *
 ****************************************************************************/

BOOL GenerateSubDirs( void )
{
	if ( 0 != _mkdir( "exe" ) )
		return FALSE;

	if ( 0 != _mkdir( "lib" ) )
		return FALSE;

	return true;
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      program entry point
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      0 on success
 *      -1 on failure
 *
 ****************************************************************************/

int __cdecl main( int argc, char* argv[] )
{	
	//
	// Set the global directory
	//

	if ( ! SetDirectory() )
	{
		printf( "Could not resolve the current directory name\n" );
		return -1;
	}

	//
	// set the global date
	//

	if ( ! SetDate() )
	{
		printf( "Could not resolve the current date\n" );
		return -1;
	}
	//
	// set the global user
	//

	if ( ! SetUser() )
	{
		printf( "Could not resolve the current user\n" );
		return -1;
	}

	//
	// generate the sub directories
	//

	if ( ! GenerateSubDirs() )
	{
		printf( "Could not generate lib and exe directories" );
		return -1;
	}

	//
	// generate the sources files
	//

	if ( ! GenerateLibSourcesFile() )
	{
		printf( "Could not generate the sources file for lib\n" );
		return -1;
	}

	if ( ! GenerateExeSourcesFile() )
	{
		printf( "Could not generate the sources file for exe\n" );
		return -1;
	}

	//
	// generate the makefiles
	//

	if ( ! GenerateLibMakeFile() )
	{
		printf( "Could not generate the makefile for lib\n" );
		return -1;
	}

	if ( ! GenerateExeMakeFile() )
	{
		printf( "Could not generate the makefile for exe\n" );
		return -1;
	}

	//
	// generate the dirs file
	//

	if ( ! GenerateDirsFile() )
	{
		printf( "Could not generate the dirs file\n" );
		return -1;
	}

	//
	// generate the .cpp file
	//

	if ( NULL == argv[1] )
	{
		if ( ! GenerateSourceCodeFile() )
		{
			printf( "Could not generate the .cpp file\n" );
			return -1;
		}

		if ( ! GenerateHeaderFile() )
		{
			printf( "Could not generate the .h file\n" );
			return - 1;
		}
	}
	else
	{
		if ( ! GenerateSourceCodeFileStandard() )
		{
			printf( "Could not generate the .cpp file\n" );
			return -1;
		}

		if ( ! GenerateHeaderFileStandard() )
		{
			printf( "Could not generate the .h file\n" );
			return -1;
		}
	}

	//
	// return success
	//

	return 0;
}
	