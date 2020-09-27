////////////////////
// EnvDaemon.h
// saves and sets up the environment before the build
// restores the original settings afterwards.

#ifndef _ENVDAEMON_H
#define _ENVDAEMON_H

class CEnvironmentKeeper
{
	CComPtr<VCPlatform> m_pPlatform;

	CComBSTR	m_bstrPath;
	CComBSTR	m_bstrInc;
	CComBSTR	m_bstrRef;
	CComBSTR	m_bstrLib;
public:
	CEnvironmentKeeper( VCPlatform* pPlatform );
	~CEnvironmentKeeper();
};

#endif //_ENVDAEMON_H
