/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    object.h

Description:

    Fundamental base class.

*******************************************************************************/

#ifndef __OBJECT_H__
#define __OBJECT_H__

//******************************************************************************
// CObject
//******************************************************************************

class CObject {

private:

    ULONG                       m_uRef;

public:

                                CObject();
    virtual                     ~CObject();

    virtual ULONG               AddRef();
    virtual ULONG               Release();

    virtual ULONG               RefCount();
};

#endif //__OBJECT_H__
