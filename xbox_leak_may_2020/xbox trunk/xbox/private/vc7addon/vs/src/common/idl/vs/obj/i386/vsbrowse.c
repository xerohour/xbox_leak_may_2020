
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:04 2002
 */
/* Compiler settings for vsbrowse.idl:
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

MIDL_DEFINE_GUID(IID, IID_IVsWebURLMRU,0xe8b06f3d,0x6d01,0x11d2,0xaa,0x7d,0x00,0xc0,0x4f,0x99,0x03,0x43);


MIDL_DEFINE_GUID(IID, IID_IVsWebFavorites,0xe8b06f4c,0x6d01,0x11d2,0xaa,0x7d,0x00,0xc0,0x4f,0x99,0x03,0x43);


MIDL_DEFINE_GUID(IID, IID_IVsFavoritesProvider,0xe8b06f4d,0x6d01,0x11d2,0xaa,0x7d,0x00,0xc0,0x4f,0x99,0x03,0x43);


MIDL_DEFINE_GUID(IID, IID_IVsWebBrowserUser,0xe8b06f4b,0x6d01,0x11d2,0xaa,0x7d,0x00,0xc0,0x4f,0x99,0x03,0x43);


MIDL_DEFINE_GUID(IID, IID_IVsWebBrowsingService,0xe8b06f51,0x6d01,0x11d2,0xaa,0x7d,0x00,0xc0,0x4f,0x99,0x03,0x43);


MIDL_DEFINE_GUID(IID, IID_IVsWebBrowser,0xe8b06f50,0x6d01,0x11d2,0xaa,0x7d,0x00,0xc0,0x4f,0x99,0x03,0x43);


MIDL_DEFINE_GUID(IID, IID_IVsWebPreviewAction,0x9EC9BA56,0xB328,0x11d2,0x9A,0x98,0x00,0xC0,0x4F,0x79,0xEF,0xC3);


MIDL_DEFINE_GUID(IID, IID_IVsWebPreview,0x9EC9BA55,0xB328,0x11d2,0x9A,0x98,0x00,0xC0,0x4F,0x79,0xEF,0xC3);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

