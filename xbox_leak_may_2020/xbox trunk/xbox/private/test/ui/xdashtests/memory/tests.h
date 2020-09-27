#ifndef _tests_h
#define _tests_h

namespace xdashmemory
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
        09-27-2000  Created

    */

    //
    //  Memory Management Tests
    //

    //    01234567890123456789012345678901
    DWORD MemColDukeMUVarsDisplay         ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemColMDVariousSizesDisplay     ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemColCannotSelectEmptyMD       ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemAreaTitleDisplay             ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemAreaFreeSpaceDisplay         ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemAreaSGWrapping               ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemAreaTitlesPerScreen          ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemAreaGeneralDisplay           ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemAreaHWDeviceRemoved          ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemAreaSorting                  ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemAreaSavedGameMetaData        ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD SavedGameTitleSingleSGDisplay   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD SavedGameTitleMultipleSGDisplay ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD SavedGameNavCancel              ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD SavedGameNavCopy                ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteOnlySavedGameFromMD       ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteSingleSavedGameFromMD     ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteGameRemovesGameProperMD   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteSingleGameFromMDPowerOff  ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteSingleGameFromMDPullsPlug ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteMultipleSavedGamesFromMD  ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteSavedGameNavConfirmCancel ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteTitleFromMD               ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD DeleteTitleNavConfirmCancel     ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD HWInsertedDuringOperation       ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD HWRemovedDuringOperation        ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD HWRemovedDuringOperationFail    ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD HWSourceRemovedBeforeCopy       ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopySingleSGFromMDToMD          ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopySingleSGFromMDToMDPowerOff  ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopySingleSGFromMDToMDPullsPlug ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopySingleSGAllUnitsFull        ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopySingleSGAllUnitsFullHSFull  ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopySingleSGAllUnitsFullHSEmpty ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopySingleSGHSFullEmptyFull     ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopyMultipleSGsFromMDToMD       ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopySingleSGFileVerification    ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD CopyMultipleSGsFileVerification ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemCopyDukeHotPlugVarsDisplay   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemCopyMUHotPlugVarsDisplay     ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemCopyMDVariousSizesDisplay    ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemCopyDestMUNotEnoughRoom      ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemCopyTitleSingleSGDisplay     ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD MemCopyTitleMultipleSGDisplay   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD BootToMemCleanUpMAFullDisplay   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD BootToMemCleanUpMAEmptyDisplay  ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD BootToMemCleanUpMAFull0blocks   ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD BootToMemCleanUpMAFullDeleteAll ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD BootToMemCleanUpClearSpace      ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD BootToMemCleanUpHotSwapMA       ( HANDLE hLog, CStepsLog* pStepsLog );
    DWORD BootToMemCleanUpNavAway         ( HANDLE hLog, CStepsLog* pStepsLog );
}; // namespace

#endif // _tests_h

// Enumerate tests of removing any hardware while meta data is displayed for EVERY screen!
