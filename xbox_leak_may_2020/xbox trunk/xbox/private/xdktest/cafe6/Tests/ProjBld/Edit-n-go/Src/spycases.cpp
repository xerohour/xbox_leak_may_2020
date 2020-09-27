///////////////////////////////////////////////////////////////////////////////
//	SPYCASES.CPP
//
//	Created by :			Date :
//		WayneBr				9/28/94
//					  
//	Description :
//		Portable Edit'n Go performance sniff tests

#include "stdafx.h"
#include "percases.h"
#include "spycases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

Project_Info  proj_spy;
Project_Info  *pProj_Spy=&proj_spy;

IMPLEMENT_TEST(CSpyCases, CPerformanceCases, "Med.  C++/MFC: V4 Spy++", -1, CSniffDriver)

void CSpyCases::Run(void)
{
	// Function to add 
	FCN="~void CMsgDoc::Pretend_Dump{(}{)} {{}{TAB}//New fuction to test incremental builds~{TAB}int x=1; //Delete_This_Later~int y;~int z; //Delete_This_Later~~y=2;~z=3.7;~z=y/z;~OutputDebugString{(}\"Pretend dump\"{)};~+{TAB}{}};~";

	//initialization for all tests goes here
	Initialize_Spy();
	pProj_Spy->Time_Start_IDE = GetSubSuite()->GetIDE()->m_dwLaunchTime;

	CPerformanceCases::Run(pProj_Spy);
};


void CSpyCases::Initialize_Spy() {
	pProj_Spy->Project=		"V4 Spy++";
	pProj_Spy->SrcProjDir=	"spyxx.v4";
	pProj_Spy->ProjDir=		"spyxx";
	pProj_Spy->Makefile=	"spyxx\\spyxx2.mak";
	pProj_Spy->Dep_Make=	"spyxx\\spyxxhk2.mak";	
	pProj_Spy->Target=		"target"; 
	pProj_Spy->AddFcn=		FCN;
	pProj_Spy->AddFcn_File=	"msgdoc.cpp";
	pProj_Spy->AddFcn_Line=	1450;
	pProj_Spy->AddDef=		"~void Pretend_Dump{(}{)};~";
	pProj_Spy->AddDef_File=	"msgdoc.h";
	pProj_Spy->AddDef_Line=	99;								   
	pProj_Spy->AddRef=		"~Pretend_Dump{(}{)};~";
	pProj_Spy->AddRef_File=	"msgdoc.cpp";
	pProj_Spy->AddRef_Line=	37;								   
	pProj_Spy->ModCode=		MOD;
	pProj_Spy->Disk_Before=	0;
	pProj_Spy->Disk_After=	0;
};

