//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xstrmrd.h
//
//  Description:    XStreamRead and its sub classes header file.
//
//  History:
//      02/23/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XSTRMRD_H_
#define _XSTRMRD_H_

#define XDATAOBJ_LIST_CHUNK_SIZE    100

typedef XObjPtrArray<XDataObj, XDATAOBJ_LIST_CHUNK_SIZE> XDataObjList;

typedef enum _StreamStatus {
    StreamParsed,           // The (Normal) stream has been parsed
    StreamParsing,          // The (Streaming) stream is being parsed
    StreamParseError        // Parse error parsing stream
} StreamStatus;

typedef enum _FileMode {
    FM_Binary,
    FM_Text
} FileMode;

typedef enum _CompressionType {
    CT_None,
    CT_ZIP
} CompressionType;

class XSource
{
public:
    virtual ~XSource() {;}

    virtual char GetByte() = 0;
    virtual void UngetByte() = 0;
    virtual int ReadWord() = 0;
    virtual BOOL ReadDword(LPDWORD pData) = 0;
    virtual BOOL ReadFloat(float *pData) = 0;
    virtual BOOL ReadDouble(double *pData) = 0;

    virtual HRESULT Read(LPVOID pvData, DWORD dwOffset, DWORD cbSize, LPDWORD pcbRead) = 0;

    virtual HRESULT GetFile(LPVOID *ppvFile, LPDWORD pcbFile) = 0;
};

class XSrcMemory : public XSource
{
    LPBYTE  m_pFile;    // Point to the beginning of the file buffer.
    DWORD   m_cbFile;   // File buffer size.
    LPBYTE  m_pEnd;     // Point to the end of the file buffer
    LPBYTE  m_pCur;     // Point to the current reading position.
    BOOL    m_fFree;    // True if need to free m_pFile.

public:
    XSrcMemory(LPVOID pvFile, DWORD cbFile, BOOL fFree = FALSE);
    virtual ~XSrcMemory();

    virtual char GetByte();
    virtual void UngetByte() { m_pCur--; }
    virtual int ReadWord();
    virtual BOOL ReadDword(LPDWORD pData);
    virtual BOOL ReadFloat(float *pData);
    virtual BOOL ReadDouble(double *pData);

    virtual HRESULT Read(LPVOID pvData, DWORD dwOffset, DWORD cbSize, LPDWORD pcbRead);

    virtual HRESULT GetFile(LPVOID *ppvFile, LPDWORD pcbFile);
};

#if 0 // Memory mapped file IO is not currently supported

class XSrcFile : public XSrcMemory
{
    HANDLE m_hFile;
    HANDLE m_hMapFile;
    LPVOID m_pMapFile;

public:
    XSrcFile(HANDLE hFile, HANDLE m_hMapFile, LPVOID pvFile, DWORD cbFile);
    virtual ~XSrcFile();
};

#endif 0

class XStreamRead
{
    XSource      *m_pSrc;       // Source for reading.

    StreamStatus  m_status;     // Current status of this stream

    FileMode      m_mode;       // Text, binary
    DWORD         m_floatSize;  // 32 or 64

    DWORD         m_flags;      // Information from the intrinsic header

    XDataObjList *m_plDataObj;  // This stream's data object list
    DWORD         m_iDataObj;   // Where we are in the data object list

    HRESULT ValidateStream();

    static HRESULT CreateFromMemory(LPVOID pvFile, DWORD cbFile, XStreamRead **ppStream);
    static HRESULT CreateFromFile(LPCSTR szFilename, XStreamRead **ppStream);

public:
    XStreamRead(XSource *pSource);
    ~XStreamRead();

    static HRESULT Create(LPVOID pvSource, DWORD loadflags, XStreamRead **ppStream);

    DWORD floatSize() { return m_floatSize; }
    FileMode mode() { return m_mode; }

    HRESULT SetIntrinsicHeaderInfo(XDataObj *header);

    int AddDataObject(XDataObj *pObj);

    HRESULT GetNextObject(XDataObj **ppObj);
    HRESULT GetObjectById(REFGUID id, XDataObj **ppObj);
    HRESULT GetObjectByName(LPCSTR name, XDataObj **ppObj);
    HRESULT GetObjectByNameAndId(LPCSTR name, REFGUID id, XDataObj **ppObj);

    // Inline read functions.

    char GetByte() { return m_pSrc->GetByte(); }
    void UngetByte() { m_pSrc->UngetByte(); }
    int ReadWord() { return m_pSrc->ReadWord(); }
    BOOL ReadDword(LPDWORD pData) { return m_pSrc->ReadDword(pData); }
    BOOL ReadFloat(float *pData) { return m_pSrc->ReadFloat(pData); }
    BOOL ReadDouble(double *pData) { return m_pSrc->ReadDouble(pData); }

    HRESULT Read(LPVOID pvData, DWORD dwOffset, DWORD cbSize, LPDWORD pcbRead)
      { return m_pSrc->Read(pvData, dwOffset, cbSize, pcbRead); }
};

#endif // _XSTRMRD_H_
