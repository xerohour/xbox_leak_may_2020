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

private:
    struct _XBEINFO
    {
        char directory[MAX_PATH + 1];
        char filename[MAX_PATH + 1];
        WCHAR titleName[XBEIMAGE_TITLE_NAME_LENGTH + 1];
    };

    // Private Properties
    CLinkedList< struct _XBEINFO* > m_XBEItems;             // XBE Item list
    BOOL m_bInitialized;                                    // Determines if we've already populated

    // Private Methods
    HRESULT LoadXBEs( char* dirname,                        // Load the XBEs in to our Linked List
                      char* fileExt );
    HRESULT GetInfoFromXBE( struct _XBEINFO* currentXBE,    // Will parse an XBE file and return the Title Name and Publisher Name
                            BOOL& bValidXBE,
                            char* pszDrive );
    HRESULT ProcessFile( char* pszFileName,                 // Process a file and add it to our list if it is valid
                         char* pszDrive,
                         char* pszDirectory,
                         char* pszExtension );
};

#endif // _XBELIST_H_
