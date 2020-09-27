/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    ctrltest.cpp

Abstract:

    USB Simulator Test app

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#include "..\..\inc\USBSimulator.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

// Tests
void DukeTest(USBSimulator &usbsim);
void MUTest(USBSimulator &usbsim);
void HawkTest(USBSimulator &usbsim);


int main(int argc, char* argv[])
    {
    printf("USB Simulator Test\n\n");

    //
    // configure the Simulator object
    //
    USBSimulator usbsim;                                    // no logging
    //USBSimulator usbsim(true, RECORDING_FORMAT_CHIEF_UTG);  // CATC Chief readable format
    //USBSimulator usbsim(true, RECORDING_FORMAT_TEXT_LOG);   // text format

    //
    // Look for simulators on the network
    //
    unsigned numSimulators = usbsim.FindSimulators();
    printf("Found %u simulators\n", numSimulators);
    if(numSimulators == 0)
        {
        printf("Startup 1 or more USB Simulators on the network first!\n");
        printf("Press Enter...");
        getchar();
        return 0;
        }

    // Point to a specific simulator
    usbsim.ip[0] = inet_addr("157.56.10.18");

    //
    // Get the version and MAC address from the first simulator
    //
    SimPacketTyped<unsigned char [128]> simData;
    memset(&simData, 0, sizeof(simData));
    usbsim.SendCommand(0, SIM_CMD_STATUS, SIM_SUBCMD_STATUS_VERSION, simData);
    simData.data[simData.header.dataSize] = '\0';
    printf("   Sim[0] Version:  %s\n", simData.data);

    memset(&simData, 0, sizeof(simData));
    usbsim.SendCommand(0, SIM_CMD_STATUS, SIM_SUBCMD_STATUS_MACADDR, simData);
    printf("   Sim[0] MAC Addr: %02X %02X %02X %02X %02X %02X\n", simData.data[0], simData.data[1], simData.data[2], simData.data[3], simData.data[4], simData.data[5]);


    //
    // Run the tests
    //
    //DukeTest(usbsim);
    //MUTest(usbsim);
    //HawkTest(usbsim);


    //
    // Done
    //
    printf("\nPress <Enter>...");
    getchar();
    return 0;
    }


/*****************************************************************************

Routine Description:

    Duke Test

Arguments:

    USBSimulator

Return Value:

    none

*****************************************************************************/
void DukeTest(USBSimulator &usbsim)
    {
    DWORD err;

    XIDInputReport xidPacket = defaultXIDReport;
    unsigned state;

    printf("\nRunning Duke Test\n");
    XIDDevice *xid = new XIDDevice;

    printf("   Inserting the device\n");
    err = usbsim.Plug(USBSIM_HOST_PORT1, 0, xid);
    _ASSERT(err == USBSIM_ERROR_OK);

    printf("   Waiting for several seconds...\n");

    Sleep(1000); // let the enumeration happen
    if(xid->IsEnumerated()) printf("   Duke enumerated!\n");

    // move down
    for(state = 0; state<80; state++)
        {
        xidPacket.thumbLY = -32000;
        xid->SetInputReport(&xidPacket);
        Sleep(50);

        xidPacket.thumbLY = 0;
        xid->SetInputReport(&xidPacket);
        Sleep(50);
        }

    // move up
    for(state = 0; state<80; state++)
        {
        xidPacket.thumbLY = 32000;
        xid->SetInputReport(&xidPacket);
        Sleep(50);

        xidPacket.thumbLY = 0;
        xid->SetInputReport(&xidPacket);
        Sleep(50);
        }

    printf("\n   Press <Enter> to unplug the device...");
    getchar();

    printf("   Removing the device\n");
    err = usbsim.Unplug(xid);
    _ASSERT(err == USBSIM_ERROR_OK);
    delete xid;
    }




/*****************************************************************************

Routine Description:

    MU Test

Arguments:

    USBSimulator

Return Value:

    none

*****************************************************************************/
void MUTest(USBSimulator &usbsim)
    {
    DWORD err;

    printf("\nRunning MU Test\n");
    BulkDevice *mu = new BulkDevice;

    printf("   Inserting the device\n");
    err = usbsim.Plug(USBSIM_HOST_PORT1, 0, mu);
    _ASSERT(err == USBSIM_ERROR_OK);

    Sleep(1000); // let stuff happen

    if(mu->IsEnumerated()) printf("   MU enumerated!\n");

    printf("\n   Press <Enter> to unplug the device...");
    getchar();

    printf("   Removing the device\n");
    err = usbsim.Unplug(mu);
    _ASSERT(err == USBSIM_ERROR_OK);
    delete mu;
    }



/*****************************************************************************

Routine Description:

    Hawk Test

Arguments:

    USBSimulator

Return Value:

    none

*****************************************************************************/
void HawkTest(USBSimulator &usbsim)
    {
    DWORD err;

    printf("\nRunning Hawk Test\n");
    IsocDevice *isoc = new IsocDevice;

    printf("   Inserting the device\n");
    err = usbsim.Plug(USBSIM_HOST_PORT1, 0, isoc);
    _ASSERT(err == USBSIM_ERROR_OK);

    printf("   Waiting for several seconds...\n");

    Sleep(1000); // let the enumeration happen
    if(isoc->IsEnumerated()) printf("   Hawk enumerated!\n");

    printf("\n   Press <Enter> to unplug the device...");
    getchar();

    printf("   Removing the device\n");
    err = usbsim.Unplug(isoc);
    _ASSERT(err == USBSIM_ERROR_OK);
    delete isoc;
    }


