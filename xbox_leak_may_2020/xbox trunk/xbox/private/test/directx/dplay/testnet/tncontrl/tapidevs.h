#ifndef __TNCONTROL_TAPIDEV__
#define __TNCONTROL_TAPIDEV__
//#pragma message("Defining __TNCONTROL_TAPIDEV__")





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
// Defines
//==================================================================================
#define TAPIDEVICELIST_FILENAME		"tapidevs.ini"




//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNTAPIDevice:public CLString
{
	// These are friends so they can access the protected members.
	friend CTNMachineInfo;
	friend CTNTAPIDevicesList;


	protected:
		DWORD	m_dwDeviceID; // ID of this TAPI device
		char*	m_pszPhoneNumber; // string representation of phone number associated with device


	public:
		CTNTAPIDevice(char* szName, DWORD dwDeviceID);
		virtual ~CTNTAPIDevice(void);

		char* GetPhoneNumber(void);
		HRESULT SetPhoneNumber(char* pszNewNumber);
};

class DLLEXPORT CTNTAPIDevicesList:public CLStringList
{
	// These are friends so they can access the protected members.
	friend CTNMachineInfo;
	friend CTNSlaveInfo;


	protected:
		//CTNTAPIDevicesList(void);
		//virtual ~CTNTAPIDevicesList(void);

		HRESULT PackIntoBuffer(LPVOID pvBuffer, DWORD* pdwSize);
		HRESULT UnpackFromBuffer(LPVOID pvBuffer, DWORD dwSize);
#ifndef _XBOX // no file printing supported
		void PrintToFile(HANDLE hFile);
#endif // ! XBOX

	public:
		HRESULT GetFirstTAPIDeviceNameWithNumber(char** ppszDeviceName);
		HRESULT GetFirstTAPIDeviceNumber(char** ppszDeviceNumber);

};





#else //__TNCONTROL_TAPIDEV__
//#pragma message("__TNCONTROL_TAPIDEV__ already included!")
#endif //__TNCONTROL_TAPIDEV__
