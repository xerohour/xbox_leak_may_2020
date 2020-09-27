/*++

Copyright (c) Microsoft Corporation

Module Name:

    rbc.h

Abstract:

    This was scsi.h from the Windows 2000 tree.  Our Xbox MU protocol is an extreme subset of the
    SCSI commands.  Additionally, we are packing a little extra information into the READ_CAPACITY
    command than is specified in the RBC specification.  Not really an issue, since do not need to
    be SCSI compatible anyway.

    So this file is paired don't to have only relevant definitions

Authors:

Revision History:

--*/
#ifndef __RBC_H__
#define __RBC_H__

//
// Command Descriptor Block. Passed by SCSI controller chip over the SCSI bus
//

#include <pshpack1.h>
typedef union _CDB {
    
    //
    // Standard 10-byte CDB
    //

    struct _CDB10 {
        UCHAR OperationCode;
        UCHAR RelativeAddress : 1;
        UCHAR Reserved1 : 2;
        UCHAR ForceUnitAccess : 1;
        UCHAR DisablePageOut : 1;
        UCHAR LogicalUnitNumber : 3;
        union {
            struct {
                UCHAR LogicalBlockByte0;
                UCHAR LogicalBlockByte1;
                UCHAR LogicalBlockByte2;
                UCHAR LogicalBlockByte3;
            };
            ULONG LogicalBlock;
        };
        UCHAR Reserved2;
        union {
            struct {
                UCHAR TransferBlocksMsb;
                UCHAR TransferBlocksLsb;
            };
            USHORT TransferBlocks;
        };
        UCHAR Control;
    } CDB10, *PCDB10;

    //
    // Access as array of ULONGS or BYTES
    //

    ULONG AsUlong[4];
    UCHAR AsByte[16];

} CDB, *PCDB;
#include <poppack.h>

//
// SCSI CDB operation codes
//

#define SCSIOP_READ_CAPACITY       0x25
#define SCSIOP_READ                0x28
#define SCSIOP_WRITE               0x2A
#define SCSIOP_VERIFY              0x2F

//
// Read Capacity Data - returned in Big Endian format
//
// (CAVEAT! - this is not the structure defined by SCSI!!!)
// We added the LogicalBlocKPerMediaBlock by carving away
// 16 bits 

typedef struct _READ_CAPACITY_DATA {
    ULONG  LogicalBlockAddress;
    USHORT LogicalBlocksPerMediaBlock; 
    USHORT BytesPerLogicalBlock;
} READ_CAPACITY_DATA, *PREAD_CAPACITY_DATA;

#endif // !defined __RBC_H__
