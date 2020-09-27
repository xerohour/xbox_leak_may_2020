// Timings
//
// Get the timing data from the HDVMU

#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "..\..\statkeys.h"
#include "xgrafx.h"
#include "hdvmudefs.h"


BOOL CXModuleGrafx::HDTVTestGetTimings(struct GET_TIMINGS_DATA *pReturnedData)
{
	CHostResponse CHR;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost = 0;
	int iError;
	GET_TIMINGS_DATA *pData = NULL;
	HDVMU_SETUP_INFO setup;

#ifdef _DEBUG
	static int timingsTestCount;
	timingsTestCount++;
	ReportDebug(DEBUG_LEVEL1, _T("HDTV Timings Test - Loop %d"), timingsTestCount);
#endif

	//Init the contents to zero.
	memset(&setup, 0, sizeof(setup));
	
	//Determine which Standard to use.
	if(!wcsicmp(m_HDTVmode, L"480p"))
		setup.Mode = (BYTE)HDTV_MODE_RGB | ((BYTE)HDTV_STD_480 << 3);
	else if(!wcsicmp(m_HDTVmode, L"720p"))
		setup.Mode = (BYTE)HDTV_MODE_RGB | ((BYTE)HDTV_STD_720 << 3);
	else if(!wcsicmp(m_HDTVmode, L"1080i"))
		setup.Mode = (BYTE)HDTV_MODE_RGB | ((BYTE)HDTV_STD_1080 << 3);
	else
	{
		err_BADPARAMETER( m_modeString, L"hdtv_mode" );
		return FALSE;
	}

	//Set the message variables equal to values obtained from ini file.
	setup.wPixelThreshold = m_wPixelThreshold;
	setup.wHSyncThreshold = m_wHSyncThreshold;
	setup.wVSyncThreshold = m_wVSyncThreshold;
		
	ReportDebug(DEBUG_LEVEL1, _T("SetupData: pixelthresh=%lu, hsyncthresh=%lu, vsyncthresh=%lu"),
								m_wPixelThreshold, m_wHSyncThreshold, m_wVSyncThreshold);
	
	// Use the full version of iSendHost because a response is expected
	if ((iError = g_host.iSendHost(MID_HDVMU_GET_TIMINGS, (char *)&setup, sizeof(setup), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, 180)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			if (err_HostResponseError(m_modeString, dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
				return FALSE;
		}
		else
		{
			// Process a communication error
			if (err_HostCommunicationError(m_modeString, iError))
			  return FALSE;
		}
	}
	else if (CHR.pvBufferFromHost == NULL) // Don't access the response buffer if it is NULL
	{
		// The response buffer is NULL - we are expecting a pointer to some data, uh-oh.
		if (err_HostResponseBufferIsNull(m_modeString))
			return FALSE;
	}
	else // The response is not NULL, and is not an error message, so process it
	{
		// Analyze the response data from the host
		pData = (GET_TIMINGS_DATA *)CHR.pvBufferFromHost;
		
		/*
		_stprintf(m_debugMsg, _T("Horiz. Timings 1st Field (clock counts): hsh=%lu, hsl=%lu, hfp=%lu, hbp=%lu\n"),
				pData->TimingData1.hSyncHigh,
				pData->TimingData1.hSyncLow,
				pData->TimingData1.hFrontPorch,
				pData->TimingData1.hBackPorch);
		OutputDebugString( m_debugMsg);
	
		_stprintf(m_debugMsg, _T("Vert. Timings 1st Field (clock counts): vsh=%lu, vsl=%lu, vfp=%lu, vbp=%lu\n"),
				pData->TimingData1.vSyncHigh,
				pData->TimingData1.vSyncLow,
				pData->TimingData1.vFrontPorch,
				pData->TimingData1.vBackPorch);
		OutputDebugString( m_debugMsg);

		_stprintf(m_debugMsg, L"Vert. Timings 2nd Field (clock counts): vfp=%lu, vbp=%lu\n",
				pData->TimingData2.vFrontPorch,
				pData->TimingData2.vBackPorch);
		OutputDebugString( m_debugMsg);
		*/

		memcpy(pReturnedData, pData, sizeof(struct GET_TIMINGS_DATA));

	}
	return TRUE;
}


BOOL CXModuleGrafx::HDTVTestCheckTimingsData()
{
	struct GET_TIMINGS_DATA data;
	BOOL pass;

	
	//Check to see if the spec values are valid.
	if(m_VertFrontPorch1 == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Vertical Front Porch Field1 (vfp1)");
	}
	if(m_VertBackPorch1 == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Vertical Back Porch Field1 (vbp1)");
	}
	if(m_VertFrontPorch2 == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Vertical Front Porch Field2 (vfp2)");
	}
	if(m_VertBackPorch2 == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Vertical Back Porch Field2 (vfp1)");
	}
	if(m_wVertSyncLow == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Vertical Sync Low (vsl)");
	}
	if(m_wVertSyncHigh == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Vertical Sync High (vsh)");
	}
	if(m_wHorizBackPorch == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Horiz Back Porch (hbp)");
	}
	if(m_wHorizFrontPorch == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Horiz Front Porch (hfp)");
	}
	if(m_wHorizSyncLow == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Horiz Sync Low (hsl)");
	}
	if(m_wHorizSyncHigh == -1)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"Horiz Sync High (hsh)");
	}
	

	ReportDebug(DEBUG_LEVEL1, _T("Timing Specs from INI file:\n"));
	ReportDebug(DEBUG_LEVEL1, _T("Horiz. Spec Timings 1st Field (clock counts): hsh=%lu, hsl=%lu, hfp=%lu, hbp=%lu\n"),
				m_wHorizSyncHigh,
				m_wHorizSyncLow,
				m_wHorizFrontPorch,
				m_wHorizBackPorch);

	ReportDebug(DEBUG_LEVEL1, _T("Vert. Spec Timings 1st Field (clock counts): vsh=%lu, vsl=%lu, vfp=%lu, vbp=%lu\n"),
			m_wVertSyncHigh,
			m_wVertSyncLow,
			m_VertFrontPorch1,
			m_VertBackPorch1);
	
	//Attempt to get the timings as many times as allowed.
	//Sometimes the count could be bad because of unexpected noise.
	for(int i = 0; i < m_testRetries; i++)
	{
		//Clear the contents of the data struct before each try.
		memset(&data, 0, sizeof(data));

		if(!HDTVTestGetTimings( &data))
			return FALSE;
		
		if(!wcsicmp(m_HDTVmode, L"480p"))
		{
			ReportStatistic(HDTV_480P_TIMINGS_HSYNC_HIGH,_T("%lu"), data.TimingData1.hSyncHigh);
			ReportStatistic(HDTV_480P_TIMINGS_HSYNC_LOW,_T("%lu"), data.TimingData1.hSyncLow);
			ReportStatistic(HDTV_480P_TIMINGS_HFPORCH,_T("%lu"), data.TimingData1.hFrontPorch);
			ReportStatistic(HDTV_480P_TIMINGS_HBPORCH,_T("%lu"), data.TimingData1.hBackPorch);
	
			ReportStatistic(HDTV_480P_TIMINGS_VSYNC_HIGH,_T("%lu"), data.TimingData1.vSyncHigh);
			ReportStatistic(HDTV_480P_TIMINGS_VSYNC_LOW,_T("%lu"), data.TimingData1.vSyncLow);
			ReportStatistic(HDTV_480P_TIMINGS_VFPORCH,_T("%d"), data.TimingData1.vFrontPorch);
			ReportStatistic(HDTV_480P_TIMINGS_VBPORCH,_T("%d"), data.TimingData1.vBackPorch);
		}
		else if(!wcsicmp(m_HDTVmode, L"720p"))
		{
			ReportStatistic(HDTV_720P_TIMINGS_HSYNC_HIGH,_T("%lu"), data.TimingData1.hSyncHigh);
			ReportStatistic(HDTV_720P_TIMINGS_HSYNC_LOW,_T("%lu"), data.TimingData1.hSyncLow);
			ReportStatistic(HDTV_720P_TIMINGS_HFPORCH,_T("%lu"), data.TimingData1.hFrontPorch);
			ReportStatistic(HDTV_720P_TIMINGS_HBPORCH,_T("%lu"), data.TimingData1.hBackPorch);
	
			ReportStatistic(HDTV_720P_TIMINGS_VSYNC_HIGH,_T("%lu"), data.TimingData1.vSyncHigh);
			ReportStatistic(HDTV_720P_TIMINGS_VSYNC_LOW,_T("%lu"), data.TimingData1.vSyncLow);
			ReportStatistic(HDTV_720P_TIMINGS_VFPORCH,_T("%d"), data.TimingData1.vFrontPorch);
			ReportStatistic(HDTV_720P_TIMINGS_VBPORCH,_T("%d"), data.TimingData1.vBackPorch);
		}
		else
		{
			ReportStatistic(HDTV_1080I_TIMINGS_HSYNC_HIGH,_T("%lu"), data.TimingData1.hSyncHigh);
			ReportStatistic(HDTV_1080I_TIMINGS_HSYNC_LOW,_T("%lu"), data.TimingData1.hSyncLow);
			ReportStatistic(HDTV_1080I_TIMINGS_HFPORCH,_T("%lu"), data.TimingData1.hFrontPorch);
			ReportStatistic(HDTV_1080I_TIMINGS_HBPORCH,_T("%lu"), data.TimingData1.hBackPorch);
	
			ReportStatistic(HDTV_1080I_TIMINGS_VSYNC_HIGH,_T("%lu"), data.TimingData1.vSyncHigh);
			ReportStatistic(HDTV_1080I_TIMINGS_VSYNC_LOW,_T("%lu"), data.TimingData1.vSyncLow);
			ReportStatistic(HDTV_1080I_TIMINGS_VFPORCH,_T("%d"), data.TimingData1.vFrontPorch);
			ReportStatistic(HDTV_1080I_TIMINGS_VBPORCH,_T("%d"), data.TimingData1.vBackPorch);
		}

		pass = HDTVTestCheckMeasuredTimings(FALSE, &data);

		if(pass)
			return TRUE;
	}
	
	//Determine what value was out of spec and report the informtation to the error stream.
	//This data will be determined from the last set of data that was collected.
	HDTVTestCheckMeasuredTimings(TRUE, &data);

	return FALSE;
}


// **************************************************************************************
// **************************************************************************************
// CdvmuGui::CheckMeasuredTimings(struct timings *specs, struct timings *measured)
//
// Abstract:  Pass a struct with the timing specs and a struct with the measured values
//			and compare the contents.
// **************************************************************************************
// **************************************************************************************
BOOL CXModuleGrafx::HDTVTestCheckMeasuredTimings(bool reportError, struct GET_TIMINGS_DATA *data)
{
	
	//Check the Horizontal values.
	if(!HDTVTestCheckTimingResult(reportError, data->TimingData1.hSyncHigh, m_wHorizSyncHigh, m_dwTolerance, L"Horiz Sync High"))
		return FALSE;
	if(!HDTVTestCheckTimingResult(reportError, data->TimingData1.hSyncLow, m_wHorizSyncLow, m_dwTolerance, L"Horiz Sync Low"))
		return FALSE;
	if(!HDTVTestCheckTimingResult(reportError, data->TimingData1.hFrontPorch, m_wHorizFrontPorch, m_dwTolerance, L"Horiz Front Porch"))
		return FALSE;
	if(!HDTVTestCheckTimingResult(reportError, data->TimingData1.hBackPorch, m_wHorizBackPorch, m_dwTolerance, L"Horiz Back Porch"))
		return FALSE;
		
	//vertical values
	if(!HDTVTestCheckTimingResult(reportError, data->TimingData1.vSyncHigh, m_wVertSyncHigh, m_dwTolerance, L"Vert Sync High"))
		return FALSE;
	if(!HDTVTestCheckTimingResult(reportError, data->TimingData1.vSyncLow, m_wVertSyncLow, m_dwTolerance, L"Vert Sync Low"))
		return FALSE;
	if(!HDTVTestCheckTimingResult(reportError, data->TimingData1.vFrontPorch, (WORD)m_VertFrontPorch1, m_dwTolerance, L"Vert Front Proch Field1"))
		return FALSE;
	if(!HDTVTestCheckTimingResult(reportError, data->TimingData1.vBackPorch, (WORD)m_VertBackPorch1, m_dwTolerance, L"Vert Back Porch Field1"))
		return FALSE;
	
	if(m_bInterlaced)
	{
		//Check second field porch values for interlaced video.
		if(!HDTVTestCheckTimingResult(reportError, data->TimingData2.vBackPorch, (WORD)m_VertBackPorch2, m_dwTolerance, L"Vert Back Porch Field2"))
			return FALSE;
		if(!HDTVTestCheckTimingResult(reportError, data->TimingData2.vBackPorch, (WORD)m_VertBackPorch2, m_dwTolerance, L"Vert Back Porch Field2"))
			return FALSE;
	}

	return TRUE;
}



// **************************************************************************************
// **************************************************************************************
// CdvmuGui::CheckTimingResult(unsigned short val, unsigned short ref, unsigned short tol, const char *msg)
//
// Abstract: Evaluate the measured value against the reference value accounting for tolerances.
//
// **************************************************************************************
// **************************************************************************************
BOOL CXModuleGrafx::HDTVTestCheckTimingResult(bool reportError, WORD val, WORD ref, DWORD tol, TCHAR *msg)
{
	
	
	if( (val > (ref+(WORD)tol )) || (val < (ref-(WORD)tol)) )
	{
		if(reportError)
		{
			_stprintf(m_errorMsg, _T("Failed %s test: Expected=%hu, Measured=%hu, Tolerance=%lu"), msg, ref, val, tol);
			err_HDVMUBadTiming(m_modeString, m_errorMsg);
		}
		return FALSE;
	}
	return TRUE;
}