/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    StdAfx.cpp

Abstract:

    source file that includes just the standard includes

Author:

    Josh Poley (jpoley)

Revision History:

Notes:
    stdafx.pch will be the pre-compiled header
    stdafx.obj will contain the pre-compiled type information

*****************************************************************************/
#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#include <atlimpl.cpp>
