#pragma once

static const PWSTR RStrings[] =
{
    L"Xbox version %hs",
    L"Xbox recovery has failed", L"because of an unexpected error (%d).",
    L"Press any gamepad button to begin recovery.", L"(Warning: Hard disk will be erased.)",
    L"Preparing the hard disk...",
    L"Copying files to the hard disk...",
    L"Please remove the recovery CD and", L"press any gamepad button to reboot.",
};

enum RECOVSTR_INDEX
{
    RS_BUILD_NUMBER_FORMAT = 0,
    RS_FAILURE_LINE1,
    RS_FAILURE_LINE2_FORMAT,
    RS_GAMEPAD_BEGIN_LINE1,
    RS_GAMEPAD_BEGIN_LINE2,
    RS_PREPARING,
    RS_COPYING,
    RS_GAMEPAD_END_LINE1,
    RS_GAMEPAD_END_LINE2,
};

