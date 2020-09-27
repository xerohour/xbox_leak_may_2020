
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:55 2002
 */
/* Compiler settings for dircontrol.idl:
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

MIDL_DEFINE_GUID(IID, IID_IDirListSite,0xC3270982,0x3ADA,0x42bf,0x8E,0xC5,0x06,0x81,0xDD,0x3F,0x77,0x89);


MIDL_DEFINE_GUID(IID, IID_IDirListEvents,0x4A68351F,0x7AEB,0x4af0,0x97,0x71,0x3F,0x20,0x6E,0x1F,0xF6,0x2F);


MIDL_DEFINE_GUID(IID, IID_IDirList,0x13ACEC02,0xFA52,0x4C89,0x91,0x5B,0x88,0x8D,0x05,0x84,0x4A,0x6F);


MIDL_DEFINE_GUID(IID, IID_IDirAutoCompleteList,0x7A51B0B6,0xC730,0x44CF,0x98,0x33,0xEC,0x5D,0x66,0x6B,0x23,0xDB);


MIDL_DEFINE_GUID(IID, LIBID_DirControlLib,0x4A0C0157,0x01BE,0x47B5,0x98,0xF1,0x0A,0xF6,0x48,0x2A,0x8C,0xA6);


MIDL_DEFINE_GUID(CLSID, CLSID_DirList,0xAFD20A96,0xEED8,0x4D8C,0xA4,0xEA,0x18,0xAB,0x96,0xF6,0xC4,0x32);


MIDL_DEFINE_GUID(CLSID, CLSID_DirAutoCompleteList,0x106E2A2B,0xF06A,0x4535,0x98,0x94,0x43,0xFA,0x46,0x73,0xF9,0xBE);


MIDL_DEFINE_GUID(CLSID, CLSID_DirListPackage,0x5010C52F,0x44AB,0x4051,0x8C,0xE1,0xD3,0x6C,0x20,0xD9,0x89,0xB4);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

