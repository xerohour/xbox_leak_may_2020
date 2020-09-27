//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xobject.h
//
//  Description:    XObject class header file.
//
//  History:
//      01/03/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XOBJECT_H_
#define _XOBJECT_H_

class XDataObj;

#define XDATA_PART_LIST_CHUNK_SIZE  10

////////////////////////////////////////////////////////////////////////////
//
//  XDataPart:
//      A data part can be integerList, floatList, stringList or XObject
//
////////////////////////////////////////////////////////////////////////////

class XDataPart
{
public:
    STDMETHOD_(ULONG, Release) () PURE;
    virtual HRESULT ProcessSelf(XDataObj *pParent) = 0;
};

typedef XObjPtrArray<XDataPart, XDATA_PART_LIST_CHUNK_SIZE> XDataPartList;


////////////////////////////////////////////////////////////////////////////
//
//  XObject: Base class for XTemplate and XData.
//           Handle reference counting.
//
////////////////////////////////////////////////////////////////////////////

class XObject : public IDirectXFileObject, public XDataPart
{
    const XString m_szName;     // Name (optional for data objects).
    const GUID    m_id;         // User's identifier for this object.
    DWORD         m_cRef;       // Reference count.

protected:
// constructor and destructor.

    XObject(LPCSTR szName, REFGUID id) : m_szName(szName), m_id(id), m_cRef(1){}
    virtual ~XObject() {}

public:
// Interfaces.
    STDMETHOD(QueryInterface) (REFIID iid, LPVOID FAR *lplpv) PURE;
    STDMETHOD_(ULONG, AddRef) ();
    STDMETHOD_(ULONG, Release) ();
    STDMETHOD(GetName) (LPSTR szName, LPDWORD pdwSize);
    STDMETHOD(GetId) (LPGUID pId);

// Properties.

    const XString &name() { return m_szName; }
    REFGUID id() { return m_id; }

// Other methods.

    virtual XDataObj *FindDataObj(LPCSTR szName) { return NULL; }
    virtual XDataObj *FindDataObj(REFGUID idSrc) { return NULL; }
    virtual BOOL ValidChild(XDataObj *pParent) = 0;
    virtual HRESULT ProcessSelf(XDataObj *pParent);
};

#endif // _XOBJECT_H_
