/*++

Copyright (c) 1998  Microsoft Corporation

Module Name:

    pic.c

Abstract:

    WinDbg Extension Api

Author:

    Santosh Jodh (santoshj) 29-June-1998

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

#define PIC_MASTER_PORT0    0x20
#define PIC_MASTER_PORT1    0x21

#define PIC_SLAVE_PORT0     0xA0
#define PIC_SLAVE_PORT1     0xA1


VOID
ShowMask (
    ULONG  Mask
    )
{
    ULONG interrupt;
    
    for (   interrupt = 0;
            interrupt <= 0x0F;
            interrupt++)
    {
        if (Mask & (1 << interrupt))        
            dprintf("  Y");                    
        else
            dprintf("  .");
    }
    
    dprintf("\n");
}

BOOLEAN
GetPICStatus (
    UCHAR   Type,
    PULONG  Status
    )
{
    ULONG   size;
    ULONG   data;
    ULONG   mask;
    
    //
    // Send OCW3 to master.
    //
    
    size = 1;
    WriteIoSpace(PIC_MASTER_PORT0, Type, &size);

    //
    // Read master's status.
    //
    
    data = 0;
    size = 1;
    ReadIoSpace(PIC_MASTER_PORT0, &data, &size);
    if (size == 1)
    {
        //
        // Send OCW3 to slave.
        //
        
        mask = data;
        size = 1;
        WriteIoSpace(PIC_SLAVE_PORT0, Type, &size);

        //
        // Get the slave's status.
        //
        
        data = 0;
        size = 1;
        ReadIoSpace(PIC_SLAVE_PORT0, &data, &size);
        if (size == 1)
        {
            mask |= (data << 8);
            *Status = mask;

            return (TRUE);
        }
    }

    *Status = 0;
    
    return (FALSE);
}

DECLARE_API(pic)

/*++

Routine Description:

    Dumps PIC information.

Input Parameters:

    args - Supplies the options.

Return Value:

    None

--*/

{
    ULONG   data;
    ULONG   size;
    ULONG   mask;
    
    //
    // Display the title.
    //

    dprintf("----- IRQ Number ----- 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");

    //
    // Dump the Interrupt Service Register information.
    //

    dprintf("Physically in service:");
    
    if (GetPICStatus(0x0B, &mask))
    {
        ShowMask(mask);
    }
    else
    {
        dprintf("Error reading PIC!\n");
    }
    

    //
    // Dump the Interrupt Mask Register information.
    //

    dprintf("Physically masked:    ");

    data = 0;
    size = 1;
    ReadIoSpace(PIC_MASTER_PORT1, &data, &size);
    if (size == 1)
    {
        mask = data;
        data = 0;
        size = 1;
        ReadIoSpace(PIC_SLAVE_PORT1, &data, &size);
        if (size == 1)
        {
            mask |= (data << 8);
            ShowMask(mask);    
        }
        else
        {
            dprintf("Error reading PIC!\n");    
        }
    }
    else
    {
        dprintf("Error reading PIC!\n");    
    }

    
    //
    // Dump the Interrupt Request Register information.
    //

    dprintf("Physically requested: ");

    if (GetPICStatus(0x0A, &mask))
    {
        ShowMask(mask);
    }
    else
    {
        dprintf("Error reading PIC!\n");
    }
}
