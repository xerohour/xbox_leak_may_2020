#ifndef _tests_h
#define _tests_h

namespace xdashboot
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
    //  Boot Sequence Tests
    //

    //    01234567890123456789012345678901
    DWORD Test1Here                       ( HANDLE hLog, CStepsLog* pStepsLog );

}; // namespace

#endif // _tests_h