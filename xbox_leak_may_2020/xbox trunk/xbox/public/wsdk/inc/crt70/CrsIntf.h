/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Thu Apr 23 13:37:19 1998
 */
/* Compiler settings for crsintf.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __crsintf_h__
#define __crsintf_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IReplicationServer_FWD_DEFINED__
#define __IReplicationServer_FWD_DEFINED__
typedef interface IReplicationServer IReplicationServer;
#endif 	/* __IReplicationServer_FWD_DEFINED__ */


#ifndef __IReplicationProject_FWD_DEFINED__
#define __IReplicationProject_FWD_DEFINED__
typedef interface IReplicationProject IReplicationProject;
#endif 	/* __IReplicationProject_FWD_DEFINED__ */


#ifndef __IReplicationItem_FWD_DEFINED__
#define __IReplicationItem_FWD_DEFINED__
typedef interface IReplicationItem IReplicationItem;
#endif 	/* __IReplicationItem_FWD_DEFINED__ */


#ifndef __IReplicationSchedule_FWD_DEFINED__
#define __IReplicationSchedule_FWD_DEFINED__
typedef interface IReplicationSchedule IReplicationSchedule;
#endif 	/* __IReplicationSchedule_FWD_DEFINED__ */


#ifndef __IReplicationEventSinkConfig_FWD_DEFINED__
#define __IReplicationEventSinkConfig_FWD_DEFINED__
typedef interface IReplicationEventSinkConfig IReplicationEventSinkConfig;
#endif 	/* __IReplicationEventSinkConfig_FWD_DEFINED__ */


#ifndef __IReplicationInstance_FWD_DEFINED__
#define __IReplicationInstance_FWD_DEFINED__
typedef interface IReplicationInstance IReplicationInstance;
#endif 	/* __IReplicationInstance_FWD_DEFINED__ */


#ifndef __IReplicationRoute_FWD_DEFINED__
#define __IReplicationRoute_FWD_DEFINED__
typedef interface IReplicationRoute IReplicationRoute;
#endif 	/* __IReplicationRoute_FWD_DEFINED__ */


#ifndef __IReplicationClient_FWD_DEFINED__
#define __IReplicationClient_FWD_DEFINED__
typedef interface IReplicationClient IReplicationClient;
#endif 	/* __IReplicationClient_FWD_DEFINED__ */


#ifndef __IReplicationClientError_FWD_DEFINED__
#define __IReplicationClientError_FWD_DEFINED__
typedef interface IReplicationClientError IReplicationClientError;
#endif 	/* __IReplicationClientError_FWD_DEFINED__ */


#ifndef __IReplicationPostingAcceptor_FWD_DEFINED__
#define __IReplicationPostingAcceptor_FWD_DEFINED__
typedef interface IReplicationPostingAcceptor IReplicationPostingAcceptor;
#endif 	/* __IReplicationPostingAcceptor_FWD_DEFINED__ */


#ifndef __CReplicationServer_FWD_DEFINED__
#define __CReplicationServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationServer CReplicationServer;
#else
typedef struct CReplicationServer CReplicationServer;
#endif /* __cplusplus */

#endif 	/* __CReplicationServer_FWD_DEFINED__ */


#ifndef __CReplicationProject_FWD_DEFINED__
#define __CReplicationProject_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationProject CReplicationProject;
#else
typedef struct CReplicationProject CReplicationProject;
#endif /* __cplusplus */

#endif 	/* __CReplicationProject_FWD_DEFINED__ */


#ifndef __CReplicationInstance_FWD_DEFINED__
#define __CReplicationInstance_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationInstance CReplicationInstance;
#else
typedef struct CReplicationInstance CReplicationInstance;
#endif /* __cplusplus */

#endif 	/* __CReplicationInstance_FWD_DEFINED__ */


#ifndef __CReplicationRoute_FWD_DEFINED__
#define __CReplicationRoute_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationRoute CReplicationRoute;
#else
typedef struct CReplicationRoute CReplicationRoute;
#endif /* __cplusplus */

#endif 	/* __CReplicationRoute_FWD_DEFINED__ */


#ifndef __CReplicationClient_FWD_DEFINED__
#define __CReplicationClient_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationClient CReplicationClient;
#else
typedef struct CReplicationClient CReplicationClient;
#endif /* __cplusplus */

#endif 	/* __CReplicationClient_FWD_DEFINED__ */


#ifndef __CReplicationClientError_FWD_DEFINED__
#define __CReplicationClientError_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationClientError CReplicationClientError;
#else
typedef struct CReplicationClientError CReplicationClientError;
#endif /* __cplusplus */

#endif 	/* __CReplicationClientError_FWD_DEFINED__ */


#ifndef __CReplicationEventSinkConfig_FWD_DEFINED__
#define __CReplicationEventSinkConfig_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationEventSinkConfig CReplicationEventSinkConfig;
#else
typedef struct CReplicationEventSinkConfig CReplicationEventSinkConfig;
#endif /* __cplusplus */

#endif 	/* __CReplicationEventSinkConfig_FWD_DEFINED__ */


#ifndef __CReplicationItem_FWD_DEFINED__
#define __CReplicationItem_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationItem CReplicationItem;
#else
typedef struct CReplicationItem CReplicationItem;
#endif /* __cplusplus */

#endif 	/* __CReplicationItem_FWD_DEFINED__ */


#ifndef __CReplicationSchedule_FWD_DEFINED__
#define __CReplicationSchedule_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationSchedule CReplicationSchedule;
#else
typedef struct CReplicationSchedule CReplicationSchedule;
#endif /* __cplusplus */

#endif 	/* __CReplicationSchedule_FWD_DEFINED__ */


#ifndef __CReplicationPostingAcceptor_FWD_DEFINED__
#define __CReplicationPostingAcceptor_FWD_DEFINED__

#ifdef __cplusplus
typedef class CReplicationPostingAcceptor CReplicationPostingAcceptor;
#else
typedef struct CReplicationPostingAcceptor CReplicationPostingAcceptor;
#endif /* __cplusplus */

#endif 	/* __CReplicationPostingAcceptor_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "crstypes.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_crsintf_0000
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


#define	CRSEVENT_SEVERITY_SUCCESS	( 0 )

#define	CRSEVENT_SEVERITY_INFORMATIONAL	( 1 )

#define	CRSEVENT_SEVERITY_WARNING	( 2 )

#define	CRSEVENT_SEVERITY_ERROR	( 3 )

#define	CRSEVENT_CATEGORY_GENERAL	( 0 )

#define	CRSEVENT_CATEGORY_FILE	( 1 )

#define	CRSEVENT_CATEGORY_REPLICATION	( 2 )

#define	CRSEVENT_CATEGORY_ADMIN	( 3 )

#define	CRSEVENT_CATEGORY_SUMMARY	( 4 )

#define	EVFLAG_FILE_EVENTS	( 0x1 )

#define	EVFLAG_ADMIN_EVENTS	( 0x2 )

#define	EVFLAG_SUMMARY_EVENTS	( 0x4 )

#define	EVFLAG_REPLICATION_EVENTS	( 0x8 )

#define	EVFLAG_ALL	( 0xff )

#define	CRS_SCHEDULE_DAY_MONDAY	( 0x1 )

#define	CRS_SCHEDULE_DAY_TUESDAY	( 0x2 )

#define	CRS_SCHEDULE_DAY_WEDNESDAY	( 0x4 )

#define	CRS_SCHEDULE_DAY_THURSDAY	( 0x8 )

#define	CRS_SCHEDULE_DAY_FRIDAY	( 0x10 )

#define	CRS_SCHEDULE_DAY_SATURDAY	( 0x20 )

#define	CRS_SCHEDULE_DAY_SUNDAY	( 0x40 )

#define	CRS_SCHEDULE_DAILY	( 0xff )











extern RPC_IF_HANDLE __MIDL_itf_crsintf_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_crsintf_0000_v0_0_s_ifspec;

#ifndef __IReplicationServer_INTERFACE_DEFINED__
#define __IReplicationServer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationServer
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 


#ifndef _LPREPLICATIONROUTESDEFINED
#define _LPREPLICATIONROUTESDEFINED
typedef /* [unique] */ IReplicationRoute __RPC_FAR *LPREPLICATION_ROUTE;

#endif 
#ifndef _LPREPLICATIONSERVERDEFINED
#define _LPREPLICATIONSERVERDEFINED
typedef IReplicationServer __RPC_FAR *LPREPLICATION_SERVER;

#endif 
#ifndef _LPREPLICATIONPROJECTDEFINED
#define _LPREPLICATIONPROJECTDEFINED
typedef IReplicationProject __RPC_FAR *LPREPLICATION_PROJECT;

#endif 
#ifndef _LPREPLICATIONINSTANCEDEFINED
#define _LPREPLICATIONINSTANCEDEFINED
typedef IReplicationInstance __RPC_FAR *LPREPLICATION_INSTANCE;

#endif 

EXTERN_C const IID IID_IReplicationServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("ABEB9802-DB7A-11d0-A800-00C04FC9DAA5")
    IReplicationServer : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ BSTR pszServer) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE InitializeEx( 
            /* [in] */ BSTR pszServer,
            /* [in] */ BSTR pszUserName,
            /* [in] */ BSTR pszPassword,
            /* [in] */ LONG lTimeout) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OpenProject( 
            /* [string][in] */ BSTR pszProject,
            /* [in] */ CRS_PROJECT_CREATION cpcCreationFlags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvInfProject) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DeleteProject( 
            /* [string][in] */ BSTR pszProject) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumProjects( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvInfProject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumServers( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [in] */ CRS_SERVER_TYPES Flags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrServer) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OpenPostingAcceptor( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvIDispPA) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Continue( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [string][in] */ BSTR pszParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Put( 
            /* [string][in] */ BSTR pszParmName,
            /* [in] */ VARIANT vParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Enum( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out] */ VARIANT __RPC_FAR *pvParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddRoute( 
            /* [string][in] */ BSTR pszRoute,
            /* [retval][out] */ VARIANT __RPC_FAR *pvRoute) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DeleteRoute( 
            /* [string][in] */ BSTR pszRoute) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumRoutes( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvRoute) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddEventSink( 
            /* [string][in] */ BSTR bstrSink,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdispSink) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DeleteEventSink( 
            /* [string][in] */ BSTR bstrSink) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumEventSinks( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdispSink) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumReplications( 
            /* [string][in] */ BSTR pszMatchProject,
            /* [in] */ LONG lMatchState,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvReplInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CheckAccess( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvlAccessMask) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE MapUrl( 
            /* [string][in] */ BSTR bstrUrl,
            /* [out] */ VARIANT __RPC_FAR *vbstrSuffix,
            /* [retval][out] */ VARIANT __RPC_FAR *vdispProject) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TranslateEventCode( 
            /* [in] */ LONG lEventCode,
            /* [out] */ VARIANT __RPC_FAR *vbstrDescription,
            /* [out] */ VARIANT __RPC_FAR *vlSeverity,
            /* [out] */ VARIANT __RPC_FAR *vlCategory) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ClearEvents( 
            /* [string][in] */ BSTR bstrEventSink) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_UseTransactions( 
            /* [string][in] */ BSTR pszTrans) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_UseTransactions( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvTrans) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_ServerName( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvServer) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvVersion) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EventTimeToLocalTime( 
            /* [in] */ VARIANT vdateEventTime,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdateLocalTime) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationServer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationServer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationServer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationServer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize )( 
            IReplicationServer __RPC_FAR * This,
            /* [in] */ BSTR pszServer);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitializeEx )( 
            IReplicationServer __RPC_FAR * This,
            /* [in] */ BSTR pszServer,
            /* [in] */ BSTR pszUserName,
            /* [in] */ BSTR pszPassword,
            /* [in] */ LONG lTimeout);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenProject )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR pszProject,
            /* [in] */ CRS_PROJECT_CREATION cpcCreationFlags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvInfProject);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteProject )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR pszProject);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumProjects )( 
            IReplicationServer __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvInfProject);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumServers )( 
            IReplicationServer __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [in] */ CRS_SERVER_TYPES Flags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrServer);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenPostingAcceptor )( 
            IReplicationServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvIDispPA);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            IReplicationServer __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IReplicationServer __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IReplicationServer __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Continue )( 
            IReplicationServer __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR pszParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Put )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR pszParmName,
            /* [in] */ VARIANT vParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Enum )( 
            IReplicationServer __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out] */ VARIANT __RPC_FAR *pvParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IReplicationServer __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddRoute )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR pszRoute,
            /* [retval][out] */ VARIANT __RPC_FAR *pvRoute);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteRoute )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR pszRoute);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumRoutes )( 
            IReplicationServer __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvRoute);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddEventSink )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR bstrSink,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdispSink);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteEventSink )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR bstrSink);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumEventSinks )( 
            IReplicationServer __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdispSink);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumReplications )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR pszMatchProject,
            /* [in] */ LONG lMatchState,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvReplInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CheckAccess )( 
            IReplicationServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvlAccessMask);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MapUrl )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR bstrUrl,
            /* [out] */ VARIANT __RPC_FAR *vbstrSuffix,
            /* [retval][out] */ VARIANT __RPC_FAR *vdispProject);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TranslateEventCode )( 
            IReplicationServer __RPC_FAR * This,
            /* [in] */ LONG lEventCode,
            /* [out] */ VARIANT __RPC_FAR *vbstrDescription,
            /* [out] */ VARIANT __RPC_FAR *vlSeverity,
            /* [out] */ VARIANT __RPC_FAR *vlCategory);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ClearEvents )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR bstrEventSink);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseTransactions )( 
            IReplicationServer __RPC_FAR * This,
            /* [string][in] */ BSTR pszTrans);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseTransactions )( 
            IReplicationServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvTrans);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ServerName )( 
            IReplicationServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvServer);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Version )( 
            IReplicationServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvVersion);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EventTimeToLocalTime )( 
            IReplicationServer __RPC_FAR * This,
            /* [in] */ VARIANT vdateEventTime,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdateLocalTime);
        
        END_INTERFACE
    } IReplicationServerVtbl;

    interface IReplicationServer
    {
        CONST_VTBL struct IReplicationServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationServer_Initialize(This,pszServer)	\
    (This)->lpVtbl -> Initialize(This,pszServer)

#define IReplicationServer_InitializeEx(This,pszServer,pszUserName,pszPassword,lTimeout)	\
    (This)->lpVtbl -> InitializeEx(This,pszServer,pszUserName,pszPassword,lTimeout)

#define IReplicationServer_OpenProject(This,pszProject,cpcCreationFlags,pvInfProject)	\
    (This)->lpVtbl -> OpenProject(This,pszProject,cpcCreationFlags,pvInfProject)

#define IReplicationServer_DeleteProject(This,pszProject)	\
    (This)->lpVtbl -> DeleteProject(This,pszProject)

#define IReplicationServer_EnumProjects(This,pvIterator,pvInfProject)	\
    (This)->lpVtbl -> EnumProjects(This,pvIterator,pvInfProject)

#define IReplicationServer_EnumServers(This,pvIterator,Flags,pvbstrServer)	\
    (This)->lpVtbl -> EnumServers(This,pvIterator,Flags,pvbstrServer)

#define IReplicationServer_OpenPostingAcceptor(This,pvIDispPA)	\
    (This)->lpVtbl -> OpenPostingAcceptor(This,pvIDispPA)

#define IReplicationServer_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IReplicationServer_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IReplicationServer_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IReplicationServer_Continue(This)	\
    (This)->lpVtbl -> Continue(This)

#define IReplicationServer_Get(This,pszParmName,pvParmValue)	\
    (This)->lpVtbl -> Get(This,pszParmName,pvParmValue)

#define IReplicationServer_Put(This,pszParmName,vParmValue)	\
    (This)->lpVtbl -> Put(This,pszParmName,vParmValue)

#define IReplicationServer_Enum(This,pvIterator,pvParmName,pvParmValue)	\
    (This)->lpVtbl -> Enum(This,pvIterator,pvParmName,pvParmValue)

#define IReplicationServer_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#define IReplicationServer_AddRoute(This,pszRoute,pvRoute)	\
    (This)->lpVtbl -> AddRoute(This,pszRoute,pvRoute)

#define IReplicationServer_DeleteRoute(This,pszRoute)	\
    (This)->lpVtbl -> DeleteRoute(This,pszRoute)

#define IReplicationServer_EnumRoutes(This,pvIterator,pvRoute)	\
    (This)->lpVtbl -> EnumRoutes(This,pvIterator,pvRoute)

#define IReplicationServer_AddEventSink(This,bstrSink,pvdispSink)	\
    (This)->lpVtbl -> AddEventSink(This,bstrSink,pvdispSink)

#define IReplicationServer_DeleteEventSink(This,bstrSink)	\
    (This)->lpVtbl -> DeleteEventSink(This,bstrSink)

#define IReplicationServer_EnumEventSinks(This,pvIterator,pvdispSink)	\
    (This)->lpVtbl -> EnumEventSinks(This,pvIterator,pvdispSink)

#define IReplicationServer_EnumReplications(This,pszMatchProject,lMatchState,pvIterator,pvReplInfo)	\
    (This)->lpVtbl -> EnumReplications(This,pszMatchProject,lMatchState,pvIterator,pvReplInfo)

#define IReplicationServer_CheckAccess(This,pvlAccessMask)	\
    (This)->lpVtbl -> CheckAccess(This,pvlAccessMask)

#define IReplicationServer_MapUrl(This,bstrUrl,vbstrSuffix,vdispProject)	\
    (This)->lpVtbl -> MapUrl(This,bstrUrl,vbstrSuffix,vdispProject)

#define IReplicationServer_TranslateEventCode(This,lEventCode,vbstrDescription,vlSeverity,vlCategory)	\
    (This)->lpVtbl -> TranslateEventCode(This,lEventCode,vbstrDescription,vlSeverity,vlCategory)

#define IReplicationServer_ClearEvents(This,bstrEventSink)	\
    (This)->lpVtbl -> ClearEvents(This,bstrEventSink)

#define IReplicationServer_put_UseTransactions(This,pszTrans)	\
    (This)->lpVtbl -> put_UseTransactions(This,pszTrans)

#define IReplicationServer_get_UseTransactions(This,pvTrans)	\
    (This)->lpVtbl -> get_UseTransactions(This,pvTrans)

#define IReplicationServer_get_ServerName(This,pvServer)	\
    (This)->lpVtbl -> get_ServerName(This,pvServer)

#define IReplicationServer_get_Version(This,pvVersion)	\
    (This)->lpVtbl -> get_Version(This,pvVersion)

#define IReplicationServer_EventTimeToLocalTime(This,vdateEventTime,pvdateLocalTime)	\
    (This)->lpVtbl -> EventTimeToLocalTime(This,vdateEventTime,pvdateLocalTime)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Initialize_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [in] */ BSTR pszServer);


void __RPC_STUB IReplicationServer_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_InitializeEx_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [in] */ BSTR pszServer,
    /* [in] */ BSTR pszUserName,
    /* [in] */ BSTR pszPassword,
    /* [in] */ LONG lTimeout);


void __RPC_STUB IReplicationServer_InitializeEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_OpenProject_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR pszProject,
    /* [in] */ CRS_PROJECT_CREATION cpcCreationFlags,
    /* [retval][out] */ VARIANT __RPC_FAR *pvInfProject);


void __RPC_STUB IReplicationServer_OpenProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_DeleteProject_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR pszProject);


void __RPC_STUB IReplicationServer_DeleteProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_EnumProjects_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvInfProject);


void __RPC_STUB IReplicationServer_EnumProjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IReplicationServer_EnumServers_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [in] */ CRS_SERVER_TYPES Flags,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrServer);


void __RPC_STUB IReplicationServer_EnumServers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_OpenPostingAcceptor_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvIDispPA);


void __RPC_STUB IReplicationServer_OpenPostingAcceptor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Start_Proxy( 
    IReplicationServer __RPC_FAR * This);


void __RPC_STUB IReplicationServer_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Stop_Proxy( 
    IReplicationServer __RPC_FAR * This);


void __RPC_STUB IReplicationServer_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Pause_Proxy( 
    IReplicationServer __RPC_FAR * This);


void __RPC_STUB IReplicationServer_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Continue_Proxy( 
    IReplicationServer __RPC_FAR * This);


void __RPC_STUB IReplicationServer_Continue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Get_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR pszParmName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);


void __RPC_STUB IReplicationServer_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Put_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR pszParmName,
    /* [in] */ VARIANT vParmValue);


void __RPC_STUB IReplicationServer_Put_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Enum_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [out] */ VARIANT __RPC_FAR *pvParmName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);


void __RPC_STUB IReplicationServer_Enum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_Commit_Proxy( 
    IReplicationServer __RPC_FAR * This);


void __RPC_STUB IReplicationServer_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_AddRoute_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR pszRoute,
    /* [retval][out] */ VARIANT __RPC_FAR *pvRoute);


void __RPC_STUB IReplicationServer_AddRoute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_DeleteRoute_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR pszRoute);


void __RPC_STUB IReplicationServer_DeleteRoute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_EnumRoutes_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvRoute);


void __RPC_STUB IReplicationServer_EnumRoutes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_AddEventSink_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR bstrSink,
    /* [retval][out] */ VARIANT __RPC_FAR *pvdispSink);


void __RPC_STUB IReplicationServer_AddEventSink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_DeleteEventSink_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR bstrSink);


void __RPC_STUB IReplicationServer_DeleteEventSink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_EnumEventSinks_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvdispSink);


void __RPC_STUB IReplicationServer_EnumEventSinks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_EnumReplications_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR pszMatchProject,
    /* [in] */ LONG lMatchState,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvReplInfo);


void __RPC_STUB IReplicationServer_EnumReplications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_CheckAccess_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvlAccessMask);


void __RPC_STUB IReplicationServer_CheckAccess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_MapUrl_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR bstrUrl,
    /* [out] */ VARIANT __RPC_FAR *vbstrSuffix,
    /* [retval][out] */ VARIANT __RPC_FAR *vdispProject);


void __RPC_STUB IReplicationServer_MapUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_TranslateEventCode_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [in] */ LONG lEventCode,
    /* [out] */ VARIANT __RPC_FAR *vbstrDescription,
    /* [out] */ VARIANT __RPC_FAR *vlSeverity,
    /* [out] */ VARIANT __RPC_FAR *vlCategory);


void __RPC_STUB IReplicationServer_TranslateEventCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_ClearEvents_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR bstrEventSink);


void __RPC_STUB IReplicationServer_ClearEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_put_UseTransactions_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [string][in] */ BSTR pszTrans);


void __RPC_STUB IReplicationServer_put_UseTransactions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_get_UseTransactions_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvTrans);


void __RPC_STUB IReplicationServer_get_UseTransactions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_get_ServerName_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvServer);


void __RPC_STUB IReplicationServer_get_ServerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_get_Version_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvVersion);


void __RPC_STUB IReplicationServer_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationServer_EventTimeToLocalTime_Proxy( 
    IReplicationServer __RPC_FAR * This,
    /* [in] */ VARIANT vdateEventTime,
    /* [retval][out] */ VARIANT __RPC_FAR *pvdateLocalTime);


void __RPC_STUB IReplicationServer_EventTimeToLocalTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationServer_INTERFACE_DEFINED__ */


#ifndef __IReplicationProject_INTERFACE_DEFINED__
#define __IReplicationProject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationProject
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationProject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("ABEB9803-DB7A-11d0-A800-00C04FC9DAA5")
    IReplicationProject : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [string][in] */ BSTR pszParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Put( 
            /* [string][in] */ BSTR pszParmName,
            /* [in] */ VARIANT vParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Enum( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out][in] */ VARIANT __RPC_FAR *pvParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Start( 
            /* [in] */ LONG lFlags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvReplInst) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Rollback( 
            /* [in] */ LONG lNumRollbacks) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumReplications( 
            /* [in] */ LONG lState,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvReplInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumItems( 
            /* [in] */ BSTR bstrSubdir,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvItemInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddSchedule( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvdispSchedule) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumSchedules( 
            /* [out][in] */ VARIANT __RPC_FAR *pvlIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdispSchedule) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ApplyTransaction( 
            /* [in] */ LONG lFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CheckAccess( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvlAccessMask) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GrantAccess( 
            /* [string][in] */ BSTR pszUser,
            /* [in] */ LONG lAccessMask) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveAccess( 
            /* [string][in] */ BSTR pszUser) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAccess( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out] */ VARIANT __RPC_FAR *pvAccess,
            /* [retval][out] */ VARIANT __RPC_FAR *pvUserName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddDestination( 
            /* [string][in] */ BSTR pszDestination) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumDestination( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvDest) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveDestination( 
            /* [string][in] */ BSTR pszDestination) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [string][in] */ BSTR pszName) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvName) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_ReplicationMethod( 
            /* [string][in] */ BSTR pszMethod) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_ReplicationMethod( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvMethod) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_LocalDirectory( 
            /* [string][in] */ BSTR pszLocalDir) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_LocalDirectory( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvLocalDir) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Flags( 
            /* [in] */ LONG lFlags) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Flags( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StartReplicationClient( 
            /* [in] */ LONG lReserved,
            /* [retval][out] */ VARIANT __RPC_FAR *pvRetVar) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationProjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationProject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationProject __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationProject __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Put )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszParmName,
            /* [in] */ VARIANT vParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Enum )( 
            IReplicationProject __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out][in] */ VARIANT __RPC_FAR *pvParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IReplicationProject __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ LONG lFlags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvReplInst);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            IReplicationProject __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Rollback )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ LONG lNumRollbacks);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumReplications )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ LONG lState,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvReplInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumItems )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ BSTR bstrSubdir,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvItemInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddSchedule )( 
            IReplicationProject __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdispSchedule);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumSchedules )( 
            IReplicationProject __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvlIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdispSchedule);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ApplyTransaction )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ LONG lFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CheckAccess )( 
            IReplicationProject __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvlAccessMask);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GrantAccess )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszUser,
            /* [in] */ LONG lAccessMask);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveAccess )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszUser);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumAccess )( 
            IReplicationProject __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out] */ VARIANT __RPC_FAR *pvAccess,
            /* [retval][out] */ VARIANT __RPC_FAR *pvUserName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddDestination )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszDestination);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumDestination )( 
            IReplicationProject __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvDest);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveDestination )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszDestination);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszName);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IReplicationProject __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvName);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReplicationMethod )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszMethod);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReplicationMethod )( 
            IReplicationProject __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvMethod);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LocalDirectory )( 
            IReplicationProject __RPC_FAR * This,
            /* [string][in] */ BSTR pszLocalDir);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LocalDirectory )( 
            IReplicationProject __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvLocalDir);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Flags )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ LONG lFlags);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Flags )( 
            IReplicationProject __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartReplicationClient )( 
            IReplicationProject __RPC_FAR * This,
            /* [in] */ LONG lReserved,
            /* [retval][out] */ VARIANT __RPC_FAR *pvRetVar);
        
        END_INTERFACE
    } IReplicationProjectVtbl;

    interface IReplicationProject
    {
        CONST_VTBL struct IReplicationProjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationProject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationProject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationProject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationProject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationProject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationProject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationProject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationProject_Get(This,pszParmName,pvParmValue)	\
    (This)->lpVtbl -> Get(This,pszParmName,pvParmValue)

#define IReplicationProject_Put(This,pszParmName,vParmValue)	\
    (This)->lpVtbl -> Put(This,pszParmName,vParmValue)

#define IReplicationProject_Enum(This,pvIterator,pvParmName,pvParmValue)	\
    (This)->lpVtbl -> Enum(This,pvIterator,pvParmName,pvParmValue)

#define IReplicationProject_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#define IReplicationProject_Start(This,lFlags,pvReplInst)	\
    (This)->lpVtbl -> Start(This,lFlags,pvReplInst)

#define IReplicationProject_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define IReplicationProject_Rollback(This,lNumRollbacks)	\
    (This)->lpVtbl -> Rollback(This,lNumRollbacks)

#define IReplicationProject_EnumReplications(This,lState,pvIterator,pvReplInfo)	\
    (This)->lpVtbl -> EnumReplications(This,lState,pvIterator,pvReplInfo)

#define IReplicationProject_EnumItems(This,bstrSubdir,pvIterator,pvItemInfo)	\
    (This)->lpVtbl -> EnumItems(This,bstrSubdir,pvIterator,pvItemInfo)

#define IReplicationProject_AddSchedule(This,pvdispSchedule)	\
    (This)->lpVtbl -> AddSchedule(This,pvdispSchedule)

#define IReplicationProject_EnumSchedules(This,pvlIterator,pvdispSchedule)	\
    (This)->lpVtbl -> EnumSchedules(This,pvlIterator,pvdispSchedule)

#define IReplicationProject_ApplyTransaction(This,lFlags)	\
    (This)->lpVtbl -> ApplyTransaction(This,lFlags)

#define IReplicationProject_CheckAccess(This,pvlAccessMask)	\
    (This)->lpVtbl -> CheckAccess(This,pvlAccessMask)

#define IReplicationProject_GrantAccess(This,pszUser,lAccessMask)	\
    (This)->lpVtbl -> GrantAccess(This,pszUser,lAccessMask)

#define IReplicationProject_RemoveAccess(This,pszUser)	\
    (This)->lpVtbl -> RemoveAccess(This,pszUser)

#define IReplicationProject_EnumAccess(This,pvIterator,pvAccess,pvUserName)	\
    (This)->lpVtbl -> EnumAccess(This,pvIterator,pvAccess,pvUserName)

#define IReplicationProject_AddDestination(This,pszDestination)	\
    (This)->lpVtbl -> AddDestination(This,pszDestination)

#define IReplicationProject_EnumDestination(This,pvIterator,pvDest)	\
    (This)->lpVtbl -> EnumDestination(This,pvIterator,pvDest)

#define IReplicationProject_RemoveDestination(This,pszDestination)	\
    (This)->lpVtbl -> RemoveDestination(This,pszDestination)

#define IReplicationProject_put_Name(This,pszName)	\
    (This)->lpVtbl -> put_Name(This,pszName)

#define IReplicationProject_get_Name(This,pvName)	\
    (This)->lpVtbl -> get_Name(This,pvName)

#define IReplicationProject_put_ReplicationMethod(This,pszMethod)	\
    (This)->lpVtbl -> put_ReplicationMethod(This,pszMethod)

#define IReplicationProject_get_ReplicationMethod(This,pvMethod)	\
    (This)->lpVtbl -> get_ReplicationMethod(This,pvMethod)

#define IReplicationProject_put_LocalDirectory(This,pszLocalDir)	\
    (This)->lpVtbl -> put_LocalDirectory(This,pszLocalDir)

#define IReplicationProject_get_LocalDirectory(This,pvLocalDir)	\
    (This)->lpVtbl -> get_LocalDirectory(This,pvLocalDir)

#define IReplicationProject_put_Flags(This,lFlags)	\
    (This)->lpVtbl -> put_Flags(This,lFlags)

#define IReplicationProject_get_Flags(This,pvFlags)	\
    (This)->lpVtbl -> get_Flags(This,pvFlags)

#define IReplicationProject_StartReplicationClient(This,lReserved,pvRetVar)	\
    (This)->lpVtbl -> StartReplicationClient(This,lReserved,pvRetVar)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_Get_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszParmName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);


void __RPC_STUB IReplicationProject_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_Put_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszParmName,
    /* [in] */ VARIANT vParmValue);


void __RPC_STUB IReplicationProject_Put_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_Enum_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [out][in] */ VARIANT __RPC_FAR *pvParmName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);


void __RPC_STUB IReplicationProject_Enum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_Commit_Proxy( 
    IReplicationProject __RPC_FAR * This);


void __RPC_STUB IReplicationProject_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_Start_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [in] */ LONG lFlags,
    /* [retval][out] */ VARIANT __RPC_FAR *pvReplInst);


void __RPC_STUB IReplicationProject_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_Cancel_Proxy( 
    IReplicationProject __RPC_FAR * This);


void __RPC_STUB IReplicationProject_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_Rollback_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [in] */ LONG lNumRollbacks);


void __RPC_STUB IReplicationProject_Rollback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_EnumReplications_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [in] */ LONG lState,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvReplInfo);


void __RPC_STUB IReplicationProject_EnumReplications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_EnumItems_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [in] */ BSTR bstrSubdir,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvItemInfo);


void __RPC_STUB IReplicationProject_EnumItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_AddSchedule_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvdispSchedule);


void __RPC_STUB IReplicationProject_AddSchedule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_EnumSchedules_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvlIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvdispSchedule);


void __RPC_STUB IReplicationProject_EnumSchedules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_ApplyTransaction_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [in] */ LONG lFlags);


void __RPC_STUB IReplicationProject_ApplyTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_CheckAccess_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvlAccessMask);


void __RPC_STUB IReplicationProject_CheckAccess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_GrantAccess_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszUser,
    /* [in] */ LONG lAccessMask);


void __RPC_STUB IReplicationProject_GrantAccess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_RemoveAccess_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszUser);


void __RPC_STUB IReplicationProject_RemoveAccess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IReplicationProject_EnumAccess_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [out] */ VARIANT __RPC_FAR *pvAccess,
    /* [retval][out] */ VARIANT __RPC_FAR *pvUserName);


void __RPC_STUB IReplicationProject_EnumAccess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_AddDestination_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszDestination);


void __RPC_STUB IReplicationProject_AddDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_EnumDestination_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvDest);


void __RPC_STUB IReplicationProject_EnumDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_RemoveDestination_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszDestination);


void __RPC_STUB IReplicationProject_RemoveDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_put_Name_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszName);


void __RPC_STUB IReplicationProject_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_get_Name_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvName);


void __RPC_STUB IReplicationProject_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_put_ReplicationMethod_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszMethod);


void __RPC_STUB IReplicationProject_put_ReplicationMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_get_ReplicationMethod_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvMethod);


void __RPC_STUB IReplicationProject_get_ReplicationMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_put_LocalDirectory_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [string][in] */ BSTR pszLocalDir);


void __RPC_STUB IReplicationProject_put_LocalDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_get_LocalDirectory_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvLocalDir);


void __RPC_STUB IReplicationProject_get_LocalDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_put_Flags_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [in] */ LONG lFlags);


void __RPC_STUB IReplicationProject_put_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_get_Flags_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvFlags);


void __RPC_STUB IReplicationProject_get_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationProject_StartReplicationClient_Proxy( 
    IReplicationProject __RPC_FAR * This,
    /* [in] */ LONG lReserved,
    /* [retval][out] */ VARIANT __RPC_FAR *pvRetVar);


void __RPC_STUB IReplicationProject_StartReplicationClient_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationProject_INTERFACE_DEFINED__ */


#ifndef __IReplicationItem_INTERFACE_DEFINED__
#define __IReplicationItem_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationItem
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("ABEB9808-DB7A-11d0-A800-00C04FC9DAA5")
    IReplicationItem : public IDispatch
    {
    public:
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvName) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvAttrs) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_CreationTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvCreationTime) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_LastAccessTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvLastAccessTime) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_LastModifiedTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvLastModified) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_SizeLow( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvSizeLow) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_SizeHigh( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvSizeHigh) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationItem __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationItem __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationItem __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationItem __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationItem __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationItem __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationItem __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IReplicationItem __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvName);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            IReplicationItem __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvAttrs);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreationTime )( 
            IReplicationItem __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvCreationTime);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LastAccessTime )( 
            IReplicationItem __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvLastAccessTime);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LastModifiedTime )( 
            IReplicationItem __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvLastModified);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SizeLow )( 
            IReplicationItem __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvSizeLow);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SizeHigh )( 
            IReplicationItem __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvSizeHigh);
        
        END_INTERFACE
    } IReplicationItemVtbl;

    interface IReplicationItem
    {
        CONST_VTBL struct IReplicationItemVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationItem_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationItem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationItem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationItem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationItem_get_Name(This,pvName)	\
    (This)->lpVtbl -> get_Name(This,pvName)

#define IReplicationItem_get_Attributes(This,pvAttrs)	\
    (This)->lpVtbl -> get_Attributes(This,pvAttrs)

#define IReplicationItem_get_CreationTime(This,pvCreationTime)	\
    (This)->lpVtbl -> get_CreationTime(This,pvCreationTime)

#define IReplicationItem_get_LastAccessTime(This,pvLastAccessTime)	\
    (This)->lpVtbl -> get_LastAccessTime(This,pvLastAccessTime)

#define IReplicationItem_get_LastModifiedTime(This,pvLastModified)	\
    (This)->lpVtbl -> get_LastModifiedTime(This,pvLastModified)

#define IReplicationItem_get_SizeLow(This,pvSizeLow)	\
    (This)->lpVtbl -> get_SizeLow(This,pvSizeLow)

#define IReplicationItem_get_SizeHigh(This,pvSizeHigh)	\
    (This)->lpVtbl -> get_SizeHigh(This,pvSizeHigh)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationItem_get_Name_Proxy( 
    IReplicationItem __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvName);


void __RPC_STUB IReplicationItem_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationItem_get_Attributes_Proxy( 
    IReplicationItem __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvAttrs);


void __RPC_STUB IReplicationItem_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationItem_get_CreationTime_Proxy( 
    IReplicationItem __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvCreationTime);


void __RPC_STUB IReplicationItem_get_CreationTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationItem_get_LastAccessTime_Proxy( 
    IReplicationItem __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvLastAccessTime);


void __RPC_STUB IReplicationItem_get_LastAccessTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationItem_get_LastModifiedTime_Proxy( 
    IReplicationItem __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvLastModified);


void __RPC_STUB IReplicationItem_get_LastModifiedTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationItem_get_SizeLow_Proxy( 
    IReplicationItem __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvSizeLow);


void __RPC_STUB IReplicationItem_get_SizeLow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationItem_get_SizeHigh_Proxy( 
    IReplicationItem __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvSizeHigh);


void __RPC_STUB IReplicationItem_get_SizeHigh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationItem_INTERFACE_DEFINED__ */


#ifndef __IReplicationSchedule_INTERFACE_DEFINED__
#define __IReplicationSchedule_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationSchedule
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationSchedule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("82D36BA0-1B37-11d1-8C8C-00C04FC2D3B9")
    IReplicationSchedule : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Remove( void) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Project( 
            /* [in] */ BSTR bstrProject) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Project( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrProject) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_RunOnce( 
            /* [in] */ VARIANT vdateRunOnce) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_RunOnce( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvdate) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_ScheduledAction( 
            /* [in] */ CRS_SCHEDULE_ACTION Action) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_ScheduledAction( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvAction) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Hour( 
            /* [in] */ LONG lHour) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Hour( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvlHour) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Minute( 
            /* [in] */ LONG lMinute) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Minute( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvlMinute) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Days( 
            /* [in] */ LONG lDays) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Days( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvlDays) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrDescription) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_IsRunOnce( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationScheduleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationSchedule __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationSchedule __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IReplicationSchedule __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IReplicationSchedule __RPC_FAR * This);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Project )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ BSTR bstrProject);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Project )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrProject);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RunOnce )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ VARIANT vdateRunOnce);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RunOnce )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvdate);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ScheduledAction )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ CRS_SCHEDULE_ACTION Action);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ScheduledAction )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvAction);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Hour )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ LONG lHour);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Hour )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvlHour);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Minute )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ LONG lMinute);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Minute )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvlMinute);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Days )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [in] */ LONG lDays);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Days )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvlDays);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrDescription);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsRunOnce )( 
            IReplicationSchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        END_INTERFACE
    } IReplicationScheduleVtbl;

    interface IReplicationSchedule
    {
        CONST_VTBL struct IReplicationScheduleVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationSchedule_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationSchedule_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationSchedule_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationSchedule_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationSchedule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationSchedule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationSchedule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationSchedule_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#define IReplicationSchedule_Remove(This)	\
    (This)->lpVtbl -> Remove(This)

#define IReplicationSchedule_put_Project(This,bstrProject)	\
    (This)->lpVtbl -> put_Project(This,bstrProject)

#define IReplicationSchedule_get_Project(This,pvbstrProject)	\
    (This)->lpVtbl -> get_Project(This,pvbstrProject)

#define IReplicationSchedule_put_RunOnce(This,vdateRunOnce)	\
    (This)->lpVtbl -> put_RunOnce(This,vdateRunOnce)

#define IReplicationSchedule_get_RunOnce(This,pvdate)	\
    (This)->lpVtbl -> get_RunOnce(This,pvdate)

#define IReplicationSchedule_put_ScheduledAction(This,Action)	\
    (This)->lpVtbl -> put_ScheduledAction(This,Action)

#define IReplicationSchedule_get_ScheduledAction(This,pvAction)	\
    (This)->lpVtbl -> get_ScheduledAction(This,pvAction)

#define IReplicationSchedule_put_Hour(This,lHour)	\
    (This)->lpVtbl -> put_Hour(This,lHour)

#define IReplicationSchedule_get_Hour(This,pvlHour)	\
    (This)->lpVtbl -> get_Hour(This,pvlHour)

#define IReplicationSchedule_put_Minute(This,lMinute)	\
    (This)->lpVtbl -> put_Minute(This,lMinute)

#define IReplicationSchedule_get_Minute(This,pvlMinute)	\
    (This)->lpVtbl -> get_Minute(This,pvlMinute)

#define IReplicationSchedule_put_Days(This,lDays)	\
    (This)->lpVtbl -> put_Days(This,lDays)

#define IReplicationSchedule_get_Days(This,pvlDays)	\
    (This)->lpVtbl -> get_Days(This,pvlDays)

#define IReplicationSchedule_get_Description(This,pvbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pvbstrDescription)

#define IReplicationSchedule_get_IsRunOnce(This,pVal)	\
    (This)->lpVtbl -> get_IsRunOnce(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_Commit_Proxy( 
    IReplicationSchedule __RPC_FAR * This);


void __RPC_STUB IReplicationSchedule_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_Remove_Proxy( 
    IReplicationSchedule __RPC_FAR * This);


void __RPC_STUB IReplicationSchedule_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_put_Project_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [in] */ BSTR bstrProject);


void __RPC_STUB IReplicationSchedule_put_Project_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_get_Project_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrProject);


void __RPC_STUB IReplicationSchedule_get_Project_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_put_RunOnce_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [in] */ VARIANT vdateRunOnce);


void __RPC_STUB IReplicationSchedule_put_RunOnce_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_get_RunOnce_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvdate);


void __RPC_STUB IReplicationSchedule_get_RunOnce_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_put_ScheduledAction_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [in] */ CRS_SCHEDULE_ACTION Action);


void __RPC_STUB IReplicationSchedule_put_ScheduledAction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_get_ScheduledAction_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvAction);


void __RPC_STUB IReplicationSchedule_get_ScheduledAction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_put_Hour_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [in] */ LONG lHour);


void __RPC_STUB IReplicationSchedule_put_Hour_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_get_Hour_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvlHour);


void __RPC_STUB IReplicationSchedule_get_Hour_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_put_Minute_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [in] */ LONG lMinute);


void __RPC_STUB IReplicationSchedule_put_Minute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_get_Minute_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvlMinute);


void __RPC_STUB IReplicationSchedule_get_Minute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_put_Days_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [in] */ LONG lDays);


void __RPC_STUB IReplicationSchedule_put_Days_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_get_Days_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvlDays);


void __RPC_STUB IReplicationSchedule_get_Days_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_get_Description_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrDescription);


void __RPC_STUB IReplicationSchedule_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationSchedule_get_IsRunOnce_Proxy( 
    IReplicationSchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationSchedule_get_IsRunOnce_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationSchedule_INTERFACE_DEFINED__ */


#ifndef __IReplicationEventSinkConfig_INTERFACE_DEFINED__
#define __IReplicationEventSinkConfig_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationEventSinkConfig
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationEventSinkConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("ABEB980A-DB7A-11d0-A800-00C04FC9DAA5")
    IReplicationEventSinkConfig : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RegisterForEvent( 
            /* [in] */ LONG lCode) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UnRegisterForEvent( 
            /* [in] */ LONG lCode) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsRegisteredForEvent( 
            /* [in] */ LONG lCode,
            /* [retval][out] */ VARIANT __RPC_FAR *pvboolReg) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrName) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_CLSID( 
            /* [in] */ BSTR bstrCLSID) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_CLSID( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrCLSID) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_ProjectFilter( 
            /* [string][in] */ BSTR bstrProject) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_ProjectFilter( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrProject) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_DataSource( 
            /* [in] */ BSTR bstrDSN) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_DataSource( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrDSN) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_UserName( 
            /* [in] */ BSTR bstrUser) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_UserName( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrUser) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Password( 
            /* [in] */ BSTR bstrPassword) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Password( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrPassword) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Server( 
            /* [in] */ BSTR bstrServer) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Server( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrServer) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Disabled( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Disabled( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Synchronous( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Synchronous( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_ProviderCLSID( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_ProviderCLSID( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_EventLifetime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvlEventLifetime) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_EventLifetime( 
            /* [in] */ LONG lEventLifetime) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationEventSinkConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationEventSinkConfig __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationEventSinkConfig __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IReplicationEventSinkConfig __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterForEvent )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ LONG lCode);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnRegisterForEvent )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ LONG lCode);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsRegisteredForEvent )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ LONG lCode,
            /* [retval][out] */ VARIANT __RPC_FAR *pvboolReg);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrName);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CLSID )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrCLSID);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CLSID )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrCLSID);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProjectFilter )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [string][in] */ BSTR bstrProject);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProjectFilter )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrProject);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DataSource )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrDSN);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSource )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrDSN);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UserName )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrUser);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UserName )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrUser);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Password )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrPassword);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Password )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrPassword);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Server )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrServer);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Server )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvbstrServer);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Disabled )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Disabled )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ VARIANT newVal);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Synchronous )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Synchronous )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ VARIANT newVal);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderCLSID )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderCLSID )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventLifetime )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvlEventLifetime);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EventLifetime )( 
            IReplicationEventSinkConfig __RPC_FAR * This,
            /* [in] */ LONG lEventLifetime);
        
        END_INTERFACE
    } IReplicationEventSinkConfigVtbl;

    interface IReplicationEventSinkConfig
    {
        CONST_VTBL struct IReplicationEventSinkConfigVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationEventSinkConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationEventSinkConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationEventSinkConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationEventSinkConfig_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationEventSinkConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationEventSinkConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationEventSinkConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationEventSinkConfig_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#define IReplicationEventSinkConfig_RegisterForEvent(This,lCode)	\
    (This)->lpVtbl -> RegisterForEvent(This,lCode)

#define IReplicationEventSinkConfig_UnRegisterForEvent(This,lCode)	\
    (This)->lpVtbl -> UnRegisterForEvent(This,lCode)

#define IReplicationEventSinkConfig_IsRegisteredForEvent(This,lCode,pvboolReg)	\
    (This)->lpVtbl -> IsRegisteredForEvent(This,lCode,pvboolReg)

#define IReplicationEventSinkConfig_put_Name(This,bstrName)	\
    (This)->lpVtbl -> put_Name(This,bstrName)

#define IReplicationEventSinkConfig_get_Name(This,pvbstrName)	\
    (This)->lpVtbl -> get_Name(This,pvbstrName)

#define IReplicationEventSinkConfig_put_CLSID(This,bstrCLSID)	\
    (This)->lpVtbl -> put_CLSID(This,bstrCLSID)

#define IReplicationEventSinkConfig_get_CLSID(This,pvbstrCLSID)	\
    (This)->lpVtbl -> get_CLSID(This,pvbstrCLSID)

#define IReplicationEventSinkConfig_put_ProjectFilter(This,bstrProject)	\
    (This)->lpVtbl -> put_ProjectFilter(This,bstrProject)

#define IReplicationEventSinkConfig_get_ProjectFilter(This,pvbstrProject)	\
    (This)->lpVtbl -> get_ProjectFilter(This,pvbstrProject)

#define IReplicationEventSinkConfig_put_DataSource(This,bstrDSN)	\
    (This)->lpVtbl -> put_DataSource(This,bstrDSN)

#define IReplicationEventSinkConfig_get_DataSource(This,pvbstrDSN)	\
    (This)->lpVtbl -> get_DataSource(This,pvbstrDSN)

#define IReplicationEventSinkConfig_put_UserName(This,bstrUser)	\
    (This)->lpVtbl -> put_UserName(This,bstrUser)

#define IReplicationEventSinkConfig_get_UserName(This,pvbstrUser)	\
    (This)->lpVtbl -> get_UserName(This,pvbstrUser)

#define IReplicationEventSinkConfig_put_Password(This,bstrPassword)	\
    (This)->lpVtbl -> put_Password(This,bstrPassword)

#define IReplicationEventSinkConfig_get_Password(This,pvbstrPassword)	\
    (This)->lpVtbl -> get_Password(This,pvbstrPassword)

#define IReplicationEventSinkConfig_put_Server(This,bstrServer)	\
    (This)->lpVtbl -> put_Server(This,bstrServer)

#define IReplicationEventSinkConfig_get_Server(This,pvbstrServer)	\
    (This)->lpVtbl -> get_Server(This,pvbstrServer)

#define IReplicationEventSinkConfig_get_Disabled(This,pVal)	\
    (This)->lpVtbl -> get_Disabled(This,pVal)

#define IReplicationEventSinkConfig_put_Disabled(This,newVal)	\
    (This)->lpVtbl -> put_Disabled(This,newVal)

#define IReplicationEventSinkConfig_get_Synchronous(This,pVal)	\
    (This)->lpVtbl -> get_Synchronous(This,pVal)

#define IReplicationEventSinkConfig_put_Synchronous(This,newVal)	\
    (This)->lpVtbl -> put_Synchronous(This,newVal)

#define IReplicationEventSinkConfig_get_ProviderCLSID(This,pVal)	\
    (This)->lpVtbl -> get_ProviderCLSID(This,pVal)

#define IReplicationEventSinkConfig_put_ProviderCLSID(This,newVal)	\
    (This)->lpVtbl -> put_ProviderCLSID(This,newVal)

#define IReplicationEventSinkConfig_get_EventLifetime(This,pvlEventLifetime)	\
    (This)->lpVtbl -> get_EventLifetime(This,pvlEventLifetime)

#define IReplicationEventSinkConfig_put_EventLifetime(This,lEventLifetime)	\
    (This)->lpVtbl -> put_EventLifetime(This,lEventLifetime)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_Commit_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This);


void __RPC_STUB IReplicationEventSinkConfig_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_RegisterForEvent_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ LONG lCode);


void __RPC_STUB IReplicationEventSinkConfig_RegisterForEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_UnRegisterForEvent_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ LONG lCode);


void __RPC_STUB IReplicationEventSinkConfig_UnRegisterForEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_IsRegisteredForEvent_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ LONG lCode,
    /* [retval][out] */ VARIANT __RPC_FAR *pvboolReg);


void __RPC_STUB IReplicationEventSinkConfig_IsRegisteredForEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_Name_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB IReplicationEventSinkConfig_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_Name_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrName);


void __RPC_STUB IReplicationEventSinkConfig_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_CLSID_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrCLSID);


void __RPC_STUB IReplicationEventSinkConfig_put_CLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_CLSID_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrCLSID);


void __RPC_STUB IReplicationEventSinkConfig_get_CLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_ProjectFilter_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [string][in] */ BSTR bstrProject);


void __RPC_STUB IReplicationEventSinkConfig_put_ProjectFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_ProjectFilter_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrProject);


void __RPC_STUB IReplicationEventSinkConfig_get_ProjectFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_DataSource_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrDSN);


void __RPC_STUB IReplicationEventSinkConfig_put_DataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_DataSource_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrDSN);


void __RPC_STUB IReplicationEventSinkConfig_get_DataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_UserName_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrUser);


void __RPC_STUB IReplicationEventSinkConfig_put_UserName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_UserName_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrUser);


void __RPC_STUB IReplicationEventSinkConfig_get_UserName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_Password_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrPassword);


void __RPC_STUB IReplicationEventSinkConfig_put_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_Password_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrPassword);


void __RPC_STUB IReplicationEventSinkConfig_get_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_Server_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrServer);


void __RPC_STUB IReplicationEventSinkConfig_put_Server_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_Server_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvbstrServer);


void __RPC_STUB IReplicationEventSinkConfig_get_Server_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_Disabled_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationEventSinkConfig_get_Disabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_Disabled_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IReplicationEventSinkConfig_put_Disabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_Synchronous_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationEventSinkConfig_get_Synchronous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_Synchronous_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IReplicationEventSinkConfig_put_Synchronous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_ProviderCLSID_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationEventSinkConfig_get_ProviderCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_ProviderCLSID_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IReplicationEventSinkConfig_put_ProviderCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_get_EventLifetime_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvlEventLifetime);


void __RPC_STUB IReplicationEventSinkConfig_get_EventLifetime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationEventSinkConfig_put_EventLifetime_Proxy( 
    IReplicationEventSinkConfig __RPC_FAR * This,
    /* [in] */ LONG lEventLifetime);


void __RPC_STUB IReplicationEventSinkConfig_put_EventLifetime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationEventSinkConfig_INTERFACE_DEFINED__ */


#ifndef __IReplicationInstance_INTERFACE_DEFINED__
#define __IReplicationInstance_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationInstance
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationInstance;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("ABEB9807-DB7A-11d0-A800-00C04FC9DAA5")
    IReplicationInstance : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Query( void) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvID) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvName) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Flags( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvFlags) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvState) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvState) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_StartTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvStartTime) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_EndTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvEndTime) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_FilesSent( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesSent) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_BytesSent( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvBytesSent) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_FilesReceived( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesReceived) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_BytesReceived( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvBytesReceived) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_FilesErrored( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesErrored) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_FilesErroredBytes( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesErroredBytes) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_FilesMatched( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesMatched) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_FilesMatchedBytes( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesMatchedBytes) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationInstanceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationInstance __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationInstance __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationInstance __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationInstance __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationInstance __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationInstance __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationInstance __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            IReplicationInstance __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Query )( 
            IReplicationInstance __RPC_FAR * This);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ID )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvID);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvName);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Flags )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvFlags);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvState);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvState);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartTime )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvStartTime);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EndTime )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvEndTime);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilesSent )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesSent);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BytesSent )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvBytesSent);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilesReceived )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesReceived);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BytesReceived )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvBytesReceived);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilesErrored )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesErrored);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilesErroredBytes )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesErroredBytes);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilesMatched )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesMatched);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilesMatchedBytes )( 
            IReplicationInstance __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvFilesMatchedBytes);
        
        END_INTERFACE
    } IReplicationInstanceVtbl;

    interface IReplicationInstance
    {
        CONST_VTBL struct IReplicationInstanceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationInstance_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationInstance_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationInstance_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationInstance_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationInstance_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationInstance_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationInstance_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationInstance_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define IReplicationInstance_Query(This)	\
    (This)->lpVtbl -> Query(This)

#define IReplicationInstance_get_ID(This,pvID)	\
    (This)->lpVtbl -> get_ID(This,pvID)

#define IReplicationInstance_get_Name(This,pvName)	\
    (This)->lpVtbl -> get_Name(This,pvName)

#define IReplicationInstance_get_Flags(This,pvFlags)	\
    (This)->lpVtbl -> get_Flags(This,pvFlags)

#define IReplicationInstance_get_State(This,pvState)	\
    (This)->lpVtbl -> get_State(This,pvState)

#define IReplicationInstance_get_Status(This,pvState)	\
    (This)->lpVtbl -> get_Status(This,pvState)

#define IReplicationInstance_get_StartTime(This,pvStartTime)	\
    (This)->lpVtbl -> get_StartTime(This,pvStartTime)

#define IReplicationInstance_get_EndTime(This,pvEndTime)	\
    (This)->lpVtbl -> get_EndTime(This,pvEndTime)

#define IReplicationInstance_get_FilesSent(This,pvFilesSent)	\
    (This)->lpVtbl -> get_FilesSent(This,pvFilesSent)

#define IReplicationInstance_get_BytesSent(This,pvBytesSent)	\
    (This)->lpVtbl -> get_BytesSent(This,pvBytesSent)

#define IReplicationInstance_get_FilesReceived(This,pvFilesReceived)	\
    (This)->lpVtbl -> get_FilesReceived(This,pvFilesReceived)

#define IReplicationInstance_get_BytesReceived(This,pvBytesReceived)	\
    (This)->lpVtbl -> get_BytesReceived(This,pvBytesReceived)

#define IReplicationInstance_get_FilesErrored(This,pvFilesErrored)	\
    (This)->lpVtbl -> get_FilesErrored(This,pvFilesErrored)

#define IReplicationInstance_get_FilesErroredBytes(This,pvFilesErroredBytes)	\
    (This)->lpVtbl -> get_FilesErroredBytes(This,pvFilesErroredBytes)

#define IReplicationInstance_get_FilesMatched(This,pvFilesMatched)	\
    (This)->lpVtbl -> get_FilesMatched(This,pvFilesMatched)

#define IReplicationInstance_get_FilesMatchedBytes(This,pvFilesMatchedBytes)	\
    (This)->lpVtbl -> get_FilesMatchedBytes(This,pvFilesMatchedBytes)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_Cancel_Proxy( 
    IReplicationInstance __RPC_FAR * This);


void __RPC_STUB IReplicationInstance_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_Query_Proxy( 
    IReplicationInstance __RPC_FAR * This);


void __RPC_STUB IReplicationInstance_Query_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_ID_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvID);


void __RPC_STUB IReplicationInstance_get_ID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_Name_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvName);


void __RPC_STUB IReplicationInstance_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_Flags_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvFlags);


void __RPC_STUB IReplicationInstance_get_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_State_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvState);


void __RPC_STUB IReplicationInstance_get_State_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_Status_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvState);


void __RPC_STUB IReplicationInstance_get_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_StartTime_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvStartTime);


void __RPC_STUB IReplicationInstance_get_StartTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_EndTime_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvEndTime);


void __RPC_STUB IReplicationInstance_get_EndTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_FilesSent_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvFilesSent);


void __RPC_STUB IReplicationInstance_get_FilesSent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_BytesSent_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvBytesSent);


void __RPC_STUB IReplicationInstance_get_BytesSent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_FilesReceived_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvFilesReceived);


void __RPC_STUB IReplicationInstance_get_FilesReceived_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_BytesReceived_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvBytesReceived);


void __RPC_STUB IReplicationInstance_get_BytesReceived_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_FilesErrored_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvFilesErrored);


void __RPC_STUB IReplicationInstance_get_FilesErrored_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_FilesErroredBytes_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvFilesErroredBytes);


void __RPC_STUB IReplicationInstance_get_FilesErroredBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_FilesMatched_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvFilesMatched);


void __RPC_STUB IReplicationInstance_get_FilesMatched_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationInstance_get_FilesMatchedBytes_Proxy( 
    IReplicationInstance __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvFilesMatchedBytes);


void __RPC_STUB IReplicationInstance_get_FilesMatchedBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationInstance_INTERFACE_DEFINED__ */


#ifndef __IReplicationRoute_INTERFACE_DEFINED__
#define __IReplicationRoute_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationRoute
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationRoute;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("ABEB9804-DB7A-11d0-A800-00C04FC9DAA5")
    IReplicationRoute : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [string][in] */ BSTR pszParm,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Put( 
            /* [string][in] */ BSTR pszParm,
            /* [in] */ VARIANT vParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Enum( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out][in] */ VARIANT __RPC_FAR *pvParm,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddDestination( 
            /* [string][in] */ BSTR pszDestination) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumDestination( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvDest) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveDestination( 
            /* [string][in] */ BSTR pszDestination) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvName) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [string][in] */ BSTR pszName) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_BaseDirectory( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvName) = 0;
        
        virtual /* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE put_BaseDirectory( 
            /* [string][in] */ BSTR pszName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationRouteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationRoute __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationRoute __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationRoute __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationRoute __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationRoute __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationRoute __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationRoute __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            IReplicationRoute __RPC_FAR * This,
            /* [string][in] */ BSTR pszParm,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Put )( 
            IReplicationRoute __RPC_FAR * This,
            /* [string][in] */ BSTR pszParm,
            /* [in] */ VARIANT vParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Enum )( 
            IReplicationRoute __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out][in] */ VARIANT __RPC_FAR *pvParm,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IReplicationRoute __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddDestination )( 
            IReplicationRoute __RPC_FAR * This,
            /* [string][in] */ BSTR pszDestination);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumDestination )( 
            IReplicationRoute __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [retval][out] */ VARIANT __RPC_FAR *pvDest);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveDestination )( 
            IReplicationRoute __RPC_FAR * This,
            /* [string][in] */ BSTR pszDestination);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IReplicationRoute __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvName);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IReplicationRoute __RPC_FAR * This,
            /* [string][in] */ BSTR pszName);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BaseDirectory )( 
            IReplicationRoute __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvName);
        
        /* [propput][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BaseDirectory )( 
            IReplicationRoute __RPC_FAR * This,
            /* [string][in] */ BSTR pszName);
        
        END_INTERFACE
    } IReplicationRouteVtbl;

    interface IReplicationRoute
    {
        CONST_VTBL struct IReplicationRouteVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationRoute_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationRoute_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationRoute_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationRoute_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationRoute_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationRoute_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationRoute_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationRoute_Get(This,pszParm,pvParmValue)	\
    (This)->lpVtbl -> Get(This,pszParm,pvParmValue)

#define IReplicationRoute_Put(This,pszParm,vParmValue)	\
    (This)->lpVtbl -> Put(This,pszParm,vParmValue)

#define IReplicationRoute_Enum(This,pvIterator,pvParm,pvParmValue)	\
    (This)->lpVtbl -> Enum(This,pvIterator,pvParm,pvParmValue)

#define IReplicationRoute_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#define IReplicationRoute_AddDestination(This,pszDestination)	\
    (This)->lpVtbl -> AddDestination(This,pszDestination)

#define IReplicationRoute_EnumDestination(This,pvIterator,pvDest)	\
    (This)->lpVtbl -> EnumDestination(This,pvIterator,pvDest)

#define IReplicationRoute_RemoveDestination(This,pszDestination)	\
    (This)->lpVtbl -> RemoveDestination(This,pszDestination)

#define IReplicationRoute_get_Name(This,pvName)	\
    (This)->lpVtbl -> get_Name(This,pvName)

#define IReplicationRoute_put_Name(This,pszName)	\
    (This)->lpVtbl -> put_Name(This,pszName)

#define IReplicationRoute_get_BaseDirectory(This,pvName)	\
    (This)->lpVtbl -> get_BaseDirectory(This,pvName)

#define IReplicationRoute_put_BaseDirectory(This,pszName)	\
    (This)->lpVtbl -> put_BaseDirectory(This,pszName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_Get_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [string][in] */ BSTR pszParm,
    /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);


void __RPC_STUB IReplicationRoute_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_Put_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [string][in] */ BSTR pszParm,
    /* [in] */ VARIANT vParmValue);


void __RPC_STUB IReplicationRoute_Put_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_Enum_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [out][in] */ VARIANT __RPC_FAR *pvParm,
    /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);


void __RPC_STUB IReplicationRoute_Enum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_Commit_Proxy( 
    IReplicationRoute __RPC_FAR * This);


void __RPC_STUB IReplicationRoute_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_AddDestination_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [string][in] */ BSTR pszDestination);


void __RPC_STUB IReplicationRoute_AddDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_EnumDestination_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [retval][out] */ VARIANT __RPC_FAR *pvDest);


void __RPC_STUB IReplicationRoute_EnumDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_RemoveDestination_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [string][in] */ BSTR pszDestination);


void __RPC_STUB IReplicationRoute_RemoveDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_get_Name_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvName);


void __RPC_STUB IReplicationRoute_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_put_Name_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [string][in] */ BSTR pszName);


void __RPC_STUB IReplicationRoute_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_get_BaseDirectory_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvName);


void __RPC_STUB IReplicationRoute_get_BaseDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationRoute_put_BaseDirectory_Proxy( 
    IReplicationRoute __RPC_FAR * This,
    /* [string][in] */ BSTR pszName);


void __RPC_STUB IReplicationRoute_put_BaseDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationRoute_INTERFACE_DEFINED__ */


#ifndef __IReplicationClient_INTERFACE_DEFINED__
#define __IReplicationClient_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationClient
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationClient;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("B24F0372-9692-11D0-BAD5-00C04FD7082F")
    IReplicationClient : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [string][in] */ BSTR pszProject) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Connect( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SendFile( 
            /* [string][in] */ BSTR pszFile) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SendFiles( 
            /* [in] */ VARIANT __RPC_FAR *pvFileArray) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DeleteFile( 
            /* [string][in] */ BSTR pszFile) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Abort( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetExtendedErrorInfo( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvErrorArray) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FastMode( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FastMode( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Force( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Force( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ACLs( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ACLs( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_NumDestinations( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SkipLockedFiles( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SkipLockedFiles( 
            /* [in] */ BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationClientVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationClient __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationClient __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationClient __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize )( 
            IReplicationClient __RPC_FAR * This,
            /* [string][in] */ BSTR pszProject);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            IReplicationClient __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendFile )( 
            IReplicationClient __RPC_FAR * This,
            /* [string][in] */ BSTR pszFile);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendFiles )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *pvFileArray);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteFile )( 
            IReplicationClient __RPC_FAR * This,
            /* [string][in] */ BSTR pszFile);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IReplicationClient __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Abort )( 
            IReplicationClient __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            IReplicationClient __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetExtendedErrorInfo )( 
            IReplicationClient __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvErrorArray);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FastMode )( 
            IReplicationClient __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FastMode )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Force )( 
            IReplicationClient __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Force )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ACLs )( 
            IReplicationClient __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ACLs )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumDestinations )( 
            IReplicationClient __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SkipLockedFiles )( 
            IReplicationClient __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SkipLockedFiles )( 
            IReplicationClient __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        END_INTERFACE
    } IReplicationClientVtbl;

    interface IReplicationClient
    {
        CONST_VTBL struct IReplicationClientVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationClient_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationClient_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationClient_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationClient_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationClient_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationClient_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationClient_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationClient_Initialize(This,pszProject)	\
    (This)->lpVtbl -> Initialize(This,pszProject)

#define IReplicationClient_Connect(This)	\
    (This)->lpVtbl -> Connect(This)

#define IReplicationClient_SendFile(This,pszFile)	\
    (This)->lpVtbl -> SendFile(This,pszFile)

#define IReplicationClient_SendFiles(This,pvFileArray)	\
    (This)->lpVtbl -> SendFiles(This,pvFileArray)

#define IReplicationClient_DeleteFile(This,pszFile)	\
    (This)->lpVtbl -> DeleteFile(This,pszFile)

#define IReplicationClient_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#define IReplicationClient_Abort(This)	\
    (This)->lpVtbl -> Abort(This)

#define IReplicationClient_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define IReplicationClient_GetExtendedErrorInfo(This,pvErrorArray)	\
    (This)->lpVtbl -> GetExtendedErrorInfo(This,pvErrorArray)

#define IReplicationClient_get_FastMode(This,pVal)	\
    (This)->lpVtbl -> get_FastMode(This,pVal)

#define IReplicationClient_put_FastMode(This,newVal)	\
    (This)->lpVtbl -> put_FastMode(This,newVal)

#define IReplicationClient_get_Force(This,pVal)	\
    (This)->lpVtbl -> get_Force(This,pVal)

#define IReplicationClient_put_Force(This,newVal)	\
    (This)->lpVtbl -> put_Force(This,newVal)

#define IReplicationClient_get_ACLs(This,pVal)	\
    (This)->lpVtbl -> get_ACLs(This,pVal)

#define IReplicationClient_put_ACLs(This,newVal)	\
    (This)->lpVtbl -> put_ACLs(This,newVal)

#define IReplicationClient_get_NumDestinations(This,pVal)	\
    (This)->lpVtbl -> get_NumDestinations(This,pVal)

#define IReplicationClient_get_SkipLockedFiles(This,pVal)	\
    (This)->lpVtbl -> get_SkipLockedFiles(This,pVal)

#define IReplicationClient_put_SkipLockedFiles(This,newVal)	\
    (This)->lpVtbl -> put_SkipLockedFiles(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_Initialize_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [string][in] */ BSTR pszProject);


void __RPC_STUB IReplicationClient_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_Connect_Proxy( 
    IReplicationClient __RPC_FAR * This);


void __RPC_STUB IReplicationClient_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_SendFile_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [string][in] */ BSTR pszFile);


void __RPC_STUB IReplicationClient_SendFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_SendFiles_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *pvFileArray);


void __RPC_STUB IReplicationClient_SendFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_DeleteFile_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [string][in] */ BSTR pszFile);


void __RPC_STUB IReplicationClient_DeleteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_Commit_Proxy( 
    IReplicationClient __RPC_FAR * This);


void __RPC_STUB IReplicationClient_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_Abort_Proxy( 
    IReplicationClient __RPC_FAR * This);


void __RPC_STUB IReplicationClient_Abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_Disconnect_Proxy( 
    IReplicationClient __RPC_FAR * This);


void __RPC_STUB IReplicationClient_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationClient_GetExtendedErrorInfo_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvErrorArray);


void __RPC_STUB IReplicationClient_GetExtendedErrorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClient_get_FastMode_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClient_get_FastMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClient_put_FastMode_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IReplicationClient_put_FastMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClient_get_Force_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClient_get_Force_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClient_put_Force_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IReplicationClient_put_Force_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClient_get_ACLs_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClient_get_ACLs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClient_put_ACLs_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IReplicationClient_put_ACLs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClient_get_NumDestinations_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClient_get_NumDestinations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClient_get_SkipLockedFiles_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClient_get_SkipLockedFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClient_put_SkipLockedFiles_Proxy( 
    IReplicationClient __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IReplicationClient_put_SkipLockedFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationClient_INTERFACE_DEFINED__ */


#ifndef __IReplicationClientError_INTERFACE_DEFINED__
#define __IReplicationClientError_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationClientError
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationClientError;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("414CC9F0-9F37-11d0-BADA-00C04FD7082F")
    IReplicationClientError : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorCode( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ErrorCode( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Destination( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Destination( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FileName( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FileName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Project( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Project( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Source( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationClientErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationClientError __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationClientError __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationClientError __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCode )( 
            IReplicationClientError __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ErrorCode )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IReplicationClientError __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Destination )( 
            IReplicationClientError __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Destination )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            IReplicationClientError __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FileName )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Project )( 
            IReplicationClientError __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Project )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            IReplicationClientError __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Source )( 
            IReplicationClientError __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IReplicationClientErrorVtbl;

    interface IReplicationClientError
    {
        CONST_VTBL struct IReplicationClientErrorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationClientError_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationClientError_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationClientError_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationClientError_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationClientError_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationClientError_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationClientError_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationClientError_get_ErrorCode(This,pVal)	\
    (This)->lpVtbl -> get_ErrorCode(This,pVal)

#define IReplicationClientError_put_ErrorCode(This,newVal)	\
    (This)->lpVtbl -> put_ErrorCode(This,newVal)

#define IReplicationClientError_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IReplicationClientError_put_Description(This,newVal)	\
    (This)->lpVtbl -> put_Description(This,newVal)

#define IReplicationClientError_get_Destination(This,pVal)	\
    (This)->lpVtbl -> get_Destination(This,pVal)

#define IReplicationClientError_put_Destination(This,newVal)	\
    (This)->lpVtbl -> put_Destination(This,newVal)

#define IReplicationClientError_get_FileName(This,pVal)	\
    (This)->lpVtbl -> get_FileName(This,pVal)

#define IReplicationClientError_put_FileName(This,newVal)	\
    (This)->lpVtbl -> put_FileName(This,newVal)

#define IReplicationClientError_get_Project(This,pVal)	\
    (This)->lpVtbl -> get_Project(This,pVal)

#define IReplicationClientError_put_Project(This,newVal)	\
    (This)->lpVtbl -> put_Project(This,newVal)

#define IReplicationClientError_get_Source(This,pVal)	\
    (This)->lpVtbl -> get_Source(This,pVal)

#define IReplicationClientError_put_Source(This,newVal)	\
    (This)->lpVtbl -> put_Source(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_get_ErrorCode_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClientError_get_ErrorCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_put_ErrorCode_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IReplicationClientError_put_ErrorCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_get_Description_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClientError_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_put_Description_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IReplicationClientError_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_get_Destination_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClientError_get_Destination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_put_Destination_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IReplicationClientError_put_Destination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_get_FileName_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClientError_get_FileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_put_FileName_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IReplicationClientError_put_FileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_get_Project_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClientError_get_Project_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_put_Project_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IReplicationClientError_put_Project_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_get_Source_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IReplicationClientError_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IReplicationClientError_put_Source_Proxy( 
    IReplicationClientError __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IReplicationClientError_put_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationClientError_INTERFACE_DEFINED__ */


#ifndef __IReplicationPostingAcceptor_INTERFACE_DEFINED__
#define __IReplicationPostingAcceptor_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IReplicationPostingAcceptor
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IReplicationPostingAcceptor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("5163ce6b-1d76-11d1-a32b-0000f8014963")
    IReplicationPostingAcceptor : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [string][in] */ BSTR pszParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Put( 
            /* [string][in] */ BSTR pszParmName,
            /* [in] */ VARIANT vParmValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Enum( 
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out][in] */ VARIANT __RPC_FAR *pvParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReplicationPostingAcceptorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IReplicationPostingAcceptor __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IReplicationPostingAcceptor __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IReplicationPostingAcceptor __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IReplicationPostingAcceptor __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IReplicationPostingAcceptor __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IReplicationPostingAcceptor __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IReplicationPostingAcceptor __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            IReplicationPostingAcceptor __RPC_FAR * This,
            /* [string][in] */ BSTR pszParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Put )( 
            IReplicationPostingAcceptor __RPC_FAR * This,
            /* [string][in] */ BSTR pszParmName,
            /* [in] */ VARIANT vParmValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Enum )( 
            IReplicationPostingAcceptor __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
            /* [out][in] */ VARIANT __RPC_FAR *pvParmName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);
        
        END_INTERFACE
    } IReplicationPostingAcceptorVtbl;

    interface IReplicationPostingAcceptor
    {
        CONST_VTBL struct IReplicationPostingAcceptorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReplicationPostingAcceptor_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IReplicationPostingAcceptor_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IReplicationPostingAcceptor_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IReplicationPostingAcceptor_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IReplicationPostingAcceptor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IReplicationPostingAcceptor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IReplicationPostingAcceptor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IReplicationPostingAcceptor_Get(This,pszParmName,pvParmValue)	\
    (This)->lpVtbl -> Get(This,pszParmName,pvParmValue)

#define IReplicationPostingAcceptor_Put(This,pszParmName,vParmValue)	\
    (This)->lpVtbl -> Put(This,pszParmName,vParmValue)

#define IReplicationPostingAcceptor_Enum(This,pvIterator,pvParmName,pvParmValue)	\
    (This)->lpVtbl -> Enum(This,pvIterator,pvParmName,pvParmValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationPostingAcceptor_Get_Proxy( 
    IReplicationPostingAcceptor __RPC_FAR * This,
    /* [string][in] */ BSTR pszParmName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);


void __RPC_STUB IReplicationPostingAcceptor_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationPostingAcceptor_Put_Proxy( 
    IReplicationPostingAcceptor __RPC_FAR * This,
    /* [string][in] */ BSTR pszParmName,
    /* [in] */ VARIANT vParmValue);


void __RPC_STUB IReplicationPostingAcceptor_Put_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IReplicationPostingAcceptor_Enum_Proxy( 
    IReplicationPostingAcceptor __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pvIterator,
    /* [out][in] */ VARIANT __RPC_FAR *pvParmName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvParmValue);


void __RPC_STUB IReplicationPostingAcceptor_Enum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IReplicationPostingAcceptor_INTERFACE_DEFINED__ */



#ifndef __CRSAPILib_LIBRARY_DEFINED__
#define __CRSAPILib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: CRSAPILib
 * at Thu Apr 23 13:37:19 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_CRSAPILib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationServer;

class DECLSPEC_UUID("66D6D7ED-22D2-11D0-9B98-00C04FD5CD09")
CReplicationServer;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationProject;

class DECLSPEC_UUID("66D6D7F2-22D2-11D0-9B98-00C04FD5CD09")
CReplicationProject;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationInstance;

class DECLSPEC_UUID("66D6D7F7-22D2-11D0-9B98-00C04FD5CD09")
CReplicationInstance;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationRoute;

class DECLSPEC_UUID("66D6D7FC-22D2-11D0-9B98-00C04FD5CD09")
CReplicationRoute;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationClient;

class DECLSPEC_UUID("B24F0373-9692-11D0-BAD5-00C04FD7082F")
CReplicationClient;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationClientError;

class DECLSPEC_UUID("6AF9D110-9F37-11d0-BADA-00C04FD7082F")
CReplicationClientError;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationEventSinkConfig;

class DECLSPEC_UUID("66D6D7FD-22D2-11D0-9B98-00C04FD5CD09")
CReplicationEventSinkConfig;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationItem;

class DECLSPEC_UUID("66D6D7FE-22D2-11D0-9B98-00C04FD5CD09")
CReplicationItem;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationSchedule;

class DECLSPEC_UUID("4E28F5F0-1B3C-11d1-8C8C-00C04FC2D3B9")
CReplicationSchedule;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CReplicationPostingAcceptor;

class DECLSPEC_UUID("055f638a-1d77-11d1-a32b-0000f8014963")
CReplicationPostingAcceptor;
#endif
#endif /* __CRSAPILib_LIBRARY_DEFINED__ */

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
