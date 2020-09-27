//-----------------------------------------------------------------------------
//  
//  File: XBContent.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
//  Implementation of the XBContent class.
//  
//-----------------------------------------------------------------------------
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//

XBContent::XBContent()
{
    m_pBundler = NULL;
    m_iContext = 0;
    m_rContextStack[0].m_Context = Root_Context;
    m_rContextStack[0].m_pData = NULL;
    m_pLocator = NULL;
    m_bErrorSuppress = false;
    m_strTemp = NULL;
    m_cchTemp = 0;
    m_pAnimList = NULL;
    m_pAnimVSPList = NULL;
    m_IncludeCount = 0;
    ZeroMemory(&m_Count, sizeof(m_Count));
}

XBContent::~XBContent()
{
    if (m_pBundler)
        delete m_pBundler;
    if (m_pLocator)
        m_pLocator->Release();
    if (m_strTemp != NULL)
        delete m_strTemp;
}

//////////////////////////////////////////////////////////////////////
// Helper for printing out an error message for an unexpected token 
//
HRESULT XBContent::UnexpectedToken(WCHAR *pwchChars, int cchChars, HRESULT errCode)
{
    const int buflen = 1000;
    WCHAR buf[buflen];
    const int tokenlen = 900;
    WCHAR token[tokenlen];
    if (cchChars >= tokenlen) cchChars = tokenlen-1;
    wcsncpy(token, pwchChars, cchChars);
    token[cchChars] = 0;
    _snwprintf(buf, buflen, L"Unexpected token \"%s\"\n", token);
    fatalError(m_pLocator, buf, errCode);
    m_bErrorSuppress = true;
    return errCode;
}

//////////////////////////////////////////////////////////////////////
// Helper for printing out an error message for an unexpected attribute token 
//
HRESULT XBContent::UnexpectedAttributeToken(WCHAR *pwchLocalName, int cchLocalName,
                                            WCHAR *pwchValue, int cchValue,
                                            HRESULT errCode)
{
    const int buflen = 1000;
    WCHAR buf[buflen];
    const int namelen = 400;
    WCHAR name[namelen];
    const int valuelen = 400;
    WCHAR value[valuelen];
    if (cchLocalName >= namelen) cchLocalName = namelen-1;
    wcsncpy(name, pwchLocalName, cchLocalName);
    name[cchLocalName] = 0;
    if (cchValue >= valuelen) cchValue = valuelen-1;
    wcsncpy(value, pwchValue, cchValue);
    value[cchValue] = 0;
    _snwprintf(buf, buflen, L"Attribute \"%s\" got unexpected token \"%s\"\n", name, value);
    fatalError(m_pLocator, buf, errCode);
    m_bErrorSuppress = true;
    return errCode;
}

//////////////////////////////////////////////////////////////////////
// Helper for making sure that a range of characters are whitespace.
//
HRESULT XBContent::MustBeWhitespace(WCHAR *pwchChars, int cchChars)
{
    for (int i = 0; i < cchChars; i++)
    {
        if (!iswspace(pwchChars[i]))
        {
            // update string pointer and length
            pwchChars += i;
            cchChars -= i;
            const int buflen = 200;
            WCHAR buf[buflen];
            const int tokenlen = 10;
            WCHAR token[tokenlen];
            if (cchChars >= tokenlen) cchChars = tokenlen - 1;
            wcsncpy(token, pwchChars, cchChars);
            token[cchChars] = 0;
            _snwprintf(buf, buflen, L"Non-whitespace character string \"%s\" found in unexpected location.\n", token);
            fatalError(m_pLocator, buf, E_FAIL);
            m_bErrorSuppress = true;
            return E_FAIL;
        }
    }
    return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Convert a wide-character string of characters to a null-terminated
// CHAR string. Returns a pointer to a static area.
//
CHAR *XBContent::CharString(WCHAR *pwch, int cch, int *pcchOut)
{
    int cchOut = wcstombs(NULL, pwch, cch);
    if (cchOut >= m_cchTemp)
    {
        if (m_strTemp)
            delete m_strTemp;
        m_cchTemp = cchOut + 1;
        m_strTemp = new CHAR [ m_cchTemp ];
    }
    cchOut = wcstombs(m_strTemp, pwch, cch);
    m_strTemp[cchOut] = 0;  // make sure it's NULL-terminated
    if (pcchOut) *pcchOut = cchOut;
    return m_strTemp;
}

//////////////////////////////////////////////////////////////////////
// include Handler
//
HRESULT XBContent::Begin_include(ISAXAttributes *pAttributes)
{
    m_IncludeCount++;   // include recursion count
#define MAX_INCLUDE_COUNT 100   
    if (m_IncludeCount > MAX_INCLUDE_COUNT) // Catch include loops
        return fatalError(m_pLocator, L"Too many included files.\n", E_FAIL);
    
    const int len = 10000;
    WCHAR URL[len];
    bool bFoundHREF = false;
    
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
        if (MATCH(href))
        {
            if (cchValue >= len)
                cchValue = len - 1;
            wcsncpy( URL, pwchValue, cchValue );
            URL[cchValue] = 0;  // make sure string is null-terminated
            bFoundHREF = true;
        }
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
    }
    
    if (!bFoundHREF)
        return fatalError(m_pLocator, L"<include> requires \"href\" attribute.\n", E_FAIL);

    // Keep URL for processing 
    WCHAR *strURL = new WCHAR [ wcslen(URL) + 1 ];
    if (strURL == NULL)
        return E_OUTOFMEMORY;
    wcscpy(strURL, URL);
    PushContext(include_Context);
    m_rContextStack[m_iContext].m_pData = strURL;

    return S_OK;
}

HRESULT XBContent::End_include()
{
    HRESULT hr;
    WCHAR *strURL = (WCHAR *)m_rContextStack[m_iContext].m_pData;
    PopContext();
    
    // Start the recursive parsing
    ISAXXMLReader* pRdr = NULL;
    hr = CoCreateInstance( __uuidof(SAXXMLReader), NULL, CLSCTX_ALL, __uuidof(ISAXXMLReader), (void **)&pRdr);
    if(FAILED(hr))
        return fatalError(m_pLocator, L"<include> unable to allocate new reader\n", hr);

    // Set the XML parser to use our callback routines on the tokens
    hr = pRdr->putContentHandler(this); // the XBContent class supports both the content and error handling
    hr = pRdr->putErrorHandler(this);

    // Save the current locator
    ISAXLocator *pLocatorSave = m_pLocator;
    if (pLocatorSave != NULL)
        pLocatorSave->AddRef();

    // Parse the input URL
    hr = pRdr->parseURL(strURL);
	
    // Cleanup
    pRdr->Release();

    // Restore locator
    this->putDocumentLocator(pLocatorSave);
    if (pLocatorSave)
        pLocatorSave->Release();

	// Print message on error
    if (FAILED(hr))
    {
		const int urlmaxlen = 200;
		const int errlen = urlmaxlen + 50;
		wchar_t err[errlen];
		
        if (hr == E_ACCESSDENIED)
            _snwprintf(err, errlen, L"<include> access denied to \"%.*s\".\n", urlmaxlen, strURL);
        else if (hr == INET_E_OBJECT_NOT_FOUND)
            _snwprintf(err, errlen, L"<include> object \"%.*s\" not found.\n", urlmaxlen, strURL);
        else if (hr == INET_E_DOWNLOAD_FAILURE)
            _snwprintf(err, errlen, L"<include> download failure for \"%.*s\".\n", urlmaxlen, strURL);
        else
            _snwprintf(err, errlen, L"<include> parsing document \"%.*s\" failed with error %08x\n", urlmaxlen, strURL, hr);
		
        m_bErrorSuppress = false;    // make sure include error is printed, too.
        return fatalError(m_pLocator, err, hr);
    }
    
    m_IncludeCount--;
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CubeTexture Handler
//
HRESULT XBContent::BeginCubeTexture(ISAXAttributes *pAttributes)
{
    PushContext(CubeTexture_Context);
    return S_OK;
}

HRESULT XBContent::EndCubeTexture()
{
    PopContext();
    return S_OK;
}

    

    
//////////////////////////////////////////////////////////////////////
// IndexBuffer Handler
//
HRESULT XBContent::BeginIndexBuffer(ISAXAttributes *pAttributes)
{
    PushContext(IndexBuffer_Context);
    CIndexBuffer *pIB = new CIndexBuffer(m_pBundler);
    if (!pIB) return E_OUTOFMEMORY;
    m_rContextStack[m_iContext].m_pData = pIB;

    // Set default resource id
    m_Count.m_IndexBuffer++;
    CHAR *strIdentifier = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier;
	ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
    _snprintf(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH, "IndexBuffer%d", m_Count.m_IndexBuffer);
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
            strIdentifier[cchValue] = 0;    // make sure the string is NULL terminated
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
    
    // Defer the save of the index buffer to the bundler until we
    // process all the indices.
    return S_OK;
}

HRESULT XBContent::IndexBufferChars(WCHAR *pwchChars, int cchChars)
{
    CIndexBuffer *pIB = (CIndexBuffer *)m_rContextStack[m_iContext].m_pData;
    if (pIB == NULL) return E_INVALIDARG;
    CHAR *strFull = CharString(pwchChars, cchChars);
    CHAR *strSeparators = " \t\n";
    CHAR *strBuff = strtok(strFull, strSeparators);
    CHAR *pEnd;
    while (strBuff != NULL)
    {
        if (m_pBundler->ValidateType(strBuff, TT_HEXNUMBER) == S_OK
            || m_pBundler->ValidateType(strBuff, TT_INTEGER) == S_OK)
        {
            pIB->AddIndex((WORD)strtoul(strBuff, &pEnd, 0));
            if (*pEnd != '\0')
            {
                m_pBundler->ErrorMsg( "Extra chars \"%s\" on token \"%s\"\n", pEnd, strBuff);
                return E_FAIL;
            }
        }
        else
        {
            m_pBundler->ErrorMsg( "Unrecognized index value \"%s\"\n", strBuff );
            return E_FAIL;
        }

        // Get next token
        strBuff = strtok(NULL, strSeparators);
    }
    return S_OK;
}

HRESULT XBContent::EndIndexBuffer()
{
    CIndexBuffer *pIB = (CIndexBuffer *)m_rContextStack[m_iContext].m_pData;
    if (pIB == NULL) return E_INVALIDARG;
    
    // TODO: save compiled push buffer instead
    
    // Get current offset
    m_pBundler->m_aResources[m_pBundler->m_nResourceCount].dwOffset = m_pBundler->m_cbHeader;

    // Save the IndexBuffer to the bundle
    DWORD cbHeader = 0, cbData = 0;
    HRESULT hr = pIB->SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
        return hr;

    // Success, so increment the resource count
    printf("IB:  Wrote %s: %d indices (%d bytes, including header)\n",
           m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier, pIB->m_IndexCount, cbHeader);
    m_pBundler->m_nResourceCount++;

    // cleanup
    delete pIB;
    PopContext();
    return S_OK;
}

    
//////////////////////////////////////////////////////////////////////
// Surface Handler
//
HRESULT XBContent::BeginSurface(ISAXAttributes *pAttributes)
{
    PushContext(Surface_Context);
    return E_NOTIMPL;
}

HRESULT XBContent::EndSurface()
{
    PopContext();
    return E_NOTIMPL;
}

    
//////////////////////////////////////////////////////////////////////
// Texture Handler
//
HRESULT XBContent::BeginTexture(ISAXAttributes *pAttributes)
{
    PushContext(Texture_Context);
    CTexture2DFill *ptex = new CTexture2DFill(m_pBundler);
    if (!ptex) return E_OUTOFMEMORY;
    m_rContextStack[m_iContext].m_pData = ptex;
        
    // Set default resource id
    m_Count.m_Texture++;
    CHAR *strIdentifier = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier;
	ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
    _snprintf(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH, "Texture%d", m_Count.m_Texture);
    strIdentifier[BUNDLER_IDENTIFIER_MAX_LENGTH - 1] = 0;   // make sure the string is NULL terminated
    
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
            strIdentifier[cchValue] = 0;    // make sure the string is NULL terminated
        }
        else if (MATCH(source))
        {
            strcpy( ptex->m_strSource, CharString(pwchValue, cchValue));
        }
        else if (MATCH(alphasource))
        {
            strcpy( ptex->m_strAlphaSource, CharString(pwchValue, cchValue));
        }
        else if (MATCH(filter))
        {
            ptex->m_dwFilter = m_pBundler->FilterFromString(CharString(pwchValue, cchValue));
        }
        else if (MATCH(format))
        {
            strcpy( ptex->m_strFormat, CharString(pwchValue, cchValue));
        }
        else if (MATCH(width))
        {
            ptex->m_dwWidth = atoi(CharString(pwchValue, cchValue));
        }
        else if (MATCH(height))
        {
            ptex->m_dwHeight = atoi(CharString(pwchValue, cchValue));
        }
        else if (MATCH(levels))
        {
            ptex->m_dwLevels = atoi(CharString(pwchValue, cchValue));
        }
        else if (MATCH(colorkey))
        {
            CHAR *pString = CharString(pwchValue, cchValue);
            CHAR *pEnd;
            ptex->m_ColorKey = strtoul(pString, &pEnd, 0);
        }
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
    }
    
    if ( m_pBundler->IsExistingIdentifier( strIdentifier ) )
    {
        m_pBundler->ErrorMsg( "Second use of identifier \"%s\"", strIdentifier );
        return E_FAIL;
    }
    
    // Defer the save of the texture to the bundler until we
    // process all the Surface elements.  This allows individual
    // mipmap levels to be specified.
    return S_OK;
}

HRESULT XBContent::TextureChars(WCHAR *pwchChars, int cchChars)
{
    CTexture2DFill *pTexture = (CTexture2DFill *)m_rContextStack[m_iContext].m_pData;
    if (pTexture == NULL) return E_INVALIDARG;

    // TODO: Make sure texture does not have a source or alphasource set

    CHAR *strFull = CharString(pwchChars, cchChars);
    CHAR *strSeparators = " \t\n";
    CHAR *strBuff = strtok(strFull, strSeparators);
    CHAR *pEnd;
    while (strBuff != NULL)
    {
        if (m_pBundler->ValidateType(strBuff, TT_HEXNUMBER) == S_OK
            || m_pBundler->ValidateType(strBuff, TT_INTEGER) == S_OK)
        {
            HRESULT hr = pTexture->SetNextTexel((DWORD)strtoul(strBuff, &pEnd, 0));
            if (FAILED(hr))
                return E_FAIL;
            if (*pEnd != '\0')
            {
                m_pBundler->ErrorMsg( "Extra chars \"%s\" on token \"%s\"\n", pEnd, strBuff);
                return E_FAIL;
            }
        }
        else
        {
            m_pBundler->ErrorMsg( "Unrecognized index value \"%s\"\n", strBuff );
            return E_FAIL;
        }

        // Get next token
        strBuff = strtok(NULL, strSeparators);
    }
    return S_OK;
}

HRESULT XBContent::EndTexture()
{
    CTexture2DFill *ptex = (CTexture2DFill *)m_rContextStack[m_iContext].m_pData;
    if (ptex == NULL) return E_INVALIDARG;
    
    // Save the texture to the bundle
    DWORD cbHeader, cbData;
    HRESULT hr = ptex->SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
        return hr;

    printf("Texture: Wrote %s out in format %s (%d bytes)\n", 
           m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier, ptex->m_strFormat, cbData );
    printf("     %d x %d, %d levels\n", ptex->m_dwWidth, ptex->m_dwHeight, ptex->m_dwLevels);

    m_pBundler->m_aResources[m_pBundler->m_nResourceCount++].dwOffset = m_pBundler->m_cbHeader;
    m_pBundler->m_cbHeader += cbHeader;
    //m_pBundler->m_cbData += cbData;

    delete ptex; // Cleanup
    PopContext();
    return S_OK;
}

    
//////////////////////////////////////////////////////////////////////
// Vertex Handler
//
HRESULT XBContent::BeginVertex(ISAXAttributes *pAttributes)
{
    if (m_rContextStack[m_iContext].m_Context != VertexBuffer_Context)
    {
        m_pBundler->ErrorMsg("<v> tag must appear within an enclosing <VertexBuffer>\n");
        return E_FAIL;
    }
    PushContext(Vertex_Context);
    // use same pointer to vertex buffer as enclosing VertexBuffer
    m_rContextStack[m_iContext].m_pData = m_rContextStack[m_iContext-1].m_pData;

    // TODO: check for index attribute and verify that we're writing to the
    // correct index
    
    return S_OK;
}

HRESULT XBContent::VertexChars(WCHAR *pwchChars, int cchChars)
{
    CVertexBuffer *pVB = (CVertexBuffer *)m_rContextStack[m_iContext].m_pData;
    if (pVB == NULL) return E_INVALIDARG;
    CHAR *strFull = CharString(pwchChars, cchChars);
    CHAR *strSeparators = " \t\n";
    CHAR *strBuff = strtok(strFull, strSeparators);
    CHAR *pEnd;
    while (strBuff != NULL)
    {
        if (m_pBundler->ValidateType(strBuff, TT_HEXNUMBER) == S_OK)
        {
            // TODO: add in_format and out_format and make sure the in_format is correct here.
            // For now, we special case hex numbers as the way D3DCOLOR values are entered as packed
            DWORD value = strtoul(strBuff, &pEnd, 16);

            // TODO: check format string for proper type of input
            D3DXCOLOR color(value); // unpack the hex value
            pVB->AddVertexData((DOUBLE)color.r);
            pVB->AddVertexData((DOUBLE)color.g);
            pVB->AddVertexData((DOUBLE)color.b);
            pVB->AddVertexData((DOUBLE)color.a);

            // pVB->AddVertexData((DOUBLE)strtoul(strBuff, &pEnd, 16));
        }
        else if (m_pBundler->ValidateType(strBuff, TT_INTEGER) == S_OK)
        {
            FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
            if (*pEnd != '\0')
            {
                m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
                // leave as just a warning
            }
            pVB->AddVertexData(fValue);
        }
        else if (m_pBundler->ValidateType(strBuff, TT_FLOAT) == S_OK)
        {
            FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
            if (*pEnd != '\0')
            {
                m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
                // leave as just a warning
            }
            pVB->AddVertexData(fValue);
        }
        else
        {
            m_pBundler->ErrorMsg( "Unrecognized vertex value \"%s\"\n", strBuff );
            return E_FAIL;
        }

        // Get next token
        strBuff = strtok(NULL, strSeparators);
    }
    return S_OK;
}

HRESULT XBContent::EndVertex()
{
    // TODO: make sure we have enough data for this vertex before
    // going on to the next one.  Pad if not?
    m_rContextStack[m_iContext].m_pData = NULL;
    PopContext();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// VertexBuffer Handler
//
HRESULT XBContent::BeginVertexBuffer(ISAXAttributes *pAttributes)
{
    PushContext(VertexBuffer_Context);
    CVertexBuffer *pVB = new CVertexBuffer(m_pBundler);
    if (!pVB) return E_OUTOFMEMORY;
    m_rContextStack[m_iContext].m_pData = pVB;

    // Set default resource id
    m_Count.m_VertexBuffer++;
    CHAR *strIdentifier = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier;
	ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
    _snprintf(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH, "VertexBuffer%d", m_Count.m_VertexBuffer);
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
            strIdentifier[cchValue] = 0;    // make sure the string is NULL terminated
        }
        // TODO: add format_in and format_out so that data can be read one way
        // and written another way.  To make this easier, move the format
        // string parsing somewhere else.
        else if (MATCH(format))
        {
            CHAR *strFull = CharString(pwchValue, cchValue);
            CHAR *strSeparators = " \t\n";
            CHAR *strBuff = strtok(strFull, strSeparators);
            while (strBuff != NULL)
            {
                DWORD VertexFormat = CVertexBuffer::VertexFormatFromString( strBuff );
                if (VertexFormat != -1)
                    pVB->AddVertexFormat(VertexFormat);
                else
                {
                    m_pBundler->ErrorMsg( "Unrecognized vertex format: %s\n", strBuff );
                    return E_FAIL;
                }

                // Get next token
                strBuff = strtok(NULL, strSeparators);
            }
        }
        else if (MATCH(names))
        {
            // TODO: names will be a way to map directly from VB's to VS's, without
            // all that stream(0) stream(1) nonsense.
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
    // process all the Vertex elements.
    return S_OK;
}

HRESULT XBContent::EndVertexBuffer()
{
    CVertexBuffer *pVB = (CVertexBuffer *)m_rContextStack[m_iContext].m_pData;
    if (pVB == NULL) return E_INVALIDARG;
    
    // Save the vertexbuffer to the bundle
    DWORD cbHeader = 0, cbData = 0;
    HRESULT hr = pVB->SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
        return hr;
    printf("VB:  Wrote %s: %d bytes per vertex, %d vertices (%d bytes)\n", 
           m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier, pVB->m_cBytesPerVertex,
           pVB->m_cVertices, cbData );
    m_pBundler->m_aResources[m_pBundler->m_nResourceCount++].dwOffset = m_pBundler->m_cbHeader;
    m_pBundler->m_cbHeader += cbHeader;
    //m_pBundler->m_cbData += cbData;

    // cleanup
    delete pVB;
    PopContext();
    return S_OK;
}

    
//////////////////////////////////////////////////////////////////////
// XDX Handler
//
HRESULT XBContent::BeginXDX(ISAXAttributes *pAttributes)
{
    HRESULT hr;
    hr = PushContext(XDX_Context);
    if (FAILED(hr))
        return hr;

    // Skip most attribute parsing on included files.
    // TODO: validate version attribute
    if (m_IncludeCount > 0)
        return S_OK;
    
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
        if (MATCH(version))
        {
            // TODO: version validation, reading of old version files, etc.
        }
        else if (MATCH(out_packedresource))
        {
            hr = m_pBundler->SetOutPackedResourceFile( CharString(pwchValue, cchValue) );
            if (FAILED(hr))
                return hr;
        }
        else if (MATCH(out_header))
        {
            hr = m_pBundler->SetOutHeaderFile( CharString(pwchValue, cchValue) );
            if (FAILED(hr))
                return hr;
        }
        else if (MATCH(out_error))
        {
            hr = m_pBundler->SetOutErrorFile( CharString(pwchValue, cchValue) );
            if (FAILED(hr))
                return hr;
        }
        else if (MATCH(out_prefix))
        {
            hr = m_pBundler->SetOutPrefix( CharString(pwchValue, cchValue) );
            if (FAILED(hr))
                return hr;
        }
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
    }
    return S_OK;
}

HRESULT XBContent::EndXDX()
{
    PopContext();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// ISAXContentHandler interface
//

HRESULT STDMETHODCALLTYPE XBContent::putDocumentLocator( 
            /* [in] */ ISAXLocator __RPC_FAR *pLocator
            )
{
    ISAXLocator *pLocatorOld = m_pLocator;
    m_pLocator = pLocator;
    if (m_pLocator)
        m_pLocator->AddRef();
    if (pLocatorOld)
        pLocatorOld->Release();
    return S_OK;
}
        
HRESULT STDMETHODCALLTYPE XBContent::startDocument()
{
    if (m_IncludeCount > 0)
        return S_OK;
    if (m_pBundler)
        delete m_pBundler;  // cleanup from last time
    m_pBundler = new CBundler;
    if (m_pBundler == NULL)
        return E_OUTOFMEMORY;

    // Get name from locator
    WCHAR *wstrName;
    HRESULT hr = m_pLocator->getPublicId(&wstrName);
    if (FAILED(hr))
        return hr;
    if (wstrName == NULL 
		|| *wstrName == 0)
    {
        hr = m_pLocator->getSystemId(&wstrName);
        if (FAILED(hr))
            return hr;
    }

    // TODO: make bundler use WCHAR paths
    // Convert wide chars to chars
    int cchOut = wcstombs(NULL, wstrName, _MAX_PATH);
    if (cchOut >= _MAX_PATH)
        return E_FAIL;
    CHAR file[_MAX_PATH];
    cchOut = wcstombs(file, wstrName, _MAX_PATH);
    file[cchOut] = 0;   // make sure it's NULL-terminated

    // Skip over prefix
    CHAR *strFileNoPrefix = file;
    CHAR *strPrefix = "file:///";
    int lenPrefix = strlen(strPrefix);
    if (strncmp(file, strPrefix, lenPrefix) == 0)
	{
        strFileNoPrefix += lenPrefix;

		// If it's a file, replace special URL chars, such as %20
		CHAR *pszSrc = strFileNoPrefix;
		CHAR *pszDst = strFileNoPrefix;
		while (*pszSrc)
		{
			// TODO: add other special chars
			if (*pszSrc == '%'
				&& *(pszSrc + 1) == '2'
				&& *(pszSrc + 2) == '0')
			{
				*pszDst = ' ';
				pszSrc += 3;	// skip %20
				pszDst++;
			}
			else
				*pszDst++ = *pszSrc++;
		}
		*pszDst = 0;
	}
    
    // Strip suffix from name
    CHAR path[_MAX_PATH];
    CHAR drive[_MAX_DRIVE];
    CHAR dir[_MAX_DIR];
    CHAR name[_MAX_PATH];
    CHAR ext[_MAX_EXT];
    _splitpath( strFileNoPrefix, drive, dir, name, ext );
    _makepath( path, drive, dir, name, ""); // .xbr, .h, and .err will be appended by bundler
    
    m_pBundler->Begin(path);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE XBContent::endDocument()
{
    if (m_IncludeCount > 0)
        return S_OK;
    HRESULT hr = m_pBundler->End();
    if (FAILED(hr))
        return hr;
    delete m_pBundler;
    m_pBundler = NULL;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE XBContent::startPrefixMapping( 
            /* [in] */ wchar_t __RPC_FAR *pwchPrefix,
            /* [in] */ int cchPrefix,
            /* [in] */ wchar_t __RPC_FAR *pwchUri,
            /* [in] */ int cchUri)
{
    return S_OK;
}
        
        
HRESULT STDMETHODCALLTYPE XBContent::endPrefixMapping( 
            /* [in] */ wchar_t __RPC_FAR *pwchPrefix,
            /* [in] */ int cchPrefix)
{
    return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Route element event based on current context
// 
HRESULT STDMETHODCALLTYPE XBContent::startElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName,
            /* [in] */ ISAXAttributes __RPC_FAR *pAttributes)
{
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
    switch (m_rContextStack[m_iContext].m_Context) {
    case Root_Context:
    {
        if (MATCH(XDX))
            return BeginXDX(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case XDX_Context:
    {
        if (MATCH(XDX))
        {
            // XDX context OK for included files only
            if (m_IncludeCount > 0)
                return BeginXDX(pAttributes);
            else
                return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
        }
        else if (MATCH(include))
            return Begin_include(pAttributes);
        else if (MATCH(Texture))
            return BeginTexture(pAttributes);
        else if (MATCH(CubeTexture)
                 || MATCH(CubeMap))
            return BeginCubeTexture(pAttributes);
        else if (MATCH(VertexBuffer))
            return BeginVertexBuffer(pAttributes);
        else if (MATCH(VertexShader))
            return BeginVertexShader(pAttributes);
        else if (MATCH(PixelShader))
            return BeginPixelShader(pAttributes);
        else if (MATCH(IndexBuffer))
            return BeginIndexBuffer(pAttributes);
        else if (MATCH(Surface))
            return BeginSurface(pAttributes);
        else if (MATCH(Frame))
            return BeginFrame(pAttributes);
        
    // TODO: use <Model> for <Effect>'s with bound geometry and
    // <Material> for <Effect>'s with unbound geometry
        else if (MATCH(Effect) || MATCH(Model) || MATCH(Material))
            return BeginEffect(pAttributes);
        
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case include_Context:
    {
        // Include element can have no child elements
        fatalError(m_pLocator, L"<include> must not have child elements.\n", E_FAIL);
        m_bErrorSuppress = true;
        return E_FAIL;
    }
    case Frame_Context:
    {
        if (MATCH(Texture))
            return BeginTexture(pAttributes);
        else if (MATCH(CubeTexture)
                 || MATCH(CubeMap))
            return BeginCubeTexture(pAttributes);
        else if (MATCH(VertexBuffer))
            return BeginVertexBuffer(pAttributes);
        else if (MATCH(VertexShader))
            return BeginVertexShader(pAttributes);
        else if (MATCH(PixelShader))
            return BeginPixelShader(pAttributes);
        else if (MATCH(IndexBuffer))
            return BeginIndexBuffer(pAttributes);
        else if (MATCH(Surface))
            return BeginSurface(pAttributes);
        else if (MATCH(Matrix))
            return BeginMatrix(pAttributes);
        else if (MATCH(Frame))
            return BeginFrame(pAttributes);

    // TODO: see note above
        else if (MATCH(Effect) || MATCH(Model) || MATCH(Material))
            return BeginEffect(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case Texture_Context:
    {
        if (MATCH(Surface))
            return BeginSurface(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case CubeTexture_Context:
    {
        if (MATCH(Surface))
            return BeginSurface(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VolumeTexture_Context:
    {
        if (MATCH(Surface))
            return BeginSurface(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexBuffer_Context:
    {
        if (MATCH(v))
            return BeginVertex(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexShader_Context:
    {
        if (MATCH(decl))
            return BeginVertexShader_decl(pAttributes);
        else if (MATCH(asm))
            return BeginVertexShader_asm(pAttributes);
        else if (MATCH(Constant))
            return BeginVertexShader_Constant(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexShader_decl_Context:
    {
        if (MATCH(stream))
            return BeginVertexShader_decl_stream(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexShader_decl_stream_Context:
    {
        if (MATCH(vreg))
            return BeginVertexShader_decl_stream_vreg(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case PixelShader_Context:
    {
        if (MATCH(asm))
            return BeginPixelShader_asm(pAttributes);
        else if (MATCH(Constant))
            return BeginPixelShader_Constant(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case Effect_Context:
    {
        if (MATCH(Pass))
            return BeginPass(pAttributes);
        if (MATCH(RenderState))
            return BeginRenderState(pAttributes);
        // Should the following be forced to be outside of the <Effect> scope?
        else if (MATCH(Texture))
            return BeginTexture(pAttributes);
        else if (MATCH(VertexBuffer))
            return BeginVertexBuffer(pAttributes);
        else if (MATCH(IndexBuffer))
            return BeginIndexBuffer(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case Pass_Context:
    {
        if (MATCH(RenderState))
            return BeginRenderState(pAttributes);
        else if (MATCH(VertexShader))
            return BeginVertexShader(pAttributes);
        else if (MATCH(PixelShader))
            return BeginPixelShader(pAttributes);
        else if (MATCH(Draw))
            return BeginDraw(pAttributes);
        // Should the following be forced to be outside of the <Pass> scope?
        else if (MATCH(Texture))
            return BeginTexture(pAttributes);
        else if (MATCH(VertexBuffer))
            return BeginVertexBuffer(pAttributes);
        else if (MATCH(IndexBuffer))
            return BeginIndexBuffer(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case RenderState_Context:
    {
        if (MATCH(TextureState))
            return BeginTextureState(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case Matrix_Context:
    {
        if (MATCH(animate))
            return BeginMatrix_animate(pAttributes);
        else if (MATCH(Matrix))
            return BeginMatrix(pAttributes);    // general matrix multiply within matrix block
        else if (MATCH(Rotate))
            return BeginRotate(pAttributes);
        else if (MATCH(Scale))
            return BeginScale(pAttributes);
        else if (MATCH(Translate))
            return BeginTranslate(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case Rotate_Context:
    {
        if (MATCH(animate))
            return BeginRotate_animate(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case Scale_Context:
    {
        if (MATCH(animate))
            return BeginScale_animate(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case Translate_Context:
    {
        if (MATCH(animate))
            return BeginTranslate_animate(pAttributes);
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    default:
        return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
#undef MATCH
}


HRESULT STDMETHODCALLTYPE XBContent::endElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName)
{
    // Close current context.
    
    // Since the parser checks to make sure that the end matches the
    // begin, we don't have to check again here.  We do anyway just
    // for a sanity check.
    
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
    switch (m_rContextStack[m_iContext].m_Context)
    {
    case Root_Context:
        break; // unexpected
    case XDX_Context:
        if (MATCH(XDX))
            return EndXDX();
        break;
    case include_Context:
        if (MATCH(include))
        {
            if (m_IncludeCount > 0) // sanity check
                return End_include();
        }
        break;
    case CubeTexture_Context:
        if (MATCH(CubeTexture)
            || MATCH(CubeMap))
            return EndCubeTexture();
        break;
    case VertexBuffer_Context:
        if (MATCH(VertexBuffer))
            return EndVertexBuffer();
        break;
    case IndexBuffer_Context:
        if (MATCH(IndexBuffer))
            return EndIndexBuffer();
        break;
    case Surface_Context:
        if (MATCH(Surface))
            return EndSurface();
        break;
    case Texture_Context:
        if (MATCH(Texture))
            return EndTexture();
        break;
    case Vertex_Context:
        if (MATCH(v))
            return EndVertex();
        break;
    case VertexShader_Context:
    {
        if (MATCH(VertexShader))
            return EndVertexShader();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexShader_decl_Context:
    {
        if (MATCH(decl))
            return EndVertexShader_decl();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexShader_decl_stream_Context:
    {
        if (MATCH(stream))
            return EndVertexShader_decl_stream();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexShader_decl_stream_vreg_Context:
    {
        if (MATCH(vreg))
            return EndVertexShader_decl_stream_vreg();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexShader_asm_Context:
    {
        if (MATCH(asm))
            return EndVertexShader_asm();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case VertexShader_Constant_Context:
    {
        if (MATCH(Constant))
            return EndVertexShader_Constant();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case PixelShader_Context:
    {
        if (MATCH(PixelShader))
            return EndPixelShader();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case PixelShader_asm_Context:
    {
        if (MATCH(asm))
            return EndPixelShader_asm();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case PixelShader_Constant_Context:
    {
        if (MATCH(Constant))
            return EndPixelShader_Constant();
        else
            return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
    }
    case Frame_Context:
        if (MATCH(Frame))
            return EndFrame();
        break;
    case Matrix_Context:
        if (MATCH(Matrix))
            return EndMatrix();
        break;
    case Matrix_animate_Context:
        if (MATCH(animate))
            return EndMatrix_animate();
        break;
    case Rotate_Context:
        if (MATCH(Rotate))
            return EndRotate();
        break;
    case Rotate_animate_Context:
        if (MATCH(animate))
            return EndRotate_animate();
        break;
    case Scale_Context:
        if (MATCH(Scale))
            return EndScale();
        break;
    case Scale_animate_Context:
        if (MATCH(animate))
            return EndScale_animate();
        break;
    case Translate_Context:
        if (MATCH(Translate))
            return EndTranslate();
        break;
    case Translate_animate_Context:
        if (MATCH(animate))
            return EndTranslate_animate();
        break;
    case Effect_Context:
        // TODO: see <Effect> note above
        if (MATCH(Effect) || MATCH(Model) || MATCH(Material))
            return EndEffect();
        break;
    case Pass_Context:
        if (MATCH(Pass))
            return EndPass();
        break;
    case RenderState_Context:
        if (MATCH(RenderState))
            return EndRenderState();
        break;
    case TextureState_Context:
        if (MATCH(TextureState))
            return EndTextureState();
        break;
    case Draw_Context:
        if (MATCH(Draw))
            return EndDraw();
        break;
    }
    return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
}

HRESULT STDMETHODCALLTYPE XBContent::characters( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars)
{
    switch (m_rContextStack[m_iContext].m_Context)
    {
    case Vertex_Context:
        return VertexChars(pwchChars, cchChars);
    case IndexBuffer_Context:
        return IndexBufferChars(pwchChars, cchChars);
    case VertexShader_asm_Context:
        return VertexShader_asm_Chars(pwchChars, cchChars);
    case VertexShader_Constant_Context:
        return VertexShader_Constant_Chars(pwchChars, cchChars);
    case PixelShader_asm_Context:
        return PixelShader_asm_Chars(pwchChars, cchChars);
    case PixelShader_Constant_Context:
        return PixelShader_Constant_Chars(pwchChars, cchChars);
    case Texture_Context:
        return TextureChars(pwchChars, cchChars);
    default:
        return MustBeWhitespace(pwchChars, cchChars);
    }
    return S_OK;
}      
       
HRESULT STDMETHODCALLTYPE XBContent::ignorableWhitespace( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars)
{
    return S_OK;
}
        

HRESULT STDMETHODCALLTYPE XBContent::processingInstruction( 
            /* [in] */ wchar_t __RPC_FAR *pwchTarget,
            /* [in] */ int cchTarget,
            /* [in] */ wchar_t __RPC_FAR *pwchData,
            /* [in] */ int cchData)
{
    return S_OK;
}
        
        
HRESULT STDMETHODCALLTYPE XBContent::skippedEntity( 
            /* [in] */ wchar_t __RPC_FAR *pwchVal,
            /* [in] */ int cchVal)
{
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// ISAXErrorHandler interface
//

static HRESULT PrintError(WCHAR *pwstrErrorType,
                          ISAXLocator __RPC_FAR *pLocator,
                          unsigned short * pwchErrorMessage,
                          HRESULT errCode)
{
    HRESULT hr;
    if (pLocator)
    {
        int iLine;
        hr = pLocator->getLineNumber(&iLine);
        if (FAILED(hr))
            return hr;
//      int iColumn;
//      hr = pLocator->getColumnNumber(&iColumn);
//      if (FAILED(hr))
//          return hr;
        WCHAR *pwstrID;
        hr = pLocator->getPublicId(&pwstrID);
        if (FAILED(hr))
            return hr;
        if (pwstrID == NULL)
        {
            hr = pLocator->getSystemId(&pwstrID);
            if (FAILED(hr))
                return hr;
        }

        // Strip file prefix so xbrc works better inside of msdev
        WCHAR *pwstrFilePrefix = L"file:///";
        INT FilePrefixLength = wcslen(pwstrFilePrefix);
        if (wcsncmp(pwstrID, pwstrFilePrefix, FilePrefixLength) == 0)
            pwstrID += FilePrefixLength;

//      fwprintf(stderr, L"\n%s(%d) : %s %08X: column %d: %s\n", 
//              pwstrID == NULL ? L"<unknown file>" : pwstrID, iLine, pwstrErrorType, errCode, iColumn, pwchErrorMessage);
        fwprintf(stderr, L"\n%s(%d) : %s %08X: %s\n", 
                 pwstrID == NULL ? L"<unknown file>" : pwstrID, iLine, pwstrErrorType, errCode, pwchErrorMessage);
    }
    else
    {
        // No locator available
        fwprintf(stderr, L"\n%s %08X: %s\n", 
                 pwstrErrorType, errCode, pwchErrorMessage);
    }
    return errCode;
}


HRESULT STDMETHODCALLTYPE XBContent::error( 
            /* [in] */ ISAXLocator __RPC_FAR *pLocator,
            /* [in] */ unsigned short * pwchErrorMessage,
            /* [in] */ HRESULT errCode)
{
    if (m_bErrorSuppress)
        return errCode;
    return PrintError(L"error", pLocator, pwchErrorMessage, errCode);
}

        
HRESULT STDMETHODCALLTYPE XBContent::fatalError( 
            /* [in] */ ISAXLocator __RPC_FAR *pLocator,
            /* [in] */ unsigned short * pwchErrorMessage,
            /* [in] */ HRESULT errCode)
{
    if (m_bErrorSuppress)
        return errCode;
    return PrintError(L"fatal error", pLocator, pwchErrorMessage, errCode);
}
        
HRESULT STDMETHODCALLTYPE XBContent::ignorableWarning( 
            /* [in] */ ISAXLocator __RPC_FAR *pLocator,
            /* [in] */ unsigned short * pwchErrorMessage,
            /* [in] */ HRESULT errCode)
{
    if (m_bErrorSuppress)
        return errCode;
    return PrintError(L"warning", pLocator, pwchErrorMessage, errCode);
}

