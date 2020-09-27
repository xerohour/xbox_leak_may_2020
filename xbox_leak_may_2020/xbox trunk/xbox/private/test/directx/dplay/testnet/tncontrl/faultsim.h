#ifndef __TNCONTROL_FAULTSIM__
#define __TNCONTROL_FAULTSIM__
//#pragma message("Defining __TNCONTROL_FAULTSIM__")






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
class DLLEXPORT CTNFaultSim:public LLITEM
{
	// These are friends so that they can access the protected members
	friend CTNExecutor;
	friend CTNExecutorPriv;
	friend CTNFaultSimIMTest;
	friend CTNMachineInfo;


	protected:
		DWORD	m_dwID; // ID of this fault sim object
		BOOL	m_fInitialized; // whether this object has been initialized or not
		HANDLE	m_hExclusiveLock; // handle to mutex preventing multi-thread/process access



		CTNFaultSim(void);
		virtual ~CTNFaultSim(void);


		HRESULT EnsureExclusiveUse(void);
		HRESULT RelenquishExclusiveUse(void);

		// Virtual functions which the derived classes should implement as well.
		virtual HRESULT Initialize(PVOID pvInitData, DWORD dwInitDataSize);
		virtual HRESULT Release(void);

		// Pure virtuals.  The derived classes must implement these because we sure
		// don't.
		virtual BOOL IsInstalledAndCanBeRun(void) = 0;

		virtual HRESULT SetBandwidth(BOOL fSend, DWORD dwHundredBytesPerSec) = 0;
		virtual HRESULT SetLatency(BOOL fSend, DWORD dwMSDelay) = 0;
		virtual HRESULT SetDropPacketsPercent(BOOL fSend, DWORD dwPercent) = 0;
		virtual HRESULT Reconnect(BOOL fSend) = 0;
		virtual HRESULT Disconnect(BOOL fSend) = 0;

		/*
		virtual HRESULT SetDropPacketsPattern(BOOL fSend, char* szPatternString) = 0;
		virtual HRESULT CorruptPackets(BOOL fSend, ?) = 0;
		virtual HRESULT ReorderPackets(BOOL fSend, ?) = 0;
		*/
};

class DLLEXPORT CTNFaultSimList:public LLIST
{
	/*
	// This is a friend so it can access the protected members
	friend CTNSlave;

	public:
		CTNFaultSimList(void);
		virtual ~CTNFaultSimList(void);
	*/
};






#else //__TNCONTROL_FAULTSIM__
//#pragma message("__TNCONTROL_FAULTSIM__ already included!")
#endif //__TNCONTROL_FAULTSIM__
