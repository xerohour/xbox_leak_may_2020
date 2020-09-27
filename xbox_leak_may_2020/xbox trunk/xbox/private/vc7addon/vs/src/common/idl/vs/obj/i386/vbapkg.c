
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:27 2002
 */
/* Compiler settings for vbapkg.idl:
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

MIDL_DEFINE_GUID(IID, LIBID_VsVbaPackageLib,0xA659F1B1,0xAD34,0x11d1,0xAB,0xAD,0x00,0x80,0xC7,0xB8,0x9C,0x95);


MIDL_DEFINE_GUID(IID, IID_IVsVba,0xA659F1B2,0xAD34,0x11d1,0xAB,0xAD,0x00,0x80,0xC7,0xB8,0x9C,0x95);


MIDL_DEFINE_GUID(IID, IID_IVsMacros,0x55ED27C1,0x4CE7,0x11d2,0x89,0x0F,0x00,0x60,0x08,0x31,0x96,0xC6);


MIDL_DEFINE_GUID(IID, IID_IVsMacroRecorder,0x04BBF6A5,0x4697,0x11d2,0x89,0x0E,0x00,0x60,0x08,0x31,0x96,0xC6);


MIDL_DEFINE_GUID(CLSID, CLSID_VsVbaPackage,0xA659F1B3,0xAD34,0x11d1,0xAB,0xAD,0x00,0x80,0xC7,0xB8,0x9C,0x95);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

