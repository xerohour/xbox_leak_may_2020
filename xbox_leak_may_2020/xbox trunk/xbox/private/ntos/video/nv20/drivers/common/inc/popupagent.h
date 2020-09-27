///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION
//
// Module: pupagent.h
//
// Description: This header describes the API for creating a popup tool
//				that works with the NVIDIA Stereo Driver.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef		STEREO_POPUP_AGENTS_DECL
#define		STEREO_POPUP_AGENTS_DECL

#include "StereoStats.h"
#include <windows.h>

///////////////////////////////////////////////////////////
// Forward declarations
//////////////////////////////////////////////////////////
class CPopupAgentBase;
class CVertexBuffer;
class CVertexShader;

class Vertex {
public:
	float sx; /* Screen coordinates */
	float sy;
	float sz;
	float rhw; /* Reciprocal of homogeneous w */
	DWORD color; /* Vertex color */
	DWORD specular; /* Specular component of vertex */
	float tu; /* Texture coordinates */
	float tv;
};

#define D3DFVF_POPUP_VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1)

///////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////
// Callback for each flip
typedef HRESULT (__cdecl *FlipCallbackType)(CPopupAgentBase *pPopupAgent);

// Function defined by the agent provider and accessed/called by the driver to enable the popup
typedef HRESULT (__cdecl *InitPopupAgentType)(CPopupAgentBase *pPopupAgent);
typedef HRESULT (__cdecl *DestroyPopupAgentType)(CPopupAgentBase *pPopupAgent);

///////////////////////////////////////////////////////////
// Flags
///////////////////////////////////////////////////////////
#define RTF_NOTEXTURE 1 // This flag tells the RenderTriangle(...) call to not use the texture when rendering the triangles

///////////////////////////////////////////////////////////
// CPopupAgent: Interface specification for the popup 
// agent, holds all of the information passed from the 
// driver to the external popup code.
///////////////////////////////////////////////////////////
class CPopupAgentBase {
public:
	enum // Popup Agent IDs. Used to distinguish the type of the agent.
	{
		LASER_SIGHT_ID		= 1,   
		NVIDIA_LOGO_ID		= 2,
		PERF_STATS_ID		= 3,
		STEREO_STATS_ID		= 4,
		MESSENGER_ID		= 5,
		CUSTOM_ID			= 9999,
		UNKNOWN_ID			= 0xFFFFFFFF
	};
public:
	CPopupAgentBase() : m_pvUserData(NULL), m_dwAgentID(UNKNOWN_ID), m_pNextAgent(NULL), m_dwTexture(0), m_pFlipCallback(NULL) {}

	// Sets the callback called every frame, this is where the agent will do the drawing
	void SetFlipCallback(FlipCallbackType pFlipCallback) {m_pFlipCallback = pFlipCallback;}
	FlipCallbackType GetFlipCallback() {return(m_pFlipCallback);}

	// User data functions
	void*					GetUserData() {return(m_pvUserData);}
	void					SetUserData(void *pvUserData) {m_pvUserData = pvUserData;}
	// Agent IDs allowing multiple agent providers
	DWORD					GetAgentID() {return(m_dwAgentID);}
	//void					SetAgentID(DWORD dwAgentID) {m_dwAgentID = dwAgentID;}
	CPopupAgentBase*		GetNextAgent() {return m_pNextAgent;};
    void                    SetNextAgent(CPopupAgentBase* pThat) {m_pNextAgent = pThat;};
	// Create the 16bit texture map available from the driver, initialize it with pusData
	virtual HRESULT			CreateTextureMap(DWORD dwWidth, DWORD dwHeight, DWORD dwColorKey, unsigned short *pusData) = 0;
	// Call through to the driver to render triangles
	virtual HRESULT			RenderTriangles(Vertex *pVertexData, unsigned short *pusIndexData, int nIndexCount, DWORD dwFlags) = 0;

protected:
	void                   *m_pvUserData;
	DWORD                   m_dwAgentID;
	CPopupAgentBase        *m_pNextAgent; // Points to the next agent in the list
	DWORD                   m_dwTexture;
	FlipCallbackType        m_pFlipCallback;
};

class CTexture;
class CNvObject;

class CPopupAgent : public CPopupAgentBase 
{
protected: //attributes
	CTexture*				pTexture;
	CNvObject*				pTextureObj;
	NVD3DCONTEXT*			pContext;
	HMODULE					hMod;
	CVertexBuffer*			pVB;
	CVertexShader*			pVS;
protected: //methods
	void					destroyTexture();
	void					destroyVertexData();
	HRESULT					createVertexData();
	HRESULT					setupStates(DWORD dwFlags);
	HRESULT					restoreStates(DWORD dwFlags);

public: //methods
	CPopupAgent();
	virtual ~CPopupAgent();
	
    HRESULT         bindPopupAgent( HMODULE hmod );
	HRESULT			unbindPopupAgent( );
	void			setContext(NVD3DCONTEXT* aContext) {pContext = aContext;};
	
	virtual HRESULT CreateTextureMap(DWORD dwWidth, DWORD dwHeight, DWORD dwColorKey, unsigned short *pusData);
	virtual HRESULT RenderTriangles(Vertex *pVertexData, unsigned short *pusIndexData, int nIndexCount, DWORD dwFlags);
};
/*
class CPerfStatPopup: public CPopupAgent, protected STEREOSTATS  
{
public:
	CPerfStatPopup();
	virtual ~CPerfStatPopup();
	
	STEREOSTATS*	get() {return (STEREOSTATS*)this;}; 
	void			setContext(NVD3DCONTEXT* aContext); //extract performance data from context
};

class CPopupAgentList
{
protected:
	CPopupAgent*	popupAgentList;
public:
	CPopupAgentList();
	~CPopupAgentList();
	HRESULT			create();
	HRESULT			destroy();
	HRESULT			render(NVD3DCONTEXT *pContext);
};
  */
  /*
//exported functions (to nvStereo.cpp)
extern	HRESULT			createPopupList();
extern	HRESULT			destroyPopupList();
extern	HRESULT			renderPopupList(NVD3DCONTEXT *pContext);
*/
#endif
