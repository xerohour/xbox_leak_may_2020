
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:10:48 2002
 */
/* Compiler settings for context.idl:
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

MIDL_DEFINE_GUID(IID, IID_IVsUserContext,0x761081DF,0xD45F,0x4683,0x9B,0x9E,0x1B,0x72,0x41,0xE5,0x6F,0x5C);


MIDL_DEFINE_GUID(IID, IID_IVsUserContextUpdate,0xF5ED7D1C,0x61B6,0x428A,0x81,0x29,0xE1,0x3B,0x36,0xD9,0xE9,0xA7);


MIDL_DEFINE_GUID(IID, IID_IVsProvideUserContext,0x997D7904,0xD948,0x4C8B,0x8B,0xAB,0x0B,0xDA,0x1E,0x21,0x2F,0x6E);


MIDL_DEFINE_GUID(IID, IID_IVsProvideUserContextForObject,0xF98CCC8A,0x9C5F,0x41EB,0x84,0x21,0x71,0x1C,0x0F,0x18,0x80,0xE6);


MIDL_DEFINE_GUID(IID, IID_IVsUserContextItemCollection,0x2A6DE4A2,0x5B3D,0x46EB,0xA6,0x5C,0x24,0xC4,0xEF,0x4F,0x39,0x6F);


MIDL_DEFINE_GUID(IID, IID_IVsUserContextItem,0x720B8500,0x17B3,0x4C89,0xAE,0x84,0x2C,0xFE,0x72,0x51,0xB4,0xB8);


MIDL_DEFINE_GUID(IID, IID_IVsHelpAttributeList,0x0A56FB1E,0x1B2F,0x4699,0x81,0x78,0x63,0xB9,0x8E,0x81,0x6F,0x35);


MIDL_DEFINE_GUID(IID, IID_IVsMonitorUserContext,0x9C074FDB,0x3D7D,0x4512,0x96,0x04,0x72,0xB3,0xB0,0xA5,0xF6,0x09);


MIDL_DEFINE_GUID(IID, IID_IVsUserContextItemProvider,0x715C98B7,0x05FB,0x4A1A,0x86,0xC8,0xFF,0x00,0xCE,0x2E,0x5D,0x64);


MIDL_DEFINE_GUID(IID, IID_IVsUserContextCustomize,0x0F817159,0x761D,0x447e,0x96,0x00,0x4C,0x33,0x87,0xF4,0xC0,0xFD);


MIDL_DEFINE_GUID(IID, IID_IVsUserContextItemEvents,0xA2078F0E,0xA310,0x420A,0xBA,0x27,0x16,0x53,0x19,0x05,0xB8,0x8F);


MIDL_DEFINE_GUID(IID, LIBID_VsContext,0x9E9C5B7E,0x02B2,0x41B5,0x8D,0x81,0xDF,0x77,0x3C,0x10,0xD0,0xBE);


MIDL_DEFINE_GUID(CLSID, CLSID_VsContextClass,0x3c1f59c6,0x69cf,0x11d2,0xaa,0x7c,0x00,0xc0,0x4f,0x99,0x03,0x43);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

