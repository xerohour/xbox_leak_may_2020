///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION
//
// Module: nvPopupAgent.h
//
// Description: implements CPopupAgent class being declared in common/PopupAgent.h
//				that works with the NVIDIA Stereo Driver.
//            
///////////////////////////////////////////////////////////////////////////////
//                                                               
// History:                                                                  
//       02/16/01  Created       
//                         
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "nvprecomp.h"
#include "nvPopupList.h"

#ifndef WINNT
	#define IMPLEMENT_POPUP_AGENT
#endif //WINNT

#ifdef IMPLEMENT_POPUP_AGENT

///////////////////////////////////////////////////////////////////////////////////////////////////
//export stuff from nvStereo.cpp

//data/events/errors logging
extern void __cdecl LOG(char * szFormat, ...);
//stereo configuration -- need it for statistics
extern CONFIG_ASSIST_INFO ConfigAssistInfo;
///////////////////////////////////////////////////////////////////////////////////////////////////
CPopupAgent*	popupAgentList=0; 
///////////////////////////////////////////////////////////////////////////////////////////////////
#define DEFAULT_MODULE_NAME "nvstres.dll"
///////////////////////////////////////////////////////////////////////////////////////////////////
CPopupAgent::CPopupAgent()
{
	pTexture=0;
	pTextureObj=0;
	pContext=0;
	hMod=0;

	pVB=0;
	pVS=0;

	HRESULT rc=createVertexData();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
CPopupAgent::~CPopupAgent()
{
	destroyTexture();
	destroyVertexData();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
#define SAFE_DELETE(p)  if(p) {delete p;p=0;}
#define SAFE_RELEASE(p) if(p) {p->release();p=0;}
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PopupStateSet_s
{
    DWORD state;
    DWORD value;
    DWORD oldvalue;
} PopupStateSet;
///////////////////////////////////////////////////////////////////////////////////////////////////
static PopupStateSet popupStatesSolid[] =
{
//for non-textured:
		{ D3DRENDERSTATE_CULLMODE,         D3DCULL_NONE             ,0},
		{ D3DRENDERSTATE_ZFUNC,            D3DCMP_ALWAYS            ,0},
		{ D3DRENDERSTATE_ALPHAFUNC,        D3DCMP_ALWAYS            ,0},
		{ D3DRENDERSTATE_ALPHABLENDENABLE, FALSE                    ,0},
		{ D3DRENDERSTATE_STENCILENABLE,    FALSE                    ,0},
		{ D3DRENDERSTATE_FILLMODE,         D3DFILL_SOLID            ,0},
		{ D3DRENDERSTATE_FOGENABLE,        FALSE                    ,0},
		{ D3DRENDERSTATE_TEXTUREMAPBLEND,  D3DPTBLENDCAPS_MODULATE  ,0},
		{ D3DRENDERSTATE_SPECULARENABLE,   FALSE                    ,0},
		//-------<< terminator HAS TO BE HERE >>-------------------------
		{0,0},
};
///////////////////////////////////////////////////////////////////////////////////////////////////
static PopupStateSet popupStatesTextured[] =
{
//for textured:
    { D3DRENDERSTATE_CULLMODE,         D3DCULL_NONE             ,0},
    { D3DRENDERSTATE_ZFUNC,            D3DCMP_ALWAYS            ,0},
    { D3DRENDERSTATE_ALPHAFUNC,        D3DCMP_ALWAYS            ,0},
    { D3DRENDERSTATE_ALPHABLENDENABLE, TRUE                     ,0},
    { D3DRENDERSTATE_SRCBLEND,         D3DBLEND_SRCALPHA        ,0},
    { D3DRENDERSTATE_DESTBLEND,        D3DBLEND_INVSRCALPHA     ,0},
    { D3DRENDERSTATE_WRAP0,            0                        ,0},
	{ D3DRENDERSTATE_STENCILENABLE,    FALSE                    ,0},
    { D3DRENDERSTATE_FILLMODE,         D3DFILL_SOLID            ,0},
    { D3DRENDERSTATE_FOGENABLE,        FALSE                    ,0},
    { D3DRENDERSTATE_WRAPU,            0                        ,0},
    { D3DRENDERSTATE_WRAPV,            0                        ,0},
    { D3DRENDERSTATE_TEXTUREMAPBLEND,  D3DPTBLENDCAPS_MODULATE  ,0},
    { D3DRENDERSTATE_SPECULARENABLE,   FALSE                    ,0},
	//-------<< terminator HAS TO BE HERE >>-------------------------
	{0,0}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
static NVD3DTEXSTAGESTATE tssPrevState[2];
///////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupAgent::destroyTexture()
{
	SAFE_RELEASE(pTextureObj);
	pTexture=0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupAgent::destroyVertexData()
{
	SAFE_DELETE(pVB);
	SAFE_DELETE(pVS);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CPopupAgent::createVertexData()
{
    pVB = new CVertexBuffer;
    if (!pVB) 
	{
        DPF("Unable to create popup vertex buffer");
        nvAssert(0);
        return E_FAIL;
    }

    pVS = new CVertexShader;
    if (!pVS) 
	{
        DPF("Unable to create popup vertex shader");
        nvAssert(0);
        return E_FAIL;
    }
	return S_OK;
}

// load library and call Init ////////////////////////////////////////////////////////////////////////
//
// [in] const char* module -- module name
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CPopupAgent::bindPopupAgent( HMODULE hmod )
{
	nvAssert(hmod);
	hMod = hmod;

	InitPopupAgentType		pInitPopupAgent=0;
	pInitPopupAgent   = (InitPopupAgentType)    GetProcAddress(hMod, "InitPopupAgent");
	
	nvAssert(pInitPopupAgent);

	if(pInitPopupAgent)
		pInitPopupAgent(this);

	//assert whether it's a valid popup
	nvAssert(m_pFlipCallback);
	
	return S_OK;
}
// call DestroyPopup and free library ////////////////////////////////////////////////////////////
HRESULT CPopupAgent::unbindPopupAgent( )
{
	
	if(!hMod) 
	{
		DPF("CPopupAgent: trying to unbind non-loaded library" );
		nvAssert(0);
		return E_FAIL;
	}
	
	DestroyPopupAgentType	pDestroyPopupAgent=0;
	pDestroyPopupAgent= (DestroyPopupAgentType) GetProcAddress(hMod, "DestroyPopupAgent");
	
	nvAssert(pDestroyPopupAgent);
	
	if(pDestroyPopupAgent)
		pDestroyPopupAgent(this);
	
	hMod = 0;
	
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CPopupAgent::CreateTextureMap(DWORD dwWidth, DWORD dwHeight, DWORD dwColorKey, unsigned short *pusData)
{
	DWORD				hei=dwHeight;
	DWORD				wid=dwWidth;
	DWORD				pitch;
	WORD				wColorKey;
	DWORD				opaque = 0xFF000000;
	DWORD*				cp=0;				//current texel
	unsigned short*		source=pusData;		//source of texels (16 bpp anticipated)
	DWORD				i,j;

	//alternatively, we can bail out here if texture has been already created
	destroyTexture();

/////////////////////////////////////////////////////////////////////////////////////
//#define	TEST_CREATE_POPUP_TEXTURE
#ifdef	TEST_CREATE_POPUP_TEXTURE
		const T_W=32;
		const T_H=32;
		static unsigned short test[T_W*T_H];

		//some nice pattern:
		for(i=0; i<T_W*T_H; i++) 
			test[i]= ((i&T_W<<2) | ~(i&T_H))<<4; 

		//now, concoct parameters:
		wid = T_W;
		hei = T_H;
		wColorKey = 0;
		source = test;
#endif	//TEST_CREATE_POPUP_TEXTURE
/////////////////////////////////////////////////////////////////////////////////////

	pTexture = new CTexture;
	if (!pTexture)
	{
		DPF("CPopupAgent::CreateTextureMap: new CTexture failed");
		return E_FAIL;
	}

	pTextureObj = new CNvObject(0);
	if (!pTextureObj)
	{
		DPF("CPopupAgent::CreateTextureMap: new CNvObject failed");
		destroyTexture();
		return E_FAIL;
	}
#ifdef WINNT
    pTextureObj->setDDSLcl(NULL);
#endif

	pTextureObj->setObject(CNvObject::NVOBJ_TEXTURE, pTexture);

	BOOL rc = pTexture->create (pTextureObj, wid, hei, 1, 4, 4, 1, NV_SURFACE_FORMAT_A8R8G8B8,
									  CSimpleSurface::HEAP_VID, CSimpleSurface::HEAP_VID);

	if (!rc) // allocation failed
	{
		DPF("CPopupAgent::CreateTextureMap: Couldn't create texture for popup agent");
		destroyTexture();
		return E_FAIL;
	}

	//ready to copy content into texture surface 
    nvAssert(wid == pTexture->getWidth() && hei == pTexture->getHeight());
	
	//convert it to the 16-bit word
    wColorKey = (WORD)(((dwColorKey & 0xF80000) >> 8) | ((dwColorKey & 0xFC00) >> 5) | ((dwColorKey & 0xF8) >> 3));

	pitch = pTexture->getPitch() >> 2; //in 32 bit words

	pTexture->updateLinearSurface();
    
	cp = (DWORD *)pTexture->getLinear()->getAddress();

    nvAssert(cp && source);
	//copy with color keying
	for (i = 0; i < hei; i++ )
    {
        for (j = 0; j < wid; j++)
        {
            WORD c = source[j];

			if ( c == wColorKey) 
				cp[j] = 0;
			else
				cp[j] = (((DWORD)c & 0xF800) << 8) | (((DWORD)c & 0x07E0) << 5) | (((DWORD)c & 0x001F) << 2) | opaque;

			//cp[j] = (((DWORD)c & 0xF800) << 8) | (((DWORD)c & 0x07E0) << 5) | (((DWORD)c & 0x001F) << 2);
        }
		source += wid;
		cp += pitch;
    }
    pTexture->getLinear()->tagUpToDate();
    pTexture->getSwizzled()->tagOutOfDate();

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////
#define RENDER_POPUP_FALLBACK
//#define RENDER_POPUP_INDEXED
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RENDER_POPUP_INDEXED
HRESULT CPopupAgent::RenderTriangles(Vertex *pVertexData, unsigned short *pusIndexData, int nIndexCount, DWORD dwFlags)
{

    // check if the context is valid
    if (!pContext) return E_FAIL;

    // check if we have a valid render target
    if (!pContext->pRenderTarget) return E_FAIL;

    // reject if the render target is a texture
    CNvObject *pNVObj = pContext->pRenderTarget->getWrapper();
    if (!pNVObj) return E_FAIL;
    if (pNVObj->getTexture()) return E_FAIL;

	DWORD		fvfCode = D3DFVF_POPUP_VERTEX;
	DWORD		vertexSize  = sizeof(Vertex);
	Vertex*		vertices = pVertexData;
	WORD*		indices = pusIndexData;		
	DWORD		triQty = nIndexCount/3;
	DWORD		vertexLength= triQty*2; //true only for special case
	
	int i,ind;
	Vertex v;
	int max_ind = 0;

	//static int dumpIt=0; int n=1;
	for(i=0; i<nIndexCount; i++)
	{
		ind = pusIndexData[i];
		v = pVertexData[ind];
		//figure out the number of vertices (assuming continuos indices) 
		if(max_ind<ind) max_ind = ind;
		
		//NB: debug stub
		/*
		if(dumpIt) 
		{
			if( !(i%3) ) LOG("--> %d", n++);
			LOG("v%d(ind=%d) = {%d, %d, %f, %d, %x, %x, %f, %f},",i,ind,(int)v.sx,(int)v.sy,v.sz,(int)v.rhw,v.color,v.specular,v.tu,v.tv);
		}
		*/
	}
	//NB: debug stub
	/*
	if(dumpIt) 
		LOG("indQty=%d,	max ind=%d, triQty=%d", nIndexCount, max_ind+1, triQty);
	++dumpIt;
	*/
	/**/
	//this is unsafe -- we should have it as an additional parameter
	//for straight case
	//vertexLength = max_ind+1;
	/**/
	nIndexCount = 3;
	triQty = nIndexCount/3;
	//vertexLength = triQty*2;
	/**/
///////////////////////////////////////////////////////////////////////////////////
//#define TEST_RENDER_TRIANGLE
#ifdef TEST_RENDER_TRIANGLE

    Vertex quad[] =
	{
		{0, 15, 0.9f, 1, 0, 0, 0.1094, 0.0146},{0, 0, 0.9f, 1, 0, 0, 0.1094, 0.0f},{7, 15, 0.9f, 1, 0, 0, 0.1162, 0.0146},
		{7, 0, 0.9f, 1, 0, 0, 0.1162, 0.0f},{7, 15, 0.9f, 1, 0, 0, 0.1162, 0.0146},{0, 0, 0.9f, 1, 0, 0, 0.1094, 0.0f},

		{7, 15, 0.9f, 1, 0, 0, 0.15625, 0.0146},{7, 0, 0.9f, 1, 0, 0, 0.15625, 0.0f},{14, 15, 0.9f, 1, 0, 0, 0.163, 0.0146},
		{14, 0, 0.9f, 1, 0, 0, 0.163, 0.0f},{14, 15, 0.9f, 1, 0, 0, 0.163, 0.0146},{7, 0, 0.9f, 1, 0, 0, 0.15625, 0.0f},

		{14, 15, 0.9f, 1, 0, 0, 0.086, 0.0146},{14, 0, 0.9f, 1, 0, 0, 0.086, 0.0f},{17, 15, 0.9f, 1, 0, 0, 0.0889, 0.0146},
		{17, 0, 0.9f, 1, 0, 0, 0.0889, 0.0f},{17, 15, 0.9f, 1, 0, 0, 0.0889, 0.0146},{14, 0, 0.9f, 1, 0, 0, 0.086, 0.0f},

		{17, 15, 0.9f, 1, 0, 0, 0.125, 0.0146},{17, 0, 0.9f, 1, 0, 0, 0.125, 0.0f},{24, 15, 0.9f, 1, 0, 0, 0.1318, 0.0146},
		{24, 0, 0.9f, 1, 0, 0, 0.1318, 0.0f},{24, 15, 0.9f, 1, 0, 0, 0.1318, 0.0146},{17, 0, 0.9f, 1, 0, 0, 0.125, 0.0f},

		{24, 15, 0.9f, 1, 0, 0, 0.164, 0.0146},{24, 0, 0.9f, 1, 0, 0, 0.164, 0.0f},{31, 15, 0.9f, 1, 0, 0, 0.170, 0.0146},
		{31, 0, 0.9f, 1, 0, 0, 0.170, 0.0f},{31, 15, 0.9f, 1, 0, 0, 0.170, 0.0146},{24, 0, 0.9f, 1, 0, 0, 0.164, 0.0f},

		{31, 15, 0.9f, 1, 0, 0, 0.0f, 0.0146},{31, 0, 0.9f, 1, 0, 0, 0.0f, 0.0f},{34, 15, 0.9f, 1, 0, 0, 0.002929, 0.0146},
		{34, 0, 0.9f, 1, 0, 0, 0.002929, 0.0f},{34, 15, 0.9f, 1, 0, 0, 0.002929, 0.0146},{31, 0, 0.9f, 1, 0, 0, 0.0f, 0.0f},

		{34, 15, 0.9f, 1, 0, 0, 0.2666, 0.0146},{34, 0, 0.9f, 1, 0, 0, 0.2666, 0.0f},{40, 15, 0.9f, 1, 0, 0, 0.2724, 0.0146},
		{40, 0, 0.9f, 1, 0, 0, 0.2724, 0.0f},{40, 15, 0.9f, 1, 0, 0, 0.2724, 0.0146},{34, 0, 0.9f, 1, 0, 0, 0.2666, 0.0f},

		{40, 15, 0.9f, 1, 0, 0, 0.3486, 0.0146},{40, 0, 0.9f, 1, 0, 0, 0.3486, 0.0f},{48, 15, 0.9f, 1, 0, 0, 0.3564, 0.0146},
		{48, 0, 0.9f, 1, 0, 0, 0.3564, 0.0f},{48, 15, 0.9f, 1, 0, 0, 0.3564, 0.0146},{40, 0, 0.9f, 1, 0, 0, 0.3486, 0.0f},

		{48, 15, 0.9f, 1, 0, 0, 0.376, 0.0146},	{48, 0, 0.9f, 1, 0, 0, 0.376, 0.0f},{56, 15, 0.9f, 1, 0, 0, 0.384, 0.0146},
		{56, 0, 0.9f, 1, 0, 0, 0.384, 0.0f},{56, 15, 0.9f, 1, 0, 0, 0.384, 0.0146},{48, 0, 0.9f, 1, 0, 0, 0.376, 0.0f},
	};
	
	WORD quadIdx[]=	
	{
		 0, 1, 2,  3, 2, 1,  4, 5, 6,  7, 6, 5,  8, 9,10, 11,10, 9, 12,13,14, 15,14,13, //0-8
		16,17,18, 19,18,17, 20,21,22, 23,22,21, 24,25,26, 27,26,25, 28,29,30, 31,30,29, //9-16
		32,33,34, 35,34,33																//17-18
	};
	
	//nIndexCount = sizeof(quadIdx)/sizeof(WORD);
	//nIndexCount=54;	triQty=18;
	nIndexCount=6;	triQty=nIndexCount/3;

	vertexLength = triQty*2;
	vertices = quad;
	indices = quadIdx;

#endif //TEST_RENDER_TRIANGLE
///////////////////////////////////////////////////////////////////////////////////
//define this to examine possible defects
#define DEBUG_MAGNIFY_POPUP

#ifdef DEBUG_MAGNIFY_POPUP
	//scale for better view
	for(i=0; i<vertexLength; i++)
	{
		vertices[i].sx*=10;
		vertices[i].sy*=10;
		vertices[i].sy+=100;
	}
#endif //DEBUG_MAGNIFY_POPUP

    DWORD dwData[256];
	memset(dwData,0,sizeof(dwData));
    void* pData = &dwData;
    LPD3DHAL_DP2COMMAND pCommands = (LPD3DHAL_DP2COMMAND)pData;

    // create the IndexedTriangleList2  command
    pCommands->bCommand        = D3DDP2OP_INDEXEDTRIANGLELIST2;
    pCommands->wPrimitiveCount = triQty;
    D3DHAL_DP2INDEXEDTRIANGLELIST2* pTriList = (D3DHAL_DP2INDEXEDTRIANGLELIST2*)((LPBYTE)(pCommands) + sizeof(D3DHAL_DP2COMMAND));
    
	//memcpy(pTriList,indices,sizeof(WORD)*nIndexCount);
	
	int k;
	for(i=0,k=0; i<triQty/2; i+=2,k+=4)
	{
		k=4;
		pTriList[i+0].wV1 = k+0;
		pTriList[i+0].wV2 = k+1;
		pTriList[i+0].wV3 = k+2;

		pTriList[i+1].wV1 = k+3;
		pTriList[i+1].wV2 = k+2;
		pTriList[i+1].wV3 = k+1;
	}
	
	
    int indSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2INDEXEDTRIANGLELIST2)*triQty;
    // move the command pointer along to the beginning of the next command
    pCommands = (LPD3DHAL_DP2COMMAND)((LPBYTE)pCommands + indSize);

	int d = (DWORD)pCommands - (DWORD)pData;
    // make sure we had enough space
	nvAssert((DWORD)pCommands < (DWORD)dwData + sizeof(dwData));
    // set up the ddraw local & global
#ifdef WINNT
    DD_SURFACE_GLOBAL ddGbl;
    DD_SURFACE_LOCAL  ddLcl;
    ddLcl.lpGbl = &ddGbl;
    ddGbl.fpVidMem = (FLATPTR)pData; // pointer to command buffer
    ddLcl.dwReserved1 = NULL;
#else
    typedef struct {
        DDRAWI_DDRAWSURFACE_GBL_MORE ddGblMore;
        LPDWORD                      dwReserved1;
        DDRAWI_DDRAWSURFACE_GBL      ddGbl;
    } EXT_DDRAWI_DDRAWSURFACE_GBL;

    DDRAWI_DDRAWSURFACE_LCL     ddLcl;
    EXT_DDRAWI_DDRAWSURFACE_GBL eddGbl;
    eddGbl.ddGbl.fpVidMem = (FLATPTR)pData; // pointer to command buffer
    eddGbl.dwReserved1 = (LPDWORD)&eddGbl.ddGblMore;
    eddGbl.ddGblMore.dwDriverReserved = NULL;
    ddLcl.lpGbl = &eddGbl.ddGbl;
#endif

    // set up the DP2 command
    D3DHAL_DRAWPRIMITIVES2DATA dp2;
    dp2.dwhContext      = (ULONG_PTR)pContext;
    dp2.dwFlags         = D3DHALDP2_USERMEMVERTICES;
    dp2.dwVertexType    = fvfCode;
	dp2.dwVertexSize    = vertexSize;
    dp2.lpDDCommands    = &ddLcl;
    dp2.dwCommandOffset = 0;
    dp2.dwCommandLength = (DWORD)pCommands - (DWORD)pData;
    dp2.lpVertices      = vertices;
    dp2.dwVertexOffset  = 0;
    dp2.dwVertexLength  = vertexLength;
    dp2.lpdwRStates     = NULL;

    // create a vertex buffer
    pVB->own((DWORD)vertices, dp2.dwVertexSize * vertexLength, CSimpleSurface::HEAP_SYS);

    pVB->setVertexStride(dp2.dwVertexSize);

    // create a vertex shader from the FVF format
    pVS->create (pContext, dp2.dwVertexType, CVertexShader::getHandleFromFvf( dp2.dwVertexType ) );

    // save the current vertex buffer and vertex shader
    CVertexShader *pOldVertexShader = pContext->pCurrentVShader;
    CVertexBuffer *pOldVertexBuffer = pContext->ppDX8Streams[0];
    DWORD          dwStreamDMACount = pContext->dwStreamDMACount;
    DWORD          dwDxAppVersion   = pContext->dwDXAppVersion;

	setupStates(dwFlags);

    // set the popup vertex buffer & shader
    pContext->pCurrentVShader  = pVS;
    pContext->ppDX8Streams[0]  = pVB;
    pContext->dwStreamDMACount = 0x00010000;
    pContext->dwDXAppVersion   = 0x800;

    nvDrawPrimitives2(&dp2);

    // restore original vertex buffer & shader
    pContext->pCurrentVShader  = pOldVertexShader;
    pContext->ppDX8Streams[0]  = pOldVertexBuffer;
    pContext->dwStreamDMACount = dwStreamDMACount;
    pContext->dwDXAppVersion   = dwDxAppVersion;

	restoreStates(dwFlags);

	return S_OK;
}
#endif //RENDER_POPUP_INDEXED

#ifdef RENDER_POPUP_FALLBACK
///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CPopupAgent::RenderTriangles(Vertex *pVertexData, unsigned short *pusIndexData, int nIndexCount, DWORD dwFlags)
{

    // check if the context is valid
    if (!pContext) return E_FAIL;

    // check if we have a valid render target
    if (!pContext->pRenderTarget) return E_FAIL;

    // reject if the render target is a texture
    CNvObject *pNVObj = pContext->pRenderTarget->getWrapper();
    if (!pNVObj) return E_FAIL;
    if (pNVObj->getTexture()) return E_FAIL;

	//BUGBUG: lame workaround -- still can't setup indexed tri list
    ////////////////////////////////////////////////////
#define POPUP_MAX_IND     120
	static Vertex	buf[POPUP_MAX_IND];
	static WORD		idx[POPUP_MAX_IND];

    if(nIndexCount>POPUP_MAX_IND)
    {
        nIndexCount = POPUP_MAX_IND;
        nvAssert(0);
    }
    ////////////////////////////////////////////////////
    DWORD		fvfCode = D3DFVF_POPUP_VERTEX;
	DWORD		vertexSize  = sizeof(Vertex);
	Vertex*		vertices = pVertexData;
	WORD*		indices = pusIndexData;		
	DWORD		triQty = nIndexCount/3;
	DWORD		vertexLength= triQty*2; //true only for special case
	
	int i,ind;
	Vertex v;
	int max_ind = 0;

	//static int dumpIt=0; int n=1;
	for(i=0; i<nIndexCount; i++)
	{
		ind = pusIndexData[i];
		v = pVertexData[ind];
		//figure out the number of vertices (assuming continuos indices) 
		if(max_ind<ind) max_ind = ind;
		
		//BUGBUG: untangling indices/vertices. Lame. 
		buf[i] = v;
		idx[i] = i;
	}
	///////// START: for untangled indices only ///////////////
	/**/
	vertexLength = nIndexCount;
	vertices = buf;
	indices  = idx;
	/**/
	///////// END: for untangled indices only ///////////////

///////////////////////////////////////////////////////////////////////////////////
//#define TEST_RENDER_TRIANGLE
#ifdef TEST_RENDER_TRIANGLE

    Vertex quad[] =
	{
		{0, 15, 0.9f, 1, 0, 0, 0.1094, 0.0146},{0, 0, 0.9f, 1, 0, 0, 0.1094, 0.0f},{7, 15, 0.9f, 1, 0, 0, 0.1162, 0.0146},
		{7, 0, 0.9f, 1, 0, 0, 0.1162, 0.0f},{7, 15, 0.9f, 1, 0, 0, 0.1162, 0.0146},{0, 0, 0.9f, 1, 0, 0, 0.1094, 0.0f},

		{7, 15, 0.9f, 1, 0, 0, 0.15625, 0.0146},{7, 0, 0.9f, 1, 0, 0, 0.15625, 0.0f},{14, 15, 0.9f, 1, 0, 0, 0.163, 0.0146},
		{14, 0, 0.9f, 1, 0, 0, 0.163, 0.0f},{14, 15, 0.9f, 1, 0, 0, 0.163, 0.0146},{7, 0, 0.9f, 1, 0, 0, 0.15625, 0.0f},

		{14, 15, 0.9f, 1, 0, 0, 0.086, 0.0146},{14, 0, 0.9f, 1, 0, 0, 0.086, 0.0f},{17, 15, 0.9f, 1, 0, 0, 0.0889, 0.0146},
		{17, 0, 0.9f, 1, 0, 0, 0.0889, 0.0f},{17, 15, 0.9f, 1, 0, 0, 0.0889, 0.0146},{14, 0, 0.9f, 1, 0, 0, 0.086, 0.0f},

		{17, 15, 0.9f, 1, 0, 0, 0.125, 0.0146},{17, 0, 0.9f, 1, 0, 0, 0.125, 0.0f},{24, 15, 0.9f, 1, 0, 0, 0.1318, 0.0146},
		{24, 0, 0.9f, 1, 0, 0, 0.1318, 0.0f},{24, 15, 0.9f, 1, 0, 0, 0.1318, 0.0146},{17, 0, 0.9f, 1, 0, 0, 0.125, 0.0f},

		{24, 15, 0.9f, 1, 0, 0, 0.164, 0.0146},{24, 0, 0.9f, 1, 0, 0, 0.164, 0.0f},{31, 15, 0.9f, 1, 0, 0, 0.170, 0.0146},
		{31, 0, 0.9f, 1, 0, 0, 0.170, 0.0f},{31, 15, 0.9f, 1, 0, 0, 0.170, 0.0146},{24, 0, 0.9f, 1, 0, 0, 0.164, 0.0f},

		{31, 15, 0.9f, 1, 0, 0, 0.0f, 0.0146},{31, 0, 0.9f, 1, 0, 0, 0.0f, 0.0f},{34, 15, 0.9f, 1, 0, 0, 0.002929, 0.0146},
		{34, 0, 0.9f, 1, 0, 0, 0.002929, 0.0f},{34, 15, 0.9f, 1, 0, 0, 0.002929, 0.0146},{31, 0, 0.9f, 1, 0, 0, 0.0f, 0.0f},

		{34, 15, 0.9f, 1, 0, 0, 0.2666, 0.0146},{34, 0, 0.9f, 1, 0, 0, 0.2666, 0.0f},{40, 15, 0.9f, 1, 0, 0, 0.2724, 0.0146},
		{40, 0, 0.9f, 1, 0, 0, 0.2724, 0.0f},{40, 15, 0.9f, 1, 0, 0, 0.2724, 0.0146},{34, 0, 0.9f, 1, 0, 0, 0.2666, 0.0f},

		{40, 15, 0.9f, 1, 0, 0, 0.3486, 0.0146},{40, 0, 0.9f, 1, 0, 0, 0.3486, 0.0f},{48, 15, 0.9f, 1, 0, 0, 0.3564, 0.0146},
		{48, 0, 0.9f, 1, 0, 0, 0.3564, 0.0f},{48, 15, 0.9f, 1, 0, 0, 0.3564, 0.0146},{40, 0, 0.9f, 1, 0, 0, 0.3486, 0.0f},

		{48, 15, 0.9f, 1, 0, 0, 0.376, 0.0146},	{48, 0, 0.9f, 1, 0, 0, 0.376, 0.0f},{56, 15, 0.9f, 1, 0, 0, 0.384, 0.0146},
		{56, 0, 0.9f, 1, 0, 0, 0.384, 0.0f},{56, 15, 0.9f, 1, 0, 0, 0.384, 0.0146},{48, 0, 0.9f, 1, 0, 0, 0.376, 0.0f},
	};
	
	//nIndexCount = sizeof(quadIdx)/sizeof(WORD);
	nIndexCount=54;	triQty=18;

	vertexLength = triQty*2;
	vertices = (Vertex*)quad;
	indices = quadIdx;

#endif //TEST_RENDER_TRIANGLE
///////////////////////////////////////////////////////////////////////////////////
//define this to examine possible defects
//#define DEBUG_MAGNIFY_POPUP

#ifdef DEBUG_MAGNIFY_POPUP
	//scale for better view
	for(i=0; i<vertexLength; i++)
	{
		vertices[i].sx*=10;
		vertices[i].sy*=10;
		vertices[i].sy+=100;
	}
#endif //DEBUG_MAGNIFY_POPUP

    DWORD dwData[256];
	memset(dwData,0,sizeof(dwData));
    void* pData = &dwData;
    LPD3DHAL_DP2COMMAND pCommands = (LPD3DHAL_DP2COMMAND)pData;

    // create the TriangleList  command
    pCommands->bCommand        = D3DDP2OP_TRIANGLELIST;
    pCommands->wPrimitiveCount = triQty;
    D3DHAL_DP2TRIANGLELIST* pTriList = (D3DHAL_DP2TRIANGLELIST*)((LPBYTE)(pCommands) + sizeof(D3DHAL_DP2COMMAND));
    
	pTriList->wVStart=0;

    int size = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2TRIANGLELIST);
    // move the command pointer along to the beginning of the next command
    pCommands = (LPD3DHAL_DP2COMMAND)((LPBYTE)pCommands + size);
    // make sure we had enough space
	nvAssert((DWORD)pCommands < (DWORD)dwData + sizeof(dwData));
    // set up the ddraw local & global
#ifdef WINNT
    DD_SURFACE_GLOBAL ddGbl;
    DD_SURFACE_LOCAL  ddLcl;
    ddLcl.lpGbl = &ddGbl;
    ddGbl.fpVidMem = (FLATPTR)pData; // pointer to command buffer
    ddLcl.dwReserved1 = NULL;
#else
    typedef struct {
        DDRAWI_DDRAWSURFACE_GBL_MORE ddGblMore;
        LPDWORD                      dwReserved1;
        DDRAWI_DDRAWSURFACE_GBL      ddGbl;
    } EXT_DDRAWI_DDRAWSURFACE_GBL;

    DDRAWI_DDRAWSURFACE_LCL     ddLcl;
    EXT_DDRAWI_DDRAWSURFACE_GBL eddGbl;
    eddGbl.ddGbl.fpVidMem = (FLATPTR)pData; // pointer to command buffer
    eddGbl.dwReserved1 = (LPDWORD)&eddGbl.ddGblMore;
    eddGbl.ddGblMore.dwDriverReserved = NULL;
    ddLcl.lpGbl = &eddGbl.ddGbl;
#endif

    // set up the DP2 command
    D3DHAL_DRAWPRIMITIVES2DATA dp2;
    dp2.dwhContext      = (ULONG_PTR)pContext;
    dp2.dwFlags         = D3DHALDP2_USERMEMVERTICES;
    dp2.dwVertexType    = fvfCode;
	dp2.dwVertexSize    = vertexSize;
    dp2.lpDDCommands    = &ddLcl;
    dp2.dwCommandOffset = 0;
    dp2.dwCommandLength = (DWORD)pCommands - (DWORD)pData;
    dp2.lpVertices      = vertices;
    dp2.dwVertexOffset  = 0;
    dp2.dwVertexLength  = vertexLength;
    dp2.lpdwRStates     = NULL;

    // create a vertex buffer
    pVB->own((DWORD)vertices, dp2.dwVertexSize * vertexLength, CSimpleSurface::HEAP_SYS);

    pVB->setVertexStride(dp2.dwVertexSize);

    // create a vertex shader from the FVF format
    pVS->create (pContext, dp2.dwVertexType, CVertexShader::getHandleFromFvf( dp2.dwVertexType ) );

    // save the current vertex buffer and vertex shader
    CVertexShader *pOldVertexShader = pContext->pCurrentVShader;
    CVertexBuffer *pOldVertexBuffer = pContext->ppDX8Streams[0];
    DWORD          dwStreamDMACount = pContext->dwStreamDMACount;
    DWORD          dwDxAppVersion   = pContext->dwDXAppVersion;

	setupStates(dwFlags);

    // set the popup vertex buffer & shader
    pContext->pCurrentVShader  = pVS;
    pContext->ppDX8Streams[0]  = pVB;
    pContext->dwStreamDMACount = 0x00010000;
    pContext->dwDXAppVersion   = 0x800;

    nvDrawPrimitives2(&dp2);

    // restore original vertex buffer & shader
    pContext->pCurrentVShader  = pOldVertexShader;
    pContext->ppDX8Streams[0]  = pOldVertexBuffer;
    pContext->dwStreamDMACount = dwStreamDMACount;
    pContext->dwDXAppVersion   = dwDxAppVersion;

	restoreStates(dwFlags);

	return S_OK;
}
#endif //RENDER_POPUP_FALLBACK
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CPopupAgent::setupStates( DWORD dwFlags )
{
	if(!pContext) return E_FAIL;
    // set up state
	HRESULT rval;
	
	PopupStateSet* popupStates = popupStatesTextured;

	if(dwFlags ==  RTF_NOTEXTURE )
		popupStates = popupStatesSolid;

	while(popupStates->state)
    {
		//save old state
        popupStates->oldvalue = pContext->dwRenderState[popupStates->state];
		//set desired state
        nvSetContextState(pContext, popupStates->state, popupStates->value, &rval);
		++popupStates;
	}

	//setting up texturing
	nvMemCopy(tssPrevState, pContext->tssState, sizeof(tssPrevState));
	memset(&pContext->tssState[0], 0, sizeof(tssPrevState));
	
	if(dwFlags == RTF_NOTEXTURE )
	{
		
		pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP    ] = 0;
		pContext->tssState[0].dwValue[D3DTSS_COLOROP       ] = D3DTOP_SELECTARG1;
		pContext->tssState[0].dwValue[D3DTSS_COLORARG1     ] = D3DTA_DIFFUSE;
		//pContext->tssState[0].dwValue[D3DTSS_ALPHAOP       ] = D3DTOP_DISABLE;
		//pContext->tssState[0].dwValue[D3DTSS_ALPHAOP       ] = D3DTOP_SELECTARG1;
		//pContext->tssState[0].dwValue[D3DTSS_ALPHAARG1     ] = D3DTA_DIFFUSE;
		//pContext->tssState[0].dwValue[D3DTSS_ADDRESSU      ] = D3DTADDRESS_WRAP;
		//pContext->tssState[0].dwValue[D3DTSS_ADDRESSV      ] = D3DTADDRESS_WRAP;
		//pContext->tssState[0].dwValue[D3DTSS_MAGFILTER     ] = D3DTFG_LINEAR;
		//pContext->tssState[0].dwValue[D3DTSS_MINFILTER     ] = D3DTFN_LINEAR;
		//pContext->tssState[0].dwValue[D3DTSS_MIPFILTER     ] = D3DTFP_NONE;
		//pContext->tssState[0].dwValue[D3DTSS_MAXANISOTROPY ] = 1;
		//pContext->tssState[1].dwValue[D3DTSS_COLOROP       ] = D3DTOP_DISABLE;
	}
	else
	{
		pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP    ] = (DWORD)pTextureObj;
		pContext->tssState[0].dwValue[D3DTSS_COLOROP       ] = D3DTOP_SELECTARG1;
		pContext->tssState[0].dwValue[D3DTSS_COLORARG1     ] = D3DTA_TEXTURE;
		pContext->tssState[0].dwValue[D3DTSS_ALPHAOP       ] = D3DTOP_SELECTARG1;
		pContext->tssState[0].dwValue[D3DTSS_ALPHAARG1     ] = D3DTA_TEXTURE;
		pContext->tssState[0].dwValue[D3DTSS_ADDRESSU      ] = D3DTADDRESS_WRAP;
		pContext->tssState[0].dwValue[D3DTSS_ADDRESSV      ] = D3DTADDRESS_WRAP;
		pContext->tssState[0].dwValue[D3DTSS_MAGFILTER     ] = D3DTFG_LINEAR;
		pContext->tssState[0].dwValue[D3DTSS_MINFILTER     ] = D3DTFN_LINEAR;
		pContext->tssState[0].dwValue[D3DTSS_MIPFILTER     ] = D3DTFP_NONE;
		pContext->tssState[0].dwValue[D3DTSS_MAXANISOTROPY ] = 1;
		pContext->tssState[1].dwValue[D3DTSS_COLOROP       ] = D3DTOP_DISABLE;
		pContext->tssState[2].dwValue[D3DTSS_COLOROP       ] = D3DTOP_DISABLE;
		pContext->tssState[3].dwValue[D3DTSS_COLOROP       ] = D3DTOP_DISABLE;
	}
    // make sure everything gets updated
#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= celsiusDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
    pContext->hwState.dwDirtyFlags |= kelvinDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF | KELVIN_DIRTY_FVF;
#endif
    NV_FORCE_TRI_SETUP(pContext);

	return S_OK;
}

HRESULT CPopupAgent::restoreStates(DWORD dwFlags)
{
	HRESULT rval;
	
	if(!pContext) return E_FAIL;
    // restore state
	PopupStateSet* popupStates = popupStatesTextured;

	if(dwFlags ==  RTF_NOTEXTURE )
		popupStates = popupStatesSolid;

    while(popupStates->state)
    {
        nvSetContextState(pContext, popupStates->state, popupStates->oldvalue, &rval);
		++popupStates;
    }

    // restore texture stage state
    nvMemCopy(pContext->tssState, tssPrevState, sizeof(tssPrevState));
    // make sure everything gets updated
#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= celsiusDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
    pContext->hwState.dwDirtyFlags |= kelvinDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF | KELVIN_DIRTY_FVF;
#endif
    NV_FORCE_TRI_SETUP(pContext);
    
#if (NVARCH >= 0x020)
    if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)) {
        if (pContext->kelvinAA.IsEnabled()) {
            //fixes flickering text in sample apps with aa and logo on.
            getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
        }
    }
#endif 

	
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
CPopupAgentList::CPopupAgentList()
{
	popupAgentList = 0;
	hMod = 0;
    bPerfStat = false;
    bStereoStat = false;
    isPopupEnabled = false;

    memset(&hotkeys,0,sizeof(hotkeys));

    strcpy(strModule, DEFAULT_MODULE_NAME);
}

CPopupAgentList::~CPopupAgentList()
{
	destroy();
}

void    CPopupAgentList::add(CPopupAgent* that)
{
    CPopupAgentBase* cp=(CPopupAgentBase*)popupAgentList;
    CPopupAgentBase* nextAgent=cp;
    
    if(!cp) //list is empty yet
    {
        popupAgentList = that;
        return;
    }
    //else, go to the end of list:
    for(nextAgent=cp->GetNextAgent(); nextAgent; cp=nextAgent);
    //insert
    cp->SetNextAgent(that);
}

HRESULT CPopupAgentList::create()
{
    return create(DEFAULT_MODULE_NAME);
}

void    CPopupAgentList::getOptions()
{
    HKEY hKey;
    char popupKey[MAX_PATH];

    nvStrCpy(popupKey, NV4_REG_GLOBAL_BASE_PATH);
    nvStrCat(popupKey, "\\");
    nvStrCat(popupKey, NV4_REG_STEREO_SUBKEY);
    nvStrCat(popupKey, "\\");
    nvStrCat(popupKey, NV_REG_STEREO_POPUP_SUBKEY);

    DWORD rc=0;

    rc = RegOpenKeyEx(NV4_REG_GLOBAL_BASE_KEY, popupKey, 0, KEY_READ, &hKey);
    if (rc == ERROR_SUCCESS) 
    {
        DWORD   dwSize = sizeof(long);
        DWORD   dwType = REG_DWORD;
        long    lValue;

        if (RegQueryValueEx(hKey, NV_REG_STEREO_POPUP_FPS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            bPerfStat = lValue?true:false;
        }
        if (RegQueryValueEx(hKey, NV_REG_STEREO_POPUP_STAT, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            bStereoStat = lValue?true:false;
        }

        //configure hot key interface

        if (RegQueryValueEx(hKey, NV_REG_STEREO_POPUP_HOTKEY, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            hotkeys.PopupToggle.dwValue = lValue;   // The hot key to toggle all popups
        }
        if (RegQueryValueEx(hKey, NV_REG_STEREO_POPUP_FPS_HOTKEY, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            hotkeys.FPStoggle.dwValue = lValue;   // The hot key to toggle FPS popup
        }
        if (RegQueryValueEx(hKey, NV_REG_STEREO_POPUP_STATS_HOTKEY, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            hotkeys.StatsToggle.dwValue = lValue;   // The hot key to toggle Stereo Statistics popup
        }
        
        RegCloseKey(hKey);
    }
}

HRESULT  CPopupAgentList::initHotKeys()
{
    // add hotkeys 
    nvAssert(pStereoData->StereoKeys.pHotKeyI);

    CREATESTEREOHOTKEY(hotkeys.PopupToggle);
    //CREATESTEREOHOTKEY(hotkeys.FPStoggle);
    //CREATESTEREOHOTKEY(hotkeys.StatsToggle);

    return S_OK;
}

HRESULT  CPopupAgentList::finiHotKeys()
{
    DESTROYSTEREOHOTKEY(hotkeys.PopupToggle);
    //DESTROYSTEREOHOTKEY(hotkeys.FPStoggle);
    //DESTROYSTEREOHOTKEY(hotkeys.StatsToggle);
    return S_OK;
}
HRESULT CPopupAgentList::create(const char* aModule)
{
	HRESULT rc=E_FAIL;

	if(popupAgentList)
	{
		rc = destroy();
	}
	
    if(aModule)
	{
		strcpy(strModule,aModule);
	}
	
    if(bPerfStat || bStereoStat)
    {
    	nvAssert(!hMod);
        hMod = LoadLibrary(strModule);
	    if(!hMod) 
	    {
		    LOG("CPopupAgentList::create : can't load %s", strModule );
		    return E_FAIL;
	    }
    }
    // construction zone. to be generalized for arbitrary number & type of popups
	/////// Create popup with FPS info /////////////////////////////
    if(bPerfStat)
    {
	    CPerfStatPopup*	fpsPopup=0;
	    fpsPopup = new CPerfStatPopup;
	    if(!fpsPopup)
	    {
		    DPF("CPopupAgentList::create : FPS popup allocation failed");
		    return E_FAIL;
	    }
	    fpsPopup->bindPopupAgent(hMod);
	    add(fpsPopup); 
    }
	/////// Create popup with stereo statistics /////////////////////////////
    if(bStereoStat)
    {
        CStereoStatPopup*	stsPopup=0;
	    stsPopup = new CStereoStatPopup;
	    if(!stsPopup)
	    {
		    DPF("CPopupAgentList::create : StereoStat popup allocation failed");
		    return E_FAIL;
	    }
	    stsPopup->bindPopupAgent(hMod);
        add(stsPopup);
    }
	/////////////////////////////////
	rc=S_OK;

	return rc;
}

HRESULT CPopupAgentList::destroy()
{
	HRESULT rc=E_FAIL;

	CPopupAgent* cp=popupAgentList;
	CPopupAgent* nextAgent;

	for(nextAgent = cp; nextAgent; cp = nextAgent )
	{
		nextAgent = (CPopupAgent*)cp->GetNextAgent();
		cp->unbindPopupAgent();
		cp->SetFlipCallback(0);
		delete cp;
		cp=0;
	}
	popupAgentList=0;

	if(hMod) 
	{
		FreeLibrary(hMod);
		hMod = 0;
	}
    
    rc=S_OK;
	return rc;
}

//render pop-up agents
//[in]  driver context 
HRESULT CPopupAgentList::render(NVD3DCONTEXT* pContext)
{
	HRESULT rc = E_FAIL;

    //required for POLLSTEREOHOTKEY
    BOOL res_ = FALSE;
    
    POLLSTEREOHOTKEY(hotkeys.PopupToggle)
    {
        toggle();
    }
    
    //TODO: add individual popup hotkey processing code 

    if(!enabled()) 
    {
        return S_OK;
    }

	CPopupAgent* cp=popupAgentList;
	CPopupAgent* nextAgent;

	for( nextAgent = cp; nextAgent; cp = nextAgent )
	{
		nextAgent = (CPopupAgent*)cp->GetNextAgent();
		//NB: context is validated during renderTriangles call
        cp->setContext(pContext);

		FlipCallbackType callback= cp->GetFlipCallback();
		
		if( !callback )
		{
			 DPF("CPopupAgentList::render : flip callback points to null");
			 nvAssert(0);
			 continue;
		}
		rc = callback(cp);					//grant application a chance to wreak havoc

	}

	rc=S_OK;
	return rc;
}

////////////////////////////////////////////////////////////////////////////////////////////
CPopupAgentList popupAgents;
//////////////////////////////////////////////////////////////////////////////////////////

CPerfStatPopup::CPerfStatPopup()
{
	m_dwAgentID = PERF_STATS_ID;
}

CPerfStatPopup::~CPerfStatPopup()
{
}
//////////////////////////////////////////////////////////////////////////////////////////
//Stereo Statistics

CStereoStatPopup::CStereoStatPopup()
{
	//memset((STEREOSTATS*)this,0,sizeof(STEREOSTATS));
	//shoot to foot -- our user data (STEREOSTATS) contained in the self
	SetUserData(this);
	m_dwAgentID = STEREO_STATS_ID;
}

CStereoStatPopup::~CStereoStatPopup()
{
}

void CStereoStatPopup::setContext(NVD3DCONTEXT* aContext)
{
	//call base class
	CPopupAgent::setContext(aContext);

	if(!STEREO_ENABLED || !pContext) return;

	dwWidth  = pContext->pRenderTarget->getWidth();
    dwHeight = pContext->pRenderTarget->getHeight();
    dwBpp = pContext->pRenderTarget->getBPP();

	dwFlags = 0;

	StereoSeparation  = pStereoData->StereoSettings.fStereoSeparation;
	StereoConvergence = pStereoData->StereoSettings.fStereoConvergence;

	if(pStereoData->dwHWTnL)
	{
		//gather HW T&L statistics
		dwFlags |= HW_TnL;
		ZNear = ConfigAssistInfo.szMin;
		ZFar = 	ConfigAssistInfo.szMax;
	}
	else
	{
		//gather SW T&L statistics
		rhwMin =  ConfigAssistInfo.rhwMin;
		rhwMax =  ConfigAssistInfo.rhwMax;
		rhwMin2D =  ConfigAssistInfo.rhwMin2D;
		rhwMax2D =  ConfigAssistInfo.rhwMax2D;

		if (pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN)
		{
			dwFlags |= RHWGREATERATSCREEN;
			RHWGreaterAtScreen = pStereoData->StereoSettings.fRHWGreaterAtScreen;
		}
		if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
		{
			dwFlags |= RHWLESSATSCREEN;
			RHWLessAtScreen = pStereoData->StereoSettings.fRHWLessAtScreen;
		}
	}
//for now, always defined in the nvStereo.h
#ifdef STEREO_CONFIG_ASSIST 
	dwFlags |= CONFIGASSISTON;
#endif

}
#endif //IMPLEMENT_POPUP_AGENT -- Gr