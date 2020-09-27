
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:11:22 2002
 */
/* Compiler settings for jprojbld.idl:
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

MIDL_DEFINE_GUID(IID, IID_IDispJProject,0xC02B123E,0xA492,0x11D0,0xBF,0xB4,0x00,0xA0,0xC9,0x1E,0xBF,0xA0);


MIDL_DEFINE_GUID(IID, IID_IDispJavaFolder,0x88C36C20,0xB10E,0x11D0,0xBF,0xBC,0x00,0xA0,0xC9,0x1E,0xBF,0xA0);


MIDL_DEFINE_GUID(IID, IID_IDispJavaFile,0xA58D5570,0xB10E,0x11D0,0xBF,0xBC,0x00,0xA0,0xC9,0x1E,0xBF,0xA0);


MIDL_DEFINE_GUID(IID, IID_IJProjectClassPathEvents,0x051BB750,0x70DB,0x11d1,0xA9,0x62,0x00,0xA0,0xC9,0x21,0xA4,0xD2);


MIDL_DEFINE_GUID(IID, IID_IJProjectDefinesEvents,0x8748B0E9,0x262A,0x11d1,0x8C,0x92,0x00,0xC0,0x4F,0xC2,0xC6,0xCC);


MIDL_DEFINE_GUID(IID, IID_IJProjectDefines,0xD26BCD91,0x2629,0x11d1,0x8C,0x92,0x00,0xC0,0x4F,0xC2,0xC6,0xCC);


MIDL_DEFINE_GUID(IID, IID_IJProjectClassPath,0x7EA77851,0xE697,0x11d0,0x8B,0xC1,0x00,0xA0,0xC9,0x0F,0x26,0xF7);


MIDL_DEFINE_GUID(IID, IID_IJavaProjects,0x60578a60,0x2fa7,0x11d1,0xbf,0xee,0x00,0xa0,0xc9,0x1e,0xbf,0xa0);


MIDL_DEFINE_GUID(IID, LIBID_JPROJBLDLib,0xC02B1231,0xA492,0x11D0,0xBF,0xB4,0x00,0xA0,0xC9,0x1E,0xBF,0xA0);


MIDL_DEFINE_GUID(CLSID, CLSID_JProjectPackage,0xC02B123F,0xA492,0x11D0,0xBF,0xB4,0x00,0xA0,0xC9,0x1E,0xBF,0xA0);


MIDL_DEFINE_GUID(CLSID, CLSID_VJLaunchSettings,0xCEC5B29E,0x5249,0x11d2,0x8C,0xFC,0x00,0xC0,0x4F,0xC2,0xC6,0xCC);


MIDL_DEFINE_GUID(CLSID, CLSID_VJCompileSettings,0xCEC5B29F,0x5249,0x11d2,0x8C,0xFC,0x00,0xC0,0x4F,0xC2,0xC6,0xCC);


MIDL_DEFINE_GUID(CLSID, CLSID_VJPrePostBuildRules,0xCEC5B2A0,0x5249,0x11d2,0x8C,0xFC,0x00,0xC0,0x4F,0xC2,0xC6,0xCC);


MIDL_DEFINE_GUID(CLSID, CLSID_VJClassPathSettings,0xCEC5B2A1,0x5249,0x11d2,0x8C,0xFC,0x00,0xC0,0x4F,0xC2,0xC6,0xCC);


MIDL_DEFINE_GUID(CLSID, CLSID_VJComClassSettings,0xCEC5B2A2,0x5249,0x11d2,0x8C,0xFC,0x00,0xC0,0x4F,0xC2,0xC6,0xCC);


MIDL_DEFINE_GUID(CLSID, CLSID_CJavaProjects,0x008386CD,0x20B2,0x11D1,0xBF,0xE5,0x00,0xA0,0xC9,0x1E,0xBF,0xA0);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

