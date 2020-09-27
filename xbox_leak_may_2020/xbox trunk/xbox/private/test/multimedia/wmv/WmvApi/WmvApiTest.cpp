/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvApiTest.cpp

Abstract:

	General API tests including parameter tests.

Author:

	Jeff Sullivan (jeffsul) 13-Sep-2001

Revision History:

	13-Sep-2001 jeffsul
		Initial Version

--*/

#include "WmvApiTest.h"

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xgmath.h>
#include <medialoader.h>

#include <stdio.h>
#include <tchar.h>

/*++

Routine Description:

	Test constructor

Arguments:

	None

Return Value:

	None

--*/
CWmvApiTest::CWmvApiTest()
{
}


/*++

Routine Description:

	Test destructor

Arguments:

	None

Return Value:

	None

--*/
CWmvApiTest::~CWmvApiTest()
{
}


/*++

Routine Description:

	read initialization variables from an ini file

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvApiTest::ReadIni()
{
	HRESULT hr = S_OK;

	// grab media files from server
#define CREATE_FLAG	MLCOPY_IFNEWER
	MLSetErrorLevel( 3 );
	MEDIAFILEDESC MediaFiles[] =
	{
		{"ml test/",	"T:\\ml test\\",			CREATE_FLAG},
		{NULL,									NULL,								0}
	};
	EXECUTE( LoadMedia( MediaFiles ) );

	ExecuteParameterChecks();

	m_bQuit = TRUE;

	return hr;
}


#define CHECKEXPECTED( expected, func )		\
	hr = func;								\
	if ( expected != hr )					\
	{										\
		DbgPrint( #func ## " returned %d, expected %d\n", hr, expected );	\
		ASSERT( 0 );						\
	}										\
	if ( SUCCEEDED( hr ) )					\
	{										\
		RELEASE( pWmvDecoder );				\
	}

LPBYTE g_AllocBuffer = NULL;
HANDLE g_File = INVALID_HANDLE_VALUE;
DWORD CALLBACK ExampleCallback(
    LPVOID pvContext,
    DWORD dwOffset,
    DWORD dwByteCount,
    LPVOID* ppvData
    )
{
	DWORD cb;
	ASSERT(dwByteCount <= 512);
	if (SetFilePointer(g_File, dwOffset, NULL, FILE_BEGIN) != dwOffset ||
		!ReadFile(g_File, g_AllocBuffer, dwByteCount, &cb, NULL)) {
		XDBGWRN("WMVDEC", "ReadFile failed: %d", GetLastError());
		return 0;
	}

	if ( NULL != pvContext && 64 != *(INT*)pvContext )
	{
		DbgPrint( "pvContext passed improperly (value = %d)\n", *(INT*)pvContext );
		ASSERT( 0 );	
	}
	
	*ppvData = g_AllocBuffer;
	return cb;
}

VOID
CWmvApiTest::ExecuteParameterChecks()
{
	HRESULT				hr					= S_OK;
	HRESULT				hrExpected			= S_OK;
	LPWMVDECODER		pWmvDecoder			= NULL;
	LPWMVVIDEOINFO		pWmvVideoInfo		= NULL;
	CHAR				szBadFileName[]		= "test.file";
	CHAR				szFileName[]		= "T:\\WMV\\test.asf";
	INT					nTest				= 64;

	// set up variables for callback function
	g_File = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == g_File )
	{
		DbgPrint( "Unable to open file %s\n", szFileName );
		return;
	}
	DWORD bufsize = 2 * ( 2*1024 + 64*1024 );
    g_AllocBuffer = (LPBYTE) VirtualAlloc( NULL, bufsize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if ( NULL == g_AllocBuffer )
	{
		CloseHandle( g_File );
		DbgPrint( "Ran out of memory allocating buffer\n" );
	}

	// check IN LPCSTR pszFileName
	hrExpected = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
//	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szBadFileName, NULL, WMVVIDEOFORMAT_RGB32, NULL, &pWmvDecoder ) );
	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szBadFileName, NULL, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder ) );
//	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szBadFileName, ExampleCallback, WMVVIDEOFORMAT_RGB32, NULL, &pWmvDecoder ) );
	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szBadFileName, ExampleCallback, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder ) );
	hrExpected = S_OK;
//	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, NULL, WMVVIDEOFORMAT_RGB32, NULL, &pWmvDecoder ) );
	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, NULL, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder ) );
//	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, ExampleCallback, WMVVIDEOFORMAT_RGB32, NULL, &pWmvDecoder ) );
	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, ExampleCallback, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder ) );
	// needs better handling of this case:
	//CHECKEXPECTED( hrExpected, WmvCreateDecoder( NULL, NULL, WMVVIDEOFORMAT_RGB32, NULL, &pWmvDecoder ) );
	//CHECKEXPECTED( hrExpected, WmvCreateDecoder( NULL, NULL, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder ) );

	// check IN LPFNWMVXMODATACALLBACK pfnCallback
	hrExpected = S_OK;
//	CHECKEXPECTED( hrExpected, WmvCreateDecoder( NULL, ExampleCallback, WMVVIDEOFORMAT_RGB32, NULL, &pWmvDecoder ) );
	CHECKEXPECTED( hrExpected, WmvCreateDecoder( NULL, ExampleCallback, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder ) );
	// check IN LPVOID pvContext
//	CHECKEXPECTED( hrExpected, WmvCreateDecoder( NULL, ExampleCallback, WMVVIDEOFORMAT_RGB32, &nTest, &pWmvDecoder ) );
	CHECKEXPECTED( hrExpected, WmvCreateDecoder( NULL, ExampleCallback, WMVVIDEOFORMAT_YUY2, &nTest, &pWmvDecoder ) );


	// check IN DWORD dwVideoOutputFormat
	hrExpected = S_OK;
//	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, NULL, 0, NULL, &pWmvDecoder ) ); // zero maps to WMVVIDEOFORMAT_RGB32
//	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, NULL, WMVVIDEOFORMAT_RGB32, NULL, &pWmvDecoder ) );
	CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, NULL, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder ) );
	hrExpected = E_FAIL;
	// needs better handling of this case:
	//CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, NULL, ULONG_MAX, NULL, &pWmvDecoder ) );

    // check OUT LPWMVDECODER* ppWmvDecoder
	hrExpected = E_FAIL;
	// unhandled exception: passing NULL for OUT LPWMVDECODER* ppWmvDecoder
	//CHECKEXPECTED( E_FAIL, WmvCreateDecoder( szFileName, NULL, WMVVIDEOFORMAT_RGB32, NULL, NULL ) );
	// produces compiler error (as expected):
	//CHECKEXPECTED( hrExpected, WmvCreateDecoder( szFileName, NULL, WMVVIDEOFORMAT_RGB32, NULL, &pWmvVideoInfo ) );


	// destroy variables for callback function
	VirtualFree( g_AllocBuffer, 0, MEM_RELEASE );
	CloseHandle( g_File );
}