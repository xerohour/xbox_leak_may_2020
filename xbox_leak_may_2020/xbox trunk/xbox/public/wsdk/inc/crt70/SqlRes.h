////////////////////////////////////////////////////////////////////////////////////////
// Header file for Replication Custom Resolver C/C++ Client Component Object interfaces.
// Copyright 1994-1999 Microsoft Corporation.
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SQLRES_H__
#define __SQLRES_H__

typedef void FAR * HENV;

typedef enum {
    REPOLEDataSource_Native 	= 0x0000,	// Native SQL Server
    REPOLEDataSource_ODBC 		= 0x0001,	// ODBC DataSource
    REPOLEDataSource_Jet 		= 0x0002,	// Jet DataSource
	REPOLEDataSource_OLEDB		= 0x0003
} REPOLE_DATASOURCE_TYPE;

typedef enum {
	REPOLESecurity_Min 			= 0,				
	REPOLESecurity_Normal 		= 0,
	REPOLESecurity_Integrated 	= 1,
	REPOLESecurity_Mixed 		= 2,
	REPOLESecurity_Max 			= 2,
	REPOLESecurity_Invalid		= -1
} REPOLE_SECURITY_TYPE;

// NOTE:  All publisher action bitmasks are same as correspording
// subscriber bitmask shifted left 16 bytes...  Please don't break this
// convention for existing #defines.

typedef enum {
	REPOLEChange_SubscriberInsert						= 0x00000001,
	REPOLEChange_PublisherInsert						= 0x00010000,

 	REPOLEChange_SubscriberDelete_NoConflict			= 0x00000002,
 	REPOLEChange_PublisherDelete_NoConflict				= 0x00020000,
 	REPOLEChange_SubscriberSystemDelete					= 0x00000004,
 	REPOLEChange_PublisherSystemDelete					= 0x00040000,
 	REPOLEChange_SubscriberDelete_Conflict				= 0x00000008,
 	REPOLEChange_PublisherDelete_Conflict				= 0x00080000,
 	REPOLEChange_SubscriberRemoveFromPartial			= 0x00000010,
 	REPOLEChange_PublisherRemoveFromPartial				= 0x00100000,

 	REPOLEChange_SubscriberUpdate_NoConflict			= 0x00000020,
 	REPOLEChange_PublisherUpdate_NoConflict				= 0x00200000,
 	REPOLEChange_SubscriberUpdate_ConflictWithDelete	= 0x00000040,
 	REPOLEChange_PublisherUpdate_ConflictWithDelete		= 0x00400000,
 	REPOLEChange_SubscriberUpdate_ConflictColTrack		= 0x00000080,
 	REPOLEChange_PublisherUpdate_ConflictColTrack		= 0x00800000,
 	REPOLEChange_SubscriberUpdate_ConflictNoColTrack	= 0x00000100,
 	REPOLEChange_PublisherUpdate_ConflictNoColTrack		= 0x01000000,


 	REPOLEChange_UploadInsertFailed						= 0x00000200,
 	REPOLEChange_DownloadInsertFailed					= 0x02000000,
 	REPOLEChange_UploadDeleteFailed						= 0x00000400,
 	REPOLEChange_DownloadDeleteFailed					= 0x04000000,
 	REPOLEChange_UploadUpdateFailed						= 0x00000800,
 	REPOLEChange_DownloadUpdateFailed					= 0x08000000
	
} REPOLE_CHANGE_TYPE;	

#define REPOLEUpdateConflicts (REPOLEChange_SubscriberUpdate_ConflictColTrack | REPOLEChange_PublisherUpdate_ConflictColTrack \
	| REPOLEChange_SubscriberUpdate_ConflictNoColTrack | REPOLEChange_PublisherUpdate_ConflictNoColTrack)

#define REPOLEAllConflicts (REPOLEChange_SubscriberDelete_Conflict | REPOLEChange_PublisherDelete_Conflict \
	| REPOLEChange_SubscriberUpdate_ConflictWithDelete | REPOLEChange_PublisherUpdate_ConflictWithDelete \
	| REPOLEUpdateConflicts)
	
#define REPOLEAllErrors (REPOLEChange_UploadInsertFailed | REPOLEChange_DownloadInsertFailed \
	| REPOLEChange_UploadDeleteFailed | REPOLEChange_DownloadDeleteFailed \
	| REPOLEChange_UploadUpdateFailed | REPOLEChange_DownloadUpdateFailed)
#define REPOLEAllNonConflicts (REPOLEChange_SubscriberInsert | REPOLEChange_PublisherInsert \
	| REPOLEChange_SubscriberDelete_NoConflict | REPOLEChange_PublisherDelete_NoConflict \
	| REPOLEChange_SubscriberSystemDelete | REPOLEChange_PublisherSystemDelete \
	| REPOLEChange_SubscriberRemoveFromPartial | REPOLEChange_SubscriberUpdate_NoConflict \
	| REPOLEChange_PublisherUpdate_NoConflict)
#define REPOLEAllChanges (REPOLEAllConflicts | REPOLEAllErrors | REPOLEAllNonConflicts)

typedef enum {
	REPOLEConflict_Min						= 1,
	REPOLEConflict_UpdateConflict			= 1,
	REPOLEConflict_ColumnUpdateConflict		= 2,
	REPOLEConflict_UpdateDeleteWinsConflict	= 3,
	REPOLEConflict_UpdateWinsDeleteConflict	= 4,
 	REPOLEConflict_UploadInsertFailed		= 5,
 	REPOLEConflict_DownloadInsertFailed		= 6,
 	REPOLEConflict_UploadDeleteFailed		= 7,
 	REPOLEConflict_DownloadDeleteFailed		= 8,
 	REPOLEConflict_UploadUpdateFailed		= 9,
 	REPOLEConflict_DownloadUpdateFailed		= 10,
	REPOLEConflict_Max						= 10
} REPOLE_CONFLICT_TYPE;

typedef enum {
	REPOLEColumn_NotUpdated			 		= 0x0001,	// No updates to column or data values match
	REPOLEColumn_UpdatedNoConflict			= 0x0002,	// Column updated at src, no column level conflict
	REPOLEColumn_UpdatedWithConflict		= 0x0003,	// Column updated at src, conflicts with change at destination
	REPOLEColumn_DifferNoTrack				= 0x0004	// No column tracking info, data values are different
} REPOLE_COLSTATUS_TYPE;	

typedef enum {
	REPOLEPriority_Source		= 0x0001,	// The source has the higher priority
	REPOLEPriority_Destination 	= 0x0002,	// The destination has the higher priority
	REPOLEPriority_Equal		= 0x0003	// Source and destination have equal priority
} REPOLE_PRIORITY_TYPE;	


/**** Implemented by the Reconciler ***/
#undef  INTERFACE
#define INTERFACE IConnectionInfo
DECLARE_INTERFACE_(IConnectionInfo, IUnknown)
{
   	//*** IUnknown methods
   	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID *ppvObj) PURE;
   	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
   	STDMETHOD_(ULONG,Release) (THIS) PURE;

   	/* IConnectionInfo Get methods */
    STDMETHOD(GetConnectName)(THIS_ LPTSTR ConnectName, DWORD cbConnectName) PURE;
    STDMETHOD(GetServerName)(THIS_ LPTSTR ServerName, DWORD cbServerName) PURE;
    STDMETHOD(GetHostName)(THIS_ LPTSTR HostName, DWORD cbHostName) PURE;
	STDMETHOD(GetServerRole)(THIS_ LPTSTR ServerRole, DWORD cbServerRole) PURE;
    STDMETHOD(GetInternetAddress)(THIS_ LPTSTR InternetAddress, DWORD cbInternetAddress) PURE;
    STDMETHOD(GetInternetNetwork)(THIS_ LPTSTR InternetNetwork, DWORD cbInternetNetwork) PURE;
    STDMETHOD(GetDatabase)(THIS_ LPTSTR Database, DWORD cbDatabase) PURE;
	STDMETHOD(GetDatabaseStatus)(THIS_ LONG *plDatabaseStatus) PURE;
    STDMETHOD(GetDatasourcePath)(THIS_ LPTSTR DatasourcePath, DWORD cbDatasourcePath) PURE;
    STDMETHOD(GetDataSourceType)(THIS_ REPOLE_DATASOURCE_TYPE FAR * pRetValue) PURE;
    STDMETHOD(GetSecurityMode)(THIS_ REPOLE_SECURITY_TYPE FAR * pRetValue) PURE;
    STDMETHOD(GetLogin)(THIS_ LPTSTR Login, DWORD cbLogin) PURE;
    STDMETHOD(GetPassword)(THIS_ LPTSTR Password, DWORD cbPassword) PURE;
    STDMETHOD(GetODBCEnvironmentHandle)(THIS_ HENV * phenv) PURE;
    STDMETHOD(GetLoginTimeout)(THIS_ LONG * plLoginTimeout) PURE;
    STDMETHOD(GetQueryTimeout)(THIS_ LONG * plQueryTimeout) PURE;
    STDMETHOD(GetPacketSize)(THIS_ unsigned long * pusPacketSize) PURE;
    STDMETHOD(GetApplicationName)(THIS_ LPTSTR ApplicationName, DWORD cbApplicationName) PURE;
	STDMETHOD(GetProviderName)(THIS_ LPTSTR ProviderName, DWORD cbProviderName) PURE;
	STDMETHOD(GetDatasource)(THIS_ LPTSTR Datasource, DWORD cbDatasource) PURE;
	STDMETHOD(GetLocation)(THIS_ LPTSTR Location, DWORD cbLocation) PURE;
	STDMETHOD(GetProviderString)(THIS_ LPTSTR ProviderString, DWORD cbProviderString) PURE;
	STDMETHOD(GetCatalog)(THIS_ LPTSTR Catalog, DWORD cbCatalog) PURE;
	STDMETHOD(GetDBCreatedThisSession)(THIS_ BOOL *pbDBCreatedThisSession) PURE;

   	/* IConnectionInfo Set methods (not accessible to third part resolver) */
    STDMETHOD(SetConnectName)(THIS_ LPCTSTR ConnectName) PURE;
    STDMETHOD(SetServerName)(THIS_ LPCTSTR ServerName) PURE;
    STDMETHOD(SetHostName)(THIS_ LPCTSTR HostName) PURE;
	STDMETHOD(SetServerRole)(THIS_ LPCTSTR ServerRole) PURE;
    STDMETHOD(SetInternetAddress)(THIS_ LPCTSTR InternetAddress) PURE;
    STDMETHOD(SetInternetNetwork)(THIS_ LPCTSTR InternetNetwork) PURE;
    STDMETHOD(SetDatabase)(THIS_ LPCTSTR Database) PURE;
	STDMETHOD(SetDatabaseStatus)(THIS_ LONG lDatabaseStatus) PURE;
    STDMETHOD(SetDatasourcePath)(THIS_ LPCTSTR DatasourcePath) PURE;
    STDMETHOD(SetDataSourceType)(THIS_ REPOLE_DATASOURCE_TYPE DataSourceType) PURE;
    STDMETHOD(SetSecurityMode)(THIS_ REPOLE_SECURITY_TYPE SecurityMode) PURE;
    STDMETHOD(SetLogin)(THIS_ LPCTSTR Login) PURE;
    STDMETHOD(SetPassword)(THIS_ LPCTSTR Password) PURE;
    STDMETHOD(SetODBCEnvironmentHandle)(THIS_ HENV henv) PURE;
    STDMETHOD(SetLoginTimeout)(THIS_ LONG lLoginTimeout) PURE;
    STDMETHOD(SetQueryTimeout)(THIS_ LONG lQueryTimeout) PURE;
    STDMETHOD(SetPacketSize)(THIS_ unsigned long usPacketSize) PURE;
    STDMETHOD(SetApplicationName)(THIS_ LPCTSTR ApplicationName) PURE;
	STDMETHOD(SetProviderName)(THIS_ LPCTSTR ProviderName) PURE;
	STDMETHOD(SetDatasource)(THIS_ LPCTSTR Datasource) PURE;
	STDMETHOD(SetLocation)(THIS_ LPCTSTR Location) PURE;
	STDMETHOD(SetProviderString)(THIS_ LPCTSTR ProviderString) PURE;
	STDMETHOD(SetCatalog)(THIS_ LPCTSTR Catalog) PURE;
	STDMETHOD(SetDBCreatedThisSession)(THIS_ BOOL bDBCreatedThisSession) PURE;
    STDMETHOD(SetBcpBatchSize)(THIS_ LONG lBcpBatchSize) PURE;
    STDMETHOD(GetBcpBatchSize)(THIS_ LONG * plBcpBatchSize) PURE;
};


/**** Implemented by the Reconciler ***/
#undef  INTERFACE
#define INTERFACE IReplRowChange
DECLARE_INTERFACE_(IReplRowChange, IUnknown)
{
   	//*** IUnknown methods
   	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID *ppvObj) PURE;
   	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
   	STDMETHOD_(ULONG,Release) (THIS) PURE;

   	/* IReplRowChange methods */
    STDMETHOD(GetTableName)(THIS_ LPTSTR pTableName, DWORD cbTableName) PURE;
    STDMETHOD(GetRowIdentifier)(THIS_ LPGUID pRowGuid) PURE;
	STDMETHOD(GetResolverProcedureName)(LPTSTR pResolverProcedureName, DWORD cbResolverProcedureName) PURE;
    STDMETHOD(GetNumColumns)(THIS_ LPDWORD pdwColumnCount) PURE;
    STDMETHOD(GetChangeType)(THIS_ REPOLE_CHANGE_TYPE * pChangeType) PURE;
    STDMETHOD(GetColumnStatus)(THIS_ DWORD ColumnId, REPOLE_COLSTATUS_TYPE FAR *pColStatus) PURE;
    STDMETHOD(GetColumnName)(THIS_ DWORD ColumnId, LPTSTR pColumnName, DWORD cbColumnName) PURE;
	STDMETHOD(GetColumnDatatype)(DWORD ColumnId, LPLONG plDataType) PURE;
	STDMETHOD(GetSourceColumnValue)(THIS_ DWORD ColumnId, LPVOID pvBuffer, DWORD cbBufferMax, LPDWORD pcbBufferActual) PURE;
	STDMETHOD(GetDestinationColumnValue)(DWORD ColumnId, LPVOID pvBuffer, DWORD cbBufferMax, LPDWORD pcbBufferActual) PURE;
    STDMETHOD(GetPriorityWinner)(THIS_ REPOLE_PRIORITY_TYPE FAR * pPriorityWinner) PURE;
    STDMETHOD(GetSourceConnectionInfo)(THIS_ IConnectionInfo ** ppSourceConnectionInfo, LPBOOL pfIsPublisher) PURE;
    STDMETHOD(GetDestinationConnectionInfo)(THIS_ IConnectionInfo ** ppDestinationConnectionInfo, LPBOOL pfIsPublisher) PURE;
    STDMETHOD(DeleteRow)(THIS) PURE;
    STDMETHOD(CopyRowFromSource)(THIS) PURE;
    STDMETHOD(CopyColumnFromSource)(THIS_ DWORD ColumnId) PURE;
    STDMETHOD(SetColumn)(THIS_ DWORD ColumnId, LPVOID pvBuffer, DWORD cbBuffer) PURE;
    STDMETHOD(UpdateRow)(THIS) PURE;
    STDMETHOD(InsertRow)(THIS) PURE;
    STDMETHOD(LogConflict)(THIS_ BOOL bLogSourceConflict, REPOLE_CONFLICT_TYPE ConflictType, BOOL bOnlyLogIfUpdater, LPTSTR pszConflictMessage = NULL) PURE;
    STDMETHOD(LogError)(THIS_ REPOLE_CHANGE_TYPE ChangeType, LPTSTR pszErrorMessage = NULL) PURE;
    STDMETHOD(GetErrorInfo)(THIS_ DWORD *pErrCode, LPTSTR pErrText) PURE;
    STDMETHOD(DoDummyUpdate)(THIS_ BOOL fUpLineage, BOOL fAtPublisher) PURE;
    STDMETHOD(GetTableOwnerName)(THIS_ LPTSTR pOwnerName, DWORD cbOwnerName) PURE;
    STDMETHOD(GetRowGuidColName)(THIS_ LPTSTR pRowGuidColName, DWORD cbRowGuidColName) PURE;
};



/**** Default implemention by the Reconciler, can be provided by the ISV ***/
#undef  INTERFACE
#define INTERFACE ICustomResolver
DECLARE_INTERFACE_(ICustomResolver, IUnknown)
{
   	//*** IUnknown methods
   	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID *ppvObj) PURE;
   	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
   	STDMETHOD_(ULONG,Release) (THIS) PURE;

   	/* ICustomResolver methods */
    STDMETHOD(Reconcile)(THIS_
                        IReplRowChange *pRowChange,
                        DWORD dwFlags,
                        PVOID pvReserved) PURE;
    STDMETHOD(GetHandledStates) (THIS_
    					DWORD	*pResolverBm) PURE;
};

#endif   /* ! __SQLRES_H__ */

