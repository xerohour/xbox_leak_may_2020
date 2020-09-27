//-----------------------------------------------------------------------------
//
// ani.cpp - Startup animation test
//
// Member of:  Grafx Module for xmta test executive.
//
//-----------------------------------------------------------------------------
#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "xgrafx.h"
#include "..\gfxtest.h"

//******************************************************************
// Title: Ani
//
// Abstract: Verifies the video output of the Xbox startup animation
//
//******************************************************************
IMPLEMENT_TESTLOCKED(Grafx, Ani, 5)
{
    GFXTEST_RESULTS gtr = {0};

   // Grafx_StartupAnimation_TestMain(&gtr);

    if (FAILED(gtr.hr)) {
        if (gtr.dwCRCResult[0] != gtr.dwCRCExpected[0]) {
    		ReportDebug(DEBUG_LEVEL1, _T("Frame buffer CRC mismatch on frame %d\n"), gtr.uFrame);
            err_BADCRC(_T("640x480"), gtr.dwCRCResult[0], gtr.dwCRCExpected[0], L"Final composite CRC.");
        }
        else {
            ReportD3DError(gtr.hr);
        }
    }
}
