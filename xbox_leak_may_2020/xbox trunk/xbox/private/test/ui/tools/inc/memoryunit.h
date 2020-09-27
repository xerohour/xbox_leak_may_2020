/*****************************************************
*** memoryunit.h
***
*** Header file for our Memory Unit class.
*** This class will contain functions and information
*** pertaining to an Xbox Memory Unit
***
*** by James N. Helm
*** March 28th, 2001
***
*****************************************************/

#ifndef _MEMORYUNIT_H_
#define _MEMORYUNIT_H_

#include "memoryarea.h"

class CMemoryUnit : public CMemoryArea
{
public:
    // Contructor and Destructors
    CMemoryUnit();
    ~CMemoryUnit();

    // Public Methods
    HRESULT Format( BOOL bMount=FALSE );                    // Format the Memory Unit
    HRESULT Unformat();                                     // Unformat the Memory Unit
    HRESULT Mount( BOOL bAsGameTitle=FALSE );               // Mount the Memory Unit
    HRESULT Unmount();                                      // Unmount the Memory Unit
    HRESULT Name( WCHAR* wpszName );                        // Name the Memory Unit
    HRESULT RefreshName();                                  // Refresh the name of the Memory Unit (it could change)
    HRESULT GetName( WCHAR* wpszNameBuffer,                 // Get the name of the MU
                     unsigned int uiBufSize );
    WCHAR* GetNamePtr() { return m_pwszMUName; };           // Get the name pointer of the MU
    DWORD GetFreeBlocks();                                  // Get the number of blocks that are free on the MU
    DWORD GetTotalBlocks();                                 // Get the total number of blocks on the MU
    DWORD GetPort() { return m_dwPort; };                   // Get the port of the MU
    DWORD GetSlot() { return m_dwSlot; };                   // Get the slot of the MU
    BOOL IsFormatted() { return m_bFormatted; };            // Returns the format state of the MU
    BOOL IsMounted() { return m_bMounted; };                // Returns the mount state of the MU
    BOOL IsNamed();                                         // Used to determine if the MU is named
    HRESULT SetPortSlot( DWORD dwPort,                      // Set the port and the slot this MU will be on
                         DWORD dwSlot );
    HRESULT MapUDataToOrigLoc();                            // Map the UDATA drive back to it's original location
    HRESULT MapUDataToTitleID( char* pszTitleID );          // Map the UDATA drive letter to the specified Title ID and Mount the MU
    HRESULT MapUDataToTitleID( WCHAR* pwszTitleID );        // Map the UDATA drive letter to the specified Title ID and Mount the MU
    HRESULT MapUDataToTitleID( DWORD dwTitleID );           // Map the UDATA drive letter to the specified Title ID and Mount the MU
    unsigned int GetIndex();                                // Get the Index of the current MU

private:
    // char    m_cDriveLetter;                                 // The drive letter the MU is mounted on
    WCHAR*  m_pwszMUName;                                   // The name of the MU
    BOOL    m_bMounted;                                     // When the MU is mounted, this is set to TRUE
    BOOL    m_bFormatted;                                   // Used to determine if the MU is formatted
    DWORD   m_dwPort;                                       // The port the MU is on (XDEVICE_PORT0 through XDEVICE_PORT3)
    DWORD   m_dwSlot;                                       // The slot of the Controller the MU is in (XDEVICE_TOP_SLOT or XDEVICE_BOTTOM_SLOT)

    // Private Methods
    HRESULT ProcessMountedMU();                             // Process a mounted MU (Get the name, etc)
};

#define MEMORY_UNIT_NAME_PATTERN    L"Memory Unit"

#endif // _MEMORYUNIT_H_