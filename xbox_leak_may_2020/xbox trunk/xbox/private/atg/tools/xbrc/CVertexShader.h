//-----------------------------------------------------------------------------
// File: CVertexShader.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the vertexshader-related functionality of the bundler tool
//
// Hist: 2001.09.26 - New for November XDK release
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef CVERTEXSHADER_H
#define CVERTEXSHADER_H

#include "vb.h"

class CBundler;

#define VERTEXSHADER_MAX_DECLARATION_COUNT	1000

//-----------------------------------------------------------------------------
// Name: class CVertexShader
// Desc: Handles VertexShader-specific processing
//-----------------------------------------------------------------------------
class CVertexShader
{
public:
    CVertexShader( CBundler * pBundler );
    ~CVertexShader();

    HRESULT SaveToBundle( DWORD *pcbHeader, DWORD *pcbData );

	// Build up a declaration
	HRESULT SetStream(UINT iStream);
	HRESULT SetVertexRegisterFormat(UINT iVertexRegister, DWORD VertexFormat);
	HRESULT SkipBytes(UINT Count);
	HRESULT SetConstant(INT iConstantRegister, CONST void *pConstantData, DWORD ConstantCount);
	
	// parse declaration into streams and constants
//	HRESULT SetDeclaration(const DWORD *pDeclaration);

	// set vertex shader microcode output from XGAssembleShader
	HRESULT SetFunction(CONST void *pFunction, UINT cbFunctionSize);

	// set resource references
	HRESULT SetVertexShaderReference(DWORD dwVertexShaderResourceIndex);
	HRESULT SetStreamVertexBufferReference(UINT iStream, DWORD dwVertexBufferResourceIndex, DWORD Stride, DWORD Offset);
	
    CBundler * m_pBundler;
#define VERTEXSHADER_IDENTIFIER_MAX_LENGTH 128
	CHAR m_Identifier[VERTEXSHADER_IDENTIFIER_MAX_LENGTH];
	DWORD m_dwDeclarationCount;
	DWORD m_rdwDeclaration[VERTEXSHADER_MAX_DECLARATION_COUNT];	// Does not include vertex constants
	UINT m_cbFunctionSize;
	BYTE *m_pFunction;
	BOOL  m_rbConstant[XD3DVS_CONSTREG_COUNT];	// is this register active?
	FLOAT m_rfConstant[XD3DVS_CONSTREG_COUNT][4];
	DWORD m_dwVertexShaderReference;			// index of vertex shader resource for initial values
	struct StreamInput {	// like D3DSTREAM_INPUT, but with vertex buffer reference instead of a pointer
		DWORD VertexBufferReference;
		DWORD Stride;
		DWORD Offset;
	} m_rStreamInput[XD3DVS_STREAMS_MAX];	// mapping of vertex buffer resources to streams
	
private:
	HRESULT AddConstantsToDeclaration();	// look through active constants in m_rbConstant and add to declaration
	HRESULT SetStridesFromDeclaration();	// parse declaration to set rStreamInput strides
};


#endif // CVERTEXSHADER_H
