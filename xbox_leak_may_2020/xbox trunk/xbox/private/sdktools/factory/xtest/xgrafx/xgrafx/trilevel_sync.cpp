// Trilevel Sync Test
//
// Get the trilevel sync data from the HDVMU

#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "xgrafx.h"
#include "hdvmudefs.h"


BOOL CXModuleGrafx::HDTVTestGetTrilevelSync()
{
	CHostResponse CHR;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost = 0;
	int iError;
	HDVMU_SETUP_INFO setup;
	
#ifdef _DEBUG
	static int syncTestCount;
	syncTestCount++;
	ReportDebug(DEBUG_LEVEL1, _T("HDTV TriLevel Sync Test - Loop %d"), syncTestCount);
#endif

	//Init the contents to zero.
	memset(&setup, 0, sizeof(setup));
	
	//Determine which Standard to use.
	if(!wcsicmp(m_HDTVmode, L"480p"))
	{
		setup.Interlaced = 0;
		if(m_syncMode == 2)
			setup.Mode = (BYTE)HDTV_MODE_NONINVERT_TRILEVEL | ((BYTE)HDTV_STD_480 << 3);
		else
		{
			MainCleanup();
			return err_BADPARAMETER( m_modeString, L"TriLevel Sync Mode (sync_mode)");
		}
	}
	else if(!wcsicmp(m_HDTVmode, L"720p"))
	{
		setup.Interlaced = 0;
		if(m_syncMode == 2)
			setup.Mode = (BYTE)HDTV_MODE_NONINVERT_TRILEVEL | ((BYTE)HDTV_STD_720 << 3);
		else if(m_syncMode == 3)
			setup.Mode = (BYTE)HDTV_MODE_INVERTED_TRILEVEL | ((BYTE)HDTV_STD_720 << 3);
		else
		{
			MainCleanup();
			return err_BADPARAMETER( m_modeString, L"TriLevel Sync Mode (sync_mode)");
		}
	}
	else if(!wcsicmp(m_HDTVmode, L"1080i"))
	{
		setup.Interlaced = 1;
		if(m_syncMode == 2)
			setup.Mode = (BYTE)HDTV_MODE_NONINVERT_TRILEVEL | ((BYTE)HDTV_STD_1080 << 3);
		else if(m_syncMode == 3)
			setup.Mode = (BYTE)HDTV_MODE_INVERTED_TRILEVEL | ((BYTE)HDTV_STD_1080 << 3);
		else
		{
			MainCleanup();
			return err_BADPARAMETER( m_modeString, L"TriLevel Sync Mode (sync_mode)");
		}
	}
	else
	{
		err_BADPARAMETER( m_modeString, L"hdtv_mode" );
		return FALSE;
	}

	//Set the message variables equal to values obtained from ini file.
	setup.wPixelThreshold = m_wPixelThreshold;
	setup.wHSyncThreshold = m_wHSyncThreshold;
	setup.wVSyncThreshold = m_wVSyncThreshold;
	setup.hSyncPolarity = 0;
	setup.vSyncPolarity = 1;
	setup.wStartLine = m_wStartLine;
	setup.wNumLines = m_wNumLines;
	setup.wNumSamples = m_wNumSamples;
	setup.hDelay = m_hDelay;
		
	
	
	ReportDebug(DEBUG_LEVEL1, _T("SetupData: pixelthresh=%hu, hsyncthresh=%hu, vsyncthresh=%hu, hsyncpol=%d, vsyncpol=%d\n"),
								m_wPixelThreshold, m_wHSyncThreshold, m_wVSyncThreshold, setup.hSyncPolarity, setup.vSyncPolarity);
	
	ReportDebug(DEBUG_LEVEL1, _T("DAC_Data: StartLine=%hu, NumLines=%hu, Samples=%hu, hDelay=%d\n"),
								m_wStartLine, m_wNumLines, m_wNumSamples, m_hDelay);
	
	// Use the full version of iSendHost because a response is expected
	if ((iError = g_host.iSendHost(MID_HDVMU_GET_ANALOG_COLORS, (char *)&setup, sizeof(setup), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, 180)) != EVERYTHINGS_OK)
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
		pColorData = (COLOR_SAMPLE *) new COLOR_SAMPLE[m_wNumLines];
		memcpy(pColorData, (COLOR_SAMPLE *)CHR.pvBufferFromHost, (sizeof(COLOR_SAMPLE) * m_wNumLines));
		
		//The trilevel sync test only gets one line of data for each measurement.
		ReportDebug(DEBUG_LEVEL1, _T("Received DAC Data: Line=1, GY=%lu, Pr=%lu, Pb=%lu\n"),
										pColorData[0].green,
										pColorData[0].red,
										pColorData[0].blue);
	}
	return TRUE;
}


// **************************************************************************************
// **************************************************************************************
//	CXModuleGrafx::HDTVTestCheckTrilevelSyncData()
//
//	Abstract:  Samples RGB data for each color according to user defined parameters.
//
// **************************************************************************************
// **************************************************************************************
BOOL CXModuleGrafx::HDTVTestCheckTrilevelSyncData()
{
	WORD wSyncLevel = 0;
	BOOL okay = FALSE;
	
	//Attempt to get the measurement as many times as allowed.
	//Sometimes the value could be bad because of unexpected noise.
	
	//Verify some of the required parameters before proceeding.
	if(m_syncMode < 2 || m_syncMode > 3)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"TriLevel Sync Mode (sync_mode)");
	}
	if(m_syncType == NULL)
	{
		MainCleanup();
		return err_BADPARAMETER( m_modeString, L"TriLevel Sync Type (sync_type)");
	}
	
	for(int i = 0; i < m_testRetries; i++)
	{
		if(!HDTVTestGetTrilevelSync())
		{
			okay = FALSE;
			goto sync_exit;
		}
		
		wSyncLevel = pColorData[0].green;
		ReportDebug(DEBUG_LEVEL1, _T("SyncLevel=%hu\n"), wSyncLevel);

		if((!wcsicmp(m_HDTVmode, L"480p")) && m_syncMode == 2 && (!wcsicmp(m_syncType, L"leading")) )
			ReportStatistic(_T("HDTV_480P_TRILEVEL_SYNC_MODE2_LEADING"),_T("%hu"),wSyncLevel);
		else if((!wcsicmp(m_HDTVmode, L"480p")) && m_syncMode == 2 && (!wcsicmp(m_syncType, L"trailing")) )
			ReportStatistic(_T("HDTV_480P_TRILEVEL_SYNC_MODE2_TRAILING"),_T("%hu"),wSyncLevel);
		else if((!wcsicmp(m_HDTVmode, L"720p")) && m_syncMode == 3 && (!wcsicmp(m_syncType, L"leading")) )
			ReportStatistic(_T("HDTV_720P_TRILEVEL_SYNC_MODE3_LEADING"),_T("%hu"),wSyncLevel);
		else if((!wcsicmp(m_HDTVmode, L"720p")) && m_syncMode == 2 && (!wcsicmp(m_syncType, L"leading")) )
			ReportStatistic(_T("HDTV_720P_TRILEVEL_SYNC_MODE2_LEADING"),_T("%hu"),wSyncLevel);
		else if((!wcsicmp(m_HDTVmode, L"720p")) && m_syncMode == 2 && (!wcsicmp(m_syncType, L"trailing")) )
			ReportStatistic(_T("HDTV_720P_TRILEVEL_SYNC_MODE2_TRAILING"),_T("%hu"),wSyncLevel);
		else if((!wcsicmp(m_HDTVmode, L"1080i")) && m_syncMode == 3 && (!wcsicmp(m_syncType, L"leading")) )
			ReportStatistic(_T("HDTV_1080I_TRILEVEL_SYNC_MODE3_LEADING"),_T("%hu"),wSyncLevel);
		else if((!wcsicmp(m_HDTVmode, L"1080i")) && m_syncMode == 2 && (!wcsicmp(m_syncType, L"leading")) )
			ReportStatistic(_T("HDTV_1080I_TRILEVEL_SYNC_MODE2_LEADING"),_T("%hu"),wSyncLevel);
		else if((!wcsicmp(m_HDTVmode, L"1080i")) && m_syncMode == 2 && (!wcsicmp(m_syncType, L"trailing")) )
			ReportStatistic(_T("HDTV_1080I_TRILEVEL_SYNC_MODE2_TRAILING"),_T("%hu"),wSyncLevel);

		if( wSyncLevel > m_GY_UpperThreshold)
		{
			//Error - out of spec on high range check.
			_stprintf(m_errorMsg, _T("(%s)Exceeds High Spec: Spec=%hu, Measured=%hu\n"), m_syncType, m_GY_UpperThreshold, wSyncLevel);
			err_HDVMUBadTrilevelSync(m_modeString, m_errorMsg);
			goto sync_exit;
		}
		
		if( wSyncLevel < m_GY_LowerThreshold)
		{
			//Error - out of spec on low range check.
			_stprintf(m_errorMsg, _T("(%s)Exceeds Low Spec: Spec=%hu, Measured=%hu\n"), m_syncType, m_GY_LowerThreshold, wSyncLevel);
			err_HDVMUBadTrilevelSync(m_modeString, m_errorMsg);
			goto sync_exit;
		}
		
	}//end for retries
	
	
	okay = TRUE;

sync_exit:
	if (pColorData) delete pColorData;

	return okay;
}