/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0158 */
/* at Thu May 28 16:58:24 1998
 */
/* Compiler settings for f:\platinum\cdontb2\src\cdo\cdo.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, app_config, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __cdo_h__
#define __cdo_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IADOStream_FWD_DEFINED__
#define __IADOStream_FWD_DEFINED__
typedef interface IADOStream IADOStream;
#endif 	/* __IADOStream_FWD_DEFINED__ */


#ifndef __IResField_FWD_DEFINED__
#define __IResField_FWD_DEFINED__
typedef interface IResField IResField;
#endif 	/* __IResField_FWD_DEFINED__ */


#ifndef __IResFields_FWD_DEFINED__
#define __IResFields_FWD_DEFINED__
typedef interface IResFields IResFields;
#endif 	/* __IResFields_FWD_DEFINED__ */


#ifndef __IBodyPart_FWD_DEFINED__
#define __IBodyPart_FWD_DEFINED__
typedef interface IBodyPart IBodyPart;
#endif 	/* __IBodyPart_FWD_DEFINED__ */


#ifndef __IMessage_FWD_DEFINED__
#define __IMessage_FWD_DEFINED__
typedef interface IMessage IMessage;
#endif 	/* __IMessage_FWD_DEFINED__ */


#ifndef __IConfig_FWD_DEFINED__
#define __IConfig_FWD_DEFINED__
typedef interface IConfig IConfig;
#endif 	/* __IConfig_FWD_DEFINED__ */


#ifndef __IMessages_FWD_DEFINED__
#define __IMessages_FWD_DEFINED__
typedef interface IMessages IMessages;
#endif 	/* __IMessages_FWD_DEFINED__ */


#ifndef __IDropDirectory_FWD_DEFINED__
#define __IDropDirectory_FWD_DEFINED__
typedef interface IDropDirectory IDropDirectory;
#endif 	/* __IDropDirectory_FWD_DEFINED__ */


#ifndef __IBodyParts_FWD_DEFINED__
#define __IBodyParts_FWD_DEFINED__
typedef interface IBodyParts IBodyParts;
#endif 	/* __IBodyParts_FWD_DEFINED__ */


#ifndef __IBodyParts_FWD_DEFINED__
#define __IBodyParts_FWD_DEFINED__
typedef interface IBodyParts IBodyParts;
#endif 	/* __IBodyParts_FWD_DEFINED__ */


#ifndef __IMessages_FWD_DEFINED__
#define __IMessages_FWD_DEFINED__
typedef interface IMessages IMessages;
#endif 	/* __IMessages_FWD_DEFINED__ */


#ifndef __ISMTPOnArrival_FWD_DEFINED__
#define __ISMTPOnArrival_FWD_DEFINED__
typedef interface ISMTPOnArrival ISMTPOnArrival;
#endif 	/* __ISMTPOnArrival_FWD_DEFINED__ */


#ifndef __INNTPOnPost_FWD_DEFINED__
#define __INNTPOnPost_FWD_DEFINED__
typedef interface INNTPOnPost INNTPOnPost;
#endif 	/* __INNTPOnPost_FWD_DEFINED__ */


#ifndef __INNTPOnPostFinal_FWD_DEFINED__
#define __INNTPOnPostFinal_FWD_DEFINED__
typedef interface INNTPOnPostFinal INNTPOnPostFinal;
#endif 	/* __INNTPOnPostFinal_FWD_DEFINED__ */


#ifndef __Message_FWD_DEFINED__
#define __Message_FWD_DEFINED__

#ifdef __cplusplus
typedef class Message Message;
#else
typedef struct Message Message;
#endif /* __cplusplus */

#endif 	/* __Message_FWD_DEFINED__ */


#ifndef __Config_FWD_DEFINED__
#define __Config_FWD_DEFINED__

#ifdef __cplusplus
typedef class Config Config;
#else
typedef struct Config Config;
#endif /* __cplusplus */

#endif 	/* __Config_FWD_DEFINED__ */


#ifndef __Stream_FWD_DEFINED__
#define __Stream_FWD_DEFINED__

#ifdef __cplusplus
typedef class Stream Stream;
#else
typedef struct Stream Stream;
#endif /* __cplusplus */

#endif 	/* __Stream_FWD_DEFINED__ */


#ifndef __DropDirectory_FWD_DEFINED__
#define __DropDirectory_FWD_DEFINED__

#ifdef __cplusplus
typedef class DropDirectory DropDirectory;
#else
typedef struct DropDirectory DropDirectory;
#endif /* __cplusplus */

#endif 	/* __DropDirectory_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_cdo_0000 */
/* [local] */ 






typedef 
enum CdoDataSource
    {	CdoDataSourceEmpty	= 0,
	CdoDataSourceStream	= CdoDataSourceEmpty + 1,
	CdoDataSourceBodyPart	= CdoDataSourceStream + 1
    }	CdoDataSource;

typedef /* [uuid][public] */ 
enum StatePropertyEnum
    {	adStatePropClosed	= 1,
	adStatePropOpen	= 2,
	adStatePropExecuting	= 4
    }	StatePropertyEnum;

typedef /* [uuid][public] */ 
enum StreamTypeEnum
    {	adTypeBinary	= 1,
	adTypeAnsi	= 2,
	adTypeUnicode	= 3,
	adTypeUTF8	= 4,
	adTypeUTF7	= 5
    }	StreamTypeEnum;

typedef /* [uuid][public] */ 
enum LineSeperatorEnum
    {	adLF	= 10,
	adCR	= 13,
	adCRLF	= -1
    }	LineSeperatorEnum;



extern RPC_IF_HANDLE __MIDL_itf_cdo_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_cdo_0000_v0_0_s_ifspec;

#ifndef __IADOStream_INTERFACE_DEFINED__
#define __IADOStream_INTERFACE_DEFINED__

/* interface IADOStream */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IADOStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D5C7917D-B941-11D1-9F62-0000F875B2C7")
    IADOStream : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ long __RPC_FAR *pSize) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_EndOfStream( 
            /* [retval][out] */ BOOL __RPC_FAR *pEOS) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Position( 
            /* [retval][out] */ long __RPC_FAR *pPos) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Position( 
            /* [in] */ long cPos) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ StreamTypeEnum __RPC_FAR *pType) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ StreamTypeEnum iType) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_LineSeperator( 
            /* [retval][out] */ LineSeperatorEnum __RPC_FAR *pLS) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_LineSeperator( 
            /* [in] */ LineSeperatorEnum iLS) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ StatePropertyEnum __RPC_FAR *pState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [defaultvalue][in] */ long cChar,
            /* [retval][out] */ VARIANT __RPC_FAR *pVar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadLine( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadAll( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ long cChar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SkipLine( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Write( 
            /* [in] */ VARIANT var) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteStream( 
            /* [in] */ VARIANT var) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteLine( 
            /* [in] */ BSTR bstr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEOS( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyTo( 
            /* [in] */ IADOStream __RPC_FAR *pStmDst,
            /* [defaultvalue][optional][in] */ long cChar = -1) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IADOStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IADOStream __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IADOStream __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IADOStream __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IADOStream __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSize);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EndOfStream )( 
            IADOStream __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pEOS);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Position )( 
            IADOStream __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPos);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Position )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ long cPos);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IADOStream __RPC_FAR * This,
            /* [retval][out] */ StreamTypeEnum __RPC_FAR *pType);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ StreamTypeEnum iType);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LineSeperator )( 
            IADOStream __RPC_FAR * This,
            /* [retval][out] */ LineSeperatorEnum __RPC_FAR *pLS);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LineSeperator )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ LineSeperatorEnum iLS);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            IADOStream __RPC_FAR * This,
            /* [retval][out] */ StatePropertyEnum __RPC_FAR *pState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Read )( 
            IADOStream __RPC_FAR * This,
            /* [defaultvalue][in] */ long cChar,
            /* [retval][out] */ VARIANT __RPC_FAR *pVar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadLine )( 
            IADOStream __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadAll )( 
            IADOStream __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ long cChar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SkipLine )( 
            IADOStream __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Write )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ VARIANT var);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteStream )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ VARIANT var);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteLine )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEOS )( 
            IADOStream __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CopyTo )( 
            IADOStream __RPC_FAR * This,
            /* [in] */ IADOStream __RPC_FAR *pStmDst,
            /* [defaultvalue][optional][in] */ long cChar);
        
        END_INTERFACE
    } IADOStreamVtbl;

    interface IADOStream
    {
        CONST_VTBL struct IADOStreamVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IADOStream_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IADOStream_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IADOStream_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IADOStream_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IADOStream_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IADOStream_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IADOStream_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IADOStream_get_Size(This,pSize)	\
    (This)->lpVtbl -> get_Size(This,pSize)

#define IADOStream_get_EndOfStream(This,pEOS)	\
    (This)->lpVtbl -> get_EndOfStream(This,pEOS)

#define IADOStream_get_Position(This,pPos)	\
    (This)->lpVtbl -> get_Position(This,pPos)

#define IADOStream_put_Position(This,cPos)	\
    (This)->lpVtbl -> put_Position(This,cPos)

#define IADOStream_get_Type(This,pType)	\
    (This)->lpVtbl -> get_Type(This,pType)

#define IADOStream_put_Type(This,iType)	\
    (This)->lpVtbl -> put_Type(This,iType)

#define IADOStream_get_LineSeperator(This,pLS)	\
    (This)->lpVtbl -> get_LineSeperator(This,pLS)

#define IADOStream_put_LineSeperator(This,iLS)	\
    (This)->lpVtbl -> put_LineSeperator(This,iLS)

#define IADOStream_get_State(This,pState)	\
    (This)->lpVtbl -> get_State(This,pState)

#define IADOStream_Read(This,cChar,pVar)	\
    (This)->lpVtbl -> Read(This,cChar,pVar)

#define IADOStream_ReadLine(This,pbstr)	\
    (This)->lpVtbl -> ReadLine(This,pbstr)

#define IADOStream_ReadAll(This,pVar)	\
    (This)->lpVtbl -> ReadAll(This,pVar)

#define IADOStream_Skip(This,cChar)	\
    (This)->lpVtbl -> Skip(This,cChar)

#define IADOStream_SkipLine(This)	\
    (This)->lpVtbl -> SkipLine(This)

#define IADOStream_Write(This,var)	\
    (This)->lpVtbl -> Write(This,var)

#define IADOStream_WriteStream(This,var)	\
    (This)->lpVtbl -> WriteStream(This,var)

#define IADOStream_WriteLine(This,bstr)	\
    (This)->lpVtbl -> WriteLine(This,bstr)

#define IADOStream_SetEOS(This)	\
    (This)->lpVtbl -> SetEOS(This)

#define IADOStream_CopyTo(This,pStmDst,cChar)	\
    (This)->lpVtbl -> CopyTo(This,pStmDst,cChar)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IADOStream_get_Size_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSize);


void __RPC_STUB IADOStream_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IADOStream_get_EndOfStream_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pEOS);


void __RPC_STUB IADOStream_get_EndOfStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IADOStream_get_Position_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pPos);


void __RPC_STUB IADOStream_get_Position_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IADOStream_put_Position_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [in] */ long cPos);


void __RPC_STUB IADOStream_put_Position_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IADOStream_get_Type_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [retval][out] */ StreamTypeEnum __RPC_FAR *pType);


void __RPC_STUB IADOStream_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IADOStream_put_Type_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [in] */ StreamTypeEnum iType);


void __RPC_STUB IADOStream_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IADOStream_get_LineSeperator_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [retval][out] */ LineSeperatorEnum __RPC_FAR *pLS);


void __RPC_STUB IADOStream_get_LineSeperator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IADOStream_put_LineSeperator_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [in] */ LineSeperatorEnum iLS);


void __RPC_STUB IADOStream_put_LineSeperator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IADOStream_get_State_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [retval][out] */ StatePropertyEnum __RPC_FAR *pState);


void __RPC_STUB IADOStream_get_State_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_Read_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [defaultvalue][in] */ long cChar,
    /* [retval][out] */ VARIANT __RPC_FAR *pVar);


void __RPC_STUB IADOStream_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_ReadLine_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);


void __RPC_STUB IADOStream_ReadLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_ReadAll_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVar);


void __RPC_STUB IADOStream_ReadAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_Skip_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [in] */ long cChar);


void __RPC_STUB IADOStream_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_SkipLine_Proxy( 
    IADOStream __RPC_FAR * This);


void __RPC_STUB IADOStream_SkipLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_Write_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [in] */ VARIANT var);


void __RPC_STUB IADOStream_Write_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_WriteStream_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [in] */ VARIANT var);


void __RPC_STUB IADOStream_WriteStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_WriteLine_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [in] */ BSTR bstr);


void __RPC_STUB IADOStream_WriteLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_SetEOS_Proxy( 
    IADOStream __RPC_FAR * This);


void __RPC_STUB IADOStream_SetEOS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IADOStream_CopyTo_Proxy( 
    IADOStream __RPC_FAR * This,
    /* [in] */ IADOStream __RPC_FAR *pStmDst,
    /* [defaultvalue][optional][in] */ long cChar);


void __RPC_STUB IADOStream_CopyTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IADOStream_INTERFACE_DEFINED__ */


#ifndef __IResField_INTERFACE_DEFINED__
#define __IResField_INTERFACE_DEFINED__

/* interface IResField */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IResField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fc23e808-9766-11d1-aafd-00c04fc31d63")
    IResField : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarValue) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT varValue) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IResField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IResField __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IResField __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IResField __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IResField __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IResField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IResField __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            IResField __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarValue);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            IResField __RPC_FAR * This,
            /* [in] */ VARIANT varValue);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IResField __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        END_INTERFACE
    } IResFieldVtbl;

    interface IResField
    {
        CONST_VTBL struct IResFieldVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResField_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IResField_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IResField_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IResField_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IResField_get_Value(This,pvarValue)	\
    (This)->lpVtbl -> get_Value(This,pvarValue)

#define IResField_put_Value(This,varValue)	\
    (This)->lpVtbl -> put_Value(This,varValue)

#define IResField_get_Name(This,pbstr)	\
    (This)->lpVtbl -> get_Name(This,pbstr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IResField_get_Value_Proxy( 
    IResField __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarValue);


void __RPC_STUB IResField_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IResField_put_Value_Proxy( 
    IResField __RPC_FAR * This,
    /* [in] */ VARIANT varValue);


void __RPC_STUB IResField_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IResField_get_Name_Proxy( 
    IResField __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);


void __RPC_STUB IResField_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResField_INTERFACE_DEFINED__ */


#ifndef __IResFields_INTERFACE_DEFINED__
#define __IResFields_INTERFACE_DEFINED__

/* interface IResFields */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IResFields;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("06cb3f3c-969e-11d1-aafc-00c04fc31d63")
    IResFields : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ IResField __RPC_FAR *__RPC_FAR *ppirfld) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pcrfld) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ BSTR pbstrName) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkRet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResFieldsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IResFields __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IResFields __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IResFields __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IResFields __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IResFields __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IResFields __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IResFields __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IResFields __RPC_FAR * This,
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ IResField __RPC_FAR *__RPC_FAR *ppirfld);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IResFields __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pcrfld);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IResFields __RPC_FAR * This,
            /* [in] */ BSTR pbstrName);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IResFields __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkRet);
        
        END_INTERFACE
    } IResFieldsVtbl;

    interface IResFields
    {
        CONST_VTBL struct IResFieldsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResFields_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResFields_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResFields_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResFields_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IResFields_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IResFields_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IResFields_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IResFields_get_Item(This,varIndex,ppirfld)	\
    (This)->lpVtbl -> get_Item(This,varIndex,ppirfld)

#define IResFields_get_Count(This,pcrfld)	\
    (This)->lpVtbl -> get_Count(This,pcrfld)

#define IResFields_Delete(This,pbstrName)	\
    (This)->lpVtbl -> Delete(This,pbstrName)

#define IResFields_get__NewEnum(This,ppUnkRet)	\
    (This)->lpVtbl -> get__NewEnum(This,ppUnkRet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IResFields_get_Item_Proxy( 
    IResFields __RPC_FAR * This,
    /* [in] */ VARIANT varIndex,
    /* [retval][out] */ IResField __RPC_FAR *__RPC_FAR *ppirfld);


void __RPC_STUB IResFields_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IResFields_get_Count_Proxy( 
    IResFields __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pcrfld);


void __RPC_STUB IResFields_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IResFields_Delete_Proxy( 
    IResFields __RPC_FAR * This,
    /* [in] */ BSTR pbstrName);


void __RPC_STUB IResFields_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IResFields_get__NewEnum_Proxy( 
    IResFields __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkRet);


void __RPC_STUB IResFields_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResFields_INTERFACE_DEFINED__ */


#ifndef __IBodyPart_INTERFACE_DEFINED__
#define __IBodyPart_INTERFACE_DEFINED__

/* interface IBodyPart */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IBodyPart;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33D47B92-8B95-11D1-82DB-00C04FB1625D")
    IBodyPart : public IDispatch
    {
    public:
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_BodyParts( 
            /* [retval][out] */ IBodyParts __RPC_FAR *__RPC_FAR *varBodyParts) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ContentTransferEncoding( 
            /* [retval][out] */ BSTR __RPC_FAR *pContentTransferEncoding) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ContentTransferEncoding( 
            /* [in] */ BSTR varContentTransferEncoding) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ContentMediaType( 
            /* [retval][out] */ BSTR __RPC_FAR *pContentMediaType) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ContentMediaType( 
            /* [in] */ BSTR varContentMediaType) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Fields( 
            /* [retval][out] */ IResFields __RPC_FAR *__RPC_FAR *varFields) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Charset( 
            /* [retval][out] */ BSTR __RPC_FAR *pCharset) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Charset( 
            /* [in] */ BSTR varCharset) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FileName( 
            /* [retval][out] */ BSTR __RPC_FAR *varFileName) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DataSourceType( 
            /* [retval][out] */ CdoDataSource __RPC_FAR *varDataSourceType) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DataSourceURL( 
            /* [retval][out] */ BSTR __RPC_FAR *varDataSourceURL) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddBodyPart( 
            /* [defaultvalue][in] */ long iIndex,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppPart) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEncodedContentStream( 
            /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDecodedContentStream( 
            /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEncodedContentStream( 
            /* [in] */ IADOStream __RPC_FAR *pStm) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDecodedContentStream( 
            /* [in] */ IADOStream __RPC_FAR *pStm) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BindToDataSource( 
            /* [in] */ IUnknown __RPC_FAR *pObj,
            /* [in] */ CdoDataSource source) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveToDataSource( 
            /* [in] */ IUnknown __RPC_FAR *pObj,
            /* [in] */ CdoDataSource source) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BindToURL( 
            /* [in] */ BSTR bstrURL) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveToFile( 
            /* [in] */ BSTR bstrURL) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBodyPartVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBodyPart __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBodyPart __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBodyPart __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BodyParts )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ IBodyParts __RPC_FAR *__RPC_FAR *varBodyParts);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContentTransferEncoding )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pContentTransferEncoding);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContentTransferEncoding )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ BSTR varContentTransferEncoding);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContentMediaType )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pContentMediaType);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContentMediaType )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ BSTR varContentMediaType);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Fields )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ IResFields __RPC_FAR *__RPC_FAR *varFields);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Charset )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pCharset);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Charset )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ BSTR varCharset);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *varFileName);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSourceType )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ CdoDataSource __RPC_FAR *varDataSourceType);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSourceURL )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *varDataSourceURL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddBodyPart )( 
            IBodyPart __RPC_FAR * This,
            /* [defaultvalue][in] */ long iIndex,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppPart);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEncodedContentStream )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDecodedContentStream )( 
            IBodyPart __RPC_FAR * This,
            /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEncodedContentStream )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ IADOStream __RPC_FAR *pStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDecodedContentStream )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ IADOStream __RPC_FAR *pStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IBodyPart __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BindToDataSource )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pObj,
            /* [in] */ CdoDataSource source);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveToDataSource )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pObj,
            /* [in] */ CdoDataSource source);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BindToURL )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ BSTR bstrURL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveToFile )( 
            IBodyPart __RPC_FAR * This,
            /* [in] */ BSTR bstrURL);
        
        END_INTERFACE
    } IBodyPartVtbl;

    interface IBodyPart
    {
        CONST_VTBL struct IBodyPartVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBodyPart_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBodyPart_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBodyPart_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBodyPart_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBodyPart_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBodyPart_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBodyPart_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBodyPart_get_BodyParts(This,varBodyParts)	\
    (This)->lpVtbl -> get_BodyParts(This,varBodyParts)

#define IBodyPart_get_ContentTransferEncoding(This,pContentTransferEncoding)	\
    (This)->lpVtbl -> get_ContentTransferEncoding(This,pContentTransferEncoding)

#define IBodyPart_put_ContentTransferEncoding(This,varContentTransferEncoding)	\
    (This)->lpVtbl -> put_ContentTransferEncoding(This,varContentTransferEncoding)

#define IBodyPart_get_ContentMediaType(This,pContentMediaType)	\
    (This)->lpVtbl -> get_ContentMediaType(This,pContentMediaType)

#define IBodyPart_put_ContentMediaType(This,varContentMediaType)	\
    (This)->lpVtbl -> put_ContentMediaType(This,varContentMediaType)

#define IBodyPart_get_Fields(This,varFields)	\
    (This)->lpVtbl -> get_Fields(This,varFields)

#define IBodyPart_get_Charset(This,pCharset)	\
    (This)->lpVtbl -> get_Charset(This,pCharset)

#define IBodyPart_put_Charset(This,varCharset)	\
    (This)->lpVtbl -> put_Charset(This,varCharset)

#define IBodyPart_get_FileName(This,varFileName)	\
    (This)->lpVtbl -> get_FileName(This,varFileName)

#define IBodyPart_get_DataSourceType(This,varDataSourceType)	\
    (This)->lpVtbl -> get_DataSourceType(This,varDataSourceType)

#define IBodyPart_get_DataSourceURL(This,varDataSourceURL)	\
    (This)->lpVtbl -> get_DataSourceURL(This,varDataSourceURL)

#define IBodyPart_AddBodyPart(This,iIndex,ppPart)	\
    (This)->lpVtbl -> AddBodyPart(This,iIndex,ppPart)

#define IBodyPart_GetEncodedContentStream(This,ppStm)	\
    (This)->lpVtbl -> GetEncodedContentStream(This,ppStm)

#define IBodyPart_GetDecodedContentStream(This,ppStm)	\
    (This)->lpVtbl -> GetDecodedContentStream(This,ppStm)

#define IBodyPart_SetEncodedContentStream(This,pStm)	\
    (This)->lpVtbl -> SetEncodedContentStream(This,pStm)

#define IBodyPart_SetDecodedContentStream(This,pStm)	\
    (This)->lpVtbl -> SetDecodedContentStream(This,pStm)

#define IBodyPart_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define IBodyPart_BindToDataSource(This,pObj,source)	\
    (This)->lpVtbl -> BindToDataSource(This,pObj,source)

#define IBodyPart_SaveToDataSource(This,pObj,source)	\
    (This)->lpVtbl -> SaveToDataSource(This,pObj,source)

#define IBodyPart_BindToURL(This,bstrURL)	\
    (This)->lpVtbl -> BindToURL(This,bstrURL)

#define IBodyPart_SaveToFile(This,bstrURL)	\
    (This)->lpVtbl -> SaveToFile(This,bstrURL)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_get_BodyParts_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ IBodyParts __RPC_FAR *__RPC_FAR *varBodyParts);


void __RPC_STUB IBodyPart_get_BodyParts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_get_ContentTransferEncoding_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pContentTransferEncoding);


void __RPC_STUB IBodyPart_get_ContentTransferEncoding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_put_ContentTransferEncoding_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ BSTR varContentTransferEncoding);


void __RPC_STUB IBodyPart_put_ContentTransferEncoding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_get_ContentMediaType_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pContentMediaType);


void __RPC_STUB IBodyPart_get_ContentMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_put_ContentMediaType_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ BSTR varContentMediaType);


void __RPC_STUB IBodyPart_put_ContentMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_get_Fields_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ IResFields __RPC_FAR *__RPC_FAR *varFields);


void __RPC_STUB IBodyPart_get_Fields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_get_Charset_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pCharset);


void __RPC_STUB IBodyPart_get_Charset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_put_Charset_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ BSTR varCharset);


void __RPC_STUB IBodyPart_put_Charset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_get_FileName_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *varFileName);


void __RPC_STUB IBodyPart_get_FileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_get_DataSourceType_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ CdoDataSource __RPC_FAR *varDataSourceType);


void __RPC_STUB IBodyPart_get_DataSourceType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_get_DataSourceURL_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *varDataSourceURL);


void __RPC_STUB IBodyPart_get_DataSourceURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_AddBodyPart_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [defaultvalue][in] */ long iIndex,
    /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppPart);


void __RPC_STUB IBodyPart_AddBodyPart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_GetEncodedContentStream_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm);


void __RPC_STUB IBodyPart_GetEncodedContentStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_GetDecodedContentStream_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm);


void __RPC_STUB IBodyPart_GetDecodedContentStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_SetEncodedContentStream_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ IADOStream __RPC_FAR *pStm);


void __RPC_STUB IBodyPart_SetEncodedContentStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_SetDecodedContentStream_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ IADOStream __RPC_FAR *pStm);


void __RPC_STUB IBodyPart_SetDecodedContentStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_Save_Proxy( 
    IBodyPart __RPC_FAR * This);


void __RPC_STUB IBodyPart_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_BindToDataSource_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pObj,
    /* [in] */ CdoDataSource source);


void __RPC_STUB IBodyPart_BindToDataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_SaveToDataSource_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pObj,
    /* [in] */ CdoDataSource source);


void __RPC_STUB IBodyPart_SaveToDataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_BindToURL_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ BSTR bstrURL);


void __RPC_STUB IBodyPart_BindToURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyPart_SaveToFile_Proxy( 
    IBodyPart __RPC_FAR * This,
    /* [in] */ BSTR bstrURL);


void __RPC_STUB IBodyPart_SaveToFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBodyPart_INTERFACE_DEFINED__ */


#ifndef __IMessage_INTERFACE_DEFINED__
#define __IMessage_INTERFACE_DEFINED__

/* interface IMessage */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMessage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33D47B91-8B95-11D1-82DB-00C04FB1625D")
    IMessage : public IBodyPart
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_BCC( 
            /* [retval][out] */ BSTR __RPC_FAR *pBCC) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_BCC( 
            /* [in] */ BSTR varBCC) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_CC( 
            /* [retval][out] */ BSTR __RPC_FAR *pCC) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_CC( 
            /* [in] */ BSTR varCC) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FollowUpTo( 
            /* [retval][out] */ BSTR __RPC_FAR *pFollowUpTo) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_FollowUpTo( 
            /* [in] */ BSTR varFollowUpTo) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_From( 
            /* [retval][out] */ BSTR __RPC_FAR *pFrom) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_From( 
            /* [in] */ BSTR varFrom) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Keywords( 
            /* [retval][out] */ BSTR __RPC_FAR *pKeywords) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Keywords( 
            /* [in] */ BSTR varKeywords) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MimeFormatted( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMimeFormatted) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MimeFormatted( 
            /* [in] */ VARIANT_BOOL varMimeFormatted) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Newsgroups( 
            /* [retval][out] */ BSTR __RPC_FAR *pNewsgroups) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Newsgroups( 
            /* [in] */ BSTR varNewsgroups) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Organization( 
            /* [retval][out] */ BSTR __RPC_FAR *pOrganization) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Organization( 
            /* [in] */ BSTR varOrganization) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TimeReceived( 
            /* [retval][out] */ DATE __RPC_FAR *varTimeReceived) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ReplyTo( 
            /* [retval][out] */ BSTR __RPC_FAR *pReplyTo) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ReplyTo( 
            /* [in] */ BSTR varReplyTo) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_RequestDeliveryReceipt( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pRequestDeliveryReceipt) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_RequestDeliveryReceipt( 
            /* [in] */ VARIANT_BOOL varRequestDeliveryReceipt) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TimeSent( 
            /* [retval][out] */ DATE __RPC_FAR *varTimeSent) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Subject( 
            /* [retval][out] */ BSTR __RPC_FAR *pSubject) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Subject( 
            /* [in] */ BSTR varSubject) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_To( 
            /* [retval][out] */ BSTR __RPC_FAR *pTo) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_To( 
            /* [in] */ BSTR varTo) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TextBody( 
            /* [retval][out] */ BSTR __RPC_FAR *pTextBody) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TextBody( 
            /* [in] */ BSTR varTextBody) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HTMLBody( 
            /* [retval][out] */ BSTR __RPC_FAR *pHTMLBody) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_HTMLBody( 
            /* [in] */ BSTR varHTMLBody) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Attachments( 
            /* [retval][out] */ IBodyParts __RPC_FAR *__RPC_FAR *varAttachments) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Sender( 
            /* [retval][out] */ BSTR __RPC_FAR *pSender) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Sender( 
            /* [in] */ BSTR varSender) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Configuration( 
            /* [retval][out] */ IConfig __RPC_FAR *__RPC_FAR *pConfiguration) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Configuration( 
            /* [in] */ IConfig __RPC_FAR *varConfiguration) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoGenerateTextBody( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoGenerateTextBody) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoGenerateTextBody( 
            /* [in] */ VARIANT_BOOL varAutoGenerateTextBody) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_EnvelopeFields( 
            /* [retval][out] */ IResFields __RPC_FAR *__RPC_FAR *varEnvelopeFields) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddRelatedAttachment( 
            /* [in] */ BSTR bstrURL,
            /* [in] */ BSTR bstrContentID,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddAttachment( 
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateMHTMLBody( 
            /* [in] */ BSTR bstrURL,
            /* [defaultvalue][in] */ long Flags = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Forward( 
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTextBodyPart( 
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHTMLBodyPart( 
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStream( 
            /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetStream( 
            /* [in] */ IADOStream __RPC_FAR *pStm) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Post( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PostReply( 
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reply( 
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReplyAll( 
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Send( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMessageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMessage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMessage __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMessage __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMessage __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMessage __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMessage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMessage __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BodyParts )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IBodyParts __RPC_FAR *__RPC_FAR *varBodyParts);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContentTransferEncoding )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pContentTransferEncoding);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContentTransferEncoding )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varContentTransferEncoding);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContentMediaType )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pContentMediaType);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContentMediaType )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varContentMediaType);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Fields )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IResFields __RPC_FAR *__RPC_FAR *varFields);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Charset )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pCharset);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Charset )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varCharset);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *varFileName);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSourceType )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ CdoDataSource __RPC_FAR *varDataSourceType);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSourceURL )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *varDataSourceURL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddBodyPart )( 
            IMessage __RPC_FAR * This,
            /* [defaultvalue][in] */ long iIndex,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppPart);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEncodedContentStream )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDecodedContentStream )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEncodedContentStream )( 
            IMessage __RPC_FAR * This,
            /* [in] */ IADOStream __RPC_FAR *pStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDecodedContentStream )( 
            IMessage __RPC_FAR * This,
            /* [in] */ IADOStream __RPC_FAR *pStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IMessage __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BindToDataSource )( 
            IMessage __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pObj,
            /* [in] */ CdoDataSource source);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveToDataSource )( 
            IMessage __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pObj,
            /* [in] */ CdoDataSource source);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BindToURL )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR bstrURL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveToFile )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR bstrURL);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BCC )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pBCC);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BCC )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varBCC);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CC )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pCC);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CC )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varCC);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FollowUpTo )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pFollowUpTo);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FollowUpTo )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varFollowUpTo);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_From )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pFrom);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_From )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varFrom);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Keywords )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pKeywords);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Keywords )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varKeywords);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MimeFormatted )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMimeFormatted);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MimeFormatted )( 
            IMessage __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL varMimeFormatted);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Newsgroups )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pNewsgroups);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Newsgroups )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varNewsgroups);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Organization )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pOrganization);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Organization )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varOrganization);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TimeReceived )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *varTimeReceived);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReplyTo )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pReplyTo);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReplyTo )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varReplyTo);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestDeliveryReceipt )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pRequestDeliveryReceipt);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestDeliveryReceipt )( 
            IMessage __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL varRequestDeliveryReceipt);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TimeSent )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *varTimeSent);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Subject )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pSubject);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Subject )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varSubject);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_To )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pTo);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_To )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varTo);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TextBody )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pTextBody);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TextBody )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varTextBody);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HTMLBody )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pHTMLBody);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HTMLBody )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varHTMLBody);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attachments )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IBodyParts __RPC_FAR *__RPC_FAR *varAttachments);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sender )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pSender);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Sender )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR varSender);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Configuration )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IConfig __RPC_FAR *__RPC_FAR *pConfiguration);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Configuration )( 
            IMessage __RPC_FAR * This,
            /* [in] */ IConfig __RPC_FAR *varConfiguration);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoGenerateTextBody )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoGenerateTextBody);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoGenerateTextBody )( 
            IMessage __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL varAutoGenerateTextBody);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnvelopeFields )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IResFields __RPC_FAR *__RPC_FAR *varEnvelopeFields);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddRelatedAttachment )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR bstrURL,
            /* [in] */ BSTR bstrContentID,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddAttachment )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateMHTMLBody )( 
            IMessage __RPC_FAR * This,
            /* [in] */ BSTR bstrURL,
            /* [defaultvalue][in] */ long Flags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Forward )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTextBodyPart )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHTMLBodyPart )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStream )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetStream )( 
            IMessage __RPC_FAR * This,
            /* [in] */ IADOStream __RPC_FAR *pStm);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Post )( 
            IMessage __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PostReply )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reply )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReplyAll )( 
            IMessage __RPC_FAR * This,
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send )( 
            IMessage __RPC_FAR * This);
        
        END_INTERFACE
    } IMessageVtbl;

    interface IMessage
    {
        CONST_VTBL struct IMessageVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMessage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMessage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMessage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMessage_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMessage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMessage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMessage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMessage_get_BodyParts(This,varBodyParts)	\
    (This)->lpVtbl -> get_BodyParts(This,varBodyParts)

#define IMessage_get_ContentTransferEncoding(This,pContentTransferEncoding)	\
    (This)->lpVtbl -> get_ContentTransferEncoding(This,pContentTransferEncoding)

#define IMessage_put_ContentTransferEncoding(This,varContentTransferEncoding)	\
    (This)->lpVtbl -> put_ContentTransferEncoding(This,varContentTransferEncoding)

#define IMessage_get_ContentMediaType(This,pContentMediaType)	\
    (This)->lpVtbl -> get_ContentMediaType(This,pContentMediaType)

#define IMessage_put_ContentMediaType(This,varContentMediaType)	\
    (This)->lpVtbl -> put_ContentMediaType(This,varContentMediaType)

#define IMessage_get_Fields(This,varFields)	\
    (This)->lpVtbl -> get_Fields(This,varFields)

#define IMessage_get_Charset(This,pCharset)	\
    (This)->lpVtbl -> get_Charset(This,pCharset)

#define IMessage_put_Charset(This,varCharset)	\
    (This)->lpVtbl -> put_Charset(This,varCharset)

#define IMessage_get_FileName(This,varFileName)	\
    (This)->lpVtbl -> get_FileName(This,varFileName)

#define IMessage_get_DataSourceType(This,varDataSourceType)	\
    (This)->lpVtbl -> get_DataSourceType(This,varDataSourceType)

#define IMessage_get_DataSourceURL(This,varDataSourceURL)	\
    (This)->lpVtbl -> get_DataSourceURL(This,varDataSourceURL)

#define IMessage_AddBodyPart(This,iIndex,ppPart)	\
    (This)->lpVtbl -> AddBodyPart(This,iIndex,ppPart)

#define IMessage_GetEncodedContentStream(This,ppStm)	\
    (This)->lpVtbl -> GetEncodedContentStream(This,ppStm)

#define IMessage_GetDecodedContentStream(This,ppStm)	\
    (This)->lpVtbl -> GetDecodedContentStream(This,ppStm)

#define IMessage_SetEncodedContentStream(This,pStm)	\
    (This)->lpVtbl -> SetEncodedContentStream(This,pStm)

#define IMessage_SetDecodedContentStream(This,pStm)	\
    (This)->lpVtbl -> SetDecodedContentStream(This,pStm)

#define IMessage_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define IMessage_BindToDataSource(This,pObj,source)	\
    (This)->lpVtbl -> BindToDataSource(This,pObj,source)

#define IMessage_SaveToDataSource(This,pObj,source)	\
    (This)->lpVtbl -> SaveToDataSource(This,pObj,source)

#define IMessage_BindToURL(This,bstrURL)	\
    (This)->lpVtbl -> BindToURL(This,bstrURL)

#define IMessage_SaveToFile(This,bstrURL)	\
    (This)->lpVtbl -> SaveToFile(This,bstrURL)


#define IMessage_get_BCC(This,pBCC)	\
    (This)->lpVtbl -> get_BCC(This,pBCC)

#define IMessage_put_BCC(This,varBCC)	\
    (This)->lpVtbl -> put_BCC(This,varBCC)

#define IMessage_get_CC(This,pCC)	\
    (This)->lpVtbl -> get_CC(This,pCC)

#define IMessage_put_CC(This,varCC)	\
    (This)->lpVtbl -> put_CC(This,varCC)

#define IMessage_get_FollowUpTo(This,pFollowUpTo)	\
    (This)->lpVtbl -> get_FollowUpTo(This,pFollowUpTo)

#define IMessage_put_FollowUpTo(This,varFollowUpTo)	\
    (This)->lpVtbl -> put_FollowUpTo(This,varFollowUpTo)

#define IMessage_get_From(This,pFrom)	\
    (This)->lpVtbl -> get_From(This,pFrom)

#define IMessage_put_From(This,varFrom)	\
    (This)->lpVtbl -> put_From(This,varFrom)

#define IMessage_get_Keywords(This,pKeywords)	\
    (This)->lpVtbl -> get_Keywords(This,pKeywords)

#define IMessage_put_Keywords(This,varKeywords)	\
    (This)->lpVtbl -> put_Keywords(This,varKeywords)

#define IMessage_get_MimeFormatted(This,pMimeFormatted)	\
    (This)->lpVtbl -> get_MimeFormatted(This,pMimeFormatted)

#define IMessage_put_MimeFormatted(This,varMimeFormatted)	\
    (This)->lpVtbl -> put_MimeFormatted(This,varMimeFormatted)

#define IMessage_get_Newsgroups(This,pNewsgroups)	\
    (This)->lpVtbl -> get_Newsgroups(This,pNewsgroups)

#define IMessage_put_Newsgroups(This,varNewsgroups)	\
    (This)->lpVtbl -> put_Newsgroups(This,varNewsgroups)

#define IMessage_get_Organization(This,pOrganization)	\
    (This)->lpVtbl -> get_Organization(This,pOrganization)

#define IMessage_put_Organization(This,varOrganization)	\
    (This)->lpVtbl -> put_Organization(This,varOrganization)

#define IMessage_get_TimeReceived(This,varTimeReceived)	\
    (This)->lpVtbl -> get_TimeReceived(This,varTimeReceived)

#define IMessage_get_ReplyTo(This,pReplyTo)	\
    (This)->lpVtbl -> get_ReplyTo(This,pReplyTo)

#define IMessage_put_ReplyTo(This,varReplyTo)	\
    (This)->lpVtbl -> put_ReplyTo(This,varReplyTo)

#define IMessage_get_RequestDeliveryReceipt(This,pRequestDeliveryReceipt)	\
    (This)->lpVtbl -> get_RequestDeliveryReceipt(This,pRequestDeliveryReceipt)

#define IMessage_put_RequestDeliveryReceipt(This,varRequestDeliveryReceipt)	\
    (This)->lpVtbl -> put_RequestDeliveryReceipt(This,varRequestDeliveryReceipt)

#define IMessage_get_TimeSent(This,varTimeSent)	\
    (This)->lpVtbl -> get_TimeSent(This,varTimeSent)

#define IMessage_get_Subject(This,pSubject)	\
    (This)->lpVtbl -> get_Subject(This,pSubject)

#define IMessage_put_Subject(This,varSubject)	\
    (This)->lpVtbl -> put_Subject(This,varSubject)

#define IMessage_get_To(This,pTo)	\
    (This)->lpVtbl -> get_To(This,pTo)

#define IMessage_put_To(This,varTo)	\
    (This)->lpVtbl -> put_To(This,varTo)

#define IMessage_get_TextBody(This,pTextBody)	\
    (This)->lpVtbl -> get_TextBody(This,pTextBody)

#define IMessage_put_TextBody(This,varTextBody)	\
    (This)->lpVtbl -> put_TextBody(This,varTextBody)

#define IMessage_get_HTMLBody(This,pHTMLBody)	\
    (This)->lpVtbl -> get_HTMLBody(This,pHTMLBody)

#define IMessage_put_HTMLBody(This,varHTMLBody)	\
    (This)->lpVtbl -> put_HTMLBody(This,varHTMLBody)

#define IMessage_get_Attachments(This,varAttachments)	\
    (This)->lpVtbl -> get_Attachments(This,varAttachments)

#define IMessage_get_Sender(This,pSender)	\
    (This)->lpVtbl -> get_Sender(This,pSender)

#define IMessage_put_Sender(This,varSender)	\
    (This)->lpVtbl -> put_Sender(This,varSender)

#define IMessage_get_Configuration(This,pConfiguration)	\
    (This)->lpVtbl -> get_Configuration(This,pConfiguration)

#define IMessage_put_Configuration(This,varConfiguration)	\
    (This)->lpVtbl -> put_Configuration(This,varConfiguration)

#define IMessage_get_AutoGenerateTextBody(This,pAutoGenerateTextBody)	\
    (This)->lpVtbl -> get_AutoGenerateTextBody(This,pAutoGenerateTextBody)

#define IMessage_put_AutoGenerateTextBody(This,varAutoGenerateTextBody)	\
    (This)->lpVtbl -> put_AutoGenerateTextBody(This,varAutoGenerateTextBody)

#define IMessage_get_EnvelopeFields(This,varEnvelopeFields)	\
    (This)->lpVtbl -> get_EnvelopeFields(This,varEnvelopeFields)

#define IMessage_AddRelatedAttachment(This,bstrURL,bstrContentID,ppBody)	\
    (This)->lpVtbl -> AddRelatedAttachment(This,bstrURL,bstrContentID,ppBody)

#define IMessage_AddAttachment(This,bstrURL,ppBody)	\
    (This)->lpVtbl -> AddAttachment(This,bstrURL,ppBody)

#define IMessage_CreateMHTMLBody(This,bstrURL,Flags)	\
    (This)->lpVtbl -> CreateMHTMLBody(This,bstrURL,Flags)

#define IMessage_Forward(This,ppMsg)	\
    (This)->lpVtbl -> Forward(This,ppMsg)

#define IMessage_GetTextBodyPart(This,ppBody)	\
    (This)->lpVtbl -> GetTextBodyPart(This,ppBody)

#define IMessage_GetHTMLBodyPart(This,ppBody)	\
    (This)->lpVtbl -> GetHTMLBodyPart(This,ppBody)

#define IMessage_GetStream(This,ppStm)	\
    (This)->lpVtbl -> GetStream(This,ppStm)

#define IMessage_SetStream(This,pStm)	\
    (This)->lpVtbl -> SetStream(This,pStm)

#define IMessage_Post(This)	\
    (This)->lpVtbl -> Post(This)

#define IMessage_PostReply(This,ppMsg)	\
    (This)->lpVtbl -> PostReply(This,ppMsg)

#define IMessage_Reply(This,ppMsg)	\
    (This)->lpVtbl -> Reply(This,ppMsg)

#define IMessage_ReplyAll(This,ppMsg)	\
    (This)->lpVtbl -> ReplyAll(This,ppMsg)

#define IMessage_Send(This)	\
    (This)->lpVtbl -> Send(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_BCC_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pBCC);


void __RPC_STUB IMessage_get_BCC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_BCC_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varBCC);


void __RPC_STUB IMessage_put_BCC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_CC_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pCC);


void __RPC_STUB IMessage_get_CC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_CC_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varCC);


void __RPC_STUB IMessage_put_CC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_FollowUpTo_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pFollowUpTo);


void __RPC_STUB IMessage_get_FollowUpTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_FollowUpTo_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varFollowUpTo);


void __RPC_STUB IMessage_put_FollowUpTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_From_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pFrom);


void __RPC_STUB IMessage_get_From_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_From_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varFrom);


void __RPC_STUB IMessage_put_From_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_Keywords_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pKeywords);


void __RPC_STUB IMessage_get_Keywords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_Keywords_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varKeywords);


void __RPC_STUB IMessage_put_Keywords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_MimeFormatted_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMimeFormatted);


void __RPC_STUB IMessage_get_MimeFormatted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_MimeFormatted_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL varMimeFormatted);


void __RPC_STUB IMessage_put_MimeFormatted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_Newsgroups_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pNewsgroups);


void __RPC_STUB IMessage_get_Newsgroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_Newsgroups_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varNewsgroups);


void __RPC_STUB IMessage_put_Newsgroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_Organization_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pOrganization);


void __RPC_STUB IMessage_get_Organization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_Organization_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varOrganization);


void __RPC_STUB IMessage_put_Organization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_TimeReceived_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *varTimeReceived);


void __RPC_STUB IMessage_get_TimeReceived_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_ReplyTo_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pReplyTo);


void __RPC_STUB IMessage_get_ReplyTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_ReplyTo_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varReplyTo);


void __RPC_STUB IMessage_put_ReplyTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_RequestDeliveryReceipt_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pRequestDeliveryReceipt);


void __RPC_STUB IMessage_get_RequestDeliveryReceipt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_RequestDeliveryReceipt_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL varRequestDeliveryReceipt);


void __RPC_STUB IMessage_put_RequestDeliveryReceipt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_TimeSent_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *varTimeSent);


void __RPC_STUB IMessage_get_TimeSent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_Subject_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pSubject);


void __RPC_STUB IMessage_get_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_Subject_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varSubject);


void __RPC_STUB IMessage_put_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_To_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pTo);


void __RPC_STUB IMessage_get_To_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_To_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varTo);


void __RPC_STUB IMessage_put_To_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_TextBody_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pTextBody);


void __RPC_STUB IMessage_get_TextBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_TextBody_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varTextBody);


void __RPC_STUB IMessage_put_TextBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_HTMLBody_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pHTMLBody);


void __RPC_STUB IMessage_get_HTMLBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_HTMLBody_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varHTMLBody);


void __RPC_STUB IMessage_put_HTMLBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_Attachments_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IBodyParts __RPC_FAR *__RPC_FAR *varAttachments);


void __RPC_STUB IMessage_get_Attachments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_Sender_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pSender);


void __RPC_STUB IMessage_get_Sender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_Sender_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR varSender);


void __RPC_STUB IMessage_put_Sender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_Configuration_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IConfig __RPC_FAR *__RPC_FAR *pConfiguration);


void __RPC_STUB IMessage_get_Configuration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_Configuration_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ IConfig __RPC_FAR *varConfiguration);


void __RPC_STUB IMessage_put_Configuration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_AutoGenerateTextBody_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoGenerateTextBody);


void __RPC_STUB IMessage_get_AutoGenerateTextBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IMessage_put_AutoGenerateTextBody_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL varAutoGenerateTextBody);


void __RPC_STUB IMessage_put_AutoGenerateTextBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessage_get_EnvelopeFields_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IResFields __RPC_FAR *__RPC_FAR *varEnvelopeFields);


void __RPC_STUB IMessage_get_EnvelopeFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_AddRelatedAttachment_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR bstrURL,
    /* [in] */ BSTR bstrContentID,
    /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);


void __RPC_STUB IMessage_AddRelatedAttachment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_AddAttachment_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR bstrURL,
    /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);


void __RPC_STUB IMessage_AddAttachment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_CreateMHTMLBody_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ BSTR bstrURL,
    /* [defaultvalue][in] */ long Flags);


void __RPC_STUB IMessage_CreateMHTMLBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_Forward_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg);


void __RPC_STUB IMessage_Forward_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_GetTextBodyPart_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);


void __RPC_STUB IMessage_GetTextBodyPart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_GetHTMLBodyPart_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);


void __RPC_STUB IMessage_GetHTMLBodyPart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_GetStream_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IADOStream __RPC_FAR *__RPC_FAR *ppStm);


void __RPC_STUB IMessage_GetStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_SetStream_Proxy( 
    IMessage __RPC_FAR * This,
    /* [in] */ IADOStream __RPC_FAR *pStm);


void __RPC_STUB IMessage_SetStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_Post_Proxy( 
    IMessage __RPC_FAR * This);


void __RPC_STUB IMessage_Post_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_PostReply_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg);


void __RPC_STUB IMessage_PostReply_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_Reply_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg);


void __RPC_STUB IMessage_Reply_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_ReplyAll_Proxy( 
    IMessage __RPC_FAR * This,
    /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMsg);


void __RPC_STUB IMessage_ReplyAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessage_Send_Proxy( 
    IMessage __RPC_FAR * This);


void __RPC_STUB IMessage_Send_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMessage_INTERFACE_DEFINED__ */


#ifndef __IConfig_INTERFACE_DEFINED__
#define __IConfig_INTERFACE_DEFINED__

/* interface IConfig */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33D47B93-8B95-11D1-82DB-00C04FB1625D")
    IConfig : public IResFields
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IConfig __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IConfig __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IConfig __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IConfig __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IConfig __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IConfig __RPC_FAR * This,
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ IResField __RPC_FAR *__RPC_FAR *ppirfld);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IConfig __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pcrfld);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IConfig __RPC_FAR * This,
            /* [in] */ BSTR pbstrName);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IConfig __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkRet);
        
        END_INTERFACE
    } IConfigVtbl;

    interface IConfig
    {
        CONST_VTBL struct IConfigVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IConfig_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IConfig_get_Item(This,varIndex,ppirfld)	\
    (This)->lpVtbl -> get_Item(This,varIndex,ppirfld)

#define IConfig_get_Count(This,pcrfld)	\
    (This)->lpVtbl -> get_Count(This,pcrfld)

#define IConfig_Delete(This,pbstrName)	\
    (This)->lpVtbl -> Delete(This,pbstrName)

#define IConfig_get__NewEnum(This,ppUnkRet)	\
    (This)->lpVtbl -> get__NewEnum(This,ppUnkRet)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IConfig_INTERFACE_DEFINED__ */


#ifndef __IMessages_INTERFACE_DEFINED__
#define __IMessages_INTERFACE_DEFINED__

/* interface IMessages */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMessages;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51624D52-B26D-11d1-956F-00C04FC2D670")
    IMessages : public IDispatch
    {
    public:
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            long iIndex,
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMessage) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *varCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ long iIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteAll( void) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMessagesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMessages __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMessages __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMessages __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMessages __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMessages __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMessages __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMessages __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IMessages __RPC_FAR * This,
            long iIndex,
            /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMessage);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IMessages __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *varCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IMessages __RPC_FAR * This,
            /* [in] */ long iIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteAll )( 
            IMessages __RPC_FAR * This);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IMessages __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);
        
        END_INTERFACE
    } IMessagesVtbl;

    interface IMessages
    {
        CONST_VTBL struct IMessagesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMessages_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMessages_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMessages_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMessages_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMessages_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMessages_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMessages_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMessages_get_Item(This,iIndex,ppMessage)	\
    (This)->lpVtbl -> get_Item(This,iIndex,ppMessage)

#define IMessages_get_Count(This,varCount)	\
    (This)->lpVtbl -> get_Count(This,varCount)

#define IMessages_Delete(This,iIndex)	\
    (This)->lpVtbl -> Delete(This,iIndex)

#define IMessages_DeleteAll(This)	\
    (This)->lpVtbl -> DeleteAll(This)

#define IMessages_get__NewEnum(This,retval)	\
    (This)->lpVtbl -> get__NewEnum(This,retval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessages_get_Item_Proxy( 
    IMessages __RPC_FAR * This,
    long iIndex,
    /* [retval][out] */ IMessage __RPC_FAR *__RPC_FAR *ppMessage);


void __RPC_STUB IMessages_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IMessages_get_Count_Proxy( 
    IMessages __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *varCount);


void __RPC_STUB IMessages_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessages_Delete_Proxy( 
    IMessages __RPC_FAR * This,
    /* [in] */ long iIndex);


void __RPC_STUB IMessages_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMessages_DeleteAll_Proxy( 
    IMessages __RPC_FAR * This);


void __RPC_STUB IMessages_DeleteAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IMessages_get__NewEnum_Proxy( 
    IMessages __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB IMessages_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMessages_INTERFACE_DEFINED__ */


#ifndef __IDropDirectory_INTERFACE_DEFINED__
#define __IDropDirectory_INTERFACE_DEFINED__

/* interface IDropDirectory */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IDropDirectory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("515A6DB9-B262-11d1-956F-00C04FC2D670")
    IDropDirectory : public IDispatch
    {
    public:
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Messages( 
            /* [optional][in] */ BSTR bstrDirName,
            /* [retval][out] */ IMessages __RPC_FAR *__RPC_FAR *Msgs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDropDirectoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDropDirectory __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDropDirectory __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDropDirectory __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDropDirectory __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDropDirectory __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDropDirectory __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDropDirectory __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Messages )( 
            IDropDirectory __RPC_FAR * This,
            /* [optional][in] */ BSTR bstrDirName,
            /* [retval][out] */ IMessages __RPC_FAR *__RPC_FAR *Msgs);
        
        END_INTERFACE
    } IDropDirectoryVtbl;

    interface IDropDirectory
    {
        CONST_VTBL struct IDropDirectoryVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDropDirectory_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDropDirectory_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDropDirectory_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDropDirectory_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDropDirectory_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDropDirectory_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDropDirectory_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDropDirectory_get_Messages(This,bstrDirName,Msgs)	\
    (This)->lpVtbl -> get_Messages(This,bstrDirName,Msgs)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDropDirectory_get_Messages_Proxy( 
    IDropDirectory __RPC_FAR * This,
    /* [optional][in] */ BSTR bstrDirName,
    /* [retval][out] */ IMessages __RPC_FAR *__RPC_FAR *Msgs);


void __RPC_STUB IDropDirectory_get_Messages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDropDirectory_INTERFACE_DEFINED__ */


#ifndef __IBodyParts_INTERFACE_DEFINED__
#define __IBodyParts_INTERFACE_DEFINED__

/* interface IBodyParts */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IBodyParts;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33D47B95-8B95-11D1-82DB-00C04FB1625D")
    IBodyParts : public IDispatch
    {
    public:
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *varCount) = 0;
        
        virtual /* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long iIndex,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ long iIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteAll( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBodyPartsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBodyParts __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBodyParts __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBodyParts __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBodyParts __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBodyParts __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBodyParts __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBodyParts __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IBodyParts __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *varCount);
        
        /* [readonly][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IBodyParts __RPC_FAR * This,
            /* [in] */ long iIndex,
            /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IBodyParts __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IBodyParts __RPC_FAR * This,
            /* [in] */ long iIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteAll )( 
            IBodyParts __RPC_FAR * This);
        
        END_INTERFACE
    } IBodyPartsVtbl;

    interface IBodyParts
    {
        CONST_VTBL struct IBodyPartsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBodyParts_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBodyParts_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBodyParts_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBodyParts_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBodyParts_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBodyParts_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBodyParts_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBodyParts_get_Count(This,varCount)	\
    (This)->lpVtbl -> get_Count(This,varCount)

#define IBodyParts_get_Item(This,iIndex,ppBody)	\
    (This)->lpVtbl -> get_Item(This,iIndex,ppBody)

#define IBodyParts_get__NewEnum(This,retval)	\
    (This)->lpVtbl -> get__NewEnum(This,retval)

#define IBodyParts_Delete(This,iIndex)	\
    (This)->lpVtbl -> Delete(This,iIndex)

#define IBodyParts_DeleteAll(This)	\
    (This)->lpVtbl -> DeleteAll(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyParts_get_Count_Proxy( 
    IBodyParts __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *varCount);


void __RPC_STUB IBodyParts_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [readonly][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IBodyParts_get_Item_Proxy( 
    IBodyParts __RPC_FAR * This,
    /* [in] */ long iIndex,
    /* [retval][out] */ IBodyPart __RPC_FAR *__RPC_FAR *ppBody);


void __RPC_STUB IBodyParts_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IBodyParts_get__NewEnum_Proxy( 
    IBodyParts __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB IBodyParts_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyParts_Delete_Proxy( 
    IBodyParts __RPC_FAR * This,
    /* [in] */ long iIndex);


void __RPC_STUB IBodyParts_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBodyParts_DeleteAll_Proxy( 
    IBodyParts __RPC_FAR * This);


void __RPC_STUB IBodyParts_DeleteAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBodyParts_INTERFACE_DEFINED__ */



#ifndef __CDO_LIBRARY_DEFINED__
#define __CDO_LIBRARY_DEFINED__

/* library CDO */
/* [helpstring][version][uuid] */ 



typedef 
enum MHTMLFlags
    {	SuppressNone	= 0,
	SuppressSrc	= 256,
	SuppressCode	= 512,
	SuppressAll	= 768
    }	MHTMLFlags;


EXTERN_C const IID LIBID_CDO;


#ifndef __EncodingType_MODULE_DEFINED__
#define __EncodingType_MODULE_DEFINED__


/* module EncodingType */
/* [dllname] */ 

const BSTR UsAscii	=	L"us-ascii";

const BSTR Enc7bit	=	L"7bit";

const BSTR QuotedPrintable	=	L"quoted-printable";

const BSTR Base64	=	L"base64";

const BSTR Enc8bit	=	L"8bit";

const BSTR Binary	=	L"binary";

const BSTR Uuencode	=	L"UUENCODE";

#endif /* __EncodingType_MODULE_DEFINED__ */


#ifndef __ContentMediaType_MODULE_DEFINED__
#define __ContentMediaType_MODULE_DEFINED__


/* module ContentMediaType */
/* [dllname] */ 

const BSTR TextPlain	=	L"text/plain";

const BSTR TextHtml	=	L"text/html";

const BSTR MultipartAlt	=	L"multipart/alternative";

const BSTR MultipartRelated	=	L"multipart/related";

const BSTR MultipartMixed	=	L"multipart/mixed";

const BSTR Message	=	L"message/rfc822";

const BSTR MultipartDigest	=	L"multipart/digest";

const BSTR MessagePartial	=	L"message/partial";

const BSTR MessageExternal	=	L"message/external-body";

const BSTR ImageGif	=	L"image/gif";

const BSTR ImageJpeg	=	L"image/jpeg";

#endif /* __ContentMediaType_MODULE_DEFINED__ */


#ifndef __ConfigProperties_MODULE_DEFINED__
#define __ConfigProperties_MODULE_DEFINED__


/* module ConfigProperties */
/* [dllname] */ 

const BSTR NNTPServer	=	L"NNTPServer";

const BSTR NNTPServerPickupDirectory	=	L"NNTPServerPickupDirectory";

const BSTR NNTPServerPort	=	L"NNTPServerPort";

const BSTR NNTPUserDisplayName	=	L"NNTPUserDisplayName";

const BSTR NNTPUserEmailAddress	=	L"NNTPUserEmailAddress";

const BSTR NNTPUserReplyEmailAddress	=	L"NNTPUserReplyEmailAddress";

const BSTR SMTPServer	=	L"SMTPServer";

const BSTR SMTPServerPickupDirectory	=	L"SMTPServerPickupDirectory";

const BSTR SMTPServerPort	=	L"SMTPServerPort";

const BSTR SMTPUserDisplayName	=	L"SMTPUserDisplayName";

const BSTR SMTPUserEmailAddress	=	L"SMTPUserEmailAddress";

const BSTR SMTPUserReplyEmailAddress	=	L"SMTPUserReplyEmailAddress";

#endif /* __ConfigProperties_MODULE_DEFINED__ */


#ifndef __Namespace_MODULE_DEFINED__
#define __Namespace_MODULE_DEFINED__


/* module Namespace */
/* [dllname] */ 

const BSTR Messaging	=	L"http://www.iana.org/standards/rfc822/";

const BSTR Mime	=	L"http://www.iana.org/standards/mime/";

#endif /* __Namespace_MODULE_DEFINED__ */


#ifndef __MimeProperties_MODULE_DEFINED__
#define __MimeProperties_MODULE_DEFINED__


/* module MimeProperties */
/* [dllname] */ 

const BSTR ContentBase	=	L"http://www.iana.org/standards/mime/Content-Base";

const BSTR ContentDescription	=	L"http://www.iana.org/standards/mime/Content-Description";

const BSTR ContentDisposition	=	L"http://www.iana.org/standards/mime/Content-Disposition";

const BSTR ContentId	=	L"http://www.iana.org/standards/mime/Content-ID";

const BSTR ContentLanguage	=	L"http://www.iana.org/standards/mime/Content-Language";

const BSTR ContentMediaType	=	L"http://www.iana.org/standards/mime/Content-Media-Type";

const BSTR ContentTransferEncoding	=	L"http://www.iana.org/standards/mime/Content-Transfer-Encoding";

const BSTR ContentType	=	L"http://www.iana.org/standards/mime/Content-Type";

#endif /* __MimeProperties_MODULE_DEFINED__ */


#ifndef __MessageProperties_MODULE_DEFINED__
#define __MessageProperties_MODULE_DEFINED__


/* module MessageProperties */
/* [dllname] */ 

const BSTR Comments	=	L"http://www.iana.org/standards/rfc822/Comments";

const BSTR Approved	=	L"http://www.iana.org/standards/rfc822/Approved";

const BSTR BodyStructure	=	L"http://www.iana.org/standards/rfc822/BodyStructure";

const BSTR BCC	=	L"http://www.iana.org/standards/rfc822/Bcc";

const BSTR CC	=	L"http://www.iana.org/standards/rfc822/CC";

const BSTR Date	=	L"http://www.iana.org/standards/rfc822/Date";

const BSTR DateReceived	=	L"http://www.iana.org/standards/rfc822/DateReceived";

const BSTR DispositionNotificationOption	=	L"http://www.iana.org/standards/rfc822/Disposition-Notification-Option";

const BSTR DispositionNotificationTo	=	L"http://www.iana.org/standards/rfc822/Disposition-Notification-To";

const BSTR Distribution	=	L"http://www.iana.org/standards/rfc822/Distribution";

const BSTR Expires	=	L"http://www.iana.org/standards/rfc822/Expires";

const BSTR FollowupTo	=	L"http://www.iana.org/standards/rfc822/Followup-To";

const BSTR From	=	L"http://www.iana.org/standards/rfc822/From";

const BSTR HasAttachment	=	L"http://www.iana.org/standards/rfc822/HasAttachment";

const BSTR HTMLDescription	=	L"http://www.iana.org/standards/rfc822/HTMLDescription";

const BSTR Importance	=	L"http://www.iana.org/standards/rfc822/Importance";

const BSTR InReplyTo	=	L"http://www.iana.org/standards/rfc822/In-Reply-To";

const BSTR Keywords	=	L"http://www.iana.org/standards/rfc822/Keywords";

const BSTR Lines	=	L"http://www.iana.org/standards/rfc822/Lines";

const BSTR MessageId	=	L"http://www.iana.org/standards/rfc822/Message-ID";

const BSTR MimeVersion	=	L"http://www.iana.org/standards/rfc822/MIME-Version";

const BSTR Newsgroups	=	L"http://www.iana.org/standards/rfc822/Newsgroups";

const BSTR NNTPPostingHost	=	L"http://www.iana.org/standards/rfc822/NNTP-Posting-Host";

const BSTR NNTPPostingUser	=	L"http://www.iana.org/standards/rfc822/NNTP-Posting-User";

const BSTR Path	=	L"http://www.iana.org/standards/rfc822/Path";

const BSTR Priority	=	L"http://www.iana.org/standards/rfc822/Priority";

const BSTR References	=	L"http://www.iana.org/standards/rfc822/References";

const BSTR RelayVersion	=	L"http://www.iana.org/standards/rfc822/Relay-Version";

const BSTR ReplyTo	=	L"http://www.iana.org/standards/rfc822/Reply-To";

const BSTR ReturnPath	=	L"http://www.iana.org/standards/rfc822/Return-Path";

const BSTR Sender	=	L"http://www.iana.org/standards/rfc822/Sender";

const BSTR Sensitivity	=	L"http://www.iana.org/standards/rfc822/Sensitivity";

const BSTR Subject	=	L"http://www.iana.org/standards/rfc822/Subject";

const BSTR Submitted	=	L"http://www.iana.org/standards/rfc822/Submitted";

const BSTR Summary	=	L"http://www.iana.org/standards/rfc822/Summary";

const BSTR TextDescription	=	L"http://www.iana.org/standards/rfc822/TextDescription";

const BSTR To	=	L"http://www.iana.org/standards/rfc822/To";

const BSTR XMailer	=	L"http://www.iana.org/standards/rfc822/X-Mailer";

const BSTR Xref	=	L"http://www.iana.org/standards/rfc822/Xref";

#endif /* __MessageProperties_MODULE_DEFINED__ */

#ifndef __ISMTPOnArrival_INTERFACE_DEFINED__
#define __ISMTPOnArrival_INTERFACE_DEFINED__

/* interface ISMTPOnArrival */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISMTPOnArrival;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("861E0DD0-AEE7-11d1-956C-00C04FC2D670")
    ISMTPOnArrival : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnArrival( 
            /* [in] */ IMessage __RPC_FAR *pIMsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISMTPOnArrivalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISMTPOnArrival __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISMTPOnArrival __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISMTPOnArrival __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISMTPOnArrival __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISMTPOnArrival __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISMTPOnArrival __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISMTPOnArrival __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnArrival )( 
            ISMTPOnArrival __RPC_FAR * This,
            /* [in] */ IMessage __RPC_FAR *pIMsg);
        
        END_INTERFACE
    } ISMTPOnArrivalVtbl;

    interface ISMTPOnArrival
    {
        CONST_VTBL struct ISMTPOnArrivalVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISMTPOnArrival_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISMTPOnArrival_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISMTPOnArrival_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISMTPOnArrival_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISMTPOnArrival_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISMTPOnArrival_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISMTPOnArrival_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISMTPOnArrival_OnArrival(This,pIMsg)	\
    (This)->lpVtbl -> OnArrival(This,pIMsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMTPOnArrival_OnArrival_Proxy( 
    ISMTPOnArrival __RPC_FAR * This,
    /* [in] */ IMessage __RPC_FAR *pIMsg);


void __RPC_STUB ISMTPOnArrival_OnArrival_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISMTPOnArrival_INTERFACE_DEFINED__ */


#ifndef __INNTPOnPost_INTERFACE_DEFINED__
#define __INNTPOnPost_INTERFACE_DEFINED__

/* interface INNTPOnPost */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_INNTPOnPost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("850BB4D4-AEF5-11d1-956C-00C04FC2D670")
    INNTPOnPost : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnPost( 
            /* [in] */ IMessage __RPC_FAR *pIMsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INNTPOnPostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INNTPOnPost __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INNTPOnPost __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INNTPOnPost __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INNTPOnPost __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INNTPOnPost __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INNTPOnPost __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INNTPOnPost __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnPost )( 
            INNTPOnPost __RPC_FAR * This,
            /* [in] */ IMessage __RPC_FAR *pIMsg);
        
        END_INTERFACE
    } INNTPOnPostVtbl;

    interface INNTPOnPost
    {
        CONST_VTBL struct INNTPOnPostVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INNTPOnPost_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INNTPOnPost_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INNTPOnPost_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INNTPOnPost_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INNTPOnPost_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INNTPOnPost_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INNTPOnPost_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INNTPOnPost_OnPost(This,pIMsg)	\
    (This)->lpVtbl -> OnPost(This,pIMsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INNTPOnPost_OnPost_Proxy( 
    INNTPOnPost __RPC_FAR * This,
    /* [in] */ IMessage __RPC_FAR *pIMsg);


void __RPC_STUB INNTPOnPost_OnPost_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INNTPOnPost_INTERFACE_DEFINED__ */


#ifndef __INNTPOnPostFinal_INTERFACE_DEFINED__
#define __INNTPOnPostFinal_INTERFACE_DEFINED__

/* interface INNTPOnPostFinal */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_INNTPOnPostFinal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("850BB4D5-AEF5-11d1-956C-00C04FC2D670")
    INNTPOnPostFinal : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnPostFinal( 
            /* [in] */ IMessage __RPC_FAR *pIMsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INNTPOnPostFinalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INNTPOnPostFinal __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INNTPOnPostFinal __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INNTPOnPostFinal __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INNTPOnPostFinal __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INNTPOnPostFinal __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INNTPOnPostFinal __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INNTPOnPostFinal __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnPostFinal )( 
            INNTPOnPostFinal __RPC_FAR * This,
            /* [in] */ IMessage __RPC_FAR *pIMsg);
        
        END_INTERFACE
    } INNTPOnPostFinalVtbl;

    interface INNTPOnPostFinal
    {
        CONST_VTBL struct INNTPOnPostFinalVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INNTPOnPostFinal_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INNTPOnPostFinal_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INNTPOnPostFinal_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INNTPOnPostFinal_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INNTPOnPostFinal_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INNTPOnPostFinal_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INNTPOnPostFinal_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INNTPOnPostFinal_OnPostFinal(This,pIMsg)	\
    (This)->lpVtbl -> OnPostFinal(This,pIMsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INNTPOnPostFinal_OnPostFinal_Proxy( 
    INNTPOnPostFinal __RPC_FAR * This,
    /* [in] */ IMessage __RPC_FAR *pIMsg);


void __RPC_STUB INNTPOnPostFinal_OnPostFinal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INNTPOnPostFinal_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Message;

#ifdef __cplusplus

class DECLSPEC_UUID("33D47B70-8B95-11D1-82DB-00C04FB1625D")
Message;
#endif

EXTERN_C const CLSID CLSID_Config;

#ifdef __cplusplus

class DECLSPEC_UUID("33D47B72-8B95-11D1-82DB-00C04FB1625D")
Config;
#endif

EXTERN_C const CLSID CLSID_Stream;

#ifdef __cplusplus

class DECLSPEC_UUID("33D47B73-8B95-11D1-82DB-00C04FB1625D")
Stream;
#endif

EXTERN_C const CLSID CLSID_DropDirectory;

#ifdef __cplusplus

class DECLSPEC_UUID("65F5F799-B2C5-11d1-956F-00C04FC2D670")
DropDirectory;
#endif
#endif /* __CDO_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
