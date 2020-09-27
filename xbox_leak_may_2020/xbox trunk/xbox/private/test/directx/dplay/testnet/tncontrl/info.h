#ifndef __TNCONTROL_INFO__
#define __TNCONTROL_INFO__
//#pragma message("Defining __TNCONTROL_INFO__")





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
class DLLEXPORT CTNMachineInfo:public LLITEM
{
	// These are friends so they can access the protected members.
	friend CTNExecutor;
	friend CTNExecutorPriv;
	friend CTNSlave;
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX

#ifndef _XBOX // no IPC supported
	friend CTNLeech;
#endif // ! XBOX


	private:
		HRESULT LoadLocalWinSockInfo(void);
		HRESULT LoadLocalRASInfo(void);
		HRESULT LoadLocalTAPIInfo(char* szTestnetRootPath);
		HRESULT LoadLocalCOMPortInfo(char* szTestnetRootPath);
		HRESULT LoadLocalSoundCardInfo(void);
		HRESULT GetFaultSimCapabilities(void);


	protected:
		HRESULT PackIntoBuffer(PVOID pvBuffer, DWORD* pdwSize);
		HRESULT UnpackFromBuffer(PVOID pvBuffer, DWORD dwSize);


	public:
		// These fields propogate to other machines (get sent over the wire).  They
		// are also filled in by the control layer on startup.

		char					m_szComputerName[MAX_COMPUTERNAME_SIZE + 32]; // name of computer, + room for uniqueness identifier
		char					m_szUserName[MAX_USERNAME_SIZE]; // currently logged in user of machine
#ifndef _XBOX
		OSVERSIONINFO			m_os; // version of operation system
#else
		OSVERSIONINFOA			m_os; // version of operation system
#endif
		WORD					m_wProcessorArchitecture; // type of processor(s) this system has
		DWORD					m_dwNumberOfProcessors; // number of processors this system has
		MEMORYSTATUS			m_memstats; // memory statistics
		WORD					m_wWinSock; // WinSock version
		BOOL					m_fIPXInstalled; // whether IPX is installed
		BOOL					m_fFaultSimIMTestAvailable; // whether the IMTest fault simulator is available
		CTNIPAddressesList		m_ipaddrs; // ip addresses this machine is using
		CTNTAPIDevicesList		m_TAPIdevices; // list of TAPI devices this machine has
		CTNCOMPortsList			m_COMports; // list ofCOM ports and their connections
		CTNSoundCardsList		m_soundcards; // list of sound cards
		CTNTrackedBinaries		m_binaries; // important binaries the module wants to note

		// Pointer to user defined data.  This is local, i.e. changes are not
		// propogated between master and slave.  It is initialized to NULL by the
		// control layer.
		PVOID					m_pvUserData;



		CTNMachineInfo(void);
		virtual ~CTNMachineInfo(void);


		BOOL IsNTBasedOS(void);
		BOOL IsMillenniumOS(void);
		HRESULT GetOSString(char* szString);

		HRESULT LoadLocalMachineInfo(char* szTestnetRootPath,
									PTNADDIMPORTANTBINARIESPROC pfnAddImportantBinaries,
									int iComputerNameUniqueness);

};


class DLLEXPORT CTNOtherMachineInfo:public CTNMachineInfo
{
	// These are friends so they can access the protected members.
	friend CTNSlave;
	friend CTNOtherMachineInfosList;
	friend CTNExecutor;
	friend CTNTestInstanceS;

#ifndef _XBOX // no IPC supported
	friend CTNLeech;
#endif // ! XBOX


	protected:
		TNCTRLMACHINEID		m_id; // ID of this machine


		CTNOtherMachineInfo(PTNCTRLMACHINEID pID);
		virtual ~CTNOtherMachineInfo(void);
};

class DLLEXPORT CTNOtherMachineInfosList:public LLIST
{
	// These are friends so they can access the protected members.
	friend CTNSlave;


	protected:
		PTNOTHERMACHINEINFO GetInfoForMachine(PTNCTRLMACHINEID pID);
};




#else //__TNCONTROL_INFO__
//#pragma message("__TNCONTROL_INFO__ already included!")
#endif //__TNCONTROL_INFO__
