/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	HTTPHeader.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 02-Aug-2001

Revision History:

	02-Aug-2001 jeffsul
		Initial Version

--*/

#include "HTTPHeader.h"

#include "MLInternal.h"
#include <stdio.h>

#define CHECKTOKEN( p )										\
if ( NULL == (p) )											\
{															\
	DbgPrint( "[HTTP] Error: Bad token\n" );					\
	RtlAssert( 0, __FILE__, __LINE__, NULL );				\
	return E_FAIL;											\
}

#define CHECKBYTESIZE( n )									\
if ( (n) > (UINT)nLength )										\
{															\
	DbgPrint( "[HTTP] Warning: Reply exceeds maximum buffer size\n" );	\
	RtlAssert( 0, __FILE__, __LINE__, NULL );				\
}	

INT
HTTPSendRequest(
	SOCKET	socket,              
	LPCSTR	lpBuf,  
	INT		nLength            
)
{
	INT		nReturnLength	= 0;

	nReturnLength = send( socket, lpBuf, nLength, 0 );

	return nReturnLength;
}


INT
HTTPGetReply(
	SOCKET	socket,              
	LPVOID	lpBuf,  
	INT		nLength
)
{
	INT		nNumBytes		=	0;
	DWORD	dwNumBytesCopied	=	0;

	LPBYTE	lpBody			=	NULL;

	CHAR* lpReply = new CHAR [HTTP_MAX_BUFFER_SIZE];
	if ( NULL == lpReply )
	{
		return SOCKET_ERROR;
	}
	ZeroMemory( lpReply, HTTP_MAX_BUFFER_SIZE );
	lpBody = new BYTE [HTTP_MAX_BUFFER_SIZE];
	if ( NULL == lpBody )
	{
		SAFEDELETEARRAY( lpReply );
		return SOCKET_ERROR;
	}
	ZeroMemory( lpBody, HTTP_MAX_BUFFER_SIZE );

	// receive from the socket
	HTTPHEADER	HTTPHeader;
	nNumBytes = recv( socket, lpReply, HTTP_MAX_BUFFER_SIZE, 0 );
	if ( SOCKET_ERROR == nNumBytes )
	{
		DbgPrint("[HTTP] Error: Unable to receive HTTP response (error %d)\n", WSAGetLastError());
		SAFEDELETEARRAY( lpReply );
		SAFEDELETEARRAY( lpBody );
		return SOCKET_ERROR;
	}

	// parse out the header and the main body
	HRESULT hr		=	S_OK;
	hr =  HTTPParseReply( lpReply, nNumBytes, &HTTPHeader, lpBody, &dwNumBytesCopied );
	SAFEDELETEARRAY( lpReply );
	if ( FAILED( hr ) )
	{
		DbgPrint("[HTTP] Error: Unable to parse HTTP response (error %d)\n", WSAGetLastError());
		SAFEDELETEARRAY( lpBody );
		return SOCKET_ERROR;
	}
	else if ( S_FALSE == hr )
	{
		SAFEDELETEARRAY( lpBody );
		return 0;
	}

	// see if we have a chunked html file
	if ( HTTP_TE_CHUNKED == HTTPHeader.dwTransferEncoding && HTTP_CT_TEXT_HTML == HTTPHeader.dwContentType )
	{
		DWORD	dwChunkSize	=	0;
		LPBYTE	lpszToken	=	NULL;
		INT		nTempNumBytes	=	0;
		LPBYTE	lpszCurrent	=	NULL;
		BOOL	bIsTerminal	=	FALSE;

		if ( 0 == dwNumBytesCopied )
		{
			// started with no chunk, get the first chunk
			ZeroMemory( lpBody, /*sizeof(lpBody)*/HTTP_MAX_BUFFER_SIZE );
			nTempNumBytes = recv( socket, (CHAR *)lpBody, HTTP_MAX_BUFFER_SIZE, 0 );
			if ( SOCKET_ERROR == nTempNumBytes )
			{
				DbgPrint("[HTTP] Error: Unable to receive HTTP response (error %d)\n", WSAGetLastError());
				SAFEDELETEARRAY( lpBody );
				return SOCKET_ERROR;
			}
			dwNumBytesCopied = nTempNumBytes;
		}

		// parse out the first chunk
		hr = HTTPParseChunk( lpBody, dwNumBytesCopied, lpBuf, &dwChunkSize, &bIsTerminal );
		if ( FAILED( hr ) )
		{
			DbgPrint("[HTTP] Error: Unable to parse HTTP chunk\n" );
			SAFEDELETEARRAY( lpBody );
			return SOCKET_ERROR;
		}
		dwNumBytesCopied = dwChunkSize;
		lpszCurrent = (LPBYTE)lpBuf + dwChunkSize;

		// get the remaining chunks
		while ( FALSE == bIsTerminal )
		{
			// receive another chunk
			ZeroMemory( lpBody, /*sizeof(lpBody)*/HTTP_MAX_BUFFER_SIZE );
			nTempNumBytes = recv( socket, (CHAR *)lpBody, HTTP_MAX_BUFFER_SIZE, 0 );
			if ( 0 == nTempNumBytes )
			{
				DbgPrint("[HTTP] Error: Expected more bytes from server (HTML file may be too large)\n" );
				SAFEDELETEARRAY( lpBody );
				return SOCKET_ERROR;
			}
			if ( SOCKET_ERROR == nTempNumBytes )
			{
				DbgPrint("[HTTP] Error: Unable to receive HTTP response (error %d)\n", WSAGetLastError());
				SAFEDELETEARRAY( lpBody );
				return SOCKET_ERROR;
			}
			
			// parse out the chunk data
			hr = HTTPParseChunk( lpBody, nTempNumBytes, lpszCurrent, &dwChunkSize, &bIsTerminal );
			if ( FAILED( hr ) )
			{
				DbgPrint("[HTTP] Error: Unable to parse HTTP chunk\n" );
				SAFEDELETEARRAY( lpBody );
				return SOCKET_ERROR;
			}
			dwNumBytesCopied += dwChunkSize;
			lpszCurrent = (LPBYTE)lpBuf + dwNumBytesCopied;
		}

		// copy all the data to the destination buffer
		CHECKBYTESIZE( dwNumBytesCopied );
		memcpy( lpBuf, lpszCurrent - dwNumBytesCopied, dwNumBytesCopied );
	}

	else //if ( HTTP_TE_CHUNKED == HTTPHeader.dwTransferEncoding ) // this is a chunked data file
	{
		memcpy( lpBuf, lpBody, dwNumBytesCopied );

		while ( HTTPHeader.dwBytesLeft > 0 )
		{
			INT		nTempNumBytes	=	0;
			
			// get another chunk
			nTempNumBytes = recv( socket, (CHAR *)lpBuf+dwNumBytesCopied, HTTPHeader.dwBytesLeft, 0 );
			if ( SOCKET_ERROR == nNumBytes )
			{
				DbgPrint("[HTTP] Error: Unable to receive HTTP response (error %d)\n", WSAGetLastError());
				SAFEDELETEARRAY( lpBody );
				return SOCKET_ERROR;
			}
			
			dwNumBytesCopied += nTempNumBytes;
			CHECKBYTESIZE( dwNumBytesCopied );
			HTTPHeader.dwBytesLeft -= nTempNumBytes;
		}
	}

	SAFEDELETEARRAY( lpBody );
	return dwNumBytesCopied;
}

INT
HTTPGetReplyToFile(
	SOCKET	socket,
	HANDLE	hFile,
	INT		nLength
)
{
	INT		nNumBytes		=	0;

	CHAR* lpReply = new CHAR [HTTP_MAX_BUFFER_SIZE];
	if ( NULL == lpReply )
	{
		return SOCKET_ERROR;
	}
	ZeroMemory( lpReply, HTTP_MAX_BUFFER_SIZE );

	LPBYTE	lpBody = new BYTE [HTTP_MAX_BUFFER_SIZE];
	if ( NULL == lpBody )
	{
		SAFEDELETEARRAY( lpReply );
		return SOCKET_ERROR;
	}
	ZeroMemory( lpBody, HTTP_MAX_BUFFER_SIZE );

	DWORD	dwNumBytesCopied	=	0;
	BOOL	bSuccess			=	FALSE;
	DWORD	dwNumBytesWritten	=	0;

	LPBYTE	lpChunk = new BYTE [ HTTP_MAX_BUFFER_SIZE ];
	if ( NULL == lpChunk )
	{
		SAFEDELETEARRAY( lpReply );
		SAFEDELETEARRAY( lpBody );
		return 0;
	}

	// receive from the socket
	HTTPHEADER	HTTPHeader;
	nNumBytes = recv( socket, lpReply, HTTP_MAX_BUFFER_SIZE, 0 );
	if ( SOCKET_ERROR == nNumBytes )
	{
		DbgPrint("[HTTP] Error: Unable to receive HTTP response (error %d)\n", WSAGetLastError());
		SAFEDELETEARRAY( lpReply );
		SAFEDELETEARRAY( lpBody );
		delete [] lpChunk;
		return SOCKET_ERROR;
	}

	// parse out the header and the main body
	HRESULT hr		=	S_OK;
	hr =  HTTPParseReply( lpReply, nNumBytes, &HTTPHeader, lpBody, &dwNumBytesCopied );
	SAFEDELETEARRAY( lpReply );
	if ( FAILED( hr ) )
	{
		DbgPrint("[HTTP] Error: Unable to parse HTTP response (error %d)\n", WSAGetLastError());
		SAFEDELETEARRAY( lpBody );
		delete [] lpChunk;
		return SOCKET_ERROR;
	}
	else if ( S_FALSE == hr )
	{
		SAFEDELETEARRAY( lpBody );
		delete [] lpChunk;
		return 0;
	}

	// see if we have a chunked html file
	if ( HTTP_TE_CHUNKED == HTTPHeader.dwTransferEncoding && HTTP_CT_TEXT_HTML == HTTPHeader.dwContentType )
	{
		DWORD	dwChunkSize	=	0;
		LPBYTE	lpszToken	=	NULL;
		INT		nTempNumBytes	=	0;
		LPBYTE	lpszCurrent	=	NULL;
		BOOL	bIsTerminal	=	FALSE;

		if ( 0 == dwNumBytesCopied )
		{
			// started with no chunk, get the first chunk
			ZeroMemory( lpBody, /*sizeof(lpBody)*/HTTP_MAX_BUFFER_SIZE );
			nTempNumBytes = recv( socket, (CHAR *)lpBody, HTTP_MAX_BUFFER_SIZE, 0 );
			if ( SOCKET_ERROR == nTempNumBytes )
			{
				DbgPrint("[HTTP] Error: Unable to receive HTTP response (error %d)\n", WSAGetLastError());
				SAFEDELETEARRAY( lpBody );
				delete [] lpChunk;
				return SOCKET_ERROR;
			}
			dwNumBytesCopied = nTempNumBytes;
		}

		// parse out the first chunk
		hr = HTTPParseChunk( lpBody, dwNumBytesCopied, lpChunk, &dwChunkSize, &bIsTerminal );
		if ( FAILED( hr ) )
		{
			DbgPrint("[HTTP] Error: Unable to parse HTTP chunk\n" );
			SAFEDELETEARRAY( lpBody );
			delete [] lpChunk;
			return SOCKET_ERROR;
		}
		dwNumBytesCopied = dwChunkSize;

		//------------------------------------------------------------------------------
		//	write buffer to file
		//------------------------------------------------------------------------------
		bSuccess = WriteFile( hFile, lpChunk, dwChunkSize, &dwNumBytesWritten, NULL );
		if ( FALSE == bSuccess || dwChunkSize != dwNumBytesWritten )
		{
			/*ML_OUTPUT( 1,*/ DbgPrint( "[MediaLoader] Error: WriteFile failed (error %d)\n", GetLastError() /*)*/ );
			SAFEDELETEARRAY( lpBody );
			delete [] lpChunk;
			return 0;
		}

		// get the remaining chunks
		while ( FALSE == bIsTerminal )
		{
			// receive another chunk
			ZeroMemory( lpBody, /*sizeof(lpBody)*/HTTP_MAX_BUFFER_SIZE );
			nTempNumBytes = recv( socket, (CHAR *)lpBody, HTTP_MAX_BUFFER_SIZE, 0 );
			if ( 0 == nTempNumBytes )
			{
				DbgPrint("[HTTP] Error: Expected more bytes from server (HTML file may be too large)\n" );
				SAFEDELETEARRAY( lpBody );
				delete [] lpChunk;
				return SOCKET_ERROR;
			}
			if ( SOCKET_ERROR == nTempNumBytes )
			{
				DbgPrint("[HTTP] Error: Unable to receive HTTP response (error %d)\n", WSAGetLastError());
				SAFEDELETEARRAY( lpBody );
				delete [] lpChunk;
				return SOCKET_ERROR;
			}
			
			// parse out the chunk data
			hr = HTTPParseChunk( lpBody, nTempNumBytes, lpChunk, &dwChunkSize, &bIsTerminal );
			if ( FAILED( hr ) )
			{
				DbgPrint("[HTTP] Error: Unable to parse HTTP chunk\n" );
				SAFEDELETEARRAY( lpBody );
				delete [] lpChunk;
				return SOCKET_ERROR;
			}
			dwNumBytesCopied += dwChunkSize;

			//------------------------------------------------------------------------------
			//	write buffer to file
			//------------------------------------------------------------------------------
			bSuccess = WriteFile( hFile, lpChunk, dwChunkSize, &dwNumBytesWritten, NULL );
			if ( FALSE == bSuccess || dwChunkSize != dwNumBytesWritten )
			{
				/*ML_OUTPUT( 1,*/ DbgPrint( "[MediaLoader] Error: WriteFile failed (error %d)\n", GetLastError() /*)*/ );
				SAFEDELETEARRAY( lpBody );
				delete [] lpChunk;
				return 0;
			}
		}

		// copy all the data to the destination buffer
		CHECKBYTESIZE( dwNumBytesCopied );
		//memcpy( lpBuf, lpszCurrent - dwNumBytesCopied, dwNumBytesCopied );
	}

	else // this is not a chunked data file
	{
		//memcpy( lpBuf, lpBody, dwNumBytesCopied );
		bSuccess = WriteFile( hFile, lpBody, dwNumBytesCopied, &dwNumBytesWritten, NULL );
		if ( FALSE == bSuccess || dwNumBytesCopied != dwNumBytesWritten )
		{
			/*ML_OUTPUT( 1,*/ DbgPrint( "[MediaLoader] Error: WriteFile failed (error %d)\n", GetLastError() ) /*)*/;
			SAFEDELETEARRAY( lpBody );
			delete [] lpChunk;
			return 0;
		}

		while ( HTTPHeader.dwBytesLeft > 0 )
		{
			INT		nTempNumBytes	=	0;
			
			// get another chunk
			nTempNumBytes = recv( socket, (CHAR *)lpBody, HTTP_MAX_BUFFER_SIZE, 0 );
			if ( SOCKET_ERROR == nTempNumBytes || 0 == nTempNumBytes )
			{
				DbgPrint("[HTTP] Error: Unable to receive HTTP response (error %d)\n", WSAGetLastError());
				SAFEDELETEARRAY( lpBody );
				delete [] lpChunk;
				return SOCKET_ERROR;
			}

			bSuccess = WriteFile( hFile, lpBody, nTempNumBytes, &dwNumBytesWritten, NULL );
			if ( FALSE == bSuccess || nTempNumBytes != dwNumBytesWritten )
			{
				/*ML_OUTPUT( 1,*/ DbgPrint( "[MediaLoader] Error: WriteFile failed (error %d)\n", GetLastError() ) /*)*/;
				SAFEDELETEARRAY( lpBody );
				delete [] lpChunk;
				return 0;
			}
			
			dwNumBytesCopied += nTempNumBytes;
			CHECKBYTESIZE( dwNumBytesCopied );
			HTTPHeader.dwBytesLeft -= nTempNumBytes;
		}
	}

	SAFEDELETEARRAY( lpBody );
	delete [] lpChunk;
	return dwNumBytesCopied;
}

HRESULT 
HTTPParseReply( 
	IN LPSTR		lpBuf, 
	IN DWORD		dwSize, 
	OUT PHTTPHEADER pHTTPHeader, 
	OUT LPVOID		lpBody,
	OUT	LPDWORD		lpdwSizeCopied
)
{
	HRESULT hr		=	S_OK;

	LPSTR	lpszToken	=	NULL;
	CHAR	szHeader[HTTP_MAX_HEADER_SIZE];

	ZeroMemory( pHTTPHeader, sizeof(HTTPHEADER) );

	// find the end of the header
	lpszToken = strstr( (CHAR *)lpBuf, "\r\n\r\n" );
	if ( NULL == lpszToken )
	{
		return E_FAIL;
	}
	lpszToken += 4;

	// calculate length of header and main body
	INT		nHeaderLength = lpszToken-lpBuf;
	*lpdwSizeCopied	 =	dwSize-nHeaderLength;

	// copy the header to a usable buffer
	memcpy( szHeader, lpBuf, nHeaderLength );
	szHeader[nHeaderLength+1] = '\0';


	if ( 0 < *lpdwSizeCopied )
	{
		// copy the main body into the destination buffer
		memcpy( lpBody, lpszToken, *lpdwSizeCopied );
	}

	// get the HTTP version, status code, and status message from the first line
	INT nSuccess = 0;
	lpszToken = strtok( szHeader, " /" );
	CHECKTOKEN( lpszToken );
	lpszToken = strtok( NULL, " \t" );
	CHECKTOKEN( lpszToken );
	pHTTPHeader->fVersion = (FLOAT)atof( lpszToken );
	lpszToken = strtok( NULL, " \t" );
	CHECKTOKEN( lpszToken );
	pHTTPHeader->nStatusCode = atoi( lpszToken );
	lpszToken = strtok( NULL, "\t\r\n" );
	CHECKTOKEN( lpszToken );
	strcpy( pHTTPHeader->szStatusMessage, lpszToken );

	// check the status code to make sure we got a 200 level status (OK)
	if ( pHTTPHeader->nStatusCode < 200 || pHTTPHeader->nStatusCode > 299 )
	{
		// if we have any error other than 404 (not found) print out an error
		if ( 404 != pHTTPHeader->nStatusCode )
		{
			DbgPrint( "[HTTP] Warning: Transfer error (error %d: %s)\n", pHTTPHeader->nStatusCode, pHTTPHeader->szStatusMessage );
		}
		return S_FALSE;
	}

	// parse out each of the data fields from the header
	lpszToken = strtok( NULL, " :\t\r\n" );
	while ( NULL != lpszToken )
	{
		if ( 0 == strcmp( lpszToken, "Server" ) )
		{
			lpszToken = strtok( NULL, "\r\n" );
			if ( NULL != lpszToken )
			{
				strcpy( pHTTPHeader->szServer, lpszToken );
			}
		}
		else if ( 0 == strcmp( lpszToken, "Date" ) )
		{
			lpszToken = strtok( NULL, "\r\n" );
			if ( NULL != lpszToken )
			{
				strcpy( pHTTPHeader->szDate, lpszToken );
			}
		}
		else if ( 0 == strcmp( lpszToken, "Transfer-Encoding" ) )
		{
			lpszToken = strtok( NULL, "\r\n" );
			if ( NULL != lpszToken )
			{
				if ( 0 == strcmp( lpszToken, " chunked" ) )
				{
					pHTTPHeader->dwTransferEncoding = HTTP_TE_CHUNKED;
				}
				else
				{
					pHTTPHeader->dwTransferEncoding = HTTP_TE_NORMAL;
				}
			}
		}
		else if ( 0 == strcmp( lpszToken, "Content-Type" ) )
		{
			lpszToken = strtok( NULL, "\r\n" );
			if ( NULL != lpszToken )
			{
				if ( 0 == strcmp( lpszToken, " text/html" ) )
				{
					pHTTPHeader->dwContentType = HTTP_CT_TEXT_HTML;
				}
				else if ( 0 == strcmp( lpszToken, " audio/wav" ) )
				{
					pHTTPHeader->dwContentType = HTTP_CT_AUDIO_WAV;
				}
				else
				{
					pHTTPHeader->dwContentType = FTTP_CT_UNDEFINED;
				}
			}
		}
		else if ( 0 == strcmp( lpszToken, "Content-Length" ) )
		{
			lpszToken = strtok( NULL, "\r\n" );
			if ( NULL != lpszToken )
			{
				pHTTPHeader->dwContentLength = atoi( lpszToken );
				pHTTPHeader->dwBytesLeft = pHTTPHeader->dwContentLength - *lpdwSizeCopied;
			}
		}

		else
		{
			// we don't care about this partivular data field,
			// skip over its data and move on
			lpszToken = strtok( NULL, "\r\n" );
		}

		// go to the next data field
		lpszToken = strtok( NULL, " :\t\r\n" );
	}

	return hr;
}



HRESULT
HTTPParseChunk(
	IN LPBYTE		lpChunk,
	IN DWORD		dwSize,
	OUT	LPVOID		lpBody,
	OUT	LPDWORD		lpdwSizeCopied,
	OUT	PBOOL		pbIsLastChunk
)
{
	HRESULT hr	=	S_OK;

	LPBYTE	lpszChunkSize, lpszNextChunkSize, lpBeginBody;
	INT		nContentLength, nNextChunkSize;

	lpszNextChunkSize = (LPBYTE)strstr( (CHAR *)lpChunk+dwSize-10, "\r\n" );
	if ( NULL == lpszNextChunkSize )
	{
		// no \r\n was found at the end of the chunk,
		// so we must have at least one more chunk and
		// the end of the actual data is the end of the chunk
		pbIsLastChunk = FALSE;
		lpszNextChunkSize = lpChunk + dwSize;
	}
	else
	{
		// calculate the size of the actual data
		INT		nNumFields;
		nNumFields = sscanf( (CHAR *)lpszNextChunkSize, "\r\n%x\r\n", &nNextChunkSize );

		// see if this is the last chunk
		if ( 0 == nNextChunkSize )
		{
			*pbIsLastChunk = TRUE;
		}
		else
		{
			*pbIsLastChunk = FALSE;
		}
	}

	lpszChunkSize = (LPBYTE)strstr( (CHAR *)lpChunk, "\r\n" );
	if ( NULL == lpszChunkSize || '0' == lpszChunkSize[2] || lpszChunkSize-lpChunk > 10 )
	{
		// if there was no \r\n or the only \r\n was at the end of the chunk,
		// then the actual data starts at the beginning of the chunk
		lpBeginBody = lpChunk;
		//*lpdwSizeCopied = lpszNextChunkSize - lpChunk;

	}
	else
	{
		// skip over the hex number to the actual data
		INT		nNumFields;
		nNumFields = sscanf( (CHAR *)lpszChunkSize + 2, "%x", &nContentLength );
		//lpBeginBody = lpszChunkSize + strstr( lpszChunkSize + 2;
		lpBeginBody = (LPBYTE)strstr( (CHAR *)lpszChunkSize, "\r\n" ) + 2;
	}

	if ( NULL == lpBeginBody )
	{
		DbgPrint( "[HTTP] Error: Unable to parse chunk correctly\n" );
		return E_FAIL;
	}

	*lpdwSizeCopied = lpszNextChunkSize - lpBeginBody;
	
	if ( 0 < *lpdwSizeCopied )
	{
		// copy the actual data to the destination buffer
		memcpy( lpBody, lpBeginBody, *lpdwSizeCopied );
	}
	

	return hr;
}