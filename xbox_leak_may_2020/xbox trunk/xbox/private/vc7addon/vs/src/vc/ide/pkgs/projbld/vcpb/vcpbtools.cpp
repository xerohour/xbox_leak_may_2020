// vcpbeng.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To merge the proxy/stub code into the object DLL, add the file 
//      dlldatax.c to the project.  Make sure precompiled headers 
//      are turned off for this file, and add _MERGE_PROXYSTUB to the 
//      defines for the project.  
//
//      If you are not running WinNT4.0 or Win95 with DCOM, then you
//      need to remove the following define from dlldatax.c
//      #define _WIN32_WINNT 0x0400
//
//      Further, if you are running MIDL without /Oicf switch, you also 
//      need to remove the following define from dlldatax.c.
//      #define USE_STUBLESS_PROXY
//
//      Modify the custom build rule for vcpbeng.idl by adding the following 
//      files to the Outputs.
//          vcpbeng_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f vcpbengps.mk in the project directory.

#include "stdafx.h"

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif


_ATL_OBJMAP_ENTRY ObjectMap2[] = {
	// Base tools

END_OBJECT_MAP()

