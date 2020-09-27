/*****************************************************
*** xbelist.h
***
*** Header file for our XBE List class.
*** This class will hold a linked list of XBE's that
*** will be populated from a directory.
***
*** by James N. Helm
*** December 2nd , 2000
***
*****************************************************/

#ifndef _XBELIST_H_
#define _XBELIST_H_

#include <xbeimage.h>

#define XBE_INITIAL_HEADER_SIZE     4096

class CXBEList
{
public:
    // Constructors and Destructors
    CXBEList();
    ~CXBEList();

    // Methods
    HRESULT Init( char* dirname, char* fileExt );                                               // Initialize the list from a directory, loading files of a certain type
    unsigned int GetNumItems() { return m_XBEItems.GetNumItems(); };                            // Get the number of items currently in our list
    char* GetXBEName( unsigned int index ) { return m_XBEItems[index]->filename; };             // Get the name of an XBE by index
    char* GetXBEDir( unsigned int index ) { return m_XBEItems[index]->directory; };             // Get the directory of an XBE by index
    WCHAR* GetXBETitle( unsigned int index ) { return m_XBEItems[index]->titleName; };          // The title name of the XBE as in the XBE CERT Information
    void SetStopXBEEnumeration( BOOL bStop ) { m_bStopEnumeration = bStop; };                   // Stops the enumeration of XBE's

private:
    class _XBEINFO
    {
    public:
        char directory[MAX_PATH + 1];
        char filename[MAX_PATH + 1];
        WCHAR titleName[XBEIMAGE_TITLE_NAME_LENGTH + 1];

        BOOL operator > ( const _XBEINFO& item )
        {
            if( _wcsicmp( titleName, item.titleName ) > 0 )
                return TRUE;

            return FALSE;
        }

        BOOL operator < ( const _XBEINFO& item )
        {
            if( _wcsicmp( titleName, item.titleName ) < 0 )
                return TRUE;

            return FALSE;
        }

        BOOL operator == ( const _XBEINFO& item )
        {
            if( _wcsicmp( titleName, item.titleName ) == 0 )
                return TRUE;

            return FALSE;
        }
    };

    // Private Properties
    CLinkedList< _XBEINFO* > m_XBEItems;                    // XBE Item list
    BOOL m_bInitialized;                                    // Determines if we've already populated
    BOOL m_bStopEnumeration;                                // Used to abort XBE Enumeration

    // Private Methods
#ifdef COOL_XDASH
    HRESULT LoadXBEs( char* dirname,                        // Load the XBEs in to our Linked List
                      char* fileExt,
                      unsigned short usLevel );
#else
    HRESULT LoadXBEs( char* dirname,                        // Load the XBEs in to our Linked List
                      char* fileExt );
#endif // COOL_XDASH
    HRESULT GetInfoFromXBE( _XBEINFO* currentXBE,           // Will parse an XBE file and return the Title Name and Publisher Name
                            BOOL& bValidXBE,
                            char* pszDrive );
    HRESULT ProcessFile( char* pszFileName,                 // Process a file and add it to our list if it is valid
                         char* pszDrive,
                         char* pszDirectory,
                         char* pszExtension );

    BOOL StopEnumeration() { return m_bStopEnumeration; };  // Used to determine if we should stop XBE Enumeration
};

#endif // _XBELIST_H_
