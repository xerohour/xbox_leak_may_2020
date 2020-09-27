// XMLFile.h : Declaration of the CXMLFile

#ifndef __XMLFILE_H_
#define __XMLFILE_H_

/////////////////////////////////////////////////////////////////////////////
// CXMLFile
class ATL_NO_VTABLE CXMLFile : 
	public CComObjectRoot,
	IVCXMLFile,
	IStream
{
public:
	CXMLFile();
	~CXMLFile();

	static HRESULT CreateInstance( IVCXMLFile **ppXMLFile, LPCOLESTR szFilename, DWORD dwBufSize, LPCOLESTR szFileEncoding,
		enumFileFormat eFileFormat );
DECLARE_NO_REGISTRY()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CXMLFile)
	COM_INTERFACE_ENTRY(IVCXMLFile)
	COM_INTERFACE_ENTRY(IStream)
END_COM_MAP()

// IVCXMLFile
public:
	STDMETHOD(Initialize)( LPCOLESTR szFilename, DWORD dwBufSize, LPCOLESTR szFileEncoding );

// IStream
public:
    STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead);
    STDMETHOD(Write)(void const *pv, ULONG cb, ULONG *pcbWritten);
    STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
    STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);
    STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
    STDMETHOD(Commit)(DWORD grfCommitFlags);
    STDMETHOD(Revert)();
    STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHOD(Stat)(STATSTG *pstatstg, DWORD grfStatFlag);
    STDMETHOD(Clone)(IStream **ppstm);

private:
	FILE *m_file;					// file pointer
	CComBSTR m_bstrFilename;		// filename
	DWORD m_dwBufSize;				// buffer size
	char *m_pBuf;					// buffer
	DWORD m_dwBytesInBuffer;		// bytes contained in the buffer waiting to be flushed
	DWORD m_dwOffset;				// current read/write offset
	CComBSTR m_bstrFileEncoding;	// file format to write

public:
	static int DetermineDefaultCodePage();
	static void DetermineDefaultFileEncoding(CComBSTR& bstrEncoding);
	static void AdjustFileFormatForFileEncoding(LPCOLESTR szOldEncoding, LPCOLESTR szNewEncoding, enumFileFormat& eFormat,
		BOOL bForce = FALSE);
	static void AdjustFileEncodingForFileFormat(enumFileFormat eOldFormat, enumFileFormat eNewFormat, CComBSTR& bstrEncoding,
		BOOL bForce = FALSE);
};

#endif //__XMLFILE_H_

