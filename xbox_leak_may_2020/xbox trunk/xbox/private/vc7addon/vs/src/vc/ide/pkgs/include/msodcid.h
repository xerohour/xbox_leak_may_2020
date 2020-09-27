/*-----------------------------------------------------------------------------
Microsoft Sterling

Microsoft Confidential
Copyright 1994-1996 Microsoft Corporation.  All Rights Reserved.

File:		msodcid.h
Contents:	This file describes the data context interfaces
-----------------------------------------------------------------------------*/

// {A43D9461-EA36-11ce-AE99-00AA00A28331}
DEFINE_GUID(SID_NAMESPACE,
0xa43d9461, 0xea36, 0x11ce, 0xae, 0x99, 0x0, 0xaa, 0x0, 0xa2, 0x83, 0x31);

// {A43D9462-EA36-11ce-AE99-00AA00A28331}
DEFINE_GUID(IID_INamespaceSource,
0xa43d9462, 0xea36, 0x11ce, 0xae, 0x99, 0x0, 0xaa, 0x0, 0xa2, 0x83, 0x31);

// {A43D9463-EA36-11ce-AE99-00AA00A28331}
DEFINE_GUID(IID_IPersistDataStore,
0xa43d9463, 0xea36, 0x11ce, 0xae, 0x99, 0x0, 0xaa, 0x0, 0xa2, 0x83, 0x31);




// **** IID_IDBConnectObject ****
//
// {B7A1D4A0-7210-11cf-BE16-00AA0062C2EF}
DEFINE_GUID(IID_IDBConnectObject, 
0xb7a1d4a0, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

// **** IID_IODBCConnectObject ****
//
// {B7A1D4A1-7210-11cf-BE16-00AA0062C2EF}
DEFINE_GUID(IID_IODBCConnectObject, 
0xb7a1d4a1, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

// **** IID_IJetConnectObject ****
//
// {B7A1D4A2-7210-11cf-BE16-00AA0062C2EF}
DEFINE_GUID(IID_IJetConnectObject, 
0xb7a1d4a2, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);



// **** IID_IDBConnectionManager ****
//
// {B7A1D4A3-7210-11cf-BE16-00AA0062C2EF}
DEFINE_GUID(IID_IDBConnectionManager, 
0xb7a1d4a3, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

#define SID_IDBConnectionManager  IID_IDBConnectionManager

// **** IID_IAsyncQueryBuilder ****
//
// {B7A1D4A4-7210-11cf-BE16-00AA0062C2EF}
DEFINE_GUID(IID_IAsyncQueryBuilder, 
0xb7a1d4a4, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);


// **** IID_IDBDisp ****
// {B7A1D4A5-7210-11cf-BE16-00AA0062C2EF}
DEFINE_GUID(IID_IDBTextDisp, 
0xb7a1d4a5, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

#define SID_IDBTextDisp IID_IDBTextDisp

// **** IID_IDataSrcClient ****
//
// {B7A1D4A6-7210-11cf-BE16-00AA0062C2EF}
DEFINE_GUID(IID_IDataSrcClient, 
0xb7a1d4a6, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

#define SID_SDataSrcClient IID_IDataSrcClient

// {BE377880-0B1D-11d0-8923-00AA00BF0506}
DEFINE_GUID(IID_IDataSrcDebug, 
0xbe377880, 0xb1d, 0x11d0, 0x89, 0x23, 0x0, 0xaa, 0x0, 0xbf, 0x5, 0x6);

#define SID_SDataSrcDebug IID_IDataSrcDebug

// **** IID_IDataSrcDebugConsumer ****
//
// {b0c47fd0-1220-11d0-9d16-00c04fd9dfd9}

DEFINE_GUID(IID_IDataSrcDebugConsumer,
    0xb0c47fd0, 0x1220, 0x11d0, 0x9d, 0x16, 0x00, 0xc0, 0x4f, 0xd9, 0xdf, 0xd9) ;

#define SID_SDataSrcDebugConsumer IID_IDataSrcDebugConsumer

// {8B072243-8C1A-11cf-BE23-00AA0062C2EF}
DEFINE_GUID(IID_DataPkgProject, 0x8b072243, 0x8c1a, 0x11cf, 0xbe, 0x23, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);
