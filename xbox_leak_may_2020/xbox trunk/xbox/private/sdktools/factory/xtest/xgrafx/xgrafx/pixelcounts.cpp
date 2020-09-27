// Pixelcounts
//
// Get the pixel count data from the HDVMU

#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "..\..\statkeys.h"
#include "xgrafx.h"
#include "hdvmudefs.h"


BOOL CXModuleGrafx::HDTVTestGetPixelcounts(struct GET_ANALOG_PIXEL_COUNT_DATA *pReturnedData)
{
	CHostResponse CHR;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost = 0;
	int iError;
	GET_ANALOG_PIXEL_COUNT_DATA *pData = NULL;
	HDVMU_SETUP_INFO setup;

#ifdef _DEBUG
	static int pixelTestCount;
	pixelTestCount++;
	ReportDebug(DEBUG_LEVEL1, _T("HDTV Pixel Count Test - Loop %d"), pixelTestCount);
#endif

	//Init the contents to zero.
	memset(&setup, 0, sizeof(setup));
	
	//Determine which Standard to use.
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
	if ((iError = g_host.iSendHost(MID_HDVMU_GET_PIXELCOUNT, (char *)&setup, sizeof(setup), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, 180)) != EVERYTHINGS_OK)
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
		pData = (GET_ANALOG_PIXEL_COUNT_DATA *)CHR.pvBufferFromHost;
		memcpy(pReturnedData, pData, sizeof(struct GET_ANALOG_PIXEL_COUNT_DATA));
	}
	return TRUE;
}


BOOL CXModuleGrafx::HDTVTestCheckPixelcountData()
{
	struct GET_ANALOG_PIXEL_COUNT_DATA data;
	DWORD gAvg, rAvg, bAvg;
	DWORD diff;
	BOOL pass;
	
	//Init variables.
	gAvg = rAvg = bAvg = 0;
	diff = 0;
	pass = TRUE;
	//Attempt to get the pixel counts as many times as allowed.
	//Sometimes the count could be bad because of unexpected noise.
	for(int i = 0; i < m_testRetries; i++)
	{
		//If we failed the pixelcount test on the first try, it was tested at the 
		//threshold specified in the ini file.  Some systems may have slightly different
		//levels for optimal threshold settings.  If the retries are set to greater than one,
		//then step through the range of valid thresholds to see if we can get a good value.
		//Report out the final threshold level as a stat for data tracking.
		if(i > 0)
		{
			if(i%2)
			{
				//Even value - make it a positive addition to range
				m_wPixelThreshold = m_wPixelThreshold + (WORD)(m_wThresholdRangeStep * i);
			}
			else
			{
				//Odd value - make it a negative addition to range
				m_wPixelThreshold = m_wPixelThreshold - (WORD)(m_wThresholdRangeStep * i);
			}
		}
		
		//Clear the contents of the data struct before each try.
		memset(&data, 0, sizeof(data));

		if(!HDTVTestGetPixelcounts( &data))
			return FALSE;
		
		ReportDebug(DEBUG_LEVEL1, _T("PixelData from CheckData(): Status=%d, G1=%lu, G2=%lu, R1=%lu, R2=%lu, B1=%lu, B2=%lu\n"),
									data.status,
									data.greenPixels[0], data.greenPixels[1],
									data.redPixels[0], data.redPixels[1],
									data.bluePixels[0], data.bluePixels[1]);
				
		//Calc the average values
		gAvg = (data.greenPixels[0] + data.greenPixels[1]) >> 1;
		rAvg = (data.redPixels[0] + data.redPixels[1]) >> 1;
		bAvg = (data.bluePixels[0] + data.bluePixels[1]) >> 1;

		ReportDebug(DEBUG_LEVEL1, _T("Pixel Avgs, Attempt %d: G=%lu, R=%lu, B=%lu, Thresh=%hu\n"),i+1, gAvg, rAvg, bAvg, m_wPixelThreshold);

		pass = TRUE;
		if ((gAvg  == m_dwExpectedGreenCount) && (rAvg == m_dwExpectedRedCount) && (bAvg == m_dwExpectedBlueCount))
			break;//return TRUE;//Couonts exactly as expected - pass and move on.
			
		// Fail if any counts are outside of tolerance
		
		diff = (gAvg > m_dwExpectedGreenCount)? (gAvg - m_dwExpectedGreenCount):(m_dwExpectedGreenCount - gAvg);
		if (diff > m_dwTolerance)
			pass = FALSE;
	
		diff = (rAvg > m_dwExpectedRedCount)? (rAvg - m_dwExpectedRedCount):(m_dwExpectedRedCount - rAvg);
		if (diff > m_dwTolerance)
			pass = FALSE;
		
		diff = (bAvg > m_dwExpectedBlueCount)? (bAvg - m_dwExpectedBlueCount):(m_dwExpectedBlueCount - bAvg);
		if (diff > m_dwTolerance)
			pass = FALSE;

		if(pass)
			break;//return TRUE;
	}
	
	//Report out the final values...
	if(!wcsicmp(m_HDTVmode, L"480p"))
	{
		ReportStatistic(HDTV_480P_G_PIXELCOUNT,_T("%lu"),gAvg);
		ReportStatistic(HDTV_480P_R_PIXELCOUNT,_T("%lu"),rAvg);
		ReportStatistic(HDTV_480P_B_PIXELCOUNT,_T("%lu"),bAvg);
		ReportStatistic(HDTV_480P_PIXEL_THRESHOLD,_T("%hu"),m_wPixelThreshold);
		
	}
	else if(!wcsicmp(m_HDTVmode, L"720p"))
	{
		ReportStatistic(HDTV_720P_G_PIXELCOUNT,_T("%lu"),gAvg);
		ReportStatistic(HDTV_720P_R_PIXELCOUNT,_T("%lu"),rAvg);
		ReportStatistic(HDTV_720P_B_PIXELCOUNT,_T("%lu"),bAvg);
		ReportStatistic(HDTV_720P_PIXEL_THRESHOLD,_T("%hu"),m_wPixelThreshold);
	}
	else
	{
		ReportStatistic(HDTV_1080I_G_PIXELCOUNT,_T("%lu"),gAvg);
		ReportStatistic(HDTV_1080I_R_PIXELCOUNT,_T("%lu"),rAvg);
		ReportStatistic(HDTV_1080I_B_PIXELCOUNT,_T("%lu"),bAvg);
		ReportStatistic(HDTV_1080I_PIXEL_THRESHOLD,_T("%hu"),m_wPixelThreshold);
	}
	
	if(pass)
		return TRUE;
	
	//Determine what value was out of spec and report the informtation to the error stream.
	//This data will be determined from the last set of data that was collected.
	//diff = (gAvg > m_dwExpectedYCount)? (gAvg - m_dwExpectedYCount):(m_dwExpectedYCount - gAvg);
	//diff = (greenAvg[i] > m_dwGreenCount)? (greenAvg[i] - m_dwGreenCount):(m_dwGreenCount - greenAvg[i]);
	//diff = (blueAvg[i] > m_dwBlueCount)? (blueAvg[i] - m_dwBlueCount):(m_dwBlueCount - blueAvg[i]);
	if( (diff = (gAvg > m_dwExpectedGreenCount)? (gAvg - m_dwExpectedGreenCount):(m_dwExpectedGreenCount - gAvg)) > m_dwTolerance)
	{
		_stprintf( m_errorMsg, _T("Failed Green Counts: Expected=%lu, Measured=%lu, Difference=%lu, Tolerance=%lu"),
													m_dwExpectedGreenCount,
													gAvg,
													diff,
													m_dwTolerance);
	}
	else if( (diff = (rAvg > m_dwExpectedRedCount)? (rAvg - m_dwExpectedRedCount):(m_dwExpectedRedCount - rAvg)) > m_dwTolerance)
	{
		_stprintf( m_errorMsg, _T("Failed Red Counts: Expected=%lu, Measured=%lu, Difference=%lu, Tolerance=%lu"),
													m_dwExpectedRedCount,
													rAvg,
													diff,
													m_dwTolerance);
	}
	else if( (diff = (bAvg > m_dwExpectedBlueCount)? (bAvg - m_dwExpectedBlueCount):(m_dwExpectedBlueCount - bAvg)) > m_dwTolerance)
	{
		_stprintf( m_errorMsg, _T("Failed Blue Counts: Expected=%lu, Measured=%lu, Difference=%lu, Tolerance=%lu"),
													m_dwExpectedBlueCount,
													bAvg,
													diff,
													m_dwTolerance);
	}

	err_HDVMUBadPixelCount( m_modeString, m_errorMsg);
	return FALSE;
}
