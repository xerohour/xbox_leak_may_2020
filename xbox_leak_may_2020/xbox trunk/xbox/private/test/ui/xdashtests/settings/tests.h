#ifndef _TESTS_H_
#define _TESTS_H_

namespace xdashsettings
{
    #define VARIATION_BUFFER_SIZE  1024

    /*

    Copyright (c) 2000 Microsoft Corporation

    Module Name:

        Tests.h

    Abstract:



    Author:

        Jim Helm (jimhelm)

    Environment:

        XBox

    Revision History:
        10-18-2000  Created

    */

    //
    //  Settings Area Tests
    //

    //    01234567890123456789012345678901

    DWORD ClockInitialBootGoodBattery     ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockInitialBootBadBattery      ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockStartRange                 ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockEndRange                   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockMonthsUseValidNumDays      ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockFebUseValidNumDaysLeapYear ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockDatesStoredToXDisk         ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockSetUsingXZone              ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockXZoneSetsDSTInfo           ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockManualSetRemovesDSTInfo    ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockDSTUpdatesXBoxAlreadyOn    ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockDSTUpdatesXBoxPoweredOn    ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockXZoneSetClockPowerOff      ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockXZoneSetClockUnplugPower   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockXZoneSetClockUnplugBB      ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD ClockNewSettingUpdatesClockOnly ( HANDLE hLog, CStepsLog* pStepsLog );

    DWORD LangXBoxRegionsCorrectLanguage  ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD LangNewLangDisplayAndConfig     ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD LangNewLangUpdatesLangOnly      ( HANDLE hLog, CStepsLog* pStepsLog );

    DWORD VideoInitalBootCorrectDefault   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD VideoNewSettingDisplayAndConfig ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD VideoNewSettingUpdatesVideoOnly ( HANDLE hLog, CStepsLog* pStepsLog );

    DWORD AudioInitalBootCorrectDefault   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD AudioChoicesNoAVPack            ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD AudioChoicesWithAVPack          ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD AudioNewSettingConfig           ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD AudioAVRemovedAfterOptionSet    ( HANDLE hLog, CStepsLog* pStepsLog );
}; // namespace

#endif // _TESTS_H_