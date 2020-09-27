//////////////////////////////////////////////////////////////////
//	toolexpt.CPP
//
//		Definition of interfaces defined in toolexpt.H
//		Import functions from VCPP
///////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
#include "toolexpt.h"
#include "tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------- CIdeInterface ------------------------------
//
///////////////////////////////////////////////////////////////////////////////

CIdeInterface::CIdeInterface()
{ 
	m_pEditDebugStatus = NULL;
	m_pSourceQuery = NULL;
	m_pProjectWorkspace = NULL;
	m_pOutputWindow = NULL;
	m_pBuildSystem = NULL;

	m_pWizBarIFace = NULL;
	m_fTriedWizBarIFace = FALSE;
}

CIdeInterface::~CIdeInterface()
{ 
}

///////////////////////////
//
// CIdeInterface::Initialize()
//
//	must be called in the beginning of the program
////////////////////////////

BOOL CIdeInterface::Initialize()
{
	ASSERT(this == &g_IdeInterface);

	GetEditDebugStatus();
	GetSourceQuery();
	GetOutputWindow();
	GetBuildSystem();

	return((m_pEditDebugStatus != NULL) && (m_pSourceQuery != NULL) &&
			(m_pOutputWindow != NULL) && (m_pBuildSystem != NULL));
}

void CIdeInterface::Terminate()
{
	ASSERT(this == &g_IdeInterface);

	ReleaseEditDebugStatus();
	ReleaseSourceQuery();
	ReleaseProjectWorkspace();
	ReleaseOutputWindow();
	ReleaseBuildSystem();

	ReleaseWizBarIFace();
}

CIdeInterface g_IdeInterface;
