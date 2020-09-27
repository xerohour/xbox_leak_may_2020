//-----------------------------------------------------------------------------
// File: CVertexShader.cpp
//
// Desc: Contains the VertexShader-specific logic for the bundler tool
//
// Hist: 2001.09.26 - New for November XDK release
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "CVertexShader.h"
#include "Bundler.h"
#include "effect.h"

//-----------------------------------------------------------------------------
// Name: CVertexShader()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CVertexShader::CVertexShader( CBundler* pBundler )
{
    m_pBundler = pBundler;
	ZeroMemory(m_Identifier, sizeof(m_Identifier));
	m_dwDeclarationCount = 0;
	m_rdwDeclaration[m_dwDeclarationCount] = 0xffffffff;	// terminator
	m_cbFunctionSize = 0;
	m_pFunction = NULL;
	ZeroMemory(m_rbConstant, sizeof(m_rbConstant));
	m_dwVertexShaderReference = 0;
	ZeroMemory(m_rStreamInput, sizeof(m_rStreamInput));
}




//-----------------------------------------------------------------------------
// Name: ~CVertexShader (destructor)
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CVertexShader::~CVertexShader()
{
	if (m_pFunction)
		delete [] m_pFunction;
}



//-----------------------------------------------------------------------------
// Name: SetStream
// Desc: Add a stream token
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SetStream(UINT iStream)
{
	if (m_dwDeclarationCount >= VERTEXSHADER_MAX_DECLARATION_COUNT)
		return E_FAIL;
	m_rdwDeclaration[m_dwDeclarationCount++] = XD3DVSD_STREAM(iStream);
	m_rdwDeclaration[m_dwDeclarationCount] = 0xffffffff;	// terminator
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetVertexRegisterFormat
// Desc: Add a vertex register token to the declaration
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SetVertexRegisterFormat(UINT iVertexRegister, DWORD VertexFormat)
{
	if (m_dwDeclarationCount >= VERTEXSHADER_MAX_DECLARATION_COUNT)
		return E_FAIL;
	m_rdwDeclaration[m_dwDeclarationCount++] = XD3DVSD_REG(iVertexRegister, VertexFormat);
	m_rdwDeclaration[m_dwDeclarationCount] = 0xffffffff;	// terminator
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SkipBytes
// Desc: Add a SkipBytes token to the declaration
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SkipBytes(UINT Count)
{
	if (m_dwDeclarationCount >= VERTEXSHADER_MAX_DECLARATION_COUNT)
		return E_FAIL;
	m_rdwDeclaration[m_dwDeclarationCount++] = XD3DVSD_SKIPBYTES(Count);
	m_rdwDeclaration[m_dwDeclarationCount] = 0xffffffff;	// terminator
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetDeclaration
// Desc: Parse declaration to set streams and constants
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SetConstant(INT Register, CONST void *pConstantData, DWORD ConstantCount)
{
	CONST FLOAT *pfConstant = (CONST FLOAT *)pConstantData;
	for (UINT i = 0; i < ConstantCount; i++)
	{
		INT iRegister = Register + (INT)i + 96;	// map -96,96 range to 0,192
		if (iRegister < 0 || iRegister >= XD3DVS_CONSTREG_COUNT)
			return E_FAIL;
		m_rbConstant[iRegister] = TRUE;	// mark register as active
		memcpy(m_rfConstant[iRegister], pfConstant, sizeof(float) * 4);
		pfConstant += 4;
	}
	return S_OK;
}
	
//-----------------------------------------------------------------------------
// Name: AddConstantsToDeclaration
// Desc: Look through active constants in m_rbConstant and add to declaration
//-----------------------------------------------------------------------------
HRESULT CVertexShader::AddConstantsToDeclaration()
{
	for (INT iConstant = 0; iConstant < XD3DVS_CONSTREG_COUNT; iConstant++)
	{
		if (m_rbConstant[iConstant])
		{
			if (m_dwDeclarationCount + 5 >= VERTEXSHADER_MAX_DECLARATION_COUNT)
			{
				m_rdwDeclaration[m_dwDeclarationCount] = 0xffffffff;	// terminator
				m_pBundler->ErrorMsg("VertexShader: Too many constants to fit in declaration\n");
				return E_FAIL;
			}
			m_rdwDeclaration[m_dwDeclarationCount] = XD3DVSD_CONST( iConstant - 96, 1 );
			memcpy(m_rdwDeclaration + m_dwDeclarationCount + 1, m_rfConstant[iConstant], sizeof(float) * 4);
			m_dwDeclarationCount += 5;
		}
	}
	m_rdwDeclaration[m_dwDeclarationCount] = 0xffffffff;	// terminator
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetStridesFromDeclaration
// Desc: Look through declaration for vertex register definitions and set
//       the appropriate strides.
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SetStridesFromDeclaration()
{
	HRESULT hr = S_OK;
	int cbDeclaration = (m_dwDeclarationCount + 1) * sizeof(DWORD);
	DWORD *pToken = m_rdwDeclaration;
	int iActiveStream = -1;
	UINT Stride = 0;
	while (*pToken != XD3DVSD_END() )
	{
		switch (*pToken & XD3DVSD_TOKENTYPEMASK)
		{
        case XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_STREAM):
		{
			if (iActiveStream != -1 && Stride != 0)
			{
				// Set Stride if currently 0, otherwise check against calculated stride.
				if (m_rStreamInput[iActiveStream].Stride == 0)
					m_rStreamInput[iActiveStream].Stride = Stride;
				else
				{
					if (Stride != m_rStreamInput[iActiveStream].Stride)
					{
						m_pBundler->ErrorMsg("SetStridesFromDeclaration: stream %d given stride (%d) does not match computed stride (%d).\n",
											 iActiveStream, m_rStreamInput[iActiveStream].Stride, Stride);
						// Leave as warning
					}
				}
			}
			
			// Get stream index
            UINT iStream = *pToken & 0xf;
			if (iStream >= VERTEXSHADER_MAX_STREAMS)
			{
				m_pBundler->ErrorMsg("SetStridesFromDeclaration: stream index out of range (%d >= %d).\n", iStream, VERTEXSHADER_MAX_STREAMS);
				hr = E_FAIL;
				goto e_Exit;
			}
			iActiveStream = iStream;
			Stride = 0;
			break;
		}
        case XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_STREAMDATA):
		{
            if (*pToken & 0x10000000) // skip bytes
            {
                // XD3DVSD_SKIP or XD3DVSD_SKIPBYTES macro
				UINT count = (*pToken & XD3DVSD_SKIPCOUNTMASK) >> XD3DVSD_SKIPCOUNTSHIFT;
                if (!(*pToken & 0x08000000))
                {
					// skip dwords
                    count <<= 2;
				}
				Stride += count;
			}
			else	// data
			{
                DWORD sizeAndType = (*pToken & XD3DVSD_DATATYPEMASK) >> XD3DVSD_DATATYPESHIFT;
                DWORD reg = (*pToken & XD3DVSD_VERTEXREGMASK) >> XD3DVSD_VERTEXREGSHIFT;
				switch (sizeAndType)
				{
				case XD3DVSDT_FLOAT1      :    // 1D float expanded to (value, 0., 0., 1.)
					Stride += 4;
					break;
					
				case XD3DVSDT_FLOAT2      :    // 2D float expanded to (value, value, 0., 1.)
					Stride += 8;
					break;
//#if XBOX
				case XD3DVSDT_FLOAT2H     :    // 2D homogeneous float expanded to (value, value,0., value.)
					Stride += 12;
					break;
//#endif
					
				case XD3DVSDT_FLOAT3      :    // 3D float expanded to (value, value, value, 1.)
					Stride += 12;
					break;
					
				case XD3DVSDT_FLOAT4      :    // 4D float
					Stride += 16;
					break;
					
				case XD3DVSDT_D3DCOLOR    :    // 4D packed unsigned bytes mapped to 0. to 1. range
					Stride += 4;
					break;
					
//#if XBOX
				// Signed shorts map to the range [-32768, 32767]
				case XD3DVSDT_SHORT1      :    // 1D signed short expanded to (value, 0., 0., 1.)
					Stride += 2;
					break;

				case XD3DVSDT_SHORT2      :    // 2D signed short expanded to (value, value, 0., 1.)
					Stride += 4;
					break;

				case XD3DVSDT_SHORT3      :    // 3D signed short expanded to (value, value, value, 1.)
					Stride += 6;
					break;

				case XD3DVSDT_SHORT4      :    // 4D signed short
					Stride += 8;
					break;

				// (signed, normalized short maps from -1.0 to 1.0)
				case XD3DVSDT_NORMSHORT1  :    // 1D signed, normalized short expanded to (value, 0, 0., 1.)
					Stride += 2;
					break;
				
				case XD3DVSDT_NORMSHORT2  :    // 2D signed, normalized short expanded to (value, value, 0., 1.)
					Stride += 4;
					break;

				case XD3DVSDT_NORMSHORT3  :    // 3D signed, normalized short expanded to (value, value, value, 1.)  
					Stride += 6;
					break;

				case XD3DVSDT_NORMSHORT4  :    // 4D signed, normalized short expanded to (value, value, value, value)  
					Stride += 8;
					break;

				case XD3DVSDT_NORMPACKED3 :    // 3 signed, normalized components packed in 32-bits.  (11,11,10).  
					Stride += 4;
					break;
				
				// Packed bytes map to the range [0, 1]
				case XD3DVSDT_PBYTE1      :    // 1D packed byte expanded to (value, 0., 0., 1.) 
					Stride += 1;
					break;
				
				case XD3DVSDT_PBYTE2      :    // 2D packed byte expanded to (value, value, 0., 1.)
					Stride += 2;
					break;

				case XD3DVSDT_PBYTE3      :    // 3D packed byte expanded to (value, value, value, 1.)
					Stride += 3;
					break;

				case XD3DVSDT_PBYTE4      :    // 4D packed byte expanded to (value, value, value, value) 
					Stride += 4;
					break;

//				case XD3DVSDT_NONE        :    // No stream data
//					break;
//#endif

				default:
					m_pBundler->ErrorMsg("SetStridesFromDeclaration: Bad vertex stream data type encoding (no D3DVSDT_* corresponds to 0x%x).", sizeAndType);
					hr = E_FAIL;
					goto e_Exit;
				}
			}
			break;
		}
			
        case XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_TESSELLATOR):
			m_pBundler->ErrorMsg("SetStridesFromDeclaration: Tessellator stream parsing not implemented.\n");
			hr = E_NOTIMPL;
			goto e_Exit;
			
        case XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_CONSTMEM):
		{
			int addr = ((*pToken & XD3DVSD_CONSTADDRESSMASK) >> XD3DVSD_CONSTADDRESSSHIFT) - 96;
            UINT count = (*pToken & XD3DVSD_CONSTCOUNTMASK) >> XD3DVSD_CONSTCOUNTSHIFT;
			pToken += 4 * count;
			break;
		}
			
        case XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_NOP):
            break;
			
        case XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_END):
            hr = S_OK;
			goto e_Exit;

		default:
			m_pBundler->ErrorMsg("SetStridesFromDeclaration: unknown token (0x%x) in the vertex shader declaration.\n", *pToken );
			hr = E_NOTIMPL;
			goto e_Exit;
		}

		// Go to the next token in the stream
		pToken++;
		if ((BYTE *)pToken - (BYTE *)m_rdwDeclaration > cbDeclaration)
		{
			m_pBundler->ErrorMsg("XDXVertexShaderDeclaration: missing end token.\n");
			hr = E_FAIL;
			goto e_Exit;
		}
	}

	// Handle last stream
	if (iActiveStream != -1 && Stride != 0)
	{
		// Set Stride if currently 0, otherwise check against calculated stride.
		if (m_rStreamInput[iActiveStream].Stride == 0)
			m_rStreamInput[iActiveStream].Stride = Stride;
		else
		{
			if (Stride != m_rStreamInput[iActiveStream].Stride)
			{
				m_pBundler->ErrorMsg("SetStridesFromDeclaration: stream %d given stride (%d) does not match computed stride (%d).\n",
									 iActiveStream, m_rStreamInput[iActiveStream].Stride, Stride);
				// Leave as warning
			}
		}
	}
			
 e_Exit:
	return hr;
}

#if 0
//-----------------------------------------------------------------------------
// Name: SetDeclaration
// Desc: Parse declaration to set streams and constants
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SetDeclaration(const DWORD *pDeclaration)
{
	// Look for end of declaration
	CONST DWORD *pDeclarationEnd = pDeclaration;
	while (*pDeclarationEnd != 0xffffffff)
	{
		pDeclarationEnd++;
		if (pDeclarationEnd - pDeclaration > VERTEXSHADER_MAX_DECLARATION_COUNT)
			return E_FAIL;
	}
	DWORD dwDeclarationCount = pDeclarationEnd - pDeclaration + 1;
	if (m_dwDeclarationCount < dwDeclarationCount)
	{
		m_dwDeclarationCount = dwDeclarationCount;
		if (m_rdwDeclaration)
			delete [] m_rdwDeclaration;
		m_rdwDeclaration = new DWORD [ m_dwDeclarationCount ];
	}
	memcpy(m_rdwDeclaration, pDeclaration, m_dwDeclarationCount);
	return S_OK;
}
#endif
	
//-----------------------------------------------------------------------------
// Name: SetFunction
// Desc: Set vertex shader microcode
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SetFunction(CONST void *pFunction, UINT cbFunctionSize)
{
	if (m_pFunction)
		delete m_pFunction;
	m_cbFunctionSize = cbFunctionSize;
	m_pFunction = new BYTE [ m_cbFunctionSize ];
	if (m_pFunction == NULL)
		return E_OUTOFMEMORY;
	memcpy(m_pFunction, pFunction, cbFunctionSize);
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SetVertexShaderReference
// Desc: Set vertex shader resource index
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SetVertexShaderReference(DWORD dwVertexShaderResourceIndex)
{
	m_dwVertexShaderReference = dwVertexShaderResourceIndex;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetStreamVertexBufferRefrence
// Desc: Set vertex buffer resource index to associate with a given stream
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SetStreamVertexBufferReference(UINT iStream, DWORD dwVertexBufferResourceIndex, DWORD Stride, DWORD Offset)
{
	if (iStream >= XD3DVS_STREAMS_MAX)
		return E_FAIL;
	m_rStreamInput[iStream].VertexBufferReference = dwVertexBufferResourceIndex;
	m_rStreamInput[iStream].Stride = Stride;
	m_rStreamInput[iStream].Offset = Offset;
	return S_OK;
}
	
//-----------------------------------------------------------------------------
// Name: SaveToBundle()
// Desc: Handles saving the appropriate data to the packed resource file
//-----------------------------------------------------------------------------
HRESULT CVertexShader::SaveToBundle( DWORD * pcbHeader, DWORD * pcbData )
{
	HRESULT hr = AddConstantsToDeclaration();
	if (FAILED(hr))
		return hr;
	hr = SetStridesFromDeclaration();
	if (FAILED(hr))
		return hr;
	
    if (m_dwDeclarationCount == 0
		&& m_cbFunctionSize == 0
		&& m_dwVertexShaderReference == 0)
    {
        m_pBundler->ErrorMsg( "Error: No declaration, function, or reference vertex shader specified\n" );
        return E_FAIL;
    }

	// Compute size of declaration (including constants) + function
	DWORD cbDeclaration = (m_dwDeclarationCount + 1) * sizeof(DWORD);	// include terminator in size
	DWORD cbFunction = m_cbFunctionSize;
	struct UserData {
		DWORD dwType;
		DWORD dwSize;
	} VSUserData;
	VSUserData.dwType = XBRC_VERTEXSHADER;
	//  vertex shader reference + vertex buffer stream references + declaration + function
	VSUserData.dwSize = sizeof(VertexShader) + cbDeclaration + cbFunction;

	// Write header
	if (FAILED(m_pBundler->WriteHeader( &VSUserData, sizeof(UserData))))
		return E_FAIL;
	m_pBundler->m_cbHeader += sizeof(UserData);	// TODO: this should be done in WriteHeader

	// Write VertexShader structure
	VertexShader VSSave;
	VSSave.Handle = 0;
	VSSave.DeclarationByteCount = cbDeclaration;
	VSSave.Declaration = (DWORD *)sizeof(VertexShader);	// file offset
	VSSave.FunctionByteCount = cbFunction;
	VSSave.Function = (DWORD *)(sizeof(VertexShader) + cbDeclaration);	// file offset
	VSSave.VertexShaderReference = m_dwVertexShaderReference;
	memcpy(VSSave.rStreamInput, m_rStreamInput, sizeof(m_rStreamInput));
	if (FAILED( m_pBundler->WriteHeader( &VSSave, sizeof(VertexShader) ) ))
		return E_FAIL;
	m_pBundler->m_cbHeader += sizeof(VertexShader); // TODO: this should be done in WriteHeader

	// Write declaration
    if ( FAILED( m_pBundler->WriteHeader( m_rdwDeclaration, cbDeclaration ) ) )
        return E_FAIL;
	m_pBundler->m_cbHeader += cbDeclaration; // TODO: this should be done in WriteHeader

	// Write function 
	if ( FAILED( m_pBundler->WriteHeader( m_pFunction, cbFunction ) ) )
		return E_FAIL;
	m_pBundler->m_cbHeader += cbFunction; // TODO: this should be done in WriteHeader

	// Set return counts
    *pcbHeader = sizeof(UserData) + VSUserData.dwSize;
	*pcbData = 0;	// no video data
    return S_OK;
}

