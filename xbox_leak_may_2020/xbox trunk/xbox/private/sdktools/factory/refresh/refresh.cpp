#include "pch.h"
#include <xboxp.h>
#include <xconfig.h>
#include "xboxvideo.h"
#include "smcdef.h"
#include "xboxverp.h"

#if DBG
extern "C" ULONG XDebugOutLevel;
#endif

COBJECT_STRING PrimaryHarddisk = CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\Partition0") );
COBJECT_STRING HdPartition1 = CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\Partition1") );
COBJECT_STRING CDrive = CONSTANT_OBJECT_STRING( OTEXT("\\??\\C:") );

//
// Copied from recovery.cpp.
//
VOID
EjectDvd (VOID)
{
    //
    // This requires matching SMC firmware and DVD firmware (DVT3 and onward)..
    //

    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_DVD_TRAY_OPERATION,
        SMC_DVD_TRAY_OPERATION_OPEN);
}


CXBoxVideo* g_pVideo;


VOID
DrawMessage(LPCWSTR pcszTextLine1, LPCWSTR pcszTextLine2, DWORD dwColor)
{
    ASSERT(g_pVideo && pcszTextLine1);

    g_pVideo->ClearScreen(dwColor);

    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_TEXTAREA_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       pcszTextLine1);

    if (NULL != pcszTextLine2)
    {
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1 + FONT_DEFAULT_HEIGHT + ITEM_VERT_SPACING,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszTextLine2);
    }


    WCHAR szBuildString[64];
    wsprintfW(szBuildString, L"XTL Version %hs", VER_PRODUCTVERSION_STR);
    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_BUILDNUM_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       szBuildString);

    g_pVideo->ShowScreen();

}


BOOL FResetHD(void)
{
    HANDLE h;
    OBJECT_ATTRIBUTES oa;
    LARGE_INTEGER WriteOffset;
    IO_STATUS_BLOCK ioStatus;
    NTSTATUS status = STATUS_SUCCESS;
    BYTE rgbSectorBytes[XBOX_HD_SECTOR_SIZE];
    int i;
    BOOL f;
    NTSTATUS st;

    // Nuke the cache DB sector and the config sectors
    memset(rgbSectorBytes, 0, sizeof rgbSectorBytes);
    InitializeObjectAttributes(&oa, (POBJECT_STRING)&PrimaryHarddisk,
        OBJ_CASE_INSENSITIVE, NULL, NULL);
    st = NtOpenFile(&h, SYNCHRONIZE | GENERIC_ALL, &oa, &ioStatus, 0,
        FILE_SYNCHRONOUS_IO_NONALERT);
    f = NT_SUCCESS(st);
    for(i = XBOX_BOOT_SECTOR_INDEX;
        i < (XBOX_CONFIG_SECTOR_INDEX + XBOX_NUM_CONFIG_SECTORS); ++i)
    {
        /* We'll preserve the refurb data and we'll preserve the online
         * machine credentials.  We don't check the online credentials for
         * validity, since obliterating them would make them invalid anyway */
        if(i != XBOX_REFURB_INFO_SECTOR_INDEX && i !=
            (XBOX_CONFIG_SECTOR_INDEX + 1))
        {
            WriteOffset.QuadPart = i * XBOX_HD_SECTOR_SIZE;
            st = NtWriteFile(h, NULL, NULL, NULL, &ioStatus, rgbSectorBytes,
                XBOX_HD_SECTOR_SIZE, &WriteOffset);
            f = NT_SUCCESS(st) && f;
        }
    }
    NtClose(h);

    // Nuke the data partition
    if(XapiFormatFATVolume((POBJECT_STRING)&HdPartition1)) {
        st = IoCreateSymbolicLink((POBJECT_STRING)&CDrive,
            (POBJECT_STRING)&HdPartition1);
        if(NT_SUCCESS(st)) {
            // Recreate the TDATA and UDATA directories
            // Do this with a bitwise OR rather than a binary OR to avoid the
            // short-circuit
            if(((!CreateDirectory("C:\\TDATA", NULL)) |
                    (!CreateDirectory("C:\\UDATA", NULL))) == 0)
                return f;
        }
    }
            
    return FALSE;
}

BOOL FResetUserEEPROMSettings(void)
{
    ULONG DvdRegion;
    EEPROM_LAYOUT Data;
    XBOX_USER_SETTINGS *UserData;
    NTSTATUS st;
    ULONG Type, Size;

    // Reset the user data, but preserve the DVD region, if set
    st = ExQueryNonVolatileSetting(XC_MAX_ALL, &Type, &Data, sizeof Data,
        &Size);
    if(!NT_SUCCESS(st))
        return FALSE;
    UserData = (XBOX_USER_SETTINGS *)Data.UserConfigSection;

    DvdRegion = UserData->DvdRegion;
    if(DvdRegion < 1 || DvdRegion > 6)
        // guard against bogus setting
        DvdRegion = 0;

    memset(UserData, 0, sizeof *UserData);
    UserData->DvdRegion = DvdRegion;
    if (XGetGameRegion() & XC_GAME_REGION_JAPAN) {
        UserData->Language = XC_LANGUAGE_JAPANESE;
        UserData->TimeZoneBias = -540;
        strcpy(UserData->TimeZoneStdName, "TST");
        strcpy(UserData->TimeZoneDltName, "TST");
    }

    UserData->Checksum = ~XConfigChecksum(UserData, sizeof *UserData);
    st = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, &Data, sizeof Data);
    return NT_SUCCESS(st);
}


//======================== The main function
void __cdecl main()
{
    BOOL f;

#if DBG
    XDebugOutLevel = XDBG_EXIT;
#endif

    //
    // Initialize our graphics.
    //
    if ((g_pVideo = new CXBoxVideo) == NULL) {

        XDBGWRN("factory", "Couldn't allocate video objects");
        Sleep(INFINITE);
    }

    g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );
    g_pVideo->ClearScreen(COLOR_BLACK);
    g_pVideo->ShowScreen();

    f = FResetHD();
    f = FResetUserEEPROMSettings() && f;

    //
    // Give status. We don't assume the user reads english, or understands the terminology
    // Although there is text, the background color is the distinguishing feature.
    // RED == Unable to update EEPROM data.
    // GREEN == EEPROM update succeeded.
    //
    if (!f)
    {
        DrawMessage (L"System refresh", L"Failed to refresh system", COLOR_TV_RED);
    }
    else
    {
        DrawMessage (L"System refresh", L"Xbox successfully updated.", COLOR_DARK_GREEN);
    }

    //
    // We are done, eject the DVD and wait forever.
    //
    EjectDvd ();
    Sleep (INFINITE);

    return;
}
