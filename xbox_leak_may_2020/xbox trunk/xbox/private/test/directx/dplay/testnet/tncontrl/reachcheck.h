#ifndef __TNCONTROL_REACHCHECK__
#define __TNCONTROL_REACHCHECK__
//#pragma message("Defining __TNCONTROL_REACHCHECK__")






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
class DLLEXPORT CTNReachCheck:public LLITEM
{
	// These are friends so they can access the protected members
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNSlave;
	friend CTNReachChecksList;
	friend CTNCtrlCommTCP;


	protected:
		DWORD				m_dwID; // ID of this check instance
		DWORD				m_dwMethod; // method of communication for this check
		PVOID				m_pvMethodData; // pointer to method data
		DWORD				m_dwMethodDataSize; // size of method data
		HANDLE				m_hEvent; // event to signal when operation is done
		BOOL				m_fResult; // is the result a success?
		HRESULT				m_hresult; // error code, if failed
		PTNPERIODICSEND		m_pPeriodicSend; // slave only; periodic send associated with check
		PVOID				m_pvAddress; // slave only; address retrieved
		DWORD				m_dwAddressSize; // slave only; size of address retrieved
		PTNCTRLCOMM			m_pCtrlComm; // slave only; control comm object being used to perform reach check
		PVOID				m_pvCommData; // slave only; data used by the control comm object


		CTNReachCheck(DWORD dwID, DWORD dwMethod, PVOID pvMethodData,
					DWORD dwMethodDataSize);
		virtual ~CTNReachCheck(void);
};

class DLLEXPORT CTNReachChecksList:public LLIST
{
	// These are friends so they can access the protected members.
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNSlave;


	protected:
		PTNREACHCHECK GetCheck(DWORD dwID);
};

class DLLEXPORT CTNReachCheckTarget:public LLITEM
{
	// These are friends so they can access the protected members
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNSlave;
	friend CTNReachCheckTargetsList;


	protected:
		TNCTRLMACHINEID		m_id; // id of other machine the list of checks pertains to
		CTNReachChecksList	m_checks; // list of checks for this machine


		CTNReachCheckTarget(PTNCTRLMACHINEID pSlaveID);
		virtual ~CTNReachCheckTarget(void);
};

class DLLEXPORT CTNReachCheckTargetsList:public LLIST
{
	// These are friends so they can access the protected members.
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNSlave;


	protected:
		PTNREACHCHECKTARGET GetTarget(PTNCTRLMACHINEID pSlaveID);
};





#else //__TNCONTROL_REACHCHECK__
//#pragma message("__TNCONTROL_REACHCHECK__ already included!")
#endif //__TNCONTROL_REACHCHECK__
