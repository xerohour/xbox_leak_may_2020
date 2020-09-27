///////////////////////////////////////////////////////////////////////////////
//	MPDCASES.CPP
//
//	Created by :			Date :
//		WayneBr				9/28/94
//					  
//	Description :
//		Portable Edit'n Go performance sniff tests

#include "stdafx.h"
#include "percases.h"
#include "mpdcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

Project_Info  proj_multipad;
Project_Info  *pProj_Multipad=&proj_multipad;

IMPLEMENT_TEST(CMultipadCases, CPerformanceCases, "Small C:       V2 Multipad", -1, CSniffDriver)

void CMultipadCases::Run(void)
{
	//initialization for all tests goes here
	Initialize_Multipad();
	pProj_Multipad->Time_Start_IDE = GetSubSuite()->GetIDE()->m_dwLaunchTime;

	CPerformanceCases::Run(pProj_Multipad);
};

void CMultipadCases::Initialize_Multipad() {
	pProj_Multipad->Project=	"V2 Multipad sample";
	pProj_Multipad->SrcProjDir=	"multipad.v2";
	pProj_Multipad->ProjDir=	"multipad";
	pProj_Multipad->Makefile=	"multipad\\multipad.mak";
	pProj_Multipad->Dep_Make=	"";	// None
	pProj_Multipad->Target=		"target"; 
	pProj_Multipad->AddFcn=		FCN;
	pProj_Multipad->AddFcn_File="multipad.c";
	pProj_Multipad->AddFcn_Line=984;
	pProj_Multipad->AddDef=		"~extern void Pretend_Dump{(}{)};~";
	pProj_Multipad->AddDef_File="multipad.h";
	pProj_Multipad->AddDef_Line=204;								   
	pProj_Multipad->AddRef=		"~Pretend_Dump{(}{)};~";
	pProj_Multipad->AddRef_File="multipad.c";
	pProj_Multipad->AddRef_Line=107;								   
	pProj_Multipad->ModCode=	MOD;
	pProj_Multipad->Disk_Before=0;
	pProj_Multipad->Disk_After=	0;
};
