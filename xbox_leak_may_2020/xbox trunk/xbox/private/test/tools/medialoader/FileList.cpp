/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	FileList.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 01-Aug-2001

Revision History:

	01-Aug-2001 jeffsul
		Initial Version

--*/

#include "FileList.h"

#include "MLInternal.h"
#include "regexpr.h"
using namespace regex;

CFileList::CFileList()
{
	m_pFirstFile	=	NULL;
	m_nNumFiles		=	0;
}


CFileList::~CFileList()
{
	PFILELISTNODE	pNode;

	pNode = m_pFirstFile;

	while ( NULL != pNode )
	{
		PFILELISTNODE	pNextNode;
		pNextNode	=	pNode->pNext;
		SAFEDELETE( pNode );
		pNode = pNextNode;
		m_nNumFiles--;
	}
}


HRESULT CFileList::AddFile( IN CONST LPWIN32_FIND_DATA lpFindFileData )
{
	HRESULT	hr		=	S_OK;

	// set up new node
	PFILELISTNODE	pNewNode = new FILELISTNODE;
	
	CHECKALLOC( pNewNode );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	pNewNode->bUsed		=	FALSE;
	memcpy( &pNewNode->FindData, lpFindFileData, sizeof(WIN32_FIND_DATA) );

	// insert node into chain
	pNewNode->pNext = m_pFirstFile;
	m_pFirstFile = pNewNode;

	m_nNumFiles++;
	return hr;
}


HRESULT CFileList::RemoveFile( IN LPCSTR lpszFileName )
{
	HRESULT	hr		=	S_OK;

	PFILELISTNODE	pNode = m_pFirstFile;
	PFILELISTNODE	pPrevNode = NULL;


	while ( NULL != pNode )
	{
		INT	nCompare;
		nCompare = strcmp( pNode->FindData.cFileName, lpszFileName );
		if ( 0 == nCompare )
		{
			if ( NULL == pPrevNode )
			{
				m_pFirstFile = pNode->pNext;
			}
			else
			{
				pPrevNode->pNext = pNode->pNext;
			}

			SAFEDELETE( pNode );
			m_nNumFiles--;
			return hr;
		}
	}

	return S_FALSE;
}

HRESULT CFileList::GetRandomUnusedFile( OUT LPWIN32_FIND_DATA lpFindFileData )
{
	HRESULT	hr			=	S_OK;

	PFILELISTNODE pNode	=	m_pFirstFile;

	if ( 0 == m_nNumFiles )
	{
		return E_FAIL;
	}

	srand( timeGetTime() );
	UINT	nFileIndex	=	rand() % m_nNumFiles;

	for ( UINT i=0; i < nFileIndex; i++ )
	{
		pNode = pNode->pNext;
		ASSERT( NULL != pNode );
	}

	while ( NULL != pNode && TRUE == pNode->bUsed )
	{
		pNode = pNode->pNext;
	}

	if ( NULL == pNode )
	{
		// no unused nodes after the randomly selected one, start from beginning
		return GetFirstUnusedFile( lpFindFileData );
	}

	memcpy( lpFindFileData, &pNode->FindData, sizeof(WIN32_FIND_DATA) );
	pNode->bUsed = TRUE;
	return hr;
}


HRESULT CFileList::GetFirstUnusedFile( OUT LPWIN32_FIND_DATA lpFindFileData )
{
	HRESULT	hr			=	S_OK;

	PFILELISTNODE pNode	=	m_pFirstFile;

	while ( NULL != pNode && TRUE == pNode->bUsed )
	{
		pNode = pNode->pNext;
	}

	if ( NULL == pNode )
	{
		// no unused nodes
		return E_FAIL;
	}

	memcpy( lpFindFileData, &pNode->FindData, sizeof(WIN32_FIND_DATA) );
	pNode->bUsed = TRUE;
	return hr;
}

INT	CFileList::GetNumFiles()
{
	return m_nNumFiles;
}


#define CHECKTOKEN( p )											\
if ( NULL == (p) )												\
{																\
	DbgPrint( "[MediaLoader] Error: Bad token\n" );				\
	SAFEDELETEARRAY( lpszBody );								\
	RtlAssert( 0, __FILE__, __LINE__, NULL );					\
	return E_FAIL;												\
}
HRESULT CFileList::ParseHTML( IN LPCSTR lpszBuf, IN DWORD dwSize, IN LPCSTR lpszSearchString )
{
	HRESULT	hr			=	S_OK;
	LPSTR	lpszFiles	=	NULL;

	LPSTR	lpszBody = new CHAR[dwSize];
	CHECKALLOC( lpszBody );
	if ( FAILED( hr ) )
	{
		return E_FAIL;
	}
	memcpy( lpszBody, lpszBuf, dwSize );

	// move down to first file
	lpszFiles = strstr( lpszBody, "<br><br>" );
	CHECKTOKEN( lpszFiles );
	lpszFiles += 8;

    if ( '/' == lpszFiles[1] )
    {
        // this is an empty directory
		SAFEDELETEARRAY( lpszBody );
        return hr;
    }

	INT i= -1;

	// create each find data structure and add it to the list
	while ( NULL != lpszFiles )
	{
		i++;
		LPCSTR	lpszDay;
		LPCSTR	lpszMonth;
		LPCSTR	lpszDate;
		LPCSTR	lpszYear;
		LPCSTR	lpszHour;
		LPCSTR	lpszMin;
		//LPCSTR	lpszTime;
		LPCSTR	lpszAmPm;
		LPCSTR	lpszSize;
		LPCSTR	lpszName;

		// get the file's date
		lpszDay = strtok( lpszFiles, " \t," );
		CHECKTOKEN( lpszDay );
		lpszMonth = strtok( NULL, " \t," );
		CHECKTOKEN( lpszMonth );
		lpszDate = strtok( NULL, " \t," );
		CHECKTOKEN( lpszDate );
		lpszYear = strtok( NULL, " \t," );
		CHECKTOKEN( lpszYear );
		//lpszTime = strtok( NULL, " \t," );
		//CHECKTOKEN( lpszTime );
		lpszHour = strtok( NULL, " :\t," );
		CHECKTOKEN( lpszHour );
		lpszMin = strtok( NULL, " \t," );
		CHECKTOKEN( lpszMin );
		lpszAmPm = strtok( NULL, " \t," );
		CHECKTOKEN( lpszAmPm );

		INT		nYear, nMonth, nDate, nHour, nMin;
		nYear = atoi( lpszYear );
		if ( 0 == strcmp( lpszMonth, "January" ) )
		{
			nMonth = 1;
		}
		else if ( 0 == strcmp( lpszMonth, "February" ) )
		{
			nMonth = 2;
		}
		else if ( 0 == strcmp( lpszMonth, "March" ) )
		{
			nMonth = 3;
		}
		else if ( 0 == strcmp( lpszMonth, "April" ) )
		{
			nMonth = 4;
		}
		else if ( 0 == strcmp( lpszMonth, "May" ) )
		{
			nMonth = 5;
		}
		else if ( 0 == strcmp( lpszMonth, "June" ) )
		{
			nMonth = 6;
		}
		else if ( 0 == strcmp( lpszMonth, "July" ) )
		{
			nMonth = 7;
		}
		else if ( 0 == strcmp( lpszMonth, "August" ) )
		{
			nMonth = 8;
		}
		else if ( 0 == strcmp( lpszMonth, "September" ) )
		{
			nMonth = 9;
		}
		else if ( 0 == strcmp( lpszMonth, "October" ) )
		{
			nMonth = 10;
		}
		else if ( 0 == strcmp( lpszMonth, "November" ) )
		{
			nMonth = 11;
		}
		else
		{
			nMonth = 12;
		}

		nDate = atoi( lpszDate );
		nHour = atoi( lpszHour );
		nMin = atoi( lpszMin );

		SYSTEMTIME time;
		//tm	time;
		time.wHour = (WORD)nHour;
		if ( 0 == strcmp( lpszAmPm, "PM" ) )
		{
			time.wHour += 12;
		}
		time.wMinute = (WORD)nMin;
		time.wMonth = (WORD)nMonth;
		time.wSecond = 0;
		time.wMilliseconds = 0;
		time.wDay = (WORD)nDate;
		time.wYear = (WORD)nYear;


		// get the file's size
		lpszSize = strtok( NULL, " \t," );
		CHECKTOKEN( lpszSize );

		// get the file's name
		lpszFiles = strstr( lpszSize+strlen(lpszSize)+1, "\"" );
		CHECKTOKEN( lpszFiles );
		lpszFiles += 1;
		lpszName = strtok( lpszFiles, " \"" );
		CHECKTOKEN( lpszName );

        UINT	k, j;
	    UINT	nLength = strlen( lpszSearchString );
        CHAR    szStringNoSpaces[ MAX_PATH ] = {0};
	    j = 0;
	    for ( k=0; k <= nLength; k++ )
	    {
		    if ( ' ' == lpszSearchString[k] )
		    {
			    szStringNoSpaces[j] = '%';
		    	szStringNoSpaces[j+1] = '2';
			    szStringNoSpaces[j+2] = '0';
			    j += 3;
		    }
		    else
		    {
		    	szStringNoSpaces[j] = lpszSearchString[k];
		    	j++;
		    }

		    if ( j >= MAX_PATH )
		    {
		    	DbgPrint("[MediaLoader] Error: Too many characters in filename %s.\n", lpszSearchString );
		    }
	    }

		BOOL	bMatches = FileNameMatches( szStringNoSpaces, lpszName );

		if ( bMatches )
		{
			// if the file name matches the search string
			// add the file to the list
			WIN32_FIND_DATA FindData;
			ZeroMemory( &FindData, sizeof(WIN32_FIND_DATA) );

			strcpy( FindData.cFileName, lpszName );
			FindData.nFileSizeLow = atoi( lpszSize );
			SystemTimeToFileTime( &time, &FindData.ftCreationTime );
			if ( 0 != FindData.nFileSizeLow )
			{
				// not a directory, or a zero length file,
				// which we wouldn't want anyway, right?
				EXECUTE( AddFile( &FindData ) );
				if ( FAILED( hr ) )
				{
					SAFEDELETEARRAY( lpszBody );
					return hr;
				}
			}
		}

		// move the pointer to the next file
		lpszFiles = strstr( lpszName+strlen(lpszName)+1, "</A><br>" );
		if ( NULL == lpszFiles )
		{
			break;
		}
		lpszFiles += 8;
		if ( lpszFiles == strstr( lpszFiles, "</pre><hr></body>" ) )
		{
			break;
		}

	}

	// do any cleaning up
	SAFEDELETEARRAY( lpszBody );

	return hr;
}


BOOL CFileList::FileNameMatches( IN LPCSTR lpszSearchString, IN LPCSTR lpszFileName )
{
	BOOL	bResult		=	FALSE;

	LPCSTR lpszFileNameOnly = strrchr( lpszFileName, '/' );
	if ( NULL == lpszFileNameOnly )
	{
		return bResult;
	}
	lpszFileNameOnly += 1;

	WCHAR	szWideFileNameOnly[MAX_PATH];
	WCHAR	szWideSearchString[MAX_PATH];

	for ( UINT i=0; i < strlen(lpszFileNameOnly) + 1; i++ )
	{
		szWideFileNameOnly[i] = (WCHAR)tolower( lpszFileNameOnly[i] );
	}
	regexpr	str( szWideFileNameOnly );

	UINT	j	= 0;
	for ( i=0; i < strlen(lpszSearchString) + 1; i++ )
	{
		if ( ' ' == lpszSearchString[i] )
		{
			lstrcat( szWideSearchString, L"%20" );
			j += 2;
			//lstrcpy( szWideSearchString + j = (TCHAR)'%'
		}
		else
		{
			szWideSearchString[j] = (WCHAR)tolower( lpszSearchString[i] );
		}
		j++;
	}
	rpattern pat( szWideSearchString );
	
	regexpr::backref_type br = str.match( pat );
	
	LPCTSTR lpszFirstMatch = &br.first[0];
	
	if ( br && szWideFileNameOnly == _tcsstr( szWideFileNameOnly, lpszFirstMatch ) )
	{
		bResult = TRUE;
	}

	return bResult;
}
