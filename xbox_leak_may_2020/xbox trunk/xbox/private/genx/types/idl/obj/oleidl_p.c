
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:33 2002
 */
/* Compiler settings for oleidl.idl:
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


#include "oleidl.h"

#define TYPE_FORMAT_STRING_SIZE   1125                              
#define PROC_FORMAT_STRING_SIZE   3535                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   6            

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


extern const MIDL_SERVER_INFO IOleCache_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleCache_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleCache2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleCache2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IOleCache2_RemoteUpdateCache_Proxy( 
    IOleCache2 * This,
    /* [in] */ LPDATAOBJECT pDataObject,
    /* [in] */ DWORD grfUpdf,
    /* [in] */ LONG_PTR pReserved)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[204],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOleCache2_RemoteUpdateCache_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IOleCache2 *This;
        LPDATAOBJECT pDataObject;
        DWORD grfUpdf;
        LONG_PTR pReserved;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOleCache2_UpdateCache_Stub(
                                      (IOleCache2 *) pParamStruct->This,
                                      pParamStruct->pDataObject,
                                      pParamStruct->grfUpdf,
                                      pParamStruct->pReserved);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleCacheControl_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleCacheControl_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IParseDisplayName_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IParseDisplayName_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleContainer_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleContainer_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleClientSite_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleClientSite_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleObject_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleWindow_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleWindow_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleLink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleLink_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleItemContainer_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleItemContainer_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceUIWindow_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceUIWindow_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceActiveObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceActiveObject_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_RemoteTranslateAccelerator_Proxy( 
    IOleInPlaceActiveObject * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2274],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOleInPlaceActiveObject_RemoteTranslateAccelerator_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IOleInPlaceActiveObject *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOleInPlaceActiveObject_TranslateAccelerator_Stub((IOleInPlaceActiveObject *) pParamStruct->This);
    
}

/* [input_sync][call_as] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_RemoteResizeBorder_Proxy( 
    IOleInPlaceActiveObject * This,
    /* [in] */ LPCRECT prcBorder,
    /* [in] */ REFIID riid,
    /* [iid_is][unique][in] */ IOleInPlaceUIWindow *pUIWindow,
    /* [in] */ BOOL fFrameWindow)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2376],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOleInPlaceActiveObject_RemoteResizeBorder_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IOleInPlaceActiveObject *This;
        LPCRECT prcBorder;
        REFIID riid;
        IOleInPlaceUIWindow *pUIWindow;
        BOOL fFrameWindow;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOleInPlaceActiveObject_ResizeBorder_Stub(
                                                    (IOleInPlaceActiveObject *) pParamStruct->This,
                                                    pParamStruct->prcBorder,
                                                    pParamStruct->riid,
                                                    pParamStruct->pUIWindow,
                                                    pParamStruct->fFrameWindow);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceFrame_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceFrame_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceObject_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceSite_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceSite_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IContinue_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IContinue_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IViewObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IViewObject_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteDraw_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ ULONG_PTR hdcTargetDev,
    /* [in] */ ULONG_PTR hdcDraw,
    /* [in] */ LPCRECTL lprcBounds,
    /* [unique][in] */ LPCRECTL lprcWBounds,
    /* [in] */ IContinue *pContinue)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2940],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IViewObject_RemoteDraw_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IViewObject *This;
        DWORD dwDrawAspect;
        LONG lindex;
        ULONG_PTR pvAspect;
        DVTARGETDEVICE *ptd;
        ULONG_PTR hdcTargetDev;
        ULONG_PTR hdcDraw;
        LPCRECTL lprcBounds;
        LPCRECTL lprcWBounds;
        IContinue *pContinue;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IViewObject_Draw_Stub(
                                (IViewObject *) pParamStruct->This,
                                pParamStruct->dwDrawAspect,
                                pParamStruct->lindex,
                                pParamStruct->pvAspect,
                                pParamStruct->ptd,
                                pParamStruct->hdcTargetDev,
                                pParamStruct->hdcDraw,
                                pParamStruct->lprcBounds,
                                pParamStruct->lprcWBounds,
                                pParamStruct->pContinue);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteGetColorSet_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ ULONG_PTR hicTargetDev,
    /* [out] */ LOGPALETTE **ppColorSet)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3024],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IViewObject_RemoteGetColorSet_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IViewObject *This;
        DWORD dwDrawAspect;
        LONG lindex;
        ULONG_PTR pvAspect;
        DVTARGETDEVICE *ptd;
        ULONG_PTR hicTargetDev;
        LOGPALETTE **ppColorSet;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IViewObject_GetColorSet_Stub(
                                       (IViewObject *) pParamStruct->This,
                                       pParamStruct->dwDrawAspect,
                                       pParamStruct->lindex,
                                       pParamStruct->pvAspect,
                                       pParamStruct->ptd,
                                       pParamStruct->hicTargetDev,
                                       pParamStruct->ppColorSet);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteFreeze_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [out] */ DWORD *pdwFreeze)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3090],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IViewObject_RemoteFreeze_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IViewObject *This;
        DWORD dwDrawAspect;
        LONG lindex;
        ULONG_PTR pvAspect;
        DWORD *pdwFreeze;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IViewObject_Freeze_Stub(
                                  (IViewObject *) pParamStruct->This,
                                  pParamStruct->dwDrawAspect,
                                  pParamStruct->lindex,
                                  pParamStruct->pvAspect,
                                  pParamStruct->pdwFreeze);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteGetAdvise_Proxy( 
    IViewObject * This,
    /* [out] */ DWORD *pAspects,
    /* [out] */ DWORD *pAdvf,
    /* [out] */ IAdviseSink **ppAdvSink)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3192],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IViewObject_RemoteGetAdvise_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IViewObject *This;
        DWORD *pAspects;
        DWORD *pAdvf;
        IAdviseSink **ppAdvSink;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IViewObject_GetAdvise_Stub(
                                     (IViewObject *) pParamStruct->This,
                                     pParamStruct->pAspects,
                                     pParamStruct->pAdvf,
                                     pParamStruct->ppAdvSink);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IViewObject2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IViewObject2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDropTarget_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDropTarget_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumOLEVERB_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumOLEVERB_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumOLEVERB_RemoteNext_Proxy( 
    IEnumOLEVERB * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPOLEVERB rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3450],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumOLEVERB_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumOLEVERB *This;
        ULONG celt;
        LPOLEVERB rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumOLEVERB_Next_Stub(
                                 (IEnumOLEVERB *) pParamStruct->This,
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

	/* Procedure Cache */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x2 ),	/* 2 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 24 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter advf */

/* 30 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwConnection */

/* 36 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 38 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 40 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 42 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure ContextSensitiveHelp */


	/* Procedure Uncache */

/* 48 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 50 */	NdrFcLong( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x4 ),	/* 4 */
/* 56 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 58 */	NdrFcShort( 0x8 ),	/* 8 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 64 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */


	/* Parameter fEnterMode */


	/* Parameter dwConnection */

/* 72 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumCache */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x5 ),	/* 5 */
/* 92 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x8 ),	/* 8 */
/* 98 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 100 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenumSTATDATA */

/* 108 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 112 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */

	/* Return value */

/* 114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InitCache */

/* 120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x6 ),	/* 6 */
/* 128 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 134 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 136 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObject */

/* 144 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 146 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 148 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Return value */

/* 150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 152 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetData */

/* 156 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x7 ),	/* 7 */
/* 164 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 166 */	NdrFcShort( 0x8 ),	/* 8 */
/* 168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 170 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 172 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x13 ),	/* 19 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 180 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 182 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 184 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter pmedium */

/* 186 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 188 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 190 */	NdrFcShort( 0x86 ),	/* Type Offset=134 */

	/* Parameter fRelease */

/* 192 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 194 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 196 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 198 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 200 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 202 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteUpdateCache */

/* 204 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 206 */	NdrFcLong( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 212 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 214 */	NdrFcShort( 0x10 ),	/* 16 */
/* 216 */	NdrFcShort( 0x8 ),	/* 8 */
/* 218 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 220 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObject */

/* 228 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 230 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 232 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Parameter grfUpdf */

/* 234 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 236 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pReserved */

/* 240 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 242 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 246 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 248 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnableModeless */


	/* Procedure DiscardCache */

/* 252 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x9 ),	/* 9 */
/* 260 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 262 */	NdrFcShort( 0x8 ),	/* 8 */
/* 264 */	NdrFcShort( 0x8 ),	/* 8 */
/* 266 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 268 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fEnable */


	/* Parameter dwDiscardOptions */

/* 276 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 278 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 282 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 284 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 286 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnRun */

/* 288 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 290 */	NdrFcLong( 0x0 ),	/* 0 */
/* 294 */	NdrFcShort( 0x3 ),	/* 3 */
/* 296 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0x8 ),	/* 8 */
/* 302 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 304 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 310 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObject */

/* 312 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 314 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 316 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Return value */

/* 318 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 320 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnStop */

/* 324 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 326 */	NdrFcLong( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x4 ),	/* 4 */
/* 332 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 338 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 340 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 350 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ParseDisplayName */

/* 354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x3 ),	/* 3 */
/* 362 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x24 ),	/* 36 */
/* 368 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 370 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 378 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 380 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 382 */	NdrFcShort( 0x218 ),	/* Type Offset=536 */

	/* Parameter pszDisplayName */

/* 384 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 386 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 388 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Parameter pchEaten */

/* 390 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 392 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmkOut */

/* 396 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 398 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 400 */	NdrFcShort( 0x22e ),	/* Type Offset=558 */

	/* Return value */

/* 402 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 404 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumObjects */

/* 408 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 410 */	NdrFcLong( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x4 ),	/* 4 */
/* 416 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 418 */	NdrFcShort( 0x8 ),	/* 8 */
/* 420 */	NdrFcShort( 0x8 ),	/* 8 */
/* 422 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 424 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfFlags */

/* 432 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 434 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenum */

/* 438 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 440 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 442 */	NdrFcShort( 0x244 ),	/* Type Offset=580 */

	/* Return value */

/* 444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 446 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LockContainer */

/* 450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0x5 ),	/* 5 */
/* 458 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 464 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 466 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fLock */

/* 474 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FContinue */


	/* Procedure SaveObject */

/* 486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x3 ),	/* 3 */
/* 494 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x8 ),	/* 8 */
/* 500 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 502 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 512 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMoniker */

/* 516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 522 */	NdrFcShort( 0x4 ),	/* 4 */
/* 524 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 526 */	NdrFcShort( 0x10 ),	/* 16 */
/* 528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 530 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 532 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 538 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAssign */

/* 540 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 542 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 544 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwWhichMoniker */

/* 546 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 548 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 550 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmk */

/* 552 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 554 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 556 */	NdrFcShort( 0x22e ),	/* Type Offset=558 */

	/* Return value */

/* 558 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 560 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 562 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContainer */

/* 564 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 566 */	NdrFcLong( 0x0 ),	/* 0 */
/* 570 */	NdrFcShort( 0x5 ),	/* 5 */
/* 572 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 576 */	NdrFcShort( 0x8 ),	/* 8 */
/* 578 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 580 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppContainer */

/* 588 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 590 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 592 */	NdrFcShort( 0x25a ),	/* Type Offset=602 */

	/* Return value */

/* 594 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 596 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnInPlaceActivate */


	/* Procedure UIDeactivate */


	/* Procedure ShowObject */

/* 600 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 602 */	NdrFcLong( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x6 ),	/* 6 */
/* 608 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x8 ),	/* 8 */
/* 614 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 616 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 624 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 626 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnShowWindow */

/* 630 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 632 */	NdrFcLong( 0x0 ),	/* 0 */
/* 636 */	NdrFcShort( 0x7 ),	/* 7 */
/* 638 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 640 */	NdrFcShort( 0x8 ),	/* 8 */
/* 642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 644 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 646 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 652 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fShow */

/* 654 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 656 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 660 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 662 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReactivateAndUndo */


	/* Procedure RequestNewObjectLayout */

/* 666 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 668 */	NdrFcLong( 0x0 ),	/* 0 */
/* 672 */	NdrFcShort( 0x8 ),	/* 8 */
/* 674 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 678 */	NdrFcShort( 0x8 ),	/* 8 */
/* 680 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 682 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 690 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 692 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetClientSite */

/* 696 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 698 */	NdrFcLong( 0x0 ),	/* 0 */
/* 702 */	NdrFcShort( 0x3 ),	/* 3 */
/* 704 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 708 */	NdrFcShort( 0x8 ),	/* 8 */
/* 710 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 712 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pClientSite */

/* 720 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 722 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 724 */	NdrFcShort( 0x270 ),	/* Type Offset=624 */

	/* Return value */

/* 726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 728 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClientSite */

/* 732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 738 */	NdrFcShort( 0x4 ),	/* 4 */
/* 740 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 746 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 748 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppClientSite */

/* 756 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 758 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 760 */	NdrFcShort( 0x282 ),	/* Type Offset=642 */

	/* Return value */

/* 762 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 764 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHostNames */

/* 768 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 770 */	NdrFcLong( 0x0 ),	/* 0 */
/* 774 */	NdrFcShort( 0x5 ),	/* 5 */
/* 776 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0x8 ),	/* 8 */
/* 782 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 784 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szContainerApp */

/* 792 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 794 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 796 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Parameter szContainerObj */

/* 798 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 800 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 802 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 804 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 806 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 808 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Unfreeze */


	/* Procedure Close */

/* 810 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 812 */	NdrFcLong( 0x0 ),	/* 0 */
/* 816 */	NdrFcShort( 0x6 ),	/* 6 */
/* 818 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 820 */	NdrFcShort( 0x8 ),	/* 8 */
/* 822 */	NdrFcShort( 0x8 ),	/* 8 */
/* 824 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 826 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x0 ),	/* 0 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwFreeze */


	/* Parameter dwSaveOption */

/* 834 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 836 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 838 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 840 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 842 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMoniker */

/* 846 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 848 */	NdrFcLong( 0x0 ),	/* 0 */
/* 852 */	NdrFcShort( 0x7 ),	/* 7 */
/* 854 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 856 */	NdrFcShort( 0x8 ),	/* 8 */
/* 858 */	NdrFcShort( 0x8 ),	/* 8 */
/* 860 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 862 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 864 */	NdrFcShort( 0x0 ),	/* 0 */
/* 866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 868 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwWhichMoniker */

/* 870 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 872 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 874 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pmk */

/* 876 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 878 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 880 */	NdrFcShort( 0x232 ),	/* Type Offset=562 */

	/* Return value */

/* 882 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 884 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMoniker */

/* 888 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 890 */	NdrFcLong( 0x0 ),	/* 0 */
/* 894 */	NdrFcShort( 0x8 ),	/* 8 */
/* 896 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 898 */	NdrFcShort( 0x10 ),	/* 16 */
/* 900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 902 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 904 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 906 */	NdrFcShort( 0x0 ),	/* 0 */
/* 908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 910 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAssign */

/* 912 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 914 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 916 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwWhichMoniker */

/* 918 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 920 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmk */

/* 924 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 926 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 928 */	NdrFcShort( 0x22e ),	/* Type Offset=558 */

	/* Return value */

/* 930 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 932 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InitFromData */

/* 936 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 938 */	NdrFcLong( 0x0 ),	/* 0 */
/* 942 */	NdrFcShort( 0x9 ),	/* 9 */
/* 944 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 946 */	NdrFcShort( 0x10 ),	/* 16 */
/* 948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 950 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 952 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObject */

/* 960 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 962 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 964 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Parameter fCreation */

/* 966 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 968 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwReserved */

/* 972 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 974 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 976 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 978 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 980 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 982 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClipboardData */

/* 984 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 986 */	NdrFcLong( 0x0 ),	/* 0 */
/* 990 */	NdrFcShort( 0xa ),	/* 10 */
/* 992 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 994 */	NdrFcShort( 0x8 ),	/* 8 */
/* 996 */	NdrFcShort( 0x8 ),	/* 8 */
/* 998 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1000 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwReserved */

/* 1008 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1010 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppDataObject */

/* 1014 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1016 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1018 */	NdrFcShort( 0x286 ),	/* Type Offset=646 */

	/* Return value */

/* 1020 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1022 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DoVerb */

/* 1026 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1028 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0xb ),	/* 11 */
/* 1034 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1036 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1038 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1040 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 1042 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1048 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter iVerb */

/* 1050 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1052 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1054 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpmsg */

/* 1056 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1058 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1060 */	NdrFcShort( 0x28a ),	/* Type Offset=650 */

	/* Parameter pActiveSite */

/* 1062 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1064 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1066 */	NdrFcShort( 0x270 ),	/* Type Offset=624 */

	/* Parameter lindex */

/* 1068 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1070 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hwndParent */

/* 1074 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1076 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1078 */	NdrFcShort( 0x2a6 ),	/* Type Offset=678 */

	/* Parameter lprcPosRect */

/* 1080 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 1082 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1084 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Return value */

/* 1086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1088 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumVerbs */

/* 1092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1098 */	NdrFcShort( 0xc ),	/* 12 */
/* 1100 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1106 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1108 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1114 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppEnumOleVerb */

/* 1116 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1118 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1120 */	NdrFcShort( 0x2dc ),	/* Type Offset=732 */

	/* Return value */

/* 1122 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1124 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeactivateAndUndo */


	/* Procedure Update */

/* 1128 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1134 */	NdrFcShort( 0xd ),	/* 13 */
/* 1136 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1140 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1142 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1144 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 1152 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1154 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsUpToDate */

/* 1158 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1164 */	NdrFcShort( 0xe ),	/* 14 */
/* 1166 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1172 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1174 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1180 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1182 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1184 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1186 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetUserClassID */

/* 1188 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1190 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1194 */	NdrFcShort( 0xf ),	/* 15 */
/* 1196 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1200 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1202 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1204 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1210 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pClsid */

/* 1212 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1214 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1216 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Return value */

/* 1218 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1220 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetUserType */

/* 1224 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1226 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1230 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1232 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1238 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1240 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1246 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwFormOfType */

/* 1248 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1250 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1252 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pszUserType */

/* 1254 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1256 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1258 */	NdrFcShort( 0x308 ),	/* Type Offset=776 */

	/* Return value */

/* 1260 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1262 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1264 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetExtent */

/* 1266 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1268 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1272 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1274 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1276 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1278 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1280 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1282 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 1290 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1292 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter psizel */

/* 1296 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1298 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1300 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Return value */

/* 1302 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1304 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExtent */

/* 1308 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1310 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1314 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1316 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1318 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1320 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1322 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1324 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1330 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 1332 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1334 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter psizel */

/* 1338 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1340 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1342 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Return value */

/* 1344 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1346 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Advise */

/* 1350 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1352 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1356 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1358 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1364 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1366 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1372 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pAdvSink */

/* 1374 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1376 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1378 */	NdrFcShort( 0x318 ),	/* Type Offset=792 */

	/* Parameter pdwConnection */

/* 1380 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1382 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1386 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1388 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Unadvise */

/* 1392 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1398 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1400 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1404 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1406 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1408 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1414 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwConnection */

/* 1416 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1418 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1422 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1424 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumAdvise */

/* 1428 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1434 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1436 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1440 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1442 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1444 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1450 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenumAdvise */

/* 1452 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1454 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1456 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */

	/* Return value */

/* 1458 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1460 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1462 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMiscStatus */

/* 1464 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1466 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1470 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1472 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1474 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1476 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1478 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1480 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1486 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAspect */

/* 1488 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1490 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1492 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwStatus */

/* 1494 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1496 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1500 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1502 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetColorScheme */

/* 1506 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1512 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1514 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1518 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1520 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1522 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1526 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1528 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pLogpal */

/* 1530 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1532 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1534 */	NdrFcShort( 0x1b4 ),	/* Type Offset=436 */

	/* Return value */

/* 1536 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1538 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindow */

/* 1542 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1544 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 1548 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1550 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1554 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1556 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1558 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1560 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1564 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phwnd */

/* 1566 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1568 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1570 */	NdrFcShort( 0x336 ),	/* Type Offset=822 */

	/* Return value */

/* 1572 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1574 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1576 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetUpdateOptions */

/* 1578 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1580 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1584 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1586 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1588 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1590 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1592 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1594 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1596 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1598 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1600 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwUpdateOpt */

/* 1602 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1604 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1606 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1608 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1610 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1612 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetUpdateOptions */

/* 1614 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1616 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1620 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1622 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1626 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1628 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1630 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1636 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pdwUpdateOpt */

/* 1638 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1640 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1644 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1646 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSourceMoniker */

/* 1650 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1652 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1656 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1658 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1660 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1662 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1664 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1666 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1672 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */

/* 1674 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1676 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1678 */	NdrFcShort( 0x232 ),	/* Type Offset=562 */

	/* Parameter rclsid */

/* 1680 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1682 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1684 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Return value */

/* 1686 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1688 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSourceMoniker */

/* 1692 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1694 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1698 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1700 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1702 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1704 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1706 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1708 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1710 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1712 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppmk */

/* 1716 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1718 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1720 */	NdrFcShort( 0x22e ),	/* Type Offset=558 */

	/* Return value */

/* 1722 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1724 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSourceDisplayName */

/* 1728 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1730 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1734 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1736 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1740 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1742 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1744 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1750 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszStatusText */

/* 1752 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1754 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1756 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Return value */

/* 1758 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1760 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1762 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSourceDisplayName */

/* 1764 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1766 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1770 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1772 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1776 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1778 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1780 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1786 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppszDisplayName */

/* 1788 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1790 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1792 */	NdrFcShort( 0x308 ),	/* Type Offset=776 */

	/* Return value */

/* 1794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1796 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BindToSource */

/* 1800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1806 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1808 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1810 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1812 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1814 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1816 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1822 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bindflags */

/* 1824 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1826 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbc */

/* 1830 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1832 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1834 */	NdrFcShort( 0x218 ),	/* Type Offset=536 */

	/* Return value */

/* 1836 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1838 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1840 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BindIfRunning */

/* 1842 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1844 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1848 */	NdrFcShort( 0xa ),	/* 10 */
/* 1850 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1854 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1856 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1858 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1862 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1864 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1866 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1868 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1870 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBoundSource */

/* 1872 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1874 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1878 */	NdrFcShort( 0xb ),	/* 11 */
/* 1880 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1884 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1886 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1888 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1894 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppunk */

/* 1896 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1898 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1900 */	NdrFcShort( 0x344 ),	/* Type Offset=836 */

	/* Return value */

/* 1902 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1904 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DiscardUndoState */


	/* Procedure UnbindSource */

/* 1908 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1910 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1914 */	NdrFcShort( 0xc ),	/* 12 */
/* 1916 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1922 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1924 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 1932 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1934 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Update */

/* 1938 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1940 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1944 */	NdrFcShort( 0xd ),	/* 13 */
/* 1946 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1950 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1952 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1954 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1960 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1962 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1964 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1966 */	NdrFcShort( 0x218 ),	/* Type Offset=536 */

	/* Return value */

/* 1968 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1970 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObject */

/* 1974 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1976 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1980 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1982 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1984 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1986 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1988 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1990 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1992 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1996 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszItem */

/* 1998 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2000 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2002 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Parameter dwSpeedNeeded */

/* 2004 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2006 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbc */

/* 2010 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2012 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2014 */	NdrFcShort( 0x218 ),	/* Type Offset=536 */

	/* Parameter riid */

/* 2016 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2018 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2020 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Parameter ppvObject */

/* 2022 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2024 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2026 */	NdrFcShort( 0x348 ),	/* Type Offset=840 */

	/* Return value */

/* 2028 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2030 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjectStorage */

/* 2034 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2036 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2040 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2042 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2044 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2046 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2048 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2050 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2052 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2056 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszItem */

/* 2058 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2060 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2062 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Parameter pbc */

/* 2064 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2066 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2068 */	NdrFcShort( 0x218 ),	/* Type Offset=536 */

	/* Parameter riid */

/* 2070 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2072 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2074 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Parameter ppvStorage */

/* 2076 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2078 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2080 */	NdrFcShort( 0x354 ),	/* Type Offset=852 */

	/* Return value */

/* 2082 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2084 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2086 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsRunning */

/* 2088 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2090 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2094 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2096 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2100 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2102 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2104 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2110 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszItem */

/* 2112 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2114 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2116 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Return value */

/* 2118 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2120 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2122 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBorder */

/* 2124 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2126 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2130 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2132 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2136 */	NdrFcShort( 0x3c ),	/* 60 */
/* 2138 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2140 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2146 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lprectBorder */

/* 2148 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2150 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2152 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Return value */

/* 2154 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2156 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2158 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RequestBorderSpace */

/* 2160 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2162 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2166 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2168 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2170 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2172 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2174 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2176 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2182 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pborderwidths */

/* 2184 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2186 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2188 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Return value */

/* 2190 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2192 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetBorderSpace */

/* 2196 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2198 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2202 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2204 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2206 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2208 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2210 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2212 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2218 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pborderwidths */

/* 2220 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2222 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2224 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Return value */

/* 2226 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2228 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetActiveObject */

/* 2232 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2234 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2238 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2240 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2244 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2246 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2248 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2254 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pActiveObject */

/* 2256 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2258 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2260 */	NdrFcShort( 0x364 ),	/* Type Offset=868 */

	/* Parameter pszObjName */

/* 2262 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2264 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2266 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 2268 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2270 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure DragLeave */


	/* Procedure CanInPlaceActivate */


	/* Procedure InPlaceDeactivate */


	/* Procedure RemoteTranslateAccelerator */

/* 2274 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2276 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2280 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2282 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2286 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2288 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2290 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2296 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2298 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2300 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnFrameWindowActivate */

/* 2304 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2306 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2310 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2312 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2314 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2316 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2318 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2320 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2326 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fActivate */

/* 2328 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2330 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2336 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnDocWindowActivate */

/* 2340 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2342 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2346 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2348 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2350 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2354 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2356 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2362 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fActivate */

/* 2364 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2366 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2368 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2370 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2372 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteResizeBorder */

/* 2376 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2378 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2382 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2384 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2386 */	NdrFcShort( 0x80 ),	/* 128 */
/* 2388 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2390 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2392 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2396 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2398 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter prcBorder */

/* 2400 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2402 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2404 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter riid */

/* 2406 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2408 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2410 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Parameter pUIWindow */

/* 2412 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2414 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2416 */	NdrFcShort( 0x37a ),	/* Type Offset=890 */

	/* Parameter fFrameWindow */

/* 2418 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2420 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2424 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2426 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InsertMenus */

/* 2430 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2432 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2436 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2438 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2440 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2442 */	NdrFcShort( 0x54 ),	/* 84 */
/* 2444 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2446 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2450 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2452 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hmenuShared */

/* 2454 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2456 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2458 */	NdrFcShort( 0x382 ),	/* Type Offset=898 */

	/* Parameter lpMenuWidths */

/* 2460 */	NdrFcShort( 0x11a ),	/* Flags:  must free, in, out, simple ref, */
/* 2462 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2464 */	NdrFcShort( 0x396 ),	/* Type Offset=918 */

	/* Return value */

/* 2466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2468 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMenu */

/* 2472 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2474 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2478 */	NdrFcShort( 0xa ),	/* 10 */
/* 2480 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2486 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 2488 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2492 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2494 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hmenuShared */

/* 2496 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2498 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2500 */	NdrFcShort( 0x382 ),	/* Type Offset=898 */

	/* Parameter holemenu */

/* 2502 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2504 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2506 */	NdrFcShort( 0x3a0 ),	/* Type Offset=928 */

	/* Parameter hwndActiveObject */

/* 2508 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2510 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2512 */	NdrFcShort( 0x2a6 ),	/* Type Offset=678 */

	/* Return value */

/* 2514 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2516 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2518 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveMenus */

/* 2520 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2522 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2526 */	NdrFcShort( 0xb ),	/* 11 */
/* 2528 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2532 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2534 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2536 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2540 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2542 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hmenuShared */

/* 2544 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2546 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2548 */	NdrFcShort( 0x382 ),	/* Type Offset=898 */

	/* Return value */

/* 2550 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2552 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetStatusText */

/* 2556 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2558 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2562 */	NdrFcShort( 0xc ),	/* 12 */
/* 2564 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2568 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2570 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2572 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2578 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszStatusText */

/* 2580 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2582 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2584 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 2586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2588 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnableModeless */

/* 2592 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2598 */	NdrFcShort( 0xd ),	/* 13 */
/* 2600 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2602 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2606 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2608 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2614 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fEnable */

/* 2616 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2618 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2620 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2622 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2624 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2626 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure TranslateAccelerator */

/* 2628 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2630 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2634 */	NdrFcShort( 0xe ),	/* 14 */
/* 2636 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2638 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2640 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2642 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2644 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2648 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2650 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lpmsg */

/* 2652 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2654 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2656 */	NdrFcShort( 0x2b8 ),	/* Type Offset=696 */

	/* Parameter wID */

/* 2658 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2660 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2662 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 2664 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2666 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2668 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetObjectRects */

/* 2670 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2672 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2676 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2678 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2680 */	NdrFcShort( 0x68 ),	/* 104 */
/* 2682 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2684 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2686 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2692 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lprcPosRect */

/* 2694 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2696 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2698 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter lprcClipRect */

/* 2700 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2702 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2704 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Return value */

/* 2706 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2708 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnUIActivate */

/* 2712 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2714 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2718 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2720 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2726 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2728 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2734 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2736 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2738 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindowContext */

/* 2742 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2744 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2748 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2750 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2754 */	NdrFcShort( 0x70 ),	/* 112 */
/* 2756 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2758 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 2760 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2762 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2764 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppFrame */

/* 2766 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2768 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2770 */	NdrFcShort( 0x3ae ),	/* Type Offset=942 */

	/* Parameter ppDoc */

/* 2772 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2774 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2776 */	NdrFcShort( 0x3c4 ),	/* Type Offset=964 */

	/* Parameter lprcPosRect */

/* 2778 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2780 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2782 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter lprcClipRect */

/* 2784 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2786 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2788 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter lpFrameInfo */

/* 2790 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 2792 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2794 */	NdrFcShort( 0x3e8 ),	/* Type Offset=1000 */

	/* Return value */

/* 2796 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2798 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2800 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Scroll */

/* 2802 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2804 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2808 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2810 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2812 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2814 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2816 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2818 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2824 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scrollExtant */

/* 2826 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2828 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2830 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Return value */

/* 2832 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2834 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2836 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnUIDeactivate */

/* 2838 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2840 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2844 */	NdrFcShort( 0xa ),	/* 10 */
/* 2846 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2848 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2850 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2852 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2854 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2856 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2858 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2860 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fUndoable */

/* 2862 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2864 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2868 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2870 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2872 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnInPlaceDeactivate */

/* 2874 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2876 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2880 */	NdrFcShort( 0xb ),	/* 11 */
/* 2882 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2886 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2888 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2890 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2894 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2896 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2898 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2900 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2902 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnPosRectChange */

/* 2904 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2906 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2910 */	NdrFcShort( 0xe ),	/* 14 */
/* 2912 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2914 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2916 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2918 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2920 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2926 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lprcPosRect */

/* 2928 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2930 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2932 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Return value */

/* 2934 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2936 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteDraw */

/* 2940 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2942 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2946 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2948 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 2950 */	NdrFcShort( 0x90 ),	/* 144 */
/* 2952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2954 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 2956 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2960 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2962 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 2964 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2966 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2968 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 2970 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2972 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvAspect */

/* 2976 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2978 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ptd */

/* 2982 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2984 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2986 */	NdrFcShort( 0x3fc ),	/* Type Offset=1020 */

	/* Parameter hdcTargetDev */

/* 2988 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2990 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hdcDraw */

/* 2994 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2996 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lprcBounds */

/* 3000 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3002 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3004 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter lprcWBounds */

/* 3006 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3008 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3010 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Parameter pContinue */

/* 3012 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3014 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 3016 */	NdrFcShort( 0x400 ),	/* Type Offset=1024 */

	/* Return value */

/* 3018 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3020 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 3022 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetColorSet */

/* 3024 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3026 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3030 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3032 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3034 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3036 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3038 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 3040 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 3042 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3044 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3046 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 3048 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3050 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3052 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 3054 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3056 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3058 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvAspect */

/* 3060 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3062 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3064 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ptd */

/* 3066 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3068 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3070 */	NdrFcShort( 0x3fc ),	/* Type Offset=1020 */

	/* Parameter hicTargetDev */

/* 3072 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3074 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3076 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppColorSet */

/* 3078 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 3080 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3082 */	NdrFcShort( 0x412 ),	/* Type Offset=1042 */

	/* Return value */

/* 3084 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3086 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3088 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteFreeze */

/* 3090 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3092 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3096 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3098 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3100 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3102 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3104 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 3106 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3112 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 3114 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3116 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 3120 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3122 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvAspect */

/* 3126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3128 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwFreeze */

/* 3132 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3134 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3140 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAdvise */

/* 3144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3150 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3152 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3154 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3158 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 3160 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter aspects */

/* 3168 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter advf */

/* 3174 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAdvSink */

/* 3180 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3184 */	NdrFcShort( 0x318 ),	/* Type Offset=792 */

	/* Return value */

/* 3186 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3188 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetAdvise */

/* 3192 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3194 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3200 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3204 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3206 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 3208 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3214 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pAspects */

/* 3216 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3218 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAdvf */

/* 3222 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3224 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppAdvSink */

/* 3228 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3232 */	NdrFcShort( 0x41a ),	/* Type Offset=1050 */

	/* Return value */

/* 3234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3236 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExtent */

/* 3240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3246 */	NdrFcShort( 0x9 ),	/* 9 */
/* 3248 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3250 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3252 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3254 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3256 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3260 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3262 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 3264 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3266 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 3270 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3272 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ptd */

/* 3276 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3278 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3280 */	NdrFcShort( 0x3fc ),	/* Type Offset=1020 */

	/* Parameter lpsizel */

/* 3282 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3284 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3286 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Return value */

/* 3288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3290 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DragEnter */

/* 3294 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3300 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3302 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3304 */	NdrFcShort( 0x3c ),	/* 60 */
/* 3306 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3308 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3310 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3316 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObj */

/* 3318 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3320 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3322 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Parameter grfKeyState */

/* 3324 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3326 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pt */

/* 3330 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3332 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3334 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Parameter pdwEffect */

/* 3336 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3338 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3342 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3344 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DragOver */

/* 3348 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3354 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3356 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3358 */	NdrFcShort( 0x3c ),	/* 60 */
/* 3360 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3362 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3364 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3370 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfKeyState */

/* 3372 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3374 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pt */

/* 3378 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3380 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3382 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Parameter pdwEffect */

/* 3384 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3386 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3390 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3392 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Drop */

/* 3396 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3402 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3404 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3406 */	NdrFcShort( 0x3c ),	/* 60 */
/* 3408 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3410 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3412 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3418 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObj */

/* 3420 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3422 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3424 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Parameter grfKeyState */

/* 3426 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3428 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pt */

/* 3432 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3434 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3436 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Parameter pdwEffect */

/* 3438 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3440 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3446 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 3450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3456 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3458 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3462 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3464 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 3466 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3468 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 3474 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 3480 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3484 */	NdrFcShort( 0x43c ),	/* Type Offset=1084 */

	/* Parameter pceltFetched */

/* 3486 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3488 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3494 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 3498 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3504 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3506 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3510 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3512 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3514 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3518 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3520 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 3522 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3524 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3526 */	NdrFcShort( 0x2dc ),	/* Type Offset=732 */

	/* Return value */

/* 3528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3530 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3532 */	0x8,		/* FC_LONG */
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
			0x12, 0x0,	/* FC_UP */
/*  4 */	NdrFcShort( 0x40 ),	/* Offset= 64 (68) */
/*  6 */	
			0x12, 0x0,	/* FC_UP */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 12 */	NdrFcShort( 0x4 ),	/* 4 */
/* 14 */	NdrFcShort( 0x2 ),	/* 2 */
/* 16 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 20 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 22 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 26 */	NdrFcShort( 0x4 ),	/* Offset= 4 (30) */
/* 28 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (27) */
/* 30 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 32 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 34 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	NdrFcShort( 0x2 ),	/* 2 */
/* 40 */	NdrFcShort( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (6) */
/* 44 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 46 */	NdrFcShort( 0x1 ),	/* 1 */
/* 48 */	0x0,		/* Corr desc:  */
			0x59,		/* FC_CALLBACK */
/* 50 */	NdrFcShort( 0x0 ),	/* 0 */
/* 52 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 54 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 56 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 58 */	NdrFcShort( 0xc ),	/* 12 */
/* 60 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (44) */
/* 62 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 64 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 66 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 68 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 70 */	NdrFcShort( 0x14 ),	/* 20 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0xc ),	/* Offset= 12 (86) */
/* 76 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 78 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (34) */
/* 80 */	0x3e,		/* FC_STRUCTPAD2 */
			0x36,		/* FC_POINTER */
/* 82 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 84 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 86 */	
			0x12, 0x0,	/* FC_UP */
/* 88 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (56) */
/* 90 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 92 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 94 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 96 */	NdrFcShort( 0x2 ),	/* Offset= 2 (98) */
/* 98 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 100 */	NdrFcLong( 0x105 ),	/* 261 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 108 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 110 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 112 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 114 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 116 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 118 */	NdrFcLong( 0x10e ),	/* 270 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 128 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 130 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 132 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 134 */	
			0x12, 0x0,	/* FC_UP */
/* 136 */	NdrFcShort( 0x186 ),	/* Offset= 390 (526) */
/* 138 */	
			0x12, 0x0,	/* FC_UP */
/* 140 */	NdrFcShort( 0x170 ),	/* Offset= 368 (508) */
/* 142 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 144 */	NdrFcShort( 0x4 ),	/* 4 */
/* 146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 148 */	NdrFcLong( 0x0 ),	/* 0 */
/* 152 */	NdrFcShort( 0x0 ),	/* Offset= 0 (152) */
/* 154 */	NdrFcLong( 0x20 ),	/* 32 */
/* 158 */	NdrFcShort( 0x28 ),	/* Offset= 40 (198) */
/* 160 */	NdrFcLong( 0x40 ),	/* 64 */
/* 164 */	NdrFcShort( 0x80 ),	/* Offset= 128 (292) */
/* 166 */	NdrFcLong( 0x10 ),	/* 16 */
/* 170 */	NdrFcShort( 0x92 ),	/* Offset= 146 (316) */
/* 172 */	NdrFcLong( 0x1 ),	/* 1 */
/* 176 */	NdrFcShort( 0x10e ),	/* Offset= 270 (446) */
/* 178 */	NdrFcLong( 0x2 ),	/* 2 */
/* 182 */	NdrFcShort( 0xffffff68 ),	/* Offset= -152 (30) */
/* 184 */	NdrFcLong( 0x4 ),	/* 4 */
/* 188 */	NdrFcShort( 0x3a ),	/* Offset= 58 (246) */
/* 190 */	NdrFcLong( 0x8 ),	/* 8 */
/* 194 */	NdrFcShort( 0x34 ),	/* Offset= 52 (246) */
/* 196 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (195) */
/* 198 */	
			0x12, 0x0,	/* FC_UP */
/* 200 */	NdrFcShort( 0x2 ),	/* Offset= 2 (202) */
/* 202 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 204 */	NdrFcShort( 0x4 ),	/* 4 */
/* 206 */	NdrFcShort( 0x2 ),	/* 2 */
/* 208 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 212 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 214 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 218 */	NdrFcShort( 0x4 ),	/* Offset= 4 (222) */
/* 220 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (219) */
/* 222 */	
			0x12, 0x0,	/* FC_UP */
/* 224 */	NdrFcShort( 0x2e ),	/* Offset= 46 (270) */
/* 226 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 228 */	NdrFcShort( 0x4 ),	/* 4 */
/* 230 */	NdrFcShort( 0x2 ),	/* 2 */
/* 232 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 236 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 238 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 242 */	NdrFcShort( 0x4 ),	/* Offset= 4 (246) */
/* 244 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (243) */
/* 246 */	
			0x12, 0x0,	/* FC_UP */
/* 248 */	NdrFcShort( 0xe ),	/* Offset= 14 (262) */
/* 250 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 252 */	NdrFcShort( 0x1 ),	/* 1 */
/* 254 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 256 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 258 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 260 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 262 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 264 */	NdrFcShort( 0x4 ),	/* 4 */
/* 266 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (250) */
/* 268 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 270 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 272 */	NdrFcShort( 0x10 ),	/* 16 */
/* 274 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 276 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 278 */	NdrFcShort( 0xc ),	/* 12 */
/* 280 */	NdrFcShort( 0xc ),	/* 12 */
/* 282 */	0x12, 0x0,	/* FC_UP */
/* 284 */	NdrFcShort( 0xffffffc6 ),	/* Offset= -58 (226) */
/* 286 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 288 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 290 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 292 */	
			0x12, 0x0,	/* FC_UP */
/* 294 */	NdrFcShort( 0x2 ),	/* Offset= 2 (296) */
/* 296 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 298 */	NdrFcShort( 0x4 ),	/* 4 */
/* 300 */	NdrFcShort( 0x2 ),	/* 2 */
/* 302 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 306 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 308 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 312 */	NdrFcShort( 0xffffffbe ),	/* Offset= -66 (246) */
/* 314 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (313) */
/* 316 */	
			0x12, 0x0,	/* FC_UP */
/* 318 */	NdrFcShort( 0x2 ),	/* Offset= 2 (320) */
/* 320 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 322 */	NdrFcShort( 0x4 ),	/* 4 */
/* 324 */	NdrFcShort( 0x2 ),	/* 2 */
/* 326 */	NdrFcLong( 0x7 ),	/* 7 */
/* 330 */	NdrFcShort( 0xa ),	/* Offset= 10 (340) */
/* 332 */	NdrFcLong( 0x5 ),	/* 5 */
/* 336 */	NdrFcShort( 0x2e ),	/* Offset= 46 (382) */
/* 338 */	NdrFcShort( 0x6c ),	/* Offset= 108 (446) */
/* 340 */	
			0x12, 0x0,	/* FC_UP */
/* 342 */	NdrFcShort( 0x2 ),	/* Offset= 2 (344) */
/* 344 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 346 */	NdrFcShort( 0x4 ),	/* 4 */
/* 348 */	NdrFcShort( 0x2 ),	/* 2 */
/* 350 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 354 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 356 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 360 */	NdrFcShort( 0x4 ),	/* Offset= 4 (364) */
/* 362 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (361) */
/* 364 */	
			0x12, 0x0,	/* FC_UP */
/* 366 */	NdrFcShort( 0x2 ),	/* Offset= 2 (368) */
/* 368 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 370 */	NdrFcShort( 0x18 ),	/* 24 */
/* 372 */	NdrFcShort( 0xffffff86 ),	/* Offset= -122 (250) */
/* 374 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 376 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 378 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 380 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 382 */	
			0x12, 0x0,	/* FC_UP */
/* 384 */	NdrFcShort( 0x2 ),	/* Offset= 2 (386) */
/* 386 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 388 */	NdrFcShort( 0x4 ),	/* 4 */
/* 390 */	NdrFcShort( 0x2 ),	/* 2 */
/* 392 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 396 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 398 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 402 */	NdrFcShort( 0x4 ),	/* Offset= 4 (406) */
/* 404 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (403) */
/* 406 */	
			0x12, 0x0,	/* FC_UP */
/* 408 */	NdrFcShort( 0x1c ),	/* Offset= 28 (436) */
/* 410 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 412 */	NdrFcShort( 0x4 ),	/* 4 */
/* 414 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 416 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 418 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 420 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 422 */	NdrFcShort( 0x4 ),	/* 4 */
/* 424 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 426 */	NdrFcShort( 0xfffe ),	/* -2 */
/* 428 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 430 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 432 */	NdrFcShort( 0xffffffea ),	/* Offset= -22 (410) */
/* 434 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 436 */	
			0x17,		/* FC_CSTRUCT */
			0x1,		/* 1 */
/* 438 */	NdrFcShort( 0x4 ),	/* 4 */
/* 440 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (420) */
/* 442 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 444 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 446 */	
			0x12, 0x0,	/* FC_UP */
/* 448 */	NdrFcShort( 0x2 ),	/* Offset= 2 (450) */
/* 450 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 454 */	NdrFcShort( 0x3 ),	/* 3 */
/* 456 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 460 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 462 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 466 */	NdrFcShort( 0xa ),	/* Offset= 10 (476) */
/* 468 */	NdrFcLong( 0x50746457 ),	/* 1349805143 */
/* 472 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 474 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (473) */
/* 476 */	
			0x12, 0x0,	/* FC_UP */
/* 478 */	NdrFcShort( 0x2 ),	/* Offset= 2 (480) */
/* 480 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 482 */	NdrFcShort( 0x8 ),	/* 8 */
/* 484 */	NdrFcShort( 0xffffff16 ),	/* Offset= -234 (250) */
/* 486 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 488 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 490 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 492 */	NdrFcLong( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 500 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 502 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 504 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 506 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 508 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 510 */	NdrFcShort( 0xc ),	/* 12 */
/* 512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 514 */	NdrFcShort( 0xc ),	/* Offset= 12 (526) */
/* 516 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 518 */	NdrFcShort( 0xfffffe88 ),	/* Offset= -376 (142) */
/* 520 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 522 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (490) */
/* 524 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 526 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 528 */	NdrFcShort( 0x1 ),	/* 1 */
/* 530 */	NdrFcShort( 0xc ),	/* 12 */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0xfffffe74 ),	/* Offset= -396 (138) */
/* 536 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 538 */	NdrFcLong( 0xe ),	/* 14 */
/* 542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 548 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 550 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 552 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 554 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 556 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 558 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 560 */	NdrFcShort( 0x2 ),	/* Offset= 2 (562) */
/* 562 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 564 */	NdrFcLong( 0xf ),	/* 15 */
/* 568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 572 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 574 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 576 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 578 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 580 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 582 */	NdrFcShort( 0x2 ),	/* Offset= 2 (584) */
/* 584 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 586 */	NdrFcLong( 0x100 ),	/* 256 */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 594 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 596 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 598 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 600 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 602 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 604 */	NdrFcShort( 0x2 ),	/* Offset= 2 (606) */
/* 606 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 608 */	NdrFcLong( 0x11b ),	/* 283 */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 616 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 618 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 620 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 622 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 624 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 626 */	NdrFcLong( 0x118 ),	/* 280 */
/* 630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 634 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 636 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 638 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 640 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 642 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 644 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (624) */
/* 646 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 648 */	NdrFcShort( 0xfffffdec ),	/* Offset= -532 (116) */
/* 650 */	
			0x12, 0x0,	/* FC_UP */
/* 652 */	NdrFcShort( 0x2c ),	/* Offset= 44 (696) */
/* 654 */	
			0x12, 0x0,	/* FC_UP */
/* 656 */	NdrFcShort( 0x2 ),	/* Offset= 2 (658) */
/* 658 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 660 */	NdrFcShort( 0x4 ),	/* 4 */
/* 662 */	NdrFcShort( 0x2 ),	/* 2 */
/* 664 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 668 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 670 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 674 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 676 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (675) */
/* 678 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 680 */	NdrFcShort( 0x2 ),	/* 2 */
/* 682 */	NdrFcShort( 0x4 ),	/* 4 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (654) */
/* 688 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 692 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 694 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 696 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 698 */	NdrFcShort( 0x1c ),	/* 28 */
/* 700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 702 */	NdrFcShort( 0x0 ),	/* Offset= 0 (702) */
/* 704 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 706 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (678) */
/* 708 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 710 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 712 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 714 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (688) */
/* 716 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 718 */	
			0x12, 0x0,	/* FC_UP */
/* 720 */	NdrFcShort( 0x2 ),	/* Offset= 2 (722) */
/* 722 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 724 */	NdrFcShort( 0x10 ),	/* 16 */
/* 726 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 728 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 730 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 732 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 734 */	NdrFcShort( 0x2 ),	/* Offset= 2 (736) */
/* 736 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 738 */	NdrFcLong( 0x104 ),	/* 260 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 746 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 748 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 750 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 752 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 754 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 756 */	NdrFcShort( 0x8 ),	/* Offset= 8 (764) */
/* 758 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 762 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 764 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 766 */	NdrFcShort( 0x10 ),	/* 16 */
/* 768 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 770 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 772 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (758) */
			0x5b,		/* FC_END */
/* 776 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 778 */	NdrFcShort( 0x2 ),	/* Offset= 2 (780) */
/* 780 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 782 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 784 */	
			0x11, 0x0,	/* FC_RP */
/* 786 */	NdrFcShort( 0xffffff9e ),	/* Offset= -98 (688) */
/* 788 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 790 */	NdrFcShort( 0xffffff9a ),	/* Offset= -102 (688) */
/* 792 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 794 */	NdrFcLong( 0x10f ),	/* 271 */
/* 798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 802 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 804 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 806 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 808 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 810 */	
			0x11, 0x0,	/* FC_RP */
/* 812 */	NdrFcShort( 0xfffffe88 ),	/* Offset= -376 (436) */
/* 814 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 816 */	NdrFcShort( 0x6 ),	/* Offset= 6 (822) */
/* 818 */	
			0x13, 0x0,	/* FC_OP */
/* 820 */	NdrFcShort( 0xffffff5e ),	/* Offset= -162 (658) */
/* 822 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 824 */	NdrFcShort( 0x2 ),	/* 2 */
/* 826 */	NdrFcShort( 0x4 ),	/* 4 */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (818) */
/* 832 */	
			0x11, 0x0,	/* FC_RP */
/* 834 */	NdrFcShort( 0xffffffba ),	/* Offset= -70 (764) */
/* 836 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 838 */	NdrFcShort( 0xfffffea4 ),	/* Offset= -348 (490) */
/* 840 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 842 */	NdrFcShort( 0x2 ),	/* Offset= 2 (844) */
/* 844 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 846 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 848 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 850 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 852 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 854 */	NdrFcShort( 0x2 ),	/* Offset= 2 (856) */
/* 856 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 858 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 860 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 862 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 864 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 866 */	NdrFcShort( 0xffffff70 ),	/* Offset= -144 (722) */
/* 868 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 870 */	NdrFcLong( 0x117 ),	/* 279 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 880 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 882 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 884 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 886 */	
			0x11, 0x0,	/* FC_RP */
/* 888 */	NdrFcShort( 0xffffff5a ),	/* Offset= -166 (722) */
/* 890 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 892 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 894 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 896 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 898 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 900 */	NdrFcShort( 0x3 ),	/* 3 */
/* 902 */	NdrFcShort( 0x4 ),	/* 4 */
/* 904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0xffffff04 ),	/* Offset= -252 (654) */
/* 908 */	
			0x11, 0x0,	/* FC_RP */
/* 910 */	NdrFcShort( 0x8 ),	/* Offset= 8 (918) */
/* 912 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 914 */	NdrFcShort( 0x18 ),	/* 24 */
/* 916 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 918 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 920 */	NdrFcShort( 0x18 ),	/* 24 */
/* 922 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 924 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (912) */
/* 926 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 928 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 930 */	NdrFcShort( 0x4 ),	/* 4 */
/* 932 */	NdrFcShort( 0x4 ),	/* 4 */
/* 934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 936 */	NdrFcShort( 0xfffffe16 ),	/* Offset= -490 (446) */
/* 938 */	
			0x11, 0x0,	/* FC_RP */
/* 940 */	NdrFcShort( 0xffffff0c ),	/* Offset= -244 (696) */
/* 942 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 944 */	NdrFcShort( 0x2 ),	/* Offset= 2 (946) */
/* 946 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 948 */	NdrFcLong( 0x116 ),	/* 278 */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 958 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 960 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 962 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 964 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 966 */	NdrFcShort( 0x2 ),	/* Offset= 2 (968) */
/* 968 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 970 */	NdrFcLong( 0x115 ),	/* 277 */
/* 974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 976 */	NdrFcShort( 0x0 ),	/* 0 */
/* 978 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 980 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 982 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 984 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 986 */	
			0x11, 0x0,	/* FC_RP */
/* 988 */	NdrFcShort( 0xc ),	/* Offset= 12 (1000) */
/* 990 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 992 */	NdrFcShort( 0x5 ),	/* 5 */
/* 994 */	NdrFcShort( 0x4 ),	/* 4 */
/* 996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 998 */	NdrFcShort( 0xffffff4c ),	/* Offset= -180 (818) */
/* 1000 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1002 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1006) */
/* 1008 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1010 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1012 */	NdrFcShort( 0xffffff42 ),	/* Offset= -190 (822) */
/* 1014 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1016 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (990) */
/* 1018 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1020 */	
			0x12, 0x0,	/* FC_UP */
/* 1022 */	NdrFcShort( 0xfffffc3a ),	/* Offset= -966 (56) */
/* 1024 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1026 */	NdrFcLong( 0x12a ),	/* 298 */
/* 1030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1034 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1036 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1038 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1040 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1042 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1044 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1046) */
/* 1046 */	
			0x13, 0x0,	/* FC_OP */
/* 1048 */	NdrFcShort( 0xfffffd9c ),	/* Offset= -612 (436) */
/* 1050 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1052 */	NdrFcShort( 0xfffffefc ),	/* Offset= -260 (792) */
/* 1054 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1056 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1058 */	
			0x11, 0x0,	/* FC_RP */
/* 1060 */	NdrFcShort( 0x18 ),	/* Offset= 24 (1084) */
/* 1062 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1064 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1066 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1068 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1070 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1072 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1074 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1076 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1078 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1080 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1082 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1084 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1086 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1088 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1090 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1092 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1094 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1096 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1098 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1100 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1102 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 1104 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1108 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1110 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1112 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1114 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1116 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1118 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1120 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc5 ),	/* Offset= -59 (1062) */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            CLIPFORMAT_UserSize
            ,CLIPFORMAT_UserMarshal
            ,CLIPFORMAT_UserUnmarshal
            ,CLIPFORMAT_UserFree
            },
            {
            STGMEDIUM_UserSize
            ,STGMEDIUM_UserMarshal
            ,STGMEDIUM_UserUnmarshal
            ,STGMEDIUM_UserFree
            },
            {
            HWND_UserSize
            ,HWND_UserMarshal
            ,HWND_UserUnmarshal
            ,HWND_UserFree
            },
            {
            HMENU_UserSize
            ,HMENU_UserMarshal
            ,HMENU_UserUnmarshal
            ,HMENU_UserFree
            },
            {
            HGLOBAL_UserSize
            ,HGLOBAL_UserMarshal
            ,HGLOBAL_UserUnmarshal
            ,HGLOBAL_UserFree
            },
            {
            HACCEL_UserSize
            ,HACCEL_UserMarshal
            ,HACCEL_UserUnmarshal
            ,HACCEL_UserFree
            }

        };


static void __RPC_USER IOleCache_DVTARGETDEVICEExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    DVTARGETDEVICE *pS	=	( DVTARGETDEVICE * )(pStubMsg->StackTop - 12);
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( pS->tdSize - sizeof( DWORD  ) - 4 * sizeof( WORD  ) );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IOleCache_DVTARGETDEVICEExprEval_0000
    };



/* Standard interface: __MIDL_itf_oleidl_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOleAdviseHolder, ver. 0.0,
   GUID={0x00000111,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOleCache, ver. 0.0,
   GUID={0x0000011e,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleCache_FormatStringOffsetTable[] =
    {
    0,
    48,
    84,
    120,
    156
    };

static const MIDL_STUBLESS_PROXY_INFO IOleCache_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleCache_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleCache_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleCache_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IOleCacheProxyVtbl = 
{
    &IOleCache_ProxyInfo,
    &IID_IOleCache,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleCache::Cache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::Uncache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::EnumCache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::InitCache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::SetData */
};

const CInterfaceStubVtbl _IOleCacheStubVtbl =
{
    &IID_IOleCache,
    &IOleCache_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleCache2, ver. 0.0,
   GUID={0x00000128,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleCache2_FormatStringOffsetTable[] =
    {
    0,
    48,
    84,
    120,
    156,
    204,
    252
    };

static const MIDL_STUBLESS_PROXY_INFO IOleCache2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleCache2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IOleCache2_StubThunkTable[] = 
    {
    0,
    0,
    0,
    0,
    0,
    IOleCache2_RemoteUpdateCache_Thunk,
    0
    };

static const MIDL_SERVER_INFO IOleCache2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleCache2_FormatStringOffsetTable[-3],
    &IOleCache2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IOleCache2ProxyVtbl = 
{
    &IOleCache2_ProxyInfo,
    &IID_IOleCache2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleCache::Cache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::Uncache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::EnumCache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::InitCache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::SetData */ ,
    IOleCache2_UpdateCache_Proxy ,
    (void *) (INT_PTR) -1 /* IOleCache2::DiscardCache */
};

const CInterfaceStubVtbl _IOleCache2StubVtbl =
{
    &IID_IOleCache2,
    &IOleCache2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleCacheControl, ver. 0.0,
   GUID={0x00000129,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleCacheControl_FormatStringOffsetTable[] =
    {
    288,
    324
    };

static const MIDL_STUBLESS_PROXY_INFO IOleCacheControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleCacheControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleCacheControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleCacheControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IOleCacheControlProxyVtbl = 
{
    &IOleCacheControl_ProxyInfo,
    &IID_IOleCacheControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleCacheControl::OnRun */ ,
    (void *) (INT_PTR) -1 /* IOleCacheControl::OnStop */
};

const CInterfaceStubVtbl _IOleCacheControlStubVtbl =
{
    &IID_IOleCacheControl,
    &IOleCacheControl_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IParseDisplayName, ver. 0.0,
   GUID={0x0000011a,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IParseDisplayName_FormatStringOffsetTable[] =
    {
    354
    };

static const MIDL_STUBLESS_PROXY_INFO IParseDisplayName_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IParseDisplayName_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IParseDisplayName_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IParseDisplayName_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IParseDisplayNameProxyVtbl = 
{
    &IParseDisplayName_ProxyInfo,
    &IID_IParseDisplayName,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IParseDisplayName::ParseDisplayName */
};

const CInterfaceStubVtbl _IParseDisplayNameStubVtbl =
{
    &IID_IParseDisplayName,
    &IParseDisplayName_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleContainer, ver. 0.0,
   GUID={0x0000011b,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleContainer_FormatStringOffsetTable[] =
    {
    354,
    408,
    450
    };

static const MIDL_STUBLESS_PROXY_INFO IOleContainer_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleContainer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IOleContainerProxyVtbl = 
{
    &IOleContainer_ProxyInfo,
    &IID_IOleContainer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IParseDisplayName::ParseDisplayName */ ,
    (void *) (INT_PTR) -1 /* IOleContainer::EnumObjects */ ,
    (void *) (INT_PTR) -1 /* IOleContainer::LockContainer */
};

const CInterfaceStubVtbl _IOleContainerStubVtbl =
{
    &IID_IOleContainer,
    &IOleContainer_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleClientSite, ver. 0.0,
   GUID={0x00000118,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleClientSite_FormatStringOffsetTable[] =
    {
    486,
    516,
    564,
    600,
    630,
    666
    };

static const MIDL_STUBLESS_PROXY_INFO IOleClientSite_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleClientSite_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleClientSite_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleClientSite_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IOleClientSiteProxyVtbl = 
{
    &IOleClientSite_ProxyInfo,
    &IID_IOleClientSite,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleClientSite::SaveObject */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::GetMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::ShowObject */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::OnShowWindow */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::RequestNewObjectLayout */
};

const CInterfaceStubVtbl _IOleClientSiteStubVtbl =
{
    &IID_IOleClientSite,
    &IOleClientSite_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleObject, ver. 0.0,
   GUID={0x00000112,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleObject_FormatStringOffsetTable[] =
    {
    696,
    732,
    768,
    810,
    846,
    888,
    936,
    984,
    1026,
    1092,
    1128,
    1158,
    1188,
    1224,
    1266,
    1308,
    1350,
    1392,
    1428,
    1464,
    1506
    };

static const MIDL_STUBLESS_PROXY_INFO IOleObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleObject_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(24) _IOleObjectProxyVtbl = 
{
    &IOleObject_ProxyInfo,
    &IID_IOleObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleObject::SetClientSite */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetClientSite */ ,
    (void *) (INT_PTR) -1 /* IOleObject::SetHostNames */ ,
    (void *) (INT_PTR) -1 /* IOleObject::Close */ ,
    (void *) (INT_PTR) -1 /* IOleObject::SetMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleObject::InitFromData */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetClipboardData */ ,
    (void *) (INT_PTR) -1 /* IOleObject::DoVerb */ ,
    (void *) (INT_PTR) -1 /* IOleObject::EnumVerbs */ ,
    (void *) (INT_PTR) -1 /* IOleObject::Update */ ,
    (void *) (INT_PTR) -1 /* IOleObject::IsUpToDate */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetUserClassID */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetUserType */ ,
    (void *) (INT_PTR) -1 /* IOleObject::SetExtent */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetExtent */ ,
    (void *) (INT_PTR) -1 /* IOleObject::Advise */ ,
    (void *) (INT_PTR) -1 /* IOleObject::Unadvise */ ,
    (void *) (INT_PTR) -1 /* IOleObject::EnumAdvise */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetMiscStatus */ ,
    (void *) (INT_PTR) -1 /* IOleObject::SetColorScheme */
};

const CInterfaceStubVtbl _IOleObjectStubVtbl =
{
    &IID_IOleObject,
    &IOleObject_ServerInfo,
    24,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: IOLETypes, ver. 0.0,
   GUID={0xB0916C84,0x7416,0x101A,{0xBC,0xEA,0x08,0x00,0x2B,0x2B,0x79,0xEF}} */


/* Object interface: IOleWindow, ver. 0.0,
   GUID={0x00000114,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleWindow_FormatStringOffsetTable[] =
    {
    1542,
    48
    };

static const MIDL_STUBLESS_PROXY_INFO IOleWindow_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleWindow_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IOleWindowProxyVtbl = 
{
    &IOleWindow_ProxyInfo,
    &IID_IOleWindow,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */
};

const CInterfaceStubVtbl _IOleWindowStubVtbl =
{
    &IID_IOleWindow,
    &IOleWindow_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleLink, ver. 0.0,
   GUID={0x0000011d,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleLink_FormatStringOffsetTable[] =
    {
    1578,
    1614,
    1650,
    1692,
    1728,
    1764,
    1800,
    1842,
    1872,
    1908,
    1938
    };

static const MIDL_STUBLESS_PROXY_INFO IOleLink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleLink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleLink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleLink_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IOleLinkProxyVtbl = 
{
    &IOleLink_ProxyInfo,
    &IID_IOleLink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleLink::SetUpdateOptions */ ,
    (void *) (INT_PTR) -1 /* IOleLink::GetUpdateOptions */ ,
    (void *) (INT_PTR) -1 /* IOleLink::SetSourceMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleLink::GetSourceMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleLink::SetSourceDisplayName */ ,
    (void *) (INT_PTR) -1 /* IOleLink::GetSourceDisplayName */ ,
    (void *) (INT_PTR) -1 /* IOleLink::BindToSource */ ,
    (void *) (INT_PTR) -1 /* IOleLink::BindIfRunning */ ,
    (void *) (INT_PTR) -1 /* IOleLink::GetBoundSource */ ,
    (void *) (INT_PTR) -1 /* IOleLink::UnbindSource */ ,
    (void *) (INT_PTR) -1 /* IOleLink::Update */
};

const CInterfaceStubVtbl _IOleLinkStubVtbl =
{
    &IID_IOleLink,
    &IOleLink_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleItemContainer, ver. 0.0,
   GUID={0x0000011c,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleItemContainer_FormatStringOffsetTable[] =
    {
    354,
    408,
    450,
    1974,
    2034,
    2088
    };

static const MIDL_STUBLESS_PROXY_INFO IOleItemContainer_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleItemContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleItemContainer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleItemContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IOleItemContainerProxyVtbl = 
{
    &IOleItemContainer_ProxyInfo,
    &IID_IOleItemContainer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IParseDisplayName::ParseDisplayName */ ,
    (void *) (INT_PTR) -1 /* IOleContainer::EnumObjects */ ,
    (void *) (INT_PTR) -1 /* IOleContainer::LockContainer */ ,
    (void *) (INT_PTR) -1 /* IOleItemContainer::GetObject */ ,
    (void *) (INT_PTR) -1 /* IOleItemContainer::GetObjectStorage */ ,
    (void *) (INT_PTR) -1 /* IOleItemContainer::IsRunning */
};

const CInterfaceStubVtbl _IOleItemContainerStubVtbl =
{
    &IID_IOleItemContainer,
    &IOleItemContainer_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceUIWindow, ver. 0.0,
   GUID={0x00000115,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceUIWindow_FormatStringOffsetTable[] =
    {
    1542,
    48,
    2124,
    2160,
    2196,
    2232
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceUIWindow_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceUIWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleInPlaceUIWindow_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceUIWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IOleInPlaceUIWindowProxyVtbl = 
{
    &IOleInPlaceUIWindow_ProxyInfo,
    &IID_IOleInPlaceUIWindow,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::GetBorder */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::RequestBorderSpace */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::SetBorderSpace */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::SetActiveObject */
};

const CInterfaceStubVtbl _IOleInPlaceUIWindowStubVtbl =
{
    &IID_IOleInPlaceUIWindow,
    &IOleInPlaceUIWindow_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceActiveObject, ver. 0.0,
   GUID={0x00000117,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceActiveObject_FormatStringOffsetTable[] =
    {
    1542,
    48,
    2274,
    2304,
    2340,
    2376,
    252
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceActiveObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceActiveObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IOleInPlaceActiveObject_StubThunkTable[] = 
    {
    0,
    0,
    IOleInPlaceActiveObject_RemoteTranslateAccelerator_Thunk,
    0,
    0,
    IOleInPlaceActiveObject_RemoteResizeBorder_Thunk,
    0
    };

static const MIDL_SERVER_INFO IOleInPlaceActiveObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceActiveObject_FormatStringOffsetTable[-3],
    &IOleInPlaceActiveObject_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IOleInPlaceActiveObjectProxyVtbl = 
{
    &IOleInPlaceActiveObject_ProxyInfo,
    &IID_IOleInPlaceActiveObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    IOleInPlaceActiveObject_TranslateAccelerator_Proxy ,
    (void *) (INT_PTR) -1 /* IOleInPlaceActiveObject::OnFrameWindowActivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceActiveObject::OnDocWindowActivate */ ,
    IOleInPlaceActiveObject_ResizeBorder_Proxy ,
    (void *) (INT_PTR) -1 /* IOleInPlaceActiveObject::EnableModeless */
};

const CInterfaceStubVtbl _IOleInPlaceActiveObjectStubVtbl =
{
    &IID_IOleInPlaceActiveObject,
    &IOleInPlaceActiveObject_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceFrame, ver. 0.0,
   GUID={0x00000116,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceFrame_FormatStringOffsetTable[] =
    {
    1542,
    48,
    2124,
    2160,
    2196,
    2232,
    2430,
    2472,
    2520,
    2556,
    2592,
    2628
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceFrame_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceFrame_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleInPlaceFrame_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceFrame_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _IOleInPlaceFrameProxyVtbl = 
{
    &IOleInPlaceFrame_ProxyInfo,
    &IID_IOleInPlaceFrame,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::GetBorder */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::RequestBorderSpace */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::SetBorderSpace */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::SetActiveObject */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::InsertMenus */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::SetMenu */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::RemoveMenus */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::SetStatusText */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::EnableModeless */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::TranslateAccelerator */
};

const CInterfaceStubVtbl _IOleInPlaceFrameStubVtbl =
{
    &IID_IOleInPlaceFrame,
    &IOleInPlaceFrame_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceObject, ver. 0.0,
   GUID={0x00000113,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceObject_FormatStringOffsetTable[] =
    {
    1542,
    48,
    2274,
    600,
    2670,
    666
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleInPlaceObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceObject_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IOleInPlaceObjectProxyVtbl = 
{
    &IOleInPlaceObject_ProxyInfo,
    &IID_IOleInPlaceObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceObject::InPlaceDeactivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceObject::UIDeactivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceObject::SetObjectRects */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceObject::ReactivateAndUndo */
};

const CInterfaceStubVtbl _IOleInPlaceObjectStubVtbl =
{
    &IID_IOleInPlaceObject,
    &IOleInPlaceObject_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceSite, ver. 0.0,
   GUID={0x00000119,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceSite_FormatStringOffsetTable[] =
    {
    1542,
    48,
    2274,
    600,
    2712,
    2742,
    2802,
    2838,
    2874,
    1908,
    1128,
    2904
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceSite_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceSite_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleInPlaceSite_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceSite_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _IOleInPlaceSiteProxyVtbl = 
{
    &IOleInPlaceSite_ProxyInfo,
    &IID_IOleInPlaceSite,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::CanInPlaceActivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnInPlaceActivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnUIActivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::GetWindowContext */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::Scroll */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnUIDeactivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnInPlaceDeactivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::DiscardUndoState */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::DeactivateAndUndo */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnPosRectChange */
};

const CInterfaceStubVtbl _IOleInPlaceSiteStubVtbl =
{
    &IID_IOleInPlaceSite,
    &IOleInPlaceSite_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IContinue, ver. 0.0,
   GUID={0x0000012a,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IContinue_FormatStringOffsetTable[] =
    {
    486
    };

static const MIDL_STUBLESS_PROXY_INFO IContinue_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IContinue_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IContinue_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IContinue_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IContinueProxyVtbl = 
{
    &IContinue_ProxyInfo,
    &IID_IContinue,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IContinue::FContinue */
};

const CInterfaceStubVtbl _IContinueStubVtbl =
{
    &IID_IContinue,
    &IContinue_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IViewObject, ver. 0.0,
   GUID={0x0000010d,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IViewObject_FormatStringOffsetTable[] =
    {
    2940,
    3024,
    3090,
    810,
    3144,
    3192
    };

static const MIDL_STUBLESS_PROXY_INFO IViewObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IViewObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IViewObject_StubThunkTable[] = 
    {
    IViewObject_RemoteDraw_Thunk,
    IViewObject_RemoteGetColorSet_Thunk,
    IViewObject_RemoteFreeze_Thunk,
    0,
    0,
    IViewObject_RemoteGetAdvise_Thunk
    };

static const MIDL_SERVER_INFO IViewObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IViewObject_FormatStringOffsetTable[-3],
    &IViewObject_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IViewObjectProxyVtbl = 
{
    &IViewObject_ProxyInfo,
    &IID_IViewObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IViewObject_Draw_Proxy ,
    IViewObject_GetColorSet_Proxy ,
    IViewObject_Freeze_Proxy ,
    (void *) (INT_PTR) -1 /* IViewObject::Unfreeze */ ,
    (void *) (INT_PTR) -1 /* IViewObject::SetAdvise */ ,
    IViewObject_GetAdvise_Proxy
};

const CInterfaceStubVtbl _IViewObjectStubVtbl =
{
    &IID_IViewObject,
    &IViewObject_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IViewObject2, ver. 0.0,
   GUID={0x00000127,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IViewObject2_FormatStringOffsetTable[] =
    {
    2940,
    3024,
    3090,
    810,
    3144,
    3192,
    3240
    };

static const MIDL_STUBLESS_PROXY_INFO IViewObject2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IViewObject2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IViewObject2_StubThunkTable[] = 
    {
    IViewObject_RemoteDraw_Thunk,
    IViewObject_RemoteGetColorSet_Thunk,
    IViewObject_RemoteFreeze_Thunk,
    0,
    0,
    IViewObject_RemoteGetAdvise_Thunk,
    0
    };

static const MIDL_SERVER_INFO IViewObject2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IViewObject2_FormatStringOffsetTable[-3],
    &IViewObject2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IViewObject2ProxyVtbl = 
{
    &IViewObject2_ProxyInfo,
    &IID_IViewObject2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IViewObject_Draw_Proxy ,
    IViewObject_GetColorSet_Proxy ,
    IViewObject_Freeze_Proxy ,
    (void *) (INT_PTR) -1 /* IViewObject::Unfreeze */ ,
    (void *) (INT_PTR) -1 /* IViewObject::SetAdvise */ ,
    IViewObject_GetAdvise_Proxy ,
    (void *) (INT_PTR) -1 /* IViewObject2::GetExtent */
};

const CInterfaceStubVtbl _IViewObject2StubVtbl =
{
    &IID_IViewObject2,
    &IViewObject2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDropSource, ver. 0.0,
   GUID={0x00000121,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDropTarget, ver. 0.0,
   GUID={0x00000122,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IDropTarget_FormatStringOffsetTable[] =
    {
    3294,
    3348,
    2274,
    3396
    };

static const MIDL_STUBLESS_PROXY_INFO IDropTarget_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDropTarget_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDropTarget_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDropTarget_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDropTargetProxyVtbl = 
{
    &IDropTarget_ProxyInfo,
    &IID_IDropTarget,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDropTarget::DragEnter */ ,
    (void *) (INT_PTR) -1 /* IDropTarget::DragOver */ ,
    (void *) (INT_PTR) -1 /* IDropTarget::DragLeave */ ,
    (void *) (INT_PTR) -1 /* IDropTarget::Drop */
};

const CInterfaceStubVtbl _IDropTargetStubVtbl =
{
    &IID_IDropTarget,
    &IDropTarget_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumOLEVERB, ver. 0.0,
   GUID={0x00000104,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumOLEVERB_FormatStringOffsetTable[] =
    {
    3450,
    48,
    2274,
    3498
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumOLEVERB_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumOLEVERB_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumOLEVERB_StubThunkTable[] = 
    {
    IEnumOLEVERB_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumOLEVERB_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumOLEVERB_FormatStringOffsetTable[-3],
    &IEnumOLEVERB_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumOLEVERBProxyVtbl = 
{
    &IEnumOLEVERB_ProxyInfo,
    &IID_IEnumOLEVERB,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumOLEVERB_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumOLEVERB::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumOLEVERB::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumOLEVERB::Clone */
};

const CInterfaceStubVtbl _IEnumOLEVERBStubVtbl =
{
    &IID_IEnumOLEVERB,
    &IEnumOLEVERB_ServerInfo,
    7,
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

const CInterfaceProxyVtbl * _oleidl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IEnumOLEVERBProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IViewObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleWindowProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceUIWindowProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceFrameProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceActiveObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleClientSiteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceSiteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IParseDisplayNameProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleContainerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleItemContainerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleLinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleCacheProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDropTargetProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IViewObject2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleCache2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleCacheControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IContinueProxyVtbl,
    0
};

const CInterfaceStubVtbl * _oleidl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IEnumOLEVERBStubVtbl,
    ( CInterfaceStubVtbl *) &_IViewObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleWindowStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceUIWindowStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceFrameStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceActiveObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleClientSiteStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceSiteStubVtbl,
    ( CInterfaceStubVtbl *) &_IParseDisplayNameStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleContainerStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleItemContainerStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleLinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleCacheStubVtbl,
    ( CInterfaceStubVtbl *) &_IDropTargetStubVtbl,
    ( CInterfaceStubVtbl *) &_IViewObject2StubVtbl,
    ( CInterfaceStubVtbl *) &_IOleCache2StubVtbl,
    ( CInterfaceStubVtbl *) &_IOleCacheControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IContinueStubVtbl,
    0
};

PCInterfaceName const _oleidl_InterfaceNamesList[] = 
{
    "IEnumOLEVERB",
    "IViewObject",
    "IOleObject",
    "IOleInPlaceObject",
    "IOleWindow",
    "IOleInPlaceUIWindow",
    "IOleInPlaceFrame",
    "IOleInPlaceActiveObject",
    "IOleClientSite",
    "IOleInPlaceSite",
    "IParseDisplayName",
    "IOleContainer",
    "IOleItemContainer",
    "IOleLink",
    "IOleCache",
    "IDropTarget",
    "IViewObject2",
    "IOleCache2",
    "IOleCacheControl",
    "IContinue",
    0
};


#define _oleidl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _oleidl, pIID, n)

int __stdcall _oleidl_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _oleidl, 20, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _oleidl, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _oleidl, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _oleidl, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _oleidl, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _oleidl, 20, *pIndex )
    
}

const ExtendedProxyFileInfo oleidl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _oleidl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _oleidl_StubVtblList,
    (const PCInterfaceName * ) & _oleidl_InterfaceNamesList,
    0, // no delegation
    & _oleidl_IID_Lookup, 
    20,
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
/* at Sun May 05 19:07:33 2002
 */
/* Compiler settings for oleidl.idl:
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


#include "oleidl.h"

#define TYPE_FORMAT_STRING_SIZE   1105                              
#define PROC_FORMAT_STRING_SIZE   3707                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   6            

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


extern const MIDL_SERVER_INFO IOleCache_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleCache_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleCache2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleCache2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IOleCache2_RemoteUpdateCache_Proxy( 
    IOleCache2 * This,
    /* [in] */ LPDATAOBJECT pDataObject,
    /* [in] */ DWORD grfUpdf,
    /* [in] */ LONG_PTR pReserved)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[214],
                  ( unsigned char * )This,
                  pDataObject,
                  grfUpdf,
                  pReserved);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOleCache2_RemoteUpdateCache_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IOleCache2 *This;
        LPDATAOBJECT pDataObject;
        DWORD grfUpdf;
        char Pad0[4];
        LONG_PTR pReserved;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOleCache2_UpdateCache_Stub(
                                      (IOleCache2 *) pParamStruct->This,
                                      pParamStruct->pDataObject,
                                      pParamStruct->grfUpdf,
                                      pParamStruct->pReserved);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleCacheControl_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleCacheControl_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IParseDisplayName_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IParseDisplayName_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleContainer_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleContainer_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleClientSite_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleClientSite_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleObject_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleWindow_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleWindow_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleLink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleLink_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleItemContainer_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleItemContainer_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceUIWindow_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceUIWindow_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceActiveObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceActiveObject_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_RemoteTranslateAccelerator_Proxy( 
    IOleInPlaceActiveObject * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2390],
                  ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOleInPlaceActiveObject_RemoteTranslateAccelerator_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IOleInPlaceActiveObject *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOleInPlaceActiveObject_TranslateAccelerator_Stub((IOleInPlaceActiveObject *) pParamStruct->This);
    
}

/* [input_sync][call_as] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_RemoteResizeBorder_Proxy( 
    IOleInPlaceActiveObject * This,
    /* [in] */ LPCRECT prcBorder,
    /* [in] */ REFIID riid,
    /* [iid_is][unique][in] */ IOleInPlaceUIWindow *pUIWindow,
    /* [in] */ BOOL fFrameWindow)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2498],
                  ( unsigned char * )This,
                  prcBorder,
                  riid,
                  pUIWindow,
                  fFrameWindow);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IOleInPlaceActiveObject_RemoteResizeBorder_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IOleInPlaceActiveObject *This;
        LPCRECT prcBorder;
        REFIID riid;
        IOleInPlaceUIWindow *pUIWindow;
        BOOL fFrameWindow;
        char Pad0[4];
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IOleInPlaceActiveObject_ResizeBorder_Stub(
                                                    (IOleInPlaceActiveObject *) pParamStruct->This,
                                                    pParamStruct->prcBorder,
                                                    pParamStruct->riid,
                                                    pParamStruct->pUIWindow,
                                                    pParamStruct->fFrameWindow);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceFrame_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceFrame_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceObject_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOleInPlaceSite_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOleInPlaceSite_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IContinue_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IContinue_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IViewObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IViewObject_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteDraw_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ ULONG_PTR hdcTargetDev,
    /* [in] */ ULONG_PTR hdcDraw,
    /* [in] */ LPCRECTL lprcBounds,
    /* [unique][in] */ LPCRECTL lprcWBounds,
    /* [in] */ IContinue *pContinue)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3090],
                  ( unsigned char * )This,
                  dwDrawAspect,
                  lindex,
                  pvAspect,
                  ptd,
                  hdcTargetDev,
                  hdcDraw,
                  lprcBounds,
                  lprcWBounds,
                  pContinue);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IViewObject_RemoteDraw_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IViewObject *This;
        DWORD dwDrawAspect;
        char Pad0[4];
        LONG lindex;
        char Pad1[4];
        ULONG_PTR pvAspect;
        DVTARGETDEVICE *ptd;
        ULONG_PTR hdcTargetDev;
        ULONG_PTR hdcDraw;
        LPCRECTL lprcBounds;
        LPCRECTL lprcWBounds;
        IContinue *pContinue;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IViewObject_Draw_Stub(
                                (IViewObject *) pParamStruct->This,
                                pParamStruct->dwDrawAspect,
                                pParamStruct->lindex,
                                pParamStruct->pvAspect,
                                pParamStruct->ptd,
                                pParamStruct->hdcTargetDev,
                                pParamStruct->hdcDraw,
                                pParamStruct->lprcBounds,
                                pParamStruct->lprcWBounds,
                                pParamStruct->pContinue);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteGetColorSet_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ ULONG_PTR hicTargetDev,
    /* [out] */ LOGPALETTE **ppColorSet)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3176],
                  ( unsigned char * )This,
                  dwDrawAspect,
                  lindex,
                  pvAspect,
                  ptd,
                  hicTargetDev,
                  ppColorSet);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IViewObject_RemoteGetColorSet_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IViewObject *This;
        DWORD dwDrawAspect;
        char Pad0[4];
        LONG lindex;
        char Pad1[4];
        ULONG_PTR pvAspect;
        DVTARGETDEVICE *ptd;
        ULONG_PTR hicTargetDev;
        LOGPALETTE **ppColorSet;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IViewObject_GetColorSet_Stub(
                                       (IViewObject *) pParamStruct->This,
                                       pParamStruct->dwDrawAspect,
                                       pParamStruct->lindex,
                                       pParamStruct->pvAspect,
                                       pParamStruct->ptd,
                                       pParamStruct->hicTargetDev,
                                       pParamStruct->ppColorSet);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteFreeze_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [out] */ DWORD *pdwFreeze)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3244],
                  ( unsigned char * )This,
                  dwDrawAspect,
                  lindex,
                  pvAspect,
                  pdwFreeze);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IViewObject_RemoteFreeze_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IViewObject *This;
        DWORD dwDrawAspect;
        char Pad0[4];
        LONG lindex;
        char Pad1[4];
        ULONG_PTR pvAspect;
        DWORD *pdwFreeze;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IViewObject_Freeze_Stub(
                                  (IViewObject *) pParamStruct->This,
                                  pParamStruct->dwDrawAspect,
                                  pParamStruct->lindex,
                                  pParamStruct->pvAspect,
                                  pParamStruct->pdwFreeze);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteGetAdvise_Proxy( 
    IViewObject * This,
    /* [out] */ DWORD *pAspects,
    /* [out] */ DWORD *pAdvf,
    /* [out] */ IAdviseSink **ppAdvSink)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3350],
                  ( unsigned char * )This,
                  pAspects,
                  pAdvf,
                  ppAdvSink);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IViewObject_RemoteGetAdvise_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IViewObject *This;
        DWORD *pAspects;
        DWORD *pAdvf;
        IAdviseSink **ppAdvSink;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IViewObject_GetAdvise_Stub(
                                     (IViewObject *) pParamStruct->This,
                                     pParamStruct->pAspects,
                                     pParamStruct->pAdvf,
                                     pParamStruct->ppAdvSink);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IViewObject2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IViewObject2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDropTarget_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDropTarget_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumOLEVERB_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumOLEVERB_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumOLEVERB_RemoteNext_Proxy( 
    IEnumOLEVERB * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPOLEVERB rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3618],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumOLEVERB_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumOLEVERB *This;
        ULONG celt;
        char Pad0[4];
        LPOLEVERB rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumOLEVERB_Next_Stub(
                                 (IEnumOLEVERB *) pParamStruct->This,
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

	/* Procedure Cache */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x2 ),	/* 2 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 26 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter advf */

/* 32 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwConnection */

/* 38 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure ContextSensitiveHelp */


	/* Procedure Uncache */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x4 ),	/* 4 */
/* 58 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 66 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */


	/* Parameter fEnterMode */


	/* Parameter dwConnection */

/* 76 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 78 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 80 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 82 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 84 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 86 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumCache */

/* 88 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 90 */	NdrFcLong( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x5 ),	/* 5 */
/* 96 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x8 ),	/* 8 */
/* 102 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 104 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenumSTATDATA */

/* 114 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 116 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 118 */	NdrFcShort( 0x60 ),	/* Type Offset=96 */

	/* Return value */

/* 120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 122 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InitCache */

/* 126 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 128 */	NdrFcLong( 0x0 ),	/* 0 */
/* 132 */	NdrFcShort( 0x6 ),	/* 6 */
/* 134 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 140 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 142 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObject */

/* 152 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 154 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 156 */	NdrFcShort( 0x76 ),	/* Type Offset=118 */

	/* Return value */

/* 158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 160 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetData */

/* 164 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 170 */	NdrFcShort( 0x7 ),	/* 7 */
/* 172 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 174 */	NdrFcShort( 0x8 ),	/* 8 */
/* 176 */	NdrFcShort( 0x8 ),	/* 8 */
/* 178 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 180 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 184 */	NdrFcShort( 0x13 ),	/* 19 */
/* 186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 188 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 190 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 192 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 194 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter pmedium */

/* 196 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 198 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 200 */	NdrFcShort( 0x88 ),	/* Type Offset=136 */

	/* Parameter fRelease */

/* 202 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 204 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 210 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteUpdateCache */

/* 214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x8 ),	/* 8 */
/* 222 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 224 */	NdrFcShort( 0x10 ),	/* 16 */
/* 226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 228 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 230 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObject */

/* 240 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 242 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 244 */	NdrFcShort( 0x76 ),	/* Type Offset=118 */

	/* Parameter grfUpdf */

/* 246 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 248 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pReserved */

/* 252 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 254 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 256 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Return value */

/* 258 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 260 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 262 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnableModeless */


	/* Procedure DiscardCache */

/* 264 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 266 */	NdrFcLong( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x9 ),	/* 9 */
/* 272 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 274 */	NdrFcShort( 0x8 ),	/* 8 */
/* 276 */	NdrFcShort( 0x8 ),	/* 8 */
/* 278 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 280 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fEnable */


	/* Parameter dwDiscardOptions */

/* 290 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 292 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 296 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 298 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnRun */

/* 302 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 304 */	NdrFcLong( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x3 ),	/* 3 */
/* 310 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x8 ),	/* 8 */
/* 316 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 318 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 326 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObject */

/* 328 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 330 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 332 */	NdrFcShort( 0x76 ),	/* Type Offset=118 */

	/* Return value */

/* 334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 336 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnStop */

/* 340 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 342 */	NdrFcLong( 0x0 ),	/* 0 */
/* 346 */	NdrFcShort( 0x4 ),	/* 4 */
/* 348 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 354 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 356 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 366 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 368 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ParseDisplayName */

/* 372 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x3 ),	/* 3 */
/* 380 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x24 ),	/* 36 */
/* 386 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 388 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 398 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 400 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 402 */	NdrFcShort( 0x216 ),	/* Type Offset=534 */

	/* Parameter pszDisplayName */

/* 404 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 406 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 408 */	NdrFcShort( 0x22a ),	/* Type Offset=554 */

	/* Parameter pchEaten */

/* 410 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 412 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmkOut */

/* 416 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 418 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 420 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Return value */

/* 422 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 424 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumObjects */

/* 428 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0x4 ),	/* 4 */
/* 436 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 438 */	NdrFcShort( 0x8 ),	/* 8 */
/* 440 */	NdrFcShort( 0x8 ),	/* 8 */
/* 442 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 444 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 450 */	NdrFcShort( 0x0 ),	/* 0 */
/* 452 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfFlags */

/* 454 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 456 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenum */

/* 460 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 462 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 464 */	NdrFcShort( 0x242 ),	/* Type Offset=578 */

	/* Return value */

/* 466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 468 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LockContainer */

/* 472 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x5 ),	/* 5 */
/* 480 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 482 */	NdrFcShort( 0x8 ),	/* 8 */
/* 484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 486 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 488 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fLock */

/* 498 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 500 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 504 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 506 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 508 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FContinue */


	/* Procedure SaveObject */

/* 510 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 512 */	NdrFcLong( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x3 ),	/* 3 */
/* 518 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 522 */	NdrFcShort( 0x8 ),	/* 8 */
/* 524 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 526 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 536 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 538 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMoniker */

/* 542 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 544 */	NdrFcLong( 0x0 ),	/* 0 */
/* 548 */	NdrFcShort( 0x4 ),	/* 4 */
/* 550 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 552 */	NdrFcShort( 0x10 ),	/* 16 */
/* 554 */	NdrFcShort( 0x8 ),	/* 8 */
/* 556 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 558 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAssign */

/* 568 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 570 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 572 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwWhichMoniker */

/* 574 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 576 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 578 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmk */

/* 580 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 582 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 584 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Return value */

/* 586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 588 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContainer */

/* 592 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 598 */	NdrFcShort( 0x5 ),	/* 5 */
/* 600 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 606 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 608 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppContainer */

/* 618 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 620 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 622 */	NdrFcShort( 0x258 ),	/* Type Offset=600 */

	/* Return value */

/* 624 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 626 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnInPlaceActivate */


	/* Procedure UIDeactivate */


	/* Procedure ShowObject */

/* 630 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 632 */	NdrFcLong( 0x0 ),	/* 0 */
/* 636 */	NdrFcShort( 0x6 ),	/* 6 */
/* 638 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 644 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 646 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 654 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 656 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 658 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnShowWindow */

/* 662 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 664 */	NdrFcLong( 0x0 ),	/* 0 */
/* 668 */	NdrFcShort( 0x7 ),	/* 7 */
/* 670 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 672 */	NdrFcShort( 0x8 ),	/* 8 */
/* 674 */	NdrFcShort( 0x8 ),	/* 8 */
/* 676 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 678 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fShow */

/* 688 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 690 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 692 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 694 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 696 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 698 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReactivateAndUndo */


	/* Procedure RequestNewObjectLayout */

/* 700 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 702 */	NdrFcLong( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0x8 ),	/* 8 */
/* 708 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 710 */	NdrFcShort( 0x0 ),	/* 0 */
/* 712 */	NdrFcShort( 0x8 ),	/* 8 */
/* 714 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 716 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x0 ),	/* 0 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 728 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetClientSite */

/* 732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 738 */	NdrFcShort( 0x3 ),	/* 3 */
/* 740 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 746 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 748 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 756 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pClientSite */

/* 758 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 760 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 762 */	NdrFcShort( 0x26e ),	/* Type Offset=622 */

	/* Return value */

/* 764 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 766 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 768 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClientSite */

/* 770 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 772 */	NdrFcLong( 0x0 ),	/* 0 */
/* 776 */	NdrFcShort( 0x4 ),	/* 4 */
/* 778 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0x8 ),	/* 8 */
/* 784 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 786 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0x0 ),	/* 0 */
/* 792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 794 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppClientSite */

/* 796 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 798 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 800 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Return value */

/* 802 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 804 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 806 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHostNames */

/* 808 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 810 */	NdrFcLong( 0x0 ),	/* 0 */
/* 814 */	NdrFcShort( 0x5 ),	/* 5 */
/* 816 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 820 */	NdrFcShort( 0x8 ),	/* 8 */
/* 822 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 824 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x0 ),	/* 0 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szContainerApp */

/* 834 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 836 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 838 */	NdrFcShort( 0x22a ),	/* Type Offset=554 */

	/* Parameter szContainerObj */

/* 840 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 842 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 844 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 846 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 848 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Unfreeze */


	/* Procedure Close */

/* 852 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 854 */	NdrFcLong( 0x0 ),	/* 0 */
/* 858 */	NdrFcShort( 0x6 ),	/* 6 */
/* 860 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 864 */	NdrFcShort( 0x8 ),	/* 8 */
/* 866 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 868 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwFreeze */


	/* Parameter dwSaveOption */

/* 878 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 880 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 884 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 886 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMoniker */

/* 890 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 892 */	NdrFcLong( 0x0 ),	/* 0 */
/* 896 */	NdrFcShort( 0x7 ),	/* 7 */
/* 898 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 902 */	NdrFcShort( 0x8 ),	/* 8 */
/* 904 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 906 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 914 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwWhichMoniker */

/* 916 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 918 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 920 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pmk */

/* 922 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 924 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 926 */	NdrFcShort( 0x230 ),	/* Type Offset=560 */

	/* Return value */

/* 928 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 930 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 932 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMoniker */

/* 934 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 936 */	NdrFcLong( 0x0 ),	/* 0 */
/* 940 */	NdrFcShort( 0x8 ),	/* 8 */
/* 942 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 944 */	NdrFcShort( 0x10 ),	/* 16 */
/* 946 */	NdrFcShort( 0x8 ),	/* 8 */
/* 948 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 950 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAssign */

/* 960 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 962 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwWhichMoniker */

/* 966 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 968 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmk */

/* 972 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 974 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 976 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Return value */

/* 978 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 980 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 982 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InitFromData */

/* 984 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 986 */	NdrFcLong( 0x0 ),	/* 0 */
/* 990 */	NdrFcShort( 0x9 ),	/* 9 */
/* 992 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 994 */	NdrFcShort( 0x10 ),	/* 16 */
/* 996 */	NdrFcShort( 0x8 ),	/* 8 */
/* 998 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1000 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObject */

/* 1010 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1012 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1014 */	NdrFcShort( 0x76 ),	/* Type Offset=118 */

	/* Parameter fCreation */

/* 1016 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1018 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwReserved */

/* 1022 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1024 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1026 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1028 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1030 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClipboardData */

/* 1034 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1036 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1040 */	NdrFcShort( 0xa ),	/* 10 */
/* 1042 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1044 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1046 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1048 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1050 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1052 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1056 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1058 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwReserved */

/* 1060 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1062 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1064 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppDataObject */

/* 1066 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1068 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1070 */	NdrFcShort( 0x284 ),	/* Type Offset=644 */

	/* Return value */

/* 1072 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1074 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1076 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DoVerb */

/* 1078 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1080 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1084 */	NdrFcShort( 0xb ),	/* 11 */
/* 1086 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 1088 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1090 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1092 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 1094 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1098 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1102 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter iVerb */

/* 1104 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1106 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpmsg */

/* 1110 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1112 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1114 */	NdrFcShort( 0x288 ),	/* Type Offset=648 */

	/* Parameter pActiveSite */

/* 1116 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1118 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1120 */	NdrFcShort( 0x26e ),	/* Type Offset=622 */

	/* Parameter lindex */

/* 1122 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1124 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hwndParent */

/* 1128 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1130 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1132 */	NdrFcShort( 0x2a4 ),	/* Type Offset=676 */

	/* Parameter lprcPosRect */

/* 1134 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 1136 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1138 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Return value */

/* 1140 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1142 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 1144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumVerbs */

/* 1146 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1148 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1152 */	NdrFcShort( 0xc ),	/* 12 */
/* 1154 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1158 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1160 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1162 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1170 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppEnumOleVerb */

/* 1172 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1174 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1176 */	NdrFcShort( 0x2dc ),	/* Type Offset=732 */

	/* Return value */

/* 1178 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1180 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeactivateAndUndo */


	/* Procedure Update */

/* 1184 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1186 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1190 */	NdrFcShort( 0xd ),	/* 13 */
/* 1192 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1198 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1200 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 1210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1212 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsUpToDate */

/* 1216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1222 */	NdrFcShort( 0xe ),	/* 14 */
/* 1224 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1230 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1232 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1240 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1242 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1244 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetUserClassID */

/* 1248 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1254 */	NdrFcShort( 0xf ),	/* 15 */
/* 1256 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1260 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1262 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1264 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1272 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pClsid */

/* 1274 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1276 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1278 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Return value */

/* 1280 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1282 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1284 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetUserType */

/* 1286 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1288 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1292 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1294 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1300 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1302 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1310 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwFormOfType */

/* 1312 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1314 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pszUserType */

/* 1318 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1320 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1322 */	NdrFcShort( 0x308 ),	/* Type Offset=776 */

	/* Return value */

/* 1324 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1326 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetExtent */

/* 1330 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1332 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1336 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1338 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1340 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1342 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1344 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1346 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1354 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 1356 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1358 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1360 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter psizel */

/* 1362 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1364 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1366 */	NdrFcShort( 0x2ae ),	/* Type Offset=686 */

	/* Return value */

/* 1368 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1370 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExtent */

/* 1374 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1376 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1380 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1382 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1384 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1386 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1388 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1390 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1398 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 1400 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1402 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1404 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter psizel */

/* 1406 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1408 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1410 */	NdrFcShort( 0x2ae ),	/* Type Offset=686 */

	/* Return value */

/* 1412 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1414 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Advise */

/* 1418 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1420 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1424 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1426 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1430 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1432 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1434 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1442 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pAdvSink */

/* 1444 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1446 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1448 */	NdrFcShort( 0x318 ),	/* Type Offset=792 */

	/* Parameter pdwConnection */

/* 1450 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1452 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1456 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1458 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Unadvise */

/* 1462 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1464 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1468 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1470 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1472 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1474 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1476 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1478 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1486 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwConnection */

/* 1488 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1490 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1492 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1494 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1496 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumAdvise */

/* 1500 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1502 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1506 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1508 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1512 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1514 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1516 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1518 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1524 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenumAdvise */

/* 1526 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1528 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1530 */	NdrFcShort( 0x60 ),	/* Type Offset=96 */

	/* Return value */

/* 1532 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1534 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMiscStatus */

/* 1538 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1540 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1544 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1546 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1548 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1550 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1552 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1554 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1562 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAspect */

/* 1564 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1566 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwStatus */

/* 1570 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1572 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1574 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1576 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1578 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetColorScheme */

/* 1582 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1584 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1588 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1590 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1594 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1596 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1598 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1602 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1606 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pLogpal */

/* 1608 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1610 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1612 */	NdrFcShort( 0x1b2 ),	/* Type Offset=434 */

	/* Return value */

/* 1614 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1616 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1618 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindow */

/* 1620 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1622 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 1626 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1628 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1632 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1634 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1636 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1638 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1644 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phwnd */

/* 1646 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1648 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1650 */	NdrFcShort( 0x336 ),	/* Type Offset=822 */

	/* Return value */

/* 1652 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1654 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1656 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetUpdateOptions */

/* 1658 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1660 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1664 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1666 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1668 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1670 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1672 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1674 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1682 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwUpdateOpt */

/* 1684 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1686 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1688 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1690 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1692 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetUpdateOptions */

/* 1696 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1698 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1702 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1704 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1708 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1710 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1712 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1720 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pdwUpdateOpt */

/* 1722 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1724 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1728 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1730 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1732 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSourceMoniker */

/* 1734 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1736 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1740 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1742 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1744 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1746 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1748 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1750 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1758 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */

/* 1760 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1762 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1764 */	NdrFcShort( 0x230 ),	/* Type Offset=560 */

	/* Parameter rclsid */

/* 1766 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1768 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1770 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Return value */

/* 1772 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1774 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1776 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSourceMoniker */

/* 1778 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1780 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1784 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1786 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1790 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1792 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1794 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1802 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppmk */

/* 1804 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1806 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1808 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */

	/* Return value */

/* 1810 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1812 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1814 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSourceDisplayName */

/* 1816 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1818 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1822 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1824 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1828 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1830 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1832 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1840 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszStatusText */

/* 1842 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1844 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1846 */	NdrFcShort( 0x22a ),	/* Type Offset=554 */

	/* Return value */

/* 1848 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1850 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1852 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSourceDisplayName */

/* 1854 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1856 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1860 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1862 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1864 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1868 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1870 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1878 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppszDisplayName */

/* 1880 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1882 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1884 */	NdrFcShort( 0x308 ),	/* Type Offset=776 */

	/* Return value */

/* 1886 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1888 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1890 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BindToSource */

/* 1892 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1894 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1898 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1900 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1902 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1904 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1906 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1908 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1916 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bindflags */

/* 1918 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1920 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbc */

/* 1924 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1926 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1928 */	NdrFcShort( 0x216 ),	/* Type Offset=534 */

	/* Return value */

/* 1930 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1932 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BindIfRunning */

/* 1936 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1938 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1942 */	NdrFcShort( 0xa ),	/* 10 */
/* 1944 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1950 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1952 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1960 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1962 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1964 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1966 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBoundSource */

/* 1968 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1970 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1974 */	NdrFcShort( 0xb ),	/* 11 */
/* 1976 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1980 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1982 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1984 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1988 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1992 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppunk */

/* 1994 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1996 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1998 */	NdrFcShort( 0x344 ),	/* Type Offset=836 */

	/* Return value */

/* 2000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2002 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DiscardUndoState */


	/* Procedure UnbindSource */

/* 2006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2012 */	NdrFcShort( 0xc ),	/* 12 */
/* 2014 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2018 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2020 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2022 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2030 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 2032 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2034 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2036 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Update */

/* 2038 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2040 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2044 */	NdrFcShort( 0xd ),	/* 13 */
/* 2046 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2050 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2052 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2054 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2056 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2058 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2062 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 2064 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2066 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2068 */	NdrFcShort( 0x216 ),	/* Type Offset=534 */

	/* Return value */

/* 2070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2072 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObject */

/* 2076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2082 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2084 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2086 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2088 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2090 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2092 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2094 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszItem */

/* 2102 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2104 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2106 */	NdrFcShort( 0x22a ),	/* Type Offset=554 */

	/* Parameter dwSpeedNeeded */

/* 2108 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2110 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbc */

/* 2114 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2116 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2118 */	NdrFcShort( 0x216 ),	/* Type Offset=534 */

	/* Parameter riid */

/* 2120 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2122 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2124 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Parameter ppvObject */

/* 2126 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2128 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2130 */	NdrFcShort( 0x348 ),	/* Type Offset=840 */

	/* Return value */

/* 2132 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2134 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjectStorage */

/* 2138 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2144 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2146 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2148 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2152 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2154 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2156 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2162 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszItem */

/* 2164 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2166 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2168 */	NdrFcShort( 0x22a ),	/* Type Offset=554 */

	/* Parameter pbc */

/* 2170 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2172 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2174 */	NdrFcShort( 0x216 ),	/* Type Offset=534 */

	/* Parameter riid */

/* 2176 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2178 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2180 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Parameter ppvStorage */

/* 2182 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2184 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2186 */	NdrFcShort( 0x354 ),	/* Type Offset=852 */

	/* Return value */

/* 2188 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2190 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsRunning */

/* 2194 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2196 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2200 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2202 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2206 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2208 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2210 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2218 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszItem */

/* 2220 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2222 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2224 */	NdrFcShort( 0x22a ),	/* Type Offset=554 */

	/* Return value */

/* 2226 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2228 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBorder */

/* 2232 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2234 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2238 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2240 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2244 */	NdrFcShort( 0x3c ),	/* 60 */
/* 2246 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2248 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2256 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lprectBorder */

/* 2258 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2260 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2262 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Return value */

/* 2264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2266 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RequestBorderSpace */

/* 2270 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2272 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2276 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2278 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2280 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2282 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2284 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2286 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2294 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pborderwidths */

/* 2296 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2298 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2300 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Return value */

/* 2302 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2304 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetBorderSpace */

/* 2308 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2310 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2314 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2316 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2318 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2320 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2322 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2324 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2332 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pborderwidths */

/* 2334 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2336 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2338 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Return value */

/* 2340 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2342 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetActiveObject */

/* 2346 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2348 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2354 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2358 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2360 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2362 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2370 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pActiveObject */

/* 2372 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2374 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2376 */	NdrFcShort( 0x364 ),	/* Type Offset=868 */

	/* Parameter pszObjName */

/* 2378 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2380 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2382 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 2384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2386 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure DragLeave */


	/* Procedure CanInPlaceActivate */


	/* Procedure InPlaceDeactivate */


	/* Procedure RemoteTranslateAccelerator */

/* 2390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2396 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2398 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2404 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2406 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2414 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2416 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2418 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnFrameWindowActivate */

/* 2422 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2424 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2428 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2430 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2432 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2436 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2438 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2446 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fActivate */

/* 2448 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2450 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2454 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2456 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnDocWindowActivate */

/* 2460 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2462 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2466 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2468 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2470 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2472 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2474 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2476 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2484 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fActivate */

/* 2486 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2488 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2494 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteResizeBorder */

/* 2498 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2500 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2506 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2508 */	NdrFcShort( 0x80 ),	/* 128 */
/* 2510 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2512 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2514 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2518 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2522 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter prcBorder */

/* 2524 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2526 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2528 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter riid */

/* 2530 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2532 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2534 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Parameter pUIWindow */

/* 2536 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2538 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2540 */	NdrFcShort( 0x37a ),	/* Type Offset=890 */

	/* Parameter fFrameWindow */

/* 2542 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2544 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2548 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2550 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InsertMenus */

/* 2554 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2556 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2560 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2562 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2564 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2566 */	NdrFcShort( 0x54 ),	/* 84 */
/* 2568 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2570 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2574 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2578 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hmenuShared */

/* 2580 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2582 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2584 */	NdrFcShort( 0x382 ),	/* Type Offset=898 */

	/* Parameter lpMenuWidths */

/* 2586 */	NdrFcShort( 0x11a ),	/* Flags:  must free, in, out, simple ref, */
/* 2588 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2590 */	NdrFcShort( 0x396 ),	/* Type Offset=918 */

	/* Return value */

/* 2592 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2594 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMenu */

/* 2598 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2600 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2604 */	NdrFcShort( 0xa ),	/* 10 */
/* 2606 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2610 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2612 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 2614 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2618 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2622 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hmenuShared */

/* 2624 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2626 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2628 */	NdrFcShort( 0x382 ),	/* Type Offset=898 */

	/* Parameter holemenu */

/* 2630 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2632 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2634 */	NdrFcShort( 0x3a0 ),	/* Type Offset=928 */

	/* Parameter hwndActiveObject */

/* 2636 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2638 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2640 */	NdrFcShort( 0x2a4 ),	/* Type Offset=676 */

	/* Return value */

/* 2642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2644 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveMenus */

/* 2648 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2654 */	NdrFcShort( 0xb ),	/* 11 */
/* 2656 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2662 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2664 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2668 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2672 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hmenuShared */

/* 2674 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2676 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2678 */	NdrFcShort( 0x382 ),	/* Type Offset=898 */

	/* Return value */

/* 2680 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2682 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2684 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetStatusText */

/* 2686 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2688 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2692 */	NdrFcShort( 0xc ),	/* 12 */
/* 2694 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2698 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2700 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2702 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2708 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2710 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszStatusText */

/* 2712 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2714 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2716 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 2718 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2720 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2722 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnableModeless */

/* 2724 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2726 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2730 */	NdrFcShort( 0xd ),	/* 13 */
/* 2732 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2734 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2736 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2738 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2740 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2748 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fEnable */

/* 2750 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2752 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2756 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2758 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure TranslateAccelerator */

/* 2762 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2764 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2768 */	NdrFcShort( 0xe ),	/* 14 */
/* 2770 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2772 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2774 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2776 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2778 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2782 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2786 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lpmsg */

/* 2788 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2790 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2792 */	NdrFcShort( 0x2b6 ),	/* Type Offset=694 */

	/* Parameter wID */

/* 2794 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2796 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2798 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 2800 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2802 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2804 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetObjectRects */

/* 2806 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2808 */	NdrFcLong( 0x20000000 ),	/* 536870912 */
/* 2812 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2814 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2816 */	NdrFcShort( 0x68 ),	/* 104 */
/* 2818 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2820 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2822 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2830 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lprcPosRect */

/* 2832 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2834 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2836 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter lprcClipRect */

/* 2838 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2840 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2842 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Return value */

/* 2844 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2846 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnUIActivate */

/* 2850 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2852 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2856 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2858 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2864 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2866 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2874 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2876 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2878 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2880 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindowContext */

/* 2882 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2884 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2888 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2890 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2894 */	NdrFcShort( 0x70 ),	/* 112 */
/* 2896 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2898 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 2900 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2902 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2906 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppFrame */

/* 2908 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2910 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2912 */	NdrFcShort( 0x3ae ),	/* Type Offset=942 */

	/* Parameter ppDoc */

/* 2914 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2916 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2918 */	NdrFcShort( 0x3c4 ),	/* Type Offset=964 */

	/* Parameter lprcPosRect */

/* 2920 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2922 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2924 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter lprcClipRect */

/* 2926 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2928 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2930 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter lpFrameInfo */

/* 2932 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 2934 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2936 */	NdrFcShort( 0x3e8 ),	/* Type Offset=1000 */

	/* Return value */

/* 2938 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2940 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Scroll */

/* 2944 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2946 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2950 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2952 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2954 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2956 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2958 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2960 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2964 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2966 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2968 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scrollExtant */

/* 2970 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2972 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2974 */	NdrFcShort( 0x2ae ),	/* Type Offset=686 */

	/* Return value */

/* 2976 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2978 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnUIDeactivate */

/* 2982 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2984 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2988 */	NdrFcShort( 0xa ),	/* 10 */
/* 2990 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2992 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2994 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2996 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2998 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3000 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3006 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fUndoable */

/* 3008 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3010 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3014 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3016 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnInPlaceDeactivate */

/* 3020 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3022 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3026 */	NdrFcShort( 0xb ),	/* 11 */
/* 3028 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3032 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3034 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 3036 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3038 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3044 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 3046 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3048 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3050 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnPosRectChange */

/* 3052 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3054 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3058 */	NdrFcShort( 0xe ),	/* 14 */
/* 3060 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3062 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3064 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3066 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3068 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3076 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lprcPosRect */

/* 3078 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3080 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3082 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Return value */

/* 3084 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3086 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3088 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteDraw */

/* 3090 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3092 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3096 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3098 */	NdrFcShort( 0x58 ),	/* ia64 Stack size/offset = 88 */
/* 3100 */	NdrFcShort( 0x90 ),	/* 144 */
/* 3102 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3104 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 3106 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3110 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3114 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 3116 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3118 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3120 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 3122 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3124 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvAspect */

/* 3128 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3130 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3132 */	0xb9,		/* FC_UINT3264 */
			0x0,		/* 0 */

	/* Parameter ptd */

/* 3134 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3136 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3138 */	NdrFcShort( 0x3fe ),	/* Type Offset=1022 */

	/* Parameter hdcTargetDev */

/* 3140 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3142 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3144 */	0xb9,		/* FC_UINT3264 */
			0x0,		/* 0 */

	/* Parameter hdcDraw */

/* 3146 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3148 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3150 */	0xb9,		/* FC_UINT3264 */
			0x0,		/* 0 */

	/* Parameter lprcBounds */

/* 3152 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3154 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3156 */	NdrFcShort( 0x2d2 ),	/* Type Offset=722 */

	/* Parameter lprcWBounds */

/* 3158 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3160 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 3162 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Parameter pContinue */

/* 3164 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3166 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 3168 */	NdrFcShort( 0x402 ),	/* Type Offset=1026 */

	/* Return value */

/* 3170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3172 */	NdrFcShort( 0x50 ),	/* ia64 Stack size/offset = 80 */
/* 3174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetColorSet */

/* 3176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3182 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3184 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 3186 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3188 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3190 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 3192 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 3194 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3196 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3200 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 3202 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3204 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 3208 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3210 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvAspect */

/* 3214 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3216 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3218 */	0xb9,		/* FC_UINT3264 */
			0x0,		/* 0 */

	/* Parameter ptd */

/* 3220 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3222 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3224 */	NdrFcShort( 0x3fe ),	/* Type Offset=1022 */

	/* Parameter hicTargetDev */

/* 3226 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3228 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3230 */	0xb9,		/* FC_UINT3264 */
			0x0,		/* 0 */

	/* Parameter ppColorSet */

/* 3232 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 3234 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3236 */	NdrFcShort( 0x414 ),	/* Type Offset=1044 */

	/* Return value */

/* 3238 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3240 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3242 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteFreeze */

/* 3244 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3246 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3250 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3252 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3254 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3256 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3258 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 3260 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3268 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 3270 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3272 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 3276 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3278 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pvAspect */

/* 3282 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3284 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3286 */	0xb9,		/* FC_UINT3264 */
			0x0,		/* 0 */

	/* Parameter pdwFreeze */

/* 3288 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3290 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3294 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3296 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3298 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAdvise */

/* 3300 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3302 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3306 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3308 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3310 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3312 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3314 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 3316 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3324 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter aspects */

/* 3326 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3328 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter advf */

/* 3332 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3334 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAdvSink */

/* 3338 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3340 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3342 */	NdrFcShort( 0x318 ),	/* Type Offset=792 */

	/* Return value */

/* 3344 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3346 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetAdvise */

/* 3350 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3352 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3356 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3358 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3362 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3364 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 3366 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3374 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pAspects */

/* 3376 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3378 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAdvf */

/* 3382 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3384 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3386 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppAdvSink */

/* 3388 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3390 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3392 */	NdrFcShort( 0x41c ),	/* Type Offset=1052 */

	/* Return value */

/* 3394 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3396 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3398 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExtent */

/* 3400 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3402 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3406 */	NdrFcShort( 0x9 ),	/* 9 */
/* 3408 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3410 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3412 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3414 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3416 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3420 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3424 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDrawAspect */

/* 3426 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3428 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 3432 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3434 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ptd */

/* 3438 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3440 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3442 */	NdrFcShort( 0x3fe ),	/* Type Offset=1022 */

	/* Parameter lpsizel */

/* 3444 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3446 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3448 */	NdrFcShort( 0x2ae ),	/* Type Offset=686 */

	/* Return value */

/* 3450 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3452 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DragEnter */

/* 3456 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3458 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3462 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3464 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3466 */	NdrFcShort( 0x3c ),	/* 60 */
/* 3468 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3470 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3472 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3480 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObj */

/* 3482 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3484 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3486 */	NdrFcShort( 0x76 ),	/* Type Offset=118 */

	/* Parameter grfKeyState */

/* 3488 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3490 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3492 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pt */

/* 3494 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3496 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3498 */	NdrFcShort( 0x2ae ),	/* Type Offset=686 */

	/* Parameter pdwEffect */

/* 3500 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3502 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3506 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3508 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DragOver */

/* 3512 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3514 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3518 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3520 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3522 */	NdrFcShort( 0x3c ),	/* 60 */
/* 3524 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3526 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3528 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3536 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfKeyState */

/* 3538 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3540 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pt */

/* 3544 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3546 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3548 */	NdrFcShort( 0x2ae ),	/* Type Offset=686 */

	/* Parameter pdwEffect */

/* 3550 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3552 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3558 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Drop */

/* 3562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3568 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3570 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3572 */	NdrFcShort( 0x3c ),	/* 60 */
/* 3574 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3576 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3578 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDataObj */

/* 3588 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3590 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3592 */	NdrFcShort( 0x76 ),	/* Type Offset=118 */

	/* Parameter grfKeyState */

/* 3594 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3596 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pt */

/* 3600 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3602 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3604 */	NdrFcShort( 0x2ae ),	/* Type Offset=686 */

	/* Parameter pdwEffect */

/* 3606 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3608 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3612 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3614 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 3618 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3620 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3624 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3626 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3628 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3630 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3632 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 3634 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3636 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3642 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 3644 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3646 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 3650 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3652 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3654 */	NdrFcShort( 0x43a ),	/* Type Offset=1082 */

	/* Parameter pceltFetched */

/* 3656 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3658 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3662 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3664 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3666 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 3668 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3670 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3674 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3676 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3680 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3682 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3684 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3692 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 3694 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3696 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3698 */	NdrFcShort( 0x2dc ),	/* Type Offset=732 */

	/* Return value */

/* 3700 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3702 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3704 */	0x8,		/* FC_LONG */
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
			0x12, 0x0,	/* FC_UP */
/*  4 */	NdrFcShort( 0x40 ),	/* Offset= 64 (68) */
/*  6 */	
			0x12, 0x0,	/* FC_UP */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	NdrFcShort( 0x2 ),	/* 2 */
/* 16 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 20 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 22 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 26 */	NdrFcShort( 0x4 ),	/* Offset= 4 (30) */
/* 28 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (27) */
/* 30 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 32 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 34 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	NdrFcShort( 0x2 ),	/* 2 */
/* 40 */	NdrFcShort( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (6) */
/* 44 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 46 */	NdrFcShort( 0x1 ),	/* 1 */
/* 48 */	0x0,		/* Corr desc:  */
			0x59,		/* FC_CALLBACK */
/* 50 */	NdrFcShort( 0x0 ),	/* 0 */
/* 52 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 54 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 56 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 58 */	NdrFcShort( 0xc ),	/* 12 */
/* 60 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (44) */
/* 62 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 64 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 66 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 68 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 70 */	NdrFcShort( 0x20 ),	/* 32 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0xe ),	/* Offset= 14 (88) */
/* 76 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 78 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (34) */
/* 80 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 82 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 84 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 86 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 88 */	
			0x12, 0x0,	/* FC_UP */
/* 90 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (56) */
/* 92 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 94 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 96 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 98 */	NdrFcShort( 0x2 ),	/* Offset= 2 (100) */
/* 100 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 102 */	NdrFcLong( 0x105 ),	/* 261 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 110 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 112 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 114 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 116 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 118 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 120 */	NdrFcLong( 0x10e ),	/* 270 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 128 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 130 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 132 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 134 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 136 */	
			0x12, 0x0,	/* FC_UP */
/* 138 */	NdrFcShort( 0x182 ),	/* Offset= 386 (524) */
/* 140 */	
			0x12, 0x0,	/* FC_UP */
/* 142 */	NdrFcShort( 0x16c ),	/* Offset= 364 (506) */
/* 144 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 150 */	NdrFcLong( 0x0 ),	/* 0 */
/* 154 */	NdrFcShort( 0x0 ),	/* Offset= 0 (154) */
/* 156 */	NdrFcLong( 0x20 ),	/* 32 */
/* 160 */	NdrFcShort( 0x28 ),	/* Offset= 40 (200) */
/* 162 */	NdrFcLong( 0x40 ),	/* 64 */
/* 166 */	NdrFcShort( 0x7c ),	/* Offset= 124 (290) */
/* 168 */	NdrFcLong( 0x10 ),	/* 16 */
/* 172 */	NdrFcShort( 0x8e ),	/* Offset= 142 (314) */
/* 174 */	NdrFcLong( 0x1 ),	/* 1 */
/* 178 */	NdrFcShort( 0x10a ),	/* Offset= 266 (444) */
/* 180 */	NdrFcLong( 0x2 ),	/* 2 */
/* 184 */	NdrFcShort( 0xffffff66 ),	/* Offset= -154 (30) */
/* 186 */	NdrFcLong( 0x4 ),	/* 4 */
/* 190 */	NdrFcShort( 0x3a ),	/* Offset= 58 (248) */
/* 192 */	NdrFcLong( 0x8 ),	/* 8 */
/* 196 */	NdrFcShort( 0x34 ),	/* Offset= 52 (248) */
/* 198 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (197) */
/* 200 */	
			0x12, 0x0,	/* FC_UP */
/* 202 */	NdrFcShort( 0x2 ),	/* Offset= 2 (204) */
/* 204 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 206 */	NdrFcShort( 0x8 ),	/* 8 */
/* 208 */	NdrFcShort( 0x2 ),	/* 2 */
/* 210 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 214 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 216 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 220 */	NdrFcShort( 0x4 ),	/* Offset= 4 (224) */
/* 222 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (221) */
/* 224 */	
			0x12, 0x0,	/* FC_UP */
/* 226 */	NdrFcShort( 0x2e ),	/* Offset= 46 (272) */
/* 228 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 232 */	NdrFcShort( 0x2 ),	/* 2 */
/* 234 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 238 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 240 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 244 */	NdrFcShort( 0x4 ),	/* Offset= 4 (248) */
/* 246 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (245) */
/* 248 */	
			0x12, 0x0,	/* FC_UP */
/* 250 */	NdrFcShort( 0xe ),	/* Offset= 14 (264) */
/* 252 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 254 */	NdrFcShort( 0x1 ),	/* 1 */
/* 256 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 258 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 260 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 262 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 264 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 266 */	NdrFcShort( 0x4 ),	/* 4 */
/* 268 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (252) */
/* 270 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 272 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 274 */	NdrFcShort( 0x18 ),	/* 24 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x8 ),	/* Offset= 8 (286) */
/* 280 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 282 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 284 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 286 */	
			0x12, 0x0,	/* FC_UP */
/* 288 */	NdrFcShort( 0xffffffc4 ),	/* Offset= -60 (228) */
/* 290 */	
			0x12, 0x0,	/* FC_UP */
/* 292 */	NdrFcShort( 0x2 ),	/* Offset= 2 (294) */
/* 294 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 298 */	NdrFcShort( 0x2 ),	/* 2 */
/* 300 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 304 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 306 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 310 */	NdrFcShort( 0xffffffc2 ),	/* Offset= -62 (248) */
/* 312 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (311) */
/* 314 */	
			0x12, 0x0,	/* FC_UP */
/* 316 */	NdrFcShort( 0x2 ),	/* Offset= 2 (318) */
/* 318 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 320 */	NdrFcShort( 0x8 ),	/* 8 */
/* 322 */	NdrFcShort( 0x2 ),	/* 2 */
/* 324 */	NdrFcLong( 0x7 ),	/* 7 */
/* 328 */	NdrFcShort( 0xa ),	/* Offset= 10 (338) */
/* 330 */	NdrFcLong( 0x5 ),	/* 5 */
/* 334 */	NdrFcShort( 0x2e ),	/* Offset= 46 (380) */
/* 336 */	NdrFcShort( 0x6c ),	/* Offset= 108 (444) */
/* 338 */	
			0x12, 0x0,	/* FC_UP */
/* 340 */	NdrFcShort( 0x2 ),	/* Offset= 2 (342) */
/* 342 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 344 */	NdrFcShort( 0x8 ),	/* 8 */
/* 346 */	NdrFcShort( 0x2 ),	/* 2 */
/* 348 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 352 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 354 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 358 */	NdrFcShort( 0x4 ),	/* Offset= 4 (362) */
/* 360 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (359) */
/* 362 */	
			0x12, 0x0,	/* FC_UP */
/* 364 */	NdrFcShort( 0x2 ),	/* Offset= 2 (366) */
/* 366 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 368 */	NdrFcShort( 0x18 ),	/* 24 */
/* 370 */	NdrFcShort( 0xffffff8a ),	/* Offset= -118 (252) */
/* 372 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 374 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 376 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 378 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 380 */	
			0x12, 0x0,	/* FC_UP */
/* 382 */	NdrFcShort( 0x2 ),	/* Offset= 2 (384) */
/* 384 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 386 */	NdrFcShort( 0x8 ),	/* 8 */
/* 388 */	NdrFcShort( 0x2 ),	/* 2 */
/* 390 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 394 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 396 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 400 */	NdrFcShort( 0x4 ),	/* Offset= 4 (404) */
/* 402 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (401) */
/* 404 */	
			0x12, 0x0,	/* FC_UP */
/* 406 */	NdrFcShort( 0x1c ),	/* Offset= 28 (434) */
/* 408 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 410 */	NdrFcShort( 0x4 ),	/* 4 */
/* 412 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 414 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 416 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 418 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 420 */	NdrFcShort( 0x4 ),	/* 4 */
/* 422 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 424 */	NdrFcShort( 0xfffe ),	/* -2 */
/* 426 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 428 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 430 */	NdrFcShort( 0xffffffea ),	/* Offset= -22 (408) */
/* 432 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 434 */	
			0x17,		/* FC_CSTRUCT */
			0x1,		/* 1 */
/* 436 */	NdrFcShort( 0x4 ),	/* 4 */
/* 438 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (418) */
/* 440 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 442 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 444 */	
			0x12, 0x0,	/* FC_UP */
/* 446 */	NdrFcShort( 0x2 ),	/* Offset= 2 (448) */
/* 448 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 450 */	NdrFcShort( 0x8 ),	/* 8 */
/* 452 */	NdrFcShort( 0x3 ),	/* 3 */
/* 454 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 458 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 460 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 464 */	NdrFcShort( 0xa ),	/* Offset= 10 (474) */
/* 466 */	NdrFcLong( 0x50746457 ),	/* 1349805143 */
/* 470 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 472 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (471) */
/* 474 */	
			0x12, 0x0,	/* FC_UP */
/* 476 */	NdrFcShort( 0x2 ),	/* Offset= 2 (478) */
/* 478 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 480 */	NdrFcShort( 0x8 ),	/* 8 */
/* 482 */	NdrFcShort( 0xffffff1a ),	/* Offset= -230 (252) */
/* 484 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 486 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 488 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 490 */	NdrFcLong( 0x0 ),	/* 0 */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 500 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 502 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 504 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 506 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 508 */	NdrFcShort( 0x18 ),	/* 24 */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0xc ),	/* Offset= 12 (524) */
/* 514 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 516 */	NdrFcShort( 0xfffffe8c ),	/* Offset= -372 (144) */
/* 518 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 520 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (488) */
/* 522 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 524 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 526 */	NdrFcShort( 0x1 ),	/* 1 */
/* 528 */	NdrFcShort( 0x18 ),	/* 24 */
/* 530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 532 */	NdrFcShort( 0xfffffe78 ),	/* Offset= -392 (140) */
/* 534 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 536 */	NdrFcLong( 0xe ),	/* 14 */
/* 540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 544 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 546 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 548 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 550 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 552 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 554 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 556 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 558 */	NdrFcShort( 0x2 ),	/* Offset= 2 (560) */
/* 560 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 562 */	NdrFcLong( 0xf ),	/* 15 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 570 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 572 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 574 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 576 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 578 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 580 */	NdrFcShort( 0x2 ),	/* Offset= 2 (582) */
/* 582 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 584 */	NdrFcLong( 0x100 ),	/* 256 */
/* 588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 594 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 596 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 598 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 600 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 602 */	NdrFcShort( 0x2 ),	/* Offset= 2 (604) */
/* 604 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 606 */	NdrFcLong( 0x11b ),	/* 283 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 614 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 616 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 618 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 620 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 622 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 624 */	NdrFcLong( 0x118 ),	/* 280 */
/* 628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 632 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 634 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 636 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 638 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 640 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 642 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (622) */
/* 644 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 646 */	NdrFcShort( 0xfffffdf0 ),	/* Offset= -528 (118) */
/* 648 */	
			0x12, 0x0,	/* FC_UP */
/* 650 */	NdrFcShort( 0x2c ),	/* Offset= 44 (694) */
/* 652 */	
			0x12, 0x0,	/* FC_UP */
/* 654 */	NdrFcShort( 0x2 ),	/* Offset= 2 (656) */
/* 656 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 658 */	NdrFcShort( 0x4 ),	/* 4 */
/* 660 */	NdrFcShort( 0x2 ),	/* 2 */
/* 662 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 666 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 668 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 672 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 674 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (673) */
/* 676 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 678 */	NdrFcShort( 0x2 ),	/* 2 */
/* 680 */	NdrFcShort( 0x8 ),	/* 8 */
/* 682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (652) */
/* 686 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 690 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 692 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 694 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 696 */	NdrFcShort( 0x30 ),	/* 48 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x0 ),	/* Offset= 0 (700) */
/* 702 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 704 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (676) */
/* 706 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 708 */	0xb9,		/* FC_UINT3264 */
			0xb8,		/* FC_INT3264 */
/* 710 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 712 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffe5 ),	/* Offset= -27 (686) */
			0x40,		/* FC_STRUCTPAD4 */
/* 716 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 718 */	
			0x12, 0x0,	/* FC_UP */
/* 720 */	NdrFcShort( 0x2 ),	/* Offset= 2 (722) */
/* 722 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 724 */	NdrFcShort( 0x10 ),	/* 16 */
/* 726 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 728 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 730 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 732 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 734 */	NdrFcShort( 0x2 ),	/* Offset= 2 (736) */
/* 736 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 738 */	NdrFcLong( 0x104 ),	/* 260 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 746 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 748 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 750 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 752 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 754 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 756 */	NdrFcShort( 0x8 ),	/* Offset= 8 (764) */
/* 758 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 762 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 764 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 766 */	NdrFcShort( 0x10 ),	/* 16 */
/* 768 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 770 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 772 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (758) */
			0x5b,		/* FC_END */
/* 776 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 778 */	NdrFcShort( 0x2 ),	/* Offset= 2 (780) */
/* 780 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 782 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 784 */	
			0x11, 0x0,	/* FC_RP */
/* 786 */	NdrFcShort( 0xffffff9c ),	/* Offset= -100 (686) */
/* 788 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 790 */	NdrFcShort( 0xffffff98 ),	/* Offset= -104 (686) */
/* 792 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 794 */	NdrFcLong( 0x10f ),	/* 271 */
/* 798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 802 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 804 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 806 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 808 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 810 */	
			0x11, 0x0,	/* FC_RP */
/* 812 */	NdrFcShort( 0xfffffe86 ),	/* Offset= -378 (434) */
/* 814 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 816 */	NdrFcShort( 0x6 ),	/* Offset= 6 (822) */
/* 818 */	
			0x13, 0x0,	/* FC_OP */
/* 820 */	NdrFcShort( 0xffffff5c ),	/* Offset= -164 (656) */
/* 822 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 824 */	NdrFcShort( 0x2 ),	/* 2 */
/* 826 */	NdrFcShort( 0x8 ),	/* 8 */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (818) */
/* 832 */	
			0x11, 0x0,	/* FC_RP */
/* 834 */	NdrFcShort( 0xffffffba ),	/* Offset= -70 (764) */
/* 836 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 838 */	NdrFcShort( 0xfffffea2 ),	/* Offset= -350 (488) */
/* 840 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 842 */	NdrFcShort( 0x2 ),	/* Offset= 2 (844) */
/* 844 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 846 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 848 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 850 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 852 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 854 */	NdrFcShort( 0x2 ),	/* Offset= 2 (856) */
/* 856 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 858 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 860 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 862 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 864 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 866 */	NdrFcShort( 0xffffff70 ),	/* Offset= -144 (722) */
/* 868 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 870 */	NdrFcLong( 0x117 ),	/* 279 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 880 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 882 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 884 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 886 */	
			0x11, 0x0,	/* FC_RP */
/* 888 */	NdrFcShort( 0xffffff5a ),	/* Offset= -166 (722) */
/* 890 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 892 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 894 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 896 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 898 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 900 */	NdrFcShort( 0x3 ),	/* 3 */
/* 902 */	NdrFcShort( 0x8 ),	/* 8 */
/* 904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0xffffff02 ),	/* Offset= -254 (652) */
/* 908 */	
			0x11, 0x0,	/* FC_RP */
/* 910 */	NdrFcShort( 0x8 ),	/* Offset= 8 (918) */
/* 912 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 914 */	NdrFcShort( 0x18 ),	/* 24 */
/* 916 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 918 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 920 */	NdrFcShort( 0x18 ),	/* 24 */
/* 922 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 924 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (912) */
/* 926 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 928 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 930 */	NdrFcShort( 0x4 ),	/* 4 */
/* 932 */	NdrFcShort( 0x8 ),	/* 8 */
/* 934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 936 */	NdrFcShort( 0xfffffe14 ),	/* Offset= -492 (444) */
/* 938 */	
			0x11, 0x0,	/* FC_RP */
/* 940 */	NdrFcShort( 0xffffff0a ),	/* Offset= -246 (694) */
/* 942 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 944 */	NdrFcShort( 0x2 ),	/* Offset= 2 (946) */
/* 946 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 948 */	NdrFcLong( 0x116 ),	/* 278 */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 958 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 960 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 962 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 964 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 966 */	NdrFcShort( 0x2 ),	/* Offset= 2 (968) */
/* 968 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 970 */	NdrFcLong( 0x115 ),	/* 277 */
/* 974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 976 */	NdrFcShort( 0x0 ),	/* 0 */
/* 978 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 980 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 982 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 984 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 986 */	
			0x11, 0x0,	/* FC_RP */
/* 988 */	NdrFcShort( 0xc ),	/* Offset= 12 (1000) */
/* 990 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 992 */	NdrFcShort( 0x5 ),	/* 5 */
/* 994 */	NdrFcShort( 0x8 ),	/* 8 */
/* 996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 998 */	NdrFcShort( 0xffffff4c ),	/* Offset= -180 (818) */
/* 1000 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1002 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1006) */
/* 1008 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1010 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1012 */	NdrFcShort( 0xffffff42 ),	/* Offset= -190 (822) */
/* 1014 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1016 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (990) */
/* 1018 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1020 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1022 */	
			0x12, 0x0,	/* FC_UP */
/* 1024 */	NdrFcShort( 0xfffffc38 ),	/* Offset= -968 (56) */
/* 1026 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1028 */	NdrFcLong( 0x12a ),	/* 298 */
/* 1032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1036 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1038 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1040 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1042 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1044 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1046 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1048) */
/* 1048 */	
			0x13, 0x0,	/* FC_OP */
/* 1050 */	NdrFcShort( 0xfffffd98 ),	/* Offset= -616 (434) */
/* 1052 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1054 */	NdrFcShort( 0xfffffefa ),	/* Offset= -262 (792) */
/* 1056 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1058 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1060 */	
			0x11, 0x0,	/* FC_RP */
/* 1062 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1082) */
/* 1064 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1066 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1070 */	NdrFcShort( 0x8 ),	/* Offset= 8 (1078) */
/* 1072 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1074 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 1076 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1078 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1080 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1082 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1086 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1088 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1090 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1092 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1094 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1096 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1098 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1100 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1064) */
/* 1102 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            CLIPFORMAT_UserSize
            ,CLIPFORMAT_UserMarshal
            ,CLIPFORMAT_UserUnmarshal
            ,CLIPFORMAT_UserFree
            },
            {
            STGMEDIUM_UserSize
            ,STGMEDIUM_UserMarshal
            ,STGMEDIUM_UserUnmarshal
            ,STGMEDIUM_UserFree
            },
            {
            HWND_UserSize
            ,HWND_UserMarshal
            ,HWND_UserUnmarshal
            ,HWND_UserFree
            },
            {
            HMENU_UserSize
            ,HMENU_UserMarshal
            ,HMENU_UserUnmarshal
            ,HMENU_UserFree
            },
            {
            HGLOBAL_UserSize
            ,HGLOBAL_UserMarshal
            ,HGLOBAL_UserUnmarshal
            ,HGLOBAL_UserFree
            },
            {
            HACCEL_UserSize
            ,HACCEL_UserMarshal
            ,HACCEL_UserUnmarshal
            ,HACCEL_UserFree
            }

        };


static void __RPC_USER IOleCache_DVTARGETDEVICEExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    DVTARGETDEVICE *pS	=	( DVTARGETDEVICE * )(pStubMsg->StackTop - 12);
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = (ULONG_PTR) ( pS->tdSize - sizeof( DWORD  ) - 4 * sizeof( WORD  ) );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IOleCache_DVTARGETDEVICEExprEval_0000
    };



/* Standard interface: __MIDL_itf_oleidl_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOleAdviseHolder, ver. 0.0,
   GUID={0x00000111,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOleCache, ver. 0.0,
   GUID={0x0000011e,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleCache_FormatStringOffsetTable[] =
    {
    0,
    50,
    88,
    126,
    164
    };

static const MIDL_STUBLESS_PROXY_INFO IOleCache_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleCache_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleCache_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleCache_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IOleCacheProxyVtbl = 
{
    &IOleCache_ProxyInfo,
    &IID_IOleCache,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleCache::Cache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::Uncache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::EnumCache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::InitCache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::SetData */
};

const CInterfaceStubVtbl _IOleCacheStubVtbl =
{
    &IID_IOleCache,
    &IOleCache_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleCache2, ver. 0.0,
   GUID={0x00000128,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleCache2_FormatStringOffsetTable[] =
    {
    0,
    50,
    88,
    126,
    164,
    214,
    264
    };

static const MIDL_STUBLESS_PROXY_INFO IOleCache2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleCache2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IOleCache2_StubThunkTable[] = 
    {
    0,
    0,
    0,
    0,
    0,
    IOleCache2_RemoteUpdateCache_Thunk,
    0
    };

static const MIDL_SERVER_INFO IOleCache2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleCache2_FormatStringOffsetTable[-3],
    &IOleCache2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IOleCache2ProxyVtbl = 
{
    &IOleCache2_ProxyInfo,
    &IID_IOleCache2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleCache::Cache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::Uncache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::EnumCache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::InitCache */ ,
    (void *) (INT_PTR) -1 /* IOleCache::SetData */ ,
    IOleCache2_UpdateCache_Proxy ,
    (void *) (INT_PTR) -1 /* IOleCache2::DiscardCache */
};

const CInterfaceStubVtbl _IOleCache2StubVtbl =
{
    &IID_IOleCache2,
    &IOleCache2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleCacheControl, ver. 0.0,
   GUID={0x00000129,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleCacheControl_FormatStringOffsetTable[] =
    {
    302,
    340
    };

static const MIDL_STUBLESS_PROXY_INFO IOleCacheControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleCacheControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleCacheControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleCacheControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IOleCacheControlProxyVtbl = 
{
    &IOleCacheControl_ProxyInfo,
    &IID_IOleCacheControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleCacheControl::OnRun */ ,
    (void *) (INT_PTR) -1 /* IOleCacheControl::OnStop */
};

const CInterfaceStubVtbl _IOleCacheControlStubVtbl =
{
    &IID_IOleCacheControl,
    &IOleCacheControl_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IParseDisplayName, ver. 0.0,
   GUID={0x0000011a,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IParseDisplayName_FormatStringOffsetTable[] =
    {
    372
    };

static const MIDL_STUBLESS_PROXY_INFO IParseDisplayName_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IParseDisplayName_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IParseDisplayName_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IParseDisplayName_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IParseDisplayNameProxyVtbl = 
{
    &IParseDisplayName_ProxyInfo,
    &IID_IParseDisplayName,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IParseDisplayName::ParseDisplayName */
};

const CInterfaceStubVtbl _IParseDisplayNameStubVtbl =
{
    &IID_IParseDisplayName,
    &IParseDisplayName_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleContainer, ver. 0.0,
   GUID={0x0000011b,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleContainer_FormatStringOffsetTable[] =
    {
    372,
    428,
    472
    };

static const MIDL_STUBLESS_PROXY_INFO IOleContainer_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleContainer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IOleContainerProxyVtbl = 
{
    &IOleContainer_ProxyInfo,
    &IID_IOleContainer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IParseDisplayName::ParseDisplayName */ ,
    (void *) (INT_PTR) -1 /* IOleContainer::EnumObjects */ ,
    (void *) (INT_PTR) -1 /* IOleContainer::LockContainer */
};

const CInterfaceStubVtbl _IOleContainerStubVtbl =
{
    &IID_IOleContainer,
    &IOleContainer_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleClientSite, ver. 0.0,
   GUID={0x00000118,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleClientSite_FormatStringOffsetTable[] =
    {
    510,
    542,
    592,
    630,
    662,
    700
    };

static const MIDL_STUBLESS_PROXY_INFO IOleClientSite_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleClientSite_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleClientSite_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleClientSite_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IOleClientSiteProxyVtbl = 
{
    &IOleClientSite_ProxyInfo,
    &IID_IOleClientSite,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleClientSite::SaveObject */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::GetMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::GetContainer */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::ShowObject */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::OnShowWindow */ ,
    (void *) (INT_PTR) -1 /* IOleClientSite::RequestNewObjectLayout */
};

const CInterfaceStubVtbl _IOleClientSiteStubVtbl =
{
    &IID_IOleClientSite,
    &IOleClientSite_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleObject, ver. 0.0,
   GUID={0x00000112,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleObject_FormatStringOffsetTable[] =
    {
    732,
    770,
    808,
    852,
    890,
    934,
    984,
    1034,
    1078,
    1146,
    1184,
    1216,
    1248,
    1286,
    1330,
    1374,
    1418,
    1462,
    1500,
    1538,
    1582
    };

static const MIDL_STUBLESS_PROXY_INFO IOleObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleObject_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(24) _IOleObjectProxyVtbl = 
{
    &IOleObject_ProxyInfo,
    &IID_IOleObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleObject::SetClientSite */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetClientSite */ ,
    (void *) (INT_PTR) -1 /* IOleObject::SetHostNames */ ,
    (void *) (INT_PTR) -1 /* IOleObject::Close */ ,
    (void *) (INT_PTR) -1 /* IOleObject::SetMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleObject::InitFromData */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetClipboardData */ ,
    (void *) (INT_PTR) -1 /* IOleObject::DoVerb */ ,
    (void *) (INT_PTR) -1 /* IOleObject::EnumVerbs */ ,
    (void *) (INT_PTR) -1 /* IOleObject::Update */ ,
    (void *) (INT_PTR) -1 /* IOleObject::IsUpToDate */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetUserClassID */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetUserType */ ,
    (void *) (INT_PTR) -1 /* IOleObject::SetExtent */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetExtent */ ,
    (void *) (INT_PTR) -1 /* IOleObject::Advise */ ,
    (void *) (INT_PTR) -1 /* IOleObject::Unadvise */ ,
    (void *) (INT_PTR) -1 /* IOleObject::EnumAdvise */ ,
    (void *) (INT_PTR) -1 /* IOleObject::GetMiscStatus */ ,
    (void *) (INT_PTR) -1 /* IOleObject::SetColorScheme */
};

const CInterfaceStubVtbl _IOleObjectStubVtbl =
{
    &IID_IOleObject,
    &IOleObject_ServerInfo,
    24,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: IOLETypes, ver. 0.0,
   GUID={0xB0916C84,0x7416,0x101A,{0xBC,0xEA,0x08,0x00,0x2B,0x2B,0x79,0xEF}} */


/* Object interface: IOleWindow, ver. 0.0,
   GUID={0x00000114,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleWindow_FormatStringOffsetTable[] =
    {
    1620,
    50
    };

static const MIDL_STUBLESS_PROXY_INFO IOleWindow_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleWindow_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IOleWindowProxyVtbl = 
{
    &IOleWindow_ProxyInfo,
    &IID_IOleWindow,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */
};

const CInterfaceStubVtbl _IOleWindowStubVtbl =
{
    &IID_IOleWindow,
    &IOleWindow_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleLink, ver. 0.0,
   GUID={0x0000011d,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleLink_FormatStringOffsetTable[] =
    {
    1658,
    1696,
    1734,
    1778,
    1816,
    1854,
    1892,
    1936,
    1968,
    2006,
    2038
    };

static const MIDL_STUBLESS_PROXY_INFO IOleLink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleLink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleLink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleLink_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IOleLinkProxyVtbl = 
{
    &IOleLink_ProxyInfo,
    &IID_IOleLink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleLink::SetUpdateOptions */ ,
    (void *) (INT_PTR) -1 /* IOleLink::GetUpdateOptions */ ,
    (void *) (INT_PTR) -1 /* IOleLink::SetSourceMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleLink::GetSourceMoniker */ ,
    (void *) (INT_PTR) -1 /* IOleLink::SetSourceDisplayName */ ,
    (void *) (INT_PTR) -1 /* IOleLink::GetSourceDisplayName */ ,
    (void *) (INT_PTR) -1 /* IOleLink::BindToSource */ ,
    (void *) (INT_PTR) -1 /* IOleLink::BindIfRunning */ ,
    (void *) (INT_PTR) -1 /* IOleLink::GetBoundSource */ ,
    (void *) (INT_PTR) -1 /* IOleLink::UnbindSource */ ,
    (void *) (INT_PTR) -1 /* IOleLink::Update */
};

const CInterfaceStubVtbl _IOleLinkStubVtbl =
{
    &IID_IOleLink,
    &IOleLink_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleItemContainer, ver. 0.0,
   GUID={0x0000011c,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleItemContainer_FormatStringOffsetTable[] =
    {
    372,
    428,
    472,
    2076,
    2138,
    2194
    };

static const MIDL_STUBLESS_PROXY_INFO IOleItemContainer_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleItemContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleItemContainer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleItemContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IOleItemContainerProxyVtbl = 
{
    &IOleItemContainer_ProxyInfo,
    &IID_IOleItemContainer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IParseDisplayName::ParseDisplayName */ ,
    (void *) (INT_PTR) -1 /* IOleContainer::EnumObjects */ ,
    (void *) (INT_PTR) -1 /* IOleContainer::LockContainer */ ,
    (void *) (INT_PTR) -1 /* IOleItemContainer::GetObject */ ,
    (void *) (INT_PTR) -1 /* IOleItemContainer::GetObjectStorage */ ,
    (void *) (INT_PTR) -1 /* IOleItemContainer::IsRunning */
};

const CInterfaceStubVtbl _IOleItemContainerStubVtbl =
{
    &IID_IOleItemContainer,
    &IOleItemContainer_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceUIWindow, ver. 0.0,
   GUID={0x00000115,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceUIWindow_FormatStringOffsetTable[] =
    {
    1620,
    50,
    2232,
    2270,
    2308,
    2346
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceUIWindow_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceUIWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleInPlaceUIWindow_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceUIWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IOleInPlaceUIWindowProxyVtbl = 
{
    &IOleInPlaceUIWindow_ProxyInfo,
    &IID_IOleInPlaceUIWindow,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::GetBorder */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::RequestBorderSpace */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::SetBorderSpace */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::SetActiveObject */
};

const CInterfaceStubVtbl _IOleInPlaceUIWindowStubVtbl =
{
    &IID_IOleInPlaceUIWindow,
    &IOleInPlaceUIWindow_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceActiveObject, ver. 0.0,
   GUID={0x00000117,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceActiveObject_FormatStringOffsetTable[] =
    {
    1620,
    50,
    2390,
    2422,
    2460,
    2498,
    264
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceActiveObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceActiveObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IOleInPlaceActiveObject_StubThunkTable[] = 
    {
    0,
    0,
    IOleInPlaceActiveObject_RemoteTranslateAccelerator_Thunk,
    0,
    0,
    IOleInPlaceActiveObject_RemoteResizeBorder_Thunk,
    0
    };

static const MIDL_SERVER_INFO IOleInPlaceActiveObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceActiveObject_FormatStringOffsetTable[-3],
    &IOleInPlaceActiveObject_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IOleInPlaceActiveObjectProxyVtbl = 
{
    &IOleInPlaceActiveObject_ProxyInfo,
    &IID_IOleInPlaceActiveObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    IOleInPlaceActiveObject_TranslateAccelerator_Proxy ,
    (void *) (INT_PTR) -1 /* IOleInPlaceActiveObject::OnFrameWindowActivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceActiveObject::OnDocWindowActivate */ ,
    IOleInPlaceActiveObject_ResizeBorder_Proxy ,
    (void *) (INT_PTR) -1 /* IOleInPlaceActiveObject::EnableModeless */
};

const CInterfaceStubVtbl _IOleInPlaceActiveObjectStubVtbl =
{
    &IID_IOleInPlaceActiveObject,
    &IOleInPlaceActiveObject_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceFrame, ver. 0.0,
   GUID={0x00000116,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceFrame_FormatStringOffsetTable[] =
    {
    1620,
    50,
    2232,
    2270,
    2308,
    2346,
    2554,
    2598,
    2648,
    2686,
    2724,
    2762
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceFrame_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceFrame_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleInPlaceFrame_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceFrame_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _IOleInPlaceFrameProxyVtbl = 
{
    &IOleInPlaceFrame_ProxyInfo,
    &IID_IOleInPlaceFrame,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::GetBorder */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::RequestBorderSpace */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::SetBorderSpace */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceUIWindow::SetActiveObject */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::InsertMenus */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::SetMenu */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::RemoveMenus */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::SetStatusText */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::EnableModeless */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceFrame::TranslateAccelerator */
};

const CInterfaceStubVtbl _IOleInPlaceFrameStubVtbl =
{
    &IID_IOleInPlaceFrame,
    &IOleInPlaceFrame_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceObject, ver. 0.0,
   GUID={0x00000113,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceObject_FormatStringOffsetTable[] =
    {
    1620,
    50,
    2390,
    630,
    2806,
    700
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleInPlaceObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceObject_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IOleInPlaceObjectProxyVtbl = 
{
    &IOleInPlaceObject_ProxyInfo,
    &IID_IOleInPlaceObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceObject::InPlaceDeactivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceObject::UIDeactivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceObject::SetObjectRects */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceObject::ReactivateAndUndo */
};

const CInterfaceStubVtbl _IOleInPlaceObjectStubVtbl =
{
    &IID_IOleInPlaceObject,
    &IOleInPlaceObject_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOleInPlaceSite, ver. 0.0,
   GUID={0x00000119,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IOleInPlaceSite_FormatStringOffsetTable[] =
    {
    1620,
    50,
    2390,
    630,
    2850,
    2882,
    2944,
    2982,
    3020,
    2006,
    1184,
    3052
    };

static const MIDL_STUBLESS_PROXY_INFO IOleInPlaceSite_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceSite_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOleInPlaceSite_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOleInPlaceSite_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _IOleInPlaceSiteProxyVtbl = 
{
    &IOleInPlaceSite_ProxyInfo,
    &IID_IOleInPlaceSite,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOleWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IOleWindow::ContextSensitiveHelp */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::CanInPlaceActivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnInPlaceActivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnUIActivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::GetWindowContext */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::Scroll */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnUIDeactivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnInPlaceDeactivate */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::DiscardUndoState */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::DeactivateAndUndo */ ,
    (void *) (INT_PTR) -1 /* IOleInPlaceSite::OnPosRectChange */
};

const CInterfaceStubVtbl _IOleInPlaceSiteStubVtbl =
{
    &IID_IOleInPlaceSite,
    &IOleInPlaceSite_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IContinue, ver. 0.0,
   GUID={0x0000012a,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IContinue_FormatStringOffsetTable[] =
    {
    510
    };

static const MIDL_STUBLESS_PROXY_INFO IContinue_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IContinue_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IContinue_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IContinue_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IContinueProxyVtbl = 
{
    &IContinue_ProxyInfo,
    &IID_IContinue,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IContinue::FContinue */
};

const CInterfaceStubVtbl _IContinueStubVtbl =
{
    &IID_IContinue,
    &IContinue_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IViewObject, ver. 0.0,
   GUID={0x0000010d,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IViewObject_FormatStringOffsetTable[] =
    {
    3090,
    3176,
    3244,
    852,
    3300,
    3350
    };

static const MIDL_STUBLESS_PROXY_INFO IViewObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IViewObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IViewObject_StubThunkTable[] = 
    {
    IViewObject_RemoteDraw_Thunk,
    IViewObject_RemoteGetColorSet_Thunk,
    IViewObject_RemoteFreeze_Thunk,
    0,
    0,
    IViewObject_RemoteGetAdvise_Thunk
    };

static const MIDL_SERVER_INFO IViewObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IViewObject_FormatStringOffsetTable[-3],
    &IViewObject_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IViewObjectProxyVtbl = 
{
    &IViewObject_ProxyInfo,
    &IID_IViewObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IViewObject_Draw_Proxy ,
    IViewObject_GetColorSet_Proxy ,
    IViewObject_Freeze_Proxy ,
    (void *) (INT_PTR) -1 /* IViewObject::Unfreeze */ ,
    (void *) (INT_PTR) -1 /* IViewObject::SetAdvise */ ,
    IViewObject_GetAdvise_Proxy
};

const CInterfaceStubVtbl _IViewObjectStubVtbl =
{
    &IID_IViewObject,
    &IViewObject_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IViewObject2, ver. 0.0,
   GUID={0x00000127,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IViewObject2_FormatStringOffsetTable[] =
    {
    3090,
    3176,
    3244,
    852,
    3300,
    3350,
    3400
    };

static const MIDL_STUBLESS_PROXY_INFO IViewObject2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IViewObject2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IViewObject2_StubThunkTable[] = 
    {
    IViewObject_RemoteDraw_Thunk,
    IViewObject_RemoteGetColorSet_Thunk,
    IViewObject_RemoteFreeze_Thunk,
    0,
    0,
    IViewObject_RemoteGetAdvise_Thunk,
    0
    };

static const MIDL_SERVER_INFO IViewObject2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IViewObject2_FormatStringOffsetTable[-3],
    &IViewObject2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IViewObject2ProxyVtbl = 
{
    &IViewObject2_ProxyInfo,
    &IID_IViewObject2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IViewObject_Draw_Proxy ,
    IViewObject_GetColorSet_Proxy ,
    IViewObject_Freeze_Proxy ,
    (void *) (INT_PTR) -1 /* IViewObject::Unfreeze */ ,
    (void *) (INT_PTR) -1 /* IViewObject::SetAdvise */ ,
    IViewObject_GetAdvise_Proxy ,
    (void *) (INT_PTR) -1 /* IViewObject2::GetExtent */
};

const CInterfaceStubVtbl _IViewObject2StubVtbl =
{
    &IID_IViewObject2,
    &IViewObject2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDropSource, ver. 0.0,
   GUID={0x00000121,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDropTarget, ver. 0.0,
   GUID={0x00000122,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IDropTarget_FormatStringOffsetTable[] =
    {
    3456,
    3512,
    2390,
    3562
    };

static const MIDL_STUBLESS_PROXY_INFO IDropTarget_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDropTarget_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDropTarget_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDropTarget_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDropTargetProxyVtbl = 
{
    &IDropTarget_ProxyInfo,
    &IID_IDropTarget,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDropTarget::DragEnter */ ,
    (void *) (INT_PTR) -1 /* IDropTarget::DragOver */ ,
    (void *) (INT_PTR) -1 /* IDropTarget::DragLeave */ ,
    (void *) (INT_PTR) -1 /* IDropTarget::Drop */
};

const CInterfaceStubVtbl _IDropTargetStubVtbl =
{
    &IID_IDropTarget,
    &IDropTarget_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumOLEVERB, ver. 0.0,
   GUID={0x00000104,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumOLEVERB_FormatStringOffsetTable[] =
    {
    3618,
    50,
    2390,
    3668
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumOLEVERB_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumOLEVERB_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumOLEVERB_StubThunkTable[] = 
    {
    IEnumOLEVERB_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumOLEVERB_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumOLEVERB_FormatStringOffsetTable[-3],
    &IEnumOLEVERB_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumOLEVERBProxyVtbl = 
{
    &IEnumOLEVERB_ProxyInfo,
    &IID_IEnumOLEVERB,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumOLEVERB_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumOLEVERB::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumOLEVERB::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumOLEVERB::Clone */
};

const CInterfaceStubVtbl _IEnumOLEVERBStubVtbl =
{
    &IID_IEnumOLEVERB,
    &IEnumOLEVERB_ServerInfo,
    7,
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

const CInterfaceProxyVtbl * _oleidl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IEnumOLEVERBProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IViewObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleWindowProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceUIWindowProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceFrameProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceActiveObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleClientSiteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleInPlaceSiteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IParseDisplayNameProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleContainerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleItemContainerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleLinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleCacheProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDropTargetProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IViewObject2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleCache2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOleCacheControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IContinueProxyVtbl,
    0
};

const CInterfaceStubVtbl * _oleidl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IEnumOLEVERBStubVtbl,
    ( CInterfaceStubVtbl *) &_IViewObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleWindowStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceUIWindowStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceFrameStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceActiveObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleClientSiteStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleInPlaceSiteStubVtbl,
    ( CInterfaceStubVtbl *) &_IParseDisplayNameStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleContainerStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleItemContainerStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleLinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IOleCacheStubVtbl,
    ( CInterfaceStubVtbl *) &_IDropTargetStubVtbl,
    ( CInterfaceStubVtbl *) &_IViewObject2StubVtbl,
    ( CInterfaceStubVtbl *) &_IOleCache2StubVtbl,
    ( CInterfaceStubVtbl *) &_IOleCacheControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IContinueStubVtbl,
    0
};

PCInterfaceName const _oleidl_InterfaceNamesList[] = 
{
    "IEnumOLEVERB",
    "IViewObject",
    "IOleObject",
    "IOleInPlaceObject",
    "IOleWindow",
    "IOleInPlaceUIWindow",
    "IOleInPlaceFrame",
    "IOleInPlaceActiveObject",
    "IOleClientSite",
    "IOleInPlaceSite",
    "IParseDisplayName",
    "IOleContainer",
    "IOleItemContainer",
    "IOleLink",
    "IOleCache",
    "IDropTarget",
    "IViewObject2",
    "IOleCache2",
    "IOleCacheControl",
    "IContinue",
    0
};


#define _oleidl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _oleidl, pIID, n)

int __stdcall _oleidl_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _oleidl, 20, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _oleidl, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _oleidl, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _oleidl, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _oleidl, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _oleidl, 20, *pIndex )
    
}

const ExtendedProxyFileInfo oleidl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _oleidl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _oleidl_StubVtblList,
    (const PCInterfaceName * ) & _oleidl_InterfaceNamesList,
    0, // no delegation
    & _oleidl_IID_Lookup, 
    20,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

