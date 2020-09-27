#pragma once

#ifdef HDSETUPAPP

static const PWSTR RStrings[] =
{
    L"[I] Xbox HD Setup version %hs (0x%02x)",
    L"Xbox HD Setup Version %hs",
    L"Xbox HD Setup has failed", L"because of an unexpected error (0x%08x).",
    L"because this Xbox is no longer supported.",
    L"This CD must be in the drive at power up.",
    L"Press any gamepad button to begin HD Setup.", L"(Warning: Hard disk will be erased.)",
    L"Lock the HD?", L"(Y button: lock, Any other button: don't lock)",
    L"Please wait.  Updating the ROM...", L"(Do not reboot or power off!)",
    L"Preparing the hard disk...",
    L"Copying files to the hard disk...",
    L"Please eject and remove the HD Setup CD.",
    L"Please remove the HD Setup CD and",
    L"press any gamepad button to reboot.",
};

#else  // HDSETUPAPP

static const PWSTR RStrings[] =
{
#ifdef ONLINEBETA
    L"Xbox Betakit version %hs (0x%02x)",
    L"Xbox Betakit version %hs (0x%02x)",
#else
    L"[Internal] Xbox version %hs (0x%02x)",
    L"Xbox version %hs",
#endif
    L"Xbox recovery has failed", L"because of an unexpected error (0x%08x).",
    L"because this Xbox is no longer supported.",
    L"This CD must be in the drive at power up.",
    L"Press any gamepad button to begin recovery.", L"(Warning: Hard disk will be erased.)",
    L"ROM is current.  Update anyway?", L"(Y: update, any other button: don't update)",
    L"A newer version of the ROM is available", L"Please run full recovery (any button to continue)",
    L"Recover as a retail Xbox or a dev kit Xbox?", L"(Y button: retail, Any other button: dev kit)",
    L"Please wait.  Updating the ROM...", L"(Do not reboot or power off!)",
    L"Preparing the hard disk...",
    L"Copying files to the hard disk...",
    L"Please eject and remove the recovery CD.",
    L"Please remove the recovery CD and",
    L"press any gamepad button to reboot.",
};

#endif // HDSETUPAPP

enum RECOVSTR_INDEX
{
    RS_BUILD_NUMBER_FORMAT_INTERNAL = 0,
    RS_BUILD_NUMBER_FORMAT,
    RS_FAILURE_LINE1,
    RS_FAILURE_LINE2_FORMAT,
    RS_FAILURE_LINE2_OLD_BOX,
    RS_FAILURE_LINE2_WARM_BOOT,
    RS_GAMEPAD_BEGIN_LINE1,
    RS_GAMEPAD_BEGIN_LINE2,
#ifndef HDSETUPAPP
    RS_OVERWRITEROM_LINE1,
    RS_OVERWRITEROM_LINE2,
    RS_NEWERROM_LINE1,
    RS_NEWERROM_LINE2,
#endif
    RS_RETAIL_LINE1,
    RS_RETAIL_LINE2,
    RS_FLASH_LINE1,
    RS_FLASH_LINE2,
    RS_PREPARING,
    RS_COPYING,
    RS_GAMEPAD_END_RETAIL,
    RS_GAMEPAD_END_LINE1,
    RS_GAMEPAD_END_LINE2,
};

