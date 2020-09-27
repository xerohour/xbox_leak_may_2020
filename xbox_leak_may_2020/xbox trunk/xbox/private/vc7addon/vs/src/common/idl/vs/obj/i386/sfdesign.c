
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:37 2002
 */
/* Compiler settings for sfdesign.idl:
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

MIDL_DEFINE_GUID(IID, LIBID_SFDesign,0xE340A220,0xF45F,0x11d2,0x8F,0xB9,0x00,0xC0,0x4F,0x79,0xA0,0xAB);


MIDL_DEFINE_GUID(IID, IID_IWebFormDesigner,0xE340A221,0xF45F,0x11d2,0x8F,0xB9,0x00,0xC0,0x4F,0x79,0xA0,0xAB);


MIDL_DEFINE_GUID(IID, IID_IWebFormsControlDesigner,0x3911F71D,0xC6D7,0x44a4,0xA1,0xEB,0x4D,0x11,0x3B,0x9B,0x2B,0x95);


MIDL_DEFINE_GUID(IID, IID_ISourceViewEventSink,0xEF55C6B7,0x5455,0x456a,0x9F,0xE4,0xAB,0xDC,0x2B,0x6F,0x01,0xB5);


MIDL_DEFINE_GUID(IID, IID_IWebFormsBehavior,0x9212C799,0xBA0F,0x421a,0x91,0x52,0xB5,0x7F,0xB7,0x16,0x26,0x72);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

