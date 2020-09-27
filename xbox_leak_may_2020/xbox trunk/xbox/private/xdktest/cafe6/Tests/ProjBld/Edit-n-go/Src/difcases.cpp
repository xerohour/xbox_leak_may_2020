///////////////////////////////////////////////////////////////////////////////
//	DIFCASES.CPP
//
//	Created by :			Date :
//		WayneBr				9/28/94
//					  
//	Description :
//		Portable Edit'n Go performance sniff tests

#include "stdafx.h"
#include "percases.h"
#include "difcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

Project_Info  proj_windiff;
Project_Info  *pProj_WinDiff=&proj_windiff;

IMPLEMENT_TEST(CWinDiffCases, CPerformanceCases, "Med.  C:       V2 WinDiff", -1, CSniffDriver)

void CWinDiffCases::Run(void)
{
	//initialization for all tests goes here
	Initialize_WinDiff();
	pProj_WinDiff->Time_Start_IDE = GetSubSuite()->GetIDE()->m_dwLaunchTime;

	CPerformanceCases::Run(pProj_WinDiff);
};

void CWinDiffCases::Initialize_WinDiff() {
	pProj_WinDiff->Project=		"V2 WinDiff sample";
	pProj_WinDiff->SrcProjDir=	"windiff.v2";
	pProj_WinDiff->ProjDir=		"windiff";
	pProj_WinDiff->Makefile=	"windiff\\windiff.mak";
	pProj_WinDiff->Dep_Make=	"windiff\\gutils.mak";
	pProj_WinDiff->Target=		"target"; 
	pProj_WinDiff->AddFcn=		FCN;
	pProj_WinDiff->AddFcn_File=	"windiff.c";
	pProj_WinDiff->AddFcn_Line=	770;
	pProj_WinDiff->AddDef=		"~void Pretend_Dump{(}{)};~";
	pProj_WinDiff->AddDef_File=	"windiff.c";
	pProj_WinDiff->AddDef_Line=	318;								   
	pProj_WinDiff->AddRef=		"~Pretend_Dump{(}{)};~";
	pProj_WinDiff->AddRef_File=	"windiff.c";
	pProj_WinDiff->AddRef_Line=	381;								   
	pProj_WinDiff->ModCode=		MOD;
	pProj_WinDiff->Disk_Before=	0;
	pProj_WinDiff->Disk_After=	0;
};
