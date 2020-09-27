#ifndef __TNCONTROL_PERIODICQUEUE__
#define __TNCONTROL_PERIODICQUEUE__
//#pragma message("Defining __TNCONTROL_PERIODICQUEUE__")






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
class DLLEXPORT CTNPeriodicSend:public CTNSendData
{
	// These are friends so they can access the protected members.
	friend CTNSlave;
	friend CTNReachCheck;
	friend DWORD WINAPI SlavePeriodicSendThreadProc(LPVOID lpvParameter);


	protected:
		PTNCTRLCOMM						m_pCtrlComm; // control comm object to use when sending
		DWORD							m_dwInterval; // how often to resend
		DWORD							m_dwFirstSent; // when this item was first sent
		DWORD							m_dwLastSent; // when this item was last sent
		DWORD							m_dwTimeout; // when this item should stop being sent
		PVOID							m_pvUserContext; // context to use for callback if/when this send times out


		CTNPeriodicSend(void);
		virtual ~CTNPeriodicSend(void);
};

class DLLEXPORT CTNPeriodicSendQueue:public CTNSendDataQueue
{
};






#else //__TNCONTROL_PERIODICQUEUE__
//#pragma message("__TNCONTROL_PERIODICQUEUE__ already included!")
#endif //__TNCONTROL_PERIODICQUEUE__
