//
//	renderer.h
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#ifndef __RENDERER_H__
#define __RENDERER_H__

///////////////////////////////////////////////////////////////////////////////
class Renderer
{
private:

	IDirect3D8				*pD3D8;
	IDirect3DDevice8		*pD3DDev8;
	D3DPRESENT_PARAMETERS	d3dPresentParams;

public:

	void Init();
	void UnInit();

	bool init(int width,int height);
	void shutdown();
};

extern IDirect3DDevice8 *gpd3dDev;
/////////////////////////////////////////////////////////////////////////////// 


#endif // __RENDERER_H__