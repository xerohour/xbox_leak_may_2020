//-----------------------------------------------------------------------------
//  
//  File: effect.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Implementation of the XBContent class that concerns effects:
//     Effect, Pass, RenderState, TextureState, and Draw tags
//-----------------------------------------------------------------------------
#include "stdafx.h"

#define PASS_COUNT_INCREMENT 100
#define MAX_RENDER_STATE D3DRS_MAX
#define MAX_TEXTURE_STATE D3DTSS_MAX
#define MAX_TEXTURE_STAGE 4
#define MAX_DRAW 40

HRESULT XBContent::BeginEffect(ISAXAttributes *pAttributes)
{
	Effect *pEffect = new Effect;
	if (pEffect == NULL)
		return E_OUTOFMEMORY;
	pEffect->RenderTargetResource = 0;
	pEffect->PassCount = 0;
	pEffect->rPass = new Pass [PASS_COUNT_INCREMENT];
	if (pEffect->rPass == NULL)
	{
		delete pEffect;
		return E_OUTOFMEMORY;
	}
	PushContext(Effect_Context);
	m_rContextStack[m_iContext].m_pData = pEffect;

	// Set default resource id
	m_Count.m_Effect++;
	CHAR *strIdentifier = pEffect->Identifier;
	ZeroMemory(strIdentifier, EFFECT_IDENTIFIER_SIZE);
	_snprintf(strIdentifier, EFFECT_IDENTIFIER_SIZE, "Effect%d", m_Count.m_Effect);
	strIdentifier[EFFECT_IDENTIFIER_SIZE - 1] = 0;

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
			ZeroMemory(strIdentifier, EFFECT_IDENTIFIER_SIZE);
			strncpy(strIdentifier, CharString(pwchValue, cchValue), cchValue );
			strIdentifier[cchValue] = 0;	// make sure the string is NULL terminated
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
	
	// Defer the save of the effect to the bundler until we process all the passes.
	return S_OK;
}

HRESULT XBContent::EndEffect()
{
	Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
	PopContext();

	// We should be compiling to a PushBuffer, but we don't currently
	// have access to the pushbuffer compilation code on the Win32
	// side. So, we make our own token stream that is compiled to a
	// pushbuffer on the Xbox side.

	// Keep track of offset into header as a bundler resource
	CHAR *strIdentifier = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier;
	ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
	strncpy(strIdentifier, pEffect->Identifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
	strIdentifier[BUNDLER_IDENTIFIER_MAX_LENGTH - 1] = 0;	// make sure the string is null terminated
    DWORD cbHeader0 = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].dwOffset = m_pBundler->m_cbHeader;
	m_pBundler->m_nResourceCount++;
	
	// Calculate size and offsets into variable-size arrays
	UINT iPass;
	const int MAX_OFFSET = 10000;
	int iOffset = 0;
	DWORD rOffset[MAX_OFFSET];
	int iOffsetTextureState = 0;
	DWORD rOffsetTextureState[MAX_OFFSET];
	DWORD dwSize = sizeof(Effect);
	for (iPass = 0; iPass < pEffect->PassCount; iPass++)
	{
		// Pass *pPass = &pEffect->rPass[iPass];
		dwSize += sizeof(Pass);
	}
	for (iPass = 0; iPass < pEffect->PassCount; iPass++)	// then the variable-length arrays
	{
		Pass *pPass = &pEffect->rPass[iPass];

		// render states
		if (iOffset >= MAX_OFFSET)
			return E_FAIL;
		rOffset[iOffset++] = dwSize;
		dwSize += sizeof(RenderState) * pPass->RenderStateCount;

		// texture stages
		if (iOffset >= MAX_OFFSET)
			return E_FAIL;
		rOffset[iOffset++] = dwSize;
		dwSize += sizeof(TextureStage) * pPass->TextureStageCount;
		for (UINT iTextureStage = 0; iTextureStage < pPass->TextureStageCount; iTextureStage++)
		{
			// texture state
			if (iOffsetTextureState >= MAX_OFFSET)
				return E_FAIL;
			rOffsetTextureState[iOffsetTextureState++] = dwSize;
			dwSize += sizeof(TextureState) * pPass->rTextureStage[iTextureStage].TextureStateCount;
		}
		
		// draw commands
		if (iOffset >= MAX_OFFSET)
			return E_FAIL;
		rOffset[iOffset++] = dwSize;
		dwSize += sizeof(Draw) * pPass->DrawCount;
	}

	// Helper writing macro
#define WRITE(POINTER, SIZE)								\
	if (FAILED(m_pBundler->WriteHeader( POINTER, SIZE )))	\
		return E_FAIL;										\
	m_pBundler->m_cbHeader += SIZE;	// TODO: this should be done in WriteHeader
	
	// write header to bundle
	struct UserData {
		DWORD dwType;				// XBRC_EFFECT						0x80000040
		DWORD dwSize;				// total size (not including this header)
	} EffectUserData;
	EffectUserData.dwType = XBRC_EFFECT;
	EffectUserData.dwSize = dwSize;
	WRITE( &EffectUserData, sizeof(UserData) );

	// Write body of effect
	Effect EffectSave = *pEffect;
	EffectSave.rPass = (Pass *)sizeof(Effect); // set file offset
	WRITE( &EffectSave, sizeof(Effect) );
	iOffset = 0;
	iOffsetTextureState = 0;
	for (iPass = 0; iPass < pEffect->PassCount; iPass++)
	{
		Pass PassSave = pEffect->rPass[iPass];
		// set file offsets
		PassSave.rRenderState = (RenderState *)rOffset[iOffset++];
		PassSave.rTextureStage = (TextureStage *)rOffset[iOffset++]; 
		PassSave.rDraw = (Draw *)rOffset[iOffset++];
		WRITE( &PassSave, sizeof(Pass) );
	}
	for (iPass = 0; iPass < pEffect->PassCount; iPass++)
	{
		Pass *pPass = &pEffect->rPass[iPass];
		WRITE(  pPass->rRenderState, sizeof(RenderState) * pPass->RenderStateCount);
		UINT iTextureStage;
		for (iTextureStage = 0; iTextureStage < pPass->TextureStageCount; iTextureStage++)
		{
			TextureStage TextureStageSave = pPass->rTextureStage[iTextureStage];
			TextureStageSave.rTextureState = (TextureState *)rOffsetTextureState[iOffsetTextureState++];	// set file offset
			WRITE( &TextureStageSave, sizeof(TextureStage) );
		}
		for (iTextureStage = 0; iTextureStage < pPass->TextureStageCount; iTextureStage++)
		{
			TextureStage *pTextureStage = &pPass->rTextureStage[iTextureStage];
			WRITE(  pTextureStage->rTextureState, sizeof(TextureState) * pTextureStage->TextureStateCount );
		}
		WRITE( pPass->rDraw, sizeof(Draw) * pPass->DrawCount );
	}
#undef WRITE
    printf("Effect: wrote %s (%d bytes)\n", strIdentifier, m_pBundler->m_cbHeader - cbHeader0);
	
	// free up memory
	for (iPass = 0; iPass < pEffect->PassCount; iPass++)
	{
		Pass *pPass = &pEffect->rPass[iPass];
		if (pPass->rRenderState)
			delete [] pPass->rRenderState;
		if (pPass->rTextureStage)
		{
			for (UINT iTextureStage = 0; iTextureStage < pPass->TextureStageCount; iTextureStage++)
			{
				TextureStage *pTextureStage = &pPass->rTextureStage[iTextureStage];
				if (pTextureStage->rTextureState)
					delete [] pTextureStage->rTextureState;
			}
			delete [] pPass->rTextureStage;
		}
		if (pPass->rDraw)
			delete [] pPass->rDraw;
	}
	delete pEffect;
	
	return S_OK;
}


HRESULT XBContent::BeginPass(ISAXAttributes *pAttributes)
{
	Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
	PushContext(Pass_Context);
	m_rContextStack[m_iContext].m_pData = pEffect;	// keep effect pointer
	UINT PassCountNew = pEffect->PassCount + 1;
	if (PassCountNew / PASS_COUNT_INCREMENT * PASS_COUNT_INCREMENT == PassCountNew)
	{
		Pass *rPassOld = pEffect->rPass;
		
		// Allocate new array
		pEffect->rPass = new Pass [ PassCountNew + PASS_COUNT_INCREMENT ];
		if (pEffect->rPass == NULL)
			return E_OUTOFMEMORY;

		// Copy passes, including pointers, to new pass array
		memcpy(pEffect->rPass, rPassOld, sizeof(Pass) * pEffect->PassCount); 
		delete [] rPassOld;	// No destructor, so arrays are kept in the new pEffect->rPass's
	}
	Pass *pPass = &pEffect->rPass[pEffect->PassCount];
	ZeroMemory(pPass, sizeof(Pass));
	
	// Loop through attributes
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ )
	{
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		// No attributes are defined for <Pass>
		return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	return S_OK;
}

HRESULT XBContent::EndPass()
{
	Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
	pEffect->PassCount++;
	PopContext();
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: PrimitiveTypeFromString
// Desc: helper function to choose primitive type
//-----------------------------------------------------------------------------
D3DPRIMITIVETYPE PrimitiveTypeFromString( CONST CHAR *str )
{
	CHAR *prefix = "D3DPT_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}
	if (_stricmp(str, "POINTLIST") == 0)
		return D3DPT_POINTLIST;
	else if (_stricmp(str, "LINELIST") == 0)
		return D3DPT_LINELIST;
	else if (_stricmp(str, "LINELOOP") == 0)
		return D3DPT_LINELOOP;
	else if (_stricmp(str, "LINESTRIP") == 0)
		return D3DPT_LINESTRIP;
	else if (_stricmp(str, "TRIANGLELIST") == 0
			 || _stricmp(str, "TRILIST") == 0)		// TODO: spec eval on this kind of looseness
		return D3DPT_TRIANGLELIST;
	else if (_stricmp(str, "TRIANGLESTRIP") == 0
			 || _stricmp(str, "TRISTRIP") == 0)		// TODO: spec eval on this kind of looseness
		return D3DPT_TRIANGLESTRIP;
	else if (_stricmp(str, "TRIANGLEFAN") == 0
			 || _stricmp(str, "TRIFAN") == 0)		// TODO: spec eval on this kind of looseness
		return D3DPT_TRIANGLEFAN;
	else if (_stricmp(str, "QUADLIST") == 0)
		return D3DPT_QUADLIST;
	else if (_stricmp(str, "QUADSTRIP") == 0)
		return D3DPT_QUADSTRIP;
	else if (_stricmp(str, "POLYGON") == 0)
		return D3DPT_POLYGON;
	else
		return (D3DPRIMITIVETYPE)-1;
}
	
	// make sure the leftover chars are whitespace
HRESULT JustWhiteSpaceLeft(const CHAR *end)
{
	while (end && *end)
	{
		if (!isspace(*end))
			return E_FAIL;
		end++;
	}
	return S_OK;
}

HRESULT DWORD_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *end;
	DWORD dwValue = strtoul( str, &end, 0 );
	*pdwValue = dwValue;
	return JustWhiteSpaceLeft(end);
}

HRESULT BYTE_FromString(const CHAR *str, DWORD *pdwValue)
{
	return DWORD_FromString(str, pdwValue);
}

HRESULT FLOAT_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *end;
	DOUBLE doubleValue = strtod( str, &end );
	FLOAT floatValue = (FLOAT)doubleValue;
	*pdwValue = *(DWORD *)&floatValue;
	return JustWhiteSpaceLeft(end);
}

HRESULT LONG_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *end;
	LONG lValue = strtol( str, &end, 0 );
	*pdwValue = *(DWORD *)&lValue;
	return JustWhiteSpaceLeft(end);
}

HRESULT TRUE_FromString(const CHAR *str, DWORD *pdwValue)
{
	if (_stricmp(str, "TRUE") == 0)
	{
		*pdwValue = TRUE;
		return S_OK;
	}
	else if (_stricmp(str, "FALSE") == 0)
	{
		*pdwValue = FALSE;
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT D3DBLEND_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DBLEND_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}
    if (_stricmp(str, "ZERO" ) == 0)
	{
		*pdwValue = D3DBLEND_ZERO;
		return S_OK;
	}
    if (_stricmp(str, "ONE" ) == 0)
	{
		*pdwValue = D3DBLEND_ONE;
		return S_OK;
	}
    if (_stricmp(str, "SRCCOLOR" ) == 0)
	{
		*pdwValue = D3DBLEND_SRCCOLOR;
		return S_OK;
	}
    if (_stricmp(str, "INVSRCCOLOR" ) == 0)
	{
		*pdwValue = D3DBLEND_INVSRCCOLOR;
		return S_OK;
	}
    if (_stricmp(str, "SRCALPHA" ) == 0)
	{
		*pdwValue = D3DBLEND_SRCALPHA;
		return S_OK;
	}
    if (_stricmp(str, "INVSRCALPHA" ) == 0)
	{
		*pdwValue = D3DBLEND_INVSRCALPHA;
		return S_OK;
	}
    if (_stricmp(str, "DESTALPHA" ) == 0)
	{
		*pdwValue = D3DBLEND_DESTALPHA;
		return S_OK;
	}
    if (_stricmp(str, "INVDESTALPHA" ) == 0)
	{
		*pdwValue = D3DBLEND_INVDESTALPHA;
		return S_OK;
	}
    if (_stricmp(str, "DESTCOLOR" ) == 0)
	{
		*pdwValue = D3DBLEND_DESTCOLOR;
		return S_OK;
	}
    if (_stricmp(str, "INVDESTCOLOR" ) == 0)
	{
		*pdwValue = D3DBLEND_INVDESTCOLOR;
		return S_OK;
	}
    if (_stricmp(str, "SRCALPHASAT" ) == 0)
	{
		*pdwValue = D3DBLEND_SRCALPHASAT;
		return S_OK;
	}
    if (_stricmp(str, "CONSTANTCOLOR" ) == 0)
	{
		*pdwValue = D3DBLEND_CONSTANTCOLOR;
		return S_OK;
	}
    if (_stricmp(str, "INVCONSTANTCOLOR" ) == 0)
	{
		*pdwValue = D3DBLEND_INVCONSTANTCOLOR;
		return S_OK;
	}
    if (_stricmp(str, "CONSTANTALPHA" ) == 0)
	{
		*pdwValue = D3DBLEND_CONSTANTALPHA;
		return S_OK;
	}
    if (_stricmp(str, "INVCONSTANTALPHA" ) == 0)
	{
		*pdwValue = D3DBLEND_INVCONSTANTALPHA;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DBLENDOP_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DBLENDOP_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "ADD" ) == 0)
	{
		*pdwValue = D3DBLENDOP_ADD;
		return S_OK;
	}
    if (_stricmp(str, "SUBTRACT" ) == 0)
	{
		*pdwValue = D3DBLENDOP_SUBTRACT;
		return S_OK;
	}
    if (_stricmp(str, "REVSUBTRACT" ) == 0)
	{
		*pdwValue = D3DBLENDOP_REVSUBTRACT;
		return S_OK;
	}
    if (_stricmp(str, "MIN" ) == 0)
	{
		*pdwValue = D3DBLENDOP_MIN;
		return S_OK;
	}
    if (_stricmp(str, "MAX" ) == 0)
	{
		*pdwValue = D3DBLENDOP_MAX;
		return S_OK;
	}
    if (_stricmp(str, "ADDSIGNED" ) == 0)
	{
		*pdwValue = D3DBLENDOP_ADDSIGNED;
		return S_OK;
	}
    if (_stricmp(str, "REVSUBTRACTSIGNED" ) == 0)
	{
		*pdwValue = D3DBLENDOP_REVSUBTRACTSIGNED;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DCMPFUNC_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DCMP_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "NEVER" ) == 0)
	{
		*pdwValue = D3DCMP_NEVER;
		return S_OK;
	}
    if (_stricmp(str, "LESS" ) == 0)
	{
		*pdwValue = D3DCMP_LESS;
		return S_OK;
	}
    if (_stricmp(str, "EQUAL" ) == 0)
	{
		*pdwValue = D3DCMP_EQUAL;
		return S_OK;
	}
    if (_stricmp(str, "LESSEQUAL" ) == 0)
	{
		*pdwValue = D3DCMP_LESSEQUAL;
		return S_OK;
	}
    if (_stricmp(str, "GREATER" ) == 0)
	{
		*pdwValue = D3DCMP_GREATER;
		return S_OK;
	}
    if (_stricmp(str, "NOTEQUAL" ) == 0)
	{
		*pdwValue = D3DCMP_NOTEQUAL;
		return S_OK;
	}
    if (_stricmp(str, "GREATEREQUAL" ) == 0)
	{
		*pdwValue = D3DCMP_GREATEREQUAL;
		return S_OK;
	}
    if (_stricmp(str, "ALWAYS" ) == 0)
	{
		*pdwValue = D3DCMP_ALWAYS;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DCOLORWRITEENABLE_FromString(const CHAR *str, DWORD *pdwValue)
{
	return E_NOTIMPL;
}

HRESULT D3DCOLOR_FromString(const CHAR *str, DWORD *pdwValue)
{
	return DWORD_FromString(str, pdwValue );
}

HRESULT D3DCULL_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DCULL_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

	if (_stricmp(str, "NONE" ) == 0)
	{
		*pdwValue = D3DCULL_NONE;
		return S_OK;
	}
    if (_stricmp(str, "CW" ) == 0)
	{
		*pdwValue = D3DCULL_CW;
		return S_OK;
	}
    if (_stricmp(str, "CCW" ) == 0)
	{
		*pdwValue = D3DCULL_CCW;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DFRONT_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DFRONT_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "CW" ) == 0)
	{
		*pdwValue = D3DFRONT_CW;
		return S_OK;
	}
    if (_stricmp(str, "CCW" ) == 0)
	{
		*pdwValue = D3DFRONT_CCW;
		return S_OK;
	}
#undef MATCH	
	return E_FAIL;
}

HRESULT D3DFILLMODE_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DFILL_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

	if (_stricmp(str, "POINT" ) == 0)
	{
		*pdwValue = D3DFILL_POINT;
		return S_OK;
	}
	if (_stricmp(str, "WIREFRAME" ) == 0)
	{
		*pdwValue = D3DFILL_WIREFRAME;
		return S_OK;
	}
    if (_stricmp(str, "SOLID" ) == 0)
	{
		*pdwValue = D3DFILL_SOLID;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DFOGMODE_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DFOG_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "NONE" ) == 0)
	{
		*pdwValue = D3DFOG_NONE;
		return S_OK;
	}
    if (_stricmp(str, "EXP" ) == 0)
	{
		*pdwValue = D3DFOG_EXP;
		return S_OK;
	}
    if (_stricmp(str, "EXP2" ) == 0)
	{
		*pdwValue = D3DFOG_EXP2;
		return S_OK;
	}
    if (_stricmp(str, "LINEAR" ) == 0)
	{
		*pdwValue = D3DFOG_LINEAR;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DLOGICOP_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DLOGICOP_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "NONE" ) == 0)
	{
		*pdwValue = D3DLOGICOP_NONE;
		return S_OK;
	}
    if (_stricmp(str, "CLEAR" ) == 0)
	{
		*pdwValue = D3DLOGICOP_CLEAR;
		return S_OK;
	}
    if (_stricmp(str, "AND" ) == 0)
	{
		*pdwValue = D3DLOGICOP_AND;
		return S_OK;
	}
    if (_stricmp(str, "AND_REVERSE" ) == 0)
	{
		*pdwValue = D3DLOGICOP_AND_REVERSE;
		return S_OK;
	}
    if (_stricmp(str, "COPY" ) == 0)
	{
		*pdwValue = D3DLOGICOP_COPY;
		return S_OK;
	}
    if (_stricmp(str, "AND_INVERTED" ) == 0)
	{
		*pdwValue = D3DLOGICOP_AND_INVERTED;
		return S_OK;
	}
    if (_stricmp(str, "NOOP" ) == 0)
	{
		*pdwValue = D3DLOGICOP_NOOP;
		return S_OK;
	}
    if (_stricmp(str, "XOR" ) == 0)
	{
		*pdwValue = D3DLOGICOP_XOR;
		return S_OK;
	}
    if (_stricmp(str, "OR" ) == 0)
	{
		*pdwValue = D3DLOGICOP_OR;
		return S_OK;
	}
    if (_stricmp(str, "NOR" ) == 0)
	{
		*pdwValue = D3DLOGICOP_NOR;
		return S_OK;
	}
    if (_stricmp(str, "EQUIV" ) == 0)
	{
		*pdwValue = D3DLOGICOP_EQUIV;
		return S_OK;
	}
    if (_stricmp(str, "INVERT" ) == 0)
	{
		*pdwValue = D3DLOGICOP_INVERT;
		return S_OK;
	}
    if (_stricmp(str, "OR_REVERSE" ) == 0)
	{
		*pdwValue = D3DLOGICOP_OR_REVERSE;
		return S_OK;
	}
    if (_stricmp(str, "COPY_INVERTED" ) == 0)
	{
		*pdwValue = D3DLOGICOP_COPY_INVERTED;
		return S_OK;
	}
    if (_stricmp(str, "OR_INVERTED" ) == 0)
	{
		*pdwValue = D3DLOGICOP_OR_INVERTED;
		return S_OK;
	}
    if (_stricmp(str, "NAND" ) == 0)
	{
		*pdwValue = D3DLOGICOP_NAND;
		return S_OK;
	}
    if (_stricmp(str, "SET" ) == 0)
	{
		*pdwValue = D3DLOGICOP_SET;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DMATERIALCOLORSOURCE_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DMCS_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "MATERIAL" ) == 0)
	{
		*pdwValue = D3DMCS_MATERIAL;
		return S_OK;
	}            // Color from material is used
    if (_stricmp(str, "COLOR1" ) == 0)
	{
		*pdwValue = D3DMCS_COLOR1;
		return S_OK;
	}            // Diffuse vertex color is used
    if (_stricmp(str, "COLOR2" ) == 0)
	{
		*pdwValue = D3DMCS_COLOR2;
		return S_OK;
	}            // Specular vertex color is used
	return E_FAIL;
}

/*
HRESULT D3DMULTISAMPLE_TYPE_FromString(const CHAR *str, DWORD *pdwValue)
{
	return DWORD_FromString(str, pdwValue );
}
*/

HRESULT D3DPATCHEDGESTYLE_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DPATCHEDGE_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

	if (_stricmp(str, "DISCRETE" ) == 0)
	{
		*pdwValue = D3DPATCHEDGE_DISCRETE;
		return S_OK;
	}
	if (_stricmp(str, "CONTINUOUS" ) == 0)
	{
		*pdwValue = D3DPATCHEDGE_CONTINUOUS;
		return S_OK;
	}
#undef MATCH
	return E_FAIL;
}

HRESULT D3DSHADEMODE_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DSHADE_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "FLAT" ) == 0)
	{
		*pdwValue = D3DSHADE_FLAT;
		return S_OK;
	}
    if (_stricmp(str, "GOURAUD" ) == 0)
	{
		*pdwValue = D3DSHADE_GOURAUD;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DSTENCILOP_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DSTENCILOP_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "KEEP" ) == 0)
	{
		*pdwValue = D3DSTENCILOP_KEEP;
		return S_OK;
	}
    if (_stricmp(str, "ZERO" ) == 0)
	{
		*pdwValue = D3DSTENCILOP_ZERO;
		return S_OK;
	}
    if (_stricmp(str, "REPLACE" ) == 0)
	{
		*pdwValue = D3DSTENCILOP_REPLACE;
		return S_OK;
	}
    if (_stricmp(str, "INCRSAT" ) == 0)
	{
		*pdwValue = D3DSTENCILOP_INCRSAT;
		return S_OK;
	}
    if (_stricmp(str, "DECRSAT" ) == 0)
	{
		*pdwValue = D3DSTENCILOP_DECRSAT;
		return S_OK;
	}
    if (_stricmp(str, "INVERT" ) == 0)
	{
		*pdwValue = D3DSTENCILOP_INVERT;
		return S_OK;
	}
    if (_stricmp(str, "INCR" ) == 0)
	{
		*pdwValue = D3DSTENCILOP_INCR;
		return S_OK;
	}
    if (_stricmp(str, "DECR" ) == 0)
	{
		*pdwValue = D3DSTENCILOP_DECR;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DSWATHWIDTH_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DSWATH_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "8" ) == 0)
	{
		*pdwValue = D3DSWATH_8;
		return S_OK;
	}
    if (_stricmp(str, "16" ) == 0)
	{
		*pdwValue = D3DSWATH_16;
		return S_OK;
	}
    if (_stricmp(str, "32" ) == 0)
	{
		*pdwValue = D3DSWATH_32;
		return S_OK;
	}
    if (_stricmp(str, "64" ) == 0)
	{
		*pdwValue = D3DSWATH_64;
		return S_OK;
	}
    if (_stricmp(str, "128" ) == 0)
	{
		*pdwValue = D3DSWATH_128;
		return S_OK;
	}
    if (_stricmp(str, "OFF" ) == 0)
	{
		*pdwValue = D3DSWATH_OFF;
		return S_OK;
	}
#undef MATCH
	return E_FAIL;
}

HRESULT D3DVERTEXBLENDFLAGS_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DVBF_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

    if (_stricmp(str, "DISABLE" ) == 0)
	{
		*pdwValue = D3DVBF_DISABLE;
		return S_OK;
	}     // Disable vertex blending
    if (_stricmp(str, "1WEIGHTS" ) == 0)
	{
		*pdwValue = D3DVBF_1WEIGHTS;
		return S_OK;
	}     // 2 matrix blending
    if (_stricmp(str, "2WEIGHTS" ) == 0)
	{
		*pdwValue = D3DVBF_2WEIGHTS;
		return S_OK;
	}     // 3 matrix blending
    if (_stricmp(str, "3WEIGHTS" ) == 0)
	{
		*pdwValue = D3DVBF_3WEIGHTS;
		return S_OK;
	}     // 4 matrix blending
    if (_stricmp(str, "2WEIGHTS2MATRICES" ) == 0)
	{
		*pdwValue = D3DVBF_2WEIGHTS2MATRICES;
		return S_OK;
	}     // Xbox extension
    if (_stricmp(str, "3WEIGHTS3MATRICES" ) == 0)
	{
		*pdwValue = D3DVBF_3WEIGHTS3MATRICES;
		return S_OK;
	}     // Xbox extension
    if (_stricmp(str, "4WEIGHTS4MATRICES" ) == 0)
	{
		*pdwValue = D3DVBF_4WEIGHTS4MATRICES;
		return S_OK;
	}     // Xbox extension
	return E_FAIL;
}

HRESULT D3DWRAP_FromString(const CHAR *str, DWORD *pdwValue)
{
	return E_NOTIMPL;
}

HRESULT D3DZBUFFERTYPE_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DZB_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}

	if (_stricmp(str, "FALSE" ) == 0)
	{
		*pdwValue = D3DZB_FALSE;
		return S_OK;
	}
    if (_stricmp(str, "TRUE" ) == 0)
	{
		*pdwValue = D3DZB_TRUE;
		return S_OK;
	}
    if (_stricmp(str, "USEW" ) == 0)
	{
		*pdwValue = D3DZB_USEW;
		return S_OK;
	}
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
// Texture state helpers
//
HRESULT D3DTEXF_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DTEXF_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}
    if (_stricmp(str, "NONE") == 0)
	{
		*pdwValue = D3DTEXF_NONE;
		return S_OK;
	}
    if (_stricmp(str, "POINT") == 0)
	{
		*pdwValue = D3DTEXF_POINT;
		return S_OK;
	}
    if (_stricmp(str, "LINEAR") == 0)
	{
		*pdwValue = D3DTEXF_LINEAR;
		return S_OK;
	}
    if (_stricmp(str, "ANISOTROPIC") == 0)
	{
		*pdwValue = D3DTEXF_ANISOTROPIC;
		return S_OK;
	}
    if (_stricmp(str, "QUINCUNX") == 0)
	{
		*pdwValue = D3DTEXF_QUINCUNX;
		return S_OK;
	}
    if (_stricmp(str, "GAUSSIANCUBIC") == 0)
	{
		*pdwValue = D3DTEXF_GAUSSIANCUBIC;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DTEXTUREADDRESS_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DTADDRESS_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}
    if (_stricmp(str, "WRAP") == 0)
	{
		*pdwValue = D3DTADDRESS_WRAP;
		return S_OK;
	}
    if (_stricmp(str, "MIRROR") == 0)
	{
		*pdwValue = D3DTADDRESS_MIRROR;
		return S_OK;
	}
    if (_stricmp(str, "CLAMP") == 0)
	{
		*pdwValue = D3DTADDRESS_CLAMP;
		return S_OK;
	}
    if (_stricmp(str, "BORDER") == 0)
	{
		*pdwValue = D3DTADDRESS_BORDER;
		return S_OK;
	}
    if (_stricmp(str, "CLAMPTOEDGE") == 0)
	{
		*pdwValue = D3DTADDRESS_CLAMPTOEDGE;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DTEXTUREALPHAKILL_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DTALPHAKILL_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}
	
    if (_stricmp(str, "DISABLE") == 0
		|| _stricmp(str, "FALSE") == 0)
	{
		*pdwValue = D3DTALPHAKILL_DISABLE;
		return S_OK;
	}
    if (_stricmp(str, "ENABLE") == 0
		|| _stricmp(str, "TRUE") == 0)		// why is alpha kill different from all the other TRUE/FALSE settings?
	{
		*pdwValue = D3DTALPHAKILL_ENABLE;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DTEXTUREARG_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DTA_";
	int len = strlen(prefix);

	// copy the input string to a temporary buffer so we can use strtok
	const int buflen = 1000;
	CHAR buf[buflen];
	strncpy(buf, str, buflen);
	buf[buflen - 1] = 0;
	CHAR *strSeparators = " \t\n|";		// '|' is a valid separator in addition to whitespace
	CHAR *strBuff = strtok(buf, strSeparators);

	// look for texture args
	bool bComplement = false;
	bool bAlphaReplicate = false;
	DWORD dwSelect = (DWORD)-1;
	while (strBuff != NULL)
	{
		// skip prefix
		if (_strnicmp(strBuff, prefix, len) == 0)
		{
			strBuff += len;
		}

		// look for matches
		if (dwSelect == -1)
		{
			if (_stricmp(strBuff, "DIFFUSE") == 0)
			{
				dwSelect =  D3DTA_DIFFUSE;
			}
			else if (_stricmp(strBuff, "CURRENT") == 0)
			{
				dwSelect =  D3DTA_CURRENT;
			}
			else if (_stricmp(strBuff, "TEXTURE") == 0)
			{
				dwSelect =  D3DTA_TEXTURE;
			}
			else if (_stricmp(strBuff, "TFACTOR") == 0)
			{
				dwSelect =  D3DTA_TFACTOR;
			}
			else if (_stricmp(strBuff, "SPECULAR") == 0)
			{
				dwSelect =  D3DTA_SPECULAR;
			}
			else if (_stricmp(strBuff, "TEMP") == 0)
			{
				dwSelect =  D3DTA_TEMP;
			}
			else if (_stricmp(strBuff, "COMPLEMENT") == 0)
			{
				bComplement = true;
			}
			else if (_stricmp(strBuff, "ALPHAREPLICATE") == 0)
			{
				bAlphaReplicate = true;
			}
			else
				return E_FAIL;
		}
		else // look just for complement and alpha replicate
		{
			if (_stricmp(strBuff, "COMPLEMENT") == 0)
			{
				bComplement = true;
			}
			else if (_stricmp(strBuff, "ALPHAREPLICATE") == 0)
			{
				bAlphaReplicate = true;
			}
			else
				return E_FAIL;
		}
#define D3DTA_COMPLEMENT        0x00000010  // take 1.0 - x (read modifier)
#define D3DTA_ALPHAREPLICATE    0x00000020  // replicate alpha to color components (read modifier)
		
		// Get next token
		strBuff = strtok(NULL, strSeparators);
	}
	if (dwSelect == -1)
		return E_FAIL;	// TODO: better error message

	// set return value
	if (bComplement)
		dwSelect |= D3DTA_COMPLEMENT;
	if (bAlphaReplicate)
		dwSelect |= D3DTA_ALPHAREPLICATE;
	*pdwValue = dwSelect;
	return S_OK;
}

HRESULT D3DTEXTURECOLORKEYOP_FromString(const CHAR *str, DWORD *pdwValue)
{
	CHAR *prefix = "D3DTCOLORKEYOP_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}
	
    if (_stricmp(str, "DISABLE") == 0)
	{
		*pdwValue = D3DTCOLORKEYOP_DISABLE;
		return S_OK;
	}
    if (_stricmp(str, "ALPHA") == 0)
	{
		*pdwValue = D3DTCOLORKEYOP_ALPHA;
		return S_OK;
	}
    if (_stricmp(str, "RGBA") == 0)
	{
		*pdwValue = D3DTCOLORKEYOP_RGBA;
		return S_OK;
	}
    if (_stricmp(str, "KILL") == 0)
	{
		*pdwValue = D3DTCOLORKEYOP_KILL;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DTEXTUREOP_FromString(const CHAR *str, DWORD *pdwValue)
{
	// These should be done with the pixel shader, not with the texture stage states. 
	CHAR *prefix = "D3DTOP_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}
	
    if (_stricmp(str, "DISABLE") == 0)
	{
		*pdwValue = D3DTOP_DISABLE;
		return S_OK;
	}
    if (_stricmp(str, "SELECTARG1") == 0)
	{
		*pdwValue = D3DTOP_SELECTARG1;
		return S_OK;
	}
    if (_stricmp(str, "SELECTARG2") == 0)
	{
		*pdwValue = D3DTOP_SELECTARG2;
		return S_OK;
	}
    if (_stricmp(str, "MODULATE") == 0)
	{
		*pdwValue = D3DTOP_MODULATE;
		return S_OK;
	}
    if (_stricmp(str, "MODULATE2X") == 0)
	{
		*pdwValue = D3DTOP_MODULATE2X;
		return S_OK;
	}
    if (_stricmp(str, "MODULATE4X") == 0)
	{
		*pdwValue = D3DTOP_MODULATE4X;
		return S_OK;
	}
    if (_stricmp(str, "ADD") == 0)
	{
		*pdwValue = D3DTOP_ADD;
		return S_OK;
	}
    if (_stricmp(str, "ADDSIGNED") == 0)
	{
		*pdwValue = D3DTOP_ADDSIGNED;
		return S_OK;
	}
    if (_stricmp(str, "ADDSIGNED2X") == 0)
	{
		*pdwValue = D3DTOP_ADDSIGNED2X;
		return S_OK;
	}
    if (_stricmp(str, "SUBTRACT") == 0)
	{
		*pdwValue = D3DTOP_SUBTRACT;
		return S_OK;
	}
    if (_stricmp(str, "ADDSMOOTH") == 0)
	{
		*pdwValue = D3DTOP_ADDSMOOTH;
		return S_OK;
	}
    if (_stricmp(str, "BLENDDIFFUSEALPHA") == 0)
	{
		*pdwValue = D3DTOP_BLENDDIFFUSEALPHA;
		return S_OK;
	}
    if (_stricmp(str, "BLENDTEXTUREALPHA") == 0)
	{
		*pdwValue = D3DTOP_BLENDTEXTUREALPHA;
		return S_OK;
	}
    if (_stricmp(str, "BLENDFACTORALPHA") == 0)
	{
		*pdwValue = D3DTOP_BLENDFACTORALPHA;
		return S_OK;
	}
    if (_stricmp(str, "BLENDTEXTUREALPHAPM") == 0)
	{
		*pdwValue = D3DTOP_BLENDTEXTUREALPHAPM;
		return S_OK;
	}
    if (_stricmp(str, "BLENDCURRENTALPHA") == 0)
	{
		*pdwValue = D3DTOP_BLENDCURRENTALPHA;
		return S_OK;
	}
    if (_stricmp(str, "PREMODULATE") == 0)
	{
		*pdwValue = D3DTOP_PREMODULATE;
		return S_OK;
	}
    if (_stricmp(str, "MODULATEALPHA_ADDCOLOR") == 0)
	{
		*pdwValue = D3DTOP_MODULATEALPHA_ADDCOLOR;
		return S_OK;
	}
    if (_stricmp(str, "MODULATECOLOR_ADDALPHA") == 0)
	{
		*pdwValue = D3DTOP_MODULATECOLOR_ADDALPHA;
		return S_OK;
	}
    if (_stricmp(str, "MODULATEINVALPHA_ADDCOLOR") == 0)
	{
		*pdwValue = D3DTOP_MODULATEINVALPHA_ADDCOLOR;
		return S_OK;
	}
    if (_stricmp(str, "MODULATEINVCOLOR_ADDALPHA") == 0)
	{
		*pdwValue = D3DTOP_MODULATEINVCOLOR_ADDALPHA;
		return S_OK;
	}
    if (_stricmp(str, "DOTPRODUCT3") == 0)
	{
		*pdwValue = D3DTOP_DOTPRODUCT3;
		return S_OK;
	}
    if (_stricmp(str, "MULTIPLYADD") == 0)
	{
		*pdwValue = D3DTOP_MULTIPLYADD;
		return S_OK;
	}
    if (_stricmp(str, "LERP") == 0)
	{
		*pdwValue = D3DTOP_LERP;
		return S_OK;
	}
    if (_stricmp(str, "BUMPENVMAP") == 0)
	{
		*pdwValue = D3DTOP_BUMPENVMAP;
		return S_OK;
	}
    if (_stricmp(str, "BUMPENVMAPLUMINANCE") == 0)
	{
		*pdwValue = D3DTOP_BUMPENVMAPLUMINANCE;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT D3DTEXTURETRANSFORMFLAGS_FromString(const CHAR *str, DWORD *pdwValue)
{
	// These should be done with the vertex shader, not here.
	return E_NOTIMPL;
/*	
	CHAR *prefix = "D3DTTFF_";
	int len = strlen(prefix);
	if (_strnicmp(str, prefix, len) == 0)
	{
		str += len;
	}
	
    if (_stricmp(str, "DISABLE") == 0)
	{
		*pdwValue = D3DTTFF_DISABLE;
		return S_OK;
	}
    if (_stricmp(str, "COUNT1") == 0)
	{
		*pdwValue = D3DTTFF_COUNT1;
		return S_OK;
	}
    if (_stricmp(str, "COUNT2") == 0)
	{
		*pdwValue = D3DTTFF_COUNT2;
		return S_OK;
	}
    if (_stricmp(str, "COUNT3") == 0)
	{
		*pdwValue = D3DTTFF_COUNT3;
		return S_OK;
	}
    if (_stricmp(str, "COUNT4") == 0)
	{
		*pdwValue = D3DTTFF_COUNT4;
		return S_OK;
	}
    if (_stricmp(str, "PROJECTED") == 0)
	{
		*pdwValue = D3DTTFF_PROJECTED;
		return S_OK;
	}
	return E_FAIL;
*/
}

HRESULT D3DTSIGN_FromString(const CHAR *str, DWORD *pdwValue)
{
	return E_NOTIMPL;
	/*
#define D3DTSIGN_ASIGNED           0x10000000
#define D3DTSIGN_AUNSIGNED         0
#define D3DTSIGN_RSIGNED           0x20000000
#define D3DTSIGN_RUNSIGNED         0
#define D3DTSIGN_GSIGNED           0x40000000
#define D3DTSIGN_GUNSIGNED         0
#define D3DTSIGN_BSIGNED           0x80000000
#define D3DTSIGN_BUNSIGNED         0
	*/
}



HRESULT XBContent::BeginDraw(ISAXAttributes *pAttributes)
{
	Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
	PushContext(Draw_Context);
	m_rContextStack[m_iContext].m_pData = pEffect;	// keep effect pointer
	Pass *pPass = &pEffect->rPass[pEffect->PassCount];
	if (pPass->rDraw == NULL)
	{
		pPass->rDraw = new Draw [ MAX_DRAW ];
		if (pPass->rDraw == NULL)
			return E_OUTOFMEMORY;
	}
	if (pPass->DrawCount >= MAX_DRAW)
		return E_NOTIMPL;
	Draw *pDraw = &pPass->rDraw[pPass->DrawCount];
	ZeroMemory(pDraw, sizeof(Draw));

	// TODO: check that draw has not already been set. If so, make
	// a new pass with the same state.

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
		if (MATCH(idref))
		{
			CHAR *strID = CharString(pwchValue, cchValue);
			DWORD dwResourceIndex;
			if (!m_pBundler->IsExistingIdentifier(strID, &dwResourceIndex))
			{
				m_pBundler->ErrorMsg("Unknown index buffer resource \"%s\"\n", strID);
				return E_FAIL;
			}
			pDraw->IndexBufferResource = dwResourceIndex;
		}
		else if (MATCH(primitive))
		{
			CHAR *strPrimitive = CharString(pwchValue, cchValue);
			pDraw->Primitive = PrimitiveTypeFromString(strPrimitive);
			if (pDraw->Primitive == (D3DPRIMITIVETYPE)-1)
			{
				m_pBundler->ErrorMsg("Unknown primitive type \"%s\"\n", strPrimitive);
				return E_FAIL;
			}
		}
		else if (MATCH(start))
		{
			pDraw->Start = atoi(CharString(pwchValue, cchValue));
		}
		else if (MATCH(count))
		{
			pDraw->Count = atoi(CharString(pwchValue, cchValue));
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	
	// TODO: check that all required parameters have been set
	pPass->DrawCount++;
	return S_OK;
}

HRESULT XBContent::EndDraw()
{
	PopContext();
	return S_OK;
}
	
HRESULT XBContent::BeginRenderState(ISAXAttributes *pAttributes)
{
	HRESULT hr;
	Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
	PushContext(RenderState_Context);
	m_rContextStack[m_iContext].m_pData = pEffect;	// keep effect pointer
	Pass *pPass = &pEffect->rPass[pEffect->PassCount];
	if (pPass->rRenderState != NULL)
	{
		fatalError(m_pLocator, L"<RenderState> tag must appear just once within a pass block\n", E_FAIL);
		m_bErrorSuppress = true;
		return E_FAIL;
	}

	// Render state to be filled in by attributes
	RenderState rRenderState[ MAX_RENDER_STATE ];
	memset(rRenderState, -1, sizeof(rRenderState));
	
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
		CHAR *str = CharString(pwchValue, cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(D3DRS_))
		{
			// Strip off D3DRS_ prefix
			pwchLocalName += 6;
			cchLocalName -= 6;
		}
		DWORD State = (DWORD)-1;
		DWORD Value;
		if (MATCH(PS))
		{
			// strip off PS prefix
			pwchLocalName += 2;
			cchLocalName -= 2;
			
			// look for pixel shader render states
			if (MATCH(ALPHAINPUTS0))
			{
				State = D3DRS_PSALPHAINPUTS0;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAINPUTS1))
			{
				State = D3DRS_PSALPHAINPUTS1;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAINPUTS2))
			{
				State = D3DRS_PSALPHAINPUTS2;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAINPUTS3))
			{
				State = D3DRS_PSALPHAINPUTS3;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAINPUTS4))
			{
				State = D3DRS_PSALPHAINPUTS4;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAINPUTS5))
			{
				State = D3DRS_PSALPHAINPUTS5;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAINPUTS6))
			{
				State = D3DRS_PSALPHAINPUTS6;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAINPUTS7))
			{
				State = D3DRS_PSALPHAINPUTS7;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(FINALCOMBINERINPUTSABCD))
			{
				State = D3DRS_PSFINALCOMBINERINPUTSABCD;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(FINALCOMBINERINPUTSEFG))
			{
				State = D3DRS_PSFINALCOMBINERINPUTSEFG;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT0_0))
			{
				State = D3DRS_PSCONSTANT0_0;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT0_1))
			{
				State = D3DRS_PSCONSTANT0_1;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT0_2))
			{
				State = D3DRS_PSCONSTANT0_2;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT0_3))
			{
				State = D3DRS_PSCONSTANT0_3;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT0_4))
			{
				State = D3DRS_PSCONSTANT0_4;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT0_5))
			{
				State = D3DRS_PSCONSTANT0_5;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT0_6))
			{
				State = D3DRS_PSCONSTANT0_6;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT0_7))
			{
				State = D3DRS_PSCONSTANT0_7;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT1_0))
			{
				State = D3DRS_PSCONSTANT1_0;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT1_1))
			{
				State = D3DRS_PSCONSTANT1_1;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT1_2))
			{
				State = D3DRS_PSCONSTANT1_2;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT1_3))
			{
				State = D3DRS_PSCONSTANT1_3;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT1_4))
			{
				State = D3DRS_PSCONSTANT1_4;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT1_5))
			{
				State = D3DRS_PSCONSTANT1_5;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT1_6))
			{
				State = D3DRS_PSCONSTANT1_6;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(CONSTANT1_7))
			{
				State = D3DRS_PSCONSTANT1_7;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOUTPUTS0))
			{
				State = D3DRS_PSALPHAOUTPUTS0;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOUTPUTS1))
			{
				State = D3DRS_PSALPHAOUTPUTS1;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOUTPUTS2))
			{
				State = D3DRS_PSALPHAOUTPUTS2;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOUTPUTS3))
			{
				State = D3DRS_PSALPHAOUTPUTS3;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOUTPUTS4))
			{
				State = D3DRS_PSALPHAOUTPUTS4;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOUTPUTS5))
			{
				State = D3DRS_PSALPHAOUTPUTS5;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOUTPUTS6))
			{
				State = D3DRS_PSALPHAOUTPUTS6;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOUTPUTS7))
			{
				State = D3DRS_PSALPHAOUTPUTS7;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBINPUTS0))
			{
				State = D3DRS_PSRGBINPUTS0;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBINPUTS1))
			{
				State = D3DRS_PSRGBINPUTS1;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBINPUTS2))
			{
				State = D3DRS_PSRGBINPUTS2;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBINPUTS3))
			{
				State = D3DRS_PSRGBINPUTS3;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBINPUTS4))
			{
				State = D3DRS_PSRGBINPUTS4;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBINPUTS5))
			{
				State = D3DRS_PSRGBINPUTS5;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBINPUTS6))
			{
				State = D3DRS_PSRGBINPUTS6;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBINPUTS7))
			{
				State = D3DRS_PSRGBINPUTS7;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(COMPAREMODE))
			{
				State = D3DRS_PSCOMPAREMODE;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(FINALCOMBINERCONSTANT0))
			{
				State = D3DRS_PSFINALCOMBINERCONSTANT0;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(FINALCOMBINERCONSTANT1))
			{
				State = D3DRS_PSFINALCOMBINERCONSTANT1;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBOUTPUTS0))
			{
				State = D3DRS_PSRGBOUTPUTS0;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBOUTPUTS1))
			{
				State = D3DRS_PSRGBOUTPUTS1;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBOUTPUTS2))
			{
				State = D3DRS_PSRGBOUTPUTS2;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBOUTPUTS3))
			{
				State = D3DRS_PSRGBOUTPUTS3;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBOUTPUTS4))
			{
				State = D3DRS_PSRGBOUTPUTS4;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBOUTPUTS5))
			{
				State = D3DRS_PSRGBOUTPUTS5;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBOUTPUTS6))
			{
				State = D3DRS_PSRGBOUTPUTS6;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(RGBOUTPUTS7))
			{
				State = D3DRS_PSRGBOUTPUTS7;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(COMBINERCOUNT))
			{
				State = D3DRS_PSCOMBINERCOUNT;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(DOTMAPPING))
			{
				State = D3DRS_PSDOTMAPPING;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(INPUTTEXTURE))
			{
				State = D3DRS_PSINPUTTEXTURE;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(TEXTUREMODES))
			{
				State = D3DRS_PSTEXTUREMODES;
				hr = DWORD_FromString(str, &Value);
			}
			else
			{
				return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
			}
		}
		// Non Pixel Shader render states
		else if (MATCH(ZFUNC))
		{
			State = D3DRS_ZFUNC;
			hr = D3DCMPFUNC_FromString(str, &Value);
		}
		else if (MATCH(ALPHAFUNC))
		{
			State = D3DRS_ALPHAFUNC;
			hr = D3DCMPFUNC_FromString(str, &Value);
		}
		else if (MATCH(ALPHABLENDENABLE))
		{
			State = D3DRS_ALPHABLENDENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(ALPHATESTENABLE))
		{
			State = D3DRS_ALPHATESTENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(ALPHAREF))
		{
			State = D3DRS_ALPHAREF;
			hr = BYTE_FromString(str, &Value);
		}
		else if (MATCH(SRCBLEND))
		{
			State = D3DRS_SRCBLEND;
			hr = D3DBLEND_FromString(str, &Value);
		}
		else if (MATCH(DESTBLEND))
		{
			State = D3DRS_DESTBLEND;
			hr = D3DBLEND_FromString(str, &Value);
		}
		else if (MATCH(ZWRITEENABLE))
		{
			State = D3DRS_ZWRITEENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(DITHERENABLE))
		{
			State = D3DRS_DITHERENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(SHADEMODE))
		{
			State = D3DRS_SHADEMODE;
			hr = D3DSHADEMODE_FromString(str, &Value);
		}
		else if (MATCH(COLORWRITEENABLE))
		{
			State = D3DRS_COLORWRITEENABLE;
			hr = D3DCOLORWRITEENABLE_FromString(str, &Value);
		}
		else if (MATCH(STENCILZFAIL))
		{
			State = D3DRS_STENCILZFAIL;
			hr = D3DSTENCILOP_FromString(str, &Value);
		}
		else if (MATCH(STENCILPASS))
		{
			State = D3DRS_STENCILPASS;
			hr = D3DSTENCILOP_FromString(str, &Value);
		}
		else if (MATCH(STENCILFUNC))
		{
			State = D3DRS_STENCILFUNC;
			hr = D3DCMPFUNC_FromString(str, &Value);
		}
		else if (MATCH(STENCILREF))
		{
			State = D3DRS_STENCILREF;
			hr = BYTE_FromString(str, &Value);
		}
		else if (MATCH(STENCILMASK))
		{
			State = D3DRS_STENCILMASK;
			hr = BYTE_FromString(str, &Value);
		}
		else if (MATCH(STENCILWRITEMASK))
		{
			State = D3DRS_STENCILWRITEMASK;
			hr = BYTE_FromString(str, &Value);
		}
		else if (MATCH(BLENDOP))
		{
			State = D3DRS_BLENDOP;
			hr = D3DBLENDOP_FromString(str, &Value);
		}
		else if (MATCH(BLENDCOLOR))
		{
			State = D3DRS_BLENDCOLOR;
			hr = D3DCOLOR_FromString(str, &Value);
		}
		else if (MATCH(SWATHWIDTH))
		{
			State = D3DRS_SWATHWIDTH;
			hr = D3DSWATHWIDTH_FromString(str, &Value);
		}
		else if (MATCH(POLYGONOFFSETZSLOPESCALE))
		{
			State = D3DRS_POLYGONOFFSETZSLOPESCALE;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(POLYGONOFFSETZOFFSET))
		{
			State = D3DRS_POLYGONOFFSETZOFFSET;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(POINTOFFSETENABLE))
		{
			State = D3DRS_POINTOFFSETENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(WIREFRAMEOFFSETENABLE))
		{
			State = D3DRS_WIREFRAMEOFFSETENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(SOLIDOFFSETENABLE))
		{
			State = D3DRS_SOLIDOFFSETENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(FOGENABLE))
		{
			State = D3DRS_FOGENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(FOGTABLEMODE))
		{
			State = D3DRS_FOGTABLEMODE;
			hr = D3DFOGMODE_FromString(str, &Value);
		}
		else if (MATCH(FOGSTART))
		{
			State = D3DRS_FOGSTART;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(FOGEND))
		{
			State = D3DRS_FOGEND;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(FOGDENSITY))
		{
			State = D3DRS_FOGDENSITY;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(RANGEFOGENABLE))
		{
			State = D3DRS_RANGEFOGENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(WRAP0))
		{
			State = D3DRS_WRAP0;
			hr = D3DWRAP_FromString(str, &Value);
		}
		else if (MATCH(WRAP1))
		{
			State = D3DRS_WRAP1;
			hr = D3DWRAP_FromString(str, &Value);
		}
		else if (MATCH(WRAP2))
		{
			State = D3DRS_WRAP2;
			hr = D3DWRAP_FromString(str, &Value);
		}
		else if (MATCH(WRAP3))
		{
			State = D3DRS_WRAP3;
			hr = D3DWRAP_FromString(str, &Value);
		}
		else if (MATCH(LIGHTING))
		{
			State = D3DRS_LIGHTING;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(SPECULARENABLE))
		{
			State = D3DRS_SPECULARENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(LOCALVIEWER))
		{
			State = D3DRS_LOCALVIEWER;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(COLORVERTEX))
		{
			State = D3DRS_COLORVERTEX;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(BACKSPECULARMATERIALSOURCE))
		{
			State = D3DRS_BACKSPECULARMATERIALSOURCE;
			hr = D3DMATERIALCOLORSOURCE_FromString(str, &Value);
		}
		else if (MATCH(BACKDIFFUSEMATERIALSOURCE))
		{
			State = D3DRS_BACKDIFFUSEMATERIALSOURCE;
			hr = D3DMATERIALCOLORSOURCE_FromString(str, &Value);
		}
		else if (MATCH(BACKAMBIENTMATERIALSOURCE))
		{
			State = D3DRS_BACKAMBIENTMATERIALSOURCE;
			hr = D3DMATERIALCOLORSOURCE_FromString(str, &Value);
		}
		else if (MATCH(BACKEMISSIVEMATERIALSOURCE))
		{
			State = D3DRS_BACKEMISSIVEMATERIALSOURCE;
			hr = D3DMATERIALCOLORSOURCE_FromString(str, &Value);
		}
		else if (MATCH(SPECULARMATERIALSOURCE))
		{
			State = D3DRS_SPECULARMATERIALSOURCE;
			hr = D3DMATERIALCOLORSOURCE_FromString(str, &Value);
		}
		else if (MATCH(DIFFUSEMATERIALSOURCE))
		{
			State = D3DRS_DIFFUSEMATERIALSOURCE;
			hr = D3DMATERIALCOLORSOURCE_FromString(str, &Value);
		}
		else if (MATCH(AMBIENTMATERIALSOURCE))
		{
			State = D3DRS_AMBIENTMATERIALSOURCE;
			hr = D3DMATERIALCOLORSOURCE_FromString(str, &Value);
		}
		else if (MATCH(EMISSIVEMATERIALSOURCE))
		{
			State = D3DRS_EMISSIVEMATERIALSOURCE;
			hr = D3DMATERIALCOLORSOURCE_FromString(str, &Value);
		}
		else if (MATCH(BACKAMBIENT))
		{
			State = D3DRS_BACKAMBIENT;
			hr = D3DCOLOR_FromString(str, &Value);
		}
		else if (MATCH(AMBIENT))
		{
			State = D3DRS_AMBIENT;
			hr = D3DCOLOR_FromString(str, &Value);
		}
		else if (MATCH(POINTSIZE))
		{
			State = D3DRS_POINTSIZE;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(POINTSIZE_MIN))
		{
			State = D3DRS_POINTSIZE_MIN;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(POINTSPRITEENABLE))
		{
			State = D3DRS_POINTSPRITEENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(POINTSCALEENABLE))
		{
			State = D3DRS_POINTSCALEENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(POINTSCALE_A))
		{
			State = D3DRS_POINTSCALE_A;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(POINTSCALE_B))
		{
			State = D3DRS_POINTSCALE_B;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(POINTSCALE_C))
		{
			State = D3DRS_POINTSCALE_C;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(POINTSIZE_MAX))
		{
			State = D3DRS_POINTSIZE_MAX;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(PATCHEDGESTYLE))
		{
			State = D3DRS_PATCHEDGESTYLE;
			hr = D3DPATCHEDGESTYLE_FromString(str, &Value);
		}
		else if (MATCH(PATCHSEGMENTS))
		{
			State = D3DRS_PATCHSEGMENTS;
			hr = DWORD_FromString(str, &Value);
		}
		else if (MATCH(VERTEXBLEND))
		{
			State = D3DRS_VERTEXBLEND;
			hr = D3DVERTEXBLENDFLAGS_FromString(str, &Value);
		}
		else if (MATCH(FOGCOLOR))
		{
			State = D3DRS_FOGCOLOR;
			hr = D3DCOLOR_FromString(str, &Value);
		}
		else if (MATCH(FILLMODE))
		{
			State = D3DRS_FILLMODE;
			hr = D3DFILLMODE_FromString(str, &Value);
		}
		else if (MATCH(BACKFILLMODE))
		{
			State = D3DRS_BACKFILLMODE;
			hr = D3DFILLMODE_FromString(str, &Value);
		}
		else if (MATCH(TWOSIDEDLIGHTING))
		{
			State = D3DRS_TWOSIDEDLIGHTING;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(NORMALIZENORMALS))
		{
			State = D3DRS_NORMALIZENORMALS;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(ZENABLE))
		{
			State = D3DRS_ZENABLE;
			hr = D3DZBUFFERTYPE_FromString(str, &Value);
		}
		else if (MATCH(STENCILENABLE))
		{
			State = D3DRS_STENCILENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(STENCILFAIL))
		{
			State = D3DRS_STENCILFAIL;
			hr = D3DSTENCILOP_FromString(str, &Value);
		}
		else if (MATCH(FRONTFACE))
		{
			State = D3DRS_FRONTFACE;
			hr = D3DFRONT_FromString(str, &Value);
		}
		else if (MATCH(CULLMODE))
		{
			State = D3DRS_CULLMODE;
			hr = D3DCULL_FromString(str, &Value);
		}
		else if (MATCH(TEXTUREFACTOR))
		{
			State = D3DRS_TEXTUREFACTOR;
			hr = D3DCOLOR_FromString(str, &Value);
		}
		else if (MATCH(ZBIAS))
		{
			State = D3DRS_ZBIAS;
			hr = LONG_FromString(str, &Value);
		}
		else if (MATCH(LOGICOP))
		{
			State = D3DRS_LOGICOP;
			hr = D3DLOGICOP_FromString(str, &Value);
		}
		else if (MATCH(EDGEANTIALIAS))
		{
			State = D3DRS_EDGEANTIALIAS;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(MULTISAMPLEANTIALIAS))
		{
			State = D3DRS_MULTISAMPLEANTIALIAS;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(MULTISAMPLEMASK))
		{
			State = D3DRS_MULTISAMPLEMASK;
			hr = DWORD_FromString(str, &Value);
		}
/*		
		else if (MATCH(MULTISAMPLETYPE))
		{
			State = D3DRS_MULTISAMPLETYPE;
			hr = D3DMULTISAMPLE_TYPE_FromString(str, &Value);
		}
*/
		else if (MATCH(SHADOWFUNC))
		{
			State = D3DRS_SHADOWFUNC;
			hr = D3DCMPFUNC_FromString(str, &Value);
		}
		else if (MATCH(LINEWIDTH))
		{
			State = D3DRS_LINEWIDTH;
			hr = FLOAT_FromString(str, &Value);
		}
		else if (MATCH(DXT1NOISEENABLE))
		{
		    State = D3DRS_DXT1NOISEENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(YUVENABLE))
		{
			State = D3DRS_YUVENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(OCCLUSIONCULLENABLE))
		{
			State = D3DRS_OCCLUSIONCULLENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(STENCILCULLENABLE))
		{
			State = D3DRS_STENCILCULLENABLE;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(ROPZCMPALWAYSREAD))
		{
			State = D3DRS_ROPZCMPALWAYSREAD;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(ROPZREAD))
		{
			State = D3DRS_ROPZREAD;
			hr = TRUE_FromString(str, &Value);
		}
		else if (MATCH(DONOTCULLUNCOMPRESSED))
		{
			State = D3DRS_DONOTCULLUNCOMPRESSED;
			hr = TRUE_FromString(str, &Value);
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);

		// If the value-parsing failed, print an error and exit.
		if (FAILED(hr))
			return UnexpectedAttributeToken(pwchLocalName, cchLocalName, pwchValue, cchValue, E_FAIL);

		// Add the render state, value pair to the list.  The rRenderState
		// array is big enough to hold all the possible render state values,
		// and on the Xbox, the enumeration is sequential, so we use the 
		// state as an index.
		rRenderState[State].State = State;
		rRenderState[State].Value = Value;
#undef MATCH
	}

	// Count number of active render states
	UINT nRenderStateCount = 0;
	UINT i;
	for (i = 0; i < MAX_RENDER_STATE; i++)
	{
		if (rRenderState[i].State != -1)
			nRenderStateCount++;
	}
	
	// Copy render state values to variably-sized array
	pPass->RenderStateCount = nRenderStateCount;
	pPass->rRenderState = new RenderState [ nRenderStateCount ];
	if (pPass->rRenderState == NULL)
	{
		fatalError(m_pLocator, L"Memory allocation failed for render state array\n", E_FAIL);
		m_bErrorSuppress = true;
		return E_FAIL;
	}
	nRenderStateCount = 0;
	for (i = 0; i < MAX_RENDER_STATE; i++)
	{
		if (rRenderState[i].State != -1)
		{
			pPass->rRenderState[nRenderStateCount] = rRenderState[i];
			nRenderStateCount++;
		}
	}
	
	return S_OK;	// default return
}

HRESULT XBContent::EndRenderState()
{
	// Find maximum active texture stage
	Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
	Pass *pPass = &pEffect->rPass[pEffect->PassCount];
	for (UINT i = 0; i < MAX_TEXTURE_STAGE; i++)
	{
		TextureStage *pTextureStage = &pPass->rTextureStage[i];
		if (pTextureStage->TextureResource != 0
			|| pTextureStage->TextureStateCount != 0)
		{
			// set stage count to one beyond maximum active stage
			pPass->TextureStageCount = i + 1;
		}
	}
	PopContext();
	return S_OK;
}
	
HRESULT XBContent::BeginTextureState(ISAXAttributes *pAttributes)
{
	HRESULT hr;
	Effect *pEffect = (Effect *)m_rContextStack[m_iContext].m_pData;
	PushContext(TextureState_Context);
	m_rContextStack[m_iContext].m_pData = pEffect;	// keep effect pointer
	Pass *pPass = &pEffect->rPass[pEffect->PassCount];
	if (pPass->rTextureStage == NULL)
	{
		pPass->rTextureStage = new TextureStage [ MAX_TEXTURE_STAGE ];
		ZeroMemory(pPass->rTextureStage, sizeof(TextureStage) * MAX_TEXTURE_STAGE );
	}

	// Texture stage state to be filled in by attributes
	DWORD Stage = (DWORD)-1;
	TextureState rTextureState[ MAX_TEXTURE_STATE ];
	memset(rTextureState, -1, sizeof(rTextureState));
	DWORD dwTextureResourceIndex = 0;
	
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
		CHAR *str = CharString(pwchValue, cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(Stage))
		{
			// The Stage token is special and is processed separately from the rest of the attributes
			hr = DWORD_FromString(str, &Stage);
			if (FAILED(hr))
				return UnexpectedToken(pwchValue, cchValue, E_FAIL);
			const int buflen = 1000;
			WCHAR buf[buflen];
			if (Stage >= MAX_TEXTURE_STAGE)
			{
				_snwprintf(buf, buflen, L"Invalid texture stage %d >= %d\n", Stage, MAX_TEXTURE_STAGE);
				fatalError(m_pLocator, buf, E_FAIL);
				m_bErrorSuppress = true;
				return E_FAIL;
			}
			TextureStage *pTextureStage = &pPass->rTextureStage[Stage];
			if (pTextureStage->rTextureState != NULL)
			{
				_snwprintf(buf, buflen, L"<TextureState> with Stage=%d already defined\n", Stage);
				fatalError(m_pLocator, buf, E_FAIL);
				m_bErrorSuppress = true;
				return E_FAIL;
			}
		}
		else if (MATCH(idref))
		{
			if (strcmp(str, "NULL") == 0)	// NULL is special case
				dwTextureResourceIndex = 0;
			else
			{
				// The idref token is special, and selects the current texture.
				if (!m_pBundler->IsExistingIdentifier(str, &dwTextureResourceIndex))
				{
					m_pBundler->ErrorMsg("Unknown resource \"%s\"\n", str);
					return E_FAIL;
				}
				// TODO: make sure this is a texture resource
			}
		}
		else
		{
			if (MATCH(D3DTSS_))
			{
				// Strip off D3DTSS_ prefix
				pwchLocalName += 7;
				cchLocalName -= 7;
			}

			DWORD State = (DWORD)-1;
			DWORD Value;
			if (MATCH(COLOROP))
			{
				State = D3DTSS_COLOROP;
				hr = D3DTEXTUREOP_FromString(str, &Value);
			}
			else if (MATCH(COLORARG0))
			{
				State = D3DTSS_COLORARG0;
				hr = D3DTEXTUREARG_FromString(str, &Value);
			}
			else if (MATCH(COLORARG1))
			{
				State = D3DTSS_COLORARG1;
				hr = D3DTEXTUREARG_FromString(str, &Value);
			}
			else if (MATCH(COLORARG2))
			{
				State = D3DTSS_COLORARG2;
				hr = D3DTEXTUREARG_FromString(str, &Value);
			}
			else if (MATCH(ALPHAOP))
			{
				State = D3DTSS_ALPHAOP;
				hr = D3DTEXTUREOP_FromString(str, &Value);
			}
			else if (MATCH(ALPHAARG0))
			{
				State = D3DTSS_ALPHAARG0;
				hr = D3DTEXTUREARG_FromString(str, &Value);
			}
			else if (MATCH(ALPHAARG1))
			{
				State = D3DTSS_ALPHAARG1;
				hr = D3DTEXTUREARG_FromString(str, &Value);
			}
			else if (MATCH(ALPHAARG2))
			{
				State = D3DTSS_ALPHAARG2;
				hr = D3DTEXTUREARG_FromString(str, &Value);
			}
			else if (MATCH(RESULTARG))
			{
				State = D3DTSS_RESULTARG;
				hr = D3DTEXTUREARG_FromString(str, &Value);
			}
			else if (MATCH(TEXTURETRANSFORMFLAGS))
			{
				State = D3DTSS_TEXTURETRANSFORMFLAGS;
				hr = D3DTEXTURETRANSFORMFLAGS_FromString(str, &Value);
			}
			else if (MATCH(ADDRESSU))
			{
				State = D3DTSS_ADDRESSU;
				hr = D3DTEXTUREADDRESS_FromString(str, &Value);
			}
			else if (MATCH(ADDRESSV))
			{
				State = D3DTSS_ADDRESSV;
				hr = D3DTEXTUREADDRESS_FromString(str, &Value);
			}
			else if (MATCH(ADDRESSW))
			{
				State = D3DTSS_ADDRESSW;
				hr = D3DTEXTUREADDRESS_FromString(str, &Value);
			}
			else if (MATCH(MAGFILTER))
			{
				State = D3DTSS_MAGFILTER;
				hr = D3DTEXF_FromString(str, &Value);
				// TODO: validation of value
			}
			else if (MATCH(MINFILTER))
			{
				State = D3DTSS_MINFILTER;
				hr = D3DTEXF_FromString(str, &Value);
				// TODO: validation of value
			}
			else if (MATCH(MIPFILTER))
			{
				State = D3DTSS_MIPFILTER;
				hr = D3DTEXF_FromString(str, &Value);
				// TODO: validation of value
			}
			else if (MATCH(MIPMAPLODBIAS))
			{
				State = D3DTSS_MIPMAPLODBIAS;
				hr = FLOAT_FromString(str, &Value);
			}
			else if (MATCH(MAXMIPLEVELS))
			{
				State = D3DTSS_MAXMIPLEVEL;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(MAXANISOTROPY))
			{
				State = D3DTSS_MAXANISOTROPY;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH(COLORKEYOP))
			{
				State = D3DTSS_COLORKEYOP;
				hr = D3DTEXTURECOLORKEYOP_FromString(str, &Value);
			}
			else if (MATCH(COLORSIGN))
			{
				State = D3DTSS_COLORSIGN;
				hr = D3DTSIGN_FromString(str, &Value);
			}
			else if (MATCH( ALPHAKILL ))
			{
				State = D3DTSS_ALPHAKILL;
				hr = D3DTEXTUREALPHAKILL_FromString(str, &Value);
			}
			else if (MATCH( BUMPENVMAT00 ))
			{
				State = D3DTSS_BUMPENVMAT00;
				hr = FLOAT_FromString(str, &Value);
			}
			else if (MATCH( BUMPENVMAT01 ))
			{
				State = D3DTSS_BUMPENVMAT01;
				hr = FLOAT_FromString(str, &Value);
			}
			else if (MATCH( BUMPENVMAT11 ))
			{
				State = D3DTSS_BUMPENVMAT11;
				hr = FLOAT_FromString(str, &Value);
			}
			else if (MATCH( BUMPENVMAT10 ))
			{
				State = D3DTSS_BUMPENVMAT10;
				hr = FLOAT_FromString(str, &Value);
			}
			else if (MATCH( BUMPENVLSCALE ))
			{
				State = D3DTSS_BUMPENVLSCALE;
				hr = FLOAT_FromString(str, &Value);
			}
			else if (MATCH( BUMPENVLOFFSET ))
			{
				State = D3DTSS_BUMPENVLOFFSET;
				hr = FLOAT_FromString(str, &Value);
			}
			else if (MATCH( TEXCOORDINDEX ))
			{
				State = D3DTSS_TEXCOORDINDEX;
				hr = DWORD_FromString(str, &Value);
			}
			else if (MATCH( D3DTSS_BORDERCOLOR ))
			{
				State = D3DTSS_BORDERCOLOR;
				hr = D3DCOLOR_FromString(str, &Value);
			}
			else if (MATCH( COLORKEYCOLOR ))
			{
				State = D3DTSS_COLORKEYCOLOR;
				hr = D3DCOLOR_FromString(str, &Value);
			}
			else
				return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);

			// If the value-parsing failed, print an error and exit.
			if (FAILED(hr))
				return UnexpectedAttributeToken(pwchLocalName, cchLocalName, pwchValue, cchValue, E_FAIL);
			
			// Add the texture stage state, value pair to the list.  The rTextureState
			// array is big enough to hold all the possible texture stage state values,
			// and on the Xbox, the enumeration is sequential, so we use the 
			// state as an index.
			rTextureState[State].State = State;
			rTextureState[State].Value = Value;
		}
	}
	if (Stage == -1)
	{
		fatalError(m_pLocator, L"<TextureState> missing required \"Stage\" attribute\n", E_FAIL);
		m_bErrorSuppress = true;
		return E_FAIL;
	}
			
	// Count number of active texture states
	UINT nTextureStateCount = 0;
	UINT i;
	for (i = 0; i < MAX_TEXTURE_STATE; i++)
	{
		if (rTextureState[i].State != -1)
			nTextureStateCount++;
	}
	
	// Copy texture state values to variably-sized array
	TextureStage *pTextureStage = &pPass->rTextureStage[Stage];
	pTextureStage->TextureStateCount = nTextureStateCount;
	pTextureStage->rTextureState = new TextureState [ nTextureStateCount ];
	if (pTextureStage->rTextureState == NULL)
	{
		fatalError(m_pLocator, L"Memory allocation failed for texture state array\n", E_FAIL);
		m_bErrorSuppress = true;
		return E_FAIL;
	}
	nTextureStateCount = 0;
	for (i = 0; i < MAX_TEXTURE_STATE; i++)
	{
		if (rTextureState[i].State != -1)
		{
			pTextureStage->rTextureState[nTextureStateCount] = rTextureState[i];
			nTextureStateCount++;
		}
	}

	// Set texture resource index
	pTextureStage->TextureResource = dwTextureResourceIndex;

	return S_OK;
}

HRESULT XBContent::EndTextureState()
{
	PopContext();
	return S_OK;
}
	
