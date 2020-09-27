// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
// XTV.cpp: implementation of the CXModuleTV class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "XTV.h"

#define AM 75   //75% Amplitude
#define SA 100  //100% Saturation
#define NUMBER_OF_BARS 8
#define STR_SIZE	64
struct CUSTOMVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };

//create vertex of NTSC TV standard for each bars
CUSTOMVERTEX wv[4], yv[4],cv[4],gv[4],mv[4],rv[4],bv[4],sbv[4],skv[4]
,skvb[4],smv[4],skv1[4],scv[4],skv2[4],swv[4],swvb[4],skvb2[4];

//create vertex of PAL TV standard for each bars
CUSTOMVERTEX pwv[4], pyv[4],pcv[4],pgv[4],pmv[4],prv[4],pbv[4],pbkv[4];

IMPLEMENT_MODULELOCKED (CXModuleTV);

bool CXModuleTV::InitializeParameters()
{

	DWORD i;
	wchar_t wszTempBuff[STR_SIZE];
	m_pVB= NULL;
	m_pTVPatternVB = NULL;


	if (!CTestObj::InitializeParameters ())
		return false;

	m_testRetries = GetCfgInt( L"retries", 1);


//***********************************************************
//******* get the upper and lower limits for NTSC color bars 
//************************************************************
	//	NtscmLuma IRE upper limits
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"luma_ire_upper",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
		gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE=DEFAULTPARAM;
//	ReportWarning(L"NtscmLumaUpperLimits::%lu\n", gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE);
	}else{
	gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"NtscmLumaUpperLimits::%lu\n", gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE);
		}
}

	//NtscmLumaIRE lower limits

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"luma_ire_lower",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE=DEFAULTPARAM;
//	ReportWarning(L"NtscmLumaLowerLimits::%lu\n", gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE);
	}else{
	gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"NtscmLumaLowerLimits::%lu\n", gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE);
	}

}
	//NtscmchromaIRE upper limits
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"chroma_ire_upper",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE=DEFAULTPARAM;
//	ReportWarning(L"NtscmChromaIREUpperLimits:%lu\n", gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE);
	}else{
	gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"NtscmChromaIREUpperLimits:%lu\n", gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE);
		}
}
	//NtscmchromaIRE lower limits

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"chroma_ire_lower",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE=DEFAULTPARAM;
	gdwColorBarLimitChromaIRE[0].dwLowerLimitChromaIRE=ZEROPARAM;
	gdwColorBarLimitChromaIRE[7].dwLowerLimitChromaIRE=ZEROPARAM;
//	ReportWarning(L"NtscmChromaIRELowerLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE);
	}else{
	gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"NtscmChromaIRELowerLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE);
	}
}
	
	//NtscmchromaPhase upper limits
	for(i=0;i<NUMBER_OF_BARS-2;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"chroma_phase_upper",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase=PHASEDEFAULTPARAM;
//	ReportWarning(L"NtscmChromaPhaseUpperLimits::%lu\n", gdwColorBarLimitChromaPhase[i].	dwUpperLimitChromaPhase);
	}else{
	gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"NtscmChromaPhaseUpperLimits::%lu\n", gdwColorBarLimitChromaPhase[i].	dwUpperLimitChromaPhase);
	}
}
	//NtscmchromaPhase lower limits

	for(i=0;i<NUMBER_OF_BARS-2;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"chroma_phase_lower",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase=PHASEDEFAULTPARAM;
//	ReportWarning(L"NtscmChromaPhaseLowerLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase);
	}else{
	gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"NtscmChromaPhaseLowerLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase);
		}
}

//***************************************************************
//******* get the standard video paramters for NTSC color bars 
//***************************************************************

	//NtscmTekStandard LumaIRE video parameters
 const int NtscmLumaIREDefaultParam[]={75,66,54,46,34,26,14,7};
 const int NtscmChromaIREDefaultParam[]={0,62,87,81,81,87,61,0};
 const int NtscmChromaPhaseDefaultParam[]={169,286,243,64,107,351};

 //NtscmTekStandard LumaIRE video parameters
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_luma_ire",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE=NtscmLumaIREDefaultParam[i];
	ReportWarning(L"TektronixStdNtscLumaIRE: %lu\n", dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE);
	}else{
		dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"TektronixStdNtscLumaIRE: %lu\n", dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE);
	}
	}

//NtscmTekStandard ChromaIRE video parameters
for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_chroma_ire",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE=NtscmChromaIREDefaultParam[i];
//	ReportWarning(L"TektronixStdNtscChromaIRE: %lu\n", 	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE);
	}else{
	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"TektronixStdNtscChromaIRE: %lu\n", 	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE);
	}
}

	//NtscmTekStandard ChromaPhase video parameters
	for(i=0;i<(NUMBER_OF_BARS-2);i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_chroma_phase",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase=NtscmChromaPhaseDefaultParam[i];
//	ReportWarning(L"TektronixStdNtscChromaPhase: %lu\n", dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase);
	}else{
		dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"TektronixStdNtscChromaPhase: %lu\n", dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase);
	}
	}

//*******************************************************************************
//**NTSCCB: Standard parmater + Upper limits and Standard parameter - lower limits
//*******************************************************************************

	// NtscmLumaIRE_Std_parameters + LumaIRE_Upper_Limits

   for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_luma_ire",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE= NtscmLumaIREDefaultParam[i];
	gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit=dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE+gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE;
//	ReportWarning(L"NtscmLumaIREUpperLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit);
	}else{

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_luma_ire",i);
	dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit=dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE+gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE;
//	ReportWarning(L"NtscmLumaIREUpperLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwLumaIREUpperLimit);
		}
	}
  }

	
	// NtscmLumaIRE_Std_parameters - LumaIRE_Lower_limits

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_luma_ire",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE=NtscmLumaIREDefaultParam[i];
	gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit=dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE-gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE;
//	ReportWarning(L"NtscmLumaIRELowerLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit);
	}else{
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_luma_ire",i);
	dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit=dbStandardNtscParams[i].dwStdNtscCbParamLumaIRE-gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE;
//	ReportWarning(L"NtscmLumaIRELowerLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwLumaIRELowerLimit);
		}
}


	// NtscmChromaIRE_Std_parameters + ChromaIRE_upper_limits

	
	for(i=1;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_chroma_ire",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE=NtscmChromaIREDefaultParam[i];
	dbStandardNtscParams[0].dwStdNtscCbParamChromaIRE=ZEROPARAM;
	dbStandardNtscParams[7].dwStdNtscCbParamChromaIRE=ZEROPARAM;
	gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit=dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE+gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE;
//	ReportWarning(L"NtscmChromaIREUpperLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_chroma_ire",i);
	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit=dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE+gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE;
//	ReportWarning(L"NtscmChromaIREUpperLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwChromaIREUpperLimit);
	}
	}
}	

	//  NtscmChromaIRE_Std_parameters - ChromaIRE_lower_limits	
	for(i=1;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_chroma_ire",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE=NtscmChromaIREDefaultParam[i];
	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE=ZEROPARAM;
	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE=ZEROPARAM;
    gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit=dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE-gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE;
//	ReportWarning(L"NtscmChromaIRELowerLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit);
	}else{

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_chroma_ire",i);
	dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit=dbStandardNtscParams[i].dwStdNtscCbParamChromaIRE-gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE;
//	ReportWarning(L"NtscmChromaIRELowerLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwChromaIRELowerLimit);
		}
	}
}

	// NtscmChromaPhase_Std_parameters + ChromaPhase_upper_limits	
	for(i=0;i<(NUMBER_OF_BARS-2);i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_chroma_phase",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit=NtscmChromaPhaseDefaultParam[i];
	gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit=dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase+gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase;
//	ReportWarning(L"NtscmChromaPhaseUpperLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit);
	}else{
	dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit=dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase+gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase;
//	ReportWarning(L"NtscmChromaPhaseUpperLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwChromaPhaseUpperLimit);
		}
	}

	// NtscmChromaPhase_Std_parameters - ChromaPhase_lower_limits	
	for(i=0;i<(NUMBER_OF_BARS-2);i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompcb", L"std_ntsccb_chroma_phase",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit=NtscmChromaPhaseDefaultParam[i];
	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit=dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase-gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase;
//	ReportWarning(L"ChromaPhaseLowerLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit);
	}else{
	dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit=dbStandardNtscParams[i].dwStdNtscCbParamChromaPhase-gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase;
//	ReportWarning(L"ChromaPhaseLowerLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwChromaPhaseLowerLimit);
	}
}


//*******************************************************************************
//**NTSCJapanCB: Tektronix Std Luma Paramters for NTSC-Japan
//*******************************************************************************

 const int NtscjLumaIREDefaultParam[]={75,66,53,44,31,22,9,7};


	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"std_param",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE=NtscjLumaIREDefaultParam[i];
//	ReportWarning(L"TektronixStdNtscJapanLumaIRE: %lu\n", dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"std_param",i);
	dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"TektronixStdNtscJapanLumaIRE: %lu\n", dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE);
		}
	}
}

//	Ntsc-japan LumaIRE Upper limits
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"upper_limits",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwJapanCBLumaIRE[i].dwJapanLumaUpper=DEFAULTPARAM;
//	ReportWarning(L"NtscJapanLumaIREUpperlimits: %lu\n", 	gdwJapanCBLumaIRE[i].dwJapanLumaUpper);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"upper_limits",i);
	gdwJapanCBLumaIRE[i].dwJapanLumaUpper=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"NtscJapanLumaIREUpperlimits: %lu\n", 	gdwJapanCBLumaIRE[i].dwJapanLumaUpper);
		}
	}
}	

// Ntsc-japan LumaIre Lower limits
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"lower_limits",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwJapanCBLumaIRE[i].dwJapanLumaLower=DEFAULTPARAM;
//	ReportWarning(L"NtscJapanLumaIRELowerlimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwJapanLumaLower);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"lower_limits",i);
	gdwJapanCBLumaIRE[i].dwJapanLumaLower=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"NtscJapanLumaIRELowerlimits: %lu\n", 	gdwJapanCBLumaIRE[i].dwJapanLumaLower);
		}
	}
}

	// Ntsc_japan_LumaIRE_Std_parameters + LumaIRE_Upper_Limits

		for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"std_param",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE= NtscjLumaIREDefaultParam[i];
	gdwJapanCBLumaIRE[i].dwJapanLumaUpper=dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE+	gdwJapanCBLumaIRE[i].dwJapanLumaUpper;
//	ReportWarning(L"NtscjLumaIREUpperLimits: %lu\n", 	gdwJapanCBLumaIRE[i].dwJapanLumaUpper);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"std_param",i);
	dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwJapanCBLumaIRE[i].dwJapanLumaUpper=dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE+	gdwJapanCBLumaIRE[i].dwJapanLumaUpper;
//	ReportWarning(L"NtscjLumaIREUpperLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwJapanLumaUpper);
		}
	}
}
	// NtscjLumaIRE_Std_parameters - LumaIRE_Lower_limits

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"std_param",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE=NtscjLumaIREDefaultParam[i];
	gdwJapanCBLumaIRE[i].dwJapanLumaLower=dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE-gdwJapanCBLumaIRE[i].dwJapanLumaLower;
//	ReportWarning(L"NtscjLumaIRELowerLimits: %lu\n", 	gdwJapanCBLumaIRE[i].dwJapanLumaLower);
	}else{

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntsc_japan", L"std_param",i);
	dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwJapanCBLumaIRE[i].dwJapanLumaLower=dbStandardNtscParams[i].dwStdNtscJapanCbParamLumaIRE-gdwJapanCBLumaIRE[i].dwJapanLumaLower;
//	ReportWarning(L"NtscjLumaIRELowerLimits: %lu\n", 	gdwJapanCBLumaIRE[i].dwJapanLumaLower);
		}
	}
}



//********************* PAL UPPPER and LOWER LIMITS ***********
//****** Get the upper and lower limits for PAL
//************************************************************
	//	PalLuma IRE upper limits

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"luma_ire_upper",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE=DEFAULTPARAM ;
//	ReportWarning(L"PalLumaUpperLimits::%lu\n", gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE);
}else{
	gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"PalLumaUpperLimits::%lu\n", gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE);
		}
}


	//LumaIRE lower limits

	for(i=0;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"luma_ire_lower",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE=DEFAULTPARAM ;
	gdwColorBarLimitLumaIRE[7].dwLowerLimitLumaIRE=ZEROPARAM;
//	ReportWarning(L"PalLumaLowerLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE);
}else{

	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"luma_ire_lower",i);
	gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
 //  	ReportWarning(L"PalLumaLowerLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE);
		}
	}
}

	//PalchromaIRE upper limits
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"chroma_ire_upper",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE=DEFAULTPARAM;
//	ReportWarning(L"PalChromaIREUpperLimits:%lu\n", gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE);
}else{
		gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
 //	ReportWarning(L"PalChromaIREUpperLimits:%lu\n", gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE);
	}
}

	//PalchromaIRE lower limits

	for(i=1;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"chroma_ire_lower",i);
 if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE=DEFAULTPARAM;
	gdwColorBarLimitChromaIRE[0].dwLowerLimitChromaIRE=ZEROPARAM;
	gdwColorBarLimitChromaIRE[7].dwLowerLimitChromaIRE=ZEROPARAM;
//	ReportWarning(L"PalChromaIRELowerLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"chroma_ire_lower",i);
	gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"PalChromaIRELowerLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE);
		}
	}
}
	
	//PalchromaPhase upper limits
	for(i=0;i<NUMBER_OF_BARS-2;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"chroma_phase_upper",i);
  if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase=PHASEDEFAULTPARAM;
//	ReportWarning(L"PalChromaPhaseUpperLimits:%lu\n", gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase);
  }else{
	gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
 //	ReportWarning(L"PalChromaPhaseUpperLimits:%lu\n", gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase);
	}
}

	//PalchromaPhase lower limits

	for(i=0;i<NUMBER_OF_BARS-2;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"chroma_phase_lower",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase=PHASEDEFAULTPARAM;
//	ReportWarning(L"PalChromaPhaseLowerLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase);
}else{
	gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
// 	ReportWarning(L"PalChromaPhaseLowerLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase);
	}
}

//********************* PAL TV standards ***********
//****** Get the standard video parameters for Pal
//**************************************************
	//Standard LumaIRE video parameters for pal tv standards
 const int PalLumaDefaultParam[]={98,65,51,43,30,21,8,0};
 const int PalChromaIREDefaultParam[]={0,63,89,83,83,90,62,0};
 const int PalChromaPhaseDefaultParam[]={169,287,244,63,106,349};

//Tektronix LumaIRE video parameters for pal tv standards
	for(i=0;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_luma_ire",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
dbStandardPalParams[i].dwStdPalCbParamLumaIRE=PalLumaDefaultParam[i];
 //ReportWarning(L"Pal-TekStdLumaIRE: %lu\n", dbStandardPalParams[i].dwStdPalCbParamLumaIRE);
}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_luma_ire",i);
	dbStandardPalParams[i].dwStdPalCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
// ReportWarning(L"Pal-TekStdLumaIRE: %lu\n", dbStandardPalParams[i].dwStdPalCbParamLumaIRE);
		}
	}
}

//Tektronix ChromaIRE video parameters for pal tv standards
	for(i=1;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_ire",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardPalParams[i].dwStdPalCbParamChromaIRE=PalChromaIREDefaultParam[i];
	dbStandardPalParams[0].dwStdPalCbParamChromaIRE=ZEROPARAM;
	dbStandardPalParams[7].dwStdPalCbParamChromaIRE=ZEROPARAM;
//	ReportWarning(L"Pal-TekStdChromaIRE: %lu\n", dbStandardPalParams[i].dwStdPalCbParamChromaIRE);
}else{	
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_ire",i);
	dbStandardPalParams[i].dwStdPalCbParamChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"Pal-TekStdChromaIRE: %lu\n", dbStandardPalParams[i].dwStdPalCbParamChromaIRE);
		}
	}
}

	//Tektronix ChromaPhase video parameters for pal tv standards
	for(i=0;i<(NUMBER_OF_BARS-2);i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_phase",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardPalParams[i].dwStdPalCbParamChromaPhase=PalChromaPhaseDefaultParam[i];
//	ReportWarning(L"Pal-TekStdChromaPhase: %lu\n", dbStandardPalParams[i].dwStdPalCbParamChromaPhase);
}else{
		dbStandardPalParams[i].dwStdPalCbParamChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
//	ReportWarning(L"Pal-TekStdChromaPhase: %lu\n", dbStandardPalParams[i].dwStdPalCbParamChromaPhase);
	}

	}


//*******************************************************************************
//***PAL_CB: Standard parmater + Upper limits and Standard parameter - lower limits
//*******************************************************************************

	// Pal-TekLumaIRE_Std_parameters + LumaIRE_Upper_Limits

		for(i=0;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_luma_ire",i);
 if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardPalParams[i].dwStdPalCbParamLumaIRE=PalLumaDefaultParam[i];
//	ReportWarning(L"Pal-LumaIREUpperLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_luma_ire",i);
		dbStandardPalParams[i].dwStdPalCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);

	gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit=dbStandardPalParams[i].dwStdPalCbParamLumaIRE + gdwColorBarLimitLumaIRE[i].dwUpperLimitLumaIRE;
//	ReportWarning(L"pal-LumaIREUpperLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwPalCbLumaIREUpperLimit);
		}
	}
}
	// Pal-LumaIRE_Std_parameters - LumaIRE_Lower_limits

	for(i=0;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_luma_ire",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardPalParams[i].dwStdPalCbParamLumaIRE=PalLumaDefaultParam[i];
//	ReportWarning(L"Pal-LumaIRELowerLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_luma_ire",i);
	dbStandardPalParams[i].dwStdPalCbParamLumaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit=dbStandardPalParams[i].dwStdPalCbParamLumaIRE-gdwColorBarLimitLumaIRE[i].dwLowerLimitLumaIRE;
//	ReportWarning(L"LumaIRELowerLimits: %lu\n", gdwColorBarLimitLumaIRE[i].dwPalCbLumaIRELowerLimit);
		}
	}
}

	// Pal-ChromaIRE_Std_parameters + ChromaIRE_upper_limits	
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_ire",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardPalParams[i].dwStdPalCbParamChromaIRE=PalChromaIREDefaultParam[i];
	dbStandardPalParams[0].dwStdPalCbParamChromaIRE=ZEROPARAM;
	dbStandardPalParams[7].dwStdPalCbParamChromaIRE=ZEROPARAM;
//	ReportWarning(L"Pal-ChromaIREUpperLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit);
	}else{
	for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_ire",i);
	dbStandardPalParams[i].dwStdPalCbParamChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit=dbStandardPalParams[i].dwStdPalCbParamChromaIRE+gdwColorBarLimitChromaIRE[i].dwUpperLimitChromaIRE;
//	ReportWarning(L"Pal-ChromaIREUpperLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwPalCbChromaIREUpperLimit);
		}
	}
}

	//  Pal-ChromaIRE_Std_parameters - ChromaIRE_lower_limits	
	for(i=1;i<NUMBER_OF_BARS-1;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_ire",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardPalParams[i].dwStdPalCbParamChromaIRE=PalChromaIREDefaultParam[i];
	dbStandardPalParams[0].dwStdPalCbParamChromaIRE=ZEROPARAM;
	dbStandardPalParams[7].dwStdPalCbParamChromaIRE=ZEROPARAM;
//	ReportWarning(L"Pal-ChromaIRELowerLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
}else{
for(i=0;i<NUMBER_OF_BARS;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_ire",i);
	dbStandardPalParams[i].dwStdPalCbParamChromaIRE=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit=	dbStandardPalParams[i].dwStdPalCbParamChromaIRE-gdwColorBarLimitChromaIRE[i].dwLowerLimitChromaIRE;
//	ReportWarning(L"Pal-ChromaIRELowerLimits: %lu\n", gdwColorBarLimitChromaIRE[i].dwPalCbChromaIRELowerLimit);
		}
	}
}

	// Pal-ChromaPhase_Std_parameters + ChromaPhase_upper_limits	
	for(i=0;i<(NUMBER_OF_BARS-2);i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_phase",i);
if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardPalParams[i].dwStdPalCbParamChromaPhase=PalChromaPhaseDefaultParam[i];
//	ReportWarning(L"Pal-ChromaPhaseUpperLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit);
}else{
	for(i=0;i<(NUMBER_OF_BARS-2);i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_phase",i);
	dbStandardPalParams[i].dwStdPalCbParamChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit=dbStandardPalParams[i].dwStdPalCbParamChromaPhase+gdwColorBarLimitChromaPhase[i].dwUpperLimitChromaPhase;
//	ReportWarning(L"Pal-ChromaPhaseUpperLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseUpperLimit);
		}
	}
}
	// Pal-ChromaPhase_Std_parameters - ChromaPhase_lower_limits	
	for(i=0;i<(NUMBER_OF_BARS-2);i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_phase",i);
	if((DWORD)GetCfgUint(wszTempBuff,NULL)==NULL){
	dbStandardPalParams[i].dwStdPalCbParamChromaPhase=PalChromaPhaseDefaultParam[i];
//	ReportWarning(L"Pal-ChromaPhaseLowerLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
	}else{
	for(i=0;i<(NUMBER_OF_BARS-2);i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palbcompcb", L"std_palcb_chroma_phase",i);
	dbStandardPalParams[i].dwStdPalCbParamChromaPhase=(DWORD)GetCfgUint(wszTempBuff,NULL);
	gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit=dbStandardPalParams[i].dwStdPalCbParamChromaPhase-gdwColorBarLimitChromaPhase[i].dwLowerLimitChromaPhase;
//	ReportWarning(L"Pal-ChromaPhaseLowerLimits: %lu\n", gdwColorBarLimitChromaPhase[i].dwPalCbChromaPhaseLowerLimit);
		}
	}
}

//*********************************************************************
//******* get the upper and lower limits for NTSC/Composite Multiburst 
//*********************************************************************


	//Multiburst NTSC/Composite upper and lower limits
	for(i=0;i<NUMBER_OF_FREQ;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompmb", L"freq_respnse_comp_lower",i);
	dbFreqLimit[i].NtscCompLowerLimit=(DWORD)GetCfgInt(wszTempBuff,NULL);
//	ReportDebug(BIT0,L"NTSCMBurstCompLowerLimits: %d(dB)\n",	dbFreqLimit[i].NtscCompLowerLimit );

	}

	for(i=0;i<NUMBER_OF_FREQ;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompmb", L"freq_respnse_comp_upper",i);
		dbFreqLimit[i].NtscCompUpperLimit=(DWORD)GetCfgInt(wszTempBuff,NULL);	
		
//	ReportDebug(BIT0,L"NTSCMBurstCompUpperLimits: %d(dB)\n",	dbFreqLimit[i].NtscCompUpperLimit );

	}
//*********************************************************************
//******* get the upper and lower limits for NTSC/S-Video Multiburst 
//*********************************************************************


	//Multiburst NTSC/S-Video upper and lower limits
	for(i=0;i<NUMBER_OF_FREQ;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompmb", L"freq_respnse_sv_lower",i);
	dbFreqLimit[i].NtscSvLowerLimit=(DWORD)GetCfgInt(wszTempBuff,NULL);
//	ReportDebug(BIT0,L"NTSCMBurstSvLowerLimits: %d(dB)\n",	dbFreqLimit[i].NtscSvLowerLimit );

	}

	for(i=0;i<NUMBER_OF_FREQ;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"ntscmcompmb", L"freq_respnse_sv_upper",i);
		dbFreqLimit[i].NtscSvUpperLimit=(DWORD)GetCfgInt(wszTempBuff,NULL);	
		
//	ReportDebug(BIT0,L"NTSCMBurstSvUpperLimits: %d(dB)\n",	dbFreqLimit[i].NtscSvUpperLimit );

	}


//********************************************************************
//******* get the upper and lower limits for PAL/Composite Multiburst 
//********************************************************************



//Multiburst PAL/Compsoite upper and lower limits
	for(i=0;i<NUMBER_OF_FREQ;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palcompmb", L"freq_respnse_comp_lower",i);
	dbFreqLimit[i].PalCompLowerLimit=(DWORD)GetCfgInt(wszTempBuff,NULL);
//	ReportDebug(BIT0,L"PALMBurstCompLowerLimits: %d(dB)\n",	dbFreqLimit[i].PalCompLowerLimit);

	}

	for(i=0;i<NUMBER_OF_FREQ;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palcompmb", L"freq_respnse_commp_upper",i);
		dbFreqLimit[i].PalCompUpperLimit=(DWORD)GetCfgInt(wszTempBuff,NULL);	
//	ReportDebug(BIT0,L"PALMBurstCompUpperLimits: %d(dB)\n",	dbFreqLimit[i].PalCompUpperLimit );

	}


//********************************************************************
//******* get the upper and lower limits for PAL/S-Video Multiburst 
//********************************************************************

//Multiburst PAL/Svideo upper and lower limits
	for(i=0;i<NUMBER_OF_FREQ;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palcompmb", L"freq_respnse_sv_lower",i);
	dbFreqLimit[i].PalSvLowerLimit=(DWORD)GetCfgInt(wszTempBuff,NULL);
//	ReportDebug(BIT0,L"PALMBurstSvLowerLimits: %d(dB)\n",	dbFreqLimit[i].PalSvLowerLimit);

	}

	for(i=0;i<NUMBER_OF_FREQ;i++){
	swprintf(wszTempBuff, L"%s.%s[%lu]", L"palcompmb", L"freq_respnse_sv_upper",i);
		dbFreqLimit[i].PalSvUpperLimit=(DWORD)GetCfgInt(wszTempBuff,NULL);	
//	ReportDebug(BIT0,L"PALMBurstSvUpperLimits: %d(dB)\n",	dbFreqLimit[i].PalSvUpperLimit );

	}

	return true;
}
 







//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------




VOID CXModuleTV::Cleanup()
{

 if( m_pVB != NULL )        
        m_pVB->Release();

 SAFE_RELEASE(m_pTVPatternVB);

 //    if( m_pD3D != NULL )       
//        m_pD3D->Release();

//g_pd3dDevice->Reset(&g_d3dpp);

}


BOOL CXModuleTV::DrawNTSCCB()
{

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_NORMAL|D3DFVF_DIFFUSE)

	HRESULT hr;
    static LPDIRECT3DVERTEXBUFFER8 m_pVB  = NULL;

if((hr = g_pd3dDevice->CreateVertexBuffer( 17*4*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB )) != D3D_OK)
	{
		ReportD3DError(hr);
		return FALSE;
	}

  //CUSTOMVERTEX* pwv;

if(FAILED(m_pVB->Lock(0,0,(BYTE**)&wv,0)))
return false;
wv[0].p = D3DXVECTOR4( 0, 0, 1.0f, 1.0f );     wv[0].color =0xffffffff;
wv[1].p = D3DXVECTOR4( 91.00f, 0, 1.0f, 1.0f );   wv[1].color =0xffffffff;
wv[2].p = D3DXVECTOR4( 0, 320.0f, 1.0f,1.0f  );   wv[2].color =0xffffffff;
wv[3].p = D3DXVECTOR4( 91.00f, 320.0f,	1.0f,1.0f ); wv[3].color =0xffffffff;
 //memcpy( pwv, wv, 4*sizeof(CUSTOMVERTEX) );

m_pVB->Unlock();

//Yellow
// CUSTOMVERTEX* pyv;
if(FAILED(m_pVB->Lock(0,0,(BYTE**)&yv,0)))
return false;

yv[0].p = D3DXVECTOR4( 91.00f, 0,	1.0f, 1.0f ); yv[0].color = 0xffffff00;
yv[1].p = D3DXVECTOR4( 182.00f, 0,	1.0f, 1.0f ); yv[1].color = 0xffffff00;
yv[2].p = D3DXVECTOR4( 91.00f, 320.0f,1.0f, 1.0f ); yv[2].color =0xffffff00;
yv[3].p = D3DXVECTOR4( 182.00f, 320.0f,1.0f, 1.0f ); yv[3].color =0xffffff00;

//memcpy( pyv, yv, 4*sizeof(CUSTOMVERTEX) );
m_pVB->Unlock();

//Cyan

//CUSTOMVERTEX* pcv;
if(FAILED(m_pVB->Lock(0,0,(BYTE**)&cv,0)))
return false;

cv[0].p = D3DXVECTOR4( 182.00f, 0,	1.0f, 1.0f ); cv[0].color =0xff00ffff;
cv[1].p = D3DXVECTOR4( 273.00f, 0,	1.0f, 1.0f ); cv[1].color = 0xff00ffff;
cv[2].p = D3DXVECTOR4( 182.00f, 320.0f,1.0f, 1.0f ); cv[3].color = 0xff00ffff;
cv[3].p = D3DXVECTOR4( 273.00f, 320.0f,1.0f, 1.0f ); cv[2].color =0xff00ffff;
 //memcpy( pcv, cv, 4*sizeof(CUSTOMVERTEX) );

m_pVB->Unlock();

//Green (blue)
//CUSTOMVERTEX* pgv;
if(FAILED(m_pVB->Lock(0,0,(BYTE**)&gv,0)))
return false;

gv[0].p = D3DXVECTOR4( 273.00f, 0,	1.0f, 1.0f ); gv[0].color = 0xff00ff00;
gv[1].p = D3DXVECTOR4( 364.00f, 0,	1.0f, 1.0f ); gv[1].color = 0xff00ff00;
gv[2].p = D3DXVECTOR4( 273.00f, 320.0f,1.0f, 1.0f ); gv[2].color =0xff00ff00;
gv[3].p = D3DXVECTOR4( 364.00f, 320.0f,1.0f, 1.0f ); gv[3].color = 0xff00ff00;
 //memcpy( pgv, gv, 4*sizeof(CUSTOMVERTEX) );

m_pVB->Unlock();

//Magenta
m_pVB->Lock(0,0,(BYTE**)&mv,0);
mv[0].p = D3DXVECTOR4( 364.00f, 0,	1.0f, 1.0f ); mv[0].color = 0xffff00ff;
mv[1].p = D3DXVECTOR4( 456.00f, 0,	1.0f, 1.0f ); mv[1].color = 0xffff00ff;
mv[2].p = D3DXVECTOR4( 364.00f, 320.0f,1.0f, 1.0f ); mv[2].color = 0xffff00ff;
mv[3].p = D3DXVECTOR4( 456.00f, 320.0f,1.0f, 1.0f ); mv[3].color =0xffff00ff; 
m_pVB->Unlock();

//Red (black)
m_pVB->Lock(0,0,(BYTE**)&rv,0);
rv[0].p = D3DXVECTOR4( 456.00f, 0,	1.0f, 1.0f ); rv[0].color = 0xffff0000;
rv[1].p = D3DXVECTOR4( 547.00f, 0,	1.0f, 1.0f ); rv[1].color = 0xffff0000;
rv[2].p = D3DXVECTOR4( 456.00f, 320.0f,1.0f, 1.0f ); rv[2].color =0xffff0000;
rv[3].p = D3DXVECTOR4( 547.00f, 320.0f,1.0f, 1.0f ); rv[3].color =0xffff0000;
m_pVB->Unlock();

//Blue
m_pVB->Lock(0,0,(BYTE**)&bv,0);
bv[0].p = D3DXVECTOR4( 547.00f, 0,	1.0f, 1.0f ); bv[0].color = 0xff0000ff;
bv[1].p = D3DXVECTOR4( 640.00f, 0,	1.0f, 1.0f ); bv[1].color =0xff0000ff;
bv[2].p = D3DXVECTOR4( 547.00f, 320.0f,1.0f, 1.0f ); bv[2].color =0xff0000ff;
bv[3].p = D3DXVECTOR4( 640.00f, 320.0f,1.0f, 1.0f ); bv[3].color =0xff0000ff;
m_pVB->Unlock();
//Small Blue bar
m_pVB->Lock(0,0,(BYTE**)&sbv,0);
sbv[0].p = D3DXVECTOR4( 0, 320.0f,	1.0f, 1.0f ); sbv[0].color = 0xff0000ff;
sbv[1].p = D3DXVECTOR4( 91.42850f, 320.0f,	1.0f, 1.0f ); sbv[1].color = 0xff0000ff;
sbv[2].p = D3DXVECTOR4( 0, 360.0f,1.0f, 1.0f ); sbv[2].color = 0xff0000ff;
sbv[3].p = D3DXVECTOR4( 91.4285f, 360.0f,1.0f, 1.0f ); sbv[3].color =0xff0000ff;
m_pVB->Unlock();

//small black bar
m_pVB->Lock(0,0,(BYTE**)&skv,0);
skv[0].p = D3DXVECTOR4( 91.4285f, 320.0f,	1.0f, 1.0f ); skv[0].color = 0;
skv[1].p = D3DXVECTOR4( 182.875f, 320.0f,	1.0f, 1.0f ); skv[1].color = 0;
skv[2].p = D3DXVECTOR4( 91.4285f, 360.0f,1.0f, 1.0f ); skv[2].color =0;
skv[3].p = D3DXVECTOR4( 182.875f, 360.0f,1.0f, 1.0f ); skv[3].color = 0;

m_pVB->Unlock();

//small Magenta bar
m_pVB->Lock(0,0,(BYTE**)&smv,0);

smv[0].p = D3DXVECTOR4( 182.875f, 320.0f,	1.0f, 1.0f ); smv[0].color =0xffff00ff; 
smv[1].p = D3DXVECTOR4( 274.304f, 320.0f,	1.0f, 1.0f ); smv[1].color = 0xffff00ff;
smv[2].p = D3DXVECTOR4( 182.875f, 360.0f,1.0f, 1.0f ); smv[2].color = 0xffff00ff;
smv[3].p = D3DXVECTOR4( 274.304f, 360.0f,1.0f, 1.0f ); smv[3].color =0xffff00ff;
m_pVB->Unlock();


// small black/green
m_pVB->Lock(0,0,(BYTE**)&skv1,0);
skv1[0].p = D3DXVECTOR4( 274.304f, 320.0f,	1.0f, 1.0f ); skv1[0].color = 0x00000000;
skv1[1].p = D3DXVECTOR4( 365.732f, 320.0f,	1.0f, 1.0f ); skv1[1].color = 0x00000000;
skv1[2].p = D3DXVECTOR4( 274.304f, 360.0f,1.0f, 1.0f ); skv1[2].color =0x00000000;
skv1[3].p = D3DXVECTOR4( 365.732f, 360.0f,1.0f, 1.0f ); skv1[3].color = 0x00000000;
m_pVB->Unlock();


// small cyan/Magenta
m_pVB->Lock(0,0,(BYTE**)&scv,0);
scv[0].p = D3DXVECTOR4( 365.732f, 320.0f,	1.0f, 1.0f ); scv[0].color =0xff00ffff; 
scv[1].p = D3DXVECTOR4( 457.161f, 320.0f,	1.0f, 1.0f ); scv[1].color =0xff00ffff; 
scv[2].p = D3DXVECTOR4( 365.732f, 360.0f,1.0f, 1.0f ); scv[2].color =0xff00ffff;
scv[3].p = D3DXVECTOR4( 457.161f, 360.0f,1.0f, 1.0f ); scv[3].color =0xff00ffff; 

m_pVB->Unlock();

//small black/red
m_pVB->Lock(0,0,(BYTE**)&skv2,0);
skv2[0].p = D3DXVECTOR4( 457.161f, 320.0f,	1.0f, 1.0f ); skv2[0].color = 0x00000000;
skv2[1].p = D3DXVECTOR4( 548.589f, 320.0f,	1.0f, 1.0f ); skv2[1].color = 0x00000000;
skv2[2].p = D3DXVECTOR4( 457.161f, 360.0f,1.0f, 1.0f ); skv2[2].color =0x00000000;
skv2[3].p = D3DXVECTOR4( 548.585f, 360.0f,1.0f, 1.0f ); skv2[3].color = 0x00000000;
m_pVB->Unlock();


//small white/blue
m_pVB->Lock(0,0,(BYTE**)&swv,0);
swv[0].p = D3DXVECTOR4( 548.589f, 320.0f, 1.0f, 1.0f );     swv[0].color = 0xffffffff;
swv[1].p = D3DXVECTOR4( 640.0f, 320.0f, 1.0f, 1.0f );   swv[1].color = 0xffffffff;
swv[2].p = D3DXVECTOR4( 548.589f, 360.0f, 1.0f,1.0f  );   swv[2].color = 0xffffffff;
swv[3].p = D3DXVECTOR4( 640.0f, 360.0f,	1.0f,1.0f ); swv[3].color = 0xffffffff;
m_pVB->Unlock();



//Bottom black bar
m_pVB->Lock(0,0,(BYTE**)&skvb,0);
skvb[0].p = D3DXVECTOR4(0, 360.0f,	1.0f, 1.0f ); skvb[0].color =0x00000000;
skvb[1].p = D3DXVECTOR4( 121.629f, 360.0f,	1.0f, 1.0f ); skvb[1].color =0x00000000; 
skvb[2].p = D3DXVECTOR4( 0, 480.0f,1.0f, 1.0f ); skvb[2].color =0x00000000;
skvb[3].p = D3DXVECTOR4( 121.629f, 480.0f,1.0f, 1.0f ); skvb[3].color =0x00000000; 
m_pVB->Unlock();

//Bottom white bar
m_pVB->Lock(0,0,(BYTE**)&swvb,0);
swvb[0].p = D3DXVECTOR4( 121.629f, 360.0f, 1.0f, 1.0f );     swvb[0].color = 0xffffffff;
swvb[1].p = D3DXVECTOR4( 228.590f, 360.0f, 1.0f, 1.0f );   swvb[1].color = 0xffffffff;
swvb[2].p = D3DXVECTOR4( 121.629f, 480.0f, 1.0f,1.0f  );   swvb[2].color = 0xffffffff;
swvb[3].p = D3DXVECTOR4( 228.590f, 480.0f,	1.0f,1.0f ); swvb[3].color = 0xffffffff;
m_pVB->Unlock();

//Bottom black bar2
m_pVB->Lock(0,0,(BYTE**)&skvb2,0);
skvb2[0].p = D3DXVECTOR4(228.590f, 360.0f,	1.0f, 1.0f ); skvb2[0].color =0x00000000;
skvb2[1].p = D3DXVECTOR4( 640.0f, 360.0f,	1.0f, 1.0f ); skvb2[1].color =0x00000000; 
skvb2[2].p = D3DXVECTOR4( 228.590f, 480.0f,1.0f, 1.0f ); skvb2[2].color =0x00000000;
skvb2[3].p = D3DXVECTOR4( 640.0f, 480.0f,1.0f, 1.0f ); skvb2[3].color =0x00000000; 
m_pVB->Unlock();



return true;

}

BOOL CXModuleTV::DrawPALCB()
{
	HRESULT hr;
static LPDIRECT3DVERTEXBUFFER8 m_pVB  = NULL;


if((hr = g_pd3dDevice->CreateVertexBuffer( 17*4*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB )) != D3D_OK)
	{
		ReportD3DError(hr);
		return FALSE;
	}


//PAL TV Standard for 640x480 video mode
//White

m_pVB->Lock(0,0,(BYTE**)&pwv,0);
pwv[0].p = D3DXVECTOR4( 0, 0, 1.0f, 1.0f );     pwv[0].color = 0xffffffff;
pwv[1].p = D3DXVECTOR4( 80.0f, 0, 1.0f, 1.0f );   pwv[1].color = 0xffffffff;
pwv[2].p = D3DXVECTOR4( 0, 480.0f, 1.0f,1.0f  );   pwv[2].color = 0xffffffff;
pwv[3].p = D3DXVECTOR4( 80.0f, 480.0f,	1.0f,1.0f ); pwv[3].color = 0xffffffff;
m_pVB->Unlock();

//Yellow
m_pVB->Lock(0,0,(BYTE**)&pyv,0);
pyv[0].p = D3DXVECTOR4( 80.0f, 0,	1.0f, 1.0f ); pyv[0].color = 0xffffff00;
pyv[1].p = D3DXVECTOR4( 160.0f, 0,	1.0f, 1.0f ); pyv[1].color = 0xffffff00;
pyv[2].p = D3DXVECTOR4( 80.0f, 480.0f,1.0f, 1.0f ); pyv[2].color = 0xffffff00;
pyv[3].p = D3DXVECTOR4( 160.0f, 480.0f,1.0f, 1.0f ); pyv[3].color = 0xffffff00;
m_pVB->Unlock();

//Cyan
m_pVB->Lock(0,0,(BYTE**)&pcv,0);
pcv[0].p = D3DXVECTOR4( 160.0f, 0,	1.0f, 1.0f ); pcv[0].color = 0xff00ffff;
pcv[1].p = D3DXVECTOR4( 240.0f, 0,	1.0f, 1.0f ); pcv[1].color = 0xff00ffff;
pcv[2].p = D3DXVECTOR4( 160.0f, 480.0f,1.0f, 1.0f ); pcv[3].color = 0xff00ffff;
pcv[3].p = D3DXVECTOR4( 240.0f, 480.0f,1.0f, 1.0f ); pcv[2].color = 0xff00ffff;
m_pVB->Unlock();

//Green
m_pVB->Lock(0,0,(BYTE**)&pgv,0);
pgv[0].p = D3DXVECTOR4( 240.0f, 0,	1.0f, 1.0f ); pgv[0].color = 0xff00ff00;
pgv[1].p = D3DXVECTOR4( 320.0f, 0,	1.0f, 1.0f ); pgv[1].color = 0xff00ff00;
pgv[2].p = D3DXVECTOR4( 240.0f, 480.0f,1.0f, 1.0f ); pgv[2].color = 0xff00ff00;
pgv[3].p = D3DXVECTOR4( 320.0f, 480.0f,1.0f, 1.0f ); pgv[3].color = 0xff00ff00;
m_pVB->Unlock();

//Magenta
m_pVB->Lock(0,0,(BYTE**)&pmv,0);
pmv[0].p = D3DXVECTOR4( 320.0f, 0,	1.0f, 1.0f ); pmv[0].color = 0xffff00ff;
pmv[1].p = D3DXVECTOR4( 400.0f, 0,	1.0f, 1.0f ); pmv[1].color = 0xffff00ff;
pmv[2].p = D3DXVECTOR4( 320.0f, 480.0f,1.0f, 1.0f ); pmv[2].color = 0xffff00ff;
pmv[3].p = D3DXVECTOR4( 400.0f, 480.0f,1.0f, 1.0f ); pmv[3].color =0xffff00ff; 
m_pVB->Unlock();

//Red
m_pVB->Lock(0,0,(BYTE**)&prv,0);
prv[0].p = D3DXVECTOR4( 400.0f, 0,	1.0f, 1.0f ); prv[0].color = 0xffff0000;
prv[1].p = D3DXVECTOR4( 480.0f, 0,	1.0f, 1.0f ); prv[1].color = 0xffff0000;
prv[2].p = D3DXVECTOR4( 400.0f, 480.0f,1.0f, 1.0f ); prv[2].color =0xffff0000;
prv[3].p = D3DXVECTOR4( 480.0f, 480.0f,1.0f, 1.0f ); prv[3].color = 0xffff0000;
m_pVB->Unlock();

//Blue
m_pVB->Lock(0,0,(BYTE**)&pbv,0);
pbv[0].p = D3DXVECTOR4( 480.0f, 0,	1.0f, 1.0f ); pbv[0].color = 0xff0000ff;
pbv[1].p = D3DXVECTOR4( 560.0f, 0,	1.0f, 1.0f ); pbv[1].color = 0xff0000ff;
pbv[2].p = D3DXVECTOR4( 480.0f, 480.0f,1.0f, 1.0f ); pbv[2].color = 0xff0000ff;
pbv[3].p = D3DXVECTOR4( 560.0f, 480.0f,1.0f, 1.0f ); pbv[3].color =0xff0000ff;
m_pVB->Unlock();
//black
m_pVB->Lock(0,0,(BYTE**)&pbkv,0);
pbkv[0].p = D3DXVECTOR4( 560.0f, 0,	1.0f, 1.0f ); pbkv[0].color = 0x00000000;
pbkv[1].p = D3DXVECTOR4( 640.0f, 0,	1.0f, 1.0f ); pbkv[1].color = 0x00000000;
pbkv[2].p = D3DXVECTOR4( 560.0f, 480.0f,1.0f, 1.0f ); pbkv[2].color = 0x00000000;
pbkv[3].p = D3DXVECTOR4( 640.0f, 480.0f,1.0f, 1.0f ); pbkv[3].color =0x00000000;
m_pVB->Unlock();



	return true;

}

VOID CXModuleTV::NTSCCBRender()
{


 g_pd3dDevice->Clear( 0, NULL,
						D3DCLEAR_TARGET|D3DCLEAR_TARGET_R|D3DCLEAR_TARGET_G|D3DCLEAR_TARGET_G|
						D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

   

    // Begin the scene
   g_pd3dDevice->BeginScene();

 //NTSC   
g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, wv, sizeof(wv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, yv, sizeof(yv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, cv, sizeof(cv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, gv, sizeof(gv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, mv, sizeof(mv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, rv, sizeof(rv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, bv, sizeof(bv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sbv, sizeof(sbv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, skv, sizeof(skv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, skvb, sizeof(skvb[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, smv, sizeof(smv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, skv1, sizeof(skv1[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, scv, sizeof(scv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, skv2, sizeof(skv2[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, swv, sizeof(swv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, swvb, sizeof(swvb[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, skvb2, sizeof(skvb2[0]));

   // End the scene
    g_pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
  //  g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

  
return;

}

VOID CXModuleTV::PALCBRender()
{

// Clear the backbuffer to a 75% white color
    
	 g_pd3dDevice->Clear( 0, NULL,
						D3DCLEAR_TARGET|D3DCLEAR_TARGET_R|D3DCLEAR_TARGET_G|D3DCLEAR_TARGET_G|
						D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(191,191,191), 1.0f, 0 );

	

    // Begin the scene
   g_pd3dDevice->BeginScene();
g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );

g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pwv, sizeof(pwv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pyv, sizeof(pyv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pcv, sizeof(pcv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pgv, sizeof(pgv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pmv, sizeof(pmv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, prv, sizeof(prv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pbv, sizeof(pbv[0]));
g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pbkv, sizeof(pbkv[0]));


    // End the scene
    g_pd3dDevice->EndScene();

   

return;
}

VOID CXModuleTV::ReportD3DError(DWORD error)
{
	switch(error)
	{
		case D3D_OK:
			err_DIRECTX_API (error, L"D3D_OK - No error occurred");
			break;

		case D3DERR_CONFLICTINGRENDERSTATE:
			err_DIRECTX_API (error, L"D3DERR_CONFLICTINGRENDERSTATE - Currently set render states can't be used together");
			break;

		case D3DERR_CONFLICTINGTEXTUREFILTER:
			err_DIRECTX_API (error, L"D3DERR_CONFLICTINGTEXTUREFILTER - Current texture filters can't be used together");
			break;

		case D3DERR_CONFLICTINGTEXTUREPALETTE:
			err_DIRECTX_API (error, L"D3DERR_CONFLICTINGTEXTUREPALETTE - Current textures can't be used simultaneously");
			break;

		case D3DERR_DEVICELOST:
			err_DIRECTX_API (error, L"D3DERR_DEVICELOST - Device is lost and can't be restored, so rendering is not possible");
			break;

		case D3DERR_DEVICENOTRESET:
			err_DIRECTX_API (error, L"D3DERR_DEVICENOTRESET - Device cannot be reset");
			break;

		case D3DERR_DRIVERINTERNALERROR:
			err_DIRECTX_API (error, L"D3DERR_DRIVERINTERNALERROR - Internal driver error");
			break;

		case D3DERR_INVALIDCALL:
			err_DIRECTX_API (error, L"D3DERR_INVALIDCALL - The method call is invalid. For example, a method's parameter may have an invalid value");
			break;

		case D3DERR_INVALIDDEVICE:
			err_DIRECTX_API (error, L"D3DERR_INVALIDDEVICE - The requested device type is not valid");
			break;

		case D3DERR_MOREDATA:
			err_DIRECTX_API (error, L"D3DERR_MOREDATA - There is more data available than the specified buffer size can hold");
			break;

		case D3DERR_NOTAVAILABLE:
			err_DIRECTX_API (error, L"D3DERR_NOTAVAILABLE - This device does not support the queried technique");
			break;

		case D3DERR_NOTFOUND:
			err_DIRECTX_API (error, L"D3DERR_NOTFOUND - The requested item was not found");
			break;

		case D3DERR_OUTOFVIDEOMEMORY:
			err_DIRECTX_API (error, L"D3DERR_OUTOFVIDEOMEMORY - Direct3D does not have enough display memory to perform the operation");
			break;

		case D3DERR_TOOMANYOPERATIONS:
			err_DIRECTX_API (error, L"D3DERR_TOOMANYOPERATIONS - Application is requesting more texture-filtering operations than the device supports");
			break;

		case D3DERR_UNSUPPORTEDALPHAARG:
			err_DIRECTX_API (error, L"D3DERR_UNSUPPORTEDALPHAARG - Device does not support a specified texture-blending argument for the alpha channel");
			break;

		case D3DERR_UNSUPPORTEDALPHAOPERATION:
			err_DIRECTX_API (error, L"D3DERR_UNSUPPORTEDALPHAOPERATION - Device does not support a specified texture-blending operation for the alpha channel");
			break;

		case D3DERR_UNSUPPORTEDCOLORARG:
			err_DIRECTX_API (error, L"D3DERR_UNSUPPORTEDCOLORARG - Device does not support a specified texture-blending argument for color values");
			break;

		case D3DERR_UNSUPPORTEDCOLOROPERATION:
			err_DIRECTX_API (error, L"D3DERR_UNSUPPORTEDCOLOROPERATION - Device does not support a specified texture-blending operation for color values");
			break;

		case D3DERR_UNSUPPORTEDFACTORVALUE:
			err_DIRECTX_API (error, L"D3DERR_UNSUPPORTEDFACTORVALUE - Device does not support the specified texture factor value");
			break;

		case D3DERR_UNSUPPORTEDTEXTUREFILTER:
			err_DIRECTX_API (error, L"D3DERR_UNSUPPORTEDTEXTUREFILTER - Device does not support the specified texture filter");
			break;

		case D3DERR_WRONGTEXTUREFORMAT:
			err_DIRECTX_API (error, L"D3DERR_WRONGTEXTUREFORMAT - Pixel format of the texture surface is not valid");
			break;

		case E_FAIL:
			err_DIRECTX_API (error, L"E_FAIL - An undetermined error occurred inside the Direct3D subsystem");
			break;

		case E_INVALIDARG:
			err_DIRECTX_API (error, L"E_INVALIDARG - An invalid parameter was passed to the returning function");
			break;

//		case E_INVALIDCALL:
//			err_DIRECTX_API (error, L"E_INVALIDCALL - The method call is invalid. For example, a method's parameter may have an invalid value.");
//			break;

		case E_OUTOFMEMORY:
			err_DIRECTX_API (error, L"E_OUTOFMEMORY - Direct3D could not allocate sufficient memory to complete the call");
			break;
	}

}





BOOL CXModuleTV::DrawNTSCMB()
{

struct CUSTOMVERTEX{ FLOAT x, y, z, rhw; DWORD color; };
#define D3DFVF_CUSTOMVERTEX_MB (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

CUSTOMVERTEX* vtx;


	DWORD i,d, Col=NTSC_COL_MODE1, pos=0, min=0, max=255, vert = 0;
	double j,Pixel_Clock=NTSC_Pixel_Clock;
	vtx= new CUSTOMVERTEX [NTSC_COL_MODE1 * 2];

//unsigned char *data=new unsigned char [BUFSIZE/(sizeof(unsigned char))];
wchar_t *data=new wchar_t [BUFSIZE/(sizeof(wchar_t))];

 

   for (i=0; i<NumFreq; i++) {
      double count=0.0;
      for (j = NTSC_mulit[i].start_time;
           j < NTSC_mulit[i].end_time;                     
		   j = j+1.0/Pixel_Clock) {
	    data[pos++]= (wchar_t)((double)AMP*
	       sin((double)(NTSC_mulit[i].freq*2*3.1416*count))  
		   *NTSC_mulit[i].amp + (NTSC_mulit[i].offset)*AMP);
	       count+=1/(1000000*Pixel_Clock);
      }
      if (i<NumFreq-1) {
	     for (j = NTSC_mulit[i].end_time;
		     j < NTSC_mulit[i+1].start_time;                     
		     j = j+1.0/Pixel_Clock) {
	       data[pos++]=(wchar_t)(285.7*AMP);
	     }
      }

                         
   }



 for (d=0;d<Col; d++) {
     if (data[d]>max) max=data[d];
     if (data[d]<min) min=data[d];
   }


	for( d = 0; d < Col; d++)
    {
		if(d < Col)
		{	
			DWORD color;
			color = data[d];
			
			vtx[vert].x = (float)d;				vtx[vert].y = 0.0f;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 0.0f;
			vtx[vert].color =D3DCOLOR_XRGB(color,color,color);
			vert++;		
			vtx[vert].x =(float)d;					vtx[vert].y = 480;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 0.0f;
			vtx[vert].color = D3DCOLOR_XRGB(color,color,color);
    		vert++;
		
			
		}
    }

   // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( NTSC_COL_MODE1*2*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_CUSTOMVERTEX_MB,
                                                  D3DPOOL_MANAGED, &m_pVB ) ) )
    {
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
		return FALSE;
    }
   
	CUSTOMVERTEX* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
        return FALSE;
	}

    memcpy( pVertices, vtx, NTSC_COL_MODE1*2*sizeof(CUSTOMVERTEX) );
    m_pVB->Unlock();
	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(vtx);

    return TRUE;
}



VOID CXModuleTV::NTSCMBRender()
{

g_pd3dDevice->Clear( 0, NULL,
						D3DCLEAR_TARGET|D3DCLEAR_TARGET_R|D3DCLEAR_TARGET_G|D3DCLEAR_TARGET_G|
						D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
		
    // Begin the scene
    g_pd3dDevice->BeginScene();


    //Draw the lines for the pixel count
	g_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX_MB );
    g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, (NTSC_COL_MODE3*2));

	
	// End the scene
    g_pd3dDevice->EndScene();


	

}

BOOL CXModuleTV::DrawPalMB()
{

struct CUSTOMVERTEX{ FLOAT x, y, z, rhw; DWORD color; };
#define D3DFVF_CUSTOMVERTEX_MB (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
CUSTOMVERTEX* vtx;



	DWORD d,i, Col=PAL_COL, pos=0, min=0, max=255, vert = 0;
	double j,Pixel_Clock=PAL_Pixel_Clock;;
	vtx= new CUSTOMVERTEX [PAL_COL * 2];

//unsigned char *data=new unsigned char [BUFSIZE/(sizeof(unsigned char))];
wchar_t *data=new wchar_t [BUFSIZE/(sizeof(wchar_t))];

 

   for (i=0; i<NumFreq; i++) {
      double count=0.0;
      for (j = PAL_mulit[i].start_time;
           j < PAL_mulit[i].end_time;                     
		   j = j+1.0/Pixel_Clock) {
	    data[pos++]= (wchar_t)((double)AMP*
	       sin((double)(PAL_mulit[i].freq*2*3.1416*count))  
		   *PAL_mulit[i].amp + (PAL_mulit[i].offset)*AMP);
	       count+=1/(1000000*Pixel_Clock);
      }
      if (i<NumFreq-1) {
	     for (j = PAL_mulit[i].end_time;
		     j < PAL_mulit[i+1].start_time;                     
		     j = j+1.0/Pixel_Clock) {
	       data[pos++]=(wchar_t)(285.7*AMP);
	     }
      }

                         
   }

 for (d=0;d<Col; d++) {
     if (data[d]>max) max=data[d];
     if (data[d]<min) min=data[d];
   }


	for( d = 0; d < Col; d++)
    {
		if(d < Col)
		{	
			DWORD color;
			color = data[d];
			
			vtx[vert].x = (float)d;				vtx[vert].y = 0.0f;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 1.0f;
			vtx[vert].color =D3DCOLOR_XRGB(color,color,color);
			vert++;		
			vtx[vert].x =(float)d;					vtx[vert].y = 480;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 1.0f;
			vtx[vert].color = D3DCOLOR_XRGB(color,color,color);
    		vert++;
		
		}
    }


   // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( PAL_COL*2*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_CUSTOMVERTEX_MB,
                                                  D3DPOOL_MANAGED, &m_pVB ) ) )
    {
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
		return FALSE;
    }

   
	CUSTOMVERTEX* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
        return FALSE;
	}

    memcpy( pVertices, vtx, PAL_COL*2*sizeof(CUSTOMVERTEX) );
    m_pVB->Unlock();
	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(vtx);
	
    return TRUE;
}

VOID CXModuleTV::PALMBRender()
{

g_pd3dDevice->Clear( 0, NULL,
						D3DCLEAR_TARGET|D3DCLEAR_TARGET_R|D3DCLEAR_TARGET_G|D3DCLEAR_TARGET_G|
						D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
		
    // Begin the scene
    g_pd3dDevice->BeginScene();

    //Draw the lines for the pixel count
	g_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX_MB );
    g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, (PAL_COL_MODE3*2));
	
	// End the scene
    g_pd3dDevice->EndScene();

}



BOOL CXModuleTV::DrawMode3NTSCCB()
{
//720x480=Mode3
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_NORMAL|D3DFVF_DIFFUSE)

	HRESULT hr;
    static LPDIRECT3DVERTEXBUFFER8 m_pVB  = NULL;

if((hr = g_pd3dDevice->CreateVertexBuffer( 17*4*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB )) != D3D_OK)
	{
		ReportD3DError(hr);
		return FALSE;
	}

  //CUSTOMVERTEX* pwv;

if(FAILED(m_pVB->Lock(0,0,(BYTE**)&wv,0)))
return false;
wv[0].p = D3DXVECTOR4( 0, 0, 1.0f, 1.0f );     wv[0].color =0xffffffff;
wv[1].p = D3DXVECTOR4( 102.85f, 0, 1.0f, 1.0f );   wv[1].color =0xffffffff;
wv[2].p = D3DXVECTOR4( 0, 320.0f, 1.0f,1.0f  );   wv[2].color =0xffffffff;
wv[3].p = D3DXVECTOR4( 102.85f, 320.0f,	1.0f,1.0f ); wv[3].color =0xffffffff;
 //memcpy( pwv, wv, 4*sizeof(CUSTOMVERTEX) );

m_pVB->Unlock();

//Yellow
// CUSTOMVERTEX* pyv;
if(FAILED(m_pVB->Lock(0,0,(BYTE**)&yv,0)))
return false;

yv[0].p = D3DXVECTOR4( 102.85f, 0,	1.0f, 1.0f ); yv[0].color = 0xffffff00;
yv[1].p = D3DXVECTOR4( 205.71f, 0,	1.0f, 1.0f ); yv[1].color = 0xffffff00;
yv[2].p = D3DXVECTOR4( 102.85f, 320.0f,1.0f, 1.0f ); yv[2].color =0xffffff00;
yv[3].p = D3DXVECTOR4( 205.71f, 320.0f,1.0f, 1.0f ); yv[3].color =0xffffff00;

//memcpy( pyv, yv, 4*sizeof(CUSTOMVERTEX) );
m_pVB->Unlock();

//Cyan

//CUSTOMVERTEX* pcv;
if(FAILED(m_pVB->Lock(0,0,(BYTE**)&cv,0)))
return false;

cv[0].p = D3DXVECTOR4( 205.71f, 0,	1.0f, 1.0f ); cv[0].color =0xff00ffff;
cv[1].p = D3DXVECTOR4( 308.56f, 0,	1.0f, 1.0f ); cv[1].color = 0xff00ffff;
cv[2].p = D3DXVECTOR4( 205.71f, 320.0f,1.0f, 1.0f ); cv[3].color = 0xff00ffff;
cv[3].p = D3DXVECTOR4( 308.56f, 320.0f,1.0f, 1.0f ); cv[2].color =0xff00ffff;
 //memcpy( pcv, cv, 4*sizeof(CUSTOMVERTEX) );

m_pVB->Unlock();

//Green (blue)
//CUSTOMVERTEX* pgv;
if(FAILED(m_pVB->Lock(0,0,(BYTE**)&gv,0)))
return false;

gv[0].p = D3DXVECTOR4( 308.56f, 0,	1.0f, 1.0f ); gv[0].color = 0xff00ff00;
gv[1].p = D3DXVECTOR4( 411.41f, 0,	1.0f, 1.0f ); gv[1].color = 0xff00ff00;
gv[2].p = D3DXVECTOR4( 308.56f, 320.0f,1.0f, 1.0f ); gv[2].color =0xff00ff00;
gv[3].p = D3DXVECTOR4( 411.41f, 320.0f,1.0f, 1.0f ); gv[3].color = 0xff00ff00;
 //memcpy( pgv, gv, 4*sizeof(CUSTOMVERTEX) );

m_pVB->Unlock();

//Magenta
m_pVB->Lock(0,0,(BYTE**)&mv,0);
mv[0].p = D3DXVECTOR4( 411.41f, 0,	1.0f, 1.0f ); mv[0].color = 0xffff00ff;
mv[1].p = D3DXVECTOR4( 514.26f, 0,	1.0f, 1.0f ); mv[1].color = 0xffff00ff;
mv[2].p = D3DXVECTOR4( 411.41f, 320.0f,1.0f, 1.0f ); mv[2].color = 0xffff00ff;
mv[3].p = D3DXVECTOR4( 514.26f, 320.0f,1.0f, 1.0f ); mv[3].color =0xffff00ff; 
m_pVB->Unlock();

//Red (black)
m_pVB->Lock(0,0,(BYTE**)&rv,0);
rv[0].p = D3DXVECTOR4( 514.26f, 0,	1.0f, 1.0f ); rv[0].color = 0xffff0000;
rv[1].p = D3DXVECTOR4( 617.11f, 0,	1.0f, 1.0f ); rv[1].color = 0xffff0000;
rv[2].p = D3DXVECTOR4( 514.26f, 320.0f,1.0f, 1.0f ); rv[2].color =0xffff0000;
rv[3].p = D3DXVECTOR4( 617.11f, 320.0f,1.0f, 1.0f ); rv[3].color =0xffff0000;
m_pVB->Unlock();

//Blue
m_pVB->Lock(0,0,(BYTE**)&bv,0);
bv[0].p = D3DXVECTOR4( 617.11f, 0,	1.0f, 1.0f ); bv[0].color = 0xff0000ff;
bv[1].p = D3DXVECTOR4( 720.00f, 0,	1.0f, 1.0f ); bv[1].color =0xff0000ff;
bv[2].p = D3DXVECTOR4( 617.11f, 320.0f,1.0f, 1.0f ); bv[2].color =0xff0000ff;
bv[3].p = D3DXVECTOR4( 720.00f, 320.0f,1.0f, 1.0f ); bv[3].color =0xff0000ff;
m_pVB->Unlock();

//Small Blue bar
m_pVB->Lock(0,0,(BYTE**)&sbv,0);
sbv[0].p = D3DXVECTOR4( 0, 320.0f,	1.0f, 1.0f ); sbv[0].color = 0xff0000ff;
sbv[1].p = D3DXVECTOR4( 102.85f, 320.0f,	1.0f, 1.0f ); sbv[1].color = 0xff0000ff;
sbv[2].p = D3DXVECTOR4( 0, 360.0f,1.0f, 1.0f ); sbv[2].color = 0xff0000ff;
sbv[3].p = D3DXVECTOR4( 102.85f, 360.0f,1.0f, 1.0f ); sbv[3].color =0xff0000ff;
m_pVB->Unlock();

//small black bar
m_pVB->Lock(0,0,(BYTE**)&skv,0);
skv[0].p = D3DXVECTOR4( 102.85f, 320.0f,	1.0f, 1.0f ); skv[0].color = 0;
skv[1].p = D3DXVECTOR4( 205.70f, 320.0f,	1.0f, 1.0f ); skv[1].color = 0;
skv[2].p = D3DXVECTOR4( 102.85f, 360.0f,1.0f, 1.0f ); skv[2].color =0;
skv[3].p = D3DXVECTOR4( 205.70f, 360.0f,1.0f, 1.0f ); skv[3].color = 0;

m_pVB->Unlock();

//small Magenta bar
m_pVB->Lock(0,0,(BYTE**)&smv,0);

smv[0].p = D3DXVECTOR4( 205.71f, 320.0f,	1.0f, 1.0f ); smv[0].color =0xffff00ff; 
smv[1].p = D3DXVECTOR4( 308.56f, 320.0f,	1.0f, 1.0f ); smv[1].color = 0xffff00ff;
smv[2].p = D3DXVECTOR4( 205.71f, 360.0f,1.0f, 1.0f ); smv[2].color = 0xffff00ff;
smv[3].p = D3DXVECTOR4( 308.56f, 360.0f,1.0f, 1.0f ); smv[3].color =0xffff00ff;
m_pVB->Unlock();


// small black/green
m_pVB->Lock(0,0,(BYTE**)&skv1,0);
skv1[0].p = D3DXVECTOR4( 308.56f, 320.0f,	1.0f, 1.0f ); skv1[0].color = 0x00000000;
skv1[1].p = D3DXVECTOR4( 411.41f, 320.0f,	1.0f, 1.0f ); skv1[1].color = 0x00000000;
skv1[2].p = D3DXVECTOR4( 308.56f, 360.0f,1.0f, 1.0f ); skv1[2].color =0x00000000;
skv1[3].p = D3DXVECTOR4( 411.41f, 360.0f,1.0f, 1.0f ); skv1[3].color = 0x00000000;
m_pVB->Unlock();


// small cyan/Magenta
m_pVB->Lock(0,0,(BYTE**)&scv,0);
scv[0].p = D3DXVECTOR4( 411.41f, 320.0f,	1.0f, 1.0f ); scv[0].color =0xff00ffff; 
scv[1].p = D3DXVECTOR4( 514.26f, 320.0f,	1.0f, 1.0f ); scv[1].color =0xff00ffff; 
scv[2].p = D3DXVECTOR4( 411.41f, 360.0f,1.0f, 1.0f ); scv[2].color =0xff00ffff;
scv[3].p = D3DXVECTOR4( 514.26f, 360.0f,1.0f, 1.0f ); scv[3].color =0xff00ffff; 

m_pVB->Unlock();

//small black/red
m_pVB->Lock(0,0,(BYTE**)&skv2,0);
skv2[0].p = D3DXVECTOR4( 514.26f, 320.0f,	1.0f, 1.0f ); skv2[0].color = 0x00000000;
skv2[1].p = D3DXVECTOR4( 617.11f, 320.0f,	1.0f, 1.0f ); skv2[1].color = 0x00000000;
skv2[2].p = D3DXVECTOR4( 514.26f, 360.0f,1.0f, 1.0f ); skv2[2].color =0x00000000;
skv2[3].p = D3DXVECTOR4( 617.11f, 360.0f,1.0f, 1.0f ); skv2[3].color = 0x00000000;
m_pVB->Unlock();


//small white/blue
m_pVB->Lock(0,0,(BYTE**)&swv,0);
swv[0].p = D3DXVECTOR4( 617.11f, 320.0f, 1.0f, 1.0f );     swv[0].color = 0xffffffff;
swv[1].p = D3DXVECTOR4( 720.0f, 320.0f, 1.0f, 1.0f );   swv[1].color = 0xffffffff;
swv[2].p = D3DXVECTOR4( 617.11f, 360.0f, 1.0f,1.0f  );   swv[2].color = 0xffffffff;
swv[3].p = D3DXVECTOR4( 720.0f, 360.0f,	1.0f,1.0f ); swv[3].color = 0xffffffff;
m_pVB->Unlock();



//Bottom black bar
m_pVB->Lock(0,0,(BYTE**)&skvb,0);
skvb[0].p = D3DXVECTOR4(0, 360.0f,	1.0f, 1.0f ); skvb[0].color =0x00000000;
skvb[1].p = D3DXVECTOR4( 137.13f, 360.0f,	1.0f, 1.0f ); skvb[1].color =0x00000000; 
skvb[2].p = D3DXVECTOR4( 0, 480.0f,1.0f, 1.0f ); skvb[2].color =0x00000000;
skvb[3].p = D3DXVECTOR4( 137.13f, 480.0f,1.0f, 1.0f ); skvb[3].color =0x00000000; 
m_pVB->Unlock();

//Bottom white bar
m_pVB->Lock(0,0,(BYTE**)&swvb,0);
swvb[0].p = D3DXVECTOR4( 137.13f, 360.0f, 1.0f, 1.0f );     swvb[0].color = 0xffffffff;
swvb[1].p = D3DXVECTOR4( 257.13f, 360.0f, 1.0f, 1.0f );   swvb[1].color = 0xffffffff;
swvb[2].p = D3DXVECTOR4( 137.13f, 480.0f, 1.0f,1.0f  );   swvb[2].color = 0xffffffff;
swvb[3].p = D3DXVECTOR4( 257.13f, 480.0f,	1.0f,1.0f ); swvb[3].color = 0xffffffff;
m_pVB->Unlock();

//Bottom black bar2
m_pVB->Lock(0,0,(BYTE**)&skvb2,0);
skvb2[0].p = D3DXVECTOR4(257.13f, 360.0f,	1.0f, 1.0f ); skvb2[0].color =0x00000000;
skvb2[1].p = D3DXVECTOR4( 720.0f, 360.0f,	1.0f, 1.0f ); skvb2[1].color =0x00000000; 
skvb2[2].p = D3DXVECTOR4( 257.13f, 480.0f,1.0f, 1.0f ); skvb2[2].color =0x00000000;
skvb2[3].p = D3DXVECTOR4( 720.0f, 480.0f,1.0f, 1.0f ); skvb2[3].color =0x00000000; 
m_pVB->Unlock();



return true;

}



BOOL CXModuleTV::DrawMode3PALCB()
{

	HRESULT hr;
static LPDIRECT3DVERTEXBUFFER8 m_pVB  = NULL;


if((hr = g_pd3dDevice->CreateVertexBuffer( 17*4*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB )) != D3D_OK)
	{
		ReportD3DError(hr);
		return FALSE;
	}


//PAL TV Standard for 720x480 video mode3
//White

m_pVB->Lock(0,0,(BYTE**)&pwv,0);
pwv[0].p = D3DXVECTOR4( 0, 0, 1.0f, 1.0f );     pwv[0].color = 0xffffffff;
pwv[1].p = D3DXVECTOR4( 90.00f, 0, 1.0f, 1.0f );   pwv[1].color = 0xffffffff;
pwv[2].p = D3DXVECTOR4( 0, 480.0f, 1.0f,1.0f  );   pwv[2].color = 0xffffffff;
pwv[3].p = D3DXVECTOR4( 90.00f, 480.0f,	1.0f,1.0f ); pwv[3].color = 0xffffffff;
m_pVB->Unlock();

//Yellow
m_pVB->Lock(0,0,(BYTE**)&pyv,0);
pyv[0].p = D3DXVECTOR4( 90.00f, 0,	1.0f, 1.0f ); pyv[0].color = 0xffffff00;
pyv[1].p = D3DXVECTOR4( 180.00f, 0,	1.0f, 1.0f ); pyv[1].color = 0xffffff00;
pyv[2].p = D3DXVECTOR4( 90.00f, 480.0f,1.0f, 1.0f ); pyv[2].color = 0xffffff00;
pyv[3].p = D3DXVECTOR4( 180.00f, 480.0f,1.0f, 1.0f ); pyv[3].color = 0xffffff00;
m_pVB->Unlock();

//Cyan
m_pVB->Lock(0,0,(BYTE**)&pcv,0);
pcv[0].p = D3DXVECTOR4( 180.00f, 0,	1.0f, 1.0f ); pcv[0].color = 0xff00ffff;
pcv[1].p = D3DXVECTOR4( 270.00f, 0,	1.0f, 1.0f ); pcv[1].color = 0xff00ffff;
pcv[2].p = D3DXVECTOR4( 180.00f, 480.0f,1.0f, 1.0f ); pcv[3].color = 0xff00ffff;
pcv[3].p = D3DXVECTOR4( 270.00f, 480.0f,1.0f, 1.0f ); pcv[2].color = 0xff00ffff;
m_pVB->Unlock();

//Green
m_pVB->Lock(0,0,(BYTE**)&pgv,0);
pgv[0].p = D3DXVECTOR4( 270.00f, 0,	1.0f, 1.0f ); pgv[0].color = 0xff00ff00;
pgv[1].p = D3DXVECTOR4( 360.00f, 0,	1.0f, 1.0f ); pgv[1].color = 0xff00ff00;
pgv[2].p = D3DXVECTOR4( 270.00f, 480.0f,1.0f, 1.0f ); pgv[2].color = 0xff00ff00;
pgv[3].p = D3DXVECTOR4( 360.00f, 480.0f,1.0f, 1.0f ); pgv[3].color = 0xff00ff00;
m_pVB->Unlock();

//Magenta
m_pVB->Lock(0,0,(BYTE**)&pmv,0);
pmv[0].p = D3DXVECTOR4( 360.00f, 0,	1.0f, 1.0f ); pmv[0].color = 0xffff00ff;
pmv[1].p = D3DXVECTOR4( 450.00f, 0,	1.0f, 1.0f ); pmv[1].color = 0xffff00ff;
pmv[2].p = D3DXVECTOR4( 360.00f, 480.0f,1.0f, 1.0f ); pmv[2].color = 0xffff00ff;
pmv[3].p = D3DXVECTOR4( 450.00f, 480.0f,1.0f, 1.0f ); pmv[3].color =0xffff00ff; 
m_pVB->Unlock();

//Red
m_pVB->Lock(0,0,(BYTE**)&prv,0);
prv[0].p = D3DXVECTOR4( 450.00f, 0,	1.0f, 1.0f ); prv[0].color = 0xffff0000;
prv[1].p = D3DXVECTOR4( 540.00f, 0,	1.0f, 1.0f ); prv[1].color = 0xffff0000;
prv[2].p = D3DXVECTOR4( 450.00f, 480.0f,1.0f, 1.0f ); prv[2].color =0xffff0000;
prv[3].p = D3DXVECTOR4( 540.00f, 480.0f,1.0f, 1.0f ); prv[3].color = 0xffff0000;
m_pVB->Unlock();

//Blue
m_pVB->Lock(0,0,(BYTE**)&pbv,0);
pbv[0].p = D3DXVECTOR4( 540.00f, 0,	1.0f, 1.0f ); pbv[0].color = 0xff0000ff;
pbv[1].p = D3DXVECTOR4( 630.00f, 0,	1.0f, 1.0f ); pbv[1].color = 0xff0000ff;
pbv[2].p = D3DXVECTOR4( 540.00f, 480.0f,1.0f, 1.0f ); pbv[2].color = 0xff0000ff;
pbv[3].p = D3DXVECTOR4( 630.00f, 480.0f,1.0f, 1.0f ); pbv[3].color =0xff0000ff;
m_pVB->Unlock();
//black
m_pVB->Lock(0,0,(BYTE**)&pbkv,0);
pbkv[0].p = D3DXVECTOR4( 630.00f, 0,	1.0f, 1.0f ); pbkv[0].color = 0x00000000;
pbkv[1].p = D3DXVECTOR4( 720.00f, 0,	1.0f, 1.0f ); pbkv[1].color = 0x00000000;
pbkv[2].p = D3DXVECTOR4( 630.00f, 480.0f,1.0f, 1.0f ); pbkv[2].color = 0x00000000;
pbkv[3].p = D3DXVECTOR4( 720.00f, 480.0f,1.0f, 1.0f ); pbkv[3].color =0x00000000;
m_pVB->Unlock();



	return true;

}



BOOL CXModuleTV::DrawMode4PALCB()
{

HRESULT hr;
static LPDIRECT3DVERTEXBUFFER8 m_pVB  = NULL;


if((hr = g_pd3dDevice->CreateVertexBuffer( 17*4*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB )) != D3D_OK)
	{
		ReportD3DError(hr);
		return FALSE;
	}

//PAL TV Standard for 640x576 video mode4
//White

m_pVB->Lock(0,0,(BYTE**)&pwv,0);
pwv[0].p = D3DXVECTOR4( 0, 0, 1.0f, 1.0f );     pwv[0].color = 0xffffffff;
pwv[1].p = D3DXVECTOR4( 80.0f, 0, 1.0f, 1.0f );   pwv[1].color = 0xffffffff;
pwv[2].p = D3DXVECTOR4( 0, 576.0f, 1.0f,1.0f  );   pwv[2].color = 0xffffffff;
pwv[3].p = D3DXVECTOR4( 80.0f, 576.0f,	1.0f,1.0f ); pwv[3].color = 0xffffffff;
m_pVB->Unlock();

//Yellow
m_pVB->Lock(0,0,(BYTE**)&pyv,0);
pyv[0].p = D3DXVECTOR4( 80.0f, 0,	1.0f, 1.0f ); pyv[0].color = 0xffffff00;
pyv[1].p = D3DXVECTOR4( 160.0f, 0,	1.0f, 1.0f ); pyv[1].color = 0xffffff00;
pyv[2].p = D3DXVECTOR4( 80.0f, 576.0f,1.0f, 1.0f ); pyv[2].color = 0xffffff00;
pyv[3].p = D3DXVECTOR4( 160.0f, 576.0f,1.0f, 1.0f ); pyv[3].color = 0xffffff00;
m_pVB->Unlock();

//Cyan
m_pVB->Lock(0,0,(BYTE**)&pcv,0);
pcv[0].p = D3DXVECTOR4( 160.0f, 0,	1.0f, 1.0f ); pcv[0].color = 0xff00ffff;
pcv[1].p = D3DXVECTOR4( 240.0f, 0,	1.0f, 1.0f ); pcv[1].color = 0xff00ffff;
pcv[2].p = D3DXVECTOR4( 160.0f, 576.0f,1.0f, 1.0f ); pcv[3].color = 0xff00ffff;
pcv[3].p = D3DXVECTOR4( 240.0f, 576.0f,1.0f, 1.0f ); pcv[2].color = 0xff00ffff;
m_pVB->Unlock();

//Green
m_pVB->Lock(0,0,(BYTE**)&pgv,0);
pgv[0].p = D3DXVECTOR4( 240.0f, 0,	1.0f, 1.0f ); pgv[0].color = 0xff00ff00;
pgv[1].p = D3DXVECTOR4( 320.0f, 0,	1.0f, 1.0f ); pgv[1].color = 0xff00ff00;
pgv[2].p = D3DXVECTOR4( 240.0f, 576.0f,1.0f, 1.0f ); pgv[2].color = 0xff00ff00;
pgv[3].p = D3DXVECTOR4( 320.0f, 576.0f,1.0f, 1.0f ); pgv[3].color = 0xff00ff00;
m_pVB->Unlock();

//Magenta
m_pVB->Lock(0,0,(BYTE**)&pmv,0);
pmv[0].p = D3DXVECTOR4( 320.0f, 0,	1.0f, 1.0f ); pmv[0].color = 0xffff00ff;
pmv[1].p = D3DXVECTOR4( 400.0f, 0,	1.0f, 1.0f ); pmv[1].color = 0xffff00ff;
pmv[2].p = D3DXVECTOR4( 320.0f, 576.0f,1.0f, 1.0f ); pmv[2].color = 0xffff00ff;
pmv[3].p = D3DXVECTOR4( 400.0f, 576.0f,1.0f, 1.0f ); pmv[3].color =0xffff00ff; 
m_pVB->Unlock();

//Red
m_pVB->Lock(0,0,(BYTE**)&prv,0);
prv[0].p = D3DXVECTOR4( 400.0f, 0,	1.0f, 1.0f ); prv[0].color = 0xffff0000;
prv[1].p = D3DXVECTOR4( 480.0f, 0,	1.0f, 1.0f ); prv[1].color = 0xffff0000;
prv[2].p = D3DXVECTOR4( 400.0f, 576.0f,1.0f, 1.0f ); prv[2].color =0xffff0000;
prv[3].p = D3DXVECTOR4( 480.0f, 576.0f,1.0f, 1.0f ); prv[3].color = 0xffff0000;
m_pVB->Unlock();

//Blue
m_pVB->Lock(0,0,(BYTE**)&pbv,0);
pbv[0].p = D3DXVECTOR4( 480.0f, 0,	1.0f, 1.0f ); pbv[0].color = 0xff0000ff;
pbv[1].p = D3DXVECTOR4( 560.0f, 0,	1.0f, 1.0f ); pbv[1].color = 0xff0000ff;
pbv[2].p = D3DXVECTOR4( 480.0f, 576.0f,1.0f, 1.0f ); pbv[2].color = 0xff0000ff;
pbv[3].p = D3DXVECTOR4( 560.0f, 576.0f,1.0f, 1.0f ); pbv[3].color =0xff0000ff;
m_pVB->Unlock();
//black
m_pVB->Lock(0,0,(BYTE**)&pbkv,0);
pbkv[0].p = D3DXVECTOR4( 560.0f, 0,	1.0f, 1.0f ); pbkv[0].color = 0x00000000;
pbkv[1].p = D3DXVECTOR4( 640.0f, 0,	1.0f, 1.0f ); pbkv[1].color = 0x00000000;
pbkv[2].p = D3DXVECTOR4( 560.0f, 576.0f,1.0f, 1.0f ); pbkv[2].color = 0x00000000;
pbkv[3].p = D3DXVECTOR4( 640.0f, 576.0f,1.0f, 1.0f ); pbkv[3].color =0x00000000;
m_pVB->Unlock();




return true;
}



BOOL CXModuleTV::DrawMode6PALCB()
{
	HRESULT hr;
static LPDIRECT3DVERTEXBUFFER8 m_pVB  = NULL;


if((hr = g_pd3dDevice->CreateVertexBuffer( 17*4*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB )) != D3D_OK)
	{
		ReportD3DError(hr);
		return FALSE;
	}

//PAL TV Standard for 720x576 video mode6
//White

m_pVB->Lock(0,0,(BYTE**)&pwv,0);
pwv[0].p = D3DXVECTOR4( 0, 0, 1.0f, 1.0f );     pwv[0].color = 0xffffffff;
pwv[1].p = D3DXVECTOR4( 90.00f, 0, 1.0f, 1.0f );   pwv[1].color = 0xffffffff;
pwv[2].p = D3DXVECTOR4( 0, 576.0f, 1.0f,1.0f  );   pwv[2].color = 0xffffffff;
pwv[3].p = D3DXVECTOR4( 90.00f, 576.0f,	1.0f,1.0f ); pwv[3].color = 0xffffffff;
m_pVB->Unlock();

//Yellow
m_pVB->Lock(0,0,(BYTE**)&pyv,0);
pyv[0].p = D3DXVECTOR4( 90.00f, 0,	1.0f, 1.0f ); pyv[0].color = 0xffffff00;
pyv[1].p = D3DXVECTOR4( 180.00f, 0,	1.0f, 1.0f ); pyv[1].color = 0xffffff00;
pyv[2].p = D3DXVECTOR4( 90.00f, 576.0f,1.0f, 1.0f ); pyv[2].color = 0xffffff00;
pyv[3].p = D3DXVECTOR4( 180.00f, 576.0f,1.0f, 1.0f ); pyv[3].color = 0xffffff00;
m_pVB->Unlock();

//Cyan
m_pVB->Lock(0,0,(BYTE**)&pcv,0);
pcv[0].p = D3DXVECTOR4( 180.00f, 0,	1.0f, 1.0f ); pcv[0].color = 0xff00ffff;
pcv[1].p = D3DXVECTOR4( 270.00f, 0,	1.0f, 1.0f ); pcv[1].color = 0xff00ffff;
pcv[2].p = D3DXVECTOR4( 180.00f, 576.0f,1.0f, 1.0f ); pcv[3].color = 0xff00ffff;
pcv[3].p = D3DXVECTOR4( 270.00f, 576.0f,1.0f, 1.0f ); pcv[2].color = 0xff00ffff;
m_pVB->Unlock();

//Green
m_pVB->Lock(0,0,(BYTE**)&pgv,0);
pgv[0].p = D3DXVECTOR4( 270.00f, 0,	1.0f, 1.0f ); pgv[0].color = 0xff00ff00;
pgv[1].p = D3DXVECTOR4( 360.00f, 0,	1.0f, 1.0f ); pgv[1].color = 0xff00ff00;
pgv[2].p = D3DXVECTOR4( 270.00f, 576.0f,1.0f, 1.0f ); pgv[2].color = 0xff00ff00;
pgv[3].p = D3DXVECTOR4( 360.00f, 576.0f,1.0f, 1.0f ); pgv[3].color = 0xff00ff00;
m_pVB->Unlock();

//Magenta
m_pVB->Lock(0,0,(BYTE**)&pmv,0);
pmv[0].p = D3DXVECTOR4( 360.00f, 0,	1.0f, 1.0f ); pmv[0].color = 0xffff00ff;
pmv[1].p = D3DXVECTOR4( 450.00f, 0,	1.0f, 1.0f ); pmv[1].color = 0xffff00ff;
pmv[2].p = D3DXVECTOR4( 360.00f, 576.0f,1.0f, 1.0f ); pmv[2].color = 0xffff00ff;
pmv[3].p = D3DXVECTOR4( 450.00f, 576.0f,1.0f, 1.0f ); pmv[3].color =0xffff00ff; 
m_pVB->Unlock();

//Red
m_pVB->Lock(0,0,(BYTE**)&prv,0);
prv[0].p = D3DXVECTOR4( 450.00f, 0,	1.0f, 1.0f ); prv[0].color = 0xffff0000;
prv[1].p = D3DXVECTOR4( 540.00f, 0,	1.0f, 1.0f ); prv[1].color = 0xffff0000;
prv[2].p = D3DXVECTOR4( 450.00f, 576.0f,1.0f, 1.0f ); prv[2].color =0xffff0000;
prv[3].p = D3DXVECTOR4( 540.00f, 576.0f,1.0f, 1.0f ); prv[3].color = 0xffff0000;
m_pVB->Unlock();

//Blue
m_pVB->Lock(0,0,(BYTE**)&pbv,0);
pbv[0].p = D3DXVECTOR4( 540.00f, 0,	1.0f, 1.0f ); pbv[0].color = 0xff0000ff;
pbv[1].p = D3DXVECTOR4( 630.00f, 0,	1.0f, 1.0f ); pbv[1].color = 0xff0000ff;
pbv[2].p = D3DXVECTOR4( 540.00f, 576.0f,1.0f, 1.0f ); pbv[2].color = 0xff0000ff;
pbv[3].p = D3DXVECTOR4( 630.00f, 576.0f,1.0f, 1.0f ); pbv[3].color =0xff0000ff;
m_pVB->Unlock();
//black
m_pVB->Lock(0,0,(BYTE**)&pbkv,0);
pbkv[0].p = D3DXVECTOR4( 630.00f, 0,	1.0f, 1.0f ); pbkv[0].color = 0x00000000;
pbkv[1].p = D3DXVECTOR4( 720.00f, 0,	1.0f, 1.0f ); pbkv[1].color = 0x00000000;
pbkv[2].p = D3DXVECTOR4( 630.00f, 576.0f,1.0f, 1.0f ); pbkv[2].color = 0x00000000;
pbkv[3].p = D3DXVECTOR4( 720.00f, 576.0f,1.0f, 1.0f ); pbkv[3].color =0x00000000;
m_pVB->Unlock();


	return true;

}

BOOL CXModuleTV::DrawMode3NTSCMB()
{

struct CUSTOMVERTEX{ FLOAT x, y, z, rhw; DWORD color; };
#define D3DFVF_CUSTOMVERTEX_MB (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

CUSTOMVERTEX* vtx;

//NTSC_COL_MODE3
	DWORD i,d, Col=720, pos=0, min=0, max=255, vert = 0;
	double j,Pixel_Clock=NTSC_Pixel_Clock;
	vtx= new CUSTOMVERTEX [NTSC_COL_MODE3 * 2];

//unsigned char *data=new unsigned char [BUFSIZE/(sizeof(unsigned char))];
wchar_t *data=new wchar_t [BUFSIZE/(sizeof(wchar_t))];

 

   for (i=0; i<NumFreq; i++) {
      double count=0.0;
      for (j = NTSCMode3_mulit[i].start_time;
           j < NTSCMode3_mulit[i].end_time;                     
		   j = j+1.0/Pixel_Clock) {
	    data[pos++]= (wchar_t)((double)AMP*
	       sin((double)(NTSCMode3_mulit[i].freq*2*3.1416*count))  
		   *NTSCMode3_mulit[i].amp + (NTSCMode3_mulit[i].offset)*AMP);
	       count+=1/(1000000*Pixel_Clock);
      }
      if (i<NumFreq-1) {
	     for (j = NTSCMode3_mulit[i].end_time;
		     j < NTSCMode3_mulit[i+1].start_time;                     
		     j = j+1.0/Pixel_Clock) {
	       data[pos++]=(wchar_t)(285.7*AMP);
	     }
      }

                         
   }



 for (d=0;d<Col; d++) {
     if (data[d]>max) max=data[d];
     if (data[d]<min) min=data[d];
   }


	for( d = 0; d < Col; d++)
    {
		if(d < Col)
		{	
			DWORD color;
			color = data[d];
			
			vtx[vert].x = (float)d;				vtx[vert].y = 0.0f;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 0.0f;
			vtx[vert].color =D3DCOLOR_XRGB(color,color,color);
			vert++;		
			vtx[vert].x =(float)d;					vtx[vert].y = 480;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 0.0f;
			vtx[vert].color = D3DCOLOR_XRGB(color,color,color);
    		vert++;
		
			
		}
    }


   // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( NTSC_COL_MODE3*2*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_CUSTOMVERTEX_MB,
                                                  D3DPOOL_MANAGED, &m_pVB ) ) )
    {
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
		return FALSE;
    }

   
	CUSTOMVERTEX* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
        return FALSE;
	}

    memcpy( pVertices, vtx, NTSC_COL_MODE3*2*sizeof(CUSTOMVERTEX) );
    m_pVB->Unlock();
	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(vtx);
	
    return TRUE;
}

BOOL CXModuleTV::DrawMode3PALMB()
{

struct CUSTOMVERTEX{ FLOAT x, y, z, rhw; DWORD color; };
#define D3DFVF_CUSTOMVERTEX_MB (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
CUSTOMVERTEX* vtx;



	DWORD d,i, Col=PAL_COL_MODE3, pos=0, min=0, max=255, vert = 0;
	double j,Pixel_Clock=PAL_Pixel_Clock;;
	vtx= new CUSTOMVERTEX [PAL_COL_MODE3 * 2];

//unsigned char *data=new unsigned char [BUFSIZE/(sizeof(unsigned char))];
wchar_t *data=new wchar_t [BUFSIZE/(sizeof(wchar_t))];
   for (i=0; i<NumFreq; i++) {
      double count=0.0;
      for (j = PALMode3_mulit[i].start_time;
           j < PALMode3_mulit[i].end_time;                     
		   j = j+1.0/Pixel_Clock) {
	    data[pos++]= (wchar_t)((double)AMP*
	       sin((double)(PALMode3_mulit[i].freq*2*3.1416*count))  
		   *PALMode3_mulit[i].amp + (PALMode3_mulit[i].offset)*AMP);
	       count+=1/(1000000*Pixel_Clock);
      }
      if (i<NumFreq-1) {
	     for (j = PALMode3_mulit[i].end_time;
		     j < PALMode3_mulit[i+1].start_time;                     
		     j = j+1.0/Pixel_Clock) {
	       data[pos++]=(wchar_t)(285.7*AMP);
	     }
      }

                         
   }

 for (d=0;d<Col; d++) {
     if (data[d]>max) max=data[d];
     if (data[d]<min) min=data[d];
   }


	for( d = 0; d < Col; d++)
    {
		if(d < Col)
		{	
			DWORD color;
			color = data[d];
			
			vtx[vert].x = (float)d;				vtx[vert].y = 0.0f;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 1.0f;
			vtx[vert].color =D3DCOLOR_XRGB(color,color,color);
			vert++;		
			vtx[vert].x =(float)d;					vtx[vert].y = 480;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 1.0f;
			vtx[vert].color = D3DCOLOR_XRGB(color,color,color);
    		vert++;
		
		}
    }


   // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( PAL_COL_MODE3*2*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_CUSTOMVERTEX_MB,
                                                  D3DPOOL_MANAGED, &m_pVB ) ) )
    {
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
		return FALSE;
    }

   
	CUSTOMVERTEX* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
        return FALSE;
	}

    memcpy( pVertices, vtx, PAL_COL_MODE3*2*sizeof(CUSTOMVERTEX) );
    m_pVB->Unlock();
	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(vtx);

    return true;
}

BOOL CXModuleTV::DrawMode4PALMB()
{

struct CUSTOMVERTEX{ FLOAT x, y, z, rhw; DWORD color; };
#define D3DFVF_CUSTOMVERTEX_MB (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
CUSTOMVERTEX* vtx;

	DWORD d,i, Col=PAL_COL_MODE4, pos=0, min=0, max=255, vert = 0;
	double j,Pixel_Clock=PAL_Pixel_Clock;;
	vtx= new CUSTOMVERTEX [PAL_COL_MODE4* 2];

//unsigned char *data=new unsigned char [BUFSIZE/(sizeof(unsigned char))];
wchar_t *data=new wchar_t [BUFSIZE/(sizeof(wchar_t))];

   for (i=0; i<NumFreq; i++) {
      double count=0.0;
      for (j = PAL_mulit[i].start_time;
           j < PAL_mulit[i].end_time;                     
		   j = j+1.0/Pixel_Clock) {
	    data[pos++]= (wchar_t)((double)AMP*
	       sin((double)(PAL_mulit[i].freq*2*3.1416*count))  
		   *PAL_mulit[i].amp + (PAL_mulit[i].offset)*AMP);
	       count+=1/(1000000*Pixel_Clock);
      }
      if (i<NumFreq-1) {
	     for (j = PAL_mulit[i].end_time;
		     j < PAL_mulit[i+1].start_time;                     
		     j = j+1.0/Pixel_Clock) {
	       data[pos++]=(wchar_t)(285.7*AMP);
	     }
      }

                         
   }

 for (d=0;d<Col; d++) {
     if (data[d]>max) max=data[d];
     if (data[d]<min) min=data[d];
   }


	for( d = 0; d < Col; d++)
    {
		if(d < Col)
		{	
			DWORD color;
			color = data[d];
			
			vtx[vert].x = (float)d;				vtx[vert].y = 0.0f;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 1.0f;
			vtx[vert].color =D3DCOLOR_XRGB(color,color,color);
			vert++;		
			vtx[vert].x =(float)d;					vtx[vert].y = 576;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 1.0f;
			vtx[vert].color = D3DCOLOR_XRGB(color,color,color);
    		vert++;
		
		}
    }


   // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( PAL_COL_MODE4*2*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_CUSTOMVERTEX_MB,
                                                  D3DPOOL_MANAGED, &m_pVB ) ) )
    {
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
		return FALSE;
    }

   
	CUSTOMVERTEX* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
        return FALSE;
	}

    memcpy( pVertices, vtx, PAL_COL_MODE4*2*sizeof(CUSTOMVERTEX) );
    m_pVB->Unlock();
	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(vtx);
	
    return true;
}

BOOL CXModuleTV::DrawMode6PALMB()
{

struct CUSTOMVERTEX{ FLOAT x, y, z, rhw; DWORD color; };
#define D3DFVF_CUSTOMVERTEX_MB (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
CUSTOMVERTEX* vtx;



	DWORD d,i, Col=PAL_COL_MODE6, pos=0, min=0, max=255, vert = 0;
	double j,Pixel_Clock=PAL_Pixel_Clock;;
	vtx= new CUSTOMVERTEX [PAL_COL_MODE6 * 2];

//unsigned char *data=new unsigned char [BUFSIZE/(sizeof(unsigned char))];
wchar_t *data=new wchar_t [BUFSIZE/(sizeof(wchar_t))];

   for (i=0; i<NumFreq; i++) {
      double count=0.0;
      for (j = PALMode6_mulit[i].start_time;
           j < PALMode6_mulit[i].end_time;                     
		   j = j+1.0/Pixel_Clock) {
	    data[pos++]= (wchar_t)((double)AMP*
	       sin((double)(PALMode6_mulit[i].freq*2*3.1416*count))  
		   *PALMode6_mulit[i].amp + (PALMode6_mulit[i].offset)*AMP);
	       count+=1/(1000000*Pixel_Clock);
      }
      if (i<NumFreq-1) {
	     for (j = PALMode6_mulit[i].end_time;
		     j < PALMode6_mulit[i+1].start_time;                     
		     j = j+1.0/Pixel_Clock) {
	       data[pos++]=(wchar_t)(285.7*AMP);
	     }
      }

                         
   }

 for (d=0;d<Col; d++) {
     if (data[d]>max) max=data[d];
     if (data[d]<min) min=data[d];
   }


	for( d = 0; d < Col; d++)
    {
		if(d < Col)
		{	
			DWORD color;
			color = data[d];
			
			vtx[vert].x = (float)d;				vtx[vert].y = 0.0f;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 1.0f;
			vtx[vert].color =D3DCOLOR_XRGB(color,color,color);
			vert++;		
			vtx[vert].x =(float)d;					vtx[vert].y = 576;
			vtx[vert].z = 0.0f;				vtx[vert].rhw = 1.0f;
			vtx[vert].color = D3DCOLOR_XRGB(color,color,color);
    		vert++;
		
		}
    }


   // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( PAL_COL_MODE6*2*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_CUSTOMVERTEX_MB,
                                                  D3DPOOL_MANAGED, &m_pVB ) ) )
    {
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
		return FALSE;
    }

   
	CUSTOMVERTEX* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(data);
		SAFE_DELETE_ARRAY(vtx);
        return FALSE;
	}

    memcpy( pVertices, vtx, PAL_COL_MODE6*2*sizeof(CUSTOMVERTEX) );
    m_pVB->Unlock();
	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(vtx);

    return true;
}
