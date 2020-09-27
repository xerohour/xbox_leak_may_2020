
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:10:49 2002
 */
/* Compiler settings for designer.idl:
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

MIDL_DEFINE_GUID(IID, IID_IActiveDesigner,0x51aae3e0,0x7486,0x11cf,0xa0,0xC2,0x00,0xaa,0x00,0x62,0xbe,0x57);


MIDL_DEFINE_GUID(IID, IID_ICodeNavigate,0x6d5140c4,0x7436,0x11ce,0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa);


MIDL_DEFINE_GUID(IID, IID_ICodeNavigate2,0x2702ad60,0x3459,0x11d1,0x88,0xfd,0x00,0xa0,0xc9,0x11,0x00,0x49);


MIDL_DEFINE_GUID(IID, IID_ISelectionContainer,0x6d5140c6,0x7436,0x11ce,0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa);


MIDL_DEFINE_GUID(IID, IID_ITrackSelection,0x6d5140c5,0x7436,0x11ce,0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa);


MIDL_DEFINE_GUID(IID, IID_IProfferTypeLib,0x718cc500,0x0a76,0x11cf,0x80,0x45,0x00,0xaa,0x00,0x60,0x09,0xfa);


MIDL_DEFINE_GUID(IID, IID_IProvideDynamicClassInfo,0x468cfb80,0xb4f9,0x11cf,0x80,0xdd,0x00,0xaa,0x00,0x61,0x48,0x95);


MIDL_DEFINE_GUID(IID, IID_IExtendedObject,0xA575C060,0x5B17,0x11d1,0xAB,0x3E,0x00,0xA0,0xC9,0x05,0x5A,0x90);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

