#ifndef _HVSLFUNCTIONS_H_
#define _HVSLFUNCTIONS_H_

#include <xfont.h>
#include "xitem.h"
#include "helpmenu.h"
#include "messagemenu.h"
#include "keypadmenu.h"
#include "keyboardmenu.h"
#include "choosesavemenu.h"
#include "memareaselectmenu.h"
#include "yesnomenu.h"
#include "memoryunit.h"
#include "xbeinfo.h"

const unsigned int g_cuiFILE_LINE_SIZE =        4096;       // Size of the buffer to read info from our file
const unsigned int g_cuiCONFIG_ITEM_SIZE =      1024;       // Size of out standard Config Item size

// Memory Unit BitMasks used for detection
static const DWORD gc_dwMUBitMask[] = { XDEVICE_PORT0_TOP_MASK,
                                        XDEVICE_PORT0_BOTTOM_MASK,
                                        XDEVICE_PORT1_TOP_MASK,
                                        XDEVICE_PORT1_BOTTOM_MASK,
                                        XDEVICE_PORT2_TOP_MASK,
                                        XDEVICE_PORT2_BOTTOM_MASK,
                                        XDEVICE_PORT3_TOP_MASK,
                                        XDEVICE_PORT3_BOTTOM_MASK };

void InitializeMenus( void );                           // Initialize our Menus
CHelpMenu* GetHelpMenuPtr( void );                      // Return a pointer to the Help Menu
CKeypadMenu* GetKeypadPtr( void );                      // Return a pointer to the Virtual Keypad
CKeyboardMenu* GetKeyboardPtr( void );                  // Return a pointer to the Virtual Keyboard
CChooseSaveMenu* GetChooseSavePtr( void );              // Return a pointer to the Choose Saved Game Menu
CMemAreaSelectMenu* GetMemAreaSelectPtr( void );        // Return a pointer to the Memory Area Select Menu
CMessageMenu* GetMessagePtr( void );                    // Return a pointer to the Message Menu
CYesNoMenu* GetYesNoPtr( void );                        // Return a pointer to the YES / NO Menu
                                                        
/////////                                               
// Fonts                                                
/////////                                               
void InitializeFonts( void );                           // Initialize our fonts
void CleanupFonts( void );                              // Close and clean up our fonts
XFONT* GetHeaderFont( void );                           // Return a pointer to our Header Font
XFONT* GetBodyFont( void );                             // Return a pointer to our Body Font
XFONT* GetButtonFont( void );                           // Return a pointer to our Button Font

/////////                                               
// Menus                                                
/////////             
CXItem* GetCurrentMenu( void );                         // Return a pointer to the current menu
void SetCurrentMenu( CXItem* pMenuPtr );                // Set the current Menu
CXItem* GetPreviousMenu( void );                        // Return a pointer to the previous Menu
void SetPreviousMenu( CXItem* pMenuPtr );               // Set the previous Menu
                                                        
////////////////////////                                
// Memory Units / Areas                                 
////////////////////////                                
void InitializeMUs( void );                             // Initialize our MUs and mount any that need mounting
BOOL GetMUsChanged( void );                             // Can be used to determine if there have been any changes to our Memory Units
void ProcessMUs( void );                                // Check to see if an MU has been inserted or removed, and properly Mounts, or Dismounts the MUs
CMemoryUnit* GetMUPtr( unsigned int uiIndex );          // Get a pointer to a memory unit based on index

////////////////////////////
// Configuration management
////////////////////////////
HRESULT LoadXBEConfigs( CXBEInfo* pXBEInfo,             // Load an XBE configuration from a file .DAT file
                        const char* pszLoadPath );      
                                                        
HRESULT SaveXBEConfigs( CXBEInfo* pXBEInfo,             // Save the current XBE configuration to a .DAT file
                        const char* pszSavePath );

/////////
// Other
/////////                                           
BOOL DeleteDirectory( const char* pszPath );            // Delete a directory and all files within
BOOL FileExists( const char* pszFileName );             // Will return TRUE if a file exists, otherise FALSE
long ThumbStickScale( int iStickValue,                  // Will return a scaled number
                      long lMinValue,
                      long lMaxValue );
HRESULT ProcessConfigFile( CXBEInfo* pXBEInfo );        // Process a configuration file for a specific XBE
CXBEInfo* GetLauncherXBEInfo( void );                   // Return a pointer to our HVS Launcher XBE Info object
void InitLauncherXBEInfo( void );                       // Initialize the HVS Launcher Config if one exists

#endif // _HVSLFUNCTIONS_H_