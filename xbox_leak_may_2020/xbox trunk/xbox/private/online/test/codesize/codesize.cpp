// ---------------------------------------------------------------------------------------
// codesize.cpp
//
// Test program which links with all of the external entry points of
// the XOnline library.  Used to track the size of the XOnline code.
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifdef _XBOX
    #include <xtl.h>
#else
    #include <windows.h>
#endif

#include <winsockx.h>
#include <winsockp.h>
#include <xonlinep.h>
#include <stdio.h>

#ifdef _XBOX
#define CODESIZEINHERIT
#else
#define CODESIZEINHERIT     : public CXOnline
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
#ifndef XONLINEBASELINE

    #undef  XNETAPI
    #define XNETAPI(ret, fname, arglist, paramlist) Ref(0, fname);
    #undef  XNETAPI_
    #define XNETAPI_(ret, fname, arglist, paramlist) Ref(0, fname);
    #undef  XNETAPIV
    #define XNETAPIV(ret, fname, arglist, paramlist) Ref(0, fname);

    XNETAPILIST()
    SOCKAPILIST()

    #undef  XONAPI
    #define XONAPI(ret, fname, arglist, paramlist) Ref(0, fname);
    #undef  XONAPI_
    #define XONAPI_(ret, fname, arglist, paramlist)
    #undef  XONAPIV
    #define XONAPIV(ret, fname, arglist, paramlist) Ref(0, fname);

    XONLINEAPILIST()

#endif
}

void __cdecl main(int argc, char * argv[])
{
    CCodeSize CodeSize;
    CodeSize.DoRef();
}
