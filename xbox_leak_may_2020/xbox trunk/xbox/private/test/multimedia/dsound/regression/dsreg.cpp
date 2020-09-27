#include <dsutil.h>

VOID WINAPI DsoundRegStartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;

#ifdef SILVER

	SETLOG( LogHandle, "danrose", "DSOUND", "Regression", "SimplePlayback" );
	EXECUTE( TestSimplePlayback() );

	SETLOG( LogHandle, "danrose", "DSOUND", "Regression", "PollingPlayback" );
	EXECUTE( TestPollingPlayback() );

#endif // SILVER

	SETLOG( LogHandle, "danrose", "DSOUND", "Regression", "BufferedPlayback" );
	EXECUTE( TestBufferedPlayback() );
//	EXECUTE( TestOutOfMemory() );

	SETLOG( LogHandle, "danrose", "DSOUND", "Regression", "SimpleConstruction" );
	EXECUTE( TestSimpleConstruction() );

	SETLOG( LogHandle, "danrose", "DSOUND", "Regression", "SequentialPlayback" );
	EXECUTE( TestSequentialPlayback() );

	SETLOG( LogHandle, "danrose", "DSOUND", "Regression", "SilmontaneousPlayback" );
	EXECUTE( TestSimultaneousPlayback() );

	SETLOG( LogHandle, "danrose", "DSOUND", "Regression", "MultiThreaded" );
	EXECUTE( TestMultiThreaded( ThreadTest ) ); 
}

VOID WINAPI DsoundRegEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dsReg )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dsReg )
    EXPORT_TABLE_ENTRY( "StartTest", DsoundRegStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DsoundRegEndTest )
END_EXPORT_TABLE( dsReg )