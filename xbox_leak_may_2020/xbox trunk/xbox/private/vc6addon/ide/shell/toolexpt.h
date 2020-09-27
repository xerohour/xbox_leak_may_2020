//////////////////////////////////////////////////////////////////
//	toolexpt.H
//
//			Inteface for importing functions from VCPP
//
//////////////////////////////////////////////////////////////////
#ifndef _TOOLEXPT_H_
#define _TOOLEXPT_H_

#include "tools.h"

#include <srcapi.h>
#include <srcguid.h>
#include <bldapi.h>
#include <bldguid.h>
#include <prjapi.h>
#include <prjguid.h>
#include <clvwapi.h>
#include <clvwguid.h>

// Class CIdeInterface:	Class to interface to the IDE

class CIdeInterface 
{
	friend class CSpawner;

public:
	CIdeInterface();
	~CIdeInterface();
	
	BOOL Initialize();
	void Terminate();

	// cache the spawner interface
	LPEDITDEBUGSTATUS GetEditDebugStatus()
	{
		if (m_pEditDebugStatus == NULL)
			theApp.FindInterface(IID_IEditDebugStatus, (LPVOID FAR *)&m_pEditDebugStatus);
		return(m_pEditDebugStatus);
	}

	void ReleaseEditDebugStatus()
	{
		if (m_pEditDebugStatus != NULL)
			m_pEditDebugStatus->Release();
	}

	// cache the source query interface
	LPSOURCEQUERY GetSourceQuery()
	{
		if (m_pSourceQuery == NULL)
			theApp.FindInterface(IID_ISourceQuery, (LPVOID FAR *)&m_pSourceQuery);
		return(m_pSourceQuery);
	}

	void ReleaseSourceQuery()
	{
		if (m_pSourceQuery != NULL)
			m_pSourceQuery->Release();
	}

	// Cache the project workspace interface
	LPPROJECTWORKSPACE GetProjectWorkspace()
	{
		if(m_pProjectWorkspace == NULL)
			theApp.FindInterface(IID_IProjectWorkspace, (void **)&m_pProjectWorkspace);
		return m_pProjectWorkspace;
	}

	void ReleaseProjectWorkspace()
	{
		if(m_pProjectWorkspace != NULL)
			m_pProjectWorkspace->Release();
		m_pProjectWorkspace = NULL;
	}


	// cache the output window interface
	LPOUTPUTWINDOW GetOutputWindow()
	{
		if (m_pOutputWindow == NULL)
			theApp.FindInterface(IID_IOutputWindow, (LPVOID FAR *)&m_pOutputWindow);
		return(m_pOutputWindow);
	}

	void ReleaseOutputWindow()
	{
		if (m_pOutputWindow != NULL)
			m_pOutputWindow->Release();
	}

	// cache the build system interface
	LPBUILDSYSTEM GetBuildSystem()
	{
		if (m_pBuildSystem == NULL)
			theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&m_pBuildSystem);
		return(m_pBuildSystem);
	}

	void ReleaseBuildSystem()
	{
		if (m_pBuildSystem != NULL)
			m_pBuildSystem->Release();
	}

	// cache the wizard bar interface
	LPWIZARDBAR GetWizBarIFace()
	{
		if (NULL == m_pWizBarIFace && !m_fTriedWizBarIFace)
		{
			theApp.FindInterface(IID_IWizardBar, (LPVOID FAR *)&m_pWizBarIFace);
			m_fTriedWizBarIFace = TRUE;	  // more efficient if iface is NOT found
		}
		return m_pWizBarIFace;
	}

	void ReleaseWizBarIFace()
	{
		if (m_pWizBarIFace != NULL)
		{
			m_pWizBarIFace->Release();
			m_pWizBarIFace = NULL;
			m_fTriedWizBarIFace = FALSE;
		}
	}



private:
	LPEDITDEBUGSTATUS	m_pEditDebugStatus;
	LPSOURCEQUERY		m_pSourceQuery;
	LPPROJECTWORKSPACE	m_pProjectWorkspace;
	LPOUTPUTWINDOW		m_pOutputWindow;
	LPBUILDSYSTEM		m_pBuildSystem;
	LPWIZARDBAR			m_pWizBarIFace;
	BOOL				m_fTriedWizBarIFace;

};

extern CIdeInterface g_IdeInterface;

#endif	 // _TOOLEXPT_H_
