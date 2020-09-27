#ifndef __TNCONTROL_IPADDRESSES__
#define __TNCONTROL_IPADDRESSES__
//#pragma message("Defining __TNCONTROL_IPADDRESSES__")






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


#define TNIPADDR_LOCAL	1 // WinSock reported this IP address
#define TNIPADDR_NAT	2 // This IP address was determined via the NAT detection routines




//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNIPAddress:public LLITEM
{
	// These are friends so they can access the protected member
	friend CTNIPAddressesList;
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend DWORD WINAPI SlavePeriodicSendThreadProc(LPVOID lpParameter); // see slave.h


	protected:
		IN_ADDR		m_ip; // IP address this item represents
		DWORD		m_dwType; // type of IP address this is


	public:
		CTNIPAddress(IN_ADDR* pinaddr, DWORD dwType);
		virtual ~CTNIPAddress(void);
};

class DLLEXPORT CTNIPAddressesList:public LLIST
{
	// These are friends so they can access the protected members
	friend CTNMachineInfo;
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNSlave;


	protected:
		HRESULT AddIP(IN_ADDR* pinaddr, DWORD dwType);
		HRESULT LoadLocalIPs(void);

		HRESULT PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize);
		HRESULT UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize);

		BOOL DoesIPExist(IN_ADDR* pinaddr);


	public:
		CTNIPAddressesList(void);
		virtual ~CTNIPAddressesList(void);


		HRESULT GetIPString(int iIndex, char* szString);
};





#else //__TNCONTROL_IPADDRESSES__
//#pragma message("__TNCONTROL_IPADDRESSES__ already included!")
#endif //__TNCONTROL_IPADDRESSES__
