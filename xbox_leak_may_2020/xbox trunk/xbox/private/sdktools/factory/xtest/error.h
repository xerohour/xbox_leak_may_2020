// error.h

#pragma once

typedef unsigned short XMTA_ERROR;
#define NO_XMTA_ERROR 0

// This class allow us to create a string inside a function call arguement list
// The following code "(LPCTSTR) CGetLastError()" will create an unnamed
// object and pass it to the called function.  When the function returns
// the destructor will be called, allowing us to perform a LocalFree.
//
class CGetLastError
{
	LPVOID m_lpMsgBuf;
	
public:
	CGetLastError ();
	~CGetLastError ();
	operator LPCTSTR () {return (LPCTSTR) m_lpMsgBuf;}
};

//CGetLastError::operator LPCTSTR () 
//CGetLastError::~CGetLastError () 

// This class contains all the error codes that are used by XMTA
// All but a couple of these error codes cause an exception to 
// be thrown.  The reason not all the error codes are throw as
// exceptions is because some error conditions are discovered
// as caught exceptions.
//

class CError
{
private:
	XMTA_ERROR m_lastXmtaError;

public:
	CError ():m_lastXmtaError(0) {}
	~CError () {}

	WORD GetLastXmtaError () {return m_lastXmtaError;} 
	void GetLastWin32Error ();
	WORD Report (WORD code, LPCTSTR format, ...);

public:
	// 0x00 reserved - It means no error

	//------- Codes shared with the tests 0x001 - 0x00f ----------------

	void ReportStructuredExecption (LPCTSTR exp, DWORD code, LPVOID addr)
	{
		Report (0x001, _T("%s (0x%08lX) at 0x%08lX.\n"), exp, code, addr);
	}
	void ReportUnknownException ()
	{
		Report (0x002, _T("Unknown Exception encountered\n")); 
	}
	void ReportTestTimeout (LPCTSTR name, bool isRunning, int maxtimeout, int loadfactor, LPCTSTR footprint)
	{
		Report (0x003, _T("%s is %s, timeout (%d seconds, load factor %d), expired.\nLast CheckAbort called from: %s.\n"), name, isRunning? _T("running"):_T("hung"), maxtimeout, loadfactor, footprint);
	}
	void ThrowConversionFailed (LPCTSTR param, LPCTSTR type)
	{
		throw CXmtaException (0x004, _T("Failed to convert parameter \"%s\" to %s.\n"), param, type); 
	}
	void ThrowParameterValueOutOfRange (LPCTSTR param)
	{
		throw CXmtaException (0x005, _T("Parameter \"%s\" value is out of range.\n"), param); 
	}
	void ThrowNullTerminatorMissing (LPCTSTR name) 
	{
		throw CXmtaException (0x006, _T("Was the NULL terminator omitted in GetChoice(%s)\n"), name);
	}
	void ReportTestThreadWontDie ()
	{
		Report (0x007, _T("Test thread won't die: name unknown.\n"));
	}



	//------- Parsing errors --------------------------------------

	void ThrowInvalidCommand (void)
	{
		throw CXmtaException (0x010, _T("Invalid command"));
	}
	void ThrowUnknownTestName (void)
	{
		throw CXmtaException (0x011, _T("Unknown test name"));
	}
	void ThrowLineTooLong (int limit)
	{
		throw CXmtaException (0x012, _T("Line exceeds length limit of %d\n"), limit);
	}
	void ThrowTokenTooLong (int limit)
	{
		throw CXmtaException (0x013, _T("Token exceeds length limit of %d\n"), limit);
	}
	void ThrowExpectingTestParameter (void)
	{
		throw CXmtaException (0x014, _T("Expecting: <name> = <value>\n"));
	}
	void ThrowExpectingParameter (void)
	{
		throw CXmtaException (0x015, _T("Expecting: <module>.<name> = <value>\n"));
	}
	void ThrowDuplicateParameterFound (LPCTSTR name)
	{
		throw CXmtaException (0x016, _T("Duplicate parameter found: %s\n"), name);
	}
	void ThrowExpectingDot (void)
	{
		throw CXmtaException (0x017, _T("Expecting a dot, as in \"x.y\"\n"));
	}
	void ThrowCommandTakesNoParameters (void)
	{
		throw CXmtaException (0x018, _T("This command does not accept parameters\n"));
	}
	void ThrowUnknownTextFollowingCommand (void)
	{
		throw CXmtaException (0x019, _T("Unexpected text following command\n"));
	}
	void ThrowExpectingName (void)
	{
		throw CXmtaException (0x01a, _T("Expecting name following command\n"));
	}
	void ThrowExpectingParameterValue (LPCTSTR name)
	{
		throw CXmtaException (0x01b, _T("Parameter (%s) missing value after \"=\"\n"), name);
	}
	void ThrowExpectingParameterEnd (LPCTSTR name)
	{
		throw CXmtaException (0x01c, _T("Unexpected text following parameter (%s)\n"), name);
	}
	void ThrowExpectingParameterModule ()
	{
		throw CXmtaException (0x01d, _T("Parameter requires a module qualifier, as in <module>.<name>\n"));
	}
	void ThrowUnexpectedEndStatement ()
	{
		throw CXmtaException (0x01e, _T("Unexpected END, not matched with START/RUN LOOP\n"));
	}

	//------- Others ------------- --------------------------------------

	void ReportFileOpenFailed (LPCTSTR name, LPCTSTR last)
	{
		Report (0x020, _T("CreateFile failed (%s):\n%s"), name, last);
	}
	void ReportReadFileFailed (LPCTSTR name, LPCTSTR last)
	{
		Report (0x021, _T("ReadFile failed: %s\n"), name, last);
	}
	void ReportUnicodeConversionFailed (LPCTSTR name, LPCTSTR last)
	{
		Report (0x022, _T("MultiByteToWideChar failed: %s\n%s\n"), name, last);
	}
	void ThrowTestNotFound (LPCTSTR name) 
	{
		throw CXmtaException (0x023, _T("Test not found: %s\n"), name);
	}
	void ThrowSectionNotFound (LPCTSTR name) 
	{
		throw CXmtaException (0x024, _T("Section \"%s\" not found in INI file\n"), name);
	}
	void ThrowInvalidCommand (LPCTSTR cmd) 
	{
		throw CXmtaException (0x025, _T("Invalid command: %s\n"), cmd);
	}
	void ThrowThreadAppearsHung (LPCTSTR name) 
	{
		throw CXmtaException (0x026, _T("Test appears hung: %s.%s\n"), name);
	}
	void ThrowFailedToStartTestThread (LPCTSTR module, LPCTSTR test) 
	{
		throw CXmtaException (0x027, _T("Failed to start test thread: %s.%s\n"), module, test);
	}
	void ThrowHandleLimitExceeded () 
	{
		throw CXmtaException (0x028, _T("Attempt to run more than 64 tests ignored\n"));
	}
	void ThrowFailedToStartThread (LPCTSTR name, LPCTSTR last) 
	{
		throw CXmtaException (0x029, _T("%s Create thread failed: %s\n"), name, last);
	}
	void err_HostCommunicationError (int i1)
	{
		Report (0x02A, _T("iSendHost communication routine returned an error code of 0x%x"), i1);
	}
	void err_INIBufferIsEmpty ()
	{
		Report (0x02B, _T("The INI file buffer from the host is empty"));
	}
	void err_D3dCreateObjectFailed ()
	{
		Report (0x02C, _T("Failed to create the D3D8 object"));
	}
	void err_D3dCreateDeviceFailed ()
	{
		Report (0x02D, _T("Failed to create the D3D device"));
	}
	void err_SocketDropped ()
	{
		Report (0x02E, _T("Socket was dropped"));
	}
	void err_HostResponseError (DWORD dwErrorCodeFromHost, LPCTSTR s1)
	{
		Report ((unsigned short)dwErrorCodeFromHost, _T("The host responded with the following error message:\n%s"), s1);
	}

};

