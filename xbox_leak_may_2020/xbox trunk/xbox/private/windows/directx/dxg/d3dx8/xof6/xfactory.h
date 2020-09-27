//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xfactory.h
//
//  Description:    XFactory class header file.
//
//  History:
//      04/22/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XFACTORY_H_
#define _XFACTORY_H_

////////////////////////////////////////////////////////////////////////////
//
//  XFactory: IClassFactory impl class.
//
////////////////////////////////////////////////////////////////////////////

class XFactory : public IClassFactory
{
private:
    ULONG m_cRef;

public:
    XFactory() : m_cRef(1) {}

    STDMETHOD(QueryInterface) (REFIID iid, LPVOID *ppv);
    STDMETHOD_(ULONG, AddRef) ();
    STDMETHOD_(ULONG, Release) ();

    STDMETHOD(CreateInstance) (LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppv);
    STDMETHOD(LockServer) (BOOL fLock);
};

#endif // _XFACTORY_H_
