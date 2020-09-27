// 
// (c) Copyright 2000-2000 Intel Corp. All rights reserved.
//   ntsccb.cpp: used to implement ntsc-m and ntsc-Japan color bars 
//   tests for composite and s-video output. The subtests include 
//	ntscmcompcb, ntscmsvcb, ntscjcompcb, and ntscjsvcb
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <wchar.h>
#include <iostream>
#include <string>
#include <sstream>
#include "..\stdafx.h"
#include "..\statkeys.h"
#include "XTV.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_NORMAL|D3DFVF_DIFFUSE)
#define MAX_VERTS 640
#define EIACb_ELEMS	22

//ntsc-m TV standard with Composite and s-video output sourse for Color Bars test
IMPLEMENT_TESTLOCKED (TV, ntscmcompcb, 1)
{
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=EIACb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	BOOL bPass = TRUE;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError,videomode;
	dwBufferToHostLength=NULL;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	//Init arrays to hold response data.
	NTSCM_COMP_CB_DATA MeasuredNtscmCompCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredNtscmCompCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredNtscmCompCbChromaPhase[NUMBER_OF_CBARS];
	
	videomode=GetCfgInt(L"videomode", 1);
	switch(videomode)
	{
		case 1:
			DrawNTSCCB();
			NTSCCBRender();
			break;
		case 3:
			DrawMode3NTSCCB();
			NTSCCBRender();
			break;


		default:
			break;
	}
    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	
	//Outer retry loop.
	//Sometimes the capture happens before the image is ready ready for processing, resulting in a false failure.
	//Retry up to the config value number of times to help prevent false failures.
	
	for(int attempts=0; attempts < m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_NTSC_M_COMP_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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

		// using string input to extract the datas in the measureresultbuff
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >>m_LumaIREWhite		>>m_LumaIREYellow 
					>>m_LumaIRECyan			>>m_LumaIREGreen
					>>m_LumaIREMagenta 		>>m_LumaIRERed 
					>>m_LumaIREBlue 		>>m_LumaIREBlack
					>>m_ChromaIREWhite		>>m_ChromaIREYellow 
					>>m_ChromaIRECyan 		>>m_ChromaIREGreen
					>>m_ChromaIREMagenta	>>m_ChromaIRERed 
					>>m_ChromaIREBlue 		>>m_ChromaIREBlack
					//>> m_ChromaPhaseWhite
					>>m_ChromaPhaseYellow	>>m_ChromaPhaseCyan 
					>>m_ChromaPhaseGreen	>>m_ChromaPhaseMagenta 
					>>m_ChromaPhaseRed		>>m_ChromaPhaseBlue ;
					//>> m_ChromaPhaseBlack;

		MeasuredNtscmCompCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredNtscmCompCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredNtscmCompCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredNtscmCompCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredNtscmCompCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredNtscmCompCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredNtscmCompCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredNtscmCompCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredNtscmCompCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredNtscmCompCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredNtscmCompCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredNtscmCompCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredNtscmCompCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredNtscmCompCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredNtscmCompCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredNtscmCompCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredNtscmCompCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredNtscmCompCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredNtscmCompCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredNtscmCompCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredNtscmCompCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredNtscmCompCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		/*for (i=0;i<NUMBER_OF_CBARS;i++)
			ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR,_T("LumaIRE[%lu]: %.1f"),i, MeasuredNtscmCompCbLumaIRE[i].dblLumaIRE);

		for (i=0;i<NUMBER_OF_CBARS;i++)
		ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR, _T("ChromaIRE[%lu] %.1f"),i, MeasuredNtscmCompCbChromaIRE[i].dblChromaIRE);

		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR, _T("ChromaPhase[%lu]  %.1f"),i, MeasuredNtscmCompCbChromaPhase[i].dblChromaPhase);
		*/
		//for (i=0;i<NUMBER_OF_CBARS;i++)
		//ReportWarning(L"%.1f, %lu, %lu, %lu",MeasuredNtscmCompCbLumaIRE[i].dblLumaIRE,dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit);

		bPass = TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscmCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit)||(MeasuredNtscmCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit))
				bPass = FALSE;
		}

		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredNtscmCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit)||(MeasuredNtscmCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit))
					bPass = FALSE;
			}
		}

		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredNtscmCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit)||(MeasuredNtscmCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit))
	 				bPass = FALSE;
			}
		}
		
		if(bPass)
			break;

	}//end retry loop

	//Now that we have the real data, report out the stats.
	//Report Statistic data of NTSC-M Composite color bar for Luma IRE:
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_WHITE, _T("%.1f"),m_LumaIREWhite);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_YELLOW, _T("%.1f"),m_LumaIREYellow);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_CYAN, _T("%.1f"),m_LumaIRECyan);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_GREEN, _T("%.1f"),m_LumaIREGreen);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_MAGENTA, _T("%.1f"),m_LumaIREMagenta);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_RED, _T("%.1f"),m_LumaIRERed);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_BLUE, _T("%.1f"),m_LumaIREBlue);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_BLACK, _T("%.1f"),m_LumaIREBlack);

	//Report Statistic data of NTSC-M Composite color bar for Chroma IRE:
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_YELLOW, _T("%.1f"),m_ChromaIREYellow);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_CYAN, _T("%.1f"),m_ChromaIRECyan);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_GREEN, _T("%.1f"),m_ChromaIREGreen);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_MAGENTA, _T("%.1f"),m_ChromaIREMagenta);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_RED, _T("%.1f"),m_ChromaIRERed);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLUE, _T("%.1f"),m_ChromaIREBlue);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);


	//Report Statistic data of NTSC-M Composite color bar for Chroma Phase:
	//ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_YELLOW, _T("%.1f"),m_ChromaPhaseYellow);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_CYAN, _T("%.1f"),m_ChromaPhaseCyan);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_GREEN, _T("%.1f"),m_ChromaPhaseGreen);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_MAGENTA, _T("%.1f"),m_ChromaPhaseMagenta);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_RED, _T("%.1f"),m_ChromaPhaseRed);
	ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLUE, _T("%.1f"),m_ChromaPhaseBlue);
	//ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	//If the test failed, sift through the data again to retrieve the failed value and report the error.
	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscmCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit)||(MeasuredNtscmCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit))
				err_LumaResultOutOfRange(i,MeasuredNtscmCompCbLumaIRE[i].dblLumaIRE, dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE ,gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit);
		}

		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscmCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit)||(MeasuredNtscmCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredNtscmCompCbChromaIRE[i].dblChromaIRE, dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE, gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit);
		}

		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredNtscmCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit)||(MeasuredNtscmCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit))
	 			err_ChromaPhaseResultOutOfRange(i,MeasuredNtscmCompCbChromaPhase[i].dblChromaPhase, 	dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit);
		}	
	}
	
	Cleanup();

	return;
}



IMPLEMENT_TESTLOCKED (TV, ntscmsvcb, 2)
{

	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EIACb_ELEMS*2*sizeof(DWORD);;
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i=0;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;
	
	if (CheckAbort (HERE)){ Cleanup(); return;};
	
	//Init the arrays to hold the measured data.
	NTSCM_SV_CB_DATA MeasuredNtscmSvCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_SV_CB_DATA MeasuredNtscmSvCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_SV_CB_DATA MeasuredNtscmSvCbChromaPhase[NUMBER_OF_CBARS];	

	videomode=GetCfgInt(L"videomode", 1);
	switch(videomode)
	{
		case 1:
			DrawNTSCCB();
			NTSCCBRender();
			break;
		case 3:
			DrawMode3NTSCCB();
			NTSCCBRender();
			break;
		default:
			break;
	}
	

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	//************************ LOOK HERE IF FAILING  ****************************
	//Why is this 5 second sleep here?
	//Sleep(5000);
	
	for(int attempts=0; attempts < m_testRetries; attempts++)
	{
		// send message to host
		if ((iError = g_host.iSendHost(MID_NTSC_M_SVIDEO_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if (CHR.pvBufferFromHost == NULL) // Don't access the response buffer if it is NULL
		{
			// The response buffer is NULL, so take appropriate action (like an error message)
			if (err_EchoBufferResponseIsNull())
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			if (dwActualBytesReceived < dwMeasureResultBuffSize)
			{
				err_RecordBufferIsNotCorrectLength(dwMeasureResultBuffSize, dwActualBytesReceived);
				return;
			}
			// We received the measure resutls buffer the size that we wanted
		}

		memcpy(dwMeasureResultBuff, CHR.pvBufferFromHost, dwMeasureResultBuffSize); 

		// using string input to extract the datas
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >>m_LumaIREWhite		>>m_LumaIREYellow
					>>m_LumaIRECyan			>>m_LumaIREGreen
					>>m_LumaIREMagenta		>>m_LumaIRERed
					>>m_LumaIREBlue			>>m_LumaIREBlack
					>>m_ChromaIREWhite		>>m_ChromaIREYellow
					>>m_ChromaIRECyan		>>m_ChromaIREGreen
					>>m_ChromaIREMagenta	>>m_ChromaIRERed 
					>>m_ChromaIREBlue		>>m_ChromaIREBlack
					>>m_ChromaPhaseYellow	>>m_ChromaPhaseCyan
					>>m_ChromaPhaseGreen	>>m_ChromaPhaseMagenta
					>>m_ChromaPhaseRed		>>m_ChromaPhaseBlue;
		//		>> m_ChromaPhaseBlack;

		MeasuredNtscmSvCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredNtscmSvCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredNtscmSvCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredNtscmSvCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredNtscmSvCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredNtscmSvCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredNtscmSvCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredNtscmSvCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredNtscmSvCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredNtscmSvCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredNtscmSvCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredNtscmSvCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredNtscmSvCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredNtscmSvCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredNtscmSvCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredNtscmSvCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredNtscmSvCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredNtscmSvCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredNtscmSvCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredNtscmSvCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredNtscmSvCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredNtscmSvCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass = TRUE;
		for (i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscmSvCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit)||(MeasuredNtscmSvCbLumaIRE[i].dblLumaIRE >=gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit))
				bPass = FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredNtscmSvCbChromaIRE[i].dblChromaIRE <gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit)||(MeasuredNtscmSvCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit))
					bPass = FALSE;	
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredNtscmSvCbChromaPhase[i].dblChromaPhase <	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit)||(MeasuredNtscmSvCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit))
					bPass = FALSE;
			}
		}
		if(bPass)
			break;

	}//end retry loop

	//Now that we have the real data, report out the stats.
	//Report Statistic data of NTSC-M Svideo color bar for Luma IRE:
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_WHITE, _T("%.1f"),m_LumaIREWhite);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_YELLOW, _T("%.1f"),m_LumaIREYellow);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_CYAN, _T("%.1f"),m_LumaIRECyan);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_GREEN, _T("%.1f"),m_LumaIREGreen);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_MAGENTA, _T("%.1f"),m_LumaIREMagenta);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_RED, _T("%.1f"),m_LumaIRERed);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_BLUE, _T("%.1f"),m_LumaIREBlue);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_BLACK, _T("%.1f"),m_LumaIREBlack);

	//Report Statistic data of NTSC-M Svideo color bar for Chroma IRE:
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_YELLOW, _T("%.1f"),m_ChromaIREYellow);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_CYAN, _T("%.1f"),m_ChromaIRECyan);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_GREEN, _T("%.1f"),m_ChromaIREGreen);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_MAGENTA, _T("%.1f"),m_ChromaIREMagenta);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_RED, _T("%.1f"),m_ChromaIRERed);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_BLUE, _T("%.1f"),m_ChromaIREBlue);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	//Report Statistic data of NTSC-M Svideo color bar for Chroma Phase:
	//ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_YELLOW, _T("%.1f"),m_ChromaPhaseYellow);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_CYAN, _T("%.1f"),m_ChromaPhaseCyan);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_GREEN, _T("%.1f"),m_ChromaPhaseGreen);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_MAGENTA, _T("%.1f"),m_ChromaPhaseMagenta);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_RED, _T("%.1f"),m_ChromaPhaseRed);
	ReportStatistic(NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLUE, _T("%.1f"),m_ChromaPhaseBlue);
	//ReportStatistic(NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	if(!bPass)
	{
		for (i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscmSvCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit)||(MeasuredNtscmSvCbLumaIRE[i].dblLumaIRE >=gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit))
				err_LumaResultOutOfRange(i, MeasuredNtscmSvCbLumaIRE[i].dblLumaIRE,dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit);
		}

		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscmSvCbChromaIRE[i].dblChromaIRE <gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit)||(MeasuredNtscmSvCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i, MeasuredNtscmSvCbChromaIRE[i].dblChromaIRE, dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit);	
		}

		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredNtscmSvCbChromaPhase[i].dblChromaPhase <	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit)||(MeasuredNtscmSvCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit))
				err_ChromaPhaseResultOutOfRange(i, MeasuredNtscmSvCbChromaPhase[i].dblChromaPhase, dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit);
		}
	}
	Cleanup();
	return;
}


//ntsc-japan TV standard with Composite and s-video output sourse for Color Bars test
IMPLEMENT_TESTLOCKED (TV, ntscjcompcb, 3)
{
	
	#define STR_SIZE	32
	CHostResponse CHR;
	DWORD  dwMeasureResultBuffSize=EIACb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	char *send_buf = NULL;
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};
	
	//Init arrays to hold measured values.
	NTSCM_COMP_CB_DATA MeasuredNtscjCompCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredNtscjCompCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredNtscjCompCbChromaPhase[NUMBER_OF_CBARS];
	
	videomode=GetCfgInt(L"videomode", 1);
	switch(videomode)
	{
	case 1:
		DrawNTSCCB();
		NTSCCBRender();
		break;
	case 3:
		DrawMode3NTSCCB();
		NTSCCBRender();
		break;


	default:
		break;
	}

	// Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );


	for(int attempts=0; attempts < m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_NTSC_JAPAN_COMP_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
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

		// using string input to extract the datas in the measureresultbuff
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >>m_LumaIREWhite		>>m_LumaIREYellow
					>>m_LumaIRECyan			>>m_LumaIREGreen
					>>m_LumaIREMagenta		>>m_LumaIRERed
					>>m_LumaIREBlue			>>m_LumaIREBlack
					>>m_ChromaIREWhite		>>m_ChromaIREYellow 
					>>m_ChromaIRECyan		>>m_ChromaIREGreen
					>>m_ChromaIREMagenta	>>m_ChromaIRERed 
					>>m_ChromaIREBlue		>>m_ChromaIREBlack
					>>m_ChromaPhaseYellow	>>m_ChromaPhaseCyan 
					>>m_ChromaPhaseGreen	>>m_ChromaPhaseMagenta
					>>m_ChromaPhaseRed		>>m_ChromaPhaseBlue;
					//>> m_ChromaPhaseBlack;

		MeasuredNtscjCompCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredNtscjCompCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredNtscjCompCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredNtscjCompCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredNtscjCompCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredNtscjCompCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredNtscjCompCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredNtscjCompCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredNtscjCompCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredNtscjCompCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredNtscjCompCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredNtscjCompCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredNtscjCompCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredNtscjCompCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredNtscjCompCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredNtscjCompCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredNtscjCompCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredNtscjCompCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredNtscjCompCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredNtscjCompCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredNtscjCompCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredNtscjCompCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass = TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscjCompCbLumaIRE[i].dblLumaIRE < gdwJapanCBLumaIRE[i].dwJapanLumaLower)||(MeasuredNtscjCompCbLumaIRE[i].dblLumaIRE > gdwJapanCBLumaIRE[i].dwJapanLumaUpper))
				bPass = FALSE;
		}

		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredNtscjCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit)||(MeasuredNtscjCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit))
					bPass = FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredNtscjCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit)||(MeasuredNtscjCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit))
	 				bPass = FALSE;
			}
		}
		if(bPass)
			break;
	}//end retry loop

	//Report Statistic data of NTSC-M Composite color bar for Luma IRE:
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_WHITE, _T("%.1f"),m_LumaIREWhite);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_YELLOW, _T("%.1f"),m_LumaIREYellow);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_CYAN, _T("%.1f"),m_LumaIRECyan);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_GREEN, _T("%.1f"),m_LumaIREGreen);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_MAGENTA, _T("%.1f"),m_LumaIREMagenta);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_RED, _T("%.1f"),m_LumaIRERed);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_BLUE, _T("%.1f"),m_LumaIREBlue);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_BLACK, _T("%.1f"),m_LumaIREBlack);

	//Report Statistic data of NTSC-M Composite color bar for Chroma IRE:
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_YELLOW, _T("%.1f"),m_ChromaIREYellow);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_CYAN, _T("%.1f"),m_ChromaIRECyan);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_GREEN, _T("%.1f"),m_ChromaIREGreen);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_MAGENTA, _T("%.1f"),m_ChromaIREMagenta);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_RED, _T("%.1f"),m_ChromaIRERed);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLUE, _T("%.1f"),m_ChromaIREBlue);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);


	//Report Statistic data of NTSC-M Composite color bar for Chroma Phase:
	//ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_YELLOW, _T("%.1f"),m_ChromaPhaseYellow);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_CYAN, _T("%.1f"),m_ChromaPhaseCyan);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_GREEN, _T("%.1f"),m_ChromaPhaseGreen);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_MAGENTA, _T("%.1f"),m_ChromaPhaseMagenta);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_RED, _T("%.1f"),m_ChromaPhaseRed);
	ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLUE, _T("%.1f"),m_ChromaPhaseBlue);
	//ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscjCompCbLumaIRE[i].dblLumaIRE < gdwJapanCBLumaIRE[i].dwJapanLumaLower)||(MeasuredNtscjCompCbLumaIRE[i].dblLumaIRE > gdwJapanCBLumaIRE[i].dwJapanLumaUpper))
				err_LumaResultOutOfRange(i,MeasuredNtscjCompCbLumaIRE[i].dblLumaIRE,dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE,gdwJapanCBLumaIRE[i].dwJapanLumaUpper,	gdwJapanCBLumaIRE[i].dwJapanLumaLower);
		}
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscjCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit)||(MeasuredNtscjCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredNtscjCompCbChromaIRE[i].dblChromaIRE, dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredNtscjCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit)||(MeasuredNtscjCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit))
	 			err_ChromaPhaseResultOutOfRange(i,MeasuredNtscjCompCbChromaPhase[i].dblChromaPhase, dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit);
		}
	}

	Cleanup();
	return;
}





IMPLEMENT_TESTLOCKED (TV, ntscjsvcb, 4)
{

	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EIACb_ELEMS*2*sizeof(DWORD);;
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i=0;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;
	
	if (CheckAbort (HERE)){ Cleanup(); return;};

	//init arrays to hold measured values.
	NTSCM_SV_CB_DATA MeasuredNtscjSvCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_SV_CB_DATA MeasuredNtscjSvCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_SV_CB_DATA MeasuredNtscjSvCbChromaPhase[NUMBER_OF_CBARS];
		
	videomode=GetCfgInt(L"videomode", 1);
	switch(videomode)
	{
		case 1:
			DrawNTSCCB();
			NTSCCBRender();
			break;
		case 3:
			DrawMode3NTSCCB();
			NTSCCBRender();
			break;

		default:
			break;
	}

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	for(int attempts=0; attempts < m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_NTSC_JAPAN_SVIDEO_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if (CHR.pvBufferFromHost == NULL) 
		{
			if (err_EchoBufferResponseIsNull())
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			if (dwActualBytesReceived < dwMeasureResultBuffSize)
			{
				err_RecordBufferIsNotCorrectLength(dwMeasureResultBuffSize, dwActualBytesReceived);
				return;
			}
			// We received the measure resutls buffer the size that we wanted
		}

		memcpy(dwMeasureResultBuff, CHR.pvBufferFromHost, dwMeasureResultBuffSize); 

		// using string input to extract the datas
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >>m_LumaIREWhite		>>m_LumaIREYellow
					>>m_LumaIRECyan			>>m_LumaIREGreen
					>>m_LumaIREMagenta		>>m_LumaIRERed
					>>m_LumaIREBlue			>>m_LumaIREBlack
					>>m_ChromaIREWhite		>>m_ChromaIREYellow
					>>m_ChromaIRECyan		>>m_ChromaIREGreen
					>>m_ChromaIREMagenta	>>m_ChromaIRERed 
					>>m_ChromaIREBlue		>>m_ChromaIREBlack
					>>m_ChromaPhaseYellow	>>m_ChromaPhaseCyan
					>>m_ChromaPhaseGreen	>>m_ChromaPhaseMagenta
					>>m_ChromaPhaseRed		>>m_ChromaPhaseBlue;
					//>> m_ChromaPhaseBlack;

		MeasuredNtscjSvCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredNtscjSvCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredNtscjSvCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredNtscjSvCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredNtscjSvCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredNtscjSvCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredNtscjSvCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredNtscjSvCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredNtscjSvCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredNtscjSvCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredNtscjSvCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredNtscjSvCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredNtscjSvCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredNtscjSvCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredNtscjSvCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredNtscjSvCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredNtscjSvCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredNtscjSvCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredNtscjSvCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredNtscjSvCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredNtscjSvCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredNtscjSvCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;
		
		bPass = TRUE;
		for (i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscjSvCbLumaIRE[i].dblLumaIRE < 	gdwJapanCBLumaIRE[i].dwJapanLumaLower)||(MeasuredNtscjSvCbLumaIRE[i].dblLumaIRE >=	gdwJapanCBLumaIRE[i].dwJapanLumaUpper))
			bPass = FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredNtscjSvCbChromaIRE[i].dblChromaIRE <gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit)||(MeasuredNtscjSvCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit))
					bPass = FALSE;	
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredNtscjSvCbChromaPhase[i].dblChromaPhase <	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit)||(MeasuredNtscjSvCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit))
					bPass = FALSE;
			}			
		}

		if(bPass)
			break;
	
	}//end retry loop
	
	//Report Statistic data of NTSC-Japan Svideo color bar for Luma IRE:
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_WHITE, _T("%.1f"),m_LumaIREWhite);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_YELLOW, _T("%.1f"),m_LumaIREYellow);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_CYAN, _T("%.1f"),m_LumaIRECyan);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_GREEN, _T("%.1f"),m_LumaIREGreen);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_MAGENTA, _T("%.1f"),m_LumaIREMagenta);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_RED, _T("%.1f"),m_LumaIRERed);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_BLUE, _T("%.1f"),m_LumaIREBlue);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_BLACK, _T("%.1f"),m_LumaIREBlack);

	//Report Statistic data of NTSC-Japan Svideo color bar for Chroma IRE:
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_YELLOW, _T("%.1f"),m_ChromaIREYellow);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_CYAN, _T("%.1f"),m_ChromaIRECyan);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_GREEN, _T("%.1f"),m_ChromaIREGreen);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_MAGENTA, _T("%.1f"),m_ChromaIREMagenta);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_RED, _T("%.1f"),m_ChromaIRERed);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_BLUE, _T("%.1f"),m_ChromaIREBlue);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);


	//Report Statistic data of NTSC-Japan Svideo color bar for Chroma Phase:
	//ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_YELLOW, _T("%.1f"),m_ChromaPhaseYellow);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_CYAN, _T("%.1f"),m_ChromaPhaseCyan);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_GREEN, _T("%.1f"),m_ChromaPhaseGreen);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_MAGENTA, _T("%.1f"),m_ChromaPhaseMagenta);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_RED, _T("%.1f"),m_ChromaPhaseRed);
	ReportStatistic(NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLUE, _T("%.1f"),m_ChromaPhaseBlue);
	//ReportStatistic(NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	if(!bPass)
	{
		for (i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscjSvCbLumaIRE[i].dblLumaIRE < 	gdwJapanCBLumaIRE[i].dwJapanLumaLower)||(MeasuredNtscjSvCbLumaIRE[i].dblLumaIRE >=	gdwJapanCBLumaIRE[i].dwJapanLumaUpper))
				err_LumaResultOutOfRange(i, MeasuredNtscjSvCbLumaIRE[i].dblLumaIRE,dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE,	gdwJapanCBLumaIRE[i].dwJapanLumaUpper,	gdwJapanCBLumaIRE[i].dwJapanLumaLower);
		}
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredNtscjSvCbChromaIRE[i].dblChromaIRE <gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit)||(MeasuredNtscjSvCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i, MeasuredNtscjSvCbChromaIRE[i].dblChromaIRE, dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit);	
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredNtscjSvCbChromaPhase[i].dblChromaPhase <	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit)||(MeasuredNtscjSvCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit))
				err_ChromaPhaseResultOutOfRange(i, MeasuredNtscjSvCbChromaPhase[i].dblChromaPhase, dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit);
		}
	}

	Cleanup();
	return;
}
