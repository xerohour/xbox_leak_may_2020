#pragma once

static const PWSTR RStrings[] =
{
    L"[Internal] Xbox version %hs (0x%02x)",
    L"Xbox version %hs",
    L"Xbox recovery has failed", L"because of an unexpected error (0x%08x).",
    L"because this Xbox is no longer supported.",
    L"Press any gamepad button to begin recovery.", L"(Warning: Hard disk will be erased.)",
    L"Recover as a retail Xbox or a dev kit Xbox?", L"(Y button: retail, Any other button: dev kit)",
    L"Please wait.  Updating the ROM...", L"(Do not reboot or power off!)",
    L"Preparing the hard disk...",
    L"Copying files to the hard disk...",
    L"Please eject and remove the recovery CD.",
    L"Please remove the recovery CD and",
    L"press any gamepad button to reboot.",
};

enum RECOVSTR_INDEX
{
    RS_BUILD_NUMBER_FORMAT_INTERNAL = 0,
    RS_BUILD_NUMBER_FORMAT,
    RS_FAILURE_LINE1,
    RS_FAILURE_LINE2_FORMAT,
    RS_FAILURE_LINE2_OLD_BOX,
    RS_GAMEPAD_BEGIN_LINE1,
    RS_GAMEPAD_BEGIN_LINE2,
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

