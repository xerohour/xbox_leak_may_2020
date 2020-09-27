
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:07 2002
 */
/* Compiler settings for vshelp.idl:
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

MIDL_DEFINE_GUID(IID, LIBID_VsHelp,0x83285928,0x227C,0x11d3,0xB8,0x70,0x00,0xC0,0x4F,0x79,0xF8,0x02);


MIDL_DEFINE_GUID(IID, IID_IVsHelpOwner,0xB9B0983A,0x364C,0x4866,0x87,0x3F,0xD5,0xED,0x19,0x01,0x38,0xFB);


MIDL_DEFINE_GUID(IID, IID_IVsHelpTopicShowEvents,0xD1AAC64A,0x6A25,0x4274,0xB2,0xC6,0xBC,0x3B,0x84,0x0B,0x6E,0x54);


MIDL_DEFINE_GUID(IID, IID_Help,0x4A791148,0x19E4,0x11d3,0xB8,0x6B,0x00,0xC0,0x4F,0x79,0xF8,0x02);


MIDL_DEFINE_GUID(IID, IID_IVsHelpEvents,0x507E4490,0x5A8C,0x11d3,0xB8,0x97,0x00,0xC0,0x4F,0x79,0xF8,0x02);


MIDL_DEFINE_GUID(CLSID, CLSID_DExploreAppObj,0x4A79114D,0x19E4,0x11d3,0xB8,0x6B,0x00,0xC0,0x4F,0x79,0xF8,0x02);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

