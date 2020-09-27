#ifndef __TNCONTROL_COMM__
#define __TNCONTROL_COMM__
//#pragma message("Defining __TNCONTROL_COMM__")





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
// External Structs
//==================================================================================
typedef struct tagCTRLCOMM_ADDRESSLIST
{
	DWORD	dwNumAddresses; // how many entries in the list
} CTRLCOMM_ADDRESSLIST, * PCTRLCOMM_ADDRESSLIST;

typedef struct tagCONTROLCOMMINITPBLOCK
{
	DWORD				dwSize; // size of this structure
	DWORD				dwControlMethodID; // ID of control method being loaded
	PTNCONTROLLAYER		pControlLayerObj; // pointer to the master or slave object
	BOOL				fMaster; // is the app a testmaster
	DWORD				dwFlags; // method specific extra flags that can be specified
	PVOID				pvData; // pointer to method specific extra data buffer
	DWORD				dwDataSize; // size of data in above buffer
} CONTROLCOMMINITPBLOCK, * PCONTROLCOMMINITPBLOCK;





//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNCtrlComm
{
	// These are friends so they can access the protected members
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX

	friend CTNSlave;


	private:
		HANDLE					m_hSendThread; // the send thread
		HANDLE					m_hReceiveThread; // the receive thread


	protected:
		DWORD					m_dwMethodID; // ID of the control method
		BOOL					m_fMaster; // are we the testmaster
		LPTHREAD_START_ROUTINE	m_lpSendThreadProc; // procedure to send data with.
		LPTHREAD_START_ROUTINE	m_lpReceiveThreadProc; // procedure to receive data with.


		HRESULT StartThreads(void);
		HRESULT KillThreads(void);


	public:
		// These are only public so the threads can access them
		DWORD					m_dwRefCount; // how many people are using us
		PTNCONTROLLAYER			m_pControlObj; // pointer to object using us that is receiving incoming messages
		CTNSendDataQueue		m_sendqueue; // linked list with data to send
		HANDLE					m_hKillSendThreadEvent; // when to kill the send thread
		HANDLE					m_hKillReceiveThreadEvent; // when to kill the receive thread


		DLLSAFE_NEWS_AND_DELETES;

		CTNCtrlComm(void);
		virtual ~CTNCtrlComm(void);


		// These are pure virtuals, derived classes must implement them because we sure
		// don't!
		virtual HRESULT Initialize(void) = 0;

		virtual HRESULT Release(void) = 0;

		virtual HRESULT GetBroadcastAddress(PVOID pvModifierData,
											DWORD dwModifierDataSize,
											PVOID pvAddress,
											DWORD* pdwAddressSize) = 0;

		virtual HRESULT ConvertStringToAddress(char* szString,
												PVOID pvModifierData,
												DWORD dwModifierDataSize,
												PVOID pvAddress,
												DWORD* pdwAddressSize) = 0;

		virtual HRESULT GetSelfAddressAsString(char* pszString,
												DWORD* pdwStringSize) = 0;

		virtual HRESULT BindDataToAddress(PCOMMDATA pCommData) = 0;

		virtual HRESULT UnbindDataFromAddress(PCOMMDATA pCommData) = 0;

		virtual HRESULT FlushSendQueue(void) = 0;


		virtual HRESULT StartAcceptingReachCheck(PTNREACHCHECK pReachCheck) = 0;

		virtual HRESULT StopAcceptingReachCheck(PTNREACHCHECK pReachCheck) = 0;

		virtual HRESULT PrepareToReachCheck(PTNREACHCHECK pReachCheck) = 0;

		virtual HRESULT CleanupReachCheck(PTNREACHCHECK pReachCheck) = 0;
};





#else //__TNCONTROL_COMM__
//#pragma message("__TNCONTROL_COMM__ already included!")
#endif //__TNCONTROL_COMM__
