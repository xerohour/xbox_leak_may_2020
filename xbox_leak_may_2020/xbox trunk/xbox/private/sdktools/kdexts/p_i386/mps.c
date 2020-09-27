/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    mps.c

Abstract:

    WinDbg Extension Api

Author:

    Peter Johnston (peterj) 30-September-1997

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

//
// Get needed MPS definitions right from the horse itself.
//

#include "..\..\ntos\nthals\inc\pcmp.inc"
#include "..\..\ntos\nthals\inc\apic.inc"

//
// xReadMemory is easier to use than ReadMemory and is
// defined in ..\devnode.c
//

BOOLEAN
xReadMemory(
    PVOID S,
    PVOID D,
    ULONG Len
    );

PUCHAR
mpsGetIntTypeDesc(
    UCHAR IntType
    )
{
    switch (IntType) {
    case INT_TYPE_INTR:
        return "intr  ";
    case INT_TYPE_NMI:
        return "nmi   ";
    case INT_TYPE_SMI:
        return "smi   ";
    case INT_TYPE_EXTINT:
        return "extint";
    default:
        return "unknwn";
    }
}

PUCHAR
mpsExtAddrTypeToText(
    UCHAR AddressType
    )
{
    switch (AddressType) {
    case MPS_ADDRESS_MAP_IO:
        return "io port     ";
    case MPS_ADDRESS_MAP_MEMORY:
        return "memory      ";
    case MPS_ADDRESS_MAP_PREFETCH_MEMORY:
        return "prefetch mem";
    case MPS_ADDRESS_MAP_UNDEFINED:
        return "mps undef   ";
    default:
        return "unknown type";
    }
}

PUCHAR
mpsExtCompatibleListToText(
    ULONG List
    )
{
    switch (List) {
    case 0:
        return "ISA";
    case 1:
        return "VGA";
    default:
        return "unknown predefined range";
    }
}


BOOLEAN
mpsBaseTable(
    PUCHAR BaseTableAddress,
    ULONG  EntryCount
    )

/*++

Routine Description:

    Dumps entries from the MPS BASE table.

Arguments:

    BaseTableAddress    Address (in local memory) of the Base Entry Table
    EntryCount          Number of entries in this table.

Return Value:

    TRUE    is all is well
    FALSE   if execution cannot continue (ie we encountered an unknown
            entry type.  Can't continue because we don't know how big
            it is.

--*/

{
    PUCHAR bp = BaseTableAddress;

    while (EntryCount--) {
        dprintf("  ");
        switch (*bp) {
        case ENTRY_PROCESSOR:
            {
                PPCMPPROCESSOR p = (PPCMPPROCESSOR)bp;
                bp += sizeof(*p);

                dprintf(
                    "processor. %s%sL.APIC ID %02x Vers %02x\n",
                    p->CpuFlags & CPU_ENABLED ? "EN " : "",
                    p->CpuFlags & BSP_CPU     ? "BP " : "",
                    p->LocalApicId,
                    p->LocalApicVersion
                    );
                dprintf(
                    "             Family %x, Model %x, Stepping %x, CPUID Flags %04x\n",
                    p->CpuIdentification.Family,
                    p->CpuIdentification.Model,
                    p->CpuIdentification.Stepping,
                    p->FeatureFlags
                    );
            }
            break;
        case ENTRY_BUS:
            {
                PPCMPBUS bus = (PPCMPBUS)bp;
                bp += sizeof(*bus);

                dprintf(
                    "bus.       id %02x, type %6.6s\n",
                    bus->BusId,
                    bus->BusType
                    );
            }
            break;
        case ENTRY_IOAPIC:
            {
                PPCMPIOAPIC ioapic = (PPCMPIOAPIC)bp;
                bp += sizeof(*ioapic);

                dprintf(
                    "io apic.   %s id %02x vers %02x @ %08x\n",
                    ioapic->IoApicFlag & IO_APIC_ENABLED ? "EN" : "DI",
                    ioapic->IoApicId,
                    ioapic->IoApicVersion,
                    ioapic->IoApicAddress
                    );
            }
            break;
        case ENTRY_INTI:
            {
                PPCMPINTI inti = (PPCMPINTI)bp;
                bp += sizeof(*inti);

                dprintf(
                    "io int.    %s po=%x el=%x, srcbus %02x irq %02x dst apic %02x intin %02x\n",
                    mpsGetIntTypeDesc(inti->IntType),
                    inti->Signal.Polarity,
                    inti->Signal.Level,
                    inti->SourceBusId,
                    inti->SourceBusIrq,
                    inti->IoApicId,
                    inti->IoApicInti
                    );
            }
            break;
        case ENTRY_LINTI:
            {
                PPCMPLINTI linti = (PPCMPLINTI)bp;
                bp += sizeof(*linti);

                dprintf(
                    "lcl int.   %s po=%x el=%x, srcbus %02x irq %02x dst apic %02x intin %02x\n",
                    mpsGetIntTypeDesc(linti->IntType),
                    linti->Signal.Polarity,
                    linti->Signal.Level,
                    linti->SourceBusId,
                    linti->SourceBusIrq,
                    linti->DestLocalApicId,
                    linti->DestLocalApicInti
                    );
            }
            break;
        default:
            dprintf(
                "Unknown MPS base type 0x%02x, cannot continue.\n",
                *bp
                );
            return FALSE;
        }
    }
    return TRUE;
}


BOOLEAN
mpsExtendedTable(
    PUCHAR ExtendedTableAddress,
    PUCHAR ExtendedTableAddressEnd
    )

/*++

Routine Description:

    Dumps entries from the MPS Extended table.

Arguments:

    BaseTableAddress    Address (in local memory) of the Base Entry Table
    EntryCount          Number of entries in this table.

Return Value:

    TRUE    is all is well
    FALSE   if execution cannot continue (ie we encountered an unknown
            entry type.  Can't continue because we don't know how big
            it is.

--*/

{
    PUCHAR bp = ExtendedTableAddress;

    if (!bp) {
        return TRUE;
    }
    dprintf("  extended table entries\n");

    while (bp < ExtendedTableAddressEnd) {

        PMPS_EXTENTRY extent = (PMPS_EXTENTRY)bp;

        if (extent->Length == 0) {
            dprintf("Malformed extended entry, length = 0, cannot continue.\n");
            return FALSE;
        }

        dprintf("  ");

        switch (extent->Type) {
        case EXTTYPE_BUS_ADDRESS_MAP:
            dprintf(
                "address.   bus %02x %s % 16I64x len %-I64x\n",
                extent->u.AddressMap.BusId,
                mpsExtAddrTypeToText(extent->u.AddressMap.Type),
                extent->u.AddressMap.Base,
                extent->u.AddressMap.Length
                );
            break;
        case EXTTYPE_BUS_HIERARCHY:
            dprintf(
                "child bus. bus %02x is child of bus %02x%s\n",
                extent->u.BusHierarchy.BusId,
                extent->u.BusHierarchy.ParentBusId,
                extent->u.BusHierarchy.SubtractiveDecode ? " subtractive" : ""
                );
            break;
        case EXTTYPE_BUS_COMPATIBLE_MAP:
            dprintf(
                "bus comp.  bus %02x %s %s ranges\n",
                extent->u.CompatibleMap.BusId,
                extent->u.CompatibleMap.Modifier ? "exclude" : "include",
                mpsExtCompatibleListToText(extent->u.CompatibleMap.List)
                );
            break;
        case EXTTYPE_PERSISTENT_STORE:
            dprintf(
                "persist.   % 16I64x len %-I64x\n",
                extent->u.PersistentStore.Address,
                extent->u.PersistentStore.Length
                );
            break;
        default:
            dprintf(
                "Unknown MPS extended type 0x%02x, cannot continue.\n",
                extent->Type
                );
            return FALSE;
        }

        //
        // Advance to the next entry.
        //

        bp += extent->Length;
    }
    return TRUE;
}


DECLARE_API( mps )

/*++

Routine Description:

    Dumps the MPS (Multi Processor Specification) BIOS Tables.

Arguments:

    None

Return Value:

    None

--*/

{
    PVOID addr;
    UCHAR halName[32];
    struct PcMpTable * PcMpTablePtr;
    struct PcMpTable PcMpCfgTable;
    ULONG entryCount;
    PUCHAR bp;
    UCHAR c;
    ULONG i;

    PUCHAR MpsBaseTable = NULL;
    PUCHAR MpsExtendedTable = NULL;
    PUCHAR MpsExtendedTableEnd;

    BOOLEAN halNameKnown = FALSE;

    //
    // Check to see if user entered the address of the MPS tables.
    // If not, try to obtain it using HAL symbols.
    //

    PcMpTablePtr = NULL;
    sscanf(args, "%lx", &PcMpTablePtr);
    if (PcMpTablePtr == NULL) {

        //
        // Get address of PC+MP structure from the HAL.
        // N.B. Should add code to allow hunting for the floating pointer.
        //

        addr = (PVOID)GetExpression("hal!HalName");

        if (addr == NULL) {
            dprintf(
                "Unable to use HAL symbols (hal!HalName), please verify symbols.\n"
                );
            return;
        }

        if (!xReadMemory(addr, &halName, sizeof(halName))) {
            dprintf(
                "Failed to read HalName from host memory, quitting.\n"
                );
            return;
        }

        halName[sizeof(halName)-1] = '\0';
        if (strstr(halName, "MPS ") == NULL) {
            dprintf("HAL = \"%s\".\n", halName);
            dprintf("HAL does not appear to be an MPS HAL, quitting.\n");
            return;
        }
        halNameKnown = TRUE;

        addr = (PVOID)GetExpression("hal!PcMpTablePtr");

        if (addr == NULL) {
            dprintf(
                "Unable to get address of hal!PcMpTablePtr, cannot continue.\n"
                );
            return;
        }

        if (!xReadMemory(addr, &PcMpTablePtr, sizeof(PcMpTablePtr))) {
            dprintf(
                "Failed to read PcMpTablePtr from host memory, cannot continue.\n"
                );
            return;
        }
    }

    if (!xReadMemory(PcMpTablePtr, &PcMpCfgTable, sizeof(PcMpCfgTable))) {
        dprintf(
            "Failed to read MP Configuration Table Header @%08x\n"
            "Cannot continue.\n",
            PcMpTablePtr
            );
        return;
    }

    if (PcMpCfgTable.Signature != PCMP_SIGNATURE) {
        dprintf(
            "MP Config Table Signature doesn't match.  Cannot continue.\n"
            );
        return;
    }

    dprintf("  BIOS Revision ");

    switch (PcMpCfgTable.Revision) {
    case 1:
        dprintf(
            "MPS 1.1 (WARNING: This BIOS might not support NT 5 depending\n"
            "                  upon system configuration.)\n"
            );
        break;
    case 4:
        dprintf(
            "MPS 1.4       "
            );
        break;
    default:
        dprintf(
            "Unknown MPS revision byte 0x%2x, dumped values\n"
            "  may be incorrect.\n"
            );
        break;
    }

    if (halNameKnown) {
        dprintf("  HAL = %s", halName);
    }
    dprintf("\n");

    bp = PcMpCfgTable.OemId + sizeof(PcMpCfgTable.OemId);
    c = *bp;
    *bp = '\0';
    dprintf(
        "  OEM ID         :%s\n",
        PcMpCfgTable.OemId
        );
    *bp = c;

    bp = PcMpCfgTable.OemProductId + sizeof(PcMpCfgTable.OemProductId);
    c = *bp;
    *bp = '\0';
    dprintf(
        "  OEM Product ID :%s\n",
        PcMpCfgTable.OemProductId
        );
    *bp = c;

    if (PcMpCfgTable.TableLength <= sizeof(PcMpCfgTable)) {
        dprintf(
            "MPS Base Table length (%d) is too small to be reasonable,\n",
            PcMpCfgTable.TableLength
            );
        dprintf(
            "Must be >= sizeof(fixed table header) (%d bytes).  "
            "Cannot continue.\n",
            sizeof(PcMpCfgTable)
            );
        return;
    }

    //
    // Get memory for the base and extended tables and read them from
    // memory.
    //

    MpsBaseTable = malloc( PcMpCfgTable.TableLength - sizeof(PcMpCfgTable));
    if (!MpsBaseTable) {
        dprintf(
            "Could not allocate %d bytes local memory, quitting.\n",
            PcMpCfgTable.TableLength - sizeof(PcMpCfgTable)
            );
        return;
    }

    if (!xReadMemory(((PUCHAR)PcMpTablePtr) + sizeof(PcMpCfgTable),
                     MpsBaseTable,
                     PcMpCfgTable.TableLength - sizeof(PcMpCfgTable))) {
        dprintf("Failed to read MPS Base Table from host memory.  Quitting.\n");
        goto cleanup;
    }

    if (PcMpCfgTable.ExtTableLength) {
        MpsExtendedTable = malloc(PcMpCfgTable.ExtTableLength);
        if (!MpsExtendedTable) {
            dprintf(
                "Could not allocate %d bytes local memory for extended MPS Table, quitting.\n",
            PcMpCfgTable.ExtTableLength
            );
            goto cleanup;
        }

        if (!xReadMemory(((PUCHAR)PcMpTablePtr) + PcMpCfgTable.TableLength,
                         MpsExtendedTable,
                         PcMpCfgTable.ExtTableLength)) {
            dprintf(
                "Could not read MPS Extended table from host memory.\n"
                "Will attempt to dump base structures.\n"
                );
            free(MpsExtendedTable);
            MpsExtendedTable = NULL;
        }
        MpsExtendedTableEnd = MpsExtendedTable + PcMpCfgTable.ExtTableLength;
    }

    //
    // Validate checksums.
    //
    // Base checksum is the sum of all bytes (inc checksum) in the
    // base table (including the fixed header).
    //

    c = 0;

    //
    // Sum fixed header.
    //

    bp = (PUCHAR)&PcMpCfgTable;
    for (i = 0; i < sizeof(PcMpCfgTable); i++) {
        c += *bp++;
    }

    //
    // Add rest of base table.
    //

    bp = MpsBaseTable;
    for (i = 0; i < PcMpCfgTable.TableLength - sizeof(PcMpCfgTable); i++) {
        c += *bp++;
    }

    //
    // The result should be zero.
    //

    if (c) {
        dprintf(
            "MPS Base Table checksum is in error.\n"
            "Found 0x%02x, Computed 0x%02x (Total 0x%02x).\n",
            PcMpCfgTable.Checksum,
            (UCHAR)(c - PcMpCfgTable.Checksum),
            c
            );
    }

    //
    // Now do the extended table checksum.  This one doesn't include
    // itself so we should just match (rather than end up with zero).
    //

    if (MpsExtendedTable) {
        c = 0;
        bp = MpsExtendedTable;
        for (i = 0; i < PcMpCfgTable.ExtTableLength; i++) {
            c += *bp++;
        }

        //
        // To sum to zero it needs to end up being it's opposite.
        //

        c = -c;

        if (c != PcMpCfgTable.ExtTableChecksum) {
            dprintf(
                "MPS Extended Table checksum is in error.\n"
                "Found 0x%02x, Computed 0x%02x.\n",
                PcMpCfgTable.ExtTableChecksum,
                c
                );
        }
    }

    //
    // Dump the base table.
    //

    if (!mpsBaseTable(MpsBaseTable, PcMpCfgTable.NumOfEntries)) {
        goto cleanup;
    }


    //
    // Dump the extended table.
    //

    if (!mpsExtendedTable(MpsExtendedTable, MpsExtendedTableEnd)) {
        goto cleanup;
    }

cleanup:
    if (MpsBaseTable) {
        free(MpsBaseTable);
    }
    if (MpsExtendedTable) {
        free(MpsExtendedTable);
    }
}
