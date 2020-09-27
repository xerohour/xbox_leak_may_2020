/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    oobauditp.cpp

Abstract:

    This module implements routine to reset EEPROM settings

--*/

#include "oobauditp.h"

#define MAX_RETRIES 3

//
// Global statically allocated video object
//

CXBoxVideo g_Video;

NTSTATUS
ResetEEPROMUserSettings(
    VOID
    )
/*++

Routine Description:

    Reset user settings in EEPROM to their defaults

Arguments:

    None

Return Value:

    Status of the operation

--*/
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    XBOX_USER_SETTINGS* userSettings;
    NTSTATUS Status;
    DWORD type, size;

    eeprom = (EEPROM_LAYOUT*)buf;
    userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
    Status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);

    if (NT_SUCCESS(Status)) {
        RtlZeroMemory(userSettings, sizeof(*userSettings));

        if (XGetGameRegion() == XC_GAME_REGION_JAPAN) {
            userSettings->Language = XC_LANGUAGE_JAPANESE;
            userSettings->TimeZoneBias = -540;
            strcpy(userSettings->TimeZoneStdName, "TST");
            strcpy(userSettings->TimeZoneDltName, "TST");
        }

        userSettings->Checksum = ~XConfigChecksum(userSettings, sizeof(*userSettings));
        Status = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, buf, sizeof(buf));
    }

    return Status;
}

VOID
DrawMessage(
    LPCWSTR pcszTextLine1,
    LPCWSTR pcszTextLine2,
    DWORD dwColor
    )
{
    ASSERT(pcszTextLine1);

    g_Video.ClearScreen(dwColor);

    g_Video.DrawText(SCREEN_X_CENTER,
                       MENUBOX_TEXTAREA_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       pcszTextLine1);

    if (NULL != pcszTextLine2)
    {
        g_Video.DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1 + FONT_DEFAULT_HEIGHT + ITEM_VERT_SPACING,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszTextLine2);
    }


    WCHAR szBuildString[64];
    wsprintfW(szBuildString, L"XTL Version %hs", VER_PRODUCTVERSION_STR);
    g_Video.DrawText(SCREEN_X_CENTER,
                       MENUBOX_BUILDNUM_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       szBuildString);

    g_Video.ShowScreen();

}

void __cdecl main()
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    int tries = 0;

    g_Video.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);
    g_Video.ClearScreen(COLOR_BLACK);
    g_Video.ShowScreen();

    //
    // Reset the manufacturing bit.
    // Per manufacturing request: We will retry MAX_RETRIES times before
    // giving up as failed.
    //

    while (tries < MAX_RETRIES && !NT_SUCCESS(Status)) {
        tries++;
        Status = ResetEEPROMUserSettings();
    }

    //
    // Give status. We don't assume the user reads english, or understands the terminology
    // Although there is text, the background color is the distinguishing feature.
    // RED == Unable to update EEPROM data.
    // GREEN == EEPROM update succeeded.
    //

    if (!NT_SUCCESS(Status)) {
        DbgPrint("Reset EEPROM failed (returned 0x%x)\r\n", Status);
        DrawMessage(L"Out Of Box Audit", L"Failed to update Xbox configuration.", COLOR_TV_RED);
    } else {
        DrawMessage(L"Out Of Box Audit", L"Xbox successfully updated.", COLOR_DARK_GREEN);
    }

    Sleep(INFINITE);
}

