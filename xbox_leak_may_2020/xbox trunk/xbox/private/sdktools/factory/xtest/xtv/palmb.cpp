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



#define Burst_Elems	8

IMPLEMENT_TESTLOCKED (TV, palbcompmb, 17)
{
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=Burst_Elems*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;


	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredPalbCompMb[NUMBER_OF_FREQ];

	videomode=GetCfgInt(L"videomode", -1);

	switch(videomode)
	{
		case 1:
			DrawPalMB();
			PALMBRender();
			break;
		case 3:
			DrawMode3PALMB();
			PALMBRender();
			break;
		case 4:
			DrawMode4PALMB();
			PALMBRender();
			break;
		case 6:
			DrawMode6PALMB();
			PALMBRender();
			break;

		default:
			break;
	}


    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_BDGHI_COMP_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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
		inputString >>m_PalbFreq1 >>m_PalbFreq2	>>m_PalbFreq3 >>m_PalbFreq4
					>>m_PalbFreq5 >>m_PalbFreq6	>>m_PalbFreq7;


		if(m_PalbFreq2>MAXATTENU) MeasuredPalbCompMb[0].dbFreqResponse=m_PalbFreq2;
		else MeasuredPalbCompMb[0].dbFreqResponse=0.0;

		if(m_PalbFreq3>MAXATTENU) MeasuredPalbCompMb[1].dbFreqResponse=m_PalbFreq3;
		else MeasuredPalbCompMb[1].dbFreqResponse=0.0;

		if(m_PalbFreq4>MAXATTENU) MeasuredPalbCompMb[2].dbFreqResponse=m_PalbFreq4;
		else MeasuredPalbCompMb[2].dbFreqResponse=0.0;

		if(m_PalbFreq5>MAXATTENU) MeasuredPalbCompMb[3].dbFreqResponse=m_PalbFreq5;
		else MeasuredPalbCompMb[3].dbFreqResponse=0.0;

		if(m_PalbFreq6>MAXATTENU) MeasuredPalbCompMb[4].dbFreqResponse=m_PalbFreq6;
		else MeasuredPalbCompMb[4].dbFreqResponse=0.0;

		if(m_PalbFreq7>MAXATTENU) MeasuredPalbCompMb[5].dbFreqResponse=m_PalbFreq7;
		else MeasuredPalbCompMb[5].dbFreqResponse=0.0;


		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalbCompMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalbCompMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				bPass=FALSE;
		}
		
	}//end retry loop
	
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"),MeasuredPalbCompMb[0].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredPalbCompMb[1].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredPalbCompMb[2].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredPalbCompMb[3].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredPalbCompMb[4].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredPalbCompMb[5].dbFreqResponse);

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalbCompMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalbCompMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				err_MBurstResultOutOfRange (i+1,MeasuredPalbCompMb[i].dbFreqResponse,dbFreqLimit[i].PalCompLowerLimit,dbFreqLimit[i].PalCompUpperLimit );
		}
	}

	Cleanup();
	return;

}



IMPLEMENT_TESTLOCKED (TV, palbsvmb, 18)
{
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=Burst_Elems*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;	
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;
	
	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredPalbSvMb[NUMBER_OF_FREQ];	
	videomode=GetCfgInt(L"videomode", -1);

	switch(videomode)
	{
		case 1:
			DrawPalMB();
			PALMBRender();
			break;
		case 3:
			DrawMode3PALMB();
			PALMBRender();
			break;
		case 4:
			DrawMode4PALMB();
			PALMBRender();
			break;
		case 6:
			DrawMode6PALMB();
			PALMBRender();
			break;

		default:
			break;
	}

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_BDGHI_SVIDEO_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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

		inputString >>m_PalbFreq1 >>m_PalbFreq2 >>m_PalbFreq3 >>m_PalbFreq4
					>>m_PalbFreq5 >>m_PalbFreq6 >>m_PalbFreq7;

		if(m_PalbFreq2>MAXATTENU) MeasuredPalbSvMb[0].dbFreqResponse=m_PalbFreq2;
		else MeasuredPalbSvMb[0].dbFreqResponse=0.0;

		if(m_PalbFreq3>MAXATTENU) MeasuredPalbSvMb[1].dbFreqResponse=m_PalbFreq3;
		else MeasuredPalbSvMb[1].dbFreqResponse=0.0;

		if(m_PalbFreq4>MAXATTENU) MeasuredPalbSvMb[2].dbFreqResponse=m_PalbFreq4;
		else MeasuredPalbSvMb[2].dbFreqResponse=0.0;

		if(m_PalbFreq5>MAXATTENU) MeasuredPalbSvMb[3].dbFreqResponse=m_PalbFreq5;
		else MeasuredPalbSvMb[3].dbFreqResponse=0.0;

		if(m_PalbFreq6>MAXATTENU) MeasuredPalbSvMb[4].dbFreqResponse=m_PalbFreq6;
		else MeasuredPalbSvMb[4].dbFreqResponse=0.0;

		if(m_PalbFreq7>MAXATTENU) MeasuredPalbSvMb[5].dbFreqResponse=m_PalbFreq7;
		else MeasuredPalbSvMb[5].dbFreqResponse=0.0;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalbSvMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalbSvMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
			break;

	}//end retry loop
	
	ReportStatistic(PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"),MeasuredPalbSvMb[0].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredPalbSvMb[1].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredPalbSvMb[2].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredPalbSvMb[3].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredPalbSvMb[4].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredPalbSvMb[5].dbFreqResponse);

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalbSvMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalbSvMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				err_MBurstResultOutOfRange (i+1,MeasuredPalbSvMb[i].dbFreqResponse,dbFreqLimit[i].PalSvLowerLimit,dbFreqLimit[i].PalCompUpperLimit );
		}
	}
	
	Cleanup();
	return;

}




IMPLEMENT_TESTLOCKED (TV, secamcompmb, 19)
{
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=Burst_Elems*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredPalbCompMb[NUMBER_OF_FREQ];

	DrawPalMB();
    PALMBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_SECAM_COMP_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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


		inputString >>m_PalbFreq1 >>m_PalbFreq2 >>m_PalbFreq3 >>m_PalbFreq4
					>>m_PalbFreq5 >>m_PalbFreq6 >>m_PalbFreq7;

		if(m_PalbFreq2>MAXATTENU) MeasuredPalbCompMb[0].dbFreqResponse=m_PalbFreq2;
		else MeasuredPalbCompMb[0].dbFreqResponse=0.0;

		if(m_PalbFreq3>MAXATTENU) MeasuredPalbCompMb[1].dbFreqResponse=m_PalbFreq3;
		else MeasuredPalbCompMb[1].dbFreqResponse=0.0;

		if(m_PalbFreq4>MAXATTENU) MeasuredPalbCompMb[2].dbFreqResponse=m_PalbFreq4;
		else MeasuredPalbCompMb[2].dbFreqResponse=0.0;

		if(m_PalbFreq5>MAXATTENU) MeasuredPalbCompMb[3].dbFreqResponse=m_PalbFreq5;
		else MeasuredPalbCompMb[3].dbFreqResponse=0.0;

		if(m_PalbFreq6>MAXATTENU) MeasuredPalbCompMb[4].dbFreqResponse=m_PalbFreq6;
		else MeasuredPalbCompMb[4].dbFreqResponse=0.0;

		if(m_PalbFreq7>MAXATTENU) MeasuredPalbCompMb[5].dbFreqResponse=m_PalbFreq7;
		else MeasuredPalbCompMb[5].dbFreqResponse=0.0;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalbCompMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalbCompMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
			break;

	}//end retry loop

	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"),MeasuredPalbCompMb[0].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredPalbCompMb[1].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredPalbCompMb[2].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredPalbCompMb[3].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredPalbCompMb[4].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredPalbCompMb[5].dbFreqResponse);

	
	if(!bPass)
	{
		if((MeasuredPalbCompMb[0].dbFreqResponse==0.0)&&(MeasuredPalbCompMb[1].dbFreqResponse==0.0)&&(MeasuredPalbCompMb[2].dbFreqResponse==0.0)&&(MeasuredPalbCompMb[3].dbFreqResponse==0.0)&&(MeasuredPalbCompMb[4].dbFreqResponse==0.0)&&(MeasuredPalbCompMb[5].dbFreqResponse==0.0))
			err_NoConnection();
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredPalbCompMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalbCompMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredPalbCompMb[i].dbFreqResponse,dbFreqLimit[i].PalCompLowerLimit,dbFreqLimit[i].PalCompUpperLimit );
			}
		}
	}

	Cleanup();
	return;

}




IMPLEMENT_TESTLOCKED (TV, secamsvmb, 20)
{
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=Burst_Elems*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredPalbSvMb[NUMBER_OF_FREQ];

	DrawPalMB();
    PALMBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    	
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_SECAM_SVIDEO_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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

		inputString >>m_PalbFreq1 >>m_PalbFreq2 >>m_PalbFreq3 >>m_PalbFreq4
					>>m_PalbFreq5 >>m_PalbFreq6 >>m_PalbFreq7;

		if(m_PalbFreq2>MAXATTENU) MeasuredPalbSvMb[0].dbFreqResponse=0.0;
		else MeasuredPalbSvMb[0].dbFreqResponse=m_PalbFreq2;

		if(m_PalbFreq3>MAXATTENU) MeasuredPalbSvMb[1].dbFreqResponse=0.0;
		else MeasuredPalbSvMb[1].dbFreqResponse=m_PalbFreq3;

		if(m_PalbFreq4>MAXATTENU) MeasuredPalbSvMb[2].dbFreqResponse=0.0;
		else MeasuredPalbSvMb[2].dbFreqResponse=m_PalbFreq4;

		if(m_PalbFreq5>MAXATTENU) MeasuredPalbSvMb[3].dbFreqResponse=0.0;
		else MeasuredPalbSvMb[3].dbFreqResponse=m_PalbFreq5;

		if(m_PalbFreq6>MAXATTENU) MeasuredPalbSvMb[4].dbFreqResponse=0.0;
		else MeasuredPalbSvMb[4].dbFreqResponse=m_PalbFreq6;

		if(m_PalbFreq7>MAXATTENU) MeasuredPalbSvMb[5].dbFreqResponse=0.0;
		else MeasuredPalbSvMb[5].dbFreqResponse=m_PalbFreq7;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalbSvMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalbSvMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
			break;

	}//end retry loop
	
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"),MeasuredPalbSvMb[0].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredPalbSvMb[1].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredPalbSvMb[2].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredPalbSvMb[3].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredPalbSvMb[4].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredPalbSvMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredPalbSvMb[0].dbFreqResponse==0.0)&&(MeasuredPalbSvMb[1].dbFreqResponse==0.0)&&(MeasuredPalbSvMb[2].dbFreqResponse==0.0)&&(MeasuredPalbSvMb[3].dbFreqResponse==0.0)&&(MeasuredPalbSvMb[4].dbFreqResponse==0.0)&&(MeasuredPalbSvMb[5].dbFreqResponse==0.0))
			err_NoConnection();
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredPalbSvMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalbSvMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredPalbSvMb[i].dbFreqResponse,dbFreqLimit[i].PalSvLowerLimit,dbFreqLimit[i].PalCompUpperLimit );
			}
		}
	}

	Cleanup();
	return;

}




IMPLEMENT_TESTLOCKED (TV, palmcompmb, 21)
{


	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=Burst_Elems*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredPalmCompMb[NUMBER_OF_FREQ];

	DrawPalMB();
	PALMBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
 	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_M_COMP_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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

		inputString >>m_PalmFreq1 >>m_PalmFreq2 >>m_PalmFreq3 >>m_PalmFreq4
					>>m_PalmFreq5 >>m_PalmFreq6 >>m_PalmFreq7;

		if(m_PalbFreq2>MAXATTENU) MeasuredPalmCompMb[0].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[0].dbFreqResponse=m_PalbFreq2;

		if(m_PalbFreq3>MAXATTENU) MeasuredPalmCompMb[1].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[1].dbFreqResponse=m_PalbFreq3;

		if(m_PalbFreq4>MAXATTENU) MeasuredPalmCompMb[2].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[2].dbFreqResponse=m_PalbFreq4;

		if(m_PalbFreq5>MAXATTENU) MeasuredPalmCompMb[3].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[3].dbFreqResponse=m_PalbFreq5;

		if(m_PalbFreq6>MAXATTENU) MeasuredPalmCompMb[4].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[4].dbFreqResponse=m_PalbFreq6;

		if(m_PalbFreq7>MAXATTENU) MeasuredPalmCompMb[5].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[5].dbFreqResponse=m_PalbFreq7;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalmCompMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalmCompMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
			break;

	}//end retry loop

	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"),MeasuredPalmCompMb[0].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredPalmCompMb[1].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredPalmCompMb[2].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredPalmCompMb[3].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredPalmCompMb[4].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredPalmCompMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredPalmCompMb[0].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[1].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[2].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[3].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[4].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[5].dbFreqResponse==0.0))
			err_NoConnection();
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredPalmCompMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalmCompMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredPalmCompMb[i].dbFreqResponse,dbFreqLimit[i].PalCompLowerLimit,dbFreqLimit[i].PalCompUpperLimit );
			}
		}
	}
	
	Cleanup();
	return;

}




IMPLEMENT_TESTLOCKED (TV, palmsvmb, 22)
{

	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=Burst_Elems*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredPalmSvMb[NUMBER_OF_FREQ];

	DrawPalMB();
	PALMBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
 	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_M_SVIDEO_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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
		
		inputString >>m_PalbFreq1 >>m_PalbFreq2 >>m_PalbFreq3 >>m_PalbFreq4
					>>m_PalbFreq5 >>m_PalbFreq6 >>m_PalbFreq7;

		if(m_PalbFreq2>MAXATTENU) MeasuredPalmSvMb[0].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[0].dbFreqResponse=m_PalbFreq2;

		if(m_PalbFreq3>MAXATTENU) MeasuredPalmSvMb[1].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[1].dbFreqResponse=m_PalbFreq3;

		if(m_PalbFreq4>MAXATTENU) MeasuredPalmSvMb[2].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[2].dbFreqResponse=m_PalbFreq4;

		if(m_PalbFreq5>MAXATTENU) MeasuredPalmSvMb[3].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[3].dbFreqResponse=m_PalbFreq5;

		if(m_PalbFreq6>MAXATTENU) MeasuredPalmSvMb[4].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[4].dbFreqResponse=m_PalbFreq6;

		if(m_PalbFreq7>MAXATTENU) MeasuredPalmSvMb[5].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[5].dbFreqResponse=m_PalbFreq7;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalmSvMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalmSvMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
			break;

	}//end retry loop

	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"),MeasuredPalmSvMb[0].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredPalmSvMb[1].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredPalmSvMb[2].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredPalmSvMb[3].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredPalmSvMb[4].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredPalmSvMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredPalmSvMb[0].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[1].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[2].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[3].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[4].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[5].dbFreqResponse==0.0))
			err_NoConnection();
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredPalmSvMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalmSvMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredPalmSvMb[i].dbFreqResponse,dbFreqLimit[i].PalSvLowerLimit,dbFreqLimit[i].PalCompUpperLimit );
			}
		}
	}

	Cleanup();
	return;

}



IMPLEMENT_TESTLOCKED (TV, palncompmb, 23)
{

	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=Burst_Elems*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;
	
	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredPalmCompMb[NUMBER_OF_FREQ];

	DrawPalMB();
    PALMBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_N_COMP_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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

		inputString >>m_PalmFreq1 >>m_PalmFreq2 >>m_PalmFreq3 >>m_PalmFreq4
					>>m_PalmFreq5 >>m_PalmFreq6 >>m_PalmFreq7;

		if(m_PalbFreq2>MAXATTENU) MeasuredPalmCompMb[0].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[0].dbFreqResponse=m_PalbFreq2;

		if(m_PalbFreq3>MAXATTENU) MeasuredPalmCompMb[1].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[1].dbFreqResponse=m_PalbFreq3;

		if(m_PalbFreq4>MAXATTENU) MeasuredPalmCompMb[2].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[2].dbFreqResponse=m_PalbFreq4;

		if(m_PalbFreq5>MAXATTENU) MeasuredPalmCompMb[3].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[3].dbFreqResponse=m_PalbFreq5;

		if(m_PalbFreq6>MAXATTENU) MeasuredPalmCompMb[4].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[4].dbFreqResponse=m_PalbFreq6;

		if(m_PalbFreq7>MAXATTENU) MeasuredPalmCompMb[5].dbFreqResponse=0.0;
		else MeasuredPalmCompMb[5].dbFreqResponse=m_PalbFreq7;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalmCompMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalmCompMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
			break;

	}//end retry loop

	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"),MeasuredPalmCompMb[0].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredPalmCompMb[1].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredPalmCompMb[2].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredPalmCompMb[3].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredPalmCompMb[4].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredPalmCompMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredPalmCompMb[0].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[1].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[2].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[3].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[4].dbFreqResponse==0.0)&&(MeasuredPalmCompMb[5].dbFreqResponse==0.0))
			err_NoConnection();
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredPalmCompMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalmCompMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredPalmCompMb[i].dbFreqResponse,dbFreqLimit[i].PalCompLowerLimit,dbFreqLimit[i].PalCompUpperLimit );
			}
		}
	}

	Cleanup();
	return;
}



IMPLEMENT_TESTLOCKED (TV, palnsvpmb, 24)
{

	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=Burst_Elems*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	MULTIBURST_CONFIG_DATA MeasuredPalmSvMb[NUMBER_OF_FREQ];

	DrawPalMB();
	PALMBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
   
 	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_N_SVIDEO_MB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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

		inputString >>m_PalbFreq1 >>m_PalbFreq2 >>m_PalbFreq3 >>m_PalbFreq4
					>>m_PalbFreq5 >>m_PalbFreq6 >>m_PalbFreq7;

		if(m_PalbFreq2>MAXATTENU) MeasuredPalmSvMb[0].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[0].dbFreqResponse=m_PalbFreq2;

		if(m_PalbFreq3>MAXATTENU) MeasuredPalmSvMb[1].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[1].dbFreqResponse=m_PalbFreq3;

		if(m_PalbFreq4>MAXATTENU) MeasuredPalmSvMb[2].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[2].dbFreqResponse=m_PalbFreq4;

		if(m_PalbFreq5>MAXATTENU) MeasuredPalmSvMb[3].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[3].dbFreqResponse=m_PalbFreq5;

		if(m_PalbFreq6>MAXATTENU) MeasuredPalmSvMb[4].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[4].dbFreqResponse=m_PalbFreq6;

		if(m_PalbFreq7>MAXATTENU) MeasuredPalmSvMb[5].dbFreqResponse=0.0;
		else MeasuredPalmSvMb[5].dbFreqResponse=m_PalbFreq7;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_FREQ;i++)
		{
			if((MeasuredPalmSvMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalmSvMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
			break;

	}//end retry loop

	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET1,_T("%.1f"),MeasuredPalmSvMb[0].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET2,_T("%.1f"), MeasuredPalmSvMb[1].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET3,_T("%.1f"), MeasuredPalmSvMb[2].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET4,_T("%.1f"), MeasuredPalmSvMb[3].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET5,_T("%.1f"), MeasuredPalmSvMb[4].dbFreqResponse);
	ReportStatistic(PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET6,_T("%.1f"), MeasuredPalmSvMb[5].dbFreqResponse);

	if(!bPass)
	{
		if((MeasuredPalmSvMb[0].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[1].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[2].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[3].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[4].dbFreqResponse==0.0)&&(MeasuredPalmSvMb[5].dbFreqResponse==0.0))
			err_NoConnection();
		else
		{
			for(i=0;i<NUMBER_OF_FREQ;i++)
			{
				if((MeasuredPalmSvMb[i].dbFreqResponse<dbFreqLimit[i].PalCompLowerLimit) || (MeasuredPalmSvMb[i].dbFreqResponse>dbFreqLimit[i].PalCompUpperLimit))
					err_MBurstResultOutOfRange (i+1,MeasuredPalmSvMb[i].dbFreqResponse,dbFreqLimit[i].PalSvLowerLimit,dbFreqLimit[i].PalCompUpperLimit );
			}
		}
	}
	
	Cleanup();
	return;

}
