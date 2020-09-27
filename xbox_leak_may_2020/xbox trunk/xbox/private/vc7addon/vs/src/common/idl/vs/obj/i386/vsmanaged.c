
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:35 2002
 */
/* Compiler settings for vsmanaged.idl:
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

MIDL_DEFINE_GUID(IID, LIBID_VSManagedDesigner,0x74946836,0x37A0,0x11d2,0xA2,0x73,0x00,0xC0,0x4F,0x8E,0xF4,0xFF);


MIDL_DEFINE_GUID(IID, IID_IVSMDCodeDomCreator,0x4CC03BF7,0x4D89,0x4198,0x8E,0x4D,0x17,0xE2,0x17,0xCA,0x07,0xB2);


MIDL_DEFINE_GUID(IID, IID_IVSMDCodeDomProvider,0x73E59688,0xC7C4,0x4a85,0xAF,0x64,0xA5,0x38,0x75,0x47,0x84,0xC5);


MIDL_DEFINE_GUID(IID, IID_IVSMDDesigner,0x7494682A,0x37A0,0x11d2,0xA2,0x73,0x00,0xC0,0x4F,0x8E,0xF4,0xFF);


MIDL_DEFINE_GUID(IID, IID_IVSMDDesignerLoader,0x74946834,0x37A0,0x11d2,0xA2,0x73,0x00,0xC0,0x4F,0x8E,0xF4,0xFF);


MIDL_DEFINE_GUID(IID, IID_IVSMDDesignerService,0x74946829,0x37A0,0x11d2,0xA2,0x73,0x00,0xC0,0x4F,0x8E,0xF4,0xFF);


MIDL_DEFINE_GUID(IID, IID_IVSMDPropertyGrid,0x74946837,0x37A0,0x11d2,0xA2,0x73,0x00,0xC0,0x4F,0x8E,0xF4,0xFF);


MIDL_DEFINE_GUID(IID, IID_IVSMDPropertyBrowser,0x74946810,0x37A0,0x11d2,0xA2,0x73,0x00,0xC0,0x4F,0x8E,0xF4,0xFF);


MIDL_DEFINE_GUID(IID, IID_IVSMDPerPropertyBrowsing,0x7494683C,0x37A0,0x11d2,0xA2,0x73,0x00,0xC0,0x4F,0x8E,0xF4,0xFF);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

