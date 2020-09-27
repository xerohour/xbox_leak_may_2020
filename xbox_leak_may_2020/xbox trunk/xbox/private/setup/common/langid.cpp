//  LANGID.CPP
//
//  Created 18-Feb-2002 [JimHelm]

#include "common.h"
#include "langid.h"

// Constructors and Destructor
CLangID::CLangID() :
m_uiListPosition( 0 )
{
}

// Copy constructor
CLangID::CLangID( const CLangID& LangID )
{
    // Copy the list of lang ids
    m_LangIDs = LangID.m_LangIDs;   // Does this work?  Does the list class contain an assignment operator that works?
}

CLangID::~CLangID()
{
    // Empty the linked list
    m_LangIDs.clear();
}

// Takes a NULL terminated string, parses it for lang IDs
// which should be separated by colons ':' (if there is more
// than one), and adds them to the list
BOOL CLangID::ParseLangIDs( char* pszLine )
{
    if( NULL == pszLine )
        return FALSE;

    // Empty the linked list
    m_LangIDs.clear();

    // Check to see if the LangID was left empty.
    // If so, add the global "0000" to our list and exit
    if( ',' == *pszLine )
    {
        m_LangIDs.push_back( 0x0000 );
        return TRUE;
    }

    // If the line wasn't empty, assume we have at least one LangID in the list
    LPSTR lpStopScan = NULL;
    LPSTR lpNextID = pszLine;  // Used to find the next LangID in our string
    DWORD dwLangID = 0;
    do
    {
        dwLangID = strtol( lpNextID, &lpStopScan, 16 );
        m_LangIDs.push_back( dwLangID );
        lpNextID = strstr( lpNextID, ":" );
        
        // Exit if there are no more LangIDs
        if( NULL == lpNextID )
            break;

        // Skip over the ':' and point at the next LangID
        lpNextID++;
    } while( TRUE );

    return TRUE;
}


// Will return TRUE if the Lang ID is valid for this machine
BOOL CLangID::IsLangIDValid( LANGID lid )
{
    BOOL bValidID = FALSE;

    for( std::list<DWORD>::iterator i = m_LangIDs.begin(); i != m_LangIDs.end(); ++i )
    {
        // If our lang ID matches the current machine, or 0000, we should stop looking
        // and return TRUE
        if( ( 0x0000 == *i ) || ( lid == *i ) )
        {
            bValidID = TRUE;
            break;
        }
    }

    return bValidID;
}


// Move our pointer to the first item in the list
BOOL CLangID::MoveFirstLangID()
{
    // Set this to 0 no matter what
    m_uiListPosition = 0;

    // If the list is not empty, return TRUE, otherwise fail
    if( !m_LangIDs.empty() )
        return TRUE;

    return FALSE;
}


// Move our pointer to the next item in the list
BOOL CLangID::MoveNextLangID()
{
    if( ( m_uiListPosition + 1 ) < m_LangIDs.size() )
    {
        m_uiListPosition++;
        return TRUE;
    }

    return FALSE;
}


// Retreive the current langID being pointed to
DWORD CLangID::GetCurrentLangID()
{
    unsigned int uiCount = 0;
    for( std::list<DWORD>::iterator i = m_LangIDs.begin(); i != m_LangIDs.end(); ++i )
    {
        if( uiCount == m_uiListPosition )
            return *i;

        ++uiCount;
    }

    return 0;
}