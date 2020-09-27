
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:43 2002
 */
/* Compiler settings for compsvcspkg.idl:
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

MIDL_DEFINE_GUID(IID, IID_IVsAddWebReferenceDlg,0xBD71396F,0x39C6,0x4e3f,0xBB,0xA2,0x79,0xCE,0x33,0xA8,0xB3,0x02);


MIDL_DEFINE_GUID(IID, IID_IEnumComponents,0x9a04b730,0x656c,0x11d3,0x85,0xfc,0x00,0xc0,0x4f,0x61,0x23,0xb3);


MIDL_DEFINE_GUID(IID, IID_IVsComponentSelectorData,0x45bd8e74,0x6727,0x11d3,0x86,0x00,0x00,0xc0,0x4f,0x61,0x23,0xb3);


MIDL_DEFINE_GUID(IID, IID_IVsComponentEnumeratorFactory,0x332cedee,0x6610,0x11d3,0x85,0xfd,0x00,0xc0,0x4f,0x61,0x23,0xb3);


MIDL_DEFINE_GUID(IID, IID_IVsComponentEnumeratorFactory2,0x64F6B8C1,0x3DEC,0x4606,0x8C,0x8C,0x65,0x1A,0x7E,0x26,0xA3,0xDE);


MIDL_DEFINE_GUID(IID, IID_IVsProvideComponentEnumeration,0x4df7bba0,0x660f,0x11d3,0x85,0xfd,0x00,0xc0,0x4f,0x61,0x23,0xb3);


MIDL_DEFINE_GUID(IID, LIBID_CompServicesLib,0x39e3ed68,0x6610,0x11d3,0x85,0xfd,0x00,0xc0,0x4f,0x61,0x23,0xb3);


MIDL_DEFINE_GUID(CLSID, CLSID_CCompServicesPackage,0x588205e0,0x66e0,0x11d3,0x86,0x00,0x00,0xc0,0x4f,0x61,0x23,0xb3);


MIDL_DEFINE_GUID(CLSID, CLSID_CCom2Enumerator,0x3129723e,0x660f,0x11d3,0x85,0xfd,0x00,0xc0,0x4f,0x61,0x23,0xb3);


MIDL_DEFINE_GUID(CLSID, CLSID_CComPlusEnumerator,0xf5bd4a64,0x67a2,0x11d3,0x86,0x00,0x00,0xc0,0x4f,0x61,0x23,0xb3);


MIDL_DEFINE_GUID(IID, IID_IVsTypeLibraryWrapperCallback,0xAFF2B95E,0x229B,0x4A14,0xA4,0x22,0xE9,0x94,0x52,0xAD,0x2F,0x8C);


MIDL_DEFINE_GUID(IID, IID_IVsTypeLibraryWrapper,0xE6065B70,0xC9B6,0x4636,0x80,0xF5,0x1C,0xF9,0x2D,0x7E,0xCE,0x5B);


MIDL_DEFINE_GUID(CLSID, CLSID_VSPIAImporter,0xe096ea37,0xd27e,0x48b6,0x97,0x0e,0xe6,0x67,0xf9,0x12,0x56,0xe6);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

