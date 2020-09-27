/*************************************************************************************************\
Locale.cpp		    : Implementation of the Locale component.
Creation Date		: 2/8/2002 6:52:28 PM
Copyright Notice	: (C) 2000 Microsoft
Author				: Victor Blanco
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/
#include "std.h"
#include "locale.h"
#include "file.h"

HRESULT ReturnLocaleFile( const char *szDirName, int nCountry, char **szReturnBuffer )
{
    CHAR strPath[MAX_PATH];
    sprintf( strPath, "%s%s\\%s.%3d", CONTENT_PATH_ANSI, szDirName, LOCALE_FILE_ANSI, nCountry );

    // Try to open the file
    File inputFile;
	if( inputFile.open( strPath, READ ) != NO_ERR )
    {
        // Return error
        CHAR strBuffer[80];
        sprintf( strBuffer, "Locale::LoadLocale(): Could not find file [%s]\n", strPath );
        OutputDebugStringA( strBuffer );
        ASSERT( false );
        return E_FAIL;
    }

    DWORD dwSize = inputFile.fileSize();

    *szReturnBuffer = new char[dwSize + 1];
    DWORD numRead = 0;

    if( !inputFile.read( (BYTE *)*szReturnBuffer, dwSize ) )
    {
        // Return error
        CHAR strBuffer[80];
        sprintf( strBuffer, "Locale::LoadLocaleFile(): Could not read from file [%s]\n", strPath );
        OutputDebugStringA( strBuffer );
        ASSERT( false );
        return E_FAIL;
    }

    (*szReturnBuffer)[dwSize] = '\0';

    // Found the file. Close the file and return
    inputFile.close();

    return S_OK;
}

LocaleInfo::LocaleInfo()
{
    m_LocaleData.m_bMonthFirstFormatting = 0;
    m_LocaleData.m_bTwentyFourHourTime   = 0;
    m_LocaleData.m_cDateSeperator        = '\0';
    m_LocaleData.m_cIntegerSeperator     = '\0';
    m_LocaleData.m_cTimeSeperator        = '\0';
}

LocaleInfo::~LocaleInfo()
{

}

HRESULT LocaleInfo::LoadLocale( int nLanguage, int nCountry )
{
    char *szLocaleStream = NULL;
    if( FAILED(ReturnLocaleFile( szLocaleDir[nLanguage], nCountry, &szLocaleStream ) ) )
    {   
        return E_FAIL;
    }

    char seps[] = "=\n\0";
    char *token = "\0";

    token = strtok( szLocaleStream, seps );     

    while( token != NULL )
    {
        if( strcmp( "MonthFirstFormatting", token ) == 0 )
        {
            m_LocaleData.m_bMonthFirstFormatting = atoi( strtok( NULL, seps ) );
        }
        else if( strcmp ("TwentyFourHourTime", token ) == 0 )
        {
             m_LocaleData.m_bTwentyFourHourTime  = atoi( strtok( NULL, seps ) );
        }
        else if( strcmp ("DateSeperator", token ) == 0 )
        {
            token = strtok( NULL, seps );
            m_LocaleData.m_cDateSeperator = token[0];
        }
        else if( strcmp ("IntegerSeperator", token ) == 0 )
        {
            token = strtok( NULL, seps );
            m_LocaleData.m_cIntegerSeperator = token[0];
        }
        else if( strcmp ("TimeSeperator", token ) == 0 )
        {
            token = strtok( NULL, seps );
            m_LocaleData.m_cTimeSeperator = token[0];
        }
        token = strtok( NULL, seps ); 
    }   

    delete [] szLocaleStream;

    return S_OK;
}

HRESULT LocaleInfo::UpdateLocaleInfo()
{
    HRESULT hr = S_OK;

    return hr;
}


