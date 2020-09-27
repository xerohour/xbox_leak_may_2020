/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashMovies.h

Abstract:



Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

*/
#ifndef _XDASHMOVIES_H_
#define _XDASHMOVIES_H_

#define COMPONENT_NAME_W    L"XDashMovies"
#define COMPONENT_NAME_A    "XDashMovies"
#define DLL_NAME_W          L"XDashMovies"
#define DLL_NAME_A          "XDashMovies"

typedef DWORD ( *APITEST_FUNC )( HANDLE hLog, CStepsLog* pStepsLog );

#endif // _XDASHMOVIES_H_