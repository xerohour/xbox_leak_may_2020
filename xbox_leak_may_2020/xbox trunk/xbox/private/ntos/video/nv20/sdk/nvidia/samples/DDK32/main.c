 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

//
// Sample configuration application.
//
// This simple program is meant to show how to access the NVIDIA configuration interface.
// The NVIDIA configuration interface allows client applications to read and modify select
// operating values within an NV device.
//
// The steps are:
//  1) Open an interface to the resource manager
//  2) Register the calling application by allocating a client
//  3) Find and identify the devices in the system by allocating a device and giving it a name.
//  4) Get and set device attributes, using the device's name.
//
// This program links with a library to provide simple wrapper routines
// for the resource manager's IOCTL interface.  This hides the complexity of 
// the Win32 IOCTL interfaces.
//
// LINKER NOTE:  You must set /nodefaultlib:"LIBC" in the link options, when using VC++.
//
//  NVRMAPI.LIB (SDK Library for Win32 clients)
//
// IMPORTANT: There are currently different libraries and include file versions
// for NV3 and NV4.  Make sure you use the copies meant for the chip you are
// using.
//
#include <windows.h>
#include <stdio.h>
#include <conio.h>

//
// NV SDK include files
//
#include "nvos.h"
#include "nv32.h"
#include "nvrmapi.h"

//
// NV DDK include files
//
#include "nvcm.h"

//
// A 32bit ID of our choice to track our allocated device (SDK)
//
#define OUR_DEVICE_ID	0xBAAD0000

int main()
{

    char            nameBuffer[32];
    unsigned long   hRm, hClient;
    int             val;

    //
    // Open up a connection to the resource manager
    //
    printf("Opening a connection to the resource manager...\n");
    hRm = (unsigned long)NvRmOpen();

    //
    // Register this client with the architecture
    //
    printf("Allocating a client...\n");
    if (NvRmAllocRoot(&hClient) != NVOS01_STATUS_SUCCESS) 
    {
        //
        // oops, an error.  Close it all back down.
        //
        printf("ERROR: Cannot allocate client.\n");
        NvRmClose();
        getch();
        return 1;
    }

    //
    // Allocate a device.  For Microsoft operating systems, the
    // OS notion of a display device is DisplayX, where X is an
    // one-based integer.  Display1 would be considered the
    // primary display in most cases.
    //
    // NV Architecture defines up to 8 devices.  They are defined
    // in the order in which they're initialized by the operating
    // system, starting at zero.  So, DEVICE_0 will usually 
	// correlate to Display1.  And so on.
    //
    // This allocation call is only necessary to determine the
    // exact type of NV device (NV3, NV4, etc).
    //
    // Let's just get the primary device.
    //
    printf("Allocating a device...\n");
    if (
    	NvRmAllocDevice(
    		hClient,                    // client handle
			OUR_DEVICE_ID,              // device name
			NV01_DEVICE_0,              // device class
			(void *)nameBuffer          // returned device type
        ) != NVOS06_STATUS_SUCCESS
    ) 
	{
        //
        // oops, an error.  Close it all back down.
        //
        printf("ERROR: Cannot allocate device.\n");
        NvRmFree(hClient, NV01_NULL_OBJECT, hClient);
        NvRmClose();
        getch();
        return 1;
    }

    //
    // Make sure this is an NV4 device...
    //
    printf("Verifying NV4 device...\n");
    if ((strcmp(nameBuffer, "NV4") != 0)) 
    { 
    	//
    	// oops, an error.  Close it all back down.
    	//
    	printf("ERROR: Not an NV4 device: %s.\n", nameBuffer);
    	NvRmFree(hClient, NV01_NULL_OBJECT, hClient);
        NvRmClose();
    	getch();
        return 1;
    }
    printf("...Found an NV4 device.\n");

    //
    // Config "get" calls are meant to return the current attributes
	// of a specific chip instance.  We've already allocated and named a
	// chip, so we use that name to extract the values.
    //
	// The general form of the NvRmConfigGet() function is as follows:
	//
	// 	NvRmConfigGet(
	// 		client_handle, 
	// 		device_name,   
	// 		attribute_name,
	// 		&returned_value
	// 	);

    //
    // Get some device attributes.
    //
    printf("Getting bus type...\n");
    if (
    	NvRmConfigGet(
			hClient,			  
			OUR_DEVICE_ID,		  
	    	NV_CFG_BUS_TYPE, 	  
			&val				  
	    ) != NVOS13_STATUS_SUCCESS
	)
    { 
    	printf("ERROR: Could not get the bus type.\n");
    }
	else
		printf("Bus Type: %u\n", val);

    printf("Getting current framebuffer size...\n");
    if (
    	NvRmConfigGet(
			hClient,
			OUR_DEVICE_ID,
	    	NV_CFG_RAM_SIZE_MB, 
			&val
	    ) != NVOS13_STATUS_SUCCESS
	)
    { 
    	printf("ERROR: Could not get frame buffer size.\n");
    }
	else
		printf("Framebuffer Size: %u MB\n", val);

    printf("Getting current value of CRTC register CR09...\n");
    if (
    	NvRmConfigGet(
			hClient,
			OUR_DEVICE_ID,
	    	NV_CFG_CRTC + 0x09, 
			&val
	    ) != NVOS13_STATUS_SUCCESS
	)
    { 
    	printf("ERROR: Could not get current value of CRTC register CR09.\n");
    }
	else
		printf("CR09: %02X\n", val);

    //
    // Config "set" calls are meant to override the current attributes
	// of a specific chip instance.
    //
	// The general form of the NvRmConfigSet() function is as follows:
	//
	// 	NvRmConfigSet(
	// 		client_handle, 
	// 		device_name,   
	// 		attribute_name,
	//		new_value,
	// 		&returned_old_value
	// 	);
	//
    // NvRmConfigSet calls are batched up until a NvRmConfigUpdate() call is made.
    // This allows us to set multiple interdependent values before the
    // override takes place.
    //
    // NvRmConfigUpdate(client_handle, device_name, NV_CFG_PERMANENT);
    //
    // WARNING: Most system settings cannot be overridden.  If a value can
    // be changed, it usually requires coordination with the display
    // driver.  For example, changing the system resolution requires
    // a reenable of the display driver to the new settings.
    //
	
    printf("Setting new refresh rate...\n");
    if (
    	NvRmConfigSet(
			hClient,
			OUR_DEVICE_ID,
	    	NV_CFG_VIDEO_REFRESH_RATE, 
			60,
			&val
	    ) != NVOS14_STATUS_SUCCESS
	)
    { 
    	printf("ERROR: Could not set refresh rate.\n");
    }
    printf("Old refresh rate: %d\n", val);
    if (
    	NvRmConfigUpdate(
    		hClient, 
    		OUR_DEVICE_ID, 
    		NV_CFG_PERMANENT
    	) != NVOS15_STATUS_SUCCESS
	)
    { 
    	printf("ERROR: Could not update the device attributes.\n");
    }
	

    //
    // Close everything up
    //
    printf("Freeing the client and device...\n");
    NvRmFree(hClient, NV01_NULL_OBJECT, hClient);
    printf("Disconnecting from the resource manager...\n");
    NvRmClose();

    getch();

    return 0;

}
