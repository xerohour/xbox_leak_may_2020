//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xblob.h
//
//  Description:    Blob data related classes header file.
//
//  History:
//      01/13/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XBLOB_H_
#define _XBLOB_H_

class XString;
class XData;
class XObject;
class XStreamRead;
class XStreamWrite;

////////////////////////////////////////////////////////////////////////////
//
//  XBlobData: Binary data object's data.
//
////////////////////////////////////////////////////////////////////////////

class XBlobData
{
public:
    DWORD   m_dwOffset;
    XString m_szMimeType;
    DWORD   m_cbSize;

    XBlobData(DWORD dwOffset, LPCSTR szMimeType, DWORD cbSize);
};

////////////////////////////////////////////////////////////////////////////
//
//  XBinaryData: Binary data object.
//
////////////////////////////////////////////////////////////////////////////

class XBinaryData : public IDirectXFileBinary,
                    public XObject
{
    XString m_szMimeType;
    LPVOID m_pvData;
    DWORD m_cbSize;
    DWORD m_cbRead;
    XStreamRead *m_pStream;
    DWORD m_dwOffset;

public:
    XBinaryData(LPCSTR szName, REFGUID id, XBlobData *pBlobData,
                XStreamRead *pStream);

    XBinaryData(LPCSTR szName, REFGUID id, LPCSTR mimeType,
                LPVOID data, DWORD cbSize);

    virtual ~XBinaryData();

    virtual BOOL ValidChild(XDataObj *pParent) { return pParent->AllowBinaryData(); }
    virtual HRESULT Save(XStreamWrite *pStream);

// Interfaces.

    STDMETHOD(QueryInterface) (REFIID iid, LPVOID FAR *lplpv);
    STDMETHOD_(ULONG, AddRef) () { return XObject::AddRef(); }
    STDMETHOD_(ULONG, Release) () { return XObject::Release(); }
    STDMETHOD(GetName) (LPSTR szName, LPDWORD pdwSize) { return XObject::GetName(szName, pdwSize); }
    STDMETHOD(GetId) (LPGUID pId) { return XObject::GetId(pId); }
    STDMETHOD(GetSize) (DWORD *);
    STDMETHOD(GetMimeType) (LPCSTR *);
    STDMETHOD(Read) (LPVOID, DWORD, LPDWORD);
};

#endif // _XBLOB_H_
