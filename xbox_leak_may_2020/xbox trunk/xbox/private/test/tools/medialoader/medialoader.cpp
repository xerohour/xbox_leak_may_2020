/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	medialoader.cpp

Abstract:

	copies media files from a server to the xbox

Author:

	Jeff Sullivan (jeffsul) 24-Jul-2001

Revision History:

	24-Jul-2001 jeffsul
		Initial Version

--*/

#include "medialoader.h"
#include "FileList.h"
#include "MLInternal.h"

#include <xnetref.h>
//#include <xtestlib.h>
//#include <xlog.h>
//#include <macros.h>
//#include <XboxDbg.h>

//#include <d3d8perf.h>
//#include <d3d8types.h>

//#include <stdio.h>
#include "regexpr.h"
using namespace regex;
//#include <CHAR.H>

#define ML_APPNAME	"MEDIAFILES"

extern HANDLE g_hLog;

// 172.26.160.17 = xstress
#define	ML_SERVER_IP	"172.26.172.21"
#define ML_SERVER_PORT	80
#define ML_SERVER_ROOT	"/stresscontent/"

static SOCKET	sock			=	0;

//------------------------------------------------------------------------------
//	Pre-define functions
//------------------------------------------------------------------------------
HRESULT
InitNetwork();

HRESULT 
DestroyNetwork();

HRESULT
ReconnectSocket();

BOOL 
FindNextFileRemote( 
	IN CFileList* 	pFileList, 
	OUT LPWIN32_FIND_DATA lpFindFileData 
);

CFileList* 
FindFirstFileRemote( 
	IN LPCSTR	lpFileName, 
	OUT LPWIN32_FIND_DATA lpFindFileData  
);

HRESULT 
CopyFileRemote( 
	IN CONST PMEDIAFILEDESC		pMediaFileDesc, 
	IN CONST LPWIN32_FIND_DATA	lpFindFileData 
);

BOOL 
ProduceFullFileName( 
	IN LPCSTR	lpszDestinationPath, 
	IN LPCSTR	lpszRemoteFilePath, 
	OUT LPSTR	lpszReturnedPath 
);

HANDLE 
CreateFile( 
	IN LPCSTR	lpszFilePath, 
	IN DWORD	dwFlags 
);

BOOL 
GetConst(
	IN LPCSTR	lpszStr, 
	IN INT		cchStr, 
	OUT PDWORD	pval
);

static INT g_nErrorLevel = 5;
VOID _stdcall
MLSetErrorLevel( 
	IN INT nErrorLevel 
)
{
	g_nErrorLevel = nErrorLevel;
}

#define ML_OUTPUT( level, func )	\
	if ( level <= g_nErrorLevel )	\
	{								\
		func;						\
	}

HRESULT _stdcall
LoadMedia( 
	IN MEDIAFILEDESC MediaFileDesc[] 
)
{
	HRESULT		hr		=		S_OK;
	UINT		i		=		0;
    BOOL        bFoundSomething = FALSE;

	EXECUTE( InitNetwork() );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	while ( NULL != MediaFileDesc[i].lpszFromPath )
	{	
        if (MediaFileDesc[i].dwFlags > MLCOPY_MAXVALUE)
        {
    		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: MediaFileDesc element %d in array has invalid dwFlags.\nAre you sure you terminated your array with a NULL structure?\n", i) );
            return E_FAIL;
        }

        if (NULL == MediaFileDesc[i].lpszFromPath[0])
        {
    		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: MediaFileDesc element %d in array has empty lpszFromPath.\n", i));
            return E_FAIL;
        }
        
        
        /*
        if (MediaFileDesc[i].lpszFromPath[strlen(MediaFileDesc[i].lpszFromPath) - 1] == '/')
        {
    		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: MediaFileDesc element %d in array ends in '/'",i) );
            return E_FAIL;
        }
        */

		//------------------------------------------------------------------------------
		//	parse out file search string and number of random files
		//------------------------------------------------------------------------------
		CHAR	szSearchString[MAX_PATH];
		LPSTR	lpszFromFile	= NULL;
		LPCSTR lpszNumFiles	= NULL;
		UINT	nNumFiles		= 1;
		BOOL	bGetAllFiles	= FALSE;

		strcpy( szSearchString, MediaFileDesc[i].lpszFromPath );
		lpszFromFile = strtok( szSearchString, "\t%" );
		ML_OUTPUT( 5, DbgPrint( "[MediaLoader] Info: Getting '%s'\n", MediaFileDesc[i].lpszFromPath ) );
		if ( NULL == lpszFromFile )
		{
			ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: Unable to parse media file request '%s'\n", MediaFileDesc[i].lpszFromPath ) );
			return E_FAIL;
		}

		lpszNumFiles = strtok( NULL, " %\t" );
		if ( NULL != lpszNumFiles )
		{
			nNumFiles = atoi( lpszNumFiles );
			if ( 0 == nNumFiles )
			{
				// some kind of error in atoi
				ML_OUTPUT( 2, DbgPrint( "[MediaLoader] Warning: Number of random files parsed incorrectly, setting to 1\n" ) );
				nNumFiles = 1;
			}
		}
		else if ( '/' == szSearchString[ strlen( szSearchString ) - 1 ] )
		{
			// get all the files in the directory
			bGetAllFiles = TRUE;
			// add *.*
			strcat( lpszFromFile, "(\\w*).(\\w*)" );
		}
		else
		{
			bGetAllFiles = TRUE;
		}

		//------------------------------------------------------------------------------
		//	get files that match the search string
		//------------------------------------------------------------------------------
		CFileList*				pFileList	=	NULL;
		WIN32_FIND_DATA			FindData;

		pFileList = FindFirstFileRemote( lpszFromFile, &FindData );
		if ( NULL == pFileList )
		{
			ML_OUTPUT( 2, DbgPrint( "[MediaLoader] Warning: No files found that match '%s'\n", lpszFromFile ) );
			i++;
			continue;
		}

		if ( TRUE == bGetAllFiles )
		{
			nNumFiles = pFileList->GetNumFiles();
		}

		//------------------------------------------------------------------------------
		//	copy up to nNumFiles that match the specified search
		//------------------------------------------------------------------------------
		for ( UINT j=0; j < nNumFiles; j++ )
		{
			BOOL	bSuccess;


            bFoundSomething = TRUE;

			// copy a matching file to the xbox with the specified flags
			hr =  CopyFileRemote( &MediaFileDesc[i], &FindData );
			if ( FAILED( hr ) )
			{
				ML_OUTPUT( 2, DbgPrint( "[MediaLoader] Warning: Unable to copy file '%s'\n", FindData.cFileName ) );
			}

			if ( TRUE == bGetAllFiles )
			{
                HRESULT hrLocal = pFileList->GetFirstUnusedFile( &FindData );
				if ( FAILED( hrLocal ) && j != nNumFiles-1 )
				{
					ML_OUTPUT( 3, DbgPrint( "[MediaLoader] Warning: Only copied %d of expected %d files that match '%s'\n", j+1, nNumFiles, lpszFromFile ) );
					break;
				}

			}
			else
			{
				bSuccess = FindNextFileRemote( pFileList, &FindData );
				if ( FALSE == bSuccess && j != nNumFiles-1 )
				{
					ML_OUTPUT( 3, DbgPrint( "[MediaLoader] Warning: Only copied %d of expected %d files that match '%s'\n", j+1, nNumFiles, lpszFromFile ) );
					break;
				}
			}
		}

		SAFEDELETE( pFileList );
		i++;
	}

	if ( 0 == i )
	{
		ML_OUTPUT( 2, DbgPrint( "[MediaLoader] Warning: No files copied\n" ) );
	}

    if (!bFoundSomething)
    {
		ML_OUTPUT( 1, DbgPrint( "[MediaLoader] Error: No files were found by any of the search patterns.\n" ) );
        hr = E_FAIL;
    }
	else
	{
		hr = S_OK;
	}

	DestroyNetwork();

	return hr;
}


#define CHECKTOKEN( p )																\
if ( NULL == (p) )																	\
{																					\
	DbgPrint( "[MediaLoader] Warning: FILE%d not loaded, bad .ini value\n", i );		\
	continue;																		\
}

#if 0
HRESULT _stdcall
LoadMediaFromIni( 
	IN LPCSTR lpszIniFile 
)
{
	HRESULT			hr				= S_OK;

	//------------------------------------------------------------------------------
	//	allocate space for structure
	//------------------------------------------------------------------------------

	UINT			nNumFiles		= 0;
	PMEDIAFILEDESC	pMediaFileDescs = NULL;
	CHAR	szKeyName[MAX_PATH];
	CHAR	szValue[MAX_PATH*3];

	// find out how many files are listed
	while ( TRUE )
	{
		sprintf( szKeyName, "FILE%d", nNumFiles );
		GetIniString( ML_APPNAME, szKeyName, "Default", szValue, MAX_PATH*3, lpszIniFile );
		if ( 0 == strcmp( szValue, "Default" ) )
		{
			// end of files
			break;
		}
		nNumFiles++;
	}

	if ( 0 == nNumFiles )
	{
		ML_OUTPUT( 2, DbgPrint( "[MediaLoader] Warning: '%s' contains no file list or does not exist.  No files copied.\n", lpszIniFile ) );
		return S_FALSE;
	}

	// allocate the memory, add one struct for NULL terminator
	pMediaFileDescs = new MEDIAFILEDESC[ nNumFiles + 1 ];
	CHECKALLOC( pMediaFileDescs );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	//------------------------------------------------------------------------------
	//	set up the media file descriptions from the ini file
	//------------------------------------------------------------------------------
	for ( UINT i=0; i < nNumFiles; i++ )
	{
		LPCSTR	lpszToken;

		// get line from ini
		sprintf( szKeyName, "FILE%d", i );
		GetIniString( ML_APPNAME, szKeyName, "Default", szValue, MAX_PATH*3, lpszIniFile );
		if ( 0 == strcmp( szValue, "Default" ) )
		{
			ML_OUTPUT( 3, DbgPrint( "[MediaLoader] Warning: FILE%d not found in .ini, skipping\n", i ) );
			break;
		}

		// allocate memory for strings
		pMediaFileDescs[i].lpszFromPath = new CHAR [MAX_PATH];
		if ( NULL == pMediaFileDescs[i].lpszFromPath )
		{
			return E_OUTOFMEMORY;
		}
		pMediaFileDescs[i].lpszToPath = new CHAR [MAX_PATH];
		if ( NULL == pMediaFileDescs[i].lpszFromPath )
		{
			return E_OUTOFMEMORY;
		}

		// get the path to search on the remote server
		lpszToken = strtok( szValue, " \t" );
		CHECKTOKEN( lpszToken );
		strcpy( pMediaFileDescs[i].lpszFromPath, lpszToken );

		// get the directory to place the files in on the xbox
		lpszToken = strtok( NULL, " \t" );
		CHECKTOKEN( lpszToken );
		if ( '%' == lpszToken[0] )
		{
			// there was a random number of files specified
			strcat( pMediaFileDescs[i].lpszFromPath, " " );
			strcat( pMediaFileDescs[i].lpszFromPath, lpszToken );
			lpszToken = strtok( NULL, " \t" );
			CHECKTOKEN( lpszToken );
		}
		strcpy( pMediaFileDescs[i].lpszToPath, lpszToken );

		// get the copy flags
		lpszToken = strtok( NULL, " \t" );
		if ( NULL == lpszToken )
		{
			// creation flag not specified, set it to the default and move on
			pMediaFileDescs[i].dwFlags = COPY_ALWAYS;
			continue;
		}
		BOOL bSuccess = FALSE;
		bSuccess = GetConst( lpszToken, MAX_PATH, &pMediaFileDescs[i].dwFlags );
		if ( FALSE == bSuccess )
		{
			// creation flag not specified properly, set it to the default and move on
			pMediaFileDescs[i].dwFlags = COPY_ALWAYS;
			continue;
		}
	}

	// fill in the last strcut with NULLs
	pMediaFileDescs[ nNumFiles ].lpszFromPath = NULL;
	pMediaFileDescs[ nNumFiles ].lpszToPath = NULL;
	pMediaFileDescs[ nNumFiles ].dwFlags = 0;

	//------------------------------------------------------------------------------
	//	call the media loader
	//------------------------------------------------------------------------------

	EXECUTE( LoadMedia( pMediaFileDescs ) );

	//------------------------------------------------------------------------------
	//	clean up
	//------------------------------------------------------------------------------
	for ( i=0; i < nNumFiles; i++ )
	{
		SAFEDELETEARRAY( pMediaFileDescs[i].lpszFromPath );
		SAFEDELETEARRAY( pMediaFileDescs[i].lpszToPath );
	}

	SAFEDELETEARRAY( pMediaFileDescs );

	return hr;
}
#endif 0


CFileList* 
FindFirstFileRemote( 
	IN LPCSTR lpFileName, 
	OUT LPWIN32_FIND_DATA lpFindFileData  
)
{
	LPBYTE	lpHTMLResponse	=	NULL;
	CHAR	szRequest[HTTP_MAX_HEADER_SIZE];
	CHAR	szFileName[MAX_PATH];

	// create the request
	UINT nRequestLength;
	
	CHAR	szDirectory[MAX_PATH];
	CHAR	szDirectoryNoSpaces[MAX_PATH];
	LPCSTR	lpszToken;	
	szDirectory[0] = 'a';
	strcpy( szDirectory, "/stresscontent/" );
	lpszToken = strrchr( lpFileName, '/' );
	if ( NULL == lpszToken )
	{
		lpszToken = lpFileName + strlen( lpFileName ) + 1;
	}
	strncat( szDirectory, lpFileName, lpszToken-lpFileName );
	strcat( szDirectory, "/" );
	strcpy( szFileName, lpszToken+1 );

	UINT	i, j;
	UINT	nLength = strlen( szDirectory );
	j = 0;
	for ( i=0; i <= nLength; i++ )
	{
		if ( ' ' == szDirectory[i] )
		{
			szDirectoryNoSpaces[j] = '%';
			szDirectoryNoSpaces[j+1] = '2';
			szDirectoryNoSpaces[j+2] = '0';
			j += 3;
		}
		else
		{
			szDirectoryNoSpaces[j] = szDirectory[i];
			j++;
		}

		if ( j >= MAX_PATH )
		{
			ML_OUTPUT( 0, DbgPrint("[MediaLoader] Error: Too many characters in filename.\n") );
			return NULL;
		}
	}

	nRequestLength = sprintf(szRequest, 
		"GET %s HTTP/1.1\r\n"
		"Accept: */*\r\n"
		"User-Agent: SMC\r\n"
		"Connection: Keep-Alive\r\n"
		"Host: %s\r\n"
		"\r\n",
		szDirectoryNoSpaces,
		ML_SERVER_IP );
	
	// send the request over the socket
	INT nNumBytes;
	nNumBytes = HTTPSendRequest( sock, szRequest, nRequestLength );
	if ( SOCKET_ERROR == nNumBytes || nNumBytes != nRequestLength )
	{
		ML_OUTPUT( 0, DbgPrint("[MediaLoader] Error: Unable to send request over socket (error %d)\n", WSAGetLastError()) );
		return NULL;
	}

	// receive the response
	lpHTMLResponse = new BYTE[HTTP_MAX_HTML_SIZE];
	if( NULL == lpHTMLResponse )
	{
		return NULL;
	}
	nNumBytes = HTTPGetReply( sock, lpHTMLResponse, HTTP_MAX_HTML_SIZE );
	if ( SOCKET_ERROR == nNumBytes )
	{
		ML_OUTPUT( 0, DbgPrint("[MediaLoader] Error: Unable to receive response on socket (error %d)\n", WSAGetLastError()) );
		SAFEDELETEARRAY( lpHTMLResponse );
		return NULL;
	}
	else if ( 0 == nNumBytes )
	{
		// re-connect the socket
		HRESULT hr = ReconnectSocket();
		if ( FAILED( hr ) )
		{
			nNumBytes = SOCKET_ERROR;
		}
		SAFEDELETEARRAY( lpHTMLResponse );
		return NULL;
	}

	lpHTMLResponse[nNumBytes+1] = '\0';

#ifdef ML_OUTPUT_HTML

	static INT nNumHTMLFiles = 0;
	HANDLE	hHTMLFile = INVALID_HANDLE_VALUE;
	CHAR	szHTMLFileName[MAX_PATH];
	DWORD	dwNumHTMLBytes;
	sprintf( szHTMLFileName, "T:\\HTML\\%d.html", nNumHTMLFiles );
	nNumHTMLFiles++;
	hHTMLFile = CreateFile( szHTMLFileName, CREATE_ALWAYS );
	if( INVALID_HANDLE_VALUE == hHTMLFile )
	{
		DbgPrint( "[MediaLoader] Warning: Unable to open HTML file '%s' for writing\n", szHTMLFileName );
	}
	if ( !WriteFile( hHTMLFile, lpHTMLResponse, nNumBytes, &dwNumHTMLBytes, NULL ) )
	{
		DbgPrint( "[MediaLoader] Warning: Unable to write HTML file '%s' (error %d)\n", szHTMLFileName, GetLastError() );
	}
	CloseHandle( hHTMLFile );

#endif ML_OUTPUT_HTML

	// create the file list
	HRESULT hr = S_OK;
	CFileList*	pFileList = new CFileList;
	CHECKALLOC( pFileList );
	if ( FAILED( hr ) )
	{
		SAFEDELETEARRAY( lpHTMLResponse );
		return NULL;
	}

	hr = pFileList->ParseHTML( (LPCSTR)lpHTMLResponse, nNumBytes, szFileName );
	if ( FAILED( hr ) )
	{
		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: Unable to parse HTML properly\n" ) );
		SAFEDELETE( pFileList );
		SAFEDELETEARRAY( lpHTMLResponse );
		return NULL;
	}
	
	// get the first file from the list
	hr =  pFileList->GetRandomUnusedFile( lpFindFileData );
	if ( FAILED( hr ) )
	{
		SAFEDELETE( pFileList );
		SAFEDELETEARRAY( lpHTMLResponse );
		return NULL;
	}
	
	// return a pointer to the find files structure
	SAFEDELETEARRAY( lpHTMLResponse );
	return pFileList;
}


BOOL 
FindNextFileRemote( 
	IN CFileList* pFileList, 
	OUT LPWIN32_FIND_DATA lpFindFileData 
)
{
	HRESULT hr;
	hr = pFileList->GetRandomUnusedFile( lpFindFileData );
	if ( FAILED( hr ) )
	{
		return FALSE;
	}

	return TRUE;
}


DWORD 
GetFileRemote( 
	IN LPCSTR lpszRemoteFile, 
	HANDLE		hFile,
	IN DWORD64 dwSize
)
{
	DWORD		dwNumBytes = 0;

	CHAR szRequest[HTTP_MAX_HEADER_SIZE] = {0};

	// create the request
	UINT nRequestLength;

	nRequestLength = sprintf(szRequest, 
		"GET %s HTTP/1.1\r\n"
		"Accept: */*\r\n"
		"User-Agent: SMC\r\n"
		"Connection: Keep-Alive\r\n"
		"Host: %s\r\n"
		"\r\n",
		lpszRemoteFile,
		ML_SERVER_IP );

	// send the request over the socket
	dwNumBytes = HTTPSendRequest( sock, szRequest, nRequestLength );
	if ( SOCKET_ERROR == dwNumBytes || dwNumBytes != nRequestLength )
	{
		ML_OUTPUT( 0, DbgPrint("[MediaLoader] Error: Unable to send request over socket (error %d)\n", WSAGetLastError()) );
		return 0;
	}

	// receive the response
	dwNumBytes = HTTPGetReplyToFile( sock, hFile, (INT)dwSize );
	if ( SOCKET_ERROR == dwNumBytes || 0 == dwNumBytes )
	{
		// re-connect the socket
		HRESULT hr = ReconnectSocket();
		if ( FAILED( hr ) )
		{
			dwNumBytes = SOCKET_ERROR;
		}
	}

	return dwNumBytes;
}


HRESULT 
CopyFileRemote( 
	IN CONST PMEDIAFILEDESC pMediaFileDesc, 
	IN CONST LPWIN32_FIND_DATA lpFindFileData 
)
{
	HRESULT hr		=	S_OK;

	//------------------------------------------------------------------------------
	//	open the file for writing
	//------------------------------------------------------------------------------
	CHAR	szFullFileName[MAX_PATH];
	HANDLE	hFile			=	NULL;
	BOOL	bSuccess		=	FALSE;

	bSuccess = ProduceFullFileName( pMediaFileDesc->lpszToPath, lpFindFileData->cFileName, szFullFileName );
	if ( FALSE == bSuccess )
	{
		return E_FAIL;
	}

	// see if the file already exists and if it is older than the remote file
	HANDLE	hFindFiles;
	BOOL	bLocalFileExists		=	FALSE;
	BOOL	bRemoteFileIsNewer		=	FALSE;
	WIN32_FIND_DATA FindFileData;
	hFindFiles = FindFirstFile( szFullFileName, &FindFileData );
	if ( INVALID_HANDLE_VALUE != hFindFiles )
	{
		bLocalFileExists = TRUE;
		DWORD64	dwLocalFileTime, dwRemoteFileTime;

		memcpy( &dwLocalFileTime, &FindFileData.ftCreationTime, sizeof(DWORD64) );
		memcpy( &dwRemoteFileTime, &lpFindFileData->ftCreationTime, sizeof(DWORD64) );

		if ( dwLocalFileTime < dwRemoteFileTime )
		{
			bRemoteFileIsNewer = TRUE;
		}
	}
	FindClose( hFindFiles );

	// se if we need to even bother writing the file
	if ( MLCOPY_IFNOTEXIST & pMediaFileDesc->dwFlags && TRUE == bLocalFileExists )
	{
		ML_OUTPUT( 5, DbgPrint( "[MediaLoader] Info: '%s' already exists, skipping\n", szFullFileName ) );
		return S_OK;
	}
	else if ( MLCOPY_IFNEWER & pMediaFileDesc->dwFlags && TRUE == bLocalFileExists && FALSE == bRemoteFileIsNewer )
	{
		ML_OUTPUT( 5, DbgPrint( "[MediaLoader] Info: '%s' is up to date, skipping\n", szFullFileName ) );
		return S_OK;
	}

	hFile = CreateFile( szFullFileName, CREATE_ALWAYS );
	if ( INVALID_HANDLE_VALUE == hFile )
	{	
		DWORD dwError = GetLastError();
		ML_OUTPUT( 1, DbgPrint( "[MediaLoader] Error: CreateFile failed (error %d) on '%s'\n", dwError, szFullFileName ) );
		return E_FAIL;
	}

	//------------------------------------------------------------------------------
	//	allocate a buffer to store the file
	//------------------------------------------------------------------------------
	//LPBYTE	lpBuf			=	NULL;
	DWORD64	dwNumBytes		=	(lpFindFileData->nFileSizeHigh * ((UINT64)MAXDWORD+1)) + lpFindFileData->nFileSizeLow;
		
	//------------------------------------------------------------------------------
	//	fill in buffer from remote server
	//------------------------------------------------------------------------------
	INT nNumBytes;
	nNumBytes = GetFileRemote( lpFindFileData->cFileName, /*lpBuf*/hFile, dwNumBytes );
	if ( nNumBytes != dwNumBytes )
	{
		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: Incorrect number of bytes returned (got %d, expected %d)\n", nNumBytes, dwNumBytes ) );
		CloseHandle( hFile );
		//SAFEDELETEARRAY( lpBuf );
		return E_FAIL;
	}

	//------------------------------------------------------------------------------
	//	clean up
	//------------------------------------------------------------------------------
	CloseHandle( hFile );
	//SAFEDELETEARRAY( lpBuf );

	return hr;
}


VOID CopyFileName( IN LPSTR lpszDest, IN LPCSTR lpszSrc )
{
	TCHAR	szWideFileNameOnly[MAX_PATH];

	for ( UINT i=0; i < strlen(lpszSrc); i++ )
	{
		szWideFileNameOnly[i] = (TCHAR)lpszSrc[i];
	}
	szWideFileNameOnly[i] = (TCHAR)'\0';
	regexpr	str( szWideFileNameOnly );
	
	rpattern pat( _T("%20"), _T(" "), GLOBAL ); 

	size_t num = str.substitute( pat );

	for ( i=0; i < str.length(); i++ )
	{
		lpszDest[i] = (CHAR)str.at( i );
	}
	lpszDest[i] = '\0';
}


BOOL 
ProduceFullFileName( 
	IN LPCSTR lpszDestinationPath, 
	IN LPCSTR lpszRemoteFilePath, 
	OUT LPSTR lpszReturnedPath 
)
{
	BOOL bReturn		=	TRUE;
	LPCSTR lpszFileName =	NULL;

	//------------------------------------------------------------------------------
	//	grab only the file name (no dir info) from the remote file name
	//------------------------------------------------------------------------------

	lpszFileName = strrchr( lpszRemoteFilePath, '/' ) + 1;
	if ( NULL == lpszFileName )
	{
		ML_OUTPUT( 1, DbgPrint( "[MediaLoader] Error: Unable to parse file name from '%s'\n", lpszRemoteFilePath ) );
		return FALSE;
	}

	//------------------------------------------------------------------------------
	//	add the file name onto the end of the destination directory
	//------------------------------------------------------------------------------

	strcpy( lpszReturnedPath, lpszDestinationPath );

	UINT nLength = strlen( lpszReturnedPath ) - 1;
	if ( '\\' != lpszReturnedPath[ nLength ] )
	{
		// add the '\' to the end of the directory
		strcat( lpszReturnedPath, "\\" );
	}

	strcat( lpszReturnedPath, lpszFileName );

	// change any '%20's in the file name to spaces
	CopyFileName( lpszReturnedPath, lpszReturnedPath );

	return bReturn;
}


HANDLE 
CreateFile( 
	IN LPCSTR lpszFilePath, 
	IN DWORD dwFlags 
)
{
	CHAR	szCurrentDir[MAX_PATH];
	CHAR	szRemainingDir[MAX_PATH];
	LPSTR	lpszNextDirectory = NULL;
	BOOL	bSuccess = FALSE;

	if ( NULL == lpszFilePath || MAX_PATH < strlen( lpszFilePath ) + 1 )
	{
		ML_OUTPUT( 1, DbgPrint( "[MediaLoader] Error: Bad filepath parameter to CreateFile: '%x'\n", lpszFilePath ) );
		return INVALID_HANDLE_VALUE;
	}

	//------------------------------------------------------------------------------
	//	make sure the directory exists that we want to copy to
	//------------------------------------------------------------------------------
	strcpy( szRemainingDir, lpszFilePath );
	strcpy( szCurrentDir, strtok( szRemainingDir, "\\" ) );
	strcat( szCurrentDir, "\\" );

	// go through each subdirectory one at a time and make sure all exist
	lpszNextDirectory = strtok( NULL, "\\" );
	while( NULL != lpszNextDirectory && strcat( szCurrentDir, lpszNextDirectory ) && NULL == strchr( szCurrentDir, '.' ) )
	{
		bSuccess = CreateDirectory( szCurrentDir, NULL );
		if ( FALSE == bSuccess )
		{
			DWORD dwError = GetLastError();
			// if the function failed because the directory already exists, that is fine
			// otherwise, we have a legitimate error
			if ( ERROR_ALREADY_EXISTS != dwError)
			{
				ML_OUTPUT( 1, DbgPrint( "[MediaLoader] Error: CreateDirectory failed (error %d) on '%s'\n", dwError, szCurrentDir ) );
				return INVALID_HANDLE_VALUE;
			}
		}
		strcat( szCurrentDir, "\\" );
		lpszNextDirectory = strtok( NULL, "\\" );
	}

	//------------------------------------------------------------------------------
	//	create the file to copy to
	//------------------------------------------------------------------------------
	return CreateFile(  lpszFilePath, 
						GENERIC_READ | GENERIC_WRITE |GENERIC_ALL, 
						0, 
						NULL, 
						dwFlags, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL );
}


HRESULT
InitNetwork()
{
	HRESULT	hr			=	S_OK;

	INT		nSuccess	=	0;

	// initialize the Xbox Secure Network Library (SNL).
	if ( 0 == XNetAddRef() )
	{
		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: Unable to initialize Xbox SNL (error %d)\n", nSuccess ) );
		return E_FAIL;
	}

	//Sleep( 2000 );

	// initialize the Xbox Winsock layer 
	WORD wVersionRequested;
	WSADATA wsaData;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	nSuccess = WSAStartup( wVersionRequested, &wsaData );
	if ( 0 != nSuccess )
	{
		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: Unable to intialize Xbox winsock layer (error %d)\n", WSAGetLastError() ) );
		XNetRelease();
		return E_FAIL;
	}
	
	// open the socket
	XNADDR	xna;
	while ( XNET_GET_XNADDR_PENDING == XNetGetTitleXnAddr( &xna ) );
	sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( INVALID_SOCKET == sock )
	{
		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: Unable to open socket (error %d)\n", WSAGetLastError() ) );
		WSACleanup();
		XNetRelease();
		return E_FAIL;
	}
	
	// connect the socket
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons( ML_SERVER_PORT );
	dest.sin_addr.s_addr = inet_addr( ML_SERVER_IP );
	
	nSuccess = connect( sock, (SOCKADDR*)&dest, sizeof(SOCKADDR) );
	if( SOCKET_ERROR == nSuccess )
	{
		ML_OUTPUT( 0, DbgPrint("[MediaLoader] Error: Unable to connect socket (error %d)\n", WSAGetLastError() ) );
		closesocket( sock );
		WSACleanup();
		XNetRelease();
		return E_FAIL;
	}

	return hr;
}

HRESULT 
ReconnectSocket()
{
	HRESULT		hr	=	S_OK;

	shutdown( sock, SD_BOTH );
	closesocket( sock );

	// open the socket
	sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( INVALID_SOCKET == sock )
	{
		ML_OUTPUT( 0, DbgPrint( "[MediaLoader] Error: Unable to re-open socket (error %d)\n", WSAGetLastError() ) );
		return E_FAIL;
	}
	
	// connect the socket
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons( ML_SERVER_PORT );
	dest.sin_addr.s_addr = inet_addr( ML_SERVER_IP );
	
	INT nSuccess = connect( sock, (SOCKADDR*)&dest, sizeof(SOCKADDR) );
	if( SOCKET_ERROR == nSuccess )
	{
		ML_OUTPUT( 0, DbgPrint("[MediaLoader] Error: Unable to re-connect socket (error %d)\n", WSAGetLastError() ) );
		closesocket( sock );
		return E_FAIL;
	}

	return hr;
}


HRESULT 
DestroyNetwork()
{
	HRESULT		hr	=	S_OK;

	shutdown( sock, SD_BOTH );
	closesocket( sock );
	WSACleanup();
	XNetRelease();

	return hr;
}

#if 0
INT	_stdcall
GetIniInt( 
	IN LPCSTR	lpAppName,				// section name
	IN LPCSTR	lpKeyName,				// key name
	IN INT		nDefault,				// return value if key name not found
	IN LPCSTR	lpFileName				// initialization file name
)
{
	return GetPrivateProfileIntA( lpAppName, lpKeyName, nDefault, lpFileName );
}


FLOAT _stdcall
GetIniFloat( 
	IN LPCSTR	lpAppName,				// section name
	IN LPCSTR	lpKeyName,				// key name
	IN FLOAT	fDefault,				// return value if key name not found
	IN LPCSTR	lpFileName				// initialization file name
)
{
	CHAR szBuf[MAX_PATH];
	CHAR szDefault[MAX_PATH];

	FLOAT fVal = 0.0f;

	sprintf( szDefault, "%f", fDefault );

	GetPrivateProfileStringA( (LPCSTR)lpAppName, (LPCSTR)lpKeyName, szDefault, szBuf, MAX_PATH, (LPCSTR)lpFileName );

	fVal = (FLOAT)atof( szBuf );

	return fVal;
}

INT _stdcall
GetIniString( 
	IN LPCSTR	lpAppName,				// section name
	IN LPCSTR	lpKeyName,				// key name
	IN LPCSTR	lpDefault,				// return value if key name not found
	OUT LPSTR	lpReturnedString,
	IN DWORD	nSize,
	IN LPCSTR	lpFileName				// initialization file name
)
{
	INT nNumChars = 0;
	CHAR* pChar;

	nNumChars = GetPrivateProfileStringA( lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName );
	if ( 0 == nNumChars )
	{
		strcpy( lpReturnedString, lpDefault );
		return nNumChars;
	}

	pChar = strtok( lpReturnedString, "\n;" );
	strcpy( lpReturnedString, pChar );
	nNumChars = strlen( lpReturnedString );
	return nNumChars;
}


DWORD _stdcall
GetIniConst( 
	IN LPCSTR	lpAppName,				// section name
	IN LPCSTR	lpKeyName,				// key name
	IN DWORD	dwDefault,				// return value if key name not found
	IN LPCSTR	lpFileName				// initialization file name
)
{
	CHAR szBuf[MAX_PATH];
	CHAR* pChar;

	DWORD dwVal = 0L;
	INT	nNumChars = 0;
	BOOL bFound = FALSE;

	nNumChars = GetPrivateProfileStringA( (LPCSTR)lpAppName, (LPCSTR)lpKeyName, "", szBuf, MAX_PATH, (LPCSTR)lpFileName );
	if ( 0 == nNumChars )
	{
		return dwDefault;
	}

	pChar = strtok( szBuf, " \t\n;" );

	bFound = GetConst( pChar, MAX_PATH, &dwVal );
	if ( FALSE == bFound )
	{
		return dwDefault;
	}

	return dwVal;
}

// macros to help us define the size of our texcoords in fillrate.ini
static const DWORD T0_SIZE1 = D3DFVF_TEXCOORDSIZE1(0);
static const DWORD T0_SIZE2 = D3DFVF_TEXCOORDSIZE2(0);
static const DWORD T0_SIZE3 = D3DFVF_TEXCOORDSIZE3(0);
static const DWORD T0_SIZE4 = D3DFVF_TEXCOORDSIZE4(0);

static const DWORD T1_SIZE1 = D3DFVF_TEXCOORDSIZE1(1);
static const DWORD T1_SIZE2 = D3DFVF_TEXCOORDSIZE2(1);
static const DWORD T1_SIZE3 = D3DFVF_TEXCOORDSIZE3(1);
static const DWORD T1_SIZE4 = D3DFVF_TEXCOORDSIZE4(1);

static const DWORD T2_SIZE1 = D3DFVF_TEXCOORDSIZE1(2);
static const DWORD T2_SIZE2 = D3DFVF_TEXCOORDSIZE2(2);
static const DWORD T2_SIZE3 = D3DFVF_TEXCOORDSIZE3(2);
static const DWORD T2_SIZE4 = D3DFVF_TEXCOORDSIZE4(2);

static const DWORD T3_SIZE1 = D3DFVF_TEXCOORDSIZE1(3);
static const DWORD T3_SIZE2 = D3DFVF_TEXCOORDSIZE2(3);
static const DWORD T3_SIZE3 = D3DFVF_TEXCOORDSIZE3(3);
static const DWORD T3_SIZE4 = D3DFVF_TEXCOORDSIZE4(3);

enum { TEX_None, TEX_2d, TEX_Cubemap, TEX_Volume, TEX_Max };
enum { TIME_Present = 0x1, TIME_Render = 0x2 };

//=========================================================================
// Callback from inifile.cpp to get int string value
//=========================================================================
//#define ARRAYSIZE(_a)     (sizeof(_a) / sizeof((_a)[0]))
BOOL _stdcall
GetConst(
	IN CONST CHAR*	szStr, 
	IN INT		cchStr, 
	OUT PDWORD	pval
)
{
    #undef XTAG
    #define XTAG(_tag) { _tag, #_tag }
    static const struct
    {
        INT Val;
        CONST CHAR *szStr;
    } rgszConsts[] =
    {
        // Pixel Shaders
        XTAG(PS_TEXTUREMODES_NONE), XTAG(PS_TEXTUREMODES_PROJECT2D), XTAG(PS_TEXTUREMODES_PROJECT3D),
        XTAG(PS_TEXTUREMODES_CUBEMAP), XTAG(PS_TEXTUREMODES_PASSTHRU), XTAG(PS_TEXTUREMODES_CLIPPLANE),
        XTAG(PS_TEXTUREMODES_BUMPENVMAP), XTAG(PS_TEXTUREMODES_BUMPENVMAP_LUM), XTAG(PS_TEXTUREMODES_BRDF),
        XTAG(PS_TEXTUREMODES_DOT_ST), XTAG(PS_TEXTUREMODES_DOT_ZW), XTAG(PS_TEXTUREMODES_DOT_RFLCT_DIFF),
        XTAG(PS_TEXTUREMODES_DOT_RFLCT_SPEC), XTAG(PS_TEXTUREMODES_DOT_STR_3D), XTAG(PS_TEXTUREMODES_DOT_STR_CUBE),
        XTAG(PS_TEXTUREMODES_DPNDNT_AR), XTAG(PS_TEXTUREMODES_DPNDNT_GB), XTAG(PS_TEXTUREMODES_DOTPRODUCT),
        XTAG(PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST), XTAG(PS_COMPAREMODE_S_LT), XTAG(PS_COMPAREMODE_S_GE),
        XTAG(PS_COMPAREMODE_T_LT), XTAG(PS_COMPAREMODE_T_GE), XTAG(PS_COMPAREMODE_R_LT),
        XTAG(PS_COMPAREMODE_R_GE), XTAG(PS_COMPAREMODE_Q_LT), XTAG(PS_COMPAREMODE_Q_GE),
        XTAG(PS_COMBINERCOUNT_MUX_LSB), XTAG(PS_COMBINERCOUNT_MUX_MSB), XTAG(PS_COMBINERCOUNT_SAME_C0),
        XTAG(PS_COMBINERCOUNT_UNIQUE_C0), XTAG(PS_COMBINERCOUNT_SAME_C1), XTAG(PS_COMBINERCOUNT_UNIQUE_C1),
        XTAG(PS_INPUTMAPPING_UNSIGNED_IDENTITY), XTAG(PS_INPUTMAPPING_UNSIGNED_INVERT), XTAG(PS_INPUTMAPPING_EXPAND_NORMAL),
        XTAG(PS_INPUTMAPPING_EXPAND_NEGATE), XTAG(PS_INPUTMAPPING_HALFBIAS_NORMAL), XTAG(PS_INPUTMAPPING_HALFBIAS_NEGATE),
        XTAG(PS_INPUTMAPPING_SIGNED_IDENTITY), XTAG(PS_INPUTMAPPING_SIGNED_NEGATE), XTAG(PS_REGISTER_ZERO),
        XTAG(PS_REGISTER_DISCARD), XTAG(PS_REGISTER_C0), XTAG(PS_REGISTER_C1),
        XTAG(PS_REGISTER_FOG), XTAG(PS_REGISTER_V0), XTAG(PS_REGISTER_V1),
        XTAG(PS_REGISTER_T0), XTAG(PS_REGISTER_T1), XTAG(PS_REGISTER_T2),
        XTAG(PS_REGISTER_T3), XTAG(PS_REGISTER_R0), XTAG(PS_REGISTER_R1),
        XTAG(PS_REGISTER_V1R0_SUM), XTAG(PS_REGISTER_EF_PROD), XTAG(PS_REGISTER_ONE),
        XTAG(PS_REGISTER_NEGATIVE_ONE), XTAG(PS_REGISTER_ONE_HALF), XTAG(PS_REGISTER_NEGATIVE_ONE_HALF),
        XTAG(PS_CHANNEL_RGB), XTAG(PS_CHANNEL_BLUE), XTAG(PS_CHANNEL_ALPHA),
        XTAG(PS_FINALCOMBINERSETTING_CLAMP_SUM), XTAG(PS_FINALCOMBINERSETTING_COMPLEMENT_V1), XTAG(PS_FINALCOMBINERSETTING_COMPLEMENT_R0),
        XTAG(PS_COMBINEROUTPUT_IDENTITY), XTAG(PS_COMBINEROUTPUT_BIAS), XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_1),
        XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS), XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_2), XTAG(PS_COMBINEROUTPUT_SHIFTRIGHT_1),
        XTAG(PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA), XTAG(PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA), XTAG(PS_COMBINEROUTPUT_AB_MULTIPLY),
        XTAG(PS_COMBINEROUTPUT_AB_DOT_PRODUCT), XTAG(PS_COMBINEROUTPUT_CD_MULTIPLY), XTAG(PS_COMBINEROUTPUT_CD_DOT_PRODUCT),
        XTAG(PS_COMBINEROUTPUT_AB_CD_SUM), XTAG(PS_COMBINEROUTPUT_AB_CD_MUX),
        XTAG(PS_DOTMAPPING_ZERO_TO_ONE), XTAG(PS_DOTMAPPING_MINUS1_TO_1_D3D), XTAG(PS_DOTMAPPING_MINUS1_TO_1_GL),
        XTAG(PS_DOTMAPPING_MINUS1_TO_1), XTAG(PS_DOTMAPPING_HILO_1), XTAG(PS_DOTMAPPING_HILO_HEMISPHERE_D3D), XTAG(PS_DOTMAPPING_HILO_HEMISPHERE_GL),
        XTAG(PS_DOTMAPPING_HILO_HEMISPHERE),

        // D3DFORMATs
        XTAG(D3DFMT_A8R8G8B8), XTAG(D3DFMT_X8R8G8B8), XTAG(D3DFMT_R5G6B5), XTAG(D3DFMT_R6G5B5),
        XTAG(D3DFMT_X1R5G5B5), XTAG(D3DFMT_A1R5G5B5), XTAG(D3DFMT_A4R4G4B4), XTAG(D3DFMT_A8),
        XTAG(D3DFMT_A8B8G8R8), XTAG(D3DFMT_B8G8R8A8), XTAG(D3DFMT_R4G4B4A4), XTAG(D3DFMT_R5G5B5A1),
        XTAG(D3DFMT_R8G8B8A8), XTAG(D3DFMT_R8B8), XTAG(D3DFMT_G8B8), XTAG(D3DFMT_P8),
        XTAG(D3DFMT_L8), XTAG(D3DFMT_A8L8), XTAG(D3DFMT_AL8), XTAG(D3DFMT_L16),
        XTAG(D3DFMT_V8U8), XTAG(D3DFMT_L6V5U5), XTAG(D3DFMT_X8L8V8U8), XTAG(D3DFMT_Q8W8V8U8),
        XTAG(D3DFMT_V16U16), XTAG(D3DFMT_D16_LOCKABLE), XTAG(D3DFMT_D16), XTAG(D3DFMT_D24S8),
        XTAG(D3DFMT_F16), XTAG(D3DFMT_F24S8), XTAG(D3DFMT_UYVY), XTAG(D3DFMT_YUY2),
        XTAG(D3DFMT_DXT1), XTAG(D3DFMT_DXT2), XTAG(D3DFMT_DXT3), XTAG(D3DFMT_DXT4),
        XTAG(D3DFMT_DXT5), XTAG(D3DFMT_LIN_A1R5G5B5), XTAG(D3DFMT_LIN_A4R4G4B4), XTAG(D3DFMT_LIN_A8),
        XTAG(D3DFMT_LIN_A8B8G8R8), XTAG(D3DFMT_LIN_A8R8G8B8), XTAG(D3DFMT_LIN_B8G8R8A8), XTAG(D3DFMT_LIN_G8B8),
        XTAG(D3DFMT_LIN_R4G4B4A4), XTAG(D3DFMT_LIN_R5G5B5A1), XTAG(D3DFMT_LIN_R5G6B5), XTAG(D3DFMT_LIN_R6G5B5),
        XTAG(D3DFMT_LIN_R8B8), XTAG(D3DFMT_LIN_R8G8B8A8), XTAG(D3DFMT_LIN_X1R5G5B5), XTAG(D3DFMT_LIN_X8R8G8B8),
        XTAG(D3DFMT_LIN_A8L8), XTAG(D3DFMT_LIN_AL8), XTAG(D3DFMT_LIN_L16), XTAG(D3DFMT_LIN_L8),
        XTAG(D3DFMT_LIN_D24S8), XTAG(D3DFMT_LIN_F24S8), XTAG(D3DFMT_LIN_D16), XTAG(D3DFMT_LIN_F16),

		// D3DSWAPEFFECT
		XTAG(D3DSWAPEFFECT_DISCARD), XTAG(D3DSWAPEFFECT_FLIP), XTAG( D3DSWAPEFFECT_COPY),

		// D3DPRESENTFLAGs
		XTAG(D3DPRESENTFLAG_LOCKABLE_BACKBUFFER), XTAG(D3DPRESENTFLAG_WIDESCREEN),
		XTAG(D3DPRESENTFLAG_INTERLACED), XTAG(D3DPRESENTFLAG_PROGRESSIVE),
		XTAG(D3DPRESENTFLAG_FIELD), XTAG(D3DPRESENTFLAG_10X11PIXELASPECTRATIO),

		// D3DMULTISAMPLE_TYPEs
        XTAG(D3DMULTISAMPLE_NONE), XTAG(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX), XTAG(D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR),
        XTAG(D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR), XTAG(D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN),
        XTAG(D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT),
        XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5),
        XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8),

        XTAG(D3DCLEAR_TARGET), XTAG(D3DCLEAR_ZBUFFER), XTAG(D3DCLEAR_STENCIL),

        // FVFs
        XTAG(D3DFVF_XYZ), XTAG(D3DFVF_XYZRHW), XTAG(D3DFVF_NORMAL), XTAG(D3DFVF_DIFFUSE),
        XTAG(D3DFVF_SPECULAR), XTAG(D3DFVF_TEX0), XTAG(D3DFVF_TEX1), XTAG(D3DFVF_TEX2),
        XTAG(D3DFVF_TEX3), XTAG(D3DFVF_TEX4),

        // textcoord sizes
        XTAG(T0_SIZE1), XTAG(T0_SIZE2), XTAG(T0_SIZE3), XTAG(T0_SIZE4),
        XTAG(T1_SIZE1), XTAG(T1_SIZE2), XTAG(T1_SIZE3), XTAG(T1_SIZE4),
        XTAG(T2_SIZE1), XTAG(T2_SIZE2), XTAG(T2_SIZE3), XTAG(T2_SIZE4),
        XTAG(T3_SIZE1), XTAG(T3_SIZE2), XTAG(T3_SIZE3), XTAG(T3_SIZE4),

        // D3DCMPs
        XTAG(D3DCMP_NEVER), XTAG(D3DCMP_LESS), XTAG(D3DCMP_EQUAL), XTAG(D3DCMP_LESSEQUAL),
        XTAG(D3DCMP_GREATER), XTAG(D3DCMP_NOTEQUAL), XTAG(D3DCMP_GREATEREQUAL), XTAG(D3DCMP_ALWAYS),

		// STENCILOPs
		XTAG(D3DSTENCILOP_KEEP),
		XTAG(D3DSTENCILOP_ZERO),
		XTAG(D3DSTENCILOP_REPLACE),
		XTAG(D3DSTENCILOP_INCRSAT),
		XTAG(D3DSTENCILOP_DECRSAT),
		XTAG(D3DSTENCILOP_INVERT),
		XTAG(D3DSTENCILOP_INCR),
		XTAG(D3DSTENCILOP_DECR),

		// D3DZBUFFERTYPE
		XTAG(D3DZB_TRUE), XTAG(D3DZB_USEW), XTAG(D3DZB_FALSE),

        XTAG(D3DTEXF_NONE), XTAG(D3DTEXF_POINT), XTAG(D3DTEXF_LINEAR), XTAG(D3DTEXF_ANISOTROPIC),
        XTAG(D3DTEXF_QUINCUNX), XTAG(D3DTEXF_GAUSSIANCUBIC),

        XTAG(TEX_None), XTAG(TEX_2d), XTAG(TEX_Cubemap), XTAG(TEX_Volume),

        XTAG(TIME_Present), XTAG(TIME_Render),

        // swaths
        XTAG(D3DSWATH_8), XTAG(D3DSWATH_16), XTAG(D3DSWATH_32),
        XTAG(D3DSWATH_64), XTAG(D3DSWATH_128), XTAG(D3DSWATH_OFF),

        XTAG(D3DCOLORWRITEENABLE_RED), XTAG(D3DCOLORWRITEENABLE_GREEN), XTAG(D3DCOLORWRITEENABLE_BLUE),
        XTAG(D3DCOLORWRITEENABLE_ALPHA), XTAG(D3DCOLORWRITEENABLE_ALL),

		// File Creation Flags
		XTAG(CREATE_NEW), XTAG(CREATE_ALWAYS), XTAG(OPEN_EXISTING), XTAG(OPEN_ALWAYS), XTAG(TRUNCATE_EXISTING),

		// File Copy Flags
		XTAG(COPY_IF_NOT_EXIST), XTAG(COPY_IF_NEWER), XTAG(COPY_ALWAYS),

        // misc
        XTAG(FALSE), XTAG(TRUE)
    };

    for(int ifmt = 0; ifmt < ARRAYSIZE(rgszConsts); ifmt++)
    {
        if(!_strnicmp(rgszConsts[ifmt].szStr, szStr, cchStr))
        {
            // set val
            *pval = (DWORD)rgszConsts[ifmt].Val;
            return true;
        }
    }

    return false;
}
#endif 0



