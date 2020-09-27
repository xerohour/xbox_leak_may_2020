//-----------------------------------------------------------------------------
//  
//  File: Resource.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
//	Wrapper around CXBPackedResource that does asynchronous reading 
//  of the resource file.
//  
//-----------------------------------------------------------------------------
#pragma once
#include <XBResource.h>

// $BUGBUG 11551
#define MAX_NUM_RESOURCES 10000

enum LOADINGSTATE { LOADING_NOTSTARTED, LOADING_HEADER, LOADING_DATA, LOADING_DONE, LOADING_FAILED };

class Resource : public CXBPackedResource
{
 public:
	Resource();
	~Resource();

	// Accessors
	DWORD Count() 					{ return m_dwNumResources; }
	DWORD GetTypeByIndex(DWORD ResourceIndex) 
		{ if (ResourceIndex >= m_dwNumResources) return 0; else return m_dwTypes[ResourceIndex]; }
	VOID *GetResourceByIndex(DWORD ResourceIndex)
		{ if (ResourceIndex >= m_dwNumResources) return NULL; else return m_ppResources[ResourceIndex]; }
	LOADINGSTATE CurrentLoadingState() { return m_LoadingState; }

    // Starts asynchronous loading of the resources out of the specified bundle
    HRESULT StartLoading( LPDIRECT3DDEVICE8 pDevice, LPSTR strFileBase );

	// Update status of loading state
	LOADINGSTATE PollLoadingState();

    // Called when Async I/O is complete to register the resources
    HRESULT OnIOComplete();

    // Unloads the resources.
    HRESULT Unload();	

	// Called by Unload to release handles, etc. to cleanup the resource before unloading.
	virtual HRESULT Cleanup(DWORD dwType,	// resource type
							BYTE *pHeader);	// pointer to resource header

	// Called by OnIOComplete to patch pointers.
	// Default Patch implementation calls register on standard resource types.
	virtual HRESULT Patch(DWORD dwType,		// resource type
						  BYTE *pHeader);	// pointer to resource header

 protected:
	LPDIRECT3DDEVICE8		m_pd3dDevice;				// for resource registration
    BYTE *				    m_ppResources[MAX_NUM_RESOURCES]; // Array of pointers to resources
    DWORD                   m_dwTypes[MAX_NUM_RESOURCES]; // Array of resource types
    HANDLE                  m_hfXPR;                // File handle for async i/o
    DWORD                   m_cbHeaders;            // Count of bytes of resource headers
    DWORD                   m_cbData;               // Count of bytes of data
    OVERLAPPED              m_overlapped;           // OVERLAPPED structure for async I/O
    LOADINGSTATE            m_LoadingState;             // current loading state
};
