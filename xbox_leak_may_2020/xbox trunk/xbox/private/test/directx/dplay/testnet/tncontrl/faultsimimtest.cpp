//==================================================================================
// Includes
//==================================================================================
#define INCL_WINSOCK_API_TYPEDEFS 1 // includes winsock2 fn proto's, for getprocaddress
#ifndef _XBOX
#include <winsock2.h>
#else // ! XBOX
#include <winsockx.h>		// Needed for XnetInitialize
#endif // XBOX

#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\sprintf.h"

//#include "..\..\imtest\inc\imtcommon.h"
//#include "..\..\imtest\inc\imtlib.h"

#include "tncontrl.h"
#include "main.h"
#include "sendq.h"
#include "comm.h"
#include "commtcp.h"

#include "faultsim.h"
#include "faultsimimtest.h"




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::CTNFaultSimIMTest()"
//==================================================================================
// CTNFaultSimIMTest constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNFaultSimIMTest object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNFaultSimIMTest::CTNFaultSimIMTest(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNFaultSimIMTest));

	this->m_dwID = TN_FAULTSIM_IMTEST;
} // CTNFaultSimIMTest::CTNFaultSimIMTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::~CTNFaultSimIMTest()"
//==================================================================================
// CTNFaultSimIMTest destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNFaultSimIMTest object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNFaultSimIMTest::~CTNFaultSimIMTest(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNFaultSimIMTest::~CTNFaultSimIMTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::Initialize()"
//==================================================================================
// CTNFaultSimIMTest::Initialize
//----------------------------------------------------------------------------------
//
// Description: Sets up this object, and prepares it for use.
//
// Arguments:
//	LPVOID lpvInitData		Pointer to data to use when initializing.
//	DWORD dwInitDataSize	Size of data to use when initializing.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSimIMTest::Initialize(LPVOID lpvInitData, DWORD dwInitDataSize)
{
#ifndef _XBOX // fault simulation not supported at this time
	HRESULT				hr;
	BOOL				fEnsuredExclusiveUse = FALSE;
	BOOL				fInitted = FALSE;
	IM_GLOBAL_PROPERTY	imgp;
	BOOL				fSourcePortSet = FALSE;
	BOOL				fDestPortSet = FALSE;


	DPL(9, "==>(%x, %u)", 2, lpvInitData, dwInitDataSize);

	hr = this->EnsureExclusiveUse();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't ensure exclusive use!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't ensure exclusive use)

	fEnsuredExclusiveUse = TRUE;


	hr = InitializeIMTest();
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't initialize IMTest driver!", 0);
		goto ERROR_EXIT;
	} // end if (failed initting IMTest)

	fInitted = TRUE;


	// Tell the driver to ignore incoming control layer traffic.
	ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
	imgp.ulGlobalProperty = GI_IGNORE_IP_SOURCE_PORT;
	imgp.ulFlagEnable = 1;
	//imgp.ucData = 0;
	imgp.ulValue = CTRLCOMM_TCPIP_PORT; // port control layer uses

	hr = SetIMGlobalProperty(&imgp);
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't tell IMTest to leave incoming control layer traffic alone!", 0);
		goto ERROR_EXIT;
	} // end if (failed turning IMTest property on)

	fSourcePortSet = TRUE;


	// Tell the driver to ignore outgoing control layer traffic.
	ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
	imgp.ulGlobalProperty = GI_IGNORE_IP_DESTINATION_PORT;
	imgp.ulFlagEnable = 1;
	//imgp.ucData = 0;
	imgp.ulValue = CTRLCOMM_TCPIP_PORT; // port control layer uses

	hr = SetIMGlobalProperty(&imgp);
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't tell IMTest to leave outgoing control layer traffic alone!", 0);
		goto ERROR_EXIT;
	} // end if (failed turning IMTest property on)

	fDestPortSet = TRUE;


	// Tell the driver to ignore Address Resolution Protocol traffic.
	ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
	imgp.ulGlobalProperty = GI_IGNORE_ARP;
	imgp.ulFlagEnable = 1;
	//imgp.ucData = 0;
	//imgp.ulValue = 0;

	hr = SetIMGlobalProperty(&imgp);
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't tell IMTest to leave ARP traffic alone!", 0);
		goto ERROR_EXIT;
	} // end if (failed turning IMTest property on)



	DPL(9, "<== S_OK", 0);

	return (S_OK);


ERROR_EXIT:

	if (fDestPortSet)
	{
		ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
		imgp.ulGlobalProperty = GI_IGNORE_IP_DESTINATION_PORT;
		//imgp.ulFlagEnable = 0;
		//imgp.ucData = 0;
		//imgp.ulValue = 0;

		// Ignore error
		SetIMGlobalProperty(&imgp);

		fDestPortSet = FALSE;
	} // end if (dest port set to be ignored)

	if (fSourcePortSet)
	{
		ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
		imgp.ulGlobalProperty = GI_IGNORE_IP_SOURCE_PORT;
		//imgp.ulFlagEnable = 0;
		//imgp.ucData = 0;
		//imgp.ulValue = 0;

		// Ignore error
		SetIMGlobalProperty(&imgp);

		fSourcePortSet = FALSE;
	} // end if (source port set to be ignored)

	if (fInitted)
	{
		CleanupIMTest();
		fInitted = FALSE;
	} // end if (initialized driver)

	if (fEnsuredExclusiveUse)
	{
		// Ignore error
		this->RelenquishExclusiveUse();
		fEnsuredExclusiveUse = FALSE;
	} // end if (we took the lock)

	DPL(9, "<== %e", 1, hr);

	return (hr);
#else // ! XBOX
#pragma TODO(tristanj, "No fault simulation supported at this time.")
	return S_OK;
#endif // XBOX
} // CTNFaultSimIMTest::Initialize
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::Release()"
//==================================================================================
// CTNFaultSimIMTest::Release
//----------------------------------------------------------------------------------
//
// Description: Releases this object after a successful call to Initialize.  Must be
//				done prior to deletion.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSimIMTest::Release(void)
{
#ifndef _XBOX // fault simulation not supported at this time
	HRESULT				hr = S_OK;
	//IM_GLOBAL_PROPERTY	imgp;


	DPL(9, "==>", 0);


	hr = ResetGlobalProperties();
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't reset/turn off all properties!", 0);
		goto DONE;
	} // end if (couldn't reset properties)


	CleanupIMTest();
	

	hr = this->RelenquishExclusiveUse();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't relenquish exclusive use!", 0);
		goto DONE;
	} // end if (couldn't relenquish exclusive use)


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
#else // ! XBOX
#pragma TODO(tristanj, "No fault simulation supported at this time.")
	return S_OK;
#endif // XBOX
} // CTNFaultSimIMTest::Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::IsInstalledAndCanBeRun()"
//==================================================================================
// CTNFaultSimIMTest::IsInstalledAndCanBeRun
//----------------------------------------------------------------------------------
//
// Description: Sets up this object, and prepares it for use.
//
// Arguments:
//	LPVOID lpvInitData		Pointer to data to use when initializing.
//	DWORD dwInitDataSize	Size of data to use when initializing.
//
// Returns: TRUE if the fault simulator is installed, FALSE if not.
//==================================================================================
BOOL CTNFaultSimIMTest::IsInstalledAndCanBeRun(void)
{
#ifndef _XBOX // fault simulation not supported at this time
	HRESULT			hr;
	BOOL			fResult = FALSE;
	OSVERSIONINFO	ovi;


	// Check to see if we're on a Windows 2000 platform

	ZeroMemory(&ovi, sizeof (OSVERSIONINFO));
	ovi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	if (! GetVersionEx(&ovi))
		goto DONE;

	if (! (ovi.dwPlatformId & VER_PLATFORM_WIN32_NT))
	{
		DPL(5, "Not Windows 2000 (non-NT OS v%i.%i.%i), IMTest not supported.",
			3, HIBYTE(HIWORD(ovi.dwBuildNumber)),
			LOBYTE(HIWORD(ovi.dwBuildNumber)),
			LOWORD(ovi.dwBuildNumber));
		goto DONE;
	} // end if (not Windows 2000)

	if (ovi.dwMajorVersion < 5)
	{
		DPL(5, "NT based OS, but not Windows 2000 (v%i.%i.%i), IMTest not supported.",
			3, ovi.dwMajorVersion, ovi.dwMinorVersion, ovi.dwBuildNumber);
		goto DONE;
	} // end if (not Windows 2000)


	// Try initializing IMTest.  If it fails, it's probably not installed.

	hr = InitializeIMTest();
	if (hr != ERROR_SUCCESS)
	{
		DPL(3, "Couldn't initialize IMTest, assuming not installed.  %e",
			1, hr);
		goto DONE;
	} // end if (couldn't initialize driver)

	CleanupIMTest();

	DPL(3, "IMTest driver appears to be installed.", 0);
	fResult = TRUE;


DONE:

	return (fResult);
#else // ! XBOX
#pragma TODO(tristanj, "No fault simulation supported at this time.")
	return FALSE;
#endif // XBOX
} // CTNFaultSimIMTest::IsInstalledAndCanBeRun
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::SetBandwidth()"
//==================================================================================
// CTNFaultSimIMTest::SetBandwidth
//----------------------------------------------------------------------------------
//
// Description: Sets the artificial send or receive bandwidth limit for this object.
//				Pass in zero to turn bandwidth limiting off.
//
// Arguments:
//	BOOL fSend					TRUE to limit the send (outgoing) bandwidth, FALSE
//								to limit the receive (incoming) bandwidth.
//	DWORD dwHundredBytesPerSec	Rate to constrict the bandwidth to, or 0 for none.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSimIMTest::SetBandwidth(BOOL fSend, DWORD dwHundredBytesPerSec)
{
#ifndef _XBOX // fault simulation not supported at this time
	HRESULT				hr;
	IM_GLOBAL_PROPERTY	imgp;


	ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
	imgp.ulGlobalProperty = (fSend) ? GI_LINK_SEND_SPEED : GI_LINK_RECEIVE_SPEED;
	imgp.ulFlagEnable = (dwHundredBytesPerSec == 0) ? 0: 1;
	//imgp.ucData = 0;
	imgp.ulValue = dwHundredBytesPerSec;

	hr = SetIMGlobalProperty(&imgp);
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't set IMTest bandwidth limiter!", 0);
		goto DONE;
	} // end if (failed setting IMTest property)


DONE:

	return (hr);
#else // ! XBOX
#pragma TODO(tristanj, "No fault simulation supported at this time.")
	return S_OK;
#endif // XBOX
} // CTNFaultSimIMTest::SetBandwidth
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::SetLatency()"
//==================================================================================
// CTNFaultSimIMTest::SetLatency
//----------------------------------------------------------------------------------
//
// Description: Sets the artificial send or receive latency for this object.
//				Pass in zero to turn artificial latency off.
//
// Arguments:
//	BOOL fSend			TRUE to incur the latency on sends (outgoing data), FALSE to
//						incur the latency on receives (incoming data).
//	DWORD dwMSDelay		Number of milliseconds to increase the latency by, or 0 for
//						none.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSimIMTest::SetLatency(BOOL fSend, DWORD dwMSDelay)
{
#ifndef _XBOX // fault simulation not supported at this time
	HRESULT				hr;
	IM_GLOBAL_PROPERTY	imgp;


	ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
	imgp.ulGlobalProperty = (fSend) ? GI_DELAY_SENDS : GI_DELAY_RECVS;
	imgp.ulFlagEnable = (dwMSDelay == 0) ? 0: 1;
	//imgp.ucData = 0;
	imgp.ulValue = dwMSDelay;

	hr = SetIMGlobalProperty(&imgp);
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't set IMTest latency!", 0);
		goto DONE;
	} // end if (failed setting IMTest property)


DONE:

	return (hr);
#else // ! XBOX
#pragma TODO(tristanj, "No fault simulation supported at this time.")
	return S_OK;
#endif // XBOX
} // CTNFaultSimIMTest::SetLatency
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::SetDropPacketsPercent()"
//==================================================================================
// CTNFaultSimIMTest::SetDropPacketsPercent
//----------------------------------------------------------------------------------
//
// Description: Sets the percentage of sent or received packets which should be
//				randomly dropped.
//				Pass 0 to turn function off.
//
// Arguments:
//	BOOL fSend			TRUE to set the drop percentage for sends (outgoing data),
//						FALSE to set the drop percentage for receives (incoming
//						data).
//	DWORD dwPercent		Percentage of packets to randomly drop, or 0 for none.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSimIMTest::SetDropPacketsPercent(BOOL fSend, DWORD dwPercent)
{
#ifndef _XBOX // fault simulation not supported at this time
	HRESULT				hr;
	IM_GLOBAL_PROPERTY	imgp;


	ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
	imgp.ulGlobalProperty = (fSend) ? GI_SIMULATE_SEND_LOSS_OF_PACKETS : GI_SIMULATE_RECV_LOSS_OF_PACKETS;
	imgp.ulFlagEnable = (dwPercent == 0) ? 0: 1;
	//imgp.ucData = 0;
	imgp.ulValue = dwPercent;

	hr = SetIMGlobalProperty(&imgp);
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't set IMTest latency!", 0);
		goto DONE;
	} // end if (failed setting IMTest property)


DONE:

	return (hr);
#else // ! XBOX
#pragma TODO(tristanj, "No fault simulation supported at this time.")
	return S_OK;
#endif // XBOX
} // CTNFaultSimIMTest::SetDropPacketsPercent
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::Reconnect()"
//==================================================================================
// CTNFaultSimIMTest::Reconnect
//----------------------------------------------------------------------------------
//
// Description: Re-enables the connection.
//
// Arguments:
//	BOOL fSend	Determines whether to set the send/receive for disconnecting
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSimIMTest::Reconnect(BOOL fSend)
{
#ifndef _XBOX // fault simulation not supported at this time
	HRESULT				hr;
	IM_GLOBAL_PROPERTY	imgp;


	ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
	imgp.ulGlobalProperty = (fSend) ? GI_DONT_SEND_PKTS : GI_DONT_RECV_PKTS;
	imgp.ulFlagEnable = 0;
	
	hr = SetIMGlobalProperty(&imgp);
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't set IMTest Reconnect(fSend = %d)!", 1, fSend);
		goto DONE;
	} // end if (failed setting IMTest property)


DONE:

	return (hr);
#else // ! XBOX
#pragma TODO(tristanj, "No fault simulation supported at this time.")
	return S_OK;
#endif // XBOX
} // CTNFaultSimIMTest::Reconnect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSimIMTest::Disconnect()"
//==================================================================================
// CTNFaultSimIMTest::Disconnect
//----------------------------------------------------------------------------------
//
// Description: Terminates the specified connection.
//
// Arguments:
//	BOOL fSend	Determines whether to set the send/receive for disconnecting
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSimIMTest::Disconnect(BOOL fSend)
{
#ifndef _XBOX // fault simulation not supported at this time
	HRESULT				hr;
	IM_GLOBAL_PROPERTY	imgp;


	ZeroMemory(&imgp, sizeof (IM_GLOBAL_PROPERTY));
	imgp.ulGlobalProperty = (fSend) ? GI_DONT_SEND_PKTS : GI_DONT_RECV_PKTS;
	imgp.ulFlagEnable = 1;
	
	hr = SetIMGlobalProperty(&imgp);
	if (hr != ERROR_SUCCESS)
	{
		DPL(0, "Couldn't set IMTest Disconnect(fSend = %d)!", 1, fSend);
		goto DONE;
	} // end if (failed setting IMTest property)


DONE:

	return (hr);
#else // ! XBOX
#pragma TODO(tristanj, "No fault simulation supported at this time.")
	return S_OK;
#endif // XBOX
} // CTNFaultSimIMTest::Disconnect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
