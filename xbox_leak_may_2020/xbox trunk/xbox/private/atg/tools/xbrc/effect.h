//-----------------------------------------------------------------------------
//  
//  File: effect.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
//  An effect is a collection of rendering passes.  Each pass needs
//  vertex shaders (programs and constants), pixel shaders (programs
//  and constants), render states (including texture states),
//  textures, vertex buffers, index buffers, and draw lists.
//  
//-----------------------------------------------------------------------------
#pragma once

#define VERTEXSHADER_MAX_STREAMS 16
struct VertexShader {
	DWORD Handle;
	DWORD DeclarationByteCount;		// size of Declaration
	DWORD *Declaration;				// the declaration maps streams to vertex registers, and defines constants
	DWORD FunctionByteCount;		// size of Function
	DWORD *Function;				// function microcode maps vertex registers to transformed vertices
	DWORD VertexShaderReference;
	struct StreamInput {
		DWORD VertexBufferReference;
		DWORD Stride;
		DWORD Offset;
	} rStreamInput[VERTEXSHADER_MAX_STREAMS];
};

// PixelShader structure is same as D3DPIXELSHADERDEF

struct RenderState {
	DWORD State;
	DWORD Value;
};

struct TextureState {
	DWORD State;
	DWORD Value;
};

struct TextureStage {
	DWORD TextureResource;
	DWORD TextureStateCount;
	TextureState *rTextureState;
};

struct Draw {
	DWORD IndexBufferResource;
	DWORD Primitive;
	DWORD Start;
	DWORD Count;
};

struct Pass {
	DWORD VertexShaderResource;
	DWORD PixelShaderResource;	// TODO: Consider inlining the pixel shader definition
	DWORD RenderStateCount;
	RenderState *rRenderState;
	DWORD TextureStageCount;
	TextureStage *rTextureStage;
	DWORD DrawCount;
	Draw *rDraw;
};

#define EFFECT_IDENTIFIER_SIZE 128
struct Effect {
	CHAR Identifier[EFFECT_IDENTIFIER_SIZE];
	DWORD RenderTargetResource;
	DWORD PassCount;
	Pass *rPass;
};
