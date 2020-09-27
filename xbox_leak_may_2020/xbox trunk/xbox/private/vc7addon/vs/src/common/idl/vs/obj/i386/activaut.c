
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:25 2002
 */
/* Compiler settings for activaut.idl:
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

MIDL_DEFINE_GUID(IID, IID_IScriptNode,0x0AEE2A94,0xBCBB,0x11d0,0x8C,0x72,0x00,0xC0,0x4F,0xC2,0xB0,0x85);


MIDL_DEFINE_GUID(IID, IID_IScriptEntry,0x0AEE2A95,0xBCBB,0x11d0,0x8C,0x72,0x00,0xC0,0x4F,0xC2,0xB0,0x85);


MIDL_DEFINE_GUID(IID, IID_IScriptScriptlet,0x0AEE2A96,0xBCBB,0x11d0,0x8C,0x72,0x00,0xC0,0x4F,0xC2,0xB0,0x85);


MIDL_DEFINE_GUID(IID, IID_IActiveScriptAuthor,0x9C109DA0,0x7006,0x11d1,0xB3,0x6C,0x00,0xA0,0xC9,0x11,0xE8,0xB2);


MIDL_DEFINE_GUID(IID, IID_IActiveScriptAuthorProcedure,0x7E2D4B70,0xBD9A,0x11d0,0x93,0x36,0x00,0xA0,0xC9,0x0D,0xCA,0xA9);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

