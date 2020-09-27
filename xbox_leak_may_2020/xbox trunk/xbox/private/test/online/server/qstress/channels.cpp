/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../qtest/qtest_i.c"
#include "channels.h"

#define SENDBUFFERS 20
#define SENDBUFFERSIZE 4096
#define RECEIVEBUFFERSIZE 0

Channels::Channels(UINT uicChannels):
m_uicChannels(uicChannels),
m_rgChannels(NULL),
m_fInit(false)
{

	//Allocate channels
	m_rgChannels=new Channel[uicChannels];
	_ASSERT(NULL!=m_rgChannels);
	if (NULL==m_rgChannels){
		return;
	}//endif

	//Zero array of structures
	ZeroMemory(m_rgChannels,sizeof(Channel)*m_uicChannels);

	HRESULT hr=S_OK;
	IClassFactory* pCF;

	//Create class factory
	hr=CoGetClassObject(CLSID_engine, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&pCF); 
	_ASSERT(SUCCEEDED(hr));
	if (FAILED(hr)){
		return;
	}//endif

	//Get Iengine and IengineEx interfaces
	for (UINT ui=0; ui<m_uicChannels; ui++){
		hr = pCF->CreateInstance(NULL, IID_Iengine, (void**)&(m_rgChannels[ui].pEngine));
		_ASSERT(SUCCEEDED(hr));
		if (FAILED(hr)){
			return;
		}//endif
		hr = m_rgChannels[ui].pEngine->QueryInterface(IID_IengineEx,(void**)&(m_rgChannels[ui].pEngineEx));
		_ASSERT(SUCCEEDED(hr));
		if (FAILED(hr)){
			return;
		}//endif

		hr = m_rgChannels[ui].pEngineEx->InitEx(SENDBUFFERS,SENDBUFFERSIZE,RECEIVEBUFFERSIZE);
		_ASSERT(SUCCEEDED(hr));
		if (FAILED(hr)){
			return;
		}//endif

	}//endfor

	//Release class factory
	pCF->Release(); 

	//Initialization success
	m_fInit=true;

}//endmethod

Channels::~Channels(){

	//Deallocate channels
	if (NULL!=m_rgChannels){

		for (UINT ui=0; ui<m_uicChannels; ui++){
			m_rgChannels[ui].pEngineEx->CleanupEx();
			if (NULL!=m_rgChannels[ui].pEngine){
				m_rgChannels[ui].pEngine->Release();
			}//endif
			if (NULL!=m_rgChannels[ui].pEngineEx){
				m_rgChannels[ui].pEngineEx->Release();
			}//endif
		}//endfor

		delete [] m_rgChannels;
		m_rgChannels=NULL;

	}//endif

}//endmethod