// ---------------------------------------------------------------------------------------
// codesize.cpp
//
// Test program which links with all of the external entry points of
// the XNet library.  Used to track the size of the XNet code.
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"

#ifdef XNET_FEATURE_XBOX
#define CODESIZEINHERIT
#else
#define CODESIZEINHERIT     : public CXNet
#endif

void __cdecl Ref(DWORD dw, ...)
{
    static DWORD s_dwCount;
    s_dwCount += dw;
}

class CCodeSize CODESIZEINHERIT
{
public:
    void DoRef();
};

void CCodeSize::DoRef()
{
#ifndef XNETBASELINE
    #undef  XNETAPI
    #define XNETAPI(ret, fname, arglist, paramlist) Ref(0, fname);
    #undef  XNETAPI_
    #define XNETAPI_(ret, fname, arglist, paramlist) Ref(0, fname);
    #undef  XNETAPIV
    #define XNETAPIV(ret, fname, arglist, paramlist) Ref(0, fname);
    XNETAPILIST()
    SOCKAPILIST()
#endif
}

void __cdecl main(int argc, char * argv[])
{
    CCodeSize CodeSize;
    CodeSize.DoRef();
}
