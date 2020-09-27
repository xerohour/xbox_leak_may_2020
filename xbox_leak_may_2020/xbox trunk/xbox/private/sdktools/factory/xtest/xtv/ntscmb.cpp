// 
// (c) Copyright 2000-2000 Intel Corp. All rights reserved.
//   ntscmb.cpp: used to implement ntsc-m and ntsc-Japan multiburst 
//   tests for composite and s-video output. The subtests include 
//	ntscmcompmb, ntscmsvmb, ntscjcompmb, and ntscjsvmb
//
//////////////////////////////////////////////////////////////////////
#include "..\stdafx.h"
#include <iostream>
#include <string>
#include <sstream>

#include "XTV.h"
#include "..\statkeys.h"


#define EIACb_ELEMS 8


IMPLEMENT_TESTLOCKED (TV, ntscmcompmb, 13)
{

	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=EIACb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredNtscmCompMb[NUMBER_OF_FREQ];

	videomode=GetCfgInt(L"videomode", 1);

	switch(videomode)
	{
		case 1:
			DrawNTSCMB();
			NTSCMBRender();
			break;
		case 3:
			DrawMode3NTSCMB();
			NTSCMBRender();
			break;
		default:
			break;
	}

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	
	//Sleep(1000);
	
	for(int attempts=0; attempts < m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_NTSC_M_COMP_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
				return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				err_HostCommunicationError(iError);
				return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			err_RecordBufferIsEmpty();
			return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			if (dwActualBytesReceived < dwMeasureResultBuffSize)
			{
				err_RecordBufferIsNotCorrectLength(dwMeasureResultBuffSize, dwActualBytesReceived);
				return;
			}
		}

		memcpy(dwMeasureResultBuff, CHR.pvBufferFromHost, dwMeasureResultBuffSize); 

		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);
		
		inputString >> m_NtscmFreq1>>m_NtscmFreq2>>m_NtscmFreq3
					>> m_NtscmFreq4>>m_NtscmFreq5>> m_NtscmFreq6>> m_NtscmFreq7;

		bPass = TRUE;
		if(m_NtscmFreq2>MAXATTENU) MeasuredNtscmCompMb[0].dbFreqResponse=m_NtscmFreq2;
		else MeasuredNtscmCompMb[0].dbFreqResponse=0.0;

		if(m_NtscmFreq3>MAXATTENU) MeasuredNtscmCompMb[1].dbFreqResponse=m_NtscmFreq3;
		else MeasuredNtscmCompMb[1].dbFreqResponse=0.0;

		if(m_NtscmFreq4>MAXATTENU) MeasuredNtscmCompMb[2].dbFreqResponse=m_NtscmFreq4;
		else MeasuredNtscmCompMb[2].dbFreqResponse=0.0;

		if(m_NtscmFreq5>MAXATTENU) MeasuredNtscmCompMb[3].dbFreqResponse=m_NtscmFreq5;
		else MeasuredNtscmCompMb[3].dbFreqResponse=0.0;

		if(m_NtscmFreq6>MAXATTENU) MeasuredNtscmCompMb[4].dbFreqResponse=m_NtscmFreq6;
		else MeasuredNtscmCompMb[4].dbFreqResponse=0.0;

		if(m_NtscmFreq7>MAXATTENU) MeasuredNtscmCompMb[5].dbFreqResponse=m_NtscmFreq7;
		else MeasuredNtscmCompMb[5].dbFreqResponse=0.0;
	
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredNtscmCompMb[i].dbFreqResponse<dbFreqLimit[i].NtscCompLowerLimit) || (MeasuredNtscmCompMb[i].dbFreqResponse>dbFreqLimit[i].NtscCompUpperLimit))
				bPass = FALSE;
		}

		if(bPass)
			break;

	}//end retry loop

	ReportStatistic(NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"), MeasuredNtscmCompMb[0].dbFreqResponse);
	ReportStatistic(NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredNtscmCompMb[1].dbFreqResponse);
	ReportStatistic(NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredNtscmCompMb[2].dbFreqResponse);
	ReportStatistic(NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredNtscmCompMb[3].dbFreqResponse);
	ReportStatistic(NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredNtscmCompMb[4].dbFreqResponse);
	ReportStatistic(NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredNtscmCompMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredNtscmCompMb[0].dbFreqResponse==0.0)&&
					(MeasuredNtscmCompMb[1].dbFreqResponse==0.0)&&
					(MeasuredNtscmCompMb[2].dbFreqResponse==0.0)&&
					(MeasuredNtscmCompMb[3].dbFreqResponse==0.0)&&
					(MeasuredNtscmCompMb[4].dbFreqResponse==0.0)&&
					(MeasuredNtscmCompMb[5].dbFreqResponse==0.0) )
		{
				err_NoConnection();
		}
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredNtscmCompMb[i].dbFreqResponse<dbFreqLimit[i].NtscCompLowerLimit) || (MeasuredNtscmCompMb[i].dbFreqResponse>dbFreqLimit[i].NtscCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredNtscmCompMb[i].dbFreqResponse,dbFreqLimit[i].NtscCompLowerLimit, dbFreqLimit[i].NtscCompUpperLimit );
			}
		}
	}

	Cleanup();
	return;
}



IMPLEMENT_TESTLOCKED (TV, ntscmsvmb, 14)
{
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=EIACb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError, videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredNtscmSvMb[NUMBER_OF_FREQ];

	videomode=GetCfgInt(L"videomode", 1);

	switch(videomode)
	{
		case 1:
			DrawNTSCMB();
			NTSCMBRender();
			break;
		case 3:
			DrawMode3NTSCMB();
			NTSCMBRender();
			break;
		default:
			break;
	}

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
 	for(int attempts=0; attempts < m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_NTSC_M_SVIDEO_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
				return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				err_HostCommunicationError(iError);
				return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			err_RecordBufferIsEmpty();
			return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			if (dwActualBytesReceived < dwMeasureResultBuffSize)
			{
				err_RecordBufferIsNotCorrectLength(dwMeasureResultBuffSize, dwActualBytesReceived);
				return;
			}
		}
	
		memcpy(dwMeasureResultBuff, CHR.pvBufferFromHost, dwMeasureResultBuffSize); 

		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);
		inputString >>m_NtscmFreq1>>m_NtscmFreq2>>m_NtscmFreq3>>m_NtscmFreq4
					>>m_NtscmFreq5>>m_NtscmFreq6>>m_NtscmFreq7;

		if(m_NtscmFreq2>MAXATTENU) MeasuredNtscmSvMb[0].dbFreqResponse=m_NtscmFreq2;
		else MeasuredNtscmSvMb[0].dbFreqResponse=0.0;

		if(m_NtscmFreq3>MAXATTENU) MeasuredNtscmSvMb[1].dbFreqResponse=m_NtscmFreq3;
		else MeasuredNtscmSvMb[1].dbFreqResponse=0.0;

		if(m_NtscmFreq4>MAXATTENU) MeasuredNtscmSvMb[2].dbFreqResponse=m_NtscmFreq4;
		else MeasuredNtscmSvMb[2].dbFreqResponse=0.0;

		if(m_NtscmFreq5>MAXATTENU) MeasuredNtscmSvMb[3].dbFreqResponse=m_NtscmFreq5;
		else MeasuredNtscmSvMb[3].dbFreqResponse=0.0;

		if(m_NtscmFreq6>MAXATTENU) MeasuredNtscmSvMb[4].dbFreqResponse=m_NtscmFreq6;
		else MeasuredNtscmSvMb[4].dbFreqResponse=0.0;

		if(m_NtscmFreq7>MAXATTENU) MeasuredNtscmSvMb[5].dbFreqResponse=m_NtscmFreq7;
		else MeasuredNtscmSvMb[5].dbFreqResponse=0.0;

		bPass = TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredNtscmSvMb[i].dbFreqResponse<dbFreqLimit[i].NtscCompLowerLimit) || (MeasuredNtscmSvMb[i].dbFreqResponse>dbFreqLimit[i].NtscCompUpperLimit))
				bPass = FALSE;
		}

		if(bPass)
			break;

	}//end retry loop
	
	ReportStatistic(NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"), MeasuredNtscmSvMb[0].dbFreqResponse);
	ReportStatistic(NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredNtscmSvMb[1].dbFreqResponse);
	ReportStatistic(NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredNtscmSvMb[2].dbFreqResponse);
	ReportStatistic(NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredNtscmSvMb[3].dbFreqResponse);
	ReportStatistic(NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredNtscmSvMb[4].dbFreqResponse);
	ReportStatistic(NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredNtscmSvMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredNtscmSvMb[0].dbFreqResponse==0.0)&&(MeasuredNtscmSvMb[1].dbFreqResponse==0.0)&&(MeasuredNtscmSvMb[2].dbFreqResponse==0.0)&&(MeasuredNtscmSvMb[3].dbFreqResponse==0.0)&&(MeasuredNtscmSvMb[4].dbFreqResponse==0.0)&&(MeasuredNtscmSvMb[5].dbFreqResponse==0.0))
		   err_NoConnection();
		
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredNtscmSvMb[i].dbFreqResponse<dbFreqLimit[i].NtscCompLowerLimit) || (MeasuredNtscmSvMb[i].dbFreqResponse>dbFreqLimit[i].NtscCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredNtscmSvMb[i].dbFreqResponse,dbFreqLimit[i].NtscCompLowerLimit, dbFreqLimit[i].NtscCompUpperLimit );
			}
		}
	}

	Cleanup();
	return;
}



IMPLEMENT_TESTLOCKED (TV, ntscjcompmb, 15)
{
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=EIACb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredNtscjCompMb[NUMBER_OF_FREQ];

	videomode=GetCfgInt(L"videomode", 1);

	switch(videomode)
	{
		case 1:
			DrawNTSCMB();
			NTSCMBRender();
			break;
		case 3:
			DrawMode3NTSCMB();
			NTSCMBRender();
			break;
		default:
			break;
	}


    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
 	
	for(int attempts=0; attempts < m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_NTSC_JAPAN_COMP_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
				return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				err_HostCommunicationError(iError);
				return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			err_RecordBufferIsEmpty();
			return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			if (dwActualBytesReceived < dwMeasureResultBuffSize)
			{
				err_RecordBufferIsNotCorrectLength(dwMeasureResultBuffSize, dwActualBytesReceived);
				return;
			}
		}
		memcpy(dwMeasureResultBuff, CHR.pvBufferFromHost, dwMeasureResultBuffSize); 

		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);
		inputString >>m_NtscjFreq1>>m_NtscjFreq2>>m_NtscjFreq3>>m_NtscjFreq4
					>>m_NtscjFreq5>>m_NtscjFreq6>>m_NtscjFreq7;

		if(m_NtscjFreq2>MAXATTENU) MeasuredNtscjCompMb[0].dbFreqResponse=m_NtscjFreq2;
		else MeasuredNtscjCompMb[0].dbFreqResponse=0.0;

		if(m_NtscjFreq3>MAXATTENU) MeasuredNtscjCompMb[1].dbFreqResponse=m_NtscjFreq3;
		else MeasuredNtscjCompMb[1].dbFreqResponse=0.0;

		if(m_NtscjFreq4>MAXATTENU) MeasuredNtscjCompMb[2].dbFreqResponse=m_NtscjFreq4;
		else MeasuredNtscjCompMb[2].dbFreqResponse=0.0;

		if(m_NtscjFreq5>MAXATTENU) MeasuredNtscjCompMb[3].dbFreqResponse=m_NtscjFreq5;
		else MeasuredNtscjCompMb[3].dbFreqResponse=0.0;

		if(m_NtscjFreq6>MAXATTENU) MeasuredNtscjCompMb[4].dbFreqResponse=m_NtscjFreq6;
		else MeasuredNtscjCompMb[4].dbFreqResponse=0.0;

		if(m_NtscjFreq7>MAXATTENU) MeasuredNtscjCompMb[5].dbFreqResponse=m_NtscjFreq7;
		else MeasuredNtscjCompMb[5].dbFreqResponse=0.0;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredNtscjCompMb[i].dbFreqResponse<dbFreqLimit[i].NtscCompLowerLimit) || (MeasuredNtscjCompMb[i].dbFreqResponse>dbFreqLimit[i].NtscCompUpperLimit))
				bPass = FALSE;
		}

		if(bPass)
			break;
	}//end retry loop

	ReportStatistic(NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"), MeasuredNtscjCompMb[0].dbFreqResponse);
	ReportStatistic(NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredNtscjCompMb[1].dbFreqResponse);
	ReportStatistic(NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredNtscjCompMb[2].dbFreqResponse);
	ReportStatistic(NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredNtscjCompMb[3].dbFreqResponse);
	ReportStatistic(NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredNtscjCompMb[4].dbFreqResponse);
	ReportStatistic(NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredNtscjCompMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredNtscjCompMb[0].dbFreqResponse==0.0)&&(MeasuredNtscjCompMb[1].dbFreqResponse==0.0)&&(MeasuredNtscjCompMb[2].dbFreqResponse==0.0)&&(MeasuredNtscjCompMb[3].dbFreqResponse==0.0)&&(MeasuredNtscjCompMb[4].dbFreqResponse==0.0)&&(MeasuredNtscjCompMb[5].dbFreqResponse==0.0))
			err_NoConnection();
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredNtscjCompMb[i].dbFreqResponse<dbFreqLimit[i].NtscCompLowerLimit) || (MeasuredNtscjCompMb[i].dbFreqResponse>dbFreqLimit[i].NtscCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredNtscjCompMb[i].dbFreqResponse,dbFreqLimit[i].NtscCompLowerLimit, dbFreqLimit[i].NtscCompUpperLimit );
			}
		}
	}

	Cleanup();
	return;
}



IMPLEMENT_TESTLOCKED (TV, ntscjsvmb, 16)
{
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=EIACb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredNtscjSvMb[NUMBER_OF_FREQ];

	videomode=GetCfgInt(L"videomode", 1);

	switch(videomode)
	{
		case 1:
			DrawNTSCMB();
			NTSCMBRender();
			break;
		case 3:
			DrawMode3NTSCMB();
			NTSCMBRender();
			break;
		default:
			break;
	}

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
 	for(int attempts=0; attempts < m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_NTSC_JAPAN_SVIDEO_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
				return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				err_HostCommunicationError(iError);
				return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			err_RecordBufferIsEmpty();
			return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			if (dwActualBytesReceived < dwMeasureResultBuffSize)
			{
				err_RecordBufferIsNotCorrectLength(dwMeasureResultBuffSize, dwActualBytesReceived);
				return;
			}
		}

		memcpy(dwMeasureResultBuff, CHR.pvBufferFromHost, dwMeasureResultBuffSize); 

		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);
		inputString >>m_NtscjFreq1>>m_NtscjFreq2>>m_NtscjFreq3>>m_NtscjFreq4
					>>m_NtscjFreq5>>m_NtscjFreq6>>m_NtscjFreq7;

		if(m_NtscjFreq2>MAXATTENU) MeasuredNtscjSvMb[0].dbFreqResponse=m_NtscjFreq2;
		else MeasuredNtscjSvMb[0].dbFreqResponse=0.0;

		if(m_NtscjFreq3>MAXATTENU) MeasuredNtscjSvMb[1].dbFreqResponse=m_NtscjFreq3;
		else MeasuredNtscjSvMb[1].dbFreqResponse=0.0;

		if(m_NtscjFreq4>MAXATTENU) MeasuredNtscjSvMb[2].dbFreqResponse=m_NtscjFreq4;
		else MeasuredNtscjSvMb[2].dbFreqResponse=0.0;

		if(m_NtscjFreq5>MAXATTENU) MeasuredNtscjSvMb[3].dbFreqResponse=m_NtscjFreq5;
		else MeasuredNtscjSvMb[3].dbFreqResponse=0.0;

		if(m_NtscjFreq6>MAXATTENU) MeasuredNtscjSvMb[4].dbFreqResponse=m_NtscjFreq6;
		else MeasuredNtscjSvMb[4].dbFreqResponse=0.0;

		if(m_NtscjFreq7>MAXATTENU) MeasuredNtscjSvMb[5].dbFreqResponse=m_NtscjFreq7;
		else MeasuredNtscjSvMb[5].dbFreqResponse=0.0;

		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredNtscjSvMb[i].dbFreqResponse<dbFreqLimit[i].NtscCompLowerLimit) || (MeasuredNtscjSvMb[i].dbFreqResponse>dbFreqLimit[i].NtscCompUpperLimit))
				bPass=FALSE;
		}
		
		if(bPass)
			break;

	}//end retry loop

	ReportStatistic(NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"), MeasuredNtscjSvMb[0].dbFreqResponse);
	ReportStatistic(NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredNtscjSvMb[1].dbFreqResponse);
	ReportStatistic(NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredNtscjSvMb[2].dbFreqResponse);
	ReportStatistic(NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredNtscjSvMb[3].dbFreqResponse);
	ReportStatistic(NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredNtscjSvMb[4].dbFreqResponse);
	ReportStatistic(NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredNtscjSvMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredNtscjSvMb[0].dbFreqResponse==0.0)&&(MeasuredNtscjSvMb[1].dbFreqResponse==0.0)&&(MeasuredNtscjSvMb[2].dbFreqResponse==0.0)&&(MeasuredNtscjSvMb[3].dbFreqResponse==0.0)&&(MeasuredNtscjSvMb[4].dbFreqResponse==0.0)&&(MeasuredNtscjSvMb[5].dbFreqResponse==0.0))
			err_NoConnection();
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredNtscjSvMb[i].dbFreqResponse<dbFreqLimit[i].NtscCompLowerLimit) || (MeasuredNtscjSvMb[i].dbFreqResponse>dbFreqLimit[i].NtscCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredNtscjSvMb[i].dbFreqResponse,dbFreqLimit[i].NtscCompLowerLimit, dbFreqLimit[i].NtscCompUpperLimit );
			}
		}
	}

	Cleanup();
	return;
}

