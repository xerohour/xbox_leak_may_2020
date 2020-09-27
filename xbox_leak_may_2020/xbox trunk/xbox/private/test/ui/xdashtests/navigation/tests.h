#ifndef _tests_h
#define _tests_h

namespace xdashnav
{
    #define VARIATION_BUFFER_SIZE  1024

    // Global list of ALL Screens and Controls
    extern CXDashScreenList g_XDashScreens;

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
    //  Navigation / Skeleton Tests
    //

    //    01234567890123456789012345678901
    DWORD AllControlsBasicNav             ( HANDLE hLog, CStepsLog* pStepsLog );

}; // namespace

#endif // _tests_h

// Enumerate navigation cases where media is inserted and the user is wisked away - Music CD's, etc