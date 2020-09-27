#ifndef __TNCONTROL_SYNCDATA__
#define __TNCONTROL_SYNCDATA__
//#pragma message("Defining __TNCONTROL_SYNCDATA__")






//==================================================================================
// Defines
//==================================================================================
#ifdef TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)

#else // ! TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllimport)

#endif // ! TNCONTRL_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG






//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNSyncData:public LLITEM
{
	// These are friends so they can access the protected members
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNSyncDataList;


	private:
		PVOID				m_pvData; // pointer to sync data, use functions to access
		DWORD				m_dwDataSize; // size of sync data, use functions to access


	protected:
		DWORD				m_dwRequestID; // request ID that generated this sync item
		char*				m_pszSyncName; // pointer to name of sync operation this falls under
		int					m_iTesterNum; // tester this item represents
		PTNSYNCDATALIST		m_pSyncList; // pointer to list of testers this guy is trying to sync with


		HRESULT SetData(PVOID pvNewData, DWORD dwNewDataSize);
		HRESULT GetData(PVOID* ppvData, DWORD* pdwDataSize);
		HRESULT DoSyncCheck(PTNSYNCDATALIST pCheckedList, BOOL* pfInSync);


	public:
		CTNSyncData(DWORD dwRequestID, char* pszSyncName, int iTesterNum);
		virtual ~CTNSyncData(void);
};

class DLLEXPORT CTNSyncDataList:public LLIST
{
	// These are friends so they can access the protected members
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNExecutorPriv;

#ifndef _XBOX // no IPC supported
	friend CTNLeech;
#endif // ! XBOX


	protected:
		PTNSYNCDATA GetItemForTester(int iTesterNum);
		PTNSYNCDATA GetItemForTester(char* szSyncName, int iTesterNum);

		HRESULT AddAndDoSyncCheck(DWORD dwRequestID, char* szSyncName,
								int iTesterNum, int iNumTargets, int* aiTargetList,
								PVOID pvData, DWORD dwDataSize,
								PTNSYNCDATALIST pAffectedList, BOOL* pfInSync);

		HRESULT AddDataFromTester(int iTesterNum, PVOID pvNewData,
									DWORD dwNewDataSize);

		HRESULT PackDataForTesterIntoBuffer(int iTesterNum,
											PVOID pvBuffer,
											DWORD* pdwBufferSize);

		HRESULT UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize);



	public:
		CTNSyncDataList(void);
		virtual ~CTNSyncDataList(void);


		HRESULT GetDataFromTester(int iTesterNum, PVOID* ppvData,
								DWORD* pdwDataSize);
};






#else //__TNCONTROL_SYNCDATA__
//#pragma message("__TNCONTROL_SYNCDATA__ already included!")
#endif //__TNCONTROL_SYNCDATA__
