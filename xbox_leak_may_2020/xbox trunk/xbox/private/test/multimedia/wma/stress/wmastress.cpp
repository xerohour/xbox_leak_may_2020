#include <xtl.h>
#include <dsutil.h>

HRESULT WMAStress( void )
{
	HRESULT hr;
	WIN32_FIND_DATA findData;
	BOOL bMore;
	HANDLE hFiles;

	CHAR szFullName[MAX_PATH];
	LPSTR szFiles = NULL;

	bMore = TRUE;

	hFiles = FindFirstFile( WMA_FILES, &findData );

	while ( INVALID_HANDLE_VALUE != hFiles && TRUE == bMore )
	{

		sprintf( szFullName, "%s\\%s", WMA_DRIVE, findData.cFileName );

		hr = PlayWmaFile( szFullName, true );

		bMore = FindNextFile( hFiles, &findData );
	}

	FindClose( hFiles );

	return hr;
}

VOID WINAPI WMAStressStartTest( HANDLE LogHandle )
{
	SETLOG( LogHandle, "danrose", "Wma", "Stress", "StartTest" );

	HRESULT hr = S_OK;

	EXECUTE( WMAStress() );
}

VOID WINAPI WMAStressEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( wmaStress )
#pragma data_seg()

BEGIN_EXPORT_TABLE( wmaStress )
    EXPORT_TABLE_ENTRY( "StartTest", WMAStressStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WMAStressEndTest )
END_EXPORT_TABLE( wmaStress )