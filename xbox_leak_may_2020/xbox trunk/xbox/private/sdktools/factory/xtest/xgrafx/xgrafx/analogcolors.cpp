// Analogcolors
//
// Get the analog color linearity data from the HDVMU

#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "..\..\statkeys.h"
#include "xgrafx.h"
#include "hdvmudefs.h"


BOOL CXModuleGrafx::HDTVTestGetAnalogColors()
{
	CHostResponse CHR;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost = 0;
	int iError;
	HDVMU_SETUP_INFO setup;
	
#ifdef _DEBUG
	static int dacTestCount;
	dacTestCount++;
	ReportDebug(DEBUG_LEVEL1, _T("HDTV DAC Test - Loop %d"), dacTestCount);
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
	setup.wStartLine = m_wStartLine;
	setup.wNumLines = m_wNumLines;
	setup.wNumSamples = m_wNumSamples;
	setup.hDelay = m_hDelay;
	setup.Interlaced = m_bInterlaced;
		
	ReportDebug(DEBUG_LEVEL1, _T("SetupData: pixelthresh=%hu, hsyncthresh=%hu, vsyncthresh=%hu\n"),
								m_wPixelThreshold, m_wHSyncThreshold, m_wVSyncThreshold);

	ReportDebug(DEBUG_LEVEL1, _T("DAC_Data: StartLine=%hu, NumLines=%hu, Samples=%hu, hDelay=%d, interlaced=%d\n"),
								m_wStartLine, m_wNumLines, m_wNumSamples, m_hDelay, m_bInterlaced);
	
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
		
		/*
		for(int line=0; line < m_wNumLines; line++)
		{
			ReportDebug(DEBUG_LEVEL1, _T("Received DAC Data: Line=%d, GY=%lu, Pr=%lu, Pb=%lu\n"),
										line,
										pColorData[line].green,
										pColorData[line].red,
										pColorData[line].blue);
		}
		*/
	}
	return TRUE;
}


// **************************************************************************************
// **************************************************************************************
//	CXModuleGrafx::HDTVTestCheckAnalogColorsData()
//
//	Abstract:  Samples RGB data for each color according to user defined parameters.
//
// **************************************************************************************
// **************************************************************************************
BOOL CXModuleGrafx::HDTVTestCheckAnalogColorsData()
{
	//double mVscale = m_vmu.getColorMVperBit(); COLOR_MV_PER_BIT (2500.0/4095.0) - from VMU code
	int GYStartLine, BPbStartLine, RPrStartLine;
	int GYEndLine, BPbEndLine, RPrEndLine;
	WORD GYLinearMax, GYLinearMin;
	WORD BPbLinearMax, BPbLinearMin;
	WORD RPrLinearMax, RPrLinearMin;
	WORD measuredValue;
	double expectedValue;
	BOOL b_validLineData = FALSE;
	BOOL okay = FALSE;
	int loop;	
	
	//Init some local variables...
	GYStartLine = BPbStartLine = RPrStartLine = 0;
	GYLinearMax = GYLinearMin = 0;
	BPbLinearMax = BPbLinearMin = 0;
	RPrLinearMax = RPrLinearMin = 0;
	expectedValue = measuredValue = 0;
		
	//Check to see if the parameters from the ini file are valid.
	if(m_wLinearTolerance == -1)
		err_BADPARAMETER( m_modeString, L"dac_tolerance");

	if(m_GY_LowerThreshold == -1)
		err_BADPARAMETER( m_modeString, L"gy_lower");

	if(m_BPb_LowerThreshold == -1)
		err_BADPARAMETER( m_modeString, L"bpb_lower");

	if(m_RPr_LowerThreshold == -1)
		err_BADPARAMETER( m_modeString, L"rpr_lower");

	if(m_GY_UpperThreshold == -1)
		err_BADPARAMETER( m_modeString, L"gy_upper");

	if(m_BPb_UpperThreshold == -1)
		err_BADPARAMETER( m_modeString, L"bpb_upper");
	
	if(m_RPr_UpperThreshold == -1)
		err_BADPARAMETER( m_modeString, L"rpr_upper");
	
	//Attempt to get the data as many times as allowed.
	//Sometimes the count could be bad because of unexpected noise.
	for(int i = 0; i < m_testRetries; i++)
	{
		//Clear the contents of the data struct before each try.
		//memset(ColorData, 0, sizeof(ColorData));

		if(!HDTVTestGetAnalogColors())
		{
			okay = FALSE;
			goto dac_exit;
		}
		//The test image consists of 4 regions.  The topmost region contains lines made of blue with the last
		//10 pixels of each line colored yellow.  This region is to help the DVMU to figure out that it has 
		//left porch space and entered active video.
		//
		//The second region contains data for the G/Y signal.  This is a 128 step gradient shade from
		//full WHITE to BLACK.
		//
		//The third region contains data for the R/Pr signal.  This is a 128 step gradient shade from
		//full RED to CYAN.
		//
		//the fourth region contains data for the B/Pb signal.  This is a 128 step gradient shade from
		//full BLUE to YELLOW.

		//Now search through the lines of data to find where the first region
		//that contains the white data starts.

		GYEndLine = BPbEndLine = RPrEndLine = 0;
		
		for(int lineData=0; lineData < m_wNumLines; lineData++)
		{
			if( pColorData[lineData].green > m_GY_LowerThreshold &&
				pColorData[lineData].green < m_GY_UpperThreshold)
			{
				//The white data was found - proceede...
				b_validLineData = TRUE;
				GYStartLine = lineData;
				GYLinearMax = pColorData[GYStartLine].green;
				GYLinearMin = pColorData[GYStartLine + 127].green;
				
				if(!wcsicmp(m_HDTVmode, L"480p"))
					ReportStatistic(HDTV_480P_Y_DAC_FULLSCALE,_T("%hu"),GYLinearMax);
				else if(!wcsicmp(m_HDTVmode, L"720p"))
					ReportStatistic(HDTV_720P_Y_DAC_FULLSCALE,_T("%hu"),GYLinearMax);
				else
					ReportStatistic(HDTV_1080I_Y_DAC_FULLSCALE,_T("%hu"),GYLinearMax);

				RPrStartLine = lineData + 128;
				RPrLinearMax = pColorData[RPrStartLine].red;
				RPrLinearMin = pColorData[RPrStartLine + 127].red;
				
				if(!wcsicmp(m_HDTVmode, L"480p"))
					ReportStatistic(HDTV_480P_PR_DAC_FULLSCALE,_T("%hu"),RPrLinearMax);
				else if(!wcsicmp(m_HDTVmode, L"720p"))
					ReportStatistic(HDTV_720P_PR_DAC_FULLSCALE,_T("%hu"),RPrLinearMax);
				else
					ReportStatistic(HDTV_1080I_PR_DAC_FULLSCALE,_T("%hu"),RPrLinearMax);
				
				if( RPrLinearMax > m_RPr_UpperThreshold ||
					RPrLinearMax < m_RPr_LowerThreshold)
				{
					//Bad signal - fail.
					//ReportStatistic (_T("xxxykkkjj"), _T("%s"), m_cfgDevice);
					_stprintf(m_errorMsg, _T("RPr FullScale signal out of spec: Meas=%hu, High=%hu, Low=%hu."),
											RPrLinearMax, m_RPr_UpperThreshold, m_RPr_LowerThreshold);
					err_HDVMUBadDAC(m_modeString, m_errorMsg);
					okay = FALSE;
					goto dac_exit;

				}
								
				BPbStartLine = lineData + 256;
				BPbLinearMax = pColorData[BPbStartLine].blue;
				BPbLinearMin = pColorData[BPbStartLine + 127].blue;

				if(!wcsicmp(m_HDTVmode, L"480p"))
					ReportStatistic(HDTV_480P_PB_DAC_FULLSCALE,_T("%hu"),BPbLinearMax);
				else if(!wcsicmp(m_HDTVmode, L"720p"))
					ReportStatistic(HDTV_720P_PB_DAC_FULLSCALE,_T("%hu"),BPbLinearMax);
				else
					ReportStatistic(HDTV_1080I_PB_DAC_FULLSCALE,_T("%hu"),BPbLinearMax);

				//Check to see if the other signals are within expected tolerances.
				if( BPbLinearMax > m_BPb_UpperThreshold ||
					BPbLinearMax < m_BPb_LowerThreshold)
				{
					//Bad signal - fail.
					_stprintf(m_errorMsg, _T("BPb FullScale signal out of spec: Meas=%hu, High=%hu, Low=%hu."),
											BPbLinearMax, m_BPb_UpperThreshold, m_BPb_LowerThreshold);
					err_HDVMUBadDAC(m_modeString, m_errorMsg);
					okay = FALSE;
					goto dac_exit;

				}

				
				//Determine what the actual range of steps are for each color signal.
				//Ideally, there would be 128 steps, but testing has shown that the BPb and RPr signals
				//roll off to zero around 123 steps or so.  Not doing this will introduce an error of at
				//least 40 to 60 dac counts as the lower end is reached.

				//If there are more than 18 end counts that are zero, then there is probably some kind
				//of bias problem.  Report this as an error.
				//Find the end of the GY signal.
				for(loop=128; loop > 100; loop--)
				{	
					if(pColorData[GYStartLine + (loop-1)].green > 0)
					{
						GYEndLine = loop;
						break;
					}
				}

				//Check to see if still zero - this means an "18 count" violation has occurred.
				if(GYEndLine == 0)
				{
					err_HDVMUBadDAC(m_modeString, L"GY Signal has more than 18 counts with ZERO value.");
					okay = FALSE;
					goto dac_exit;
				}
				
				//Find the end of the BPb signal.
				for(loop=128; loop > 100; loop--)
				{	
					if(pColorData[BPbStartLine + (loop-1)].blue > 0)
					{
						BPbEndLine = loop;
						break;
					}
				}

				//Check to see if still zero - this means an "18 count" violation has occurred.
				if(BPbEndLine == 0)
				{
					err_HDVMUBadDAC(m_modeString, L"BPb Signal has more than 18 counts with ZERO value.");
					okay = FALSE;
					goto dac_exit;
				}
				
				//Find the end of the RPr signal.
				for(loop=128; loop > 100; loop--)
				{	
					if(pColorData[RPrStartLine + (loop-1)].red > 0)
					{
						RPrEndLine = loop;
						break;
					}
				}
				
				//Check to see if any are still zero - this means an "18 count" violation has occurred.
				if(RPrEndLine == 0)
				{
					err_HDVMUBadDAC(m_modeString, L"RPr Signal has more than 18 counts with ZERO value.");
					okay = FALSE;
					goto dac_exit;
				}

				//Report out the calculated expected values
				ReportDebug(DEBUG_LEVEL2, L"GYMax=%hu, GYMin=%hu, EndLine=%hu, Step=%.2f",
							GYLinearMax, GYLinearMin, GYEndLine,
							(float)(GYLinearMax - GYLinearMin) / (float)GYEndLine);
				
				ReportDebug(DEBUG_LEVEL2, L"RPrMax=%hu, RPrMin=%hu, EndLine=%hu, Step=%.2f",
							RPrLinearMax, RPrLinearMin, RPrEndLine,
							(float)(RPrLinearMax - RPrLinearMin) / (float)RPrEndLine);
				
				ReportDebug(DEBUG_LEVEL2, L"BPbMax=%hu, BPbMin=%hu, EndLine=%hu, Step=%.2f",
							BPbLinearMax, BPbLinearMin, BPbEndLine,
							(float)(BPbLinearMax - BPbLinearMin) / (float)BPbEndLine);

				break;
			}
		}

		if(!b_validLineData)
		{
			//Error - unable to detect start of white color data!
			err_HDVMUBadDAC(m_modeString, L"Unable to detect valid start of WHITE color data for measurement.");
			okay = FALSE;
			goto dac_exit;
		}

		//The next 384 lines should be the linear transitions for each of the 3 regions.
		//Calc the expected and measured value arrays.
		//Check to see if the value is in spec.
		for(loop=0; loop < GYEndLine; loop++)
		{
			//Check the GY values
			
			expectedValue = (double)GYLinearMax - ( ((double)(GYLinearMax - GYLinearMin) / (float)GYEndLine) * (double)loop);
			measuredValue = pColorData[GYStartLine + loop].green;
			ReportDebug(DEBUG_LEVEL2, L"Line %d, GY Exp=%.2f, Meas=%hu", loop, expectedValue, measuredValue);
			if( (measuredValue > ((WORD)expectedValue + m_wLinearTolerance)) ||
				(measuredValue < ((WORD)expectedValue - m_wLinearTolerance)) )
			{
				//Fail - value out of spec.
				_stprintf(m_errorMsg, _T("GY Linearity signal out of spec: Line=%d, Meas=%hu, High=%hu, Low=%hu."),
											GYStartLine + loop,
											measuredValue,
											(WORD)expectedValue + m_wLinearTolerance,
											(WORD)expectedValue - m_wLinearTolerance);
				err_HDVMUBadDAC(m_modeString, m_errorMsg);
				okay = FALSE;
				goto dac_exit;
			}
			
		}	
			
			
		for(loop=0; loop < BPbEndLine; loop++)
		{
			//Check the BPb values 
			expectedValue = (double)BPbLinearMax - ( ((double)(BPbLinearMax - BPbLinearMin) / (float)BPbEndLine) * (double)loop);
			measuredValue = pColorData[BPbStartLine + loop].blue;
			ReportDebug(DEBUG_LEVEL2, L"Line %d, BPb Exp=%.2f, Meas=%hu", loop, expectedValue, measuredValue);
			if( (measuredValue > ((WORD)expectedValue + m_wLinearTolerance)) ||
				(measuredValue < ((WORD)expectedValue - m_wLinearTolerance)) )
			{
				//Fail - value out of spec.
				_stprintf(m_errorMsg, _T("BPb Linearity signal out of spec: Line=%d, Meas=%hu, High=%hu, Low=%hu."),
											BPbStartLine + loop,
											measuredValue,
											(WORD)expectedValue + m_wLinearTolerance,
											(WORD)expectedValue - m_wLinearTolerance);
				err_HDVMUBadDAC(m_modeString, m_errorMsg);
				okay = FALSE;
				goto dac_exit;
			}
		}

		for(loop=0; loop < RPrEndLine; loop++)
		{
			//Check the Rpr values.
			expectedValue = (double)RPrLinearMax - ( ((double)(RPrLinearMax - RPrLinearMin) / (float)RPrEndLine) * (double)loop);
			measuredValue = pColorData[RPrStartLine + loop].red;
			ReportDebug(DEBUG_LEVEL2, L"Line %d, RPr Exp=%.2f, Meas=%hu", loop, expectedValue, measuredValue);
			if( (measuredValue > ((WORD)expectedValue + m_wLinearTolerance)) ||
				(measuredValue < ((WORD)expectedValue - m_wLinearTolerance)) )
			{
				//Fail - value out of spec.
				_stprintf(m_errorMsg, _T("RPr Linearity signal out of spec: Line=%d, Meas=%hu, High=%hu, Low=%hu."),
											RPrStartLine + loop,
											measuredValue,
											(WORD)expectedValue + m_wLinearTolerance,
											(WORD)expectedValue - m_wLinearTolerance);
				err_HDVMUBadDAC(m_modeString, m_errorMsg);
				okay = FALSE;
				goto dac_exit;
			}
		}
		
		//If we make it out of the above loop, the linearity check has passed for all signals.
		
	}//end for retries
	
	okay = TRUE;

dac_exit:
	if (pColorData) delete pColorData;

	return okay;
}