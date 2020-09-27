#include <dsutil.h>

VOID WINAPI DsoundBVTStartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;
	SETLOG( LogHandle, "danrose", "DSOUND", "Functionality", "BVT" );
	EXECUTE( TestBVTPlayback() );
}

VOID WINAPI DsoundBVTEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dsBVT )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dsBVT )
    EXPORT_TABLE_ENTRY( "StartTest", DsoundBVTStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DsoundBVTEndTest )
END_EXPORT_TABLE( dsBVT )



