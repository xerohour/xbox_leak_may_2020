/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    launch.cpp

Abstract:

    HVS Test Helper functions

Notes:

*****************************************************************************/

#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <xbeimage.h>
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

#include <xtl.h>
#include <stdio.h>
#include "hvsUtils.h"


/*****************************************************************************

Routine Description:

    LaunchImage

    Wrapper for XLaunchNewImage, which allows us to pass LAUNCH_DATA to an
    xbe that does not have the same title ID as the caller's ID.

Arguments:

    char *xbe       - full path to the new xbe
    LAUNCH_DATA     - info to pass

Return Value:

    none - this function will NOT RETURN

*****************************************************************************/
void LaunchImage(char *xbe, LAUNCH_DATA *launchInfo)
    {
    if(xbe)
        {
        FILE *f = fopen(xbe, "rb");
        if(f)
            {
            // read the title ID from the destination XBE
            unsigned char image[512];
            _XBEIMAGE_HEADER *h = (_XBEIMAGE_HEADER*)image;
            if(fread(image, 1, 512, f) != 512)
                {
                OutputDebugStringA("Reading of target image failed...\n");
                }

            // XLaunchNewImage looks at our images' title ID.
            // So we spoof it to make it look like we are the destination
            XeImageHeader()->Certificate->TitleID = *(DWORD*)&image[0x180];

            fclose(f);
            }
        }

    // Now we can pass LAUNCH_DATA to the XBE with a different title ID
    XLaunchNewImage(xbe, launchInfo);
    }

