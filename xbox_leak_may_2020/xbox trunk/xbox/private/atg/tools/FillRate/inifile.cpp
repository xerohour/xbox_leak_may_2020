//-----------------------------------------------------------------------------
// FILE: INIFILE.CPP
//
// Desc: INI file routines
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include <assert.h>

#include "utils.h"
#include "inifile.h"




//-----------------------------------------------------------------------------
// Name: GetIniConstValue
//
// Desc: Get the value of a constant
//-----------------------------------------------------------------------------
LPCSTR CIniFile::GetIniConstValue( const char *szStr, INIVAL *pinival )
{
    // skip whitespace
    while( *szStr == ' ' || *szStr == '\t' )
        szStr++;

    if( ( *szStr >= '0' && *szStr <= '9' ) || ( *szStr == '.' ) )
    {
        char *endptr;
        int base = 10;

        if( szStr[1] == 'x' )
        {
            // hexadecimal: skip over leading '0x'
            base = 16;
            szStr += 2;
        }
        else if( pinival->type == INIVAL_FLOAT )
        {
            // check for floating point value
            for( const char *szT = szStr;
                *szT && ( ( *szT >= '0' && *szStr <= '9' ) || *szT == '.' );
                szT++ )
            {
                if( *szT == '.' )
                {
                    for( szT++; *szT && ( *szT >= '0' && *szStr <= '9' ); szT++ )
                        ;

                    // check for trailing f
                    if( *szT == 'f' )
                        szT++;

                    pinival->SetVal( (float) atof( szStr ) );
                    return szT;
                }
            }
        }

        pinival->SetVal( (int) strtoul( szStr, &endptr, base ) );
        return endptr;
    }

    // token length
    int cchStr = 0;

    // find token length
    for( const char *szT = szStr; 
         *szT && ( *szT != ';' ) && ( *szT != '|' ) && !isspace( *szT ); szT++ )
        cchStr++;

    // see if the token is known by the app
    if( GetIniConstf( szStr, cchStr, &pinival->valf ) )
    {
        pinival->val = (int)pinival->valf;
        return szStr + cchStr;
    }
    else if( GetIniConst( szStr, cchStr, &pinival->val ) )
    {
        pinival->valf = (float)pinival->val;
        return szStr + cchStr;
    }

    // couldn't find token - search in [defines] section
    if( m_recurselevel < 10 )
    {
        char szKeyName[64];
        static const char szSectName[] = "defines";

        lstrcpynA( szKeyName, szStr, min( sizeof( szKeyName ), cchStr + 1 ) );

        LPCSTR szLineStart;
        LPCSTR szLine = GetIniStr( szSectName, szKeyName, NULL, &szLineStart );

        if( szLine )
        {
            bool fFoundDefine = false;

            m_recurselevel++;

            // did not find entry - search in defines section
            if( pinival->type == INIVAL_FLOAT )
            {
                fFoundDefine = ParseFloatVal( szLineStart, szLine, &pinival->valf );
                pinival->val = (int)pinival->valf;
            }
            else
            {
                fFoundDefine = ParseIntVal( szLineStart, szLine, &pinival->val );
                pinival->valf = (float)pinival->val;
            }

            m_recurselevel--;

            if( fFoundDefine )
                return szStr + cchStr;
        }
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// Name: SzReadFile
//
// Desc: Read a file and return malloc'd pointer to data
//-----------------------------------------------------------------------------
char *SzReadFile( LPCSTR szFileName )
{

    HANDLE hFile = CreateFile( szFileName,
                          GENERIC_READ,
                          FILE_SHARE_READ,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL );
    if( hFile != INVALID_HANDLE_VALUE )
    {
        DWORD dwSize = GetFileSize( hFile, NULL );
        char *szFile = (char *)malloc( dwSize + 1 );
        assert( szFile != NULL);

        DWORD NumberOfBytesRead;
		BOOL bReadOK;
		bReadOK = ReadFile( hFile, szFile, dwSize, &NumberOfBytesRead, NULL );
		assert(bReadOK);
        assert(dwSize == NumberOfBytesRead );

        szFile[NumberOfBytesRead] = 0;

        CloseHandle( hFile );
        return szFile;
    }
    else
    {
        HRESULT result = GetLastError();
        dprintf( "Could not open %s   Error: %d\n", szFileName, result);
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// Name: ReadFile
//
// Desc: Read/refresh ini file
//-----------------------------------------------------------------------------
bool CIniFile::ReadFile()
{
    if( m_szFile )
    {
        free( m_szFile );
        m_szFile = NULL;
    }

    m_szFile = SzReadFile( m_szFileName );
    if( !m_szFile )
    {
        // if file read failed - sleep a short bit in case xbcp
        // had file open still.
        Sleep( 500 );
        m_szFile = SzReadFile( m_szFileName);
    }

    if( m_szFile )
    {
        HANDLE hFile = CreateFile( m_szFileName,
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL );

        if( hFile != INVALID_HANDLE_VALUE )
            GetFileTime( hFile, NULL, NULL, &m_ftFileReadTime );

        CloseHandle(hFile);

        return true;
    }

    return false;
}




//-----------------------------------------------------------------------------
// Name: GetLine
//
// Desc: Find the next real line and return pointer
//-----------------------------------------------------------------------------
LPCSTR GetLine( LPCSTR szLine )
{
    for( ;; )
    {
        // skip whitespace
        while( isspace( *szLine ) )
            szLine++;

        // check for comments
        if( *szLine == ';' || ( szLine[0] == '/' && szLine[1] == '/' ) )
        {
            // found a comment - cruise to end of line
            while( *szLine && *szLine != '\n' )
                szLine++;
            continue;
        }

        return szLine;
    }
}




//-----------------------------------------------------------------------------
// Name: FindFirst
//
// Desc: Get first line of INI file
//-----------------------------------------------------------------------------
LPCSTR FindFirstLine( LPCSTR szFile )
{
    if( szFile )
        return GetLine( szFile );

    return NULL;
}




//-----------------------------------------------------------------------------
// Name: FindNextLine
//
// Desc: Get line after szLine
//-----------------------------------------------------------------------------
LPCSTR FindNextLine( LPCSTR szLine )
{
    if( szLine )
    {
        while( *szLine && *szLine++ != '\n' )
            ;
        return GetLine( szLine );
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// Name: FindNextChar
//
// Desc: Find the next token's first char
//-----------------------------------------------------------------------------
char FindNextChar( LPCSTR szLine, LPCSTR *ppszLine = NULL )
{
    // skip whitespace
    while( *szLine == ' ' || *szLine == '\t' )
        szLine++;

    if( ppszLine )
        *ppszLine = szLine;
    return *szLine;
}




//-----------------------------------------------------------------------------
// Name: OuputLineErr
//
// Desc: Print out error message for something we couldn't parse
//-----------------------------------------------------------------------------
void OutputLineErr(LPCSTR szErr, LPCSTR szLine)
{
    LPCSTR szEnd = szLine;
    while( *szEnd && ( *szEnd != '\r' && *szEnd != '\n' ) )
        szEnd++;

    char chSav = *szEnd;
    *(char *)szEnd = 0;

    OutputDebugStringA( szErr );
    OutputDebugStringA( ":" );
    OutputDebugStringA( szLine );
    OutputDebugStringA( "\n" );

    *(char *)szEnd = chSav;
}




//-----------------------------------------------------------------------------
// Name: ParseFloatVal
//
// Desc: Parse a token stream and return the value
//-----------------------------------------------------------------------------
bool CIniFile::ParseFloatVal( LPCSTR szLineStart, LPCSTR szLine, float *pval )
{
    INIVAL valT = { INIVAL_FLOAT };
    if( GetIniConstValue( szLine, &valT ) )
    {
        *pval = valT.valf;
        return true;
    }

    OutputLineErr("unknown token", szLineStart );
    return false;
}




//-----------------------------------------------------------------------------
// Name: ParseIntVal
//
// Desc: Parse a token stream and return the value
//-----------------------------------------------------------------------------
bool CIniFile::ParseIntVal( LPCSTR szLineStart, LPCSTR szLine, int *pval )
{
    INIVAL valT = { INIVAL_INT };
    int val = 0;

    while( szLine = GetIniConstValue( szLine, &valT ) )
    {
        val |= valT.val;

        char ch = FindNextChar( szLine, &szLine );
        switch( ch )
        {
        case '|':
            szLine++;
            break;
             
        case '\r':
        case '\n':
        case '\0':
        case ';':
            // end of the line
            *pval = val;
            return true;

        default:
            OutputLineErr( "unknown char", szLineStart );
            return false;
        }
    }

    OutputLineErr( "unknown token", szLineStart );
    return false;
}




//-----------------------------------------------------------------------------
// Name: GetIniFloat
//
// Desc: Get float value for ini entry
//-----------------------------------------------------------------------------
float CIniFile::GetIniFloat( LPCSTR lpSectionName, LPCSTR lpKeyName, 
                             float nDefault )
{
    LPCSTR szLineStart;
    LPCSTR szLine = GetIniStr( lpSectionName, lpKeyName, NULL, &szLineStart );

    if( !szLine )
        return nDefault;

    ParseFloatVal( szLineStart, szLine, &nDefault );
    return nDefault;
}




//-----------------------------------------------------------------------------
// Name: GetIniInt
//
// Desc: Get int value for ini entry
//-----------------------------------------------------------------------------
int CIniFile::GetIniInt( LPCSTR lpSectionName, LPCSTR lpKeyName, int nDefault )
{
    LPCSTR szLineStart;
    LPCSTR szLine = GetIniStr( lpSectionName, lpKeyName, NULL, &szLineStart );

    if( !szLine )
        return nDefault;

    ParseIntVal( szLineStart, szLine, &nDefault );
    return nDefault;
}




//-----------------------------------------------------------------------------
// Name: IniEntryExists
//
// Desc: Check if an ini entry exists
//-----------------------------------------------------------------------------
bool CIniFile::IniEntryExists( LPCSTR lpSectionName, LPCSTR lpKeyName )
{
    bool fverboseBak = m_fverbose;
    m_fverbose = false;

    LPCSTR szLine = GetIniStr( lpSectionName, lpKeyName, NULL, NULL );

    m_fverbose = fverboseBak;
    return !!szLine;
}




//-----------------------------------------------------------------------------
// Name: GetIniStrBuf
//
// Desc: Get an INI string and shtuff it into a buffer
//-----------------------------------------------------------------------------
bool CIniFile::GetIniStrBuf( LPCSTR lpSectionName, LPCSTR lpKeyName,
                             LPCSTR szDefault, char *buf, int buflen )
{
    LPCSTR szLine = GetIniStr( lpSectionName, lpKeyName, NULL, NULL );
    if( szLine )
    {
        strncpy( buf, szLine, buflen );

        for( int nT = 0; nT < buflen; nT++ )
        {
            if( buf[nT] == '\n' || buf[nT] == '\r' || buf[nT] == ';' )
            {
                buf[nT] = 0;
                break;
            }
        }
    }
    else
    {
        strncpy( buf, szDefault, buflen );
    }

    buf[buflen - 1] = 0;
    return !!szLine;
}




//-----------------------------------------------------------------------------
// Name: GetIniStr
//
// Desc: Return non-nil terminated string for keyname
//-----------------------------------------------------------------------------
LPCSTR CIniFile::GetIniStr( LPCSTR lpSectionName, LPCSTR lpKeyName, 
                            LPCSTR szDefault, LPCSTR *ppszLineStart )
{
    assert( m_szFile );
    if( !m_szFile )
        return szDefault;

    bool finsection = false;
    int cchSectionName = lstrlenA( lpSectionName );
    int cchKeyName = lstrlenA( lpKeyName );
    LPCSTR szLine = FindFirstLine( m_szFile );

    while( szLine && *szLine )
    {
        if( ppszLineStart )
            *ppszLineStart = szLine;

        if( finsection )
        {
            if( *szLine == '[' )
            {
                // new section - not found
                break;
            }
            else if( !_strnicmp(lpKeyName, szLine, cchKeyName ) &&
                   ( FindNextChar(szLine + cchKeyName, &szLine ) == '=' ) )
            {
                // skip over '='
                szLine++;

                // skip whitespace
                while( *szLine == ' ' || *szLine == '\t' )
                    szLine++;

                return szLine;
            }
        }
        else if( *szLine == '[' )
        {
            // section name matches?
            szLine++;
            finsection = !_strnicmp( lpSectionName, szLine, cchSectionName ) &&
                                   ( FindNextChar( szLine + cchSectionName ) == ']' );
        }

        szLine = FindNextLine( szLine );
    }

    if( m_fverbose )
        dprintf( "entry [%s]/%s not found\n", lpSectionName, lpKeyName );

    return szDefault;
}

