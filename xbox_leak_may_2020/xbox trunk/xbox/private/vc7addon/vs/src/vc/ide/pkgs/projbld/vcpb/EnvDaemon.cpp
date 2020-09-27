////////////////////
// EnvDaemon.cpp
// saves and sets up the environment before the build
// restores the original settings afterwards.

#include "stdafx.h"
#include "envdaemon.h"
#include "vcprojectengine.h"
#include "util2.h"

CEnvironmentKeeper::CEnvironmentKeeper( VCPlatform* pPlatform ) :
	m_pPlatform( pPlatform )
{
	if( !m_pPlatform )
		return;

	// first, save the PATH, INC, and LIB environment variables
	// get the environment variables for the platform parameters
	VCGetEnvironmentVariableW(L"PATH", &m_bstrPath);
	VCGetEnvironmentVariableW(L"INCLUDE", &m_bstrInc);
	VCGetEnvironmentVariableW(L"LIBPATH", &m_bstrRef);
	VCGetEnvironmentVariableW(L"LIB", &m_bstrLib);

	// now we mangle the global environment with the platform's settings 
	// (from the tools dialog)

	HRESULT hr;
	CComQIPtr<IVCPropertyContainer> pPropCnt;

	// get the project collection
	CComPtr<IDispatch> pDispColl;
	g_pProjectEngine->get_Projects( &pDispColl );
	CComQIPtr<IVCCollection> pProjectColl = pDispColl;
	if( pProjectColl )
	{
		CComPtr<IDispatch> pDisp;
		// get the 1st project
		hr = pProjectColl->Item( CComVariant( 1 ), &pDisp );
		if( SUCCEEDED( hr ) )
		{
			CComQIPtr<VCProject> pProject = pDisp;
			if( pProject )
			{
				// get the config collection
				pDispColl = NULL;
				pProject->get_Configurations( &pDispColl );
				CComQIPtr<IVCCollection> pConfigColl = pDispColl;
				if( pConfigColl )
				{
					pDisp = NULL;
					// get the 1st config
					hr = pConfigColl->Item( CComVariant( 1 ), &pDisp );
					if( SUCCEEDED( hr ) )
					{
						CComQIPtr<VCConfiguration> pConfig = pDisp;
						if( pConfig )
							pPropCnt = pConfig;
					}
				}
			}
		}
	}
	if (pPropCnt == NULL)	// must have gotten here through attach followed by edit & continue
		pPropCnt = g_pProjectEngine;
	
	CComBSTR strExe;
	CComBSTR strInc;
	CComBSTR strRef;
	CComBSTR strLib;
	m_pPlatform->get_ExecutableDirectories( &strExe );
	m_pPlatform->get_IncludeDirectories( &strInc );
	m_pPlatform->get_ReferenceDirectories( &strRef );
	m_pPlatform->get_LibraryDirectories( &strLib );
	// if we got a config, we can expand env var macros
	if( pPropCnt )
	{
		CComBSTR bstrExpandedExe;
		pPropCnt->Evaluate( strExe, &bstrExpandedExe );
		::SetEnvironmentVariableW( L"PATH", bstrExpandedExe );

		CComBSTR bstrExpandedInc;
		pPropCnt->Evaluate( strInc, &bstrExpandedInc );
		::SetEnvironmentVariableW( L"INCLUDE", bstrExpandedInc );

		CComBSTR bstrExpandedRef;
		pPropCnt->Evaluate( strRef, &bstrExpandedRef );
		::SetEnvironmentVariableW( L"LIBPATH", bstrExpandedRef );

		CComBSTR bstrExpandedLib;
		pPropCnt->Evaluate( strLib, &bstrExpandedLib );
		::SetEnvironmentVariableW( L"LIB", bstrExpandedLib );
	}
	// otherwise we can't, and we're probably in trouble
	else
	{
		::SetEnvironmentVariableW( L"PATH", strExe );
		::SetEnvironmentVariableW( L"INCLUDE", strInc );
		::SetEnvironmentVariableW( L"LIBPATH", strRef );
		::SetEnvironmentVariableW( L"LIB", strLib );
	}
}


CEnvironmentKeeper::~CEnvironmentKeeper()
{
	if( !m_pPlatform )
		return;

	// set the environment variables with the cached values.
	::SetEnvironmentVariableW( L"PATH", m_bstrPath );
	::SetEnvironmentVariableW( L"INCLUDE", m_bstrInc );
	::SetEnvironmentVariableW( L"LIBPATH", m_bstrRef );
	::SetEnvironmentVariableW( L"LIB", m_bstrLib );

}

