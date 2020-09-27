// 
// (c) Copyright 2000-2000 Intel Corp. All rights reserved.
//   palbcb.cpp: used to implement pal(b) secam tv-standard for color bars 
//   tests with composite and s-video output. The subtests include 
//	 palbcompcb, palbsvcb, palmcompcb, palmsvcb, secamcompcb, and secamsvcb
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include <iostream>
#include <string>
#include <sstream>
#include "XTV.h"
#include "..\statkeys.h"


#define EBUCb_ELEMS	22

IMPLEMENT_TESTLOCKED (TV, palbcompcb, 5)
{
	
	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EBUCb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	NTSCM_COMP_CB_DATA MeasuredPalbCompCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaPhase[NUMBER_OF_CBARS];

	videomode=GetCfgInt(L"videomode", -1);

	switch(videomode)
	{
		case 1:
			DrawPALCB();
			PALCBRender();
			break;
		case 3:
			DrawMode3PALCB();
			PALCBRender();
			break;
		case 4:
			DrawMode4PALCB();
			PALCBRender();
			break;
		case 6:
			DrawMode6PALCB();
			PALCBRender();
			break;

		default:
			break;
	}

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_BDGHI_COMP_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
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

		// using string input to extract the datas from host in the measureresultbuff
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

		MeasuredPalbCompCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredPalbCompCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredPalbCompCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredPalbCompCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredPalbCompCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredPalbCompCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredPalbCompCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredPalbCompCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredPalbCompCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredPalbCompCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredPalbCompCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredPalbCompCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredPalbCompCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredPalbCompCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredPalbCompCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredPalbCompCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredPalbCompCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredPalbCompCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredPalbCompCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredPalbCompCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredPalbCompCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredPalbCompCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass = TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				bPass = FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
					bPass = FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 				bPass = FALSE;
			}
		}

		if(bPass)
			break;
	
	}//end retry loop


	//Report Statistic data of PAL-BDGHI Composite color bar for Luma IRE:
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_WHITE, _T("%.1f"),m_LumaIREWhite);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_YELLOW, _T("%.1f"),m_LumaIREYellow);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_CYAN, _T("%.1f"),m_LumaIRECyan);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_GREEN, _T("%.1f"),m_LumaIREGreen);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_MAGENTA, _T("%.1f"),m_LumaIREMagenta);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_RED, _T("%.1f"),m_LumaIRERed);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_BLUE, _T("%.1f"),m_LumaIREBlue);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_BLACK, _T("%.1f"),m_LumaIREBlack);

	//Report Statistic data of PAL-BDGHI Composite color bar for Chroma IRE:
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_YELLOW, _T("%.1f"),m_ChromaIREYellow);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_CYAN, _T("%.1f"),m_ChromaIRECyan);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_GREEN, _T("%.1f"),m_ChromaIREGreen);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_MAGENTA, _T("%.1f"),m_ChromaIREMagenta);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_RED, _T("%.1f"),m_ChromaIRERed);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLUE, _T("%.1f"),m_ChromaIREBlue);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);


	//Report Statistic data of PAL-BDGHI Composite color bar for Chroma Phase:
	//ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_YELLOW, _T("%.1f"),m_ChromaPhaseYellow);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_CYAN, _T("%.1f"),m_ChromaPhaseCyan);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_GREEN, _T("%.1f"),m_ChromaPhaseGreen);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_MAGENTA, _T("%.1f"),m_ChromaPhaseMagenta);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_RED, _T("%.1f"),m_ChromaPhaseRed);
	ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLUE, _T("%.1f"),m_ChromaPhaseBlue);
	//ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				err_LumaResultOutOfRange(i,MeasuredPalbCompCbLumaIRE[i].dblLumaIRE,dbStandardPalParams[i].dwStdPalCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredPalbCompCbChromaIRE[i].dblChromaIRE, dbStandardPalParams[i].dwStdPalCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 			 err_ChromaPhaseResultOutOfRange(i,MeasuredPalbCompCbChromaPhase[i].dblChromaPhase, 	dbStandardPalParams[i].dwStdPalCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
		}
	}

	Cleanup();
	return;
	
}


//pal(b) tv standard with s-video output source for the color bar test
IMPLEMENT_TESTLOCKED (TV, palbsvcb, 6)
{

	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EBUCb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError,videomode;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	NTSCM_COMP_CB_DATA MeasuredPalbSvCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbSvCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbSvCbChromaPhase[NUMBER_OF_CBARS];

	videomode=GetCfgInt(L"videomode", -1);
	switch(videomode)
	{
		case 1:
			DrawPALCB();
			PALCBRender();
			break;
		case 3:
			DrawMode3PALCB();
			PALCBRender();
			break;
		case 4:
			DrawMode4PALCB();
			PALCBRender();
			break;
		case 6:
			DrawMode6PALCB();
			PALCBRender();
			break;

		default:
			break;
	}

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
 
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_BDGHI_SVIDEO_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
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

		//ReportDebug(BIT0,L"After conversion to widerchar:\n%s", (LPCTSTR)CHR.pvBufferFromHost);

		memcpy(dwMeasureResultBuff, CHR.pvBufferFromHost, dwMeasureResultBuffSize); 
		// using string input to extract the datas in the measureresultbuff
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >>m_LumaIREWhite >>m_LumaIREYellow >>m_LumaIRECyan >>m_LumaIREGreen
					>>m_LumaIREMagenta >>m_LumaIRERed >>m_LumaIREBlue >>m_LumaIREBlack
					>> m_ChromaIREWhite >>m_ChromaIREYellow >>m_ChromaIRECyan >>m_ChromaIREGreen
					>>m_ChromaIREMagenta >>m_ChromaIRERed >>m_ChromaIREBlue >>m_ChromaIREBlack
					>>m_ChromaPhaseYellow >>m_ChromaPhaseCyan >>m_ChromaPhaseGreen
					>>m_ChromaPhaseMagenta >>m_ChromaPhaseRed >>m_ChromaPhaseBlue;

		MeasuredPalbSvCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredPalbSvCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredPalbSvCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredPalbSvCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredPalbSvCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredPalbSvCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredPalbSvCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredPalbSvCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredPalbSvCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredPalbSvCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredPalbSvCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredPalbSvCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredPalbSvCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredPalbSvCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredPalbSvCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredPalbSvCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredPalbSvCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredPalbSvCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredPalbSvCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredPalbSvCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredPalbSvCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredPalbSvCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;
		
		bPass = TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbSvCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbSvCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				bPass = FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredPalbSvCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbSvCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
					bPass = FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredPalbSvCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbSvCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 				bPass = FALSE;
			}
		}
		if(bPass)
			break;

	}//end retry loop

	//Report Statistic data of PAL-BDGHI Svideo color bar for Luma IRE:
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_WHITE, _T("%.1f"),m_LumaIREWhite);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_YELLOW, _T("%.1f"),m_LumaIREYellow);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_CYAN, _T("%.1f"),m_LumaIRECyan);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_GREEN, _T("%.1f"),m_LumaIREGreen);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_MAGENTA, _T("%.1f"),m_LumaIREMagenta);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_RED, _T("%.1f"),m_LumaIRERed);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_BLUE, _T("%.1f"),m_LumaIREBlue);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_BLACK, _T("%.1f"),m_LumaIREBlack);

	//Report Statistic data of PAL-BDGHI Svideo color bar for Chroma IRE:
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_YELLOW, _T("%.1f"),m_ChromaIREYellow);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_CYAN, _T("%.1f"),m_ChromaIRECyan);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_GREEN, _T("%.1f"),m_ChromaIREGreen);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_MAGENTA, _T("%.1f"),m_ChromaIREMagenta);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_RED, _T("%.1f"),m_ChromaIRERed);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_BLUE, _T("%.1f"),m_ChromaIREBlue);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	//Report Statistic data of PAL-BDGHI Svideo color bar for Chroma Phase:
	//ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_YELLOW, _T("%.1f"),m_ChromaPhaseYellow);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_CYAN, _T("%.1f"),m_ChromaPhaseCyan);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_GREEN, _T("%.1f"),m_ChromaPhaseGreen);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_MAGENTA, _T("%.1f"),m_ChromaPhaseMagenta);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_RED, _T("%.1f"),m_ChromaPhaseRed);
	ReportStatistic(PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLUE, _T("%.1f"),m_ChromaPhaseBlue);
	//ReportStatistic(PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbSvCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbSvCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				err_LumaResultOutOfRange(i,MeasuredPalbSvCbLumaIRE[i].dblLumaIRE,	dbStandardPalParams[i].dwStdPalCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}

		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbSvCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbSvCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredPalbSvCbChromaIRE[i].dblChromaIRE, 	dbStandardPalParams[i].dwStdPalCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}

		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredPalbSvCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbSvCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
				err_ChromaPhaseResultOutOfRange(i,MeasuredPalbSvCbChromaPhase[i].dblChromaPhase, 	dbStandardPalParams[i].dwStdPalCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
		}
	}

	Cleanup();
	return;
}


//secam tv standard with composite output source for the color bar test
IMPLEMENT_TESTLOCKED (TV, secamcompcb, 7)
{
	
	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EBUCb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	NTSCM_COMP_CB_DATA MeasuredSecamCompCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredSecamCompCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredSecamCompCbChromaPhase[NUMBER_OF_CBARS];
		
	DrawPALCB();
    // Render the scene
    PALCBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_SECAM_COMP_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
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

		inputString >> m_LumaIREWhite >>m_LumaIREYellow>>m_LumaIRECyan >> m_LumaIREGreen
					>>m_LumaIREMagenta >> m_LumaIRERed >> m_LumaIREBlue >> m_LumaIREBlack
					>> m_ChromaIREWhite >>m_ChromaIREYellow >>m_ChromaIRECyan >> m_ChromaIREGreen
					>>m_ChromaIREMagenta >> m_ChromaIRERed >> m_ChromaIREBlue >> m_ChromaIREBlack
					>>m_ChromaPhaseYellow >>m_ChromaPhaseCyan >> m_ChromaPhaseGreen>>m_ChromaPhaseMagenta 
					>> m_ChromaPhaseRed >> m_ChromaPhaseBlue;

		MeasuredSecamCompCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredSecamCompCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredSecamCompCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredSecamCompCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredSecamCompCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredSecamCompCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredSecamCompCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredSecamCompCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredSecamCompCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredSecamCompCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredSecamCompCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredSecamCompCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredSecamCompCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredSecamCompCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredSecamCompCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredSecamCompCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredSecamCompCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredSecamCompCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredSecamCompCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredSecamCompCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredSecamCompCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredSecamCompCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass=TRUE;		
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredSecamCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredSecamCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				bPass=FALSE;
		}

		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredSecamCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredSecamCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
					bPass = FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredSecamCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredSecamCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 				bPass=FALSE;		
			}
		}
		if(bPass)
			break;

	}//end retry loop


	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredSecamCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredSecamCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				err_LumaResultOutOfRange(i,MeasuredSecamCompCbLumaIRE[i].dblLumaIRE, dbStandardPalParams[i].dwStdPalCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredSecamCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredSecamCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredSecamCompCbChromaIRE[i].dblChromaIRE, dbStandardPalParams[i].dwStdPalCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredSecamCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredSecamCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
				err_ChromaPhaseResultOutOfRange(i,MeasuredSecamCompCbChromaPhase[i].dblChromaPhase, 	dbStandardPalParams[i].dwStdPalCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);		
		}
	}

	Cleanup();
	return;
}

//secam tv standard with s-video output source for the color bar test
IMPLEMENT_TESTLOCKED (TV, secamsvcb, 8)
{
	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EBUCb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;


	if (CheckAbort (HERE)){ Cleanup(); return;};
	
	NTSCM_COMP_CB_DATA MeasuredSecamSvCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredSecamSvCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredSecamSvCbChromaPhase[NUMBER_OF_CBARS];
	
	DrawPALCB();

    // Render the scene
    PALCBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
  

	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_SECAM_SVIDEO_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
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

		inputString >> m_LumaIREWhite >>m_LumaIREYellow >>m_LumaIRECyan >> m_LumaIREGreen
					>>m_LumaIREMagenta >> m_LumaIRERed >> m_LumaIREBlue >> m_LumaIREBlack
					>> m_ChromaIREWhite >>m_ChromaIREYellow >>m_ChromaIRECyan >> m_ChromaIREGreen
					>>m_ChromaIREMagenta >> m_ChromaIRERed >> m_ChromaIREBlue >> m_ChromaIREBlack
					>>m_ChromaPhaseYellow >>m_ChromaPhaseCyan >> m_ChromaPhaseGreen>>m_ChromaPhaseMagenta 
					>> m_ChromaPhaseRed >> m_ChromaPhaseBlue;

		MeasuredSecamSvCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredSecamSvCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredSecamSvCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredSecamSvCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredSecamSvCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredSecamSvCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredSecamSvCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredSecamSvCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredSecamSvCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredSecamSvCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredSecamSvCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredSecamSvCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredSecamSvCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredSecamSvCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredSecamSvCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredSecamSvCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredSecamSvCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredSecamSvCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredSecamSvCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredSecamSvCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredSecamSvCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredSecamSvCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredSecamSvCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredSecamSvCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredSecamSvCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredSecamSvCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
					bPass=FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredSecamSvCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredSecamSvCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 				bPass = FALSE;
			}
		}

	}//end retry loop

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredSecamSvCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredSecamSvCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				err_LumaResultOutOfRange(i,MeasuredSecamSvCbLumaIRE[i].dblLumaIRE,	dbStandardPalParams[i].dwStdPalCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS;i++){
			if(	(MeasuredSecamSvCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredSecamSvCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredSecamSvCbChromaIRE[i].dblChromaIRE, 	dbStandardPalParams[i].dwStdPalCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++){
			if(	(MeasuredSecamSvCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredSecamSvCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 			err_ChromaPhaseResultOutOfRange(i,MeasuredSecamSvCbChromaPhase[i].dblChromaPhase, 	dbStandardPalParams[i].dwStdPalCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
		}
	}

	Cleanup();
	return;
	
}



IMPLEMENT_TESTLOCKED (TV, palmcompcb, 9)
{
CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EBUCb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass = TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	NTSCM_COMP_CB_DATA MeasuredPalbCompCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaPhase[NUMBER_OF_CBARS];
	
	DrawPALCB();
	PALCBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	
	
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_M_COMP_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
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

		// using string input to extract the datas from host in the measureresultbuff
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >> m_LumaIREWhite >>m_LumaIREYellow >>m_LumaIRECyan >> m_LumaIREGreen>>m_LumaIREMagenta 
					>> m_LumaIRERed >> m_LumaIREBlue >> m_LumaIREBlack
					>> m_ChromaIREWhite >>m_ChromaIREYellow >>m_ChromaIRECyan >> m_ChromaIREGreen
					>>m_ChromaIREMagenta >> m_ChromaIRERed >> m_ChromaIREBlue >> m_ChromaIREBlack
					>>m_ChromaPhaseYellow >>m_ChromaPhaseCyan >> m_ChromaPhaseGreen>>m_ChromaPhaseMagenta >> m_ChromaPhaseRed 
					>> m_ChromaPhaseBlue;

		MeasuredPalbCompCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredPalbCompCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredPalbCompCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredPalbCompCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredPalbCompCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredPalbCompCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredPalbCompCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredPalbCompCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredPalbCompCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredPalbCompCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredPalbCompCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredPalbCompCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredPalbCompCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredPalbCompCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredPalbCompCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredPalbCompCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredPalbCompCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredPalbCompCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredPalbCompCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredPalbCompCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredPalbCompCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredPalbCompCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass = TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
					bPass=FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 				bPass=FALSE;
			}
		}
		if(bPass)
			break;

	}//end retry loop

	//Report Statistic data of PAL-M Composite color bar for Luma IRE:
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_WHITE, _T("%.1f"),m_LumaIREWhite);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_YELLOW, _T("%.1f"),m_LumaIREYellow);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_CYAN, _T("%.1f"),m_LumaIRECyan);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_GREEN, _T("%.1f"),m_LumaIREGreen);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_MAGENTA, _T("%.1f"),m_LumaIREMagenta);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_RED, _T("%.1f"),m_LumaIRERed);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_BLUE, _T("%.1f"),m_LumaIREBlue);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_BLACK, _T("%.1f"),m_LumaIREBlack);

	//Report Statistic data of PAL-M Composite color bar for Chroma IRE:
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_YELLOW, _T("%.1f"),m_ChromaIREYellow);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_CYAN, _T("%.1f"),m_ChromaIRECyan);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_GREEN, _T("%.1f"),m_ChromaIREGreen);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_MAGENTA, _T("%.1f"),m_ChromaIREMagenta);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_RED, _T("%.1f"),m_ChromaIRERed);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLUE, _T("%.1f"),m_ChromaIREBlue);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);


	//Report Statistic data of PAL-M Composite color bar for Chroma Phase:
	//ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_YELLOW, _T("%.1f"),m_ChromaPhaseYellow);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_CYAN, _T("%.1f"),m_ChromaPhaseCyan);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_GREEN, _T("%.1f"),m_ChromaPhaseGreen);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_MAGENTA, _T("%.1f"),m_ChromaPhaseMagenta);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_RED, _T("%.1f"),m_ChromaPhaseRed);
	ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLUE, _T("%.1f"),m_ChromaPhaseBlue);
	//ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				err_LumaResultOutOfRange(i,MeasuredPalbCompCbLumaIRE[i].dblLumaIRE, dbStandardPalParams[i].dwStdPalCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredPalbCompCbChromaIRE[i].dblChromaIRE, dbStandardPalParams[i].dwStdPalCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 			err_ChromaPhaseResultOutOfRange(i,MeasuredPalbCompCbChromaPhase[i].dblChromaPhase, 	dbStandardPalParams[i].dwStdPalCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
		}
	}

	Cleanup();
	return;
	
}



IMPLEMENT_TESTLOCKED (TV, palmsvcb, 10)
{
CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EBUCb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	NTSCM_COMP_CB_DATA MeasuredPalbCompCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaPhase[NUMBER_OF_CBARS];
	
	DrawPALCB();
	PALCBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_M_SVIDEO_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
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

		// using string input to extract the datas from host in the measureresultbuff
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >> m_LumaIREWhite >>m_LumaIREYellow >>m_LumaIRECyan >> m_LumaIREGreen>>m_LumaIREMagenta 
					>> m_LumaIRERed >> m_LumaIREBlue >> m_LumaIREBlack
					>> m_ChromaIREWhite >>m_ChromaIREYellow >>m_ChromaIRECyan >> m_ChromaIREGreen
					>>m_ChromaIREMagenta >> m_ChromaIRERed >> m_ChromaIREBlue >> m_ChromaIREBlack
					>>m_ChromaPhaseYellow >>m_ChromaPhaseCyan >> m_ChromaPhaseGreen>>m_ChromaPhaseMagenta >> m_ChromaPhaseRed 
					>> m_ChromaPhaseBlue;

		MeasuredPalbCompCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredPalbCompCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredPalbCompCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredPalbCompCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredPalbCompCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredPalbCompCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredPalbCompCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredPalbCompCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredPalbCompCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredPalbCompCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredPalbCompCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredPalbCompCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredPalbCompCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredPalbCompCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredPalbCompCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredPalbCompCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredPalbCompCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredPalbCompCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredPalbCompCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredPalbCompCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredPalbCompCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredPalbCompCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
					bPass=FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 				bPass=FALSE;
			}
		}
		if(bPass)
			break;
	
	}//end retry loop

	//Report Statistic data of PAL-M Svideo color bar for Luma IRE:
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_WHITE, _T("%.1f"),m_LumaIREWhite);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_YELLOW, _T("%.1f"),m_LumaIREYellow);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_CYAN, _T("%.1f"),m_LumaIRECyan);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_GREEN, _T("%.1f"),m_LumaIREGreen);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_MAGENTA, _T("%.1f"),m_LumaIREMagenta);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_RED, _T("%.1f"),m_LumaIRERed);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_BLUE, _T("%.1f"),m_LumaIREBlue);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_BLACK, _T("%.1f"),m_LumaIREBlack);

	//Report Statistic data of PAL-M Svideo color bar for Chroma IRE:
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_YELLOW, _T("%.1f"),m_ChromaIREYellow);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_CYAN, _T("%.1f"),m_ChromaIRECyan);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_GREEN, _T("%.1f"),m_ChromaIREGreen);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_MAGENTA, _T("%.1f"),m_ChromaIREMagenta);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_RED, _T("%.1f"),m_ChromaIRERed);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_BLUE, _T("%.1f"),m_ChromaIREBlue);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	//Report Statistic data of PAL-M Svideo color bar for Chroma Phase:
	//ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE, _T("%.1f"),m_ChromaIREWhite);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_YELLOW, _T("%.1f"),m_ChromaPhaseYellow);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_CYAN, _T("%.1f"),m_ChromaPhaseCyan);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_GREEN, _T("%.1f"),m_ChromaPhaseGreen);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_MAGENTA, _T("%.1f"),m_ChromaPhaseMagenta);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_RED, _T("%.1f"),m_ChromaPhaseRed);
	ReportStatistic(PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLUE, _T("%.1f"),m_ChromaPhaseBlue);
	//ReportStatistic(PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK, _T("%.1f"),m_ChromaIREBlack);

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				err_LumaResultOutOfRange(i, MeasuredPalbCompCbLumaIRE[i].dblLumaIRE, dbStandardPalParams[i].dwStdPalCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}

		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredPalbCompCbChromaIRE[i].dblChromaIRE, dbStandardPalParams[i].dwStdPalCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 			err_ChromaPhaseResultOutOfRange(i,MeasuredPalbCompCbChromaPhase[i].dblChromaPhase, 	dbStandardPalParams[i].dwStdPalCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
		}
	}

	Cleanup();
	return;

}


IMPLEMENT_TESTLOCKED (TV, palncompcb, 11)
{
	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EBUCb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	NTSCM_COMP_CB_DATA MeasuredPalbCompCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaPhase[NUMBER_OF_CBARS];

	
	DrawPALCB();
	PALCBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_N_COMP_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
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

		// using string input to extract the datas from host in the measureresultbuff
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >> m_LumaIREWhite >>m_LumaIREYellow >>m_LumaIRECyan >> m_LumaIREGreen>>m_LumaIREMagenta 
					>> m_LumaIRERed >> m_LumaIREBlue >> m_LumaIREBlack
					>> m_ChromaIREWhite >>m_ChromaIREYellow >>m_ChromaIRECyan >> m_ChromaIREGreen
					>>m_ChromaIREMagenta >> m_ChromaIRERed >> m_ChromaIREBlue >> m_ChromaIREBlack
					>>m_ChromaPhaseYellow >>m_ChromaPhaseCyan >> m_ChromaPhaseGreen>>m_ChromaPhaseMagenta >> m_ChromaPhaseRed 
					>> m_ChromaPhaseBlue;

		MeasuredPalbCompCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredPalbCompCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredPalbCompCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredPalbCompCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredPalbCompCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredPalbCompCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredPalbCompCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredPalbCompCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredPalbCompCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredPalbCompCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredPalbCompCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredPalbCompCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredPalbCompCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredPalbCompCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredPalbCompCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredPalbCompCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredPalbCompCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredPalbCompCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredPalbCompCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredPalbCompCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredPalbCompCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredPalbCompCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
					bPass=FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++){
				if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 				bPass=FALSE;
			}
		}
		if(bPass)
			break;
		
	}//end retry loop

	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				err_LumaResultOutOfRange(i,MeasuredPalbCompCbLumaIRE[i].dblLumaIRE, dbStandardPalParams[i].dwStdPalCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredPalbCompCbChromaIRE[i].dblChromaIRE, dbStandardPalParams[i].dwStdPalCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 			err_ChromaPhaseResultOutOfRange(i,MeasuredPalbCompCbChromaPhase[i].dblChromaPhase, 	dbStandardPalParams[i].dwStdPalCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
		}
	}
	
	Cleanup();
	return;

}


IMPLEMENT_TESTLOCKED (TV, palnsvpcb, 12)
{

	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD  dwMeasureResultBuffSize=EBUCb_ELEMS*2*sizeof(DWORD);
	DWORD *dwMeasureResultBuff=new DWORD[dwMeasureResultBuffSize/(sizeof(DWORD))];
	DWORD dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost,i;
	int iError;
	dwBufferToHostLength=NULL;
	BOOL bPass=TRUE;

	if (CheckAbort (HERE)){ Cleanup(); return;};

	NTSCM_COMP_CB_DATA MeasuredPalbCompCbLumaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaIRE[NUMBER_OF_CBARS];
	NTSCM_COMP_CB_DATA MeasuredPalbCompCbChromaPhase[NUMBER_OF_CBARS];
	
	DrawPALCB();
	PALCBRender();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		if ((iError = g_host.iSendHost(MID_PAL_N_SVIDEO_CB, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, XTV_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
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

		// using string input to extract the datas from host in the measureresultbuff
		using namespace std;
		string input((char *)dwMeasureResultBuff);
		istringstream inputString(input);

		inputString >> m_LumaIREWhite >>m_LumaIREYellow >>m_LumaIRECyan >> m_LumaIREGreen>>m_LumaIREMagenta 
					>> m_LumaIRERed >> m_LumaIREBlue >> m_LumaIREBlack
					>> m_ChromaIREWhite >>m_ChromaIREYellow >>m_ChromaIRECyan >> m_ChromaIREGreen
					>>m_ChromaIREMagenta >> m_ChromaIRERed >> m_ChromaIREBlue >> m_ChromaIREBlack
					>>m_ChromaPhaseYellow >>m_ChromaPhaseCyan >> m_ChromaPhaseGreen>>m_ChromaPhaseMagenta >> m_ChromaPhaseRed 
					>> m_ChromaPhaseBlue;

		MeasuredPalbCompCbLumaIRE[0].dblLumaIRE=m_LumaIREWhite;
		MeasuredPalbCompCbLumaIRE[1].dblLumaIRE= m_LumaIREYellow;
		MeasuredPalbCompCbLumaIRE[2].dblLumaIRE= m_LumaIRECyan;
		MeasuredPalbCompCbLumaIRE[3].dblLumaIRE= m_LumaIREGreen;
		MeasuredPalbCompCbLumaIRE[4].dblLumaIRE= m_LumaIREMagenta;
		MeasuredPalbCompCbLumaIRE[5].dblLumaIRE= m_LumaIRERed;
		MeasuredPalbCompCbLumaIRE[6].dblLumaIRE= m_LumaIREBlue;
		MeasuredPalbCompCbLumaIRE[7].dblLumaIRE= m_LumaIREBlack;

		MeasuredPalbCompCbChromaIRE[0].dblChromaIRE= m_ChromaIREWhite;
		MeasuredPalbCompCbChromaIRE[1].dblChromaIRE= m_ChromaIREYellow;
		MeasuredPalbCompCbChromaIRE[2].dblChromaIRE= m_ChromaIRECyan;
		MeasuredPalbCompCbChromaIRE[3].dblChromaIRE= m_ChromaIREGreen;
		MeasuredPalbCompCbChromaIRE[4].dblChromaIRE= m_ChromaIREMagenta;
		MeasuredPalbCompCbChromaIRE[5].dblChromaIRE= m_ChromaIRERed;
		MeasuredPalbCompCbChromaIRE[6].dblChromaIRE= m_ChromaIREBlue;
		MeasuredPalbCompCbChromaIRE[7].dblChromaIRE= m_ChromaIREBlack;

		//MeasuredNtscChromaPhase[0].dblChromaPhase= m_ChromaPhaseWhite;
		MeasuredPalbCompCbChromaPhase[0].dblChromaPhase= m_ChromaPhaseYellow;
		MeasuredPalbCompCbChromaPhase[1].dblChromaPhase= m_ChromaPhaseCyan;
		MeasuredPalbCompCbChromaPhase[2].dblChromaPhase= m_ChromaPhaseGreen;
		MeasuredPalbCompCbChromaPhase[3].dblChromaPhase= m_ChromaPhaseMagenta;
		MeasuredPalbCompCbChromaPhase[4].dblChromaPhase= m_ChromaPhaseRed;
		MeasuredPalbCompCbChromaPhase[5].dblChromaPhase= m_ChromaPhaseBlue;
		//MeasuredNtscChromaPhase[7].dblChromaPhase= m_ChromaPhaseBlack;

		bPass=TRUE;
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				bPass=FALSE;
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS;i++)
			{
				if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
					bPass=FALSE;
			}
		}
		if(bPass)
		{
			for(i=0;i<NUMBER_OF_CBARS-2;i++)
			{
				if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 				bPass=FALSE;
			}
		}
		if(bPass)
			break;

	}//end retry loop
	
	
	if(!bPass)
	{
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE < gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit)||(MeasuredPalbCompCbLumaIRE[i].dblLumaIRE > gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit))
				err_LumaResultOutOfRange(i, MeasuredPalbCompCbLumaIRE[i].dblLumaIRE, dbStandardPalParams[i].dwStdPalCbParamLumaIRE,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit,gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS;i++)
		{
			if(	(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE<	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit)||(MeasuredPalbCompCbChromaIRE[i].dblChromaIRE>gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit))
				err_ChromaIREResultOutOfRange(i,MeasuredPalbCompCbChromaIRE[i].dblChromaIRE, dbStandardPalParams[i].dwStdPalCbParamChromaIRE,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit,gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}
		for(i=0;i<NUMBER_OF_CBARS-2;i++)
		{
			if(	(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase<	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit)||(MeasuredPalbCompCbChromaPhase[i].dblChromaPhase>gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit))
	 			err_ChromaPhaseResultOutOfRange(i,MeasuredPalbCompCbChromaPhase[i].dblChromaPhase, 	dbStandardPalParams[i].dwStdPalCbParamChromaPhase,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit,gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
		}
	}

	Cleanup();
	return;

}



