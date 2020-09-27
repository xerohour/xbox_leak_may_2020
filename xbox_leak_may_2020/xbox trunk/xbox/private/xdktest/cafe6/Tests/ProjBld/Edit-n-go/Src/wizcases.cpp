///////////////////////////////////////////////////////////////////////////////
//	WIZCASES.CPP
//
//	Created by :			Date :
//		WayneBr				9/28/94
//					  
//	Description :
//		Portable Edit'n Go performance sniff tests

#include "stdafx.h"
#include "percases.h"
#include "wizcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

Project_Info  proj_def_appwiz;
Project_Info  *pProj_Def_Appwiz=&proj_def_appwiz;

IMPLEMENT_TEST(CDefAppWizCases, CPerformanceCases, "Small C++/MFC: V2 DefAppWiz", -1, CSniffDriver)

void CDefAppWizCases::Run(void)
{
	// Function to add 
	FCN="~void CDefApp::Pretend_Dump{(}{)} {{}{TAB}//New fuction to test incremental builds~{TAB}int x=1; //Delete_This_Later~int y;~int z; //Delete_This_Later~~y=2;~z=3.7;~z=y/z;~OutputDebugString{(}\"Pretend dump\"{)};~+{TAB}{}};~";

	//initialization for all tests goes here
	Initialize_Def_Appwiz();
	pProj_Def_Appwiz->Time_Start_IDE = GetSubSuite()->GetIDE()->m_dwLaunchTime;

	CPerformanceCases::Run(pProj_Def_Appwiz);
};
	

void CDefAppWizCases::Initialize_Def_Appwiz() {
	pProj_Def_Appwiz->Project=		"V2 Default Appwizard";
	pProj_Def_Appwiz->SrcProjDir=	"appwiz.v2";
	pProj_Def_Appwiz->ProjDir=		"appwiz";
	pProj_Def_Appwiz->Makefile=		"appwiz\\def.mak";
	pProj_Def_Appwiz->Dep_Make=		"";	// None
	pProj_Def_Appwiz->Target=		"target"; 
	pProj_Def_Appwiz->AddFcn=		FCN;
	pProj_Def_Appwiz->AddFcn_File=	"def.cpp";
//	pProj_Def_Appwiz->AddFcn_Old=	"defcpp.old";
	pProj_Def_Appwiz->AddFcn_Line=	153;
	pProj_Def_Appwiz->AddDef=		"~void Pretend_Dump{(}{)};~";
	pProj_Def_Appwiz->AddDef_File=	"def.h";
//	pProj_Def_Appwiz->AddDef_Old=	"defh.old";
	pProj_Def_Appwiz->AddDef_Line=	19;								   
	pProj_Def_Appwiz->AddRef=		"~Pretend_Dump{(}{)};~";
	pProj_Def_Appwiz->AddRef_File=	"def.cpp";
//	pProj_Def_Appwiz->AddRef_Old=	"defcpp.old";
	pProj_Def_Appwiz->AddRef_Line=	57;								   
	pProj_Def_Appwiz->ModCode=		MOD;
	pProj_Def_Appwiz->Disk_Before=	0;
	pProj_Def_Appwiz->Disk_After=	0;
};
