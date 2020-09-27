
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:13:57 2002
 */
/* Compiler settings for enc.idl:
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


#include "enc.h"

#define TYPE_FORMAT_STRING_SIZE   1325                              
#define PROC_FORMAT_STRING_SIZE   4127                              
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


extern const MIDL_SERVER_INFO IDebugENC_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENC_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCLineMap_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCLineMap_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugENCInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugENCInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCRelinkInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCRelinkInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugENCRelinkInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugENCRelinkInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugIDBInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugIDBInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugIDBInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugIDBInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCBuildInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCBuildInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCUpdateOnRelinkEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCUpdateOnRelinkEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCUpdateOnStaleCodeEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCUpdateOnStaleCodeEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCUpdate_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCUpdate_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCSnapshot2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCSnapshot2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugENCSnapshots2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugENCSnapshots2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugErrorInfos2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugErrorInfos2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugComPlusSnapshot2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugComPlusSnapshot2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugNativeSnapshot2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugNativeSnapshot2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCStackFrame2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCStackFrame2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMetaDataEmit2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMetaDataEmit2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMetaDataDebugEmit2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMetaDataDebugEmit2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugENCStateEvents_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugENCStateEvents_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute, more than 32 methods in the interface.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure HasDependentTargets */


	/* Procedure SetRegistryRoot */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszSourcePath */


	/* Parameter in_szRegistryRoot */

/* 16 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Return value */


	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnterDebuggingSession */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x4 ),	/* 4 */
/* 36 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x8 ),	/* 8 */
/* 42 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter in_pServiceProvider */

/* 44 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 46 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 48 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 52 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetENCProjectBuildOption */

/* 56 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x5 ),	/* 5 */
/* 64 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 66 */	NdrFcShort( 0x44 ),	/* 68 */
/* 68 */	NdrFcShort( 0x8 ),	/* 8 */
/* 70 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter in_guidOption */

/* 72 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter in_szOptionValue */

/* 78 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Return value */

/* 84 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 86 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 88 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InquireENCState */

/* 90 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 92 */	NdrFcLong( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x6 ),	/* 6 */
/* 98 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 100 */	NdrFcShort( 0x22 ),	/* 34 */
/* 102 */	NdrFcShort( 0x8 ),	/* 8 */
/* 104 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter in_pENCSTATE */

/* 106 */	NdrFcShort( 0x2008 ),	/* Flags:  in, srv alloc size=8 */
/* 108 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 110 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Parameter fOnContinue */

/* 112 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 114 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 118 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 120 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 122 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InquireENCRelinkState */

/* 124 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 126 */	NdrFcLong( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x7 ),	/* 7 */
/* 132 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 134 */	NdrFcShort( 0x1c ),	/* 28 */
/* 136 */	NdrFcShort( 0x8 ),	/* 8 */
/* 138 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter in_pbENCRelinking */

/* 140 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 142 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 146 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 148 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MapToEdited */

/* 152 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x8 ),	/* 8 */
/* 160 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 162 */	NdrFcShort( 0x10 ),	/* 16 */
/* 164 */	NdrFcShort( 0x40 ),	/* 64 */
/* 166 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter in_szFile */

/* 168 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 172 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter in_LineNo */

/* 174 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter in_ColumnNo */

/* 180 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_pLineNo */

/* 186 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 188 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_pColumnNo */

/* 192 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 194 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 196 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 198 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 200 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 202 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MapToSuperceded */

/* 204 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 206 */	NdrFcLong( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x9 ),	/* 9 */
/* 212 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 214 */	NdrFcShort( 0x10 ),	/* 16 */
/* 216 */	NdrFcShort( 0x40 ),	/* 64 */
/* 218 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter in_szFile */

/* 220 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 222 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 224 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter in_LineNo */

/* 226 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 228 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter in_ColumnNo */

/* 232 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 234 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 236 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_pLineNo */

/* 238 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 240 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 242 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_pColumnNo */

/* 244 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 246 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 250 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 252 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ApplyCodeChanges */

/* 256 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 258 */	NdrFcLong( 0x0 ),	/* 0 */
/* 262 */	NdrFcShort( 0xa ),	/* 10 */
/* 264 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 266 */	NdrFcShort( 0x8 ),	/* 8 */
/* 268 */	NdrFcShort( 0x22 ),	/* 34 */
/* 270 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter in_pSession */

/* 272 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 274 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 276 */	NdrFcShort( 0x3a ),	/* Type Offset=58 */

	/* Parameter in_fOnContinue */

/* 278 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 280 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter result */

/* 284 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 286 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 288 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Return value */

/* 290 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 292 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CancelApplyCodeChanges */

/* 296 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0xb ),	/* 11 */
/* 304 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 310 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter in_pProgram */

/* 312 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 314 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 316 */	NdrFcShort( 0x4c ),	/* Type Offset=76 */

	/* Return value */

/* 318 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 320 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LeaveDebuggingSession */

/* 324 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 326 */	NdrFcLong( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0xc ),	/* 12 */
/* 332 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 338 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 340 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 342 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AdviseENCStateEvents */

/* 346 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 348 */	NdrFcLong( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0xd ),	/* 13 */
/* 354 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 358 */	NdrFcShort( 0x8 ),	/* 8 */
/* 360 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter in_pENCStateEvents */

/* 362 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 364 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 366 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */

	/* Return value */

/* 368 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 370 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnadviseENCStateEvents */

/* 374 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 376 */	NdrFcLong( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0xe ),	/* 14 */
/* 382 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0x8 ),	/* 8 */
/* 388 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter in_pENCStateEvents */

/* 390 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 392 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 394 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */

	/* Return value */

/* 396 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 398 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFileName */

/* 402 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 404 */	NdrFcLong( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0xf ),	/* 15 */
/* 410 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x8 ),	/* 8 */
/* 416 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter in_szURL */

/* 418 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 420 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 422 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter out_pbstrFileName */

/* 424 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 426 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 428 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Return value */

/* 430 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 432 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFileDisplayName */

/* 436 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 438 */	NdrFcLong( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x10 ),	/* 16 */
/* 444 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x8 ),	/* 8 */
/* 450 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter in_szURL */

/* 452 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 454 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 456 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter out_pbstrDisplayFileName */

/* 458 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 460 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 462 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Return value */

/* 464 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 466 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 468 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ClearENCState */

/* 470 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 472 */	NdrFcLong( 0x0 ),	/* 0 */
/* 476 */	NdrFcShort( 0x11 ),	/* 17 */
/* 478 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 482 */	NdrFcShort( 0x8 ),	/* 8 */
/* 484 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 486 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 488 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */


	/* Procedure GetEditedSource */

/* 492 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 494 */	NdrFcLong( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x3 ),	/* 3 */
/* 500 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 506 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_pbstrIDBFile */


	/* Parameter out_pbstrEditedSource */

/* 508 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 510 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 512 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Return value */


	/* Return value */

/* 514 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 516 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 518 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSupercededSource */

/* 520 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 522 */	NdrFcLong( 0x0 ),	/* 0 */
/* 526 */	NdrFcShort( 0x4 ),	/* 4 */
/* 528 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 532 */	NdrFcShort( 0x8 ),	/* 8 */
/* 534 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_pbstrSupercededSource */

/* 536 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 538 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 540 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Return value */

/* 542 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 544 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsLineModified */

/* 548 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 550 */	NdrFcLong( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x5 ),	/* 5 */
/* 556 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 558 */	NdrFcShort( 0x8 ),	/* 8 */
/* 560 */	NdrFcShort( 0x8 ),	/* 8 */
/* 562 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter in_LineNoFromSupercededSource */

/* 564 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 566 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 570 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 572 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 574 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSaveSize */


	/* Procedure LineMap */

/* 576 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 578 */	NdrFcLong( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x6 ),	/* 6 */
/* 584 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 586 */	NdrFcShort( 0x8 ),	/* 8 */
/* 588 */	NdrFcShort( 0x24 ),	/* 36 */
/* 590 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter fSave */


	/* Parameter in_LineNoFromSupercededSource */

/* 592 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 594 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwSaveSize */


	/* Parameter out_pLineNoFromEditedSource */

/* 598 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 600 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 602 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 604 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 606 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 608 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReverseLineMap */

/* 610 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 612 */	NdrFcLong( 0x0 ),	/* 0 */
/* 616 */	NdrFcShort( 0x7 ),	/* 7 */
/* 618 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 620 */	NdrFcShort( 0x8 ),	/* 8 */
/* 622 */	NdrFcShort( 0x24 ),	/* 36 */
/* 624 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter in_LineNoFromEditedSource */

/* 626 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 628 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_pLineNoFromSupercededSource */

/* 632 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 634 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 638 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 640 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 644 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 646 */	NdrFcLong( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x3 ),	/* 3 */
/* 652 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 656 */	NdrFcShort( 0x8 ),	/* 8 */
/* 658 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_pENCINFO */

/* 660 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 662 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 664 */	NdrFcShort( 0xd0 ),	/* Type Offset=208 */

	/* Return value */

/* 666 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 668 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 672 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 674 */	NdrFcLong( 0x0 ),	/* 0 */
/* 678 */	NdrFcShort( 0x3 ),	/* 3 */
/* 680 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 682 */	NdrFcShort( 0x8 ),	/* 8 */
/* 684 */	NdrFcShort( 0x24 ),	/* 36 */
/* 686 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter in_NoOfElementsRequested */

/* 688 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 690 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 692 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_ArrayOfpENCInfo */

/* 694 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 696 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 698 */	NdrFcShort( 0x102 ),	/* Type Offset=258 */

	/* Parameter out_pNoOfElementsFetched */

/* 700 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 702 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 706 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 708 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */

/* 712 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 714 */	NdrFcLong( 0x0 ),	/* 0 */
/* 718 */	NdrFcShort( 0x4 ),	/* 4 */
/* 720 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 722 */	NdrFcShort( 0x8 ),	/* 8 */
/* 724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 726 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter celt */


	/* Parameter celt */


	/* Parameter in_NoOfElements */


	/* Parameter in_NoOfElements */


	/* Parameter in_NoOfElements */

/* 728 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 730 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 732 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 734 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 736 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure CancelENC */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */

/* 740 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 742 */	NdrFcLong( 0x0 ),	/* 0 */
/* 746 */	NdrFcShort( 0x5 ),	/* 5 */
/* 748 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 752 */	NdrFcShort( 0x8 ),	/* 8 */
/* 754 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 756 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 758 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 762 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 764 */	NdrFcLong( 0x0 ),	/* 0 */
/* 768 */	NdrFcShort( 0x6 ),	/* 6 */
/* 770 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 772 */	NdrFcShort( 0x0 ),	/* 0 */
/* 774 */	NdrFcShort( 0x8 ),	/* 8 */
/* 776 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_ppEnumENCInfo */

/* 778 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 780 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 782 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Return value */

/* 784 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 786 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */

/* 790 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 792 */	NdrFcLong( 0x0 ),	/* 0 */
/* 796 */	NdrFcShort( 0x7 ),	/* 7 */
/* 798 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 802 */	NdrFcShort( 0x24 ),	/* 36 */
/* 804 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter out_pCount */


	/* Parameter out_pCount */


	/* Parameter out_pCount */

/* 806 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 808 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 812 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 814 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 818 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 820 */	NdrFcLong( 0x0 ),	/* 0 */
/* 824 */	NdrFcShort( 0x3 ),	/* 3 */
/* 826 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x24 ),	/* 36 */
/* 832 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x6,		/* 6 */

	/* Parameter out_pbstrWorkingDir */

/* 834 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 836 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 838 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter out_pbstrCommand */

/* 840 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 842 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 844 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter out_pbstrOutFile */

/* 846 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 848 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 850 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter out_pbstrDebugFile */

/* 852 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 854 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 856 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter out_pbEditFromLib */

/* 858 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 860 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 864 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 866 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 868 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 870 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 872 */	NdrFcLong( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x3 ),	/* 3 */
/* 878 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 882 */	NdrFcShort( 0x24 ),	/* 36 */
/* 884 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter in_NoOfElementsRequested */

/* 886 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 888 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 890 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_ArrayOfpENCInfo */

/* 892 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 894 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 896 */	NdrFcShort( 0x140 ),	/* Type Offset=320 */

	/* Parameter out_pNoOfElementsFetched */

/* 898 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 900 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 902 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 904 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 906 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 908 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 910 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 912 */	NdrFcLong( 0x0 ),	/* 0 */
/* 916 */	NdrFcShort( 0x6 ),	/* 6 */
/* 918 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 920 */	NdrFcShort( 0x0 ),	/* 0 */
/* 922 */	NdrFcShort( 0x8 ),	/* 8 */
/* 924 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_ppEnumENCRelinkInfo */

/* 926 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 928 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 930 */	NdrFcShort( 0x152 ),	/* Type Offset=338 */

	/* Return value */

/* 932 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 934 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 938 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 940 */	NdrFcLong( 0x0 ),	/* 0 */
/* 944 */	NdrFcShort( 0x3 ),	/* 3 */
/* 946 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 950 */	NdrFcShort( 0x24 ),	/* 36 */
/* 952 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter in_NoOfElementsRequested */

/* 954 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 956 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 958 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_ArrayOfpIDBInfo2 */

/* 960 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 962 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 964 */	NdrFcShort( 0x17e ),	/* Type Offset=382 */

	/* Parameter out_pNoOfElementsFetched */

/* 966 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 968 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 972 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 974 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 976 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 978 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 980 */	NdrFcLong( 0x0 ),	/* 0 */
/* 984 */	NdrFcShort( 0x6 ),	/* 6 */
/* 986 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 988 */	NdrFcShort( 0x0 ),	/* 0 */
/* 990 */	NdrFcShort( 0x8 ),	/* 8 */
/* 992 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_ppEnumIDBInfo2 */

/* 994 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 996 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 998 */	NdrFcShort( 0x190 ),	/* Type Offset=400 */

	/* Return value */

/* 1000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1002 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTargetBuildInfo */

/* 1006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1012 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1014 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1018 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1020 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter in_szTargetPath */

/* 1022 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1024 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1026 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter out_pbstrSourcePath */

/* 1028 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1030 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1032 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter out_pbstrCommand */

/* 1034 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1036 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1038 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter out_pbstrCurrentdir */

/* 1040 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1042 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1044 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Return value */

/* 1046 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1048 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1050 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsTargetEligible */

/* 1052 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1054 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1058 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1060 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1064 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1066 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter in_szTargetPath */

/* 1068 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1070 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1072 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Return value */

/* 1074 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1076 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1078 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumDebugIDBInfo */

/* 1080 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1082 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1086 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1088 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1092 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1094 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_ppEnumIDBInfo */

/* 1096 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1098 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1100 */	NdrFcShort( 0x190 ),	/* Type Offset=400 */

	/* Return value */

/* 1102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 1108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1114 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1116 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1122 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_ppEnumENCRelinkInfo */

/* 1124 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1126 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1128 */	NdrFcShort( 0x152 ),	/* Type Offset=338 */

	/* Return value */

/* 1130 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1132 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 1136 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1138 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1142 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1144 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1150 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_ppEnumENCInfo */

/* 1152 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1154 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1156 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Return value */

/* 1158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1160 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHResult */

/* 1164 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1170 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1172 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1176 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1178 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter out_pHResult */

/* 1180 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1182 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1186 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1188 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumENCSnapshots */

/* 1192 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1194 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1198 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1200 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1204 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1206 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pszModule */

/* 1208 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1210 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1212 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter ppEnum */

/* 1214 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1216 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1218 */	NdrFcShort( 0x1a6 ),	/* Type Offset=422 */

	/* Return value */

/* 1220 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1222 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumENCSnapshotsByGuid */

/* 1226 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1228 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1232 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1234 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1236 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1238 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1240 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter guidModule */

/* 1242 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1244 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1246 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter ppEnum */

/* 1248 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1250 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1252 */	NdrFcShort( 0x1a6 ),	/* Type Offset=422 */

	/* Return value */

/* 1254 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1256 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnENCAttemptComplete */

/* 1260 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1262 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1266 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1268 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1272 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1274 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 1276 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1278 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetENCSnapshotInfo */

/* 1282 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1290 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1294 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1296 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pSnapshotInfo */

/* 1298 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1300 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1302 */	NdrFcShort( 0x20c ),	/* Type Offset=524 */

	/* Return value */

/* 1304 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1306 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ApplyCodeChange */

/* 1310 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1316 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1318 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1320 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1324 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter in_NoOfLineMaps */

/* 1326 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1328 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter in_ArrayOfLineMaps */

/* 1332 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1334 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1336 */	NdrFcShort( 0x234 ),	/* Type Offset=564 */

	/* Parameter in_pServiceProvider */

/* 1338 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1340 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1342 */	NdrFcShort( 0x246 ),	/* Type Offset=582 */

	/* Parameter in_fOnContinue */

/* 1344 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1346 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter out_ppEnumENCInfo */

/* 1350 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1352 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1354 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Return value */

/* 1356 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1358 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1360 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CommitChange */

/* 1362 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1364 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1368 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1370 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1374 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1376 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter out_ppEnumENCInfo */

/* 1378 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1380 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1382 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Return value */

/* 1384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1386 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 1390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1398 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1400 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1402 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1404 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 1406 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1408 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 1412 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1414 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1416 */	NdrFcShort( 0x26e ),	/* Type Offset=622 */

	/* Parameter pceltFetched */

/* 1418 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1420 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1424 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1426 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 1430 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1432 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1436 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1438 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1442 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1444 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 1446 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1448 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1450 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Return value */

/* 1452 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1454 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 1458 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1460 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1464 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1466 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1468 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1470 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1472 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 1474 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 1480 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1484 */	NdrFcShort( 0x2ac ),	/* Type Offset=684 */

	/* Parameter pceltFetched */

/* 1486 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1488 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1494 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 1498 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1504 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1506 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1510 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1512 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 1514 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1516 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1518 */	NdrFcShort( 0x2be ),	/* Type Offset=702 */

	/* Return value */

/* 1520 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1522 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1524 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CopyMetaData */

/* 1526 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1528 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1532 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1534 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1538 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1540 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pIStream */

/* 1542 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1544 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1546 */	NdrFcShort( 0x2d4 ),	/* Type Offset=724 */

	/* Parameter pMvid */

/* 1548 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1550 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1552 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 1554 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1556 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMvid */

/* 1560 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1562 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1566 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1568 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1572 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1574 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pMvid */

/* 1576 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1578 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1580 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 1582 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1584 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1586 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRoDataRVA */

/* 1588 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1590 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1594 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1596 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1598 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1600 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1602 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pRoDataRVA */

/* 1604 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1606 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1608 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1610 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1612 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1614 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRwDataRVA */

/* 1616 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1618 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1622 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1624 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1628 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1630 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pRwDataRVA */

/* 1632 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1634 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1638 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1640 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPEBytes */

/* 1644 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1646 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1650 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1652 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1654 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1656 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1658 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pBytes */

/* 1660 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1662 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1664 */	NdrFcShort( 0x2ee ),	/* Type Offset=750 */

	/* Parameter dwBytes */

/* 1666 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1668 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1672 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1674 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetILMap */

/* 1678 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1680 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1686 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1688 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1692 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter mdFunction */

/* 1694 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1696 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1698 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cMapSize */

/* 1700 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1702 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter map */

/* 1706 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1708 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1710 */	NdrFcShort( 0x304 ),	/* Type Offset=772 */

	/* Return value */

/* 1712 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1714 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSymbolBytes */

/* 1718 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1720 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1724 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1726 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1732 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pBytes */

/* 1734 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1736 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1738 */	NdrFcShort( 0x2ee ),	/* Type Offset=750 */

	/* Parameter dwBytes */

/* 1740 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1742 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1744 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1746 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1748 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSymbolProvider */

/* 1752 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1754 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1758 */	NdrFcShort( 0xa ),	/* 10 */
/* 1760 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1764 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1766 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppSym */

/* 1768 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1770 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1772 */	NdrFcShort( 0x312 ),	/* Type Offset=786 */

	/* Return value */

/* 1774 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1776 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1778 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAppDomainAndModuleIDs */

/* 1780 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1782 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1786 */	NdrFcShort( 0xb ),	/* 11 */
/* 1788 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1790 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1792 */	NdrFcShort( 0x68 ),	/* 104 */
/* 1794 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pulAppDomainID */

/* 1796 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1798 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1800 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pguidModule */

/* 1802 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1804 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1806 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 1808 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1810 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1812 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RequestILMap */

/* 1814 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1816 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1820 */	NdrFcShort( 0xc ),	/* 12 */
/* 1822 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1824 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1826 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1828 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter mdFunction */

/* 1830 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1832 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1834 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1836 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1838 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1840 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateILMaps */

/* 1842 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1844 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1848 */	NdrFcShort( 0xd ),	/* 13 */
/* 1850 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1852 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1854 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1856 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter in_NoOfLineMaps */

/* 1858 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1860 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter in_ArrayOfLineMaps */

/* 1864 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1866 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1868 */	NdrFcShort( 0x33e ),	/* Type Offset=830 */

	/* Return value */

/* 1870 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1872 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1874 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumDependentImages */

/* 1876 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1878 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1882 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1884 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1888 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1890 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pszSourcePath */

/* 1892 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1894 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1896 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter ppEnum */

/* 1898 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1900 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1902 */	NdrFcShort( 0x350 ),	/* Type Offset=848 */

	/* Return value */

/* 1904 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1906 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1908 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumDependentTargets */

/* 1910 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1912 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1916 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1918 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1922 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1924 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter cSrc */

/* 1926 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1928 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1930 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pszSourcePath */

/* 1932 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1934 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1936 */	NdrFcShort( 0x366 ),	/* Type Offset=870 */

	/* Parameter ppEnum */

/* 1938 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1940 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1942 */	NdrFcShort( 0x350 ),	/* Type Offset=848 */

	/* Return value */

/* 1944 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1946 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTargetBuildInfo */

/* 1950 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1952 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1956 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1958 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1962 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1964 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter pszTargetPath */

/* 1966 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1968 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1970 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter pbstrSourcePath */

/* 1972 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1974 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1976 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter pbstrCommand */

/* 1978 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1980 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1982 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter pbstrCommandArgs */

/* 1984 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1986 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1988 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Parameter pbstrCurrentDir */

/* 1990 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1992 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1994 */	NdrFcShort( 0x8c ),	/* Type Offset=140 */

	/* Return value */

/* 1996 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1998 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Notify */

/* 2002 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2004 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2008 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2010 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2012 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2014 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2016 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter encnotify */

/* 2018 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2020 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2022 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Return value */

/* 2024 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2026 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2028 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsTargetEligible */

/* 2030 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2032 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2036 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2038 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2042 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2044 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszTargetPath */

/* 2046 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2048 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2050 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Return value */

/* 2052 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2054 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddRecompiledTarget */

/* 2058 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2060 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2064 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2066 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2070 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2072 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter in_szTargetPath */

/* 2074 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2076 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2078 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter in_szSavedTargetPath */

/* 2080 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2082 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2084 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Return value */

/* 2086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2088 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllLocalsProperty */

/* 2092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2098 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2100 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2106 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppProperty */

/* 2108 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2112 */	NdrFcShort( 0x384 ),	/* Type Offset=900 */

	/* Return value */

/* 2114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetModuleProps */

/* 2120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2126 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2128 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2130 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2134 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter szName */

/* 2136 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2138 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2140 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter ppid */

/* 2142 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2144 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2146 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter lcid */

/* 2148 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2150 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2152 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2154 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2156 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2158 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */

/* 2160 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2162 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2166 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2168 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2172 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2174 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter szFile */

/* 2176 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2178 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2180 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter dwSaveFlags */

/* 2182 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2184 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2186 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2188 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2190 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveToStream */

/* 2194 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2196 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2200 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2202 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2204 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2206 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2208 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pIStream */

/* 2210 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2212 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2214 */	NdrFcShort( 0x2d4 ),	/* Type Offset=724 */

	/* Parameter dwSaveFlags */

/* 2216 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2218 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2222 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2224 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineCustomValueAsBlob */

/* 2228 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2230 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2234 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2236 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2238 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2240 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2242 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter tkObj */

/* 2244 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2246 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szName */

/* 2250 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2252 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2254 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter pCustomValue */

/* 2256 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2258 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2260 */	NdrFcShort( 0x39e ),	/* Type Offset=926 */

	/* Parameter cbCustomValue */

/* 2262 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2264 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2266 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcv */

/* 2268 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 2270 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2274 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2276 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineTypeDef */

/* 2280 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2282 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2286 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2288 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 2290 */	NdrFcShort( 0x90 ),	/* 144 */
/* 2292 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2294 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0xd,		/* 13 */

	/* Parameter szNamespace */

/* 2296 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2298 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2300 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter szTypeDef */

/* 2302 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2304 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2306 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter pguid */

/* 2308 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2310 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2312 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter pVer */

/* 2314 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 2316 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2318 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter dwTypeDefFlags */

/* 2320 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2322 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tkExtends */

/* 2326 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2328 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwExtendsFlags */

/* 2332 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2334 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwImplements */

/* 2338 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2340 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2342 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rtkImplements */

/* 2344 */	NdrFcShort( 0x1b ),	/* Flags:  must size, must free, in, out, */
/* 2346 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 2348 */	NdrFcShort( 0x3ac ),	/* Type Offset=940 */

	/* Parameter dwEvents */

/* 2350 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2352 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 2354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rtkEvents */

/* 2356 */	NdrFcShort( 0x1b ),	/* Flags:  must size, must free, in, out, */
/* 2358 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 2360 */	NdrFcShort( 0x3ba ),	/* Type Offset=954 */

	/* Parameter ptd */

/* 2362 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2364 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 2366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2368 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2370 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 2372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTypeDefProps */

/* 2374 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2376 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2380 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2382 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 2384 */	NdrFcShort( 0x54 ),	/* 84 */
/* 2386 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2388 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0xa,		/* 10 */

	/* Parameter td */

/* 2390 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2392 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVer */

/* 2396 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 2398 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2400 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter dwTypeDefFlags */

/* 2402 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2404 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tkExtends */

/* 2408 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2410 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwExtendsFlags */

/* 2414 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2416 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwImplements */

/* 2420 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2422 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2424 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rtkImplements */

/* 2426 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2428 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2430 */	NdrFcShort( 0x3c8 ),	/* Type Offset=968 */

	/* Parameter dwEvents */

/* 2432 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2434 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rtkEvents */

/* 2438 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2440 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 2442 */	NdrFcShort( 0x3ac ),	/* Type Offset=940 */

	/* Return value */

/* 2444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2446 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 2448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetClassSvcsContext */

/* 2450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2456 */	NdrFcShort( 0xa ),	/* 10 */
/* 2458 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2460 */	NdrFcShort( 0x28 ),	/* 40 */
/* 2462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2464 */	0x4,		/* Oi2 Flags:  has return, */
			0x6,		/* 6 */

	/* Parameter td */

/* 2466 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2468 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwClassActivateAttr */

/* 2472 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2474 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2476 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwClassThreadAttr */

/* 2478 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2480 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2482 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwXactionAttr */

/* 2484 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2486 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2488 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwSynchAttr */

/* 2490 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2492 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2494 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2496 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2498 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2500 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineTypeRefByGUID */

/* 2502 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2504 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2508 */	NdrFcShort( 0xb ),	/* 11 */
/* 2510 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2512 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2514 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2516 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pguid */

/* 2518 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2520 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2522 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter ptr */

/* 2524 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2526 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2530 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2532 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetModuleReg */

/* 2536 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2538 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2542 */	NdrFcShort( 0xc ),	/* 12 */
/* 2544 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2546 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2548 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2550 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter dwModuleRegAttr */

/* 2552 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2554 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2556 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pguid */

/* 2558 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2560 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2562 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 2564 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2566 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetClassReg */

/* 2570 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2576 */	NdrFcShort( 0xd ),	/* 13 */
/* 2578 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 2580 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2582 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2584 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x9,		/* 9 */

	/* Parameter td */

/* 2586 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2588 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szProgID */

/* 2592 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2594 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2596 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter szVIProgID */

/* 2598 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2600 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2602 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter szIconURL */

/* 2604 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2606 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2608 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter ulIconResource */

/* 2610 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2612 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2614 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szSmallIconURL */

/* 2616 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2618 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2620 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter ulSmallIconResource */

/* 2622 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2624 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2626 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szDefaultDispName */

/* 2628 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2630 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2632 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Return value */

/* 2634 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2636 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 2638 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetIfaceReg */

/* 2640 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2642 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2646 */	NdrFcShort( 0xe ),	/* 14 */
/* 2648 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2650 */	NdrFcShort( 0x54 ),	/* 84 */
/* 2652 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2654 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter td */

/* 2656 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2658 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwIfaceSvcs */

/* 2662 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2664 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2666 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter proxyStub */

/* 2668 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2670 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2672 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 2674 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2676 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2678 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCategoryImpl */

/* 2680 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2682 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2686 */	NdrFcShort( 0xf ),	/* 15 */
/* 2688 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2690 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2692 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2694 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter td */

/* 2696 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2698 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2700 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwImpl */

/* 2702 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2704 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2706 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rGuidCoCatImpl */

/* 2708 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2710 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2712 */	NdrFcShort( 0x3d6 ),	/* Type Offset=982 */

	/* Parameter dwReqd */

/* 2714 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2716 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rGuidCoCatReqd */

/* 2720 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2722 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2724 */	NdrFcShort( 0x3e8 ),	/* Type Offset=1000 */

	/* Return value */

/* 2726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2728 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetRedirectProgID */

/* 2732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2738 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2740 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2742 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2746 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter td */

/* 2748 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2750 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwProgIds */

/* 2754 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2756 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszRedirectProgID */

/* 2760 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2762 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2764 */	NdrFcShort( 0x3fa ),	/* Type Offset=1018 */

	/* Return value */

/* 2766 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2768 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2770 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMimeTypeImpl */

/* 2772 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2774 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2778 */	NdrFcShort( 0x11 ),	/* 17 */
/* 2780 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2782 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2784 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2786 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter td */

/* 2788 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2790 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwTypes */

/* 2794 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2796 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszMimeType */

/* 2800 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2802 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2804 */	NdrFcShort( 0x3fa ),	/* Type Offset=1018 */

	/* Return value */

/* 2806 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2808 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFormatImpl */

/* 2812 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2814 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2818 */	NdrFcShort( 0x12 ),	/* 18 */
/* 2820 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 2822 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2824 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2826 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0xe,		/* 14 */

	/* Parameter td */

/* 2828 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2830 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwSupported */

/* 2834 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2836 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2838 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszFormatSupported */

/* 2840 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2842 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2844 */	NdrFcShort( 0x3fa ),	/* Type Offset=1018 */

	/* Parameter dwFrom */

/* 2846 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2848 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszFormatConvertsFrom */

/* 2852 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2854 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2856 */	NdrFcShort( 0x41c ),	/* Type Offset=1052 */

	/* Parameter dwTo */

/* 2858 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2860 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszFormatConvertsTo */

/* 2864 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2866 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2868 */	NdrFcShort( 0x43e ),	/* Type Offset=1086 */

	/* Parameter dwDefault */

/* 2870 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2872 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2874 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszFormatDefault */

/* 2876 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2878 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 2880 */	NdrFcShort( 0x460 ),	/* Type Offset=1120 */

	/* Parameter dwExt */

/* 2882 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2884 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 2886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszFileExt */

/* 2888 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2890 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 2892 */	NdrFcShort( 0x482 ),	/* Type Offset=1154 */

	/* Parameter dwType */

/* 2894 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2896 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 2898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszFileType */

/* 2900 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2902 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 2904 */	NdrFcShort( 0x4a4 ),	/* Type Offset=1188 */

	/* Return value */

/* 2906 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2908 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 2910 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetRoleCheck */

/* 2912 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2914 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2918 */	NdrFcShort( 0x13 ),	/* 19 */
/* 2920 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2922 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2924 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2926 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter tk */

/* 2928 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2930 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2932 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwNames */

/* 2934 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2936 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rszName */

/* 2940 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2942 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2944 */	NdrFcShort( 0x3fa ),	/* Type Offset=1018 */

	/* Parameter dwFlags */

/* 2946 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2948 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2950 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rdwRoleFlags */

/* 2952 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2954 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2956 */	NdrFcShort( 0x4c6 ),	/* Type Offset=1222 */

	/* Return value */

/* 2958 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2960 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2962 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineMethod */

/* 2964 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2966 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2970 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2972 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 2974 */	NdrFcShort( 0x30 ),	/* 48 */
/* 2976 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2978 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0xa,		/* 10 */

	/* Parameter td */

/* 2980 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2982 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2984 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szName */

/* 2986 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2988 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2990 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter dwMethodFlags */

/* 2992 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2994 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2996 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvSigBlob */

/* 2998 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3000 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3002 */	NdrFcShort( 0x4d8 ),	/* Type Offset=1240 */

	/* Parameter cbSigBlob */

/* 3004 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3006 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulSlot */

/* 3010 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3012 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3014 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulCodeRVA */

/* 3016 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3018 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwImplFlags */

/* 3022 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3024 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3026 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pmd */

/* 3028 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3030 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 3032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3034 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3036 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 3038 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineField */

/* 3040 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3042 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3046 */	NdrFcShort( 0x15 ),	/* 21 */
/* 3048 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 3050 */	NdrFcShort( 0x28 ),	/* 40 */
/* 3052 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3054 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0xa,		/* 10 */

	/* Parameter td */

/* 3056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3058 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3060 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szName */

/* 3062 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3064 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3066 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter dwFieldFlags */

/* 3068 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3070 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvSigBlob */

/* 3074 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3076 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3078 */	NdrFcShort( 0x4d8 ),	/* Type Offset=1240 */

	/* Parameter cbSigBlob */

/* 3080 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3082 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3084 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwCPlusTypeFlag */

/* 3086 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3088 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValue */

/* 3092 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3094 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3096 */	NdrFcShort( 0x4e6 ),	/* Type Offset=1254 */

	/* Parameter cbValue */

/* 3098 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3100 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3102 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pmd */

/* 3104 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3106 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 3108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3110 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3112 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 3114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetParamProps */

/* 3116 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3118 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3122 */	NdrFcShort( 0x16 ),	/* 22 */
/* 3124 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 3126 */	NdrFcShort( 0x28 ),	/* 40 */
/* 3128 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3130 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x9,		/* 9 */

	/* Parameter md */

/* 3132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulParamSeq */

/* 3138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3140 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szName */

/* 3144 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3146 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3148 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter dwParamFlags */

/* 3150 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3152 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwCPlusTypeFlag */

/* 3156 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3158 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValue */

/* 3162 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3164 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3166 */	NdrFcShort( 0x4f4 ),	/* Type Offset=1268 */

	/* Parameter cbValue */

/* 3168 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3170 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppd */

/* 3174 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3176 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3182 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 3184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineMethodImpl */

/* 3186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3192 */	NdrFcShort( 0x17 ),	/* 23 */
/* 3194 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3196 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3198 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3200 */	0x4,		/* Oi2 Flags:  has return, */
			0x6,		/* 6 */

	/* Parameter td */

/* 3202 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3204 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tk */

/* 3208 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3210 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulCodeRVA */

/* 3214 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3216 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwImplFlags */

/* 3220 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3222 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pmi */

/* 3226 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3228 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3232 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3234 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3236 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetRVA */

/* 3238 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3240 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3244 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3246 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3248 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3250 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3252 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter md */

/* 3254 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3256 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulCodeRVA */

/* 3260 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3262 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3264 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwImplFlags */

/* 3266 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3268 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3272 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3274 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineTypeRefByName */

/* 3278 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3280 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3284 */	NdrFcShort( 0x19 ),	/* 25 */
/* 3286 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3290 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3292 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter szNamespace */

/* 3294 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3296 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3298 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter szType */

/* 3300 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3302 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3304 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter ptr */

/* 3306 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3308 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3312 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3314 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTypeRefBind */

/* 3318 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3320 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3324 */	NdrFcShort( 0x1a ),	/* 26 */
/* 3326 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3328 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3330 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3332 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter tr */

/* 3334 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3336 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwBindFlags */

/* 3340 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3342 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwMinVersion */

/* 3346 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3348 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3350 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwMaxVersion */

/* 3352 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3354 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3356 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szCodebase */

/* 3358 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3360 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3362 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Return value */

/* 3364 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3366 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3368 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineMemberRef */

/* 3370 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3372 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3376 */	NdrFcShort( 0x1b ),	/* 27 */
/* 3378 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3380 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3382 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3384 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter tkImport */

/* 3386 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3388 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szName */

/* 3392 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3394 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3396 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter pvSigBlob */

/* 3398 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3400 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3402 */	NdrFcShort( 0x39e ),	/* Type Offset=926 */

	/* Parameter cbSigBlob */

/* 3404 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3406 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pmr */

/* 3410 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3412 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3416 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3418 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineException */

/* 3422 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3424 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3428 */	NdrFcShort( 0x1c ),	/* 28 */
/* 3430 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3432 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3434 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3436 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter mb */

/* 3438 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3440 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tk */

/* 3444 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3446 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pex */

/* 3450 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3452 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3456 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3458 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineProperty */

/* 3462 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3464 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3468 */	NdrFcShort( 0x1d ),	/* 29 */
/* 3470 */	NdrFcShort( 0x50 ),	/* x86 Stack size/offset = 80 */
/* 3472 */	NdrFcShort( 0x68 ),	/* 104 */
/* 3474 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3476 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x13,		/* 19 */

	/* Parameter td */

/* 3478 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3480 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3482 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szProperty */

/* 3484 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3486 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3488 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter dwPropFlags */

/* 3490 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3492 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3494 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvSig */

/* 3496 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3498 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3500 */	NdrFcShort( 0x4d8 ),	/* Type Offset=1240 */

	/* Parameter cbSig */

/* 3502 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3504 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3506 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwCPlusTypeFlag */

/* 3508 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3510 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3512 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValue */

/* 3514 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3516 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3518 */	NdrFcShort( 0x4e6 ),	/* Type Offset=1254 */

	/* Parameter cbValue */

/* 3520 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3522 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3524 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mdSetter */

/* 3526 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3528 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 3530 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mdGetter */

/* 3532 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3534 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 3536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mdReset */

/* 3538 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3540 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 3542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mdTestDefault */

/* 3544 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3546 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 3548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwOthers */

/* 3550 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3552 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 3554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rmdOtherMethods */

/* 3556 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3558 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 3560 */	NdrFcShort( 0x4fe ),	/* Type Offset=1278 */

	/* Parameter evNotifyChanging */

/* 3562 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3564 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 3566 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter evNotifyChanged */

/* 3568 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3570 */	NdrFcShort( 0x40 ),	/* x86 Stack size/offset = 64 */
/* 3572 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fdBackingField */

/* 3574 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3576 */	NdrFcShort( 0x44 ),	/* x86 Stack size/offset = 68 */
/* 3578 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pmdProp */

/* 3580 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3582 */	NdrFcShort( 0x48 ),	/* x86 Stack size/offset = 72 */
/* 3584 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3588 */	NdrFcShort( 0x4c ),	/* x86 Stack size/offset = 76 */
/* 3590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineEvent */

/* 3592 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3598 */	NdrFcShort( 0x1e ),	/* 30 */
/* 3600 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 3602 */	NdrFcShort( 0x38 ),	/* 56 */
/* 3604 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3606 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0xb,		/* 11 */

	/* Parameter td */

/* 3608 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3610 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3612 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter szEvent */

/* 3614 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3616 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3618 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter dwEventFlags */

/* 3620 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3622 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tkEventType */

/* 3626 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3628 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mdAddOn */

/* 3632 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3634 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mdRemoveOn */

/* 3638 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3640 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mdFire */

/* 3644 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3646 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwOthers */

/* 3650 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3652 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rmdOtherMethods */

/* 3656 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3658 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 3660 */	NdrFcShort( 0x3ac ),	/* Type Offset=940 */

	/* Parameter pmdEvent */

/* 3662 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3664 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 3666 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3668 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3670 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 3672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFieldMarshal */

/* 3674 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3680 */	NdrFcShort( 0x1f ),	/* 31 */
/* 3682 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3684 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3686 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3688 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter tk */

/* 3690 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3692 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvNativeType */

/* 3696 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3698 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3700 */	NdrFcShort( 0x510 ),	/* Type Offset=1296 */

	/* Parameter cbNativeType */

/* 3702 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3704 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3706 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3708 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3710 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefinePermissionSet */

/* 3714 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3716 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3720 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3722 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3724 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3726 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3728 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter tk */

/* 3730 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3732 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwAction */

/* 3736 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3738 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvPermission */

/* 3742 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3744 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3746 */	NdrFcShort( 0x39e ),	/* Type Offset=926 */

	/* Parameter cbPermission */

/* 3748 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3750 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppm */

/* 3754 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3756 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3760 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3762 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3764 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMemberIndex */

/* 3766 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3768 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3772 */	NdrFcShort( 0x21 ),	/* 33 */
/* 3774 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3776 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3778 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3780 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter md */

/* 3782 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3784 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulIndex */

/* 3788 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3790 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3796 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTokenFromSig */

/* 3800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3806 */	NdrFcShort( 0x22 ),	/* 34 */
/* 3808 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3810 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3812 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3814 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pvSig */

/* 3816 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3818 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3820 */	NdrFcShort( 0x51e ),	/* Type Offset=1310 */

	/* Parameter cbSig */

/* 3822 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3824 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pmsig */

/* 3828 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 3830 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3834 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3836 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3838 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineModuleRef */

/* 3840 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3842 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3846 */	NdrFcShort( 0x23 ),	/* 35 */
/* 3848 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3850 */	NdrFcShort( 0x88 ),	/* 136 */
/* 3852 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3854 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter szName */

/* 3856 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3858 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3860 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter pguid */

/* 3862 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3864 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3866 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter pmvid */

/* 3868 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3870 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3872 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter pmur */

/* 3874 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3876 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3878 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3880 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3882 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3884 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetParent */

/* 3886 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3888 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3892 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3894 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3896 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3898 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3900 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter mr */

/* 3902 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3904 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tk */

/* 3908 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3910 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3912 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3914 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3916 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3918 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTokenFromArraySpec */

/* 3920 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3922 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3926 */	NdrFcShort( 0x25 ),	/* 37 */
/* 3928 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3930 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3932 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3934 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pvSig */

/* 3936 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3938 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3940 */	NdrFcShort( 0x51e ),	/* Type Offset=1310 */

	/* Parameter cbSig */

/* 3942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3944 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter parrspec */

/* 3948 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3950 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3952 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3954 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3956 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3958 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineSourceFile */

/* 3960 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3962 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3966 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3968 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3972 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3974 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter szFileName */

/* 3976 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3978 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3980 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter psourcefile */

/* 3982 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3984 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3986 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3988 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3990 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineBlock */

/* 3994 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3996 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4000 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4002 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4004 */	NdrFcShort( 0x31 ),	/* 49 */
/* 4006 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4008 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter member */

/* 4010 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4012 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4014 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter sourcefile */

/* 4016 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4018 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAttr */

/* 4022 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 4024 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4026 */	0x1,		/* FC_BYTE */
			0x0,		/* 0 */

	/* Parameter cbAttr */

/* 4028 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4030 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4034 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4036 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4038 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DefineLocalVarScope */

/* 4040 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4042 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4046 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4048 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4050 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4052 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4054 */	0x4,		/* Oi2 Flags:  has return, */
			0x6,		/* 6 */

	/* Parameter scopeParent */

/* 4056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4058 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4060 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulStartLine */

/* 4062 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4064 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulEndLine */

/* 4068 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4070 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter member */

/* 4074 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4076 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4078 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter plocalvarscope */

/* 4080 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4082 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4084 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4088 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnStateChange */

/* 4092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4098 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4100 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4102 */	NdrFcShort( 0x22 ),	/* 34 */
/* 4104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4106 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter in_pENCSTATE */

/* 4108 */	NdrFcShort( 0x2008 ),	/* Flags:  in, srv alloc size=8 */
/* 4110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4112 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Parameter in_fReserved */

/* 4114 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4122 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4124 */	0x8,		/* FC_LONG */
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
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  4 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  8 */	NdrFcLong( 0x6d5140c1 ),	/* 1834041537 */
/* 12 */	NdrFcShort( 0x7436 ),	/* 29750 */
/* 14 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 16 */	0x80,		/* 128 */
			0x34,		/* 52 */
/* 18 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 20 */	0x0,		/* 0 */
			0x60,		/* 96 */
/* 22 */	0x9,		/* 9 */
			0xfa,		/* 250 */
/* 24 */	
			0x11, 0x0,	/* FC_RP */
/* 26 */	NdrFcShort( 0x8 ),	/* Offset= 8 (34) */
/* 28 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 30 */	NdrFcShort( 0x8 ),	/* 8 */
/* 32 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 34 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 36 */	NdrFcShort( 0x10 ),	/* 16 */
/* 38 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 40 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 42 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (28) */
			0x5b,		/* FC_END */
/* 46 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 48 */	0xd,		/* FC_ENUM16 */
			0x5c,		/* FC_PAD */
/* 50 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 52 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 54 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 56 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 58 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 60 */	NdrFcLong( 0x8948300f ),	/* -1991757809 */
/* 64 */	NdrFcShort( 0x8bd5 ),	/* -29739 */
/* 66 */	NdrFcShort( 0x4728 ),	/* 18216 */
/* 68 */	0xa1,		/* 161 */
			0xd8,		/* 216 */
/* 70 */	0x83,		/* 131 */
			0xd1,		/* 209 */
/* 72 */	0x72,		/* 114 */
			0x29,		/* 41 */
/* 74 */	0x5a,		/* 90 */
			0x9d,		/* 157 */
/* 76 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 78 */	NdrFcLong( 0x69d172ef ),	/* 1775334127 */
/* 82 */	NdrFcShort( 0xf2c4 ),	/* -3388 */
/* 84 */	NdrFcShort( 0x44e1 ),	/* 17633 */
/* 86 */	0x89,		/* 137 */
			0xf7,		/* 247 */
/* 88 */	0xc8,		/* 200 */
			0x62,		/* 98 */
/* 90 */	0x31,		/* 49 */
			0xe7,		/* 231 */
/* 92 */	0x6,		/* 6 */
			0xe9,		/* 233 */
/* 94 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 96 */	NdrFcLong( 0xec80d064 ),	/* -327102364 */
/* 100 */	NdrFcShort( 0x102e ),	/* 4142 */
/* 102 */	NdrFcShort( 0x435f ),	/* 17247 */
/* 104 */	0xaa,		/* 170 */
			0xfb,		/* 251 */
/* 106 */	0xd3,		/* 211 */
			0x7e,		/* 126 */
/* 108 */	0x2a,		/* 42 */
			0x4e,		/* 78 */
/* 110 */	0xf6,		/* 246 */
			0x54,		/* 84 */
/* 112 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 114 */	NdrFcShort( 0x1a ),	/* Offset= 26 (140) */
/* 116 */	
			0x13, 0x0,	/* FC_OP */
/* 118 */	NdrFcShort( 0xc ),	/* Offset= 12 (130) */
/* 120 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 122 */	NdrFcShort( 0x2 ),	/* 2 */
/* 124 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 126 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 128 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 130 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 134 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (120) */
/* 136 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 138 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 140 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x4 ),	/* 4 */
/* 146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 148 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (116) */
/* 150 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 152 */	NdrFcShort( 0x38 ),	/* Offset= 56 (208) */
/* 154 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 156 */	NdrFcLong( 0x1412926f ),	/* 336761455 */
/* 160 */	NdrFcShort( 0x5dd6 ),	/* 24022 */
/* 162 */	NdrFcShort( 0x4e58 ),	/* 20056 */
/* 164 */	0xb6,		/* 182 */
			0x48,		/* 72 */
/* 166 */	0xe1,		/* 225 */
			0xc6,		/* 198 */
/* 168 */	0x3e,		/* 62 */
			0x1,		/* 1 */
/* 170 */	0x3d,		/* 61 */
			0x51,		/* 81 */
/* 172 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 174 */	NdrFcLong( 0xac17b76b ),	/* -1407731861 */
/* 178 */	NdrFcShort( 0x2b09 ),	/* 11017 */
/* 180 */	NdrFcShort( 0x419a ),	/* 16794 */
/* 182 */	0xad,		/* 173 */
			0x5f,		/* 95 */
/* 184 */	0x7d,		/* 125 */
			0x74,		/* 116 */
/* 186 */	0x2,		/* 2 */
			0xda,		/* 218 */
/* 188 */	0x88,		/* 136 */
			0x75,		/* 117 */
/* 190 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 192 */	NdrFcLong( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 200 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 202 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 204 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 206 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 208 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 210 */	NdrFcShort( 0x1c ),	/* 28 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x16 ),	/* Offset= 22 (236) */
/* 216 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 218 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffbf ),	/* Offset= -65 (154) */
			0x8,		/* FC_LONG */
/* 222 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 224 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffab ),	/* Offset= -85 (140) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 228 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc7 ),	/* Offset= -57 (172) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 232 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd5 ),	/* Offset= -43 (190) */
			0x5b,		/* FC_END */
/* 236 */	
			0x11, 0x0,	/* FC_RP */
/* 238 */	NdrFcShort( 0x14 ),	/* Offset= 20 (258) */
/* 240 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 242 */	NdrFcLong( 0x6b56106f ),	/* 1800802415 */
/* 246 */	NdrFcShort( 0xbd51 ),	/* -17071 */
/* 248 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 250 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 252 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 254 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 256 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 258 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 262 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 264 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 266 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 268 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 270 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 272 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (240) */
/* 274 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 276 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 278 */	NdrFcShort( 0x2 ),	/* Offset= 2 (280) */
/* 280 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 282 */	NdrFcLong( 0x7b076ad1 ),	/* 2064083665 */
/* 286 */	NdrFcShort( 0xbd51 ),	/* -17071 */
/* 288 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 290 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 292 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 294 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 296 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 298 */	
			0x11, 0x0,	/* FC_RP */
/* 300 */	NdrFcShort( 0x14 ),	/* Offset= 20 (320) */
/* 302 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 304 */	NdrFcLong( 0xcbb63a8d ),	/* -877249907 */
/* 308 */	NdrFcShort( 0xbd57 ),	/* -17065 */
/* 310 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 312 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 314 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 316 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 318 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 320 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 326 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 328 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 330 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 332 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 334 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (302) */
/* 336 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 338 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 340 */	NdrFcShort( 0x2 ),	/* Offset= 2 (342) */
/* 342 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 344 */	NdrFcLong( 0xe51be743 ),	/* -451156157 */
/* 348 */	NdrFcShort( 0xbd57 ),	/* -17065 */
/* 350 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 352 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 354 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 356 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 358 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 360 */	
			0x11, 0x0,	/* FC_RP */
/* 362 */	NdrFcShort( 0x14 ),	/* Offset= 20 (382) */
/* 364 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 366 */	NdrFcLong( 0x9b7de9a9 ),	/* -1686246999 */
/* 370 */	NdrFcShort( 0xbd59 ),	/* -17063 */
/* 372 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 374 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 376 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 378 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 380 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 382 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 388 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 390 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 392 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 394 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 396 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (364) */
/* 398 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 400 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 402 */	NdrFcShort( 0x2 ),	/* Offset= 2 (404) */
/* 404 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 406 */	NdrFcLong( 0xb34e469b ),	/* -1286715749 */
/* 410 */	NdrFcShort( 0xbd59 ),	/* -17063 */
/* 412 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 414 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 416 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 418 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 420 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 422 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 424 */	NdrFcShort( 0x2 ),	/* Offset= 2 (426) */
/* 426 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 428 */	NdrFcLong( 0xf6f94d1a ),	/* -151433958 */
/* 432 */	NdrFcShort( 0x78c2 ),	/* 30914 */
/* 434 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 436 */	0x8f,		/* 143 */
			0xfe,		/* 254 */
/* 438 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 440 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 442 */	0x83,		/* 131 */
			0x14,		/* 20 */
/* 444 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 446 */	NdrFcShort( 0x4e ),	/* Offset= 78 (524) */
/* 448 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 450 */	NdrFcLong( 0x69d172ef ),	/* 1775334127 */
/* 454 */	NdrFcShort( 0xf2c4 ),	/* -3388 */
/* 456 */	NdrFcShort( 0x44e1 ),	/* 17633 */
/* 458 */	0x89,		/* 137 */
			0xf7,		/* 247 */
/* 460 */	0xc8,		/* 200 */
			0x62,		/* 98 */
/* 462 */	0x31,		/* 49 */
			0xe7,		/* 231 */
/* 464 */	0x6,		/* 6 */
			0xe9,		/* 233 */
/* 466 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 468 */	NdrFcShort( 0x4 ),	/* 4 */
/* 470 */	NdrFcShort( 0x2 ),	/* 2 */
/* 472 */	NdrFcLong( 0x1 ),	/* 1 */
/* 476 */	NdrFcShort( 0x1c ),	/* Offset= 28 (504) */
/* 478 */	NdrFcLong( 0x2 ),	/* 2 */
/* 482 */	NdrFcShort( 0x24 ),	/* Offset= 36 (518) */
/* 484 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 486 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 488 */	NdrFcLong( 0xf6f94d20 ),	/* -151433952 */
/* 492 */	NdrFcShort( 0x78c2 ),	/* 30914 */
/* 494 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 496 */	0x8f,		/* 143 */
			0xfe,		/* 254 */
/* 498 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 500 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 502 */	0x83,		/* 131 */
			0x14,		/* 20 */
/* 504 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 506 */	NdrFcShort( 0x4 ),	/* 4 */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 510 */	NdrFcShort( 0x8 ),	/* Offset= 8 (518) */
/* 512 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 514 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (486) */
/* 516 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 518 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 520 */	NdrFcShort( 0x4 ),	/* 4 */
/* 522 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 524 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 526 */	NdrFcShort( 0xc ),	/* 12 */
/* 528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 530 */	NdrFcShort( 0xc ),	/* Offset= 12 (542) */
/* 532 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 534 */	NdrFcShort( 0xffffffaa ),	/* Offset= -86 (448) */
/* 536 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 538 */	NdrFcShort( 0xffffffb8 ),	/* Offset= -72 (466) */
/* 540 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 542 */	
			0x11, 0x0,	/* FC_RP */
/* 544 */	NdrFcShort( 0x14 ),	/* Offset= 20 (564) */
/* 546 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 548 */	NdrFcLong( 0x8706233b ),	/* -2029640901 */
/* 552 */	NdrFcShort( 0xbd4c ),	/* -17076 */
/* 554 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 556 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 558 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 560 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 562 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 564 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 568 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 570 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 572 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 576 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 578 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (546) */
/* 580 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 582 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 584 */	NdrFcLong( 0x6d5140c1 ),	/* 1834041537 */
/* 588 */	NdrFcShort( 0x7436 ),	/* 29750 */
/* 590 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 592 */	0x80,		/* 128 */
			0x34,		/* 52 */
/* 594 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 596 */	0x0,		/* 0 */
			0x60,		/* 96 */
/* 598 */	0x9,		/* 9 */
			0xfa,		/* 250 */
/* 600 */	
			0x11, 0x0,	/* FC_RP */
/* 602 */	NdrFcShort( 0x14 ),	/* Offset= 20 (622) */
/* 604 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 606 */	NdrFcLong( 0xf6f94d0e ),	/* -151433970 */
/* 610 */	NdrFcShort( 0x78c2 ),	/* 30914 */
/* 612 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 614 */	0x8f,		/* 143 */
			0xfe,		/* 254 */
/* 616 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 618 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 620 */	0x83,		/* 131 */
			0x14,		/* 20 */
/* 622 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 628 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 630 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 632 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 634 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 636 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (604) */
/* 638 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 640 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 642 */	NdrFcShort( 0x2 ),	/* Offset= 2 (644) */
/* 644 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 646 */	NdrFcLong( 0xf6f94d1a ),	/* -151433958 */
/* 650 */	NdrFcShort( 0x78c2 ),	/* 30914 */
/* 652 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 654 */	0x8f,		/* 143 */
			0xfe,		/* 254 */
/* 656 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 658 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 660 */	0x83,		/* 131 */
			0x14,		/* 20 */
/* 662 */	
			0x11, 0x0,	/* FC_RP */
/* 664 */	NdrFcShort( 0x14 ),	/* Offset= 20 (684) */
/* 666 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 668 */	NdrFcLong( 0x1cf2b120 ),	/* 485667104 */
/* 672 */	NdrFcShort( 0x547d ),	/* 21629 */
/* 674 */	NdrFcShort( 0x101b ),	/* 4123 */
/* 676 */	0x8e,		/* 142 */
			0x65,		/* 101 */
/* 678 */	0x8,		/* 8 */
			0x0,		/* 0 */
/* 680 */	0x2b,		/* 43 */
			0x2b,		/* 43 */
/* 682 */	0xd1,		/* 209 */
			0x19,		/* 25 */
/* 684 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 688 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 690 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 692 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 694 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 696 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 698 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (666) */
/* 700 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 702 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 704 */	NdrFcShort( 0x2 ),	/* Offset= 2 (706) */
/* 706 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 708 */	NdrFcLong( 0xf6f94d23 ),	/* -151433949 */
/* 712 */	NdrFcShort( 0x78c2 ),	/* 30914 */
/* 714 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 716 */	0x8f,		/* 143 */
			0xfe,		/* 254 */
/* 718 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 720 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 722 */	0x83,		/* 131 */
			0x14,		/* 20 */
/* 724 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 726 */	NdrFcLong( 0xc ),	/* 12 */
/* 730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 734 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 736 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 738 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 740 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 742 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 744 */	NdrFcShort( 0xfffffd3a ),	/* Offset= -710 (34) */
/* 746 */	
			0x11, 0x0,	/* FC_RP */
/* 748 */	NdrFcShort( 0x2 ),	/* Offset= 2 (750) */
/* 750 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 752 */	NdrFcShort( 0x1 ),	/* 1 */
/* 754 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 756 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 758 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 760 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 762 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 764 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 766 */	NdrFcShort( 0xc ),	/* 12 */
/* 768 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 770 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 772 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 774 */	NdrFcShort( 0xc ),	/* 12 */
/* 776 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 778 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 780 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 782 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (764) */
/* 784 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 786 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 788 */	NdrFcShort( 0x2 ),	/* Offset= 2 (790) */
/* 790 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 792 */	NdrFcLong( 0xc2e34eaf ),	/* -1025290577 */
/* 796 */	NdrFcShort( 0x8b9d ),	/* -29795 */
/* 798 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 800 */	0x90,		/* 144 */
			0x14,		/* 20 */
/* 802 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 804 */	0x4f,		/* 79 */
			0xa3,		/* 163 */
/* 806 */	0x83,		/* 131 */
			0x38,		/* 56 */
/* 808 */	
			0x11, 0x0,	/* FC_RP */
/* 810 */	NdrFcShort( 0x14 ),	/* Offset= 20 (830) */
/* 812 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 814 */	NdrFcLong( 0x8706233b ),	/* -2029640901 */
/* 818 */	NdrFcShort( 0xbd4c ),	/* -17076 */
/* 820 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 822 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 824 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 826 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 828 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 830 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 834 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 836 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 838 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 842 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 844 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (812) */
/* 846 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 848 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 850 */	NdrFcShort( 0x2 ),	/* Offset= 2 (852) */
/* 852 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 854 */	NdrFcLong( 0x101 ),	/* 257 */
/* 858 */	NdrFcShort( 0x0 ),	/* 0 */
/* 860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 862 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 864 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 866 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 868 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 870 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 872 */	NdrFcShort( 0x4 ),	/* 4 */
/* 874 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 876 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 878 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 880 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 882 */	NdrFcShort( 0x4 ),	/* 4 */
/* 884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 886 */	NdrFcShort( 0x1 ),	/* 1 */
/* 888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 892 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 894 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 896 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 898 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 900 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 902 */	NdrFcShort( 0x2 ),	/* Offset= 2 (904) */
/* 904 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 906 */	NdrFcLong( 0xa7ee3e7e ),	/* -1477558658 */
/* 910 */	NdrFcShort( 0x2dd2 ),	/* 11730 */
/* 912 */	NdrFcShort( 0x4ad7 ),	/* 19159 */
/* 914 */	0x96,		/* 150 */
			0x97,		/* 151 */
/* 916 */	0xf4,		/* 244 */
			0xaa,		/* 170 */
/* 918 */	0xe3,		/* 227 */
			0x42,		/* 66 */
/* 920 */	0x77,		/* 119 */
			0x62,		/* 98 */
/* 922 */	
			0x11, 0x0,	/* FC_RP */
/* 924 */	NdrFcShort( 0x2 ),	/* Offset= 2 (926) */
/* 926 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 928 */	NdrFcShort( 0x1 ),	/* 1 */
/* 930 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 932 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 934 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 936 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 938 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 940 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 942 */	NdrFcShort( 0x4 ),	/* 4 */
/* 944 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 946 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 948 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 950 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 952 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 954 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 956 */	NdrFcShort( 0x4 ),	/* 4 */
/* 958 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 960 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 962 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 964 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 966 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 968 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 970 */	NdrFcShort( 0x4 ),	/* 4 */
/* 972 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 974 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 976 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 978 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 980 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 982 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 984 */	NdrFcShort( 0x10 ),	/* 16 */
/* 986 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 988 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 990 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 992 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 994 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 996 */	NdrFcShort( 0xfffffc3e ),	/* Offset= -962 (34) */
/* 998 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1000 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1002 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1004 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1006 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1008 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1010 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1012 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1014 */	NdrFcShort( 0xfffffc2c ),	/* Offset= -980 (34) */
/* 1016 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1018 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1020 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1022 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1024 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1026 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1028 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1030 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1032 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 1034 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1036 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1038 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1044 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1046 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1048 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1050 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1052 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1054 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1056 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1058 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1060 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1062 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1064 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1066 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 1068 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1072 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1078 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1080 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1082 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1084 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1086 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1088 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1090 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1092 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1094 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1096 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1098 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1100 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 1102 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1106 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1112 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1114 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1116 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1118 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1120 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1122 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1124 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1126 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1128 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1130 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1132 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1134 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 1136 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1140 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1146 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1148 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1150 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1152 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1154 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1156 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1158 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1160 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1162 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1164 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1166 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1168 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 1170 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1174 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1180 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1182 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1184 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1186 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1188 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1190 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1192 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1194 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 1196 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1198 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 1200 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1202 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 1204 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1208 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1214 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1216 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1218 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1220 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1222 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1224 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1226 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1228 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1230 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1232 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1234 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1236 */	
			0x11, 0x0,	/* FC_RP */
/* 1238 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1240) */
/* 1240 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1242 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1244 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1246 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1248 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1250 */	
			0x11, 0x0,	/* FC_RP */
/* 1252 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1254) */
/* 1254 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1256 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1258 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1260 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1262 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1264 */	
			0x11, 0x0,	/* FC_RP */
/* 1266 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1268) */
/* 1268 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1270 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1272 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1274 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1276 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1278 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1280 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1282 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1284 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 1286 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1288 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 1290 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1292 */	
			0x11, 0x0,	/* FC_RP */
/* 1294 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1296) */
/* 1296 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1298 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1300 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1302 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1304 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1306 */	
			0x11, 0x0,	/* FC_RP */
/* 1308 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1310) */
/* 1310 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1312 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1314 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1316 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1318 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1320 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1322 */	0x1,		/* FC_BYTE */
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
            }

        };



/* Standard interface: __MIDL_itf_enc_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDebugENC, ver. 0.0,
   GUID={0xB104D8B7,0xAF19,0x11d2,{0x92,0x2C,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENC_FormatStringOffsetTable[] =
    {
    0,
    28,
    56,
    90,
    124,
    152,
    204,
    256,
    296,
    324,
    346,
    374,
    402,
    436,
    470
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENC_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENC_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENC_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENC_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(18) _IDebugENCProxyVtbl = 
{
    &IDebugENC_ProxyInfo,
    &IID_IDebugENC,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENC::SetRegistryRoot */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::EnterDebuggingSession */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::SetENCProjectBuildOption */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::InquireENCState */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::InquireENCRelinkState */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::MapToEdited */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::MapToSuperceded */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::ApplyCodeChanges */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::CancelApplyCodeChanges */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::LeaveDebuggingSession */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::AdviseENCStateEvents */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::UnadviseENCStateEvents */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::GetFileName */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::GetFileDisplayName */ ,
    (void *) (INT_PTR) -1 /* IDebugENC::ClearENCState */
};

const CInterfaceStubVtbl _IDebugENCStubVtbl =
{
    &IID_IDebugENC,
    &IDebugENC_ServerInfo,
    18,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCLineMap, ver. 0.0,
   GUID={0x8706233B,0xBD4C,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCLineMap_FormatStringOffsetTable[] =
    {
    492,
    520,
    548,
    576,
    610
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCLineMap_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCLineMap_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCLineMap_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCLineMap_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IDebugENCLineMapProxyVtbl = 
{
    &IDebugENCLineMap_ProxyInfo,
    &IID_IDebugENCLineMap,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCLineMap::GetEditedSource */ ,
    (void *) (INT_PTR) -1 /* IDebugENCLineMap::GetSupercededSource */ ,
    (void *) (INT_PTR) -1 /* IDebugENCLineMap::IsLineModified */ ,
    (void *) (INT_PTR) -1 /* IDebugENCLineMap::LineMap */ ,
    (void *) (INT_PTR) -1 /* IDebugENCLineMap::ReverseLineMap */
};

const CInterfaceStubVtbl _IDebugENCLineMapStubVtbl =
{
    &IID_IDebugENCLineMap,
    &IDebugENCLineMap_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_enc_0373, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IDebugENCInfo2, ver. 0.0,
   GUID={0x6B56106F,0xBD51,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCInfo2_FormatStringOffsetTable[] =
    {
    644
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugENCInfo2ProxyVtbl = 
{
    &IDebugENCInfo2_ProxyInfo,
    &IID_IDebugENCInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCInfo2::GetInfo */
};

const CInterfaceStubVtbl _IDebugENCInfo2StubVtbl =
{
    &IID_IDebugENCInfo2,
    &IDebugENCInfo2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugENCInfo2, ver. 0.0,
   GUID={0x7B076AD1,0xBD51,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugENCInfo2_FormatStringOffsetTable[] =
    {
    672,
    712,
    740,
    762,
    790
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugENCInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugENCInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugENCInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugENCInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugENCInfo2ProxyVtbl = 
{
    &IEnumDebugENCInfo2_ProxyInfo,
    &IID_IEnumDebugENCInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCInfo2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCInfo2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCInfo2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCInfo2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCInfo2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugENCInfo2StubVtbl =
{
    &IID_IEnumDebugENCInfo2,
    &IEnumDebugENCInfo2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCRelinkInfo2, ver. 0.0,
   GUID={0xCBB63A8D,0xBD57,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCRelinkInfo2_FormatStringOffsetTable[] =
    {
    818
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCRelinkInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCRelinkInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCRelinkInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCRelinkInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugENCRelinkInfo2ProxyVtbl = 
{
    &IDebugENCRelinkInfo2_ProxyInfo,
    &IID_IDebugENCRelinkInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCRelinkInfo2::GetInfo */
};

const CInterfaceStubVtbl _IDebugENCRelinkInfo2StubVtbl =
{
    &IID_IDebugENCRelinkInfo2,
    &IDebugENCRelinkInfo2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugENCRelinkInfo2, ver. 0.0,
   GUID={0xE51BE743,0xBD57,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugENCRelinkInfo2_FormatStringOffsetTable[] =
    {
    870,
    712,
    740,
    910,
    790
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugENCRelinkInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugENCRelinkInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugENCRelinkInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugENCRelinkInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugENCRelinkInfo2ProxyVtbl = 
{
    &IEnumDebugENCRelinkInfo2_ProxyInfo,
    &IID_IEnumDebugENCRelinkInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCRelinkInfo2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCRelinkInfo2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCRelinkInfo2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCRelinkInfo2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCRelinkInfo2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugENCRelinkInfo2StubVtbl =
{
    &IID_IEnumDebugENCRelinkInfo2,
    &IEnumDebugENCRelinkInfo2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugIDBInfo2, ver. 0.0,
   GUID={0x9B7DE9A9,0xBD59,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugIDBInfo2_FormatStringOffsetTable[] =
    {
    492
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugIDBInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugIDBInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugIDBInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugIDBInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugIDBInfo2ProxyVtbl = 
{
    &IDebugIDBInfo2_ProxyInfo,
    &IID_IDebugIDBInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugIDBInfo2::GetInfo */
};

const CInterfaceStubVtbl _IDebugIDBInfo2StubVtbl =
{
    &IID_IDebugIDBInfo2,
    &IDebugIDBInfo2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugIDBInfo2, ver. 0.0,
   GUID={0xB34E469B,0xBD59,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugIDBInfo2_FormatStringOffsetTable[] =
    {
    938,
    712,
    740,
    978,
    790
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugIDBInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugIDBInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugIDBInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugIDBInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugIDBInfo2ProxyVtbl = 
{
    &IEnumDebugIDBInfo2_ProxyInfo,
    &IID_IEnumDebugIDBInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugIDBInfo2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugIDBInfo2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugIDBInfo2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugIDBInfo2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugIDBInfo2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugIDBInfo2StubVtbl =
{
    &IID_IEnumDebugIDBInfo2,
    &IEnumDebugIDBInfo2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCBuildInfo2, ver. 0.0,
   GUID={0xEA70281B,0xBD58,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCBuildInfo2_FormatStringOffsetTable[] =
    {
    1006,
    1052,
    1080
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCBuildInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCBuildInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCBuildInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCBuildInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDebugENCBuildInfo2ProxyVtbl = 
{
    &IDebugENCBuildInfo2_ProxyInfo,
    &IID_IDebugENCBuildInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCBuildInfo2::GetTargetBuildInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugENCBuildInfo2::IsTargetEligible */ ,
    (void *) (INT_PTR) -1 /* IDebugENCBuildInfo2::EnumDebugIDBInfo */
};

const CInterfaceStubVtbl _IDebugENCBuildInfo2StubVtbl =
{
    &IID_IDebugENCBuildInfo2,
    &IDebugENCBuildInfo2_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCUpdateOnRelinkEvent2, ver. 0.0,
   GUID={0x0EBF1959,0xBD57,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCUpdateOnRelinkEvent2_FormatStringOffsetTable[] =
    {
    1108
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCUpdateOnRelinkEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCUpdateOnRelinkEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCUpdateOnRelinkEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCUpdateOnRelinkEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugENCUpdateOnRelinkEvent2ProxyVtbl = 
{
    &IDebugENCUpdateOnRelinkEvent2_ProxyInfo,
    &IID_IDebugENCUpdateOnRelinkEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCUpdateOnRelinkEvent2::GetInfo */
};

const CInterfaceStubVtbl _IDebugENCUpdateOnRelinkEvent2StubVtbl =
{
    &IID_IDebugENCUpdateOnRelinkEvent2,
    &IDebugENCUpdateOnRelinkEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCUpdateOnStaleCodeEvent2, ver. 0.0,
   GUID={0x2F01EB29,0xBD57,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCUpdateOnStaleCodeEvent2_FormatStringOffsetTable[] =
    {
    1136,
    1164
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCUpdateOnStaleCodeEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCUpdateOnStaleCodeEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCUpdateOnStaleCodeEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCUpdateOnStaleCodeEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugENCUpdateOnStaleCodeEvent2ProxyVtbl = 
{
    &IDebugENCUpdateOnStaleCodeEvent2_ProxyInfo,
    &IID_IDebugENCUpdateOnStaleCodeEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCUpdateOnStaleCodeEvent2::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugENCUpdateOnStaleCodeEvent2::GetHResult */
};

const CInterfaceStubVtbl _IDebugENCUpdateOnStaleCodeEvent2StubVtbl =
{
    &IID_IDebugENCUpdateOnStaleCodeEvent2,
    &IDebugENCUpdateOnStaleCodeEvent2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCUpdate, ver. 0.0,
   GUID={0x978BAEE7,0xBD4C,0x11d2,{0x92,0x38,0x00,0xA0,0x24,0x48,0x79,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCUpdate_FormatStringOffsetTable[] =
    {
    1192,
    1226,
    740,
    1260
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCUpdate_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCUpdate_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCUpdate_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCUpdate_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDebugENCUpdateProxyVtbl = 
{
    &IDebugENCUpdate_ProxyInfo,
    &IID_IDebugENCUpdate,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCUpdate::EnumENCSnapshots */ ,
    (void *) (INT_PTR) -1 /* IDebugENCUpdate::EnumENCSnapshotsByGuid */ ,
    (void *) (INT_PTR) -1 /* IDebugENCUpdate::CancelENC */ ,
    (void *) (INT_PTR) -1 /* IDebugENCUpdate::OnENCAttemptComplete */
};

const CInterfaceStubVtbl _IDebugENCUpdateStubVtbl =
{
    &IID_IDebugENCUpdate,
    &IDebugENCUpdate_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCSnapshot2, ver. 0.0,
   GUID={0xf6f94d0e,0x78c2,0x11d2,{0x8f,0xfe,0x00,0xc0,0x4f,0xa3,0x83,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCSnapshot2_FormatStringOffsetTable[] =
    {
    1282,
    1310,
    1362
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCSnapshot2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCSnapshot2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCSnapshot2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCSnapshot2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDebugENCSnapshot2ProxyVtbl = 
{
    &IDebugENCSnapshot2_ProxyInfo,
    &IID_IDebugENCSnapshot2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCSnapshot2::GetENCSnapshotInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugENCSnapshot2::ApplyCodeChange */ ,
    (void *) (INT_PTR) -1 /* IDebugENCSnapshot2::CommitChange */
};

const CInterfaceStubVtbl _IDebugENCSnapshot2StubVtbl =
{
    &IID_IDebugENCSnapshot2,
    &IDebugENCSnapshot2_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugENCSnapshots2, ver. 0.0,
   GUID={0xf6f94d1a,0x78c2,0x11d2,{0x8f,0xfe,0x00,0xc0,0x4f,0xa3,0x83,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugENCSnapshots2_FormatStringOffsetTable[] =
    {
    1390,
    712,
    740,
    1430,
    790
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugENCSnapshots2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugENCSnapshots2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugENCSnapshots2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugENCSnapshots2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugENCSnapshots2ProxyVtbl = 
{
    &IEnumDebugENCSnapshots2_ProxyInfo,
    &IID_IEnumDebugENCSnapshots2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCSnapshots2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCSnapshots2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCSnapshots2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCSnapshots2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugENCSnapshots2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugENCSnapshots2StubVtbl =
{
    &IID_IEnumDebugENCSnapshots2,
    &IEnumDebugENCSnapshots2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugErrorInfos2, ver. 0.0,
   GUID={0xf6f94d23,0x78c2,0x11d2,{0x8f,0xfe,0x00,0xc0,0x4f,0xa3,0x83,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugErrorInfos2_FormatStringOffsetTable[] =
    {
    1458,
    712,
    740,
    1498,
    790
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugErrorInfos2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugErrorInfos2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugErrorInfos2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugErrorInfos2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugErrorInfos2ProxyVtbl = 
{
    &IEnumDebugErrorInfos2_ProxyInfo,
    &IID_IEnumDebugErrorInfos2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorInfos2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorInfos2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorInfos2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorInfos2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorInfos2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugErrorInfos2StubVtbl =
{
    &IID_IEnumDebugErrorInfos2,
    &IEnumDebugErrorInfos2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugComPlusSnapshot2, ver. 0.0,
   GUID={0xf6f94d20,0x78c2,0x11d2,{0x8f,0xfe,0x00,0xc0,0x4f,0xa3,0x83,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugComPlusSnapshot2_FormatStringOffsetTable[] =
    {
    1526,
    1560,
    1588,
    1616,
    1644,
    1678,
    1718,
    1752,
    1780,
    1814,
    1842
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugComPlusSnapshot2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugComPlusSnapshot2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugComPlusSnapshot2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugComPlusSnapshot2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IDebugComPlusSnapshot2ProxyVtbl = 
{
    &IDebugComPlusSnapshot2_ProxyInfo,
    &IID_IDebugComPlusSnapshot2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::CopyMetaData */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::GetMvid */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::GetRoDataRVA */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::GetRwDataRVA */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::SetPEBytes */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::SetILMap */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::SetSymbolBytes */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::GetSymbolProvider */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::GetAppDomainAndModuleIDs */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::RequestILMap */ ,
    (void *) (INT_PTR) -1 /* IDebugComPlusSnapshot2::CreateILMaps */
};

const CInterfaceStubVtbl _IDebugComPlusSnapshot2StubVtbl =
{
    &IID_IDebugComPlusSnapshot2,
    &IDebugComPlusSnapshot2_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugNativeSnapshot2, ver. 0.0,
   GUID={0x461fda3e,0xbba5,0x11d2,{0xb1,0x0f,0x00,0xc0,0x4f,0x72,0xdc,0x32}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugNativeSnapshot2_FormatStringOffsetTable[] =
    {
    0,
    1876,
    1910,
    1950,
    2002,
    2030,
    2058
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugNativeSnapshot2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugNativeSnapshot2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugNativeSnapshot2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugNativeSnapshot2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IDebugNativeSnapshot2ProxyVtbl = 
{
    &IDebugNativeSnapshot2_ProxyInfo,
    &IID_IDebugNativeSnapshot2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugNativeSnapshot2::HasDependentTargets */ ,
    (void *) (INT_PTR) -1 /* IDebugNativeSnapshot2::EnumDependentImages */ ,
    (void *) (INT_PTR) -1 /* IDebugNativeSnapshot2::EnumDependentTargets */ ,
    (void *) (INT_PTR) -1 /* IDebugNativeSnapshot2::GetTargetBuildInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugNativeSnapshot2::Notify */ ,
    (void *) (INT_PTR) -1 /* IDebugNativeSnapshot2::IsTargetEligible */ ,
    (void *) (INT_PTR) -1 /* IDebugNativeSnapshot2::AddRecompiledTarget */
};

const CInterfaceStubVtbl _IDebugNativeSnapshot2StubVtbl =
{
    &IID_IDebugNativeSnapshot2,
    &IDebugNativeSnapshot2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCStackFrame2, ver. 0.0,
   GUID={0xB3C64D7F,0xDB9D,0x47c7,{0xB4,0x79,0xC5,0x79,0xC7,0xF0,0x71,0x03}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCStackFrame2_FormatStringOffsetTable[] =
    {
    2092
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCStackFrame2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCStackFrame2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCStackFrame2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCStackFrame2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugENCStackFrame2ProxyVtbl = 
{
    &IDebugENCStackFrame2_ProxyInfo,
    &IID_IDebugENCStackFrame2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCStackFrame2::GetAllLocalsProperty */
};

const CInterfaceStubVtbl _IDebugENCStackFrame2StubVtbl =
{
    &IID_IDebugENCStackFrame2,
    &IDebugENCStackFrame2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_enc_0389, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IDebugMetaDataEmit2, ver. 0.0,
   GUID={0xf6f94d21,0x78c2,0x11d2,{0x8f,0xfe,0x00,0xc0,0x4f,0xa3,0x83,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMetaDataEmit2_FormatStringOffsetTable[] =
    {
    2120,
    2160,
    2194,
    576,
    2228,
    2280,
    2374,
    2450,
    2502,
    2536,
    2570,
    2640,
    2680,
    2732,
    2772,
    2812,
    2912,
    2964,
    3040,
    3116,
    3186,
    3238,
    3278,
    3318,
    3370,
    3422,
    3462,
    3592,
    3674,
    3714,
    3766,
    3800,
    3840,
    3886,
    3920
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMetaDataEmit2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMetaDataEmit2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMetaDataEmit2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMetaDataEmit2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(38) _IDebugMetaDataEmit2ProxyVtbl = 
{
    &IDebugMetaDataEmit2_ProxyInfo,
    &IID_IDebugMetaDataEmit2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetModuleProps */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::Save */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SaveToStream */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::GetSaveSize */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineCustomValueAsBlob */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineTypeDef */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetTypeDefProps */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetClassSvcsContext */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineTypeRefByGUID */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetModuleReg */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetClassReg */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetIfaceReg */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetCategoryImpl */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetRedirectProgID */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetMimeTypeImpl */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetFormatImpl */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetRoleCheck */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineMethod */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineField */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetParamProps */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineMethodImpl */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetRVA */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineTypeRefByName */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetTypeRefBind */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineMemberRef */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineException */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineProperty */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineEvent */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetFieldMarshal */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefinePermissionSet */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetMemberIndex */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::GetTokenFromSig */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::DefineModuleRef */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::SetParent */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataEmit2::GetTokenFromArraySpec */
};

const CInterfaceStubVtbl _IDebugMetaDataEmit2StubVtbl =
{
    &IID_IDebugMetaDataEmit2,
    &IDebugMetaDataEmit2_ServerInfo,
    38,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugMetaDataDebugEmit2, ver. 0.0,
   GUID={0xf6f94d22,0x78c2,0x11d2,{0x8f,0xfe,0x00,0xc0,0x4f,0xa3,0x83,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMetaDataDebugEmit2_FormatStringOffsetTable[] =
    {
    3960,
    3994,
    4040
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMetaDataDebugEmit2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMetaDataDebugEmit2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMetaDataDebugEmit2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMetaDataDebugEmit2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDebugMetaDataDebugEmit2ProxyVtbl = 
{
    &IDebugMetaDataDebugEmit2_ProxyInfo,
    &IID_IDebugMetaDataDebugEmit2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataDebugEmit2::DefineSourceFile */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataDebugEmit2::DefineBlock */ ,
    (void *) (INT_PTR) -1 /* IDebugMetaDataDebugEmit2::DefineLocalVarScope */
};

const CInterfaceStubVtbl _IDebugMetaDataDebugEmit2StubVtbl =
{
    &IID_IDebugMetaDataDebugEmit2,
    &IDebugMetaDataDebugEmit2_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugENCStateEvents, ver. 0.0,
   GUID={0xec80d064,0x102e,0x435f,{0xaa,0xfb,0xd3,0x7e,0x2a,0x4e,0xf6,0x54}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugENCStateEvents_FormatStringOffsetTable[] =
    {
    4092
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugENCStateEvents_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugENCStateEvents_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugENCStateEvents_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugENCStateEvents_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugENCStateEventsProxyVtbl = 
{
    &IDebugENCStateEvents_ProxyInfo,
    &IID_IDebugENCStateEvents,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugENCStateEvents::OnStateChange */
};

const CInterfaceStubVtbl _IDebugENCStateEventsStubVtbl =
{
    &IID_IDebugENCStateEvents,
    &IDebugENCStateEvents_ServerInfo,
    4,
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

const CInterfaceProxyVtbl * _enc_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IDebugENCSnapshot2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugENCSnapshots2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCBuildInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugComPlusSnapshot2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMetaDataEmit2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMetaDataDebugEmit2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugErrorInfos2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCUpdateOnStaleCodeEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCLineMapProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugNativeSnapshot2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugENCRelinkInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCUpdateOnRelinkEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCStateEventsProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCStackFrame2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCRelinkInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugIDBInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugIDBInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugENCInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugENCUpdateProxyVtbl,
    0
};

const CInterfaceStubVtbl * _enc_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IDebugENCSnapshot2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugENCSnapshots2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCBuildInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugComPlusSnapshot2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMetaDataEmit2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMetaDataDebugEmit2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugErrorInfos2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCUpdateOnStaleCodeEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCLineMapStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugNativeSnapshot2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugENCRelinkInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCUpdateOnRelinkEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCStateEventsStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCStackFrame2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCRelinkInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugIDBInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugIDBInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugENCInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugENCUpdateStubVtbl,
    0
};

PCInterfaceName const _enc_InterfaceNamesList[] = 
{
    "IDebugENCSnapshot2",
    "IEnumDebugENCSnapshots2",
    "IDebugENCBuildInfo2",
    "IDebugComPlusSnapshot2",
    "IDebugMetaDataEmit2",
    "IDebugMetaDataDebugEmit2",
    "IEnumDebugErrorInfos2",
    "IDebugENCUpdateOnStaleCodeEvent2",
    "IDebugENCLineMap",
    "IDebugNativeSnapshot2",
    "IEnumDebugENCRelinkInfo2",
    "IDebugENCUpdateOnRelinkEvent2",
    "IDebugENCStateEvents",
    "IDebugENCInfo2",
    "IDebugENCStackFrame2",
    "IDebugENCRelinkInfo2",
    "IEnumDebugIDBInfo2",
    "IDebugIDBInfo2",
    "IDebugENC",
    "IEnumDebugENCInfo2",
    "IDebugENCUpdate",
    0
};


#define _enc_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _enc, pIID, n)

int __stdcall _enc_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _enc, 21, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _enc, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _enc, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _enc, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _enc, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _enc, 21, *pIndex )
    
}

const ExtendedProxyFileInfo enc_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _enc_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _enc_StubVtblList,
    (const PCInterfaceName * ) & _enc_InterfaceNamesList,
    0, // no delegation
    & _enc_IID_Lookup, 
    21,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

