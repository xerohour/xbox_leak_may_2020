#ifndef __TNCONTROL_COMPORTS__
#define __TNCONTROL_COMPORTS__
//#pragma message("Defining __TNCONTROL_COMPORTS__")





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
#define COMPORTLIST_FILENAME	"comports.ini"




//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNCOMPort:public CLString
{
	// These are friends so they can access the protected members.
	friend CTNMachineInfo;
	friend CTNCOMPortsList;


	protected:
		DWORD	m_dwCOMPort; // COM port number of this object


	public:
		CTNCOMPort(char* szConnectedMachineName, DWORD dwCOMPort);
		virtual ~CTNCOMPort(void);

		DWORD GetCOMPort(void);
};

class DLLEXPORT CTNCOMPortsList:public CLStringList
{
	// These are friends so they can access the protected members.
	friend CTNMachineInfo;
	friend CTNSlaveInfo;


	protected:
		//CTNCOMPortsList(void);
		//virtual ~CTNCOMPortsList(void);

		HRESULT PackIntoBuffer(PVOID pvBuffer, DWORD* pdwSize);
		HRESULT UnpackFromBuffer(PVOID pvBuffer, DWORD dwSize);
#ifndef _XBOX // no file printing supported
		void PrintToFile(HANDLE hFile);
#endif // ! XBOX
};





#else //__TNCONTROL_COMPORTS__
//#pragma message("__TNCONTROL_COMPORTS__ already included!")
#endif //__TNCONTROL_COMPORTS__
