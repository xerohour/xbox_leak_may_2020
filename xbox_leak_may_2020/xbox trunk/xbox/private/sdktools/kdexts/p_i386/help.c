#include "precomp.h"
#pragma hdrstop

#include "..\help.c"



VOID
SpecificHelp (
    VOID
    )
{
    dprintf("\n");
    dprintf("X86-specific:\n\n");
    dprintf("callback <address> [num]       - Dump callback frames for specified thread\n");
    dprintf("cxr                            - Dump context record at specified address\n");
    dprintf("dcs <Bus>.<Dev>.<Fn>           - Dump PCI ConfigSpace of device\n");
    dprintf("exca <BasePort>.<SktNum>       - Dump ExCA registers\n");
    dprintf("mtrr                           - Dumps MTTR\n");
    dprintf("npx [base]                     - Dumps NPX save area\n");
    dprintf("pci [flag] [bus] [device] [function] [rawdump:minaddr] [maxaddr] - Dumps pci type1 config\n");
    dprintf("    flag: 0x01 - verbose\n");
    dprintf("          0x02 - from bus 0 to 'bus'\n");
    dprintf("          0x04 - dump raw bytes\n");
    dprintf("          0x08 - dump raw dwords\n");
    dprintf("          0x10 - do not skip invalid devices\n");
    dprintf("          0x20 - do not skip invalid functions\n");
    dprintf("          0x40 - dump Capabilities if found\n");
    dprintf("          0x80 - dump device specific on VendorID:8086\n");
    dprintf("pcr                            - Dumps the PCR\n");
    dprintf("pic                            - Dumps PIC(8259) information\n");
    dprintf("pte                            - Dumps the corresponding PDE and PTE for the entered address\n");
    dprintf("sel [selector]                 - Examine selector values\n");
    dprintf("trap [base]                    - Dump trap frame\n");
    dprintf("tss [register]                 - Dump TSS\n");
    dprintf("\n");
}
