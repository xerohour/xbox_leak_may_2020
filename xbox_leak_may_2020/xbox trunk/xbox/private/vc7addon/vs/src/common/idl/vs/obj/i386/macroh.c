
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:30 2002
 */
/* Compiler settings for macroh.idl:
    Oicf, W0, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)

#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_VsMacroHierarchyLib,0x7E9B6563,0xC142,0x11d2,0xB2,0x5F,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(IID, IID_IRootProps,0x1E9AD28F,0xC21D,0x11d2,0xB2,0x60,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(IID, IID_IProjectProps,0x7E9B6564,0xC142,0x11d2,0xB2,0x5F,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(IID, IID_IModuleProps,0x5B52E1CD,0xC20D,0x11d2,0xB2,0x60,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(IID, IID_IMacroProps,0x5B52E1CE,0xC20D,0x11d2,0xB2,0x60,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(IID, DIID__dispEnvironmentEvents,0x336CA0A6,0x0E0B,0x11d3,0xB2,0x99,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(IID, IID__EnvironmentEvents,0x336CA0A7,0x0E0B,0x11d3,0xB2,0x99,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(CLSID, CLSID_EnvironmentEvents,0x336CA0A5,0x0E0B,0x11d3,0xB2,0x99,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(IID, DIID__dispVSHostExtender,0xC4D19869,0x0EF8,0x11d3,0xB2,0x99,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(IID, IID__VSHostExtender,0xC4D1986A,0x0EF8,0x11d3,0xB2,0x99,0x00,0xC0,0x4F,0x68,0x8E,0x57);


MIDL_DEFINE_GUID(CLSID, CLSID_VSHostExtender,0xC4D1986B,0x0EF8,0x11d3,0xB2,0x99,0x00,0xC0,0x4F,0x68,0x8E,0x57);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

