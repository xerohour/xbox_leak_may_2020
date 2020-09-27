
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:14:01 2002
 */
/* Compiler settings for sh.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "sh.h"

#define TYPE_FORMAT_STRING_SIZE   765                               
#define PROC_FORMAT_STRING_SIZE   2923                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugSymbolProvider_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugSymbolProvider_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugComPlusSymbolProvider_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugComPlusSymbolProvider_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugNativeSymbolProvider_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugNativeSymbolProvider_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugContainerField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugContainerField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMethodField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMethodField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugClassField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugClassField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPropertyField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPropertyField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugArrayField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugArrayField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPointerField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPointerField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEnumField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEnumField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBitField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBitField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDynamicField_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDynamicField_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDynamicFieldCOMPlus_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDynamicFieldCOMPlus_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEngineSymbolProviderServices_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEngineSymbolProviderServices_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugAddress_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugAddress_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugFields_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugFields_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugAddresses_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugAddresses_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugCustomAttributeQuery_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugCustomAttributeQuery_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Initialize */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pServices */

/* 16 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Uninitialize */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x4 ),	/* 4 */
/* 36 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x8 ),	/* 8 */
/* 42 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContainerField */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x5 ),	/* 5 */
/* 58 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pAddress */

/* 66 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 68 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 70 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter ppContainerField */

/* 72 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 74 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 76 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 80 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetField */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x6 ),	/* 6 */
/* 92 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x8 ),	/* 8 */
/* 98 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pAddress */

/* 100 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 102 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 104 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter pAddressCur */

/* 106 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 108 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 110 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter ppField */

/* 112 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 114 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 116 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */

/* 118 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 120 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 122 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAddressesFromPosition */

/* 124 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 126 */	NdrFcLong( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x7 ),	/* 7 */
/* 132 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 134 */	NdrFcShort( 0x8 ),	/* 8 */
/* 136 */	NdrFcShort( 0x8 ),	/* 8 */
/* 138 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter pDocPos */

/* 140 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 142 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 144 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Parameter fStatmentOnly */

/* 146 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 148 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnumBegAddresses */

/* 152 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 154 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 156 */	NdrFcShort( 0x64 ),	/* Type Offset=100 */

	/* Parameter ppEnumEndAddresses */

/* 158 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 160 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 162 */	NdrFcShort( 0x64 ),	/* Type Offset=100 */

	/* Return value */

/* 164 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 166 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAddressesFromContext */

/* 170 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 172 */	NdrFcLong( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x8 ),	/* 8 */
/* 178 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 182 */	NdrFcShort( 0x8 ),	/* 8 */
/* 184 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter pDocContext */

/* 186 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 188 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 190 */	NdrFcShort( 0x7a ),	/* Type Offset=122 */

	/* Parameter fStatmentOnly */

/* 192 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 194 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 196 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnumBegAddresses */

/* 198 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 200 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 202 */	NdrFcShort( 0x64 ),	/* Type Offset=100 */

	/* Parameter ppEnumEndAddresses */

/* 204 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 206 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 208 */	NdrFcShort( 0x64 ),	/* Type Offset=100 */

	/* Return value */

/* 210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 212 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContextFromAddress */

/* 216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 222 */	NdrFcShort( 0x9 ),	/* 9 */
/* 224 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 230 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pAddress */

/* 232 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 234 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 236 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter ppDocContext */

/* 238 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 240 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 242 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Return value */

/* 244 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 246 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLanguage */

/* 250 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 252 */	NdrFcLong( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0xa ),	/* 10 */
/* 258 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 262 */	NdrFcShort( 0x90 ),	/* 144 */
/* 264 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pAddress */

/* 266 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 268 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 270 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter pguidLanguage */

/* 272 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 274 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 276 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter pguidLanguageVendor */

/* 278 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 280 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 282 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Return value */

/* 284 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 286 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetGlobalContainer */

/* 290 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 292 */	NdrFcLong( 0x0 ),	/* 0 */
/* 296 */	NdrFcShort( 0xb ),	/* 11 */
/* 298 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0x8 ),	/* 8 */
/* 304 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pField */

/* 306 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 308 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 310 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */

/* 312 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 314 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMethodFieldsByName */

/* 318 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 320 */	NdrFcLong( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0xc ),	/* 12 */
/* 326 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 328 */	NdrFcShort( 0x6 ),	/* 6 */
/* 330 */	NdrFcShort( 0x8 ),	/* 8 */
/* 332 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pszFullName */

/* 334 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 336 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 338 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Parameter nameMatch */

/* 340 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 342 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 344 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 346 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 348 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 350 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 352 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 354 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 356 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClassTypeByName */

/* 358 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0xd ),	/* 13 */
/* 366 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 368 */	NdrFcShort( 0x6 ),	/* 6 */
/* 370 */	NdrFcShort( 0x8 ),	/* 8 */
/* 372 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pszClassName */

/* 374 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 376 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 378 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Parameter nameMatch */

/* 380 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 382 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 384 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter ppField */

/* 386 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 388 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 390 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 392 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 394 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 396 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNamespacesUsedAtAddress */

/* 398 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 400 */	NdrFcLong( 0x0 ),	/* 0 */
/* 404 */	NdrFcShort( 0xe ),	/* 14 */
/* 406 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 410 */	NdrFcShort( 0x8 ),	/* 8 */
/* 412 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pAddress */

/* 414 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 416 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 418 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter ppEnum */

/* 420 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 422 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 424 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 426 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 428 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeByName */

/* 432 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 434 */	NdrFcLong( 0x0 ),	/* 0 */
/* 438 */	NdrFcShort( 0xf ),	/* 15 */
/* 440 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 442 */	NdrFcShort( 0x6 ),	/* 6 */
/* 444 */	NdrFcShort( 0x8 ),	/* 8 */
/* 446 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pszClassName */

/* 448 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 450 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 452 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Parameter nameMatch */

/* 454 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 456 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 458 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter ppField */

/* 460 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 462 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 464 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */

/* 466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 468 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNextAddress */

/* 472 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x10 ),	/* 16 */
/* 480 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 482 */	NdrFcShort( 0x8 ),	/* 8 */
/* 484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 486 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pAddress */

/* 488 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 490 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 492 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter fStatmentOnly */

/* 494 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 496 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppAddress */

/* 500 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 502 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 504 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Return value */

/* 506 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 508 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadSymbols */

/* 512 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 514 */	NdrFcLong( 0x0 ),	/* 0 */
/* 518 */	NdrFcShort( 0x11 ),	/* 17 */
/* 520 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 522 */	NdrFcShort( 0x48 ),	/* 72 */
/* 524 */	NdrFcShort( 0x8 ),	/* 8 */
/* 526 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter ulAppDomainID */

/* 528 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 530 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 534 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 536 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 538 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter baseAddress */

/* 540 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 542 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 544 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pUnkMetadataImport */

/* 546 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 548 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 550 */	NdrFcShort( 0xda ),	/* Type Offset=218 */

	/* Parameter bstrModuleName */

/* 552 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 554 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 556 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */

	/* Parameter bstrSymSearchPath */

/* 558 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 560 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 562 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */

	/* Return value */

/* 564 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 566 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnloadSymbols */

/* 570 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 576 */	NdrFcShort( 0x12 ),	/* 18 */
/* 578 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 580 */	NdrFcShort( 0x38 ),	/* 56 */
/* 582 */	NdrFcShort( 0x8 ),	/* 8 */
/* 584 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter ulAppDomainID */

/* 586 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 588 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 592 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 594 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 596 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Return value */

/* 598 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 600 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 602 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEntryPoint */

/* 604 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 606 */	NdrFcLong( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x13 ),	/* 19 */
/* 612 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 614 */	NdrFcShort( 0x38 ),	/* 56 */
/* 616 */	NdrFcShort( 0x8 ),	/* 8 */
/* 618 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter ulAppDomainID */

/* 620 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 622 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 626 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 628 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 630 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter ppAddress */

/* 632 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 634 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 636 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Return value */

/* 638 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 640 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeFromAddress */

/* 644 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 646 */	NdrFcLong( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x14 ),	/* 20 */
/* 652 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 656 */	NdrFcShort( 0x8 ),	/* 8 */
/* 658 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pAddress */

/* 660 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 662 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 664 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter ppField */

/* 666 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 668 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 670 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */

/* 672 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 674 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UpdateSymbols */

/* 678 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 680 */	NdrFcLong( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0x15 ),	/* 21 */
/* 686 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 688 */	NdrFcShort( 0x38 ),	/* 56 */
/* 690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 692 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter ulAppDomainID */

/* 694 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 696 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 698 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 700 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 702 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 704 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter pUpdateStream */

/* 706 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 708 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 710 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */

/* 712 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 714 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateTypeFromPrimitive */

/* 718 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 720 */	NdrFcLong( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0x16 ),	/* 22 */
/* 726 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 732 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter dwPrimType */

/* 734 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 736 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAddress */

/* 740 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 742 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 744 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter ppType */

/* 746 */	NdrFcShort( 0x200b ),	/* Flags:  must size, must free, in, srv alloc size=8 */
/* 748 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 750 */	NdrFcShort( 0x120 ),	/* Type Offset=288 */

	/* Return value */

/* 752 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 754 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 756 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFunctionLineOffset */

/* 758 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 760 */	NdrFcLong( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0x17 ),	/* 23 */
/* 766 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 768 */	NdrFcShort( 0x8 ),	/* 8 */
/* 770 */	NdrFcShort( 0x8 ),	/* 8 */
/* 772 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pAddress */

/* 774 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 776 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 778 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter dwLine */

/* 780 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 782 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 784 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppNewAddress */

/* 786 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 788 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 790 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Return value */

/* 792 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 794 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAddressesInModuleFromPosition */

/* 798 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 800 */	NdrFcLong( 0x0 ),	/* 0 */
/* 804 */	NdrFcShort( 0x18 ),	/* 24 */
/* 806 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 808 */	NdrFcShort( 0x40 ),	/* 64 */
/* 810 */	NdrFcShort( 0x8 ),	/* 8 */
/* 812 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter ulAppDomainID */

/* 814 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 816 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 818 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 820 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 822 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 824 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter pDocPos */

/* 826 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 828 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 830 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Parameter fStatmentOnly */

/* 832 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 834 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 836 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnumBegAddresses */

/* 838 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 840 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 842 */	NdrFcShort( 0x64 ),	/* Type Offset=100 */

	/* Parameter ppEnumEndAddresses */

/* 844 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 846 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 848 */	NdrFcShort( 0x64 ),	/* Type Offset=100 */

	/* Return value */

/* 850 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 852 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetArrayTypeFromAddress */

/* 856 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 858 */	NdrFcLong( 0x0 ),	/* 0 */
/* 862 */	NdrFcShort( 0x19 ),	/* 25 */
/* 864 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 870 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter pAddress */

/* 872 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 874 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 876 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter pSig */

/* 878 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 880 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 882 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Parameter dwSigLength */

/* 884 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 886 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppField */

/* 890 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 892 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 894 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */

/* 896 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 898 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSymAttribute */

/* 902 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 904 */	NdrFcLong( 0x0 ),	/* 0 */
/* 908 */	NdrFcShort( 0x1a ),	/* 26 */
/* 910 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 912 */	NdrFcShort( 0x48 ),	/* 72 */
/* 914 */	NdrFcShort( 0x24 ),	/* 36 */
/* 916 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x8,		/* 8 */

	/* Parameter ulAppDomainID */

/* 918 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 924 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 926 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 928 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter tokParent */

/* 930 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 932 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pstrName */

/* 936 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 938 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 940 */	NdrFcShort( 0x138 ),	/* Type Offset=312 */

	/* Parameter cBuffer */

/* 942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 944 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcBuffer */

/* 948 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 950 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 952 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 954 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 956 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 958 */	NdrFcShort( 0x13e ),	/* Type Offset=318 */

	/* Return value */

/* 960 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 962 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReplaceSymbols */

/* 966 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 968 */	NdrFcLong( 0x0 ),	/* 0 */
/* 972 */	NdrFcShort( 0x1b ),	/* 27 */
/* 974 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 976 */	NdrFcShort( 0x38 ),	/* 56 */
/* 978 */	NdrFcShort( 0x8 ),	/* 8 */
/* 980 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter ulAppDomainID */

/* 982 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 984 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 986 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 988 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 990 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 992 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter pStream */

/* 994 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 996 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 998 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */

/* 1000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1002 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AreSymbolsLoaded */

/* 1006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1012 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1014 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1016 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1018 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1020 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter ulAppDomainID */

/* 1022 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1024 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1026 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 1028 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1030 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1032 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Return value */

/* 1034 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1036 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1038 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadSymbolsFromStream */

/* 1040 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1042 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x1d ),	/* 29 */
/* 1048 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 1050 */	NdrFcShort( 0x48 ),	/* 72 */
/* 1052 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1054 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter ulAppDomainID */

/* 1056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1058 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1060 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 1062 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1064 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1066 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter baseAddress */

/* 1068 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1070 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1072 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pUnkMetadataImport */

/* 1074 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1076 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1078 */	NdrFcShort( 0xda ),	/* Type Offset=218 */

	/* Parameter pStream */

/* 1080 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1082 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1084 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */

/* 1086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1088 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSymUnmanagedReader */

/* 1092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1098 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1100 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1102 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1106 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter ulAppDomainID */

/* 1108 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 1114 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1118 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter ppSymUnmanagedReader */

/* 1120 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1122 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1124 */	NdrFcShort( 0x14c ),	/* Type Offset=332 */

	/* Return value */

/* 1126 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1128 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAttributedClassesinModule */

/* 1132 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1134 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1138 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1140 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1142 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1146 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter ulAppDomainID */

/* 1148 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1150 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1152 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 1154 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1156 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1158 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter pstrAttribute */

/* 1160 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1162 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1164 */	NdrFcShort( 0x138 ),	/* Type Offset=312 */

	/* Parameter ppEnum */

/* 1166 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1168 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1170 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 1172 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1174 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1176 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAttributedClassesForLanguage */

/* 1178 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1180 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1184 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1186 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1188 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1192 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter guidLanguage */

/* 1194 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1196 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1198 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter pstrAttribute */

/* 1200 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1202 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1204 */	NdrFcShort( 0x138 ),	/* Type Offset=312 */

	/* Parameter ppEnum */

/* 1206 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1208 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1210 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 1212 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1214 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1216 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsHiddenCode */

/* 1218 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1220 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1224 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1226 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1232 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pAddress */

/* 1234 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1236 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1238 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Return value */

/* 1240 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1242 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsFunctionDeleted */

/* 1246 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1248 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1252 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1254 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1260 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pAddress */

/* 1262 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1264 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1266 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Return value */

/* 1268 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1270 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNameFromToken */

/* 1274 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1276 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1280 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1282 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1286 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1288 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pMetadataImport */

/* 1290 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1292 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1294 */	NdrFcShort( 0xda ),	/* Type Offset=218 */

	/* Parameter dwToken */

/* 1296 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1298 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrName */

/* 1302 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1304 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1306 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Return value */

/* 1308 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1310 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsFunctionStale */

/* 1314 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1316 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1320 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1322 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1326 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1328 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pAddress */

/* 1330 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1332 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1334 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Return value */

/* 1336 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1338 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLocalVariablelayout */

/* 1342 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1348 */	NdrFcShort( 0x25 ),	/* 37 */
/* 1350 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1352 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1354 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1356 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter ulAppDomainID */

/* 1358 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1360 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1362 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 1364 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1366 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1368 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter cMethods */

/* 1370 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1372 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgMethodTokens */

/* 1376 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1378 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1380 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter pStreamLayout */

/* 1382 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1384 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1386 */	NdrFcShort( 0x16c ),	/* Type Offset=364 */

	/* Return value */

/* 1388 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1390 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1392 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAssemblyName */

/* 1394 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1396 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1400 */	NdrFcShort( 0x26 ),	/* 38 */
/* 1402 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1404 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1406 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1408 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter ulAppDomainID */

/* 1410 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1412 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 1416 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1418 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1420 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter pbstrName */

/* 1422 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1424 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1426 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Return value */

/* 1428 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1430 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadSymbols */

/* 1434 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1436 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1440 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1442 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1446 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1448 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszFileName */

/* 1450 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1452 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1454 */	NdrFcShort( 0x138 ),	/* Type Offset=312 */

	/* Return value */

/* 1456 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1458 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 1462 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1464 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1468 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1470 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1472 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1474 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1476 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwFields */

/* 1478 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1480 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1482 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFieldInfo */

/* 1484 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 1486 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1488 */	NdrFcShort( 0x174 ),	/* Type Offset=372 */

	/* Return value */

/* 1490 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1492 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1494 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetKind */

/* 1496 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1498 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1502 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1504 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1508 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1510 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwKind */

/* 1512 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1514 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1516 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1518 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1520 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1522 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetType */

/* 1524 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1526 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1530 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1532 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1536 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1538 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppType */

/* 1540 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1542 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1544 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */

/* 1546 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1548 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1550 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContainer */

/* 1552 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1554 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1558 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1560 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1564 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1566 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppContainerField */

/* 1568 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1570 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1572 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */

/* 1574 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1576 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1578 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAddress */

/* 1580 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1582 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1586 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1588 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1592 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1594 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppAddress */

/* 1596 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1598 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1600 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Return value */

/* 1602 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1604 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1606 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSize */

/* 1608 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1610 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1614 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1616 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1620 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1622 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwSize */

/* 1624 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1626 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1630 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1632 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1634 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExtendedInfo */

/* 1636 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1638 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1642 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1644 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1646 */	NdrFcShort( 0x60 ),	/* 96 */
/* 1648 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1650 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter guidExtendedInfo */

/* 1652 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1654 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1656 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter prgBuffer */

/* 1658 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1660 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1662 */	NdrFcShort( 0x194 ),	/* Type Offset=404 */

	/* Parameter pdwLen */

/* 1664 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1666 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1668 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1672 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Equal */

/* 1676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1682 */	NdrFcShort( 0xa ),	/* 10 */
/* 1684 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1690 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pField */

/* 1692 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1694 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1696 */	NdrFcShort( 0x40 ),	/* Type Offset=64 */

	/* Return value */

/* 1698 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfo */

/* 1704 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1706 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1710 */	NdrFcShort( 0xb ),	/* 11 */
/* 1712 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1716 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1718 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pTypeInfo */

/* 1720 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 1722 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1724 */	NdrFcShort( 0x1be ),	/* Type Offset=446 */

	/* Return value */

/* 1726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1728 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumFields */

/* 1732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1738 */	NdrFcShort( 0xc ),	/* 12 */
/* 1740 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1742 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1746 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter dwKindFilter */

/* 1748 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1750 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwModifiersFilter */

/* 1754 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1756 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pszNameFilter */

/* 1760 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1762 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1764 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Parameter nameMatch */

/* 1766 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1768 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1770 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 1772 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1774 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1776 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 1778 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1780 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumBaseClasses */


	/* Procedure EnumParameters */

/* 1784 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1786 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1790 */	NdrFcShort( 0xd ),	/* 13 */
/* 1792 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1796 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1798 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */


	/* Parameter ppParams */

/* 1800 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1802 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1804 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */


	/* Return value */

/* 1806 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1808 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetThis */

/* 1812 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1814 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1818 */	NdrFcShort( 0xe ),	/* 14 */
/* 1820 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1824 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1826 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppClass */

/* 1828 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1830 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1832 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 1834 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1836 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1838 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumAllLocals */

/* 1840 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1842 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1846 */	NdrFcShort( 0xf ),	/* 15 */
/* 1848 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1852 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1854 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pAddress */

/* 1856 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1858 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1860 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter ppLocals */

/* 1862 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1864 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1866 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 1868 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1870 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1872 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumLocals */

/* 1874 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1876 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1880 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1882 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1886 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1888 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pAddress */

/* 1890 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1892 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1894 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter ppLocals */

/* 1896 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1898 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1900 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 1902 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1904 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsCustomAttributeDefined */

/* 1908 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1910 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1914 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1916 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1922 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszCustomAttributeName */

/* 1924 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1926 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1928 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Return value */

/* 1930 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1932 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumStaticLocals */

/* 1936 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1938 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1942 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1944 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1950 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppLocals */

/* 1952 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1954 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1956 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 1958 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1960 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1962 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetGlobalContainer */

/* 1964 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1966 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1970 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1972 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1976 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1978 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppClass */

/* 1980 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1982 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1984 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 1986 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1988 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1990 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumNestedEnums */


	/* Procedure EnumArguments */

/* 1992 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1994 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1998 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2000 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2004 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2006 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */


	/* Parameter ppParams */

/* 2008 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2010 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2012 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */


	/* Return value */

/* 2014 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2016 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DoesInterfaceExist */

/* 2020 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 2022 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2026 */	NdrFcShort( 0xe ),	/* 14 */
/* 2028 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2032 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2034 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszInterfaceName */

/* 2036 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2038 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2040 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Return value */

/* 2042 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2044 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2046 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumNestedClasses */

/* 2048 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2050 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2054 */	NdrFcShort( 0xf ),	/* 15 */
/* 2056 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2058 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2060 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2062 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 2064 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2066 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2068 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 2070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2072 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEnclosingClass */

/* 2076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2082 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2084 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2088 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2090 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppClassField */

/* 2092 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2094 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2096 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 2098 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2100 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2102 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumInterfacesImplemented */

/* 2104 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2106 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2110 */	NdrFcShort( 0x11 ),	/* 17 */
/* 2112 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2116 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2118 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 2120 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2122 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2124 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 2126 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2128 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumConstructors */

/* 2132 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2134 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2138 */	NdrFcShort( 0x12 ),	/* 18 */
/* 2140 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2142 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2146 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter cMatch */

/* 2148 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2150 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2152 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 2154 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2156 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2158 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 2160 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2162 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2164 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDefaultIndexer */

/* 2166 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2168 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2172 */	NdrFcShort( 0x13 ),	/* 19 */
/* 2174 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2180 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pbstrIndexer */

/* 2182 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2184 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2186 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Return value */

/* 2188 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2190 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPropertyGetter */

/* 2194 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2196 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2200 */	NdrFcShort( 0xd ),	/* 13 */
/* 2202 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2206 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2208 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppField */

/* 2210 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2212 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2214 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Return value */

/* 2216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2218 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPropertySetter */

/* 2222 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2228 */	NdrFcShort( 0xe ),	/* 14 */
/* 2230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2236 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppField */

/* 2238 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2240 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2242 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Return value */

/* 2244 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2246 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNumberOfElements */

/* 2250 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2252 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2256 */	NdrFcShort( 0xd ),	/* 13 */
/* 2258 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2262 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2264 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwNumElements */

/* 2266 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2268 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2272 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2274 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetElementType */

/* 2278 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2280 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2284 */	NdrFcShort( 0xe ),	/* 14 */
/* 2286 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2290 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2292 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppType */

/* 2294 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2296 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2298 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */

/* 2300 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2302 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRank */

/* 2306 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2308 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2312 */	NdrFcShort( 0xf ),	/* 15 */
/* 2314 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2318 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2320 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwRank */

/* 2322 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2324 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2328 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2330 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetUnderlyingSymbol */


	/* Procedure GetDereferencedField */

/* 2334 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2336 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2340 */	NdrFcShort( 0xd ),	/* 13 */
/* 2342 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2346 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2348 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppField */


	/* Parameter ppField */

/* 2350 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2352 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2354 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */


	/* Return value */

/* 2356 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2358 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2360 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStringFromValue */

/* 2362 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2364 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2368 */	NdrFcShort( 0xe ),	/* 14 */
/* 2370 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2372 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2374 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2376 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter value */

/* 2378 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2380 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2382 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pbstrValue */

/* 2384 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2386 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2388 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Return value */

/* 2390 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2392 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValueFromString */

/* 2396 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 2398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2402 */	NdrFcShort( 0xf ),	/* 15 */
/* 2404 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2408 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2410 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pszValue */

/* 2412 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2414 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2416 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Parameter pvalue */

/* 2418 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2420 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2422 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 2424 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2426 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValueFromStringCaseInsensitive */

/* 2430 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 2432 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2436 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2438 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2442 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2444 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pszValue */

/* 2446 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2448 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2450 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Parameter pvalue */

/* 2452 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2454 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2456 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 2458 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2460 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2462 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStart */

/* 2464 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2466 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2470 */	NdrFcShort( 0xc ),	/* 12 */
/* 2472 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2476 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2478 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwBitOffset */

/* 2480 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2482 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2486 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2488 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeFromPrimitive */

/* 2492 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2494 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2498 */	NdrFcShort( 0xc ),	/* 12 */
/* 2500 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2502 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2506 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwCorElementType */

/* 2508 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2510 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2512 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppType */

/* 2514 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2516 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2518 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */

/* 2520 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2522 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2524 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeFromTypeDef */

/* 2526 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2528 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2532 */	NdrFcShort( 0xd ),	/* 13 */
/* 2534 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 2536 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2538 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2540 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter ulAppDomainID */

/* 2542 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2544 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidModule */

/* 2548 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2550 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2552 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter tokClass */

/* 2554 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2556 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppType */

/* 2560 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2562 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2564 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Return value */

/* 2566 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2568 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumCodeContexts */

/* 2572 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2574 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2578 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2580 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2582 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2584 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2586 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter rgpAddresses */

/* 2588 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2590 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2592 */	NdrFcShort( 0x1fc ),	/* Type Offset=508 */

	/* Parameter celtAddresses */

/* 2594 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2596 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 2600 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2602 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2604 */	NdrFcShort( 0x20e ),	/* Type Offset=526 */

	/* Return value */

/* 2606 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2608 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAddress */

/* 2612 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2614 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2618 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2620 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2624 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2626 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pAddress */

/* 2628 */	NdrFcShort( 0xe113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=56 */
/* 2630 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2632 */	NdrFcShort( 0x2aa ),	/* Type Offset=682 */

	/* Return value */

/* 2634 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2636 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2638 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 2640 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2642 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2646 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2648 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2650 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2652 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2654 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 2656 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2658 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 2662 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2664 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2666 */	NdrFcShort( 0x2c2 ),	/* Type Offset=706 */

	/* Parameter pceltFetched */

/* 2668 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 2670 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2674 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2676 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2678 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure Skip */

/* 2680 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2682 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2686 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2688 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2692 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2694 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter celt */


	/* Parameter celt */

/* 2696 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2698 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2700 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2702 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2704 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2706 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure Reset */

/* 2708 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2710 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2714 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2716 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2720 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2722 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */

/* 2724 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2726 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2728 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 2730 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2732 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2736 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2738 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2742 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2744 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 2746 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2748 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2750 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 2752 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2754 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2756 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCount */


	/* Procedure GetCount */

/* 2758 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2760 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2764 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2766 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2768 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2770 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2772 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pcelt */


	/* Parameter pcelt */

/* 2774 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2776 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2778 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2780 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2782 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2784 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 2786 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2788 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2792 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2794 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2796 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2798 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2800 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 2802 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2804 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2806 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 2808 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2810 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2812 */	NdrFcShort( 0x2d8 ),	/* Type Offset=728 */

	/* Parameter pceltFetched */

/* 2814 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 2816 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2818 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2820 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2822 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2824 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 2826 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2828 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2832 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2834 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2838 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2840 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 2842 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2844 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2846 */	NdrFcShort( 0x64 ),	/* Type Offset=100 */

	/* Return value */

/* 2848 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2850 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2852 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsCustomAttributeDefined */

/* 2854 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 2856 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2860 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2862 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2864 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2868 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszCustomAttributeName */

/* 2870 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2872 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2874 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Return value */

/* 2876 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2878 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2880 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCustomAttributeByName */

/* 2882 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 2884 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2888 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2890 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2892 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2894 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2896 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pszCustomAttributeName */

/* 2898 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2900 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2902 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Parameter ppBlob */

/* 2904 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 2906 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2908 */	NdrFcShort( 0x2ee ),	/* Type Offset=750 */

	/* Parameter pdwLen */

/* 2910 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 2912 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2914 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2916 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2918 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2920 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  4 */	NdrFcLong( 0x83919262 ),	/* -2087611806 */
/*  8 */	NdrFcShort( 0xacd6 ),	/* -21290 */
/* 10 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 12 */	0x90,		/* 144 */
			0x28,		/* 40 */
/* 14 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 16 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 18 */	0x2,		/* 2 */
			0xa1,		/* 161 */
/* 20 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 22 */	NdrFcLong( 0xc2e34ebb ),	/* -1025290565 */
/* 26 */	NdrFcShort( 0x8b9d ),	/* -29795 */
/* 28 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 30 */	0x90,		/* 144 */
			0x14,		/* 20 */
/* 32 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 34 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 36 */	0x83,		/* 131 */
			0x38,		/* 56 */
/* 38 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 40 */	NdrFcShort( 0x2 ),	/* Offset= 2 (42) */
/* 42 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 44 */	NdrFcLong( 0xc2e34eb2 ),	/* -1025290574 */
/* 48 */	NdrFcShort( 0x8b9d ),	/* -29795 */
/* 50 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 52 */	0x90,		/* 144 */
			0x14,		/* 20 */
/* 54 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 56 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 58 */	0x83,		/* 131 */
			0x38,		/* 56 */
/* 60 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 62 */	NdrFcShort( 0x2 ),	/* Offset= 2 (64) */
/* 64 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 66 */	NdrFcLong( 0xc2e34eb1 ),	/* -1025290575 */
/* 70 */	NdrFcShort( 0x8b9d ),	/* -29795 */
/* 72 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 74 */	0x90,		/* 144 */
			0x14,		/* 20 */
/* 76 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 78 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 80 */	0x83,		/* 131 */
			0x38,		/* 56 */
/* 82 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 84 */	NdrFcLong( 0xbdde0eee ),	/* -1109520658 */
/* 88 */	NdrFcShort( 0x3b8d ),	/* 15245 */
/* 90 */	NdrFcShort( 0x4c82 ),	/* 19586 */
/* 92 */	0xb5,		/* 181 */
			0x29,		/* 41 */
/* 94 */	0x33,		/* 51 */
			0xf1,		/* 241 */
/* 96 */	0x6b,		/* 107 */
			0x42,		/* 66 */
/* 98 */	0x83,		/* 131 */
			0x2e,		/* 46 */
/* 100 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 102 */	NdrFcShort( 0x2 ),	/* Offset= 2 (104) */
/* 104 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 106 */	NdrFcLong( 0xc2e34ebd ),	/* -1025290563 */
/* 110 */	NdrFcShort( 0x8b9d ),	/* -29795 */
/* 112 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 114 */	0x90,		/* 144 */
			0x14,		/* 20 */
/* 116 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 118 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 120 */	0x83,		/* 131 */
			0x38,		/* 56 */
/* 122 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 124 */	NdrFcLong( 0x931516ad ),	/* -1827334483 */
/* 128 */	NdrFcShort( 0xb600 ),	/* -18944 */
/* 130 */	NdrFcShort( 0x419c ),	/* 16796 */
/* 132 */	0x88,		/* 136 */
			0xfc,		/* 252 */
/* 134 */	0xdc,		/* 220 */
			0xf5,		/* 245 */
/* 136 */	0x18,		/* 24 */
			0x3b,		/* 59 */
/* 138 */	0x5f,		/* 95 */
			0xa9,		/* 169 */
/* 140 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 142 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (122) */
/* 144 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 146 */	NdrFcShort( 0x8 ),	/* Offset= 8 (154) */
/* 148 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 152 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 154 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 156 */	NdrFcShort( 0x10 ),	/* 16 */
/* 158 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 160 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 162 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (148) */
			0x5b,		/* FC_END */
/* 166 */	
			0x14, 0x8,	/* FC_FP [simple_pointer] */
/* 168 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 170 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 172 */	NdrFcShort( 0x2 ),	/* Offset= 2 (174) */
/* 174 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 176 */	NdrFcLong( 0xc2e34ebc ),	/* -1025290564 */
/* 180 */	NdrFcShort( 0x8b9d ),	/* -29795 */
/* 182 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 184 */	0x90,		/* 144 */
			0x14,		/* 20 */
/* 186 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 188 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 190 */	0x83,		/* 131 */
			0x38,		/* 56 */
/* 192 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 194 */	NdrFcShort( 0x2 ),	/* Offset= 2 (196) */
/* 196 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 198 */	NdrFcLong( 0xc2e34eb5 ),	/* -1025290571 */
/* 202 */	NdrFcShort( 0x8b9d ),	/* -29795 */
/* 204 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 206 */	0x90,		/* 144 */
			0x14,		/* 20 */
/* 208 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 210 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 212 */	0x83,		/* 131 */
			0x38,		/* 56 */
/* 214 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 216 */	NdrFcShort( 0xffffff3c ),	/* Offset= -196 (20) */
/* 218 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 220 */	NdrFcLong( 0x0 ),	/* 0 */
/* 224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 228 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 230 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 232 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 234 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 236 */	
			0x12, 0x0,	/* FC_UP */
/* 238 */	NdrFcShort( 0xc ),	/* Offset= 12 (250) */
/* 240 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 242 */	NdrFcShort( 0x2 ),	/* 2 */
/* 244 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 246 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 248 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 250 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 254 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (240) */
/* 256 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 258 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 260 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x4 ),	/* 4 */
/* 266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 268 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (236) */
/* 270 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 272 */	NdrFcLong( 0xc ),	/* 12 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 282 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 284 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 286 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 288 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 290 */	NdrFcShort( 0xffffff1e ),	/* Offset= -226 (64) */
/* 292 */	
			0x11, 0x0,	/* FC_RP */
/* 294 */	NdrFcShort( 0x2 ),	/* Offset= 2 (296) */
/* 296 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 298 */	NdrFcShort( 0x1 ),	/* 1 */
/* 300 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 302 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 304 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 306 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 308 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 310 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 312 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 314 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 316 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 318 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 320 */	NdrFcShort( 0x1 ),	/* 1 */
/* 322 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 324 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 326 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 328 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 330 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 332 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 334 */	NdrFcShort( 0xffffff8c ),	/* Offset= -116 (218) */
/* 336 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 338 */	NdrFcShort( 0x6 ),	/* Offset= 6 (344) */
/* 340 */	
			0x13, 0x0,	/* FC_OP */
/* 342 */	NdrFcShort( 0xffffffa4 ),	/* Offset= -92 (250) */
/* 344 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x4 ),	/* 4 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (340) */
/* 354 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 356 */	NdrFcShort( 0x4 ),	/* 4 */
/* 358 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 360 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 362 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 364 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 366 */	NdrFcShort( 0xffffffa0 ),	/* Offset= -96 (270) */
/* 368 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 370 */	NdrFcShort( 0x2 ),	/* Offset= 2 (372) */
/* 372 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 374 */	NdrFcShort( 0x14 ),	/* 20 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x0 ),	/* Offset= 0 (378) */
/* 380 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 382 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd9 ),	/* Offset= -39 (344) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 386 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd5 ),	/* Offset= -43 (344) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 390 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd1 ),	/* Offset= -47 (344) */
			0x8,		/* FC_LONG */
/* 394 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 396 */	
			0x11, 0x0,	/* FC_RP */
/* 398 */	NdrFcShort( 0xffffff0c ),	/* Offset= -244 (154) */
/* 400 */	
			0x11, 0x0,	/* FC_RP */
/* 402 */	NdrFcShort( 0x2 ),	/* Offset= 2 (404) */
/* 404 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 406 */	NdrFcShort( 0x4 ),	/* 4 */
/* 408 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 410 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 412 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 414 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 416 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 418 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 420 */	NdrFcShort( 0x4 ),	/* 4 */
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 424 */	NdrFcShort( 0x1 ),	/* 1 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 430 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 432 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 434 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 436 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 438 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 440 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 442 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 444 */	NdrFcShort( 0x2 ),	/* Offset= 2 (446) */
/* 446 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 448 */	NdrFcShort( 0x18 ),	/* 24 */
/* 450 */	NdrFcShort( 0x2 ),	/* 2 */
/* 452 */	NdrFcLong( 0x1 ),	/* 1 */
/* 456 */	NdrFcShort( 0xa ),	/* Offset= 10 (466) */
/* 458 */	NdrFcLong( 0x2 ),	/* 2 */
/* 462 */	NdrFcShort( 0x4 ),	/* Offset= 4 (466) */
/* 464 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 466 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 468 */	NdrFcShort( 0x18 ),	/* 24 */
/* 470 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 472 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffec1 ),	/* Offset= -319 (154) */
			0x8,		/* FC_LONG */
/* 476 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 478 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 480 */	NdrFcShort( 0x2 ),	/* Offset= 2 (482) */
/* 482 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 484 */	NdrFcLong( 0xc2e34eb4 ),	/* -1025290572 */
/* 488 */	NdrFcShort( 0x8b9d ),	/* -29795 */
/* 490 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 492 */	0x90,		/* 144 */
			0x14,		/* 20 */
/* 494 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 496 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 498 */	0x83,		/* 131 */
			0x38,		/* 56 */
/* 500 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 502 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 504 */	
			0x11, 0x0,	/* FC_RP */
/* 506 */	NdrFcShort( 0x2 ),	/* Offset= 2 (508) */
/* 508 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 514 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 516 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 518 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 520 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 522 */	NdrFcShort( 0xfffffe0a ),	/* Offset= -502 (20) */
/* 524 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 526 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 528 */	NdrFcShort( 0x2 ),	/* Offset= 2 (530) */
/* 530 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 532 */	NdrFcLong( 0xad47a80b ),	/* -1387812853 */
/* 536 */	NdrFcShort( 0xeda7 ),	/* -4697 */
/* 538 */	NdrFcShort( 0x459e ),	/* 17822 */
/* 540 */	0xaf,		/* 175 */
			0x82,		/* 130 */
/* 542 */	0x64,		/* 100 */
			0x7c,		/* 124 */
/* 544 */	0xc9,		/* 201 */
			0xfb,		/* 251 */
/* 546 */	0xaa,		/* 170 */
			0x50,		/* 80 */
/* 548 */	0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 550 */	NdrFcShort( 0x84 ),	/* Offset= 132 (682) */
/* 552 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 554 */	NdrFcShort( 0x18 ),	/* 24 */
/* 556 */	NdrFcShort( 0x9 ),	/* 9 */
/* 558 */	NdrFcLong( 0x1 ),	/* 1 */
/* 562 */	NdrFcShort( 0x34 ),	/* Offset= 52 (614) */
/* 564 */	NdrFcLong( 0x2 ),	/* 2 */
/* 568 */	NdrFcShort( 0x34 ),	/* Offset= 52 (620) */
/* 570 */	NdrFcLong( 0x5 ),	/* 5 */
/* 574 */	NdrFcShort( 0x36 ),	/* Offset= 54 (628) */
/* 576 */	NdrFcLong( 0x10 ),	/* 16 */
/* 580 */	NdrFcShort( 0x28 ),	/* Offset= 40 (620) */
/* 582 */	NdrFcLong( 0x11 ),	/* 17 */
/* 586 */	NdrFcShort( 0x1c ),	/* Offset= 28 (614) */
/* 588 */	NdrFcLong( 0x12 ),	/* 18 */
/* 592 */	NdrFcShort( 0x2a ),	/* Offset= 42 (634) */
/* 594 */	NdrFcLong( 0x13 ),	/* 19 */
/* 598 */	NdrFcShort( 0x16 ),	/* Offset= 22 (620) */
/* 600 */	NdrFcLong( 0x14 ),	/* 20 */
/* 604 */	NdrFcShort( 0x2e ),	/* Offset= 46 (650) */
/* 606 */	NdrFcLong( 0x15 ),	/* 21 */
/* 610 */	NdrFcShort( 0x36 ),	/* Offset= 54 (664) */
/* 612 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 614 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 616 */	NdrFcShort( 0x4 ),	/* 4 */
/* 618 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 620 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 622 */	NdrFcShort( 0xc ),	/* 12 */
/* 624 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 626 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 628 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 630 */	NdrFcShort( 0x8 ),	/* 8 */
/* 632 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 634 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 636 */	NdrFcShort( 0xc ),	/* 12 */
/* 638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 640 */	NdrFcShort( 0xa ),	/* Offset= 10 (650) */
/* 642 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 644 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffe55 ),	/* Offset= -427 (218) */
			0x8,		/* FC_LONG */
/* 648 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 650 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 652 */	NdrFcShort( 0x8 ),	/* 8 */
/* 654 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 656 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 658 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 660 */	NdrFcShort( 0xa ),	/* 10 */
/* 662 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 664 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 666 */	NdrFcShort( 0x18 ),	/* 24 */
/* 668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 670 */	NdrFcShort( 0x0 ),	/* Offset= 0 (670) */
/* 672 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 674 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 676 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffed ),	/* Offset= -19 (658) */
			0x3e,		/* FC_STRUCTPAD2 */
/* 680 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 682 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 684 */	NdrFcShort( 0x38 ),	/* 56 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 688 */	NdrFcShort( 0x0 ),	/* Offset= 0 (688) */
/* 690 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 692 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffde5 ),	/* Offset= -539 (154) */
			0x8,		/* FC_LONG */
/* 696 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 698 */	NdrFcShort( 0xffffff6e ),	/* Offset= -146 (552) */
/* 700 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 702 */	
			0x11, 0x0,	/* FC_RP */
/* 704 */	NdrFcShort( 0x2 ),	/* Offset= 2 (706) */
/* 706 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 708 */	NdrFcShort( 0x0 ),	/* 0 */
/* 710 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 712 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 714 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 716 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 718 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 720 */	NdrFcShort( 0xfffffd70 ),	/* Offset= -656 (64) */
/* 722 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 724 */	
			0x11, 0x0,	/* FC_RP */
/* 726 */	NdrFcShort( 0x2 ),	/* Offset= 2 (728) */
/* 728 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 732 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 734 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 736 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 738 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 740 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 742 */	NdrFcShort( 0xfffffd2e ),	/* Offset= -722 (20) */
/* 744 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 746 */	
			0x11, 0x0,	/* FC_RP */
/* 748 */	NdrFcShort( 0x2 ),	/* Offset= 2 (750) */
/* 750 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 752 */	NdrFcShort( 0x1 ),	/* 1 */
/* 754 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 756 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 758 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 760 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 762 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Standard interface: __MIDL_itf_sh_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDebugSymbolProvider, ver. 0.0,
   GUID={0xc2e34eae,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugSymbolProvider_FormatStringOffsetTable[] =
    {
    0,
    28,
    50,
    84,
    124,
    170,
    216,
    250,
    290,
    318,
    358,
    398,
    432,
    472
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugSymbolProvider_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugSymbolProvider_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugSymbolProvider_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugSymbolProvider_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _IDebugSymbolProviderProxyVtbl = 
{
    &IDebugSymbolProvider_ProxyInfo,
    &IID_IDebugSymbolProvider,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::Initialize */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::Uninitialize */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetContainerField */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetField */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetAddressesFromPosition */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetAddressesFromContext */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetContextFromAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetLanguage */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetGlobalContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetMethodFieldsByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetClassTypeByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetNamespacesUsedAtAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetTypeByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetNextAddress */
};

const CInterfaceStubVtbl _IDebugSymbolProviderStubVtbl =
{
    &IID_IDebugSymbolProvider,
    &IDebugSymbolProvider_ServerInfo,
    17,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugComPlusSymbolProvider, ver. 0.0,
   GUID={0xc2e34eaf,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugComPlusSymbolProvider_FormatStringOffsetTable[] =
    {
    0,
    28,
    50,
    84,
    124,
    170,
    216,
    250,
    290,
    318,
    358,
    398,
    432,
    472,
    512,
    570,
    604,
    644,
    678,
    718,
    758,
    798,
    856,
    902,
    966,
    1006,
    1040,
    1092,
    1132,
    1178,
    1218,
    1246,
    1274,
    1314,
    1342,
    1394
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugComPlusSymbolProvider_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugComPlusSymbolProvider_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugComPlusSymbolProvider_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugComPlusSymbolProvider_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(39) _IDebugComPlusSymbolProviderProxyVtbl = 
{
    &IDebugComPlusSymbolProvider_ProxyInfo,
    &IID_IDebugComPlusSymbolProvider,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::Initialize */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::Uninitialize */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetContainerField */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetField */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetAddressesFromPosition */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetAddressesFromContext */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetContextFromAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetLanguage */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetGlobalContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetMethodFieldsByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetClassTypeByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetNamespacesUsedAtAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetTypeByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetNextAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::LoadSymbols */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::UnloadSymbols */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetEntryPoint */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetTypeFromAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::UpdateSymbols */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::CreateTypeFromPrimitive */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetFunctionLineOffset */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetAddressesInModuleFromPosition */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetArrayTypeFromAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetSymAttribute */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::ReplaceSymbols */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::AreSymbolsLoaded */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::LoadSymbolsFromStream */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetSymUnmanagedReader */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetAttributedClassesinModule */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetAttributedClassesForLanguage */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::IsHiddenCode */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::IsFunctionDeleted */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetNameFromToken */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::IsFunctionStale */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetLocalVariablelayout */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSymbolProvider::GetAssemblyName */
};

const CInterfaceStubVtbl _IDebugComPlusSymbolProviderStubVtbl =
{
    &IID_IDebugComPlusSymbolProvider,
    &IDebugComPlusSymbolProvider_ServerInfo,
    39,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugNativeSymbolProvider, ver. 0.0,
   GUID={0xc2e34eb0,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugNativeSymbolProvider_FormatStringOffsetTable[] =
    {
    0,
    28,
    50,
    84,
    124,
    170,
    216,
    250,
    290,
    318,
    358,
    398,
    432,
    472,
    1434
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugNativeSymbolProvider_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugNativeSymbolProvider_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugNativeSymbolProvider_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugNativeSymbolProvider_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(18) _IDebugNativeSymbolProviderProxyVtbl = 
{
    &IDebugNativeSymbolProvider_ProxyInfo,
    &IID_IDebugNativeSymbolProvider,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::Initialize */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::Uninitialize */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetContainerField */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetField */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetAddressesFromPosition */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetAddressesFromContext */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetContextFromAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetLanguage */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetGlobalContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetMethodFieldsByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetClassTypeByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetNamespacesUsedAtAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetTypeByName */ ,
    (void *) (INT_PTR) -1 /* IDebugSymbolProvider::GetNextAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugNativeSymbolProvider::LoadSymbols */
};

const CInterfaceStubVtbl _IDebugNativeSymbolProviderStubVtbl =
{
    &IID_IDebugNativeSymbolProvider,
    &IDebugNativeSymbolProvider_ServerInfo,
    18,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugField, ver. 0.0,
   GUID={0xc2e34eb1,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IDebugFieldProxyVtbl = 
{
    &IDebugField_ProxyInfo,
    &IID_IDebugField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */
};

const CInterfaceStubVtbl _IDebugFieldStubVtbl =
{
    &IID_IDebugField,
    &IDebugField_ServerInfo,
    12,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugContainerField, ver. 0.0,
   GUID={0xc2e34eb2,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugContainerField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    1732
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugContainerField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugContainerField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugContainerField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugContainerField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IDebugContainerFieldProxyVtbl = 
{
    &IDebugContainerField_ProxyInfo,
    &IID_IDebugContainerField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugContainerField::EnumFields */
};

const CInterfaceStubVtbl _IDebugContainerFieldStubVtbl =
{
    &IID_IDebugContainerField,
    &IDebugContainerField_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugMethodField, ver. 0.0,
   GUID={0xc2e34eb4,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMethodField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    1732,
    1784,
    1812,
    1840,
    1874,
    1908,
    1936,
    1964,
    1992
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMethodField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMethodField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMethodField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMethodField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(21) _IDebugMethodFieldProxyVtbl = 
{
    &IDebugMethodField_ProxyInfo,
    &IID_IDebugMethodField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugContainerField::EnumFields */ ,
    (void *) (INT_PTR) -1 /* IDebugMethodField::EnumParameters */ ,
    (void *) (INT_PTR) -1 /* IDebugMethodField::GetThis */ ,
    (void *) (INT_PTR) -1 /* IDebugMethodField::EnumAllLocals */ ,
    (void *) (INT_PTR) -1 /* IDebugMethodField::EnumLocals */ ,
    (void *) (INT_PTR) -1 /* IDebugMethodField::IsCustomAttributeDefined */ ,
    (void *) (INT_PTR) -1 /* IDebugMethodField::EnumStaticLocals */ ,
    (void *) (INT_PTR) -1 /* IDebugMethodField::GetGlobalContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugMethodField::EnumArguments */
};

const CInterfaceStubVtbl _IDebugMethodFieldStubVtbl =
{
    &IID_IDebugMethodField,
    &IDebugMethodField_ServerInfo,
    21,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_sh_0379, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IDebugClassField, ver. 0.0,
   GUID={0xc2e34eb5,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugClassField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    1732,
    1784,
    2020,
    2048,
    2076,
    2104,
    2132,
    2166,
    1992
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugClassField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugClassField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugClassField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugClassField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(21) _IDebugClassFieldProxyVtbl = 
{
    &IDebugClassField_ProxyInfo,
    &IID_IDebugClassField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugContainerField::EnumFields */ ,
    (void *) (INT_PTR) -1 /* IDebugClassField::EnumBaseClasses */ ,
    (void *) (INT_PTR) -1 /* IDebugClassField::DoesInterfaceExist */ ,
    (void *) (INT_PTR) -1 /* IDebugClassField::EnumNestedClasses */ ,
    (void *) (INT_PTR) -1 /* IDebugClassField::GetEnclosingClass */ ,
    (void *) (INT_PTR) -1 /* IDebugClassField::EnumInterfacesImplemented */ ,
    (void *) (INT_PTR) -1 /* IDebugClassField::EnumConstructors */ ,
    (void *) (INT_PTR) -1 /* IDebugClassField::GetDefaultIndexer */ ,
    (void *) (INT_PTR) -1 /* IDebugClassField::EnumNestedEnums */
};

const CInterfaceStubVtbl _IDebugClassFieldStubVtbl =
{
    &IID_IDebugClassField,
    &IDebugClassField_ServerInfo,
    21,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPropertyField, ver. 0.0,
   GUID={0xc2e34eb6,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPropertyField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    1732,
    2194,
    2222
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPropertyField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPropertyField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPropertyField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPropertyField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _IDebugPropertyFieldProxyVtbl = 
{
    &IDebugPropertyField_ProxyInfo,
    &IID_IDebugPropertyField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugContainerField::EnumFields */ ,
    (void *) (INT_PTR) -1 /* IDebugPropertyField::GetPropertyGetter */ ,
    (void *) (INT_PTR) -1 /* IDebugPropertyField::GetPropertySetter */
};

const CInterfaceStubVtbl _IDebugPropertyFieldStubVtbl =
{
    &IID_IDebugPropertyField,
    &IDebugPropertyField_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugArrayField, ver. 0.0,
   GUID={0xc2e34eb7,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugArrayField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    1732,
    2250,
    2278,
    2306
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugArrayField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugArrayField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugArrayField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugArrayField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(16) _IDebugArrayFieldProxyVtbl = 
{
    &IDebugArrayField_ProxyInfo,
    &IID_IDebugArrayField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugContainerField::EnumFields */ ,
    (void *) (INT_PTR) -1 /* IDebugArrayField::GetNumberOfElements */ ,
    (void *) (INT_PTR) -1 /* IDebugArrayField::GetElementType */ ,
    (void *) (INT_PTR) -1 /* IDebugArrayField::GetRank */
};

const CInterfaceStubVtbl _IDebugArrayFieldStubVtbl =
{
    &IID_IDebugArrayField,
    &IDebugArrayField_ServerInfo,
    16,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPointerField, ver. 0.0,
   GUID={0xc2e34eb8,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPointerField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    1732,
    2334
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPointerField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPointerField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPointerField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPointerField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IDebugPointerFieldProxyVtbl = 
{
    &IDebugPointerField_ProxyInfo,
    &IID_IDebugPointerField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugContainerField::EnumFields */ ,
    (void *) (INT_PTR) -1 /* IDebugPointerField::GetDereferencedField */
};

const CInterfaceStubVtbl _IDebugPointerFieldStubVtbl =
{
    &IID_IDebugPointerField,
    &IDebugPointerField_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEnumField, ver. 0.0,
   GUID={0xc2e34eb9,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEnumField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    1732,
    2334,
    2362,
    2396,
    2430
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEnumField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEnumField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEnumField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEnumField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _IDebugEnumFieldProxyVtbl = 
{
    &IDebugEnumField_ProxyInfo,
    &IID_IDebugEnumField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugContainerField::EnumFields */ ,
    (void *) (INT_PTR) -1 /* IDebugEnumField::GetUnderlyingSymbol */ ,
    (void *) (INT_PTR) -1 /* IDebugEnumField::GetStringFromValue */ ,
    (void *) (INT_PTR) -1 /* IDebugEnumField::GetValueFromString */ ,
    (void *) (INT_PTR) -1 /* IDebugEnumField::GetValueFromStringCaseInsensitive */
};

const CInterfaceStubVtbl _IDebugEnumFieldStubVtbl =
{
    &IID_IDebugEnumField,
    &IDebugEnumField_ServerInfo,
    17,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugBitField, ver. 0.0,
   GUID={0xc2e34eba,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBitField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    2464
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBitField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBitField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBitField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBitField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IDebugBitFieldProxyVtbl = 
{
    &IDebugBitField_ProxyInfo,
    &IID_IDebugBitField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugBitField::GetStart */
};

const CInterfaceStubVtbl _IDebugBitFieldStubVtbl =
{
    &IID_IDebugBitField,
    &IDebugBitField_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugDynamicField, ver. 0.0,
   GUID={0xB5A2A5EA,0xD5AB,0x11d2,{0x90,0x33,0x00,0xC0,0x4F,0xA3,0x02,0xA1}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDynamicField_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDynamicField_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDynamicField_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDynamicField_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDynamicField_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IDebugDynamicFieldProxyVtbl = 
{
    0,
    &IID_IDebugDynamicField,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* forced delegation IDebugField::GetInfo */ ,
    0 /* forced delegation IDebugField::GetKind */ ,
    0 /* forced delegation IDebugField::GetType */ ,
    0 /* forced delegation IDebugField::GetContainer */ ,
    0 /* forced delegation IDebugField::GetAddress */ ,
    0 /* forced delegation IDebugField::GetSize */ ,
    0 /* forced delegation IDebugField::GetExtendedInfo */ ,
    0 /* forced delegation IDebugField::Equal */ ,
    0 /* forced delegation IDebugField::GetTypeInfo */
};


static const PRPC_STUB_FUNCTION IDebugDynamicField_table[] =
{
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IDebugDynamicFieldStubVtbl =
{
    &IID_IDebugDynamicField,
    &IDebugDynamicField_ServerInfo,
    12,
    &IDebugDynamicField_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IDebugDynamicFieldCOMPlus, ver. 0.0,
   GUID={0xB5B20820,0xE233,0x11d2,{0x90,0x37,0x00,0xC0,0x4F,0xA3,0x02,0xA1}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDynamicFieldCOMPlus_FormatStringOffsetTable[] =
    {
    1462,
    1496,
    1524,
    1552,
    1580,
    1608,
    1636,
    1676,
    1704,
    2492,
    2526
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDynamicFieldCOMPlus_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDynamicFieldCOMPlus_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDynamicFieldCOMPlus_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDynamicFieldCOMPlus_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IDebugDynamicFieldCOMPlusProxyVtbl = 
{
    &IDebugDynamicFieldCOMPlus_ProxyInfo,
    &IID_IDebugDynamicFieldCOMPlus,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugField::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetKind */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetType */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetAddress */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetExtendedInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugField::Equal */ ,
    (void *) (INT_PTR) -1 /* IDebugField::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugDynamicFieldCOMPlus::GetTypeFromPrimitive */ ,
    (void *) (INT_PTR) -1 /* IDebugDynamicFieldCOMPlus::GetTypeFromTypeDef */
};

const CInterfaceStubVtbl _IDebugDynamicFieldCOMPlusStubVtbl =
{
    &IID_IDebugDynamicFieldCOMPlus,
    &IDebugDynamicFieldCOMPlus_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEngineSymbolProviderServices, ver. 0.0,
   GUID={0x83919262,0xACD6,0x11d2,{0x90,0x28,0x00,0xC0,0x4F,0xA3,0x02,0xA1}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEngineSymbolProviderServices_FormatStringOffsetTable[] =
    {
    2572
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEngineSymbolProviderServices_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEngineSymbolProviderServices_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEngineSymbolProviderServices_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEngineSymbolProviderServices_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugEngineSymbolProviderServicesProxyVtbl = 
{
    &IDebugEngineSymbolProviderServices_ProxyInfo,
    &IID_IDebugEngineSymbolProviderServices,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugEngineSymbolProviderServices::EnumCodeContexts */
};

const CInterfaceStubVtbl _IDebugEngineSymbolProviderServicesStubVtbl =
{
    &IID_IDebugEngineSymbolProviderServices,
    &IDebugEngineSymbolProviderServices_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugAddress, ver. 0.0,
   GUID={0xc2e34ebb,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugAddress_FormatStringOffsetTable[] =
    {
    2612
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugAddress_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugAddress_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugAddress_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugAddress_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugAddressProxyVtbl = 
{
    &IDebugAddress_ProxyInfo,
    &IID_IDebugAddress,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugAddress::GetAddress */
};

const CInterfaceStubVtbl _IDebugAddressStubVtbl =
{
    &IID_IDebugAddress,
    &IDebugAddress_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugFields, ver. 0.0,
   GUID={0xc2e34ebc,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugFields_FormatStringOffsetTable[] =
    {
    2640,
    2680,
    2708,
    2730,
    2758
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugFields_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugFields_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugFields_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugFields_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugFieldsProxyVtbl = 
{
    &IEnumDebugFields_ProxyInfo,
    &IID_IEnumDebugFields,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugFields::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugFields::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugFields::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugFields::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugFields::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugFieldsStubVtbl =
{
    &IID_IEnumDebugFields,
    &IEnumDebugFields_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugAddresses, ver. 0.0,
   GUID={0xc2e34ebd,0x8b9d,0x11d2,{0x90,0x14,0x00,0xc0,0x4f,0xa3,0x83,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugAddresses_FormatStringOffsetTable[] =
    {
    2786,
    2680,
    2708,
    2826,
    2758
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugAddresses_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugAddresses_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugAddresses_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugAddresses_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugAddressesProxyVtbl = 
{
    &IEnumDebugAddresses_ProxyInfo,
    &IID_IEnumDebugAddresses,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugAddresses::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugAddresses::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugAddresses::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugAddresses::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugAddresses::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugAddressesStubVtbl =
{
    &IID_IEnumDebugAddresses,
    &IEnumDebugAddresses_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugCustomAttributeQuery, ver. 0.0,
   GUID={0xDFD37B5A,0x1E3A,0x4f15,{0x80,0x98,0x22,0x0A,0xBA,0xDC,0x62,0x0B}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugCustomAttributeQuery_FormatStringOffsetTable[] =
    {
    2854,
    2882
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugCustomAttributeQuery_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugCustomAttributeQuery_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugCustomAttributeQuery_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugCustomAttributeQuery_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugCustomAttributeQueryProxyVtbl = 
{
    &IDebugCustomAttributeQuery_ProxyInfo,
    &IID_IDebugCustomAttributeQuery,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugCustomAttributeQuery::IsCustomAttributeDefined */ ,
    (void *) (INT_PTR) -1 /* IDebugCustomAttributeQuery::GetCustomAttributeByName */
};

const CInterfaceStubVtbl _IDebugCustomAttributeQueryStubVtbl =
{
    &IID_IDebugCustomAttributeQuery,
    &IDebugCustomAttributeQuery_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _sh_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IDebugDynamicFieldCOMPlusProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugCustomAttributeQueryProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEngineSymbolProviderServicesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugSymbolProviderProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugComPlusSymbolProviderProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugNativeSymbolProviderProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugContainerFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMethodFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugClassFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPropertyFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugArrayFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPointerFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEnumFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugBitFieldProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugAddressProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugFieldsProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugAddressesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugDynamicFieldProxyVtbl,
    0
};

const CInterfaceStubVtbl * _sh_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IDebugDynamicFieldCOMPlusStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugCustomAttributeQueryStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEngineSymbolProviderServicesStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugSymbolProviderStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugComPlusSymbolProviderStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugNativeSymbolProviderStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugContainerFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMethodFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugClassFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPropertyFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugArrayFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPointerFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEnumFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugBitFieldStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugAddressStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugFieldsStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugAddressesStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugDynamicFieldStubVtbl,
    0
};

PCInterfaceName const _sh_InterfaceNamesList[] = 
{
    "IDebugDynamicFieldCOMPlus",
    "IDebugCustomAttributeQuery",
    "IDebugEngineSymbolProviderServices",
    "IDebugSymbolProvider",
    "IDebugComPlusSymbolProvider",
    "IDebugNativeSymbolProvider",
    "IDebugField",
    "IDebugContainerField",
    "IDebugMethodField",
    "IDebugClassField",
    "IDebugPropertyField",
    "IDebugArrayField",
    "IDebugPointerField",
    "IDebugEnumField",
    "IDebugBitField",
    "IDebugAddress",
    "IEnumDebugFields",
    "IEnumDebugAddresses",
    "IDebugDynamicField",
    0
};

const IID *  _sh_BaseIIDList[] = 
{
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    &IID_IDebugField,   /* forced */
    0
};


#define _sh_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _sh, pIID, n)

int __stdcall _sh_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _sh, 19, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _sh, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _sh, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _sh, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _sh, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _sh, 19, *pIndex )
    
}

const ExtendedProxyFileInfo sh_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _sh_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _sh_StubVtblList,
    (const PCInterfaceName * ) & _sh_InterfaceNamesList,
    (const IID ** ) & _sh_BaseIIDList,
    & _sh_IID_Lookup, 
    19,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

