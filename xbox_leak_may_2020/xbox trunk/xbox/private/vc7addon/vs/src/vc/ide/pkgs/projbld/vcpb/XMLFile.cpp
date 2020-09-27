// XMLFile.cpp : Implementation of CXMLFile

#include "stdafx.h"
#include "XMLFile.h"
#include "VCProjectEngine.h"
#include "util2.h"

/////////////////////////////////////////////////////////////////////////////
// CCXMLFile

int CXMLFile::DetermineDefaultCodePage()
{
	int cChars = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, NULL, 0);
	CStringW strBuf;
	wchar_t *szInfo = strBuf.GetBuffer(cChars+1);
	GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, szInfo, cChars);
	int iCodePage = _wtoi(szInfo);
	if (!iCodePage)
		iCodePage = CP_ACP;
	strBuf.ReleaseBuffer();

	return iCodePage;
}

void CXMLFile::DetermineDefaultFileEncoding(CComBSTR& bstrEncoding)
{
	CVCProjectEngine::GetCodePageName(DetermineDefaultCodePage(), bstrEncoding);
}

void CXMLFile::AdjustFileFormatForFileEncoding(LPCOLESTR szOldEncoding, LPCOLESTR szNewEncoding, enumFileFormat& eFormat, 
	BOOL bForce /* = FALSE */)
{
	if (!bForce)
	{
		if (!szOldEncoding && !szNewEncoding)
			return;		// they're NULL, but the same
		else if (szOldEncoding && szNewEncoding && wcscmp(szOldEncoding, szNewEncoding) == 0)
			return;		// nothing really changed
	}


	CStringW strEncoding = szNewEncoding;
	if (strEncoding.GetLength() < 5)
	{
		eFormat = eANSI;
		return;
	}

	strEncoding.MakeLower();
	if (wcsncmp(strEncoding, L"utf-8", 5) == 0)
		eFormat = eUTF8;
	else if (wcsncmp(strEncoding, L"utf-", 4) == 0)
		eFormat = eUnicode;
	else
		eFormat = eANSI;
}

void CXMLFile::AdjustFileEncodingForFileFormat(enumFileFormat eOldFormat, enumFileFormat eNewFormat, CComBSTR& bstrEncoding,
	BOOL bForce /* = FALSE */)
{
	if (!bForce && eOldFormat == eNewFormat)
		return;		// nothing more to do

	switch (eNewFormat)
	{
	case eUTF8:
		bstrEncoding = L"UTF-8";
		break;
	case eUnicode:
		bstrEncoding = L"UTF-16";
		break;
	default:	// eANSI
		if (!bForce || bstrEncoding.Length() == 0)
			DetermineDefaultFileEncoding(bstrEncoding);
		break;
	}
}


HRESULT CXMLFile::CreateInstance( IVCXMLFile **ppXMLFile, LPCOLESTR szFilename, DWORD dwBufSize, LPCOLESTR szFileEncoding,
	enumFileFormat eFileFormat)
{
	HRESULT hr;
	CXMLFile *pVar;
	CComObject<CXMLFile> *pObj;
	hr = CComObject<CXMLFile>::CreateInstance(&pObj);
	if( SUCCEEDED( hr ) )
	{
		pVar = pObj;
		pVar->AddRef();
		*ppXMLFile = pVar;
		CComBSTR bstrFileEncoding;
		if (szFileEncoding)
			bstrFileEncoding = szFileEncoding;
		else
			AdjustFileEncodingForFileFormat(eFileFormat, eFileFormat, bstrFileEncoding, TRUE);
		hr = pVar->Initialize( szFilename, dwBufSize, szFileEncoding );
	}
	return hr;
}

CXMLFile::CXMLFile() : 
	m_file( NULL ), 
	m_bstrFilename( NULL ), 
	m_dwBufSize( 0 ), 
	m_pBuf( NULL ),
	m_dwBytesInBuffer( 0 ),
	m_dwOffset( 0 )
{
}

CXMLFile::~CXMLFile()
{
	if (m_pBuf)
	{
		delete [] m_pBuf;
		m_pBuf = NULL;
	}
	if (m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

STDMETHODIMP CXMLFile::Initialize( LPCOLESTR szFilename, DWORD dwBufSize, LPCOLESTR szFileEncoding )
{
	m_bstrFilename = szFilename;

	m_file = fopenW( m_bstrFilename, L"wb" );
	if( !m_file )
	{
		// TODO: determine why the file wasn't opened so we can generate a
		// somewhat meaningful error message
		return E_ACCESSDENIED;
	}
	// set the buffer size
	m_dwBufSize = dwBufSize;
	// allocate the buffer
	m_pBuf = new char[m_dwBufSize];
	VSASSERT( m_pBuf, "Failed to allocate stream buffer" );
	RETURN_ON_NULL2(m_pBuf, E_OUTOFMEMORY);

	// file format to write 
	m_bstrFileEncoding = szFileEncoding;

	return S_OK;
}

STDMETHODIMP CXMLFile::Read(
	void *pv,
	ULONG cb,
	ULONG *pcbRead)
{
	// validate cb
	if( (m_dwOffset + cb) > m_dwBytesInBuffer )
		return STG_E_INVALIDPOINTER;
	// copy data
	memcpy( pv, (void*)(m_pBuf + m_dwOffset), cb );
	*pcbRead = cb;

	return S_OK;
}

STDMETHODIMP CXMLFile::Write(
	void const *pv,
	ULONG cb,
	ULONG *pcbWritten)
{
	// append into the buffer, expanding it if necessary
	if( (m_dwBytesInBuffer + cb) > m_dwBufSize )
	{
		char *newBuf = new char[m_dwBufSize * 2];
		memcpy( (void*)newBuf, (void*)m_pBuf, m_dwBytesInBuffer );
		delete[] m_pBuf;
		m_pBuf = newBuf;
		m_dwBufSize *= 2;
	}
	memcpy( (void*)(m_pBuf + m_dwOffset), pv, cb );
	*pcbWritten = cb;

	// adjust m_dwOffset, m_dwBytesInBuffer
	m_dwOffset += cb;
	m_dwBytesInBuffer += cb;
	
	return S_OK;
}

STDMETHODIMP CXMLFile::Seek(
	LARGE_INTEGER dlibMove,
	DWORD dwOrigin,
	ULARGE_INTEGER *plibNewPosition)
{
	// NOTE:
	// this method only uses the low 32 bits of the offset given!!!

	// validate, set m_dwOffset
	DWORD newOffset;
	switch( dwOrigin )
	{
	case STREAM_SEEK_SET:
		newOffset = dlibMove.LowPart;
		break;
	case STREAM_SEEK_CUR:
		newOffset = m_dwOffset + dlibMove.LowPart;
		break;
	case STREAM_SEEK_END:
		newOffset = m_dwBytesInBuffer + dlibMove.LowPart;
		break;
	default:
		return STG_E_INVALIDFUNCTION;
	}

	// if the offset is greater than the end of the stream, bail
	if( newOffset > m_dwBytesInBuffer )
		return STG_E_INVALIDPOINTER;
	// if the offset is less than 0, bail
	else if( newOffset < 0 )
		return STG_E_INVALIDPOINTER;
	else
		m_dwOffset = newOffset + dlibMove.LowPart;

	// write plibNewPosition, if it's not NULL
	if( plibNewPosition )
	{
		plibNewPosition->LowPart = m_dwOffset;
		plibNewPosition->HighPart = 0;
	}

	return S_OK;
}

STDMETHODIMP CXMLFile::SetSize(
	ULARGE_INTEGER libNewSize)
{
	// NOTE:
	// this method only uses the low 32 bits of the offset given!!!
	
	// validate (new size not less than existing data)
	if( libNewSize.LowPart < m_dwBytesInBuffer )
		return STG_E_INVALIDPOINTER;
	// allocate new buffer
	char *newBuf = new char[libNewSize.LowPart];
	// copy old buffer into new buffer
	memcpy( (void*)newBuf, (void*)m_pBuf, m_dwBytesInBuffer );
	// free old buffer
	delete[] m_pBuf;
	// set buffer pointer to new buffer
	m_pBuf = newBuf;
	// set buf size
	m_dwBufSize = libNewSize.LowPart;

	return S_OK;
}

STDMETHODIMP CXMLFile::CopyTo(
	IStream *pstm,
	ULARGE_INTEGER cb,
	ULARGE_INTEGER *pcbRead,
	ULARGE_INTEGER *pcbWritten)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXMLFile::Commit(DWORD grfCommitFlags)
{
	RETURN_ON_NULL2(m_file, S_FALSE);		// nothing to do in this case...

	char* buffer;
	unsigned int buf_size = 0;
	// write the correct format
	enumFileFormat eFileFormat;
	AdjustFileFormatForFileEncoding(NULL, m_bstrFileEncoding, eFileFormat, TRUE);

	HRESULT hr = S_OK;
	switch( eFileFormat )
	{
	case eANSI:
	{
		// convert the entire buffer

		// determine the user's choice of code page
		int iCodePage = CVCProjectEngine::GetCodePageFromCharSet(m_bstrFileEncoding);
		if (!iCodePage)
			iCodePage = CP_ACP;

		// determine the buffer size in ANSI MBCS
		buf_size = CVCProjectEngine::ConvertFromUnicodeMlang( iCodePage, TRUE, TRUE, (wchar_t*)m_pBuf, 
			m_dwBytesInBuffer/sizeof(wchar_t), NULL, 0, NULL );
		if( buf_size )
		{
			// alloc a buffer
			buffer = new char[buf_size];
			// convert
			CVCProjectEngine::ConvertFromUnicodeMlang( iCodePage, TRUE, TRUE, (wchar_t*)m_pBuf, m_dwBytesInBuffer/sizeof(wchar_t),
				buffer, buf_size, NULL /* &<usedDefault>*/);
			// WARNING: RichardS (in VS7 200565) says we shouldn't be telling the user if there was a problem, so don't.
			// set the member buffer to the converted one
			delete[] m_pBuf;
			m_pBuf = buffer;
			buffer = NULL;
		}
		break;
	}
	case eUTF8:
	{
		// convert the entire buffer

		// determine the buffer size in UTF-8 MBCS
		buf_size = CVCProjectEngine::ConvertFromUnicodeMlang( CP_UTF8, TRUE, TRUE, (wchar_t*)m_pBuf, 
			m_dwBytesInBuffer/sizeof(wchar_t), NULL, 0, NULL );
		if( buf_size )
		{
			// alloc a buffer of buf_size + 3 byte UTF-8 file prefix
			buffer = new char[buf_size+3];
			buffer[0] = (char)0xEF;
			buffer[1] = (char)0xBB;
			buffer[2] = (char)0xBF;
			// convert
			CVCProjectEngine::ConvertFromUnicodeMlang( CP_UTF8, TRUE, TRUE, (wchar_t*)m_pBuf, m_dwBytesInBuffer/sizeof(wchar_t),
				buffer+3, buf_size, NULL);
			buf_size += 3;
			// set the member buffer to the converted one
			delete[] m_pBuf;
			m_pBuf = buffer;
			buffer = NULL;
		}
		break;
	}
	case eUnicode:
	{
		// write the two byte Unicode file prefix
		unsigned char buf[2] = { 0xFF, 0xFE };
		fwrite( (void*)buf, 1, 2, m_file );
		// set the buffer size (number of bytes to write)
		buf_size = m_dwBytesInBuffer;
		break;
	}
	default:
		break;
	}

	// if we have a file
	if( m_file )
	{
		if (SUCCEEDED(hr))
		{
			// flush the buffer to disk
			DWORD dwBytesWritten;
			dwBytesWritten = (DWORD) fwrite( (void*)m_pBuf, 1, buf_size, m_file );
			if( dwBytesWritten != buf_size )
			{
				int err = ferror( m_file );
				if( err )
				{
					// an error occurred
					clearerr( m_file );
					return E_FAIL;
				}
			}
		}
		// close the file
		fclose( m_file );
		m_file = NULL;
	}

	// free the buffer
	delete[] m_pBuf;
	m_pBuf = NULL;

	return hr;
}

STDMETHODIMP CXMLFile::Revert()
{
	return E_NOTIMPL;
}

STDMETHODIMP CXMLFile::LockRegion(
	ULARGE_INTEGER libOffset,
	ULARGE_INTEGER cb,
	DWORD dwLockType)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXMLFile::UnlockRegion(
	ULARGE_INTEGER libOffset,
	ULARGE_INTEGER cb,
	DWORD dwLockType)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXMLFile::Stat(
	STATSTG *pstatstg,
	DWORD grfStatFlag)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXMLFile::Clone(
	IStream **ppstm)
{
	return E_NOTIMPL;
}


