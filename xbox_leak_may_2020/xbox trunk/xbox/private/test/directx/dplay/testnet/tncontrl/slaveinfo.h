#ifndef __TNCONTROL_SLAVEINFO__
#define __TNCONTROL_SLAVEINFO__
//#pragma message("Defining __TNCONTROL_SLAVEINFO__")






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
// Classes
//==================================================================================
class DLLEXPORT CTNSlaveInfo:public CTNMachineInfo
{
	// Declare these as friends so they can access the protected members.
	friend class CTNSlavesList;
	friend class CTNTestInstanceM;
#ifndef _XBOX 
	friend class CTNReport;					// Reports not supported
	friend class CTNMaster;					// Masters not supported
#endif // ! XBOX
	friend class CTNJob;


	protected:
		TNCTRLMACHINEID				m_id; // ID of this slave
		COMMDATA					m_commdata; // structure with ctrl comm related data in it
		DWORD						m_fReadyToTest; // has the slave completely joined the session yet
		DWORD						m_dwEndtime; // time when the slave should stop getting additional tests
		PTNTESTINSTANCEM			m_pCurrentTest; // test this machine is currently working on
		CTNTestMsList				m_ongoingtestslist; // list of all tests this slave has outstanding
		//CTNTestMsList				m_completedtestslist; // list of all tests this slave has finished
		CTNReachCheckTargetsList	m_reachchecktargets; // list of targets of reach checks that have been performed
		DWORD						m_dwLastReceiveTime; // last time we received something from the slave
		DWORD						m_dwNumPingsSinceReceive; // how many pings we've sent since we last received from this slave
		DWORD						m_dwMultiInstanceKey; // key generated to help detect multiple slave instances on the same machine
		int							m_iNumOtherInstancesInSession; // how many other slaves on same machine are in the current session


		CTNSlaveInfo(void);
		virtual ~CTNSlaveInfo(void);

		PTNTESTINSTANCEM GetTest(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID);
		void PrintToFile(HANDLE hFile);
		HRESULT	UpdateLastReceiveTime(void);
		DWORD TimeSinceLastReceive(void);
		DWORD PingsSinceLastReceive(void);
};






#else //__TNCONTROL_SLAVEINFO__
//#pragma message("__TNCONTROL_SLAVEINFO__ already included!")
#endif //__TNCONTROL_SLAVEINFO__
