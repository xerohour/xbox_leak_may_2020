//-----------------------------------------------------------------------------
//  
//  File: XBR.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  XML DirectX compiled resource drawing.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "XBR.h"
#include "Print.h"

// Called by OnIOComplete to patch pointers to all the effect types.
HRESULT XBR::Patch(DWORD dwType, BYTE *pHeader)
{
	HRESULT hr;
	if (dwType == XBRC_INDEXBUFFER)
	{
		// Patch index data offset
		D3DIndexBuffer *pIB = (D3DIndexBuffer *)pHeader;
		pIB->Data += (DWORD)pIB;	// convert structure offset to pointer
	}
	else if (dwType == XBRC_VERTEXSHADER)
	{
		// Currently, we keep references to resources as indices.
		// We could instead patch these to resource pointers.
		// The vertex shader refers to other vertex
		// shaders and to vertex buffers for stream data.
		VertexShader *pVertexShader = (VertexShader *)pHeader;
		pVertexShader->Declaration = (DWORD *)(pHeader + (DWORD)pVertexShader->Declaration);
		pVertexShader->Function = (DWORD *)(pHeader + (DWORD)pVertexShader->Function);
	}
	else if (dwType == XBRC_PIXELSHADER)
	{
		// Pixel shaders are defined inline and do not need patching.
	}
	else if (dwType == XBRC_EFFECT)
	{
		// Patch the file offsets to memory offsets
		Effect *pEffect = (Effect *)pHeader;
		pEffect->rPass = (Pass *)(pHeader + (DWORD)pEffect->rPass);	// overwrite rPass
		for (UINT iPass = 0; iPass < pEffect->PassCount; iPass++)
		{
			Pass *pPass = &pEffect->rPass[iPass];
			pPass->rRenderState = (RenderState *)(pHeader + (DWORD)pPass->rRenderState);
			pPass->rTextureStage = (TextureStage *)(pHeader + (DWORD)pPass->rTextureStage);
			for (UINT iTextureStage = 0; iTextureStage < pPass->TextureStageCount; iTextureStage++)
			{
				TextureStage *pTextureStage = &pPass->rTextureStage[iTextureStage];
				pTextureStage->rTextureState = (TextureState *)(pHeader + (DWORD)pTextureStage->rTextureState);
			}
			pPass->rDraw = (Draw *)(pHeader + (DWORD)pPass->rDraw);
		}
	}
	else if (dwType == XBRC_SKELETON)
	{
		// Patch the file offsets to memory offsets
		Skeleton *pSkeleton = (Skeleton *)pHeader;
		*(DWORD *)&pSkeleton->m_rMatrix += (DWORD)m_pSysMemData;
		*(DWORD *)&pSkeleton->m_pRoot += (DWORD)m_pSysMemData;
		Frame *pFrame = pSkeleton->m_pRoot;
		for (UINT iFrame = 0; iFrame < pSkeleton->m_FrameCount; iFrame++)
		{
			if (pFrame->m_pChild != NULL)
				*(DWORD *)&pFrame->m_pChild += (DWORD)m_pSysMemData;
			if (pFrame->m_pNext != NULL)
				*(DWORD *)&pFrame->m_pNext += (DWORD)m_pSysMemData;
			pFrame++;	// frames are laid out in an array
		}
	}
	else if (dwType == XBRC_ANIMATION)
	{
		// Patch the file offsets to memory offsets
		Animation *pAnimation = (Animation *)pHeader;
		if (pAnimation->m_rVSP != NULL)
		{
			*(DWORD *)&pAnimation->m_rVSP += (DWORD)m_pSysMemData;
			for (UINT iVSP = 0; iVSP < pAnimation->m_VSPCount; iVSP++)
			{
				AnimVSP *pVSP = &pAnimation->m_rVSP[iVSP];
				*(DWORD *)&pVSP->m_pSource += (DWORD)m_pSysMemData;	// source matrix
			}
		}
		if (pAnimation->m_pAnimList != NULL)
		{
			*(DWORD *)&pAnimation->m_pAnimList += (DWORD)m_pSysMemData;
			for (UINT iAnim = 0; iAnim < pAnimation->m_AnimCount; iAnim++)
			{
				AnimLink *pAnim = &pAnimation->m_pAnimList[iAnim];	// list is laid out in an array
				if (pAnim->m_pNext != NULL)
					*(DWORD *)&pAnim->m_pNext += (DWORD)m_pSysMemData;
				*(DWORD *)&pAnim->m_pAnimFrame += (DWORD)m_pSysMemData;
				hr = pAnim->m_pAnimFrame->Patch(m_pSysMemData);
				if (FAILED(hr))
					return hr;
				*(DWORD *)&pAnim->m_pFrame += (DWORD)m_pSysMemData;	// target frame
			}
		}
	}
	else if (dwType == XBRC_SYMBOLTABLE)
	{
		// Use symbol table only if user did not specify a resource tag array
		if (m_pResourceTags != NULL)
			return S_OK;

		// Get symbol count and resource tag pointer
		m_dwSymbolCount = *(DWORD *)pHeader;
		m_pResourceTags = (XBRESOURCE *)(pHeader + sizeof(DWORD));
		
		// Convert file offsets to string pointers
		for (UINT iSymbol = 0; iSymbol < m_dwSymbolCount; iSymbol++)
			*(DWORD *)&m_pResourceTags[iSymbol].strName += *(DWORD *)&m_pSysMemData;
	}
	else
		return Resource::Patch(dwType, pHeader);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Called by Unload to release handles, etc. to cleanup the resource before unloading.
//-----------------------------------------------------------------------------
HRESULT XBR::Cleanup(DWORD dwType, BYTE *pHeader)
{
	// Currently, VertexShaders are the only resource with "dangling" references
	if (dwType == XBRC_VERTEXSHADER)
	{
		VertexShader *pVertexShader = (VertexShader *)pHeader;
		if (pVertexShader->Handle)
			m_pd3dDevice->DeleteVertexShader(pVertexShader->Handle);
	}
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SetVertexShaderConstant
// Desc: Look through VertexShader Declaration and set constant
//-----------------------------------------------------------------------------
static HRESULT SetVertexShaderConstant(VertexShader *pVertexShader, INT Register, CONST VOID *pConstantData, DWORD ConstantCount)
{
	if (pVertexShader->DeclarationByteCount == 0)
		return E_INVALIDARG;	// no declaration to parse
	DWORD *pToken = (DWORD *)pVertexShader->Declaration;
	while (*pToken != D3DVSD_END() )
	{
		switch (*pToken & D3DVSD_TOKENTYPEMASK)
		{
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM):
		{
			// parse the token
			INT addr = ((*pToken & D3DVSD_CONSTADDRESSMASK) >> D3DVSD_CONSTADDRESSSHIFT) - 96;
			UINT count = (*pToken & D3DVSD_CONSTCOUNTMASK) >> D3DVSD_CONSTCOUNTSHIFT;
			FLOAT *pValue = (float *)(pToken + 1);
			if (Register < (INT)(addr + count) && addr < (INT)(Register + ConstantCount))
			{
				// get the range to set
				INT addrStart = Register;
				if (addrStart < addr)
					addrStart = addr;
				INT addrEnd = Register + ConstantCount;
				if (addrEnd > (INT)(addr + count))
					addrEnd = addr + count;
				
				// set the constants
				FLOAT *pValueSrc = (FLOAT *)pConstantData + 4 * (addrStart - Register);
				FLOAT *pValueDst = pValue + 4 * (addrStart - addr);
				memcpy(pValueDst, pValueSrc, 4 * (addrEnd - addrStart) * sizeof(float));
			}
			pToken += 4 * count;
			break;
		}

		// ignore stream and nop tokens
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM):
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA):
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_NOP):
			break;
			
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_END):
			goto loop_break;
			
		default:
			Printf(L"Vertex shader declaration token (0x%x) not handled.\n", *pToken );
			return E_FAIL;
		}

		// Go to the next token in the stream
		pToken++;
		if ((BYTE *)pToken - (BYTE *)pVertexShader->Declaration > (INT)pVertexShader->DeclarationByteCount)
		{
			Out(L"Vertex shader declaration missing end token.\n");
			goto loop_break;
		}
	}
 loop_break:
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ApplyVertexShaderConstants
// Desc: Call m_pd3dDevice->SetVertexShaderConstants on all the constants
//       in the declaration.
//-----------------------------------------------------------------------------
static HRESULT ApplyVertexShaderConstants(LPDIRECT3DDEVICE8 pd3dDevice,
										  VertexShader *pVertexShader,
										  bool *pbStreamDeclaration)
{
	if (pbStreamDeclaration)
		*pbStreamDeclaration = false;
	if (pVertexShader->DeclarationByteCount == 0)
		return S_FALSE;	// no declaration to parse
	static bool bDumpConstants = false;	// debug
	DWORD *pToken = (DWORD *)pVertexShader->Declaration;
	while (*pToken != D3DVSD_END() )
	{
		switch (*pToken & D3DVSD_TOKENTYPEMASK)
		{
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM):
		{
			// parse the token
			int addr = ((*pToken & D3DVSD_CONSTADDRESSMASK) >> D3DVSD_CONSTADDRESSSHIFT) - 96;
			UINT count = (*pToken & D3DVSD_CONSTCOUNTMASK) >> D3DVSD_CONSTCOUNTSHIFT;
					
			// set the constants
			pd3dDevice->SetVertexShaderConstant(addr, pToken + 1, count);

			if (bDumpConstants)	// debug
			{
				// print out the constants for debugging
				float *pValue = (float *)(pToken + 1);
				for (UINT i = 0; i < count; i++)
				{
					Printf(L"c%d %g %g %g %g\n",
						   addr + i, 
						   *(float *)&pValue[i*4], 
						   *(float *)&pValue[i*4+1],
						   *(float *)&pValue[i*4+2], 
						   *(float *)&pValue[i*4+3]);
				}
			}
			
			pToken += 4 * count;
			break;
		}
		
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM):
			// Ignore stream tokens
			break;
					
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA):
			if (pbStreamDeclaration)
				*pbStreamDeclaration = true;
			break;
					
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_NOP):
			break;
			
		case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_END):
			goto loop_break;
			
		default:
			Printf(L"Vertex shader declaration token (0x%x) not handled.\n", *pToken );
			return E_FAIL;
		}

		// Go to the next token in the stream
		pToken++;
		if ((BYTE *)pToken - (BYTE *)pVertexShader->Declaration > (INT)pVertexShader->DeclarationByteCount)
		{
			Out(L"Vertex shader declaration missing end token.\n");
			goto loop_break;
		}
	}
 loop_break:
	bDumpConstants = false;		// debug
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetVertexShaderResource
// Desc: Create a vertex shader from a resource and set it to be current
//-----------------------------------------------------------------------------
HRESULT XBR::SetVertexShaderResource(DWORD VertexShaderResourceIndex)
{
	HRESULT hr;
	if (VertexShaderResourceIndex == 0)
		return S_FALSE;	// NULL
	if (this->GetTypeByIndex(VertexShaderResourceIndex - 1) != XBRC_VERTEXSHADER)
		return E_FAIL;	// bad type
	VertexShader *pVS = (VertexShader *)this->GetResourceByIndex(VertexShaderResourceIndex - 1);
	VertexShader *pVSRef = NULL;
	if (pVS->VertexShaderReference == 0)
	{
		// No vertex shader reference, so simply set the shader.
		if (pVS->Handle == 0)
		{
			// Create the vertex shader
			DWORD flags = 0;
			hr = m_pd3dDevice->CreateVertexShader( pVS->Declaration, pVS->Function, &pVS->Handle, flags );
			if (FAILED(hr))
				return hr;
		}
		// new version of d3d complains if SetVertexShaderInput is active when we
		// call SetVertexShader, so turn it off first
		hr = m_pd3dDevice->SetVertexShaderInput(0, 0, 0);
		if (FAILED(hr))
			return hr;
			
		hr = m_pd3dDevice->SetVertexShader(pVS->Handle);
		if (FAILED(hr))
			return hr;

		// Set any vertex shader constants that may be animated
		// TODO: Fix redundancy for non-animated vertex shader constants.
		bool bStreamDeclaration;
		hr = ApplyVertexShaderConstants(m_pd3dDevice, pVS, &bStreamDeclaration);
		if (FAILED(hr))
			return hr;
		
		// Map vertex buffers to streams
		D3DSTREAM_INPUT rStreamInput[VERTEXSHADER_MAX_STREAMS];
		UINT StreamCount = 0;
		for (UINT iStream = 0; iStream < VERTEXSHADER_MAX_STREAMS; iStream++)
		{
			DWORD iVB = pVS->rStreamInput[iStream].VertexBufferReference;
			if (iVB != 0)
			{
				if (iVB - 1 >= this->Count()
					|| this->GetTypeByIndex(iVB - 1) != D3DCOMMON_TYPE_VERTEXBUFFER)
					return E_FAIL;
				rStreamInput[iStream].VertexBuffer = (D3DVertexBuffer *)this->GetResourceByIndex(iVB - 1);
				rStreamInput[iStream].Stride = pVS->rStreamInput[iStream].Stride;
				rStreamInput[iStream].Offset = pVS->rStreamInput[iStream].Offset;
				StreamCount = iStream + 1;	// use maximum stream index to set count
			}
			else
			{
				rStreamInput[iStream].VertexBuffer = NULL;
				rStreamInput[iStream].Stride = 0;
				rStreamInput[iStream].Offset = 0;
			}
		}
		if (StreamCount)
		{
			hr = m_pd3dDevice->SetVertexShaderInput(pVS->Handle, StreamCount, rStreamInput);
			if (FAILED(hr))
				return hr;
		}
	}
	else
	{
		// Lookup vertex shader reference
		
		// TODO: Resolve all this stuff at compile time.

		// We should probably move away from using the vertex shader
		// reference business, and separate out the declaration from
		// the constants and stream mapping.

		// Right now, the only reference supported is a change in the
		// stream mapping and a change in the constants.
		hr = SetVertexShaderResource(pVS->VertexShaderReference);
		if (FAILED(hr))
			return hr;
		
		if (pVS->FunctionByteCount != 0)
			return E_FAIL;

		// Get the reference vertex shader description
		pVSRef = (VertexShader *)this->GetResourceByIndex(pVS->VertexShaderReference - 1);

		// Look through our own declaration for constants. For
		// shaders that reference other shaders, this is the
		// way that constants are over-ridden from the default.
		bool bStreamDeclaration = false;	// and check to see if the declaration has changed
		hr = ApplyVertexShaderConstants(m_pd3dDevice, pVS, &bStreamDeclaration);
		if (FAILED(hr))
			return hr;
		if (bStreamDeclaration)
		{
			// TODO: We redeclared the shader declaration, so we need to create a new version of the vertex shader
		}

		// Map vertex buffers to streams
		UINT StreamCount = 0;
		D3DSTREAM_INPUT rStreamInput[VERTEXSHADER_MAX_STREAMS];		// TODO: include stream count in VertexShader class
		for (UINT iStream = 0; iStream < VERTEXSHADER_MAX_STREAMS; iStream++)
		{
			// Override stream mapping using the new vertex shader
			DWORD iVB = pVS->rStreamInput[iStream].VertexBufferReference;
			if (iVB == 0)
				iVB = pVSRef->rStreamInput[iStream].VertexBufferReference;
			if (iVB != 0)
			{
				if (iVB - 1 >= this->Count()
					|| this->GetTypeByIndex(iVB - 1) != D3DCOMMON_TYPE_VERTEXBUFFER)
					return E_FAIL;
				rStreamInput[iStream].VertexBuffer = (D3DVertexBuffer *)this->GetResourceByIndex(iVB - 1);
				if (pVS->rStreamInput[iStream].Stride != 0)
				{
					// use the new stream
					rStreamInput[iStream].Stride = pVS->rStreamInput[iStream].Stride;
					rStreamInput[iStream].Offset = pVS->rStreamInput[iStream].Offset;
				}
				else
				{
					// use the reference stream
					rStreamInput[iStream].Stride = pVSRef->rStreamInput[iStream].Stride;
					rStreamInput[iStream].Offset = pVSRef->rStreamInput[iStream].Offset;
				}
				StreamCount = iStream + 1;	// use maximum stream touched as stream count
			}
			else
			{
				rStreamInput[iStream].VertexBuffer = NULL;
				rStreamInput[iStream].Stride = 0;
				rStreamInput[iStream].Offset = 0;
			}
		}
		if (StreamCount > 0)
		{
			hr = m_pd3dDevice->SetVertexShaderInput(pVSRef->Handle, StreamCount, rStreamInput);
			if (FAILED(hr))
				return hr;
		}
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetPixelShaderResource
// Desc: Create a pixel shader from a resource and set it to be current
//-----------------------------------------------------------------------------
HRESULT XBR::SetPixelShaderResource(DWORD PixelShaderResourceIndex)
{
	if (PixelShaderResourceIndex == 0)
		return S_FALSE;	// NULL
	D3DPIXELSHADERDEF *psd = (D3DPIXELSHADERDEF *)this->GetResourceByIndex(PixelShaderResourceIndex - 1);
	return m_pd3dDevice->SetPixelShaderProgram(psd);
}

//-----------------------------------------------------------------------------
// Name: SetIndexBufferResource
// Desc: Set the current index buffer from a resource index
//-----------------------------------------------------------------------------
HRESULT XBR::SetIndexBufferResource(DWORD IndexBufferResourceIndex)
{
	if (IndexBufferResourceIndex == 0)
		return S_FALSE;	// NULL
	if (IndexBufferResourceIndex - 1 >= this->Count()
		|| this->GetTypeByIndex(IndexBufferResourceIndex - 1) != XBRC_INDEXBUFFER)
		return E_FAIL;
	D3DIndexBuffer *pIB = (D3DIndexBuffer *)this->GetResourceByIndex(IndexBufferResourceIndex - 1);
	UINT BaseVertexIndex = 0;	// TODO: should we include this in the <Draw> tag
	return m_pd3dDevice->SetIndices(pIB, BaseVertexIndex);
}

//-----------------------------------------------------------------------------
// Name: SetTextureResource
// Desc: Set a texture resource to be current
//-----------------------------------------------------------------------------
HRESULT XBR::SetTextureResource(DWORD iTextureStage, DWORD TextureResourceIndex)
{
	if (TextureResourceIndex == 0)
		return S_FALSE;	// NULL
	if (TextureResourceIndex - 1 >= this->Count()
		|| this->GetTypeByIndex(TextureResourceIndex - 1) != D3DCOMMON_TYPE_TEXTURE)
		return E_FAIL;
	D3DBaseTexture *pBaseTexture = (D3DBaseTexture *)this->GetResourceByIndex(TextureResourceIndex - 1);
	return m_pd3dDevice->SetTexture( iTextureStage, pBaseTexture);
}


//-----------------------------------------------------------------------------
// Name: DrawEffect
// Desc: Execute the currently selected effect
//-----------------------------------------------------------------------------
HRESULT XBR::DrawEffect(Effect *pEffect)
{
	m_pd3dDevice->SetShaderConstantMode(D3DSCM_192CONSTANTS);
	
	// ignore RenderTargetResource for now
	for (UINT iPass = 0; iPass < pEffect->PassCount; iPass++)
	{
		Pass *pPass = &pEffect->rPass[iPass];
		if (pPass->VertexShaderResource != 0)
			SetVertexShaderResource(pPass->VertexShaderResource);
		if (pPass->PixelShaderResource != 0)
			SetPixelShaderResource(pPass->PixelShaderResource);
		for (UINT iRS = 0; iRS < pPass->RenderStateCount; iRS++)
		{
			RenderState *pRS = &pPass->rRenderState[iRS];
			m_pd3dDevice->SetRenderStateNotInline((D3DRENDERSTATETYPE)pRS->State, pRS->Value);
		}
		for (UINT iTS = 0; iTS < pPass->TextureStageCount; iTS++)
		{
			TextureStage *pTS = &pPass->rTextureStage[iTS];
			if (pTS->TextureResource != 0)
				SetTextureResource(iTS, pTS->TextureResource);
			for (UINT iTSS = 0; iTSS < pTS->TextureStateCount; iTSS++)
			{
				TextureState *pTSS = &pTS->rTextureState[iTSS];
				m_pd3dDevice->SetTextureStageStateNotInline(iTS, (D3DTEXTURESTAGESTATETYPE)pTSS->State, pTSS->Value);
			}
		}
		for (UINT iDraw = 0; iDraw < pPass->DrawCount; iDraw++)
		{
			Draw *pDraw = &pPass->rDraw[iDraw];
			if (pDraw->IndexBufferResource != 0)
			{
				SetIndexBufferResource(pDraw->IndexBufferResource);	// This sets D3D__IndexData
				// m_pd3dDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)pDraw->Primitive, 0, 0, pDraw->Start, pDraw->Count);
				m_pd3dDevice->DrawIndexedVertices((D3DPRIMITIVETYPE)pDraw->Primitive, pDraw->Count, D3D__IndexData + pDraw->Start);
			}
			else
			{
				m_pd3dDevice->DrawVertices((D3DPRIMITIVETYPE)pDraw->Primitive, pDraw->Start, pDraw->Count);
			}
		}
	}

	m_pd3dDevice->SetShaderConstantMode(D3DSCM_96CONSTANTS);
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SetTime
// Desc: Set the time for all of the animations in the resource set
//-----------------------------------------------------------------------------
HRESULT XBR::SetTime(float fGlobalTime)
{
	// TODO: this will not work if there are multiple animation
	// resources for the same skeleton, which is a very common
	// case. To fix this, add a selection which binds effect,
	// geometry, skeleton, and animation together for evaluation.
	
	// Walk the list of resources, looking for animations
	UINT iResource;
	for (iResource = 0; iResource < this->Count(); iResource++)
	{
		if (this->GetTypeByIndex(iResource) == XBRC_ANIMATION)
		{
			// Sample animation curves
			Animation *pAnimation = (Animation *)this->GetResourceByIndex(iResource);
			AnimLink *pAnimCurr = pAnimation->m_pAnimList;
			while (pAnimCurr != NULL)
			{
				pAnimCurr->m_pAnimFrame->SetTime(&pAnimCurr->m_pFrame->m_Matrix, fGlobalTime);
				pAnimCurr = pAnimCurr->m_pNext;
			}
		}
	}

	// Walk the list of resources, looking for skeletons
	for (iResource = 0; iResource < this->Count(); iResource++)
	{
		if (this->GetTypeByIndex(iResource) == XBRC_SKELETON)
		{
			// Update matrix hierarchy
			Skeleton *pSkeleton = (Skeleton *)this->GetResourceByIndex(iResource);
			pSkeleton->UpdateTransformations();
		}
	}

	// TODO: this will not work if there are multiple animation
	// resources for the same vertex shader, which is a very common case.

	// TODO: Move VSP's out of animation class and make a first-class object
	// that would be useful as a callback for generic parameter setting
	
	// Walk the list of resources, looking for animations
	for (UINT iResource = 0; iResource < this->Count(); iResource++)
	{
		if (this->GetTypeByIndex(iResource) == XBRC_ANIMATION)
		{
			// Set the vertex shader parameters that correspond to the VertexShaderResourceIndex
			Animation *pAnimation = (Animation *)this->GetResourceByIndex(iResource);
			for (UINT iVSP = 0; iVSP < pAnimation->m_VSPCount; iVSP++)
			{
				AnimVSP *pVSP = &pAnimation->m_rVSP[iVSP];
				VertexShader *pVertexShader = (VertexShader *)this->GetResourceByIndex(pVSP->m_VertexShaderReference - 1);
				
				// TODO: increase the kinds of supported animation
				XGMATRIX *pMatrix = (XGMATRIX *)pVSP->m_pSource;
				XGMATRIX mat;
				XGMatrixTranspose(&mat, pMatrix);

				// Set the constant in the declaration, so that when the vertex shader
				// is made active, we'll use the updated value.
				SetVertexShaderConstant(pVertexShader, pVSP->m_Index, (FLOAT *)mat, pVSP->m_Count);
			}
		}
	}
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: PrintVertexShaderDeclaration
// Desc: Print the vertex shader declaration to the debug display.
//-----------------------------------------------------------------------------
HRESULT XBR::PrintVertexShaderDeclaration(DWORD VertexShaderResourceIndex)
{
	VertexShader *pVertexShader = (VertexShader *)GetResourceByIndex(VertexShaderResourceIndex - 1);
	HRESULT hr = S_OK;
	DWORD *pToken = pVertexShader->Declaration;
	while (*pToken != D3DVSD_END() )
	{
		switch (*pToken & D3DVSD_TOKENTYPEMASK)
		{
			
        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM):
		{
			// Get stream index
            UINT iStream = *pToken & 0xf;
			Printf(L"D3DVSD_STREAM(%d),\n", iStream);
			break;
		}
		
        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA):
		{
            if (*pToken & 0x10000000) // skip
            {
                // D3DVSD_SKIP or D3DVSD_SKIPBYTES macro
				UINT count = (*pToken & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;
                if (!(*pToken & 0x08000000))
  					Printf(L"\tD3DVSD_SKIP(%d),\t// skip %d bytes\n", count, count * 4);
  				else
					Printf(L"\tD3DVSD_SKIPBYTES(%d),\n", count);
			}
			else	// data
			{
                DWORD sizeAndType = (*pToken & D3DVSD_DATATYPEMASK) >> D3DVSD_DATATYPESHIFT;
                DWORD reg = (*pToken & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT;
				switch (sizeAndType)
				{
				case D3DVSDT_FLOAT1      :    // 1D float expanded to (value, 0., 0., 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_FLOAT1),\n", reg);
					break;
					
				case D3DVSDT_FLOAT2      :    // 2D float expanded to (value, value, 0., 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_FLOAT2),\n", reg);
					break;
					
				case D3DVSDT_FLOAT2H     :    // 2D homogeneous float expanded to (value, value,0., value.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_FLOAT2H),\n", reg);
					break;
					
				case D3DVSDT_FLOAT3      :    // 3D float expanded to (value, value, value, 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_FLOAT3),\n", reg);
					break;
					
				case D3DVSDT_FLOAT4      :    // 4D float
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_FLOAT4),\n", reg);
					break;
					
				case D3DVSDT_D3DCOLOR    :    // 4D packed unsigned bytes mapped to 0. to 1. range
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_D3DCOLOR),\n", reg);
					break;
					
				// Signed shorts map to the range [-32768, 32767]
				case D3DVSDT_SHORT1      :    // 1D signed short expanded to (value, 0., 0., 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_SHORT1),\n", reg);
					break;

				case D3DVSDT_SHORT2      :    // 2D signed short expanded to (value, value, 0., 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_SHORT2),\n", reg);
					break;

				case D3DVSDT_SHORT3      :    // 3D signed short expanded to (value, value, value, 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_SHORT3),\n", reg);
					break;

				case D3DVSDT_SHORT4      :    // 4D signed short
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_SHORT4),\n", reg);
					break;

				// (signed, normalized short maps from -1.0 to 1.0)
				case D3DVSDT_NORMSHORT1  :    // 1D signed, normalized short expanded to (value, 0, 0., 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_NORMSHORT1),\n", reg);
					break;
				
				case D3DVSDT_NORMSHORT2  :    // 2D signed, normalized short expanded to (value, value, 0., 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_NORMSHORT2),\n", reg);
					break;

				case D3DVSDT_NORMSHORT3  :    // 3D signed, normalized short expanded to (value, value, value, 1.)  
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_NORMSHORT3),\n", reg);
					break;

				case D3DVSDT_NORMSHORT4  :    // 4D signed, normalized short expanded to (value, value, value, value)  
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_NORMSHORT4),\n", reg);
					break;

				case D3DVSDT_NORMPACKED3 :    // 3 signed, normalized components packed in 32-bits.  (11,11,10).  
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_NORMPACKED3),\n", reg);
					break;
				
				// Packed bytes map to the range [0, 1]
				case D3DVSDT_PBYTE1      :    // 1D packed byte expanded to (value, 0., 0., 1.) 
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_PBYTE1),\n", reg);
					break;
				
				case D3DVSDT_PBYTE2      :    // 2D packed byte expanded to (value, value, 0., 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_PBYTE2),\n", reg);
					break;

				case D3DVSDT_PBYTE3      :    // 3D packed byte expanded to (value, value, value, 1.)
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_PBYTE3),\n", reg);
					break;

				case D3DVSDT_PBYTE4      :    // 4D packed byte expanded to (value, value, value, value) 
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_PBYTE4),\n", reg);
					break;

				case D3DVSDT_NONE        :    // No stream data
					Printf(L"\tD3DVSD_REG(%d, D3DVSDT_NONE),\n", reg);
					break;

				default:
					Printf(L"Bad vertex stream data type encoding (no D3DVSDT_* corresponds to 0x%x).", sizeAndType);
					hr = E_FAIL;
					goto e_Exit;
				}
			}
			break;
		}
			
        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM):
		{
			int addr = ((*pToken & D3DVSD_CONSTADDRESSMASK) >> D3DVSD_CONSTADDRESSSHIFT) - 96;
            UINT count = (*pToken & D3DVSD_CONSTCOUNTMASK) >> D3DVSD_CONSTCOUNTSHIFT;
			// Printf(L"\tD3DVSD_CONST(%d, %d),\n", addr, count);
			for (UINT i = 0; i < count; i++)
			{
				FLOAT *pValue = (float *)&pToken[1];
				Printf(L"\tc%d %g %g %g %g\n",
					   addr + i, pValue[0], pValue[1], pValue[2], pValue[3]);
				pToken += 4;
			}
			break;
		}
			
        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_NOP):
			Out(L"\tD3DVSD_NOP(),\n");
            break;
			
        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_TESSELLATOR):
			Out(L"Tessellator stream parsing not implemented.\n");
			hr = E_NOTIMPL;
			goto e_Exit;
			
		// case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_END):
			// we should not hit this, since this is the main loop test
			
		default:
			Printf(L"unknown token (0x%x) in the vertex shader declaration.\n", *pToken );
			hr = E_NOTIMPL;
			goto e_Exit;
		}
		pToken++; // Go to the next token in the stream
	}
	Out(L"\tD3DVSD_END()\n");
 e_Exit:
	return hr;
}

//-----------------------------------------------------------------------------
// Name: PrintVertexShaderInputs
// Desc: Print the vertex shader inputs to the debug display.
//-----------------------------------------------------------------------------
HRESULT XBR::PrintVertexShaderInputs(DWORD VertexShaderResourceIndex)
{
	VertexShader *pVS = (VertexShader *)GetResourceByIndex(VertexShaderResourceIndex - 1);
	VertexShader *pVSRef = NULL;
	if (pVS->VertexShaderReference != 0)
	{
		// Lookup vertex shader reference
		if (pVS->VertexShaderReference - 1 >= this->Count()
			|| this->GetTypeByIndex(pVS->VertexShaderReference - 1) != XBRC_VERTEXSHADER)
			return E_FAIL;
		pVSRef = (VertexShader *)this->GetResourceByIndex(pVS->VertexShaderReference - 1);
	}
	// Dump vertex buffer mapping to stream
	for (UINT iStream = 0; iStream < VERTEXSHADER_MAX_STREAMS; iStream++)
	{
		DWORD iVB = pVS->rStreamInput[iStream].VertexBufferReference;
		if (iVB == 0 && pVSRef != NULL)
			iVB = pVSRef->rStreamInput[iStream].VertexBufferReference;
		if (iVB != 0)
		{
			bool bGoodVB = (iVB <= this->Count()) && this->GetTypeByIndex(iVB - 1) == D3DCOMMON_TYPE_VERTEXBUFFER;
			DWORD Stride = 0;
			DWORD Offset = 0;
			if (pVS->rStreamInput[iStream].Stride != 0)
			{
				// use the stride and offset 
				Stride = pVS->rStreamInput[iStream].Stride;
				Offset = pVS->rStreamInput[iStream].Offset;
			}
			else if (pVSRef != NULL)
			{
				// use the reference stride and offset
				Stride = pVSRef->rStreamInput[iStream].Stride;
				Offset = pVSRef->rStreamInput[iStream].Offset;
			}
			Printf(L"Stream%d : VertexBuffer=%d%s Stride=%d Offset=%d\n",
				iStream, iVB, bGoodVB ? L"" : L"<invalid resource>", Stride, Offset);
		}
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetSymbolName
// Desc: Return name of symbol at given index, or NULL if no symbol table
//-----------------------------------------------------------------------------
CONST CHAR *XBR::GetSymbolName(DWORD SymbolIndex)
{
	if (SymbolIndex >= m_dwSymbolCount)
		return NULL;
	else
		return m_pResourceTags[SymbolIndex].strName;
}


//-----------------------------------------------------------------------------
// Name: GetResourceIndexBySymbolIndex
// Desc: Return index of resource with given symbol index
//-----------------------------------------------------------------------------
DWORD XBR::GetResourceIndexBySymbolIndex(DWORD SymbolIndex)
{
	// TODO: reconsider symbol table sorting
	// Look through index table for matching offset
	for (DWORD ResourceIndex = 0; ResourceIndex < m_dwNumResources; ResourceIndex++)
	{
		DWORD dwType = m_dwTypes[ ResourceIndex ];
		DWORD dwOffset = m_ppResources[ ResourceIndex ] - m_pSysMemData;
		if (dwType & XBRC_USER_DATA_FLAG)
			dwOffset -= 8;	// move offset back to original type + size fields
		if (dwOffset == m_pResourceTags[SymbolIndex].dwOffset)
			return ResourceIndex;
	}
	return (DWORD)-1;
}

//-----------------------------------------------------------------------------
// Name: GetSymbolIndex
// Desc: Return symbol index with given symbol name. 
//-----------------------------------------------------------------------------
DWORD XBR::GetSymbolIndex(CONST CHAR *strSymbol)
{ 
	// Look through symbol table for matching symbol
	for (DWORD SymbolIndex = 0; SymbolIndex < m_dwSymbolCount; SymbolIndex++)
	{
		if (!strcmp(strSymbol, m_pResourceTags[SymbolIndex].strName))
			return SymbolIndex;
	}
	return (DWORD)-1;
}


