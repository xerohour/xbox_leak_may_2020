//-----------------------------------------------------------------------------
//  
//  File: PixelShader.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Implementation of the XBContent PixelShaders
//-----------------------------------------------------------------------------
#include "stdafx.h"


// Uncomment this if you want to break into the debugger when an error occurs
// #define BREAK_ON_ERROR

#if defined(DBG) && defined(BREAK_ON_ERROR)
#define SETERROR(HR,CODE) \
    do { \
if(FAILED(HR = CODE)){ \
            DebugBreak(); \
        } \
    } while(0)
#else
#define SETERROR(HR,CODE) \
    HR = CODE
#endif

HRESULT CreateBufferFromFile(const char* path, LPXGBUFFER* ppBuffer){
    HRESULT hr = S_OK;
    if(!path || !ppBuffer){
        SETERROR(hr,E_FAIL);
    }
    HANDLE file = INVALID_HANDLE_VALUE;
    if(SUCCEEDED(hr)){
        file = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
            NULL);
        if(file == INVALID_HANDLE_VALUE){
            SETERROR(hr,E_FAIL);
        }
    }
    DWORD size = 0;
    if(SUCCEEDED(hr)){
        size = GetFileSize(file,NULL);
        if(size == ~0){
            SETERROR(hr,E_FAIL);
        }
    }
    LPXGBUFFER pBuffer = NULL;
    if(SUCCEEDED(hr)){
        hr = XGBufferCreate(size, &pBuffer);
    }
    if(SUCCEEDED(hr)){
        DWORD sizeRead;
        BOOL result = ReadFile(file, pBuffer->GetBufferPointer(), size, &sizeRead, NULL);
        if(! result || sizeRead != size){
            SETERROR(hr,E_FAIL);
        }
    }
    if(file != INVALID_HANDLE_VALUE){
        CloseHandle(file);
    }
    if(SUCCEEDED(hr)){
        *ppBuffer = pBuffer;
    }
    else {
        if(pBuffer) pBuffer->Release();
    }
    return hr;
}

//////////////////////////////////////////////////////////////////////
// The resolver is responsible for opening #include'd files
//
HRESULT IncludeResolver(
	LPVOID pResolverUserData,
	BOOL isSystemInclude,
	LPCSTR sourceFilePath,
	LPCSTR includeFileName,
	LPSTR resolvedFilePath,
	DWORD resolvedFilePathSize,
	LPXGBUFFER* ppResolvedFile)
{
	// TODO: distinguish between system include and local, look in more places for the file
	// try opening the file from the current directory
	HRESULT hr = CreateBufferFromFile(includeFileName, ppResolvedFile);
	if (FAILED(hr))
		return hr;
	strncpy(resolvedFilePath, includeFileName, resolvedFilePathSize);
	return S_OK;
}



#define PIXELSHADER_IDENTIFIER_SIZE BUNDLER_IDENTIFIER_MAX_LENGTH

struct PixelShader {
	CHAR Identifier[PIXELSHADER_IDENTIFIER_SIZE];
	D3DPIXELSHADERDEF def;

	PixelShader()
	{
		ZeroMemory(Identifier, PIXELSHADER_IDENTIFIER_SIZE);
		ZeroMemory(&def, sizeof(def));
	}

	// Helper function to assign values inside the def structure.  Uses the d3d mappings.
	HRESULT SetConstant(UINT iConstantRegister, CONST void *pConstantData, DWORD ConstantCount)
	{
		float *pFloatData = (float *)pConstantData;
		for (UINT i=0; i<ConstantCount; i++, iConstantRegister++, pFloatData += 4) 
		{
			DWORD dwValue = D3DXCOLOR(pFloatData[0], pFloatData[1], pFloatData[2], pFloatData[3]);

			if (iConstantRegister >= D3DPS_CONSTREG_MAX_DX8)
				return E_FAIL;

			// The pixelshaderdef struct has three dwords that map D3D constants.
			// Scan the mapping to determine which registers to write with this D3D constant.
			for(UINT r=0; r<8; r++) // check c0 registers
				if(((def.PSC0Mapping >> (4*r)) & 0xf) == iConstantRegister)
					def.PSConstant0[r] = dwValue;

			for(r=0; r<8; r++) // check c1 registers
				if((def.PSC1Mapping >> (4*r) & 0xf) == iConstantRegister)
					def.PSConstant1[r] = dwValue;
    
			for(r=0; r<2; r++) // check final combiner registers
				if((def.PSFinalCombinerConstants >> (4*r) & 0xf) == iConstantRegister)
					(&def.PSFinalCombinerConstant0)[r] = dwValue;	// PSFinalCombinerConstant0 or  PSFinalCombinerConstant1
		}
		return S_OK;
	}
};

HRESULT XBContent::BeginPixelShader(ISAXAttributes *pAttributes)
{
	PushContext(PixelShader_Context);
    PixelShader *pPS = new PixelShader;
	if (!pPS) return E_OUTOFMEMORY;
	m_rContextStack[m_iContext].m_pData = pPS;
	ZeroMemory(pPS, sizeof(PixelShader));

	// Set default resource id
	m_Count.m_PixelShader++;
	CHAR *strIdentifier = pPS->Identifier;
	ZeroMemory(strIdentifier, PIXELSHADER_IDENTIFIER_SIZE);
	_snprintf(strIdentifier, PIXELSHADER_IDENTIFIER_SIZE, "PixelShader%d", m_Count.m_PixelShader);
	strIdentifier[PIXELSHADER_IDENTIFIER_SIZE - 1] = 0;

	// Loop through attributes
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(id))
		{
			if (cchValue >= PIXELSHADER_IDENTIFIER_SIZE)
				cchValue = PIXELSHADER_IDENTIFIER_SIZE - 1;
			ZeroMemory(strIdentifier, PIXELSHADER_IDENTIFIER_SIZE);
			strncpy(strIdentifier, CharString(pwchValue, cchValue), cchValue );
			strIdentifier[cchValue] = 0;	// make sure the string is NULL terminated
		}
		else if (MATCH(idref))
		{
			CHAR *strID = CharString(pwchValue, cchValue);
			DWORD dwResourceIndex;
			if (!m_pBundler->IsExistingIdentifier(strID, &dwResourceIndex))
			{
				m_pBundler->ErrorMsg("Unknown resource \"%s\"\n", strID);
				return E_FAIL;
			}

			// Copy reference pixel shader description over current description
			DWORD Offset = m_pBundler->m_aResources[dwResourceIndex-1].dwOffset + 2 * sizeof(DWORD);	// skip user data header
			D3DPIXELSHADERDEF *pdef = (D3DPIXELSHADERDEF *)(m_pBundler->m_pbHeaderBuff + Offset);
			pPS->def = *pdef;
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}

	// Check if identifier is unique
	if ( m_pBundler->IsExistingIdentifier( strIdentifier ) )
	{
        m_pBundler->ErrorMsg( "Second use of identifier \"%s\"", strIdentifier );
		return E_FAIL;
	}
	
	// Defer the save of the pixel buffer to the bundler until we
	// process all the PixelShader elements.
	return S_OK;
}

HRESULT XBContent::EndPixelShader()
{
	PixelShader *pPS = (PixelShader *)m_rContextStack[m_iContext].m_pData;
	if (pPS == NULL) return E_INVALIDARG;

	// Keep track of offset into header as a bundler resource
	CHAR *strIdentifier = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier;
	ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
	strncpy(strIdentifier, pPS->Identifier, BUNDLER_IDENTIFIER_MAX_LENGTH - 1);
	strIdentifier[BUNDLER_IDENTIFIER_MAX_LENGTH] = 0;	// make sure the string is null terminated
    DWORD cbHeader0 = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].dwOffset = m_pBundler->m_cbHeader;
	m_pBundler->m_nResourceCount++;
	
	// Helper writing macro
#define WRITE(POINTER, SIZE)								\
	if (FAILED(m_pBundler->WriteHeader( POINTER, SIZE )))	\
		return E_FAIL;										\
	m_pBundler->m_cbHeader += SIZE;	// TODO: this should be done in WriteHeader
	
	// write header to bundle
	struct _UserData {
		DWORD dwType;				// XBRC_PIXELSHADER
		DWORD dwSize;				// total size (not including this header)
	} UserData;
	UserData.dwType = XBRC_PIXELSHADER;
	UserData.dwSize = sizeof(D3DPIXELSHADERDEF);
	
	WRITE( &UserData, sizeof(UserData) );
	WRITE( &pPS->def, sizeof(D3DPIXELSHADERDEF) );
	
#undef WRITE
	
    printf("PixelShader:  wrote %s (%d bytes)\n", strIdentifier, m_pBundler->m_cbHeader - cbHeader0);

	// cleanup
    delete pPS;
	PopContext();

	// If we're inside a pass context, save reference to pixel shader
	if (m_rContextStack[m_iContext].m_Context == Pass_Context)
	{
		Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
		Pass *pPass = &pEffect->rPass[pEffect->PassCount];
		
		// one-based index (since the resource count was already incremented)
		if (pPass->PixelShaderResource != 0)
			m_pBundler->ErrorMsg("<PixelShader> tag must appear just once within a <Pass> block\n");
		pPass->PixelShaderResource = m_pBundler->m_nResourceCount;
	}
	return S_OK;
}

#define PIXELSHADER_ASM_BUFFER_SIZE 10000
struct PixelShader_asm {
	PixelShader *m_pPS;
	INT m_cchCount;	// number of chars added to buffer
	CHAR m_buf[PIXELSHADER_ASM_BUFFER_SIZE];

	PixelShader_asm(PixelShader *pPS)
	{
		m_pPS = pPS;
		m_cchCount = 0;
		m_buf[0] = 0;
	}
};

HRESULT XBContent::BeginPixelShader_asm(ISAXAttributes *pAttributes)
{
	// Sanity check:
	if (m_rContextStack[m_iContext].m_Context != PixelShader_Context)
	{
		m_pBundler->ErrorMsg("<asm> tag must appear within an enclosing <PixelShader>\n");
		return E_FAIL;
	}

	// Make a new asm struct and push the context
	PixelShader *pPS = (PixelShader *)m_rContextStack[m_iContext].m_pData;
	PixelShader_asm *pPS_asm = new PixelShader_asm(pPS);
	if (pPS_asm == NULL)
		return E_OUTOFMEMORY;
	PushContext(PixelShader_asm_Context);
	m_rContextStack[m_iContext].m_pData = pPS_asm;

	return S_OK;
}

HRESULT XBContent::PixelShader_asm_Chars(WCHAR *pwchChars, int cchChars)
{
	PixelShader_asm *pPS_asm = (PixelShader_asm *)m_rContextStack[m_iContext].m_pData;
	int cchOut;
	CHAR *strFull = CharString(pwchChars, cchChars, &cchOut);
	if (cchOut + pPS_asm->m_cchCount >= PIXELSHADER_ASM_BUFFER_SIZE)
		return E_FAIL;
	strcat(pPS_asm->m_buf, strFull);
	pPS_asm->m_cchCount += cchOut;
	return S_OK;
}

HRESULT XBContent::EndPixelShader_asm()
{
	PixelShader_asm *pPS_asm = (PixelShader_asm *)m_rContextStack[m_iContext].m_pData;
	
	// Assemble the shader
	LPXGBUFFER pCompiledShader = NULL;
	LPXGBUFFER pErrorLog = NULL;
	LPXGBUFFER pListing = NULL;
	DWORD ShaderType;
	LPCSTR pSourceFileName = "<asm>";
	DWORD dwFlags = 0;
	HRESULT hr = XGAssembleShader(pSourceFileName, pPS_asm->m_buf, pPS_asm->m_cchCount, dwFlags,
								  NULL, &pCompiledShader, &pErrorLog, &pListing, IncludeResolver, this, &ShaderType);
	if (FAILED(hr))
	{
		m_pBundler->ErrorMsg("Pixel shader assembling failed.\n%.*s\n", pErrorLog->GetBufferSize(), pErrorLog->GetBufferPointer());
		goto e_Exit;
	}
	if (SASMT_SHADERTYPE(ShaderType) != SASMT_PIXELSHADER)
	{
		m_pBundler->ErrorMsg("Non-pixel shader specified in pixel shader context.\n");
	}
	if (pCompiledShader)
	{
		// Copy function code to PixelShader
		D3DPIXELSHADERDEF *pdef = (D3DPIXELSHADERDEF *)(pCompiledShader->GetBufferPointer());
		ASSERT(pCompiledShader->GetBufferSize() == sizeof(D3DPIXELSHADERDEF));
		pPS_asm->m_pPS->def = *pdef;
	}

	// Cleanup
 e_Exit:	
	if (pCompiledShader)
		pCompiledShader->Release();
	if (pErrorLog)
		pErrorLog->Release();
	if (pListing)
		pListing->Release();
	delete pPS_asm;
	PopContext();
	return hr;
}


#define PIXELSHADER_CONSTANT_BUFFER_SIZE 2000
struct PixelShader_Constant {
	PixelShader *m_pPS;
	UINT m_iConstantRegister;
	UINT m_iConstantCount;
	INT m_cchCount;	// number of chars added to buffer
	CHAR m_buf[PIXELSHADER_CONSTANT_BUFFER_SIZE];
	
	PixelShader_Constant(PixelShader *pPS)
	{
		m_pPS = pPS;
		m_iConstantRegister = 0x7ffffff;
		m_iConstantCount = 1;
		m_cchCount = 0;
		m_buf[0] = 0;
	}
};

HRESULT XBContent::BeginPixelShader_Constant(ISAXAttributes *pAttributes)
{
	// Sanity check:
	if (m_rContextStack[m_iContext].m_Context != PixelShader_Context)
	{
		m_pBundler->ErrorMsg("<Constant> tag must appear within an enclosing <PixelShader>\n");
		return E_FAIL;
	}

	// Make a new Constant struct and push the context
	PixelShader *pPS = (PixelShader *)m_rContextStack[m_iContext].m_pData;
	PixelShader_Constant *pPS_Constant = new PixelShader_Constant(pPS);
	if (pPS_Constant == NULL)
		return E_OUTOFMEMORY;
	PushContext(PixelShader_Constant_Context);
	m_rContextStack[m_iContext].m_pData = pPS_Constant;

	// Loop through attributes
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(index))
		{
			pPS_Constant->m_iConstantRegister = atoi(CharString(pwchValue, cchValue));
		}
		else if (MATCH(count))
		{
			pPS_Constant->m_iConstantCount = atoi(CharString(pwchValue, cchValue));
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	return S_OK;
}

HRESULT XBContent::PixelShader_Constant_Chars(WCHAR *pwchChars, int cchChars)
{
	PixelShader_Constant *pPS_Constant = (PixelShader_Constant *)m_rContextStack[m_iContext].m_pData;
	int cchOut;
	CHAR *strFull = CharString(pwchChars, cchChars, &cchOut);
	if (cchOut + pPS_Constant->m_cchCount >= PIXELSHADER_CONSTANT_BUFFER_SIZE)
		return E_FAIL;
	strcat(pPS_Constant->m_buf, strFull);
	pPS_Constant->m_cchCount += cchOut;
	return S_OK;
}

HRESULT XBContent::EndPixelShader_Constant()
{
	HRESULT hr = S_OK;
	PixelShader_Constant *pPS_Constant = (PixelShader_Constant *)m_rContextStack[m_iContext].m_pData;
	
	// Add constants to Pixel shader
	UINT iConstantRegister = pPS_Constant->m_iConstantRegister;
	UINT iConstantCount = 0;
	FLOAT rConstantValue[4];
	UINT iConstantValueIndex = 0;
	CHAR *strSeparators = " \t\n";
	CHAR *strBuff = strtok(pPS_Constant->m_buf, strSeparators);
	CHAR *pEnd;
	while (strBuff != NULL)
	{
		if (m_pBundler->ValidateType(strBuff, TT_FLOAT) == S_OK)
		{
			FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
			if (*pEnd != '\0')
			{
				m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
				// leave as just a warning
			}
			rConstantValue[iConstantValueIndex++] = fValue;
			if (iConstantValueIndex == 4)
			{
				// Every 4 values we add a new constant
				iConstantValueIndex = 0;
				if (iConstantCount >= pPS_Constant->m_iConstantCount)
				{
					m_pBundler->ErrorMsg("Warning: too many constant values (%d > %d) between <Constant> ... </Constant>.\n",
										 iConstantCount + 1, pPS_Constant->m_iConstantCount);
					goto e_Exit;
				}
				hr = pPS_Constant->m_pPS->SetConstant(iConstantRegister + iConstantCount, rConstantValue, 1);
				if (FAILED(hr))
				{
					m_pBundler->ErrorMsg("Could not set constant register %d.\n", iConstantRegister + iConstantCount);
					goto e_Exit;
				}
				iConstantCount++;
			}
		}
		else
		{
			m_pBundler->ErrorMsg( "Unrecognized constant value \"%s\"\n", strBuff );
			hr = E_FAIL;
			goto e_Exit;
		}

		// Get next token
		strBuff = strtok(NULL, strSeparators);
	}
	if (iConstantValueIndex != 0)
	{
		m_pBundler->ErrorMsg("Warning: extra data skipped in <Constant>\n");
	}
	if (iConstantCount != pPS_Constant->m_iConstantCount)
	{
		m_pBundler->ErrorMsg("Warning: missing constant values (found %d out of %d) <Constant>\n",
							 iConstantCount, pPS_Constant->m_iConstantCount);
	}
 e_Exit:

	// Cleanup
	delete pPS_Constant;
	PopContext();
	return hr;
}
