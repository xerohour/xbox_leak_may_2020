
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:14:20 2002
 */
/* Compiler settings for vcpbobjects.idl:
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

MIDL_DEFINE_GUID(IID, IID_IVCCollection,0xA54AAE9A,0x30C2,0x11D3,0x87,0xBF,0xA0,0x4A,0x4C,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_VCPlatform,0xA54AAE90,0x30C2,0x11D3,0x87,0xBF,0xA0,0x4A,0x4C,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_VCDebugSettings,0xA54AAE87,0x30C2,0x11D3,0x87,0xBF,0xA0,0x4A,0x4C,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_VCProjectItem,0x33F3D4A0,0x6763,0x4b88,0xB8,0xC5,0x47,0x84,0x46,0xEF,0xB5,0x1E);


MIDL_DEFINE_GUID(IID, IID_VCProject,0x625C6494,0x2C7E,0x11D3,0x87,0xBF,0xA0,0x49,0x4C,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_VCFile,0x625C6497,0x2C7E,0x11D3,0x87,0xBF,0xA0,0x49,0x4C,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_VCFilter,0x625C649A,0x2C7E,0x11D3,0x87,0xBF,0xA0,0x49,0x4C,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_VCConfiguration,0x625C649D,0x2C7E,0x11D3,0x87,0xBF,0xA0,0x49,0x4C,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_VCStyleSheet,0xCC7AEBB9,0xA788,0x11d3,0xB9,0x9F,0x00,0xC0,0x4F,0x68,0x5D,0x26);


MIDL_DEFINE_GUID(IID, IID_VCFileConfiguration,0xA54AAE8A,0x30C2,0x11D3,0x87,0xBF,0xA0,0x4A,0x4C,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_VCProjectEngine,0x9F367026,0x33C6,0x11D3,0x8D,0x52,0x00,0xC0,0x4F,0x8E,0xCD,0xB6);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

