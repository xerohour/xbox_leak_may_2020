// This is a part of the Active Template Library.
// Copyright (C) 1996-2001 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLSOAP_H__
#define __ATLSOAP_H__

#pragma once

#if (defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_))
	#error require winsock2.h -- include <winsock2.h> before you include <windows.h>
#endif

#if ((_WIN32_WINNT < 0x0400) && (_WIN32_WINDOWS <= 0x0400))
	#error require _WIN32_WINNT >= 0x0400 or _WIN32_WINDOWS > 0x0400
#endif

[ emitidl("restricted") ];

#include <winsock2.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <atlbase.h>
#include <msxml2.h>
#include <atlenc.h>
#include <fcntl.h>
#include <float.h>
#include <atlisapi.h>
#include <atlstencil.h>
#include <atlhttp.h>
#include <atlhttp.inl>

#ifndef ATLSOAP_NOWININET
	#include <wininet.h>
	#ifndef ATLSOAPINET_CLIENT
		#define ATLSOAPINET_CLIENT _T("VCSoapClient")
	#endif
#endif

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "msxml2.lib")
	#ifndef ATLSOAP_NOWININET
		#pragma comment(lib, "wininet.lib")
	#endif
#endif

#define _ATLSOAP_MAKEWIDESTR( str ) L ## str
#define ATLSOAP_MAKEWIDESTR( str ) _ATLSOAP_MAKEWIDESTR( str )

namespace ATL
{

////////////////////////////////////////////////////////////////////////////////
//
// IStreamImpl - stub IStream implementation class
//
////////////////////////////////////////////////////////////////////////////////

class IStreamImpl : public IStream
{
public:

	HRESULT __stdcall Read(void * /*pDest*/, ULONG /*nMaxLen*/, ULONG * /*pnRead*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Write(const void * /*pv*/, ULONG /*cb*/, ULONG * /*pcbWritten*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Seek(LARGE_INTEGER /*dlibMove*/, DWORD /*dwOrigin*/, 
		ULARGE_INTEGER * /*pLibNewPosition*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall SetSize(ULARGE_INTEGER /*libNewSize*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall CopyTo(IStream * /*pStream*/, ULARGE_INTEGER /*cb*/, 
		ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Commit(DWORD /*grfCommitFlags*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Revert() throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Stat(STATSTG * /*pstatstg*/, DWORD /*grfStatFlag*/) throw()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Clone(IStream ** /*ppstm*/) throw()
	{
		return E_NOTIMPL;
	}
}; // class IStreamImpl

////////////////////////////////////////////////////////////////////////////////
//
// CStreamOnServerContext
//
////////////////////////////////////////////////////////////////////////////////

class CStreamOnServerContext : public IStreamImpl//, public CComObjectRootEx<CComSingleThreadModel>
{
public:
//	BEGIN_COM_MAP(CStreamOnServerContext)
//		COM_INTERFACE_ENTRY(IStream)
//		COM_INTERFACE_ENTRY(ISequentialStream)
//	END_COM_MAP()

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_IStream) ||
			InlineIsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = static_cast<IStream *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

private:

	IHttpServerContext * m_pServerContext;
	DWORD m_dwBytesRead;

public:

	CStreamOnServerContext(IHttpServerContext *pServerContext = NULL) throw()
		: m_pServerContext(pServerContext), m_dwBytesRead(0)
	{
	}

	void SetServerContext(IHttpServerContext *pServerContext) throw()
	{
		ATLASSERT( m_pServerContext == NULL );

		m_pServerContext = pServerContext;
	}

	HRESULT __stdcall Read(void *pDest, ULONG nMaxLen, ULONG *pnRead) throw()
	{
		ATLASSERT( pDest != NULL );
		ATLASSERT( m_pServerContext != NULL );

		DWORD dwToRead = min(m_pServerContext->GetTotalBytes()-m_dwBytesRead, nMaxLen);
		if (ReadClientData(m_pServerContext, (LPSTR) pDest, &dwToRead, m_dwBytesRead) != FALSE)
		{
			m_dwBytesRead+= dwToRead;

			if (pnRead != NULL)
			{
				*pnRead = dwToRead;
			}

			return S_OK;
		}

		return E_FAIL;
	}
}; // class CStreamOnServerContext

////////////////////////////////////////////////////////////////////////////////
//
// CReadStreamOnSocket
//
////////////////////////////////////////////////////////////////////////////////

template <typename TSocketClass>
class CReadStreamOnSocket : public IStreamImpl//, public CComObjectRootEx<CComSingleThreadModel>
{
public:

//	BEGIN_COM_MAP(CReadStreamOnSocket)
//		COM_INTERFACE_ENTRY(IStream)
//		COM_INTERFACE_ENTRY(ISequentialStream)
//	END_COM_MAP()

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_IStream) ||
			InlineIsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = static_cast<IStream *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

private:

	CAtlHttpClientT<TSocketClass> * m_pSocket;
	LPCSTR m_szBuffer;
	LPCSTR m_szCurr;
	long m_nBodyLen;

public:

	CReadStreamOnSocket() throw()
		: m_pSocket(NULL), m_szBuffer(NULL), m_szCurr(NULL), m_nBodyLen(0)
	{
	}

	BOOL Init(CAtlHttpClientT<TSocketClass> *pSocket) throw()
	{
		ATLASSERT( pSocket != NULL );

		m_pSocket = pSocket;
		m_szBuffer = (LPCSTR) pSocket->GetBody();
		if (m_szBuffer != NULL)
		{
			m_szCurr = m_szBuffer;
			m_nBodyLen = pSocket->GetBodyLength();
			if (m_nBodyLen != 0)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	HRESULT __stdcall Read(void *pDest, ULONG nMaxLen, ULONG *pnRead) throw()
	{
		ATLASSERT( pDest != NULL );
		ATLASSERT( m_pSocket != NULL );
		ATLASSERT( m_szBuffer != NULL );

		if (pnRead != NULL)
		{
			*pnRead = 0;
		}

		long nRead = (int) (m_szCurr-m_szBuffer);
		if (nRead < m_nBodyLen)
		{
			long nLength = min((int)(m_nBodyLen-nRead), (LONG) nMaxLen);
			memcpy(pDest, m_szCurr, nLength);
			m_szCurr+= nLength;

			if (pnRead != NULL)
			{
				*pnRead = (ULONG) nLength;
			}
		}

		return S_OK;
	}
}; // class CReadStreamOnSocket

////////////////////////////////////////////////////////////////////////////////
//
// CWriteStreamOnCString
//
////////////////////////////////////////////////////////////////////////////////

class CWriteStreamOnCString : public IWriteStream
{

public:

	CStringA m_str;

	HRESULT WriteStream(LPCSTR szOut, int nLen, LPDWORD pdwWritten) throw()
	{
		ATLASSERT( szOut != NULL );

		if (nLen < 0)
		{
			nLen = (int) strlen(szOut);
		}

		_ATLTRY
		{
			m_str.Append(szOut, nLen);
		}
		_ATLCATCHALL()
		{
			return E_OUTOFMEMORY;
		}

		if (pdwWritten != NULL)
		{
			*pdwWritten = (DWORD) nLen;
		}

		return S_OK;
	}

	HRESULT FlushStream() throw()
	{
		return S_OK;
	}

	void Cleanup() throw()
	{
		m_str.Empty();
	}
}; // class CWriteStreamOnCString

////////////////////////////////////////////////////////////////////////////////
//
// Namespaces
//
////////////////////////////////////////////////////////////////////////////////

#define SOAPENV_NAMESPACEA "http://schemas.xmlsoap.org/soap/envelope/"
#define SOAPENV_NAMESPACEW ATLSOAP_MAKEWIDESTR( SOAPENV_NAMESPACEA )

#define SOAPENC_NAMESPACEA "http://schemas.xmlsoap.org/soap/encoding/"
#define SOAPENC_NAMESPACEW ATLSOAP_MAKEWIDESTR( SOAPENC_NAMESPACEA )

#define XSI_NAMESPACEA  "http://www.w3.org/2000/10/XMLSchema-instance"
#define XSI_NAMESPACEW  ATLSOAP_MAKEWIDESTR( XSI_NAMESPACEA )

#define XSD_NAMESPACEA  "http://www.w3.org/2000/10/XMLSchema"
#define XSD_NAMESPACEW  ATLSOAP_MAKEWIDESTR( XSD_NAMESPACEA )

#ifndef ATLSOAP_GENERIC_NAMESPACE
#define ATLSOAP_GENERIC_NAMESPACE L"http://www.tempuri.org"
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Helpers
//
////////////////////////////////////////////////////////////////////////////////

inline HRESULT GetAttribute(
	ISAXAttributes *pAttributes, 
	const wchar_t *wszAttrName, int cchName, 
	const wchar_t **pwszValue, int *pcchValue,
	wchar_t *wszNamespace = NULL, int cchNamespace = 0) throw()
{
	if (!pAttributes || !wszAttrName || !pwszValue || !pcchValue)
	{
		return E_INVALIDARG;
	}

	*pwszValue = NULL;
	*pcchValue = 0;
	if (!wszNamespace)
	{
		return (pAttributes->getValueFromQName(wszAttrName, cchName, pwszValue, pcchValue) == S_OK ? S_OK : E_FAIL);
	}
	return (pAttributes->getValueFromName(wszNamespace, cchNamespace, 
		wszAttrName, cchName, pwszValue, pcchValue) == S_OK ? S_OK : E_FAIL);
}

inline HRESULT GetAttribute(
	ISAXAttributes *pAttributes, 
	const wchar_t *wszAttrName, int cchName, 
	CStringW &strValue,
	wchar_t *wszNamespace = NULL, int cchNamespace = 0) throw()
{
	const wchar_t *wszValue = NULL;
	int cchValue = 0;

	if (!pAttributes || !wszAttrName)
	{
		return E_INVALIDARG;
	}

	HRESULT hr;
	if (!wszNamespace)
	{
		hr = (pAttributes->getValueFromQName(wszAttrName, cchName, &wszValue, &cchValue) == S_OK ? S_OK : E_FAIL);
	}
	else
	{
		hr = (pAttributes->getValueFromName(wszNamespace, cchNamespace, 
			wszAttrName, cchName, &wszValue, &cchValue) == S_OK ? S_OK : E_FAIL);
	}

	if (hr == S_OK)
	{
		_ATLTRY
		{
			strValue.SetString(wszValue, cchValue);
		}
		_ATLCATCHALL()
		{
			hr = E_OUTOFMEMORY;
		}
	}

	return hr;
}

inline HRESULT AtlSoapGetArraySize(ISAXAttributes *pAttributes, size_t *pnSize, 
	const wchar_t **pwszTypeStart = NULL, const wchar_t **pwszTypeEnd = NULL) throw()
{
	if (pnSize == NULL)
	{
		return E_POINTER;
	}

	if (pAttributes == NULL)
	{
		return E_INVALIDARG;
	}

	*pnSize = 0;

	const wchar_t *wsz;
	int cch;

	HRESULT hr = GetAttribute(pAttributes, L"arrayType", sizeof("arrayType")-1, 
		&wsz, &cch, SOAPENC_NAMESPACEW, sizeof(SOAPENC_NAMESPACEA)-1);

	if ((SUCCEEDED(hr)) && (wsz != NULL))
	{
		hr = E_FAIL;
		
		const wchar_t *wszTypeStart = NULL;
		const wchar_t *wszTypeEnd = NULL;

		// skip spaces
		while (iswspace(*wsz) != 0)
		{
			wsz++;
		}

		// no need to walk the string if the caller is not interested
		if ((pwszTypeStart != NULL) && (pwszTypeEnd != NULL))
		{
			wszTypeStart = wsz;
			wszTypeEnd = wcschr(wszTypeStart, L':');
			if (wszTypeEnd != NULL)
			{
				wszTypeStart = wszTypeEnd+1;
			}
		}

		// SOAP Section 5 encodings are of the form:
		//   <soap_enc namespace>:arrayType="<type_qname>[dim1(,dim_i)*]
		//   for example: SOAP-ENC:arrayType="xsd:string[2,4]"

		wsz = wcschr(wsz, L'[');
		if (wsz != NULL)
		{
			wszTypeEnd = wsz-1;

			*pnSize = 1;

			// get the size of each dimension
			while (wsz != NULL)
			{
				wsz++;
				*pnSize *= (size_t) _wtoi(wsz);

				wsz = wcschr(wsz, L',');
			}

			if ((pwszTypeStart != NULL) && (pwszTypeEnd != NULL))
			{
				*pwszTypeStart = wszTypeStart;
				*pwszTypeEnd = wszTypeEnd;
			}

			hr = S_OK;
		}
	}
	else
	{
		// not a section-5 encoding
		hr = S_OK;
	}

	return hr;
}

inline size_t AtlSoapGetArrayDims(const int *pDims) throw()
{
	if (pDims == NULL)
	{
		return 0;
	}

	size_t nRet = 1;
	for (int i=1; i<=pDims[0]; i++)
	{
		nRet *= pDims[i];
	}

	return nRet;
}

} // namespace ATL

////////////////////////////////////////////////////////////////////////////////
//
// BLOB data type - use this struct when you want to send BLOB data
//   the attribute provider and proxy generator will only properly special
//   case blob data when using this struct.
//
////////////////////////////////////////////////////////////////////////////////

[ export ]
typedef struct _tagATLSOAP_BLOB
{
	unsigned long size;
	unsigned char *data;
} ATLSOAP_BLOB;


//
// the following function template will not compile
// this is by design to catch types that are not handled
// by specializations
//
template <typename T>
inline HRESULT AtlGetSAXValue(T * /* pVal */, const wchar_t * /* wsz */, int /* cch */) throw()
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AtlGetXMLValue (for IXMLDOMDocument) - get the real type from the XML data
//
///////////////////////////////////////////////////////////////////////////////

//
// generic IXMLDOMNode template function
// delegates to AtlGetSAXValue
//
template <typename T>
inline HRESULT AtlGetXMLValue(IXMLDOMNode *pParam, T *pVal) throw()
{
	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal);
	if (SUCCEEDED(hr))
	{
		hr = AtlGetSAXValue(pVal, bstrVal, bstrVal.Length());
	}

	return hr;
}

// specialization for BSTR
inline HRESULT AtlGetXMLValue<BSTR>(IXMLDOMNode *pParam, BSTR *pbstrVal) throw()
{
	if (pParam == NULL)
	{
		return E_INVALIDARG;
	}
	if (pbstrVal == NULL)
	{
		return E_POINTER;
	}

	CComPtr<IXMLDOMNode> spChild;
	if (pParam->get_firstChild(&spChild) == S_OK)
	{
		CComPtr<IXMLDOMNode> spXmlChild;
		if (spChild->get_firstChild(&spXmlChild) == S_OK)
		{
			return (pParam->get_xml(pbstrVal) == S_OK ? S_OK : E_FAIL);
		}
	}

	return (pParam->get_text(pbstrVal) == S_OK) ? S_OK : E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////
//
// AtlGetSAXValue - (for SAX or generic) get the real type from the XML data
//
////////////////////////////////////////////////////////////////////////////////

inline HRESULT AtlGetSAXValue<bool>(bool *pVal, const wchar_t *wsz, int cch) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	*pVal = false;

	HRESULT hr = E_FAIL;
	switch (wsz[0])
	{
		case L'1':
		{
			if (cch==1)
			{
				*pVal = true;
				hr = S_OK;
			}
			break;
		}
		case L'0':
		{
			if (cch==1)
			{
				*pVal = false;
				hr = S_OK;
			}
			break;
		}
		case L't':
		{
			if (cch==sizeof("true")-1 && !wcsncmp(wsz, L"true", cch))
			{
				*pVal = true;
				hr = S_OK;
			}
			break;
		}
		case L'f':
		{
			if (cch==sizeof("false")-1 && !wcsncmp(wsz, L"false", cch))
			{
				*pVal = false;
				hr = S_OK;
			}
			break;
		}
	}

	return hr;
}

inline HRESULT AtlGetSAXValue<char>(char *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	*pVal = (char) _wtoi(wsz);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<unsigned char>(unsigned char *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	wchar_t *p;
	*pVal = (unsigned char) wcstoul(wsz, &p, 10);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<short>(short *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	*pVal = (short) _wtoi(wsz);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<unsigned short>(unsigned short *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	wchar_t *p;
	*pVal = (unsigned short) wcstoul(wsz, &p, 10);

	return S_OK;
}

#ifdef _NATIVE_WCHAR_T_DEFINED
inline HRESULT AtlGetSAXValue<wchar_t>(wchar_t *pVal, const wchar_t *wsz, int cch) throw()
{
	return AtlGetSAXValue((unsigned short *)pVal, wsz, cch);
}
#endif

inline HRESULT AtlGetSAXValue<int>(int *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	*pVal = (int) _wtoi(wsz);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<unsigned int>(unsigned int *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	wchar_t *p;
	*pVal = (unsigned int) wcstoul(wsz, &p, 10);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<long>(long *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	*pVal = (long) _wtoi(wsz);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<unsigned long>(unsigned long *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	wchar_t *p;
	*pVal = (unsigned long) wcstoul(wsz, &p, 10);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<__int64>(__int64 *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	wchar_t *p;
	*pVal = _wcstoi64(wsz, &p, 10);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<unsigned __int64>(unsigned __int64 *pVal, const wchar_t *wsz, int /*cch*/) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	wchar_t *p = NULL;
	*pVal = _wcstoui64(wsz, &p, 10);

	return S_OK;
}

inline HRESULT AtlGetSAXValue<double>(double *pVal, const wchar_t *wsz, int cch) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	if (cch == 3)
	{
		if (wsz[0]==L'I' && !wcsncmp(wsz, L"INF", cch))
		{
			*(((int *) pVal)+0) = 0x0000000;
			*(((int *) pVal)+1) = 0x7FF00000;
		}
		else if (wsz[0]==L'N' && !wcsncmp(wsz, L"NAN", cch))
		{
			*(((int *) pVal)+0) = 0x0000000;
			*(((int *) pVal)+1) = 0xFFF80000;
		}
	}
	else if (cch == 4 && wsz[1]==L'I' && !wcsncmp(wsz, L"-INF", cch))
	{
		*(((int *) pVal)+0) = 0x0000000;
		*(((int *) pVal)+1) = 0xFFF00000;
	}
	else
	{
		wchar_t *p;
		*pVal = wcstod(wsz, &p);
	}

	return S_OK;
}

inline HRESULT AtlGetSAXValue<float>(float *pVal, const wchar_t *wsz, int cch) throw()
{
	ATLASSERT( wsz != NULL );

	if (!pVal)
	{
		return E_POINTER;
	}

	double d = *pVal;
	if (SUCCEEDED(AtlGetSAXValue(&d, wsz, cch)))
	{
		*pVal = (float) d;
		return S_OK;
	}

	return E_FAIL;
}

inline HRESULT AtlGetSAXValue<BSTR>(BSTR *pVal, const wchar_t *wsz, int cch) throw()
{
	ATLASSERT( wsz != NULL );

	if (pVal == NULL)
	{
		return E_POINTER;
	}

	*pVal = SysAllocStringLen(wsz, cch);

	return ((*pVal != NULL) ? S_OK : E_OUTOFMEMORY);
}

////////////////////////////////////////////////////////////////////////////////
//
// AtlGenXMLValue template and specializations
//
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline HRESULT AtlGenXMLValue(IWriteStream *pStream, T *pVal) throw()
{
	if ((pStream == NULL) || (pVal == NULL))
	{
		return E_INVALIDARG;
	}

	//
	// delegate to CWriteStreamHelper
	//
	CWriteStreamHelper s(pStream);

	return (s.Write(*pVal) == TRUE ? S_OK : E_FAIL);
}

#ifdef _NATIVE_WCHAR_T_DEFINED
inline HRESULT AtlGenXMLValue<wchar_t>(IWriteStream *pStream, wchar_t *pVal) throw()
{
	return AtlGenXMLValue(pStream, (unsigned short *)pVal);
}
#endif

//
// REVIEW: overload on other kinds of string (LPCSTR, etc.)?
//
inline HRESULT AtlGenXMLValue<wchar_t *>(IWriteStream *pStream, wchar_t **pVal) throw()
{
	if ((pStream == NULL) || (*pVal == NULL))
	{
		return E_INVALIDARG;
	}

	wchar_t *wszWrite = *pVal;
	int nSrcLen = (int)wcslen(*pVal);
	int nCnt = EscapeXML(*pVal, nSrcLen, NULL, 0);
	if (nCnt > nSrcLen)
	{
		nCnt++;
		wszWrite = (wchar_t *)malloc((nCnt)*sizeof(wchar_t));
		if (wszWrite == NULL)
		{
			return E_OUTOFMEMORY;
		}
		
		nCnt = EscapeXML(*pVal, nSrcLen, wszWrite, nCnt);
		if (nCnt == 0)
		{
			free(wszWrite);
			return E_FAIL;
		}
		wszWrite[nCnt] = L'\0';
		nSrcLen = nCnt;
	}

	nCnt = AtlUnicodeToUTF8(wszWrite, nSrcLen, NULL, 0);
	HRESULT hr = E_FAIL;
	if ((nCnt == 0) || (nCnt == nSrcLen))
	{
		CWriteStreamHelper s(pStream);

		hr = (s.Write(wszWrite) == TRUE ? S_OK : E_FAIL);
	}
	else
	{
		nCnt++;
		CHeapPtr<char> szWrite;
		szWrite.AllocateBytes((size_t)(nCnt));
		if (szWrite != NULL)
		{
			nCnt = AtlUnicodeToUTF8(wszWrite, nSrcLen, szWrite, nCnt);
			if (nCnt != 0)
			{
				hr = pStream->WriteStream(szWrite, nCnt, NULL);
			}
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if (wszWrite != *pVal)
	{
		free(wszWrite);
	}

	return hr;
}

inline HRESULT AtlGenXMLValue<double>(IWriteStream *pStream, double *pVal) throw()
{
	if ((pStream == NULL) || (pVal == NULL))
	{
		return E_INVALIDARG;
	}

	HRESULT hr;
	switch (_fpclass(*pVal))
	{
		case _FPCLASS_SNAN: 
		case _FPCLASS_QNAN:
		{
			hr = pStream->WriteStream("NAN", 3, NULL);
			break;
		}
		case _FPCLASS_NINF:
		{
			hr = pStream->WriteStream("-INF", 4, NULL);
			break;
		}
		case _FPCLASS_PINF:
		{
			hr = pStream->WriteStream("INF", 3, NULL);
			break;
		}
		case _FPCLASS_NZ:
		{
			hr = pStream->WriteStream("-0", 2, NULL);
			break;
		}
		default:
		{
			CWriteStreamHelper s(pStream);
			hr = (s.Write(*pVal) == TRUE ? S_OK : E_FAIL);
			break;
		}
	}

	return hr;
}

inline HRESULT AtlGenXMLValue<float>(IWriteStream *pStream, float *pVal) throw()
{
	if ((pStream == NULL) || (pVal == NULL))
	{
		return E_INVALIDARG;
	}

	double d = *pVal;

	return AtlGenXMLValue(pStream, &d);
}

inline HRESULT AtlGenXMLValue<bool>(IWriteStream *pStream, bool *pVal) throw()
{
	if ((pStream == NULL) || (pVal == NULL))
	{
		return E_INVALIDARG;
	}

	if (*pVal == true)
	{
		return pStream->WriteStream("true", 4, NULL);
	}

	return pStream->WriteStream("false", 5, NULL);
}

inline HRESULT AtlGenXMLValue<ATLSOAP_BLOB>(IWriteStream *pStream, ATLSOAP_BLOB *pVal) throw()
{
	if ((pStream == NULL) || (pVal == NULL))
	{
		return E_INVALIDARG;
	}

	HRESULT hr = pStream->WriteStream("<size>", sizeof("<size>")-1, NULL);
	if (SUCCEEDED(hr))
	{
		hr = AtlGenXMLValue(pStream, &pVal->size);
		if (SUCCEEDED(hr))
		{
			hr = pStream->WriteStream("</size>", sizeof("</size>")-1, NULL);
			if (SUCCEEDED(hr))
			{
				hr = E_FAIL;
				int nLength = Base64EncodeGetRequiredLength(pVal->size, ATL_BASE64_FLAG_NOCRLF);
				
				char *pEnc = (char *) malloc(nLength*sizeof(char));
				if (pEnc != NULL)
				{
					if (Base64Encode(pVal->data, pVal->size, pEnc, &nLength, ATL_BASE64_FLAG_NOCRLF))
					{
						hr = pStream->WriteStream("<data>", sizeof("<data>")-1, NULL);
						if (SUCCEEDED(hr))
						{
							hr = pStream->WriteStream(pEnc, nLength, NULL);
							if (SUCCEEDED(hr))
							{
								hr = pStream->WriteStream("</data>", sizeof("</data>")-1, NULL);
							}
						}
					}

					free(pEnc);
				}
			}
		}
	}

	return hr;
}

template <typename T>
inline HRESULT AtlCleanupValue(T * /*pVal*/) throw()
{
	return S_OK;
}

inline HRESULT AtlCleanupValue<ATLSOAP_BLOB>(ATLSOAP_BLOB *pVal) throw()
{
	if (pVal == NULL)
	{
		return E_INVALIDARG;
	}

	if (pVal->data != NULL)
	{
		free(pVal->data);
		pVal->data = NULL;
	}

	return S_OK;
}

inline HRESULT AtlCleanupValue<BSTR>(BSTR *pVal) throw()
{
	if (pVal == NULL)
	{
		// should never happen
		ATLASSERT( FALSE );
		return E_INVALIDARG;
	}

	if ((*pVal) != NULL)
	{
		// null strings are okay
		SysFreeString(*pVal);
	}

	return S_OK;
}

// single dimensional arrays
template <typename T>
inline HRESULT AtlCleanupArray(T *pArray, const int nCnt) throw()
{
	if (pArray == NULL)
	{
		return E_INVALIDARG;
	}

	for (int i=0; i<nCnt; i++)
	{
		AtlCleanupValue(&pArray[i]);
	}

	return S_OK;
}

// multi-dimensional arrays
template <typename T>
inline HRESULT AtlCleanupArrayMD(T *pArray, const int *pDims) throw()
{
	if ((pArray == NULL) || (pDims == NULL))
	{
		return E_INVALIDARG;
	}

	// calculate size
	int nCnt = 1;
	for (int i=1; i<=pDims[0]; i++)
	{
		nCnt*= pDims[i];
	}

	return AtlCleanupArray(pArray, nCnt);
}

namespace ATL
{

////////////////////////////////////////////////////////////////////////////////
//
// CSAXSoapErrorHandler
//
////////////////////////////////////////////////////////////////////////////////

class CSAXSoapErrorHandler : public ISAXErrorHandler
{
private:
	
	CStringW m_strParseError;

public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (!ppv)
		{
			return E_POINTER;
		}

		if (InlineIsEqualGUID(riid, __uuidof(ISAXErrorHandler)) ||
			InlineIsEqualGUID(riid, __uuidof(IUnknown)))
		{
			*ppv = static_cast<ISAXErrorHandler*>(this);
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

	const CStringW& GetParseError() throw()
	{
		return m_strParseError;
	}

	HRESULT __stdcall error( 
		ISAXLocator *pLocator,
		const wchar_t *wszErrorMessage,
		HRESULT hrErrorCode) throw()
	{
		pLocator;
		wszErrorMessage;
		hrErrorCode;

		ATLTRACE( _T("%ws\n"), wszErrorMessage );
		_ATLTRY
		{
			m_strParseError = wszErrorMessage;
		}
		_ATLCATCHALL()
		{
			return E_FAIL;
		}

		return hrErrorCode;
	}

	HRESULT __stdcall fatalError(
		ISAXLocator  *pLocator,
		const wchar_t *wszErrorMessage,
		HRESULT hrErrorCode) throw()
	{
		pLocator;
		wszErrorMessage;
		hrErrorCode;

		ATLTRACE( _T("%ws\n"), wszErrorMessage );
		_ATLTRY
		{
			m_strParseError = wszErrorMessage;
		}
		_ATLCATCHALL()
		{
			return E_FAIL;
		}

		return hrErrorCode;
	}

	HRESULT __stdcall ignorableWarning(
		ISAXLocator  *pLocator,
		const wchar_t *wszErrorMessage,
		HRESULT hrErrorCode) throw()
	{
		pLocator;
		wszErrorMessage;
		hrErrorCode;

		ATLTRACE( _T("%ws\n"), wszErrorMessage );

		return hrErrorCode;
	}
};

////////////////////////////////////////////////////////////////////////////////
//
// ISAXContentHandlerImpl
//
////////////////////////////////////////////////////////////////////////////////

class ISAXContentHandlerImpl : 
	public ISAXContentHandler
{
public:

	//
	// ISAXContentHandler interface
	//

	HRESULT __stdcall putDocumentLocator(ISAXLocator  * /*pLocator*/) throw()
	{
		return S_OK;
	}

	HRESULT __stdcall startDocument() throw()
	{
		return S_OK;
	}

	HRESULT __stdcall endDocument() throw()
	{
		return S_OK;
	}

	HRESULT __stdcall startPrefixMapping(
	     const wchar_t  * /*wszPrefix*/,
	     int /*cchPrefix*/,
	     const wchar_t  * /*wszUri*/,
	     int /*cchUri*/) throw()
	{
		return S_OK;
	}

	HRESULT __stdcall endPrefixMapping( 
	     const wchar_t  * /*wszPrefix*/,
	     int /*cchPrefix*/) throw()
	{
		return S_OK;
	}

	HRESULT __stdcall startElement( 
	     const wchar_t  * /*wszNamespaceUri*/,
	     int /*cchNamespaceUri*/,
	     const wchar_t  * /*wszLocalName*/,
	     int /*cchLocalName*/,
	     const wchar_t  * /*wszQName*/,
	     int /*cchQName*/,
	     ISAXAttributes  * /*pAttributes*/) throw()
	{
		return S_OK;
	}

	HRESULT __stdcall endElement( 
	     const wchar_t  * /*wszNamespaceUri*/,
	     int /*cchNamespaceUri*/,
	     const wchar_t  * /*wszLocalName*/,
	     int /*cchLocalName*/,
	     const wchar_t  * /*wszQName*/,
	     int /*cchQName*/) throw()
	{
		return S_OK;
	}

	HRESULT __stdcall characters( 
	     const wchar_t  * /*wszChars*/,
	     int /*cchChars*/) throw()
	{
		return S_OK;
	}

	HRESULT __stdcall ignorableWhitespace( 
	     const wchar_t  * /*wszChars*/,
	     int /*cchChars*/) throw()
	{
		return S_OK;
	}

	HRESULT __stdcall processingInstruction( 
	     const wchar_t  * /*wszTarget*/,
	     int /*cchTarget*/,
	     const wchar_t  * /*wszData*/,
	     int /*cchData*/) throw()
	{
		return S_OK;
	}

	HRESULT __stdcall skippedEntity( 
	     const wchar_t  * /*wszName*/,
	     int /*cchName*/) throw()
	{
		return S_OK;
	}
}; // class ISAXContentHandlerImpl

////////////////////////////////////////////////////////////////////////////////
//
// SAX skip element handler utility class
// (skip an element and all its child elements)
//
////////////////////////////////////////////////////////////////////////////////

class CSkipHandler : public ISAXContentHandlerImpl
{
public:
	
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

private:

	DWORD m_dwReset;
	CComPtr<ISAXXMLReader> m_spReader;
	CComPtr<ISAXContentHandler> m_spParent;

	DWORD DisableReset(DWORD dwCnt = 1) throw()
	{
		m_dwReset += dwCnt;

		return m_dwReset;
	}

	DWORD EnableReset() throw()
	{
		if (m_dwReset > 0)
		{
			--m_dwReset;
		}

		return m_dwReset;
	}

public:

	CSkipHandler(ISAXContentHandler *pParent = NULL, ISAXXMLReader *pReader = NULL) throw()
		: m_spParent(pParent), m_spReader(pReader), m_dwReset(1)
	{
	}

	void SetParent(ISAXContentHandler *pParent) throw()
	{
		m_spParent = pParent;
	}

	void SetReader(ISAXXMLReader *pReader) throw()
	{
		m_spReader = pReader;
	}

	HRESULT __stdcall startElement( 
	     const wchar_t  * /*wszNamespaceUri*/,
	     int /*cchNamespaceUri*/,
	     const wchar_t  * /*wszLocalName*/,
	     int /*cchLocalName*/,
	     const wchar_t  * /*wszQName*/,
	     int /*cchQName*/,
	     ISAXAttributes  * /*pAttributes*/) throw()
	{
		DisableReset();
		return S_OK;
	}

	HRESULT __stdcall endElement( 
	     const wchar_t  * /*wszNamespaceUri*/,
	     int /*cchNamespaceUri*/,
	     const wchar_t  * /*wszLocalName*/,
	     int /*cchLocalName*/,
	     const wchar_t  * /*wszQName*/,
	     int /*cchQName*/) throw()
	{
		if (EnableReset() == 0)
		{
			m_spReader->putContentHandler(m_spParent);
		}

		return S_OK;
	}
}; // class CSkipHandler


////////////////////////////////////////////////////////////////////////////////
//
// SAX ATLSOAP_BLOB handler class
//
////////////////////////////////////////////////////////////////////////////////

class CBlobHandler : public ISAXContentHandlerImpl
{
public:
	
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

private:

	const static DWORD STATE_START             = 0;
	const static DWORD STATE_GOTSIZE           = 1;
	const static DWORD STATE_PARSESIZE         = 2;
	const static DWORD STATE_GOTDATA_PARSESIZE = 3;
	const static DWORD STATE_GOTDATA           = 4;
	const static DWORD STATE_PARSEDATA         = 5;
	const static DWORD STATE_GOTSIZE_PARSEDATA = 6;
	const static DWORD STATE_GOTALL            = 7;

	ATLSOAP_BLOB *m_pBlob;
	ISAXContentHandler *m_pParent;
	ISAXXMLReader *m_pReader;

	DWORD m_dwReset;
	DWORD m_dwState;

	DWORD DisableReset(DWORD dwCnt = 1) throw()
	{
		m_dwReset += dwCnt;

		return m_dwReset;
	}

	DWORD EnableReset() throw()
	{
		if (m_dwReset > 0)
		{
			--m_dwReset;
		}

		return m_dwReset;
	}

public:

	CBlobHandler(ATLSOAP_BLOB *pBlob = NULL, ISAXContentHandler *pParent = NULL, 
		ISAXXMLReader *pReader = NULL) throw()
		: m_pBlob(pBlob), m_pParent(pParent), 
		  m_pReader(pReader), m_dwReset(0), m_dwState(STATE_START)
	{
	}

	void SetParent(ISAXContentHandler *pParent) throw()
	{
		m_pParent = pParent;
	}

	void SetReader(ISAXXMLReader *pReader) throw()
	{
		m_pReader = pReader;
	}

	void SetBlob(ATLSOAP_BLOB *pBlob) throw()
	{
		m_pBlob = pBlob;
	}

	ATLSOAP_BLOB * GetBlob() throw()
	{
		return m_pBlob;
	}

	void Clear() throw()
	{
		m_pBlob = NULL;
		m_pParent = NULL;
		m_pReader = NULL;
		m_dwReset = 0;
		m_dwState = STATE_START;
	}

	HRESULT __stdcall startElement( 
	     const wchar_t  * /*wszNamespaceUri*/,
	     int /*cchNamespaceUri*/,
	     const wchar_t  * wszLocalName,
	     int cchLocalName,
	     const wchar_t  * /*wszQName*/,
	     int /*cchQName*/,
	     ISAXAttributes  * /*pAttributes*/) throw()
	{
		DisableReset();

		HRESULT hr = S_OK;

		if (cchLocalName==sizeof("size")-1 && !wcsncmp(wszLocalName, L"size", cchLocalName))
		{
			switch (m_dwState)
			{
				case STATE_GOTDATA :
				{
					m_dwState = STATE_GOTDATA_PARSESIZE;
					break;
				}
				case STATE_START :
				{
					DisableReset();
					m_dwState = STATE_PARSESIZE;
					break;
				}
				default:
				{
					hr = E_FAIL;
				}
			}
		}
		else if (cchLocalName==sizeof("data")-1 && !wcsncmp(wszLocalName, L"data", cchLocalName))
		{
			switch (m_dwState)
			{
				case STATE_GOTSIZE :
				{
					m_dwState = STATE_GOTSIZE_PARSEDATA;
					break;
				}
				case STATE_START :
				{
					DisableReset();
					m_dwState = STATE_PARSEDATA;
					break;
				}
				default:
				{
					hr = E_FAIL;
				}
			}
		}
		else
		{
			hr = E_FAIL;
		}

		return hr;
	}

	HRESULT __stdcall endElement( 
	     const wchar_t  * /*wszNamespaceUri*/,
	     int /*cchNamespaceUri*/,
	     const wchar_t  * /*wszLocalName*/,
	     int /*cchLocalName*/,
	     const wchar_t  * /*wszQName*/,
	     int /*cchQName*/) throw()
	{
		if (EnableReset() == 0)
		{
			m_dwState = STATE_START;
			m_pReader->putContentHandler(m_pParent);
		}

		return S_OK;
	}

	HRESULT __stdcall characters( 
	     const wchar_t  *wszChars,
	     int cchChars) throw()
	{
		HRESULT hr = S_OK;
		if (m_dwState == STATE_PARSESIZE || m_dwState == STATE_GOTDATA_PARSESIZE)
		{
			hr = E_FAIL;
			//
			// get the size
			//

			ATLASSERT( m_pBlob != NULL );

			hr = AtlGetSAXValue(&m_pBlob->size, wszChars, cchChars);

			if (m_dwState == STATE_PARSESIZE)
			{
				m_dwState = STATE_GOTSIZE;
			}
			else
			{
				m_dwState = STATE_GOTALL;
			}
		}
		else if (m_dwState == STATE_GOTSIZE_PARSEDATA || m_dwState == STATE_PARSEDATA)
		{
			hr = E_FAIL;
			//
			// get the data
			//

			int nLength = WideCharToMultiByte(CP_ACP, 0, wszChars, cchChars, NULL, 0, NULL, NULL);

			if (nLength != 0)
			{
				char * pSrc = (char *) malloc(nLength*sizeof(char));
				if (pSrc != NULL)
				{
					nLength = WideCharToMultiByte(CP_ACP, 0, wszChars, cchChars, pSrc, nLength, NULL, NULL);
					if (nLength != 0)
					{
						ATLASSERT( m_pBlob != NULL );

						m_pBlob->data = (unsigned char *)malloc(nLength*sizeof(unsigned char));
						if (m_pBlob->data != NULL)
						{
							int nDataLength = nLength;
							if (Base64Decode(pSrc, nLength, m_pBlob->data, &nDataLength))
							{
								hr = S_OK;
							}
						}
					}

					free(pSrc);
				}
			}

			if (hr != S_OK)
			{
				if (m_pBlob->data != NULL)
				{
					free(m_pBlob->data);
					m_pBlob->data = NULL;
				}
			}

			if (m_dwState == STATE_PARSEDATA)
			{
				m_dwState = STATE_GOTDATA;
			}
			else
			{
				m_dwState = STATE_GOTALL;
			}
		}
		
		return hr;
	}
}; // class CBlobHandler


////////////////////////////////////////////////////////////////////////////////
//
// SAX string builder class
//
////////////////////////////////////////////////////////////////////////////////

class CSAXStringBuilder : public ISAXContentHandlerImpl
{
public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

private:

	ISAXContentHandler * m_pParent;
	ISAXXMLReader * m_pReader;
	DWORD m_dwReset;
	CStringW m_str;

	DWORD DisableReset(DWORD dwReset = 1) throw()
	{
		m_dwReset+= dwReset;

		return m_dwReset;
	}

	DWORD EnableReset() throw()
	{
		if (m_dwReset > 0)
		{
			--m_dwReset;
		}

		return m_dwReset;
	}

public:

	CSAXStringBuilder(ISAXXMLReader *pReader = NULL, ISAXContentHandler *pParent = NULL) throw()
		:m_pReader(pReader), m_pParent(pParent), m_dwReset(0)
	{
	}

	void SetReader(ISAXXMLReader *pReader) throw()
	{
		m_pReader = pReader;
	}

	void SetParent(ISAXContentHandler *pParent) throw()
	{
		m_pParent = pParent;
	}

	const CStringW& GetString() throw()
	{
		return m_str;
	}

	void Clear() throw()
	{
		m_str.Empty();
	}

	HRESULT __stdcall startElement( 
	     const wchar_t  * /*wszNamespaceUri*/,
	     int /*cchNamespaceUri*/,
	     const wchar_t  * /*wszLocalName*/,
	     int /*cchLocalName*/,
	     const wchar_t  *wszQName,
	     int cchQName,
	     ISAXAttributes  *pAttributes) throw()
	{
		DisableReset();

		int nAttrs = 0;
		HRESULT hr = pAttributes->getLength(&nAttrs);

		_ATLTRY
		{
			if (SUCCEEDED(hr))
			{
				m_str.Append(L"<", 1);
				m_str.Append(wszQName, cchQName);

				const wchar_t *wszAttrNamespaceUri = NULL;
				const wchar_t *wszAttrLocalName = NULL;
				const wchar_t *wszAttrQName = NULL;
				const wchar_t *wszAttrValue = NULL;
				int cchAttrUri = 0;
				int cchAttrLocalName = 0;
				int cchAttrQName = 0;
				int cchAttrValue = 0;

				for (int i=0; i<nAttrs; i++)
				{
					hr = pAttributes->getName(i, &wszAttrNamespaceUri, &cchAttrUri, 
						&wszAttrLocalName, &cchAttrLocalName, &wszAttrQName, &cchAttrQName);

					if (FAILED(hr))
					{
						break;
					}

					m_str.Append(L" ", 1);
					m_str.Append(wszAttrQName, cchAttrQName);

					hr = pAttributes->getValue(i, &wszAttrValue, &cchAttrValue);
					
					if (FAILED(hr))
					{
						break;
					}

					if (cchAttrValue != 0)
					{
						m_str.Append(L"=\"", 1);
						m_str.Append(wszAttrValue, cchAttrValue);
						m_str.Append(L"\"", 1);
					}
				}

				if (SUCCEEDED(hr))
				{
					m_str.Append(L">", 1);
				}
			}
		}
		_ATLCATCHALL()
		{
			hr = E_OUTOFMEMORY;
		}

		return hr;
	}

	HRESULT __stdcall endElement( 
	     const wchar_t  * wszNamespaceUri,
	     int cchNamespaceUri,
	     const wchar_t  * wszLocalName,
	     int cchLocalName,
	     const wchar_t  *wszQName,
	     int cchQName) throw()
	{
		HRESULT hr = S_OK;
		_ATLTRY
		{
			if (m_dwReset > 0)
			{
				m_str.Append(L"</", 2);
				m_str.Append(wszQName, cchQName);
				m_str.Append(L">", 1);
			}

			if (EnableReset() == 0)
			{
				hr = m_pParent->characters((LPCWSTR) m_str, m_str.GetLength());
				if (SUCCEEDED(hr))
				{
					hr = m_pParent->endElement(wszNamespaceUri, cchNamespaceUri,
							wszLocalName, cchLocalName, wszQName, cchQName);
				}

				m_pReader->putContentHandler(m_pParent);
			}
		}
		_ATLCATCHALL()
		{
			hr = E_OUTOFMEMORY;
		}

		return hr;
	}

	HRESULT __stdcall characters(
	     const wchar_t  *wszChars,
	     int cchChars) throw()
	{
		_ATLTRY
		{
			m_str.Append(wszChars, cchChars);
		}
		_ATLCATCHALL()
		{
			return E_OUTOFMEMORY;
		}

		return S_OK;
	}

	HRESULT __stdcall ignorableWhitespace( 
	     const wchar_t  *wszChars,
	     int cchChars) throw()
	{
		_ATLTRY
		{
			m_str.Append(wszChars, cchChars);
		}
		_ATLCATCHALL()
		{
			return E_OUTOFMEMORY;
		}
		
		return S_OK;
	}
}; // class CSAXStringBuilder

} // namespace ATL

////////////////////////////////////////////////////////////////////////////////
//
// SOAP data structure definitions
//
////////////////////////////////////////////////////////////////////////////////

//
// *****************************  WARNING  *****************************
// THESE STRUCTURES ARE INTERNAL ONLY, FOR USE WITH THE ATL SERVER SOAP 
// ATTRIBUTES. USERS SHOULD NOT USE THESE TYPES DIRECTLY. ABSOLUTELY NO 
// GUARANTEES ARE MADE ABOUT BACKWARD COMPATIBILITY FOR DIRECT USE OF 
// THESE TYPES.
//

////////////////////////////////////////////////////////////////////////////////
//
// BEGIN PRIVATE DEFINITIONS
//
////////////////////////////////////////////////////////////////////////////////

enum SOAPFLAGS
{
	SOAPFLAG_NONE           = 0x00000000,
	SOAPFLAG_IN             = 0x00000001,
	SOAPFLAG_OUT            = 0x00000002,
	SOAPFLAG_RETVAL         = 0x00000004,
	SOAPFLAG_DYNARR         = 0x00000008,
	SOAPFLAG_FIXEDARR       = 0x00000010,
	SOAPFLAG_MUSTUNDERSTAND = 0x00000020,
	SOAPFLAG_HEX            = 0x00000040,
	SOAPFLAG_BASE64         = 0x00000080,
	SOAPFLAG_UNKSIZE        = 0x00000100,
	SOAPFLAG_READYSTATE     = 0x00000200,
	SOAPFLAG_FIELD          = 0x00000400,
	SOAPFLAG_NOMARSHAL      = 0x00000800,

	SOAPFLAG_DOCUMENT       = 0x00001000,
	SOAPFLAG_RPC            = 0x00002000,
	SOAPFLAG_LITERAL        = 0x00004000,
	SOAPFLAG_ENCODED        = 0x00008000,
	SOAPFLAG_PID            = 0x00010000,
	SOAPFLAG_PAD            = 0x00020000,
	SOAPFLAG_CHAIN          = 0x00040000,
	SOAPFLAG_DISABLEPOP     = 0x00080000
};

enum SOAPMAPTYPE
{
	SOAPMAP_ERR = 0,
	SOAPMAP_ENUM,
	SOAPMAP_FUNC,
	SOAPMAP_STRUCT,
	SOAPMAP_UNION,
	SOAPMAP_HEADER,
	SOAPMAP_PARAM
};

struct _soapmap;

struct _soapmapentry
{
	ULONG nHash;
	const char * szField;
	const WCHAR * wszField;
	int cchField;
	int nVal;
	DWORD dwFlags;

	size_t nOffset;
	const int * pDims;
	
	const _soapmap * pChain;

	int nSizeIs;
};

struct _soapmap
{
	ULONG nHash;
	const char * szName;
	const wchar_t * wszName;
	int cchName;
	int cchWName;
	SOAPMAPTYPE mapType;
	const _soapmapentry * pEntries;
	size_t nElementSize;
	size_t nElements;
	int nRetvalIndex;

	DWORD dwCallFlags;
};

enum SOAPTYPES
{
	SOAPTYPE_ERR = -2,
	SOAPTYPE_UNK = -1,
	SOAPTYPE_STRING = 0,
	SOAPTYPE_BOOLEAN,
	SOAPTYPE_FLOAT,
	SOAPTYPE_DOUBLE,
	SOAPTYPE_DECIMAL,
	SOAPTYPE_TIMEDURATION,
	SOAPTYPE_RECURRINGDURATION,
	SOAPTYPE_BINARY,
	SOAPTYPE_URIREFERENCE,
	SOAPTYPE_ID,
	SOAPTYPE_IDREF,
	SOAPTYPE_ENTITY,
	SOAPTYPE_NOTATION,
	SOAPTYPE_QNAME,
	SOAPTYPE_CDATA,
	SOAPTYPE_TOKEN,
	SOAPTYPE_LANGUAGE,
	SOAPTYPE_IDREFS,
	SOAPTYPE_ENTITIES,
	SOAPTYPE_NMTOKEN,
	SOAPTYPE_NMTOKENS,
	SOAPTYPE_NAME,
	SOAPTYPE_NCNAME,
	SOAPTYPE_INTEGER,
	SOAPTYPE_NONPOSITIVEINTEGER,
	SOAPTYPE_NEGATIVEINTEGER,
	SOAPTYPE_LONG,
	SOAPTYPE_INT,
	SOAPTYPE_SHORT,
	SOAPTYPE_BYTE,
	SOAPTYPE_NONNEGATIVEINTEGER,
	SOAPTYPE_UNSIGNEDLONG,
	SOAPTYPE_UNSIGNEDINT,
	SOAPTYPE_UNSIGNEDSHORT,
	SOAPTYPE_UNSIGNEDBYTE,
	SOAPTYPE_POSITIVEINTEGER,
	SOAPTYPE_TIMEINSTANT,
	SOAPTYPE_TIME,
	SOAPTYPE_TIMEPERIOD,
	SOAPTYPE_DATE,
	SOAPTYPE_MONTH,
	SOAPTYPE_YEAR,
	SOAPTYPE_CENTURY,
	SOAPTYPE_RECURRINGDATE,
	SOAPTYPE_RECURRINGDAY,

	//
	// for ATLSOAP_BLOB
	//
	SOAPTYPE_BLOB,

	SOAPTYPE_USERBASE
};

inline ULONG AtlSoapHashStr(const char * sz) throw()
{
	ULONG nHash = 0;
	while (*sz != 0)
	{
		nHash = (nHash<<5)+nHash+(*sz);
		sz++;
	}

	return nHash;
}

inline ULONG AtlSoapHashStr(const wchar_t * sz) throw()
{
	ULONG nHash = 0;
	while (*sz != 0)
	{
		nHash = (nHash<<5)+nHash+(*sz);
		sz++;
	}

	return nHash;
}

inline ULONG AtlSoapHashStr(const char * sz, int cch) throw()
{
	ULONG nHash = 0;
	for (int i=0; i<cch; i++)
	{
		nHash = (nHash<<5)+nHash+(*sz);
		sz++;
	}

	return nHash;
}

inline ULONG AtlSoapHashStr(const wchar_t * sz, int cch) throw()
{
	ULONG nHash = 0;
	for (int i=0; i<cch; i++)
	{
		nHash = (nHash<<5)+nHash+(*sz);
		sz++;
	}

	return nHash;
}

inline size_t AtlSoapGetElementSize(SOAPTYPES type) throw()
{
	size_t nRet;
	switch (type)
	{
		case SOAPTYPE_BOOLEAN:
			nRet = sizeof(bool);
			break;
		case SOAPTYPE_FLOAT:
			nRet = sizeof(float);
			break;
		case SOAPTYPE_DOUBLE: 
		case SOAPTYPE_DECIMAL:
			nRet = sizeof(double);
			break;
		case SOAPTYPE_BINARY:
			nRet = sizeof(BYTE);
			break;
		case SOAPTYPE_INTEGER: 
		case SOAPTYPE_NONPOSITIVEINTEGER:
		case SOAPTYPE_NEGATIVEINTEGER:
		case SOAPTYPE_LONG:
			nRet = sizeof(__int64);
			break;
		case SOAPTYPE_INT:
			nRet = sizeof(int);
			break;
		case SOAPTYPE_SHORT:
			nRet = sizeof(short);
			break;
		case SOAPTYPE_BYTE:
			nRet = sizeof(char);
			break;
		case SOAPTYPE_NONNEGATIVEINTEGER:
		case SOAPTYPE_UNSIGNEDLONG:
			nRet = sizeof(unsigned __int64);
			break;
		case SOAPTYPE_UNSIGNEDINT:
			nRet = sizeof(unsigned int);
			break;
		case SOAPTYPE_UNSIGNEDSHORT:
			nRet = sizeof(unsigned short);
			break;
		case SOAPTYPE_UNSIGNEDBYTE:
			nRet = sizeof(unsigned char);
			break;
		case SOAPTYPE_BLOB:
			nRet = sizeof(ATLSOAP_BLOB);
			break;
		default:
			if ((type != SOAPTYPE_ERR) && (type != SOAPTYPE_UNK) && (type != SOAPTYPE_USERBASE))
			{
				// treat as string
				nRet = sizeof(BSTR);
			}
			else
			{
				// should never get here
				ATLASSERT( FALSE );
				nRet = 0;
			}
			break;
	}

	return nRet;
}

inline HRESULT AtlSoapGetElementValue(const wchar_t *wsz, int cch, 
	void *pVal, SOAPTYPES type) throw()
{
	HRESULT hr = E_FAIL;

	switch (type)
	{
		case SOAPTYPE_BOOLEAN:
			hr = AtlGetSAXValue((bool *)pVal, wsz, cch);
			break;
		case SOAPTYPE_FLOAT:
			hr = AtlGetSAXValue((float *)pVal, wsz, cch);
			break;
		case SOAPTYPE_DOUBLE: 
		case SOAPTYPE_DECIMAL:
			hr = AtlGetSAXValue((double *)pVal, wsz, cch);
			break;
		case SOAPTYPE_BINARY:
			ATLASSERT( FALSE );
			//
			// TODO: implement
			//
			break;
		case SOAPTYPE_INTEGER: 
		case SOAPTYPE_NONPOSITIVEINTEGER:
		case SOAPTYPE_NEGATIVEINTEGER:
		case SOAPTYPE_LONG:
			hr = AtlGetSAXValue((__int64 *)pVal, wsz, cch);
			break;
		case SOAPTYPE_INT:
			hr = AtlGetSAXValue((int *)pVal, wsz, cch);
			break;
		case SOAPTYPE_SHORT:
			hr = AtlGetSAXValue((short *)pVal, wsz, cch);
			break;
		case SOAPTYPE_BYTE:
			hr = AtlGetSAXValue((char *)pVal, wsz, cch);
			break;
		case SOAPTYPE_NONNEGATIVEINTEGER:
		case SOAPTYPE_UNSIGNEDLONG:
			hr = AtlGetSAXValue((unsigned __int64 *)pVal, wsz, cch);
			break;
		case SOAPTYPE_UNSIGNEDINT:
			hr = AtlGetSAXValue((unsigned int *)pVal, wsz, cch);
			break;
		case SOAPTYPE_UNSIGNEDSHORT:
			hr = AtlGetSAXValue((unsigned short *)pVal, wsz, cch);
			break;
		case SOAPTYPE_UNSIGNEDBYTE:
			hr = AtlGetSAXValue((unsigned char *)pVal, wsz, cch);
			break;
		case SOAPTYPE_BLOB:
			// should never get here
			ATLASSERT( FALSE );
			break;
		default:
			if ((type != SOAPTYPE_ERR) && (type != SOAPTYPE_UNK) && (type != SOAPTYPE_USERBASE))
			{
				hr = AtlGetSAXValue((BSTR *)pVal, wsz, cch);
			}
#ifdef _DEBUG
			else
			{
				// should never get here
				ATLASSERT( FALSE );
			}
#endif
			break;
	}

	return hr;
}

inline HRESULT AtlSoapGenElementValue(void *pVal, IWriteStream *pStream, SOAPTYPES type) throw()
{
	HRESULT hr = E_FAIL;

	switch (type)
	{
		case SOAPTYPE_BOOLEAN:
			hr = AtlGenXMLValue(pStream, (bool *)pVal);
			break;
		case SOAPTYPE_FLOAT:
			hr = AtlGenXMLValue(pStream, (float *)pVal);
			break;
		case SOAPTYPE_DOUBLE: 
		case SOAPTYPE_DECIMAL:
			hr = AtlGenXMLValue(pStream, (double *)pVal);
			break;
		case SOAPTYPE_BINARY:
			ATLASSERT( FALSE );
			//
			// TODO: implement
			//
			break;
		case SOAPTYPE_INTEGER: 
		case SOAPTYPE_NONPOSITIVEINTEGER:
		case SOAPTYPE_NEGATIVEINTEGER:
		case SOAPTYPE_LONG:
			hr = AtlGenXMLValue(pStream, (__int64 *)pVal);
			break;
		case SOAPTYPE_INT:
			hr = AtlGenXMLValue(pStream, (int *)pVal);
			break;
		case SOAPTYPE_SHORT:
			hr = AtlGenXMLValue(pStream, (short *)pVal);
			break;
		case SOAPTYPE_BYTE:
			hr = AtlGenXMLValue(pStream, (char *)pVal);
			break;
		case SOAPTYPE_NONNEGATIVEINTEGER:
		case SOAPTYPE_UNSIGNEDLONG:
			hr = AtlGenXMLValue(pStream, (unsigned __int64 *)pVal);
			break;
		case SOAPTYPE_UNSIGNEDINT:
			hr = AtlGenXMLValue(pStream, (unsigned int *)pVal);
			break;
		case SOAPTYPE_UNSIGNEDSHORT:
			hr = AtlGenXMLValue(pStream, (unsigned short *)pVal);
			break;
		case SOAPTYPE_UNSIGNEDBYTE:
			hr = AtlGenXMLValue(pStream, (unsigned char *)pVal);
			break;
		case SOAPTYPE_BLOB:
			hr = AtlGenXMLValue(pStream, (ATLSOAP_BLOB *)pVal);
			break;
		default:
			if ((type != SOAPTYPE_ERR) && (type != SOAPTYPE_UNK) && (type != SOAPTYPE_USERBASE))
			{
				hr = AtlGenXMLValue(pStream, (BSTR *)pVal);
			}
#ifdef _DEBUG
			else
			{
				// should never get here
				ATLASSERT( FALSE );
			}
#endif
			break;
	}

	return hr;
}

inline HRESULT AtlSoapCleanupElement(void *pVal, SOAPTYPES type) throw()
{
	HRESULT hr = S_OK;

	switch (type)
	{
		case SOAPTYPE_BOOLEAN:
		case SOAPTYPE_FLOAT:
		case SOAPTYPE_DOUBLE: 
		case SOAPTYPE_DECIMAL:
		case SOAPTYPE_INT:
		case SOAPTYPE_INTEGER: 
		case SOAPTYPE_NONPOSITIVEINTEGER:
		case SOAPTYPE_NEGATIVEINTEGER:
		case SOAPTYPE_LONG:
		case SOAPTYPE_SHORT:
		case SOAPTYPE_BYTE:
		case SOAPTYPE_NONNEGATIVEINTEGER:
		case SOAPTYPE_UNSIGNEDLONG:
		case SOAPTYPE_UNSIGNEDINT:
		case SOAPTYPE_UNSIGNEDSHORT:
		case SOAPTYPE_UNSIGNEDBYTE:
			break;

		case SOAPTYPE_BINARY:
			ATLASSERT( FALSE );
			//
			// TODO: implement
			//
			break;
		
		case SOAPTYPE_BLOB:
			// should never get here
			hr = AtlCleanupValue((ATLSOAP_BLOB *)pVal);
			break;
		default:
			if ((type != SOAPTYPE_ERR) && (type != SOAPTYPE_UNK) && (type != SOAPTYPE_USERBASE))
			{
				// treat as string
				hr = AtlCleanupValue((BSTR *)pVal);
			}
#ifdef _DEBUG
			else
			{
				// should never get here
				ATLASSERT( FALSE );
			}
#endif
			break;
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
//
// END PRIVATE DEFINITIONS
//
////////////////////////////////////////////////////////////////////////////////

#define SOAP_ENVELOPEA "Envelope"
#define SOAP_ENVELOPEW ATLSOAP_MAKEWIDESTR( SOAP_ENVELOPEA )

#define SOAP_HEADERA   "Header"
#define SOAP_HEADERW   ATLSOAP_MAKEWIDESTR( SOAP_HEADERA )

#define SOAP_BODYA     "Body"
#define SOAP_BODYW     ATLSOAP_MAKEWIDESTR( SOAP_BODYA )

namespace ATL
{

//
// SOAP fault helpers
//

enum SOAP_ERROR_CODE 
{ 
	SOAP_E_UNK=0,
	SOAP_E_VERSION_MISMATCH=100,
	SOAP_E_MUST_UNDERSTAND=200,
	SOAP_E_CLIENT=300,
	SOAP_E_SERVER=400
};

// forward declaration of CSoapFault
class CSoapFault;

class CSoapFaultParser : public ISAXContentHandlerImpl
{
private:

	CSoapFault *m_pFault;

	DWORD m_dwState;

	const static DWORD STATE_ERROR       = 0;
	const static DWORD STATE_ENVELOPE    = 1;
	const static DWORD STATE_BODY        = 2;
	const static DWORD STATE_START       = 4;
	const static DWORD STATE_FAULTCODE   = 8;
	const static DWORD STATE_FAULTSTRING = 16;
	const static DWORD STATE_FAULTACTOR  = 32;
	const static DWORD STATE_DETAIL      = 64;
	const static DWORD STATE_RESET       = 128;
	

	CComPtr<ISAXXMLReader> m_spReader;
	CSAXStringBuilder m_stringBuilder;

	const wchar_t *m_wszSoapPrefix;
	int      m_cchSoapPrefix;

public:

	// IUnknown interface
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

	// constructor

	CSoapFaultParser(CSoapFault *pFault, ISAXXMLReader *pReader) throw()
		:m_pFault(pFault), m_dwState(STATE_ERROR), m_spReader(pReader)
	{
		ATLASSERT( pFault != NULL );
		ATLASSERT( pReader != NULL );
	}

	// ISAXContentHandler interface
	HRESULT __stdcall startElement( 
	     const wchar_t  * wszNamespaceUri,
	     int cchNamespaceUri,
	     const wchar_t  * wszLocalName,
	     int cchLocalName,
	     const wchar_t  * /*wszQName*/,
	     int /*cchQName*/,
	     ISAXAttributes  * /*pAttributes*/) throw()
	{
		struct _faultmap
		{
			const wchar_t *wszTag;
			int cchTag;
			DWORD dwState;
		};

		const static _faultmap s_faultParseMap[] =
		{
			{ L"Envelope", sizeof("Envelope")-1, CSoapFaultParser::STATE_ENVELOPE },
			{ L"Body", sizeof("Body")-1, CSoapFaultParser::STATE_BODY },
			{ L"Fault", sizeof("Fault")-1, CSoapFaultParser::STATE_START },
			{ L"faultcode", sizeof("faultcode")-1, CSoapFaultParser::STATE_FAULTCODE },
			{ L"faultstring", sizeof("faultstring")-1, CSoapFaultParser::STATE_FAULTSTRING },
			{ L"faultactor", sizeof("faultactor")-1, CSoapFaultParser::STATE_FAULTACTOR },
			{ L"detail", sizeof("detail")-1, CSoapFaultParser::STATE_DETAIL }
		};

		if (m_spReader.p == NULL)
		{
			return E_INVALIDARG;
		}

		m_dwState &= ~STATE_RESET;
		for (int i=0; i<(sizeof(s_faultParseMap)/sizeof(s_faultParseMap[0])); i++)
		{
			if ((cchLocalName == s_faultParseMap[i].cchTag) &&
				(!wcsncmp(wszLocalName, s_faultParseMap[i].wszTag, cchLocalName)))
			{
				DWORD dwState = s_faultParseMap[i].dwState;
				if ((dwState & (STATE_START | STATE_ENVELOPE | STATE_BODY)) == 0)
				{
					m_stringBuilder.SetReader(m_spReader);
					m_stringBuilder.SetParent(this);

					m_stringBuilder.Clear();
					m_spReader->putContentHandler( &m_stringBuilder );
				}
				else
				{
					if ((dwState <= m_dwState) || 
						(cchNamespaceUri != sizeof(SOAPENV_NAMESPACEA)-1) ||
						(wcsncmp(wszNamespaceUri, SOAPENV_NAMESPACEW, cchNamespaceUri)))
					{
						return E_FAIL;
					}
				}

				m_dwState = dwState;
				return S_OK;
			}
		}

		return E_FAIL;
	}

	HRESULT __stdcall startPrefixMapping(
	     const wchar_t  * wszPrefix,
	     int cchPrefix,
	     const wchar_t  * wszUri,
	     int cchUri) throw()
	{
		if ((cchUri == sizeof(SOAPENV_NAMESPACEA)-1) &&
			(!wcsncmp(wszUri, SOAPENV_NAMESPACEW, cchUri)))
		{
			m_wszSoapPrefix = wszPrefix;
			m_cchSoapPrefix = cchPrefix;
		}

		return S_OK;
	}

	HRESULT __stdcall characters( 
	     const wchar_t  * wszChars,
	     int cchChars) throw();
};

extern __declspec(selectany) const int ATLS_SOAPFAULT_CNT = 4;

class CSoapFault
{
private:

	struct _faultcode
	{
		const wchar_t *wsz;
		int cch;
		const wchar_t *wszFaultString;
		int cchFaultString;
		SOAP_ERROR_CODE errCode;
	};

	static const _faultcode s_faultCodes[];

public:

	// members
	SOAP_ERROR_CODE m_soapErrCode;
	CStringW m_strFaultString;
	CStringW m_strFaultActor;
	CStringW m_strDetail;

	CSoapFault() throw()
		: m_soapErrCode(SOAP_E_UNK)
	{
	}

	HRESULT SetErrorCode(
		const wchar_t *wsz, 
		const wchar_t *wszSoapPrefix,
		int cch = -1, 
		int cchSoapPrefix = -1,
		bool bSetFaultString = true) throw()
	{
		if ((wsz == NULL) || (wszSoapPrefix == NULL))
		{
			return E_INVALIDARG;
		}

		if (cch == -1)
		{
			cch = (int) wcslen(wsz);
		}

		if (cchSoapPrefix == -1)
		{
			cchSoapPrefix = (int) wcslen(wszSoapPrefix);
		}

		const wchar_t *wszLocalName = wcschr(wsz, L':');
		if (wszLocalName == NULL)
		{
			// faultCode must be QName
			return E_FAIL;
		}

		// make sure the the namespace of the fault is the
		// SOAPENV namespace
		if ((cchSoapPrefix != (int)(wszLocalName-wsz)) ||
			(wcsncmp(wsz, wszSoapPrefix, cchSoapPrefix)))
		{
			return E_FAIL;
		}

		wszLocalName++;
		cch -= (int) (wszLocalName-wsz);

		HRESULT hr = E_FAIL;
		_ATLTRY
		{
			for (int i=0; i<ATLS_SOAPFAULT_CNT; i++)
			{
				if ((cch == s_faultCodes[i].cch) &&
					(!wcsncmp(wszLocalName, s_faultCodes[i].wsz, cch)))
				{
					m_soapErrCode = s_faultCodes[i].errCode;
					if (bSetFaultString != false)
					{
						m_strFaultString.SetString(s_faultCodes[i].wszFaultString, s_faultCodes[i].cchFaultString);
					}
					hr = S_OK;
					break;
				}
			}
		}
		_ATLCATCHALL()
		{
			hr = E_OUTOFMEMORY;
		}
		return hr;
	}

	HRESULT ParseFault(IStream *pStream, ISAXXMLReader *pReader = NULL) throw()
	{
		if (pStream == NULL)
		{
			return E_INVALIDARG;
		}

		CComPtr<ISAXXMLReader> spReader;
		if (pReader != NULL)
		{
			spReader = pReader;
		}
		else
		{
			if (FAILED(spReader.CoCreateInstance(__uuidof(SAXXMLReader30))))
			{
				return E_FAIL;
			}
		}

		Clear();
		CSoapFaultParser parser(const_cast<CSoapFault *>(this), spReader);
		spReader->putContentHandler(&parser);

		CComVariant varStream;
		varStream = static_cast<IUnknown*>(pStream);

		HRESULT hr = spReader->parse(varStream);
		spReader->putContentHandler(NULL);
		return hr;
	}

	HRESULT GenerateFault(IWriteStream *pWriteStream) throw()
	{
		if ((pWriteStream == NULL) || (m_soapErrCode == SOAP_E_UNK))
		{
			return E_INVALIDARG;
		}

		ATLASSERT( (m_soapErrCode == SOAP_E_UNK) || 
		           (m_soapErrCode == SOAP_E_VERSION_MISMATCH) ||
		           (m_soapErrCode == SOAP_E_MUST_UNDERSTAND) || 
				   (m_soapErrCode == SOAP_E_CLIENT) ||
				   (m_soapErrCode == SOAP_E_SERVER) );

		HRESULT hr = S_OK;
		_ATLTRY
		{
			int i;
			for (i=0; i<4; i++)
			{
				if (s_faultCodes[i].errCode == m_soapErrCode)
				{
					if (m_strFaultString.GetLength() == 0)
					{
						m_strFaultString.SetString(s_faultCodes[i].wszFaultString, 
								s_faultCodes[i].cchFaultString);
					}
					break;
				}
			}
		
			const LPCSTR s_szErrorFormat =
				"<SOAP:Envelope xmlns:SOAP=\"" SOAPENV_NAMESPACEA "\">\n"
				"	<SOAP:Body>\n"
				"		<SOAP:Fault>\n"
				"			<faultcode>SOAP:%ws</faultcode>\n"
				"			<faultstring>%ws</faultstring>\n"
				"%s%ws%s"
				"%s%ws%s"
				"		</SOAP:Fault>\n"
				"	</SOAP:Body>\n"
				"</SOAP:Envelope>\n";

			CStringA strFault;
			strFault.Format(s_szErrorFormat, s_faultCodes[i].wsz, m_strFaultString, 
				m_strFaultActor.GetLength() ? "<faultactor>" : "", m_strFaultActor, 
				m_strFaultActor.GetLength() ? "</faultactor>\n" : "",
				m_strDetail.GetLength() ? "<detail>" : "", m_strDetail,
				m_strDetail.GetLength() ? "</detail>\n" : "");

			hr = pWriteStream->WriteStream(strFault, strFault.GetLength(), NULL);
		}
		_ATLCATCHALL()
		{
			hr = E_OUTOFMEMORY;
		}

		return hr;
	}

	void Clear() throw()
	{
		m_soapErrCode = SOAP_E_UNK;
		m_strFaultString.Empty();
		m_strFaultActor.Empty();
		m_strDetail.Empty();
	}
}; // class CSoapFault

#define DECLARE_SOAP_FAULT(__name, __faultstring, __errcode) \
	{ L ## __name, sizeof(__name)-1, L ## __faultstring, sizeof(__faultstring), __errcode },

__declspec(selectany) const CSoapFault::_faultcode CSoapFault::s_faultCodes[] =
{
	DECLARE_SOAP_FAULT("VersionMismatch", "SOAP Version Mismatch Error", SOAP_E_VERSION_MISMATCH)
	DECLARE_SOAP_FAULT("MustUnderstand", "SOAP Must Understand Error", SOAP_E_MUST_UNDERSTAND)
	DECLARE_SOAP_FAULT("Client", "SOAP Invalid Request", SOAP_E_CLIENT)
	DECLARE_SOAP_FAULT("Server", "SOAP Server Application Faulted", SOAP_E_SERVER)
};

ATL_NOINLINE inline HRESULT __stdcall CSoapFaultParser::characters( 
	     const wchar_t  * wszChars,
	     int cchChars) throw()
{
	if (m_pFault == NULL)
	{
		return E_INVALIDARG;
	}

	if (m_dwState & STATE_RESET)
	{
		return S_OK;
	}

	HRESULT hr = E_FAIL;
	_ATLTRY
	{
		switch (m_dwState)
		{
			case STATE_FAULTCODE:
				if (m_pFault->m_soapErrCode == SOAP_E_UNK)
				{
					hr = m_pFault->SetErrorCode(wszChars, m_wszSoapPrefix, 
						cchChars, m_cchSoapPrefix, false);
				}
				break;
			case STATE_FAULTSTRING:
				if (m_pFault->m_strFaultString.GetLength() == 0)
				{
					m_pFault->m_strFaultString.SetString(wszChars, cchChars);
					hr = S_OK;
				}
				break;
			case STATE_FAULTACTOR:
				if (m_pFault->m_strFaultActor.GetLength() == 0)
				{
					m_pFault->m_strFaultActor.SetString(wszChars, cchChars);
					hr = S_OK;
				}
				break;
			case STATE_DETAIL:
				if (m_pFault->m_strDetail.GetLength() == 0)
				{
					m_pFault->m_strDetail.SetString(wszChars, cchChars);
					hr = S_OK;
				}
				break;
			case STATE_START: case STATE_ENVELOPE : case STATE_BODY :
				hr = S_OK;
				break;
			default:
				// should never get here
				ATLASSERT( FALSE );
				break;
		}
	}
	_ATLCATCHALL()
	{
		hr = E_OUTOFMEMORY;
	}
	
	m_dwState |= STATE_RESET;

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
//
// CSoapRootHandler - the class that does most of the work
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ATLSOAP_STACKSIZE
// 16 will be plenty for the 99% case
#define ATLSOAP_STACKSIZE 16
#endif

class CSoapRootHandler : public ISAXContentHandlerImpl
{
private:

	friend class _CSDLGenerator;

	//
	// state constants
	//
	const static DWORD SOAP_START        = 0;
	const static DWORD SOAP_ENVELOPE     = 1;
	const static DWORD SOAP_HEADERS      = 2;
	const static DWORD SOAP_BODY         = 3;
	const static DWORD SOAP_PARAMS       = 4;
	const static DWORD SOAP_CALLED       = 5;
	const static DWORD SOAP_RESPONSE     = 6;
	const static DWORD SOAP_HEADERS_DONE = 7;

	//
	// hash values for SOAP namespaces and elements
	//
	const static ULONG SOAP_ENV = 0x5D3574E2;
	const static ULONG SOAP_ENC = 0xBD62724B;
	const static ULONG ENVELOPE = 0xDBE6009E;
	const static ULONG HEADER   = 0xAF4DFFC9;
	const static ULONG BODY     = 0x0026168E;

	//
	// XSD Names
	//
	struct XSDEntry
	{
		wchar_t * wszName;
		char * szName;
		int cchName;
	};

	const static XSDEntry s_xsdNames[];

	//
	// CBitVector - a dynamically sized bit vector class
	//
	class CBitVector
	{
	private:

		// 64 bits will handle the 99% case
		unsigned __int64 m_nBits;

		// when we need to grow
		unsigned __int64 * m_pBits;

		size_t m_nSize;

		bool Grow(size_t nIndex) throw()
		{
			// round up to nearest 64 bits
			size_t nAllocSize = nIndex+(64-(nIndex%64));

			if (m_pBits != &m_nBits)
			{
				m_pBits = (unsigned __int64 *) realloc(m_pBits, nAllocSize/4);
			}
			else
			{
				m_pBits = (unsigned __int64 *) malloc(nAllocSize/4);
				if (m_pBits != NULL)
				{
					memcpy(m_pBits, &m_nBits, sizeof(m_nBits));
				}
			}
			
			if (m_pBits != NULL)
			{
				// set new bits to 0
				memset(m_pBits+(m_nSize/(sizeof(m_nBits)*8)), 0x00, (nAllocSize-m_nSize)/sizeof(m_nBits));
				m_nSize = nAllocSize;
				return true;
			}

			return false;
		}
		
	public:

		CBitVector() throw()
			: m_nBits(0), m_nSize(64)
		{
			m_pBits = &m_nBits;
		}

		CBitVector(const CBitVector&) throw()
		{
			m_pBits = &m_nBits;
		}

		const CBitVector& operator=(const CBitVector& that) throw()
		{
			if (this != &that)
			{
				m_pBits = &m_nBits;
			}

			return *this;
		}

		bool GetBit(size_t nIndex) throw()
		{
			if (nIndex >= m_nSize)
			{
				return false;
			}
			
			size_t i = nIndex/64;
			size_t nBits = nIndex-i*64;
			return ((m_pBits[i] >> nBits) & 0x01);
		}
		
		bool SetBit(size_t nIndex) throw()
		{
			if (nIndex >= m_nSize)
			{
				if (!Grow(nIndex))
				{
					return false;
				}
			}
			
			size_t i = nIndex/64;
			size_t nBits = nIndex-i*64;
			m_pBits[i] |= (((unsigned __int64) 1) << nBits);
			
			return true;
		}

		void Clear() throw()
		{
			if (m_pBits == &m_nBits)
			{
				m_nBits = 0;
			}
			else
			{
				memset(m_pBits, 0x00, (m_nSize/4));
			}
		}
		
		~CBitVector() throw()
		{
			if (m_pBits != &m_nBits)
			{
				free(m_pBits);
			}

			m_pBits = &m_nBits;
			m_nSize = 64;
		}
	}; // class CBitVector

	//
	// Parsing State
	//
	struct ParseState
	{
		void *pvElement;
		DWORD dwFlags;
		size_t nAllocSize;
		size_t nExpectedElements;
		size_t nElement;
		const _soapmap *pMap;
		const _soapmapentry *pEntry;

		// mark when we get an item
		CBitVector vec;

		ParseState(void *pvElement_ = NULL, DWORD dwFlags_ = 0, 
			size_t nAllocSize_ = 0, size_t nExpectedElements_ = 0, 
			size_t nElement_ = 0, const _soapmap *pMap_ = NULL, 
			const _soapmapentry *pEntry_ = NULL) throw()
			: pvElement(pvElement_), dwFlags(dwFlags_), nAllocSize(nAllocSize_),
			  nExpectedElements(nExpectedElements_), nElement(nElement_), pMap(pMap_),
			  pEntry(pEntry_)
		{
			vec.Clear();
		}

		ParseState(const ParseState& that) throw()
		{
			pvElement = that.pvElement;
			dwFlags = that.dwFlags;
			nAllocSize = that.nAllocSize;
			nExpectedElements = that.nExpectedElements;
			nElement = that.nElement;
			pMap = that.pMap;
			pEntry = that.pEntry;

			vec.Clear();
		}

		~ParseState() throw()
		{
			pvElement = NULL;
			dwFlags = 0;
			nAllocSize = 0;
			nExpectedElements = 0;
			nElement = 0;
			pMap = NULL;
			pEntry = NULL;

			vec.Clear();
		}
	}; // struct ParseState

	//
	// members
	//
	CAtlArray<ParseState> m_stateStack;
	size_t m_nState;

	DWORD m_dwState;
	bool m_bErrorsOccurred;

	CComPtr<ISAXXMLReader> m_spReader;

	CSAXStringBuilder m_stringBuilder;
	CBlobHandler m_blobHandler;
	CSkipHandler m_skipHandler;

	IAtlMemMgr * m_pMemMgr;

	static CCRTHeap m_crtHeap;

	bool m_bClient;

	void *m_pvParam;

	typedef CFixedStringT<CStringW, 16> REFSTRING;

	// used for rpc/encoded messages with href's
	typedef CAtlMap<REFSTRING, ParseState, CStringRefElementTraits<REFSTRING> > REFMAP;
	REFMAP m_refMap;

	//
	// Implementation helpers
	//

	HRESULT PushState(void *pvElement = NULL, const _soapmap *pMap = NULL,
			const _soapmapentry *pEntry = NULL, DWORD dwFlags = 0, size_t nAllocSize = 0, 
			size_t nExpectedElements = 0, size_t nElement = 0) throw()
	{
		// REVIEW: CAtlArray::Add() and CAtlArray::SetCount do not throw if the default
		// constructor of the element doesn't throw, but should review with dbartol if
		// he plans to change it.
		if (m_stateStack.IsEmpty())
		{
			// 16 will be plenty for the 99% case
			if (!m_stateStack.SetCount(0, 16))
			{
				// REVIEW: not necessary right now, but might change in the future (?)
				return E_OUTOFMEMORY;
			}
		}

		size_t nCnt = m_stateStack.GetCount();
		m_nState = m_stateStack.Add();
		if (m_stateStack.GetCount() <= nCnt)
		{
			return E_OUTOFMEMORY;
		}

		ParseState &state = m_stateStack[m_nState];

		state.pvElement = pvElement;
		state.dwFlags = dwFlags;
		state.nAllocSize = nAllocSize;
		state.nExpectedElements = nExpectedElements;
		state.nElement = nElement;
		state.pMap = pMap;
		state.pEntry = pEntry;

		return S_OK;
	}

	ParseState& GetState() throw()
	{
		return m_stateStack[m_nState];
	}

	ParseState PopState(bool bForce = false) throw()
	{
		ParseState popState( m_stateStack[m_nState] );

		if ((m_nState != 0) || (bForce != false))
		{
			if ((m_stateStack[m_nState].dwFlags & SOAPFLAG_DISABLEPOP) == 0)
			{
				m_stateStack.RemoveAt(m_nState);
				--m_nState;
			}
		}

		m_stateStack[m_nState].dwFlags &= ~SOAPFLAG_DISABLEPOP;
		return popState;
	}

	BOOL IsEqualElement(int cchLocalNameCheck, const wchar_t *wszLocalNameCheck, 
		int cchNamespaceUriCheck, const wchar_t *wszNamespaceUriCheck,
		int cchLocalName, const wchar_t *wszLocalName,
		int cchNamespaceUri, const wchar_t *wszNamespaceUri) throw()
	{
		if (cchLocalName == cchLocalNameCheck &&
			cchNamespaceUri == cchNamespaceUriCheck &&
			!wcsncmp(wszLocalName, wszLocalNameCheck, cchLocalName) &&
			!wcsncmp(wszNamespaceUri, wszNamespaceUriCheck, cchNamespaceUri))
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOL IsEqualElement(int cchLocalNameCheck, const wchar_t *wszLocalNameCheck, 
		int cchLocalName, const wchar_t *wszLocalName) throw()
	{
		if (cchLocalName == cchLocalNameCheck &&
			!wcsncmp(wszLocalName, wszLocalNameCheck, cchLocalName))
		{
			return TRUE;
		}

		return FALSE;
	}

	void SetOffsetValue(void *pBase, void *pSrc, size_t nOffset) throw()
	{
		void **ppDest = (void **)(((unsigned char *)pBase)+nOffset);
		*ppDest = pSrc;
	}

	HRESULT ValidateArrayEntry(
		const ParseState& state,
	    const wchar_t  *wszLocalName,
	    int cchLocalName) throw()
	{
		ATLASSERT( state.pEntry != NULL );

		// check number of elements
		if (state.nElement == state.nExpectedElements)
		{
			// too many elements
			return E_FAIL;
		}

		// REVIEW: validate namespace?

		// validate element type

		// if UDT
		if (state.pEntry->nVal == SOAPTYPE_UNK)
		{
			ATLASSERT( state.pEntry->pChain != NULL );

			// validate against name in entry

			if ((cchLocalName != state.pEntry->pChain->cchWName) ||
				(wcsncmp(wszLocalName, state.pEntry->pChain->wszName, cchLocalName)))
			{
				return E_FAIL;
			}
		}
		else // simple type or SOAP_BLOB
		{
			ATLASSERT( state.pEntry->nVal != SOAPTYPE_ERR );
			ATLASSERT( state.pEntry->nVal != SOAPTYPE_USERBASE );

			// validate against name s_xsdNames

			if ((cchLocalName != s_xsdNames[state.pEntry->nVal].cchName) ||
				(wcsncmp(wszLocalName, s_xsdNames[state.pEntry->nVal].wszName, cchLocalName)))
			{
				return E_FAIL;
			}
		}

		return S_OK;
	}

	HRESULT CheckID(
		const wchar_t *wszNamespaceUri,
		const wchar_t *wszLocalName,
		int cchLocalName,
		ISAXAttributes *pAttributes) throw()
	{
		wszNamespaceUri;
		ATLASSERT( pAttributes != NULL );

		const wchar_t *wsz = NULL;
		int cch = 0;

		HRESULT hr = GetAttribute(pAttributes, L"id", sizeof("id")-1, &wsz, &cch);
		if ((hr == S_OK) && (wsz != NULL))
		{
			const REFMAP::CPair *p = NULL;
			_ATLTRY
			{
				REFSTRING strRef(wsz, cch);
				p = m_refMap.Lookup(strRef);
				if (p == NULL)
				{
					// not referenced
					// REVIEW: just skip element?
					return E_FAIL;
				}
			}
			_ATLCATCHALL()
			{
				return E_OUTOFMEMORY;
			}
			const ParseState& state = p->m_value;

			// make sure it's the right element
			// REVIEW (jasjitg): some namespace stuff is screwed up
			if ((cchLocalName == state.pMap->cchWName) &&
				(!wcsncmp(wszLocalName, state.pMap->wszName, cchLocalName)))/* &&
				(!wcscmp(wszNamespaceUri, GetNamespaceUri())))*/
			{
				// REVIEW: should check the current state to make sure we push only at valid times
				if (S_OK != PushState(state.pvElement, state.pMap, state.pEntry, state.dwFlags, 0, state.pMap->nElements))
				{
					return E_OUTOFMEMORY;
				}

				m_refMap.RemoveAtPos(const_cast<REFMAP::CPair*>(p));
				return S_OK;
			}

			// not the right type
			return E_FAIL;
		}

		return S_FALSE;
	}

	HRESULT GetElementEntry(
		ParseState& state,
		const wchar_t *wszNamespaceUri,
	    const wchar_t *wszLocalName,
	    int cchLocalName,
		ISAXAttributes *pAttributes,
		const _soapmapentry **ppEntry) throw()
	{
		ATLASSERT( state.pMap != NULL );
		ATLASSERT( ppEntry != NULL );

		*ppEntry = NULL;
		const _soapmapentry *pEntries = state.pMap->pEntries;
		DWORD dwIncludeFlags;
		DWORD dwExcludeFlags;

		HRESULT hr = CheckID(wszNamespaceUri, wszLocalName, cchLocalName, pAttributes);
		if (hr != S_FALSE)
		{
			if (hr == S_OK)
			{
				hr = S_FALSE;
			}
			return hr;
		}

		if (m_bClient != false)
		{
			dwIncludeFlags = SOAPFLAG_OUT;
			dwExcludeFlags = SOAPFLAG_IN;
		}
		else
		{
			dwIncludeFlags = SOAPFLAG_IN;
			dwExcludeFlags = SOAPFLAG_OUT;
		}

		ULONG nHash = AtlSoapHashStr(wszLocalName, cchLocalName);

		for (size_t i=0; pEntries[i].nHash != 0; i++)
		{
			//
			// TODO: check flags here (?)
			//

			if (nHash == pEntries[i].nHash && 
				((pEntries[i].dwFlags & dwIncludeFlags) || 
				 ((pEntries[i].dwFlags & dwExcludeFlags) == 0)) &&
				IsEqualElement(pEntries[i].cchField, pEntries[i].wszField, 
				cchLocalName, wszLocalName)/* &&
				!wcscmp(wszNamespaceUri, wszNamespace)*/)
			{
				// check bit vector

				if (state.vec.GetBit(i) == false)
				{
					if (state.vec.SetBit(i) == false)
					{
						return E_OUTOFMEMORY;
					}
				}
				else
				{
					// already received this element

					return E_FAIL;
				}

				state.nElement++;
				*ppEntry = &pEntries[i];

				return S_OK;
			}
		}

		return E_FAIL;
	}

	HRESULT CheckMustUnderstandHeader(ISAXAttributes *pAttributes) throw()
	{
		ATLASSERT( pAttributes != NULL );

		const wchar_t* wszMustUnderstand;
		int cchMustUnderstand;
		bool bMustUnderstand= false;
		
		if (SUCCEEDED(GetAttribute(pAttributes, L"mustUnderstand", sizeof("mustUnderstand")-1, 
				&wszMustUnderstand, &cchMustUnderstand,
				SOAPENV_NAMESPACEW, sizeof(SOAPENV_NAMESPACEA)-1)) && 
				(wszMustUnderstand != NULL))
		{
			if (FAILED(AtlGetSAXValue(&bMustUnderstand, wszMustUnderstand, cchMustUnderstand)))
			{
				bMustUnderstand = true;
			}
		}

		if (bMustUnderstand == false)
		{
			ATLASSERT( GetReader() != NULL );

			m_skipHandler.SetReader(GetReader());
			m_skipHandler.SetParent(this);

			return GetReader()->putContentHandler( &m_skipHandler );
		}
//		else
//		{
//			// TODO: SOAP Fault with mustUnderstand
//		}

		return E_FAIL;
	}

	HRESULT GetSection5Info(
		const ParseState& state,
		const _soapmapentry *pEntry,
		ISAXAttributes *pAttributes) throw()
	{
		ATLASSERT( pEntry != NULL );
		ATLASSERT( pAttributes != NULL );

		size_t nElements;
		DWORD dwFlags = 0;
		HRESULT hr = AtlSoapGetArraySize(pAttributes, &nElements);
		if (SUCCEEDED(hr))
		{
			// do any allocations if necessary

			size_t nElementSize;
			if (pEntry->dwFlags & SOAPFLAG_DYNARR)
			{
				if (pEntry->nVal != SOAPTYPE_UNK)
				{
					nElementSize = AtlSoapGetElementSize((SOAPTYPES) pEntry->nVal);
				}
				else // UDT
				{
					ATLASSERT( pEntry->pChain != NULL );

					nElementSize = pEntry->pChain->nElementSize;
				}

				if (nElementSize != 0)
				{
					ATLASSERT( nElements != 0 );

					void *p = m_pMemMgr->Allocate(nElementSize*nElements);
					if (p == NULL)
					{
						return E_OUTOFMEMORY;
					}

					memset(p, 0x00, nElementSize*nElements);

					SetOffsetValue(state.pvElement, p, pEntry->nOffset);
				}
				else
				{
					// fatal internal error

					ATLASSERT( FALSE );

					return E_FAIL;
				}

				// set size_is value

				ATLASSERT( GetState().pMap != NULL );

				int *pnSizeIs = (int *)(((unsigned char *)state.pvElement)+
					(state.pMap->pEntries[pEntry->nSizeIs].nOffset));
				*pnSizeIs = (int)nElements;
			}
			else
			{
				ATLASSERT( pEntry->dwFlags & SOAPFLAG_FIXEDARR );

				// for fixed size arrays, we know the number of elements

				if (nElements != AtlSoapGetArrayDims(pEntry->pDims))
				{
					return E_FAIL;
				}
			}
		}
		else if (pEntry->dwFlags & SOAPFLAG_DYNARR)
		{
			// TODO: missing section-5 info: allocate as necessary

			// dwFlags |= SOAPFLAG_UNKSIZE;

			return E_FAIL;
		}
		else
		{
			ATLASSERT( pEntry->dwFlags & SOAPFLAG_FIXEDARR );

			nElements = AtlSoapGetArrayDims(pEntry->pDims);
		}

		dwFlags |= pEntry->dwFlags;

		// push element with array flag

		if (S_OK != PushState(((unsigned char *)state.pvElement)+pEntry->nOffset, 
			NULL, pEntry, dwFlags & ~SOAPFLAG_READYSTATE, 0, nElements))
		{
			return E_OUTOFMEMORY;
		}

		return S_OK;
	}

	void * UpdateArray(ParseState& state, const _soapmapentry *pEntry) throw()
	{
		size_t nSize;
		void *pVal = NULL;
		if (pEntry->nVal != SOAPTYPE_UNK)
		{
			nSize = AtlSoapGetElementSize((SOAPTYPES) pEntry->nVal);
		}
		else
		{
			ATLASSERT( pEntry->pChain != NULL );

			nSize = pEntry->pChain->nElementSize;
		}

		if (state.dwFlags & SOAPFLAG_FIXEDARR)
		{
			unsigned char *ppArr = (unsigned char *)state.pvElement;
			pVal = ppArr+(state.nElement*nSize);
		}
		else
		{
			ATLASSERT( state.dwFlags & SOAPFLAG_DYNARR );

			unsigned char **ppArr = (unsigned char **)state.pvElement;
			pVal = (*ppArr)+(state.nElement*nSize);
		}

		state.nElement++;

		return pVal;
	}

	HRESULT ProcessString(const _soapmapentry *pEntry, ISAXAttributes *pAttributes, void *pVal) throw()
	{
		ATLASSERT( pEntry != NULL );
		ATLASSERT( pAttributes != NULL );

		// check for the xsi:null attribute
		const wchar_t *wszNull;
		int cchNull;

		DWORD dwFlags = SOAPFLAG_NONE;
		if (SUCCEEDED(GetAttribute(pAttributes, L"null", sizeof("null")-1, &wszNull, &cchNull,
				XSI_NAMESPACEW, sizeof(XSI_NAMESPACEA)-1)) && 
				(wszNull != NULL) && 
				(cchNull == sizeof("true")-1) &&
				(!wcsncmp(wszNull, L"true", cchNull)))
		{
			*((unsigned char *)pVal) = NULL;
		}
		else
		{
			//  set to the string builder class

			ATLASSERT( GetReader() != NULL );

			m_stringBuilder.SetReader(GetReader());
			m_stringBuilder.SetParent(this);

			m_stringBuilder.Clear();
			GetReader()->putContentHandler( &m_stringBuilder );

			dwFlags = SOAPFLAG_READYSTATE;
		}

		if (S_OK != PushState(pVal, NULL, pEntry, dwFlags | pEntry->dwFlags))
		{
			return E_OUTOFMEMORY;
		}

		return S_OK;
	}

	HRESULT ProcessBlob(void *pVal) throw()
	{
		ATLASSERT( GetReader() != NULL );
		ATLASSERT( pVal != NULL );

		m_blobHandler.SetBlob((ATLSOAP_BLOB *) pVal);
		m_blobHandler.SetReader(GetReader());
		m_blobHandler.SetParent(this);
		return GetReader()->putContentHandler( &m_blobHandler );
	}

	HRESULT CheckHref(
		const _soapmapentry *pEntry,
		void *pVal,
		ISAXAttributes *pAttributes) throw()
	{
		ATLASSERT( pEntry != NULL );
		ATLASSERT( pVal != NULL );
		ATLASSERT( pAttributes != NULL );
		ATLASSERT( pEntry->pChain != NULL );

		const _soapmap *pMap = pEntry->pChain;
		if (pMap->mapType != SOAPMAP_STRUCT)
		{
			// can only be used on structs
			return E_FAIL;
		}

		const wchar_t *wsz = NULL;
		int cch = 0;

		HRESULT hr = GetAttribute(pAttributes, L"href", sizeof("href")-1, &wsz, &cch);
		if ((hr == S_OK) && (wsz != NULL))
		{
			_ATLTRY
			{
				if (*wsz == L'#')
				{
					wsz++;
					cch--;
				}

				REFSTRING strRef(wsz, cch);
				if (m_refMap.Lookup(strRef) != NULL)
				{
					// ATL Server does not support multi-reference objects 
					ATLASSERT( FALSE );
					return E_FAIL;
				}

				ParseState state;
				state.pvElement = pVal;
				state.dwFlags = pEntry->dwFlags;
				state.nExpectedElements = pMap->nElements;
				state.nElement = 0;
				state.pMap = pMap;
				state.pEntry = pEntry;

				if (!m_refMap.SetAt(strRef, state))
				{
					return E_OUTOFMEMORY;
				}

				GetState().dwFlags |= SOAPFLAG_DISABLEPOP;
				return S_OK;
			}
			_ATLCATCHALL()
			{
				return E_OUTOFMEMORY;
			}
		}

		return S_FALSE;
	}

	HRESULT ProcessUDT(
		const _soapmapentry *pEntry, 
		void *pVal, 
		ISAXAttributes *pAttributes) throw()
	{
		ATLASSERT( pEntry != NULL );
		ATLASSERT( pVal != NULL );
		ATLASSERT( pAttributes != NULL );
		ATLASSERT( pEntry->nVal != SOAPTYPE_ERR );
		ATLASSERT( pEntry->nVal != SOAPTYPE_USERBASE );

		if ((pEntry->dwFlags & (SOAPFLAG_RPC | SOAPFLAG_ENCODED)) ==
			 (SOAPFLAG_RPC | SOAPFLAG_ENCODED))
		{
			// check for href
			// we ONLY do this for rpc/encoded (required for interop)
			// NOTE: ATL Server does not support object graphs, so 
			// only single-reference elements are allowed
			HRESULT hr = CheckHref(pEntry, pVal, pAttributes);
			if (hr != S_FALSE)
			{
				return hr;
			}
		}

		// if it is a complex type, get the chain entry
		// and push the new state on the stack

		DWORD dwFlags = pEntry->dwFlags;
		if (pEntry->pChain->mapType != SOAPMAP_ENUM)
		{
			dwFlags &= ~(SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR);
		}
		else
		{
			// enum
			dwFlags |= SOAPFLAG_READYSTATE;
		}

		if (S_OK != PushState(pVal, pEntry->pChain, pEntry, dwFlags, 0, pEntry->pChain->nElements))
		{
			return E_OUTOFMEMORY;
		}

		return S_OK;
	}

	HRESULT ChainEntry(
		const ParseState& state,
		const wchar_t  *wszNamespaceUri,
		int cchNamespaceUri,
	    const wchar_t  *wszLocalName,
	    int cchLocalName,
	    ISAXAttributes  *pAttributes) throw()
	{
		ATLASSERT( state.pMap != NULL );

		// PAD is only supported on the client
		const _soapmap *pMap = state.pMap;
		if ((pMap->dwCallFlags & SOAPFLAG_CHAIN)==0)
		{
			return S_FALSE;
		}
		
		ATLASSERT( pMap->dwCallFlags & SOAPFLAG_PAD );
		ATLASSERT( pMap->nElements == 1 );
		const _soapmapentry *pEntries = pMap->pEntries;
		ATLASSERT( pEntries != NULL );

//		if (m_bClient != false)
//		{
//			dwIncludeFlags = SOAPFLAG_OUT;
//			dwExcludeFlags = SOAPFLAG_IN;
//		}
//		else
//		{
//			dwIncludeFlags = SOAPFLAG_IN;
//			dwExcludeFlags = SOAPFLAG_OUT;
//		}

		int nIndex;
		if (pEntries[0].dwFlags & SOAPFLAG_OUT)
		{
			nIndex = 0;
		}
		else
		{
			nIndex = 1;
		}

		const _soapmapentry *pEntry = &pEntries[nIndex];
		ATLASSERT( pEntry->nHash != 0 );
		ATLASSERT( pEntry->pChain != NULL );

		if (S_OK != PushState(state.pvElement, pEntry->pChain, pEntry, pEntry->dwFlags, 0, pEntry->pChain->nElements))
		{
			return E_OUTOFMEMORY;
		}

		return ProcessParams(wszNamespaceUri, cchNamespaceUri, wszLocalName, cchLocalName, pAttributes);
	}

	HRESULT ProcessParams(
		const wchar_t  *wszNamespaceUri,
		int cchNamespaceUri,
	    const wchar_t  *wszLocalName,
	    int cchLocalName,
	    ISAXAttributes  *pAttributes) throw()
	{
		wszNamespaceUri;
		cchNamespaceUri;

		if (m_stateStack.IsEmpty())
		{
			// fatal internal error
			ATLASSERT( FALSE );

			return E_FAIL;
		}

		ParseState &state = GetState();

		ATLASSERT( state.pvElement != NULL );

//		const wchar_t * wszNamespace = GetNamespaceUri();
		HRESULT hr = E_FAIL;
		const _soapmapentry *pEntry = NULL;

		// if array element
		if (state.dwFlags & (SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR))
		{
			hr = ValidateArrayEntry(state, wszLocalName, cchLocalName);
			
			if (SUCCEEDED(hr))
			{
				pEntry = state.pEntry;
			}
			else
			{
				return hr;
			}
		}
		else // not an array element
		{
			// special-case for PAD with type=
			hr = ChainEntry(state, wszNamespaceUri, cchNamespaceUri,
				wszLocalName, cchLocalName, pAttributes);

			if (hr == S_FALSE)
			{
				hr = GetElementEntry(state, wszNamespaceUri, wszLocalName, cchLocalName, pAttributes, &pEntry);
				if (hr != S_OK)
				{
					if (hr == S_FALSE)
					{
						hr = S_OK;
					}
					return hr;
				}

				ATLASSERT( pEntry != NULL );
			}
			else
			{
				return hr;
			}
		}

		if (pEntry == NULL)
		{
			if (m_dwState == SOAP_HEADERS)
			{
				return CheckMustUnderstandHeader(pAttributes);
			}
			return E_FAIL;
		}

		// if is array
		if (((pEntry->pDims != NULL) || (pEntry->dwFlags & (SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR))) && 
			((state.dwFlags & (SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR)) == 0))
		{
			// get SOAP section-5 info (if it is there)
			return GetSection5Info(state, pEntry, pAttributes);
		}
		else
		{
			// if it is a simple type, push a new (ready) state on the stack
			void *pVal;
			if (state.dwFlags & (SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR))
			{
				pVal = UpdateArray(state, pEntry);
				ATLASSERT( pVal != NULL );
			}
			else
			{
				pVal = (((unsigned char *)state.pvElement)+pEntry->nOffset);
			}

			if (pEntry->nVal != SOAPTYPE_UNK)
			{
				// if it is a string
				if (pEntry->nVal == SOAPTYPE_STRING)
				{
					hr = ProcessString(pEntry, pAttributes, pVal);
				}
				else if (pEntry->nVal == SOAPTYPE_BLOB)
				{
					// if it is a BLOB, set to the CBlobHandler class
					hr = ProcessBlob(pVal);
				}
				else
				{
					// basic simple type
					if (S_OK != PushState(pVal, NULL, pEntry, SOAPFLAG_READYSTATE | pEntry->dwFlags))
					{
						hr = E_OUTOFMEMORY;
					}
				}
			}
			else
			{
				hr = ProcessUDT(pEntry, pVal, pAttributes);
			}
		}

		return hr;
	}

	size_t GetSizeIsValue(void *pvParam, const _soapmap *pMap, const _soapmapentry *pEntry) throw()
	{
		ATLASSERT( pvParam != NULL );
		ATLASSERT( pMap != NULL );
		ATLASSERT( pEntry != NULL );

		int nSizeIs = pEntry->nSizeIs;
		size_t nOffset = pMap->pEntries[nSizeIs].nOffset;
		void *pVal = ((unsigned char *)pvParam)+nOffset;

		__int64 nVal = 0;
		switch(pMap->pEntries[nSizeIs].nVal)
		{
			case SOAPTYPE_INTEGER: 
			case SOAPTYPE_NONPOSITIVEINTEGER:
			case SOAPTYPE_NEGATIVEINTEGER:
			case SOAPTYPE_LONG:
				nVal = *((__int64 *)pVal);
				break;
			case SOAPTYPE_INT:
				nVal = *((int *)pVal);
				break;
			case SOAPTYPE_SHORT:
				nVal = *((short *)pVal);
				break;
			case SOAPTYPE_BYTE:
				nVal = *((char *)pVal);
				break;
			case SOAPTYPE_NONNEGATIVEINTEGER:
			case SOAPTYPE_UNSIGNEDLONG:
				unsigned __int64 n;
				n = *((unsigned __int64 *)pVal);
				if (n > _I64_MAX)
				{
					// come on ...
					nVal = 0;
				}
				else
				{
					nVal = (__int64)n;
				}
				break;
			case SOAPTYPE_UNSIGNEDINT:
				nVal = *((unsigned int *)pVal);
				break;
			case SOAPTYPE_UNSIGNEDSHORT:
				nVal = *((unsigned short *)pVal);
				break;
			case SOAPTYPE_UNSIGNEDBYTE:
				nVal = *((unsigned char *)pVal);
				break;
			default:
				nVal = 0;
		}

		if (nVal < 0)
		{
			nVal = 0;
		}

		return (size_t) nVal;
	}

	HRESULT GenerateArrayInfo(const _soapmapentry *pEntry, const int *pDims, IWriteStream *pStream) throw()
	{
		ATLASSERT( pEntry != NULL );
		ATLASSERT( pStream != NULL );

		HRESULT hr = S_OK;
		if ((pEntry->nVal != SOAPTYPE_UNK) && (pEntry->nVal != SOAPTYPE_BLOB))
		{
			// xsd type
			hr = pStream->WriteStream(" soapenc:arrayType=\"xsd:", 
				sizeof("  soapenc:arrayType=\"xsd:")-1, NULL);
		}
		else
		{
			hr = pStream->WriteStream(" xmlns:q1=\"", sizeof(" xmlns:q1=\"")-1, NULL);
			if (SUCCEEDED(hr))
			{
				hr = pStream->WriteStream(GetNamespaceUriA(), -1, NULL);
				if (SUCCEEDED(hr))
				{
					hr = pStream->WriteStream(" soapenc:arrayType=\"q1:", 
						sizeof("  soapenc:arrayType=\"q1:")-1, NULL);
				}
			}
		}

		if (FAILED(hr))
		{
			return hr;
		}

		if (pEntry->nVal != SOAPTYPE_UNK)
		{
			hr = pStream->WriteStream(s_xsdNames[pEntry->nVal].szName , 
				s_xsdNames[pEntry->nVal].cchName, NULL);
		}
		else
		{
			ATLASSERT( pEntry->pChain != NULL );

			hr = pStream->WriteStream(pEntry->pChain->szName, pEntry->pChain->cchName, NULL);
		}

		if (FAILED(hr))
		{
			return hr;
		}

		hr = pStream->WriteStream("[", 1, NULL);
		if (FAILED(hr))
		{
			return hr;
		}

		CWriteStreamHelper s( pStream );
		for (int i=1; i<=pDims[0]; i++)
		{
			if (!s.Write(pDims[i]) || 
				((i < pDims[0]) && (S_OK != pStream->WriteStream(", ", 2, NULL))))
			{
				return E_FAIL;
			}
		}

		hr = pStream->WriteStream("]\"", 2, NULL);
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}

	HRESULT GenerateXSDWrapper(bool bStart, int nVal, IWriteStream *pStream) throw()
	{
		ATLASSERT( pStream != NULL );

		HRESULT hr = pStream->WriteStream((bStart != false) ? "<" : "</", 
			(bStart != false) ? 1 : 2, NULL);
		if (SUCCEEDED(hr))
		{
			hr = pStream->WriteStream(s_xsdNames[nVal].szName, 
				s_xsdNames[nVal].cchName, NULL);
			if (SUCCEEDED(hr))
			{
				hr = pStream->WriteStream(">", 1, NULL);
			}
		}

		return hr;
	}

	HRESULT GenerateGenericWrapper(bool bStart, const _soapmap *pMap, IWriteStream *pStream) throw()
	{
		ATLASSERT( pStream != NULL );
		ATLASSERT( pMap != NULL );

		HRESULT hr = pStream->WriteStream((bStart != false) ? "<" : "</", 
			(bStart != false) ? 1 : 2, NULL);
		if (SUCCEEDED(hr))
		{
			hr = pStream->WriteStream(pMap->szName, pMap->cchName, NULL);
			if (SUCCEEDED(hr))
			{
				hr = pStream->WriteStream(">", 1, NULL);
			}
		}

		return hr;
	}

	HRESULT GenerateHeaders(const _soapmap *pMap, IWriteStream *pStream) throw()
	{
		ATLASSERT( pStream != NULL );
		
		if (pMap == NULL)
		{
			// REVIEW: fail?
			return S_OK;
		}

		DWORD dwIncludeFlags = SOAPFLAG_OUT;
		if (m_bClient != false)
		{
			dwIncludeFlags = SOAPFLAG_IN;
		}

		// TODO: precalculate this in provider/sproxy
		size_t nCnt = 0;
		for (size_t i=0; pMap->pEntries[i].nHash != 0; i++)
		{
			if (pMap->pEntries[i].dwFlags & dwIncludeFlags)
			{
				nCnt++;
			}
		}

		// no headers to be sent
		if (nCnt == 0)
		{
			return S_OK;
		}

		HRESULT hr = pStream->WriteStream("<soap:Header ", sizeof("<soap:Header ")-1, NULL);
		if (SUCCEEDED(hr))
		{
			hr = pStream->WriteStream(" xmlns=\"", sizeof(" xmlns=\"")-1, NULL);
			if (SUCCEEDED(hr))
			{
				hr = pStream->WriteStream(GetNamespaceUriA(), -1, NULL);
				if (SUCCEEDED(hr))
				{
					hr = pStream->WriteStream("\">", 1, NULL);
					if (SUCCEEDED(hr))
					{
						hr = GenerateResponseHelper(pMap, GetHeaderValue(), pStream);
						if (SUCCEEDED(hr))
						{
							hr = pStream->WriteStream("</soap:Header>", 
								sizeof("</soap:Header>")-1, NULL);
						}
					}
				}
			}
		}

		return hr;
	}

	HRESULT GenerateResponseHelper(const _soapmap *pMap, void *pvParam, IWriteStream *pStream, 
		bool bArrayElement = false) throw()
	{
		ATLASSERT( pMap != NULL );
		ATLASSERT( pvParam != NULL );
		ATLASSERT( pStream != NULL );

		HRESULT hr = S_OK;
		if ((bArrayElement != false) && 
			((pMap->dwCallFlags & SOAPFLAG_PAD)==0))
		{
			// output type name
			hr = pStream->WriteStream("<", 1, NULL);
			if (SUCCEEDED(hr))
			{
				hr = pStream->WriteStream(pMap->szName, pMap->cchName, NULL);
				if (SUCCEEDED(hr))
				{
					if ((pMap->mapType == SOAPMAP_FUNC) && 
						(m_bClient == false) && 
						(pMap->dwCallFlags & SOAPFLAG_PID))
					{
						hr = pStream->WriteStream("Response", sizeof("Response")-1, NULL);
						if (FAILED(hr))
						{
							return hr;
						}
					}

					hr = pStream->WriteStream(" xmlns=\"", sizeof(" xmlns=\"")-1, NULL);
					if (SUCCEEDED(hr))
					{
						hr = pStream->WriteStream(GetNamespaceUriA(), -1, NULL);
						if (SUCCEEDED(hr))
						{
							hr = pStream->WriteStream("\">", sizeof("\">")-1, NULL);
						}
					}
				}

				if (FAILED(hr))
				{
					return hr;
				}
			}
		}

		ATLASSERT( pMap->pEntries != NULL );

		const _soapmapentry *pEntries = pMap->pEntries;
		size_t i;

		DWORD dwIncludeFlags;
		DWORD dwExcludeFlags;
		if (m_bClient != false)
		{
			dwIncludeFlags = SOAPFLAG_IN;
			dwExcludeFlags = SOAPFLAG_OUT;
		}
		else
		{
			dwIncludeFlags = SOAPFLAG_OUT;
			dwExcludeFlags = SOAPFLAG_IN;
		}

		for (i=0; pEntries[i].nHash != 0; i++)
		{
			if (((pEntries[i].dwFlags & dwIncludeFlags) ||
				((pEntries[i].dwFlags & dwExcludeFlags)==0)) &&
				((pEntries[i].dwFlags & SOAPFLAG_NOMARSHAL)==0))
			{
				// output name
				hr = pStream->WriteStream("<", 1, NULL);
				if (SUCCEEDED(hr))
				{
					if ((pEntries[i].dwFlags & SOAPFLAG_RETVAL)==0)
					{
						hr = pStream->WriteStream(pEntries[i].szField, pEntries[i].cchField, NULL);
					}
					else
					{
						// REVIEW: can probably remove this
						hr = pStream->WriteStream("return", sizeof("return")-1, NULL);
					}
				}
				if (FAILED(hr))
				{
					return hr;
				}
				if (pEntries[i].dwFlags & SOAPFLAG_MUSTUNDERSTAND)
				{
					// output mustUnderstand
					hr = pStream->WriteStream(" soap:mustUnderstand=\"1\"", sizeof(" soap:mustUnderstand=\"1\"")-1, NULL);
					if (FAILED(hr))
					{
						return hr;
					}
				}
				if (pEntries[i].dwFlags & SOAPFLAG_PAD)
				{
					// output mustUnderstand
					hr = pStream->WriteStream(" xmlns=\"", sizeof(" xmlns=\"")-1, NULL);
					if (SUCCEEDED(hr))
					{
						hr = pStream->WriteStream(GetNamespaceUriA(), -1, NULL);
						if (SUCCEEDED(hr))
						{
							hr = pStream->WriteStream("\"", sizeof("\"")-1, NULL);
						}
					}
					if (FAILED(hr))
					{
						return hr;
					}
				}
				
				size_t nElementSize = 0;
				size_t nCnt = 1;

				const int *pDims = NULL;
				int arrDims[2] = { 0 };

				bool bArray = (pEntries[i].dwFlags & (SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR)) != 0;

				if (bArray != false)
				{
					if (pEntries[i].dwFlags & SOAPFLAG_FIXEDARR)
					{
						pDims = pEntries[i].pDims;
					}
					else
					{
						ATLASSERT( pEntries[i].dwFlags & SOAPFLAG_DYNARR );

//						unsigned char **ppArr = (unsigned char **)GetState().pvElement;
//						// REVIEW: check for null here?
//						pVal = *ppArr;
						nCnt = GetSizeIsValue(pvParam, pMap, &pEntries[i]);

						if (nCnt == 0)
						{
							// REVIEW: should we fail here, or do something else?
							return E_FAIL;
						}

						arrDims[0] = 1;
						arrDims[1] = (int) nCnt;

						pDims = arrDims;
					}

					// output array information
					hr = GenerateArrayInfo(&pEntries[i], pDims, pStream);
					if (FAILED(hr))
					{
						return hr;
					}

					nCnt = AtlSoapGetArrayDims(pDims);

					ATLASSERT( nCnt != 0 );

					if (pEntries[i].nVal != SOAPTYPE_UNK)
					{
						nElementSize = AtlSoapGetElementSize((SOAPTYPES) pEntries[i].nVal);
					}
					else
					{
						ATLASSERT( pEntries[i].pChain != NULL );

						nElementSize = pEntries[i].pChain->nElementSize;
					}					
				}

				hr = pStream->WriteStream(">", 1, NULL);
				if (FAILED(hr))
				{
					return hr;
				}

				void *pvCurrent = ((unsigned char *)pvParam)+pEntries[i].nOffset;

				for (size_t nElement=0; nElement<nCnt; nElement++)
				{
					void *pVal;

					// get updated value
					if (bArray != false)
					{
						if (pEntries[i].dwFlags & SOAPFLAG_FIXEDARR)
						{
							unsigned char *ppArr = (unsigned char *)pvCurrent;
							pVal = ppArr+(nElement*nElementSize);
						}
						else
						{
							ATLASSERT( pEntries[i].dwFlags & SOAPFLAG_DYNARR );

							unsigned char **ppArr = (unsigned char **)pvCurrent;
							pVal = (*ppArr)+(nElement*nElementSize);
						}
					}
					else
					{
						pVal = pvCurrent;
					}

					if (pEntries[i].nVal != SOAPTYPE_UNK)
					{
						if (bArray != false)
						{
							hr = GenerateXSDWrapper(true, pEntries[i].nVal, pStream);
							if (FAILED(hr))
							{
								return hr;
							}
						}
						// TODO: generate xsi:null where appropriate
						hr = AtlSoapGenElementValue(pVal, pStream, (SOAPTYPES) pEntries[i].nVal);
						if ((SUCCEEDED(hr)) && (bArray != false))
						{
							hr = GenerateXSDWrapper(false, pEntries[i].nVal, pStream);
						}

						if (FAILED(hr))
						{
							return hr;
						}
					}
					else
					{
						ATLASSERT( pEntries[i].pChain != NULL );

						if (pEntries[i].pChain->mapType != SOAPMAP_ENUM)
						{
							// struct
							hr = GenerateResponseHelper(pEntries[i].pChain, pVal, pStream, bArray);
						}
						else
						{
							if (bArray != false)
							{
								hr = GenerateGenericWrapper(true, pEntries[i].pChain, pStream);
								if (FAILED(hr))
								{
									return hr;
								}
							}

							// enum
							int nVal = *((int *)pVal);
							const _soapmapentry *pEnumEntries = pEntries[i].pChain->pEntries;
							
							ATLASSERT( pEnumEntries != NULL );
							size_t j;
							for (j=0; pEnumEntries[j].nHash != 0; j++)
							{
								if (nVal == pEnumEntries[j].nVal)
								{
									hr = pStream->WriteStream(pEnumEntries[j].szField, pEnumEntries[j].cchField, NULL);
									if ((bArray != false) && (SUCCEEDED(hr)))
									{
										hr = GenerateGenericWrapper(false, pEntries[i].pChain, pStream);
									}
									break;
								}
							}
							if (pEnumEntries[j].nHash == 0)
							{
								hr = E_FAIL;
							}
						}
					}
				}

				// output element close
				if (SUCCEEDED(hr))
				{
					hr = pStream->WriteStream("</", 2, NULL);
					if (SUCCEEDED(hr))
					{
						if ((pEntries[i].dwFlags & SOAPFLAG_RETVAL)==0)
						{
							hr = pStream->WriteStream(pEntries[i].szField, pEntries[i].cchField, NULL);
						}
						else
						{
							// REVIEW: can probably remove this
							hr = pStream->WriteStream("return", sizeof("return")-1, NULL);
						}
						if (SUCCEEDED(hr))
						{
							hr = pStream->WriteStream(">", 1, NULL);
						}
					}
				}
			}

			if (FAILED(hr))
			{
				return hr;
			}
		}

		if ((bArrayElement != false) && 
			((pMap->dwCallFlags & SOAPFLAG_PAD)==0))
		{
			// output type name
			hr = pStream->WriteStream("</", 2, NULL);
			if (SUCCEEDED(hr))
			{
				hr = pStream->WriteStream(pMap->szName, pMap->cchName, NULL);
				if (SUCCEEDED(hr))
				{
					if ((pMap->mapType == SOAPMAP_FUNC) && 
						(m_bClient == false) && 
						(pMap->dwCallFlags & SOAPFLAG_PID))
					{
						hr = pStream->WriteStream("Response", sizeof("Response")-1, NULL);
						if (FAILED(hr))
						{
							return hr;
						}
					}
					hr = pStream->WriteStream(">", 1, NULL);
				}

				if (FAILED(hr))
				{
					return hr;
				}
			}
		}

		return S_OK;
	}

	void CleanupHelper(const _soapmap *pMap, void *pvParam) throw()
	{
		ATLASSERT( pMap != NULL );
		ATLASSERT( pvParam != NULL );

		ATLASSERT( pMap->pEntries != NULL );

		const _soapmapentry *pEntries = pMap->pEntries;
		size_t i;

		for (i=0; pEntries[i].nHash != 0; i++)
		{	
			if ((m_bClient != false) && (pEntries[i].dwFlags & SOAPFLAG_IN))
			{
				continue;
			}

			size_t nElementSize = 0;
			size_t nCnt = 1;

			const int *pDims = NULL;
			int arrDims[2] = { 0 };

			bool bArray = (pEntries[i].dwFlags & (SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR)) != 0;

			if (bArray != false)
			{
				if (pEntries[i].dwFlags & SOAPFLAG_FIXEDARR)
				{
					pDims = pEntries[i].pDims;
				}
				else
				{
					ATLASSERT( pEntries[i].dwFlags & SOAPFLAG_DYNARR );

//					unsigned char **ppArr = (unsigned char **)GetState().pvElement;
//					// REVIEW: check for null here?
//					pVal = *ppArr;
					nCnt = GetSizeIsValue(pvParam, pMap, &pEntries[i]);

//					if (nCnt == 0)
//					{
//						// REVIEW: should we fail here, or do something else?
//						return;
//					}

					arrDims[0] = 1;
					arrDims[1] = (int) nCnt;

					pDims = arrDims;
				}

				nCnt = AtlSoapGetArrayDims(pDims);

				ATLASSERT( nCnt != 0 );

				if (pEntries[i].nVal != SOAPTYPE_UNK)
				{
					nElementSize = AtlSoapGetElementSize((SOAPTYPES) pEntries[i].nVal);
				}
				else
				{
					ATLASSERT( pEntries[i].pChain != NULL );

					nElementSize = pEntries[i].pChain->nElementSize;
				}					
			}

			void *pvCurrent = ((unsigned char *)pvParam)+pEntries[i].nOffset;

			for (size_t nElement=0; nElement<nCnt; nElement++)
			{
				void *pVal;

				// get updated value
				if (bArray != false)
				{
					if (pEntries[i].dwFlags & SOAPFLAG_FIXEDARR)
					{
						unsigned char *ppArr = (unsigned char *)pvCurrent;
						pVal = ppArr+(nElement*nElementSize);
					}
					else
					{
						ATLASSERT( pEntries[i].dwFlags & SOAPFLAG_DYNARR );

						unsigned char **ppArr = (unsigned char **)pvCurrent;
						pVal = (*ppArr)+(nElement*nElementSize);
					}
				}
				else
				{
					pVal = pvCurrent;
				}

				if (pEntries[i].nVal != SOAPTYPE_UNK)
				{
					AtlSoapCleanupElement(pVal, (SOAPTYPES) pEntries[i].nVal);
				}
				else
				{
					ATLASSERT( pEntries[i].pChain != NULL );

					if (pEntries[i].pChain->mapType != SOAPMAP_ENUM)
					{
						CleanupHelper(pEntries[i].pChain, pVal);
					}
				}
			}

			if (pEntries[i].dwFlags & SOAPFLAG_DYNARR)
			{
				// free it
				unsigned char **ppArr = (unsigned char **)pvCurrent;
				
				ATLASSERT( ppArr != NULL );

				if (*ppArr != NULL)
				{
					m_pMemMgr->Free(*ppArr);
				}
			}
		}
	}

	const _soapmap * GetSoapMapFromName(
		const wchar_t * wszName, 
		int cchName = -1, 
		const wchar_t * wszNamespaceUri = NULL,
		int cchNamespaceUri = -1,
		int *pnVal = NULL,
		bool bHeader = false) throw()
	{
		cchNamespaceUri;

		const _soapmap ** pEntry = NULL;

		if (bHeader == false)
		{
			pEntry = GetFunctionMap();
		}
		else
		{
			pEntry = GetHeaderMap();
		}

		if (pEntry == NULL)
		{
			return NULL;
		}

		if (cchName < 0)
		{
			cchName = (int)wcslen(wszName);
		}

		ULONG nFunctionHash = AtlSoapHashStr(wszName, cchName);

		int i;
		for (i=0; pEntry[i] != NULL; i++)
		{
			if (nFunctionHash == pEntry[i]->nHash && 
				cchName == pEntry[i]->cchWName &&
				!wcsncmp(wszName, pEntry[i]->wszName, 
					cchName) && 
				!wcscmp(GetNamespaceUri(), wszNamespaceUri))
			{
				break;
			}
		}

		if (pnVal != NULL)
		{
			*pnVal = i;
		}
		return pEntry[i];
	}

protected:

	virtual const _soapmap ** GetFunctionMap() = 0;
	virtual const _soapmap ** GetHeaderMap() = 0;
	virtual const wchar_t * GetNamespaceUri() = 0;
	virtual const char * GetServiceName() = 0;
	virtual const char * GetNamespaceUriA() = 0;
	virtual HRESULT CallFunction(
		void *pvParam, 
		const wchar_t *wszLocalName, int cchLocalName,
		size_t nItem) = 0;
	virtual void * GetHeaderValue() = 0;

	ISAXXMLReader * SetReader(ISAXXMLReader *pReader) throw()
	{
		ISAXXMLReader *pPrevRdr = m_spReader;
		m_spReader = pReader;

		return pPrevRdr;
	}

	ISAXXMLReader * GetReader() throw()
	{
		return m_spReader;
	}

	HRESULT SetSoapMapFromName(
		const wchar_t * wszName, 
		int cchName = -1, 
		const wchar_t * wszNamespaceUri = NULL,
		int cchNamespaceUri = -1,
		bool bHeader = false) throw()
	{
		cchNamespaceUri;

		int nVal;
		const _soapmap *pMap = NULL;
		if (m_stateStack.GetCount() == 1)
		{
			ATLASSERT( GetState().pMap != NULL );
			nVal = (int) GetState().nAllocSize;
			ATLASSERT( GetFunctionMap() != NULL );
			pMap = GetFunctionMap()[nVal];
		}
		else
		{
			pMap = GetSoapMapFromName(wszName, cchName,
				wszNamespaceUri, cchNamespaceUri, &nVal, bHeader);
		}

		if (pMap == NULL)
		{
			return E_FAIL;
		}

		HRESULT hr = E_OUTOFMEMORY;

		// allocate the parameter struct

		void *pvParam = NULL;
		if (bHeader != false)
		{
			pvParam = GetHeaderValue();
		}
		else 
		{
			if (m_bClient == false)
			{
				m_pvParam = m_pMemMgr->Allocate(pMap->nElementSize);
			}
			pvParam = m_pvParam;
		}

		if (pvParam != NULL)
		{
			if (bHeader == false)
			{
				memset(pvParam, 0x00, pMap->nElementSize);
			}

			// push initial state

			if (m_stateStack.GetCount() != 0)
			{
				m_stateStack.RemoveAll();
			}

			hr = PushState(pvParam, pMap, NULL, 0, nVal, pMap->nElements);
			
			if (FAILED(hr))
			{
				if ((m_bClient == false) && (bHeader == false))
				{
					m_pMemMgr->Free(pvParam);
				}
			}
		}

		return hr;
	}

public:

	CSoapRootHandler(ISAXXMLReader *pReader = NULL) throw()
		: m_dwState(SOAP_START), m_nState(0), m_bErrorsOccurred(false), 
		m_pMemMgr(&m_crtHeap), m_spReader(pReader), m_bClient(false),
		m_pvParam(NULL)
	{
	}

	~CSoapRootHandler() throw()
	{
//		ATLASSERT( m_stateStack.IsEmpty() == true );
	}

	IAtlMemMgr * SetMemMgr(IAtlMemMgr *pMemMgr) throw()
	{
		IAtlMemMgr *pPrevMgr = m_pMemMgr;
		m_pMemMgr = pMemMgr;

		return pPrevMgr;
	}
	
	IAtlMemMgr * GetMemMgr() throw()
	{
		return m_pMemMgr;
	}

	//
	// implementation
	//

	HRESULT __stdcall startElement( 
	     const wchar_t  *wszNamespaceUri,
	     int cchNamespaceUri,
	     const wchar_t  *wszLocalName,
	     int cchLocalName,
	     const wchar_t  * wszQName,
	     int cchQName,
	     ISAXAttributes  *pAttributes) throw()
	{
		HRESULT hr = S_OK;
		switch (m_dwState)
		{
			case SOAP_PARAMS: case SOAP_HEADERS:
			{
				hr = ProcessParams(wszNamespaceUri, cchNamespaceUri, wszLocalName, 
					cchLocalName, pAttributes);

				break;
			}
			case SOAP_START: case SOAP_ENVELOPE: case SOAP_HEADERS_DONE:
			{
				ULONG nNamespaceHash = AtlSoapHashStr(wszNamespaceUri, 
					cchNamespaceUri);
				if (nNamespaceHash != SOAP_ENV)
				{
					return E_FAIL;
				}

				ULONG nElementHash = AtlSoapHashStr(wszLocalName, cchLocalName);

				if (nElementHash == ENVELOPE && 
					IsEqualElement(
						sizeof(SOAP_ENVELOPEA)-1, SOAP_ENVELOPEW,
						sizeof(SOAPENV_NAMESPACEA)-1, SOAPENV_NAMESPACEW,
						cchLocalName, wszLocalName,
						cchNamespaceUri, wszNamespaceUri))
				{
					// Envelope must be first element in package

					if (m_dwState != SOAP_START)
					{
						hr = E_FAIL;
					}
					m_dwState = SOAP_ENVELOPE;
				}
				else if (nElementHash == HEADER &&
					IsEqualElement(sizeof(SOAP_HEADERA)-1, SOAP_HEADERW,
						sizeof(SOAPENV_NAMESPACEA)-1, SOAPENV_NAMESPACEW,
						cchLocalName, wszLocalName,
						cchNamespaceUri, wszNamespaceUri))
				{
					// header map should have alredy been set
					if ((m_stateStack.GetCount() == 0) ||
						(m_dwState != SOAP_ENVELOPE))
					{
						hr = E_FAIL;
					}

					m_dwState = SOAP_HEADERS;
				}
				else if (nElementHash == BODY &&
					IsEqualElement(sizeof(SOAP_BODYA)-1, SOAP_BODYW,
						sizeof(SOAPENV_NAMESPACEA)-1, SOAPENV_NAMESPACEW,
						cchLocalName, wszLocalName,
						cchNamespaceUri, wszNamespaceUri))
				{
					// TODO: REVIEW: NOTE: depending on flags (RPC/PAD/PID), might
					// set to SOAP_PARAMS right here
					if (m_dwState == SOAP_START)
					{
						hr = E_FAIL;
					}
					m_dwState = SOAP_BODY;
				}

				break;
			}
			case SOAP_BODY:
			{
				hr = DispatchSoapCall(wszNamespaceUri, cchNamespaceUri,
						wszLocalName, cchLocalName);

				m_dwState = SOAP_PARAMS;

				if (SUCCEEDED(hr))
				{
					if (GetState().pMap->dwCallFlags & SOAPFLAG_PAD)
					{
						hr = startElement(wszNamespaceUri, cchNamespaceUri,
								wszLocalName, cchLocalName, wszQName, cchQName,
								pAttributes);
					}
				}
				
				break;
			}
			default:
			{
				__assume( 0 );
			}
		}

		return hr;
	}

	HRESULT __stdcall characters( 
	     const wchar_t  *wszChars,
	     int cchChars) throw()
	{
		// if it is a ready state, get the value
		if (m_stateStack.IsEmpty() == false)
		{
			ParseState& state = GetState();
			if (state.dwFlags & SOAPFLAG_READYSTATE)
			{
				if ((state.pMap == NULL) || (state.pMap->mapType != SOAPMAP_ENUM))
				{
					return AtlSoapGetElementValue(wszChars, cchChars, 
						state.pvElement, (SOAPTYPES)state.pEntry->nVal);
				}
				else
				{
					// enum

					ATLASSERT( state.pMap != NULL );
					ATLASSERT( state.pMap->pEntries != NULL );

					ULONG nHash = AtlSoapHashStr(wszChars, cchChars);
					const _soapmapentry *pEntries = state.pMap->pEntries;

					size_t i;
					for (i=0; pEntries[i].nHash != 0; i++)
					{
						if ((nHash == pEntries[i].nHash) &&
							(cchChars == pEntries[i].cchField) &&
							(!wcsncmp(wszChars, pEntries[i].wszField, cchChars)))
						{
							break;
						}
					}

					if (pEntries[i].nHash != 0)
					{
						*((int *)state.pvElement) = pEntries[i].nVal;
						state.nElement++;
						return S_OK;
					}

					// no matching enum entry found
					return E_FAIL;
				}
			}
		}
		
		// otherwise, ignore

		return S_OK;
	}

	HRESULT __stdcall endElement( 
	     const wchar_t  * wszNamespaceUri,
	     int cchNamespaceUri,
	     const wchar_t  * wszLocalName,
	     int cchLocalName,
	     const wchar_t  * /*wszQName*/,
	     int /*cchQName*/) throw()
	{
		// pop if appropriate

		if (m_stateStack.IsEmpty() != false)
		{
			return S_OK;
		}

		const ParseState& state = GetState();

		if ((m_dwState == SOAP_HEADERS) && (m_stateStack.GetCount() == 1))
		{
			if (IsEqualElement(sizeof(SOAP_HEADERA)-1, SOAP_HEADERW,
					sizeof(SOAPENV_NAMESPACEA)-1, SOAPENV_NAMESPACEW,
					cchLocalName, wszLocalName,
					cchNamespaceUri, wszNamespaceUri))
			{
				m_dwState = SOAP_HEADERS_DONE;
				if (m_bClient != false)
				{
					// set the param entry
					size_t nEntry = state.nAllocSize;
					const _soapmap ** pEntries = GetFunctionMap();
					ATLASSERT( pEntries != NULL );
					PopState();
					return PushState(m_pvParam, pEntries[nEntry], NULL, 0, nEntry, pEntries[nEntry]->nElements);
				}
				return S_OK;
			}

			// some sort of error
			return E_FAIL;
		}

		if (state.dwFlags & (SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR))
		{
			if (state.dwFlags & SOAPFLAG_READYSTATE)
			{
				PopState();
			}

			const ParseState& currstate = GetState();
			ATLASSERT( currstate.pEntry != NULL );

			if (currstate.pEntry->cchField == cchLocalName &&
				!wcsncmp(currstate.pEntry->wszField, wszLocalName, cchLocalName))
			{
				if (currstate.nExpectedElements != currstate.nElement)
				{
					// invalid number of elements
					return E_FAIL;
				}

				PopState();
			}
		}
		else
		{
			if (state.pMap != NULL)
			{
				if (state.nExpectedElements != state.nElement)
				{
					return E_FAIL;
				}
			}	
			PopState();
		}

		return S_OK;
	}

	HRESULT SetClientStruct(void *pvParam, int nMapIndex /*const _soapmap *pMap*/) throw()
	{
		ATLASSERT( pvParam != NULL );
		ATLASSERT( nMapIndex >= 0 );

		// this is the params struct
		// store for later use
		m_pvParam = pvParam;

		const _soapmap ** pEntries = GetHeaderMap();
		ATLASSERT( pEntries != NULL );

		// push header value
		return PushState(GetHeaderValue(), pEntries[nMapIndex], NULL, 0, nMapIndex, pEntries[nMapIndex]->nElements);
	}

	void ResetClientState(bool bFull = false) throw()
	{
		m_stateStack.RemoveAll();
		m_nState = 0;
		if (bFull != false)
		{
			m_dwState = SOAP_START;
			m_pvParam = NULL;
		}
	}

	HRESULT CreateReader() throw()
	{
		return m_spReader.CoCreateInstance(CLSID_SAXXMLReader30);
	}

	HRESULT InitializeSOAP(IServiceProvider *pProvider) throw()
	{
		HRESULT hr = S_OK;
		
		if (m_spReader.p == NULL)
		{
			hr = E_FAIL;
			if (pProvider != NULL)
			{
				IAtlMemMgr *pMemMgr = NULL;
				hr = pProvider->QueryService(__uuidof(IAtlMemMgr), 
					__uuidof(IAtlMemMgr), (void **)&pMemMgr);
				if ((SUCCEEDED(hr)) && (pMemMgr != NULL))
				{
					SetMemMgr(pMemMgr);
				}
	
				hr = pProvider->QueryService(__uuidof(ISAXXMLReader), 
					__uuidof(ISAXXMLReader), (void **)&m_spReader);
			}
	
			if (FAILED(hr))
			{
				hr = CreateReader();
			}
		}
		
		if (SUCCEEDED(hr))
		{
			hr = m_spReader->putContentHandler(this);
		}

		return hr;
	}

	void UninitializeSOAP() throw()
	{
		if (m_spReader.p != NULL)
		{
			m_spReader->putContentHandler(NULL);
			m_spReader.Release();
		}
	}

	virtual HRESULT DispatchSoapCall(const wchar_t *wszNamespaceUri,
		int cchNamespaceUri, const wchar_t *wszLocalName,
		int cchLocalName) throw()
	{
		return SetSoapMapFromName(wszLocalName, cchLocalName, 
			wszNamespaceUri, cchNamespaceUri);
	}

	virtual HRESULT BeginParse(IStream *pStream) throw()
	{
		ATLASSERT( pStream != NULL );

		CComVariant varStream;
		varStream = static_cast<IUnknown*>(pStream);

		return m_spReader->parse(varStream);
	}

	HRESULT CallFunctionInternal() throw()
	{
		HRESULT hr = E_FAIL;
		_ATLTRY
		{
			const ParseState& state = m_stateStack[0];
			hr = CallFunction(
				state.pvElement, 
				state.pMap->wszName,
				state.pMap->cchWName,
				state.nAllocSize);
		}
		_ATLCATCHALL()
		{
			hr = E_FAIL;
		}

		return hr;
	}

	virtual HRESULT GenerateResponse(IWriteStream *pStream) throw()
	{
		ATLASSERT( m_stateStack.IsEmpty() == false );
		ATLASSERT( m_nState == 0 );
		ATLASSERT( GetState().pMap != NULL );
		ATLASSERT( GetState().pvElement != NULL );

		const ParseState& state = GetState();

		
		const _soapmap *pHeaderMap = NULL;
		if (m_bClient == false)
		{
			const _soapmap **ppHeaderMap = GetHeaderMap();
			if (ppHeaderMap != NULL)
			{
				pHeaderMap = ppHeaderMap[state.nAllocSize];
			}
		}
		else
		{
			pHeaderMap = state.pMap;
		}

		const _soapmap *pFuncMap = NULL;
		if (m_bClient == false)
		{
			pFuncMap = state.pMap;
		}
		else
		{
			const _soapmap **ppFuncMap = GetFunctionMap();
			ATLASSERT( ppFuncMap != NULL );
			pFuncMap = ppFuncMap[state.nAllocSize];
		}

		HRESULT hr = pStream->WriteStream("<soap:Envelope "
			"xmlns:soap=\"" SOAPENV_NAMESPACEA "\" "
			"xmlns:xsi=\"" XSI_NAMESPACEA "\" "
			"xmlns:xsd=\"" XSD_NAMESPACEA "\"",
			sizeof("<soap:Envelope "
				"xmlns:soap=\"" SOAPENV_NAMESPACEA "\" "
				"xmlns:xsi=\"" XSI_NAMESPACEA "\" "
				"xmlns:xsd=\"" XSD_NAMESPACEA "\"")-1,
			NULL);

		if ((pFuncMap->dwCallFlags & SOAPFLAG_RPC) && (SUCCEEDED(hr)))
		{
			ATLASSERT( pFuncMap->dwCallFlags & SOAPFLAG_ENCODED );
			hr = pStream->WriteStream(" xmlns:soapenc=\"" SOAPENC_NAMESPACEA "\"", 
				sizeof(" xmlns:soapenc=\"" SOAPENC_NAMESPACEA "\"")-1, NULL);
		}

		if (SUCCEEDED(hr))
		{
			hr = pStream->WriteStream(">", 1, NULL);

			if (SUCCEEDED(hr))
			{
				// generate headers if necessary
				hr = GenerateHeaders(pHeaderMap, pStream);

				if (SUCCEEDED(hr))
				{
					hr = pStream->WriteStream("<soap:Body", sizeof("<soap:Body")-1, NULL);
					if ((pFuncMap->dwCallFlags & SOAPFLAG_RPC) && (SUCCEEDED(hr)))
					{
						hr = pStream->WriteStream(" soap:encodingStyle=\"" SOAPENC_NAMESPACEA "\"", 
							sizeof(" soap:encodingStyle=\"" SOAPENC_NAMESPACEA "\"")-1, NULL);
					}
					if (SUCCEEDED(hr))
					{
						hr = pStream->WriteStream(">", 1, NULL);
						if (SUCCEEDED(hr))
						{
							hr = GenerateResponseHelper(pFuncMap, m_pvParam, pStream, true);
							if (SUCCEEDED(hr))
							{
								hr = pStream->WriteStream("</soap:Body></soap:Envelope>", 
									sizeof("</soap:Body></soap:Envelope>")-1, NULL);
							}
						}
					}
				}
			}
		}

		return hr;
	}

	virtual void Cleanup() throw()
	{
		if (m_stateStack.IsEmpty() == false)
		{
			CleanupHelper(GetState().pMap, GetState().pvElement);
			if ((m_pvParam != NULL) && (m_bClient == false))
			{
				m_pMemMgr->Free(m_pvParam);
			}
			m_stateStack.RemoveAll();
		}
	}

	void SetClient(bool bClient) throw()
	{
		m_bClient = bClient;
	}

}; // class CSoapRootHandler

#define DECLARE_XSD_ENTRY( __name ) \
	{ L ## __name, __name, sizeof(__name)-1 },

__declspec(selectany) const CSoapRootHandler::XSDEntry CSoapRootHandler::s_xsdNames[] =
{
	DECLARE_XSD_ENTRY("string")
	DECLARE_XSD_ENTRY("boolean")
	DECLARE_XSD_ENTRY("float")
	DECLARE_XSD_ENTRY("double")
	DECLARE_XSD_ENTRY("decimal")
	DECLARE_XSD_ENTRY("timeDuration")
	DECLARE_XSD_ENTRY("recurringDuration")
	DECLARE_XSD_ENTRY("binary")
	DECLARE_XSD_ENTRY("URIReference")
	DECLARE_XSD_ENTRY("ID")
	DECLARE_XSD_ENTRY("IDREF")
	DECLARE_XSD_ENTRY("ENTITY")
	DECLARE_XSD_ENTRY("NOTATION")
	DECLARE_XSD_ENTRY("QName")
	DECLARE_XSD_ENTRY("CDATA")
	DECLARE_XSD_ENTRY("token")
	DECLARE_XSD_ENTRY("language")
	DECLARE_XSD_ENTRY("IDREFS")
	DECLARE_XSD_ENTRY("ENTITIES")
	DECLARE_XSD_ENTRY("NMTOKEN")
	DECLARE_XSD_ENTRY("NMTOKENS")
	DECLARE_XSD_ENTRY("Name")
	DECLARE_XSD_ENTRY("NCName")
	DECLARE_XSD_ENTRY("integer")
	DECLARE_XSD_ENTRY("nonPositiveInteger")
	DECLARE_XSD_ENTRY("negativeInteger")
	DECLARE_XSD_ENTRY("long")
	DECLARE_XSD_ENTRY("int")
	DECLARE_XSD_ENTRY("short")
	DECLARE_XSD_ENTRY("byte")
	DECLARE_XSD_ENTRY("nonNegativeInteger")
	DECLARE_XSD_ENTRY("unsignedLong")
	DECLARE_XSD_ENTRY("unsignedInt")
	DECLARE_XSD_ENTRY("unsignedShort")
	DECLARE_XSD_ENTRY("unsignedByte")
	DECLARE_XSD_ENTRY("positiveInteger")
	DECLARE_XSD_ENTRY("timeInstant")
	DECLARE_XSD_ENTRY("time")
	DECLARE_XSD_ENTRY("timePeriod")
	DECLARE_XSD_ENTRY("date")
	DECLARE_XSD_ENTRY("month")
	DECLARE_XSD_ENTRY("year")
	DECLARE_XSD_ENTRY("century")
	DECLARE_XSD_ENTRY("recurringDate")
	DECLARE_XSD_ENTRY("recurringDay")
	DECLARE_XSD_ENTRY("ATLSOAP_BLOB")
};

__declspec(selectany) CCRTHeap CSoapRootHandler::m_crtHeap;

template <typename THandler>
class CSoapHandler : 
	public CSoapRootHandler, 
	public CComObjectRootEx<CComMultiThreadModel>,
	public IRequestHandlerImpl<THandler>
{
private:

	HTTP_CODE m_hcErr;

public:

	void SetHttpError(HTTP_CODE hcErr) throw()
	{
		m_hcErr = hcErr;
	}

	HTTP_CODE SoapFault(CHttpResponse &HttpResponse, SOAP_ERROR_CODE errCode) throw()
	{
		HttpResponse.ClearHeaders();
		HttpResponse.ClearContent();
		HttpResponse.SetContentType("text/xml");
		HttpResponse.SetStatusCode(500);
		CSoapFault fault;
		fault.m_soapErrCode = errCode;
		fault.GenerateFault(&HttpResponse);
		return HTTP_ERROR(500, SUBERR_NO_PROCESS);
	}

	BEGIN_COM_MAP(CSoapHandler<THandler>)
		COM_INTERFACE_ENTRY(ISAXContentHandler)
		COM_INTERFACE_ENTRY(IRequestHandler)
	END_COM_MAP()

	HTTP_CODE HandleRequest(AtlServerRequest *pRequestInfo, IServiceProvider *pProvider) throw()
	{
		m_hcErr = HTTP_SUCCESS;
		m_spServiceProvider = pProvider;

		CHttpResponse HttpResponse(pRequestInfo->pServerContext);
		HRESULT hr = InitializeSOAP(m_spServiceProvider);
		if (FAILED(hr))
		{
			return SoapFault(HttpResponse, SOAP_E_SERVER);
		}

		// set the header map

		// REVIEW: technically not compliant here: we will allow omission of the 
		//         SOAPAction header and attempt to continue processing.
		//         We will fail iff the client attempts to send headers without
		//         sending the SOAPAction header

		char szBuf[ATL_URL_MAX_URL_LENGTH+1];
		szBuf[0] = '\0';
		DWORD dwLen = ATL_URL_MAX_URL_LENGTH;
		if (m_spServerContext->GetServerVariable("HTTP_SOAPACTION", szBuf, &dwLen) != FALSE)
		{
			// drop the last "
			szBuf[dwLen-2] = '\0';
			char *szMethod = strrchr(szBuf, '#');
			if (szMethod != NULL)
			{
				_ATLTRY
				{
					// ignore return code here (REVIEW above)
					SetSoapMapFromName(CA2W( szMethod+1 ), -1, GetNamespaceUri(), -1, true);
				}
				_ATLCATCHALL()
				{
					return HTTP_ERROR(500, ISE_SUBERR_OUTOFMEM);
				}
			}
		}

		CStreamOnServerContext s(pRequestInfo->pServerContext);

		// REVIEW: add these in under #ifdef _DEBUG ?
//		CSAXSoapErrorHandler err;
//		GetReader()->putErrorHandler(&err);

		hr = BeginParse(&s);
		if (FAILED(hr))
		{
			return SoapFault(HttpResponse, SOAP_E_CLIENT);
		}

		hr = CallFunctionInternal();
		if (FAILED(hr))
		{
			Cleanup();
			HttpResponse.ClearHeaders();
			HttpResponse.ClearContent();
			if (m_hcErr != HTTP_SUCCESS)
			{
				HttpResponse.SetStatusCode(HTTP_ERROR_CODE(m_hcErr));
				return HTTP_SUCCESS_NO_PROCESS;
			}
			HttpResponse.SetStatusCode(500);
			GenerateAppError(&HttpResponse, hr);
			return HTTP_ERROR(500, SUBERR_NO_PROCESS);
		}

		HttpResponse.SetContentType("text/xml");
		hr = GenerateResponse(&HttpResponse);
		Cleanup();
		if (FAILED(hr))
		{
			return SoapFault(HttpResponse, SOAP_E_SERVER);
		}

		return HTTP_SUCCESS;
	}

	virtual ATL_NOINLINE HRESULT GenerateAppError(IWriteStream *pStream, HRESULT hr) throw()
	{
		if (pStream == NULL)
		{
			return E_INVALIDARG;
		}

		LPWSTR pwszMessage = NULL;
		DWORD dwLen = ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, hr, 0, (LPWSTR) &pwszMessage, 0, NULL);
		
		if (dwLen == 0)
		{
			pwszMessage = L"Application Error";
		}

		_ATLTRY
		{
			CSoapFault fault;
			fault.m_soapErrCode = SOAP_E_SERVER;
			fault.m_strDetail = (L"<detail xmlns:e=\"" ATLSOAP_GENERIC_NAMESPACE L"\"\n"
	            L"xmlns:xsd=\"" XSD_NAMESPACEW L"\"\n"
				L"xsd:type=\"e:ApplicationFault\">"
				L"<message>");
			fault.m_strDetail += pwszMessage;
			fault.m_strDetail.AppendFormat(L"</message>\n<errorcode>0x%08X</errorcode>\n</detail>\n", hr);
			hr = fault.GenerateFault(pStream);
		}
		_ATLCATCHALL()
		{
			hr = E_OUTOFMEMORY;
		}

		if (dwLen != 0)
		{
			::LocalFree(pwszMessage);
		}

		return hr;
	}

	void UninitializeHandler() throw()
	{
		UninitializeSOAP();
	}
};

template <typename TSocketClass = ZEvtSyncSocket>
class CSoapSocketClientT
{
private:

	CUrl m_url;
	CWriteStreamOnCString m_writeStream;
	CReadStreamOnSocket<TSocketClass> m_readStream;
	DWORD m_dwTimeout;

protected:

	virtual HRESULT GetClientReader(ISAXXMLReader **pReader) throw()
	{
		if (pReader == NULL)
		{
			return E_POINTER;
		}
		*pReader = NULL;

		CComPtr<ISAXXMLReader> spReader;
		HRESULT hr = spReader.CoCreateInstance(__uuidof(SAXXMLReader30));
		if (SUCCEEDED(hr))
		{
			*pReader = spReader.Detach();
		}
		return hr;
	}

public:

	// note : not shared across stock client implementations
	CAtlHttpClientT<TSocketClass> m_socket;

	CSoapFault m_fault;

	// constructor
	CSoapSocketClientT(LPCTSTR szUrl) throw()
		: m_dwTimeout(0)
	{
		m_url.CrackUrl(szUrl);
	}

	CSoapSocketClientT(LPCTSTR szServer, LPCTSTR szUri, ATL_URL_PORT nPort=80) throw()
	{
		ATLASSERT( szServer != NULL );
		ATLASSERT( szUri != NULL );

		m_url.SetUrlPath(szUri);
		m_url.SetHostName(szServer);
		m_url.SetPortNumber(nPort);
	}

	~CSoapSocketClientT() throw()
	{
		CleanupClient();
	}

	IWriteStream * GetWriteStream() throw()
	{
		return &m_writeStream;
	}

	HRESULT GetReadStream(IStream **ppStream) throw()
	{
		if (ppStream == NULL)
		{
			return E_POINTER;
		}

		*ppStream = &m_readStream;
		return S_OK;
	}

	void CleanupClient() throw()
	{
		m_writeStream.Cleanup();
	}

	HRESULT SendRequest(LPCTSTR szAction) throw()
	{
		HRESULT hr = E_FAIL;
		_ATLTRY
		{
			m_fault.Clear();
			// create extra headers to send with request
			CString strExtraHeaders(szAction);
			strExtraHeaders.Append(_T("Accept: text/xml\r\n"), sizeof("Accept: text/xml\r\n")-1);
			CAtlNavigateData navData;
			navData.SetMethod(ATL_HTTP_METHOD_POST);
			navData.SetPort(m_url.GetPortNumber());
			navData.SetExtraHeaders(strExtraHeaders);
			navData.SetData((LPBYTE)(LPCSTR)m_writeStream.m_str, m_writeStream.m_str.GetLength(), _T("text/xml"));
			
			if (m_dwTimeout != 0)
			{
				navData.SetSocketTimeout(m_dwTimeout);
			}

			if (m_socket.Navigate(&m_url, &navData) != false)
			{
				hr = (m_readStream.Init(&m_socket) != FALSE ? S_OK : E_FAIL);
			}
			else if (GetStatusCode() == 500)
			{
				// if returned 500, get the SOAP fault
				if (m_readStream.Init(&m_socket) != FALSE)
				{
					CComPtr<ISAXXMLReader> spReader;
					if (SUCCEEDED(GetClientReader(&spReader)))
					{
						CComPtr<IStream> spReadStream;
						if (SUCCEEDED(GetReadStream(&spReadStream)))
						{
							m_fault.ParseFault(spReadStream, spReader);
						}
					}
				}
			}
		}
		_ATLCATCHALL()
		{
			hr = E_FAIL;
		}

		return hr;
	}

	HRESULT SetUrl(LPCTSTR szUrl) throw()
	{
		return (m_url.CrackUrl(szUrl) != FALSE) ? S_OK : E_FAIL;
	}

	HRESULT GetUrl(LPTSTR szUrl, LPDWORD pdwLen) throw()
	{
		if ((szUrl == NULL) || (pdwLen == NULL))
		{
			return E_INVALIDARG;
		}

		return (m_url.CreateUrl(szUrl, pdwLen) != FALSE) ? S_OK : E_FAIL;
	}


	HRESULT SetProxy(LPCTSTR szProxy = NULL, short nProxyPort = 80) throw()
	{
		BOOL bRet;
		_ATLTRY
		{
			bRet = m_socket.SetProxy(szProxy, nProxyPort);
		}
		_ATLCATCHALL()
		{
			bRet = FALSE;
		}

		return (bRet != FALSE) ? S_OK : E_FAIL;
	}

	void SetTimeout(DWORD dwTimeout) throw()
	{
		m_dwTimeout = dwTimeout;
	}

	int GetStatusCode() throw()
	{
		return m_socket.GetStatus();
	}

}; // CSoapSocketClientT

#ifndef ATLSOAP_NOWININET

class CReadStreamOnInet : public IStreamImpl
{
public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_IStream) ||
			InlineIsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = static_cast<IStream *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

private:

	HINTERNET m_hFile;

public:

	CReadStreamOnInet() throw()
		:m_hFile(NULL)
	{
	}

	void Init(HINTERNET hFile) throw()
	{
		m_hFile = hFile;
	}

	HRESULT STDMETHODCALLTYPE Read(void *pDest, ULONG dwMaxLen, ULONG *pdwRead) throw()
	{
		BOOL bRet = InternetReadFile(m_hFile, pDest, dwMaxLen, pdwRead);
		return (bRet != FALSE) ? S_OK : E_FAIL;
	}

}; // CStreamOnInet

class CSoapWininetClient
{
private:
	
	CUrl m_url;
	CWriteStreamOnCString m_writeStream;
	CReadStreamOnInet m_readStream;
	CString m_strProxy;
	DWORD m_dwTimeout;

	void CloseAll() throw()
	{
		if (m_hRequest != NULL)
		{
			InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
		if (m_hConnection != NULL)
		{
			InternetCloseHandle(m_hConnection);
			m_hConnection = NULL;
		}
		if (m_hInternet != NULL)
		{
			InternetCloseHandle(m_hInternet);
			m_hInternet = NULL;
		}
	}

	HRESULT ConnectToServer() throw()
	{
		if (m_hConnection != NULL)
		{
			return S_OK;
		}

		m_hInternet = InternetOpen(ATLSOAPINET_CLIENT, INTERNET_OPEN_TYPE_PRECONFIG,
			m_strProxy.GetLength() ? (LPCTSTR) m_strProxy : NULL,
			NULL, 0);

		if (m_hInternet != NULL)
		{
			if (m_dwTimeout != 0)
			{
				InternetSetOption(m_hInternet, INTERNET_OPTION_CONNECT_TIMEOUT,
					&m_dwTimeout, sizeof(m_dwTimeout)/sizeof(TCHAR));
				InternetSetOption(m_hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT,
					&m_dwTimeout, sizeof(m_dwTimeout)/sizeof(TCHAR));
				InternetSetOption(m_hInternet, INTERNET_OPTION_SEND_TIMEOUT,
					&m_dwTimeout, sizeof(m_dwTimeout)/sizeof(TCHAR));
			}
			m_hConnection = InternetConnect(m_hInternet, m_url.GetHostName(), 
				(INTERNET_PORT) m_url.GetPortNumber(), NULL, NULL,
				INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_UI, NULL);

			if (m_hConnection != NULL)
			{
				return S_OK;
			}
		}
		CloseAll();
		return E_FAIL;
	}

protected:

	virtual HRESULT GetClientReader(ISAXXMLReader **pReader) throw()
	{
		if (pReader == NULL)
		{
			return E_POINTER;
		}
		*pReader = NULL;

		CComPtr<ISAXXMLReader> spReader;
		HRESULT hr = spReader.CoCreateInstance(__uuidof(SAXXMLReader30));
		if (SUCCEEDED(hr))
		{
			*pReader = spReader.Detach();
		}
		return hr;
	}

public:

	// note : not shared across stock client implementations
	HINTERNET m_hInternet;
	HINTERNET m_hConnection;
	HINTERNET m_hRequest;

	CSoapFault m_fault;

	CSoapWininetClient(LPCTSTR szUrl) throw()
		:m_hInternet(NULL), m_hConnection(NULL), m_hRequest(NULL), m_dwTimeout(0)
	{
		m_url.CrackUrl(szUrl);
		SetProxy();
	}

	CSoapWininetClient(LPCTSTR szServer, LPCTSTR szUri, short nPort=80) throw()
		:m_hInternet(NULL), m_hConnection(NULL), m_hRequest(NULL)
	{
		m_url.SetHostName(szServer);
		m_url.SetUrlPath(szUri);
		m_url.SetPortNumber((ATL_URL_PORT) nPort);
	}

	~CSoapWininetClient() throw()
	{
		CleanupClient();
	}

	IWriteStream * GetWriteStream() throw()
	{
		return &m_writeStream;
	}

	HRESULT GetReadStream(IStream **ppStream) throw()
	{
		if (ppStream == NULL)
		{
			return E_POINTER;
		}

		*ppStream = &m_readStream;
		return S_OK;
	}

	void CleanupClient() throw()
	{
		m_writeStream.Cleanup();
		InternetCloseHandle(m_hRequest);
		m_hRequest = NULL;
	}

	HRESULT SendRequest(LPCTSTR szAction) throw()
	{
		m_fault.Clear();
		if (ConnectToServer() != S_OK)
		{
			return E_FAIL;
		}

		static LPCTSTR s_szAcceptTypes[] = { _T("text/*"), NULL };
		m_hRequest = HttpOpenRequest(m_hConnection, _T("POST"), 
			m_url.GetUrlPath(), _T("HTTP/1.0"), NULL,
			s_szAcceptTypes, 
			INTERNET_FLAG_NO_UI | ((m_url.GetPortNumber() == ATL_URL_DEFAULT_HTTPS_PORT) ? INTERNET_FLAG_SECURE : 0)
			, NULL);

		if (m_hRequest != NULL)
		{
			if (FALSE != HttpSendRequest(m_hRequest, szAction, (DWORD) _tcslen(szAction),
				(void *)(LPCSTR)m_writeStream.m_str, m_writeStream.m_str.GetLength()))
			{
				m_readStream.Init(m_hRequest);
				if (GetStatusCode() != HTTP_STATUS_SERVER_ERROR)
				{
					return S_OK;
				}
				else
				{
					CComPtr<ISAXXMLReader> spReader;
					if (SUCCEEDED(GetClientReader(&spReader)))
					{
						CComPtr<IStream> spReadStream;
						if (SUCCEEDED(GetReadStream(&spReadStream)))
						{
							m_fault.ParseFault(spReadStream, spReader);
						}
					}
				}
			}
		}

		return E_FAIL;
	}

	HRESULT SetUrl(LPCTSTR szUrl) throw()
	{
		CloseAll();
		return (m_url.CrackUrl(szUrl) != FALSE ? S_OK : E_FAIL);
	}

	HRESULT GetUrl(LPTSTR szUrl, LPDWORD pdwLen) throw()
	{
		if ((szUrl == NULL) || (pdwLen == NULL))
		{
			return E_INVALIDARG;
		}

		return (m_url.CreateUrl(szUrl, pdwLen) != FALSE) ? S_OK : E_FAIL;
	}

	HRESULT SetProxy(LPCTSTR szProxy = NULL, short nProxyPort = 80) throw()
	{
		nProxyPort;
		_ATLTRY
		{
			m_strProxy = szProxy;
		}
		_ATLCATCHALL()
		{
			return E_OUTOFMEMORY;
		}
		return S_OK;
	}

	void SetTimeout(DWORD dwTimeout) throw()
	{
		m_dwTimeout = dwTimeout;
	}

	int GetStatusCode() throw()
	{
		DWORD dwLen = 256;
		TCHAR szBuf[256];
		if (HttpQueryInfo(m_hRequest, HTTP_QUERY_STATUS_CODE, szBuf, &dwLen, NULL))
		{
			return _ttoi(szBuf);
		}
		return 0;
	}
}; // CSoapWininetClient
#endif

#ifndef ATLSOAP_NOMSXML_INET
class CSoapMSXMLInetClient
{
	CUrl m_url;
	CWriteStreamOnCString m_writeStream;
	DWORD m_dwTimeout;

	HRESULT ConnectToServer() throw()
	{
		TCHAR szURL[ATL_URL_MAX_URL_LENGTH];
		DWORD dwLen = ATL_URL_MAX_URL_LENGTH;
		HRESULT hr = E_FAIL;

		if (m_spHttpRequest)
			return S_OK;

		if (!m_url.CreateUrl(szURL, &dwLen))
			return E_FAIL;


		hr = m_spHttpRequest.CoCreateInstance(__uuidof(ServerXMLHTTP30));
		if (hr != S_OK)
			return hr;
	
		CComVariant vEmpty;
		hr = m_spHttpRequest->open( CComBSTR(L"POST"),
									CComBSTR(szURL),
									CComVariant(VARIANT_BOOL(VARIANT_FALSE)),
									vEmpty,
									vEmpty );
		if (hr != S_OK)
		{
			m_spHttpRequest.Release();
		   return hr;
		}
		
		return S_OK;
	}

protected:

	virtual HRESULT GetClientReader(ISAXXMLReader **pReader) throw()
	{
		if (pReader == NULL)
		{
			return E_POINTER;
		}
		*pReader = NULL;

		CComPtr<ISAXXMLReader> spReader;
		HRESULT hr = spReader.CoCreateInstance(__uuidof(SAXXMLReader30));
		if (SUCCEEDED(hr))
		{
			*pReader = spReader.Detach();
		}
		return hr;
	}

public:

	// note : not shared across stock client implementations
	CComPtr<IServerXMLHTTPRequest> m_spHttpRequest;

	CSoapFault m_fault;

	CSoapMSXMLInetClient(LPCTSTR szUrl) throw()
		:m_dwTimeout(0)
	{
		m_url.CrackUrl(szUrl);
	}

	CSoapMSXMLInetClient(LPCTSTR szServer, LPCTSTR szUri, short nPort=80) throw()
	{
		m_url.SetHostName(szServer);
		m_url.SetUrlPath(szUri);
		m_url.SetPortNumber((ATL_URL_PORT) nPort);
	}

	~CSoapMSXMLInetClient() throw()
	{
		CleanupClient();
	}

	IWriteStream * GetWriteStream() throw()
	{
		return &m_writeStream;
	}

	HRESULT GetReadStream(IStream **ppStream) throw()
	{
		if (ppStream == NULL)
		{
			return E_POINTER;
		}

		*ppStream = NULL;
		HRESULT hr = E_FAIL;

		if (m_spHttpRequest)
		{
			VARIANT vResponseStream;
			VariantInit(&vResponseStream);
			hr = m_spHttpRequest->get_responseStream(&vResponseStream);
			if (S_OK == hr)
			{
				hr = E_FAIL;
				if (vResponseStream.punkVal != NULL)
				{
					// we return the refcount with the pointer!
					*ppStream = (IStream*)vResponseStream.punkVal;
					hr = S_OK;
				}
			}
		}
		return hr;
	}

	void CleanupClient() throw()
	{
		m_writeStream.Cleanup();
		m_spHttpRequest.Release();
	}

	HRESULT SendRequest(LPCTSTR szAction) throw()
	{
		m_fault.Clear();

		if (ConnectToServer() != S_OK)
		{
			return E_FAIL;
		}
		
		// set the action header
		LPCTSTR szColon = _tcschr(szAction, _T(':'));
		if (szColon != NULL)
		{
			do
			{
				szColon++;
			} while (_istspace(*szColon));

			if (FAILED(m_spHttpRequest->setRequestHeader(
						CComBSTR( L"SOAPAction" ), CComBSTR( szColon ))))
			{
				return E_FAIL;
			}
		} // if SOAPAction header not properly formed, attempt to send anyway
		
		// set timeout
		if (m_dwTimeout != 0)
		{
			long nTimeout = (long) m_dwTimeout;
			m_spHttpRequest->setTimeouts(nTimeout, nTimeout, nTimeout, nTimeout);
			// reset timeout
			m_dwTimeout = 0;
		}

		CComVariant vBody(m_writeStream.m_str);
		HRESULT hr = m_spHttpRequest->send(vBody);
		if ((SUCCEEDED(hr)) && (GetStatusCode() == 500))
		{
			hr = E_FAIL;
			CComPtr<ISAXXMLReader> spReader;
			if (SUCCEEDED(GetClientReader(&spReader)))
			{
				CComPtr<IStream> spReadStream;
				if (SUCCEEDED(GetReadStream(&spReadStream)))
				{
					m_fault.ParseFault(spReadStream, spReader);
				}
			}
		}

		return hr;
	}

	HRESULT SetUrl(LPCTSTR szUrl) throw()
	{
		CleanupClient();
		return (m_url.CrackUrl(szUrl) != FALSE ? S_OK : E_FAIL);
	}

	HRESULT GetUrl(LPTSTR szUrl, LPDWORD pdwLen) throw()
	{
		if ((szUrl == NULL) || (pdwLen == NULL))
		{
			return E_INVALIDARG;
		}

		return (m_url.CreateUrl(szUrl, pdwLen) != FALSE) ? S_OK : E_FAIL;
	}

	void SetTimeout(DWORD dwTimeout) throw()
	{
		m_dwTimeout = dwTimeout;
	}

	int GetStatusCode() throw()
	{
		long lStatus;
		if (m_spHttpRequest->get_status(&lStatus) == S_OK)
		{
			return (int) lStatus;
		}
		return 0;
	}
}; // CSoapMSXMLInetClient
#endif


class _CSDLGenerator : public ITagReplacerImpl<_CSDLGenerator>
{
private:

	typedef CAtlMap<CStringA, const _soapmap *, CStringElementTraits<CStringA> >  WSDLMAP;
	typedef CAtlMap<CStringA, const _soapmapentry *, CStringElementTraits<CStringA> > HEADERMAP;

	HRESULT GenerateWSDLHelper(const _soapmap *pMap, WSDLMAP& structMap, WSDLMAP& enumMap) throw()
	{
		ATLASSERT( pMap != NULL );

		const _soapmapentry *pEntries = pMap->pEntries;
		ATLASSERT( pEntries != NULL );

		HRESULT hr = S_OK;

		for (int i=0; pEntries[i].nHash != 0; i++)
		{
			if (pEntries[i].nVal == SOAPTYPE_UNK)
			{
				ATLASSERT( pEntries[i].pChain != NULL );

				_ATLTRY
				{
					POSITION pos = NULL;
					CStringA strName(pEntries[i].pChain->szName, pEntries[i].pChain->cchName);
					if (pEntries[i].pChain->mapType == SOAPMAP_STRUCT)
					{
						pos = structMap.SetAt(strName, pEntries[i].pChain);
					}
					else if (pEntries[i].pChain->mapType == SOAPMAP_ENUM)
					{
						pos = enumMap.SetAt(strName, pEntries[i].pChain);
					}
					if (pos == NULL)
					{
						hr = E_OUTOFMEMORY;
						break;
					}
				}
				_ATLCATCHALL()
				{
					hr = E_OUTOFMEMORY;
					break;
				}

				hr = GenerateWSDLHelper(pEntries[i].pChain, structMap, enumMap);
				if (FAILED(hr))
				{
					break;
				}
			}
		}

		return hr;
	}

	HTTP_CODE IsUDT(const _soapmapentry *pEntry) throw()
	{
		ATLASSERT( pEntry != NULL );
		return ( (pEntry->nVal != SOAPTYPE_UNK) && (pEntry->nVal != SOAPTYPE_BLOB) ) ? HTTP_S_FALSE : HTTP_SUCCESS;
	}

	HTTP_CODE GetSoapDims(const _soapmapentry *pEntry) throw()
	{
		ATLASSERT( pEntry != NULL );
		if (pEntry->pDims[0] != 0)
		{
			if (SUCCEEDED(m_pWriteStream->WriteStream("[", 1, NULL)))
			{
				for (int i=1; i<=pEntry->pDims[0]; i++)
				{
					if (m_writeHelper.Write(pEntry->pDims[i]) != FALSE)
					{
						if (i < pEntry->pDims[0])
						{
							if (FAILED(m_pWriteStream->WriteStream(", ", 2, NULL)))
							{
								return HTTP_FAIL;
							}
						}
					}
				}
				if (SUCCEEDED(m_pWriteStream->WriteStream("]", 1, NULL)))
				{
					return HTTP_SUCCESS;
				}
			}
		}
		return HTTP_FAIL;
	}

	const _soapmap **m_pFuncs;
	const _soapmap **m_pHeaders;
	int m_nFunc;
	int m_nParam;
	int m_nHeader;
	WSDLMAP m_structMap;
	WSDLMAP m_enumMap;
	POSITION m_currUDTPos;
	int m_nCurrUDTField;

	HEADERMAP m_headerMap;
	POSITION m_currHeaderPos;

	CWriteStreamHelper m_writeHelper;

	CStringA m_strServiceName;
	CStringA m_strNamespaceUri;

	IWriteStream *m_pWriteStream;

	CComPtr<IHttpServerContext> m_spHttpServerContext;

protected:

	void SetWriteStream(IWriteStream *pStream) throw()
	{
		m_pWriteStream = pStream;
		m_writeHelper.Attach(m_pWriteStream);
	}

	void SetHttpServerContext(IHttpServerContext *pServerContext) throw()
	{
		m_spHttpServerContext = pServerContext;
	}

	static HTTP_CODE GetSoapType(int nVal, IWriteStream *pStream) throw()
	{
		return (pStream->WriteStream(CSoapRootHandler::s_xsdNames[nVal].szName, 
			CSoapRootHandler::s_xsdNames[nVal].cchName, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_FAIL;
	}
	

	HRESULT InitializeSDL(CSoapRootHandler *pHdlr) throw()
	{
		m_pFuncs = pHdlr->GetFunctionMap();

		if (m_pFuncs == NULL)
		{
			return E_FAIL;
		}

		size_t i;
		for (i=0; m_pFuncs[i] != NULL; i++)
		{
			const _soapmap *pMap = m_pFuncs[i];
			HRESULT hr = GenerateWSDLHelper(pMap, m_structMap, m_enumMap);
			if (FAILED(hr))
			{
				return hr;
			}
		}

		m_pHeaders = pHdlr->GetHeaderMap();
		if (m_pHeaders != NULL)
		{
			for (i=0; m_pHeaders[i] != NULL; i++)
			{
				const _soapmap *pMap = m_pHeaders[i];
				HRESULT hr = GenerateWSDLHelper(pMap, m_structMap, m_enumMap);
				if (FAILED(hr))
				{
					return hr;
				}
			}

			for (i=0; m_pHeaders[i] != NULL; i++)
			{
				const _soapmap *pMap = m_pHeaders[i];
				for (size_t j=0; pMap->pEntries[j].nHash != 0; j++)
				{
					HRESULT hr = S_OK;
					_ATLTRY
					{
						if (m_headerMap.SetAt(pMap->pEntries[j].szField, &pMap->pEntries[j]) == NULL)
						{
							hr = E_OUTOFMEMORY;
						}
					}
					_ATLCATCHALL()
					{
						hr = E_OUTOFMEMORY;
					}
					if (FAILED(hr))
					{
						return hr;
					}
				}
			}
		}

		_ATLTRY
		{
			m_strServiceName = pHdlr->GetServiceName();
			m_strNamespaceUri = pHdlr->GetNamespaceUriA();
		}
		_ATLCATCHALL()
		{
			return E_OUTOFMEMORY;
		}
		return S_OK;
	}

	virtual const char * GetHandlerName() throw() = 0;

public:

	_CSDLGenerator() throw()
		:m_pFuncs(NULL), m_nFunc(-1), m_nParam(-1),
		 m_currUDTPos(NULL), m_nCurrUDTField(-1),
		 m_pWriteStream(NULL), m_nHeader(-1), m_currHeaderPos(NULL)
	{
	}

	HTTP_CODE OnGetURL() throw()
	{
		char szURL[ATL_URL_MAX_URL_LENGTH];
		DWORD dwUrlSize = sizeof(szURL);
		char szServer[ATL_URL_MAX_HOST_NAME_LENGTH];
		DWORD dwServerSize = sizeof(szServer);
		char szHttps[16];
		DWORD dwHttpsLen = sizeof(szHttps);

		if (m_spHttpServerContext->GetServerVariable("URL", szURL, &dwUrlSize) != FALSE)
		{
			if (m_spHttpServerContext->GetServerVariable("SERVER_NAME", szServer, &dwServerSize) != FALSE)
			{
				bool bHttps = false;
				if ((m_spHttpServerContext->GetServerVariable("HTTPS", szHttps, &dwHttpsLen) != FALSE) &&
					(!_stricmp(szHttps, "ON")))
				{
					bHttps = true;
				}
				_ATLTRY
				{
					CStringA strUrl;
					strUrl.Format("http%s://%s%s?Handler=%s", bHttps ? "s" : "", szServer, szURL, GetHandlerName());
					if (S_OK == m_pWriteStream->WriteStream(strUrl, strUrl.GetLength(), NULL))
					{
						return HTTP_SUCCESS;
					}
				}
				_ATLCATCHALL()
				{
					return HTTP_FAIL;
				}
			}
		}
		return HTTP_FAIL;
	}

	HTTP_CODE OnGetNamespace() throw()
	{
		return (m_pWriteStream->WriteStream(m_strNamespaceUri, 
			m_strNamespaceUri.GetLength(), NULL) == S_OK) ? HTTP_SUCCESS : HTTP_FAIL;
	}

	HTTP_CODE OnGetNextFunction() throw()
	{
		m_nFunc++;
		if (m_pFuncs[m_nFunc] == NULL)
		{
			m_nFunc = -1;
			return HTTP_S_FALSE;
		}
		return HTTP_SUCCESS;
	}

	HTTP_CODE OnGetFunctionName() throw()
	{
		return (m_pWriteStream->WriteStream(m_pFuncs[m_nFunc]->szName, 
			m_pFuncs[m_nFunc]->cchName, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_S_FALSE; 
	}

	HTTP_CODE OnGetNextParameter() throw()
	{
		++m_nParam;
		if (m_pFuncs[m_nFunc]->pEntries[m_nParam].nHash != 0)
		{
			if (m_pFuncs[m_nFunc]->pEntries[m_nParam].dwFlags & SOAPFLAG_NOMARSHAL)
			{
				return OnGetNextParameter();
			}
			return HTTP_SUCCESS;
		}
		m_nParam = -1;
		return HTTP_S_FALSE;
	}

	HTTP_CODE OnIsInParameter() throw()
	{
		return (m_pFuncs[m_nFunc]->pEntries[m_nParam].dwFlags & SOAPFLAG_IN) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetParameterName() throw()
	{
		HRESULT hr = S_OK;
		if (m_pFuncs[m_nFunc]->pEntries[m_nParam].dwFlags & SOAPFLAG_RETVAL)
		{
			hr = m_pWriteStream->WriteStream("return", sizeof("return")-1, NULL);
		}
		else
		{
			hr = m_pWriteStream->WriteStream(m_pFuncs[m_nFunc]->pEntries[m_nParam].szField, 
					m_pFuncs[m_nFunc]->pEntries[m_nParam].cchField, NULL);
		}
		
		return (hr == S_OK) ? HTTP_SUCCESS : HTTP_FAIL; 
	}

	HTTP_CODE OnNotIsArrayParameter() throw()
	{
		return (m_pFuncs[m_nFunc]->pEntries[m_nParam].dwFlags & (SOAPFLAG_FIXEDARR | SOAPFLAG_DYNARR)) 
			? HTTP_S_FALSE: HTTP_SUCCESS;
	}

	HTTP_CODE OnIsParameterUDT() throw()
	{
		return IsUDT(&m_pFuncs[m_nFunc]->pEntries[m_nParam]);
	}

	HTTP_CODE OnGetParameterSoapType() throw()
	{
		if (m_pFuncs[m_nFunc]->pEntries[m_nParam].nVal != SOAPTYPE_UNK)
		{
			return GetSoapType(m_pFuncs[m_nFunc]->pEntries[m_nParam].nVal, m_pWriteStream);
		}
		ATLASSERT( m_pFuncs[m_nFunc]->pEntries[m_nParam].pChain != NULL );
		return (m_pWriteStream->WriteStream(m_pFuncs[m_nFunc]->pEntries[m_nParam].pChain->szName, 
			m_pFuncs[m_nFunc]->pEntries[m_nParam].pChain->cchName, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_S_FALSE; 
	}

	HTTP_CODE OnIsParameterDynamicArray() throw()
	{
		return (m_pFuncs[m_nFunc]->pEntries[m_nParam].dwFlags & SOAPFLAG_DYNARR) ? HTTP_SUCCESS: HTTP_S_FALSE;
	}

	HTTP_CODE OnIsArrayParameter() throw()
	{
		return (OnNotIsArrayParameter() != HTTP_SUCCESS) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnIsParameterOneDimensional() throw()
	{
		return (m_pFuncs[m_nFunc]->pEntries[m_nParam].pDims[0] == 1) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetParameterArraySize() throw()
	{
		return (m_writeHelper.Write(m_pFuncs[m_nFunc]->pEntries[m_nParam].pDims[1]) != FALSE)
			? HTTP_SUCCESS : HTTP_FAIL;
	}

	HTTP_CODE OnGetParameterArraySoapDims() throw()
	{
		return GetSoapDims(&m_pFuncs[m_nFunc]->pEntries[m_nParam]);
	}

	HTTP_CODE OnIsOutParameter() throw()
	{
		return (m_pFuncs[m_nFunc]->pEntries[m_nParam].dwFlags & SOAPFLAG_OUT) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetNextEnum() throw()
	{
		if (m_currUDTPos == NULL)
		{
			m_currUDTPos = m_enumMap.GetStartPosition();
		}
		else
		{
			m_enumMap.GetNext(m_currUDTPos);
		}

		return (m_currUDTPos != NULL) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetEnumName() throw()
	{
		const _soapmap *pMap = m_enumMap.GetValueAt(m_currUDTPos);
		return (m_pWriteStream->WriteStream(pMap->szName, pMap->cchName, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetNextEnumElement() throw()
	{
		const _soapmap *pMap = m_enumMap.GetValueAt(m_currUDTPos);
		++m_nCurrUDTField;
		if (pMap->pEntries[m_nCurrUDTField].nHash != 0)
		{
			return HTTP_SUCCESS;
		}
		m_nCurrUDTField = -1;
		return HTTP_S_FALSE;
	}

	HTTP_CODE OnGetEnumElementName() throw()
	{
		const _soapmap *pMap = m_enumMap.GetValueAt(m_currUDTPos);
		return (m_pWriteStream->WriteStream(pMap->pEntries[m_nCurrUDTField].szField,
			pMap->pEntries[m_nCurrUDTField].cchField, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetNextStruct() throw()
	{
		if (m_currUDTPos == NULL)
		{
			m_currUDTPos = m_structMap.GetStartPosition();
		}
		else
		{
			m_structMap.GetNext(m_currUDTPos);
		}

		return (m_currUDTPos != NULL) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetStructName() throw()
	{
		const _soapmap *pMap = m_enumMap.GetValueAt(m_currUDTPos);
		return (m_pWriteStream->WriteStream(pMap->szName, pMap->cchName, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetNextStructField() throw()
	{
		const _soapmap *pMap = m_structMap.GetValueAt(m_currUDTPos);
		++m_nCurrUDTField;
		if (pMap->pEntries[m_nCurrUDTField].nHash != 0)
		{
			return HTTP_SUCCESS;
		}
		m_nCurrUDTField = -1;
		return HTTP_S_FALSE;
	}

	HTTP_CODE OnGetStructFieldName() throw()
	{
		const _soapmap *pMap = m_structMap.GetValueAt(m_currUDTPos);
		return (m_pWriteStream->WriteStream(pMap->pEntries[m_nCurrUDTField].szField,
			pMap->pEntries[m_nCurrUDTField].cchField, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnNotIsArrayField() throw()
	{
		const _soapmap *pMap = m_structMap.GetValueAt(m_currUDTPos);
		return (pMap->pEntries[m_nCurrUDTField].dwFlags & SOAPFLAG_FIXEDARR) ? HTTP_S_FALSE : HTTP_SUCCESS;
	}

	HTTP_CODE OnIsFieldUDT() throw()
	{
		const _soapmap *pMap = m_structMap.GetValueAt(m_currUDTPos);
		return IsUDT(&pMap->pEntries[m_nCurrUDTField]);
	}

	HTTP_CODE OnGetStructFieldSoapType() throw()
	{
		const _soapmap *pMap = m_structMap.GetValueAt(m_currUDTPos);
		if (pMap->pEntries[m_nCurrUDTField].nVal != SOAPTYPE_UNK)
		{
			return GetSoapType(pMap->pEntries[m_nCurrUDTField].nVal, m_pWriteStream);
		}
		ATLASSERT( pMap->pEntries[m_nCurrUDTField].pChain != NULL );
		return (m_pWriteStream->WriteStream(pMap->pEntries[m_nCurrUDTField].pChain->szName, 
			pMap->pEntries[m_nCurrUDTField].pChain->cchName, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_S_FALSE; 
	}

	HTTP_CODE OnIsArrayField() throw()
	{
		return (OnNotIsArrayField() != HTTP_SUCCESS) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnIsFieldOneDimensional() throw()
	{
		const _soapmap *pMap = m_structMap.GetValueAt(m_currUDTPos);
		return (pMap->pEntries[m_nCurrUDTField].pDims[0] == 1) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetFieldArraySize() throw()
	{
		const _soapmap *pMap = m_structMap.GetValueAt(m_currUDTPos);
		return (m_writeHelper.Write(pMap->pEntries[m_nCurrUDTField].pDims[1]) != FALSE) ? 
				HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetFieldArraySoapDims() throw()
	{
		const _soapmap *pMap = m_structMap.GetValueAt(m_currUDTPos);
		return GetSoapDims(&pMap->pEntries[m_nCurrUDTField]);
	}

	HTTP_CODE OnGetServiceName() throw()
	{
		return (m_pWriteStream->WriteStream(m_strServiceName, 
			m_strServiceName.GetLength(), NULL) == S_OK) ? HTTP_SUCCESS : HTTP_FAIL;
	}

	HTTP_CODE OnGetNextHeader() throw()
	{	
		if (m_currHeaderPos == NULL)
		{
			m_currHeaderPos = m_headerMap.GetStartPosition();
		}
		else
		{
			m_headerMap.GetNext(m_currHeaderPos);
		}

		return (m_currHeaderPos != NULL) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnIsInHeader() throw()
	{
		return (m_pHeaders[m_nFunc]->pEntries[m_nHeader].dwFlags & SOAPFLAG_IN) 
			? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnIsOutHeader() throw()
	{
		return (m_pHeaders[m_nFunc]->pEntries[m_nHeader].dwFlags & SOAPFLAG_OUT) 
			? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnIsRequiredHeader() throw()
	{
		return (m_pHeaders[m_nFunc]->pEntries[m_nHeader].dwFlags & SOAPFLAG_MUSTUNDERSTAND) 
			? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetHeaderName() throw()
	{
		const _soapmapentry *pEntry = m_headerMap.GetValueAt(m_currHeaderPos);
		return (m_pWriteStream->WriteStream(pEntry->szField, 
			pEntry->cchField, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_FAIL;
	}

	HTTP_CODE OnNotIsArrayHeader() throw()
	{
		const _soapmapentry *pEntry = m_headerMap.GetValueAt(m_currHeaderPos);
		return (pEntry->dwFlags & SOAPFLAG_FIXEDARR) ? HTTP_S_FALSE : HTTP_SUCCESS;
	}

	HTTP_CODE OnIsHeaderUDT() throw()
	{
		return IsUDT(m_headerMap.GetValueAt(m_currHeaderPos));
	}

	HTTP_CODE OnGetHeaderSoapType() throw()
	{
		const _soapmapentry *pEntry = m_headerMap.GetValueAt(m_currHeaderPos);
		if (pEntry->nVal != SOAPTYPE_UNK)
		{
			return GetSoapType(pEntry->nVal, m_pWriteStream);
		}
		ATLASSERT( pEntry->pChain != NULL );
		return (m_pWriteStream->WriteStream(pEntry->pChain->szName, 
			pEntry->pChain->cchName, NULL) == S_OK) ? HTTP_SUCCESS : HTTP_S_FALSE; 
	}

	HTTP_CODE OnIsHeaderOneDimensional() throw()
	{
		const _soapmapentry *pEntry = m_headerMap.GetValueAt(m_currHeaderPos);
		return (pEntry->pDims[0] == 1) ? HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetHeaderArraySize() throw()
	{
		const _soapmapentry *pEntry = m_headerMap.GetValueAt(m_currHeaderPos);
		return (m_writeHelper.Write(pEntry->pDims[1]) != FALSE) ? 
				HTTP_SUCCESS : HTTP_S_FALSE;
	}

	HTTP_CODE OnGetHeaderArraySoapDims() throw()
	{
		return GetSoapDims(m_headerMap.GetValueAt(m_currHeaderPos));
	}

	HTTP_CODE OnGetNextFunctionHeader() throw()
	{
		++m_nHeader;
		if (m_pHeaders[m_nFunc]->pEntries[m_nHeader].nHash != 0)
		{
			if (m_pHeaders[m_nFunc]->pEntries[m_nHeader].dwFlags & SOAPFLAG_NOMARSHAL)
			{
				return OnGetNextHeader();
			}
			return HTTP_SUCCESS;
		}
		m_nHeader = -1;
		return HTTP_S_FALSE;
	}

	HTTP_CODE OnGetFunctionHeaderName() throw()
	{
		return (m_pWriteStream->WriteStream(
					m_pHeaders[m_nFunc]->pEntries[m_nHeader].szField,
					m_pHeaders[m_nFunc]->pEntries[m_nHeader].cchField,
					NULL) == S_OK) ? HTTP_SUCCESS : HTTP_FAIL;
	}

	HTTP_CODE OnIsArrayHeader() throw()
	{
		return (OnNotIsArrayHeader() == HTTP_SUCCESS) ? HTTP_S_FALSE : HTTP_SUCCESS;
	}

	BEGIN_REPLACEMENT_METHOD_MAP(_CSDLGenerator)
		REPLACEMENT_METHOD_ENTRY("GetNamespace", OnGetNamespace)
		REPLACEMENT_METHOD_ENTRY("GetNextFunction", OnGetNextFunction)
		REPLACEMENT_METHOD_ENTRY("GetFunctionName", OnGetFunctionName)
		REPLACEMENT_METHOD_ENTRY("GetNextParameter", OnGetNextParameter)
		REPLACEMENT_METHOD_ENTRY("IsInParameter", OnIsInParameter)
		REPLACEMENT_METHOD_ENTRY("GetParameterName", OnGetParameterName)
		REPLACEMENT_METHOD_ENTRY("NotIsArrayParameter", OnNotIsArrayParameter)
		REPLACEMENT_METHOD_ENTRY("IsParameterUDT", OnIsParameterUDT)
		REPLACEMENT_METHOD_ENTRY("GetParameterSoapType", OnGetParameterSoapType)
		REPLACEMENT_METHOD_ENTRY("IsParameterDynamicArray", OnIsParameterDynamicArray)
		REPLACEMENT_METHOD_ENTRY("IsArrayParameter", OnIsArrayParameter)
		REPLACEMENT_METHOD_ENTRY("IsParameterOneDimensional", OnIsParameterOneDimensional)
		REPLACEMENT_METHOD_ENTRY("GetParameterArraySize", OnGetParameterArraySize)
		REPLACEMENT_METHOD_ENTRY("GetParameterArraySoapDims", OnGetParameterArraySoapDims)
		REPLACEMENT_METHOD_ENTRY("IsOutParameter", OnIsOutParameter)
		REPLACEMENT_METHOD_ENTRY("GetNextEnum", OnGetNextEnum)
		REPLACEMENT_METHOD_ENTRY("GetEnumName", OnGetEnumName)
		REPLACEMENT_METHOD_ENTRY("GetNextEnumElement", OnGetNextEnumElement)
		REPLACEMENT_METHOD_ENTRY("GetEnumElementName", OnGetEnumElementName)
		REPLACEMENT_METHOD_ENTRY("GetNextStruct", OnGetNextStruct)
		REPLACEMENT_METHOD_ENTRY("GetStructName", OnGetStructName)
		REPLACEMENT_METHOD_ENTRY("GetNextStructField", OnGetNextStructField)
		REPLACEMENT_METHOD_ENTRY("GetStructFieldName", OnGetStructFieldName)
		REPLACEMENT_METHOD_ENTRY("NotIsArrayField", OnNotIsArrayField)
		REPLACEMENT_METHOD_ENTRY("IsFieldUDT", OnIsFieldUDT)
		REPLACEMENT_METHOD_ENTRY("GetStructFieldSoapType", OnGetStructFieldSoapType)
		REPLACEMENT_METHOD_ENTRY("IsArrayField", OnIsArrayField)
		REPLACEMENT_METHOD_ENTRY("IsFieldOneDimensional", OnIsFieldOneDimensional)
		REPLACEMENT_METHOD_ENTRY("GetFieldArraySize", OnGetFieldArraySize)
		REPLACEMENT_METHOD_ENTRY("GetFieldArraySoapDims", OnGetFieldArraySoapDims)
		REPLACEMENT_METHOD_ENTRY("GetServiceName", OnGetServiceName)
		REPLACEMENT_METHOD_ENTRY("GetURL", OnGetURL)

		REPLACEMENT_METHOD_ENTRY("GetNextHeader", OnGetNextHeader)
		REPLACEMENT_METHOD_ENTRY("GetHeaderName", OnGetHeaderName)
		REPLACEMENT_METHOD_ENTRY("NotIsArrayHeader", OnNotIsArrayHeader)
		REPLACEMENT_METHOD_ENTRY("IsArrayHeader", OnIsArrayHeader)
		REPLACEMENT_METHOD_ENTRY("IsHeaderUDT", OnIsHeaderUDT)
		REPLACEMENT_METHOD_ENTRY("GetHeaderSoapType", OnGetHeaderSoapType)
		REPLACEMENT_METHOD_ENTRY("IsHeaderOneDimensional", OnIsHeaderOneDimensional)
		REPLACEMENT_METHOD_ENTRY("GetHeaderArraySize", OnGetHeaderArraySize)
		REPLACEMENT_METHOD_ENTRY("GetHeaderArraySoapDims", OnGetHeaderArraySoapDims)
		REPLACEMENT_METHOD_ENTRY("GetNextFunctionHeader", OnGetNextFunctionHeader)
		REPLACEMENT_METHOD_ENTRY("GetFunctionHeaderName", OnGetFunctionHeaderName)
		REPLACEMENT_METHOD_ENTRY("IsInHeader", OnIsInHeader)
		REPLACEMENT_METHOD_ENTRY("IsOutHeader", OnIsOutHeader)
		REPLACEMENT_METHOD_ENTRY("IsRequiredHeader", OnIsRequiredHeader)
	END_REPLACEMENT_METHOD_MAP()
}; // class _CSDLGenerator

template <class THandler, const char *szHandlerName>
class CSDLGenerator :
	public _CSDLGenerator,
	public IRequestHandlerImpl<CSDLGenerator>,
	public CComObjectRootEx<CComSingleThreadModel>
{
private:

public:
	typedef CSDLGenerator<THandler, szHandlerName> _sdlGenerator;

	BEGIN_COM_MAP(_sdlGenerator)
		COM_INTERFACE_ENTRY(IRequestHandler)
		COM_INTERFACE_ENTRY(ITagReplacer)
	END_COM_MAP()

	inline BOOL CachePage() throw()
	{
		// cache the page since it's never going to change until the DLL is unloaded
		return TRUE;
	}

	HTTP_CODE InitializeHandler(AtlServerRequest *pRequestInfo, IServiceProvider *pServiceProvider) throw()
	{
		IRequestHandlerImpl<CSDLGenerator>::InitializeHandler(pRequestInfo, pServiceProvider);

		CComObjectStack<THandler> handler;
		if (FAILED(InitializeSDL(&handler)))
		{
			return HTTP_FAIL;
		}

		CStencil s;
		HTTP_CODE hcErr = s.LoadFromString(s_szAtlsWSDLSrf, (DWORD) s_nAtlsWSDLSrfLen);
		if (hcErr == HTTP_SUCCESS)
		{
			hcErr = HTTP_FAIL;
			CHttpResponse HttpResponse(pRequestInfo->pServerContext);
			HttpResponse.SetContentType("text/xml");
			if (s.ParseReplacements(this) != false)
			{
				s.FinishParseReplacements();
				SetStream(&HttpResponse);
				SetWriteStream(&HttpResponse);
				SetHttpServerContext(m_spServerContext);
				hcErr = s.Render(this, &HttpResponse);
			}
		}

		return hcErr;
	}

	const char * GetHandlerName() throw()
	{
		return szHandlerName;
	}
}; // class CSDLGenerator

} // namespace ATL

#endif // __ATLSOAP_H__