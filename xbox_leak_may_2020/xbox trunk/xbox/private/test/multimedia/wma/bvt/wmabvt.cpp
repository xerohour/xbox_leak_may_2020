#include <dsutil.h>

VOID WINAPI WMABVTStartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;
	SETLOG( LogHandle, "danrose", "WMA", "Bvt", "PlayWmaFile" );
	EXECUTE( PlayWmaFile( WMA_BVT_FILE ) );
}

VOID WINAPI WMABVTEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( WMABVT )
#pragma data_seg()

BEGIN_EXPORT_TABLE( WMABVT )
    EXPORT_TABLE_ENTRY( "StartTest", WMABVTStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WMABVTEndTest )
END_EXPORT_TABLE( WMABVT )



