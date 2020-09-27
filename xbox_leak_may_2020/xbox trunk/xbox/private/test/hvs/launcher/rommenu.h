/*****************************************************
*** rommenu.h
***
*** Header file for our ROM Menu class.
*** This menu will allow the user to select a ROM and
*** flash it to their Xbox.
***
*** by James N. Helm
*** January 21st, 2002
***
*****************************************************/

#ifndef _ROMMENU_H_
#define _ROMMENU_H_

#include "flashutils.h"
#include "menuscreen.h"

const WCHAR  gc_pwszROMMENU_NO_ROMS_MESSAGE[] =                 L"No ROMs were found.";
static const WCHAR*  gc_pwszROMMENU_FLASHING_ROM_MESSAGE[] =
{
    L"Please wait.  Updating the ROM...",
    L"(Do not reboot or power off!)"
};
const float  gc_fROMMENU_TEXT_YPOS =                            125.0f;

#define ROM_SIZE                                                (1024 * 1024)
#define ROM_FLASH_RETRIES                                       25
#define ROM_VERSION_BYTE_OFFSET                                 0x78
#define ARRAYSIZE(a)                                            (sizeof(a) / sizeof(a[0]))

enum ROMMENU_ACTIONS
{
    ENUM_ROMMENU_NOACTION,
    ENUM_ROMMENU_FLASHCONFIG_YESNO,
    ENUM_ROMMENU_FLASHINGROM_MESSAGE,
    ENUM_ROMMENU_FLASHINGROM_PROGRAM
};

class CROMMenu : public CMenuScreen
{
public:
    // Constructors and Destructor
    CROMMenu( void );
    ~CROMMenu( void );

    // Public Methods
    HRESULT Init( XFONT* pMenuItemFont,                     // Initialize the Menu
                  XFONT* pMenuTitleFont );
    void Action( CUDTexture* pTexture );                    // Renders to the texture and calls any per-frame processing
    void RenderSelector( CUDTexture* pTexture );            // Render the selector for our menu
    void RenderMenuItems( CUDTexture* pTexture );           // Render the menu items properly on our menu
    void HandleInput( enum BUTTONS buttonPressed,           // Handles input (of the BUTTONS) for the current menu
                      BOOL bFirstPress );                   
                                                            
private:                                                    
    // Properties
    PUCHAR m_pucROMImage;                                   // Pointer to our ROM image that will be flashed
    enum ROMMENU_ACTIONS m_eCurrentAction;                  // Used to track the current action of the user
    
    // Methods
    void ProcessMenuActions( void );                        // If the user is performing an action, this will handle it
    HRESULT LoadROMImage( char* pszROMFileName );           // Load the ROM image in to memory
    HRESULT FlashROMImage( void );                          // Flash the ROM image on to the Xbox
    BOOL CheckROMVersion( BYTE* pbyImageVer = NULL,			// Check to make sure the version of the ROM matches the Xbox
						  BYTE* pbyXboxROMVer = NULL );
};

#endif // _ROMMENU_H_