//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       errclass.cpp
//
//--------------------------------------------------------------------------

#include "errparse.h"


//**************************************************************
CErrorMessage::CErrorMessage( DWORD dwErrorNumber, char *pszSymbolicName, char *pszDescription )
// Constructor
//**************************************************************
{
    m_pszSymbolicName = m_pszDescription = NULL;
    m_dwErrorNumber = dwErrorNumber;

    if( NULL != pszSymbolicName )
    {
        m_pszSymbolicName = new char[( strlen( pszSymbolicName ) + 1 )];
        if( NULL != m_pszSymbolicName )
        {
            strcpy( m_pszSymbolicName, pszSymbolicName );
        }
    }

    if( NULL != pszDescription )
    {
        m_pszDescription = new char[( strlen( pszDescription ) + 1 )];
        if( NULL != m_pszDescription )
        {
            strcpy( m_pszDescription, pszDescription );
        }
    }
}


//**************************************************************
BOOL CErrorMessage::CSetErrorMessage( DWORD dwErrorNumber, char *pszSymbolicName, char *pszDescription )
// Real Constructor
//**************************************************************
{
    if( m_pszSymbolicName != NULL )
    {
        delete m_pszDescription;
    }

    if( m_pszDescription != NULL )
    {
        delete m_pszDescription;
    }
    m_pszSymbolicName = m_pszDescription = NULL;
    m_dwErrorNumber = dwErrorNumber;


    if( NULL != pszSymbolicName )
    {
        m_pszSymbolicName = new char[ ( strlen( pszSymbolicName ) + 1 ) ];
        if( NULL != m_pszSymbolicName )
        {
            strcpy( m_pszSymbolicName, pszSymbolicName );
        }
    }

    if( NULL != pszDescription )
    {
        m_pszDescription = new char[ ( strlen( pszDescription ) + 1 ) ];
        if( NULL != m_pszDescription )
        {
            strcpy( m_pszDescription, pszDescription );
        }
    }
    return( TRUE );
}


//**************************************************************
CErrorMessage::~CErrorMessage()
// Deconstructor
//**************************************************************
{
    delete m_pszSymbolicName;
    delete m_pszDescription;
}

//**************************************************************
BOOL CErrorMessage::CPrint( FILE * pFile )
// Prints out object info to given file
//**************************************************************
{
    char * pFirstChar;
    
    if( NULL == &pFile )
    {
        return( FALSE );
    }

    //We can store incorrectly, but we ensure here that we print correctly
    while( NULL != ( pFirstChar = strchr( m_pszDescription, '\\' ) ) )
    {
       *pFirstChar = '/';
    }

    while( NULL != ( pFirstChar = strchr( m_pszDescription, '\"' ) ) )
    {
       *pFirstChar = '\'';
    }
    
    if( 0 == (fprintf( pFile, "    { %lu, \"%s\", \"%s\" },\n", m_dwErrorNumber, m_pszSymbolicName, m_pszDescription) ) )
    {
        return( FALSE );
    }
    
    return( TRUE );
}
