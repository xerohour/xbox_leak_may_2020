
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:13:31 2002
 */
/* Compiler settings for vsempweb.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
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

MIDL_DEFINE_GUID(IID, IID_IVsEmpWeb,0x3675ACF5,0x0C81,0x11d3,0x85,0xC9,0x00,0xA0,0xC9,0xCF,0xCC,0x16);


MIDL_DEFINE_GUID(IID, IID_IVsEmpWebResource,0x3675ACF6,0x0C81,0x11d3,0x85,0xC9,0x00,0xA0,0xC9,0xCF,0xCC,0x16);


MIDL_DEFINE_GUID(IID, IID_IEnumVsEmpWebResourceCheckouts,0x2ABA4462,0x0C9F,0x11d3,0x85,0xC9,0x00,0xA0,0xC9,0xCF,0xCC,0x16);


MIDL_DEFINE_GUID(IID, IID_IEnumVsEmpWebResource,0x2ABA4461,0x0C9F,0x11d3,0x85,0xC9,0x00,0xA0,0xC9,0xCF,0xCC,0x16);


MIDL_DEFINE_GUID(IID, IID_IVsWecFactory,0x8F25BFF6,0x151B,0x11d3,0x85,0xCB,0x00,0xA0,0xC9,0xCF,0xCC,0x16);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

