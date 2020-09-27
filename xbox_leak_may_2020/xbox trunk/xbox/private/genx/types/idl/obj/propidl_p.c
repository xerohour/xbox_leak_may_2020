
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:28 2002
 */
/* Compiler settings for propidl.idl:
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


#include "propidl.h"

#define TYPE_FORMAT_STRING_SIZE   2611                              
#define PROC_FORMAT_STRING_SIZE   883                               
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


extern const MIDL_SERVER_INFO IPropertyStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPropertyStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPropertySetStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPropertySetStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATPROPSTG_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPSTG_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSTG_RemoteNext_Proxy( 
    IEnumSTATPROPSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSTG *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[684],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumSTATPROPSTG_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumSTATPROPSTG *This;
        ULONG celt;
        STATPROPSTG *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumSTATPROPSTG_Next_Stub(
                                     (IEnumSTATPROPSTG *) pParamStruct->This,
                                     pParamStruct->celt,
                                     pParamStruct->rgelt,
                                     pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATPROPSETSTG_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPSETSTG_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSETSTG_RemoteNext_Proxy( 
    IEnumSTATPROPSETSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSETSTG *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[834],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumSTATPROPSETSTG_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumSTATPROPSETSTG *This;
        ULONG celt;
        STATPROPSETSTG *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumSTATPROPSETSTG_Next_Stub(
                                        (IEnumSTATPROPSETSTG *) pParamStruct->This,
                                        pParamStruct->celt,
                                        pParamStruct->rgelt,
                                        pParamStruct->pceltFetched);
    
}


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
/*  8 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 18 */	NdrFcShort( 0x44 ),	/* 68 */
/* 20 */	NdrFcShort( 0x2 ),	/* 2 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpspec */

/* 24 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpspec */

/* 30 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	NdrFcShort( 0x30 ),	/* Type Offset=48 */

	/* Parameter rgpropvar */

/* 36 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 38 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 40 */	NdrFcShort( 0x918 ),	/* Type Offset=2328 */

	/* Return value */

/* 42 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteMultiple */

/* 48 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 50 */	NdrFcLong( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x4 ),	/* 4 */
/* 56 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 58 */	NdrFcShort( 0x10 ),	/* 16 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 64 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x46 ),	/* 70 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpspec */

/* 72 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpspec */

/* 78 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	NdrFcShort( 0x30 ),	/* Type Offset=48 */

	/* Parameter rgpropvar */

/* 84 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 86 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 88 */	NdrFcShort( 0x918 ),	/* Type Offset=2328 */

	/* Parameter propidNameFirst */

/* 90 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 92 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 94 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 96 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 98 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteMultiple */

/* 102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x5 ),	/* 5 */
/* 110 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 114 */	NdrFcShort( 0x8 ),	/* 8 */
/* 116 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 118 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x2 ),	/* 2 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpspec */

/* 126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 128 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpspec */

/* 132 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 134 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 136 */	NdrFcShort( 0x30 ),	/* Type Offset=48 */

	/* Return value */

/* 138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 140 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReadPropertyNames */

/* 144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x6 ),	/* 6 */
/* 152 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 154 */	NdrFcShort( 0x8 ),	/* 8 */
/* 156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 158 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 160 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 162 */	NdrFcShort( 0x1 ),	/* 1 */
/* 164 */	NdrFcShort( 0x1 ),	/* 1 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpropid */

/* 168 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpropid */

/* 174 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 178 */	NdrFcShort( 0x92e ),	/* Type Offset=2350 */

	/* Parameter rglpwstrName */

/* 180 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 184 */	NdrFcShort( 0x93a ),	/* Type Offset=2362 */

	/* Return value */

/* 186 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 188 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WritePropertyNames */

/* 192 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 194 */	NdrFcLong( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x7 ),	/* 7 */
/* 200 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 204 */	NdrFcShort( 0x8 ),	/* 8 */
/* 206 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 208 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x2 ),	/* 2 */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpropid */

/* 216 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 218 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpropid */

/* 222 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 224 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 226 */	NdrFcShort( 0x92e ),	/* Type Offset=2350 */

	/* Parameter rglpwstrName */

/* 228 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 232 */	NdrFcShort( 0x95a ),	/* Type Offset=2394 */

	/* Return value */

/* 234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 236 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeletePropertyNames */

/* 240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 248 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 250 */	NdrFcShort( 0x8 ),	/* 8 */
/* 252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 254 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 256 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 260 */	NdrFcShort( 0x1 ),	/* 1 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpropid */

/* 264 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 266 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpropid */

/* 270 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 272 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 274 */	NdrFcShort( 0x92e ),	/* Type Offset=2350 */

	/* Return value */

/* 276 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 278 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Commit */

/* 282 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0x9 ),	/* 9 */
/* 290 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 292 */	NdrFcShort( 0x8 ),	/* 8 */
/* 294 */	NdrFcShort( 0x8 ),	/* 8 */
/* 296 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 298 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 304 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfCommitFlags */

/* 306 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 308 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 312 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 314 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Revert */

/* 318 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 320 */	NdrFcLong( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0xa ),	/* 10 */
/* 326 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x8 ),	/* 8 */
/* 332 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 334 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 336 */	NdrFcShort( 0x0 ),	/* 0 */
/* 338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 340 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 342 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 344 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Enum */

/* 348 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0xb ),	/* 11 */
/* 356 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x8 ),	/* 8 */
/* 362 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 364 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 372 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 374 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 376 */	NdrFcShort( 0x97a ),	/* Type Offset=2426 */

	/* Return value */

/* 378 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 380 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTimes */

/* 384 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 390 */	NdrFcShort( 0xc ),	/* 12 */
/* 392 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 394 */	NdrFcShort( 0x84 ),	/* 132 */
/* 396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 398 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 400 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 406 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pctime */

/* 408 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 410 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 412 */	NdrFcShort( 0x2a2 ),	/* Type Offset=674 */

	/* Parameter patime */

/* 414 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 416 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 418 */	NdrFcShort( 0x2a2 ),	/* Type Offset=674 */

	/* Parameter pmtime */

/* 420 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 422 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 424 */	NdrFcShort( 0x2a2 ),	/* Type Offset=674 */

	/* Return value */

/* 426 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 428 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetClass */

/* 432 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 434 */	NdrFcLong( 0x0 ),	/* 0 */
/* 438 */	NdrFcShort( 0xd ),	/* 13 */
/* 440 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 442 */	NdrFcShort( 0x44 ),	/* 68 */
/* 444 */	NdrFcShort( 0x8 ),	/* 8 */
/* 446 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 448 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 450 */	NdrFcShort( 0x0 ),	/* 0 */
/* 452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 454 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter clsid */

/* 456 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 458 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 460 */	NdrFcShort( 0x2b4 ),	/* Type Offset=692 */

	/* Return value */

/* 462 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 464 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 466 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stat */

/* 468 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 470 */	NdrFcLong( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0xe ),	/* 14 */
/* 476 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 480 */	NdrFcShort( 0xdc ),	/* 220 */
/* 482 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 484 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstatpsstg */

/* 492 */	NdrFcShort( 0x112 ),	/* Flags:  must free, out, simple ref, */
/* 494 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 496 */	NdrFcShort( 0x99c ),	/* Type Offset=2460 */

	/* Return value */

/* 498 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 500 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Create */

/* 504 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 506 */	NdrFcLong( 0x0 ),	/* 0 */
/* 510 */	NdrFcShort( 0x3 ),	/* 3 */
/* 512 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 514 */	NdrFcShort( 0x98 ),	/* 152 */
/* 516 */	NdrFcShort( 0x8 ),	/* 8 */
/* 518 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 520 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 526 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rfmtid */

/* 528 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 530 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 532 */	NdrFcShort( 0x2b4 ),	/* Type Offset=692 */

	/* Parameter pclsid */

/* 534 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 536 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 538 */	NdrFcShort( 0x9b8 ),	/* Type Offset=2488 */

	/* Parameter grfFlags */

/* 540 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 542 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 544 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter grfMode */

/* 546 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 548 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 550 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppprstg */

/* 552 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 554 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 556 */	NdrFcShort( 0x9bc ),	/* Type Offset=2492 */

	/* Return value */

/* 558 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 560 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 562 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Open */

/* 564 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 566 */	NdrFcLong( 0x0 ),	/* 0 */
/* 570 */	NdrFcShort( 0x4 ),	/* 4 */
/* 572 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 574 */	NdrFcShort( 0x4c ),	/* 76 */
/* 576 */	NdrFcShort( 0x8 ),	/* 8 */
/* 578 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 580 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rfmtid */

/* 588 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 590 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 592 */	NdrFcShort( 0x2b4 ),	/* Type Offset=692 */

	/* Parameter grfMode */

/* 594 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 596 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppprstg */

/* 600 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 602 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 604 */	NdrFcShort( 0x9bc ),	/* Type Offset=2492 */

	/* Return value */

/* 606 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 608 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Delete */

/* 612 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 614 */	NdrFcLong( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x5 ),	/* 5 */
/* 620 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 622 */	NdrFcShort( 0x44 ),	/* 68 */
/* 624 */	NdrFcShort( 0x8 ),	/* 8 */
/* 626 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 628 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 634 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rfmtid */

/* 636 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 638 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 640 */	NdrFcShort( 0x2b4 ),	/* Type Offset=692 */

	/* Return value */

/* 642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 644 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */


	/* Procedure Enum */

/* 648 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 654 */	NdrFcShort( 0x6 ),	/* 6 */
/* 656 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 662 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 664 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 670 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */


	/* Parameter ppenum */

/* 672 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 674 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 676 */	NdrFcShort( 0x9d2 ),	/* Type Offset=2514 */

	/* Return value */


	/* Return value */

/* 678 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 680 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 682 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 684 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 686 */	NdrFcLong( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x3 ),	/* 3 */
/* 692 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 694 */	NdrFcShort( 0x8 ),	/* 8 */
/* 696 */	NdrFcShort( 0x24 ),	/* 36 */
/* 698 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 700 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 702 */	NdrFcShort( 0x1 ),	/* 1 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 708 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 710 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 714 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 716 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 718 */	NdrFcShort( 0x9fe ),	/* Type Offset=2558 */

	/* Parameter pceltFetched */

/* 720 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 722 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 724 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 728 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure Skip */

/* 732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 738 */	NdrFcShort( 0x4 ),	/* 4 */
/* 740 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 742 */	NdrFcShort( 0x8 ),	/* 8 */
/* 744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 746 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 748 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */


	/* Parameter celt */

/* 756 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 758 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 762 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 764 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure Reset */

/* 768 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 770 */	NdrFcLong( 0x0 ),	/* 0 */
/* 774 */	NdrFcShort( 0x5 ),	/* 5 */
/* 776 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0x8 ),	/* 8 */
/* 782 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 784 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 792 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 794 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 798 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 800 */	NdrFcLong( 0x0 ),	/* 0 */
/* 804 */	NdrFcShort( 0x6 ),	/* 6 */
/* 806 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 810 */	NdrFcShort( 0x8 ),	/* 8 */
/* 812 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 814 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 820 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 822 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 824 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 826 */	NdrFcShort( 0x97a ),	/* Type Offset=2426 */

	/* Return value */

/* 828 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 830 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 834 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 836 */	NdrFcLong( 0x0 ),	/* 0 */
/* 840 */	NdrFcShort( 0x3 ),	/* 3 */
/* 842 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 844 */	NdrFcShort( 0x8 ),	/* 8 */
/* 846 */	NdrFcShort( 0x24 ),	/* 36 */
/* 848 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 850 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 852 */	NdrFcShort( 0x1 ),	/* 1 */
/* 854 */	NdrFcShort( 0x0 ),	/* 0 */
/* 856 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 858 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 860 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 864 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 866 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 868 */	NdrFcShort( 0xa1c ),	/* Type Offset=2588 */

	/* Parameter pceltFetched */

/* 870 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 872 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 874 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 876 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 878 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 880 */	0x8,		/* FC_LONG */
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
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/*  4 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/*  6 */	NdrFcShort( 0xfffc ),	/* -4 */
/*  8 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 10 */	NdrFcShort( 0x2 ),	/* Offset= 2 (12) */
/* 12 */	NdrFcShort( 0x4 ),	/* 4 */
/* 14 */	NdrFcShort( 0x2 ),	/* 2 */
/* 16 */	NdrFcLong( 0x1 ),	/* 1 */
/* 20 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 22 */	NdrFcLong( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x4 ),	/* Offset= 4 (30) */
/* 28 */	NdrFcShort( 0x0 ),	/* Offset= 0 (28) */
/* 30 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 32 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 34 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 36 */	NdrFcShort( 0x8 ),	/* 8 */
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x0 ),	/* Offset= 0 (40) */
/* 42 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 44 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd5 ),	/* Offset= -43 (2) */
			0x5b,		/* FC_END */
/* 48 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 50 */	NdrFcShort( 0x0 ),	/* 0 */
/* 52 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 54 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 56 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 58 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 62 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 64 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 66 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (34) */
/* 68 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 70 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x7,		/* FC_USHORT */
/* 72 */	0x0,		/* Corr desc:  */
			0x59,		/* FC_CALLBACK */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 78 */	NdrFcShort( 0x2 ),	/* Offset= 2 (80) */
/* 80 */	NdrFcShort( 0x8 ),	/* 8 */
/* 82 */	NdrFcShort( 0x61 ),	/* 97 */
/* 84 */	NdrFcLong( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x0 ),	/* Offset= 0 (88) */
/* 90 */	NdrFcLong( 0x1 ),	/* 1 */
/* 94 */	NdrFcShort( 0x0 ),	/* Offset= 0 (94) */
/* 96 */	NdrFcLong( 0x10 ),	/* 16 */
/* 100 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 102 */	NdrFcLong( 0x11 ),	/* 17 */
/* 106 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 108 */	NdrFcLong( 0x2 ),	/* 2 */
/* 112 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 114 */	NdrFcLong( 0x12 ),	/* 18 */
/* 118 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 120 */	NdrFcLong( 0x3 ),	/* 3 */
/* 124 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 126 */	NdrFcLong( 0x13 ),	/* 19 */
/* 130 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 132 */	NdrFcLong( 0x16 ),	/* 22 */
/* 136 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 138 */	NdrFcLong( 0x17 ),	/* 23 */
/* 142 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 144 */	NdrFcLong( 0xe ),	/* 14 */
/* 148 */	NdrFcShort( 0x208 ),	/* Offset= 520 (668) */
/* 150 */	NdrFcLong( 0x14 ),	/* 20 */
/* 154 */	NdrFcShort( 0x202 ),	/* Offset= 514 (668) */
/* 156 */	NdrFcLong( 0x15 ),	/* 21 */
/* 160 */	NdrFcShort( 0x1fc ),	/* Offset= 508 (668) */
/* 162 */	NdrFcLong( 0x4 ),	/* 4 */
/* 166 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 168 */	NdrFcLong( 0x5 ),	/* 5 */
/* 172 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 174 */	NdrFcLong( 0xb ),	/* 11 */
/* 178 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 180 */	NdrFcLong( 0xffff ),	/* 65535 */
/* 184 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 186 */	NdrFcLong( 0xa ),	/* 10 */
/* 190 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 192 */	NdrFcLong( 0x6 ),	/* 6 */
/* 196 */	NdrFcShort( 0x1d8 ),	/* Offset= 472 (668) */
/* 198 */	NdrFcLong( 0x7 ),	/* 7 */
/* 202 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 204 */	NdrFcLong( 0x40 ),	/* 64 */
/* 208 */	NdrFcShort( 0x1d2 ),	/* Offset= 466 (674) */
/* 210 */	NdrFcLong( 0x48 ),	/* 72 */
/* 214 */	NdrFcShort( 0x1d4 ),	/* Offset= 468 (682) */
/* 216 */	NdrFcLong( 0x47 ),	/* 71 */
/* 220 */	NdrFcShort( 0x1e4 ),	/* Offset= 484 (704) */
/* 222 */	NdrFcLong( 0x8 ),	/* 8 */
/* 226 */	NdrFcShort( 0x21e ),	/* Offset= 542 (768) */
/* 228 */	NdrFcLong( 0xfff ),	/* 4095 */
/* 232 */	NdrFcShort( 0x22e ),	/* Offset= 558 (790) */
/* 234 */	NdrFcLong( 0x41 ),	/* 65 */
/* 238 */	NdrFcShort( 0x228 ),	/* Offset= 552 (790) */
/* 240 */	NdrFcLong( 0x46 ),	/* 70 */
/* 244 */	NdrFcShort( 0x222 ),	/* Offset= 546 (790) */
/* 246 */	NdrFcLong( 0x1e ),	/* 30 */
/* 250 */	NdrFcShort( 0x230 ),	/* Offset= 560 (810) */
/* 252 */	NdrFcLong( 0x1f ),	/* 31 */
/* 256 */	NdrFcShort( 0x22e ),	/* Offset= 558 (814) */
/* 258 */	NdrFcLong( 0xd ),	/* 13 */
/* 262 */	NdrFcShort( 0x22c ),	/* Offset= 556 (818) */
/* 264 */	NdrFcLong( 0x9 ),	/* 9 */
/* 268 */	NdrFcShort( 0x238 ),	/* Offset= 568 (836) */
/* 270 */	NdrFcLong( 0x42 ),	/* 66 */
/* 274 */	NdrFcShort( 0x244 ),	/* Offset= 580 (854) */
/* 276 */	NdrFcLong( 0x44 ),	/* 68 */
/* 280 */	NdrFcShort( 0x23e ),	/* Offset= 574 (854) */
/* 282 */	NdrFcLong( 0x43 ),	/* 67 */
/* 286 */	NdrFcShort( 0x24a ),	/* Offset= 586 (872) */
/* 288 */	NdrFcLong( 0x45 ),	/* 69 */
/* 292 */	NdrFcShort( 0x244 ),	/* Offset= 580 (872) */
/* 294 */	NdrFcLong( 0x49 ),	/* 73 */
/* 298 */	NdrFcShort( 0x250 ),	/* Offset= 592 (890) */
/* 300 */	NdrFcLong( 0x2010 ),	/* 8208 */
/* 304 */	NdrFcShort( 0x5d0 ),	/* Offset= 1488 (1792) */
/* 306 */	NdrFcLong( 0x2011 ),	/* 8209 */
/* 310 */	NdrFcShort( 0x5ca ),	/* Offset= 1482 (1792) */
/* 312 */	NdrFcLong( 0x2002 ),	/* 8194 */
/* 316 */	NdrFcShort( 0x5c4 ),	/* Offset= 1476 (1792) */
/* 318 */	NdrFcLong( 0x2012 ),	/* 8210 */
/* 322 */	NdrFcShort( 0x5be ),	/* Offset= 1470 (1792) */
/* 324 */	NdrFcLong( 0x2003 ),	/* 8195 */
/* 328 */	NdrFcShort( 0x5b8 ),	/* Offset= 1464 (1792) */
/* 330 */	NdrFcLong( 0x2013 ),	/* 8211 */
/* 334 */	NdrFcShort( 0x5b2 ),	/* Offset= 1458 (1792) */
/* 336 */	NdrFcLong( 0x2016 ),	/* 8214 */
/* 340 */	NdrFcShort( 0x5ac ),	/* Offset= 1452 (1792) */
/* 342 */	NdrFcLong( 0x2017 ),	/* 8215 */
/* 346 */	NdrFcShort( 0x5a6 ),	/* Offset= 1446 (1792) */
/* 348 */	NdrFcLong( 0x2004 ),	/* 8196 */
/* 352 */	NdrFcShort( 0x5a0 ),	/* Offset= 1440 (1792) */
/* 354 */	NdrFcLong( 0x2005 ),	/* 8197 */
/* 358 */	NdrFcShort( 0x59a ),	/* Offset= 1434 (1792) */
/* 360 */	NdrFcLong( 0x2006 ),	/* 8198 */
/* 364 */	NdrFcShort( 0x594 ),	/* Offset= 1428 (1792) */
/* 366 */	NdrFcLong( 0x2007 ),	/* 8199 */
/* 370 */	NdrFcShort( 0x58e ),	/* Offset= 1422 (1792) */
/* 372 */	NdrFcLong( 0x2008 ),	/* 8200 */
/* 376 */	NdrFcShort( 0x588 ),	/* Offset= 1416 (1792) */
/* 378 */	NdrFcLong( 0x200b ),	/* 8203 */
/* 382 */	NdrFcShort( 0x582 ),	/* Offset= 1410 (1792) */
/* 384 */	NdrFcLong( 0x200e ),	/* 8206 */
/* 388 */	NdrFcShort( 0x57c ),	/* Offset= 1404 (1792) */
/* 390 */	NdrFcLong( 0x2009 ),	/* 8201 */
/* 394 */	NdrFcShort( 0x576 ),	/* Offset= 1398 (1792) */
/* 396 */	NdrFcLong( 0x200d ),	/* 8205 */
/* 400 */	NdrFcShort( 0x570 ),	/* Offset= 1392 (1792) */
/* 402 */	NdrFcLong( 0x200a ),	/* 8202 */
/* 406 */	NdrFcShort( 0x56a ),	/* Offset= 1386 (1792) */
/* 408 */	NdrFcLong( 0x200c ),	/* 8204 */
/* 412 */	NdrFcShort( 0x564 ),	/* Offset= 1380 (1792) */
/* 414 */	NdrFcLong( 0x1010 ),	/* 4112 */
/* 418 */	NdrFcShort( 0x574 ),	/* Offset= 1396 (1814) */
/* 420 */	NdrFcLong( 0x1011 ),	/* 4113 */
/* 424 */	NdrFcShort( 0x56e ),	/* Offset= 1390 (1814) */
/* 426 */	NdrFcLong( 0x1002 ),	/* 4098 */
/* 430 */	NdrFcShort( 0x4dc ),	/* Offset= 1244 (1674) */
/* 432 */	NdrFcLong( 0x1012 ),	/* 4114 */
/* 436 */	NdrFcShort( 0x4d6 ),	/* Offset= 1238 (1674) */
/* 438 */	NdrFcLong( 0x1003 ),	/* 4099 */
/* 442 */	NdrFcShort( 0x4f0 ),	/* Offset= 1264 (1706) */
/* 444 */	NdrFcLong( 0x1013 ),	/* 4115 */
/* 448 */	NdrFcShort( 0x4ea ),	/* Offset= 1258 (1706) */
/* 450 */	NdrFcLong( 0x1014 ),	/* 4116 */
/* 454 */	NdrFcShort( 0x574 ),	/* Offset= 1396 (1850) */
/* 456 */	NdrFcLong( 0x1015 ),	/* 4117 */
/* 460 */	NdrFcShort( 0x56e ),	/* Offset= 1390 (1850) */
/* 462 */	NdrFcLong( 0x1004 ),	/* 4100 */
/* 466 */	NdrFcShort( 0x588 ),	/* Offset= 1416 (1882) */
/* 468 */	NdrFcLong( 0x1005 ),	/* 4101 */
/* 472 */	NdrFcShort( 0x5a2 ),	/* Offset= 1442 (1914) */
/* 474 */	NdrFcLong( 0x100b ),	/* 4107 */
/* 478 */	NdrFcShort( 0x4ac ),	/* Offset= 1196 (1674) */
/* 480 */	NdrFcLong( 0x100a ),	/* 4106 */
/* 484 */	NdrFcShort( 0x4c6 ),	/* Offset= 1222 (1706) */
/* 486 */	NdrFcLong( 0x1006 ),	/* 4102 */
/* 490 */	NdrFcShort( 0x550 ),	/* Offset= 1360 (1850) */
/* 492 */	NdrFcLong( 0x1007 ),	/* 4103 */
/* 496 */	NdrFcShort( 0x58a ),	/* Offset= 1418 (1914) */
/* 498 */	NdrFcLong( 0x1040 ),	/* 4160 */
/* 502 */	NdrFcShort( 0x5a8 ),	/* Offset= 1448 (1950) */
/* 504 */	NdrFcLong( 0x1048 ),	/* 4168 */
/* 508 */	NdrFcShort( 0x5c6 ),	/* Offset= 1478 (1986) */
/* 510 */	NdrFcLong( 0x1047 ),	/* 4167 */
/* 514 */	NdrFcShort( 0x5f6 ),	/* Offset= 1526 (2040) */
/* 516 */	NdrFcLong( 0x1008 ),	/* 4104 */
/* 520 */	NdrFcShort( 0x61a ),	/* Offset= 1562 (2082) */
/* 522 */	NdrFcLong( 0x1fff ),	/* 8191 */
/* 526 */	NdrFcShort( 0x64a ),	/* Offset= 1610 (2136) */
/* 528 */	NdrFcLong( 0x101e ),	/* 4126 */
/* 532 */	NdrFcShort( 0x678 ),	/* Offset= 1656 (2188) */
/* 534 */	NdrFcLong( 0x101f ),	/* 4127 */
/* 538 */	NdrFcShort( 0x6a6 ),	/* Offset= 1702 (2240) */
/* 540 */	NdrFcLong( 0x100c ),	/* 4108 */
/* 544 */	NdrFcShort( 0x6ca ),	/* Offset= 1738 (2282) */
/* 546 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 550 */	NdrFcShort( 0x3cc ),	/* Offset= 972 (1522) */
/* 552 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 556 */	NdrFcShort( 0x3c6 ),	/* Offset= 966 (1522) */
/* 558 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 562 */	NdrFcShort( 0x37e ),	/* Offset= 894 (1456) */
/* 564 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 568 */	NdrFcShort( 0x378 ),	/* Offset= 888 (1456) */
/* 570 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 574 */	NdrFcShort( 0x376 ),	/* Offset= 886 (1460) */
/* 576 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 580 */	NdrFcShort( 0x370 ),	/* Offset= 880 (1460) */
/* 582 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 586 */	NdrFcShort( 0x36a ),	/* Offset= 874 (1460) */
/* 588 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 592 */	NdrFcShort( 0x364 ),	/* Offset= 868 (1460) */
/* 594 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 598 */	NdrFcShort( 0x362 ),	/* Offset= 866 (1464) */
/* 600 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 604 */	NdrFcShort( 0x360 ),	/* Offset= 864 (1468) */
/* 606 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 610 */	NdrFcShort( 0x34e ),	/* Offset= 846 (1456) */
/* 612 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 616 */	NdrFcShort( 0x386 ),	/* Offset= 902 (1518) */
/* 618 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 622 */	NdrFcShort( 0x346 ),	/* Offset= 838 (1460) */
/* 624 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 628 */	NdrFcShort( 0x34c ),	/* Offset= 844 (1472) */
/* 630 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 634 */	NdrFcShort( 0x342 ),	/* Offset= 834 (1468) */
/* 636 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 640 */	NdrFcShort( 0x67a ),	/* Offset= 1658 (2298) */
/* 642 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 646 */	NdrFcShort( 0x342 ),	/* Offset= 834 (1480) */
/* 648 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 652 */	NdrFcShort( 0x340 ),	/* Offset= 832 (1484) */
/* 654 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 658 */	NdrFcShort( 0x66c ),	/* Offset= 1644 (2302) */
/* 660 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 664 */	NdrFcShort( 0x66a ),	/* Offset= 1642 (2306) */
/* 666 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (665) */
/* 668 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 670 */	NdrFcShort( 0x8 ),	/* 8 */
/* 672 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 674 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 676 */	NdrFcShort( 0x8 ),	/* 8 */
/* 678 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 680 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 682 */	
			0x13, 0x0,	/* FC_OP */
/* 684 */	NdrFcShort( 0x8 ),	/* Offset= 8 (692) */
/* 686 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 690 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 692 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 694 */	NdrFcShort( 0x10 ),	/* 16 */
/* 696 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 698 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 700 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (686) */
			0x5b,		/* FC_END */
/* 704 */	
			0x13, 0x0,	/* FC_OP */
/* 706 */	NdrFcShort( 0xe ),	/* Offset= 14 (720) */
/* 708 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 710 */	NdrFcShort( 0x1 ),	/* 1 */
/* 712 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 714 */	NdrFcShort( 0x1 ),	/* 1 */
/* 716 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 718 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 720 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 722 */	NdrFcShort( 0xc ),	/* 12 */
/* 724 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 726 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 732 */	0x13, 0x0,	/* FC_OP */
/* 734 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (708) */
/* 736 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 738 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 740 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 742 */	
			0x13, 0x0,	/* FC_OP */
/* 744 */	NdrFcShort( 0xe ),	/* Offset= 14 (758) */
/* 746 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 748 */	NdrFcShort( 0x2 ),	/* 2 */
/* 750 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 752 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 754 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 756 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 758 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 762 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (746) */
/* 764 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 766 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 768 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 772 */	NdrFcShort( 0x4 ),	/* 4 */
/* 774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 776 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (742) */
/* 778 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 780 */	NdrFcShort( 0x1 ),	/* 1 */
/* 782 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 788 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 790 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 792 */	NdrFcShort( 0x8 ),	/* 8 */
/* 794 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 796 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 798 */	NdrFcShort( 0x4 ),	/* 4 */
/* 800 */	NdrFcShort( 0x4 ),	/* 4 */
/* 802 */	0x13, 0x0,	/* FC_OP */
/* 804 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (778) */
/* 806 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 808 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 810 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 812 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 814 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 816 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 818 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 820 */	NdrFcLong( 0x0 ),	/* 0 */
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
/* 838 */	NdrFcLong( 0x20400 ),	/* 132096 */
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
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 856 */	NdrFcLong( 0xc ),	/* 12 */
/* 860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 862 */	NdrFcShort( 0x0 ),	/* 0 */
/* 864 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 866 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 868 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 870 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 872 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 874 */	NdrFcLong( 0xb ),	/* 11 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x0 ),	/* 0 */
/* 882 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 884 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 886 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 888 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 890 */	
			0x13, 0x0,	/* FC_OP */
/* 892 */	NdrFcShort( 0x2 ),	/* Offset= 2 (894) */
/* 894 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 896 */	NdrFcShort( 0x14 ),	/* 20 */
/* 898 */	NdrFcShort( 0x0 ),	/* 0 */
/* 900 */	NdrFcShort( 0xc ),	/* Offset= 12 (912) */
/* 902 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 904 */	NdrFcShort( 0xffffff2c ),	/* Offset= -212 (692) */
/* 906 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 908 */	NdrFcShort( 0xffffffca ),	/* Offset= -54 (854) */
/* 910 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 912 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 914 */	NdrFcShort( 0x2 ),	/* Offset= 2 (916) */
/* 916 */	
			0x13, 0x0,	/* FC_OP */
/* 918 */	NdrFcShort( 0x358 ),	/* Offset= 856 (1774) */
/* 920 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 922 */	NdrFcShort( 0x18 ),	/* 24 */
/* 924 */	NdrFcShort( 0xa ),	/* 10 */
/* 926 */	NdrFcLong( 0x8 ),	/* 8 */
/* 930 */	NdrFcShort( 0x5a ),	/* Offset= 90 (1020) */
/* 932 */	NdrFcLong( 0xd ),	/* 13 */
/* 936 */	NdrFcShort( 0x7e ),	/* Offset= 126 (1062) */
/* 938 */	NdrFcLong( 0x9 ),	/* 9 */
/* 942 */	NdrFcShort( 0x9e ),	/* Offset= 158 (1100) */
/* 944 */	NdrFcLong( 0xc ),	/* 12 */
/* 948 */	NdrFcShort( 0x276 ),	/* Offset= 630 (1578) */
/* 950 */	NdrFcLong( 0x24 ),	/* 36 */
/* 954 */	NdrFcShort( 0x2a0 ),	/* Offset= 672 (1626) */
/* 956 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 960 */	NdrFcShort( 0x2aa ),	/* Offset= 682 (1642) */
/* 962 */	NdrFcLong( 0x10 ),	/* 16 */
/* 966 */	NdrFcShort( 0xffffff50 ),	/* Offset= -176 (790) */
/* 968 */	NdrFcLong( 0x2 ),	/* 2 */
/* 972 */	NdrFcShort( 0x2be ),	/* Offset= 702 (1674) */
/* 974 */	NdrFcLong( 0x3 ),	/* 3 */
/* 978 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (1706) */
/* 980 */	NdrFcLong( 0x14 ),	/* 20 */
/* 984 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (1738) */
/* 986 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (985) */
/* 988 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 990 */	NdrFcShort( 0x4 ),	/* 4 */
/* 992 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 996 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 998 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1000 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1002 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1012 */	0x13, 0x0,	/* FC_OP */
/* 1014 */	NdrFcShort( 0xffffff00 ),	/* Offset= -256 (758) */
/* 1016 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1018 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1020 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1022 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1024 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1026 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1028 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1030 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1032 */	0x11, 0x0,	/* FC_RP */
/* 1034 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (988) */
/* 1036 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1038 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1040 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1044 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1048 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1050 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1054 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1056 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1058 */	NdrFcShort( 0xffffff10 ),	/* Offset= -240 (818) */
/* 1060 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1062 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1064 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1068 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1074) */
/* 1070 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1072 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1074 */	
			0x11, 0x0,	/* FC_RP */
/* 1076 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1040) */
/* 1078 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1082 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1086 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1088 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1092 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1094 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1096 */	NdrFcShort( 0xfffffefc ),	/* Offset= -260 (836) */
/* 1098 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1100 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1102 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1106 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1112) */
/* 1108 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1110 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1112 */	
			0x11, 0x0,	/* FC_RP */
/* 1114 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1078) */
/* 1116 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1118 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1120 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 1122 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1124 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1126) */
/* 1126 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1128 */	NdrFcShort( 0x2b ),	/* 43 */
/* 1130 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1134 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1136 */	NdrFcLong( 0x11 ),	/* 17 */
/* 1140 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 1142 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1146 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1148 */	NdrFcLong( 0x4 ),	/* 4 */
/* 1152 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 1154 */	NdrFcLong( 0x5 ),	/* 5 */
/* 1158 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 1160 */	NdrFcLong( 0xb ),	/* 11 */
/* 1164 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1166 */	NdrFcLong( 0xa ),	/* 10 */
/* 1170 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1172 */	NdrFcLong( 0x6 ),	/* 6 */
/* 1176 */	NdrFcShort( 0xfffffe04 ),	/* Offset= -508 (668) */
/* 1178 */	NdrFcLong( 0x7 ),	/* 7 */
/* 1182 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 1184 */	NdrFcLong( 0x8 ),	/* 8 */
/* 1188 */	NdrFcShort( 0xfffffe42 ),	/* Offset= -446 (742) */
/* 1190 */	NdrFcLong( 0xd ),	/* 13 */
/* 1194 */	NdrFcShort( 0xfffffe88 ),	/* Offset= -376 (818) */
/* 1196 */	NdrFcLong( 0x9 ),	/* 9 */
/* 1200 */	NdrFcShort( 0xfffffe94 ),	/* Offset= -364 (836) */
/* 1202 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 1206 */	NdrFcShort( 0xb8 ),	/* Offset= 184 (1390) */
/* 1208 */	NdrFcLong( 0x24 ),	/* 36 */
/* 1212 */	NdrFcShort( 0xba ),	/* Offset= 186 (1398) */
/* 1214 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 1218 */	NdrFcShort( 0xb4 ),	/* Offset= 180 (1398) */
/* 1220 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 1224 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (1452) */
/* 1226 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 1230 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (1456) */
/* 1232 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 1236 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (1460) */
/* 1238 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 1242 */	NdrFcShort( 0xde ),	/* Offset= 222 (1464) */
/* 1244 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 1248 */	NdrFcShort( 0xdc ),	/* Offset= 220 (1468) */
/* 1250 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 1254 */	NdrFcShort( 0xca ),	/* Offset= 202 (1456) */
/* 1256 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 1260 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (1460) */
/* 1262 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 1266 */	NdrFcShort( 0xce ),	/* Offset= 206 (1472) */
/* 1268 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 1272 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1468) */
/* 1274 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 1278 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (1476) */
/* 1280 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 1284 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1480) */
/* 1286 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 1290 */	NdrFcShort( 0xc2 ),	/* Offset= 194 (1484) */
/* 1292 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 1296 */	NdrFcShort( 0xc0 ),	/* Offset= 192 (1488) */
/* 1298 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 1302 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (1500) */
/* 1304 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1308 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 1310 */	NdrFcLong( 0x12 ),	/* 18 */
/* 1314 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1316 */	NdrFcLong( 0x13 ),	/* 19 */
/* 1320 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1322 */	NdrFcLong( 0x16 ),	/* 22 */
/* 1326 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1328 */	NdrFcLong( 0x17 ),	/* 23 */
/* 1332 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1334 */	NdrFcLong( 0xe ),	/* 14 */
/* 1338 */	NdrFcShort( 0xaa ),	/* Offset= 170 (1508) */
/* 1340 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 1344 */	NdrFcShort( 0xae ),	/* Offset= 174 (1518) */
/* 1346 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 1350 */	NdrFcShort( 0xac ),	/* Offset= 172 (1522) */
/* 1352 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 1356 */	NdrFcShort( 0x64 ),	/* Offset= 100 (1456) */
/* 1358 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 1362 */	NdrFcShort( 0x62 ),	/* Offset= 98 (1460) */
/* 1364 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 1368 */	NdrFcShort( 0x5c ),	/* Offset= 92 (1460) */
/* 1370 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 1374 */	NdrFcShort( 0x56 ),	/* Offset= 86 (1460) */
/* 1376 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1380 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1380) */
/* 1382 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1386 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1386) */
/* 1388 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1387) */
/* 1390 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1392 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1394) */
/* 1394 */	
			0x13, 0x0,	/* FC_OP */
/* 1396 */	NdrFcShort( 0x17a ),	/* Offset= 378 (1774) */
/* 1398 */	
			0x13, 0x0,	/* FC_OP */
/* 1400 */	NdrFcShort( 0x20 ),	/* Offset= 32 (1432) */
/* 1402 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1404 */	NdrFcLong( 0x2f ),	/* 47 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1412 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1414 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1416 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1418 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1420 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1422 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1424 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1426 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1428 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1430 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1432 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1434 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1438 */	NdrFcShort( 0xa ),	/* Offset= 10 (1448) */
/* 1440 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1442 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1444 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (1402) */
/* 1446 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1448 */	
			0x13, 0x0,	/* FC_OP */
/* 1450 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1420) */
/* 1452 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1454 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1456 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1458 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1460 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1462 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1464 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1466 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1468 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1470 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1472 */	
			0x13, 0x0,	/* FC_OP */
/* 1474 */	NdrFcShort( 0xfffffcda ),	/* Offset= -806 (668) */
/* 1476 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1478 */	NdrFcShort( 0xfffffd20 ),	/* Offset= -736 (742) */
/* 1480 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1482 */	NdrFcShort( 0xfffffd68 ),	/* Offset= -664 (818) */
/* 1484 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1486 */	NdrFcShort( 0xfffffd76 ),	/* Offset= -650 (836) */
/* 1488 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1490 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1492) */
/* 1492 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1494 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1496) */
/* 1496 */	
			0x13, 0x0,	/* FC_OP */
/* 1498 */	NdrFcShort( 0x114 ),	/* Offset= 276 (1774) */
/* 1500 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1502 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1504) */
/* 1504 */	
			0x13, 0x0,	/* FC_OP */
/* 1506 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1526) */
/* 1508 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1510 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1512 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1514 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1516 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1518 */	
			0x13, 0x0,	/* FC_OP */
/* 1520 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1508) */
/* 1522 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1524 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1526 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1528 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1532 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1532) */
/* 1534 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1536 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1538 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1540 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1542 */	NdrFcShort( 0xfffffe56 ),	/* Offset= -426 (1116) */
/* 1544 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1546 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1548 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1550 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1554 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1556 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1558 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1560 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1564 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1570 */	0x13, 0x0,	/* FC_OP */
/* 1572 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (1526) */
/* 1574 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1576 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1578 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1580 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1584 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1590) */
/* 1586 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1588 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1590 */	
			0x11, 0x0,	/* FC_RP */
/* 1592 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (1546) */
/* 1594 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1596 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1598 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1602 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1604 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1606 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1608 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1612 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1618 */	0x13, 0x0,	/* FC_OP */
/* 1620 */	NdrFcShort( 0xffffff44 ),	/* Offset= -188 (1432) */
/* 1622 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1624 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1626 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1628 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1632 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1638) */
/* 1634 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1636 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1638 */	
			0x11, 0x0,	/* FC_RP */
/* 1640 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (1594) */
/* 1642 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1644 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1648 */	NdrFcShort( 0xa ),	/* Offset= 10 (1658) */
/* 1650 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1652 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1654 */	NdrFcShort( 0xfffffc3e ),	/* Offset= -962 (692) */
/* 1656 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1658 */	
			0x11, 0x0,	/* FC_RP */
/* 1660 */	NdrFcShort( 0xfffffd94 ),	/* Offset= -620 (1040) */
/* 1662 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1664 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1666 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1670 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1672 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1674 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1676 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1678 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1680 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1682 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1684 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1686 */	0x13, 0x0,	/* FC_OP */
/* 1688 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1662) */
/* 1690 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1692 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1694 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1696 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1698 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1702 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1704 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1706 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1708 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1710 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1712 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1714 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1716 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1718 */	0x13, 0x0,	/* FC_OP */
/* 1720 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1694) */
/* 1722 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1724 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1726 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1730 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1734 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1736 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1738 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1740 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1742 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1744 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1746 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1748 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1750 */	0x13, 0x0,	/* FC_OP */
/* 1752 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1726) */
/* 1754 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1756 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1758 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1762 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1764 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 1766 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1768 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1770 */	NdrFcShort( 0xfffffbb8 ),	/* Offset= -1096 (674) */
/* 1772 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1774 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1776 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1778 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (1758) */
/* 1780 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1780) */
/* 1782 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1784 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1786 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1788 */	NdrFcShort( 0xfffffc9c ),	/* Offset= -868 (920) */
/* 1790 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1792 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1794 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1796 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1800 */	NdrFcShort( 0xfffffc88 ),	/* Offset= -888 (912) */
/* 1802 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1804 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1806 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1810 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1812 */	0x2,		/* FC_CHAR */
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
/* 1828 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1802) */
/* 1830 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1832 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1834 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1836 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1838 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1842 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1844 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1846 */	NdrFcShort( 0xfffffb66 ),	/* Offset= -1178 (668) */
/* 1848 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1850 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1852 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1854 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1856 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1858 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1860 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1862 */	0x13, 0x0,	/* FC_OP */
/* 1864 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1834) */
/* 1866 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1868 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1870 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1872 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1874 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1878 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1880 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 1882 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1884 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1886 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1888 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1890 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1892 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1894 */	0x13, 0x0,	/* FC_OP */
/* 1896 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1870) */
/* 1898 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1900 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1902 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1904 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1906 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1910 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1912 */	0xc,		/* FC_DOUBLE */
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
/* 1928 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1902) */
/* 1930 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1932 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1934 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1936 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1938 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1940 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1942 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1944 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1946 */	NdrFcShort( 0xfffffb08 ),	/* Offset= -1272 (674) */
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
/* 1972 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1974 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1976 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1978 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1980 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1982 */	NdrFcShort( 0xfffffaf6 ),	/* Offset= -1290 (692) */
/* 1984 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1986 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1988 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1990 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1992 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1994 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1996 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1998 */	0x13, 0x0,	/* FC_OP */
/* 2000 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1970) */
/* 2002 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2004 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2006 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2008 */	NdrFcShort( 0xc ),	/* 12 */
/* 2010 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2014 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2016 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2018 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2020 */	NdrFcShort( 0xc ),	/* 12 */
/* 2022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2024 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2026 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2028 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2030 */	0x13, 0x0,	/* FC_OP */
/* 2032 */	NdrFcShort( 0xfffffad4 ),	/* Offset= -1324 (708) */
/* 2034 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2036 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffadb ),	/* Offset= -1317 (720) */
			0x5b,		/* FC_END */
/* 2040 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2042 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2044 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2046 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2048 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2050 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2052 */	0x13, 0x0,	/* FC_OP */
/* 2054 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (2006) */
/* 2056 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2058 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2060 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2064 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2068 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2070 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2074 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2076 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2078 */	NdrFcShort( 0xfffffae2 ),	/* Offset= -1310 (768) */
/* 2080 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2082 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2084 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2086 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2088 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2090 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2092 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2094 */	0x13, 0x0,	/* FC_OP */
/* 2096 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2060) */
/* 2098 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2100 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2102 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2106 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2110 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2112 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2114 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2116 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2120 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2122 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2124 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2126 */	0x13, 0x0,	/* FC_OP */
/* 2128 */	NdrFcShort( 0xfffffaba ),	/* Offset= -1350 (778) */
/* 2130 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2132 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffac1 ),	/* Offset= -1343 (790) */
			0x5b,		/* FC_END */
/* 2136 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2140 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2142 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2144 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2146 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2148 */	0x13, 0x0,	/* FC_OP */
/* 2150 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (2102) */
/* 2152 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2154 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2156 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2158 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2160 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2164 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2166 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2168 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2170 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2174 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2180 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2182 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 2184 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2186 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2188 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2192 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2194 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2196 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2198 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2200 */	0x13, 0x0,	/* FC_OP */
/* 2202 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (2156) */
/* 2204 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2206 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2208 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2210 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2212 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2216 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2218 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2220 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2222 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2226 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2232 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2234 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2236 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2238 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2240 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2242 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2244 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2246 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2248 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2250 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2252 */	0x13, 0x0,	/* FC_OP */
/* 2254 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (2208) */
/* 2256 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2258 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2260 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 2262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2264 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2268 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2270 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2274 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2276 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2278 */	NdrFcShort( 0x20 ),	/* Offset= 32 (2310) */
/* 2280 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2282 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2288 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2294) */
/* 2290 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 2292 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2294 */	
			0x13, 0x0,	/* FC_OP */
/* 2296 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2260) */
/* 2298 */	
			0x13, 0x0,	/* FC_OP */
/* 2300 */	NdrFcShort( 0xfffffa04 ),	/* Offset= -1532 (768) */
/* 2302 */	
			0x13, 0x0,	/* FC_OP */
/* 2304 */	NdrFcShort( 0xfffffe00 ),	/* Offset= -512 (1792) */
/* 2306 */	
			0x13, 0x0,	/* FC_OP */
/* 2308 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2310) */
/* 2310 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 2312 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2316 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2316) */
/* 2318 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 2320 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 2322 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2324 */	NdrFcShort( 0xfffff732 ),	/* Offset= -2254 (70) */
/* 2326 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2328 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 2330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2332 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2334 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2336 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2338 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2342 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2344 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2346 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2310) */
/* 2348 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2350 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2352 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2354 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2356 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2358 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2360 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2362 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2364 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2366 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2368 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2370 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2372 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2374 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2376 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2380 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2386 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2388 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2390 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2392 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2394 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2396 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2398 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2400 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2402 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2404 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2406 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2408 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2412 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2418 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2420 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2422 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2424 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2426 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2428 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2430) */
/* 2430 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2432 */	NdrFcLong( 0x139 ),	/* 313 */
/* 2436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2440 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 2442 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2444 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2446 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 2448 */	
			0x11, 0x0,	/* FC_RP */
/* 2450 */	NdrFcShort( 0xfffff910 ),	/* Offset= -1776 (674) */
/* 2452 */	
			0x11, 0x0,	/* FC_RP */
/* 2454 */	NdrFcShort( 0xfffff91e ),	/* Offset= -1762 (692) */
/* 2456 */	
			0x11, 0x0,	/* FC_RP */
/* 2458 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2460) */
/* 2460 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 2462 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2464 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2466 */	NdrFcShort( 0xfffff912 ),	/* Offset= -1774 (692) */
/* 2468 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2470 */	NdrFcShort( 0xfffff90e ),	/* Offset= -1778 (692) */
/* 2472 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2474 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff8f7 ),	/* Offset= -1801 (674) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2478 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff8f3 ),	/* Offset= -1805 (674) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2482 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff8ef ),	/* Offset= -1809 (674) */
			0x8,		/* FC_LONG */
/* 2486 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2488 */	
			0x12, 0x0,	/* FC_UP */
/* 2490 */	NdrFcShort( 0xfffff8fa ),	/* Offset= -1798 (692) */
/* 2492 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2494 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2496) */
/* 2496 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2498 */	NdrFcLong( 0x138 ),	/* 312 */
/* 2502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2506 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 2508 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2510 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2512 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 2514 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2516 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2518) */
/* 2518 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2520 */	NdrFcLong( 0x13b ),	/* 315 */
/* 2524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2528 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 2530 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2532 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2534 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 2536 */	
			0x11, 0x0,	/* FC_RP */
/* 2538 */	NdrFcShort( 0x14 ),	/* Offset= 20 (2558) */
/* 2540 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2542 */	NdrFcShort( 0xc ),	/* 12 */
/* 2544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2546 */	NdrFcShort( 0x8 ),	/* Offset= 8 (2554) */
/* 2548 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2550 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 2552 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2554 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2556 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2558 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2562 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2564 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2566 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2568 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 2570 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2572 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2574 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2576 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2540) */
/* 2578 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2580 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 2582 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 2584 */	
			0x11, 0x0,	/* FC_RP */
/* 2586 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2588) */
/* 2588 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 2590 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2592 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2594 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2596 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2598 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 2600 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2602 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2604 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2606 */	NdrFcShort( 0xffffff6e ),	/* Offset= -146 (2460) */
/* 2608 */	0x5c,		/* FC_PAD */
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
            },
            {
            LPSAFEARRAY_UserSize
            ,LPSAFEARRAY_UserMarshal
            ,LPSAFEARRAY_UserUnmarshal
            ,LPSAFEARRAY_UserFree
            }

        };


static void __RPC_USER IPropertyStorage_PROPVARIANTExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    PROPVARIANT *pS	=	( PROPVARIANT * )(pStubMsg->StackTop - 8);
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( ( unsigned short  )pS->vt );
}

static void __RPC_USER IPropertyStorage_CLIPDATAExprEval_0001( PMIDL_STUB_MESSAGE pStubMsg )
{
    CLIPDATA *pS	=	( CLIPDATA * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( pS->cbSize - 4 );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IPropertyStorage_PROPVARIANTExprEval_0000
    ,IPropertyStorage_CLIPDATAExprEval_0001
    };



/* Standard interface: __MIDL_itf_propidl_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IPropertyStorage, ver. 0.0,
   GUID={0x00000138,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPropertyStorage_FormatStringOffsetTable[] =
    {
    0,
    48,
    102,
    144,
    192,
    240,
    282,
    318,
    348,
    384,
    432,
    468
    };

static const MIDL_STUBLESS_PROXY_INFO IPropertyStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPropertyStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPropertyStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPropertyStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _IPropertyStorageProxyVtbl = 
{
    &IPropertyStorage_ProxyInfo,
    &IID_IPropertyStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::ReadMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::WriteMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::DeleteMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::ReadPropertyNames */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::WritePropertyNames */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::DeletePropertyNames */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::Commit */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::Revert */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::Enum */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::SetTimes */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::SetClass */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::Stat */
};

const CInterfaceStubVtbl _IPropertyStorageStubVtbl =
{
    &IID_IPropertyStorage,
    &IPropertyStorage_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPropertySetStorage, ver. 0.0,
   GUID={0x0000013A,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPropertySetStorage_FormatStringOffsetTable[] =
    {
    504,
    564,
    612,
    648
    };

static const MIDL_STUBLESS_PROXY_INFO IPropertySetStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPropertySetStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPropertySetStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPropertySetStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IPropertySetStorageProxyVtbl = 
{
    &IPropertySetStorage_ProxyInfo,
    &IID_IPropertySetStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPropertySetStorage::Create */ ,
    (void *) (INT_PTR) -1 /* IPropertySetStorage::Open */ ,
    (void *) (INT_PTR) -1 /* IPropertySetStorage::Delete */ ,
    (void *) (INT_PTR) -1 /* IPropertySetStorage::Enum */
};

const CInterfaceStubVtbl _IPropertySetStorageStubVtbl =
{
    &IID_IPropertySetStorage,
    &IPropertySetStorage_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATPROPSTG, ver. 0.0,
   GUID={0x00000139,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATPROPSTG_FormatStringOffsetTable[] =
    {
    684,
    732,
    768,
    798
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPSTG_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPSTG_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumSTATPROPSTG_StubThunkTable[] = 
    {
    IEnumSTATPROPSTG_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumSTATPROPSTG_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPSTG_FormatStringOffsetTable[-3],
    &IEnumSTATPROPSTG_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATPROPSTGProxyVtbl = 
{
    &IEnumSTATPROPSTG_ProxyInfo,
    &IID_IEnumSTATPROPSTG,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumSTATPROPSTG_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSTG::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSTG::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSTG::Clone */
};

const CInterfaceStubVtbl _IEnumSTATPROPSTGStubVtbl =
{
    &IID_IEnumSTATPROPSTG,
    &IEnumSTATPROPSTG_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATPROPSETSTG, ver. 0.0,
   GUID={0x0000013B,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATPROPSETSTG_FormatStringOffsetTable[] =
    {
    834,
    732,
    768,
    648
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPSETSTG_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPSETSTG_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumSTATPROPSETSTG_StubThunkTable[] = 
    {
    IEnumSTATPROPSETSTG_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumSTATPROPSETSTG_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPSETSTG_FormatStringOffsetTable[-3],
    &IEnumSTATPROPSETSTG_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATPROPSETSTGProxyVtbl = 
{
    &IEnumSTATPROPSETSTG_ProxyInfo,
    &IID_IEnumSTATPROPSETSTG,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumSTATPROPSETSTG_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSETSTG::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSETSTG::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSETSTG::Clone */
};

const CInterfaceStubVtbl _IEnumSTATPROPSETSTGStubVtbl =
{
    &IID_IEnumSTATPROPSETSTG,
    &IEnumSTATPROPSETSTG_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_propidl_0109, ver. 0.0,
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

const CInterfaceProxyVtbl * _propidl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IPropertyStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATPROPSTGProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPropertySetStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATPROPSETSTGProxyVtbl,
    0
};

const CInterfaceStubVtbl * _propidl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IPropertyStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATPROPSTGStubVtbl,
    ( CInterfaceStubVtbl *) &_IPropertySetStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATPROPSETSTGStubVtbl,
    0
};

PCInterfaceName const _propidl_InterfaceNamesList[] = 
{
    "IPropertyStorage",
    "IEnumSTATPROPSTG",
    "IPropertySetStorage",
    "IEnumSTATPROPSETSTG",
    0
};


#define _propidl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _propidl, pIID, n)

int __stdcall _propidl_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _propidl, 4, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _propidl, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _propidl, 4, *pIndex )
    
}

const ExtendedProxyFileInfo propidl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _propidl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _propidl_StubVtblList,
    (const PCInterfaceName * ) & _propidl_InterfaceNamesList,
    0, // no delegation
    & _propidl_IID_Lookup, 
    4,
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
/* at Sun May 05 19:07:28 2002
 */
/* Compiler settings for propidl.idl:
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


#include "propidl.h"

#define TYPE_FORMAT_STRING_SIZE   2641                              
#define PROC_FORMAT_STRING_SIZE   925                               
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


extern const MIDL_SERVER_INFO IPropertyStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPropertyStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPropertySetStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPropertySetStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATPROPSTG_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPSTG_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSTG_RemoteNext_Proxy( 
    IEnumSTATPROPSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSTG *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[716],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumSTATPROPSTG_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumSTATPROPSTG *This;
        ULONG celt;
        char Pad0[4];
        STATPROPSTG *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumSTATPROPSTG_Next_Stub(
                                     (IEnumSTATPROPSTG *) pParamStruct->This,
                                     pParamStruct->celt,
                                     pParamStruct->rgelt,
                                     pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATPROPSETSTG_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPSETSTG_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSETSTG_RemoteNext_Proxy( 
    IEnumSTATPROPSETSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSETSTG *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[874],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumSTATPROPSETSTG_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumSTATPROPSETSTG *This;
        ULONG celt;
        char Pad0[4];
        STATPROPSETSTG *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumSTATPROPSETSTG_Next_Stub(
                                        (IEnumSTATPROPSETSTG *) pParamStruct->This,
                                        pParamStruct->celt,
                                        pParamStruct->rgelt,
                                        pParamStruct->pceltFetched);
    
}


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
/*  8 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 18 */	NdrFcShort( 0x44 ),	/* 68 */
/* 20 */	NdrFcShort( 0x2 ),	/* 2 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpspec */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpspec */

/* 32 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter rgpropvar */

/* 38 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	NdrFcShort( 0x94a ),	/* Type Offset=2378 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteMultiple */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x4 ),	/* 4 */
/* 58 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 60 */	NdrFcShort( 0x10 ),	/* 16 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 66 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x46 ),	/* 70 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpspec */

/* 76 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 78 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 80 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpspec */

/* 82 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 84 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 86 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter rgpropvar */

/* 88 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 90 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 92 */	NdrFcShort( 0x94a ),	/* Type Offset=2378 */

	/* Parameter propidNameFirst */

/* 94 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 96 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 98 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 102 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteMultiple */

/* 106 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x5 ),	/* 5 */
/* 114 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 116 */	NdrFcShort( 0x8 ),	/* 8 */
/* 118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 120 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 122 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x2 ),	/* 2 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpspec */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpspec */

/* 138 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 140 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 142 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 144 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 146 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReadPropertyNames */

/* 150 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x6 ),	/* 6 */
/* 158 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 160 */	NdrFcShort( 0x8 ),	/* 8 */
/* 162 */	NdrFcShort( 0x8 ),	/* 8 */
/* 164 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 166 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 168 */	NdrFcShort( 0x1 ),	/* 1 */
/* 170 */	NdrFcShort( 0x1 ),	/* 1 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpropid */

/* 176 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 178 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 180 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpropid */

/* 182 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 184 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 186 */	NdrFcShort( 0x960 ),	/* Type Offset=2400 */

	/* Parameter rglpwstrName */

/* 188 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 190 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 192 */	NdrFcShort( 0x96c ),	/* Type Offset=2412 */

	/* Return value */

/* 194 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 196 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WritePropertyNames */

/* 200 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 202 */	NdrFcLong( 0x0 ),	/* 0 */
/* 206 */	NdrFcShort( 0x7 ),	/* 7 */
/* 208 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 212 */	NdrFcShort( 0x8 ),	/* 8 */
/* 214 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 216 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x2 ),	/* 2 */
/* 222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 224 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpropid */

/* 226 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 228 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpropid */

/* 232 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 234 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 236 */	NdrFcShort( 0x960 ),	/* Type Offset=2400 */

	/* Parameter rglpwstrName */

/* 238 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 240 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 242 */	NdrFcShort( 0x982 ),	/* Type Offset=2434 */

	/* Return value */

/* 244 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 246 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeletePropertyNames */

/* 250 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 252 */	NdrFcLong( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0x8 ),	/* 8 */
/* 258 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 260 */	NdrFcShort( 0x8 ),	/* 8 */
/* 262 */	NdrFcShort( 0x8 ),	/* 8 */
/* 264 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 266 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x1 ),	/* 1 */
/* 272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cpropid */

/* 276 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 278 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpropid */

/* 282 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 284 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 286 */	NdrFcShort( 0x960 ),	/* Type Offset=2400 */

	/* Return value */

/* 288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 290 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Commit */

/* 294 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0x9 ),	/* 9 */
/* 302 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 304 */	NdrFcShort( 0x8 ),	/* 8 */
/* 306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 308 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 310 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfCommitFlags */

/* 320 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 322 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 326 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 328 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Revert */

/* 332 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 334 */	NdrFcLong( 0x0 ),	/* 0 */
/* 338 */	NdrFcShort( 0xa ),	/* 10 */
/* 340 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x8 ),	/* 8 */
/* 346 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 348 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 358 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 360 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 362 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Enum */

/* 364 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 366 */	NdrFcLong( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0xb ),	/* 11 */
/* 372 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 376 */	NdrFcShort( 0x8 ),	/* 8 */
/* 378 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 380 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 388 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 390 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 392 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 394 */	NdrFcShort( 0x998 ),	/* Type Offset=2456 */

	/* Return value */

/* 396 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 398 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTimes */

/* 402 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 404 */	NdrFcLong( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0xc ),	/* 12 */
/* 410 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 412 */	NdrFcShort( 0x84 ),	/* 132 */
/* 414 */	NdrFcShort( 0x8 ),	/* 8 */
/* 416 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 418 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pctime */

/* 428 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 430 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 432 */	NdrFcShort( 0x2a4 ),	/* Type Offset=676 */

	/* Parameter patime */

/* 434 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 436 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 438 */	NdrFcShort( 0x2a4 ),	/* Type Offset=676 */

	/* Parameter pmtime */

/* 440 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 442 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 444 */	NdrFcShort( 0x2a4 ),	/* Type Offset=676 */

	/* Return value */

/* 446 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 448 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetClass */

/* 452 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 454 */	NdrFcLong( 0x0 ),	/* 0 */
/* 458 */	NdrFcShort( 0xd ),	/* 13 */
/* 460 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 462 */	NdrFcShort( 0x44 ),	/* 68 */
/* 464 */	NdrFcShort( 0x8 ),	/* 8 */
/* 466 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 468 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter clsid */

/* 478 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 480 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 482 */	NdrFcShort( 0x2b6 ),	/* Type Offset=694 */

	/* Return value */

/* 484 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 486 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 488 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stat */

/* 490 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 492 */	NdrFcLong( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0xe ),	/* 14 */
/* 498 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 502 */	NdrFcShort( 0xdc ),	/* 220 */
/* 504 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 506 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstatpsstg */

/* 516 */	NdrFcShort( 0x112 ),	/* Flags:  must free, out, simple ref, */
/* 518 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 520 */	NdrFcShort( 0x9ba ),	/* Type Offset=2490 */

	/* Return value */

/* 522 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 524 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Create */

/* 528 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 530 */	NdrFcLong( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x3 ),	/* 3 */
/* 536 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 538 */	NdrFcShort( 0x98 ),	/* 152 */
/* 540 */	NdrFcShort( 0x8 ),	/* 8 */
/* 542 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 544 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x0 ),	/* 0 */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rfmtid */

/* 554 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 556 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 558 */	NdrFcShort( 0x2b6 ),	/* Type Offset=694 */

	/* Parameter pclsid */

/* 560 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 562 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 564 */	NdrFcShort( 0x9d6 ),	/* Type Offset=2518 */

	/* Parameter grfFlags */

/* 566 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 568 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter grfMode */

/* 572 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 574 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 576 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppprstg */

/* 578 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 580 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 582 */	NdrFcShort( 0x9da ),	/* Type Offset=2522 */

	/* Return value */

/* 584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 586 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Open */

/* 590 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 596 */	NdrFcShort( 0x4 ),	/* 4 */
/* 598 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 600 */	NdrFcShort( 0x4c ),	/* 76 */
/* 602 */	NdrFcShort( 0x8 ),	/* 8 */
/* 604 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 606 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 614 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rfmtid */

/* 616 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 618 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 620 */	NdrFcShort( 0x2b6 ),	/* Type Offset=694 */

	/* Parameter grfMode */

/* 622 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 624 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 626 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppprstg */

/* 628 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 630 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 632 */	NdrFcShort( 0x9da ),	/* Type Offset=2522 */

	/* Return value */

/* 634 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 636 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 638 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Delete */

/* 640 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 642 */	NdrFcLong( 0x0 ),	/* 0 */
/* 646 */	NdrFcShort( 0x5 ),	/* 5 */
/* 648 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 650 */	NdrFcShort( 0x44 ),	/* 68 */
/* 652 */	NdrFcShort( 0x8 ),	/* 8 */
/* 654 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 656 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 664 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rfmtid */

/* 666 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 668 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 670 */	NdrFcShort( 0x2b6 ),	/* Type Offset=694 */

	/* Return value */

/* 672 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 674 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */


	/* Procedure Enum */

/* 678 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 680 */	NdrFcLong( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0x6 ),	/* 6 */
/* 686 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 692 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 694 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 702 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */


	/* Parameter ppenum */

/* 704 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 706 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 708 */	NdrFcShort( 0x9f0 ),	/* Type Offset=2544 */

	/* Return value */


	/* Return value */

/* 710 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 712 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 714 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 716 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 718 */	NdrFcLong( 0x0 ),	/* 0 */
/* 722 */	NdrFcShort( 0x3 ),	/* 3 */
/* 724 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 726 */	NdrFcShort( 0x8 ),	/* 8 */
/* 728 */	NdrFcShort( 0x24 ),	/* 36 */
/* 730 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 732 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 734 */	NdrFcShort( 0x1 ),	/* 1 */
/* 736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 742 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 744 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 748 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 750 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 752 */	NdrFcShort( 0xa1c ),	/* Type Offset=2588 */

	/* Parameter pceltFetched */

/* 754 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 756 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 760 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 762 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 764 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure Skip */

/* 766 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 768 */	NdrFcLong( 0x0 ),	/* 0 */
/* 772 */	NdrFcShort( 0x4 ),	/* 4 */
/* 774 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 776 */	NdrFcShort( 0x8 ),	/* 8 */
/* 778 */	NdrFcShort( 0x8 ),	/* 8 */
/* 780 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 782 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */


	/* Parameter celt */

/* 792 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 794 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 798 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 800 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure Reset */

/* 804 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 806 */	NdrFcLong( 0x0 ),	/* 0 */
/* 810 */	NdrFcShort( 0x5 ),	/* 5 */
/* 812 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 816 */	NdrFcShort( 0x8 ),	/* 8 */
/* 818 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 820 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 830 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 832 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 834 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 836 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 838 */	NdrFcLong( 0x0 ),	/* 0 */
/* 842 */	NdrFcShort( 0x6 ),	/* 6 */
/* 844 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 848 */	NdrFcShort( 0x8 ),	/* 8 */
/* 850 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 852 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 854 */	NdrFcShort( 0x0 ),	/* 0 */
/* 856 */	NdrFcShort( 0x0 ),	/* 0 */
/* 858 */	NdrFcShort( 0x0 ),	/* 0 */
/* 860 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 862 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 864 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 866 */	NdrFcShort( 0x998 ),	/* Type Offset=2456 */

	/* Return value */

/* 868 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 870 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 872 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 874 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 876 */	NdrFcLong( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x3 ),	/* 3 */
/* 882 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 884 */	NdrFcShort( 0x8 ),	/* 8 */
/* 886 */	NdrFcShort( 0x24 ),	/* 36 */
/* 888 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 890 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 892 */	NdrFcShort( 0x1 ),	/* 1 */
/* 894 */	NdrFcShort( 0x0 ),	/* 0 */
/* 896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 898 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 900 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 902 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 904 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 906 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 908 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 910 */	NdrFcShort( 0xa3a ),	/* Type Offset=2618 */

	/* Parameter pceltFetched */

/* 912 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 914 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 916 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 918 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 920 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 922 */	0x8,		/* FC_LONG */
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
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/*  4 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/*  6 */	NdrFcShort( 0xfff8 ),	/* -8 */
/*  8 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 10 */	NdrFcShort( 0x2 ),	/* Offset= 2 (12) */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	NdrFcShort( 0x2 ),	/* 2 */
/* 16 */	NdrFcLong( 0x1 ),	/* 1 */
/* 20 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 22 */	NdrFcLong( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x4 ),	/* Offset= 4 (30) */
/* 28 */	NdrFcShort( 0x0 ),	/* Offset= 0 (28) */
/* 30 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 32 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 34 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 36 */	NdrFcShort( 0x10 ),	/* 16 */
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x0 ),	/* Offset= 0 (40) */
/* 42 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 44 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 46 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (2) */
/* 48 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 50 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 52 */	NdrFcShort( 0x0 ),	/* 0 */
/* 54 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 56 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 58 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 60 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 64 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 66 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 68 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (34) */
/* 70 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 72 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x7,		/* FC_USHORT */
/* 74 */	0x0,		/* Corr desc:  */
			0x59,		/* FC_CALLBACK */
/* 76 */	NdrFcShort( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 80 */	NdrFcShort( 0x2 ),	/* Offset= 2 (82) */
/* 82 */	NdrFcShort( 0x10 ),	/* 16 */
/* 84 */	NdrFcShort( 0x61 ),	/* 97 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x0 ),	/* Offset= 0 (90) */
/* 92 */	NdrFcLong( 0x1 ),	/* 1 */
/* 96 */	NdrFcShort( 0x0 ),	/* Offset= 0 (96) */
/* 98 */	NdrFcLong( 0x10 ),	/* 16 */
/* 102 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 104 */	NdrFcLong( 0x11 ),	/* 17 */
/* 108 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 110 */	NdrFcLong( 0x2 ),	/* 2 */
/* 114 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 116 */	NdrFcLong( 0x12 ),	/* 18 */
/* 120 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 122 */	NdrFcLong( 0x3 ),	/* 3 */
/* 126 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 128 */	NdrFcLong( 0x13 ),	/* 19 */
/* 132 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 134 */	NdrFcLong( 0x16 ),	/* 22 */
/* 138 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 140 */	NdrFcLong( 0x17 ),	/* 23 */
/* 144 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 146 */	NdrFcLong( 0xe ),	/* 14 */
/* 150 */	NdrFcShort( 0x208 ),	/* Offset= 520 (670) */
/* 152 */	NdrFcLong( 0x14 ),	/* 20 */
/* 156 */	NdrFcShort( 0x202 ),	/* Offset= 514 (670) */
/* 158 */	NdrFcLong( 0x15 ),	/* 21 */
/* 162 */	NdrFcShort( 0x1fc ),	/* Offset= 508 (670) */
/* 164 */	NdrFcLong( 0x4 ),	/* 4 */
/* 168 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 170 */	NdrFcLong( 0x5 ),	/* 5 */
/* 174 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 176 */	NdrFcLong( 0xb ),	/* 11 */
/* 180 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 182 */	NdrFcLong( 0xffff ),	/* 65535 */
/* 186 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 188 */	NdrFcLong( 0xa ),	/* 10 */
/* 192 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 194 */	NdrFcLong( 0x6 ),	/* 6 */
/* 198 */	NdrFcShort( 0x1d8 ),	/* Offset= 472 (670) */
/* 200 */	NdrFcLong( 0x7 ),	/* 7 */
/* 204 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 206 */	NdrFcLong( 0x40 ),	/* 64 */
/* 210 */	NdrFcShort( 0x1d2 ),	/* Offset= 466 (676) */
/* 212 */	NdrFcLong( 0x48 ),	/* 72 */
/* 216 */	NdrFcShort( 0x1d4 ),	/* Offset= 468 (684) */
/* 218 */	NdrFcLong( 0x47 ),	/* 71 */
/* 222 */	NdrFcShort( 0x1e4 ),	/* Offset= 484 (706) */
/* 224 */	NdrFcLong( 0x8 ),	/* 8 */
/* 228 */	NdrFcShort( 0x218 ),	/* Offset= 536 (764) */
/* 230 */	NdrFcLong( 0xfff ),	/* 4095 */
/* 234 */	NdrFcShort( 0x228 ),	/* Offset= 552 (786) */
/* 236 */	NdrFcLong( 0x41 ),	/* 65 */
/* 240 */	NdrFcShort( 0x232 ),	/* Offset= 562 (802) */
/* 242 */	NdrFcLong( 0x46 ),	/* 70 */
/* 246 */	NdrFcShort( 0x22c ),	/* Offset= 556 (802) */
/* 248 */	NdrFcLong( 0x1e ),	/* 30 */
/* 252 */	NdrFcShort( 0x236 ),	/* Offset= 566 (818) */
/* 254 */	NdrFcLong( 0x1f ),	/* 31 */
/* 258 */	NdrFcShort( 0x234 ),	/* Offset= 564 (822) */
/* 260 */	NdrFcLong( 0xd ),	/* 13 */
/* 264 */	NdrFcShort( 0x232 ),	/* Offset= 562 (826) */
/* 266 */	NdrFcLong( 0x9 ),	/* 9 */
/* 270 */	NdrFcShort( 0x23e ),	/* Offset= 574 (844) */
/* 272 */	NdrFcLong( 0x42 ),	/* 66 */
/* 276 */	NdrFcShort( 0x24a ),	/* Offset= 586 (862) */
/* 278 */	NdrFcLong( 0x44 ),	/* 68 */
/* 282 */	NdrFcShort( 0x244 ),	/* Offset= 580 (862) */
/* 284 */	NdrFcLong( 0x43 ),	/* 67 */
/* 288 */	NdrFcShort( 0x250 ),	/* Offset= 592 (880) */
/* 290 */	NdrFcLong( 0x45 ),	/* 69 */
/* 294 */	NdrFcShort( 0x24a ),	/* Offset= 586 (880) */
/* 296 */	NdrFcLong( 0x49 ),	/* 73 */
/* 300 */	NdrFcShort( 0x256 ),	/* Offset= 598 (898) */
/* 302 */	NdrFcLong( 0x2010 ),	/* 8208 */
/* 306 */	NdrFcShort( 0x5b8 ),	/* Offset= 1464 (1770) */
/* 308 */	NdrFcLong( 0x2011 ),	/* 8209 */
/* 312 */	NdrFcShort( 0x5b2 ),	/* Offset= 1458 (1770) */
/* 314 */	NdrFcLong( 0x2002 ),	/* 8194 */
/* 318 */	NdrFcShort( 0x5ac ),	/* Offset= 1452 (1770) */
/* 320 */	NdrFcLong( 0x2012 ),	/* 8210 */
/* 324 */	NdrFcShort( 0x5a6 ),	/* Offset= 1446 (1770) */
/* 326 */	NdrFcLong( 0x2003 ),	/* 8195 */
/* 330 */	NdrFcShort( 0x5a0 ),	/* Offset= 1440 (1770) */
/* 332 */	NdrFcLong( 0x2013 ),	/* 8211 */
/* 336 */	NdrFcShort( 0x59a ),	/* Offset= 1434 (1770) */
/* 338 */	NdrFcLong( 0x2016 ),	/* 8214 */
/* 342 */	NdrFcShort( 0x594 ),	/* Offset= 1428 (1770) */
/* 344 */	NdrFcLong( 0x2017 ),	/* 8215 */
/* 348 */	NdrFcShort( 0x58e ),	/* Offset= 1422 (1770) */
/* 350 */	NdrFcLong( 0x2004 ),	/* 8196 */
/* 354 */	NdrFcShort( 0x588 ),	/* Offset= 1416 (1770) */
/* 356 */	NdrFcLong( 0x2005 ),	/* 8197 */
/* 360 */	NdrFcShort( 0x582 ),	/* Offset= 1410 (1770) */
/* 362 */	NdrFcLong( 0x2006 ),	/* 8198 */
/* 366 */	NdrFcShort( 0x57c ),	/* Offset= 1404 (1770) */
/* 368 */	NdrFcLong( 0x2007 ),	/* 8199 */
/* 372 */	NdrFcShort( 0x576 ),	/* Offset= 1398 (1770) */
/* 374 */	NdrFcLong( 0x2008 ),	/* 8200 */
/* 378 */	NdrFcShort( 0x570 ),	/* Offset= 1392 (1770) */
/* 380 */	NdrFcLong( 0x200b ),	/* 8203 */
/* 384 */	NdrFcShort( 0x56a ),	/* Offset= 1386 (1770) */
/* 386 */	NdrFcLong( 0x200e ),	/* 8206 */
/* 390 */	NdrFcShort( 0x564 ),	/* Offset= 1380 (1770) */
/* 392 */	NdrFcLong( 0x2009 ),	/* 8201 */
/* 396 */	NdrFcShort( 0x55e ),	/* Offset= 1374 (1770) */
/* 398 */	NdrFcLong( 0x200d ),	/* 8205 */
/* 402 */	NdrFcShort( 0x558 ),	/* Offset= 1368 (1770) */
/* 404 */	NdrFcLong( 0x200a ),	/* 8202 */
/* 408 */	NdrFcShort( 0x552 ),	/* Offset= 1362 (1770) */
/* 410 */	NdrFcLong( 0x200c ),	/* 8204 */
/* 414 */	NdrFcShort( 0x54c ),	/* Offset= 1356 (1770) */
/* 416 */	NdrFcLong( 0x1010 ),	/* 4112 */
/* 420 */	NdrFcShort( 0x55c ),	/* Offset= 1372 (1792) */
/* 422 */	NdrFcLong( 0x1011 ),	/* 4113 */
/* 426 */	NdrFcShort( 0x566 ),	/* Offset= 1382 (1808) */
/* 428 */	NdrFcLong( 0x1002 ),	/* 4098 */
/* 432 */	NdrFcShort( 0x570 ),	/* Offset= 1392 (1824) */
/* 434 */	NdrFcLong( 0x1012 ),	/* 4114 */
/* 438 */	NdrFcShort( 0x57a ),	/* Offset= 1402 (1840) */
/* 440 */	NdrFcLong( 0x1003 ),	/* 4099 */
/* 444 */	NdrFcShort( 0x584 ),	/* Offset= 1412 (1856) */
/* 446 */	NdrFcLong( 0x1013 ),	/* 4115 */
/* 450 */	NdrFcShort( 0x58e ),	/* Offset= 1422 (1872) */
/* 452 */	NdrFcLong( 0x1014 ),	/* 4116 */
/* 456 */	NdrFcShort( 0x5a8 ),	/* Offset= 1448 (1904) */
/* 458 */	NdrFcLong( 0x1015 ),	/* 4117 */
/* 462 */	NdrFcShort( 0x5b2 ),	/* Offset= 1458 (1920) */
/* 464 */	NdrFcLong( 0x1004 ),	/* 4100 */
/* 468 */	NdrFcShort( 0x5c8 ),	/* Offset= 1480 (1948) */
/* 470 */	NdrFcLong( 0x1005 ),	/* 4101 */
/* 474 */	NdrFcShort( 0x5de ),	/* Offset= 1502 (1976) */
/* 476 */	NdrFcLong( 0x100b ),	/* 4107 */
/* 480 */	NdrFcShort( 0x5e8 ),	/* Offset= 1512 (1992) */
/* 482 */	NdrFcLong( 0x100a ),	/* 4106 */
/* 486 */	NdrFcShort( 0x5f2 ),	/* Offset= 1522 (2008) */
/* 488 */	NdrFcLong( 0x1006 ),	/* 4102 */
/* 492 */	NdrFcShort( 0x5fc ),	/* Offset= 1532 (2024) */
/* 494 */	NdrFcLong( 0x1007 ),	/* 4103 */
/* 498 */	NdrFcShort( 0x606 ),	/* Offset= 1542 (2040) */
/* 500 */	NdrFcLong( 0x1040 ),	/* 4160 */
/* 504 */	NdrFcShort( 0x620 ),	/* Offset= 1568 (2072) */
/* 506 */	NdrFcLong( 0x1048 ),	/* 4168 */
/* 510 */	NdrFcShort( 0x63a ),	/* Offset= 1594 (2104) */
/* 512 */	NdrFcLong( 0x1047 ),	/* 4167 */
/* 516 */	NdrFcShort( 0x65a ),	/* Offset= 1626 (2142) */
/* 518 */	NdrFcLong( 0x1008 ),	/* 4104 */
/* 522 */	NdrFcShort( 0x67a ),	/* Offset= 1658 (2180) */
/* 524 */	NdrFcLong( 0x1fff ),	/* 8191 */
/* 528 */	NdrFcShort( 0x69a ),	/* Offset= 1690 (2218) */
/* 530 */	NdrFcLong( 0x101e ),	/* 4126 */
/* 534 */	NdrFcShort( 0x6ba ),	/* Offset= 1722 (2256) */
/* 536 */	NdrFcLong( 0x101f ),	/* 4127 */
/* 540 */	NdrFcShort( 0x6da ),	/* Offset= 1754 (2294) */
/* 542 */	NdrFcLong( 0x100c ),	/* 4108 */
/* 546 */	NdrFcShort( 0x6fa ),	/* Offset= 1786 (2332) */
/* 548 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 552 */	NdrFcShort( 0x3c4 ),	/* Offset= 964 (1516) */
/* 554 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 558 */	NdrFcShort( 0x3be ),	/* Offset= 958 (1516) */
/* 560 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 564 */	NdrFcShort( 0x376 ),	/* Offset= 886 (1450) */
/* 566 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 570 */	NdrFcShort( 0x370 ),	/* Offset= 880 (1450) */
/* 572 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 576 */	NdrFcShort( 0x36e ),	/* Offset= 878 (1454) */
/* 578 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 582 */	NdrFcShort( 0x368 ),	/* Offset= 872 (1454) */
/* 584 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 588 */	NdrFcShort( 0x362 ),	/* Offset= 866 (1454) */
/* 590 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 594 */	NdrFcShort( 0x35c ),	/* Offset= 860 (1454) */
/* 596 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 600 */	NdrFcShort( 0x35a ),	/* Offset= 858 (1458) */
/* 602 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 606 */	NdrFcShort( 0x358 ),	/* Offset= 856 (1462) */
/* 608 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 612 */	NdrFcShort( 0x346 ),	/* Offset= 838 (1450) */
/* 614 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 618 */	NdrFcShort( 0x37e ),	/* Offset= 894 (1512) */
/* 620 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 624 */	NdrFcShort( 0x33e ),	/* Offset= 830 (1454) */
/* 626 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 630 */	NdrFcShort( 0x344 ),	/* Offset= 836 (1466) */
/* 632 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 636 */	NdrFcShort( 0x33a ),	/* Offset= 826 (1462) */
/* 638 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 642 */	NdrFcShort( 0x6aa ),	/* Offset= 1706 (2348) */
/* 644 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 648 */	NdrFcShort( 0x33a ),	/* Offset= 826 (1474) */
/* 650 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 654 */	NdrFcShort( 0x338 ),	/* Offset= 824 (1478) */
/* 656 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 660 */	NdrFcShort( 0x69c ),	/* Offset= 1692 (2352) */
/* 662 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 666 */	NdrFcShort( 0x69a ),	/* Offset= 1690 (2356) */
/* 668 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (667) */
/* 670 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 672 */	NdrFcShort( 0x8 ),	/* 8 */
/* 674 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 676 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 678 */	NdrFcShort( 0x8 ),	/* 8 */
/* 680 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 682 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 684 */	
			0x13, 0x0,	/* FC_OP */
/* 686 */	NdrFcShort( 0x8 ),	/* Offset= 8 (694) */
/* 688 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 692 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 694 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 696 */	NdrFcShort( 0x10 ),	/* 16 */
/* 698 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 700 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 702 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (688) */
			0x5b,		/* FC_END */
/* 706 */	
			0x13, 0x0,	/* FC_OP */
/* 708 */	NdrFcShort( 0xe ),	/* Offset= 14 (722) */
/* 710 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 712 */	NdrFcShort( 0x1 ),	/* 1 */
/* 714 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 716 */	NdrFcShort( 0x1 ),	/* 1 */
/* 718 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 720 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 722 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 724 */	NdrFcShort( 0x10 ),	/* 16 */
/* 726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 728 */	NdrFcShort( 0x6 ),	/* Offset= 6 (734) */
/* 730 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 732 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 734 */	
			0x13, 0x0,	/* FC_OP */
/* 736 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (710) */
/* 738 */	
			0x13, 0x0,	/* FC_OP */
/* 740 */	NdrFcShort( 0xe ),	/* Offset= 14 (754) */
/* 742 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 744 */	NdrFcShort( 0x2 ),	/* 2 */
/* 746 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 748 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 750 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 752 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 754 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 756 */	NdrFcShort( 0x8 ),	/* 8 */
/* 758 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (742) */
/* 760 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 762 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 764 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	NdrFcShort( 0x8 ),	/* 8 */
/* 770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 772 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (738) */
/* 774 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 776 */	NdrFcShort( 0x1 ),	/* 1 */
/* 778 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 784 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 786 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 788 */	NdrFcShort( 0x10 ),	/* 16 */
/* 790 */	NdrFcShort( 0x0 ),	/* 0 */
/* 792 */	NdrFcShort( 0x6 ),	/* Offset= 6 (798) */
/* 794 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 796 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 798 */	
			0x13, 0x0,	/* FC_OP */
/* 800 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (774) */
/* 802 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 804 */	NdrFcShort( 0x10 ),	/* 16 */
/* 806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 808 */	NdrFcShort( 0x6 ),	/* Offset= 6 (814) */
/* 810 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 812 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 814 */	
			0x13, 0x0,	/* FC_OP */
/* 816 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (774) */
/* 818 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 820 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 822 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 824 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 826 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 828 */	NdrFcLong( 0x0 ),	/* 0 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 836 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 838 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 840 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 842 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 844 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 846 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 854 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 856 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 858 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 860 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 862 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 864 */	NdrFcLong( 0xc ),	/* 12 */
/* 868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 874 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 876 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 878 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 880 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 882 */	NdrFcLong( 0xb ),	/* 11 */
/* 886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 890 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 892 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 894 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 896 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 898 */	
			0x13, 0x0,	/* FC_OP */
/* 900 */	NdrFcShort( 0x2 ),	/* Offset= 2 (902) */
/* 902 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 904 */	NdrFcShort( 0x18 ),	/* 24 */
/* 906 */	NdrFcShort( 0x0 ),	/* 0 */
/* 908 */	NdrFcShort( 0xc ),	/* Offset= 12 (920) */
/* 910 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 912 */	NdrFcShort( 0xffffff26 ),	/* Offset= -218 (694) */
/* 914 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 916 */	NdrFcShort( 0xffffffca ),	/* Offset= -54 (862) */
/* 918 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 920 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 922 */	NdrFcShort( 0x2 ),	/* Offset= 2 (924) */
/* 924 */	
			0x13, 0x0,	/* FC_OP */
/* 926 */	NdrFcShort( 0x33a ),	/* Offset= 826 (1752) */
/* 928 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 930 */	NdrFcShort( 0x20 ),	/* 32 */
/* 932 */	NdrFcShort( 0xa ),	/* 10 */
/* 934 */	NdrFcLong( 0x8 ),	/* 8 */
/* 938 */	NdrFcShort( 0x50 ),	/* Offset= 80 (1018) */
/* 940 */	NdrFcLong( 0xd ),	/* 13 */
/* 944 */	NdrFcShort( 0x70 ),	/* Offset= 112 (1056) */
/* 946 */	NdrFcLong( 0x9 ),	/* 9 */
/* 950 */	NdrFcShort( 0x90 ),	/* Offset= 144 (1094) */
/* 952 */	NdrFcLong( 0xc ),	/* 12 */
/* 956 */	NdrFcShort( 0x25e ),	/* Offset= 606 (1562) */
/* 958 */	NdrFcLong( 0x24 ),	/* 36 */
/* 962 */	NdrFcShort( 0x27e ),	/* Offset= 638 (1600) */
/* 964 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 968 */	NdrFcShort( 0x288 ),	/* Offset= 648 (1616) */
/* 970 */	NdrFcLong( 0x10 ),	/* 16 */
/* 974 */	NdrFcShort( 0x296 ),	/* Offset= 662 (1636) */
/* 976 */	NdrFcLong( 0x2 ),	/* 2 */
/* 980 */	NdrFcShort( 0x2ac ),	/* Offset= 684 (1664) */
/* 982 */	NdrFcLong( 0x3 ),	/* 3 */
/* 986 */	NdrFcShort( 0x2c2 ),	/* Offset= 706 (1692) */
/* 988 */	NdrFcLong( 0x14 ),	/* 20 */
/* 992 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (1720) */
/* 994 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (993) */
/* 996 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 998 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1000 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1006 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1010 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1012 */	
			0x13, 0x0,	/* FC_OP */
/* 1014 */	NdrFcShort( 0xfffffefc ),	/* Offset= -260 (754) */
/* 1016 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1018 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1020 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1024 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1030) */
/* 1026 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1028 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1030 */	
			0x11, 0x0,	/* FC_RP */
/* 1032 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (996) */
/* 1034 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1036 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1038 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1042 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1044 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1048 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1050 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1052 */	NdrFcShort( 0xffffff1e ),	/* Offset= -226 (826) */
/* 1054 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1056 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1058 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1062 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1068) */
/* 1064 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1066 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1068 */	
			0x11, 0x0,	/* FC_RP */
/* 1070 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1034) */
/* 1072 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1076 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1080 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1082 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1086 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1088 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1090 */	NdrFcShort( 0xffffff0a ),	/* Offset= -246 (844) */
/* 1092 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1094 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1096 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1100 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1106) */
/* 1102 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1104 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1106 */	
			0x11, 0x0,	/* FC_RP */
/* 1108 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1072) */
/* 1110 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1112 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1114 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 1116 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1118 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1120) */
/* 1120 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1122 */	NdrFcShort( 0x2b ),	/* 43 */
/* 1124 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1128 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1130 */	NdrFcLong( 0x11 ),	/* 17 */
/* 1134 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 1136 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1140 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1142 */	NdrFcLong( 0x4 ),	/* 4 */
/* 1146 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 1148 */	NdrFcLong( 0x5 ),	/* 5 */
/* 1152 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 1154 */	NdrFcLong( 0xb ),	/* 11 */
/* 1158 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1160 */	NdrFcLong( 0xa ),	/* 10 */
/* 1164 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1166 */	NdrFcLong( 0x6 ),	/* 6 */
/* 1170 */	NdrFcShort( 0xfffffe0c ),	/* Offset= -500 (670) */
/* 1172 */	NdrFcLong( 0x7 ),	/* 7 */
/* 1176 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 1178 */	NdrFcLong( 0x8 ),	/* 8 */
/* 1182 */	NdrFcShort( 0xfffffe44 ),	/* Offset= -444 (738) */
/* 1184 */	NdrFcLong( 0xd ),	/* 13 */
/* 1188 */	NdrFcShort( 0xfffffe96 ),	/* Offset= -362 (826) */
/* 1190 */	NdrFcLong( 0x9 ),	/* 9 */
/* 1194 */	NdrFcShort( 0xfffffea2 ),	/* Offset= -350 (844) */
/* 1196 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 1200 */	NdrFcShort( 0xb8 ),	/* Offset= 184 (1384) */
/* 1202 */	NdrFcLong( 0x24 ),	/* 36 */
/* 1206 */	NdrFcShort( 0xba ),	/* Offset= 186 (1392) */
/* 1208 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 1212 */	NdrFcShort( 0xb4 ),	/* Offset= 180 (1392) */
/* 1214 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 1218 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (1446) */
/* 1220 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 1224 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (1450) */
/* 1226 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 1230 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (1454) */
/* 1232 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 1236 */	NdrFcShort( 0xde ),	/* Offset= 222 (1458) */
/* 1238 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 1242 */	NdrFcShort( 0xdc ),	/* Offset= 220 (1462) */
/* 1244 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 1248 */	NdrFcShort( 0xca ),	/* Offset= 202 (1450) */
/* 1250 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 1254 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (1454) */
/* 1256 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 1260 */	NdrFcShort( 0xce ),	/* Offset= 206 (1466) */
/* 1262 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 1266 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1462) */
/* 1268 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 1272 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (1470) */
/* 1274 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 1278 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1474) */
/* 1280 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 1284 */	NdrFcShort( 0xc2 ),	/* Offset= 194 (1478) */
/* 1286 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 1290 */	NdrFcShort( 0xc0 ),	/* Offset= 192 (1482) */
/* 1292 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 1296 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (1494) */
/* 1298 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1302 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 1304 */	NdrFcLong( 0x12 ),	/* 18 */
/* 1308 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 1310 */	NdrFcLong( 0x13 ),	/* 19 */
/* 1314 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1316 */	NdrFcLong( 0x16 ),	/* 22 */
/* 1320 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1322 */	NdrFcLong( 0x17 ),	/* 23 */
/* 1326 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1328 */	NdrFcLong( 0xe ),	/* 14 */
/* 1332 */	NdrFcShort( 0xaa ),	/* Offset= 170 (1502) */
/* 1334 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 1338 */	NdrFcShort( 0xae ),	/* Offset= 174 (1512) */
/* 1340 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 1344 */	NdrFcShort( 0xac ),	/* Offset= 172 (1516) */
/* 1346 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 1350 */	NdrFcShort( 0x64 ),	/* Offset= 100 (1450) */
/* 1352 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 1356 */	NdrFcShort( 0x62 ),	/* Offset= 98 (1454) */
/* 1358 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 1362 */	NdrFcShort( 0x5c ),	/* Offset= 92 (1454) */
/* 1364 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 1368 */	NdrFcShort( 0x56 ),	/* Offset= 86 (1454) */
/* 1370 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1374 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1374) */
/* 1376 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1380 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1380) */
/* 1382 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1381) */
/* 1384 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1386 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1388) */
/* 1388 */	
			0x13, 0x0,	/* FC_OP */
/* 1390 */	NdrFcShort( 0x16a ),	/* Offset= 362 (1752) */
/* 1392 */	
			0x13, 0x0,	/* FC_OP */
/* 1394 */	NdrFcShort( 0x20 ),	/* Offset= 32 (1426) */
/* 1396 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1398 */	NdrFcLong( 0x2f ),	/* 47 */
/* 1402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1406 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1408 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1410 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1412 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1414 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1416 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1418 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1420 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1422 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1424 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1426 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1428 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1432 */	NdrFcShort( 0xa ),	/* Offset= 10 (1442) */
/* 1434 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1436 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1438 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (1396) */
/* 1440 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1442 */	
			0x13, 0x0,	/* FC_OP */
/* 1444 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1414) */
/* 1446 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1448 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1450 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1452 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1454 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1456 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1458 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1460 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1462 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1464 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1466 */	
			0x13, 0x0,	/* FC_OP */
/* 1468 */	NdrFcShort( 0xfffffce2 ),	/* Offset= -798 (670) */
/* 1470 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1472 */	NdrFcShort( 0xfffffd22 ),	/* Offset= -734 (738) */
/* 1474 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1476 */	NdrFcShort( 0xfffffd76 ),	/* Offset= -650 (826) */
/* 1478 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1480 */	NdrFcShort( 0xfffffd84 ),	/* Offset= -636 (844) */
/* 1482 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1484 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1486) */
/* 1486 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1488 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1490) */
/* 1490 */	
			0x13, 0x0,	/* FC_OP */
/* 1492 */	NdrFcShort( 0x104 ),	/* Offset= 260 (1752) */
/* 1494 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1496 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1498) */
/* 1498 */	
			0x13, 0x0,	/* FC_OP */
/* 1500 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1520) */
/* 1502 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1504 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1506 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1508 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1510 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1512 */	
			0x13, 0x0,	/* FC_OP */
/* 1514 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1502) */
/* 1516 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1518 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1520 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1522 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1526 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1526) */
/* 1528 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1530 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1532 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1534 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1536 */	NdrFcShort( 0xfffffe56 ),	/* Offset= -426 (1110) */
/* 1538 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1540 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1544 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1548 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1550 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1554 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1556 */	
			0x13, 0x0,	/* FC_OP */
/* 1558 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (1520) */
/* 1560 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1562 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1564 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1568 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1574) */
/* 1570 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1572 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1574 */	
			0x11, 0x0,	/* FC_RP */
/* 1576 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1540) */
/* 1578 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1582 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1586 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1588 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1592 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1594 */	
			0x13, 0x0,	/* FC_OP */
/* 1596 */	NdrFcShort( 0xffffff56 ),	/* Offset= -170 (1426) */
/* 1598 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1600 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1602 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1606 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1612) */
/* 1608 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1610 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1612 */	
			0x11, 0x0,	/* FC_RP */
/* 1614 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1578) */
/* 1616 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1618 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1622 */	NdrFcShort( 0xa ),	/* Offset= 10 (1632) */
/* 1624 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1626 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1628 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffc59 ),	/* Offset= -935 (694) */
			0x5b,		/* FC_END */
/* 1632 */	
			0x11, 0x0,	/* FC_RP */
/* 1634 */	NdrFcShort( 0xfffffda8 ),	/* Offset= -600 (1034) */
/* 1636 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1638 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1642 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1648) */
/* 1644 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1646 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1648 */	
			0x13, 0x0,	/* FC_OP */
/* 1650 */	NdrFcShort( 0xfffffc94 ),	/* Offset= -876 (774) */
/* 1652 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1654 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1656 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1660 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1662 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1664 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1666 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1670 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1676) */
/* 1672 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1674 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1676 */	
			0x13, 0x0,	/* FC_OP */
/* 1678 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1652) */
/* 1680 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1682 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1684 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1688 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1690 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1692 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1694 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1698 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1704) */
/* 1700 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1702 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1704 */	
			0x13, 0x0,	/* FC_OP */
/* 1706 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1680) */
/* 1708 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1710 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1712 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1716 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1718 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1720 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1722 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1726 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1732) */
/* 1728 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1730 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1732 */	
			0x13, 0x0,	/* FC_OP */
/* 1734 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1708) */
/* 1736 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1738 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1740 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1742 */	NdrFcShort( 0xffc8 ),	/* -56 */
/* 1744 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1746 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1748 */	NdrFcShort( 0xfffffbd0 ),	/* Offset= -1072 (676) */
/* 1750 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1752 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1754 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1756 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (1736) */
/* 1758 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1758) */
/* 1760 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1762 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1764 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1766 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffcb9 ),	/* Offset= -839 (928) */
			0x5b,		/* FC_END */
/* 1770 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1772 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1774 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1778 */	NdrFcShort( 0xfffffca6 ),	/* Offset= -858 (920) */
/* 1780 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1782 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1784 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1788 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1790 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
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
/* 1806 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1780) */
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
/* 1822 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (1780) */
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
/* 1838 */	NdrFcShort( 0xffffff46 ),	/* Offset= -186 (1652) */
/* 1840 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1842 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1846 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1852) */
/* 1848 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1850 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1852 */	
			0x13, 0x0,	/* FC_OP */
/* 1854 */	NdrFcShort( 0xffffff36 ),	/* Offset= -202 (1652) */
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
/* 1870 */	NdrFcShort( 0xffffff42 ),	/* Offset= -190 (1680) */
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
/* 1886 */	NdrFcShort( 0xffffff32 ),	/* Offset= -206 (1680) */
/* 1888 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1890 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1892 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1894 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1896 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1898 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1900 */	NdrFcShort( 0xfffffb32 ),	/* Offset= -1230 (670) */
/* 1902 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1904 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1906 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1910 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1916) */
/* 1912 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1914 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1916 */	
			0x13, 0x0,	/* FC_OP */
/* 1918 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1888) */
/* 1920 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1922 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1926 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1932) */
/* 1928 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1930 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1932 */	
			0x13, 0x0,	/* FC_OP */
/* 1934 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (1888) */
/* 1936 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1938 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1940 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1942 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1944 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1946 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 1948 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1950 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1954 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1960) */
/* 1956 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1958 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1960 */	
			0x13, 0x0,	/* FC_OP */
/* 1962 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1936) */
/* 1964 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1966 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1968 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1972 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1974 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
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
/* 1990 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1964) */
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
/* 2006 */	NdrFcShort( 0xfffffe9e ),	/* Offset= -354 (1652) */
/* 2008 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2010 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2014 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2020) */
/* 2016 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2018 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2020 */	
			0x13, 0x0,	/* FC_OP */
/* 2022 */	NdrFcShort( 0xfffffeaa ),	/* Offset= -342 (1680) */
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
/* 2038 */	NdrFcShort( 0xffffff6a ),	/* Offset= -150 (1888) */
/* 2040 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2042 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2046 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2052) */
/* 2048 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2050 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2052 */	
			0x13, 0x0,	/* FC_OP */
/* 2054 */	NdrFcShort( 0xffffffa6 ),	/* Offset= -90 (1964) */
/* 2056 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2058 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2060 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2064 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2066 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2068 */	NdrFcShort( 0xfffffa90 ),	/* Offset= -1392 (676) */
/* 2070 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2072 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2074 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2078 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2084) */
/* 2080 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2082 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2084 */	
			0x13, 0x0,	/* FC_OP */
/* 2086 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (2056) */
/* 2088 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2090 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2092 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2096 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2098 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2100 */	NdrFcShort( 0xfffffa82 ),	/* Offset= -1406 (694) */
/* 2102 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2104 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2106 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2110 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2116) */
/* 2112 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2114 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2116 */	
			0x13, 0x0,	/* FC_OP */
/* 2118 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (2088) */
/* 2120 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2124 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2128 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2130 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2134 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2136 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2138 */	NdrFcShort( 0xfffffa78 ),	/* Offset= -1416 (722) */
/* 2140 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2142 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2144 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2148 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2154) */
/* 2150 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2152 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2154 */	
			0x13, 0x0,	/* FC_OP */
/* 2156 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2120) */
/* 2158 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2162 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2166 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2168 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2172 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2174 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2176 */	NdrFcShort( 0xfffffa7c ),	/* Offset= -1412 (764) */
/* 2178 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2180 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2182 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2186 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2192) */
/* 2188 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2190 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2192 */	
			0x13, 0x0,	/* FC_OP */
/* 2194 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2158) */
/* 2196 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2200 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2204 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2206 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2210 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2212 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2214 */	NdrFcShort( 0xfffffa6c ),	/* Offset= -1428 (786) */
/* 2216 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2218 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2220 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2224 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2230) */
/* 2226 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2228 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2230 */	
			0x13, 0x0,	/* FC_OP */
/* 2232 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2196) */
/* 2234 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2238 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2242 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2244 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2248 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2250 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2252 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 2254 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2256 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2258 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2262 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2268) */
/* 2264 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2266 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2268 */	
			0x13, 0x0,	/* FC_OP */
/* 2270 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2234) */
/* 2272 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2276 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2280 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2282 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2286 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2288 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2290 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2292 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2294 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2296 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2300 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2306) */
/* 2302 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2304 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2306 */	
			0x13, 0x0,	/* FC_OP */
/* 2308 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2272) */
/* 2310 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 2312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2314 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2318 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2320 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2324 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2326 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2328 */	NdrFcShort( 0x20 ),	/* Offset= 32 (2360) */
/* 2330 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2332 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2334 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2336 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2338 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2344) */
/* 2340 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2342 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2344 */	
			0x13, 0x0,	/* FC_OP */
/* 2346 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2310) */
/* 2348 */	
			0x13, 0x0,	/* FC_OP */
/* 2350 */	NdrFcShort( 0xfffff9ce ),	/* Offset= -1586 (764) */
/* 2352 */	
			0x13, 0x0,	/* FC_OP */
/* 2354 */	NdrFcShort( 0xfffffdb8 ),	/* Offset= -584 (1770) */
/* 2356 */	
			0x13, 0x0,	/* FC_OP */
/* 2358 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2360) */
/* 2360 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 2362 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2366 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2366) */
/* 2368 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 2370 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 2372 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2374 */	NdrFcShort( 0xfffff702 ),	/* Offset= -2302 (72) */
/* 2376 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2378 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 2380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2382 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2384 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2386 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2388 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2392 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2394 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2396 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2360) */
/* 2398 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2400 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2402 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2404 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2406 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2408 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2410 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2412 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2416 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2418 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2420 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2422 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2426 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2428 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2430 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2432 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2434 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2438 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2440 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2442 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2444 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2448 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2450 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2452 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2454 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2456 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2458 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2460) */
/* 2460 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2462 */	NdrFcLong( 0x139 ),	/* 313 */
/* 2466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2470 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 2472 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2474 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2476 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 2478 */	
			0x11, 0x0,	/* FC_RP */
/* 2480 */	NdrFcShort( 0xfffff8f4 ),	/* Offset= -1804 (676) */
/* 2482 */	
			0x11, 0x0,	/* FC_RP */
/* 2484 */	NdrFcShort( 0xfffff902 ),	/* Offset= -1790 (694) */
/* 2486 */	
			0x11, 0x0,	/* FC_RP */
/* 2488 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2490) */
/* 2490 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 2492 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2494 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2496 */	NdrFcShort( 0xfffff8f6 ),	/* Offset= -1802 (694) */
/* 2498 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2500 */	NdrFcShort( 0xfffff8f2 ),	/* Offset= -1806 (694) */
/* 2502 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2504 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff8db ),	/* Offset= -1829 (676) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2508 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff8d7 ),	/* Offset= -1833 (676) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2512 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff8d3 ),	/* Offset= -1837 (676) */
			0x8,		/* FC_LONG */
/* 2516 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2518 */	
			0x12, 0x0,	/* FC_UP */
/* 2520 */	NdrFcShort( 0xfffff8de ),	/* Offset= -1826 (694) */
/* 2522 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2524 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2526) */
/* 2526 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2528 */	NdrFcLong( 0x138 ),	/* 312 */
/* 2532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2536 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 2538 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2540 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2542 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 2544 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2546 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2548) */
/* 2548 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2550 */	NdrFcLong( 0x13b ),	/* 315 */
/* 2554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2558 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 2560 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2562 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2564 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 2566 */	
			0x11, 0x0,	/* FC_RP */
/* 2568 */	NdrFcShort( 0x14 ),	/* Offset= 20 (2588) */
/* 2570 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2572 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2576 */	NdrFcShort( 0x8 ),	/* Offset= 8 (2584) */
/* 2578 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2580 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 2582 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2584 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 2586 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2588 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2592 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2594 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2596 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2598 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 2600 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2602 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2604 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2606 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (2570) */
/* 2608 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2610 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 2612 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 2614 */	
			0x11, 0x0,	/* FC_RP */
/* 2616 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2618) */
/* 2618 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 2620 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2622 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2624 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2626 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2628 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 2630 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2632 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2634 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2636 */	NdrFcShort( 0xffffff6e ),	/* Offset= -146 (2490) */
/* 2638 */	0x5c,		/* FC_PAD */
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
            },
            {
            LPSAFEARRAY_UserSize
            ,LPSAFEARRAY_UserMarshal
            ,LPSAFEARRAY_UserUnmarshal
            ,LPSAFEARRAY_UserFree
            }

        };


static void __RPC_USER IPropertyStorage_PROPVARIANTExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    PROPVARIANT *pS	=	( PROPVARIANT * )(pStubMsg->StackTop - 8);
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = (ULONG_PTR) ( ( unsigned short  )pS->vt );
}

static void __RPC_USER IPropertyStorage_CLIPDATAExprEval_0001( PMIDL_STUB_MESSAGE pStubMsg )
{
    CLIPDATA *pS	=	( CLIPDATA * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = (ULONG_PTR) ( pS->cbSize - 4 );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IPropertyStorage_PROPVARIANTExprEval_0000
    ,IPropertyStorage_CLIPDATAExprEval_0001
    };



/* Standard interface: __MIDL_itf_propidl_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IPropertyStorage, ver. 0.0,
   GUID={0x00000138,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPropertyStorage_FormatStringOffsetTable[] =
    {
    0,
    50,
    106,
    150,
    200,
    250,
    294,
    332,
    364,
    402,
    452,
    490
    };

static const MIDL_STUBLESS_PROXY_INFO IPropertyStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPropertyStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPropertyStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPropertyStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _IPropertyStorageProxyVtbl = 
{
    &IPropertyStorage_ProxyInfo,
    &IID_IPropertyStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::ReadMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::WriteMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::DeleteMultiple */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::ReadPropertyNames */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::WritePropertyNames */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::DeletePropertyNames */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::Commit */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::Revert */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::Enum */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::SetTimes */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::SetClass */ ,
    (void *) (INT_PTR) -1 /* IPropertyStorage::Stat */
};

const CInterfaceStubVtbl _IPropertyStorageStubVtbl =
{
    &IID_IPropertyStorage,
    &IPropertyStorage_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPropertySetStorage, ver. 0.0,
   GUID={0x0000013A,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPropertySetStorage_FormatStringOffsetTable[] =
    {
    528,
    590,
    640,
    678
    };

static const MIDL_STUBLESS_PROXY_INFO IPropertySetStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPropertySetStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPropertySetStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPropertySetStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IPropertySetStorageProxyVtbl = 
{
    &IPropertySetStorage_ProxyInfo,
    &IID_IPropertySetStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPropertySetStorage::Create */ ,
    (void *) (INT_PTR) -1 /* IPropertySetStorage::Open */ ,
    (void *) (INT_PTR) -1 /* IPropertySetStorage::Delete */ ,
    (void *) (INT_PTR) -1 /* IPropertySetStorage::Enum */
};

const CInterfaceStubVtbl _IPropertySetStorageStubVtbl =
{
    &IID_IPropertySetStorage,
    &IPropertySetStorage_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATPROPSTG, ver. 0.0,
   GUID={0x00000139,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATPROPSTG_FormatStringOffsetTable[] =
    {
    716,
    766,
    804,
    836
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPSTG_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPSTG_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumSTATPROPSTG_StubThunkTable[] = 
    {
    IEnumSTATPROPSTG_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumSTATPROPSTG_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPSTG_FormatStringOffsetTable[-3],
    &IEnumSTATPROPSTG_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATPROPSTGProxyVtbl = 
{
    &IEnumSTATPROPSTG_ProxyInfo,
    &IID_IEnumSTATPROPSTG,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumSTATPROPSTG_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSTG::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSTG::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSTG::Clone */
};

const CInterfaceStubVtbl _IEnumSTATPROPSTGStubVtbl =
{
    &IID_IEnumSTATPROPSTG,
    &IEnumSTATPROPSTG_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATPROPSETSTG, ver. 0.0,
   GUID={0x0000013B,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATPROPSETSTG_FormatStringOffsetTable[] =
    {
    874,
    766,
    804,
    678
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATPROPSETSTG_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPSETSTG_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumSTATPROPSETSTG_StubThunkTable[] = 
    {
    IEnumSTATPROPSETSTG_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumSTATPROPSETSTG_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATPROPSETSTG_FormatStringOffsetTable[-3],
    &IEnumSTATPROPSETSTG_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATPROPSETSTGProxyVtbl = 
{
    &IEnumSTATPROPSETSTG_ProxyInfo,
    &IID_IEnumSTATPROPSETSTG,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumSTATPROPSETSTG_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSETSTG::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSETSTG::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATPROPSETSTG::Clone */
};

const CInterfaceStubVtbl _IEnumSTATPROPSETSTGStubVtbl =
{
    &IID_IEnumSTATPROPSETSTG,
    &IEnumSTATPROPSETSTG_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_propidl_0109, ver. 0.0,
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

const CInterfaceProxyVtbl * _propidl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IPropertyStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATPROPSTGProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPropertySetStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATPROPSETSTGProxyVtbl,
    0
};

const CInterfaceStubVtbl * _propidl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IPropertyStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATPROPSTGStubVtbl,
    ( CInterfaceStubVtbl *) &_IPropertySetStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATPROPSETSTGStubVtbl,
    0
};

PCInterfaceName const _propidl_InterfaceNamesList[] = 
{
    "IPropertyStorage",
    "IEnumSTATPROPSTG",
    "IPropertySetStorage",
    "IEnumSTATPROPSETSTG",
    0
};


#define _propidl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _propidl, pIID, n)

int __stdcall _propidl_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _propidl, 4, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _propidl, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _propidl, 4, *pIndex )
    
}

const ExtendedProxyFileInfo propidl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _propidl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _propidl_StubVtblList,
    (const PCInterfaceName * ) & _propidl_InterfaceNamesList,
    0, // no delegation
    & _propidl_IID_Lookup, 
    4,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

