//-----------------------------------------------------------------------------
//  
//  File: xbrc.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Resource type constants
//-----------------------------------------------------------------------------
 
#define XBRC_USER_DATA_FLAG 0x80000000

// D3D resources
//#define XBRC_SURFACE       D3DRTYPE_SURFACE
//#define XBRC_VOLUME        D3DRTYPE_VOLUME
//#define XBRC_TEXTURE       	 D3DRTYPE_TEXTURE
//#define XBRC_VOLUMETEXTURE 	 D3DRTYPE_VOLUMETEXTURE
//#define XBRC_CUBETEXTURE   	 D3DRTYPE_CUBETEXTURE  
//#define XBRC_VERTEXBUFFER  	 D3DRTYPE_VERTEXBUFFER
//#define XBRC_PUSHBUFFER    D3DRTYPE_PUSHBUFFER
//#define XBRC_PALETTE       D3DRTYPE_PALETTE
//#define XBRC_FIXUP         D3DRTYPE_FIXUP

// Additional resources
// TODO: change index buffer to (XBRC_USER_DATA_FLAG|D3DRTYPE_INDEXBUFFER)
#define XBRC_INDEXBUFFER			(XBRC_USER_DATA_FLAG|D3DCOMMON_TYPE_INDEXBUFFER)
#define XBRC_VERTEXSHADER 			(XBRC_USER_DATA_FLAG|0x010)
#define XBRC_PIXELSHADER			(XBRC_USER_DATA_FLAG|0x020)
#define XBRC_EFFECT					(XBRC_USER_DATA_FLAG|0x040)
#define XBRC_SKELETON				(XBRC_USER_DATA_FLAG|0x050)
#define XBRC_ANIMATION				(XBRC_USER_DATA_FLAG|0x060)

// The symbol table maps strings to offsets.
#define XBRC_SYMBOLTABLE				(XBRC_USER_DATA_FLAG|0x100)
