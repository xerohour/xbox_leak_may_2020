/*****************************************************
*** sysinfomenu.cpp
***
*** CPP file for our XShell System Information menu 
*** class.  This menu will display system information
*** and allow the user to scroll through this list.
***
*** by James N. Helm
*** August 7th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "sysinfomenu.h"

extern XFONT* g_pFont;

extern "C"
{
    DECLSPEC_IMPORT
    void
    WINAPI
    HalReadWritePCISpace(
        IN ULONG BusNumber,
        IN ULONG SlotNumber,
        IN ULONG RegisterNumber,
        IN PVOID Buffer,
        IN ULONG Length,
        IN BOOLEAN WritePCISpace
        );
    #define HalReadPCISpace(BusNumber, SlotNumber, RegisterNumber, Buffer, Length) \
        HalReadWritePCISpace(BusNumber, SlotNumber, RegisterNumber, Buffer, Length, FALSE)

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );

    DECLSPEC_IMPORT
    void*
    WINAPI
    MmMapIoSpace(
        IN ULONG_PTR PhysicalAddress,
        IN SIZE_T NumberOfBytes,
        IN ULONG Protect
        );

    DECLSPEC_IMPORT
    void
    WINAPI
    MmUnmapIoSpace(
        IN PVOID BaseAddress,
        IN SIZE_T NumberOfBytes
        );
}

typedef struct _PCI_SLOT_NUMBER {
    union {
        struct {
            ULONG   DeviceNumber:5;
            ULONG   FunctionNumber:3;
            ULONG   Reserved:24;
        } bits;
        ULONG   AsULONG;
    } u;
} PCI_SLOT_NUMBER, *PPCI_SLOT_NUMBER;

FORCEINLINE ULONG
REG_RD32(VOID* Ptr, ULONG Addr)
{
    return *((volatile DWORD*)((BYTE*)(Ptr) + (Addr)));
}


//////////////////////////////////////////////////////
//// Functions / Structures to access machine settings
//////////////////////////////////////////////////////
static DWORD WriteSMC(unsigned char addr, unsigned char value)
{
    return HalWriteSMBusValue(0x20, addr, FALSE, value);
}

static DWORD ReadSMC(unsigned char addr)
{
    DWORD value = 0xCCCCCCCC;
    DWORD status = HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
}

static const char * const vmodes[] =
{
    "SDTV%20SCART%20Analog/Digital",
    "HDTV",
    "VGA",
    "RFU",
    "SDTV%20Analog/Digital",
    "Power%20Off",
    "SDTV%20Analog",
    "No%20Pack",
    "SMC%20Error"
};

static const char * const hwversions[] =
{
    "Unknown",
    "DVT3", 
    "DVT3",
    "DVT4/5"
};

static const char * const lang[] = 
{
    "Unknown",
    "English",
    "Japanese",
    "German",
    "French",
    "Spanish",
    "Italian"
};

static const char * const videostd[] =
{
    "Unknown",
    "NTSC_M",
    "NTSC_J",
    "PAL_I"
};

char* GetHWVersion(unsigned &hwver)
{
    // #define PAGE_SIZE                   4096
    #define ROM_SIZE                    (1024 * 1024)
    #define ROM_VERSION_OFFSET          30 // In DWORDs (0x78 is the absolute offset)
    #define ROM_VERSION_BYTE_OFFSET     0x78
    #define FLASH_BASE_ADDRESS          0xFFF00000
    #define FLASH_REGION_SIZE           (0xFFFFFFFF-FLASH_BASE_ADDRESS-1)

    LPBYTE RomBase = (LPBYTE)MmMapIoSpace(FLASH_BASE_ADDRESS, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);

    if(RomBase == NULL) return (char*)hwversions[0];

    hwver = RomBase[ROM_VERSION_BYTE_OFFSET];

    MmUnmapIoSpace(RomBase, PAGE_SIZE);

    //
    // Init table version < 0x40: Old DVT 3 Boxes with NV2A A02 old metal 7 (NVCLK 155, CPU 665)
    // Init table version = 0x40: New DVT 3 Boxes with NV2A A02 new metal 7 (Faster)
    // Init table version >= 0x46: DVT4
    //
    //DebugPrint("hwver: 0x%X\n", hwver);

    if(hwver < 0x40) return (char*)hwversions[1];
    if(hwver == 0x40) return (char*)hwversions[2];
    if(hwver >= 0x46) return (char*)hwversions[3];
    return (char*)hwversions[0];
}


void DumpClocks(char *buffer, unsigned hwver)
{
    #define NV_PRAMDAC_NVPLL_COEFF 0x00680500 /* RW-4R */
    const float F_XTAL_135 = 13.5f;
    const float F_XTAL_166 = 16.6667f;

    PCI_SLOT_NUMBER SlotNumber;
    DWORD MPLLCoeff;
    DWORD m;
    DWORD n;
    DWORD p;
    DWORD fsbspeed;
    DWORD vcofreq;
    DWORD nvclk;
    DWORD nvpll;
    DWORD fsb_pdiv;
    DWORD mem_pdiv;
    DWORD mclk;
    DWORD cpuspeed;
    float XTAL;

    if(hwver >= 0x46) 
    {
        XTAL = F_XTAL_166;
    }
    else
    {
        XTAL = F_XTAL_135;
    }

    //
    // Read CR_CPU_MPLL_COEFF 
    //
    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = 0;
    SlotNumber.u.bits.FunctionNumber = 3;
    HalReadPCISpace(0, SlotNumber.u.AsULONG, 0x6C, &MPLLCoeff, sizeof(MPLLCoeff));

    m = MPLLCoeff & 0xFF;
    n = (MPLLCoeff >> 8) & 0xFF;

    if(m != 0)
    {
        //
        // Calculate cpu frequency
        //
        fsbspeed = (DWORD)((XTAL / m) * n);

        cpuspeed = (DWORD)((XTAL / m) * n * 11 / 2);
    
        //
        // Calculate nvclk
        //
        nvpll = REG_RD32((void*)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PRAMDAC_NVPLL_COEFF);
        m = nvpll & 0xFF;
        n = (nvpll >> 8)  & 0xFF;
        p = (nvpll >> 16) & 0xFF;
    
        nvclk = (m != 0) ? (DWORD)((n * XTAL / (1 << p) / m)) : 0;
    
        //
        // Calculate vco
        //
        m = MPLLCoeff & 0xFF;
        n = (MPLLCoeff >> 8) & 0xFF;
        fsb_pdiv = (MPLLCoeff >> 16) & 0xF;
        
        vcofreq = (DWORD)((XTAL / m) * (fsb_pdiv * 2 * n));
    
        //
        // Calculate mclk
        //
        mem_pdiv = (MPLLCoeff >> 20) & 0xF;
        mclk = (DWORD)(vcofreq / (2 * mem_pdiv));

        sprintf(buffer, "Crystal:%s, FSB:%d, CPU:%d, NVCLK:%d, VCO:%d, MCLK:%d",
            (hwver >= 0x46) ? "16.6" : "13.5", fsbspeed, cpuspeed, nvclk, vcofreq, mclk);
    }
    else
    {
        sprintf(buffer, "unknown");
    }
}


void GetDriveID(char* device, char* model, char* serial, char* firmware)
{
    unsigned i;
    DWORD returned;
    NTSTATUS status;
    HANDLE fileHandle;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;
    char buffer[sizeof(ATA_PASS_THROUGH) + 512];
    PATA_PASS_THROUGH atapt = (PATA_PASS_THROUGH)buffer;


    RtlInitObjectString(&VolumeString, device);
    InitializeObjectAttributes(&ObjA,&VolumeString,OBJ_CASE_INSENSITIVE,NULL,NULL);

    status = NtCreateFile(&fileHandle,
                            SYNCHRONIZE|GENERIC_READ,
                            &ObjA,
                            &IoStatusBlock,
                            0,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ, FILE_OPEN,
                            FILE_SYNCHRONOUS_IO_NONALERT);

    atapt->DataBufferSize = 512;
    atapt->DataBuffer = atapt + 1;

    atapt->IdeReg.bFeaturesReg     = 0;
    atapt->IdeReg.bSectorCountReg  = 0;
    atapt->IdeReg.bSectorNumberReg = 0;
    atapt->IdeReg.bCylLowReg       = 0;
    atapt->IdeReg.bCylHighReg      = 0;
    atapt->IdeReg.bDriveHeadReg    = 0;
    atapt->IdeReg.bHostSendsData   = 0;

    if(strstr(device, "CdRom") != NULL) atapt->IdeReg.bCommandReg = 0xa1;
    else atapt->IdeReg.bCommandReg = 0xec;

    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             FALSE);
    NtClose(fileHandle);

    PIDE_IDENTIFY_DATA IdData = (PIDE_IDENTIFY_DATA)atapt->DataBuffer;

    for ( i=0; i<sizeof(IdData->ModelNumber); i+=2 )
    {
        model[i + 0] = IdData->ModelNumber[i + 1];
        model[i + 1] = IdData->ModelNumber[i + 0];
    }
    model[i] = 0;
    //DebugPrint("Model Number: %s\n", model);

    for ( i=0; i<sizeof(IdData->SerialNumber); i+=2 )
    {
        serial[i + 0] = IdData->SerialNumber[i + 1];
        serial[i + 1] = IdData->SerialNumber[i + 0];
    }
    
    serial[i] = 0;
    //DebugPrint("Serial Number: %s\n", serial);

    for ( i=0; i<sizeof(IdData->FirmwareRevision); i+=2 )
    {
        firmware[i + 0] = IdData->FirmwareRevision[i + 1];
        firmware[i + 1] = IdData->FirmwareRevision[i + 0];
    }
    
    firmware[i] = 0;
    //DebugPrint("Firmware Revision: %s\n", firmware);
}


// Trim spaces from the right side of a string
void RTrimString( char* pszString )
{
    // Trim spaces from the end of the string
    for( unsigned int x = strlen( pszString ) - 1; x >= 0; x-- )
    {
        if( pszString[x] != ' ' )
        {
            break;
        }
        else
        {
            pszString[x] = '\0';
        }
    }
}

void GetPeripheral(char *buffer);

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

extern CXSettings           g_XboxSettings;         // Settings that are stored on the Xbox
extern CSoundCollection     g_Sounds;               // Used to generate all sounds for the XShell
extern BOOL                 g_bDisplayUpArrow;      // Used to determine if we should render the Up Arrow
extern BOOL                 g_bDisplayDownArrow;    // Used to determine if we should render the Down Arrow

// Constructors
CSysInfoMenu::CSysInfoMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_uiTopItemIndex( 0 ),
m_bDropKeyPress( TRUE )
{
    XDBGTRC( APP_TITLE_NAME_A, "CSysInfoMenu::CSysInfoMenu(ex)" );

    m_uiNumItems = SYSINFOMENU_NUM_ITEMS;
    SetSelectedItem( 0 );

    for( unsigned int x = 0; x < SYSINFOMENU_NUM_ITEMS; x++ )
    {
        m_pawszMenuItems[x] = new WCHAR[MAX_PATH+1];
        ZeroMemory( m_pawszMenuItems[x], sizeof( WCHAR ) * MAX_PATH+1 );
    }

    SetTitle( MENU_TITLE_SYSINFO );

    m_dwTickCount = GetTickCount();
}


// Destructor
CSysInfoMenu::~CSysInfoMenu()
{
    // Clean up memory
    for( unsigned int x = 0; x < SYSINFOMENU_NUM_ITEMS; x++ )
    {
        if( m_pawszMenuItems[x] )
        {
            delete[] m_pawszMenuItems[x];
            m_pawszMenuItems[x] = NULL;
        }
    }    
}


// Draws a menu on to the screen
void CSysInfoMenu::Action( CUDTexture* pTexture )
{
    g_XboxSettings.LoadSettings();
    if( g_XboxSettings.SettingsHaveChanged() )
    {
        m_bUpdateTexture = TRUE;
    }

    // Make sure our texture updates once a second
    if( GetTickCount() > ( m_dwTickCount + 1000 ) )
    {
        m_dwTickCount = GetTickCount();

        m_bUpdateTexture = TRUE;
    }

    // Only update the texture if we need to, otherwise return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else
    {
        m_bUpdateTexture = FALSE;
    }

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Update our Font Height
    XFONT_SetTextHeight( g_pFont, ITEM_OPTIONMENU_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_OPTIONMENU_FONT_HEIGHT;

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    //
    // Populate our Menu Item WCHAR array
    //
    unsigned int uiIndex = 0;
    UCHAR pszOutBuffer[MAX_PATH+1];
    UCHAR pszBuffer[MAX_PATH+1];
    WCHAR pwszBuffer[MAX_PATH+1];

    //
    // Hardware Version Info
    //
    unsigned uiBaseInitTable = 0;
    char* pszHWVersion;
    char pszSMCVersion[4];

    // SMC Version
    pszSMCVersion[0] = (char)ReadSMC(0x01);
    pszSMCVersion[1] = (char)ReadSMC(0x01);
    pszSMCVersion[2] = (char)ReadSMC(0x01);
    pszSMCVersion[3] = '\0';


    pszHWVersion = GetHWVersion( uiBaseInitTable );

    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Hardware Version Information:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   %hs %hs,  SMC:%hs,  GPU:%X,  MCP:%X", pszHWVersion, (XboxHardwareInfo->Flags&XBOX_HW_FLAG_DEVKIT_KERNEL)?"DEVKIT":"RETAIL", pszSMCVersion, XboxHardwareInfo->GpuRevision, XboxHardwareInfo->McpRevision );


    //
    // Clock Speeds
    //
    char pszClocks[255];
    pszClocks[0] = '\0';

    DumpClocks( pszClocks, uiBaseInitTable );

    // Clock Speeds 1
    char pszClocks1[255];
    ZeroMemory( pszClocks1, 255 );
    char* pszClocks2 = NULL;

    pszClocks2 = strstr( pszClocks, "NVCLK" );  // REVIEW: Should we care about checking for this error case?
    if( NULL == pszClocks2 )
    {
        XDBGERR( APP_TITLE_NAME_A, "CSysInfoMenu::Action():Failed to find our expected string!!" );
    }
    strncpy( pszClocks1, pszClocks, pszClocks2 - pszClocks - 2 );

    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Clock Speeds 1:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   %hs", pszClocks1 );

    // Clock Speeds 2
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Clock Speeds 2:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   %hs", pszClocks2 );


    //
    // Disk Manufacturers
    //
    char cdModel[64], cdSerial[64], cdFirmware[64];
    char hdModel[64], hdSerial[64], hdFirmware[64];
    
    cdModel[0] = '\0';
    cdSerial[0] = '\0';
    cdFirmware[0] = '\0';
    hdModel[0] = '\0';
    hdSerial[0] = '\0';
    hdFirmware[0] = '\0';

    GetDriveID( "\\Device\\CdRom0", cdModel, cdSerial, cdFirmware );
    GetDriveID( "\\Device\\Harddisk0\\Partition0", hdModel, hdSerial, hdFirmware );

    RTrimString( cdModel );
    RTrimString( cdFirmware );
    RTrimString( hdModel );
    RTrimString( hdFirmware );

    // DVD Drive
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"DVD Drive:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   %hs (%hs)", cdModel, cdFirmware );

    // Hard Drive
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Hard Drive:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   %hs (%hs)", hdModel, hdFirmware );


    //
    // Memory Type
    //
    #define NV_PEXTDEV_BOOT_0                   0x00101000 // private\windows\directx\dxg\d3d8\se\nv_ref_2a.h
    #define XPCICFG_GPU_MEMORY_REGISTER_BASE_0  0xFD000000 // private\ntos\inc\xpcicfg.h

    // Get the total amount of memory available
    MEMORYSTATUS memStatus;
    ZeroMemory( &memStatus, sizeof( memStatus ) );

    GlobalMemoryStatus( &memStatus );

    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Memory Type:" );

    if( ( REG_RD32( (void*)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PEXTDEV_BOOT_0 ) & 0x0000C0000 ) == 0 )
        _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   %dMB (Micron)", memStatus.dwTotalPhys / ( 1024 * 1024 ) );
    else
        _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   %dMB (Samsung)", memStatus.dwTotalPhys / ( 1024 * 1024 ) );


    //
    // SMC Information
    //
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"SMC Information:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   CPUTemp:%d,  AIRTemp:%d,  Fan:%d,  Error:%d",  ReadSMC(0x09), ReadSMC(0x0A), ReadSMC(0x10), ReadSMC(0x0F) );


    //
    // USB Hub Information
    //
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"USB Hub Installed:" );
    
    if( XboxHardwareInfo->Flags & XBOX_HW_FLAG_INTERNAL_USB_HUB )
    {
        _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   Yes" );
    }
    else
    {
        _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"   No" );
    }

    ASSERT( SYSINFOMENU_NUM_ITEMS == uiIndex );

    //
    // Draw our Menu Items
    //

    for( unsigned int x = m_uiTopItemIndex; x < ( SYSINFOMENU_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ); x++ )
    {
        // Make sure we don't try to draw too many items
        if( x == GetNumItems() )
        {
            break;
        }

        float XPos = MENUBOX_TEXTAREA_X1;
        float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( x - m_uiTopItemIndex ) );

        DWORD dwColor;
        if( ( x % 2 ) == 0 )
        {
            dwColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
        }
        else
        {
            dwColor = COLOR_DARK_GREEN2;
        }

        pTexture->DrawText( XPos, YPos, dwColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_pawszMenuItems[x] );
    }

    // Correct our Font Height
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    // Unlock the texture
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CSysInfoMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
{
    switch( joystick )
    {
    // This is the LEFT Thumb Joystick on the controller
    case JOYSTICK_LEFT:
        {
            // Y
            if( ( nThumbY < 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) ) // Move the left joystick down
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_DOWN, bFirstYPress );
            }
            else if( ( nThumbY > 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) )// Move left joystick up
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_UP, bFirstYPress );
            }

            // X
            if( ( nThumbX < 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) ) // Move the left joystick left
            {
            }
            else if( ( nThumbX > 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) )// Move the left joystick right
            {
            }
            break;
        }
    }
}


// Handles input (of the CONTROLS) for the current menu
void CSysInfoMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // If this is a repeat button press, let's delay a bit
    if( bFirstPress )
    {
        m_bDropKeyPress = FALSE;

        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = KEY_PRESS_INITIAL_DELAY;
    }
    else // Check to see if the repeat press is within our timer, otherwise bail
    {
        // If the interval is too small, bail
        if( ( GetTickCount() - m_keyPressDelayTimer ) < m_keyPressDelayInterval )
        {
            return;
        }
        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = KEY_PRESS_REPEAT_DELAY;
    }

    if( m_bDropKeyPress )
    {
        m_bDropKeyPress = FALSE;

        return;
    }

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            if( m_uiTopItemIndex != 0 )
            {
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

                m_uiTopItemIndex -= SYSINFOMENU_NUM_ITEMS_TO_DISPLAY;

                AdjustDisplayArrows();
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( m_uiTopItemIndex < ( GetNumItems() - SYSINFOMENU_NUM_ITEMS_TO_DISPLAY ) )
            {
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

                m_uiTopItemIndex += SYSINFOMENU_NUM_ITEMS_TO_DISPLAY;

                AdjustDisplayArrows();
            }

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CSysInfoMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, or input is disabled, let's bail
    if( ( !bFirstPress ) || ( m_bDropKeyPress ) )
    {
        m_bDropKeyPress = FALSE;

        return;
    }

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
            }

            break;
        }
    case BUTTON_B:
        {
            // Go Back one menu
            if( GetParent() )
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    }
}


// Initialize the Menu
HRESULT CSysInfoMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CSysInfoMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    return hr;
}


// Should be called whenever a user leaves the menu
void CSysInfoMenu::Leave( CXItem* pItem )
{
    // Call our base class Leave
    CMenuScreen::Leave( pItem );

    g_bDisplayUpArrow = FALSE;
    g_bDisplayDownArrow = FALSE;
}


// This will be called whenever the user enters this menu
void CSysInfoMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Set our selected item to the top of the menu
    // SetSelectedItem( 0 );

    // Make sure the proper arrows are displayed
    AdjustDisplayArrows();

    // Make sure we don't process the first keypress, and set our menu item back to 0
    m_bDropKeyPress = TRUE;
}


// Adjust the UP and DOWN arrows on the screen
void CSysInfoMenu::AdjustDisplayArrows()
{
    // Decide if we need to turn on the Up Arrow
    if( 0 == m_uiTopItemIndex )
    //
    // We can turn off the Up Arrow, because there is nothing above us
    //
    {
        g_bDisplayUpArrow = FALSE;
    }
    else
    //
    // We have to turn on the Up Arrow, because a menu choice is above us
    //
    {
        g_bDisplayUpArrow = TRUE;
    }

    // Decide if we need to turn on the Down Arrow
    if( SYSINFOMENU_NUM_ITEMS > ( m_uiTopItemIndex + SYSINFOMENU_NUM_ITEMS_TO_DISPLAY ) )
    //
    // We have to turn on the Down Arrow, because there are menu choices below us
    //
    {
        g_bDisplayDownArrow = TRUE;
    }
    else
    //
    // We can turn off the Down arrow, because nothing is below us
    //
    {
        g_bDisplayDownArrow = FALSE;
    }
}
