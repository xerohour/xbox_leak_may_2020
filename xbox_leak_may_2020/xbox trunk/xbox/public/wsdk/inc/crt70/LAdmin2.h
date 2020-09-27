/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 20:01:24 1998
 */
/* Compiler settings for ladmin2.idl:
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

#ifndef __ladmin2_h__
#define __ladmin2_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __Ildapcfg_FWD_DEFINED__
#define __Ildapcfg_FWD_DEFINED__
typedef interface Ildapcfg Ildapcfg;
#endif 	/* __Ildapcfg_FWD_DEFINED__ */


#ifndef __ldapcfg_FWD_DEFINED__
#define __ldapcfg_FWD_DEFINED__

#ifdef __cplusplus
typedef class ldapcfg ldapcfg;
#else
typedef struct ldapcfg ldapcfg;
#endif /* __cplusplus */

#endif 	/* __ldapcfg_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __Ildapcfg_INTERFACE_DEFINED__
#define __Ildapcfg_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: Ildapcfg
 * at Wed Apr 22 20:01:24 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_Ildapcfg;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("40E9AD63-E2A1-11D0-920F-00C04FB954C7")
    Ildapcfg : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AttachToRemoteMachine( 
            /* [in] */ VARIANT szMachineName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AttachToLocalMachine( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadConfig( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveConfig( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateConfig( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DeleteConfig( 
            /* [in] */ VARIANT lInstanceId) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Port( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Port( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SecurePort( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SecurePort( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IP( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_IP( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SupportedIPs( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DNSName( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DNSName( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ReadOnlyMode( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ReadOnlyMode( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableAllSub( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableAllSub( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MaxPageSize( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MaxPageSize( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MaxResultSet( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MaxResultSet( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MaxQueryTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MaxQueryTime( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MaxConnection( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MaxConnection( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionTimeout( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionTimeout( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableShortTermIPBlacklisting( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableShortTermIPBlacklisting( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableShortTermAccountBlacklisting( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableShortTermAccountBlacklisting( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Authorization( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Authorization( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableSSL( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableSSL( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableSSL128( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableSSL128( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableDynamic( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableDynamic( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MinTTL( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MinTTL( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MaxDynamObj( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MaxDynamObj( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableRTPerson( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableRTPerson( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableReplicate( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableReplicate( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableLog( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnableLog( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DBType( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DBType( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DBSource( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DBSource( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DBName( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DBName( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DBUsername( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DBUsername( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DBPassword( 
            /* [retval][out] */ VARIANT __RPC_FAR *pData) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DBPassword( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetServerState( 
            /* [in] */ VARIANT Instance,
            /* [out] */ VARIANT __RPC_FAR *lState) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StartServer( 
            /* [in] */ VARIANT lInstanceId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StopServer( 
            /* [in] */ VARIANT lInstanceId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PauseServer( 
            /* [in] */ VARIANT lInstanceId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ContinueServer( 
            /* [in] */ VARIANT lInstanceId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CompactDatabase( 
            /* [in] */ VARIANT lInstanceId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RepairDatabase( 
            /* [in] */ VARIANT lInstanceId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDynamicReplicationList( 
            /* [in] */ VARIANT lInstanceId,
            /* [out] */ VARIANT __RPC_FAR *rgszReplNames,
            /* [out] */ VARIANT __RPC_FAR *rgszReplIDs) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDirectoryProperties( 
            /* [in] */ VARIANT lInstanceId,
            /* [out] */ VARIANT __RPC_FAR *szRealm,
            /* [out] */ VARIANT __RPC_FAR *szDnPrefix,
            /* [out] */ VARIANT __RPC_FAR *fIsSQL,
            /* [out] */ VARIANT __RPC_FAR *fExtSec,
            /* [out] */ VARIANT __RPC_FAR *rgszContainerPartitions) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDirectoryProperties( 
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szRealm,
            /* [in] */ VARIANT szDnPrefix) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSubPartitions( 
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainer,
            /* [out] */ VARIANT __RPC_FAR *rgdwSubPartitionNumber) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPartitionDbInfo( 
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [out] */ VARIANT __RPC_FAR *rgdwServerId,
            /* [out] */ VARIANT __RPC_FAR *rgszServerName,
            /* [out] */ VARIANT __RPC_FAR *rgszDatabaseName,
            /* [out] */ VARIANT __RPC_FAR *rgszLogin,
            /* [out] */ VARIANT __RPC_FAR *rgszPassword,
            /* [out] */ VARIANT __RPC_FAR *rgdwMaxCnx,
            /* [out] */ VARIANT __RPC_FAR *rgdwTimeout,
            /* [out] */ VARIANT __RPC_FAR *rgdwReplicationType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPartitionDbServer( 
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [in] */ VARIANT dwServerId,
            /* [out] */ VARIANT __RPC_FAR *szServerName,
            /* [out] */ VARIANT __RPC_FAR *szDatabaseName,
            /* [out] */ VARIANT __RPC_FAR *szLogin,
            /* [out] */ VARIANT __RPC_FAR *szPassword,
            /* [out] */ VARIANT __RPC_FAR *dwMaxCnx,
            /* [out] */ VARIANT __RPC_FAR *dwTimeout,
            /* [out] */ VARIANT __RPC_FAR *dwReplicationType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddPartitionDbServer( 
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [out] */ VARIANT __RPC_FAR *dwServerId,
            /* [in] */ VARIANT szServerName,
            /* [in] */ VARIANT szDatabaseName,
            /* [in] */ VARIANT szLogin,
            /* [in] */ VARIANT szPassword,
            /* [in] */ VARIANT dwMaxCnx,
            /* [in] */ VARIANT dwTimeout,
            /* [in] */ VARIANT dwReplicationType,
            /* [in] */ VARIANT lSubrefPartitionID,
            /* [in] */ VARIANT lSubrefObjectID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EditPartitionDbServer( 
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [in] */ VARIANT dwServerId,
            /* [in] */ VARIANT szServerName,
            /* [in] */ VARIANT szDatabaseName,
            /* [in] */ VARIANT szLogin,
            /* [in] */ VARIANT szPassword,
            /* [in] */ VARIANT dwMaxCnx,
            /* [in] */ VARIANT dwTimeout,
            /* [in] */ VARIANT dwReplicationType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemovePartitionDbServer( 
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [in] */ VARIANT dwServerId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateDSPartition( 
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szDn,
            /* [in] */ VARIANT lCount,
            /* [out] */ VARIANT __RPC_FAR *plPartitionID,
            /* [out] */ VARIANT __RPC_FAR *plObjectID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ListAllReplica( 
            /* [out] */ VARIANT __RPC_FAR *rgServerId,
            /* [out] */ VARIANT __RPC_FAR *rgszServerName,
            /* [out] */ VARIANT __RPC_FAR *rgszRpcGuid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddReplica( 
            /* [in] */ long lServerId,
            /* [in] */ BSTR szServerName,
            /* [in] */ BSTR szRpcGuid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DelReplica( 
            /* [in] */ long lServerId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConfigLocalReplica( 
            /* [in] */ long lNewServerId,
            /* [out] */ VARIANT __RPC_FAR *szRpcGuid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE QueryLocalReplica( 
            /* [out] */ VARIANT __RPC_FAR *lServerId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsAddrInUse( 
            /* [in] */ long lIP,
            /* [in] */ long lPort) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IldapcfgVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Ildapcfg __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Ildapcfg __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Ildapcfg __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AttachToRemoteMachine )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT szMachineName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AttachToLocalMachine )( 
            Ildapcfg __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadConfig )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveConfig )( 
            Ildapcfg __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateConfig )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteConfig )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Port )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Port )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SecurePort )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SecurePort )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IP )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IP )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SupportedIPs )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DNSName )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DNSName )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReadOnlyMode )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReadOnlyMode )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableAllSub )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableAllSub )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxPageSize )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxPageSize )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxResultSet )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxResultSet )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxQueryTime )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxQueryTime )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxConnection )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxConnection )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionTimeout )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectionTimeout )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableShortTermIPBlacklisting )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableShortTermIPBlacklisting )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableShortTermAccountBlacklisting )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableShortTermAccountBlacklisting )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Authorization )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Authorization )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableSSL )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableSSL )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableSSL128 )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableSSL128 )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableDynamic )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableDynamic )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MinTTL )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MinTTL )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxDynamObj )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxDynamObj )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableRTPerson )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableRTPerson )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableReplicate )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableReplicate )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableLog )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableLog )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DBType )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DBType )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DBSource )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DBSource )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DBName )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DBName )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DBUsername )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DBUsername )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DBPassword )( 
            Ildapcfg __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pData);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DBPassword )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetServerState )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT Instance,
            /* [out] */ VARIANT __RPC_FAR *lState);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartServer )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopServer )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PauseServer )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ContinueServer )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CompactDatabase )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RepairDatabase )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDynamicReplicationList )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [out] */ VARIANT __RPC_FAR *rgszReplNames,
            /* [out] */ VARIANT __RPC_FAR *rgszReplIDs);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirectoryProperties )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [out] */ VARIANT __RPC_FAR *szRealm,
            /* [out] */ VARIANT __RPC_FAR *szDnPrefix,
            /* [out] */ VARIANT __RPC_FAR *fIsSQL,
            /* [out] */ VARIANT __RPC_FAR *fExtSec,
            /* [out] */ VARIANT __RPC_FAR *rgszContainerPartitions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDirectoryProperties )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szRealm,
            /* [in] */ VARIANT szDnPrefix);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubPartitions )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainer,
            /* [out] */ VARIANT __RPC_FAR *rgdwSubPartitionNumber);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPartitionDbInfo )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [out] */ VARIANT __RPC_FAR *rgdwServerId,
            /* [out] */ VARIANT __RPC_FAR *rgszServerName,
            /* [out] */ VARIANT __RPC_FAR *rgszDatabaseName,
            /* [out] */ VARIANT __RPC_FAR *rgszLogin,
            /* [out] */ VARIANT __RPC_FAR *rgszPassword,
            /* [out] */ VARIANT __RPC_FAR *rgdwMaxCnx,
            /* [out] */ VARIANT __RPC_FAR *rgdwTimeout,
            /* [out] */ VARIANT __RPC_FAR *rgdwReplicationType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPartitionDbServer )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [in] */ VARIANT dwServerId,
            /* [out] */ VARIANT __RPC_FAR *szServerName,
            /* [out] */ VARIANT __RPC_FAR *szDatabaseName,
            /* [out] */ VARIANT __RPC_FAR *szLogin,
            /* [out] */ VARIANT __RPC_FAR *szPassword,
            /* [out] */ VARIANT __RPC_FAR *dwMaxCnx,
            /* [out] */ VARIANT __RPC_FAR *dwTimeout,
            /* [out] */ VARIANT __RPC_FAR *dwReplicationType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddPartitionDbServer )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [out] */ VARIANT __RPC_FAR *dwServerId,
            /* [in] */ VARIANT szServerName,
            /* [in] */ VARIANT szDatabaseName,
            /* [in] */ VARIANT szLogin,
            /* [in] */ VARIANT szPassword,
            /* [in] */ VARIANT dwMaxCnx,
            /* [in] */ VARIANT dwTimeout,
            /* [in] */ VARIANT dwReplicationType,
            /* [in] */ VARIANT lSubrefPartitionID,
            /* [in] */ VARIANT lSubrefObjectID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EditPartitionDbServer )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [in] */ VARIANT dwServerId,
            /* [in] */ VARIANT szServerName,
            /* [in] */ VARIANT szDatabaseName,
            /* [in] */ VARIANT szLogin,
            /* [in] */ VARIANT szPassword,
            /* [in] */ VARIANT dwMaxCnx,
            /* [in] */ VARIANT dwTimeout,
            /* [in] */ VARIANT dwReplicationType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemovePartitionDbServer )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szContainerPartition,
            /* [in] */ VARIANT lSubPartitionNumber,
            /* [in] */ VARIANT dwServerId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateDSPartition )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ VARIANT lInstanceId,
            /* [in] */ VARIANT szDn,
            /* [in] */ VARIANT lCount,
            /* [out] */ VARIANT __RPC_FAR *plPartitionID,
            /* [out] */ VARIANT __RPC_FAR *plObjectID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ListAllReplica )( 
            Ildapcfg __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *rgServerId,
            /* [out] */ VARIANT __RPC_FAR *rgszServerName,
            /* [out] */ VARIANT __RPC_FAR *rgszRpcGuid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddReplica )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ long lServerId,
            /* [in] */ BSTR szServerName,
            /* [in] */ BSTR szRpcGuid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DelReplica )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ long lServerId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConfigLocalReplica )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ long lNewServerId,
            /* [out] */ VARIANT __RPC_FAR *szRpcGuid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryLocalReplica )( 
            Ildapcfg __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *lServerId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsAddrInUse )( 
            Ildapcfg __RPC_FAR * This,
            /* [in] */ long lIP,
            /* [in] */ long lPort);
        
        END_INTERFACE
    } IldapcfgVtbl;

    interface Ildapcfg
    {
        CONST_VTBL struct IldapcfgVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Ildapcfg_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Ildapcfg_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Ildapcfg_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Ildapcfg_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Ildapcfg_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Ildapcfg_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Ildapcfg_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Ildapcfg_AttachToRemoteMachine(This,szMachineName)	\
    (This)->lpVtbl -> AttachToRemoteMachine(This,szMachineName)

#define Ildapcfg_AttachToLocalMachine(This)	\
    (This)->lpVtbl -> AttachToLocalMachine(This)

#define Ildapcfg_LoadConfig(This,Data)	\
    (This)->lpVtbl -> LoadConfig(This,Data)

#define Ildapcfg_SaveConfig(This)	\
    (This)->lpVtbl -> SaveConfig(This)

#define Ildapcfg_CreateConfig(This,Data)	\
    (This)->lpVtbl -> CreateConfig(This,Data)

#define Ildapcfg_DeleteConfig(This,lInstanceId)	\
    (This)->lpVtbl -> DeleteConfig(This,lInstanceId)

#define Ildapcfg_get_Port(This,pData)	\
    (This)->lpVtbl -> get_Port(This,pData)

#define Ildapcfg_put_Port(This,Data)	\
    (This)->lpVtbl -> put_Port(This,Data)

#define Ildapcfg_get_SecurePort(This,pData)	\
    (This)->lpVtbl -> get_SecurePort(This,pData)

#define Ildapcfg_put_SecurePort(This,Data)	\
    (This)->lpVtbl -> put_SecurePort(This,Data)

#define Ildapcfg_get_IP(This,pData)	\
    (This)->lpVtbl -> get_IP(This,pData)

#define Ildapcfg_put_IP(This,Data)	\
    (This)->lpVtbl -> put_IP(This,Data)

#define Ildapcfg_get_SupportedIPs(This,pData)	\
    (This)->lpVtbl -> get_SupportedIPs(This,pData)

#define Ildapcfg_get_DNSName(This,pData)	\
    (This)->lpVtbl -> get_DNSName(This,pData)

#define Ildapcfg_put_DNSName(This,Data)	\
    (This)->lpVtbl -> put_DNSName(This,Data)

#define Ildapcfg_get_ReadOnlyMode(This,pData)	\
    (This)->lpVtbl -> get_ReadOnlyMode(This,pData)

#define Ildapcfg_put_ReadOnlyMode(This,Data)	\
    (This)->lpVtbl -> put_ReadOnlyMode(This,Data)

#define Ildapcfg_get_EnableAllSub(This,pData)	\
    (This)->lpVtbl -> get_EnableAllSub(This,pData)

#define Ildapcfg_put_EnableAllSub(This,Data)	\
    (This)->lpVtbl -> put_EnableAllSub(This,Data)

#define Ildapcfg_get_MaxPageSize(This,pData)	\
    (This)->lpVtbl -> get_MaxPageSize(This,pData)

#define Ildapcfg_put_MaxPageSize(This,Data)	\
    (This)->lpVtbl -> put_MaxPageSize(This,Data)

#define Ildapcfg_get_MaxResultSet(This,pData)	\
    (This)->lpVtbl -> get_MaxResultSet(This,pData)

#define Ildapcfg_put_MaxResultSet(This,Data)	\
    (This)->lpVtbl -> put_MaxResultSet(This,Data)

#define Ildapcfg_get_MaxQueryTime(This,pData)	\
    (This)->lpVtbl -> get_MaxQueryTime(This,pData)

#define Ildapcfg_put_MaxQueryTime(This,Data)	\
    (This)->lpVtbl -> put_MaxQueryTime(This,Data)

#define Ildapcfg_get_MaxConnection(This,pData)	\
    (This)->lpVtbl -> get_MaxConnection(This,pData)

#define Ildapcfg_put_MaxConnection(This,Data)	\
    (This)->lpVtbl -> put_MaxConnection(This,Data)

#define Ildapcfg_get_ConnectionTimeout(This,pData)	\
    (This)->lpVtbl -> get_ConnectionTimeout(This,pData)

#define Ildapcfg_put_ConnectionTimeout(This,Data)	\
    (This)->lpVtbl -> put_ConnectionTimeout(This,Data)

#define Ildapcfg_get_EnableShortTermIPBlacklisting(This,pData)	\
    (This)->lpVtbl -> get_EnableShortTermIPBlacklisting(This,pData)

#define Ildapcfg_put_EnableShortTermIPBlacklisting(This,Data)	\
    (This)->lpVtbl -> put_EnableShortTermIPBlacklisting(This,Data)

#define Ildapcfg_get_EnableShortTermAccountBlacklisting(This,pData)	\
    (This)->lpVtbl -> get_EnableShortTermAccountBlacklisting(This,pData)

#define Ildapcfg_put_EnableShortTermAccountBlacklisting(This,Data)	\
    (This)->lpVtbl -> put_EnableShortTermAccountBlacklisting(This,Data)

#define Ildapcfg_get_Authorization(This,pData)	\
    (This)->lpVtbl -> get_Authorization(This,pData)

#define Ildapcfg_put_Authorization(This,Data)	\
    (This)->lpVtbl -> put_Authorization(This,Data)

#define Ildapcfg_get_EnableSSL(This,pData)	\
    (This)->lpVtbl -> get_EnableSSL(This,pData)

#define Ildapcfg_put_EnableSSL(This,Data)	\
    (This)->lpVtbl -> put_EnableSSL(This,Data)

#define Ildapcfg_get_EnableSSL128(This,pData)	\
    (This)->lpVtbl -> get_EnableSSL128(This,pData)

#define Ildapcfg_put_EnableSSL128(This,Data)	\
    (This)->lpVtbl -> put_EnableSSL128(This,Data)

#define Ildapcfg_get_EnableDynamic(This,pData)	\
    (This)->lpVtbl -> get_EnableDynamic(This,pData)

#define Ildapcfg_put_EnableDynamic(This,Data)	\
    (This)->lpVtbl -> put_EnableDynamic(This,Data)

#define Ildapcfg_get_MinTTL(This,pData)	\
    (This)->lpVtbl -> get_MinTTL(This,pData)

#define Ildapcfg_put_MinTTL(This,Data)	\
    (This)->lpVtbl -> put_MinTTL(This,Data)

#define Ildapcfg_get_MaxDynamObj(This,pData)	\
    (This)->lpVtbl -> get_MaxDynamObj(This,pData)

#define Ildapcfg_put_MaxDynamObj(This,Data)	\
    (This)->lpVtbl -> put_MaxDynamObj(This,Data)

#define Ildapcfg_get_EnableRTPerson(This,pData)	\
    (This)->lpVtbl -> get_EnableRTPerson(This,pData)

#define Ildapcfg_put_EnableRTPerson(This,Data)	\
    (This)->lpVtbl -> put_EnableRTPerson(This,Data)

#define Ildapcfg_get_EnableReplicate(This,pData)	\
    (This)->lpVtbl -> get_EnableReplicate(This,pData)

#define Ildapcfg_put_EnableReplicate(This,Data)	\
    (This)->lpVtbl -> put_EnableReplicate(This,Data)

#define Ildapcfg_get_EnableLog(This,pData)	\
    (This)->lpVtbl -> get_EnableLog(This,pData)

#define Ildapcfg_put_EnableLog(This,Data)	\
    (This)->lpVtbl -> put_EnableLog(This,Data)

#define Ildapcfg_get_DBType(This,pData)	\
    (This)->lpVtbl -> get_DBType(This,pData)

#define Ildapcfg_put_DBType(This,Data)	\
    (This)->lpVtbl -> put_DBType(This,Data)

#define Ildapcfg_get_DBSource(This,pData)	\
    (This)->lpVtbl -> get_DBSource(This,pData)

#define Ildapcfg_put_DBSource(This,Data)	\
    (This)->lpVtbl -> put_DBSource(This,Data)

#define Ildapcfg_get_DBName(This,pData)	\
    (This)->lpVtbl -> get_DBName(This,pData)

#define Ildapcfg_put_DBName(This,Data)	\
    (This)->lpVtbl -> put_DBName(This,Data)

#define Ildapcfg_get_DBUsername(This,pData)	\
    (This)->lpVtbl -> get_DBUsername(This,pData)

#define Ildapcfg_put_DBUsername(This,Data)	\
    (This)->lpVtbl -> put_DBUsername(This,Data)

#define Ildapcfg_get_DBPassword(This,pData)	\
    (This)->lpVtbl -> get_DBPassword(This,pData)

#define Ildapcfg_put_DBPassword(This,Data)	\
    (This)->lpVtbl -> put_DBPassword(This,Data)

#define Ildapcfg_GetServerState(This,Instance,lState)	\
    (This)->lpVtbl -> GetServerState(This,Instance,lState)

#define Ildapcfg_StartServer(This,lInstanceId)	\
    (This)->lpVtbl -> StartServer(This,lInstanceId)

#define Ildapcfg_StopServer(This,lInstanceId)	\
    (This)->lpVtbl -> StopServer(This,lInstanceId)

#define Ildapcfg_PauseServer(This,lInstanceId)	\
    (This)->lpVtbl -> PauseServer(This,lInstanceId)

#define Ildapcfg_ContinueServer(This,lInstanceId)	\
    (This)->lpVtbl -> ContinueServer(This,lInstanceId)

#define Ildapcfg_CompactDatabase(This,lInstanceId)	\
    (This)->lpVtbl -> CompactDatabase(This,lInstanceId)

#define Ildapcfg_RepairDatabase(This,lInstanceId)	\
    (This)->lpVtbl -> RepairDatabase(This,lInstanceId)

#define Ildapcfg_GetDynamicReplicationList(This,lInstanceId,rgszReplNames,rgszReplIDs)	\
    (This)->lpVtbl -> GetDynamicReplicationList(This,lInstanceId,rgszReplNames,rgszReplIDs)

#define Ildapcfg_GetDirectoryProperties(This,lInstanceId,szRealm,szDnPrefix,fIsSQL,fExtSec,rgszContainerPartitions)	\
    (This)->lpVtbl -> GetDirectoryProperties(This,lInstanceId,szRealm,szDnPrefix,fIsSQL,fExtSec,rgszContainerPartitions)

#define Ildapcfg_SetDirectoryProperties(This,lInstanceId,szRealm,szDnPrefix)	\
    (This)->lpVtbl -> SetDirectoryProperties(This,lInstanceId,szRealm,szDnPrefix)

#define Ildapcfg_GetSubPartitions(This,lInstanceId,szContainer,rgdwSubPartitionNumber)	\
    (This)->lpVtbl -> GetSubPartitions(This,lInstanceId,szContainer,rgdwSubPartitionNumber)

#define Ildapcfg_GetPartitionDbInfo(This,lInstanceId,szContainerPartition,lSubPartitionNumber,rgdwServerId,rgszServerName,rgszDatabaseName,rgszLogin,rgszPassword,rgdwMaxCnx,rgdwTimeout,rgdwReplicationType)	\
    (This)->lpVtbl -> GetPartitionDbInfo(This,lInstanceId,szContainerPartition,lSubPartitionNumber,rgdwServerId,rgszServerName,rgszDatabaseName,rgszLogin,rgszPassword,rgdwMaxCnx,rgdwTimeout,rgdwReplicationType)

#define Ildapcfg_GetPartitionDbServer(This,lInstanceId,szContainerPartition,lSubPartitionNumber,dwServerId,szServerName,szDatabaseName,szLogin,szPassword,dwMaxCnx,dwTimeout,dwReplicationType)	\
    (This)->lpVtbl -> GetPartitionDbServer(This,lInstanceId,szContainerPartition,lSubPartitionNumber,dwServerId,szServerName,szDatabaseName,szLogin,szPassword,dwMaxCnx,dwTimeout,dwReplicationType)

#define Ildapcfg_AddPartitionDbServer(This,lInstanceId,szContainerPartition,lSubPartitionNumber,dwServerId,szServerName,szDatabaseName,szLogin,szPassword,dwMaxCnx,dwTimeout,dwReplicationType,lSubrefPartitionID,lSubrefObjectID)	\
    (This)->lpVtbl -> AddPartitionDbServer(This,lInstanceId,szContainerPartition,lSubPartitionNumber,dwServerId,szServerName,szDatabaseName,szLogin,szPassword,dwMaxCnx,dwTimeout,dwReplicationType,lSubrefPartitionID,lSubrefObjectID)

#define Ildapcfg_EditPartitionDbServer(This,lInstanceId,szContainerPartition,lSubPartitionNumber,dwServerId,szServerName,szDatabaseName,szLogin,szPassword,dwMaxCnx,dwTimeout,dwReplicationType)	\
    (This)->lpVtbl -> EditPartitionDbServer(This,lInstanceId,szContainerPartition,lSubPartitionNumber,dwServerId,szServerName,szDatabaseName,szLogin,szPassword,dwMaxCnx,dwTimeout,dwReplicationType)

#define Ildapcfg_RemovePartitionDbServer(This,lInstanceId,szContainerPartition,lSubPartitionNumber,dwServerId)	\
    (This)->lpVtbl -> RemovePartitionDbServer(This,lInstanceId,szContainerPartition,lSubPartitionNumber,dwServerId)

#define Ildapcfg_CreateDSPartition(This,lInstanceId,szDn,lCount,plPartitionID,plObjectID)	\
    (This)->lpVtbl -> CreateDSPartition(This,lInstanceId,szDn,lCount,plPartitionID,plObjectID)

#define Ildapcfg_ListAllReplica(This,rgServerId,rgszServerName,rgszRpcGuid)	\
    (This)->lpVtbl -> ListAllReplica(This,rgServerId,rgszServerName,rgszRpcGuid)

#define Ildapcfg_AddReplica(This,lServerId,szServerName,szRpcGuid)	\
    (This)->lpVtbl -> AddReplica(This,lServerId,szServerName,szRpcGuid)

#define Ildapcfg_DelReplica(This,lServerId)	\
    (This)->lpVtbl -> DelReplica(This,lServerId)

#define Ildapcfg_ConfigLocalReplica(This,lNewServerId,szRpcGuid)	\
    (This)->lpVtbl -> ConfigLocalReplica(This,lNewServerId,szRpcGuid)

#define Ildapcfg_QueryLocalReplica(This,lServerId)	\
    (This)->lpVtbl -> QueryLocalReplica(This,lServerId)

#define Ildapcfg_IsAddrInUse(This,lIP,lPort)	\
    (This)->lpVtbl -> IsAddrInUse(This,lIP,lPort)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_AttachToRemoteMachine_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT szMachineName);


void __RPC_STUB Ildapcfg_AttachToRemoteMachine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_AttachToLocalMachine_Proxy( 
    Ildapcfg __RPC_FAR * This);


void __RPC_STUB Ildapcfg_AttachToLocalMachine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_LoadConfig_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_LoadConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_SaveConfig_Proxy( 
    Ildapcfg __RPC_FAR * This);


void __RPC_STUB Ildapcfg_SaveConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_CreateConfig_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_CreateConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_DeleteConfig_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId);


void __RPC_STUB Ildapcfg_DeleteConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_Port_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_Port_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_Port_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_Port_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_SecurePort_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_SecurePort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_SecurePort_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_SecurePort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_IP_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_IP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_IP_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_IP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_SupportedIPs_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_SupportedIPs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_DNSName_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_DNSName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_DNSName_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_DNSName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_ReadOnlyMode_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_ReadOnlyMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_ReadOnlyMode_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_ReadOnlyMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableAllSub_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableAllSub_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableAllSub_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableAllSub_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_MaxPageSize_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_MaxPageSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_MaxPageSize_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_MaxPageSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_MaxResultSet_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_MaxResultSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_MaxResultSet_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_MaxResultSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_MaxQueryTime_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_MaxQueryTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_MaxQueryTime_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_MaxQueryTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_MaxConnection_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_MaxConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_MaxConnection_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_MaxConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_ConnectionTimeout_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_ConnectionTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_ConnectionTimeout_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_ConnectionTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableShortTermIPBlacklisting_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableShortTermIPBlacklisting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableShortTermIPBlacklisting_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableShortTermIPBlacklisting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableShortTermAccountBlacklisting_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableShortTermAccountBlacklisting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableShortTermAccountBlacklisting_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableShortTermAccountBlacklisting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_Authorization_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_Authorization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_Authorization_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_Authorization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableSSL_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableSSL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableSSL_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableSSL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableSSL128_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableSSL128_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableSSL128_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableSSL128_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableDynamic_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableDynamic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableDynamic_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableDynamic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_MinTTL_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_MinTTL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_MinTTL_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_MinTTL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_MaxDynamObj_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_MaxDynamObj_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_MaxDynamObj_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_MaxDynamObj_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableRTPerson_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableRTPerson_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableRTPerson_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableRTPerson_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableReplicate_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableReplicate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableReplicate_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableReplicate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_EnableLog_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_EnableLog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_EnableLog_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_EnableLog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_DBType_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_DBType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_DBType_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_DBType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_DBSource_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_DBSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_DBSource_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_DBSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_DBName_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_DBName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_DBName_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_DBName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_DBUsername_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_DBUsername_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_DBUsername_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_DBUsername_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Ildapcfg_get_DBPassword_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pData);


void __RPC_STUB Ildapcfg_get_DBPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE Ildapcfg_put_DBPassword_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Data);


void __RPC_STUB Ildapcfg_put_DBPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_GetServerState_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT Instance,
    /* [out] */ VARIANT __RPC_FAR *lState);


void __RPC_STUB Ildapcfg_GetServerState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_StartServer_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId);


void __RPC_STUB Ildapcfg_StartServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_StopServer_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId);


void __RPC_STUB Ildapcfg_StopServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_PauseServer_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId);


void __RPC_STUB Ildapcfg_PauseServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_ContinueServer_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId);


void __RPC_STUB Ildapcfg_ContinueServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_CompactDatabase_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId);


void __RPC_STUB Ildapcfg_CompactDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_RepairDatabase_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId);


void __RPC_STUB Ildapcfg_RepairDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_GetDynamicReplicationList_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [out] */ VARIANT __RPC_FAR *rgszReplNames,
    /* [out] */ VARIANT __RPC_FAR *rgszReplIDs);


void __RPC_STUB Ildapcfg_GetDynamicReplicationList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_GetDirectoryProperties_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [out] */ VARIANT __RPC_FAR *szRealm,
    /* [out] */ VARIANT __RPC_FAR *szDnPrefix,
    /* [out] */ VARIANT __RPC_FAR *fIsSQL,
    /* [out] */ VARIANT __RPC_FAR *fExtSec,
    /* [out] */ VARIANT __RPC_FAR *rgszContainerPartitions);


void __RPC_STUB Ildapcfg_GetDirectoryProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_SetDirectoryProperties_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [in] */ VARIANT szRealm,
    /* [in] */ VARIANT szDnPrefix);


void __RPC_STUB Ildapcfg_SetDirectoryProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_GetSubPartitions_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [in] */ VARIANT szContainer,
    /* [out] */ VARIANT __RPC_FAR *rgdwSubPartitionNumber);


void __RPC_STUB Ildapcfg_GetSubPartitions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_GetPartitionDbInfo_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [in] */ VARIANT szContainerPartition,
    /* [in] */ VARIANT lSubPartitionNumber,
    /* [out] */ VARIANT __RPC_FAR *rgdwServerId,
    /* [out] */ VARIANT __RPC_FAR *rgszServerName,
    /* [out] */ VARIANT __RPC_FAR *rgszDatabaseName,
    /* [out] */ VARIANT __RPC_FAR *rgszLogin,
    /* [out] */ VARIANT __RPC_FAR *rgszPassword,
    /* [out] */ VARIANT __RPC_FAR *rgdwMaxCnx,
    /* [out] */ VARIANT __RPC_FAR *rgdwTimeout,
    /* [out] */ VARIANT __RPC_FAR *rgdwReplicationType);


void __RPC_STUB Ildapcfg_GetPartitionDbInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_GetPartitionDbServer_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [in] */ VARIANT szContainerPartition,
    /* [in] */ VARIANT lSubPartitionNumber,
    /* [in] */ VARIANT dwServerId,
    /* [out] */ VARIANT __RPC_FAR *szServerName,
    /* [out] */ VARIANT __RPC_FAR *szDatabaseName,
    /* [out] */ VARIANT __RPC_FAR *szLogin,
    /* [out] */ VARIANT __RPC_FAR *szPassword,
    /* [out] */ VARIANT __RPC_FAR *dwMaxCnx,
    /* [out] */ VARIANT __RPC_FAR *dwTimeout,
    /* [out] */ VARIANT __RPC_FAR *dwReplicationType);


void __RPC_STUB Ildapcfg_GetPartitionDbServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_AddPartitionDbServer_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [in] */ VARIANT szContainerPartition,
    /* [in] */ VARIANT lSubPartitionNumber,
    /* [out] */ VARIANT __RPC_FAR *dwServerId,
    /* [in] */ VARIANT szServerName,
    /* [in] */ VARIANT szDatabaseName,
    /* [in] */ VARIANT szLogin,
    /* [in] */ VARIANT szPassword,
    /* [in] */ VARIANT dwMaxCnx,
    /* [in] */ VARIANT dwTimeout,
    /* [in] */ VARIANT dwReplicationType,
    /* [in] */ VARIANT lSubrefPartitionID,
    /* [in] */ VARIANT lSubrefObjectID);


void __RPC_STUB Ildapcfg_AddPartitionDbServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_EditPartitionDbServer_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [in] */ VARIANT szContainerPartition,
    /* [in] */ VARIANT lSubPartitionNumber,
    /* [in] */ VARIANT dwServerId,
    /* [in] */ VARIANT szServerName,
    /* [in] */ VARIANT szDatabaseName,
    /* [in] */ VARIANT szLogin,
    /* [in] */ VARIANT szPassword,
    /* [in] */ VARIANT dwMaxCnx,
    /* [in] */ VARIANT dwTimeout,
    /* [in] */ VARIANT dwReplicationType);


void __RPC_STUB Ildapcfg_EditPartitionDbServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_RemovePartitionDbServer_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [in] */ VARIANT szContainerPartition,
    /* [in] */ VARIANT lSubPartitionNumber,
    /* [in] */ VARIANT dwServerId);


void __RPC_STUB Ildapcfg_RemovePartitionDbServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_CreateDSPartition_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ VARIANT lInstanceId,
    /* [in] */ VARIANT szDn,
    /* [in] */ VARIANT lCount,
    /* [out] */ VARIANT __RPC_FAR *plPartitionID,
    /* [out] */ VARIANT __RPC_FAR *plObjectID);


void __RPC_STUB Ildapcfg_CreateDSPartition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_ListAllReplica_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *rgServerId,
    /* [out] */ VARIANT __RPC_FAR *rgszServerName,
    /* [out] */ VARIANT __RPC_FAR *rgszRpcGuid);


void __RPC_STUB Ildapcfg_ListAllReplica_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_AddReplica_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ long lServerId,
    /* [in] */ BSTR szServerName,
    /* [in] */ BSTR szRpcGuid);


void __RPC_STUB Ildapcfg_AddReplica_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_DelReplica_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ long lServerId);


void __RPC_STUB Ildapcfg_DelReplica_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_ConfigLocalReplica_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ long lNewServerId,
    /* [out] */ VARIANT __RPC_FAR *szRpcGuid);


void __RPC_STUB Ildapcfg_ConfigLocalReplica_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_QueryLocalReplica_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *lServerId);


void __RPC_STUB Ildapcfg_QueryLocalReplica_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Ildapcfg_IsAddrInUse_Proxy( 
    Ildapcfg __RPC_FAR * This,
    /* [in] */ long lIP,
    /* [in] */ long lPort);


void __RPC_STUB Ildapcfg_IsAddrInUse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Ildapcfg_INTERFACE_DEFINED__ */



#ifndef __LADMIN2Lib_LIBRARY_DEFINED__
#define __LADMIN2Lib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: LADMIN2Lib
 * at Wed Apr 22 20:01:24 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_LADMIN2Lib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_ldapcfg;

class DECLSPEC_UUID("40E9AD64-E2A1-11D0-920F-00C04FB954C7")
ldapcfg;
#endif
#endif /* __LADMIN2Lib_LIBRARY_DEFINED__ */

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
