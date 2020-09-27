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
#include "xbeinfo.h"
#include "configitems.h"

#define XBE_INITIAL_HEADER_SIZE     4096

class CXBEList
{
public:
    // Constructors and Destructors
    CXBEList( void );
    ~CXBEList( void );

    // Methods
    HRESULT Init( char* dirname, char* fileExt );               // Initialize the list from a directory, loading files of a certain type
    unsigned int GetNumItems( void )                            // Get the number of items currently in our list
        { return m_XBEItems.GetNumItems(); };
    char* GetXBEName( unsigned int index )                      // Get the name of an XBE by index
        { return m_XBEItems[index]->GetFilename(); };
    char* GetXBEDir( unsigned int index )                       // Get the directory of an XBE by index
        { return m_XBEItems[index]->GetDirectory(); };
    char* GetXBEExt( unsigned int index )                       // Get the extension of an XBE by index
        { return m_XBEItems[index]->GetFileExtension(); };
    WCHAR* GetXBETitle( unsigned int index )                    // The title name of the XBE as in the XBE CERT Information
        { return m_XBEItems[index]->GetTitleName(); };
    CXBEInfo* GetXBEInfoPtr( unsigned int uiIndex )             // Return a pointer to our CXBEInfo item
        { return m_XBEItems[uiIndex]; };

    BOOL MoveFirst( void ) { return m_XBEItems.MoveFirst(); };  // Move to the first XBE in our list
    BOOL MovePrev( void ) { return m_XBEItems.MovePrev(); };    // Move to the next XBE in our list
    BOOL MoveNext( void ) { return m_XBEItems.MoveNext(); };    // Move to the next XBE in our list
    BOOL MoveLast( void ) { return m_XBEItems.MoveLast(); };    // Move to the last XBE in our list
    HRESULT MoveTo( unsigned int uiIndex)                       // Move to the XBE specified by the index
        { return m_XBEItems.MoveTo( uiIndex ); };
    unsigned int GetCurrentIndex( void )                        // Get the index of the current item
        { return m_XBEItems.GetCurrentIndex(); };
    CXBEInfo* GetCurrentXBEInfoPtr( void )                      // Return a pointer to our CXBEInfo item
        { return m_XBEItems.GetCurrentNode(); };
    unsigned int GetIndexOfXBE( char* pszFilename );            // Find the index of an XBE by filename

private:
    // Private Properties
    CLinkedList< CXBEInfo* > m_XBEItems;                    // XBE Item list
    BOOL m_bInitialized;                                    // Determines if we've already populated

    // Private Methods
    HRESULT LoadXBEs( char* pszDirname,                     // Load the XBEs in to our Linked List
                      char* pszFileExt );
    HRESULT GetInfoFromXBE( CXBEInfo* currentXBE,           // Will parse an XBE file and return the Title Name and Publisher Name
                            BOOL& bValidXBE,
                            char* pszDrive );
    HRESULT ProcessFile( char* pszFileName,                 // Process a file and add it to our list if it is valid
                         char* pszDrive,
                         char* pszDirectory,
                         char* pszExtension );
};

#endif // _XBELIST_H_
