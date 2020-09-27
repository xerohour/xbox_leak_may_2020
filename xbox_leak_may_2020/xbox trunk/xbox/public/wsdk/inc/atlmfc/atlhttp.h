// This is a part of the Active Template Library.
// Copyright (C) 1996-2001 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLHTTP_H__
#define __ATLHTTP_H__

#pragma once
#ifndef __CPPUNWIND
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
#ifndef _WINSOCKAPI_
	#include <winsock2.h>
#endif
#include <atlutil.h>
#include <atlcoll.h>
#include <atlfile.h>
#define SECURITY_WIN32
#include <security.h>
#include <atlenc.h>
#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "SECUR32.LIB")
#endif  // !_ATL_NO_DEFAULT_LIBS

#include <atlspriv.h>

namespace ATL {

template <class TSocketClass>
class CAtlHttpClientT;
class CAtlBaseAuthObject;

enum status_headerparse{
				ATL_HEADER_PARSE_COMPLETE=0,
				ATL_HEADER_PARSE_HEADERNOTCOMPLETE,
				ATL_HEADER_PARSE_HEADERERROR
};

enum readstate{rs_init=0, rs_readheader, rs_scanheader, rs_readbody, rs_complete};

#define ATL_DEFAULT_DATA_TYPE _T("application/x-www-form-urlencoded")
#define ATL_HEADER_END "\r\n\r\n"
#define ATL_HEADER_END_LEN 4
#define ATL_DW_HEADER_END 0x0a0d0a0d
#define ATL_FIELDNAME_DELIMITER _T(':')
#define ATL_MAX_FIELDNAME_LEN 1024
#define ATL_MAX_VALUE_LEN 1024
#define ATL_AUTH_HDR_SIZE 1024
#define ATL_READ_BUFF_SIZE 2048
#define ATL_INVALID_STATUS -1
#define ATL_HTTP_HEADER _T(" HTTP/1.1\r\n")
#define ATL_HTTP_HEADER_PROXY _T(" HTTP/1.1\r\n")
#define ATL_HTTP_FOOTER \
					_T("User-Agent: Microsoft-ATL-Native/7.00\r\n")

#define ATL_IS_INVALIDCREDHANDLE(x) ((x.dwLower==0xFFFFFFFF) && (x.dwUpper==0xFFFFFFFF))					
#define ATL_HTTP_AUTHTYPE_NTLM _T("NTLM")
#define ATL_HTTP_AUTHTYPE_BASIC _T("BASIC")
#define ATL_HTTP_METHOD_GET _T("GET")
#define ATL_HTTP_METHOD_POST _T("POST")
#define CHUNK_BUFF_SIZE 2048

#ifndef ATL_MAX_DOMAIN
	#define ATL_MAX_DOMAIN 64
#endif

#ifndef ATL_MAX_USERNAME
	#define ATL_MAX_USERNAME 64
#endif

#ifndef ATL_MAX_PWD
	#define ATL_MAX_PWD 64
#endif

#ifndef MAX_REALM_LEN
	#define MAX_REALM_LEN 1024
#endif

#define ATL_MAX_BASIC_ID_LEN (ATL_MAX_USERNAME + ATL_MAX_PWD + 2)

__interface IAuthInfo;
typedef bool (WINAPI *PFNATLCHUNKEDCB)(BYTE** ppData, DWORD *pdwSize);
typedef bool (WINAPI *PFNATLSTATUSCALLBACK)(DWORD dwBytesSent, DWORD dwCookie);

#define ATL_HTTP_FLAG_AUTO_REDIRECT				0x1
#define ATL_HTTP_FLAG_PROCESS_RESULT			0x2
#define ATL_HTTP_FLAG_SEND_CALLBACK				0x4
#define ATL_HTTP_FLAG_SEND_CHUNKS				0x8
#define ATL_HTTP_FLAG_INVALID_FLAGS				0xFFFFFFFF

#ifndef ATL_HTTP_DEFAULT_CHUNK_SIZE
	#define ATL_HTTP_DEFAULT_CHUNK_SIZE 4096
#endif

struct ATL_NAVIGATE_DATA
{
	LPCTSTR szExtraHeaders;
	LPCTSTR szMethod;
	LPCTSTR szDataType;
	DWORD dwDataLen;
	DWORD dwFlags;
	DWORD dwTimeout;
	DWORD dwSendChunkSize;
	DWORD dwReadChunkSize;
	DWORD m_lParamSend;
	DWORD m_lParamRead;
	short nPort;
	BYTE *pData;
	PFNATLCHUNKEDCB pfnChunkCallback;
	PFNATLSTATUSCALLBACK pfnSendStatusCallback;
	PFNATLSTATUSCALLBACK pfnReadStatusCallback;
};

class CAtlNavigateData : public ATL_NAVIGATE_DATA
{
public:
	CAtlNavigateData() throw(); // public construction
	DWORD SetFlags(DWORD dwNewFlags) throw(); // set all flags
	DWORD GetFlags() throw(); // get value of flags
	DWORD AddFlags(DWORD dwFlagsToAdd) throw(); // add one or more flags to existing flags
	DWORD RemoveFlags(DWORD dwFlagsToRemove) throw(); // remove one or more flags from existing flags
	LPCTSTR SetExtraHeaders(LPCTSTR szNewHeaders) throw(); // set the extra request headers
	LPCTSTR GetExtraHeaders() throw(); // get the extra request headers
	LPCTSTR SetMethod(LPCTSTR szNewMethod) throw(); // set the HTTP request method
	LPCTSTR GetMethod() throw(); // get the HTTP request method
	short SetPort(short newPort) throw(); // set the TCP port for this request
	short GetPort() throw(); // get the TCP port for this request
	void SetData(BYTE *pData, DWORD dwDataLen, LPCTSTR szDataType) throw(); // Set data to be sent as the reqeust entity body
	DWORD SetSocketTimeout(DWORD dwNewTimeout) throw(); // Set the timeout for this socket
	DWORD GetSocketTimeout() throw(); // Get the timeout for this socket
	DWORD SetSendChunkSize(DWORD dwChunkSize) throw(); // Set the size of the chunks used to send data
	DWORD GetSendChunkSize() throw(); // get the size of the chunks used to send data
	DWORD SetReadChunkSize(DWORD dwChunkSize) throw(); // Set the size of the chunks used to send data
	DWORD GetReadChunkSize() throw(); // get the size of the chunks used to send data
	PFNATLCHUNKEDCB SetChunkCallback(PFNATLCHUNKEDCB pfn) throw(); // set the callback function used for sending chunked data
	PFNATLCHUNKEDCB GetChunkCallback() throw(); // get the chunked callback function
	PFNATLSTATUSCALLBACK SetSendStatusCallback(PFNATLSTATUSCALLBACK pfn, DWORD dwData) throw(); // sets a function pointer to be called after bytes are sent over the socket
	PFNATLSTATUSCALLBACK GetSendStatusCallback() throw(); // returns current status callback function
	PFNATLSTATUSCALLBACK SetReadStatusCallback(PFNATLSTATUSCALLBACK pfn, DWORD dwData) throw();
	PFNATLSTATUSCALLBACK GetReadStatusCallback() throw();
};

template <class TSocketClass>
class CAtlHttpClientT : 
	private TSocketClass
{
public:
	CAtlHttpClientT() throw();

	// Use these functions to send an HTTP request and retrieve
	// the response.
	bool Navigate(
				const CUrl* pUrl,
				ATL_NAVIGATE_DATA *pNavData = NULL
				) throw();

	bool Navigate(
				LPCTSTR szServer,
				LPCTSTR szPath,
				ATL_NAVIGATE_DATA *pNavData = NULL
				) throw();

	bool Navigate(
				LPCTSTR szURL,
				ATL_NAVIGATE_DATA *pNavData = NULL
				) throw();


	// Performs navigation, sending data with Transfer-Coding: chunked
	bool NavigateChunked(
			LPCTSTR szURL,
			PFNATLCHUNKEDCB,
			DWORD dwFlags=ATL_HTTP_FLAG_AUTO_REDIRECT,
			LPCTSTR szExtraHeaders=NULL,
			LPCTSTR szMethod=ATL_HTTP_METHOD_GET,
			short nPort=ATL_URL_DEFAULT_HTTP_PORT,
			LPCTSTR szDataType=NULL,
			bool bProcessResult=true
			) throw();

	// Use to set/retrieve information about the proxy server used
	// when making this request via a proxy server.
	BOOL SetProxy(LPCTSTR szProxy = NULL, short nProxyPort = 0) throw();
	void RemoveProxy() throw();
	LPCTSTR GetProxy() const throw();

	// Use these functions to add/remove/find objects that will 
	// be used to authorize request when a 401 Not Authorized response
	// is received. This class maps these objects by scheme name in map.
	// Override NegotiateAuth to change the way authorization negotiation occurs.
	bool AddAuthObj(LPCTSTR szScheme, CAtlBaseAuthObject *pObject, IAuthInfo *pInfo=NULL) throw();
	const CAtlBaseAuthObject* FindAuthObject(LPCTSTR szScheme) throw();
	bool RemoveAuthObject(LPCTSTR szScheme) throw();
	virtual bool NegotiateAuth(bool bProxy) throw();
	

	// Retrieve the value of a response header
	bool GetHeaderValue(LPCTSTR szName, CString& strValue) const throw(); 
	bool GetHeaderValue(LPCTSTR szName, LPTSTR szBuffer, int *pdwLen) const throw();

	const int GetResponseLength() throw(); // Get the number of bytes in the response
	const BYTE* GetResponse() throw(); // Get the entire response
	DWORD GetBodyLength() const throw(); // Get the length of the body of the response (everything after the \r\n\r\n)
	const BYTE* GetBody() throw(); // Get the body of the response (length is determined by GetBodyLength())
	DWORD GetRawRequestHeaderLength() throw(); // Get the length of the raw request headers
	bool GetRawRequestHeaders(LPBYTE szBuffer, int *pdwLen) throw(); // Get the raw request headers
	LPCURL GetCurrentUrl() const throw(); // Get a pointer to the current URL for this request
	void SetFlags(DWORD dwFlags) throw(); // Set flags used for processing this request
	DWORD GetFlags() const throw(); // Retrieve flags used for processing this request
	int GetStatus() throw(); // Get the HTTP status code that resulted from making this request
	LPCTSTR GetMethod() throw(); // Get the HTTP method used for making this request
	BYTE* GetData() throw(); // Get a pointer to raw data being sent with this request
	DWORD GetDataLen() throw(); // Get the length of the raw data sent with this request
	LPCTSTR GetDataType() throw(); // Get the data type (sent as Content-Type header) for this request
	DWORD GetLastError() throw(); // Retrieves errors from the underlying socket
	const SOCKET& GetSocket() throw(); // Retrieves the underlying socket. Be careful!
	void Close(); // Close the connection
	DWORD SetSocketTimeout(DWORD dwNewTimeout); // Sets a new socket timeout, returns the old timeout.
	DWORD GetSocketTimeout(); // retrieves the current socket timeout
	void AuthProtocolFailed(LPCTSTR szProto); // notifies us of failure to connect with the named protocol

// Implementation
	enum HTTP_RESPONSE_READ_STATUS
	{
		RR_OK = 0, // response was successfully processed
		RR_FAIL, // an unknown error occurred reading the HTTP response
		RR_STATUS_INVALID, // could not parse the status line
		RR_PARSEHEADERS_FAILED, // failed to parse HTTP response headers
		RR_READSOCKET_FAILED, // failed to read response data from socket
		RR_READBODY_FAILED, // failed to successfully read the entity body of the HTTP response
		RR_READCHUNKEDBODY_FAILED // failed to read a 'Transfer-Encoding: chunked' response body
	};
	HTTP_RESPONSE_READ_STATUS ReadHttpResponse() throw();
	void ResetConnection() throw();
	bool ProcessStatus(DWORD dwFlags) throw();
	bool BuildRequest(/*out*/CString *pstrRequest,
						LPCTSTR szMethod,
						LPCTSTR szServer,
						LPCTSTR szPath,
						LPCTSTR szExtraInfo=NULL,
						const BYTE* pData=NULL,
						DWORD dwDataLen=0,
						LPCTSTR szDataType=NULL,
						LPCTSTR szExtraHeaders=NULL) throw();

protected:
	DWORD WriteWithNoData(LPCSTR pRequest, DWORD dwRequestLen);
	DWORD WriteWithCallback(LPCSTR pRequest, DWORD dwRequestLen);
	DWORD WriteWithChunks(LPCSTR pRequest, DWORD dwRequestLen);
	DWORD WriteWithData(LPCSTR pRequest, DWORD dwRequestLen);
	bool SetDefaultUrl(LPCTSTR szUrl, short nPortNumber=ATL_URL_DEFAULT_HTTP_PORT) throw();
	bool SetDefaultUrl(LPCURL pUrl, short nPortNumber=ATL_URL_DEFAULT_HTTP_PORT) throw();
	bool SetDefaultMethod(LPCTSTR szMethod) throw();
	void InitializeObject() throw();
	bool ReadSocket() throw();
	unsigned char* FindHeaderEnd(unsigned char** ppBegin) throw();
	bool LookupRegProxy() throw();
	bool DisconnectIfRequired() throw();
	bool ConnectSocket() throw();

	long GetContentLength() throw();
	LPCSTR NextLine(BYTE* pCurr) throw();
	bool IsMsgBodyChunked() throw();
	LPCSTR FindEndOfHeader(LPCSTR pszStart) throw();
	bool DecodeHeader(LPCSTR pHeaderStart, LPCSTR pHeaderEnd) throw();
	virtual void OnSetCookie(LPCTSTR /*szCookie*/) throw();
	LPCSTR ParseStatusLine(BYTE* pBuffer) throw();
	int CrackResponseHeader(LPCSTR pBuffer, /*out*/ LPCSTR *pEnd) throw();
	bool ReadBody(int nContentLen, int nCurrentBodyLen) throw();
	bool ReadChunkedBody() throw();
	bool ReconnectIfRequired() throw();
	bool CompleteURL(CString& strURL) throw();
	bool ProcessObjectMoved() throw();
	bool _SetDefaultUrl(LPCTSTR szURL, short nPort) throw();

	enum CHUNK_STATE{
		READ_CHUNK_SIZE, // need to read the size of a chunk.
		READ_CHUNK_SIZE_FOOTER,
		READ_CHUNK_DATA, // need to read the actual data
		READ_CHUNK_DATA_FOOTER, // need to read the chunk footer.
		READ_CHUNK_TRAILER, // Read the trailer headers at the end of the chunk data
		READ_CHUNK_TRAILER_FOOTER, // read the final crlf
		CHUNK_READ_DATA_COMPLETE, // done reading chunk data.
	};

	enum CHUNK_LEX_RESULT{
		LEX_OK,
		LEX_OUTOFDATA,
		LEX_ERROR,
		LEX_TRAILER_COMPLETE
	};

	CHUNK_LEX_RESULT get_chunked_size(char *&pBuffStart, char *&pBuffEnd, long* pnChunkSize);
	bool move_leftover_bytes(char *pBufferStart, int nLen, char *&pBuffStart, char *&pBuffEnd);
	CHUNK_LEX_RESULT get_chunked_data(char *&pBufferStart, char *&pBufferEnd, long nChunkSize,
								  char **ppDataStart, long *pnDataLen);
	CHUNK_LEX_RESULT consume_chunk_trailer(char *&pBufferStart, char *pBufferEnd);
	CHUNK_LEX_RESULT consume_chunk_footer(char *&pBufferStart, char *&pBufferEnd);

	typedef CAtlMap< 
				CString,
				CString,
				CStringElementTraitsI<CString>,
				CStringElementTraitsI<CString>
			   > HeaderMapType;

	typedef CAtlMap <
				CString,
				CAtlBaseAuthObject*,
				CStringElementTraitsI<CString>
				> AuthMapType;

	typedef CAtlArray<
				CString,
				CStringElementTraitsI<CString>
				> AuthListType;

	HeaderMapType m_HeaderMap; // Map of response headers
	AuthMapType m_AuthMap; // Map of pointers to authorization objects.
	AuthListType m_AuthTypes; // list of authorization types the server is willing to use.
	CAtlIsapiBuffer<> m_current; // The entire response
	CUrl m_urlCurrent; // URL of current request

	CString m_strMethod; // Current request method.
	CString m_strProxy; // Path to current proxy server.
	CString m_strDataType; // If this is a post, this contains the data type of the data

	long m_nStatus; // Current response status (from status line)
	short m_nProxyPort; // Port used on current proxy server
	DWORD m_dwBodyLen; // Length of body
	DWORD m_dwHeaderLen; // Length of current raw headers
	DWORD m_dwHeaderStart;
	BYTE *m_pData; // Pointer to data sent with request (warning: we don't make our own copy of the data!)
	DWORD m_dwDataLen; // Length of current data in bytes
	DWORD m_dwFlags; // Flags that describe the request.
	BYTE *m_pCurrent;
	ATL_NAVIGATE_DATA *m_pNavData;
	HTTP_RESPONSE_READ_STATUS m_LastResponseParseError;
}; //CAtlHttpClientT
typedef CAtlHttpClientT<ZEvtSyncSocket> CAtlHttpClient;


// Interface used to acquire authentication information from clients
__interface IAuthInfo
{
	bool GetPassword(LPTSTR szPwd, DWORD dwBuffSize);
	bool GetUsername(LPTSTR szUid, DWORD dwBuffSize);
	bool GetDomain(LPTSTR szDomain, DWORD dwBuffSize);
};

// pure virtual class that describes required functions for authoriztion
// objects
class CAtlBaseAuthObject
{
public:
	CAtlBaseAuthObject();
	virtual bool Authenticate(LPCTSTR szAuthTypes, bool bProxy) = 0;
	virtual void Init(CAtlHttpClient *pSocket, IAuthInfo *pAuthInfo) = 0;
	bool m_bFailed;
};

// strings used for authentication.
__declspec(selectany)const TCHAR *g_pszWWWAuthenticate = _T("www-authenticate");
__declspec(selectany)const TCHAR *g_pszProxyAuthenticate = _T("proxy-authenticate");

// Performs NTLM authentication
class CNTLMAuthObject :
	public CAtlBaseAuthObject
{
public:
	~CNTLMAuthObject() throw();
	CNTLMAuthObject() throw();
	CNTLMAuthObject(IAuthInfo *pAuthInfo) throw();
	void SetAuthInfo(IAuthInfo *pAuthInfo) throw();
	
	// Called by the CAtlHttpClient class to authenticate a user.
	virtual void Init(CAtlHttpClient *pSocket, IAuthInfo *pAuthInfo=NULL) throw();
	
	// Called by the CAtlHttpClient class to initialize this authentication object.
	virtual bool Authenticate(LPCTSTR szAuthTypes, bool bProxy) throw();

// Implementation
protected:
	bool AcquireCredHandle() throw();
	// This function creates an NTML Authorization header
	// and sends it to the HTTP server.
	bool SendSecurityInfo(SecBuffer *pSecBuffer, LPSTR *pszBuffer);
	bool DoNTLMAuthenticate() throw();

	IAuthInfo *m_pAuthInfo;
	CAtlHttpClient *m_pSocket;
	CredHandle m_hCredentials;
	int m_nMaxTokenSize;
	TimeStamp m_ts;
	bool m_bProxy;
	static const char *m_pszFmtWWW;
	static const char *m_pszFmtProxy;

}; // CNTLMAuthObject

// Performs BASIC authentication for an CAtlHttpClient
// object. Caller must implement an IAuthInfo interface
// and pass it to this object before this object attempts
// to authenticate or authentication will fail.
class CBasicAuthObject : 
	public CAtlBaseAuthObject
{
public:
	CBasicAuthObject() throw();
	CBasicAuthObject(IAuthInfo *pAuthInfo) throw();
	void SetAuthInfo(IAuthInfo *pAuthInfo) throw();
	LPCTSTR GetRealm() throw(); // Retrieve's the realm being used.

	// Called by the CAtlHttpClient class to authenticate a user.
	virtual bool Authenticate(LPCTSTR szAuthTypes, bool bProxy) throw();

	// Called by the CAtlHttpClient class to initialize this authentication object.
	virtual void Init(CAtlHttpClient *pSocket, IAuthInfo *pAuthInfo=NULL) throw();
protected:
	bool DoBasicAuthenticate() throw();
	void CrackRealm(LPTSTR szHeader) throw();

	IAuthInfo *m_pAuthInfo;
	CAtlHttpClient *m_pClient;
	TCHAR m_szRealm[MAX_REALM_LEN];
	bool m_bProxy;
	static const char *m_pszFmtWWW;
	static const char *m_pszFmtProxy;
}; // CBasicAuthObject

__declspec(selectany)const char *CBasicAuthObject::m_pszFmtWWW = "Authorization: Basic ";
__declspec(selectany)const char *CBasicAuthObject::m_pszFmtProxy = "Proxy-Authorization: Basic ";
__declspec(selectany)const char *CNTLMAuthObject::m_pszFmtWWW = "Authorization: NTLM %s\r\n";
__declspec(selectany)const char *CNTLMAuthObject::m_pszFmtProxy = "Proxy-Authorization: NTLM %s\r\n";


//
// Security Service Provider Interface (sspi) Helper classes
// These classes are used as helpers for structures used in 
// SSPI functions.
//
class CSecAuthIdentity : public SEC_WINNT_AUTH_IDENTITY_EX
{
public:
	CSecAuthIdentity() throw()
	{
		Version = SEC_WINNT_AUTH_IDENTITY_VERSION;
		Length = sizeof(SEC_WINNT_AUTH_IDENTITY_EX);
#ifdef _UNICODE
		Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
#else
		Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
#endif
	}

	bool Init(IAuthInfo *pAuthInfo) throw()
	{
		if (!pAuthInfo)
			return false;

		if (!pAuthInfo->GetUsername(szUserName, ATL_MAX_USERNAME))
			return false;

		if (!pAuthInfo->GetPassword(szPassword, ATL_MAX_PWD))
			return false;

		if (!pAuthInfo->GetDomain(szDomain, ATL_MAX_DOMAIN))
			return false;
#ifndef _UNICODE
		User = (unsigned char*)szUserName;
		Domain = (unsigned char*)szDomain;
		Password = (unsigned char*)szPassword;
#else
		// have to cast to unsigned short *, because SEC_WINNT_AUTH_IDENTITY_EXW
		// uses unsigned short instead of wchar_t for some reason
		User = (unsigned short *)szUserName;
		Domain = (unsigned short *)szDomain;
		Password = (unsigned short *)szPassword;
#endif
		UserLength = (DWORD)_tcslen(szUserName);
		DomainLength = (DWORD)_tcslen(szDomain);
		PasswordLength = (DWORD)_tcslen(szPassword);
		return true;
	}

protected:
	TCHAR szUserName[ATL_MAX_USERNAME];
	TCHAR szPassword[ATL_MAX_PWD];
	TCHAR szDomain[ATL_MAX_DOMAIN];
}; // CSecAuthIdentity

class CSecBuffer : public SecBuffer
{
public:
	CSecBuffer() throw()
	{
		cbBuffer = 0;
		BufferType = 0;
		pvBuffer = NULL;
	}

	~CSecBuffer() throw()
	{
		if (pvBuffer)
			delete [] static_cast<unsigned char*>(pvBuffer);
	}

	bool SetSize(unsigned int nSize) throw()
	{
		if (!nSize)
			return false;

		if (pvBuffer)
		{
			delete [] static_cast<unsigned char*>(pvBuffer);
			pvBuffer = NULL;
			cbBuffer = 0;
		}

		ATLTRY(pvBuffer = static_cast<void*>(new unsigned char[nSize]));
		if (pvBuffer)
		{
			cbBuffer = nSize;
			BufferType = SECBUFFER_TOKEN;
			return true;
		}
		return false;
	}

	void ClearBuffer(int nSize) throw()
	{
		ZeroMemory(pvBuffer, min((int)cbBuffer, nSize));
		cbBuffer = nSize;
	}

	unsigned long Size()
	{
		return cbBuffer;
	}

	unsigned char *Buffer() throw()
	{
		return static_cast<unsigned char*>(pvBuffer);
	}

	operator SecBuffer*() throw()
	{
		return (SecBuffer*)this;
	}
}; // CSecBuffer

class CSecBufferDesc : public SecBufferDesc
{
public:
	CSecBufferDesc() throw()
	{
		ulVersion = SECBUFFER_VERSION;
		cBuffers = 0;
		pBuffers = NULL;
	}

	~CSecBufferDesc() throw()
	{
		cBuffers = 0;
		
		if (pBuffers)
		{
			CSecBuffer *psb = (CSecBuffer*)pBuffers;
			delete [] psb;
		}
	}

	CSecBuffer* Buffers(unsigned int i) throw()
	{
		CSecBuffer *pBuffer = (CSecBuffer*)(&pBuffers[i]);
		return pBuffer;
	}

	bool AddBuffers(unsigned int nCount, unsigned int nBufferSize) throw()
	{
		if (!nCount)
			return true;

		if (cBuffers == 0)
		{
			CSecBuffer *pSecBuffer = NULL;
			ATLTRY(pSecBuffer = new CSecBuffer[nCount]);
			if (!pSecBuffer)
				return false;
			for (unsigned int i=0; i<nCount; i++)
			{
				if (!pSecBuffer[i].SetSize(nBufferSize))
					return false;
			}
			cBuffers = nCount;
			pBuffers = (SecBuffer*)pSecBuffer;
		}
		else // realloc
		{
			CSecBuffer *pSecBuffer = NULL;
			ATLTRY(pSecBuffer = new CSecBuffer[nCount + cBuffers]);
			if (!pSecBuffer)
				return false;
			memcpy(pSecBuffer, pBuffers, sizeof(CSecBuffer)*cBuffers);
			delete [] pBuffers;

			// initialize new buffers
			for (unsigned int i=0; i<nCount; i++)
			{
				if (!pSecBuffer[cBuffers+i].SetSize(nBufferSize))
					return false;
			}
			pBuffers = pSecBuffer;
			cBuffers = nCount + cBuffers;
		}
		return true;
	}

	operator PSecBufferDesc() throw()
	{
		return static_cast<PSecBufferDesc>(this);
	}
}; // CSecBufferDesc

#include <atlhttp.inl>
#ifndef __CPPUNWIND
#pragma warning(pop)
#endif
} // ATL

#endif // __ATLHTTP_H__
