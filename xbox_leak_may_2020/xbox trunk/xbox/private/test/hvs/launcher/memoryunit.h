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
*** Modified
*** 11/30/2001  by James N. Helm -- Removed 'special'
***             code, only working with MU as if I
***             were a game now
***
*****************************************************/

#ifndef _MEMORYUNIT_H_
#define _MEMORYUNIT_H_

const WCHAR gc_pwszMEMORY_UNIT_NAME_PATTERN[] =         L"Memory Unit";
const DWORD gc_dwMEMORY_AREA_BLOCK_SIZE_IN_BYTES =      16384;

class CMemoryUnit
{
public:
    // Contructor and Destructors
    CMemoryUnit( void );
    ~CMemoryUnit( void );

    // Public Methods
    HRESULT Mount( void );                                  // Mount the Memory Unit
    HRESULT Unmount( void );                                // Unmount the Memory Unit
    char* GetDrivePath( void )                              // Get the pointer to the drive path (i.e. "u:\")
        { return m_pszDrivePath; };                         // Get the pointer to our default name
    WCHAR* GetDefaultName( void )
        { return m_pwszDefaultName; };
    WCHAR* GetName( void ) { return m_pwszMUName; };        // Get the name of the MU
    DWORD GetFreeBlocks( void );                            // Get the number of blocks that are free on the MU
    DWORD GetTotalBlocks( void );                           // Get the total number of blocks on the MU
    BOOL IsFormatted( void ) { return m_bFormatted; };      // Returns the format state of the MU
    BOOL IsMounted( void ) { return m_bMounted; };          // Returns the mount state of the MU
    BOOL IsNamed( void );                                   // Used to determine if the MU is named
    HRESULT SetPortSlot( DWORD dwPort,                      // Set the port and the slot this MU will be on
                         DWORD dwSlot );                    
    unsigned int GetIndex( void );                          // Get the Index of the current MU
                                                            
private:                                                    
    char    m_cDriveLetter;                                 // The drive letter the MU is mounted on
    char    m_pszDrivePath[4];                              // The path of the current MU (i.e. "u:\", "f:\")
    WCHAR*  m_pwszDefaultName;                              // The default name of the MU
    WCHAR*  m_pwszMUName;                                   // The name of the MU
    BOOL    m_bMounted;                                     // When the MU is mounted, this is set to TRUE
    BOOL    m_bFormatted;                                   // Used to determine if the MU is formatted
    DWORD   m_dwPort;                                       // The port the MU is on (XDEVICE_PORT0 through XDEVICE_PORT3)
    DWORD   m_dwSlot;                                       // The slot of the Controller the MU is in (XDEVICE_TOP_SLOT or XDEVICE_BOTTOM_SLOT)

    // Private Methods
    HRESULT ProcessMountedMU( void );                       // Process a mounted MU (Get the name, etc)
    HRESULT CalcSpace( DWORD* pdwTotalBlocks,               // Retreives block information for the current MU
                       DWORD* pdwFreeBlocks );
};

#endif // _MEMORYUNIT_H_