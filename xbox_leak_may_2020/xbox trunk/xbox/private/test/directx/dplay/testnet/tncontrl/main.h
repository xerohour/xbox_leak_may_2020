#ifndef __TNCONTROL_MAIN__
#define __TNCONTROL_MAIN__
//#pragma message("Defining __TNCONTROL_MAIN__")




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


#undef DEBUG_MODULE
#define DEBUG_MODULE	"TNCONTROL"



//==================================================================================
// Defines
//==================================================================================
#define CURRENT_METAMASTER_API_VERSION		3



#define TIMEOUT_DIE_CONTROLSENDTHREAD		20000 // in milliseconds
#define TIMEOUT_DIE_CONTROLRECEIVETHREAD	5000 // in milliseconds


#define MESSAGE_PUMP_CHECK_INTERVAL			500 // in milliseconds




//----------------------------------------------------------------------------------
// Private testcase options, should not be used by user
//----------------------------------------------------------------------------------

// Means the script should not be included in the various case table access
// functions.
#define TNTCO_BUILTIN	0x0001




//----------------------------------------------------------------------------------
// Private control method flags and data, should not be used by user
//----------------------------------------------------------------------------------

// Means the control object being created is solely for the purpose of doing some
// NAT detection/analysis stuff.
#define TN_CTRLMETHODFLAG_TCPIP_NATDETECT	0x0001


//----------------------------------------------------------------------------------
// Reach check method types
//----------------------------------------------------------------------------------

#define TNRCM_CANCEL			1
#define TNRCM_UDPBROADCAST		2
#define TNRCM_UDP				3
#define TNRCM_TCP				4
#define TNRCM_IPX				5
#define TNRCM_MODEM				6
#define TNRCM_SERIAL			7





//==================================================================================
// Structs
//==================================================================================
typedef struct tagCOMMDATA
{
	//HANDLE		hStatusEvent; // event to set when the ctrl comm status has changed
	BOOL		fDropped; // has the connection to this machine been removed for any reason?
	PVOID		pvAddress; // pointer to data buffer holding address
	DWORD		dwAddressSize; // size of address buffer
} COMMDATA, * PCOMMDATA;

typedef struct tagUSERRESPONSEDATA
{
	DWORD	dwSize; // size of this structure
	DWORD	dwResponse; // user defined response ID or command
	DWORD	dwResponseDataSize; // size of the following extra response data buffer

	// Anything after this is part of the response data blob
} USERRESPONSEDATA, * PUSERRESPONSEDATA;






//==================================================================================
// External Statics
//==================================================================================
extern HINSTANCE			s_hInstance;







#else //__TNCONTROL_MAIN__
//#pragma message("__TNCONTROL_MAIN__ already included!")
#endif //__TNCONTROL_MAIN__
