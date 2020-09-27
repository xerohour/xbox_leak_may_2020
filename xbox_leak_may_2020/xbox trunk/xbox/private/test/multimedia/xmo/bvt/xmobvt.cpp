#include <xtl.h>
#include <dsutil.h>
#include <srcxmo.h>

VOID WINAPI XMOBVTStartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;
	SETLOG( LogHandle, "danrose", "XMO", "BVT", "StartTest" );

	EXECUTE( PlayOneFile( BVT_FILE, PLAY_MODE_STREAM ) );
}

VOID WINAPI XMOBVTEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xmoBVT )
#pragma data_seg()

BEGIN_EXPORT_TABLE( xmoBVT )
    EXPORT_TABLE_ENTRY( "StartTest", XMOBVTStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XMOBVTEndTest )
END_EXPORT_TABLE( xmoBVT )