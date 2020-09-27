//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   ximplapi.h
//
//  Description:    api interface implementation classes.
//
//  History:
//      01/13/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XIMPLAPI_H_
#define _XIMPLAPI_H_

////////////////////////////////////////////////////////////////////////////
//
//  XDirctXFile: Implement IDirectXFile interface.
//
////////////////////////////////////////////////////////////////////////////

class XDirectXFile : public IDirectXFile {
private:
    ULONG m_cRef;

public:
    XDirectXFile() { m_cRef = 1; }

    STDMETHOD(QueryInterface) (REFIID iid, LPVOID FAR *lplpv);
    STDMETHOD_(ULONG, AddRef) ();
    STDMETHOD_(ULONG, Release) ();

    STDMETHOD(CreateEnumObject) (LPVOID, DWORD, IDirectXFileEnumObject **);

    STDMETHOD(CreateSaveObject) (LPCSTR, DWORD, IDirectXFileSaveObject **);

    STDMETHOD(RegisterTemplates) (LPVOID, DWORD);
};

////////////////////////////////////////////////////////////////////////////
//
//  XEnumObject: Implement IDirectXFileEnumObject interface.
//
////////////////////////////////////////////////////////////////////////////

class XEnumObject : public IDirectXFileEnumObject {
private:
    ULONG m_cRef;
    XStreamRead *m_pStream;

public:
    XEnumObject(XStreamRead *pStream)
      : m_pStream(pStream), m_cRef(1) {}

    STDMETHOD(QueryInterface) (REFIID iid, LPVOID FAR *lplpv);
    STDMETHOD_(ULONG, AddRef) ();
    STDMETHOD_(ULONG, Release) ();
    STDMETHOD(GetNextDataObject) (IDirectXFileData **);
    STDMETHOD(GetDataObjectById) (REFGUID, IDirectXFileData **);
    STDMETHOD(GetDataObjectByName) (LPCSTR, IDirectXFileData **);
};


#endif // _XIMPLAPI_H_
