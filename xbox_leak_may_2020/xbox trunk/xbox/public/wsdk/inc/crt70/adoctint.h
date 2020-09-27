//--------------------------------------------------------------------
// ADOCTINT.h
//
// Copyright: (c) 1996-1998 Microsoft Corporation.
//
//
//
// Contents: ADO Interface header
//
// Comment:
//--------------------------------------------------------------------
#ifndef _ADOCTINT_H_
#define _ADOCTINT_H_

#ifndef _INC_TCHAR
#include <tchar.h>
#endif

#pragma warning( disable: 4049 )  /* more than 64k source lines */
/* this ALWAYS GENERATED file contains the definitions for the interfaces */
 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Apr 07 05:37:28 2000
 */
/* Compiler settings for AdoCat.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )
/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif
#include "rpc.h"
#include "rpcndr.h"
#ifndef __m_adocat_h__
#define __m_adocat_h__
/* Forward Declarations */ 
#ifndef ___ADOCollection_FWD_DEFINED__
#define ___ADOCollection_FWD_DEFINED__
typedef interface _ADOADOCollection _ADOCollection;
#endif 	/* ___ADOCollection_FWD_DEFINED__ */
#ifndef ___ADODynaCollection_FWD_DEFINED__
#define ___ADODynaCollection_FWD_DEFINED__
typedef interface _ADODynaADOCollection _ADODynaCollection;
#endif 	/* ___ADODynaCollection_FWD_DEFINED__ */
#ifndef ___Catalog_FWD_DEFINED__
#define ___Catalog_FWD_DEFINED__
typedef interface _ADOCatalog _Catalog;
#endif 	/* ___Catalog_FWD_DEFINED__ */
#ifndef ___Table_FWD_DEFINED__
#define ___Table_FWD_DEFINED__
typedef interface _ADOTable _Table;
#endif 	/* ___Table_FWD_DEFINED__ */
#ifndef ___Group25_FWD_DEFINED__
#define ___Group25_FWD_DEFINED__
typedef interface _Group25 _Group25;
#endif 	/* ___Group25_FWD_DEFINED__ */
#ifndef ___Group_FWD_DEFINED__
#define ___Group_FWD_DEFINED__
typedef interface _ADOGroup _Group;
#endif 	/* ___Group_FWD_DEFINED__ */
#ifndef ___User25_FWD_DEFINED__
#define ___User25_FWD_DEFINED__
typedef interface _User25 _User25;
#endif 	/* ___User25_FWD_DEFINED__ */
#ifndef ___User_FWD_DEFINED__
#define ___User_FWD_DEFINED__
typedef interface _ADOUser _User;
#endif 	/* ___User_FWD_DEFINED__ */
#ifndef ___Column_FWD_DEFINED__
#define ___Column_FWD_DEFINED__
typedef interface _ADOColumn _Column;
#endif 	/* ___Column_FWD_DEFINED__ */
#ifndef ___Index_FWD_DEFINED__
#define ___Index_FWD_DEFINED__
typedef interface _ADOIndex _Index;
#endif 	/* ___Index_FWD_DEFINED__ */
#ifndef ___Key_FWD_DEFINED__
#define ___Key_FWD_DEFINED__
typedef interface _ADOKey _Key;
#endif 	/* ___Key_FWD_DEFINED__ */
#ifndef __View_FWD_DEFINED__
#define __View_FWD_DEFINED__
typedef interface ADOView View;
#endif 	/* __View_FWD_DEFINED__ */
#ifndef __Procedure_FWD_DEFINED__
#define __Procedure_FWD_DEFINED__
typedef interface ADOProcedure Procedure;
#endif 	/* __Procedure_FWD_DEFINED__ */
#ifndef __Catalog_FWD_DEFINED__
#define __Catalog_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOCatalog Catalog;
#else
typedef struct ADOCatalog Catalog;
#endif /* __cplusplus */
#endif 	/* __Catalog_FWD_DEFINED__ */
#ifndef __Table_FWD_DEFINED__
#define __Table_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOTable Table;
#else
typedef struct ADOTable Table;
#endif /* __cplusplus */
#endif 	/* __Table_FWD_DEFINED__ */
#ifndef __Property_FWD_DEFINED__
#define __Property_FWD_DEFINED__
typedef interface ADOProperty Property;
#endif 	/* __Property_FWD_DEFINED__ */
#ifndef __Group_FWD_DEFINED__
#define __Group_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOGroup Group;
#else
typedef struct ADOGroup Group;
#endif /* __cplusplus */
#endif 	/* __Group_FWD_DEFINED__ */
#ifndef __User_FWD_DEFINED__
#define __User_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOUser User;
#else
typedef struct ADOUser User;
#endif /* __cplusplus */
#endif 	/* __User_FWD_DEFINED__ */
#ifndef __Column_FWD_DEFINED__
#define __Column_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOColumn Column;
#else
typedef struct ADOColumn Column;
#endif /* __cplusplus */
#endif 	/* __Column_FWD_DEFINED__ */
#ifndef __Index_FWD_DEFINED__
#define __Index_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOIndex Index;
#else
typedef struct ADOIndex Index;
#endif /* __cplusplus */
#endif 	/* __Index_FWD_DEFINED__ */
#ifndef __Key_FWD_DEFINED__
#define __Key_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOKey Key;
#else
typedef struct ADOKey Key;
#endif /* __cplusplus */
#endif 	/* __Key_FWD_DEFINED__ */
#ifndef __Tables_FWD_DEFINED__
#define __Tables_FWD_DEFINED__
typedef interface ADOTables Tables;
#endif 	/* __Tables_FWD_DEFINED__ */
#ifndef __Columns_FWD_DEFINED__
#define __Columns_FWD_DEFINED__
typedef interface ADOColumns Columns;
#endif 	/* __Columns_FWD_DEFINED__ */
#ifndef __Procedures_FWD_DEFINED__
#define __Procedures_FWD_DEFINED__
typedef interface ADOProcedures Procedures;
#endif 	/* __Procedures_FWD_DEFINED__ */
#ifndef __Views_FWD_DEFINED__
#define __Views_FWD_DEFINED__
typedef interface ADOViews Views;
#endif 	/* __Views_FWD_DEFINED__ */
#ifndef __Indexes_FWD_DEFINED__
#define __Indexes_FWD_DEFINED__
typedef interface ADOIndexes Indexes;
#endif 	/* __Indexes_FWD_DEFINED__ */
#ifndef __Keys_FWD_DEFINED__
#define __Keys_FWD_DEFINED__
typedef interface ADOKeys Keys;
#endif 	/* __Keys_FWD_DEFINED__ */
#ifndef __Users_FWD_DEFINED__
#define __Users_FWD_DEFINED__
typedef interface ADOUsers Users;
#endif 	/* __Users_FWD_DEFINED__ */
#ifndef __Groups_FWD_DEFINED__
#define __Groups_FWD_DEFINED__
typedef interface ADOGroups Groups;
#endif 	/* __Groups_FWD_DEFINED__ */
#ifndef __Properties_FWD_DEFINED__
#define __Properties_FWD_DEFINED__
typedef interface ADOProperties Properties;
#endif 	/* __Properties_FWD_DEFINED__ */
/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#ifdef __cplusplus
extern "C"{
#endif 
void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 
/* interface __MIDL_itf_AdoCat_0000 */
/* [local] */ 
typedef /* [helpcontext] */ 
enum RuleEnum
    {	adRINone	= 0,
	adRICascade	= 1,
	adRISetNull	= 2,
	adRISetDefault	= 3
    }	RuleEnum;
typedef /* [helpcontext] */ 
enum KeyTypeEnum
    {	adKeyPrimary	= 1,
	adKeyForeign	= 2,
	adKeyUnique	= 3
    }	KeyTypeEnum;
typedef /* [helpcontext] */ 
enum ActionEnum
    {	adAccessGrant	= 1,
	adAccessSet	= 2,
	adAccessDeny	= 3,
	adAccessRevoke	= 4
    }	ActionEnum;
typedef /* [helpcontext] */ 
enum ColumnAttributesEnum
    {	adColFixed	= 1,
	adColNullable	= 2
    }	ColumnAttributesEnum;
typedef /* [helpcontext] */ 
enum SortOrderEnum
    {	adSortAscending	= 1,
	adSortDescending	= 2
    }	SortOrderEnum;
typedef /* [helpcontext] */ 
enum RightsEnum
    {	adRightNone	= 0L,
	adRightDrop	= 0x100L,
	adRightExclusive	= 0x200L,
	adRightReadDesign	= 0x400L,
	adRightWriteDesign	= 0x800L,
	adRightWithGrant	= 0x1000L,
	adRightReference	= 0x2000L,
	adRightCreate	= 0x4000L,
	adRightInsert	= 0x8000L,
	adRightDelete	= 0x10000L,
	adRightReadPermissions	= 0x20000L,
	adRightWritePermissions	= 0x40000L,
	adRightWriteOwner	= 0x80000L,
	adRightMaximumAllowed	= 0x2000000L,
	adRightFull	= 0x10000000L,
	adRightExecute	= 0x20000000L,
	adRightUpdate	= 0x40000000L,
	adRightRead	= 0x80000000L
    }	RightsEnum;
typedef /* [helpcontext] */ 
#ifdef _ADOINT_H_  //Avoid conflicting with ADO def
 class dummy dummy;
#else
enum DataTypeEnum
    {	adEmpty	= 0,
	adTinyInt	= 16,
	adSmallInt	= 2,
	adInteger	= 3,
	adBigInt	= 20,
	adUnsignedTinyInt	= 17,
	adUnsignedSmallInt	= 18,
	adUnsignedInt	= 19,
	adUnsignedBigInt	= 21,
	adSingle	= 4,
	adDouble	= 5,
	adCurrency	= 6,
	adDecimal	= 14,
	adNumeric	= 131,
	adBoolean	= 11,
	adError	= 10,
	adUserDefined	= 132,
	adVariant	= 12,
	adIDispatch	= 9,
	adIUnknown	= 13,
	adGUID	= 72,
	adDate	= 7,
	adDBDate	= 133,
	adDBTime	= 134,
	adDBTimeStamp	= 135,
	adBSTR	= 8,
	adChar	= 129,
	adVarChar	= 200,
	adLongVarChar	= 201,
	adWChar	= 130,
	adVarWChar	= 202,
	adLongVarWChar	= 203,
	adBinary	= 128,
	adVarBinary	= 204,
	adLongVarBinary	= 205,
	adChapter	= 136,
	adFileTime	= 64,
	adPropVariant	= 138,
	adVarNumeric	= 139
}	DataTypeEnum;
#endif //ifdef _ADOINT.H_
typedef /* [helpcontext] */ 
enum AllowNullsEnum
    {	adIndexNullsAllow	= 0,
	adIndexNullsDisallow	= 1,
	adIndexNullsIgnore	= 2,
	adIndexNullsIgnoreAny	= 4
    }	AllowNullsEnum;
typedef /* [helpcontext] */ 
enum ObjectTypeEnum
    {	adPermObjProviderSpecific	= -1,
	adPermObjTable	= 1,
	adPermObjColumn	= 2,
	adPermObjDatabase	= 3,
	adPermObjProcedure	= 4,
	adPermObjView	= 5
    }	ObjectTypeEnum;
typedef /* [helpcontext] */ 
enum InheritTypeEnum
    {	adInheritNone	= 0,
	adInheritObjects	= 1,
	adInheritContainers	= 2,
	adInheritBoth	= 3,
	adInheritNoPropogate	= 4
    }	InheritTypeEnum;
extern RPC_IF_HANDLE __MIDL_itf_AdoCat_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_AdoCat_0000_v0_0_s_ifspec;
#ifndef __ADOX_LIBRARY_DEFINED__
#define __ADOX_LIBRARY_DEFINED__
/* library ADOX */
/* [helpstring][helpfile][version][uuid] */ 
EXTERN_C const IID LIBID_ADOX;
#ifndef ___ADOCollection_INTERFACE_DEFINED__
#define ___ADOCollection_INTERFACE_DEFINED__
/* interface _ADOADOCollection */
/* [object][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID__ADOCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000512-0000-0010-8000-00AA006D2EA4")
    _ADOADOCollection : public IDispatch
    {
    public:
        virtual /* [helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *c) = 0;
        
        virtual /* [id][restricted] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _ADOCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOADOCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOADOCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOADOCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOADOCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOADOCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOADOCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOADOCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            _ADOADOCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            _ADOADOCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            _ADOADOCollection __RPC_FAR * This);
        
        END_INTERFACE
    } _ADOCollectionVtbl;
    interface _ADOCollection
    {
        CONST_VTBL struct _ADOCollectionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _ADOCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _ADOCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _ADOCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _ADOCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _ADOCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _ADOCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _ADOCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Collection_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define _ADOCollection__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define _ADOCollection_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget] */ HRESULT STDMETHODCALLTYPE _Collection_get_Count_Proxy( 
    _ADOADOCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *c);
void __RPC_STUB _Collection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [id][restricted] */ HRESULT STDMETHODCALLTYPE _ADOCollection__NewEnum_Proxy( 
    _ADOADOCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _ADOCollection__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE _ADOCollection_Refresh_Proxy( 
    _ADOADOCollection __RPC_FAR * This);
void __RPC_STUB _ADOCollection_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___ADOCollection_INTERFACE_DEFINED__ */
#ifndef ___ADODynaCollection_INTERFACE_DEFINED__
#define ___ADODynaCollection_INTERFACE_DEFINED__
/* interface _ADODynaADOCollection */
/* [object][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID__ADODynaCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000513-0000-0010-8000-00AA006D2EA4")
_ADODynaADOCollection : public _ADOCollection
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ IDispatch __RPC_FAR *Object) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _ADODynaCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADODynaADOCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADODynaADOCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            _ADODynaADOCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *Object);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            _ADODynaADOCollection __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } _ADODynaCollectionVtbl;
    interface _ADODynaCollection
    {
        CONST_VTBL struct _ADODynaCollectionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _ADODynaCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _ADODynaCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _ADODynaCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _ADODynaCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _ADODynaCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _ADODynaCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _ADODynaCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _DynaCollection_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define _ADODynaCollection__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define _ADODynaCollection_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define _ADODynaCollection_Append(This,Object)	\
    (This)->lpVtbl -> Append(This,Object)
#define _ADODynaCollection_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
HRESULT STDMETHODCALLTYPE _ADODynaCollection_Append_Proxy( 
    _ADODynaADOCollection __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *Object);
void __RPC_STUB _ADODynaCollection_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE _ADODynaCollection_Delete_Proxy( 
    _ADODynaADOCollection __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB _ADODynaCollection_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___ADODynaCollection_INTERFACE_DEFINED__ */
#ifndef ___Catalog_INTERFACE_DEFINED__
#define ___Catalog_INTERFACE_DEFINED__
/* interface _ADOCatalog */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID__Catalog;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000603-0000-0010-8000-00AA006D2EA4")
    _ADOCatalog : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tables( 
            /* [retval][out] */ ADOTables __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActiveConnection( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ActiveConnection( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_ActiveConnection( 
            /* [in] */ IDispatch __RPC_FAR *pCon) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Procedures( 
            /* [retval][out] */ ADOProcedures __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Views( 
            /* [retval][out] */ ADOViews __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Groups( 
            /* [retval][out] */ ADOGroups __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Users( 
            /* [retval][out] */ ADOUsers __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ BSTR ConnectString,
            /* [retval][out] */ VARIANT __RPC_FAR *Connection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetObjectOwner( 
            /* [in] */ BSTR ObjectName,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [optional][in] */ VARIANT ObjectTypeId,
            /* [retval][out] */ BSTR __RPC_FAR *OwnerName) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetObjectOwner( 
            /* [in] */ BSTR ObjectName,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [in] */ BSTR UserName,
            /* [optional][in] */ VARIANT ObjectTypeId) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _CatalogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOCatalog __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOCatalog __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOCatalog __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tables )( 
            _ADOCatalog __RPC_FAR * This,
            /* [retval][out] */ ADOTables __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            _ADOCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveConnection )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pCon);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Procedures )( 
            _ADOCatalog __RPC_FAR * This,
            /* [retval][out] */ ADOProcedures __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Views )( 
            _ADOCatalog __RPC_FAR * This,
            /* [retval][out] */ ADOViews __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Groups )( 
            _ADOCatalog __RPC_FAR * This,
            /* [retval][out] */ ADOGroups __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Users )( 
            _ADOCatalog __RPC_FAR * This,
            /* [retval][out] */ ADOUsers __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ BSTR ConnectString,
            /* [retval][out] */ VARIANT __RPC_FAR *Connection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetObjectOwner )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ BSTR ObjectName,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [optional][in] */ VARIANT ObjectTypeId,
            /* [retval][out] */ BSTR __RPC_FAR *OwnerName);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetObjectOwner )( 
            _ADOCatalog __RPC_FAR * This,
            /* [in] */ BSTR ObjectName,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [in] */ BSTR UserName,
            /* [optional][in] */ VARIANT ObjectTypeId);
        
        END_INTERFACE
    } _CatalogVtbl;
    interface _Catalog
    {
        CONST_VTBL struct _CatalogVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Catalog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Catalog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Catalog_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Catalog_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Catalog_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Catalog_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Catalog_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Catalog_get_Tables(This,ppvObject)	\
    (This)->lpVtbl -> get_Tables(This,ppvObject)
#define _Catalog_get_ActiveConnection(This,pVal)	\
    (This)->lpVtbl -> get_ActiveConnection(This,pVal)
#define _Catalog_put_ActiveConnection(This,newVal)	\
    (This)->lpVtbl -> put_ActiveConnection(This,newVal)
#define _Catalog_putref_ActiveConnection(This,pCon)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,pCon)
#define _Catalog_get_Procedures(This,ppvObject)	\
    (This)->lpVtbl -> get_Procedures(This,ppvObject)
#define _Catalog_get_Views(This,ppvObject)	\
    (This)->lpVtbl -> get_Views(This,ppvObject)
#define _Catalog_get_Groups(This,ppvObject)	\
    (This)->lpVtbl -> get_Groups(This,ppvObject)
#define _Catalog_get_Users(This,ppvObject)	\
    (This)->lpVtbl -> get_Users(This,ppvObject)
#define _Catalog_Create(This,ConnectString,Connection)	\
    (This)->lpVtbl -> Create(This,ConnectString,Connection)
#define _Catalog_GetObjectOwner(This,ObjectName,ObjectType,ObjectTypeId,OwnerName)	\
    (This)->lpVtbl -> GetObjectOwner(This,ObjectName,ObjectType,ObjectTypeId,OwnerName)
#define _Catalog_SetObjectOwner(This,ObjectName,ObjectType,UserName,ObjectTypeId)	\
    (This)->lpVtbl -> SetObjectOwner(This,ObjectName,ObjectType,UserName,ObjectTypeId)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Catalog_get_Tables_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [retval][out] */ ADOTables __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Catalog_get_Tables_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Catalog_get_ActiveConnection_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB _Catalog_get_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Catalog_put_ActiveConnection_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [in] */ VARIANT newVal);
void __RPC_STUB _Catalog_put_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE _Catalog_putref_ActiveConnection_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pCon);
void __RPC_STUB _Catalog_putref_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Catalog_get_Procedures_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [retval][out] */ ADOProcedures __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Catalog_get_Procedures_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Catalog_get_Views_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [retval][out] */ ADOViews __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Catalog_get_Views_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Catalog_get_Groups_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [retval][out] */ ADOGroups __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Catalog_get_Groups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Catalog_get_Users_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [retval][out] */ ADOUsers __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Catalog_get_Users_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Catalog_Create_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [in] */ BSTR ConnectString,
    /* [retval][out] */ VARIANT __RPC_FAR *Connection);
void __RPC_STUB _Catalog_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Catalog_GetObjectOwner_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [in] */ BSTR ObjectName,
    /* [in] */ ObjectTypeEnum ObjectType,
    /* [optional][in] */ VARIANT ObjectTypeId,
    /* [retval][out] */ BSTR __RPC_FAR *OwnerName);
void __RPC_STUB _Catalog_GetObjectOwner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Catalog_SetObjectOwner_Proxy( 
    _ADOCatalog __RPC_FAR * This,
    /* [in] */ BSTR ObjectName,
    /* [in] */ ObjectTypeEnum ObjectType,
    /* [in] */ BSTR UserName,
    /* [optional][in] */ VARIANT ObjectTypeId);
void __RPC_STUB _Catalog_SetObjectOwner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Catalog_INTERFACE_DEFINED__ */
#ifndef ___Table_INTERFACE_DEFINED__
#define ___Table_INTERFACE_DEFINED__
/* interface _ADOTable */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID__Table;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000610-0000-0010-8000-00AA006D2EA4")
    _ADOTable : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Columns( 
            /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Indexes( 
            /* [retval][out] */ ADOIndexes __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Keys( 
            /* [retval][out] */ ADOKeys __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DateCreated( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DateModified( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParentCatalog( 
            /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ParentCatalog( 
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_ParentCatalog( 
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _TableVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOTable __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOTable __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOTable __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOTable __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOTable __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOTable __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOTable __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Columns )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _ADOTable __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Indexes )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ ADOIndexes __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Keys )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ ADOKeys __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateCreated )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateModified )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParentCatalog )( 
            _ADOTable __RPC_FAR * This,
            /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ParentCatalog )( 
            _ADOTable __RPC_FAR * This,
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ParentADOCatalog )( 
            _ADOTable __RPC_FAR * This,
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _TableVtbl;
    interface _Table
    {
        CONST_VTBL struct _TableVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Table_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Table_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Table_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Table_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Table_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Table_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Table_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Table_get_Columns(This,ppvObject)	\
    (This)->lpVtbl -> get_Columns(This,ppvObject)
#define _Table_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define _Table_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)
#define _Table_get_Type(This,pVal)	\
    (This)->lpVtbl -> get_Type(This,pVal)
#define _Table_get_Indexes(This,ppvObject)	\
    (This)->lpVtbl -> get_Indexes(This,ppvObject)
#define _Table_get_Keys(This,ppvObject)	\
    (This)->lpVtbl -> get_Keys(This,ppvObject)
#define _Table_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _Table_get_DateCreated(This,pVal)	\
    (This)->lpVtbl -> get_DateCreated(This,pVal)
#define _Table_get_DateModified(This,pVal)	\
    (This)->lpVtbl -> get_DateModified(This,pVal)
#define _Table_get_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> get_ParentCatalog(This,ppvObject)
#define _Table_put_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> put_ParentCatalog(This,ppvObject)
#define _Table_putref_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> putref_ParentCatalog(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_Columns_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Table_get_Columns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_Name_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _Table_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Table_put_Name_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [in] */ BSTR newVal);
void __RPC_STUB _Table_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_Type_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _Table_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_Indexes_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ ADOIndexes __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Table_get_Indexes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_Keys_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ ADOKeys __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Table_get_Keys_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_Properties_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Table_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_DateCreated_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB _Table_get_DateCreated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_DateModified_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB _Table_get_DateModified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Table_get_ParentCatalog_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Table_get_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Table_put_ParentCatalog_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
void __RPC_STUB _Table_put_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE _Table_putref_ParentCatalog_Proxy( 
    _ADOTable __RPC_FAR * This,
    /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
void __RPC_STUB _Table_putref_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Table_INTERFACE_DEFINED__ */
#ifndef ___Group25_INTERFACE_DEFINED__
#define ___Group25_INTERFACE_DEFINED__
/* interface _Group25 */
/* [helpcontext][unique][dual][uuid][hidden][nonextensible][object] */ 
EXTERN_C const IID IID__Group25;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000616-0000-0010-8000-00AA006D2EA4")
    _Group25 : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetPermissions( 
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [optional][in] */ VARIANT ObjectTypeId,
            /* [retval][out] */ RightsEnum __RPC_FAR *Rights) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetPermissions( 
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [in] */ ActionEnum Action,
            /* [in] */ RightsEnum Rights,
            /* [defaultvalue][in] */ InheritTypeEnum Inherit,
            /* [optional][in] */ VARIANT ObjectTypeId) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Users( 
            /* [retval][out] */ ADOUsers __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _Group25Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _Group25 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _Group25 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _Group25 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _Group25 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _Group25 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _Group25 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _Group25 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _Group25 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _Group25 __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPermissions )( 
            _Group25 __RPC_FAR * This,
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [optional][in] */ VARIANT ObjectTypeId,
            /* [retval][out] */ RightsEnum __RPC_FAR *Rights);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPermissions )( 
            _Group25 __RPC_FAR * This,
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [in] */ ActionEnum Action,
            /* [in] */ RightsEnum Rights,
            /* [defaultvalue][in] */ InheritTypeEnum Inherit,
            /* [optional][in] */ VARIANT ObjectTypeId);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Users )( 
            _Group25 __RPC_FAR * This,
            /* [retval][out] */ ADOUsers __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _Group25Vtbl;
    interface _Group25
    {
        CONST_VTBL struct _Group25Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Group25_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Group25_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Group25_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Group25_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Group25_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Group25_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Group25_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Group25_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define _Group25_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)
#define _Group25_GetPermissions(This,Name,ObjectType,ObjectTypeId,Rights)	\
    (This)->lpVtbl -> GetPermissions(This,Name,ObjectType,ObjectTypeId,Rights)
#define _Group25_SetPermissions(This,Name,ObjectType,Action,Rights,Inherit,ObjectTypeId)	\
    (This)->lpVtbl -> SetPermissions(This,Name,ObjectType,Action,Rights,Inherit,ObjectTypeId)
#define _Group25_get_Users(This,ppvObject)	\
    (This)->lpVtbl -> get_Users(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Group25_get_Name_Proxy( 
    _Group25 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _Group25_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Group25_put_Name_Proxy( 
    _Group25 __RPC_FAR * This,
    /* [in] */ BSTR newVal);
void __RPC_STUB _Group25_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Group25_GetPermissions_Proxy( 
    _Group25 __RPC_FAR * This,
    /* [in] */ VARIANT Name,
    /* [in] */ ObjectTypeEnum ObjectType,
    /* [optional][in] */ VARIANT ObjectTypeId,
    /* [retval][out] */ RightsEnum __RPC_FAR *Rights);
void __RPC_STUB _Group25_GetPermissions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Group25_SetPermissions_Proxy( 
    _Group25 __RPC_FAR * This,
    /* [in] */ VARIANT Name,
    /* [in] */ ObjectTypeEnum ObjectType,
    /* [in] */ ActionEnum Action,
    /* [in] */ RightsEnum Rights,
    /* [defaultvalue][in] */ InheritTypeEnum Inherit,
    /* [optional][in] */ VARIANT ObjectTypeId);
void __RPC_STUB _Group25_SetPermissions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Group25_get_Users_Proxy( 
    _Group25 __RPC_FAR * This,
    /* [retval][out] */ ADOUsers __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Group25_get_Users_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Group25_INTERFACE_DEFINED__ */
#ifndef ___Group_INTERFACE_DEFINED__
#define ___Group_INTERFACE_DEFINED__
/* interface _ADOGroup */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID__Group;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000628-0000-0010-8000-00AA006D2EA4")
    _ADOGroup : public _Group25
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParentCatalog( 
            /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ParentCatalog( 
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_ParentCatalog( 
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _GroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOGroup __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOGroup __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOGroup __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _ADOGroup __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPermissions )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [optional][in] */ VARIANT ObjectTypeId,
            /* [retval][out] */ RightsEnum __RPC_FAR *Rights);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPermissions )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [in] */ ActionEnum Action,
            /* [in] */ RightsEnum Rights,
            /* [defaultvalue][in] */ InheritTypeEnum Inherit,
            /* [optional][in] */ VARIANT ObjectTypeId);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Users )( 
            _ADOGroup __RPC_FAR * This,
            /* [retval][out] */ ADOUsers __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADOGroup __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParentCatalog )( 
            _ADOGroup __RPC_FAR * This,
            /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ParentCatalog )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ParentADOCatalog )( 
            _ADOGroup __RPC_FAR * This,
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _GroupVtbl;
    interface _Group
    {
        CONST_VTBL struct _GroupVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Group_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Group_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Group_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Group_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Group_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Group_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Group_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Group_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define _Group_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)
#define _Group_GetPermissions(This,Name,ObjectType,ObjectTypeId,Rights)	\
    (This)->lpVtbl -> GetPermissions(This,Name,ObjectType,ObjectTypeId,Rights)
#define _Group_SetPermissions(This,Name,ObjectType,Action,Rights,Inherit,ObjectTypeId)	\
    (This)->lpVtbl -> SetPermissions(This,Name,ObjectType,Action,Rights,Inherit,ObjectTypeId)
#define _Group_get_Users(This,ppvObject)	\
    (This)->lpVtbl -> get_Users(This,ppvObject)
#define _Group_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _Group_get_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> get_ParentCatalog(This,ppvObject)
#define _Group_put_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> put_ParentCatalog(This,ppvObject)
#define _Group_putref_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> putref_ParentCatalog(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Group_get_Properties_Proxy( 
    _ADOGroup __RPC_FAR * This,
    /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Group_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Group_get_ParentCatalog_Proxy( 
    _ADOGroup __RPC_FAR * This,
    /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Group_get_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Group_put_ParentCatalog_Proxy( 
    _ADOGroup __RPC_FAR * This,
    /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
void __RPC_STUB _Group_put_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE _Group_putref_ParentCatalog_Proxy( 
    _ADOGroup __RPC_FAR * This,
    /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
void __RPC_STUB _Group_putref_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Group_INTERFACE_DEFINED__ */
#ifndef ___User25_INTERFACE_DEFINED__
#define ___User25_INTERFACE_DEFINED__
/* interface _User25 */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID__User25;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000619-0000-0010-8000-00AA006D2EA4")
    _User25 : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetPermissions( 
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [optional][in] */ VARIANT ObjectTypeId,
            /* [retval][out] */ RightsEnum __RPC_FAR *Rights) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetPermissions( 
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [in] */ ActionEnum Action,
            /* [in] */ RightsEnum Rights,
            /* [defaultvalue][in] */ InheritTypeEnum Inherit,
            /* [optional][in] */ VARIANT ObjectTypeId) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ChangePassword( 
            /* [in] */ BSTR OldPassword,
            /* [in] */ BSTR NewPassword) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Groups( 
            /* [retval][out] */ ADOGroups __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _User25Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _User25 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _User25 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _User25 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _User25 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _User25 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _User25 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _User25 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _User25 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _User25 __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPermissions )( 
            _User25 __RPC_FAR * This,
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [optional][in] */ VARIANT ObjectTypeId,
            /* [retval][out] */ RightsEnum __RPC_FAR *Rights);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPermissions )( 
            _User25 __RPC_FAR * This,
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [in] */ ActionEnum Action,
            /* [in] */ RightsEnum Rights,
            /* [defaultvalue][in] */ InheritTypeEnum Inherit,
            /* [optional][in] */ VARIANT ObjectTypeId);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangePassword )( 
            _User25 __RPC_FAR * This,
            /* [in] */ BSTR OldPassword,
            /* [in] */ BSTR NewPassword);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Groups )( 
            _User25 __RPC_FAR * This,
            /* [retval][out] */ ADOGroups __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _User25Vtbl;
    interface _User25
    {
        CONST_VTBL struct _User25Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _User25_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _User25_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _User25_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _User25_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _User25_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _User25_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _User25_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _User25_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define _User25_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)
#define _User25_GetPermissions(This,Name,ObjectType,ObjectTypeId,Rights)	\
    (This)->lpVtbl -> GetPermissions(This,Name,ObjectType,ObjectTypeId,Rights)
#define _User25_SetPermissions(This,Name,ObjectType,Action,Rights,Inherit,ObjectTypeId)	\
    (This)->lpVtbl -> SetPermissions(This,Name,ObjectType,Action,Rights,Inherit,ObjectTypeId)
#define _User25_ChangePassword(This,OldPassword,NewPassword)	\
    (This)->lpVtbl -> ChangePassword(This,OldPassword,NewPassword)
#define _User25_get_Groups(This,ppvObject)	\
    (This)->lpVtbl -> get_Groups(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _User25_get_Name_Proxy( 
    _User25 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _User25_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _User25_put_Name_Proxy( 
    _User25 __RPC_FAR * This,
    /* [in] */ BSTR newVal);
void __RPC_STUB _User25_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _User25_GetPermissions_Proxy( 
    _User25 __RPC_FAR * This,
    /* [in] */ VARIANT Name,
    /* [in] */ ObjectTypeEnum ObjectType,
    /* [optional][in] */ VARIANT ObjectTypeId,
    /* [retval][out] */ RightsEnum __RPC_FAR *Rights);
void __RPC_STUB _User25_GetPermissions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _User25_SetPermissions_Proxy( 
    _User25 __RPC_FAR * This,
    /* [in] */ VARIANT Name,
    /* [in] */ ObjectTypeEnum ObjectType,
    /* [in] */ ActionEnum Action,
    /* [in] */ RightsEnum Rights,
    /* [defaultvalue][in] */ InheritTypeEnum Inherit,
    /* [optional][in] */ VARIANT ObjectTypeId);
void __RPC_STUB _User25_SetPermissions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _User25_ChangePassword_Proxy( 
    _User25 __RPC_FAR * This,
    /* [in] */ BSTR OldPassword,
    /* [in] */ BSTR NewPassword);
void __RPC_STUB _User25_ChangePassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _User25_get_Groups_Proxy( 
    _User25 __RPC_FAR * This,
    /* [retval][out] */ ADOGroups __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _User25_get_Groups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___User25_INTERFACE_DEFINED__ */
#ifndef ___User_INTERFACE_DEFINED__
#define ___User_INTERFACE_DEFINED__
/* interface _ADOUser */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID__User;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000627-0000-0010-8000-00AA006D2EA4")
    _ADOUser : public _User25
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParentCatalog( 
            /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ParentCatalog( 
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_ParentCatalog( 
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _UserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOUser __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOUser __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOUser __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _ADOUser __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPermissions )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [optional][in] */ VARIANT ObjectTypeId,
            /* [retval][out] */ RightsEnum __RPC_FAR *Rights);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPermissions )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ VARIANT Name,
            /* [in] */ ObjectTypeEnum ObjectType,
            /* [in] */ ActionEnum Action,
            /* [in] */ RightsEnum Rights,
            /* [defaultvalue][in] */ InheritTypeEnum Inherit,
            /* [optional][in] */ VARIANT ObjectTypeId);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangePassword )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ BSTR OldPassword,
            /* [in] */ BSTR NewPassword);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Groups )( 
            _ADOUser __RPC_FAR * This,
            /* [retval][out] */ ADOGroups __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADOUser __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParentCatalog )( 
            _ADOUser __RPC_FAR * This,
            /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ParentCatalog )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ParentADOCatalog )( 
            _ADOUser __RPC_FAR * This,
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _UserVtbl;
    interface _User
    {
        CONST_VTBL struct _UserVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _User_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _User_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _User_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _User_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _User_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _User_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _User_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _User_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define _User_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)
#define _User_GetPermissions(This,Name,ObjectType,ObjectTypeId,Rights)	\
    (This)->lpVtbl -> GetPermissions(This,Name,ObjectType,ObjectTypeId,Rights)
#define _User_SetPermissions(This,Name,ObjectType,Action,Rights,Inherit,ObjectTypeId)	\
    (This)->lpVtbl -> SetPermissions(This,Name,ObjectType,Action,Rights,Inherit,ObjectTypeId)
#define _User_ChangePassword(This,OldPassword,NewPassword)	\
    (This)->lpVtbl -> ChangePassword(This,OldPassword,NewPassword)
#define _User_get_Groups(This,ppvObject)	\
    (This)->lpVtbl -> get_Groups(This,ppvObject)
#define _User_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _User_get_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> get_ParentCatalog(This,ppvObject)
#define _User_put_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> put_ParentCatalog(This,ppvObject)
#define _User_putref_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> putref_ParentCatalog(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _User_get_Properties_Proxy( 
    _ADOUser __RPC_FAR * This,
    /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _User_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _User_get_ParentCatalog_Proxy( 
    _ADOUser __RPC_FAR * This,
    /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _User_get_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _User_put_ParentCatalog_Proxy( 
    _ADOUser __RPC_FAR * This,
    /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
void __RPC_STUB _User_put_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE _User_putref_ParentCatalog_Proxy( 
    _ADOUser __RPC_FAR * This,
    /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
void __RPC_STUB _User_putref_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___User_INTERFACE_DEFINED__ */
#ifndef ___Column_INTERFACE_DEFINED__
#define ___Column_INTERFACE_DEFINED__
/* interface _ADOColumn */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID__Column;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000061C-0000-0010-8000-00AA006D2EA4")
    _ADOColumn : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ ColumnAttributesEnum __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Attributes( 
            /* [in] */ ColumnAttributesEnum newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DefinedSize( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_DefinedSize( 
            /* [in] */ long DefinedSize) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_NumericScale( 
            /* [retval][out] */ BYTE __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_NumericScale( 
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Precision( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Precision( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_RelatedColumn( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_RelatedColumn( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SortOrder( 
            /* [retval][out] */ SortOrderEnum __RPC_FAR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_SortOrder( 
            /* [in] */ SortOrderEnum newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ DataTypeEnum __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ DataTypeEnum newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParentCatalog( 
            /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ParentCatalog( 
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_ParentCatalog( 
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _ColumnVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOColumn __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOColumn __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOColumn __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ ColumnAttributesEnum __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Attributes )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ ColumnAttributesEnum newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefinedSize )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefinedSize )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ long DefinedSize);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumericScale )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NumericScale )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ BYTE newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Precision )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Precision )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RelatedColumn )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RelatedColumn )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SortOrder )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ SortOrderEnum __RPC_FAR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SortOrder )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ SortOrderEnum newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ DataTypeEnum __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ DataTypeEnum newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParentCatalog )( 
            _ADOColumn __RPC_FAR * This,
            /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ParentCatalog )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ParentADOCatalog )( 
            _ADOColumn __RPC_FAR * This,
            /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _ColumnVtbl;
    interface _Column
    {
        CONST_VTBL struct _ColumnVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Column_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Column_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Column_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Column_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Column_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Column_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Column_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Column_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define _Column_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)
#define _Column_get_Attributes(This,pVal)	\
    (This)->lpVtbl -> get_Attributes(This,pVal)
#define _Column_put_Attributes(This,newVal)	\
    (This)->lpVtbl -> put_Attributes(This,newVal)
#define _Column_get_DefinedSize(This,pVal)	\
    (This)->lpVtbl -> get_DefinedSize(This,pVal)
#define _Column_put_DefinedSize(This,DefinedSize)	\
    (This)->lpVtbl -> put_DefinedSize(This,DefinedSize)
#define _Column_get_NumericScale(This,pVal)	\
    (This)->lpVtbl -> get_NumericScale(This,pVal)
#define _Column_put_NumericScale(This,newVal)	\
    (This)->lpVtbl -> put_NumericScale(This,newVal)
#define _Column_get_Precision(This,pVal)	\
    (This)->lpVtbl -> get_Precision(This,pVal)
#define _Column_put_Precision(This,newVal)	\
    (This)->lpVtbl -> put_Precision(This,newVal)
#define _Column_get_RelatedColumn(This,pVal)	\
    (This)->lpVtbl -> get_RelatedColumn(This,pVal)
#define _Column_put_RelatedColumn(This,newVal)	\
    (This)->lpVtbl -> put_RelatedColumn(This,newVal)
#define _Column_get_SortOrder(This,pVal)	\
    (This)->lpVtbl -> get_SortOrder(This,pVal)
#define _Column_put_SortOrder(This,newVal)	\
    (This)->lpVtbl -> put_SortOrder(This,newVal)
#define _Column_get_Type(This,pVal)	\
    (This)->lpVtbl -> get_Type(This,pVal)
#define _Column_put_Type(This,newVal)	\
    (This)->lpVtbl -> put_Type(This,newVal)
#define _Column_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _Column_get_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> get_ParentCatalog(This,ppvObject)
#define _Column_put_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> put_ParentCatalog(This,ppvObject)
#define _Column_putref_ParentCatalog(This,ppvObject)	\
    (This)->lpVtbl -> putref_ParentCatalog(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_Name_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _Column_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_Name_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ BSTR newVal);
void __RPC_STUB _Column_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_Attributes_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ ColumnAttributesEnum __RPC_FAR *pVal);
void __RPC_STUB _Column_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_Attributes_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ ColumnAttributesEnum newVal);
void __RPC_STUB _Column_put_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_DefinedSize_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);
void __RPC_STUB _Column_get_DefinedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_DefinedSize_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ long DefinedSize);
void __RPC_STUB _Column_put_DefinedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_NumericScale_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ BYTE __RPC_FAR *pVal);
void __RPC_STUB _Column_get_NumericScale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_NumericScale_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ BYTE newVal);
void __RPC_STUB _Column_put_NumericScale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_Precision_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);
void __RPC_STUB _Column_get_Precision_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_Precision_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ long newVal);
void __RPC_STUB _Column_put_Precision_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_RelatedColumn_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _Column_get_RelatedColumn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_RelatedColumn_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ BSTR newVal);
void __RPC_STUB _Column_put_RelatedColumn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_SortOrder_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ SortOrderEnum __RPC_FAR *pVal);
void __RPC_STUB _Column_get_SortOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_SortOrder_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ SortOrderEnum newVal);
void __RPC_STUB _Column_put_SortOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_Type_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ DataTypeEnum __RPC_FAR *pVal);
void __RPC_STUB _Column_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_Type_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ DataTypeEnum newVal);
void __RPC_STUB _Column_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_Properties_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Column_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Column_get_ParentCatalog_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [retval][out] */ _ADOCatalog __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Column_get_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Column_put_ParentCatalog_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
void __RPC_STUB _Column_put_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE _Column_putref_ParentCatalog_Proxy( 
    _ADOColumn __RPC_FAR * This,
    /* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
void __RPC_STUB _Column_putref_ParentCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Column_INTERFACE_DEFINED__ */
#ifndef ___Index_INTERFACE_DEFINED__
#define ___Index_INTERFACE_DEFINED__
/* interface _ADOIndex */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID__Index;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000061F-0000-0010-8000-00AA006D2EA4")
    _ADOIndex : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Clustered( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Clustered( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_IndexNulls( 
            /* [retval][out] */ AllowNullsEnum __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_IndexNulls( 
            /* [in] */ AllowNullsEnum newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_PrimaryKey( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_PrimaryKey( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Unique( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Unique( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Columns( 
            /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _IndexVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOIndex __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOIndex __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOIndex __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _ADOIndex __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Clustered )( 
            _ADOIndex __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Clustered )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IndexNulls )( 
            _ADOIndex __RPC_FAR * This,
            /* [retval][out] */ AllowNullsEnum __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IndexNulls )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ AllowNullsEnum newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrimaryKey )( 
            _ADOIndex __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrimaryKey )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Unique )( 
            _ADOIndex __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Unique )( 
            _ADOIndex __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Columns )( 
            _ADOIndex __RPC_FAR * This,
            /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADOIndex __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _IndexVtbl;
    interface _Index
    {
        CONST_VTBL struct _IndexVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Index_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Index_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Index_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Index_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Index_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Index_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Index_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Index_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define _Index_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)
#define _Index_get_Clustered(This,pVal)	\
    (This)->lpVtbl -> get_Clustered(This,pVal)
#define _Index_put_Clustered(This,newVal)	\
    (This)->lpVtbl -> put_Clustered(This,newVal)
#define _Index_get_IndexNulls(This,pVal)	\
    (This)->lpVtbl -> get_IndexNulls(This,pVal)
#define _Index_put_IndexNulls(This,newVal)	\
    (This)->lpVtbl -> put_IndexNulls(This,newVal)
#define _Index_get_PrimaryKey(This,pVal)	\
    (This)->lpVtbl -> get_PrimaryKey(This,pVal)
#define _Index_put_PrimaryKey(This,newVal)	\
    (This)->lpVtbl -> put_PrimaryKey(This,newVal)
#define _Index_get_Unique(This,pVal)	\
    (This)->lpVtbl -> get_Unique(This,pVal)
#define _Index_put_Unique(This,newVal)	\
    (This)->lpVtbl -> put_Unique(This,newVal)
#define _Index_get_Columns(This,ppvObject)	\
    (This)->lpVtbl -> get_Columns(This,ppvObject)
#define _Index_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Index_get_Name_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _Index_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Index_put_Name_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [in] */ BSTR newVal);
void __RPC_STUB _Index_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Index_get_Clustered_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
void __RPC_STUB _Index_get_Clustered_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Index_put_Clustered_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);
void __RPC_STUB _Index_put_Clustered_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Index_get_IndexNulls_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [retval][out] */ AllowNullsEnum __RPC_FAR *pVal);
void __RPC_STUB _Index_get_IndexNulls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Index_put_IndexNulls_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [in] */ AllowNullsEnum newVal);
void __RPC_STUB _Index_put_IndexNulls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Index_get_PrimaryKey_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
void __RPC_STUB _Index_get_PrimaryKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Index_put_PrimaryKey_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);
void __RPC_STUB _Index_put_PrimaryKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Index_get_Unique_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
void __RPC_STUB _Index_get_Unique_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Index_put_Unique_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);
void __RPC_STUB _Index_put_Unique_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Index_get_Columns_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Index_get_Columns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Index_get_Properties_Proxy( 
    _ADOIndex __RPC_FAR * This,
    /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Index_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Index_INTERFACE_DEFINED__ */
#ifndef ___Key_INTERFACE_DEFINED__
#define ___Key_INTERFACE_DEFINED__
/* interface _ADOKey */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID__Key;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000622-0000-0010-8000-00AA006D2EA4")
    _ADOKey : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DeleteRule( 
            /* [retval][out] */ RuleEnum __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_DeleteRule( 
            /* [in] */ RuleEnum newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ KeyTypeEnum __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ KeyTypeEnum newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_RelatedTable( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_RelatedTable( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_UpdateRule( 
            /* [retval][out] */ RuleEnum __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_UpdateRule( 
            /* [in] */ RuleEnum newVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Columns( 
            /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _KeyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOKey __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOKey __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOKey __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _ADOKey __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DeleteRule )( 
            _ADOKey __RPC_FAR * This,
            /* [retval][out] */ RuleEnum __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DeleteRule )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ RuleEnum newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            _ADOKey __RPC_FAR * This,
            /* [retval][out] */ KeyTypeEnum __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ KeyTypeEnum newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RelatedTable )( 
            _ADOKey __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RelatedTable )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UpdateRule )( 
            _ADOKey __RPC_FAR * This,
            /* [retval][out] */ RuleEnum __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UpdateRule )( 
            _ADOKey __RPC_FAR * This,
            /* [in] */ RuleEnum newVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Columns )( 
            _ADOKey __RPC_FAR * This,
            /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _KeyVtbl;
    interface _Key
    {
        CONST_VTBL struct _KeyVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Key_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Key_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Key_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Key_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Key_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Key_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Key_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Key_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define _Key_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)
#define _Key_get_DeleteRule(This,pVal)	\
    (This)->lpVtbl -> get_DeleteRule(This,pVal)
#define _Key_put_DeleteRule(This,newVal)	\
    (This)->lpVtbl -> put_DeleteRule(This,newVal)
#define _Key_get_Type(This,pVal)	\
    (This)->lpVtbl -> get_Type(This,pVal)
#define _Key_put_Type(This,newVal)	\
    (This)->lpVtbl -> put_Type(This,newVal)
#define _Key_get_RelatedTable(This,pVal)	\
    (This)->lpVtbl -> get_RelatedTable(This,pVal)
#define _Key_put_RelatedTable(This,newVal)	\
    (This)->lpVtbl -> put_RelatedTable(This,newVal)
#define _Key_get_UpdateRule(This,pVal)	\
    (This)->lpVtbl -> get_UpdateRule(This,pVal)
#define _Key_put_UpdateRule(This,newVal)	\
    (This)->lpVtbl -> put_UpdateRule(This,newVal)
#define _Key_get_Columns(This,ppvObject)	\
    (This)->lpVtbl -> get_Columns(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Key_get_Name_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _Key_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Key_put_Name_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [in] */ BSTR newVal);
void __RPC_STUB _Key_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Key_get_DeleteRule_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [retval][out] */ RuleEnum __RPC_FAR *pVal);
void __RPC_STUB _Key_get_DeleteRule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Key_put_DeleteRule_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [in] */ RuleEnum newVal);
void __RPC_STUB _Key_put_DeleteRule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Key_get_Type_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [retval][out] */ KeyTypeEnum __RPC_FAR *pVal);
void __RPC_STUB _Key_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Key_put_Type_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [in] */ KeyTypeEnum newVal);
void __RPC_STUB _Key_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Key_get_RelatedTable_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB _Key_get_RelatedTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Key_put_RelatedTable_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [in] */ BSTR newVal);
void __RPC_STUB _Key_put_RelatedTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Key_get_UpdateRule_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [retval][out] */ RuleEnum __RPC_FAR *pVal);
void __RPC_STUB _Key_get_UpdateRule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Key_put_UpdateRule_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [in] */ RuleEnum newVal);
void __RPC_STUB _Key_put_UpdateRule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Key_get_Columns_Proxy( 
    _ADOKey __RPC_FAR * This,
    /* [retval][out] */ ADOColumns __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _Key_get_Columns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Key_INTERFACE_DEFINED__ */
#ifndef __View_INTERFACE_DEFINED__
#define __View_INTERFACE_DEFINED__
/* interface ADOView */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID_View;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000613-0000-0010-8000-00AA006D2EA4")
    ADOView : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Command( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Command( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Command( 
            /* [in] */ IDispatch __RPC_FAR *pComm) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DateCreated( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DateModified( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOView __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOView __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOView __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOView __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOView __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOView __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOView __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Command )( 
            ADOView __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Command )( 
            ADOView __RPC_FAR * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Command )( 
            ADOView __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pComm);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ADOView __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateCreated )( 
            ADOView __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateModified )( 
            ADOView __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        END_INTERFACE
    } ViewVtbl;
    interface View
    {
        CONST_VTBL struct ViewVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define View_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define View_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define View_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define View_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define View_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define View_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define View_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define View_get_Command(This,pVal)	\
    (This)->lpVtbl -> get_Command(This,pVal)
#define View_put_Command(This,newVal)	\
    (This)->lpVtbl -> put_Command(This,newVal)
#define View_putref_Command(This,pComm)	\
    (This)->lpVtbl -> putref_Command(This,pComm)
#define View_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define View_get_DateCreated(This,pVal)	\
    (This)->lpVtbl -> get_DateCreated(This,pVal)
#define View_get_DateModified(This,pVal)	\
    (This)->lpVtbl -> get_DateModified(This,pVal)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE View_get_Command_Proxy( 
    ADOView __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB View_get_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE View_put_Command_Proxy( 
    ADOView __RPC_FAR * This,
    /* [in] */ VARIANT newVal);
void __RPC_STUB View_put_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE View_putref_Command_Proxy( 
    ADOView __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pComm);
void __RPC_STUB View_putref_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE View_get_Name_Proxy( 
    ADOView __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB View_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE View_get_DateCreated_Proxy( 
    ADOView __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB View_get_DateCreated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE View_get_DateModified_Proxy( 
    ADOView __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB View_get_DateModified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __View_INTERFACE_DEFINED__ */
#ifndef __Procedure_INTERFACE_DEFINED__
#define __Procedure_INTERFACE_DEFINED__
/* interface ADOProcedure */
/* [helpcontext][unique][dual][uuid][nonextensible][object] */ 
EXTERN_C const IID IID_Procedure;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000625-0000-0010-8000-00AA006D2EA4")
    ADOProcedure : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Command( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVar) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Command( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Command( 
            /* [in] */ IDispatch __RPC_FAR *pComm) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DateCreated( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DateModified( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ProcedureVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOProcedure __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOProcedure __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOProcedure __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOProcedure __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOProcedure __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOProcedure __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOProcedure __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Command )( 
            ADOProcedure __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVar);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Command )( 
            ADOProcedure __RPC_FAR * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Command )( 
            ADOProcedure __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pComm);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ADOProcedure __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateCreated )( 
            ADOProcedure __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateModified )( 
            ADOProcedure __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        END_INTERFACE
    } ProcedureVtbl;
    interface Procedure
    {
        CONST_VTBL struct ProcedureVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Procedure_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Procedure_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Procedure_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Procedure_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Procedure_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Procedure_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Procedure_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Procedure_get_Command(This,pVar)	\
    (This)->lpVtbl -> get_Command(This,pVar)
#define Procedure_put_Command(This,newVal)	\
    (This)->lpVtbl -> put_Command(This,newVal)
#define Procedure_putref_Command(This,pComm)	\
    (This)->lpVtbl -> putref_Command(This,pComm)
#define Procedure_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)
#define Procedure_get_DateCreated(This,pVal)	\
    (This)->lpVtbl -> get_DateCreated(This,pVal)
#define Procedure_get_DateModified(This,pVal)	\
    (This)->lpVtbl -> get_DateModified(This,pVal)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Procedure_get_Command_Proxy( 
    ADOProcedure __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVar);
void __RPC_STUB Procedure_get_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Procedure_put_Command_Proxy( 
    ADOProcedure __RPC_FAR * This,
    /* [in] */ VARIANT newVal);
void __RPC_STUB Procedure_put_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE Procedure_putref_Command_Proxy( 
    ADOProcedure __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pComm);
void __RPC_STUB Procedure_putref_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Procedure_get_Name_Proxy( 
    ADOProcedure __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);
void __RPC_STUB Procedure_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Procedure_get_DateCreated_Proxy( 
    ADOProcedure __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB Procedure_get_DateCreated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Procedure_get_DateModified_Proxy( 
    ADOProcedure __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB Procedure_get_DateModified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Procedure_INTERFACE_DEFINED__ */
EXTERN_C const CLSID CLSID_Catalog;
#ifdef __cplusplus
Catalog;
#endif
EXTERN_C const CLSID CLSID_Table;
#ifdef __cplusplus
Table;
#endif
#ifndef __Property_INTERFACE_DEFINED__
#define __Property_INTERFACE_DEFINED__
/* interface ADOProperty */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID_Property;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000503-0000-0010-8000-00AA006D2EA4")
    ADOProperty : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT __RPC_FAR *pval) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT val) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ DataTypeEnum __RPC_FAR *ptype) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ long __RPC_FAR *plAttributes) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Attributes( 
            /* [in] */ long lAttributes) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct PropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOProperty __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOProperty __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOProperty __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ADOProperty __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pval);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ VARIANT val);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ADOProperty __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            ADOProperty __RPC_FAR * This,
            /* [retval][out] */ DataTypeEnum __RPC_FAR *ptype);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            ADOProperty __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAttributes);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Attributes )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ long lAttributes);
        
        END_INTERFACE
    } PropertyVtbl;
    interface Property
    {
        CONST_VTBL struct PropertyVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Property_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Property_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Property_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Property_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Property_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Property_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Property_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Property_get_Value(This,pval)	\
    (This)->lpVtbl -> get_Value(This,pval)
#define Property_put_Value(This,val)	\
    (This)->lpVtbl -> put_Value(This,val)
#define Property_get_Name(This,pbstr)	\
    (This)->lpVtbl -> get_Name(This,pbstr)
#define Property_get_Type(This,ptype)	\
    (This)->lpVtbl -> get_Type(This,ptype)
#define Property_get_Attributes(This,plAttributes)	\
    (This)->lpVtbl -> get_Attributes(This,plAttributes)
#define Property_put_Attributes(This,lAttributes)	\
    (This)->lpVtbl -> put_Attributes(This,lAttributes)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Property_get_Value_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pval);
void __RPC_STUB Property_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Property_put_Value_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [in] */ VARIANT val);
void __RPC_STUB Property_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Property_get_Name_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Property_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Property_get_Type_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [retval][out] */ DataTypeEnum __RPC_FAR *ptype);
void __RPC_STUB Property_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Property_get_Attributes_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAttributes);
void __RPC_STUB Property_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Property_put_Attributes_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [in] */ long lAttributes);
void __RPC_STUB Property_put_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Property_INTERFACE_DEFINED__ */
EXTERN_C const CLSID CLSID_Group;
#ifdef __cplusplus
Group;
#endif
EXTERN_C const CLSID CLSID_User;
#ifdef __cplusplus
User;
#endif
EXTERN_C const CLSID CLSID_Column;
#ifdef __cplusplus
Column;
#endif
EXTERN_C const CLSID CLSID_Index;
#ifdef __cplusplus
Index;
#endif
EXTERN_C const CLSID CLSID_Key;
#ifdef __cplusplus
Key;
#endif
#ifndef __Tables_INTERFACE_DEFINED__
#define __Tables_INTERFACE_DEFINED__
/* interface ADOTables */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Tables;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000611-0000-0010-8000-00AA006D2EA4")
    ADOTables : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Table	__RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ VARIANT Item) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct TablesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOTables __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOTables __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOTables __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOTables __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOTables __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOTables __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOTables __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOTables __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOTables __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOTables __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOTables __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Table	__RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOTables __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOTables __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } TablesVtbl;
    interface Tables
    {
        CONST_VTBL struct TablesVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Tables_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Tables_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Tables_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Tables_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Tables_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Tables_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Tables_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Tables_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Tables__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Tables_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Tables_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#define Tables_Append(This,Item)	\
    (This)->lpVtbl -> Append(This,Item)
#define Tables_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Tables_get_Item_Proxy( 
    ADOTables __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ Table	__RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Tables_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Tables_Append_Proxy( 
    ADOTables __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Tables_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Tables_Delete_Proxy( 
    ADOTables __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Tables_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Tables_INTERFACE_DEFINED__ */
#ifndef __Columns_INTERFACE_DEFINED__
#define __Columns_INTERFACE_DEFINED__
/* interface ADOColumns */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Columns;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000061D-0000-0010-8000-00AA006D2EA4")
    ADOColumns : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Column	__RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ VARIANT Item,
            /* [defaultvalue][in] */ DataTypeEnum Type = adVarWChar,
            /* [defaultvalue][in] */ long DefinedSize = 0) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ColumnsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOColumns __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOColumns __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOColumns __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOColumns __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOColumns __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOColumns __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOColumns __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOColumns __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOColumns __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOColumns __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOColumns __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Column	__RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOColumns __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [defaultvalue][in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ long DefinedSize);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOColumns __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } ColumnsVtbl;
    interface Columns
    {
        CONST_VTBL struct ColumnsVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Columns_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Columns_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Columns_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Columns_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Columns_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Columns_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Columns_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Columns_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Columns__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Columns_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Columns_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#define Columns_Append(This,Item,Type,DefinedSize)	\
    (This)->lpVtbl -> Append(This,Item,Type,DefinedSize)
#define Columns_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Columns_get_Item_Proxy( 
    ADOColumns __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ Column	__RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Columns_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Columns_Append_Proxy( 
    ADOColumns __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [defaultvalue][in] */ DataTypeEnum Type,
    /* [defaultvalue][in] */ long DefinedSize);
void __RPC_STUB Columns_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Columns_Delete_Proxy( 
    ADOColumns __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Columns_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Columns_INTERFACE_DEFINED__ */
#ifndef __Procedures_INTERFACE_DEFINED__
#define __Procedures_INTERFACE_DEFINED__
/* interface ADOProcedures */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Procedures;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000626-0000-0010-8000-00AA006D2EA4")
    ADOProcedures : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ ADOProcedure __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ BSTR Name,
            /* [in] */ IDispatch __RPC_FAR *Command) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ProceduresVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOProcedures __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOProcedures __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOProcedures __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOProcedures __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOProcedures __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOProcedures __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOProcedures __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOProcedures __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOProcedures __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOProcedures __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOProcedures __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ ADOProcedure __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOProcedures __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ IDispatch __RPC_FAR *Command);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOProcedures __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } ProceduresVtbl;
    interface Procedures
    {
        CONST_VTBL struct ProceduresVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Procedures_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Procedures_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Procedures_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Procedures_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Procedures_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Procedures_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Procedures_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Procedures_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Procedures__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Procedures_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Procedures_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#define Procedures_Append(This,Name,Command)	\
    (This)->lpVtbl -> Append(This,Name,Command)
#define Procedures_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Procedures_get_Item_Proxy( 
    ADOProcedures __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ ADOProcedure __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Procedures_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Procedures_Append_Proxy( 
    ADOProcedures __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ IDispatch __RPC_FAR *Command);
void __RPC_STUB Procedures_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Procedures_Delete_Proxy( 
    ADOProcedures __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Procedures_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Procedures_INTERFACE_DEFINED__ */
#ifndef __Views_INTERFACE_DEFINED__
#define __Views_INTERFACE_DEFINED__
/* interface ADOViews */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Views;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000614-0000-0010-8000-00AA006D2EA4")
    ADOViews : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ ADOView __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ BSTR Name,
            /* [in] */ IDispatch __RPC_FAR *Command) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ViewsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOViews __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOViews __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOViews __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOViews __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOViews __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOViews __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOViews __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOViews __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOViews __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOViews __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOViews __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ ADOView __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOViews __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ IDispatch __RPC_FAR *Command);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOViews __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } ViewsVtbl;
    interface Views
    {
        CONST_VTBL struct ViewsVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Views_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Views_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Views_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Views_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Views_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Views_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Views_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Views_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Views__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Views_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Views_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#define Views_Append(This,Name,Command)	\
    (This)->lpVtbl -> Append(This,Name,Command)
#define Views_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Views_get_Item_Proxy( 
    ADOViews __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ ADOView __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Views_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Views_Append_Proxy( 
    ADOViews __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ IDispatch __RPC_FAR *Command);
void __RPC_STUB Views_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Views_Delete_Proxy( 
    ADOViews __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Views_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Views_INTERFACE_DEFINED__ */
#ifndef __Indexes_INTERFACE_DEFINED__
#define __Indexes_INTERFACE_DEFINED__
/* interface ADOIndexes */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Indexes;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000620-0000-0010-8000-00AA006D2EA4")
    ADOIndexes : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Index	__RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ VARIANT Item,
            /* [optional][in] */ VARIANT columns) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct IndexesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOIndexes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOIndexes __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOIndexes __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOIndexes __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOIndexes __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOIndexes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOIndexes __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOIndexes __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOIndexes __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOIndexes __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOIndexes __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Index	__RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOIndexes __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [optional][in] */ VARIANT columns);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOIndexes __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } IndexesVtbl;
    interface Indexes
    {
        CONST_VTBL struct IndexesVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Indexes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Indexes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Indexes_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Indexes_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Indexes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Indexes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Indexes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Indexes_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Indexes__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Indexes_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Indexes_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#define Indexes_Append(This,Item,columns)	\
    (This)->lpVtbl -> Append(This,Item,columns)
#define Indexes_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Indexes_get_Item_Proxy( 
    ADOIndexes __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ Index	__RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Indexes_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Indexes_Append_Proxy( 
    ADOIndexes __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [optional][in] */ VARIANT columns);
void __RPC_STUB Indexes_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Indexes_Delete_Proxy( 
    ADOIndexes __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Indexes_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Indexes_INTERFACE_DEFINED__ */
#ifndef __Keys_INTERFACE_DEFINED__
#define __Keys_INTERFACE_DEFINED__
/* interface ADOKeys */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Keys;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000623-0000-0010-8000-00AA006D2EA4")
    ADOKeys : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Key	__RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ VARIANT Item,
            /* [defaultvalue][in] */ KeyTypeEnum Type,
            /* [optional][in] */ VARIANT Column,
            /* [defaultvalue][in] */ BSTR RelatedADOTable = L"",
            /* [defaultvalue][in] */ BSTR RelatedADOColumn = L"") = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct KeysVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOKeys __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOKeys __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOKeys __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOKeys __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOKeys __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOKeys __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOKeys __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOKeys __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOKeys __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOKeys __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOKeys __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Key	__RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOKeys __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [defaultvalue][in] */ KeyTypeEnum Type,
            /* [optional][in] */ VARIANT Column,
            /* [defaultvalue][in] */ BSTR RelatedTable,
            /* [defaultvalue][in] */ BSTR RelatedColumn);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOKeys __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } KeysVtbl;
    interface Keys
    {
        CONST_VTBL struct KeysVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Keys_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Keys_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Keys_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Keys_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Keys_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Keys_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Keys_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Keys_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Keys__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Keys_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Keys_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#define Keys_Append(This,Item,Type,Column,RelatedTable,RelatedColumn)	\
    (This)->lpVtbl -> Append(This,Item,Type,Column,RelatedTable,RelatedColumn)
#define Keys_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Keys_get_Item_Proxy( 
    ADOKeys __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ Key	__RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Keys_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Keys_Append_Proxy( 
    ADOKeys __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [defaultvalue][in] */ KeyTypeEnum Type,
    /* [optional][in] */ VARIANT Column,
    /* [defaultvalue][in] */ BSTR RelatedTable,
    /* [defaultvalue][in] */ BSTR RelatedColumn);
void __RPC_STUB Keys_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Keys_Delete_Proxy( 
    ADOKeys __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Keys_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Keys_INTERFACE_DEFINED__ */
#ifndef __Users_INTERFACE_DEFINED__
#define __Users_INTERFACE_DEFINED__
/* interface ADOUsers */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Users;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000061A-0000-0010-8000-00AA006D2EA4")
    ADOUsers : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ User	__RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ VARIANT Item,
            /* [defaultvalue][in] */ BSTR Password = L"") = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct UsersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOUsers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOUsers __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOUsers __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOUsers __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOUsers __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOUsers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOUsers __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOUsers __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOUsers __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOUsers __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOUsers __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ User	__RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOUsers __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [defaultvalue][in] */ BSTR Password);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOUsers __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } UsersVtbl;
    interface Users
    {
        CONST_VTBL struct UsersVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Users_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Users_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Users_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Users_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Users_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Users_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Users_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Users_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Users__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Users_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Users_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#define Users_Append(This,Item,Password)	\
    (This)->lpVtbl -> Append(This,Item,Password)
#define Users_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Users_get_Item_Proxy( 
    ADOUsers __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ User	__RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Users_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Users_Append_Proxy( 
    ADOUsers __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [defaultvalue][in] */ BSTR Password);
void __RPC_STUB Users_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Users_Delete_Proxy( 
    ADOUsers __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Users_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Users_INTERFACE_DEFINED__ */
#ifndef __Groups_INTERFACE_DEFINED__
#define __Groups_INTERFACE_DEFINED__
/* interface ADOGroups */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Groups;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000617-0000-0010-8000-00AA006D2EA4")
    ADOGroups : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Group	__RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ VARIANT Item) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct GroupsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOGroups __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOGroups __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOGroups __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOGroups __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOGroups __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOGroups __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOGroups __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOGroups __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOGroups __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOGroups __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOGroups __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ Group	__RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOGroups __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOGroups __RPC_FAR * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } GroupsVtbl;
    interface Groups
    {
        CONST_VTBL struct GroupsVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Groups_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Groups_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Groups_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Groups_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Groups_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Groups_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Groups_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Groups_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Groups__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Groups_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Groups_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#define Groups_Append(This,Item)	\
    (This)->lpVtbl -> Append(This,Item)
#define Groups_Delete(This,Item)	\
    (This)->lpVtbl -> Delete(This,Item)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Groups_get_Item_Proxy( 
    ADOGroups __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ Group	__RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Groups_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Groups_Append_Proxy( 
    ADOGroups __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Groups_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Groups_Delete_Proxy( 
    ADOGroups __RPC_FAR * This,
    /* [in] */ VARIANT Item);
void __RPC_STUB Groups_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Groups_INTERFACE_DEFINED__ */
#ifndef __Properties_INTERFACE_DEFINED__
#define __Properties_INTERFACE_DEFINED__
/* interface ADOProperties */
/* [object][uuid][helpcontext][nonextensible][dual] */ 
EXTERN_C const IID IID_Properties;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000504-0000-0010-8000-00AA006D2EA4")
    ADOProperties : public _ADOCollection
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Item,
            /* [retval][out] */ ADOProperty __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct PropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOProperties __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOProperties __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOProperties __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOProperties __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOProperties __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOProperties __RPC_FAR * This);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ VARIANT Item,
            /* [retval][out] */ ADOProperty __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } PropertiesVtbl;
    interface Properties
    {
        CONST_VTBL struct PropertiesVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Properties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Properties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Properties_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Properties_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Properties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Properties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Properties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Properties_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Properties__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Properties_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Properties_get_Item(This,Item,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Item,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Properties_get_Item_Proxy( 
    ADOProperties __RPC_FAR * This,
    /* [in] */ VARIANT Item,
    /* [retval][out] */ ADOProperty __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Properties_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Properties_INTERFACE_DEFINED__ */
#endif /* __ADOX_LIBRARY_DEFINED__ */
/* Additional Prototypes for ALL interfaces */
/* end of Additional Prototypes */
#ifdef __cplusplus
}
#endif
#endif
/***********************************
Forwards
*/
#define ADOCatalog _ADOCatalog
#define ADOTable _ADOTable
#define ADOGroup _ADOGroup
#define ADOUser _ADOUser
#define ADOIndex _ADOIndex
#define ADOColumn _ADOColumn
#define ADOKey _ADOKey
#define ADOParameter _ADOParameter
#define ADOCollection _ADOCollection
#define ADODynaCollection _ADODynaCollection


#endif // _ADOCTINT_H_
