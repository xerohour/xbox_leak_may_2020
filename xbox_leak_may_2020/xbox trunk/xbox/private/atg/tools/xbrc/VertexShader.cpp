//-----------------------------------------------------------------------------
//  
//  File: VertexShader.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Implementation of the XBContent VertexShaders
//-----------------------------------------------------------------------------
#include "stdafx.h"

HRESULT XBContent::BeginVertexShader(ISAXAttributes *pAttributes)
{
	HRESULT hr;
	PushContext(VertexShader_Context);
    CVertexShader *pVS = new CVertexShader(m_pBundler);
	if (!pVS) return E_OUTOFMEMORY;
	m_rContextStack[m_iContext].m_pData = pVS;

	// Set default resource id
	m_Count.m_VertexShader++;
	CHAR *strIdentifier = pVS->m_Identifier;
	ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
	_snprintf(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH, "VertexShader%d", m_Count.m_VertexShader);
	strIdentifier[BUNDLER_IDENTIFIER_MAX_LENGTH - 1] = 0;
	
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
			if (cchValue >= BUNDLER_IDENTIFIER_MAX_LENGTH)
				cchValue = BUNDLER_IDENTIFIER_MAX_LENGTH - 1;
			ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
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
			// TODO: make sure this is a vertex shader resource
			hr = pVS->SetVertexShaderReference(dwResourceIndex);
			if (FAILED(hr))
				return hr;
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
	
	// Defer the save of the vertex buffer to the bundler until we
	// process all the VertexShader elements.
	return S_OK;
}

HRESULT XBContent::EndVertexShader()
{
	CVertexShader *pVS = (CVertexShader *)m_rContextStack[m_iContext].m_pData;
	if (pVS == NULL) return E_INVALIDARG;

	// set resource name
	CHAR *strIdentifier = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier;
	ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
	strncpy(strIdentifier, pVS->m_Identifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
	strIdentifier[BUNDLER_IDENTIFIER_MAX_LENGTH - 1] = 0;	// make sure the string is null terminated
	
	// save current resource offset
    m_pBundler->m_aResources[m_pBundler->m_nResourceCount].dwOffset = m_pBundler->m_cbHeader;
	
    // Save the vertex shader to the bundle
    DWORD cbHeader = 0, cbData = 0;
	HRESULT hr = pVS->SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
	{
		delete pVS;
        return hr;
	}
	
    printf("VertexShader:  wrote %s (%d bytes)\n", 
           m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier,
		   cbHeader);

	// increment resource count
	m_pBundler->m_nResourceCount++;
	
	// cleanup
    delete pVS;
	PopContext();

	// If we're inside a pass context, save reference to vertex shader
	if (m_rContextStack[m_iContext].m_Context == Pass_Context)
	{
		Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
		Pass *pPass = &pEffect->rPass[pEffect->PassCount];
		
		// one-based index (since the resource count was already incremented)
		if (pPass->VertexShaderResource != 0)
			m_pBundler->ErrorMsg("<VertexShader> tag must appear just once within a <Pass> block\n");
		pPass->VertexShaderResource = m_pBundler->m_nResourceCount;
	}
	return S_OK;
}

HRESULT XBContent::BeginVertexShader_decl(ISAXAttributes *pAttributes)
{
	// Sanity check:
	if (m_rContextStack[m_iContext].m_Context != VertexShader_Context)
	{
		m_pBundler->ErrorMsg("<decl> tag must appear within an enclosing <VertexShader>\n");
		return E_FAIL;
	}
	PushContext(VertexShader_decl_Context);
	// use same pointer to vertex shader as enclosing VertexShader
	m_rContextStack[m_iContext].m_pData = m_rContextStack[m_iContext-1].m_pData;
	
	return S_OK;
}

HRESULT XBContent::EndVertexShader_decl()
{
	m_rContextStack[m_iContext].m_pData = NULL;
	PopContext();
	return S_OK;
}

HRESULT XBContent::BeginVertexShader_decl_stream(ISAXAttributes *pAttributes)
{
	HRESULT hr;
	// Sanity check:
	if (m_rContextStack[m_iContext].m_Context != VertexShader_decl_Context)
	{
		m_pBundler->ErrorMsg("<stream> tag must appear within an enclosing <decl>\n");
		return E_FAIL;
	}
	// use same pointer to vertex shader as enclosing decl
	CVertexShader *pVS = (CVertexShader *)m_rContextStack[m_iContext].m_pData;
	PushContext(VertexShader_decl_stream_Context);
	m_rContextStack[m_iContext].m_pData = pVS;

	// Loop through attributes
	UINT StreamIndex = -1;
	INT Stride = 0;
	DWORD dwVertexBufferResourceIndex = -1;
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
			StreamIndex = atoi(CharString(pwchValue, cchValue)); 
		}
		else if (MATCH(idref))
		{
			CHAR *strID = CharString(pwchValue, cchValue);
			if (!m_pBundler->IsExistingIdentifier(strID, &dwVertexBufferResourceIndex))
			{
				m_pBundler->ErrorMsg("Unknown vertex buffer resource \"%s\"\n", strID);
				return E_FAIL;
			}
			// TODO: make sure that it's really a vertex buffer
		}
		else if (MATCH(stride))
		{
			Stride = atoi(CharString(pwchValue, cchValue));
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	if (StreamIndex == -1)
	{
		m_pBundler->ErrorMsg("<stream> tag requires an index attribute\n");
		return E_FAIL;
	}
	else
	{
		hr = pVS->SetStream(StreamIndex);
		if (FAILED(hr))
		{
			m_pBundler->ErrorMsg("Problem with <stream> attribute index=%d\n", StreamIndex);
			return hr;
		}
		if (dwVertexBufferResourceIndex != -1)
		{
			hr = pVS->SetStreamVertexBufferReference(StreamIndex, dwVertexBufferResourceIndex, Stride, 0);
			if (FAILED(hr))
			{
				m_pBundler->ErrorMsg("Problem with <stream> vertex buffer attribute idref=%d stride=%d\n", dwVertexBufferResourceIndex, Stride);
				return hr;
			}
		}
	}
	return S_OK;
}

HRESULT XBContent::EndVertexShader_decl_stream()
{
	m_rContextStack[m_iContext].m_pData = NULL;
	PopContext();
	return S_OK;
}

HRESULT XBContent::BeginVertexShader_decl_stream_vreg(ISAXAttributes *pAttributes)
{
	// Sanity check:
	if (m_rContextStack[m_iContext].m_Context != VertexShader_decl_stream_Context)
	{
		m_pBundler->ErrorMsg("<vreg> tag must appear within an enclosing <stream>\n");
		return E_FAIL;
	}
	// use same pointer to vertex shader as enclosing stream
	CVertexShader *pVS = (CVertexShader *)m_rContextStack[m_iContext].m_pData;
	PushContext(VertexShader_decl_stream_vreg_Context);
	m_rContextStack[m_iContext].m_pData = pVS;

	// Loop through attributes
	UINT iVertexRegister = -1;
	DWORD VertexFormat = -1;
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
			iVertexRegister = atoi(CharString(pwchValue, cchValue));
		}
		else if (MATCH(name))
		{
			// TODO: save name and validate when setting vertex buffer as source for this vertex shader
		}
		else if (MATCH(format))
		{
			CHAR *strFull = CharString(pwchValue, cchValue);
			CHAR *strSeparators = " \t\n";
			CHAR *strBuff = strtok(strFull, strSeparators);
			VertexFormat = CVertexBuffer::VertexFormatFromString( strBuff );
			if (VertexFormat == -1)
			{
				m_pBundler->ErrorMsg("Vertex format <vreg format=\"%s\"> is missing or invalid.\n", strBuff);
				return E_FAIL;
			}
			strBuff = strtok(NULL, strSeparators);
			if (strBuff != NULL)
			{
				m_pBundler->ErrorMsg( "Extra characters in format attribute \"%s\"\n", strBuff );
				return E_FAIL;
			}
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	if (iVertexRegister == -1)
	{
		m_pBundler->ErrorMsg("Vertex register index is missing or invalid.\n");
		return E_FAIL;
	}
	if (VertexFormat == -1)
	{
		m_pBundler->ErrorMsg("Vertex register format is missing or invalid.\n");
		return E_FAIL;
	}
	HRESULT hr = pVS->SetVertexRegisterFormat(iVertexRegister, VertexFormat);
	if (FAILED(hr))
	{
		m_pBundler->ErrorMsg("Unable to set vertex register format (index=%d format=0x%08x)\n",
			iVertexRegister, VertexFormat);
		return hr;
	}
	return S_OK;
}

HRESULT XBContent::EndVertexShader_decl_stream_vreg()
{
	m_rContextStack[m_iContext].m_pData = NULL;
	PopContext();
	return S_OK;
}

#define VERTEXSHADER_ASM_BUFFER_SIZE 10000
struct VertexShader_asm {
	CVertexShader *m_pVS;
	INT m_cchCount;	// number of chars added to buffer
	CHAR m_buf[VERTEXSHADER_ASM_BUFFER_SIZE];

	VertexShader_asm(CVertexShader *pVS)
	{
		m_pVS = pVS;
		m_cchCount = 0;
		m_buf[0] = 0;
	}
};

HRESULT XBContent::BeginVertexShader_asm(ISAXAttributes *pAttributes)
{
	// Sanity check:
	if (m_rContextStack[m_iContext].m_Context != VertexShader_Context)
	{
		m_pBundler->ErrorMsg("<asm> tag must appear within an enclosing <VertexShader>\n");
		return E_FAIL;
	}

	// Make a new asm struct and push the context
	CVertexShader *pVS = (CVertexShader *)m_rContextStack[m_iContext].m_pData;
	VertexShader_asm *pVS_asm = new VertexShader_asm(pVS);
	if (pVS_asm == NULL)
		return E_OUTOFMEMORY;
	PushContext(VertexShader_asm_Context);
	m_rContextStack[m_iContext].m_pData = pVS_asm;

	/*
	// Loop through attributes
	UINT iVertexRegister = -1;
	DWORD VertexFormat = -1;
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
		if (MATCH(source))
		{
			// TODO: read file in 
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	*/
	return S_OK;
}

HRESULT XBContent::VertexShader_asm_Chars(WCHAR *pwchChars, int cchChars)
{
	VertexShader_asm *pVS_asm = (VertexShader_asm *)m_rContextStack[m_iContext].m_pData;
	int cchOut;
	CHAR *strFull = CharString(pwchChars, cchChars, &cchOut);
	if (cchOut + pVS_asm->m_cchCount >= VERTEXSHADER_ASM_BUFFER_SIZE)
		return E_FAIL;
	strcat(pVS_asm->m_buf, strFull);
	pVS_asm->m_cchCount += cchOut;
	return S_OK;
}

extern HRESULT IncludeResolver(
	LPVOID pResolverUserData,
	BOOL isSystemInclude,
	LPCSTR sourceFilePath,
	LPCSTR includeFileName,
	LPSTR resolvedFilePath,
	DWORD resolvedFilePathSize,
	LPXGBUFFER* ppResolvedFile);

HRESULT XBContent::EndVertexShader_asm()
{
	VertexShader_asm *pVS_asm = (VertexShader_asm *)m_rContextStack[m_iContext].m_pData;
	
	// Assemble the shader
	LPXGBUFFER pConstants = NULL;
	LPXGBUFFER pCompiledShader = NULL;
	LPXGBUFFER pErrorLog = NULL;
	LPXGBUFFER pListing = NULL;
	DWORD ShaderType;
	LPCSTR pSourceFileName = "<asm>";
	DWORD dwFlags = 0;
	HRESULT hr = XGAssembleShader(pSourceFileName, pVS_asm->m_buf, pVS_asm->m_cchCount, dwFlags,
								  &pConstants, &pCompiledShader, &pErrorLog, &pListing, IncludeResolver, this, &ShaderType);
	if (FAILED(hr))
	{
		m_pBundler->ErrorMsg("Vertex shader assembling failed.\n%.*s\n", pErrorLog->GetBufferSize(), pErrorLog->GetBufferPointer());
		goto e_Exit;
	}
	if (SASMT_SHADERTYPE(ShaderType) == SASMT_PIXELSHADER)
	{
		m_pBundler->ErrorMsg("Pixel shader specified in vertex shader context.\n");
	}
	if (pCompiledShader)
	{
		// Add function code to VertexShader
		hr = pVS_asm->m_pVS->SetFunction(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize());
		if (FAILED(hr))
			goto e_Exit;
	}
	if (pConstants)
	{
		// Add constants to vertex shader
		UINT iConstantCount = pConstants->GetBufferSize() / (5 * sizeof(DWORD));
		DWORD *pConstant = (DWORD *)pConstants->GetBufferPointer();
		for (UINT iConstant = 0; iConstant < iConstantCount; iConstant++)
		{
			UINT iConstantRegister = (*pConstant & XD3DVSD_CONSTADDRESSMASK) >> XD3DVSD_CONSTADDRESSSHIFT;
			UINT iCount = (*pConstant & XD3DVSD_CONSTCOUNTMASK) >> XD3DVSD_CONSTCOUNTSHIFT;
			if (iCount != 1)
			{
				m_pBundler->ErrorMsg("Shader constant count %d != 1 is not handled.\n", iCount);
				hr = E_FAIL;
				goto e_Exit;
			}
			pVS_asm->m_pVS->SetConstant(iConstantRegister - 96, pConstant + 1, iCount);
			pConstant += 5;
		}
	}

	// Cleanup
 e_Exit:	
	if (pConstants)
		pConstants->Release();
	if (pCompiledShader)
		pCompiledShader->Release();
	if (pErrorLog)
		pErrorLog->Release();
	if (pListing)
		pListing->Release();
	delete pVS_asm;
	PopContext();
	return hr;
}

#define VERTEXSHADER_CONSTANT_BUFFER_SIZE 2000
struct VertexShader_Constant {
	CVertexShader *m_pVS;
	INT m_iConstantRegister;
	UINT m_iConstantCount;
	INT m_cchCount;	// number of chars added to buffer
	CHAR m_buf[VERTEXSHADER_CONSTANT_BUFFER_SIZE];
	
	VertexShader_Constant(CVertexShader *pVS)
	{
		m_pVS = pVS;
		m_iConstantRegister = 0x7ffffff;
		m_iConstantCount = 1;
		m_cchCount = 0;
		m_buf[0] = 0;
	}
};

HRESULT XBContent::BeginVertexShader_Constant(ISAXAttributes *pAttributes)
{
	// Sanity check:
	if (m_rContextStack[m_iContext].m_Context != VertexShader_Context)
	{
		m_pBundler->ErrorMsg("<Constant> tag must appear within an enclosing <VertexShader>\n");
		return E_FAIL;
	}

	// Make a new Constant struct and push the context
	CVertexShader *pVS = (CVertexShader *)m_rContextStack[m_iContext].m_pData;
	VertexShader_Constant *pVS_Constant = new VertexShader_Constant(pVS);
	if (pVS_Constant == NULL)
		return E_OUTOFMEMORY;
	PushContext(VertexShader_Constant_Context);
	m_rContextStack[m_iContext].m_pData = pVS_Constant;

	// Loop through attributes
	CHAR *strParameter = NULL;
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
			pVS_Constant->m_iConstantRegister = atoi(CharString(pwchValue, cchValue));
		}
		else if (MATCH(name))
		{
			// TODO: use name instead of index to set vertex shader. This will
			// be handy when splicing and reorganizing
		}
		else if (MATCH(count))
		{
			pVS_Constant->m_iConstantCount = atoi(CharString(pwchValue, cchValue));
		}
		else if (MATCH(parameter))
		{
			// Save parameter string for process after the index and
			// count attributes are set.
			CHAR *str = CharString(pwchValue, cchValue);
			strParameter = new CHAR [ strlen(str) + 1 ];
			if (strParameter == NULL)
				return E_OUTOFMEMORY;
			strcpy(strParameter, str);
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}

	if (strParameter)
	{
		// Add this constant to the list of things that need to be updated
		// during the SetTime phase.
		AnimVertexShaderParameter *pAnimVSP = new AnimVertexShaderParameter;
		if (pAnimVSP == NULL)
			return E_OUTOFMEMORY;
		pAnimVSP->m_strVertexShaderReference = new CHAR [ strlen(pVS->m_Identifier) + 1 ];
		if (pAnimVSP->m_strVertexShaderReference == NULL)
			return E_OUTOFMEMORY;
		strcpy(pAnimVSP->m_strVertexShaderReference, pVS->m_Identifier);
		pAnimVSP->m_Index = pVS_Constant->m_iConstantRegister;
		pAnimVSP->m_Count = pVS_Constant->m_iConstantCount;
		pAnimVSP->m_strExpression = strParameter;	// allocated above
		strParameter = NULL;
		pAnimVSP->m_pSource = NULL;

		// Add to head of animated vertex shader parameter list
		pAnimVSP->m_pNext = m_pAnimVSPList;
		m_pAnimVSPList = pAnimVSP;

		// TODO: What about effects whose parameters change during
		// the skeleton traversal?  Perhaps the SetTime should be right
		// before rendering with this vertex shader. In other words,
		// the parameter evaluation happens at the same time as the
		// SetVertexShader.  This would be pretty easy to compile in
		// as push buffer fixups.
	}
	return S_OK;
}

HRESULT XBContent::VertexShader_Constant_Chars(WCHAR *pwchChars, int cchChars)
{
	VertexShader_Constant *pVS_Constant = (VertexShader_Constant *)m_rContextStack[m_iContext].m_pData;
	int cchOut;
	CHAR *strFull = CharString(pwchChars, cchChars, &cchOut);
	if (cchOut + pVS_Constant->m_cchCount >= VERTEXSHADER_CONSTANT_BUFFER_SIZE)
		return E_FAIL;
	strcat(pVS_Constant->m_buf, strFull);
	pVS_Constant->m_cchCount += cchOut;
	return S_OK;
}

HRESULT XBContent::EndVertexShader_Constant()
{
	HRESULT hr = S_OK;
	VertexShader_Constant *pVS_Constant = (VertexShader_Constant *)m_rContextStack[m_iContext].m_pData;
	
	// Add constants to vertex shader
	INT iConstantRegister = pVS_Constant->m_iConstantRegister;
	UINT iConstantCount = 0;
	FLOAT rConstantValue[4];
	UINT iConstantValueIndex = 0;
	CHAR *strSeparators = " \t\n";
	CHAR *strBuff = strtok(pVS_Constant->m_buf, strSeparators);
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
				if (iConstantCount >= pVS_Constant->m_iConstantCount)
				{
					m_pBundler->ErrorMsg("Warning: too many constant values (%d > %d) between <Constant> ... </Constant>.\n",
										 iConstantCount + 1, pVS_Constant->m_iConstantCount);
					goto e_Exit;
				}
				hr = pVS_Constant->m_pVS->SetConstant(iConstantRegister + iConstantCount, rConstantValue, 1);
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
	if (iConstantCount != pVS_Constant->m_iConstantCount)
	{
		m_pBundler->ErrorMsg("Warning: missing constant values (found %d out of %d) <Constant>\n",
							 iConstantCount, pVS_Constant->m_iConstantCount);
	}
 e_Exit:

	// Cleanup
	delete pVS_Constant;
	PopContext();
	return hr;
}
