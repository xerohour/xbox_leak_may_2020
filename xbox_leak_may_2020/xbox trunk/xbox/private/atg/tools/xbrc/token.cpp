//-----------------------------------------------------------------------------
// File: Token.cpp
//
// Desc: Contains the logic for extracting tokens from the resource
//       description file
//
// Hist: 2001.02.06 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Bundler.h"
#include <stdio.h>

/*

// Token defintions
const BUNDLERTOKEN g_Tokens[] =
{
    // Token string         token enumeration                   type            handler function
    { "",                   TOKEN_RESOURCE_EOF,                 TT_ANY,         CBundler::HandleEOFToken        },
//    { "{",                  TOKEN_RESOURCE_OPENBRACE,           TT_ANY,         NULL                            },
//    { "}",                  TOKEN_RESOURCE_CLOSEBRACE,          TT_ANY,         NULL                            },
    { "out_packedresource", TOKEN_RESOURCE_OUT_PACKEDRESOURCE,  TT_ANY,         CBundler::HandleOutPackedResourceToken      },
    { "out_header",         TOKEN_RESOURCE_OUT_HEADER,          TT_ANY,         CBundler::HandleOutHeaderToken  },
    { "out_prefix",         TOKEN_RESOURCE_OUT_PREFIX,          TT_ANY,         CBundler::HandleOutPrefixToken  },
    { "out_error",          TOKEN_RESOURCE_OUT_ERROR,           TT_ANY,         CBundler::HandleOutErrorToken   },

    { "Texture",            TOKEN_RESOURCE_TEXTURE,             TT_ANY,         CBundler::HandleTextureToken    },
    { "Source",             TOKEN_PROPERTY_TEXTURE_SOURCE,      TT_FILENAME,    NULL                            },
    { "AlphaSource",        TOKEN_PROPERTY_TEXTURE_ALPHASOURCE, TT_FILENAME,    NULL                            },
    { "Filter",             TOKEN_PROPERTY_TEXTURE_FILTER,      TT_ANY,         NULL                            },
    { "Format",             TOKEN_PROPERTY_TEXTURE_FORMAT,      TT_IDENTIFIER,  NULL                            },
    { "Width",              TOKEN_PROPERTY_TEXTURE_WIDTH,       TT_INTEGER,     NULL                            },
    { "Height",             TOKEN_PROPERTY_TEXTURE_HEIGHT,      TT_INTEGER,     NULL                            },
    { "Levels",             TOKEN_PROPERTY_TEXTURE_LEVELS,      TT_INTEGER,     NULL                            },
    
    { "Cubemap",            TOKEN_RESOURCE_CUBEMAP,                TT_ANY,         CBundler::HandleCubemapToken    },
    { "SourceXP",           TOKEN_PROPERTY_CUBEMAP_SOURCE_XP,      TT_FILENAME,    NULL                            },
    { "SourceXN",           TOKEN_PROPERTY_CUBEMAP_SOURCE_XN,      TT_FILENAME,    NULL                            },
    { "SourceYP",           TOKEN_PROPERTY_CUBEMAP_SOURCE_YP,      TT_FILENAME,    NULL                            },
    { "SourceYN",           TOKEN_PROPERTY_CUBEMAP_SOURCE_YN,      TT_FILENAME,    NULL                            },
    { "SourceZP",           TOKEN_PROPERTY_CUBEMAP_SOURCE_ZP,      TT_FILENAME,    NULL                            },
    { "SourceZN",           TOKEN_PROPERTY_CUBEMAP_SOURCE_ZN,      TT_FILENAME,    NULL                            },
    { "AlphaSourceXP",      TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_XP, TT_FILENAME,    NULL                            },
    { "AlphaSourceXN",      TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_XN, TT_FILENAME,    NULL                            },
    { "AlphaSourceYP",      TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_YP, TT_FILENAME,    NULL                            },
    { "AlphaSourceYN",      TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_YN, TT_FILENAME,    NULL                            },
    { "AlphaSourceZP",      TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_ZP, TT_FILENAME,    NULL                            },
    { "AlphaSourceZN",      TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_ZN, TT_FILENAME,    NULL                            },
    { "Size",               TOKEN_PROPERTY_CUBEMAP_SIZE,           TT_INTEGER,     NULL                            },

    { "VolumeTexture",      TOKEN_RESOURCE_VOLUMETEXTURE,             TT_ANY,         CBundler::HandleVolumeTextureToken    },
//    { "Source",             TOKEN_PROPERTY_VOLUMETEXTURE_SOURCE,      TT_FILENAME,    NULL                            },
//    { "AlphaSource",        TOKEN_PROPERTY_VOLUMETEXTURE_ALPHASOURCE, TT_FILENAME,    NULL                            },
    { "Depth",              TOKEN_PROPERTY_VOLUMETEXTURE_DEPTH,       TT_INTEGER,     NULL                            },

    { "VertexBuffer",       TOKEN_RESOURCE_VERTEXBUFFER,        TT_ANY,         CBundler::HandleVertexBufferToken},
    { "VertexData",         TOKEN_PROPERTY_VB_VERTEXDATA,       TT_ANY,         NULL                            },
    { "VertexFormat",       TOKEN_PROPERTY_VB_VERTEXFORMAT,     TT_ANY,         NULL                            },

    { "UserData",           TOKEN_RESOURCE_USERDATA,            TT_ANY,         CBundler::HandleUserDataToken},
    { "DataFile",           TOKEN_PROPERTY_USERDATA_DATAFILE,   TT_ANY,         NULL },
};

*/


//-----------------------------------------------------------------------------
// Name: IsAlphaChar()
// Desc: TRUE for [a-zA-Z]
//-----------------------------------------------------------------------------
BOOL IsAlphaChar( char ch )
{
    return( ( ( ch >= 'a' ) && ( ch <= 'z' ) )  ||
            ( ( ch >= 'A' ) && ( ch <= 'Z' ) ) );
}




//-----------------------------------------------------------------------------
// Name: IsNumberChar()
// Desc: TRUE for [0-9]
//-----------------------------------------------------------------------------
BOOL IsNumberChar( char ch )
{
    return( ( ch >= '0' ) && ( ch <= '9' ) );
}




//-----------------------------------------------------------------------------
// Name: IsIdentifierChar()
// Desc: TRUE for [a-zA-Z0-9_]
//-----------------------------------------------------------------------------
BOOL IsIdentifierChar( char ch )
{
    return( IsAlphaChar( ch )  ||
            IsNumberChar( ch ) ||
            ch == '_' );
}




//-----------------------------------------------------------------------------
// Name: IsHexChar()
// Desc: TRUE for [a-fA-F0-9]
//-----------------------------------------------------------------------------
BOOL IsHexChar( char ch )
{
    return( ( ( ch >= 'a' ) && ( ch <= 'f' ) )  ||
            ( ( ch >= 'A' ) && ( ch <= 'F' ) )  ||
            IsNumberChar( ch ) );
}
            



//-----------------------------------------------------------------------------
// Name: IsWhiteSpaceChar()
// Desc: TRUE for [ \t\r\n]
//-----------------------------------------------------------------------------
BOOL IsWhiteSpaceChar( char ch )
{
    return( ( ch == ' '  ) ||
            ( ch == '\t' ) ||
            ( ch == '\r' ) ||
            ( ch == '\n' ) ||
            ( ch == ','  ) );   // commas are whitespace for this parser
}




//-----------------------------------------------------------------------------
// Name: TerminatesToken()
// Desc: True if next char starts new token or breaks between token
//-----------------------------------------------------------------------------
BOOL TerminatesToken( char ch )
{
    if(IsWhiteSpaceChar( ch )) return TRUE;
    if((ch == '{') ||
       (ch == '}') ||
       (ch == TOKEOF)) return TRUE;

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: IsFilenameChar()
// Desc: TRUE for [^\[\]\:\\/<>\=;,\t\r\n ]
//-----------------------------------------------------------------------------
BOOL IsFilenameChar( char ch )
{
    return( ch != '[' &&
            ch != ']' &&
            ch != ':' &&
            ch != '\\' &&
            ch != '/' &&
            ch != '<' &&
            ch != '>' &&
            ch != '=' &&
            ch != ';' &&
            ch != ',' &&
            ch != '\t' &&
            ch != '\r' &&
            ch != '\n' );
}



/*
//-----------------------------------------------------------------------------
// Name: GetChar()
// Desc: Gets the next character from the input file, removed comments
//-----------------------------------------------------------------------------
CHAR CBundler::GetChar()
{
    DWORD cb;
    CHAR tmp = m_cNext0;
    m_cNext0 = m_cNext1;
    m_cNext1 = m_cNext2;
    m_cNext2 = m_cNext3;
    ReadFile( m_hfRDF, &m_cNext3, 1, &cb, NULL );
    if(cb == 0)
        m_cNext3 = TOKEOF;

    if((m_cNext0 == (CHAR)0xff) && (m_cNext1 == (CHAR)0xfe))
    {
        ErrorMsg( "Error: Unicode files are not supported\n");
        return TOKEOF;
    }
    
    while(TRUE)
    {
        // check for // comment
        if((m_cNext2 == '/') && (m_cNext3 == '/'))
        {
            // skip until \n for eof
            while((m_cNext2 != '\n') && (m_cNext2 != TOKEOF))
            {
                m_cNext2 = m_cNext3;
                ReadFile( m_hfRDF, &m_cNext3, 1, &cb, NULL );
                if(cb == 0)
                    m_cNext3 = TOKEOF;
            }
        }
        // check for / * comment
        if((m_cNext2 == '/') && (m_cNext3 == '*'))
        {
            // skip until * / or eof
            while(!(((m_cNext2 == '*') && (m_cNext3 == '/')) || (m_cNext2 == TOKEOF)))
            {
                m_cNext2 = m_cNext3;
                ReadFile( m_hfRDF, &m_cNext3, 1, &cb, NULL );
                if(cb == 0)
                    m_cNext3 = TOKEOF;
            }
            ReadFile( m_hfRDF, &m_cNext2, 1, &cb, NULL );
            if(cb == 0)
                m_cNext2 = TOKEOF;
            ReadFile( m_hfRDF, &m_cNext3, 1, &cb, NULL );
            if(cb == 0)
                m_cNext3 = TOKEOF;
        }
        return tmp;
    }
}




//-----------------------------------------------------------------------------
// Name: PeekChar()
// Desc: Gets the next character from the input file without advancing the read
//       pointer.
//-----------------------------------------------------------------------------
CHAR CBundler::PeekChar()
{
    return m_cNext0;
}




//-----------------------------------------------------------------------------
// Name: GetNextTokenString()
// Desc: Gets the next token from the resource definition file.  
//-----------------------------------------------------------------------------
HRESULT CBundler::GetNextTokenString( char *strTokenString, TOKENTYPE tt )
{
    CHAR *cPtr = strTokenString;
    CHAR c;

    // skip whitespace
    while(IsWhiteSpaceChar(c=GetChar()));
    
    if(c == TOKEOF)
        return S_OK;        // eof

    *cPtr++ = c;

    if((c == '{') ||
       (c == '}'))
        return S_OK;        // single character tokens
    
    while(!TerminatesToken(PeekChar()))
        *cPtr++ = GetChar();

    *cPtr++ = '\0';     // terminate the string

    // Check that it's an appropriate type
    if( FAILED( ValidateType( strTokenString, tt ) ) )
        return E_FAIL;

    return S_OK;
}
*/



//-----------------------------------------------------------------------------
// Name: ValidateIdentifier()
// Desc: Verifies that the token is in the proper form for an identifier:
// ([A-za-z]+[A-Za-z0-9_]*)
//-----------------------------------------------------------------------------
static BOOL ValidateIdentifier( char* strToken )
{
    int i;

    if( !IsAlphaChar( strToken[0] ) )
        return FALSE;

    for( i = 1; strToken[i] != 0; i++ )
    {
        if( !IsIdentifierChar( strToken[i] ) )
            return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: ValidateHexNumber()
// Desc: Verifies that the token is in the proper form for a hex number:
// (0[x|X][0-9A-Fa-f]+)
//-----------------------------------------------------------------------------
static BOOL ValidateHexNumber( char* strToken )
{
    int i;

    if( strToken[0] != '0' )
        return FALSE;
    if( strToken[1] != 'x' &&
        strToken[1] != 'X' )
        return FALSE;
    
    for( i = 2; strToken[i] != 0; i++ )
    {
        if( !IsHexChar( strToken[i] ) )
            return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: ValidateInteger()
// Desc: Verifies that the token is in the proper form for a integer:
// ([-|][0-9]+)
//-----------------------------------------------------------------------------
static BOOL ValidateInteger( char* strToken )
{
    int i = 0;

    if( strToken[0] == '-' )
        i++;

    for( ; strToken[i] != 0; i++ )
    {
        if( !IsNumberChar( strToken[i] ) )
            return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: ValidateFloat()
// Desc: Verifies that the token is in the proper form for a float:
// [-|](([0-9]+(\.[0-9]+|)|\.[0-9]+)([Ee](\+|-|)[0-9]+|))
//-----------------------------------------------------------------------------
static BOOL ValidateFloat( char* strToken )
{
    int i = 0;
    BOOL bDecimal = FALSE;
    BOOL bExponent = FALSE;
    
    if( strToken[0] == '-' )
        i++;

    for( ; strToken[i] != 0; i++ )
    {
        if( strToken[i] == '.' )
        {
            if( bDecimal )
                return FALSE;
            bDecimal = TRUE;
        }
        else if( strToken[i] == 'e' ||
                 strToken[i] == 'E' )
        {
            if( bExponent )
                return FALSE;

            bDecimal = bExponent = TRUE;
            if( strToken[i + 1] == '+' ||
                strToken[i + 1] == '-' )
                i++;
        }
        else if( !IsNumberChar( strToken[i] ) )
            return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: ValidateFilename()
// Desc: Verifies that the token is in the proper form for a filename:
// (([A-Za-z]:|)(\\|/)(\:f(\\|/))*\:f)  where :f is a sequence of filename chars
//-----------------------------------------------------------------------------
static BOOL ValidateFilename( char* strToken )
{
    int i = 0;
    BOOL bLastWasSlash = FALSE;

    if( IsAlphaChar( strToken[0] ) &&
        strToken[1] == ':' )
    {
        i = 2;
    }

    for( ; strToken[i] != 0; i++ )
    {
        if( strToken[i] == '\\' ||
            strToken[i] == '/' )
        {
            bLastWasSlash = TRUE;
            continue;
        }

        if( !IsFilenameChar( strToken[i] ) )
            return FALSE;

        bLastWasSlash = FALSE;
    }

    return !bLastWasSlash;
}




//-----------------------------------------------------------------------------
// Name: ValidateType()
// Desc: Checks to see if the token is of the appropriate type.  If not, 
//       outputs an error message
//-----------------------------------------------------------------------------
HRESULT CBundler::ValidateType( char* strToken, TOKENTYPE tt )
{
    BOOL bValid;

    switch( tt )
    {
		case TT_ANY:
			bValid = TRUE;
			break;
		case TT_IDENTIFIER:
			bValid = ValidateIdentifier( strToken );
			break;
		case TT_HEXNUMBER:
			bValid = ValidateHexNumber( strToken );
			break;
		case TT_INTEGER:
			bValid = ValidateInteger( strToken );
			break;
		case TT_FLOAT:
			bValid = ValidateFloat( strToken );
			break;
		case TT_FILENAME:
			bValid = ValidateFilename( strToken );
			break;
    }

    if( !bValid )
        return E_FAIL;

    return S_OK;
}



/*

//-----------------------------------------------------------------------------
// Name: GetTokenFromString()
// Desc: Finds the token matching the given string
//-----------------------------------------------------------------------------
HRESULT CBundler::GetTokenFromString( char * strTokenString, BUNDLERTOKEN * pTok )
{
    // This lookup could be made faster via binary search or hashing
    for( int i = 0; i < sizeof( g_Tokens ) / sizeof( g_Tokens[0] ); i++ )
    {
        // Do we have a match?
        if( !lstrcmpi( strTokenString, g_Tokens[i].strKeyword ) )
        {
            *pTok = g_Tokens[i];
            return S_OK;
        }
    }

    ErrorMsg( "Unknown token <%s>\n", strTokenString );
    return E_FAIL;
}


//-----------------------------------------------------------------------------
// Name: GetNextToken()
// Desc: Gets the next token from the resource definition file
//-----------------------------------------------------------------------------
HRESULT CBundler::GetNextToken( BUNDLERTOKEN* pTok )
{
    HRESULT hr;
    char strTemp[MAX_PATH];

    ZeroMemory(strTemp, MAX_PATH);   // clear out old token string
    hr = GetNextTokenString( strTemp, TT_ANY );
    if( FAILED( hr ) )
        return hr;

    hr = GetTokenFromString( strTemp, pTok );
    if( FAILED( hr ) )
        return hr;

    return hr;
}

*/

