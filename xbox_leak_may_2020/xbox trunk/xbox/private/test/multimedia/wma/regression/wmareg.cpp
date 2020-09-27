#include <dsutil.h>

VOID WINAPI WMARegStartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;
	SETLOG( LogHandle, "danrose", "WMA", "Regression", "WMAMultiThreaded" );
	EXECUTE( TestMultiThreaded( WMAThreadTest ) ); 
}

VOID WINAPI WMARegEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( WMAReg )
#pragma data_seg()

BEGIN_EXPORT_TABLE( WMAReg )
    EXPORT_TABLE_ENTRY( "StartTest", WMARegStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WMARegEndTest )
END_EXPORT_TABLE( WMAReg )



