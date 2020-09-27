
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:31 2002
 */
/* Compiler settings for pbagex.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
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
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "pbagex.h"

#define TYPE_FORMAT_STRING_SIZE   2449                              
#define PROC_FORMAT_STRING_SIZE   415                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

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


extern const MIDL_SERVER_INFO IPropertyBagEx_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPropertyBagEx_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATPROPBAG_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPBAG_ProxyInfo;


extern const EXPR_EVAL ExprEvalRoutines[];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure ReadMultiple */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 16 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 18 */	NdrFcShort( 0x44 ),	/* 68 */
/* 20 */	NdrFcShort( 0x45 ),	/* 69 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cprops */

/* 24 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgwszPropNames */

/* 30 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter rgvar */

/* 36 */	NdrFcShort( 0x1b ),	/* Flags:  must size, must free, in, out, */
/* 38 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 40 */	NdrFcShort( 0x8f4 ),	/* Type Offset=2292 */

	/* Parameter pErrorLog */

/* 42 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	NdrFcShort( 0x90a ),	/* Type Offset=2314 */

	/* Return value */

/* 48 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 50 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 52 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteMultiple */

/* 54 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 56 */	NdrFcLong( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x4 ),	/* 4 */
/* 62 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 64 */	NdrFcShort( 0x8 ),	/* 8 */
/* 66 */	NdrFcShort( 0x8 ),	/* 8 */
/* 68 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 70 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x45 ),	/* 69 */
/* 76 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cprops */

/* 78 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 80 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgwszPropNames */

/* 84 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 86 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 88 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter rgvar */

/* 90 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 92 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 94 */	NdrFcShort( 0x8f4 ),	/* Type Offset=2292 */

	/* Return value */

/* 96 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 98 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteMultiple */

/* 102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x5 ),	/* 5 */
/* 110 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 112 */	NdrFcShort( 0x10 ),	/* 16 */
/* 114 */	NdrFcShort( 0x8 ),	/* 8 */
/* 116 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 118 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x1 ),	/* 1 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cprops */

/* 126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 128 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgwszPropNames */

/* 132 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 134 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 136 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter dwReserved */

/* 138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 140 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 144 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 146 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Open */

/* 150 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x6 ),	/* 6 */
/* 158 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 160 */	NdrFcShort( 0x7c ),	/* 124 */
/* 162 */	NdrFcShort( 0x8 ),	/* 8 */
/* 164 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 166 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 168 */	NdrFcShort( 0x1 ),	/* 1 */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUnkOuter */

/* 174 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 176 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 178 */	NdrFcShort( 0x30e ),	/* Type Offset=782 */

	/* Parameter wszPropName */

/* 180 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 182 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 184 */	NdrFcShort( 0x91e ),	/* Type Offset=2334 */

	/* Parameter guidPropertyType */

/* 186 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 188 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 190 */	NdrFcShort( 0x290 ),	/* Type Offset=656 */

	/* Parameter dwFlags */

/* 192 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 194 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 196 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 198 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 200 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 202 */	NdrFcShort( 0x290 ),	/* Type Offset=656 */

	/* Parameter ppUnk */

/* 204 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 206 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 208 */	NdrFcShort( 0x924 ),	/* Type Offset=2340 */

	/* Return value */

/* 210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 212 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Enum */

/* 216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 222 */	NdrFcShort( 0x7 ),	/* 7 */
/* 224 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 230 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 232 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter wszPropNameMask */

/* 240 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 242 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 244 */	NdrFcShort( 0x91e ),	/* Type Offset=2334 */

	/* Parameter dwFlags */

/* 246 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 248 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenum */

/* 252 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 254 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 256 */	NdrFcShort( 0x930 ),	/* Type Offset=2352 */

	/* Return value */

/* 258 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 260 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 262 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 264 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 266 */	NdrFcLong( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x3 ),	/* 3 */
/* 272 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 274 */	NdrFcShort( 0x8 ),	/* 8 */
/* 276 */	NdrFcShort( 0x24 ),	/* 36 */
/* 278 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 280 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 282 */	NdrFcShort( 0x1 ),	/* 1 */
/* 284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 288 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 290 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 294 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 296 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 298 */	NdrFcShort( 0x964 ),	/* Type Offset=2404 */

	/* Parameter pceltFetched */

/* 300 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 302 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 306 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 308 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */

/* 312 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x4 ),	/* 4 */
/* 320 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 324 */	NdrFcShort( 0x8 ),	/* 8 */
/* 326 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 328 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 336 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 338 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 342 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 344 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */

/* 348 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x5 ),	/* 5 */
/* 356 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x8 ),	/* 8 */
/* 362 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 364 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 374 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 378 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x6 ),	/* 6 */
/* 386 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 390 */	NdrFcShort( 0x8 ),	/* 8 */
/* 392 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 394 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 400 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 402 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 404 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 406 */	NdrFcShort( 0x930 ),	/* Type Offset=2352 */

	/* Return value */

/* 408 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 410 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 412 */	0x8,		/* FC_LONG */
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
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/*  4 */	NdrFcShort( 0x4 ),	/* 4 */
/*  6 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/*  8 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 10 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 12 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 14 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 16 */	NdrFcShort( 0x4 ),	/* 4 */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x1 ),	/* 1 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 28 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 30 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 32 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 34 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x7,		/* FC_USHORT */
/* 36 */	0x0,		/* Corr desc:  */
			0x59,		/* FC_CALLBACK */
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 42 */	NdrFcShort( 0x2 ),	/* Offset= 2 (44) */
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	NdrFcShort( 0x61 ),	/* 97 */
/* 48 */	NdrFcLong( 0x0 ),	/* 0 */
/* 52 */	NdrFcShort( 0x0 ),	/* Offset= 0 (52) */
/* 54 */	NdrFcLong( 0x1 ),	/* 1 */
/* 58 */	NdrFcShort( 0x0 ),	/* Offset= 0 (58) */
/* 60 */	NdrFcLong( 0x10 ),	/* 16 */
/* 64 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 66 */	NdrFcLong( 0x11 ),	/* 17 */
/* 70 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 72 */	NdrFcLong( 0x2 ),	/* 2 */
/* 76 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 78 */	NdrFcLong( 0x12 ),	/* 18 */
/* 82 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 84 */	NdrFcLong( 0x3 ),	/* 3 */
/* 88 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 90 */	NdrFcLong( 0x13 ),	/* 19 */
/* 94 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 96 */	NdrFcLong( 0x16 ),	/* 22 */
/* 100 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 102 */	NdrFcLong( 0x17 ),	/* 23 */
/* 106 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 108 */	NdrFcLong( 0xe ),	/* 14 */
/* 112 */	NdrFcShort( 0x208 ),	/* Offset= 520 (632) */
/* 114 */	NdrFcLong( 0x14 ),	/* 20 */
/* 118 */	NdrFcShort( 0x202 ),	/* Offset= 514 (632) */
/* 120 */	NdrFcLong( 0x15 ),	/* 21 */
/* 124 */	NdrFcShort( 0x1fc ),	/* Offset= 508 (632) */
/* 126 */	NdrFcLong( 0x4 ),	/* 4 */
/* 130 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 132 */	NdrFcLong( 0x5 ),	/* 5 */
/* 136 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 138 */	NdrFcLong( 0xb ),	/* 11 */
/* 142 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 144 */	NdrFcLong( 0xffff ),	/* 65535 */
/* 148 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 150 */	NdrFcLong( 0xa ),	/* 10 */
/* 154 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 156 */	NdrFcLong( 0x6 ),	/* 6 */
/* 160 */	NdrFcShort( 0x1d8 ),	/* Offset= 472 (632) */
/* 162 */	NdrFcLong( 0x7 ),	/* 7 */
/* 166 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 168 */	NdrFcLong( 0x40 ),	/* 64 */
/* 172 */	NdrFcShort( 0x1d2 ),	/* Offset= 466 (638) */
/* 174 */	NdrFcLong( 0x48 ),	/* 72 */
/* 178 */	NdrFcShort( 0x1d4 ),	/* Offset= 468 (646) */
/* 180 */	NdrFcLong( 0x47 ),	/* 71 */
/* 184 */	NdrFcShort( 0x1e4 ),	/* Offset= 484 (668) */
/* 186 */	NdrFcLong( 0x8 ),	/* 8 */
/* 190 */	NdrFcShort( 0x21e ),	/* Offset= 542 (732) */
/* 192 */	NdrFcLong( 0xfff ),	/* 4095 */
/* 196 */	NdrFcShort( 0x22e ),	/* Offset= 558 (754) */
/* 198 */	NdrFcLong( 0x41 ),	/* 65 */
/* 202 */	NdrFcShort( 0x228 ),	/* Offset= 552 (754) */
/* 204 */	NdrFcLong( 0x46 ),	/* 70 */
/* 208 */	NdrFcShort( 0x222 ),	/* Offset= 546 (754) */
/* 210 */	NdrFcLong( 0x1e ),	/* 30 */
/* 214 */	NdrFcShort( 0x230 ),	/* Offset= 560 (774) */
/* 216 */	NdrFcLong( 0x1f ),	/* 31 */
/* 220 */	NdrFcShort( 0x22e ),	/* Offset= 558 (778) */
/* 222 */	NdrFcLong( 0xd ),	/* 13 */
/* 226 */	NdrFcShort( 0x22c ),	/* Offset= 556 (782) */
/* 228 */	NdrFcLong( 0x9 ),	/* 9 */
/* 232 */	NdrFcShort( 0x238 ),	/* Offset= 568 (800) */
/* 234 */	NdrFcLong( 0x42 ),	/* 66 */
/* 238 */	NdrFcShort( 0x244 ),	/* Offset= 580 (818) */
/* 240 */	NdrFcLong( 0x44 ),	/* 68 */
/* 244 */	NdrFcShort( 0x23e ),	/* Offset= 574 (818) */
/* 246 */	NdrFcLong( 0x43 ),	/* 67 */
/* 250 */	NdrFcShort( 0x24a ),	/* Offset= 586 (836) */
/* 252 */	NdrFcLong( 0x45 ),	/* 69 */
/* 256 */	NdrFcShort( 0x244 ),	/* Offset= 580 (836) */
/* 258 */	NdrFcLong( 0x49 ),	/* 73 */
/* 262 */	NdrFcShort( 0x250 ),	/* Offset= 592 (854) */
/* 264 */	NdrFcLong( 0x2010 ),	/* 8208 */
/* 268 */	NdrFcShort( 0x5d0 ),	/* Offset= 1488 (1756) */
/* 270 */	NdrFcLong( 0x2011 ),	/* 8209 */
/* 274 */	NdrFcShort( 0x5ca ),	/* Offset= 1482 (1756) */
/* 276 */	NdrFcLong( 0x2002 ),	/* 8194 */
/* 280 */	NdrFcShort( 0x5c4 ),	/* Offset= 1476 (1756) */
/* 282 */	NdrFcLong( 0x2012 ),	/* 8210 */
/* 286 */	NdrFcShort( 0x5be ),	/* Offset= 1470 (1756) */
/* 288 */	NdrFcLong( 0x2003 ),	/* 8195 */
/* 292 */	NdrFcShort( 0x5b8 ),	/* Offset= 1464 (1756) */
/* 294 */	NdrFcLong( 0x2013 ),	/* 8211 */
/* 298 */	NdrFcShort( 0x5b2 ),	/* Offset= 1458 (1756) */
/* 300 */	NdrFcLong( 0x2016 ),	/* 8214 */
/* 304 */	NdrFcShort( 0x5ac ),	/* Offset= 1452 (1756) */
/* 306 */	NdrFcLong( 0x2017 ),	/* 8215 */
/* 310 */	NdrFcShort( 0x5a6 ),	/* Offset= 1446 (1756) */
/* 312 */	NdrFcLong( 0x2004 ),	/* 8196 */
/* 316 */	NdrFcShort( 0x5a0 ),	/* Offset= 1440 (1756) */
/* 318 */	NdrFcLong( 0x2005 ),	/* 8197 */
/* 322 */	NdrFcShort( 0x59a ),	/* Offset= 1434 (1756) */
/* 324 */	NdrFcLong( 0x2006 ),	/* 8198 */
/* 328 */	NdrFcShort( 0x594 ),	/* Offset= 1428 (1756) */
/* 330 */	NdrFcLong( 0x2007 ),	/* 8199 */
/* 334 */	NdrFcShort( 0x58e ),	/* Offset= 1422 (1756) */
/* 336 */	NdrFcLong( 0x2008 ),	/* 8200 */
/* 340 */	NdrFcShort( 0x588 ),	/* Offset= 1416 (1756) */
/* 342 */	NdrFcLong( 0x200b ),	/* 8203 */
/* 346 */	NdrFcShort( 0x582 ),	/* Offset= 1410 (1756) */
/* 348 */	NdrFcLong( 0x200e ),	/* 8206 */
/* 352 */	NdrFcShort( 0x57c ),	/* Offset= 1404 (1756) */
/* 354 */	NdrFcLong( 0x2009 ),	/* 8201 */
/* 358 */	NdrFcShort( 0x576 ),	/* Offset= 1398 (1756) */
/* 360 */	NdrFcLong( 0x200d ),	/* 8205 */
/* 364 */	NdrFcShort( 0x570 ),	/* Offset= 1392 (1756) */
/* 366 */	NdrFcLong( 0x200a ),	/* 8202 */
/* 370 */	NdrFcShort( 0x56a ),	/* Offset= 1386 (1756) */
/* 372 */	NdrFcLong( 0x200c ),	/* 8204 */
/* 376 */	NdrFcShort( 0x564 ),	/* Offset= 1380 (1756) */
/* 378 */	NdrFcLong( 0x1010 ),	/* 4112 */
/* 382 */	NdrFcShort( 0x574 ),	/* Offset= 1396 (1778) */
/* 384 */	NdrFcLong( 0x1011 ),	/* 4113 */
/* 388 */	NdrFcShort( 0x56e ),	/* Offset= 1390 (1778) */
/* 390 */	NdrFcLong( 0x1002 ),	/* 4098 */
/* 394 */	NdrFcShort( 0x4dc ),	/* Offset= 1244 (1638) */
/* 396 */	NdrFcLong( 0x1012 ),	/* 4114 */
/* 400 */	NdrFcShort( 0x4d6 ),	/* Offset= 1238 (1638) */
/* 402 */	NdrFcLong( 0x1003 ),	/* 4099 */
/* 406 */	NdrFcShort( 0x4f0 ),	/* Offset= 1264 (1670) */
/* 408 */	NdrFcLong( 0x1013 ),	/* 4115 */
/* 412 */	NdrFcShort( 0x4ea ),	/* Offset= 1258 (1670) */
/* 414 */	NdrFcLong( 0x1014 ),	/* 4116 */
/* 418 */	NdrFcShort( 0x574 ),	/* Offset= 1396 (1814) */
/* 420 */	NdrFcLong( 0x1015 ),	/* 4117 */
/* 424 */	NdrFcShort( 0x56e ),	/* Offset= 1390 (1814) */
/* 426 */	NdrFcLong( 0x1004 ),	/* 4100 */
/* 430 */	NdrFcShort( 0x588 ),	/* Offset= 1416 (1846) */
/* 432 */	NdrFcLong( 0x1005 ),	/* 4101 */
/* 436 */	NdrFcShort( 0x5a2 ),	/* Offset= 1442 (1878) */
/* 438 */	NdrFcLong( 0x100b ),	/* 4107 */
/* 442 */	NdrFcShort( 0x4ac ),	/* Offset= 1196 (1638) */
/* 444 */	NdrFcLong( 0x100a ),	/* 4106 */
/* 448 */	NdrFcShort( 0x4c6 ),	/* Offset= 1222 (1670) */
/* 450 */	NdrFcLong( 0x1006 ),	/* 4102 */
/* 454 */	NdrFcShort( 0x550 ),	/* Offset= 1360 (1814) */
/* 456 */	NdrFcLong( 0x1007 ),	/* 4103 */
/* 460 */	NdrFcShort( 0x58a ),	/* Offset= 1418 (1878) */
/* 462 */	NdrFcLong( 0x1040 ),	/* 4160 */
/* 466 */	NdrFcShort( 0x5a8 ),	/* Offset= 1448 (1914) */
/* 468 */	NdrFcLong( 0x1048 ),	/* 4168 */
/* 472 */	NdrFcShort( 0x5c6 ),	/* Offset= 1478 (1950) */
/* 474 */	NdrFcLong( 0x1047 ),	/* 4167 */
/* 478 */	NdrFcShort( 0x5f6 ),	/* Offset= 1526 (2004) */
/* 480 */	NdrFcLong( 0x1008 ),	/* 4104 */
/* 484 */	NdrFcShort( 0x61a ),	/* Offset= 1562 (2046) */
/* 486 */	NdrFcLong( 0x1fff ),	/* 8191 */
/* 490 */	NdrFcShort( 0x64a ),	/* Offset= 1610 (2100) */
/* 492 */	NdrFcLong( 0x101e ),	/* 4126 */
/* 496 */	NdrFcShort( 0x678 ),	/* Offset= 1656 (2152) */
/* 498 */	NdrFcLong( 0x101f ),	/* 4127 */
/* 502 */	NdrFcShort( 0x6a6 ),	/* Offset= 1702 (2204) */
/* 504 */	NdrFcLong( 0x100c ),	/* 4108 */
/* 508 */	NdrFcShort( 0x6ca ),	/* Offset= 1738 (2246) */
/* 510 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 514 */	NdrFcShort( 0x3cc ),	/* Offset= 972 (1486) */
/* 516 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 520 */	NdrFcShort( 0x3c6 ),	/* Offset= 966 (1486) */
/* 522 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 526 */	NdrFcShort( 0x37e ),	/* Offset= 894 (1420) */
/* 528 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 532 */	NdrFcShort( 0x378 ),	/* Offset= 888 (1420) */
/* 534 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 538 */	NdrFcShort( 0x376 ),	/* Offset= 886 (1424) */
/* 540 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 544 */	NdrFcShort( 0x370 ),	/* Offset= 880 (1424) */
/* 546 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 550 */	NdrFcShort( 0x36a ),	/* Offset= 874 (1424) */
/* 552 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 556 */	NdrFcShort( 0x364 ),	/* Offset= 868 (1424) */
/* 558 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 562 */	NdrFcShort( 0x362 ),	/* Offset= 866 (1428) */
/* 564 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 568 */	NdrFcShort( 0x360 ),	/* Offset= 864 (1432) */
/* 570 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 574 */	NdrFcShort( 0x34e ),	/* Offset= 846 (1420) */
/* 576 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 580 */	NdrFcShort( 0x386 ),	/* Offset= 902 (1482) */
/* 582 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 586 */	NdrFcShort( 0x346 ),	/* Offset= 838 (1424) */
/* 588 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 592 */	NdrFcShort( 0x34c ),	/* Offset= 844 (1436) */
/* 594 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 598 */	NdrFcShort( 0x342 ),	/* Offset= 834 (1432) */
/* 600 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 604 */	NdrFcShort( 0x67a ),	/* Offset= 1658 (2262) */
/* 606 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 610 */	NdrFcShort( 0x342 ),	/* Offset= 834 (1444) */
/* 612 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 616 */	NdrFcShort( 0x340 ),	/* Offset= 832 (1448) */
/* 618 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 622 */	NdrFcShort( 0x66c ),	/* Offset= 1644 (2266) */
/* 624 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 628 */	NdrFcShort( 0x66a ),	/* Offset= 1642 (2270) */
/* 630 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (629) */
/* 632 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 634 */	NdrFcShort( 0x8 ),	/* 8 */
/* 636 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 638 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 640 */	NdrFcShort( 0x8 ),	/* 8 */
/* 642 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 644 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 646 */	
			0x13, 0x0,	/* FC_OP */
/* 648 */	NdrFcShort( 0x8 ),	/* Offset= 8 (656) */
/* 650 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 652 */	NdrFcShort( 0x8 ),	/* 8 */
/* 654 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 656 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 658 */	NdrFcShort( 0x10 ),	/* 16 */
/* 660 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 662 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 664 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (650) */
			0x5b,		/* FC_END */
/* 668 */	
			0x13, 0x0,	/* FC_OP */
/* 670 */	NdrFcShort( 0xe ),	/* Offset= 14 (684) */
/* 672 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 674 */	NdrFcShort( 0x1 ),	/* 1 */
/* 676 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 678 */	NdrFcShort( 0x1 ),	/* 1 */
/* 680 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 682 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 684 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 686 */	NdrFcShort( 0xc ),	/* 12 */
/* 688 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 690 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 692 */	NdrFcShort( 0x8 ),	/* 8 */
/* 694 */	NdrFcShort( 0x8 ),	/* 8 */
/* 696 */	0x13, 0x0,	/* FC_OP */
/* 698 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (672) */
/* 700 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 702 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 704 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 706 */	
			0x13, 0x0,	/* FC_OP */
/* 708 */	NdrFcShort( 0xe ),	/* Offset= 14 (722) */
/* 710 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 712 */	NdrFcShort( 0x2 ),	/* 2 */
/* 714 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 716 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 718 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 720 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 722 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 726 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (710) */
/* 728 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 730 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 732 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 736 */	NdrFcShort( 0x4 ),	/* 4 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (706) */
/* 742 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 744 */	NdrFcShort( 0x1 ),	/* 1 */
/* 746 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 750 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 752 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 754 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 756 */	NdrFcShort( 0x8 ),	/* 8 */
/* 758 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 760 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 762 */	NdrFcShort( 0x4 ),	/* 4 */
/* 764 */	NdrFcShort( 0x4 ),	/* 4 */
/* 766 */	0x13, 0x0,	/* FC_OP */
/* 768 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (742) */
/* 770 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 772 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 774 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 776 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 778 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 780 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 782 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 784 */	NdrFcLong( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0x0 ),	/* 0 */
/* 792 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 794 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 796 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 798 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 800 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 802 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 810 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 812 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 814 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 816 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 818 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 820 */	NdrFcLong( 0xc ),	/* 12 */
/* 824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 828 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 830 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 832 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 834 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 836 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 838 */	NdrFcLong( 0xb ),	/* 11 */
/* 842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 846 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 848 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 850 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 852 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 854 */	
			0x13, 0x0,	/* FC_OP */
/* 856 */	NdrFcShort( 0x2 ),	/* Offset= 2 (858) */
/* 858 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 860 */	NdrFcShort( 0x14 ),	/* 20 */
/* 862 */	NdrFcShort( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0xc ),	/* Offset= 12 (876) */
/* 866 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 868 */	NdrFcShort( 0xffffff2c ),	/* Offset= -212 (656) */
/* 870 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 872 */	NdrFcShort( 0xffffffca ),	/* Offset= -54 (818) */
/* 874 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 876 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 878 */	NdrFcShort( 0x2 ),	/* Offset= 2 (880) */
/* 880 */	
			0x13, 0x0,	/* FC_OP */
/* 882 */	NdrFcShort( 0x358 ),	/* Offset= 856 (1738) */
/* 884 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 886 */	NdrFcShort( 0x18 ),	/* 24 */
/* 888 */	NdrFcShort( 0xa ),	/* 10 */
/* 890 */	NdrFcLong( 0x8 ),	/* 8 */
/* 894 */	NdrFcShort( 0x5a ),	/* Offset= 90 (984) */
/* 896 */	NdrFcLong( 0xd ),	/* 13 */
/* 900 */	NdrFcShort( 0x7e ),	/* Offset= 126 (1026) */
/* 902 */	NdrFcLong( 0x9 ),	/* 9 */
/* 906 */	NdrFcShort( 0x9e ),	/* Offset= 158 (1064) */
/* 908 */	NdrFcLong( 0xc ),	/* 12 */
/* 912 */	NdrFcShort( 0x276 ),	/* Offset= 630 (1542) */
/* 914 */	NdrFcLong( 0x24 ),	/* 36 */
/* 918 */	NdrFcShort( 0x2a0 ),	/* Offset= 672 (1590) */
/* 920 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 924 */	NdrFcShort( 0x2aa ),	/* Offset= 682 (1606) */
/* 926 */	NdrFcLong( 0x10 ),	/* 16 */
/* 930 */	NdrFcShort( 0xffffff50 ),	/* Offset= -176 (754) */
/* 932 */	NdrFcLong( 0x2 ),	/* 2 */
/* 936 */	NdrFcShort( 0x2be ),	/* Offset= 702 (1638) */
/* 938 */	NdrFcLong( 0x3 ),	/* 3 */
/* 942 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (1670) */
/* 944 */	NdrFcLong( 0x14 ),	/* 20 */
/* 948 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (1702) */
/* 950 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (949) */
/* 952 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 954 */	NdrFcShort( 0x4 ),	/* 4 */
/* 956 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 960 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 962 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 964 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 966 */	NdrFcShort( 0x4 ),	/* 4 */
/* 968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 970 */	NdrFcShort( 0x1 ),	/* 1 */
/* 972 */	NdrFcShort( 0x0 ),	/* 0 */
/* 974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 976 */	0x13, 0x0,	/* FC_OP */
/* 978 */	NdrFcShort( 0xffffff00 ),	/* Offset= -256 (722) */
/* 980 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 982 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 984 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 986 */	NdrFcShort( 0x8 ),	/* 8 */
/* 988 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 990 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 992 */	NdrFcShort( 0x4 ),	/* 4 */
/* 994 */	NdrFcShort( 0x4 ),	/* 4 */
/* 996 */	0x11, 0x0,	/* FC_RP */
/* 998 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (952) */
/* 1000 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1002 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1004 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1008 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1012 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1014 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1018 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1020 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1022 */	NdrFcShort( 0xffffff10 ),	/* Offset= -240 (782) */
/* 1024 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1026 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1028 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1038) */
/* 1034 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1036 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1038 */	
			0x11, 0x0,	/* FC_RP */
/* 1040 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1004) */
/* 1042 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1050 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1052 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1056 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1058 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1060 */	NdrFcShort( 0xfffffefc ),	/* Offset= -260 (800) */
/* 1062 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1064 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1066 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1070 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1076) */
/* 1072 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1074 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1076 */	
			0x11, 0x0,	/* FC_RP */
/* 1078 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1042) */
/* 1080 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1082 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1084 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 1086 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1088 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1090) */
/* 1090 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1092 */	NdrFcShort( 0x2b ),	/* 43 */
/* 1094 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1098 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1100 */	NdrFcLong( 0x11 ),	/* 17 */
/* 1104 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 1106 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1110 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1112 */	NdrFcLong( 0x4 ),	/* 4 */
/* 1116 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 1118 */	NdrFcLong( 0x5 ),	/* 5 */
/* 1122 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 1124 */	NdrFcLong( 0xb ),	/* 11 */
/* 1128 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1130 */	NdrFcLong( 0xa ),	/* 10 */
/* 1134 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1136 */	NdrFcLong( 0x6 ),	/* 6 */
/* 1140 */	NdrFcShort( 0xfffffe04 ),	/* Offset= -508 (632) */
/* 1142 */	NdrFcLong( 0x7 ),	/* 7 */
/* 1146 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 1148 */	NdrFcLong( 0x8 ),	/* 8 */
/* 1152 */	NdrFcShort( 0xfffffe42 ),	/* Offset= -446 (706) */
/* 1154 */	NdrFcLong( 0xd ),	/* 13 */
/* 1158 */	NdrFcShort( 0xfffffe88 ),	/* Offset= -376 (782) */
/* 1160 */	NdrFcLong( 0x9 ),	/* 9 */
/* 1164 */	NdrFcShort( 0xfffffe94 ),	/* Offset= -364 (800) */
/* 1166 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 1170 */	NdrFcShort( 0xb8 ),	/* Offset= 184 (1354) */
/* 1172 */	NdrFcLong( 0x24 ),	/* 36 */
/* 1176 */	NdrFcShort( 0xba ),	/* Offset= 186 (1362) */
/* 1178 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 1182 */	NdrFcShort( 0xb4 ),	/* Offset= 180 (1362) */
/* 1184 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 1188 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (1416) */
/* 1190 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 1194 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (1420) */
/* 1196 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 1200 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (1424) */
/* 1202 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 1206 */	NdrFcShort( 0xde ),	/* Offset= 222 (1428) */
/* 1208 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 1212 */	NdrFcShort( 0xdc ),	/* Offset= 220 (1432) */
/* 1214 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 1218 */	NdrFcShort( 0xca ),	/* Offset= 202 (1420) */
/* 1220 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 1224 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (1424) */
/* 1226 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 1230 */	NdrFcShort( 0xce ),	/* Offset= 206 (1436) */
/* 1232 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 1236 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1432) */
/* 1238 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 1242 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (1440) */
/* 1244 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 1248 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1444) */
/* 1250 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 1254 */	NdrFcShort( 0xc2 ),	/* Offset= 194 (1448) */
/* 1256 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 1260 */	NdrFcShort( 0xc0 ),	/* Offset= 192 (1452) */
/* 1262 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 1266 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (1464) */
/* 1268 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1272 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 1274 */	NdrFcLong( 0x12 ),	/* 18 */
/* 1278 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1280 */	NdrFcLong( 0x13 ),	/* 19 */
/* 1284 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1286 */	NdrFcLong( 0x16 ),	/* 22 */
/* 1290 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1292 */	NdrFcLong( 0x17 ),	/* 23 */
/* 1296 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1298 */	NdrFcLong( 0xe ),	/* 14 */
/* 1302 */	NdrFcShort( 0xaa ),	/* Offset= 170 (1472) */
/* 1304 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 1308 */	NdrFcShort( 0xae ),	/* Offset= 174 (1482) */
/* 1310 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 1314 */	NdrFcShort( 0xac ),	/* Offset= 172 (1486) */
/* 1316 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 1320 */	NdrFcShort( 0x64 ),	/* Offset= 100 (1420) */
/* 1322 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 1326 */	NdrFcShort( 0x62 ),	/* Offset= 98 (1424) */
/* 1328 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 1332 */	NdrFcShort( 0x5c ),	/* Offset= 92 (1424) */
/* 1334 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 1338 */	NdrFcShort( 0x56 ),	/* Offset= 86 (1424) */
/* 1340 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1344 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1344) */
/* 1346 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1350 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1350) */
/* 1352 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1351) */
/* 1354 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1356 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1358) */
/* 1358 */	
			0x13, 0x0,	/* FC_OP */
/* 1360 */	NdrFcShort( 0x17a ),	/* Offset= 378 (1738) */
/* 1362 */	
			0x13, 0x0,	/* FC_OP */
/* 1364 */	NdrFcShort( 0x20 ),	/* Offset= 32 (1396) */
/* 1366 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1368 */	NdrFcLong( 0x2f ),	/* 47 */
/* 1372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1376 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1378 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1380 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1382 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1384 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1386 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1388 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1390 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1392 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1394 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1396 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1398 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1402 */	NdrFcShort( 0xa ),	/* Offset= 10 (1412) */
/* 1404 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1406 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1408 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (1366) */
/* 1410 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1412 */	
			0x13, 0x0,	/* FC_OP */
/* 1414 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1384) */
/* 1416 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1418 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1420 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1422 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1424 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1426 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1428 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1430 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1432 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1434 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1436 */	
			0x13, 0x0,	/* FC_OP */
/* 1438 */	NdrFcShort( 0xfffffcda ),	/* Offset= -806 (632) */
/* 1440 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1442 */	NdrFcShort( 0xfffffd20 ),	/* Offset= -736 (706) */
/* 1444 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1446 */	NdrFcShort( 0xfffffd68 ),	/* Offset= -664 (782) */
/* 1448 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1450 */	NdrFcShort( 0xfffffd76 ),	/* Offset= -650 (800) */
/* 1452 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1454 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1456) */
/* 1456 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1458 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1460) */
/* 1460 */	
			0x13, 0x0,	/* FC_OP */
/* 1462 */	NdrFcShort( 0x114 ),	/* Offset= 276 (1738) */
/* 1464 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1466 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1468) */
/* 1468 */	
			0x13, 0x0,	/* FC_OP */
/* 1470 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1490) */
/* 1472 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1474 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1476 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1478 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1480 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1482 */	
			0x13, 0x0,	/* FC_OP */
/* 1484 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1472) */
/* 1486 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1488 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1490 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1492 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1496 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1496) */
/* 1498 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1500 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1502 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1504 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1506 */	NdrFcShort( 0xfffffe56 ),	/* Offset= -426 (1080) */
/* 1508 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1510 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1512 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1514 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1518 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1520 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1522 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1524 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1528 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1534 */	0x13, 0x0,	/* FC_OP */
/* 1536 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (1490) */
/* 1538 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1540 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1542 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1544 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1548 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1554) */
/* 1550 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1552 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1554 */	
			0x11, 0x0,	/* FC_RP */
/* 1556 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (1510) */
/* 1558 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1560 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1562 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1566 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1568 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1570 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1572 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1576 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1578 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1582 */	0x13, 0x0,	/* FC_OP */
/* 1584 */	NdrFcShort( 0xffffff44 ),	/* Offset= -188 (1396) */
/* 1586 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1588 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1590 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1592 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1596 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1602) */
/* 1598 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1600 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1602 */	
			0x11, 0x0,	/* FC_RP */
/* 1604 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (1558) */
/* 1606 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1608 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1612 */	NdrFcShort( 0xa ),	/* Offset= 10 (1622) */
/* 1614 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1616 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1618 */	NdrFcShort( 0xfffffc3e ),	/* Offset= -962 (656) */
/* 1620 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1622 */	
			0x11, 0x0,	/* FC_RP */
/* 1624 */	NdrFcShort( 0xfffffd94 ),	/* Offset= -620 (1004) */
/* 1626 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1628 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1630 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1634 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1636 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1638 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1640 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1642 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1644 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1646 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1648 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1650 */	0x13, 0x0,	/* FC_OP */
/* 1652 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1626) */
/* 1654 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1656 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1658 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1660 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1662 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1664 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1666 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1668 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1670 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1672 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1674 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1676 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1678 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1680 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1682 */	0x13, 0x0,	/* FC_OP */
/* 1684 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1658) */
/* 1686 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1688 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1690 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1692 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1694 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1698 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1700 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1702 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1704 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1706 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1708 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1710 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1712 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1714 */	0x13, 0x0,	/* FC_OP */
/* 1716 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1690) */
/* 1718 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1720 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1722 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1726 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1728 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 1730 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1732 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1734 */	NdrFcShort( 0xfffffbb8 ),	/* Offset= -1096 (638) */
/* 1736 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1738 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1740 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1742 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (1722) */
/* 1744 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1744) */
/* 1746 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1748 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1750 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1752 */	NdrFcShort( 0xfffffc9c ),	/* Offset= -868 (884) */
/* 1754 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1756 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1758 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1760 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1764 */	NdrFcShort( 0xfffffc88 ),	/* Offset= -888 (876) */
/* 1766 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1768 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1770 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1772 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1774 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1776 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1778 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1780 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1782 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1784 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1786 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1788 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1790 */	0x13, 0x0,	/* FC_OP */
/* 1792 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1766) */
/* 1794 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1796 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1798 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1800 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1802 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1806 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1808 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1810 */	NdrFcShort( 0xfffffb66 ),	/* Offset= -1178 (632) */
/* 1812 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1814 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1816 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1818 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1820 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1822 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1824 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1826 */	0x13, 0x0,	/* FC_OP */
/* 1828 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1798) */
/* 1830 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1832 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1834 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1836 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1838 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1842 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1844 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 1846 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1848 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1850 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1852 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1854 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1856 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1858 */	0x13, 0x0,	/* FC_OP */
/* 1860 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1834) */
/* 1862 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1864 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1866 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1870 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1874 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1876 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 1878 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1882 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1884 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1886 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1888 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1890 */	0x13, 0x0,	/* FC_OP */
/* 1892 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1866) */
/* 1894 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1896 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1898 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1902 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1906 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1908 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1910 */	NdrFcShort( 0xfffffb08 ),	/* Offset= -1272 (638) */
/* 1912 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1914 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1916 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1918 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1920 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1922 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1924 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1926 */	0x13, 0x0,	/* FC_OP */
/* 1928 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1898) */
/* 1930 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1932 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1934 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1936 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1938 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1940 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1942 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1944 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1946 */	NdrFcShort( 0xfffffaf6 ),	/* Offset= -1290 (656) */
/* 1948 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1950 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1954 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1956 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1958 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1960 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1962 */	0x13, 0x0,	/* FC_OP */
/* 1964 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1934) */
/* 1966 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1968 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1970 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1972 */	NdrFcShort( 0xc ),	/* 12 */
/* 1974 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1976 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1978 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1980 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1982 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1984 */	NdrFcShort( 0xc ),	/* 12 */
/* 1986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1988 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1990 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1992 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1994 */	0x13, 0x0,	/* FC_OP */
/* 1996 */	NdrFcShort( 0xfffffad4 ),	/* Offset= -1324 (672) */
/* 1998 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2000 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffadb ),	/* Offset= -1317 (684) */
			0x5b,		/* FC_END */
/* 2004 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2006 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2008 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2010 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2012 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2014 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2016 */	0x13, 0x0,	/* FC_OP */
/* 2018 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (1970) */
/* 2020 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2022 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2024 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2028 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2032 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2034 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2038 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2040 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2042 */	NdrFcShort( 0xfffffae2 ),	/* Offset= -1310 (732) */
/* 2044 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2046 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2048 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2050 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2052 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2054 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2056 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2058 */	0x13, 0x0,	/* FC_OP */
/* 2060 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2024) */
/* 2062 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2064 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2066 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2068 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2070 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2074 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2076 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2078 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2080 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2084 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2086 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2088 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2090 */	0x13, 0x0,	/* FC_OP */
/* 2092 */	NdrFcShort( 0xfffffaba ),	/* Offset= -1350 (742) */
/* 2094 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2096 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffac1 ),	/* Offset= -1343 (754) */
			0x5b,		/* FC_END */
/* 2100 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2102 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2104 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2106 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2108 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2110 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2112 */	0x13, 0x0,	/* FC_OP */
/* 2114 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (2066) */
/* 2116 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2118 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2120 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2122 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2124 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2128 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2130 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2132 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2134 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2138 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2144 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2146 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 2148 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2150 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2152 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2154 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2156 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2158 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2160 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2162 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2164 */	0x13, 0x0,	/* FC_OP */
/* 2166 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (2120) */
/* 2168 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2170 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2172 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2174 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2176 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2180 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2182 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2184 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2186 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2190 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2196 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2198 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2200 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2202 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2204 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2206 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2208 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2210 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2212 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2214 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2216 */	0x13, 0x0,	/* FC_OP */
/* 2218 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (2172) */
/* 2220 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2222 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2224 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 2226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2228 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2232 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2234 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2238 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2240 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2242 */	NdrFcShort( 0x20 ),	/* Offset= 32 (2274) */
/* 2244 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2246 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2248 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2252 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2258) */
/* 2254 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 2256 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2258 */	
			0x13, 0x0,	/* FC_OP */
/* 2260 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2224) */
/* 2262 */	
			0x13, 0x0,	/* FC_OP */
/* 2264 */	NdrFcShort( 0xfffffa04 ),	/* Offset= -1532 (732) */
/* 2266 */	
			0x13, 0x0,	/* FC_OP */
/* 2268 */	NdrFcShort( 0xfffffe00 ),	/* Offset= -512 (1756) */
/* 2270 */	
			0x13, 0x0,	/* FC_OP */
/* 2272 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2274) */
/* 2274 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 2276 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2280 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2280) */
/* 2282 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 2284 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 2286 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2288 */	NdrFcShort( 0xfffff732 ),	/* Offset= -2254 (34) */
/* 2290 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2292 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 2294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2296 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2298 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2300 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2302 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2306 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2308 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2310 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2274) */
/* 2312 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2314 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2316 */	NdrFcLong( 0x3127ca40 ),	/* 824691264 */
/* 2320 */	NdrFcShort( 0x446e ),	/* 17518 */
/* 2322 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 2324 */	0x81,		/* 129 */
			0x35,		/* 53 */
/* 2326 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 2328 */	0x0,		/* 0 */
			0x4b,		/* 75 */
/* 2330 */	0xb8,		/* 184 */
			0x51,		/* 81 */
/* 2332 */	0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 2334 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2336 */	
			0x11, 0x0,	/* FC_RP */
/* 2338 */	NdrFcShort( 0xfffff96e ),	/* Offset= -1682 (656) */
/* 2340 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2342 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2344) */
/* 2344 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 2346 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 2348 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2350 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 2352 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2354 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2356) */
/* 2356 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2358 */	NdrFcLong( 0x20021801 ),	/* 537008129 */
/* 2362 */	NdrFcShort( 0x5de6 ),	/* 24038 */
/* 2364 */	NdrFcShort( 0x11d1 ),	/* 4561 */
/* 2366 */	0x8e,		/* 142 */
			0x38,		/* 56 */
/* 2368 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 2370 */	0x4f,		/* 79 */
			0xb9,		/* 185 */
/* 2372 */	0x38,		/* 56 */
			0x6d,		/* 109 */
/* 2374 */	
			0x11, 0x0,	/* FC_RP */
/* 2376 */	NdrFcShort( 0x1c ),	/* Offset= 28 (2404) */
/* 2378 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2380 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2382 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2384 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2390 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2392 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2394 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2396 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 2398 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2400 */	NdrFcShort( 0xfffff930 ),	/* Offset= -1744 (656) */
/* 2402 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2404 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 2406 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2408 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2410 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2412 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2414 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 2416 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2418 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2420 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2422 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 2424 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2428 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2434 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2436 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2438 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2440 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc1 ),	/* Offset= -63 (2378) */
			0x5b,		/* FC_END */
/* 2444 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 2446 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

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
            },
            {
            LPSAFEARRAY_UserSize
            ,LPSAFEARRAY_UserMarshal
            ,LPSAFEARRAY_UserUnmarshal
            ,LPSAFEARRAY_UserFree
            }

        };


static void __RPC_USER IPropertyBagEx_PROPVARIANTExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    PROPVARIANT *pS	=	( PROPVARIANT * )(pStubMsg->StackTop - 8);
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( ( unsigned short  )pS->vt );
}

static void __RPC_USER IPropertyBagEx_CLIPDATAExprEval_0001( PMIDL_STUB_MESSAGE pStubMsg )
{
    CLIPDATA *pS	=	( CLIPDATA * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( pS->cbSize - 4 );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IPropertyBagEx_PROPVARIANTExprEval_0000
    ,IPropertyBagEx_CLIPDATAExprEval_0001
    };



/* Standard interface: __MIDL_itf_pbagex_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IPropertyBagEx, ver. 0.0,
   GUID={0x20011801,0x5DE6,0x11D1,{0x8E,0x38,0x00,0xC0,0x4F,0xB9,0x38,0x6D}} */

#pragma code_seg(".orpc")
static const unsigned short IPropertyBagEx_FormatStringOffsetTable[] =
    {
    0,
    54,
    102,
    150,
    216
    };

static const MIDL_STUBLESS_PROXY_INFO IPropertyBagEx_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPropertyBagEx_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPropertyBagEx_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPropertyBagEx_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IPropertyBagExProxyVtbl = 
{
    &IPropertyBagEx_ProxyInfo,
    &IID_IPropertyBagEx,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::ReadMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::WriteMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::DeleteMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::Open */ ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::Enum */
};

const CInterfaceStubVtbl _IPropertyBagExStubVtbl =
{
    &IID_IPropertyBagEx,
    &IPropertyBagEx_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATPROPBAG, ver. 0.0,
   GUID={0x20021801,0x5DE6,0x11D1,{0x8E,0x38,0x00,0xC0,0x4F,0xB9,0x38,0x6D}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATPROPBAG_FormatStringOffsetTable[] =
    {
    264,
    312,
    348,
    378
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPBAG_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPBAG_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumSTATPROPBAG_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPBAG_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATPROPBAGProxyVtbl = 
{
    &IEnumSTATPROPBAG_ProxyInfo,
    &IID_IEnumSTATPROPBAG,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPBAG::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPBAG::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPBAG::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPBAG::Clone */
};

const CInterfaceStubVtbl _IEnumSTATPROPBAGStubVtbl =
{
    &IID_IEnumSTATPROPBAG,
    &IEnumSTATPROPBAG_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_pbagex_0112, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    ExprEvalRoutines,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
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

const CInterfaceProxyVtbl * _pbagex_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IPropertyBagExProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATPROPBAGProxyVtbl,
    0
};

const CInterfaceStubVtbl * _pbagex_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IPropertyBagExStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATPROPBAGStubVtbl,
    0
};

PCInterfaceName const _pbagex_InterfaceNamesList[] = 
{
    "IPropertyBagEx",
    "IEnumSTATPROPBAG",
    0
};


#define _pbagex_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _pbagex, pIID, n)

int __stdcall _pbagex_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _pbagex, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _pbagex, 2, *pIndex )
    
}

const ExtendedProxyFileInfo pbagex_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _pbagex_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _pbagex_StubVtblList,
    (const PCInterfaceName * ) & _pbagex_InterfaceNamesList,
    0, // no delegation
    & _pbagex_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/


#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:31 2002
 */
/* Compiler settings for pbagex.idl:
    Oicf, W1, Zp8, env=Win64 (32b run,appending)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if defined(_M_IA64) || defined(_M_AMD64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "pbagex.h"

#define TYPE_FORMAT_STRING_SIZE   2465                              
#define PROC_FORMAT_STRING_SIZE   433                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

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


extern const MIDL_SERVER_INFO IPropertyBagEx_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPropertyBagEx_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATPROPBAG_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPBAG_ProxyInfo;


extern const EXPR_EVAL ExprEvalRoutines[];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure ReadMultiple */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 16 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 18 */	NdrFcShort( 0x44 ),	/* 68 */
/* 20 */	NdrFcShort( 0x45 ),	/* 69 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cprops */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgwszPropNames */

/* 32 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter rgvar */

/* 38 */	NdrFcShort( 0x1b ),	/* Flags:  must size, must free, in, out, */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	NdrFcShort( 0x91a ),	/* Type Offset=2330 */

	/* Parameter pErrorLog */

/* 44 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 46 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 48 */	NdrFcShort( 0x930 ),	/* Type Offset=2352 */

	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 52 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteMultiple */

/* 56 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x4 ),	/* 4 */
/* 64 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 66 */	NdrFcShort( 0x8 ),	/* 8 */
/* 68 */	NdrFcShort( 0x8 ),	/* 8 */
/* 70 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 72 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x45 ),	/* 69 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cprops */

/* 82 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 84 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 86 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgwszPropNames */

/* 88 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 90 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 92 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter rgvar */

/* 94 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 96 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 98 */	NdrFcShort( 0x91a ),	/* Type Offset=2330 */

	/* Return value */

/* 100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 102 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteMultiple */

/* 106 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x5 ),	/* 5 */
/* 114 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 116 */	NdrFcShort( 0x10 ),	/* 16 */
/* 118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 120 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 122 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x1 ),	/* 1 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cprops */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgwszPropNames */

/* 138 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 140 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 142 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter dwReserved */

/* 144 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 146 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 152 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Open */

/* 156 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x6 ),	/* 6 */
/* 164 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 166 */	NdrFcShort( 0x7c ),	/* 124 */
/* 168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 170 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 172 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 174 */	NdrFcShort( 0x1 ),	/* 1 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUnkOuter */

/* 182 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 184 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 186 */	NdrFcShort( 0x30a ),	/* Type Offset=778 */

	/* Parameter wszPropName */

/* 188 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 190 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 192 */	NdrFcShort( 0x944 ),	/* Type Offset=2372 */

	/* Parameter guidPropertyType */

/* 194 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 196 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 198 */	NdrFcShort( 0x286 ),	/* Type Offset=646 */

	/* Parameter dwFlags */

/* 200 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 202 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 206 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 208 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 210 */	NdrFcShort( 0x286 ),	/* Type Offset=646 */

	/* Parameter ppUnk */

/* 212 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 214 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 216 */	NdrFcShort( 0x94a ),	/* Type Offset=2378 */

	/* Return value */

/* 218 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 220 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Enum */

/* 224 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 226 */	NdrFcLong( 0x0 ),	/* 0 */
/* 230 */	NdrFcShort( 0x7 ),	/* 7 */
/* 232 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 238 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 240 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 248 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter wszPropNameMask */

/* 250 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 252 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 254 */	NdrFcShort( 0x944 ),	/* Type Offset=2372 */

	/* Parameter dwFlags */

/* 256 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 258 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenum */

/* 262 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 264 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 266 */	NdrFcShort( 0x956 ),	/* Type Offset=2390 */

	/* Return value */

/* 268 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 270 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 274 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 276 */	NdrFcLong( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x3 ),	/* 3 */
/* 282 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 286 */	NdrFcShort( 0x24 ),	/* 36 */
/* 288 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 290 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 292 */	NdrFcShort( 0x1 ),	/* 1 */
/* 294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 298 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 300 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 302 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 306 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 308 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 310 */	NdrFcShort( 0x986 ),	/* Type Offset=2438 */

	/* Parameter pceltFetched */

/* 312 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 314 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 318 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 320 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */

/* 324 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 326 */	NdrFcLong( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x4 ),	/* 4 */
/* 332 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 334 */	NdrFcShort( 0x8 ),	/* 8 */
/* 336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 338 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 340 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 350 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 352 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 356 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 358 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 360 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */

/* 362 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 364 */	NdrFcLong( 0x0 ),	/* 0 */
/* 368 */	NdrFcShort( 0x5 ),	/* 5 */
/* 370 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x8 ),	/* 8 */
/* 376 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 378 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 388 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 390 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 392 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 394 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 396 */	NdrFcLong( 0x0 ),	/* 0 */
/* 400 */	NdrFcShort( 0x6 ),	/* 6 */
/* 402 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 406 */	NdrFcShort( 0x8 ),	/* 8 */
/* 408 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 410 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 418 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 420 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 422 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 424 */	NdrFcShort( 0x956 ),	/* Type Offset=2390 */

	/* Return value */

/* 426 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 428 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 430 */	0x8,		/* FC_LONG */
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
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/*  4 */	NdrFcShort( 0x0 ),	/* 0 */
/*  6 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/*  8 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 10 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 12 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 16 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 18 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 20 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 22 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 24 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x7,		/* FC_USHORT */
/* 26 */	0x0,		/* Corr desc:  */
			0x59,		/* FC_CALLBACK */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */
/* 30 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 32 */	NdrFcShort( 0x2 ),	/* Offset= 2 (34) */
/* 34 */	NdrFcShort( 0x10 ),	/* 16 */
/* 36 */	NdrFcShort( 0x61 ),	/* 97 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x0 ),	/* Offset= 0 (42) */
/* 44 */	NdrFcLong( 0x1 ),	/* 1 */
/* 48 */	NdrFcShort( 0x0 ),	/* Offset= 0 (48) */
/* 50 */	NdrFcLong( 0x10 ),	/* 16 */
/* 54 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 56 */	NdrFcLong( 0x11 ),	/* 17 */
/* 60 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 62 */	NdrFcLong( 0x2 ),	/* 2 */
/* 66 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 68 */	NdrFcLong( 0x12 ),	/* 18 */
/* 72 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 74 */	NdrFcLong( 0x3 ),	/* 3 */
/* 78 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 80 */	NdrFcLong( 0x13 ),	/* 19 */
/* 84 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 86 */	NdrFcLong( 0x16 ),	/* 22 */
/* 90 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 92 */	NdrFcLong( 0x17 ),	/* 23 */
/* 96 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 98 */	NdrFcLong( 0xe ),	/* 14 */
/* 102 */	NdrFcShort( 0x208 ),	/* Offset= 520 (622) */
/* 104 */	NdrFcLong( 0x14 ),	/* 20 */
/* 108 */	NdrFcShort( 0x202 ),	/* Offset= 514 (622) */
/* 110 */	NdrFcLong( 0x15 ),	/* 21 */
/* 114 */	NdrFcShort( 0x1fc ),	/* Offset= 508 (622) */
/* 116 */	NdrFcLong( 0x4 ),	/* 4 */
/* 120 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 122 */	NdrFcLong( 0x5 ),	/* 5 */
/* 126 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 128 */	NdrFcLong( 0xb ),	/* 11 */
/* 132 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 134 */	NdrFcLong( 0xffff ),	/* 65535 */
/* 138 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 140 */	NdrFcLong( 0xa ),	/* 10 */
/* 144 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 146 */	NdrFcLong( 0x6 ),	/* 6 */
/* 150 */	NdrFcShort( 0x1d8 ),	/* Offset= 472 (622) */
/* 152 */	NdrFcLong( 0x7 ),	/* 7 */
/* 156 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 158 */	NdrFcLong( 0x40 ),	/* 64 */
/* 162 */	NdrFcShort( 0x1d2 ),	/* Offset= 466 (628) */
/* 164 */	NdrFcLong( 0x48 ),	/* 72 */
/* 168 */	NdrFcShort( 0x1d4 ),	/* Offset= 468 (636) */
/* 170 */	NdrFcLong( 0x47 ),	/* 71 */
/* 174 */	NdrFcShort( 0x1e4 ),	/* Offset= 484 (658) */
/* 176 */	NdrFcLong( 0x8 ),	/* 8 */
/* 180 */	NdrFcShort( 0x218 ),	/* Offset= 536 (716) */
/* 182 */	NdrFcLong( 0xfff ),	/* 4095 */
/* 186 */	NdrFcShort( 0x228 ),	/* Offset= 552 (738) */
/* 188 */	NdrFcLong( 0x41 ),	/* 65 */
/* 192 */	NdrFcShort( 0x232 ),	/* Offset= 562 (754) */
/* 194 */	NdrFcLong( 0x46 ),	/* 70 */
/* 198 */	NdrFcShort( 0x22c ),	/* Offset= 556 (754) */
/* 200 */	NdrFcLong( 0x1e ),	/* 30 */
/* 204 */	NdrFcShort( 0x236 ),	/* Offset= 566 (770) */
/* 206 */	NdrFcLong( 0x1f ),	/* 31 */
/* 210 */	NdrFcShort( 0x234 ),	/* Offset= 564 (774) */
/* 212 */	NdrFcLong( 0xd ),	/* 13 */
/* 216 */	NdrFcShort( 0x232 ),	/* Offset= 562 (778) */
/* 218 */	NdrFcLong( 0x9 ),	/* 9 */
/* 222 */	NdrFcShort( 0x23e ),	/* Offset= 574 (796) */
/* 224 */	NdrFcLong( 0x42 ),	/* 66 */
/* 228 */	NdrFcShort( 0x24a ),	/* Offset= 586 (814) */
/* 230 */	NdrFcLong( 0x44 ),	/* 68 */
/* 234 */	NdrFcShort( 0x244 ),	/* Offset= 580 (814) */
/* 236 */	NdrFcLong( 0x43 ),	/* 67 */
/* 240 */	NdrFcShort( 0x250 ),	/* Offset= 592 (832) */
/* 242 */	NdrFcLong( 0x45 ),	/* 69 */
/* 246 */	NdrFcShort( 0x24a ),	/* Offset= 586 (832) */
/* 248 */	NdrFcLong( 0x49 ),	/* 73 */
/* 252 */	NdrFcShort( 0x256 ),	/* Offset= 598 (850) */
/* 254 */	NdrFcLong( 0x2010 ),	/* 8208 */
/* 258 */	NdrFcShort( 0x5b8 ),	/* Offset= 1464 (1722) */
/* 260 */	NdrFcLong( 0x2011 ),	/* 8209 */
/* 264 */	NdrFcShort( 0x5b2 ),	/* Offset= 1458 (1722) */
/* 266 */	NdrFcLong( 0x2002 ),	/* 8194 */
/* 270 */	NdrFcShort( 0x5ac ),	/* Offset= 1452 (1722) */
/* 272 */	NdrFcLong( 0x2012 ),	/* 8210 */
/* 276 */	NdrFcShort( 0x5a6 ),	/* Offset= 1446 (1722) */
/* 278 */	NdrFcLong( 0x2003 ),	/* 8195 */
/* 282 */	NdrFcShort( 0x5a0 ),	/* Offset= 1440 (1722) */
/* 284 */	NdrFcLong( 0x2013 ),	/* 8211 */
/* 288 */	NdrFcShort( 0x59a ),	/* Offset= 1434 (1722) */
/* 290 */	NdrFcLong( 0x2016 ),	/* 8214 */
/* 294 */	NdrFcShort( 0x594 ),	/* Offset= 1428 (1722) */
/* 296 */	NdrFcLong( 0x2017 ),	/* 8215 */
/* 300 */	NdrFcShort( 0x58e ),	/* Offset= 1422 (1722) */
/* 302 */	NdrFcLong( 0x2004 ),	/* 8196 */
/* 306 */	NdrFcShort( 0x588 ),	/* Offset= 1416 (1722) */
/* 308 */	NdrFcLong( 0x2005 ),	/* 8197 */
/* 312 */	NdrFcShort( 0x582 ),	/* Offset= 1410 (1722) */
/* 314 */	NdrFcLong( 0x2006 ),	/* 8198 */
/* 318 */	NdrFcShort( 0x57c ),	/* Offset= 1404 (1722) */
/* 320 */	NdrFcLong( 0x2007 ),	/* 8199 */
/* 324 */	NdrFcShort( 0x576 ),	/* Offset= 1398 (1722) */
/* 326 */	NdrFcLong( 0x2008 ),	/* 8200 */
/* 330 */	NdrFcShort( 0x570 ),	/* Offset= 1392 (1722) */
/* 332 */	NdrFcLong( 0x200b ),	/* 8203 */
/* 336 */	NdrFcShort( 0x56a ),	/* Offset= 1386 (1722) */
/* 338 */	NdrFcLong( 0x200e ),	/* 8206 */
/* 342 */	NdrFcShort( 0x564 ),	/* Offset= 1380 (1722) */
/* 344 */	NdrFcLong( 0x2009 ),	/* 8201 */
/* 348 */	NdrFcShort( 0x55e ),	/* Offset= 1374 (1722) */
/* 350 */	NdrFcLong( 0x200d ),	/* 8205 */
/* 354 */	NdrFcShort( 0x558 ),	/* Offset= 1368 (1722) */
/* 356 */	NdrFcLong( 0x200a ),	/* 8202 */
/* 360 */	NdrFcShort( 0x552 ),	/* Offset= 1362 (1722) */
/* 362 */	NdrFcLong( 0x200c ),	/* 8204 */
/* 366 */	NdrFcShort( 0x54c ),	/* Offset= 1356 (1722) */
/* 368 */	NdrFcLong( 0x1010 ),	/* 4112 */
/* 372 */	NdrFcShort( 0x55c ),	/* Offset= 1372 (1744) */
/* 374 */	NdrFcLong( 0x1011 ),	/* 4113 */
/* 378 */	NdrFcShort( 0x566 ),	/* Offset= 1382 (1760) */
/* 380 */	NdrFcLong( 0x1002 ),	/* 4098 */
/* 384 */	NdrFcShort( 0x570 ),	/* Offset= 1392 (1776) */
/* 386 */	NdrFcLong( 0x1012 ),	/* 4114 */
/* 390 */	NdrFcShort( 0x57a ),	/* Offset= 1402 (1792) */
/* 392 */	NdrFcLong( 0x1003 ),	/* 4099 */
/* 396 */	NdrFcShort( 0x584 ),	/* Offset= 1412 (1808) */
/* 398 */	NdrFcLong( 0x1013 ),	/* 4115 */
/* 402 */	NdrFcShort( 0x58e ),	/* Offset= 1422 (1824) */
/* 404 */	NdrFcLong( 0x1014 ),	/* 4116 */
/* 408 */	NdrFcShort( 0x5a8 ),	/* Offset= 1448 (1856) */
/* 410 */	NdrFcLong( 0x1015 ),	/* 4117 */
/* 414 */	NdrFcShort( 0x5b2 ),	/* Offset= 1458 (1872) */
/* 416 */	NdrFcLong( 0x1004 ),	/* 4100 */
/* 420 */	NdrFcShort( 0x5c8 ),	/* Offset= 1480 (1900) */
/* 422 */	NdrFcLong( 0x1005 ),	/* 4101 */
/* 426 */	NdrFcShort( 0x5de ),	/* Offset= 1502 (1928) */
/* 428 */	NdrFcLong( 0x100b ),	/* 4107 */
/* 432 */	NdrFcShort( 0x5e8 ),	/* Offset= 1512 (1944) */
/* 434 */	NdrFcLong( 0x100a ),	/* 4106 */
/* 438 */	NdrFcShort( 0x5f2 ),	/* Offset= 1522 (1960) */
/* 440 */	NdrFcLong( 0x1006 ),	/* 4102 */
/* 444 */	NdrFcShort( 0x5fc ),	/* Offset= 1532 (1976) */
/* 446 */	NdrFcLong( 0x1007 ),	/* 4103 */
/* 450 */	NdrFcShort( 0x606 ),	/* Offset= 1542 (1992) */
/* 452 */	NdrFcLong( 0x1040 ),	/* 4160 */
/* 456 */	NdrFcShort( 0x620 ),	/* Offset= 1568 (2024) */
/* 458 */	NdrFcLong( 0x1048 ),	/* 4168 */
/* 462 */	NdrFcShort( 0x63a ),	/* Offset= 1594 (2056) */
/* 464 */	NdrFcLong( 0x1047 ),	/* 4167 */
/* 468 */	NdrFcShort( 0x65a ),	/* Offset= 1626 (2094) */
/* 470 */	NdrFcLong( 0x1008 ),	/* 4104 */
/* 474 */	NdrFcShort( 0x67a ),	/* Offset= 1658 (2132) */
/* 476 */	NdrFcLong( 0x1fff ),	/* 8191 */
/* 480 */	NdrFcShort( 0x69a ),	/* Offset= 1690 (2170) */
/* 482 */	NdrFcLong( 0x101e ),	/* 4126 */
/* 486 */	NdrFcShort( 0x6ba ),	/* Offset= 1722 (2208) */
/* 488 */	NdrFcLong( 0x101f ),	/* 4127 */
/* 492 */	NdrFcShort( 0x6da ),	/* Offset= 1754 (2246) */
/* 494 */	NdrFcLong( 0x100c ),	/* 4108 */
/* 498 */	NdrFcShort( 0x6fa ),	/* Offset= 1786 (2284) */
/* 500 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 504 */	NdrFcShort( 0x3c4 ),	/* Offset= 964 (1468) */
/* 506 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 510 */	NdrFcShort( 0x3be ),	/* Offset= 958 (1468) */
/* 512 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 516 */	NdrFcShort( 0x376 ),	/* Offset= 886 (1402) */
/* 518 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 522 */	NdrFcShort( 0x370 ),	/* Offset= 880 (1402) */
/* 524 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 528 */	NdrFcShort( 0x36e ),	/* Offset= 878 (1406) */
/* 530 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 534 */	NdrFcShort( 0x368 ),	/* Offset= 872 (1406) */
/* 536 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 540 */	NdrFcShort( 0x362 ),	/* Offset= 866 (1406) */
/* 542 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 546 */	NdrFcShort( 0x35c ),	/* Offset= 860 (1406) */
/* 548 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 552 */	NdrFcShort( 0x35a ),	/* Offset= 858 (1410) */
/* 554 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 558 */	NdrFcShort( 0x358 ),	/* Offset= 856 (1414) */
/* 560 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 564 */	NdrFcShort( 0x346 ),	/* Offset= 838 (1402) */
/* 566 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 570 */	NdrFcShort( 0x37e ),	/* Offset= 894 (1464) */
/* 572 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 576 */	NdrFcShort( 0x33e ),	/* Offset= 830 (1406) */
/* 578 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 582 */	NdrFcShort( 0x344 ),	/* Offset= 836 (1418) */
/* 584 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 588 */	NdrFcShort( 0x33a ),	/* Offset= 826 (1414) */
/* 590 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 594 */	NdrFcShort( 0x6aa ),	/* Offset= 1706 (2300) */
/* 596 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 600 */	NdrFcShort( 0x33a ),	/* Offset= 826 (1426) */
/* 602 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 606 */	NdrFcShort( 0x338 ),	/* Offset= 824 (1430) */
/* 608 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 612 */	NdrFcShort( 0x69c ),	/* Offset= 1692 (2304) */
/* 614 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 618 */	NdrFcShort( 0x69a ),	/* Offset= 1690 (2308) */
/* 620 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (619) */
/* 622 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 624 */	NdrFcShort( 0x8 ),	/* 8 */
/* 626 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 628 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 630 */	NdrFcShort( 0x8 ),	/* 8 */
/* 632 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 634 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 636 */	
			0x13, 0x0,	/* FC_OP */
/* 638 */	NdrFcShort( 0x8 ),	/* Offset= 8 (646) */
/* 640 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 644 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 646 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 648 */	NdrFcShort( 0x10 ),	/* 16 */
/* 650 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 652 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 654 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (640) */
			0x5b,		/* FC_END */
/* 658 */	
			0x13, 0x0,	/* FC_OP */
/* 660 */	NdrFcShort( 0xe ),	/* Offset= 14 (674) */
/* 662 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 664 */	NdrFcShort( 0x1 ),	/* 1 */
/* 666 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 668 */	NdrFcShort( 0x1 ),	/* 1 */
/* 670 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 672 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 674 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 676 */	NdrFcShort( 0x10 ),	/* 16 */
/* 678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 680 */	NdrFcShort( 0x6 ),	/* Offset= 6 (686) */
/* 682 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 684 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 686 */	
			0x13, 0x0,	/* FC_OP */
/* 688 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (662) */
/* 690 */	
			0x13, 0x0,	/* FC_OP */
/* 692 */	NdrFcShort( 0xe ),	/* Offset= 14 (706) */
/* 694 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 696 */	NdrFcShort( 0x2 ),	/* 2 */
/* 698 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 700 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 702 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 704 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 706 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 708 */	NdrFcShort( 0x8 ),	/* 8 */
/* 710 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (694) */
/* 712 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 714 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 716 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x8 ),	/* 8 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (690) */
/* 726 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 728 */	NdrFcShort( 0x1 ),	/* 1 */
/* 730 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 734 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 736 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 738 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 740 */	NdrFcShort( 0x10 ),	/* 16 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x6 ),	/* Offset= 6 (750) */
/* 746 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 748 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 750 */	
			0x13, 0x0,	/* FC_OP */
/* 752 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (726) */
/* 754 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 756 */	NdrFcShort( 0x10 ),	/* 16 */
/* 758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 760 */	NdrFcShort( 0x6 ),	/* Offset= 6 (766) */
/* 762 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 764 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 766 */	
			0x13, 0x0,	/* FC_OP */
/* 768 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (726) */
/* 770 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 772 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 774 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 776 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 778 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 780 */	NdrFcLong( 0x0 ),	/* 0 */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 790 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 792 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 794 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 796 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 798 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 806 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 808 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 810 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 812 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 814 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 816 */	NdrFcLong( 0xc ),	/* 12 */
/* 820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 824 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 826 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 828 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 830 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 832 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 834 */	NdrFcLong( 0xb ),	/* 11 */
/* 838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 842 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 844 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 846 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 848 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 850 */	
			0x13, 0x0,	/* FC_OP */
/* 852 */	NdrFcShort( 0x2 ),	/* Offset= 2 (854) */
/* 854 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 856 */	NdrFcShort( 0x18 ),	/* 24 */
/* 858 */	NdrFcShort( 0x0 ),	/* 0 */
/* 860 */	NdrFcShort( 0xc ),	/* Offset= 12 (872) */
/* 862 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 864 */	NdrFcShort( 0xffffff26 ),	/* Offset= -218 (646) */
/* 866 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 868 */	NdrFcShort( 0xffffffca ),	/* Offset= -54 (814) */
/* 870 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 872 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 874 */	NdrFcShort( 0x2 ),	/* Offset= 2 (876) */
/* 876 */	
			0x13, 0x0,	/* FC_OP */
/* 878 */	NdrFcShort( 0x33a ),	/* Offset= 826 (1704) */
/* 880 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 882 */	NdrFcShort( 0x20 ),	/* 32 */
/* 884 */	NdrFcShort( 0xa ),	/* 10 */
/* 886 */	NdrFcLong( 0x8 ),	/* 8 */
/* 890 */	NdrFcShort( 0x50 ),	/* Offset= 80 (970) */
/* 892 */	NdrFcLong( 0xd ),	/* 13 */
/* 896 */	NdrFcShort( 0x70 ),	/* Offset= 112 (1008) */
/* 898 */	NdrFcLong( 0x9 ),	/* 9 */
/* 902 */	NdrFcShort( 0x90 ),	/* Offset= 144 (1046) */
/* 904 */	NdrFcLong( 0xc ),	/* 12 */
/* 908 */	NdrFcShort( 0x25e ),	/* Offset= 606 (1514) */
/* 910 */	NdrFcLong( 0x24 ),	/* 36 */
/* 914 */	NdrFcShort( 0x27e ),	/* Offset= 638 (1552) */
/* 916 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 920 */	NdrFcShort( 0x288 ),	/* Offset= 648 (1568) */
/* 922 */	NdrFcLong( 0x10 ),	/* 16 */
/* 926 */	NdrFcShort( 0x296 ),	/* Offset= 662 (1588) */
/* 928 */	NdrFcLong( 0x2 ),	/* 2 */
/* 932 */	NdrFcShort( 0x2ac ),	/* Offset= 684 (1616) */
/* 934 */	NdrFcLong( 0x3 ),	/* 3 */
/* 938 */	NdrFcShort( 0x2c2 ),	/* Offset= 706 (1644) */
/* 940 */	NdrFcLong( 0x14 ),	/* 20 */
/* 944 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (1672) */
/* 946 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (945) */
/* 948 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 952 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 958 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 962 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 964 */	
			0x13, 0x0,	/* FC_OP */
/* 966 */	NdrFcShort( 0xfffffefc ),	/* Offset= -260 (706) */
/* 968 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 970 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 972 */	NdrFcShort( 0x10 ),	/* 16 */
/* 974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 976 */	NdrFcShort( 0x6 ),	/* Offset= 6 (982) */
/* 978 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 980 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 982 */	
			0x11, 0x0,	/* FC_RP */
/* 984 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (948) */
/* 986 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 988 */	NdrFcShort( 0x0 ),	/* 0 */
/* 990 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 994 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 996 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1000 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1002 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1004 */	NdrFcShort( 0xffffff1e ),	/* Offset= -226 (778) */
/* 1006 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1008 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1010 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1014 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1020) */
/* 1016 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1018 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1020 */	
			0x11, 0x0,	/* FC_RP */
/* 1022 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (986) */
/* 1024 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1028 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1034 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1038 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1040 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1042 */	NdrFcShort( 0xffffff0a ),	/* Offset= -246 (796) */
/* 1044 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1046 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1048 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1050 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1052 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1058) */
/* 1054 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1056 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1058 */	
			0x11, 0x0,	/* FC_RP */
/* 1060 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1024) */
/* 1062 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1064 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1066 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 1068 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1070 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1072) */
/* 1072 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1074 */	NdrFcShort( 0x2b ),	/* 43 */
/* 1076 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1080 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1082 */	NdrFcLong( 0x11 ),	/* 17 */
/* 1086 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 1088 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1092 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1094 */	NdrFcLong( 0x4 ),	/* 4 */
/* 1098 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 1100 */	NdrFcLong( 0x5 ),	/* 5 */
/* 1104 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 1106 */	NdrFcLong( 0xb ),	/* 11 */
/* 1110 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1112 */	NdrFcLong( 0xa ),	/* 10 */
/* 1116 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1118 */	NdrFcLong( 0x6 ),	/* 6 */
/* 1122 */	NdrFcShort( 0xfffffe0c ),	/* Offset= -500 (622) */
/* 1124 */	NdrFcLong( 0x7 ),	/* 7 */
/* 1128 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 1130 */	NdrFcLong( 0x8 ),	/* 8 */
/* 1134 */	NdrFcShort( 0xfffffe44 ),	/* Offset= -444 (690) */
/* 1136 */	NdrFcLong( 0xd ),	/* 13 */
/* 1140 */	NdrFcShort( 0xfffffe96 ),	/* Offset= -362 (778) */
/* 1142 */	NdrFcLong( 0x9 ),	/* 9 */
/* 1146 */	NdrFcShort( 0xfffffea2 ),	/* Offset= -350 (796) */
/* 1148 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 1152 */	NdrFcShort( 0xb8 ),	/* Offset= 184 (1336) */
/* 1154 */	NdrFcLong( 0x24 ),	/* 36 */
/* 1158 */	NdrFcShort( 0xba ),	/* Offset= 186 (1344) */
/* 1160 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 1164 */	NdrFcShort( 0xb4 ),	/* Offset= 180 (1344) */
/* 1166 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 1170 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (1398) */
/* 1172 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 1176 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (1402) */
/* 1178 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 1182 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (1406) */
/* 1184 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 1188 */	NdrFcShort( 0xde ),	/* Offset= 222 (1410) */
/* 1190 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 1194 */	NdrFcShort( 0xdc ),	/* Offset= 220 (1414) */
/* 1196 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 1200 */	NdrFcShort( 0xca ),	/* Offset= 202 (1402) */
/* 1202 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 1206 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (1406) */
/* 1208 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 1212 */	NdrFcShort( 0xce ),	/* Offset= 206 (1418) */
/* 1214 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 1218 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1414) */
/* 1220 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 1224 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (1422) */
/* 1226 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 1230 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1426) */
/* 1232 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 1236 */	NdrFcShort( 0xc2 ),	/* Offset= 194 (1430) */
/* 1238 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 1242 */	NdrFcShort( 0xc0 ),	/* Offset= 192 (1434) */
/* 1244 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 1248 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (1446) */
/* 1250 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1254 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 1256 */	NdrFcLong( 0x12 ),	/* 18 */
/* 1260 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1262 */	NdrFcLong( 0x13 ),	/* 19 */
/* 1266 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1268 */	NdrFcLong( 0x16 ),	/* 22 */
/* 1272 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1274 */	NdrFcLong( 0x17 ),	/* 23 */
/* 1278 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1280 */	NdrFcLong( 0xe ),	/* 14 */
/* 1284 */	NdrFcShort( 0xaa ),	/* Offset= 170 (1454) */
/* 1286 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 1290 */	NdrFcShort( 0xae ),	/* Offset= 174 (1464) */
/* 1292 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 1296 */	NdrFcShort( 0xac ),	/* Offset= 172 (1468) */
/* 1298 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 1302 */	NdrFcShort( 0x64 ),	/* Offset= 100 (1402) */
/* 1304 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 1308 */	NdrFcShort( 0x62 ),	/* Offset= 98 (1406) */
/* 1310 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 1314 */	NdrFcShort( 0x5c ),	/* Offset= 92 (1406) */
/* 1316 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 1320 */	NdrFcShort( 0x56 ),	/* Offset= 86 (1406) */
/* 1322 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1326 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1326) */
/* 1328 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1332 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1332) */
/* 1334 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1333) */
/* 1336 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1338 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1340) */
/* 1340 */	
			0x13, 0x0,	/* FC_OP */
/* 1342 */	NdrFcShort( 0x16a ),	/* Offset= 362 (1704) */
/* 1344 */	
			0x13, 0x0,	/* FC_OP */
/* 1346 */	NdrFcShort( 0x20 ),	/* Offset= 32 (1378) */
/* 1348 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1350 */	NdrFcLong( 0x2f ),	/* 47 */
/* 1354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1358 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1360 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1362 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1364 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1366 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1368 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1370 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1372 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1374 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1376 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1378 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1380 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1384 */	NdrFcShort( 0xa ),	/* Offset= 10 (1394) */
/* 1386 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1388 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1390 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (1348) */
/* 1392 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1394 */	
			0x13, 0x0,	/* FC_OP */
/* 1396 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1366) */
/* 1398 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1400 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1402 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1404 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1406 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1408 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1410 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1412 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1414 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1416 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1418 */	
			0x13, 0x0,	/* FC_OP */
/* 1420 */	NdrFcShort( 0xfffffce2 ),	/* Offset= -798 (622) */
/* 1422 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1424 */	NdrFcShort( 0xfffffd22 ),	/* Offset= -734 (690) */
/* 1426 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1428 */	NdrFcShort( 0xfffffd76 ),	/* Offset= -650 (778) */
/* 1430 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1432 */	NdrFcShort( 0xfffffd84 ),	/* Offset= -636 (796) */
/* 1434 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1436 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1438) */
/* 1438 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1440 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1442) */
/* 1442 */	
			0x13, 0x0,	/* FC_OP */
/* 1444 */	NdrFcShort( 0x104 ),	/* Offset= 260 (1704) */
/* 1446 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1448 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1450) */
/* 1450 */	
			0x13, 0x0,	/* FC_OP */
/* 1452 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1472) */
/* 1454 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1456 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1458 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1460 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1462 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1464 */	
			0x13, 0x0,	/* FC_OP */
/* 1466 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1454) */
/* 1468 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1470 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1472 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1474 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1478 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1478) */
/* 1480 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1482 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1484 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1486 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1488 */	NdrFcShort( 0xfffffe56 ),	/* Offset= -426 (1062) */
/* 1490 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1492 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1496 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1500 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1502 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1506 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1508 */	
			0x13, 0x0,	/* FC_OP */
/* 1510 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (1472) */
/* 1512 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1514 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1516 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1518 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1520 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1526) */
/* 1522 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1524 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1526 */	
			0x11, 0x0,	/* FC_RP */
/* 1528 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1492) */
/* 1530 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1534 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1538 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1540 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1544 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1546 */	
			0x13, 0x0,	/* FC_OP */
/* 1548 */	NdrFcShort( 0xffffff56 ),	/* Offset= -170 (1378) */
/* 1550 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1552 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1554 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1558 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1564) */
/* 1560 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1562 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1564 */	
			0x11, 0x0,	/* FC_RP */
/* 1566 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1530) */
/* 1568 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1570 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1574 */	NdrFcShort( 0xa ),	/* Offset= 10 (1584) */
/* 1576 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1578 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1580 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffc59 ),	/* Offset= -935 (646) */
			0x5b,		/* FC_END */
/* 1584 */	
			0x11, 0x0,	/* FC_RP */
/* 1586 */	NdrFcShort( 0xfffffda8 ),	/* Offset= -600 (986) */
/* 1588 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1590 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1594 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1600) */
/* 1596 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1598 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1600 */	
			0x13, 0x0,	/* FC_OP */
/* 1602 */	NdrFcShort( 0xfffffc94 ),	/* Offset= -876 (726) */
/* 1604 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1606 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1608 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1612 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1614 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1616 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1618 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1622 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1628) */
/* 1624 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1626 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1628 */	
			0x13, 0x0,	/* FC_OP */
/* 1630 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1604) */
/* 1632 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1634 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1636 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1640 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1642 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1644 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1646 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1650 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1656) */
/* 1652 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1654 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1656 */	
			0x13, 0x0,	/* FC_OP */
/* 1658 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1632) */
/* 1660 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1662 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1664 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1668 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1670 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1672 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1674 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1678 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1684) */
/* 1680 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1682 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1684 */	
			0x13, 0x0,	/* FC_OP */
/* 1686 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1660) */
/* 1688 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1692 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1694 */	NdrFcShort( 0xffc8 ),	/* -56 */
/* 1696 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1698 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1700 */	NdrFcShort( 0xfffffbd0 ),	/* Offset= -1072 (628) */
/* 1702 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1704 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1706 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1708 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (1688) */
/* 1710 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1710) */
/* 1712 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1714 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1716 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1718 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffcb9 ),	/* Offset= -839 (880) */
			0x5b,		/* FC_END */
/* 1722 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1724 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1726 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1728 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1730 */	NdrFcShort( 0xfffffca6 ),	/* Offset= -858 (872) */
/* 1732 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1734 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1736 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1740 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1742 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1744 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1746 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1750 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1756) */
/* 1752 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1754 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1756 */	
			0x13, 0x0,	/* FC_OP */
/* 1758 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1732) */
/* 1760 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1762 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1766 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1772) */
/* 1768 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1770 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1772 */	
			0x13, 0x0,	/* FC_OP */
/* 1774 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (1732) */
/* 1776 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1778 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1782 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1788) */
/* 1784 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1786 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1788 */	
			0x13, 0x0,	/* FC_OP */
/* 1790 */	NdrFcShort( 0xffffff46 ),	/* Offset= -186 (1604) */
/* 1792 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1794 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1798 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1804) */
/* 1800 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1802 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1804 */	
			0x13, 0x0,	/* FC_OP */
/* 1806 */	NdrFcShort( 0xffffff36 ),	/* Offset= -202 (1604) */
/* 1808 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1810 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1814 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1820) */
/* 1816 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1818 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1820 */	
			0x13, 0x0,	/* FC_OP */
/* 1822 */	NdrFcShort( 0xffffff42 ),	/* Offset= -190 (1632) */
/* 1824 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1826 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1830 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1836) */
/* 1832 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1834 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1836 */	
			0x13, 0x0,	/* FC_OP */
/* 1838 */	NdrFcShort( 0xffffff32 ),	/* Offset= -206 (1632) */
/* 1840 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1844 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1848 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1850 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1852 */	NdrFcShort( 0xfffffb32 ),	/* Offset= -1230 (622) */
/* 1854 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1856 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1858 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1862 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1868) */
/* 1864 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1866 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1868 */	
			0x13, 0x0,	/* FC_OP */
/* 1870 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1840) */
/* 1872 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1874 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1878 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1884) */
/* 1880 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1882 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1884 */	
			0x13, 0x0,	/* FC_OP */
/* 1886 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (1840) */
/* 1888 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1890 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1892 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1894 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1896 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1898 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 1900 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1902 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1906 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1912) */
/* 1908 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1910 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1912 */	
			0x13, 0x0,	/* FC_OP */
/* 1914 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1888) */
/* 1916 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1918 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1920 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1924 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1926 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 1928 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1930 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1932 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1934 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1940) */
/* 1936 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1938 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1940 */	
			0x13, 0x0,	/* FC_OP */
/* 1942 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1916) */
/* 1944 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1946 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1950 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1956) */
/* 1952 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1954 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1956 */	
			0x13, 0x0,	/* FC_OP */
/* 1958 */	NdrFcShort( 0xfffffe9e ),	/* Offset= -354 (1604) */
/* 1960 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1962 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1964 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1966 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1972) */
/* 1968 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1970 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1972 */	
			0x13, 0x0,	/* FC_OP */
/* 1974 */	NdrFcShort( 0xfffffeaa ),	/* Offset= -342 (1632) */
/* 1976 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1978 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1982 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1988) */
/* 1984 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1986 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1988 */	
			0x13, 0x0,	/* FC_OP */
/* 1990 */	NdrFcShort( 0xffffff6a ),	/* Offset= -150 (1840) */
/* 1992 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1994 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1998 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2004) */
/* 2000 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2002 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2004 */	
			0x13, 0x0,	/* FC_OP */
/* 2006 */	NdrFcShort( 0xffffffa6 ),	/* Offset= -90 (1916) */
/* 2008 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2010 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2012 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2014 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2016 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2018 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2020 */	NdrFcShort( 0xfffffa90 ),	/* Offset= -1392 (628) */
/* 2022 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2024 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2026 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2030 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2036) */
/* 2032 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2034 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2036 */	
			0x13, 0x0,	/* FC_OP */
/* 2038 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (2008) */
/* 2040 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2042 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2044 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2048 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2050 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2052 */	NdrFcShort( 0xfffffa82 ),	/* Offset= -1406 (646) */
/* 2054 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2056 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2058 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2062 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2068) */
/* 2064 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2066 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2068 */	
			0x13, 0x0,	/* FC_OP */
/* 2070 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (2040) */
/* 2072 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2076 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2080 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2082 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2086 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2088 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2090 */	NdrFcShort( 0xfffffa78 ),	/* Offset= -1416 (674) */
/* 2092 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2094 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2096 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2100 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2106) */
/* 2102 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2104 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2106 */	
			0x13, 0x0,	/* FC_OP */
/* 2108 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2072) */
/* 2110 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2114 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2118 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2120 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2124 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2126 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2128 */	NdrFcShort( 0xfffffa7c ),	/* Offset= -1412 (716) */
/* 2130 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2132 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2134 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2138 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2144) */
/* 2140 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2142 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2144 */	
			0x13, 0x0,	/* FC_OP */
/* 2146 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2110) */
/* 2148 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2152 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2156 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2158 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2162 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2164 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2166 */	NdrFcShort( 0xfffffa6c ),	/* Offset= -1428 (738) */
/* 2168 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2170 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2172 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2176 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2182) */
/* 2178 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2180 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2182 */	
			0x13, 0x0,	/* FC_OP */
/* 2184 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2148) */
/* 2186 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2190 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2194 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2196 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2200 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2202 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2204 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 2206 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2208 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2210 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2214 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2220) */
/* 2216 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2218 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2220 */	
			0x13, 0x0,	/* FC_OP */
/* 2222 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2186) */
/* 2224 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2228 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2232 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2234 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2238 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2240 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2242 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2244 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2246 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2248 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2252 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2258) */
/* 2254 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2256 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2258 */	
			0x13, 0x0,	/* FC_OP */
/* 2260 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2224) */
/* 2262 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 2264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2266 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2270 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2272 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2276 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2278 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2280 */	NdrFcShort( 0x20 ),	/* Offset= 32 (2312) */
/* 2282 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2284 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2286 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2290 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2296) */
/* 2292 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2294 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2296 */	
			0x13, 0x0,	/* FC_OP */
/* 2298 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2262) */
/* 2300 */	
			0x13, 0x0,	/* FC_OP */
/* 2302 */	NdrFcShort( 0xfffff9ce ),	/* Offset= -1586 (716) */
/* 2304 */	
			0x13, 0x0,	/* FC_OP */
/* 2306 */	NdrFcShort( 0xfffffdb8 ),	/* Offset= -584 (1722) */
/* 2308 */	
			0x13, 0x0,	/* FC_OP */
/* 2310 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2312) */
/* 2312 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 2314 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2318 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2318) */
/* 2320 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 2322 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 2324 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2326 */	NdrFcShort( 0xfffff702 ),	/* Offset= -2302 (24) */
/* 2328 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2330 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 2332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2334 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2336 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2338 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2340 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2344 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2346 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2348 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2312) */
/* 2350 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2352 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2354 */	NdrFcLong( 0x3127ca40 ),	/* 824691264 */
/* 2358 */	NdrFcShort( 0x446e ),	/* 17518 */
/* 2360 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 2362 */	0x81,		/* 129 */
			0x35,		/* 53 */
/* 2364 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 2366 */	0x0,		/* 0 */
			0x4b,		/* 75 */
/* 2368 */	0xb8,		/* 184 */
			0x51,		/* 81 */
/* 2370 */	0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 2372 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2374 */	
			0x11, 0x0,	/* FC_RP */
/* 2376 */	NdrFcShort( 0xfffff93e ),	/* Offset= -1730 (646) */
/* 2378 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2380 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2382) */
/* 2382 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 2384 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 2386 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2388 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 2390 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2392 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2394) */
/* 2394 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2396 */	NdrFcLong( 0x20021801 ),	/* 537008129 */
/* 2400 */	NdrFcShort( 0x5de6 ),	/* 24038 */
/* 2402 */	NdrFcShort( 0x11d1 ),	/* 4561 */
/* 2404 */	0x8e,		/* 142 */
			0x38,		/* 56 */
/* 2406 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 2408 */	0x4f,		/* 79 */
			0xb9,		/* 185 */
/* 2410 */	0x38,		/* 56 */
			0x6d,		/* 109 */
/* 2412 */	
			0x11, 0x0,	/* FC_RP */
/* 2414 */	NdrFcShort( 0x18 ),	/* Offset= 24 (2438) */
/* 2416 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2418 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2422 */	NdrFcShort( 0xc ),	/* Offset= 12 (2434) */
/* 2424 */	0x36,		/* FC_POINTER */
			0x6,		/* FC_SHORT */
/* 2426 */	0x3e,		/* FC_STRUCTPAD2 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2428 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff909 ),	/* Offset= -1783 (646) */
			0x40,		/* FC_STRUCTPAD4 */
/* 2432 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2434 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2436 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2438 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2442 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2444 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2446 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2448 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 2450 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2452 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2454 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2456 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (2416) */
/* 2458 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2460 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 2462 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

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
            },
            {
            LPSAFEARRAY_UserSize
            ,LPSAFEARRAY_UserMarshal
            ,LPSAFEARRAY_UserUnmarshal
            ,LPSAFEARRAY_UserFree
            }

        };


static void __RPC_USER IPropertyBagEx_PROPVARIANTExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    PROPVARIANT *pS	=	( PROPVARIANT * )(pStubMsg->StackTop - 8);
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = (ULONG_PTR) ( ( unsigned short  )pS->vt );
}

static void __RPC_USER IPropertyBagEx_CLIPDATAExprEval_0001( PMIDL_STUB_MESSAGE pStubMsg )
{
    CLIPDATA *pS	=	( CLIPDATA * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = (ULONG_PTR) ( pS->cbSize - 4 );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IPropertyBagEx_PROPVARIANTExprEval_0000
    ,IPropertyBagEx_CLIPDATAExprEval_0001
    };



/* Standard interface: __MIDL_itf_pbagex_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IPropertyBagEx, ver. 0.0,
   GUID={0x20011801,0x5DE6,0x11D1,{0x8E,0x38,0x00,0xC0,0x4F,0xB9,0x38,0x6D}} */

#pragma code_seg(".orpc")
static const unsigned short IPropertyBagEx_FormatStringOffsetTable[] =
    {
    0,
    56,
    106,
    156,
    224
    };

static const MIDL_STUBLESS_PROXY_INFO IPropertyBagEx_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPropertyBagEx_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPropertyBagEx_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPropertyBagEx_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IPropertyBagExProxyVtbl = 
{
    &IPropertyBagEx_ProxyInfo,
    &IID_IPropertyBagEx,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::ReadMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::WriteMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::DeleteMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::Open */ ,
    (void *) (INT_PTR) -1 /* IPropertyBagEx::Enum */
};

const CInterfaceStubVtbl _IPropertyBagExStubVtbl =
{
    &IID_IPropertyBagEx,
    &IPropertyBagEx_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATPROPBAG, ver. 0.0,
   GUID={0x20021801,0x5DE6,0x11D1,{0x8E,0x38,0x00,0xC0,0x4F,0xB9,0x38,0x6D}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATPROPBAG_FormatStringOffsetTable[] =
    {
    274,
    324,
    362,
    394
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPBAG_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPBAG_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumSTATPROPBAG_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPBAG_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATPROPBAGProxyVtbl = 
{
    &IEnumSTATPROPBAG_ProxyInfo,
    &IID_IEnumSTATPROPBAG,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPBAG::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPBAG::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPBAG::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPBAG::Clone */
};

const CInterfaceStubVtbl _IEnumSTATPROPBAGStubVtbl =
{
    &IID_IEnumSTATPROPBAG,
    &IEnumSTATPROPBAG_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_pbagex_0112, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    ExprEvalRoutines,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
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

const CInterfaceProxyVtbl * _pbagex_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IPropertyBagExProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATPROPBAGProxyVtbl,
    0
};

const CInterfaceStubVtbl * _pbagex_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IPropertyBagExStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATPROPBAGStubVtbl,
    0
};

PCInterfaceName const _pbagex_InterfaceNamesList[] = 
{
    "IPropertyBagEx",
    "IEnumSTATPROPBAG",
    0
};


#define _pbagex_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _pbagex, pIID, n)

int __stdcall _pbagex_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _pbagex, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _pbagex, 2, *pIndex )
    
}

const ExtendedProxyFileInfo pbagex_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _pbagex_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _pbagex_StubVtblList,
    (const PCInterfaceName * ) & _pbagex_InterfaceNamesList,
    0, // no delegation
    & _pbagex_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

