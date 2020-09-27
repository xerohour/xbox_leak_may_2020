
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:15 2002
 */
/* Compiler settings for undoredo.idl:
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

MIDL_DEFINE_GUID(IID, IID_IAction,0xD8C8AB11,0x7D4A,0x11d0,0xA8,0xAA,0x00,0xA0,0xC9,0x21,0xA4,0xD2);


MIDL_DEFINE_GUID(IID, IID_IEnumActions,0x7A6335C8,0x7884,0x11d0,0xA8,0xA9,0x00,0xA0,0xC9,0x21,0xA4,0xD2);


MIDL_DEFINE_GUID(IID, IID_IActionHistory,0x8D5DE85B,0x7D42,0x11D0,0xA8,0xAA,0x00,0xA0,0xC9,0x21,0xA4,0xD2);


MIDL_DEFINE_GUID(IID, LIBID_UNDOREDOLib,0x8D5DE84E,0x7D42,0x11D0,0xA8,0xAA,0x00,0xA0,0xC9,0x21,0xA4,0xD2);


MIDL_DEFINE_GUID(CLSID, CLSID_ActionHistory,0xF5E7E71F,0x1401,0x11d1,0x88,0x3B,0x00,0x00,0xF8,0x75,0x79,0xD2);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

