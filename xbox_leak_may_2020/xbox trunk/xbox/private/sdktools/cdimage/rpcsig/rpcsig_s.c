
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the RPC server stubs */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun Apr 07 15:08:27 2002
 */
/* Compiler settings for rpcsig.idl:
    Oi, W3, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, app_config, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)
#include <string.h>
#include "rpcsig.h"

#define TYPE_FORMAT_STRING_SIZE   39                                
#define PROC_FORMAT_STRING_SIZE   57                                
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

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

/* Standard interface: RpcSignatureServerInterface, ver. 2.0,
   GUID={0x9437a860,0xcae9,0x11cf,{0x99,0xb3,0x00,0x00,0x4c,0x75,0x27,0xda}} */


extern const MIDL_SERVER_INFO RpcSignatureServerInterface_ServerInfo;

extern RPC_DISPATCH_TABLE RpcSignatureServerInterface_v2_0_DispatchTable;

static const RPC_SERVER_INTERFACE RpcSignatureServerInterface___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x9437a860,0xcae9,0x11cf,{0x99,0xb3,0x00,0x00,0x4c,0x75,0x27,0xda}},{2,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    &RpcSignatureServerInterface_v2_0_DispatchTable,
    0,
    0,
    0,
    &RpcSignatureServerInterface_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE RpcSignatureServerInterface_v2_0_s_ifspec = (RPC_IF_HANDLE)& RpcSignatureServerInterface___RpcServerInterface;

extern const MIDL_STUB_DESC RpcSignatureServerInterface_StubDesc;


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/* 16 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 18 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 20 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x3,		/* FC_SMALL */
/* 22 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 24 */	NdrFcLong( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0x1 ),	/* 1 */
/* 30 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 32 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 34 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 36 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/* 38 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 40 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 42 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 44 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 46 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 48 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */
/* 50 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 52 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */
/* 54 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x3,		/* FC_SMALL */

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
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x0,	/* FC_RP */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x1 ),	/* 1 */
/* 14 */	0x40,		/* Corr desc:  constant, val=16 */
			0x0,		/* 0 */
/* 16 */	NdrFcShort( 0x10 ),	/* 16 */
/* 18 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 20 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 22 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 24 */	
			0x11, 0x0,	/* FC_RP */
/* 26 */	NdrFcShort( 0x2 ),	/* Offset= 2 (28) */
/* 28 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 30 */	NdrFcShort( 0x1 ),	/* 1 */
/* 32 */	0x40,		/* Corr desc:  constant, val=128 */
			0x0,		/* 0 */
/* 34 */	NdrFcShort( 0x80 ),	/* 128 */
/* 36 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const unsigned short RpcSignatureServerInterface_FormatStringOffsetTable[] =
    {
    0,
    22
    };


static const MIDL_STUB_DESC RpcSignatureServerInterface_StubDesc = 
    {
    (void *)& RpcSignatureServerInterface___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

static RPC_DISPATCH_FUNCTION RpcSignatureServerInterface_table[] =
    {
    NdrServerCall,
    NdrServerCall,
    0
    };
RPC_DISPATCH_TABLE RpcSignatureServerInterface_v2_0_DispatchTable = 
    {
    2,
    RpcSignatureServerInterface_table
    };

static const SERVER_ROUTINE RpcSignatureServerInterface_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)RpcSignatureServerAuthenticate,
    (SERVER_ROUTINE)RpcSignatureServerGenSignature
    };

static const MIDL_SERVER_INFO RpcSignatureServerInterface_ServerInfo = 
    {
    &RpcSignatureServerInterface_StubDesc,
    RpcSignatureServerInterface_ServerRoutineTable,
    __MIDL_ProcFormatString.Format,
    RpcSignatureServerInterface_FormatStringOffsetTable,
    0,
    0,
    0,
    0};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

