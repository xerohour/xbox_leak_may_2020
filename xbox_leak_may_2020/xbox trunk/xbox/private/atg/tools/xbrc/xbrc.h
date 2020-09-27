//-----------------------------------------------------------------------------
//  
//  File: xbrc.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Resource constants
//
//-----------------------------------------------------------------------------
 
#define USER_DATA_FLAG 0x80000000

// All of this should instead be compiled into pushbuffers, but
// that functionality is not yet exposed on the PC side.  

#define XBRC_INDEXBUFFER				(USER_DATA_FLAG|D3DCOMMON_TYPE_INDEXBUFFER)
#define XBRC_VERTEXSHADER 				0x80000010
#define XBRC_PIXELSHADER				0x80000020


/*
#define XBRC_BATCH						0x80000030

#define BATCH_SetRenderState 			1
#define BATCH_SetTextureStageState		2
#define BATCH_SetTexture				3
#define BATCH_SetPalette				4
#define BATCH_SetVertexShader			5
#define BATCH_SetVertexShaderInput		6
#define BATCH_SetVertexShaderConstant	7
#define BATCH_SetPixelShader			8
#define BATCH_SetRenderTarget			9
*/

#define XBRC_EFFECT						0x80000040
#define XBRC_SKELETON					0x80000050
#define XBRC_ANIMATION					0x80000060

// For preview and debugging the symbol table maps strings to offsets.
#define XBRC_SYMBOLTABLE				0x80000100
