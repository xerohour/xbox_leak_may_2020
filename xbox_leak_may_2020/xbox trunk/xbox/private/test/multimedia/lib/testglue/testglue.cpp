#include <testGlue.h>

void SeedRandomNumberGenerator( void )
{
	ULONG ulSeed = (ULONG) time( NULL );
	srand( ulSeed );

	DbgPrint( "Random Seed: %u\n", ulSeed );
}

void __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow ) 
{	
	DbgPrint( "Beginning Test" );

	SeedRandomNumberGenerator();
	CovMonRegisterAllNoImportCoverageBinaries();

	RunTest();

	CovMonInternalPollLoggingAllBinaries( NULL );
	CovMonInternalCloseLogFileAllBinaries();

	DbgPrint( "Ending Test" );

	while ( true );

}