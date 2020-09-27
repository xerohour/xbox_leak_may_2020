//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xdata.h
//
//  Description:    Data object classes header file.
//
//  History:
//      01/10/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XDATA_H_
#define _XDATA_H_

class XTemplate;
class XStreamWrite;
class XStreamRead;

#define INTEGER_LIST_CHUNK_SIZE     10
#define FLOAT_LIST_CHUNK_SIZE       250
#define STRING_LIST_CHUNK_SIZE      10
#define XOBJECT_LIST_CHUNK_SIZE     10

typedef XObjPtrArray<XObject, XOBJECT_LIST_CHUNK_SIZE> XObjectList;

////////////////////////////////////////////////////////////////////////////
//
//  XDataObj: Data objects with real data.
//
////////////////////////////////////////////////////////////////////////////

class XDataObj : public IDirectXFileData,
                 public XObject
{
    XTemplate      *m_pTemplate;
    XDataObj       *m_pParent;      // This object's parent.
    XObjectList    *m_plChildren;   // Optional data objects.
    DWORD           m_iChild;       // Current child index in the list.
    XDataPartList  *m_plDataPart;   // data part list.
    DWORD           m_cbSize;       // number of bytes m_pData points to.
    LPVOID          m_pData;        // pointer to the chunk of data.

    XDataObj(XTemplate *pTemplate, LPCSTR szName, REFGUID id,
             DWORD cbSize, LPVOID pData);

    XDataObj(XTemplate *pTemplate, LPCSTR szName, REFGUID id,
             XDataPartList *plDataPart);

    HRESULT ProcessDataPartList();

public:
    static HRESULT Create(REFGUID idType,
                          LPCSTR szName,
                          REFGUID id,
                          DWORD cbSize,
                          LPVOID pData,
                          XDataObj **ppObj);

    static XDataObj *Create(LPCSTR szType,
                            LPCSTR szName,
                            REFGUID id,
                            XDataPartList *plDataPart);

    virtual ~XDataObj();

    virtual XDataObj *FindDataObj(LPCSTR szName);
    virtual XDataObj *FindDataObj(REFGUID id);
    virtual BOOL ValidChild(XDataObj *pParent) { return pParent->Allow(m_pTemplate->name()); }

    BOOL Allow(LPCSTR szType);
    BOOL AllowDataRef() { return !m_pTemplate->IsClosed(); }
    BOOL AllowBinaryData() { return Allow(SZBINARY); }
    HRESULT AddChild(XObject *pChild);
    HRESULT AddData(LPVOID pData, DWORD cbData);
    void ResetChildIndex() { m_iChild = 0; }

// Interface methods.

    STDMETHOD(QueryInterface) (REFIID iid, LPVOID FAR *lplpv);
    STDMETHOD_(ULONG, AddRef) () { return XObject::AddRef(); }
    STDMETHOD_(ULONG, Release) () { return XObject::Release(); }
    STDMETHOD(GetName) (LPSTR szName, LPDWORD pdwSize) { return XObject::GetName(szName, pdwSize); }
    STDMETHOD(GetId) (LPGUID pId) { return XObject::GetId(pId); }
    STDMETHOD(GetData) (const CHAR *, DWORD *, void **);
    STDMETHOD(GetType) (const CLSID **);
    STDMETHOD(GetNextObject) (interface IDirectXFileObject **);
    STDMETHOD(AddDataObject) (interface IDirectXFileData *);
    STDMETHOD(AddDataReference) (LPCSTR, const GUID *);
    STDMETHOD(AddBinaryObject) (LPCSTR, const GUID *, LPCSTR, LPVOID, DWORD);
};

////////////////////////////////////////////////////////////////////////////
//
//  XDataRef: Data that reference to other data objects.
//
////////////////////////////////////////////////////////////////////////////

class XDataRef : public IDirectXFileDataReference,
                 public XObject
{
    const XString  m_szRef;
    const GUID     m_idRef;
    XStreamRead   *m_pStream;

public:
    XDataRef(LPCSTR szRef, REFGUID idRef, XStreamRead *pStream);

    virtual BOOL ValidChild(XDataObj *pParent) { return pParent->AllowDataRef(); }

// Interface methods.

    STDMETHOD(QueryInterface) (REFIID iid, LPVOID FAR *lplpv);
    STDMETHOD_(ULONG, AddRef) () { return XObject::AddRef(); }
    STDMETHOD_(ULONG, Release) () { return XObject::Release(); }
    STDMETHOD(GetName) (LPSTR szName, LPDWORD pdwSize) { return XObject::GetName(szName, pdwSize); }
    STDMETHOD(GetId) (LPGUID pId) { return XObject::GetId(pId); }
    STDMETHOD(Resolve) (IDirectXFileData **);
};

////////////////////////////////////////////////////////////////////////////
//
//  XDataArray: Template class for integerList, floatList and stringList.
//
////////////////////////////////////////////////////////////////////////////

typedef DWORD  stgInt;
typedef float  stgFlt;

template<class T>
class XSizedData {
public:
    DWORD count;
    T *data;
};

typedef XSizedData<stgInt> integerListBinData;
typedef XSizedData<stgFlt> floatListBinData;

template<class T, DWORD cChunk>
class XDataArray : public XArray<T, cChunk>, public XDataPart
{
public:
    XDataArray() : XArray<T, cChunk>() {}

    XDataArray(const XSizedData<T> &src)
      : XArray<T, cChunk>(src.count, src.data) {}

    STDMETHOD_(ULONG, Release) () { delete this; return 0; }
    virtual HRESULT ProcessSelf(XDataObj *pParent) { return pParent->AddData((LPVOID)list(), count()*sizeof(T)); }
};

typedef XDataArray<stgInt, INTEGER_LIST_CHUNK_SIZE> integerList;

typedef XDataArray<stgFlt, FLOAT_LIST_CHUNK_SIZE> floatList;

class stringList : public XDataArray<LPCSTR, STRING_LIST_CHUNK_SIZE>
{
    STDMETHOD_(ULONG, Release) ();
};

#endif // _XDATA_H_
