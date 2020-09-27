
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:37 2002
 */
/* Compiler settings for storext.idl:
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


#include "storext.h"

#define TYPE_FORMAT_STRING_SIZE   79                                
#define PROC_FORMAT_STRING_SIZE   205                               
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


extern const MIDL_SERVER_INFO IOverlappedCompletion_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOverlappedCompletion_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOverlappedStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOverlappedStream_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IOverlappedStream_RemoteReadOverlapped_Proxy( 
    IOverlappedStream * This,
    /* [size_is][in] */ byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbRead,
    /* [in] */ STGOVERLAPPED *lpOverlapped)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[48],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOverlappedStream_RemoteReadOverlapped_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IOverlappedStream *This;
        byte *pv;
        ULONG cb;
        ULONG *pcbRead;
        STGOVERLAPPED *lpOverlapped;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOverlappedStream_ReadOverlapped_Stub(
                                                (IOverlappedStream *) pParamStruct->This,
                                                pParamStruct->pv,
                                                pParamStruct->cb,
                                                pParamStruct->pcbRead,
                                                pParamStruct->lpOverlapped);
    
}

/* [call_as] */ HRESULT __stdcall IOverlappedStream_RemoteWriteOverlapped_Proxy( 
    IOverlappedStream * This,
    /* [size_is][in] */ byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten,
    /* [in] */ STGOVERLAPPED *lpOverlapped)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[102],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOverlappedStream_RemoteWriteOverlapped_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IOverlappedStream *This;
        byte *pv;
        ULONG cb;
        ULONG *pcbWritten;
        STGOVERLAPPED *lpOverlapped;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOverlappedStream_WriteOverlapped_Stub(
                                                 (IOverlappedStream *) pParamStruct->This,
                                                 pParamStruct->pv,
                                                 pParamStruct->cb,
                                                 pParamStruct->pcbWritten,
                                                 pParamStruct->lpOverlapped);
    
}


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

	/* Procedure OnComplete */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 10 */	NdrFcShort( 0x10 ),	/* 16 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hr */

/* 24 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbTransferred */

/* 30 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpOverlapped */

/* 36 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 38 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 40 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 42 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteReadOverlapped */

/* 48 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 50 */	NdrFcLong( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0xe ),	/* 14 */
/* 56 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 58 */	NdrFcShort( 0x8 ),	/* 8 */
/* 60 */	NdrFcShort( 0x24 ),	/* 36 */
/* 62 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 64 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x1 ),	/* 1 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pv */

/* 72 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	NdrFcShort( 0x3e ),	/* Type Offset=62 */

	/* Parameter cb */

/* 78 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbRead */

/* 84 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 86 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 88 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpOverlapped */

/* 90 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 92 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 94 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 96 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 98 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteWriteOverlapped */

/* 102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0xf ),	/* 15 */
/* 110 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 114 */	NdrFcShort( 0x24 ),	/* 36 */
/* 116 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 118 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x1 ),	/* 1 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pv */

/* 126 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 128 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 130 */	NdrFcShort( 0x3e ),	/* Type Offset=62 */

	/* Parameter cb */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbWritten */

/* 138 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 140 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpOverlapped */

/* 144 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 146 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 148 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 152 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOverlappedResult */

/* 156 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x10 ),	/* 16 */
/* 164 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 166 */	NdrFcShort( 0x8 ),	/* 8 */
/* 168 */	NdrFcShort( 0x24 ),	/* 36 */
/* 170 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 172 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lpOverlapped */

/* 180 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 182 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 184 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Parameter plcbTransfer */

/* 186 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 188 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fWait */

/* 192 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 194 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 196 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 198 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 200 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 202 */	0x8,		/* FC_LONG */
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
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x20 ),	/* Offset= 32 (36) */
/*  6 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  8 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x4 ),	/* 4 */
/* 14 */	NdrFcShort( 0x4 ),	/* 4 */
/* 16 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (6) */
/* 18 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 20 */	NdrFcLong( 0x521a28f0 ),	/* 1377446128 */
/* 24 */	NdrFcShort( 0xe40b ),	/* -7157 */
/* 26 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 28 */	0xb2,		/* 178 */
			0xc9,		/* 201 */
/* 30 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 32 */	0x0,		/* 0 */
			0x68,		/* 104 */
/* 34 */	0x9,		/* 9 */
			0x37,		/* 55 */
/* 36 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 38 */	NdrFcShort( 0x1c ),	/* 28 */
/* 40 */	NdrFcShort( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x10 ),	/* Offset= 16 (58) */
/* 44 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 46 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 48 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 50 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (8) */
/* 52 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 54 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (18) */
/* 56 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 58 */	
			0x11, 0x0,	/* FC_RP */
/* 60 */	NdrFcShort( 0x2 ),	/* Offset= 2 (62) */
/* 62 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 64 */	NdrFcShort( 0x1 ),	/* 1 */
/* 66 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 68 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 70 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 72 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 74 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 76 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            HEVENT_UserSize
            ,HEVENT_UserMarshal
            ,HEVENT_UserUnmarshal
            ,HEVENT_UserFree
            }

        };



/* Standard interface: __MIDL_itf_storext_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOverlappedCompletion, ver. 0.0,
   GUID={0x521a28f0,0xe40b,0x11ce,{0xb2,0xc9,0x00,0xaa,0x00,0x68,0x09,0x37}} */

#pragma code_seg(".orpc")
static const unsigned short IOverlappedCompletion_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IOverlappedCompletion_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOverlappedCompletion_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOverlappedCompletion_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOverlappedCompletion_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IOverlappedCompletionProxyVtbl = 
{
    &IOverlappedCompletion_ProxyInfo,
    &IID_IOverlappedCompletion,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOverlappedCompletion::OnComplete */
};

const CInterfaceStubVtbl _IOverlappedCompletionStubVtbl =
{
    &IID_IOverlappedCompletion,
    &IOverlappedCompletion_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISequentialStream, ver. 0.0,
   GUID={0x0c733a30,0x2a1c,0x11ce,{0xad,0xe5,0x00,0xaa,0x00,0x44,0x77,0x3d}} */


/* Object interface: IStream, ver. 0.0,
   GUID={0x0000000c,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOverlappedStream, ver. 0.0,
   GUID={0x49384070,0xe40a,0x11ce,{0xb2,0xc9,0x00,0xaa,0x00,0x68,0x09,0x37}} */

#pragma code_seg(".orpc")
static const unsigned short IOverlappedStream_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    48,
    102,
    156
    };

static const MIDL_STUBLESS_PROXY_INFO IOverlappedStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOverlappedStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IOverlappedStream_StubThunkTable[] = 
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
    IOverlappedStream_RemoteReadOverlapped_Thunk,
    IOverlappedStream_RemoteWriteOverlapped_Thunk,
    0
    };

static const MIDL_SERVER_INFO IOverlappedStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOverlappedStream_FormatStringOffsetTable[-3],
    &IOverlappedStream_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _IOverlappedStreamProxyVtbl = 
{
    &IOverlappedStream_ProxyInfo,
    &IID_IOverlappedStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* ISequentialStream_Read_Proxy */ ,
    0 /* ISequentialStream_Write_Proxy */ ,
    0 /* IStream_Seek_Proxy */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::SetSize */ ,
    0 /* IStream_CopyTo_Proxy */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::Commit */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::Revert */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::LockRegion */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::UnlockRegion */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::Stat */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::Clone */ ,
    IOverlappedStream_ReadOverlapped_Proxy ,
    IOverlappedStream_WriteOverlapped_Proxy ,
    (void *) (INT_PTR) -1 /* IOverlappedStream::GetOverlappedResult */
};


static const PRPC_STUB_FUNCTION IOverlappedStream_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IOverlappedStreamStubVtbl =
{
    &IID_IOverlappedStream,
    &IOverlappedStream_ServerInfo,
    17,
    &IOverlappedStream_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
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

const CInterfaceProxyVtbl * _storext_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IOverlappedStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOverlappedCompletionProxyVtbl,
    0
};

const CInterfaceStubVtbl * _storext_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IOverlappedStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IOverlappedCompletionStubVtbl,
    0
};

PCInterfaceName const _storext_InterfaceNamesList[] = 
{
    "IOverlappedStream",
    "IOverlappedCompletion",
    0
};

const IID *  _storext_BaseIIDList[] = 
{
    &IID_IStream,
    0,
    0
};


#define _storext_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _storext, pIID, n)

int __stdcall _storext_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _storext, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _storext, 2, *pIndex )
    
}

const ExtendedProxyFileInfo storext_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _storext_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _storext_StubVtblList,
    (const PCInterfaceName * ) & _storext_InterfaceNamesList,
    (const IID ** ) & _storext_BaseIIDList,
    & _storext_IID_Lookup, 
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
/* at Sun May 05 19:07:37 2002
 */
/* Compiler settings for storext.idl:
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


#include "storext.h"

#define TYPE_FORMAT_STRING_SIZE   81                                
#define PROC_FORMAT_STRING_SIZE   213                               
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


extern const MIDL_SERVER_INFO IOverlappedCompletion_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOverlappedCompletion_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOverlappedStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOverlappedStream_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IOverlappedStream_RemoteReadOverlapped_Proxy( 
    IOverlappedStream * This,
    /* [size_is][in] */ byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbRead,
    /* [in] */ STGOVERLAPPED *lpOverlapped)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[50],
                  ( unsigned char * )This,
                  pv,
                  cb,
                  pcbRead,
                  lpOverlapped);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOverlappedStream_RemoteReadOverlapped_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IOverlappedStream *This;
        byte *pv;
        ULONG cb;
        char Pad0[4];
        ULONG *pcbRead;
        STGOVERLAPPED *lpOverlapped;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOverlappedStream_ReadOverlapped_Stub(
                                                (IOverlappedStream *) pParamStruct->This,
                                                pParamStruct->pv,
                                                pParamStruct->cb,
                                                pParamStruct->pcbRead,
                                                pParamStruct->lpOverlapped);
    
}

/* [call_as] */ HRESULT __stdcall IOverlappedStream_RemoteWriteOverlapped_Proxy( 
    IOverlappedStream * This,
    /* [size_is][in] */ byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten,
    /* [in] */ STGOVERLAPPED *lpOverlapped)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[106],
                  ( unsigned char * )This,
                  pv,
                  cb,
                  pcbWritten,
                  lpOverlapped);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOverlappedStream_RemoteWriteOverlapped_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IOverlappedStream *This;
        byte *pv;
        ULONG cb;
        char Pad0[4];
        ULONG *pcbWritten;
        STGOVERLAPPED *lpOverlapped;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOverlappedStream_WriteOverlapped_Stub(
                                                 (IOverlappedStream *) pParamStruct->This,
                                                 pParamStruct->pv,
                                                 pParamStruct->cb,
                                                 pParamStruct->pcbWritten,
                                                 pParamStruct->lpOverlapped);
    
}


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure OnComplete */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 10 */	NdrFcShort( 0x10 ),	/* 16 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hr */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbTransferred */

/* 32 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpOverlapped */

/* 38 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteReadOverlapped */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0xe ),	/* 14 */
/* 58 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	NdrFcShort( 0x24 ),	/* 36 */
/* 64 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 66 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x1 ),	/* 1 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pv */

/* 76 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 78 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 80 */	NdrFcShort( 0x40 ),	/* Type Offset=64 */

	/* Parameter cb */

/* 82 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 84 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 86 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbRead */

/* 88 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 90 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 92 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpOverlapped */

/* 94 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 96 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 98 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 102 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteWriteOverlapped */

/* 106 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0xf ),	/* 15 */
/* 114 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 116 */	NdrFcShort( 0x8 ),	/* 8 */
/* 118 */	NdrFcShort( 0x24 ),	/* 36 */
/* 120 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 122 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x1 ),	/* 1 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pv */

/* 132 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 134 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 136 */	NdrFcShort( 0x40 ),	/* Type Offset=64 */

	/* Parameter cb */

/* 138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 140 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbWritten */

/* 144 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 146 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpOverlapped */

/* 150 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 152 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 154 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 156 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 158 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOverlappedResult */

/* 162 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 164 */	NdrFcLong( 0x0 ),	/* 0 */
/* 168 */	NdrFcShort( 0x10 ),	/* 16 */
/* 170 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 172 */	NdrFcShort( 0x8 ),	/* 8 */
/* 174 */	NdrFcShort( 0x24 ),	/* 36 */
/* 176 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 178 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lpOverlapped */

/* 188 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 190 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 192 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Parameter plcbTransfer */

/* 194 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 196 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fWait */

/* 200 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 202 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 208 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 210 */	0x8,		/* FC_LONG */
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
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x20 ),	/* Offset= 32 (36) */
/*  6 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  8 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	NdrFcShort( 0x4 ),	/* 4 */
/* 16 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (6) */
/* 18 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 20 */	NdrFcLong( 0x521a28f0 ),	/* 1377446128 */
/* 24 */	NdrFcShort( 0xe40b ),	/* -7157 */
/* 26 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 28 */	0xb2,		/* 178 */
			0xc9,		/* 201 */
/* 30 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 32 */	0x0,		/* 0 */
			0x68,		/* 104 */
/* 34 */	0x9,		/* 9 */
			0x37,		/* 55 */
/* 36 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 38 */	NdrFcShort( 0x28 ),	/* 40 */
/* 40 */	NdrFcShort( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x12 ),	/* Offset= 18 (60) */
/* 44 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 46 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 48 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 50 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (8) */
/* 52 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 54 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (18) */
/* 56 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 58 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 60 */	
			0x11, 0x0,	/* FC_RP */
/* 62 */	NdrFcShort( 0x2 ),	/* Offset= 2 (64) */
/* 64 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 66 */	NdrFcShort( 0x1 ),	/* 1 */
/* 68 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 70 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 72 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 74 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 76 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 78 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            HEVENT_UserSize
            ,HEVENT_UserMarshal
            ,HEVENT_UserUnmarshal
            ,HEVENT_UserFree
            }

        };



/* Standard interface: __MIDL_itf_storext_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOverlappedCompletion, ver. 0.0,
   GUID={0x521a28f0,0xe40b,0x11ce,{0xb2,0xc9,0x00,0xaa,0x00,0x68,0x09,0x37}} */

#pragma code_seg(".orpc")
static const unsigned short IOverlappedCompletion_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IOverlappedCompletion_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOverlappedCompletion_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOverlappedCompletion_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOverlappedCompletion_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IOverlappedCompletionProxyVtbl = 
{
    &IOverlappedCompletion_ProxyInfo,
    &IID_IOverlappedCompletion,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOverlappedCompletion::OnComplete */
};

const CInterfaceStubVtbl _IOverlappedCompletionStubVtbl =
{
    &IID_IOverlappedCompletion,
    &IOverlappedCompletion_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISequentialStream, ver. 0.0,
   GUID={0x0c733a30,0x2a1c,0x11ce,{0xad,0xe5,0x00,0xaa,0x00,0x44,0x77,0x3d}} */


/* Object interface: IStream, ver. 0.0,
   GUID={0x0000000c,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOverlappedStream, ver. 0.0,
   GUID={0x49384070,0xe40a,0x11ce,{0xb2,0xc9,0x00,0xaa,0x00,0x68,0x09,0x37}} */

#pragma code_seg(".orpc")
static const unsigned short IOverlappedStream_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    50,
    106,
    162
    };

static const MIDL_STUBLESS_PROXY_INFO IOverlappedStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOverlappedStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IOverlappedStream_StubThunkTable[] = 
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
    IOverlappedStream_RemoteReadOverlapped_Thunk,
    IOverlappedStream_RemoteWriteOverlapped_Thunk,
    0
    };

static const MIDL_SERVER_INFO IOverlappedStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOverlappedStream_FormatStringOffsetTable[-3],
    &IOverlappedStream_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _IOverlappedStreamProxyVtbl = 
{
    &IOverlappedStream_ProxyInfo,
    &IID_IOverlappedStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* ISequentialStream_Read_Proxy */ ,
    0 /* ISequentialStream_Write_Proxy */ ,
    0 /* IStream_Seek_Proxy */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::SetSize */ ,
    0 /* IStream_CopyTo_Proxy */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::Commit */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::Revert */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::LockRegion */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::UnlockRegion */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::Stat */ ,
    0 /* (void *) (INT_PTR) -1 /* IStream::Clone */ ,
    IOverlappedStream_ReadOverlapped_Proxy ,
    IOverlappedStream_WriteOverlapped_Proxy ,
    (void *) (INT_PTR) -1 /* IOverlappedStream::GetOverlappedResult */
};


static const PRPC_STUB_FUNCTION IOverlappedStream_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IOverlappedStreamStubVtbl =
{
    &IID_IOverlappedStream,
    &IOverlappedStream_ServerInfo,
    17,
    &IOverlappedStream_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
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

const CInterfaceProxyVtbl * _storext_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IOverlappedStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOverlappedCompletionProxyVtbl,
    0
};

const CInterfaceStubVtbl * _storext_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IOverlappedStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IOverlappedCompletionStubVtbl,
    0
};

PCInterfaceName const _storext_InterfaceNamesList[] = 
{
    "IOverlappedStream",
    "IOverlappedCompletion",
    0
};

const IID *  _storext_BaseIIDList[] = 
{
    &IID_IStream,
    0,
    0
};


#define _storext_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _storext, pIID, n)

int __stdcall _storext_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _storext, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _storext, 2, *pIndex )
    
}

const ExtendedProxyFileInfo storext_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _storext_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _storext_StubVtblList,
    (const PCInterfaceName * ) & _storext_InterfaceNamesList,
    (const IID ** ) & _storext_BaseIIDList,
    & _storext_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

