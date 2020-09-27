
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:10:51 2002
 */
/* Compiler settings for discoveryservice.idl:
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

MIDL_DEFINE_GUID(IID, IID_IVsDiscoveryService,0xB9A32C80,0xB14D,0x4ae3,0xA9,0x55,0x5C,0xBC,0x3E,0x7F,0xAB,0x10);


MIDL_DEFINE_GUID(IID, IID_IDiscoverUrlCallBack,0x0EEA651C,0xB208,0x4ede,0x96,0xCE,0x51,0x94,0xF4,0xDC,0x4E,0x4A);


MIDL_DEFINE_GUID(IID, IID_IDiscoveryResult,0xB9A32C91,0xB14D,0x4ae3,0xA9,0x55,0x5C,0xBC,0x3E,0x75,0xFC,0xA5);


MIDL_DEFINE_GUID(IID, IID_IReferenceInfo,0xB9A32C92,0xB14D,0x4ae3,0xA9,0x55,0x5C,0xBC,0x3E,0x75,0xFC,0xA5);


MIDL_DEFINE_GUID(IID, IID_ISchemaReferenceInfo,0xB9A32C92,0xB14D,0x4ae3,0xA9,0x55,0x5C,0xBC,0x3E,0x75,0xFC,0xA8);


MIDL_DEFINE_GUID(IID, IID_IUpdateWebReferenceService,0xBB67D0B5,0xA285,0x4085,0x9A,0xA2,0xF3,0xA8,0x56,0x1B,0xD4,0x37);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

