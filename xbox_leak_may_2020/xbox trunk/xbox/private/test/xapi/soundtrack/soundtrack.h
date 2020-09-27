/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    soundtrack.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

*/

typedef DWORD (*APITEST_FUNC)(HANDLE hLog, DWORD ThreadID);

namespace Soundtrack
    {
    //
    //  XAPI soundtrack Tests
    //
    extern DWORD XFindSoundtrack_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD XGetSoundtrackSongInfo_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD XOpenSoundtrackSong_Test(HANDLE hLog, DWORD ThreadID);
    } // namespace Soundtrack