#include <xtl.h>
#include <dsutil.h>

HRESULT MMStress( void )
{
	HRESULT hr;
	WIN32_FIND_DATA findData;
	BOOL bMore;
	HANDLE hFiles;

	CHAR szFullName[MAX_PATH];
	LPSTR szFiles = NULL;

	int iRand = 0;
	int iRand2 = 0;

	while ( true )
	{
	   	bMore = TRUE;

		iRand2 = rand() % 2;

		hFiles = FindFirstFile( 0 == iRand2 ? DVD_FILES : WMA_FILES, &findData );

		while ( INVALID_HANDLE_VALUE != hFiles && TRUE == bMore )
		{
			iRand = rand() % 4;

            //
            // added checking code to see if findfirst/next or printf is giving 
            // us a null pointer. yes, it is overkill.
            //     

            //
            // looking for a stress failure where it appears we pass a NULL 
            // pointer from here
            //
            
            if (NULL == findData.cFileName) {
                OutputDebugStringA("NULL == findData.cFileName - send email to xstress!\n");
                DebugBreak();
            }
			
            sprintf( szFullName, "%s\\%s", 0 == iRand2 ? DVD_DRIVE : WMA_DRIVE, findData.cFileName );

            //
            // looking for a stress failure where it appears we pass a NULL 
            // pointer from here
            //

            if (NULL == szFullName) {
                OutputDebugStringA("NULL == szFullName - send email to xstress!\n");
                DebugBreak();
            }

			if ( 0 == iRand2 )
				hr = PlayOneFile( szFullName, (PLAY_MODE) iRand, true );
			else
				hr = PlayWmaFile( szFullName );

			bMore = FindNextFile( hFiles, &findData );
		}

		if ( INVALID_HANDLE_VALUE != hFiles )
			FindClose( hFiles );
	}

	return hr;
}

VOID WINAPI MMStressStartTest( HANDLE LogHandle )
{
	SETLOG( LogHandle, "danrose", "Multimedia", "Stress", "StartTest" );

	HRESULT hr = S_OK;

	EXECUTE( MMStress(); );
}

VOID WINAPI MMStressEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( mmStress )
#pragma data_seg()

BEGIN_EXPORT_TABLE( mmStress )
    EXPORT_TABLE_ENTRY( "StartTest", MMStressStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MMStressEndTest )
END_EXPORT_TABLE( mmStress )