/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    XLoadSection.cpp

Abstract:

    Tests the section loading / unloading APIs

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

*/

#include "stdafx.h"
#include "commontest.h"
#include "sections.h"
#include <xbeimage.h>

/*

Routine Description:

    Tests the section loading / unloading APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    DWORD - 

*/
extern int seg_iSegment00001;
extern int *segmentValues[2048];

DWORD XESECTIONS::XLoadSection_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    */

    char segName[9];
    char testcase[32];
    HANDLE hSection;
    void *pSection1, *pSection2;
    DWORD size;
    PXBEIMAGE_SECTION oSection;
    unsigned refCount;


    //
    // This chunk is needed so that the linker doesnt throw out 
    // all of our sections
    //
    XLoadSection("seg00001");
    seg_iSegment00001 = 1;
    XFreeSection("seg00001");


    /*
        this test has 2048 sections in it, use for(i=0; i<2048, i++) for
        the full test.
    */
     for(unsigned i=0; i<512; i++)
        {
        sprintf(segName, "seg%05u", i);
        sprintf(testcase, "Modify seg%05u", i);

        xSetFunctionName(hLog, "XGetSectionHandle");
        TESTCASE(segName)
            {
            hSection = XGetSectionHandleA(segName);
            if(hSection != INVALID_HANDLE_VALUE) 
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            } ENDTESTCASE;

        if(hSection == INVALID_HANDLE_VALUE) continue;
        oSection = (PXBEIMAGE_SECTION)hSection;
        refCount = oSection->SectionReferenceCount;

        xSetFunctionName(hLog, "XLoadSection");
        TESTCASE(segName)
            {
            pSection1 = XLoadSection(segName);
            if(pSection1)
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "ptr=%08X (ec: %ld)", pSection1, GetLastError());

            if(oSection->SectionReferenceCount == refCount+1)
                TESTPASS(hLog, "SectionReferenceCount==%u (ec: %ld)", oSection->SectionReferenceCount, GetLastError());
            else
                TESTFAIL(hLog, "SectionReferenceCount==%u (ec: %ld)", oSection->SectionReferenceCount, GetLastError());
            } ENDTESTCASE;
        TESTCASE(testcase)
            {
            *segmentValues[i] = i; // this will throw an exception if segment isnt loaded
            if(*segmentValues[i] == i)
                TESTPASS(hLog, "seg_iSegment%05u(%u) == %u", i, *segmentValues[i], i);
            else
                TESTFAIL(hLog, "seg_iSegment%05u(%u) != %u", i, *segmentValues[i], i);
            } ENDTESTCASE;

        xSetFunctionName(hLog, "XLoadSectionByHandle");
        TESTCASE(segName)
            {
            pSection2 = XLoadSectionByHandle(hSection);
            if(pSection2)
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "ptr=%08X (ec: %ld)", pSection2, GetLastError());

            if(oSection->SectionReferenceCount == refCount+2) 
                TESTPASS(hLog, "SectionReferenceCount==%u (ec: %ld)", oSection->SectionReferenceCount, GetLastError());
            else
                TESTFAIL(hLog, "SectionReferenceCount==%u (ec: %ld)", oSection->SectionReferenceCount, GetLastError());
            } ENDTESTCASE;

        xSetFunctionName(hLog, "XFreeSection");
        TESTCASE(segName)
            {
            if(XFreeSection(segName))
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());

            if(oSection->SectionReferenceCount == refCount+1) 
                TESTPASS(hLog, "SectionReferenceCount==%u (ec: %ld)", oSection->SectionReferenceCount, GetLastError());
            else
                TESTFAIL(hLog, "SectionReferenceCount==%u (ec: %ld)", oSection->SectionReferenceCount, GetLastError());
            } ENDTESTCASE;


        xSetFunctionName(hLog, "XGetSectionSize");
        TESTCASE(segName)
            {
            size = XGetSectionSize(hSection);
            if(size)
                TESTPASS(hLog, "Section Size: %u (ec: %ld)", size, GetLastError());
            else
                TESTFAIL(hLog, "Section Size: %u (ec: %ld)", size, GetLastError());
            } ENDTESTCASE;

        xSetFunctionName(hLog, "XFreeSectionByHandle");
        TESTCASE(segName)
            {
            if(XFreeSectionByHandle(hSection))
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());

            if(oSection->SectionReferenceCount == refCount) 
                TESTPASS(hLog, "SectionReferenceCount==%u (ec: %ld)", oSection->SectionReferenceCount, GetLastError());
            else
                TESTFAIL(hLog, "SectionReferenceCount==%u (ec: %ld)", oSection->SectionReferenceCount, GetLastError());
            } ENDTESTCASE;
        }


    DWORD time = GetTickCount();




    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////



    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////





    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("Mini Stress")
        {
        DebugPrint("XESECTIONS(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxStress = 1000;
        unsigned failures = 0;
        unsigned i;
        for(i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0)
                {
                DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }

            //if(XGetSoundtrackSongInfo(1, 0, &songID, &songLength, songname, 1024) != TRUE)
            //    ++failures;
            }
        if(failures)
            TESTFAIL(hLog, "Mini stress failures: %u", failures);
        else
            TESTPASS(hLog, "Mini stress failures: %u", failures);
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); });
*/

    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Cleanup
    //////////////////////////////////////////////////////////////////////////


    return 0;
    }
