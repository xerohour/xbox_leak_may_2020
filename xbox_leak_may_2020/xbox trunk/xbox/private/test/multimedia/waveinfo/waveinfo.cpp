#include <xtl.h>
#include <WaveLdr.h>
#include <DbgTrace.h>

void __stdcall wWinMain ( 
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPTSTR lpCmdLine, 
	int nCmdShow 
) 
{

	BOOL			bDone					= TRUE;
	HRESULT			hr						= S_OK;

	TCHAR			tszFullPath[MAX_PATH];
	TCHAR			tszInfo[MAX_PATH];
	CWaveFile		waveFile;
	WAVEFORMATEX	wavFormat;
	WIN32_FIND_DATA findFileData;
	DWORD			dwDuration;
	DWORD			dwAmount;
	DWORD			dwNumWritten;
	HANDLE			hFileList;
	HANDLE			hFile;
	float			fDuration;

	DBGTRACE( TEXT( "Starting app" ) );

	hFile = CreateFile( TEXT( "t:\\media\\bvt\\format.log" ),
						GENERIC_WRITE,
						0,
						NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL );

	hFileList = FindFirstFile( TEXT( "t:\\media\\bvt\\*.wav" ), &findFileData );

	while ( INVALID_HANDLE_VALUE != hFile && 
		    INVALID_HANDLE_VALUE != hFileList && 
			TRUE == bDone && 
			SUCCEEDED( hr ) ) 
	{

		wsprintf( tszFullPath, TEXT( "t:\\media\\bvt\\%s" ), findFileData.cFileName );

		hr = waveFile.Open( tszFullPath );
		
		if ( SUCCEEDED( hr ) ) 
		{
			hr = waveFile.GetFormat( &wavFormat, sizeof( WAVEFORMATEX ), &dwAmount );
		}

		if ( SUCCEEDED( hr ) ) 
		{
			hr = waveFile.GetDuration( &dwDuration );
		}

		if ( SUCCEEDED( hr ) ) 
		{
			fDuration = (float) dwDuration / (float) ( wavFormat.nSamplesPerSec * wavFormat.wBitsPerSample * wavFormat.nChannels / 8 );

			swprintf( tszInfo,  
				      TEXT( "File: %11s " )				\
					  TEXT( "Duration: %10f " )			\
					  TEXT( "Samples per sec: %6u " )	\
					  TEXT( "Bits Per sample: %3u " )	\
					  TEXT( "Num channels: %u\r\n"  ), 
					  findFileData.cFileName, 
					  fDuration, 
					  wavFormat.nSamplesPerSec, 
					  wavFormat.wBitsPerSample, 
					  wavFormat.nChannels );

			bDone = WriteFile( hFile, 
							   (void*) tszInfo, 
							   sizeof( TCHAR ) * wcslen( tszInfo ),
							   &dwNumWritten,
							   NULL );

			if ( bDone ) {
				bDone = FindNextFile( hFileList, &findFileData );
			}
		}
	}
	
	if ( INVALID_HANDLE_VALUE != hFileList )
		FindClose( hFileList );

	if ( INVALID_HANDLE_VALUE != hFile )
		CloseHandle( hFile );
	
	DBGTRACE( TEXT( "Ending app" ) );

}
