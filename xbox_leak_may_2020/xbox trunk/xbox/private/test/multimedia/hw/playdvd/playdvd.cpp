#include <xtl.h>
#include <dsoundtest.h>

HRESULT PlayOneFile( LPSTR szFileName )
{
	CDSoundTest testFile;
	testFile.SetBufferDuration( DEFAULT_BUFFER_DURATION );
	testFile.SetPlayFlags( DSBPLAY_LOOPING );

	CHAR szFile[MAX_PATH];

	sprintf( szFile, "%s\\%s", DVD_DRIVE, szFileName );

	return testFile.OpenAndPlay( szFile );
}

void __cdecl main()
{
	HRESULT hr;
	WIN32_FIND_DATA findData;
	BOOL bMore;
	HANDLE hFiles;

	while ( true )
	{
	   	bMore = TRUE;

		hFiles = FindFirstFile( DVD_FILES, &findData );

		while ( INVALID_HANDLE_VALUE != hFiles && TRUE == bMore )
		{
			hr = PlayOneFile( findData.cFileName );
			bMore = FindNextFile( hFiles, &findData );
		}

		FindClose( hFiles );
	}
}


