//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xstrmrd.cpp
//
//  Description:    Implement XStreamRead and its subclasses.
//
//  History:
//      02/24/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

#define URL_READ_CHUNK_SIZE 0x8000

int YaccIsEof(void);
void YaccResetEof(void);
int YaccParseXOF(XStreamRead *);

HRESULT DecompressFile_Zip(XSource *pSrc, XSrcMemory **ppSrcMem);

///////////////////////////////////////////////////////////////////////////////
//
//  XStreamRead class
//
///////////////////////////////////////////////////////////////////////////////

XStreamRead::XStreamRead(XSource *pSrc)
  : m_pSrc(pSrc),
    m_status(StreamParsing),
    m_plDataObj(NULL),
    m_iDataObj(0)
{
}

XStreamRead::~XStreamRead()
{
    if (m_plDataObj)
        delete m_plDataObj;

    if (m_pSrc)
        delete m_pSrc;
}

HRESULT XStreamRead::ValidateStream()
{
    DWORD magic, version, format, floatBits;
    CompressionType ct = CT_None;


    if (!ReadDword(&magic) || magic != XFILE_MAGIC) {
        DPF_ERR("Bad magic number");
        return DXFILEERR_BADFILETYPE;
    }

    if (!ReadDword(&version) ||
        version != XFILE_VERSION && version != XFILE_VERSION_DX3) {
        DPF_ERR("Invalid file format version");
        return DXFILEERR_BADFILEVERSION;
    }

    if (!ReadDword(&format))
        return DXFILEERR_BADFILETYPE;

    if (format == XFILE_FORMAT_TEXT) {
        m_mode = FM_Text;
        ct = CT_None;
    } else if (format == XFILE_FORMAT_BINARY) {
        m_mode = FM_Binary;
        ct = CT_None;
    } else if (format == XFILE_FORMAT_TEXT_ZIP) {
        m_mode = FM_Text;
        ct = CT_ZIP;
    } else if (format == XFILE_FORMAT_BINARY_ZIP) {
        m_mode = FM_Binary;
        ct = CT_ZIP;
    } else
        return DXFILEERR_BADFILETYPE;

    if (!ReadDword(&floatBits))
        return DXFILEERR_BADFILETYPE;

    if (floatBits == XFILE_FLOATSIZE_32)
        m_floatSize = sizeof(FLOAT);
    else if (floatBits == XFILE_FLOATSIZE_64)
        m_floatSize = sizeof(double);
    else
        return DXFILEERR_BADFILEFLOATSIZE;

    if (ct == CT_ZIP) {
        HRESULT hr;

        XSrcMemory *pSrcMem;

        hr = DecompressFile_Zip(m_pSrc, &pSrcMem);

        if (SUCCEEDED(hr)) {
            delete m_pSrc;
            m_pSrc = pSrcMem;
            hr = ValidateStream();
        }

        return hr;
    }

    return DXFILE_OK;
}

HRESULT XStreamRead::Create(LPVOID pvSource, DWORD loadflags,
                            XStreamRead **ppStream)
{
    HRESULT hr;

    if (loadflags & DXFILELOAD_FROMRESOURCE) {
        DPF_ERR("Opening a .x stream from a resource is not supported.");

        hr = D3DERR_INVALIDCALL;
    } else if (loadflags & DXFILELOAD_FROMMEMORY) {
        LPDXFILELOADMEMORY lMemory = (LPDXFILELOADMEMORY)pvSource;

        hr = CreateFromMemory(lMemory->lpMemory,
                              lMemory->dSize,
                              ppStream);

    } else if (loadflags & DXFILELOAD_FROMURL) {
        DPF_ERR("Opening a .x stream from a url is not supported.");

        hr = D3DERR_INVALIDCALL;
    } else if (loadflags & DXFILELOAD_FROMSTREAM) {
        hr = DXFILEERR_NOTDONEYET;

    } else {
        hr = CreateFromFile((LPCSTR)pvSource, ppStream);
    }

    if (SUCCEEDED(hr)) {
        hr = (*ppStream)->ValidateStream();

        if (FAILED(hr))
            delete *ppStream;
    }

    return hr;
}

HRESULT XStreamRead::CreateFromMemory(LPVOID pvFile, DWORD cbFile,
                                      XStreamRead **ppStream)
{
    if (!pvFile || !cbFile)
        return DXFILEERR_BADVALUE;

    XSrcMemory *pSrc = new XSrcMemory(pvFile, cbFile);

    if (!pSrc)
        return DXFILEERR_BADALLOC;

    *ppStream = new XStreamRead(pSrc);

    if (!*ppStream) {
        delete pSrc;
        return DXFILEERR_BADALLOC;
    }

    return DXFILE_OK;
}

HRESULT XStreamRead::CreateFromFile(LPCSTR szFilename, XStreamRead **ppStream)
{
    HANDLE hFile;

    hFile = CreateFileA(szFilename, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        DPF_ERR("Failed to open file.");
        return DXFILEERR_FILENOTFOUND;
    }

    DWORD cbFile;

    cbFile = GetFileSize(hFile, NULL);

    if (cbFile == -1) {
        CloseHandle(hFile);
        DPF_ERR("Failed to get file size.");
        return DXFILEERR_BADFILE;
    }


#if 1

    BYTE *pbMemory;
    
    XMalloc((void **)&pbMemory, cbFile);

    if (!pbMemory)
    {
        CloseHandle(hFile);
        DPF_ERR("Failed to allocate memory for the file.");
        return DXFILEERR_BADALLOC;
    }

    DWORD dwRead;

    if (!ReadFile(hFile, pbMemory, cbFile, &dwRead, NULL))
    {
        CloseHandle(hFile);

        DPF_ERR("Failed to read the file into memory.");
        return DXFILEERR_BADFILE;
    }

    XSrcMemory *pSrc;

    pSrc = new XSrcMemory(pbMemory, cbFile, TRUE);    

    if (!pSrc)
    {
        CloseHandle(hFile);
        return DXFILEERR_BADALLOC;
    }

#else 0

    HANDLE hMapFile;

    hMapFile = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, NULL);

    if (hMapFile == INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        DPF_ERR("Failed to create file mapping.");
        return DXFILEERR_BADFILE;
    }

    LPVOID pvFile;

    pvFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);

    if (!pvFile) {
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        DPF_ERR("Failed to map view of file on.");
        return DXFILEERR_BADFILE;
    }

    XSrcFile *pSrc;

    pSrc = new XSrcFile(hFile, hMapFile, pvFile, cbFile);

    if (!pSrc) {

        UnmapViewOfFile(pvFile);
        CloseHandle(hMapFile);

        CloseHandle(hFile);
        return DXFILEERR_BADALLOC;
    }

#endif 0

    *ppStream = new XStreamRead(pSrc);

    if (!*ppStream) {
        delete pSrc;
        return DXFILEERR_BADALLOC;
    }

    return DXFILE_OK;
}

HRESULT XStreamRead::SetIntrinsicHeaderInfo(XDataObj *pHeaderObj)
{
    LPVOID   data;
    DWORD    cbSize;
    HRESULT hr;

    hr = pHeaderObj->GetData("flag", &cbSize, &data);

    if (SUCCEEDED(hr)) {
        m_flags = *(DWORD *)data;

        if (m_flags & 1)
            m_mode = FM_Text;
        else
            m_mode = FM_Binary;
    }

    return hr;
}

int XStreamRead::AddDataObject(XDataObj *pObj)
{
    if (!m_plDataObj) {
        m_plDataObj = new XDataObjList();

        if (!m_plDataObj)
            return 0;
    }

    return m_plDataObj->Add(pObj);
}

HRESULT XStreamRead::GetNextObject(XDataObj **ppObj)
{
    *ppObj = NULL;

    if (m_status != StreamParsed) {
        if (!YaccParseXOF(this)) {
            if (YaccIsEof()) {
                m_status = StreamParsed;
                YaccResetEof();
            } else {
                m_status = StreamParsing;
            }
        } else {
            DPF_ERR("Error while parsing stream");
            m_status = StreamParseError;
        }
    }

    if (m_status == StreamParseError) {
        DPF_ERR("GetNextObject: Parse error.");
        return DXFILEERR_PARSEERROR;
    }

    if (m_plDataObj && m_plDataObj->count() > m_iDataObj) {
        *ppObj = (*m_plDataObj)[m_iDataObj];
        m_iDataObj++;
        return DXFILE_OK;
    }

    return DXFILEERR_NOMOREOBJECTS;
}

HRESULT XStreamRead::GetObjectById(REFGUID id, XDataObj **ppObj)
{
    *ppObj = NULL;

    if (m_plDataObj) {
        for (DWORD i = 0; i < m_plDataObj->count(); i++) {
            *ppObj = (*m_plDataObj)[i]->FindDataObj(id);

            if (*ppObj) {
                (*ppObj)->AddRef();
                return DXFILE_OK;
            }
        }
    }

    XDataObj *pObj;

    while (SUCCEEDED(GetNextObject(&pObj))) {
        if (*ppObj = pObj->FindDataObj(id)) {
            (*ppObj)->AddRef();
            return DXFILE_OK;
        }
    }

    return DXFILEERR_NOTFOUND;
}

HRESULT XStreamRead::GetObjectByName(LPCSTR name, XDataObj **ppObj)
{
    *ppObj = NULL;

    if (!name || !*name)
        return DXFILEERR_BADVALUE;

    if (m_plDataObj) {
        for (DWORD i = 0; i < m_plDataObj->count(); i++) {
            *ppObj = (*m_plDataObj)[i]->FindDataObj(name);

            if (*ppObj) {
                (*ppObj)->AddRef();
                return DXFILE_OK;
            }
        }
    }

    XDataObj *pObj;

    while (SUCCEEDED(GetNextObject(&pObj))) {
        if (*ppObj = pObj->FindDataObj(name)) {
            (*ppObj)->AddRef();
            return DXFILE_OK;
        }
    }

    return DXFILEERR_NOTFOUND;
}

HRESULT XStreamRead::GetObjectByNameAndId(LPCSTR name, REFGUID id, XDataObj **ppObj)
{
    *ppObj = NULL;

    XDataObj *pObj;

    if (m_plDataObj) {
        for (DWORD i = 0; i < m_plDataObj->count(); i++) {
            pObj = (*m_plDataObj)[i]->FindDataObj(id);

            if (pObj && pObj->name() && !xstrcmp(name, pObj->name())) {
                *ppObj = pObj;
                (*ppObj)->AddRef();
                return DXFILE_OK;
            }
        }
    }

    XDataObj *pObjNext;

    while (SUCCEEDED(GetNextObject(&pObjNext))) {
        if ((pObj = pObjNext->FindDataObj(id)) &&
            pObj->name() && !xstrcmp(name, pObj->name())) {
            *ppObj = pObj;
            (*ppObj)->AddRef();
            return DXFILE_OK;
        }
    }

    return DXFILEERR_NOTFOUND;
}

///////////////////////////////////////////////////////////////////////////////
//
//  XSrcMemory class
//
///////////////////////////////////////////////////////////////////////////////

XSrcMemory::XSrcMemory(LPVOID pvFile, DWORD cbFile, BOOL fFree)
  : m_pFile((LPBYTE)pvFile),
    m_cbFile(cbFile),
    m_pEnd(m_pFile),
    m_pCur(m_pFile),
    m_fFree(fFree)
{
    m_pEnd += m_cbFile;
}

XSrcMemory::~XSrcMemory()
{
    if (m_fFree)
        XFree(m_pFile);
}

char XSrcMemory::GetByte()
{
    return (m_pCur < m_pEnd)? *m_pCur++ : EOF;
}

int XSrcMemory::ReadWord()
{
    if (m_pCur + sizeof(WORD) <= m_pEnd) {
        WORD wRet = *((LPWORD)m_pCur);
        m_pCur += sizeof(WORD);
        return wRet;
    }

    return EOF;
}

BOOL XSrcMemory::ReadDword(LPDWORD pData)
{
    if (m_pCur + sizeof(DWORD) <= m_pEnd) {
        *pData = *((LPDWORD)m_pCur);
        m_pCur += sizeof(DWORD);
        return TRUE;
    }

    return FALSE;
}

BOOL XSrcMemory::ReadFloat(float *pData)
{
    if (m_pCur + sizeof(float) <= m_pEnd) {
        *pData = *((float *)m_pCur);
        m_pCur += sizeof(float);
        return TRUE;
    }

    return FALSE;
}

BOOL XSrcMemory::ReadDouble(double *pData)
{
    if (m_pCur + sizeof(double) <= m_pEnd) {
        *pData = *((double *)m_pCur);
        m_pCur += sizeof(double);
        return TRUE;
    }

    return FALSE;
}

HRESULT XSrcMemory::Read(LPVOID pvData, DWORD dwOffset, DWORD cbSize, LPDWORD pcbRead)
{
    if (dwOffset >= m_cbFile)
        return DXFILEERR_NOMOREDATA;

    if (cbSize > m_cbFile - dwOffset)
        cbSize = m_cbFile - dwOffset;

    memcpy(pvData, m_pFile+dwOffset, cbSize);

    *pcbRead = cbSize;

    return DXFILE_OK;
}

HRESULT XSrcMemory::GetFile(LPVOID *ppvFile, LPDWORD pcbFile)
{
    *ppvFile = m_pFile;
    *pcbFile = m_cbFile;
    return DXFILE_OK;
}

#if 0

///////////////////////////////////////////////////////////////////////////////
//
//  XSrcFile class
//
///////////////////////////////////////////////////////////////////////////////

XSrcFile::XSrcFile(HANDLE hFile, HANDLE hMapFile, LPVOID pvFile, DWORD cbFile)
  : XSrcMemory(pvFile, cbFile),
    m_hFile(hFile),
    m_hMapFile(hMapFile),
    m_pMapFile(pvFile)
{
}

XSrcFile::~XSrcFile()
{
    UnmapViewOfFile(m_pMapFile);
    CloseHandle(m_hMapFile);
    CloseHandle(m_hFile);
}

#endif 0
