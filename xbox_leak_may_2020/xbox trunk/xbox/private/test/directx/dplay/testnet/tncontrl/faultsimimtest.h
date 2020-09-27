#ifndef __TNCONTROL_FAULTSIMIMTEST__
#define __TNCONTROL_FAULTSIMIMTEST__
//#pragma message("Defining __TNCONTROL_FAULTSIMIMTEST__")






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
class DLLEXPORT CTNFaultSimIMTest:public CTNFaultSim
{
	// These are friends so that they can access the protected members
	friend CTNExecutor;
	friend CTNMachineInfo;


	protected:
		CTNFaultSimIMTest(void);
		virtual ~CTNFaultSimIMTest(void);


		// These are implementations of virtual functions.
		HRESULT Initialize(LPVOID lpvInitData, DWORD dwInitDataSize);
		HRESULT Release(void);

		// This is an implementation of a pure virtual function.
		BOOL IsInstalledAndCanBeRun(void);

		HRESULT SetBandwidth(BOOL fSend, DWORD dwHundredBytesPerSec);
		HRESULT SetLatency(BOOL fSend, DWORD dwMSDelay);
		HRESULT SetDropPacketsPercent(BOOL fSend, DWORD dwPercent);
		/*
		HRESULT SetDropPacketsPattern(BOOL fSend, char* szPatternString);
		HRESULT CorruptPackets(BOOL fSend, ?);
		HRESULT ReorderPackets(BOOL fSend, ?);
		*/

		HRESULT Reconnect(BOOL fSend);
		HRESULT Disconnect(BOOL fSend);
};







#else //__TNCONTROL_FAULTSIMIMTEST__
//#pragma message("__TNCONTROL_FAULTSIMIMTEST__ already included!")
#endif //__TNCONTROL_FAULTSIMIMTEST__
