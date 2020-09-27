#ifndef __TNCONTROL_COMMTCP__
#define __TNCONTROL_COMMTCP__
//#pragma message("Defining __TNCONTROL_COMMTCP__")






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



// This is the primary port used to receive data.
#define CTRLCOMM_TCPIP_PORT				1977

#define INITIAL_RECEIVEBUFFER_SIZE		256 // in bytes


#ifndef INCL_WINSOCK_API_TYPEDEFS
#pragma message("ERROR: You will need to #define INCL_WINSOCK_API_TYPEDEFS 1 and #include <winsock2.h> before you #include <windows.h>")
#endif //! INCL_WINSOCK_API_TYPEDEFS




//==================================================================================
// Macros
//==================================================================================
#define BYTEREVERSEWORD(word)	(MAKEWORD(HIBYTE(word), LOBYTE(word)))



//==================================================================================
// External Prototypes
//==================================================================================
HRESULT CtrlCommTCPLoadOptimal(PCONTROLCOMMINITPBLOCK pParams,
								PTNCTRLCOMM* ppTNCtrlComm);
HRESULT CtrlCommTCPLoadWinSock1(PCONTROLCOMMINITPBLOCK pParams,
								PTNCTRLCOMM* ppTNCtrlComm);
HRESULT CtrlCommTCPLoadWinSock2OneToOne(PCONTROLCOMMINITPBLOCK pParams,
										PTNCTRLCOMM* ppTNCtrlComm);





//==================================================================================
// Structs
//==================================================================================
typedef struct tagCTRLCOMM_TCP_DATAHEADER
{
	DWORD	dwSize;
} CTRLCOMM_TCP_DATAHEADER, * PCTRLCOMM_TCP_DATAHEADER;





//==================================================================================
// Local Prototypes
//==================================================================================
DWORD WINAPI WSSendThreadProc(LPVOID lpvParameter);
DWORD WINAPI WS1ReceiveThreadProc(LPVOID lpvParameter);
DWORD WINAPI WS2ReceiveThreadProc(LPVOID lpvParameter);
DWORD WINAPI WS2OneToOneReceiveThreadProc(LPVOID lpvParameter);





//==================================================================================
// Classes
//==================================================================================
typedef class CLinkedSocket	CLinkedSocket, * PLINKEDSOCKET;
class CLinkedSocket:public LLITEM
{
	// This is declared a friend class so it can access the protected data.
	friend class CTNCtrlCommTCP;

	protected:
		SOCKET			m_socket; // the socket
		SOCKADDR_IN		m_address; // who the socket is connected to
		BOOL			m_fTCP; // is this a TCP (as opposed to UDP) socket?
		BOOL			m_fListen; // if it's TCP, is it a listening socket?
		BOOL			m_fBroadcast; // if it's UDP, can it send to the broadcast?
		PCOMMDATA		m_pCommData; // pointer to structure shared with control layer
		HANDLE			m_hWS2Event; // event associated with socket
		DWORD			m_dwFoundPriorToReachCheck; // number of times reach check functions searched for the socket and found it already existing


	public:
		CLinkedSocket(SOCKET sNew, SOCKADDR_IN address, BOOL fTCP, BOOL fListen,
					BOOL fBroadcast, HANDLE hWinSock2Event);
		virtual ~CLinkedSocket(void);


		SOCKET* GetSocket(void);
		SOCKADDR_IN* GetAddress(void);
		HANDLE GetEvent(void);

		BOOL IsTCPListen(void);
		BOOL IsUDP(void);
		BOOL IsUDPBroadcast(void);

		HRESULT CloseSocket(void);
};



typedef class CLSocketsList	CLSocketsList, * PLSOCKETSLIST;
class CLSocketsList:public LLIST
{
	public:
		HANDLE		m_hListChangedEvent; // event to set when the list changes


		CLSocketsList(void);
		virtual ~CLSocketsList(void);

		/*
		void EnterCritSection(void);
		void LeaveCritSection(void);
		*/

		HRESULT AddSocket(SOCKET sNew, SOCKADDR_IN address, BOOL fTCP, BOOL fListen,
							BOOL fBroadcast, HANDLE hWinSock2Event,
							PLINKEDSOCKET* ppLSocket);

		HRESULT AddNewConnectionFromSocket(SOCKET* pListenSocket,
											HANDLE hWinSock2Event,
											LPFN_WSAEVENTSELECT lpfnWSAEventSelect,
											BOOL fSocketOwnsEvent,
											PLINKEDSOCKET* ppLSocket);

		PLINKEDSOCKET GetLSocketByAddress(SOCKADDR_IN* pAddress, BOOL fTCP);
};


class CTNCtrlCommTCP:public CTNCtrlComm
{
	// Declare these as friends so they can access the protected members
	friend CTNSlave;
	friend DWORD WINAPI SlavePeriodicSendThreadProc(LPVOID lpvParameter); // see slave.h

	friend DWORD WINAPI WSSendThreadProc(LPVOID lpvParameter);
	friend DWORD WINAPI WS1ReceiveThreadProc(LPVOID lpvParameter);
	friend DWORD WINAPI WS2ReceiveThreadProc(LPVOID lpvParameter);
	friend DWORD WINAPI WS2OneToOneReceiveThreadProc(LPVOID lpvParameter);


	private:
		BOOL				m_fCtrlMethodMode; // whether this a real comm tcp object or only temporary
		BOOL				m_fWinSockStarted; // whether WSAStartup was successfully called yet or not
		HINSTANCE			m_hWinSock2DLL; // handle to WinSock 2 DLL if loaded
		CTNReachChecksList	m_reachchecks; // list of all currently outstanding IP data accepts or connects


		HRESULT InitWinSock2(void);
		HRESULT CreateUDPSocket(WORD wPort, BOOL fBroadcast,
								PLINKEDSOCKET* ppLSocket);
		HRESULT CreateTCPListenSocket(WORD wPort, PLINKEDSOCKET* ppLSocket);
		HRESULT CreateAndConnectTCPSocket(SOCKADDR_IN* pAddress,
										PLINKEDSOCKET* ppLSocket);


	protected:
		CLSocketsList					m_socketlist; // linked list of socket descriptors and other info
		BOOL							m_fWinSock2; // can we use WinSock 2 functionality?
		HANDLE							m_hAllSocketsReceiveEvent; // handle to read event associated with all sockets
		LPFN_WSAEVENTSELECT				m_lpfnWSAEventSelect; // pointer to WinSock2 proc
		LPFN_WSAWAITFORMULTIPLEEVENTS	m_lpfnWSAWaitForMultipleEvents; // pointer to WinSock2 proc
		LPFN_WSARESETEVENT				m_lpfnWSAResetEvent; // pointer to WinSock2 proc

		HRESULT SendFirstData(void);
		HRESULT CheckForReceiveData(void);
		HRESULT CheckAndHandleEventOnSocket(PLINKEDSOCKET pLSocket);
		HRESULT HandleEventOnSocket(PLINKEDSOCKET pLSocket);
		HRESULT InternalCloseReachCheck(PTNREACHCHECK pReachCheck);


		HRESULT SendUDPDataTo(SOCKADDR_IN* pAddress, PVOID pvData,
							DWORD dwDataSize);
		HRESULT SendTCPDataTo(SOCKADDR_IN* pAddress, PVOID pvData,
							DWORD dwDataSize);


	public:
		CTNCtrlCommTCP(PCONTROLCOMMINITPBLOCK pParams, BOOL fWinSock2);
		virtual ~CTNCtrlCommTCP(void);


		// Implementation of virtuals
		virtual HRESULT Initialize(void);

		virtual HRESULT Release(void);

		virtual HRESULT GetBroadcastAddress(PVOID pvModifierData,
											DWORD dwModifierDataSize,
											PVOID pvAddress,
											DWORD* pdwAddressSize);

		virtual HRESULT ConvertStringToAddress(char* szString,
												PVOID pvModifierData,
												DWORD dwModifierDataSize,
												PVOID pvAddress,
												DWORD* pdwAddressSize);

		virtual HRESULT GetSelfAddressAsString(char* pszString,
												DWORD* pdwStringSize);

		virtual HRESULT BindDataToAddress(PCOMMDATA pCommData);

		virtual HRESULT UnbindDataFromAddress(PCOMMDATA pCommData);

		virtual HRESULT FlushSendQueue(void);


		virtual HRESULT StartAcceptingReachCheck(PTNREACHCHECK pReachCheck);

		virtual HRESULT StopAcceptingReachCheck(PTNREACHCHECK pReachCheck);

		virtual HRESULT PrepareToReachCheck(PTNREACHCHECK pReachCheck);

		virtual HRESULT CleanupReachCheck(PTNREACHCHECK pReachCheck);
};





#else //__TNCONTROL_COMMTCP__
//#pragma message("__TNCONTROL_COMMTCP__ already included!")
#endif //__TNCONTROL_COMMTCP__
