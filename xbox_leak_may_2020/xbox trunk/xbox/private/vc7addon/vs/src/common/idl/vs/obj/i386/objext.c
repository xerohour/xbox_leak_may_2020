
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:54 2002
 */
/* Compiler settings for objext.idl:
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

MIDL_DEFINE_GUID(IID, IID_IDocumentSite,0x94A0F6F1,0x10BC,0x11d0,0x8D,0x09,0x00,0xA0,0xC9,0x0F,0x27,0x32);


MIDL_DEFINE_GUID(IID, IID_IDocumentSite2,0x61D4A8A1,0x2C90,0x11d2,0xAD,0xE4,0x00,0xC0,0x4F,0x98,0xF4,0x17);


MIDL_DEFINE_GUID(IID, IID_IRequireClasses,0x6d5140d0,0x7436,0x11ce,0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa);


MIDL_DEFINE_GUID(IID, IID_ILicensedClassManager,0x6d5140d4,0x7436,0x11ce,0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa);


MIDL_DEFINE_GUID(IID, IID_IExtendedTypeLib,0x6d5140d6,0x7436,0x11ce,0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa);


MIDL_DEFINE_GUID(IID, IID_ILocalRegistry,0x6d5140d3,0x7436,0x11ce,0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa);


MIDL_DEFINE_GUID(IID, IID_ILocalRegistry2,0x77BB19B0,0x0462,0x11d1,0xAA,0xF6,0x00,0xA0,0xC9,0x05,0x5A,0x90);


MIDL_DEFINE_GUID(IID, IID_ILocalRegistry3,0x1B01F13F,0xABEE,0x4761,0x91,0xAF,0x76,0xCE,0x6B,0x4C,0x9E,0x7A);


MIDL_DEFINE_GUID(IID, IID_IUIElement,0x759d0500,0xd979,0x11ce,0x84,0xec,0x00,0xaa,0x00,0x61,0x4f,0x3e);


MIDL_DEFINE_GUID(IID, IID_ICategorizeProperties,0x4D07FC10,0xF931,0x11ce,0xB0,0x01,0x00,0xAA,0x00,0x68,0x84,0xE5);


MIDL_DEFINE_GUID(IID, IID_IHelp,0x6d5140c8,0x7436,0x11ce,0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

