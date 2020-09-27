#ifndef _XBOX // no IPC supported
#ifndef __TNCONTROL_IPC_OBJECT__
#define __TNCONTROL_IPC_OBJECT__
//#pragma message("Defining __TNCONTROL_IPC_OBJECT__")






//==================================================================================
// Common Defines
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
// Defines
//==================================================================================
#define MAX_MESSAGE_SIZE						(1024 * 1024) // in bytes, = 1 MB

#define TIMEOUT_DIE_OBJMSGRECVTHREAD			4000 // in milliseconds
#define TIMEOUT_DIE_OBJMONITORTHREAD			5000 // in milliseconds

#define OBJMSGRECVTHREAD_PING_INTERVAL			5000 // in milliseconds

#define IPCOBJ_ACK_WAIT_TIMEOUT					45000 // in milliseconds



#define OWNERNOTCONNECTED_WARNING_NUMINTERVALS	5 // how many ping intervals before warning about not connected

#define OWNERNOTCONNECTED_ABORT_NUMINTERVALS	10 // how many ping intervals before failing when not connected




//==================================================================================
// Internal prototypes
//==================================================================================
DWORD WINAPI IPCObjMsgRecvThreadProc(LPVOID lpvParameter);
DWORD WINAPI IPCObjMonitorThreadProc(LPVOID lpvParameter);




//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNIPCObject:public LLITEM
{
	// These are friends so they can access the protected members.
	friend CTNIPCObjectsList;
	friend CTNLeech;
	friend CTNLeechesList;
	friend CTNExecutor;
	friend DWORD WINAPI IPCObjMsgRecvThreadProc(LPVOID lpvParameter);
	friend DWORD WINAPI IPCObjMonitorThreadProc(LPVOID lpvParameter);


	private:
		PTNEXECUTORPRIV		m_pExecutor; // pointer to executor which owns this object
		BOOL				m_fOwnerCopy; // whether this is an owner copy or not
		BOOL				m_fConnected; // whether the other side has connected yet
		DWORD				m_dwMMWriteLockRefCount; // how many times we've taken the shared memory write lock
		HANDLE				m_hMMWriteMutex; // the shared memory write lock
		HANDLE				m_hMMFile; // handle to the shared memory
		HANDLE				m_hRedirMutex; // mutex to protect the redirection memory
		HANDLE				m_hRedirMMFile; // handle to the redirection shared memory
		HANDLE				m_hSendEvent; // event that will be set when a message is outgoing
		HANDLE				m_hAckEvent; // event that will be set when a message is acknowledge being received
		HANDLE				m_hConnectEvent; // event to set when the other side connects
		PVOID				m_pvConnectData; // pointer to data sent by other side upon connection
		DWORD				m_dwConnectDataSize; // size of data sent by other side upon connection

		HANDLE				m_hObjMsgRecvThread; // process side only; handle to thread used to receive messages from slave
		HANDLE				m_hKillObjMsgRecvThreadEvent; // process side only; event to set when the receive thread should be killed


		HRESULT StartObjMsgRecvThread(void);
		HRESULT KillObjMsgRecvThread(void);
		HRESULT HandleConnectMsg(PVOID pvData, DWORD dwDataSize);
		HRESULT HandleDisconnectMsg(void);

		// These are pure virtual functions, derived classes must implement them
		// because we sure don't.
		virtual HRESULT HandleMessage(DWORD dwCommand,
									PVOID pvSentData, DWORD dwSentDataSize,
									PVOID pvReplyData, DWORD* pdwReplyDataSize) = 0;
		virtual HRESULT HandleDisconnect(void) = 0;


	protected:
		char*				m_pszAttachPointID; // string that identifies this specific object connection pair
		HANDLE				m_hReceiveEvent; // event that will be set when a message comes in
		DWORD				m_dwLastMessageReceived; // time to update when message was last received


		CTNIPCObject(PTNEXECUTORPRIV pExecutor);
		virtual ~CTNIPCObject(void);
		
		HRESULT Connect(BOOL fOwnerCopy, PTNMODULEID pTNModuleID,
						char* szAttachPointID, HANDLE hConnectEvent,
						PVOID pvSendConnectData, DWORD dwSendConnectDataSize);
		HRESULT Disconnect(void);

		HRESULT AllowStaticIDConnection(PTNMODULEID pTNModuleID, char* szPrePlannedID);
		HRESULT ConnectWithStaticID(PTNMODULEID pTNModuleID, char* szPrePlannedID,
									HANDLE hConnectEvent, PVOID pvSendConnectData,
									DWORD dwSendConnectDataSize);

		HRESULT GetConnectData(PVOID* ppvData, DWORD* pdwDataSize);

		HRESULT ReceiveMessage(void);

		HRESULT SendMessage(DWORD dwCommand, PVOID pvData, DWORD dwDataSize,
							DWORD dwTimeout);

		HRESULT PrepareSendBuffer(DWORD dwCommand, PVOID* ppvSendBuffer,
								DWORD dwSendBufferSize);
		HRESULT SendBuffer(PVOID pvSendBuffer, DWORD dwSendBufferSize,
						DWORD dwTimeout);
		HRESULT SendBufferAndGetReply(PVOID pvSendBuffer, DWORD dwSendBufferSize,
									PVOID* ppvReplyBuffer, DWORD* pdwReplyBufferSize,
									DWORD dwTimeout);
		void ReturnMessageBuffers(PVOID pvSendBuffer, DWORD dwSendBufferSize,
								PVOID pvReplyBuffer, DWORD dwReplyBufferSize);

		HRESULT PingOwner(void);
};


class DLLEXPORT CTNIPCObjectsList:public LLIST
{
	// These are friends so they can access the protected members.
	friend DWORD WINAPI IPCObjMonitorThreadProc(LPVOID lpvParameter);


	private:
		HANDLE		m_hObjMonitorThread; // handle to thread the watches the objects for incoming events


		HRESULT StartObjMonitorThread(void);
		HRESULT KillObjMonitorThread(void);


	protected:
		HANDLE		m_hKillObjMonitorThreadEvent; // handle to event to set when monitor thread should die
		HANDLE		m_hObjListChangedEvent;  // handle to event to set when list changes


		CTNIPCObjectsList(void);
		virtual ~CTNIPCObjectsList(void);

		HRESULT AddObject(PTNIPCOBJECT pObject);
		HRESULT RemoveObject(PTNIPCOBJECT pObject);


	//public:
};



#else //__TNCONTROL_IPC_OBJECT__
//#pragma message("__TNCONTROL_IPC_OBJECT__ already included!")
#endif //__TNCONTROL_IPC_OBJECT__
#endif // ! XBOX
