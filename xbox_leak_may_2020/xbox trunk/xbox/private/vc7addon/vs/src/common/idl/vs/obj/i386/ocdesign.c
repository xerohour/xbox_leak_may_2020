
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:42 2002
 */
/* Compiler settings for ocdesign.idl:
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

MIDL_DEFINE_GUID(IID, IID_IBuilderWizardManager,0x95FC88C3,0x9FCB,0x11cf,0xA4,0x05,0x00,0xAA,0x00,0xC0,0x09,0x40);


MIDL_DEFINE_GUID(IID, IID_IProvidePropertyBuilder,0x33C0C1D8,0x33CF,0x11d3,0xBF,0xF2,0x00,0xC0,0x4F,0x99,0x02,0x35);


MIDL_DEFINE_GUID(IID, IID_IPerPropertyBrowsing2,0x33C0C1DA,0x33CF,0x11d3,0xBF,0xF2,0x00,0xC0,0x4F,0x99,0x02,0x35);


MIDL_DEFINE_GUID(IID, IID_IPropertyPageUndoString,0x33C0C1DB,0x33CF,0x11d3,0xBF,0xF2,0x00,0xC0,0x4F,0x99,0x02,0x35);


MIDL_DEFINE_GUID(IID, IID_IProvideRuntimeHTML,0x33C0C1DC,0x33CF,0x11d3,0xBF,0xF2,0x00,0xC0,0x4F,0x99,0x02,0x35);


MIDL_DEFINE_GUID(IID, IID_IProvideAltHTML,0x33C0C1DD,0x33CF,0x11d3,0xBF,0xF2,0x00,0xC0,0x4F,0x99,0x02,0x35);


MIDL_DEFINE_GUID(IID, LIBID_OCDesign,0x33C0C1DF,0x33CF,0x11d3,0xBF,0xF2,0x00,0xC0,0x4F,0x99,0x02,0x35);


MIDL_DEFINE_GUID(CLSID, CLSID_BuilderWizardManager,0x9FCF0840,0xF8F7,0x11d2,0xA6,0xAE,0x00,0x10,0x4B,0xCC,0x72,0x69);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

