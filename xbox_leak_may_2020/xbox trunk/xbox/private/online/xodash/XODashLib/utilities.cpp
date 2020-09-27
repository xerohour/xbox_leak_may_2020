#include "std.h"

#include "globals.h"

float XAppGetNow()
{
	return *g_fAppTime;
}

void Unicode(wchar_t* wsz, const char* sz, int nMaxChars)
{
	while (nMaxChars-- > 0)
	{
		if ((*wsz++ = (unsigned char)*sz++) == 0)
			return;
	}
}

void Ansi(char* sz, const TCHAR* wsz, int nMaxChars)
{
	while (nMaxChars-- > 0)
	{
		if ((*sz++ = (char)*wsz++) == 0)
			return;
	}
}	

float GetFloat( char** pStr )
{
	float retVal = 0.f;
	char* pTmp = *pStr;
	bool bNumFound = 0;
	char* pStart = *pStr;
	while(true)
	{
		if ( isspace( *pTmp ) )
		{
			if ( bNumFound )
				break;
			
			pTmp++;
		}
		else if ( *pTmp == NULL )
			break;
		else if ( !isdigit( *pTmp ) && *pTmp != '.' && *pTmp != '-' )
			break;
		else
		{
			if ( !bNumFound )
				pStart = pTmp;
			pTmp++;
			bNumFound = true;
		}
	}

	char tmpBuffer[32];
	tmpBuffer[0] = 0;
	strncpy( tmpBuffer, pStart, pTmp - pStart );
	tmpBuffer[pTmp-pStart] = 0;

	retVal = (float)atof( tmpBuffer );

	*pStr = pTmp;

	return retVal;
}
//-------------------------------------------------------------------------------
void GetNameData (char *rawData, char *result)
{
	long startIndex = 0;
	long endIndex = 0;
	while (	(rawData[startIndex] != '"') )
	{
		startIndex++;
	}

	startIndex++;
	endIndex = startIndex;
	while (	(rawData[endIndex] != '"') )
	{
		endIndex++;
	}

	strncpy(result,&rawData[startIndex],endIndex - startIndex);
	result[endIndex-startIndex] = 0;
}


//-------------------------------------------------------------------------------
// Parse Functions
void GetNumberData (char *rawData, char *result)
{
	long startIndex = 0;
	long endIndex = 0;

    if ( NULL == rawData )
    {
        *result = 0;
        return;
    }


	while (	(rawData[startIndex] != '+') &&
			(rawData[startIndex] != '-') &&
			(rawData[startIndex] != '.') &&
			(rawData[startIndex] < '0') ||
			(rawData[startIndex] > '9'))
	{
		startIndex++;
	}

	endIndex = startIndex;
	while (	(rawData[endIndex] == '+') ||
			(rawData[endIndex] == '-') ||
			(rawData[endIndex] == '.') ||
			(rawData[endIndex] >= '0') &&
			(rawData[endIndex] <= '9'))
	{
		endIndex++;
	}

	strncpy(result,&rawData[startIndex],endIndex - startIndex);
	result[endIndex-startIndex] = 0;
}	

//-------------------------------------------------------------------------------
void GetWordData (char *rawData, char *result)
{
	long startIndex = 0;
	long endIndex = 0;
	while (	(rawData[startIndex] < 'A') ||
			(rawData[startIndex] > 'Z') &&
			(rawData[startIndex] < 'a') ||
			(rawData[startIndex] > 'z'))
	{
		startIndex++;
	}

	endIndex = startIndex;
	while (	((rawData[endIndex] >= 'A') && (rawData[endIndex] <= 'Z')) ||
			(rawData[endIndex] == '_') ||
			((rawData[endIndex] >= 'a') && (rawData[endIndex] <= 'z')) )
	{
		endIndex++;
	}

	strncpy(result,&rawData[startIndex],endIndex - startIndex);
	result[endIndex-startIndex] = 0;
}	


extern "C" bool AssertFailed(const TCHAR* szFile, int nLine, HRESULT hr)
{
	TCHAR szBuffer [256];

	if (hr == 0)
	{
		_stprintf(szBuffer, _T("Assertion failed in %s at line %d (last error code is 0x%x)."), szFile, nLine, GetLastError());
	}
	else
	{
	//	TCHAR szComError [100];
	//	XAppGetErrorString(hr, szComError, countof(szComError));
	//	_stprintf(szBuffer, _T("COM Failure in %s at line %d.\n\nFacility: %d %s\nCode %d (0x%04x)\n\n%s"),
	//		szFile, nLine, HRESULT_FACILITY(hr), GetFacilityName(HRESULT_FACILITY(hr)), HRESULT_CODE(hr), HRESULT_CODE(hr), szComError);
	}

#ifdef _XBOX
	DbgPrint("\001XApp: %s\n", szBuffer);
	return true;
#else
//	_tcscat(szBuffer, _T("\n\nDo you want to debug?"));
//	return XAppMessageBox(szBuffer, MB_YESNO) == IDYES;
#endif
}

#ifdef _DEBUG
#define countof(n) (sizeof (n) / sizeof (n[0]))


extern "C" void Trace(char* szMsg, ...)
{
	DbgPrint("[%s][line %d] ",__FILE__, __LINE__);

	va_list args;
	va_start(args, szMsg);

	CHAR szBuffer [512];
	_vsnprintf(szBuffer, 512, szMsg, args);
	DbgPrint("%s\n", szBuffer);
	va_end(args);
}

extern "C" void TraceInFile(char* szMsg, ...)
{
	va_list args;
	va_start(args, szMsg);

	CHAR szBuffer [512];
	_vsnprintf(szBuffer, 512, szMsg, args);
	Trace("%s\n", szBuffer);
	va_end(args);
// #ifdef TRACEFILE
	FILE *stream = fopen("c:\\temp\\temp1.txt", "w");
	if (!stream)
	{
		;
	}
	fprintf(stream,"%s\n",szBuffer);
	fclose(stream);
// #endif

}





extern "C" void Alert(const TCHAR* szMsg, ...)
{
	va_list args;
	va_start(args, szMsg);

	TCHAR szBuffer [512];
	_vsntprintf(szBuffer, 512, szMsg, args);
	DbgPrint("\007%s\n", szBuffer);
	va_end(args);
}


const TCHAR* GetFacilityName(int nFacility)
{
	const TCHAR* szFacility;

	switch (nFacility)
	{
	default:
		szFacility = _T("Unknown Facility");
		break;

	case FACILITY_NULL:
		szFacility = _T("null");
		break;

	case FACILITY_RPC:
		szFacility = _T("RPC");
		break;

	case FACILITY_DISPATCH:
		szFacility = _T("Dispatch");
		break;

	case FACILITY_STORAGE:
		szFacility = _T("Storage");
		break;

	case FACILITY_ITF:
		szFacility = _T("ITF");
		break;

	case FACILITY_WIN32:
		szFacility = _T("Win32");
		break;

	case FACILITY_WINDOWS:
		szFacility = _T("Windows");
		break;

	case FACILITY_SSPI:
		szFacility = _T("SSPI");
		break;

	case FACILITY_CONTROL:
		szFacility = _T("Control");
		break;

	case FACILITY_CERT:
		szFacility = _T("Cert");
		break;

	case FACILITY_INTERNET:
		szFacility = _T("Internet");
		break;

	case FACILITY_MEDIASERVER:
		szFacility = _T("Media Server");
		break;

	case FACILITY_MSMQ:
		szFacility = _T("MSMQ");
		break;

	case FACILITY_SETUPAPI:
		szFacility = _T("Setup API");
		break;

	case 0x15: // _FACDPV
		szFacility = _T("DirectPlayVoice");
		break;

	case 0x876: // _FACDD, _FACD3D
		szFacility = _T("Direct3D");
		break;

	case 0x877: // _FACDP
		szFacility = _T("DirectPlay");
		break;

	case 0x878: // _FACDS
		szFacility = _T("DirectSound");
		break;
	}

	return szFacility;
}
///////////////////////////


/////////////////////////////////////
// Nat Tracing utils
void XAppGetErrorString(HRESULT hr, TCHAR* szErrorBuf, int cchErrorBuf)
{
	int nCode = HRESULT_CODE(hr);
	int nFacility = HRESULT_FACILITY(hr);

	const TCHAR* szFacility = GetFacilityName(nFacility);
	_tcsncpy(szErrorBuf, szFacility, cchErrorBuf);
	int cchFacility = _tcslen(szFacility);
	szErrorBuf += cchFacility;
	cchErrorBuf -= cchFacility;

	if (cchErrorBuf > 2)
	{
		*szErrorBuf++ = ':';
		*szErrorBuf++ = ' ';
		cchErrorBuf -= 2;
	}

#ifdef _LAN
	if (nFacility == FACILITY_INTERNET)
	{
		extern void GetInternetErrorString(int nCode, TCHAR* szBuf, int cchBuf);
		GetInternetErrorString(nCode, szErrorBuf, cchErrorBuf);
		return;
	}
#endif

	if (nFacility == FACILITY_WIN32)
	{
#ifndef _XBOX
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, nCode, 0, szErrorBuf, cchErrorBuf, NULL) == 0)
#endif
			_stprintf(szErrorBuf, _T("Lookup %d in winerror.h!"), nCode);
		return;
	}

	D3DXGetErrorString(hr, szErrorBuf, cchErrorBuf);
}

const TCHAR* XAppGetErrorString(HRESULT hr)
{
	static TCHAR szBuf [100];
	XAppGetErrorString(hr, szBuf, (sizeof (szBuf) / sizeof (szBuf[0]))  );
	return szBuf;
}

void LogComError(HRESULT hr, const char* szFunc/*= NULL*/)
{
	TCHAR szError [100];
	XAppGetErrorString(hr, szError, countof(szError));

	DbgPrint("\001Error in function: %s\n\001%s\n", szFunc == NULL ? "unknown" : szFunc, szError);
}

void LogError(const char* szFunc)
{
	DbgPrint("\001Error in function: %s\n", szFunc);
}




#endif
 