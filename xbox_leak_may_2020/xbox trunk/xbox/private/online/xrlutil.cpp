/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing common functionality for asynchronous XRL

Module Name:

    xrlutil.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Request strings - all are ANSI. HTTP headers are always ANSI. These 
// strings are used for request packets
//
const char XRL_HEADER_GET_AND_AGENT[] =	"GET %s HTTP/1.1\r\nUser-Agent: %x/1.0\r\n";
const char XRL_HEADER_POST_AND_AGENT[] =	"POST %s HTTP/1.0\r\nUser-Agent: %x/1.0\r\n";
const char XRL_HEADER_HOST[] =				"Host: %d.%d.%d.%d\r\n";
const char XRL_HEADER_CONTENT_LENGTH[] =	"Content-Length: %I64u\r\n";
const char XRL_HEADER_IFRANGE[] =			"If-Range: %s\r\n";
const char XRL_HEADER_RANGE[] =			"Range: bytes=%s-\r\n";
const char XRL_HEADER_AUTHORIZATION[] =	"Authorization: %s %s=\"%d.%d\",%s=\"%s\",%s=\"%s\"\r\n";
const char XRL_HEADER_RETURN[] =			"\r\n";
const char XRL_HEADER_CONTENTLENGTH[] =    "Content-Length:";
const char XRL_HEADER_DATE[] =             "Date:";
const char XRL_HEADER_LASTMODIFIED[] =     "Last-Modified:";
const char XRL_HEADER_HTTP[] =             "HTTP";
const char XRL_HEADER_XERR[] =             "X-Err:";

// Assume 100GB limit, nice for alignment too.
#define MAX_FILESIZE_SIZE			12

#define	MAX_TIME_SIZE				40

const char * const g_rgszDays[] = 
{
	"Sun", "Mon", "Tue", "Wed", 
	"Thu", "Fri", "Sat" 
};

const char * const g_rgszMonths[] = 
{
	"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
};

HRESULT CXo::XRL_FileTimeAsString(LPSTR pBuffer, DWORD * pcbBuffer, PFILETIME pft)
{
    // We'll use RFC 1123 (actually a subset)
    // Sun, 06 Nov 1994 08:49:37 GMT
    //
    SYSTEMTIME  st;

    // check the buffer size
    if (MAX_TIME_SIZE > *pcbBuffer)
        return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));


    // BUGBUG (michaelw) We should probably make sure the following succeeds but it sucks for callers
    // to have to check our return.  Consider using an Assert
    FileTimeToSystemTime(pft, &st);

    // assemble the final string
    *pcbBuffer = sprintf(pBuffer, "%s, %02d %s %4d %02d:%02d:%02d GMT", 
    			g_rgszDays[st.wDayOfWeek], st.wDay, g_rgszMonths[st.wMonth], 
    			st.wYear, st.wHour, st.wMinute, st.wSecond);
    pBuffer[*pcbBuffer] = '\0';

    return(S_OK);
}

BOOL CXo::XRL_FileTimeFromString(LPCSTR sz, PFILETIME pft)
{
    // Assumes RFC 1123
    // Sun, 06 Nov 1994 08:49:37 GMT
    // 01234567890123456789012345678
    //           1         2

    char    timebuf[30];
    WORD    i;
    SYSTEMTIME st;

    // A quick format check
    if (strlen(sz) != 29)
    return FALSE;

    // Make a copy so we can modify it
    strcpy(timebuf, sz);

    if (timebuf[3] != ',' || timebuf[4] != ' ' 
            || timebuf[11] != ' ' || timebuf[16] != ' ' 
            || timebuf[19] != ':' || timebuf[22] != ':' 
            || timebuf[25] != ' ' || timebuf[26] != 'G' 
            || timebuf[27] != 'M' || timebuf[28] != 'T')
        return FALSE;

    // Setup string terminators for atoi
    timebuf[7] = '\0';
    timebuf[16] = '\0';
    timebuf[19] = '\0';
    timebuf[22] = '\0';
    timebuf[25] = '\0';


    // Get the day of the week
    ZeroMemory(&st, sizeof(st));
    for ( i = 0 ; i < 7 ; i++)
    {
        if (memcmp(g_rgszDays[i], timebuf, 3) == 0)
        break;
    }

    if (i == 7)
        return FALSE;

    st.wDayOfWeek = i;

    // Get the day of the month
    st.wDay = (WORD)atol(timebuf + 5);

    if (st.wDay == 0)
        return FALSE;

    // Get the month
    for (i = 1 ; i < 13 ; i++)
    {
        if (memcmp(g_rgszMonths[i], timebuf + 8, 3) == 0)
            break;
    }

    if (i == 13)
        return FALSE;

    st.wMonth = i;

    // Get the year
    st.wYear = (WORD)atol(timebuf + 12);

    // Get the hours, minutes, seconds
    st.wHour = (WORD)atol(timebuf + 17);
    st.wMinute = (WORD)atol(timebuf + 20);
    st.wSecond = (WORD)atol(timebuf + 23);

    return (SystemTimeToFileTime(&st, pft) == 1);
}

//
// Helper function to insert authentication headers into an HTTP
// request packet.
//
#if 0
HRESULT XRL_InsertAuthHeaders(
			XRL					xrlRelative,
			DWORD				cbRelative,
			char				**ppBuffer, 
			PXAPPSERVICEINFO	pServiceInfo
			)
{
	BOOL    fRetVal;
	DWORD   dwEncodedTicketLen;
	BYTE    encryptedAuthenticator[XCRYPT_HEADER_SIZE+sizeof(XAUTHENTICATOR)];
	DWORD   cbEncryptedAuthenticator;
	BYTE    encodedAuthenticator[BASE64_ENCODE_BUFFER_SIZE(sizeof(encryptedAuthenticator))];
	BYTE    encodedTicket[BASE64_ENCODE_BUFFER_SIZE(X_MAX_TICKET_SIZE)];
	XAUTHENTICATOR authenticator;
	XCRYPT_STATE_BUFFER xcryptState;
	XC_ONLINE_INFO_STRUCT* pOnlineInfo = XonlineGetInfo();

	// base 64 encode the ticket
	fRetVal = Base64Encode((BYTE *)pServiceInfo->ticket, 
							pServiceInfo->dwTicketLen, encodedTicket);
	Assert(fRetVal);

	// This value was originally u.tDelta
	// Add the current Xbox time to get the corrected UTC time
	// of the authentication server.
	pServiceInfo->u.tTimestamp += time(NULL);

	// Use session key for MAC and encryption
	XcryptInitialize(pServiceInfo->serviceInfo.sessionKey, X_KEY_LEN, &xcryptState);

	// Fill in the authenticator
	authenticator.qwXboxID = pOnlineInfo->XboxID;
	XcryptComputeMAC(&xcryptState, xrlRelative, cbRelative, authenticator.pURLMAC);
	authenticator.tAuthTime = pServiceInfo->u.tTimestamp;

	// encrypt the authenticator
	XcryptEncrypt(&xcryptState, (BYTE *)&authenticator, sizeof(authenticator),
					encryptedAuthenticator, &cbEncryptedAuthenticator );
    Assert(cbEncryptedAuthenticator == sizeof(encryptedAuthenticator));

	// base64 encode the encrypted authenticator
	fRetVal = Base64Encode((BYTE *)&encryptedAuthenticator, cbEncryptedAuthenticator, 
					encodedAuthenticator);
	Assert(fRetVal);

	// Add authentication stuff to the HTTP header
	*ppBuffer += sprintf(*ppBuffer, XRL_HEADER_AUTHORIZATION, 
				X_AUTH_SCHEME, X_VERSION_TOKEN, 
				XONLINE_PROTOCOL_VERSION_MAJOR, XONLINE_PROTOCOL_VERSION_MINOR,
				X_TICKET_TOKEN, encodedTicket,
				X_AUTH_TOKEN, encodedAuthenticator);
	return(S_OK);				
}
#endif

//
// Helper function to build an HTTP GET request header, optionally
// with resume semantics using the If-Range header.
//
HRESULT CXo::XRL_BuildGetRequestWithResume(
			DWORD				dwServiceID,
			LPCSTR				szResourcePath,
			DWORD				dwIPAddress,
			char				*pBuffer, 
			DWORD				*pcbBuffer, 
			PBYTE				pbExtraHeaders,
			DWORD				cbExtraHeaders,
			FILETIME			*pModifiedSince, 
			LARGE_INTEGER		liResumeFrom
			)
{
    HRESULT hr = S_OK;
    DWORD	cbRelative;
    DWORD   cbBuffNeeded;
    DWORD   cbTime;
	DWORD   cbEncodedTicket;
    char    chTimeGMT[40];
    char	chResumeFrom[MAX_FILESIZE_SIZE];
    char	*pBufEnd;
    PBYTE	pbIP;
    
    Assert(NULL != pBuffer);
    Assert(NULL != pcbBuffer);
    Assert(NULL != szResourcePath);
    
    // start by doing the GMT time conversion if requested
    if ( pModifiedSince != NULL )
    {
        cbTime = sizeof(chTimeGMT);
        XRL_FileTimeAsString(chTimeGMT, &cbTime, pModifiedSince);
    }

    // Calculate the amount of buffer we will need to hold the request.
    // We might ask for a little more than we really need, but that's OK
    // Return with an error if the supplied buffer is too small
    cbRelative = strlen(szResourcePath);
    cbBuffNeeded = sizeof(XRL_HEADER_GET_AND_AGENT) + 5;
    cbBuffNeeded += cbRelative;

    // HTTP/1.1 requires the Host header
    cbBuffNeeded += sizeof(XRL_HEADER_HOST) + 3;

	// Account for any extra headers
	if (pbExtraHeaders)
		cbBuffNeeded += cbExtraHeaders;

#if 0    
    // Add length of authentication related stuff if needed
    if (pServiceInfo->u.dwRequiresAuthentication)
    {
        cbEncodedTicket = BASE64_ENCODE_BUFFER_SIZE(pServiceInfo->dwTicketLen);
        cbBuffNeeded += sizeof(XRL_HEADER_AUTHORIZATION) - 1;
        cbBuffNeeded += cbEncodedTicket;
        cbBuffNeeded += BASE64_ENCODE_BUFFER_SIZE(XCRYPT_HEADER_SIZE+sizeof(XAUTHENTICATOR));
        cbBuffNeeded += 10; // strlen(X_TICKET_TOKEN) + strlen(X_AUTH_TOKEN)
    }
#endif    
    
    // Use the If-Range headers to resume if a last modification time was
    // supplied
    if ( pModifiedSince != NULL )
    {
        cbBuffNeeded += sizeof(XRL_HEADER_IFRANGE) - 1;
        cbBuffNeeded += cbTime;
       	cbBuffNeeded += sizeof(XRL_HEADER_RANGE) - 1;
       	cbBuffNeeded += sprintf(chResumeFrom, "%I64u", liResumeFrom);
    }
    cbBuffNeeded += sizeof(XRL_HEADER_RETURN);

    // do any math to account for authentication headers here

    // check if there is enough buffer space
    if (*pcbBuffer < cbBuffNeeded)
    {
        // set the buffer needed into the response
        *pcbBuffer = cbBuffNeeded;
        return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
    }

    // build the send request
    pBufEnd = pBuffer;
    pBufEnd += sprintf(pBufEnd, XRL_HEADER_GET_AND_AGENT, szResourcePath, dwServiceID);

	// Build the required Host: header, as required by HTTP/1.1.
    pbIP = (PBYTE)&dwIPAddress;
    pBufEnd += sprintf(pBufEnd, XRL_HEADER_HOST, pbIP[0], pbIP[1], pbIP[2], pbIP[3]);

    // if the modified header was requested, then add it too
    if (pModifiedSince != NULL)
    {        
        pBufEnd += sprintf(pBufEnd, XRL_HEADER_IFRANGE, chTimeGMT);
        pBufEnd += sprintf(pBufEnd, XRL_HEADER_RANGE, chResumeFrom);
    }

#if 0
    // if authentication is required, then add it to header
    if (pServiceInfo->u.dwRequiresAuthentication)
    {
    	XRL_InsertAuthHeaders(xrlRelative, cbRelative, &pBufEnd, pServiceInfo);
    }
#endif    

	// Add in any extra headers here
	if (pbExtraHeaders)
	{
		MoveMemory(pBufEnd, pbExtraHeaders, cbExtraHeaders);
		pBufEnd += cbExtraHeaders;
	}

    // close the request
    pBufEnd += sprintf(pBufEnd, XRL_HEADER_RETURN);

    Assert((DWORD)(pBufEnd - pBuffer) <= cbBuffNeeded);
    
    // set the final actual size
    *pcbBuffer = pBufEnd - pBuffer;

    return(S_OK);
}

//
// Helper function to build an HTTP POST request header.
//
HRESULT CXo::XRL_BuildPostRequest(
			DWORD			dwServiceID,
			LPCSTR			szTargetPath,
			DWORD			dwIPAddress,
			char			*pBuffer, 
			DWORD			*pcbBuffer, 
			PBYTE			pbExtraHeaders,
			DWORD			cbExtraHeaders,
			ULARGE_INTEGER	uliFileSize			
			)
{
    HRESULT hr = S_OK;
    DWORD   cbRelative;
    DWORD   cbBuffNeeded;
	DWORD   cbEncodedTicket;
    char	*pBufEnd;
    
    Assert(NULL != pBuffer);
    Assert(NULL != pcbBuffer);
    Assert(NULL != szTargetPath);
    
    // Calculate the amount of buffer we will need to hold the request.
    // We might ask for a little more than we really need, but that's OK
    // Return with an error if the supplied buffer is too small
    cbRelative = strlen(szTargetPath);
    cbBuffNeeded = sizeof(XRL_HEADER_POST_AND_AGENT) + 5;
    cbBuffNeeded += cbRelative;

    // HTTP/1.1 requires the Host header
    //cbBuffNeeded += sizeof(XRL_HEADER_HOST) + 3;

	// Account for any extra headers
	if (pbExtraHeaders)
		cbBuffNeeded += cbExtraHeaders;

#if 0    
    // Add length of authentication related stuff if needed
    if (pServiceInfo->u.dwRequiresAuthentication)
    {
        cbEncodedTicket = BASE64_ENCODE_BUFFER_SIZE(pServiceInfo->dwTicketLen);
        cbBuffNeeded += sizeof(XRL_HEADER_AUTHORIZATION) - 1;
        cbBuffNeeded += cbEncodedTicket;
        cbBuffNeeded += BASE64_ENCODE_BUFFER_SIZE(XCRYPT_HEADER_SIZE+sizeof(XAUTHENTICATOR));
        cbBuffNeeded += 10; // strlen(X_TICKET_TOKEN) + strlen(X_AUTH_TOKEN)
    }
#endif    
    
    cbBuffNeeded += sizeof(XRL_HEADER_CONTENT_LENGTH) + MAX_FILESIZE_SIZE;
    cbBuffNeeded += sizeof(XRL_HEADER_RETURN);

    // do any math to account for authentication headers here

    // check if there is enough buffer space
    if (*pcbBuffer < cbBuffNeeded)
    {
        // set the buffer needed into the response
        *pcbBuffer = cbBuffNeeded;
        return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
    }

    // build the send request
    pBufEnd = pBuffer;
    pBufEnd += sprintf(pBufEnd, XRL_HEADER_POST_AND_AGENT, szTargetPath, dwServiceID);

	// Build the required Host: header, as required by HTTP/1.1.
    //pbIP = (PBYTE)&dwIPAddress;
    //pBufEnd += sprintf(pBufEnd, XRL_HEADER_HOST, pbIP[0], pbIP[1], pbIP[2], pbIP[3]);

#if 0
    // if authentication is required, then add it to header
    if (pServiceInfo->u.dwRequiresAuthentication)
    {
    	XRL_InsertAuthHeaders(xrlRelative, cbRelative, &pBufEnd, pServiceInfo);
    }
#endif    

	// Fill in the content length header
    pBufEnd += sprintf(pBufEnd, XRL_HEADER_CONTENT_LENGTH, 
    			uliFileSize.QuadPart);

	// Add in any extra headers here
	if (pbExtraHeaders)
	{
		MoveMemory(pBufEnd, pbExtraHeaders, cbExtraHeaders);
		pBufEnd += cbExtraHeaders;
	}

    // Close the request
    pBufEnd += sprintf(pBufEnd, XRL_HEADER_RETURN);

    Assert((DWORD)(pBufEnd - pBuffer) <= cbBuffNeeded);
    
    // set the final actual size
    *pcbBuffer = pBufEnd - pBuffer;

    return(S_OK);
}

HRESULT    CXo::XRL_LookupServiceIPAddress(
			PXONLINE_SERVICE_INFO	pService,
			SOCKADDR_IN 			*psockaddr
			)
{
    Assert(NULL != pService);
    Assert(NULL != psockaddr);

    // copy over the found address into the socket address structure
    ZeroMemory(psockaddr, sizeof(SOCKADDR_IN));

    // we must set the internet flag
    psockaddr->sin_family = AF_INET;

    // bring in the port number from the previous lookup
    psockaddr->sin_port = htons(pService->wServicePort);

    // bring over the IP address from the previous lookup
    psockaddr->sin_addr.S_un.S_addr = pService->serviceIP.S_un.S_addr;

    return(S_OK);
}

//
// Helper function to skip the host portion of an XRL string
// and return the beginning of the URI portion. This call 
// assumes that the first element in the XRL is a host address.
// The host address is treated as a string and is not further 
// processed to determine what kind of address it is.
//
// Note: cbXRL and *pcbURI do NOT include any NULL terminator,
// and pbXRL does NOT have to be NULL-terminated.
//
HRESULT CXo::XRL_SkipToURI(
			PBYTE		pbXRL,
			DWORD		cbXRL,
			PBYTE		*ppbURI,
			DWORD		*pcbURI
			)
{
	*ppbURI = NULL;
	*pcbURI = 0;

	while (cbXRL--)
	{
		if (*pbXRL == '/')
		{
			*ppbURI = pbXRL;
			*pcbURI = cbXRL + 1;
			return(S_OK);
		}
		pbXRL++;
	}
	return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
}

//
// Helper function to convert a host string into a 4-byte
// IP address. The following heuristics are used:
//
// 1) The string must have exactly four components separated
//    by periods ('.').
// 2) Each component must only consist of numerals (0-9), with
//    no spaces in between.
// 3) Each component must enumerate to a value between 0-255.
//
// Note: cbHost does NOT include any NULL terminators, and 
// pbHost does NOT have to be NULL-terminated.
//
HRESULT CXo::XRL_ConvertToIPAddressAndPort(
			PBYTE		pbHost,
			DWORD		cbHost,
			DWORD		*pdwIPAddress,
			WORD		*pwPort
			)
{
	DWORD	dwComponent = 0;
	DWORD	dwValue = 0;
	DWORD	dwIP = 0;
	DWORD	dwPort = 0;
	BOOL	fEmpty = TRUE;
	BOOL	fHasPort = FALSE;

	*pdwIPAddress = 0;
	*pwPort = 80;
	
	while (cbHost--)
	{
		// 1) Max 4 components (0-3)
		if (dwComponent > 3)
			goto Error;

		// 2) Must be numeric
		if ((*pbHost >= '0') && (*pbHost <= '9'))
		{
			dwValue *= 10;
			dwValue += (*pbHost - '0');

			// 3) Must not exceed 255
			if (dwValue > 255)
				goto Error;

			fEmpty = FALSE;
		}
		else if (*pbHost == '.')
		{
			// Put it into the IP Address using 32-bit rotate
			Assert(dwValue <= 255);
			dwIP |= dwValue;
			dwIP = _rotr(dwIP, 8);
			dwValue = 0;
			dwComponent++;
			fEmpty = TRUE;
		}
		else if (*pbHost == ':')
		{
			fHasPort = TRUE;
			break;
		}
		else
			goto Error;

		pbHost++;
	}

	// Finish the last component
	if ((dwComponent == 3) && !fEmpty)
	{
		// Put it into the IP Address using 32-bit rotate
		Assert(dwValue <= 255);
		dwIP |= dwValue;
		dwIP = _rotr(dwIP, 8);

		// Return the IP
		*pdwIPAddress = dwIP;

		// Now see if we have a port number as well
		if (fHasPort && cbHost && (*pbHost == ':'))
		{
			pbHost++;
			while (cbHost--)
			{
				if ((*pbHost < '0') || (*pbHost > '9'))
					goto Error;
				dwPort *= 10;
				dwPort += (*pbHost - '0');
				if (dwPort > 0xffff)
					goto Error;

				pbHost++;
			}
			
			// Return the port
			*pwPort = (WORD)(dwPort & 0xffff);
		}
		
		return(S_OK);
	}
	
Error:
	return(HRESULT_FROM_WIN32(ERROR_BAD_NETPATH));
}

//
// Helper function to look up the XRL service address, and create
// a socket connection
//
HRESULT CXo::XRL_AsyncConnect(PXRL_ASYNC pxrlasync)
{
    HRESULT     		hr = S_OK;
    SOCKET				socketTemp = INVALID_SOCKET;
    PXONLINETASK_SOCKIO	psockio = &(pxrlasync->sockio);
  
    int				serr;
    SOCKADDR_IN		sockaddr;
    LINGER			l_linger;

    // Get the XRL's target IP address
    hr = XRL_LookupServiceIPAddress(&pxrlasync->serviceInfo, &sockaddr);
    if (FAILED(hr))
        goto Error;

    // Open up the socket to the target address. This socket uses TCP over IP
    socketTemp = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socketTemp == INVALID_SOCKET)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

	// If the requested connection is insecure, then we will set the 
	// insecure status of the socket.
	if (pxrlasync->dwFlags & XRL_CONNECTION_INSECURE)
	{
		XnSetInsecure(socketTemp);
	}

	// We immiediately hand this socket over to the socket I/O context. This
	// way, when we free the parent XRL_ASYNC structure, we will also free 
	// this socket
    StreamConnectInitializeContext(socketTemp, 
    			pxrlasync->xontask.hEventWorkAvailable, psockio);

    // Initiate the connection asynchronously
    hr = StreamConnectBegin(&sockaddr, psockio);
    if (FAILED(hr))
    	goto Error;

Cleanup:
	return(hr);

Error:
	goto Cleanup;
}			

//
// This function parses one header starting from the specified
/// starting position in the receive buffer.
// 
// Arguments (pxrlasync->):
//
// pStart - pointer to beginning of header string to parse
// dwHTTPResponseCode - receives the HTTP response
// uliContentLenght - receives the content length
// ftLastModified - receives the tast modified time
// ftResponse - receives the server response timestamp
//
// Return Values:
//
// This function either returns TRUE if there are no more headers
// to parse, or FALSE to indicate more headers.
//
BOOL CXo::XRL_ParseOneHeader(LPSTR pStart, PXRL_ASYNC pxrlasync)
{
    DWORD		dwEnd = 0;
    char*		pEnd;
    BOOL		fRetVal;
    ULONGLONG	ull;

#if 0    
    DWORD   dwDecodedSauthLen;
    XCRYPT_STATE_BUFFER xcryptState;
    BYTE    decryptedSauth[sizeof(time_t)];
    DWORD   cbDecryptedSauth;
#endif    
    
    Assert(NULL != pStart);
    Assert(NULL != pxrlasync);

    // scan forward over any whitespace - this means NO HEADER FOLDING
    while ( (*pStart == ' ') || (*pStart == '\t') )
        pStart++;

    // test if this is the last header
    if ( pStart[0] == '\0' || (pStart[0] == '\r' && pStart[1] == '\n') )
        return TRUE;

    // test for the header string
    if ( _strnicmp(pStart, XRL_HEADER_CONTENTLENGTH, sizeof(XRL_HEADER_CONTENTLENGTH)-1) == 0)
    {
        // found the header. Move to the start of the data
        pStart += (sizeof(XRL_HEADER_CONTENTLENGTH)-1);

        // scan forward over any whitespace - this means NO HEADER FOLDING
        while ( (*pStart == ' ') || (*pStart == '\t') )
            pStart++;

        // get the data
        pxrlasync->uliContentLength.QuadPart = _atoi64(pStart);

        // we are done. return false because this is not the end header.
        return FALSE;
    }

    // test for the header string - remember the sizeof includes the null
    if ( _strnicmp(pStart, XRL_HEADER_LASTMODIFIED, sizeof(XRL_HEADER_LASTMODIFIED)-1) == 0 )
    {
        // found the header. Move to the start of the data
        pStart += (sizeof(XRL_HEADER_LASTMODIFIED)-1);

        // scan forward over any whitespace - this means NO HEADER FOLDING
        while ( (*pStart == ' ') || (*pStart == '\t') )
            pStart++;

        // get the string as a FILETIME value
        XRL_FileTimeFromString(pStart, &(pxrlasync->ftLastModified));

        // we are done. return false because this is not the end header.
        return FALSE;
    }

    // test for the header string
    if ( _strnicmp(pStart, XRL_HEADER_DATE, sizeof(XRL_HEADER_DATE)-1) == 0 )
    {
        // found the header. Move to the start of the data
        pStart += (sizeof(XRL_HEADER_DATE)-1);

        // scan forward over any whitespace - this means NO HEADER FOLDING
        while ( (*pStart == ' ') || (*pStart == '\t') )
            pStart++;

        // get the string as a FILETIME value
        XRL_FileTimeFromString(pStart, &(pxrlasync->ftResponse));

        // we are done. return false because this is not the end header.
        return FALSE;
    }

#if 0
    // check if this is the authentication response.
    if ( _strnicmp(pStart, XRL_HEADER_AUTHEN_INFO, sizeof(XRL_HEADER_AUTHEN_INFO)-1) == 0 )
    {
        // found the authentication response. Move to the start of the data
        pStart += (sizeof(XRL_HEADER_DATE)-1);

        // scan forward to find start of sauth
        pStart = strstr(pStart, XRL_HEADER_SAUTH_START);
        if (pStart == NULL)
        {
            return FALSE;
        }
        pStart += (sizeof(XRL_HEADER_SAUTH_START)-1);
        
        // scan forward to find end of sauth
        pEnd = strchr(pStart, '\"');
        if (pEnd == NULL)
        {
            return FALSE;
        }

        // base64decode the sauth
        fRetVal = Base64Decode( pStart, pEnd - pStart, pStart, &dwDecodedSauthLen );
        if (!fRetVal)
        {
            return FALSE;
        }

        // Check sauth length
        if ( dwDecodedSauthLen != XCRYPT_HEADER_SIZE + sizeof(decryptedSauth) )
        {
            return FALSE;
        }

        // decrypt with the session key
        XcryptInitialize( pServiceInfo->serviceInfo.sessionKey, X_KEY_LEN, &xcryptState );
        
        // decrypt sauth and check if sauth looks decent
        if ( !XcryptDecrypt( &xcryptState,
                pStart, dwDecodedSauthLen,
                decryptedSauth, &cbDecryptedSauth ) )
        {
            return FALSE;
        }
        Assert( cbDecryptedSauth == sizeof(decryptedSauth) );

        // Check if timestamp matches what we sent
        if ( *((time_t*)decryptedSauth) == pServiceInfo->u.tTimestamp )
        {
            // Response sauth is good, remember that.
            // We now have mutual authentication and will trust this server.
            pServiceInfo->u.dwServerNotTrusted = FALSE;
        }
        
        // we are done. return false because this is not the end header.
        return FALSE;
    }
#endif    

    // check if this is the X-Err code
    if ( _strnicmp(pStart, XRL_HEADER_XERR, sizeof(XRL_HEADER_XERR)-1) == 0 )
    {
        // This is the optional X-Err code. Move to the start of the data
        // the code starts after the first space character
        while ( (*pStart != ' ') && (*pStart !='\0') )
            pStart++;

        // scan forward over any whitespace - this means NO HEADER FOLDING
        while ( (*pStart == ' ') || (*pStart == '\t') )
            pStart++;

        // get the X-Err: code
        HexStringToUlonglong(pStart, strlen(pStart), &ull);
        pxrlasync->hrXErr = (DWORD)ull;

        // we are done. return false because this is not the end header.
        return FALSE;
    }

    // check if this is the main response code.
    // check this last. If the others are found, then code is short-circuited.
    if ( _strnicmp(pStart, XRL_HEADER_HTTP, sizeof(XRL_HEADER_HTTP)-1) == 0 )
    {
        // found the main response code. Move to the start of the data
        // the code starts after the first space character
        while ( (*pStart != ' ') && (*pStart !='\0') )
            pStart++;

        // scan forward over any whitespace - this means NO HEADER FOLDING
        while ( (*pStart == ' ') || (*pStart == '\t') )
            pStart++;

        // turn the second space into a null too
        dwEnd = 0;
        while ( (pStart[dwEnd] != ' ') && (pStart[dwEnd] != '\0') )
            dwEnd++;
        pStart[dwEnd] = '\0';

        // get the response code
        pxrlasync->dwExtendedStatus = atoi(pStart);

        // Set a default code if the HTTP code is any error
        if (XONLINEDOWNLOAD_EXSTATUS_ANYERROR(pxrlasync->dwExtendedStatus))
        	pxrlasync->hrXErr = HRESULT_FROM_WIN32(ERROR_SERVICE_SPECIFIC_ERROR);

        // we are done. return false because this is not the end header.
        return FALSE;
    }

    // this is not the last header
    return FALSE;
}

//
// This function processes the data in the current receive buffer
// and either parses its headers (if in header mode), or dumps
// the HTTP content to the buffer. During the transition from header
// mode to content mode, this function returns the HTTP response code
// 
// Arguments (pxrlasync->):
//
// pBuffer -  buffer to containg the data to process
// dwCurrent - The number of received bytes in the buffer
// uliContentLength - receives the expected CONTENT length.
// uliTotal - The cumulative length of CONTENT received (excl. headers)
// dwNewBytesRead - amount of new data read
//
// Return Values:
//
// This function either returns S_OK if the data was processed
// successfully. E_FAIL is returned if a hard error had occurred.
//
HRESULT CXo::XRL_ProcessHttp(PXRL_ASYNC pxrlasync, DWORD dwNewBytesRead, BOOL * pfDoneHeaders)
{
    HRESULT	hr = S_OK;

	*pfDoneHeaders = FALSE;
	
	// Process if there is actual data read
	if (IS_HEADER_MODE(pxrlasync))
	{
		PBYTE				pbData = pxrlasync->pBuffer;
		PBYTE				pbStart = pbData;
		DWORD				dwBytesLeft;
		
		// Iterate through the data
		dwBytesLeft = pxrlasync->dwCurrent + dwNewBytesRead;
		while ((dwBytesLeft > 1) && IS_HEADER_MODE(pxrlasync))
		{
			if ((pbData[0] == '\r') && (pbData[1] == '\n'))
			{
				// Found a line break here, terminate this string
				// by directly modifying the buffer.
				*pbData++ = '\0';
				*pbData++ = '\0';
				dwBytesLeft -= 2;

				// Pares this line, and see if we are done headers
				// Note: If the response is coming from an ISAPI extension,
				// IIS 5 might return a response that does not contain a trailing
				// empty line (i.e. CRLF). We need to handle this case.
				if (XRL_ParseOneHeader((LPSTR)pbStart, pxrlasync) || !dwBytesLeft)
				{
					// Done processing headers
					pxrlasync->dwTemp &= (~XRL_FLAG_HEADER_MODE);
					*pfDoneHeaders = TRUE;
#if 0
					// Make sure we trust this server
	                if (pxrlasync->serviceinfo.u.dwServerNotTrusted)
	                {
	                    hr = HRESULT_FROM_WIN32(ERROR_TRUST_FAILURE);
	                    goto Error;
	                }
#endif	                

					// Whatever remains becomes content
					CopyMemory(pxrlasync->pBuffer, pbData, dwBytesLeft);
					pxrlasync->dwCurrent = dwBytesLeft;

					break;
				}

				// Setup for next header line
				pbStart = pbData;
			}
			else
			{
				pbData++;
				dwBytesLeft--;
			}
			
		}	// while (dwBytesLeft > 1 && IS_HEADER_MODE(pxrlasync))

		if (IS_HEADER_MODE(pxrlasync))
		{
			// Any leftover data would become part of the next header
			pxrlasync->dwCurrent = (pbData - pbStart) + dwBytesLeft;
			MoveMemory(pxrlasync->pBuffer, pbStart, pxrlasync->dwCurrent);
		}
		
	}	// if (IS_HEADER_MODE(pxrlasync))

#if 0
Cleanup:
#endif

	// Return the result code in xontask->hr
	return(hr);

#if 0
Error:
	hrReturn = E_FAIL;
	goto Cleanup;
#endif	
}

//
// Generic function to create an event if hEventWorkAvailable is not
// supplied by the caller.
//
// This function also optionally returns the event handle just in case 
// some other component needs to share this handle. If an event is 
// already specified, an event is not created, and the original handle
// is returned as output.
//
HRESULT CXo::XRL_CreateWorkEventIfNecessary(PXRL_ASYNC pxrlasync, HANDLE * pHandle)
{
	if (!pxrlasync->xontask.hEventWorkAvailable)
	{
		pxrlasync->xontask.hEventWorkAvailable = 
					CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!pxrlasync->xontask.hEventWorkAvailable)
		{
			return(HRESULT_FROM_WIN32(GetLastError()));
		}

		pxrlasync->dwFlags |= XRL_HEVENT_CREATED_BY_API;
	}

	if (pHandle)
		*pHandle = pxrlasync->xontask.hEventWorkAvailable;
		
	return(S_OK);
}

//
// Generic function for cleanup
//
void CXo::XRL_CleanupHandler(PXRL_ASYNC pxrlasync)
{
	PXRL_EXTENSION	pxrlext;

	// Close the socket
	if ((pxrlasync->sockio.socket != INVALID_SOCKET) &&
		(pxrlasync->sockio.socket != 0))
	{
		closesocket(pxrlasync->sockio.socket);
		pxrlasync->sockio.socket = INVALID_SOCKET;
	}

	// Close any open files
	if ((pxrlasync->fileio.hFile != INVALID_HANDLE_VALUE) &&
		(pxrlasync->fileio.hFile != NULL))
	{
		CloseHandle(pxrlasync->fileio.hFile);
		pxrlasync->fileio.hFile = INVALID_HANDLE_VALUE;
	}

	// Propagate cleanup to extensions ...
	pxrlext = &pxrlasync->xrlext;
	if (pxrlasync->fDownload)
	{
		// Cleanup download tasks
		if (pxrlext->dl.pfnCleanup)
        {
			(this->*(pxrlext->dl.pfnCleanup))(pxrlasync);
        }
	}
	else
	{
		// Cleanup upload tasks
		if (pxrlext->ul.pfnCleanup)
        {
			(this->*(pxrlext->ul.pfnCleanup))(pxrlasync);
        }
	}

	// Free the event handle if we allocated one
	if (pxrlasync->dwFlags & XRL_HEVENT_CREATED_BY_API)
	{
		CloseHandle(pxrlasync->xontask.hEventWorkAvailable);
		pxrlasync->xontask.hEventWorkAvailable = NULL;
		pxrlasync->dwFlags &= (~XRL_HEVENT_CREATED_BY_API);
	}
}

//
// Function to determine if the amount of disk space currently
// available is sufficient to finish the download. If the specified
// file already exists, this also accounts for the disk space
// currenlty used by that file.
//
// This function also performs some basic checks on the file name path
// to make sure it is at least well-formed.
//
// Arguments:
//
// szFilename - name of the target file
// uliFileSize - expected size of the file
//
// Return Values:
//
// This function returns S_OK if the amount of disk space available
// is greater than or equal to uliFileSize. If the required disk
// space is not available, then S_FALSE is returned. E_FAIL is
// returned on any hard error. Note that since multiple dowloads 
// may be executing, it is possible to run out of disk space later
// on, even this check returns S_OK.
//
HRESULT CXo::XRL_CheckDiskSpaceAccurate(char *szFilename, ULARGE_INTEGER uliFileSize)
{
	HRESULT			hr = S_OK;
	char			szFolder[MAX_PATH];
	char			*pCurrent;
	DWORD			dwLength;
	ULARGE_INTEGER	uliDiskFree, uliTemp;
    WIN32_FILE_ATTRIBUTE_DATA   fileAttributes;

	// Do some rudimentary check for the filename
	dwLength = strlen(szFilename);
	if (!dwLength || (szFilename[dwLength - 1] == '\\'))
		return(E_INVALIDARG);

	// Walk backwards to find a backslash
	pCurrent = szFilename + dwLength;
	while (pCurrent > szFilename)
		if (*(--pCurrent) == '\\')
			break;

	// See if we found a backslash			
	if (pCurrent == szFilename)
		return(E_INVALIDARG);
		
	// The path up to the backslash is the parent folder name
	dwLength = pCurrent - szFilename;
	strncpy(szFolder, szFilename, dwLength);
	szFolder[dwLength - 1] = '\0';

	// OK, now check disk space available
	if (!GetDiskFreeSpaceEx(
				szFolder,
				&uliDiskFree,
				&uliTemp,
				NULL))
	{
		// unable to test the disk space at all
		return(HRESULT_FROM_WIN32(GetLastError()));
	}

	// If this is not enough, see if the file already exists, and
	// account for that space if so.
	if (uliDiskFree.QuadPart < uliFileSize.QuadPart)
	{
		if (GetFileAttributesEx(
					szFilename,
					GetFileExInfoStandard,
					&fileAttributes))
		{
			// we got the info on the file. Add the existing size to the freespace
			// then we get the real freespace that is available.
			uliTemp.LowPart = fileAttributes.nFileSizeLow;
			uliTemp.HighPart = fileAttributes.nFileSizeHigh;
			uliDiskFree.QuadPart += uliTemp.QuadPart;
		}

		if (uliDiskFree.QuadPart < uliFileSize.QuadPart)
			return(HRESULT_FROM_WIN32(ERROR_DISK_FULL));
	}

	return(S_OK);
}

//
// Define the main do work function
//
HRESULT CXo::XRL_MainContinue(
			XONLINETASK_HANDLE		hTask, 
			const PFNXRL_HANDLER	rgpfnxrlHandlers[],
			DWORD					dwFinalState
			)
{
	HRESULT			hr = S_OK;
	PXRL_ASYNC		pxrlasync = (PXRL_ASYNC)hTask;
	DWORD			dwPreviousState;

	Assert(hTask != NULL);

	// Always signal the event. If we actually do any async work, we 
	// will reset the event. This makes sure we don't stall
	if (pxrlasync->xontask.hEventWorkAvailable)
		SetEvent(pxrlasync->xontask.hEventWorkAvailable);

	// Save the previous state
	dwPreviousState = pxrlasync->dwCurrentState;
	
	if (pxrlasync->dwCurrentState < dwFinalState)
	{
		// Call our handler to do more work
		hr = (this->*(rgpfnxrlHandlers[pxrlasync->dwCurrentState]))(pxrlasync);
	}
	else if (pxrlasync->dwCurrentState != dwFinalState)
	{
		AssertSz(FALSE, "XRL_MainContinue: Bad state");
		pxrlasync->dwCurrentState = dwFinalState;
		pxrlasync->hrFinal = E_NOTIMPL;
	}

	// When the next state is xrldtmDone, we set the flag so we don't
	// continue.
	if (pxrlasync->dwCurrentState == dwFinalState)
	{
		// Indicate that we are done and return the final code
		if (SUCCEEDED(pxrlasync->hrFinal))
		{
			// We will pass thru the X-Err information if the server
			// explicitly returned a value other than S_OK.
			if (pxrlasync->hrXErr != S_OK)
				pxrlasync->hrFinal = pxrlasync->hrXErr;
			else
				pxrlasync->hrFinal = XONLINETASK_S_SUCCESS;
		}

		hr = pxrlasync->hrFinal;
		goto Exit;
	}

	// We see if the state has changed, if yes, then refresh our timestamp
	if (pxrlasync->dwCurrentState != dwPreviousState)
	{
		pxrlasync->dwLastStateChange = GetTickCount();
	}

	// At the end of this timeslice, see if a timeout has occurred
	if ((pxrlasync->dwTimeoutMs) &&
		((GetTickCount() - pxrlasync->dwLastStateChange) > pxrlasync->dwTimeoutMs))
	{
		// Indicate that we are done and return the final code
		pxrlasync->dwCurrentState = dwFinalState;
		hr = pxrlasync->hrFinal = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
	}

Exit:	
	return(hr);
}

//
// Define the main close function
//
VOID CXo::XRL_MainClose(XONLINETASK_HANDLE hTask)
{
	PXRL_ASYNC		pxrlasync = (PXRL_ASYNC)hTask;

	Assert(hTask != NULL);

	// Release all resources and leave
	XRL_CleanupHandler(pxrlasync);
	SysFree(pxrlasync);
}
