/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	HTTPHeader.h

Abstract:

	Abstracts internals of client HTTP & TCP/IP send/recv calls
	to simply HTTPSendRequest and HTTPGetReply

Author:

	Jeff Sullivan (jeffsul) 02-Aug-2001

Revision History:

	02-Aug-2001 jeffsul
		Initial Version

--*/

#ifndef __HTTPHEADER_H__
#define __HTTPHEADER_H__

#include <xtl.h>

//------------------------------------------------------------------------------
//	Public:
//------------------------------------------------------------------------------

/*++

Routine Description:

	Sends an HTTP request over the specified socket

Arguments:

	see below

Return Value:

	the number of bytes sent if ok
	SOCKET_ERROR if failure

--*/
extern INT
HTTPSendRequest(
	SOCKET	socket,			//	the socket to send the request on   
	LPCSTR	lpBuf,			//	the actual request buffer
	INT		nLength			//	the length of the request buffer
);


/*++

Routine Description:

	Gets an HTTP reply from the specifed socket, in the process
	the function strips out all HTTP information and 
	gets only the actual data of the response

Arguments:

	see below

Return Value:

	the number of bytes in the actual data if ok
	SOCKET_ERROR if failure

--*/
extern INT
HTTPGetReply(
	SOCKET	socket,			//	the socket to receive the reply on
	LPVOID	lpBuf,			//	the buffer to store the actual reply
	INT		nLength			//	the max size of the reply buffer
);


extern INT
HTTPGetReplyToFile(
	SOCKET	socket,			//	the socket to receive the reply on
	HANDLE	hFile,			//	a handle to the file to save the reply
	INT		nLength			//	the max size of the file
);


//------------------------------------------------------------------------------
//	Internal:
//------------------------------------------------------------------------------

#define HTTP_MAX_HEADER_SIZE	1000
#define HTTP_MAX_BUFFER_SIZE	5000
#define	HTTP_MAX_HTML_SIZE		500000

// Transfer Encodings:
#define HTTP_TE_NORMAL		0x00000000
#define	HTTP_TE_CHUNKED		0x00000001

// Content Types:
#define HTTP_CT_TEXT_HTML	0x00000000
#define HTTP_CT_AUDIO_WAV	0x00000001
#define HTTP_CT_IMAGE		0x00000002
#define FTTP_CT_UNDEFINED	0xffffffff

typedef struct _HTTPHEADER				//	a structure to hold the information in an http transfer header
{
	FLOAT	fVersion;					//	http version number
	INT		nStatusCode;				//	http status code ( 200 = ok, 404 = not found )
	CHAR	szStatusMessage[MAX_PATH];	//	message relating to status code
	CHAR	szServer[MAX_PATH];			//	the name of the server sending the information
	CHAR	szDate[MAX_PATH];			//	date of the transfer
	DWORD	dwTransferEncoding;			//	transfer encoding ( HTTP_TE_NORMAL, HTTP_TE_CHUNKED, etc )
	DWORD	dwContentType;				//	text/html, audio/wav, etc
	DWORD	dwContentLength;			//	length of the transfer ( not including the header )
	DWORD	dwBytesLeft;				//	bytes left in the transmission
} HTTPHEADER, *PHTTPHEADER;

extern HRESULT							//	separates an http reply into the header and body
HTTPParseReply( 
	IN LPSTR		lpBuf,				//	the buffer received from the server
	IN DWORD		dwSize,				//	the size of the received buffer
	OUT PHTTPHEADER pHTTPHeader,		//	a pointer to a structure to fill in with http header info
	OUT LPVOID		lpBody,				//	a pointer to a buffer to hold the body of the transfer
	OUT	LPDWORD		lpdwSizeCopied		//	a pointer to a DWORD that will hold the number of bytes copied to lpBody
);

extern HRESULT							//	separates an http chunk into any info tags and the main body
HTTPParseChunk(
	IN LPBYTE		lpChunk,			//	the chunk received from the server
	IN DWORD		dwSize,				//	the size of the chunk received
	OUT	LPVOID		lpBody,				//	a pointer to a buffer to hold the body of the chunk
	OUT	LPDWORD		lpdwSizeCopied,		//	a pointer to a DWORD that will hold the number of bytes copied to lpBody
	OUT	PBOOL		pbIsLastChunk		//	a pointer to a BOOL that will be TRUE if this is the last chunk of a transfer
);

/*#define SAFEDELETEARRAY( p )			\
{										\
	if ( NULL != (p) )					\
	{									\
		delete [] (p);					\
		(p) = NULL;						\
	}									\
}*/
	
#endif // #ifndef __HTTPHEADER_H__
