
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:12 2002
 */
/* Compiler settings for textfind.idl:
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

MIDL_DEFINE_GUID(IID, LIBID_TextFind,0xD8052AA0,0xC06A,0x11d2,0x93,0x6C,0xD7,0x14,0x76,0x6E,0x8B,0x50);


MIDL_DEFINE_GUID(IID, IID_IVsTextSpanSet,0xE6BBDFD0,0xC06A,0x11d2,0x93,0x6C,0xD7,0x14,0x76,0x6E,0x8B,0x50);


MIDL_DEFINE_GUID(IID, IID_IVsFinder,0xD0C79C30,0xC06A,0x11d2,0x93,0x6C,0xD7,0x14,0x76,0x6E,0x8B,0x50);


MIDL_DEFINE_GUID(IID, IID_IVsFindHelper,0xBB02E777,0xEC3E,0x479f,0x98,0x0E,0x8E,0x36,0xB9,0x61,0x7D,0xCA);


MIDL_DEFINE_GUID(IID, IID_IVsFindTarget,0xDE89D360,0xC06A,0x11d2,0x93,0x6C,0xD7,0x14,0x76,0x6E,0x8B,0x50);


MIDL_DEFINE_GUID(IID, IID_IVsFindScope,0x0FE9496A,0x129C,0x40ee,0x99,0xD8,0x77,0x05,0xA6,0x28,0x35,0x18);


MIDL_DEFINE_GUID(IID, IID_IVsRegisterFindScope,0x08363D65,0xC178,0x4484,0xA2,0x93,0xD1,0x48,0x17,0x3D,0xD7,0x50);


MIDL_DEFINE_GUID(IID, IID_IVsHighlight,0x22212CD2,0x83FF,0x11d2,0x92,0xE6,0x00,0x53,0x45,0x00,0x00,0x00);


MIDL_DEFINE_GUID(IID, IID_IVsTextImageUtilities,0xC5557BAA,0xB685,0x46dc,0xAF,0x46,0xC1,0x6B,0xFD,0xE8,0xFA,0xB7);


MIDL_DEFINE_GUID(CLSID, CLSID_VsFinder,0x9F842E53,0xE27B,0x426f,0xAE,0x69,0x8F,0x07,0x57,0x70,0xC6,0xF3);


MIDL_DEFINE_GUID(CLSID, CLSID_VsTextSpanSet,0x31A81214,0x5A9F,0x4532,0xAC,0xD0,0x32,0x04,0xA9,0xAB,0x5D,0x08);


MIDL_DEFINE_GUID(CLSID, CLSID_VsTextImage,0x66B88230,0x2363,0x4992,0xB7,0x40,0xB0,0x45,0x0A,0x6F,0x51,0xCA);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

