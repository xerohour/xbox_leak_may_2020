/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    .c

Abstract:

    WinDbg Extension Api

Author:

    Ramon J San Andres (ramonsa) 5-Nov-1993

Environment:

    User Mode.

Revision History:

--*/

VOID
CommonHelp (
    VOID
    );



DECLARE_API( help  )

/*++

Routine Description:

    Displays help

Arguments:

    args - [ user | kernel ]

Return Value:

    None

--*/

{
    //
    //  Display common help
    //
    CommonHelp();

    //
    //  Display platform-specific help
    //
    SpecificHelp();
}

VOID
CommonHelp (
    VOID
    )
{
    dprintf("?                            - Displays this list\n" );
    dprintf("bushnd [address]             - Dump a HAL \"BUS HANDLER\" structure\n");
    dprintf("                               [address] if specified is the handler\n");
    dprintf("                               to be dumped. If not specified, dumps\n");
    dprintf("                               the list of handlers (brief).\n");
    dprintf("ca <address> [flags]         - Dump the control area of a section\n");
    dprintf("calldata <table name>        - Dump call data hash table\n" );
    dprintf("cmreslist <CM Resource List> - Dump CM resource list\n" );
    dprintf("db <physical address>        - Display physical memory\n");
    dprintf("dd <physical address>        - Display physical memory\n");
    dprintf("dblink <address> [count] [bias] - Dumps a list via its blinks\n");
    dprintf("dflink <address> [count] [bias] - Dumps a list via its flinks\n");
    dprintf("       bias - a mask of bits to ignore in each pointer\n");
    dprintf("defwrites                    - Dumps the deferred write queue and\n");
    dprintf("                                and triages cached write throttles\n");
    dprintf("devobj <device>             - Dump the device object and Irp queue\n");
    dprintf("                   <device> - device object address or name\n");
    dprintf("devstack <device>           - Dump device stack associated with device object\n");
    dprintf("drvobj <driver> [flags]     - Dump the driver object and related information\n");
    dprintf("                   <driver> - driver object address or name\n");
    dprintf("                   flags:1  - Dump device object list\n");
    dprintf("                   flags:2  - Dump driver entry points\n");
    dprintf("drivers                     - Display information about all loaded system modules\n");
    dprintf("eb <physical address> <byte>  <byte, byte ,...> - modify physical memory\n");
    dprintf("ed <physical address> <dword> <dword,dword,...> - modify physical memory\n");
    dprintf("exqueue [flags]             - Dump the ExWorkerQueues\n");
    dprintf("        flags:     1/2/4    - same as !thread/!process\n");
    dprintf("                   10       - only critical work queue\n");
    dprintf("                   20       - only delayed work queue\n");
    dprintf("                   40       - only hypercritical work queue\n");
    dprintf("exr <address>               - Dump exception record at specified address\n");
    dprintf("filecache                   - Dumps information about the file system cache\n");
    dprintf("filetime                    - Dumps a 64-bit FILETIME as a human-readable time\n");
    dprintf("filelock <address>          - Dump a file lock structure\n");
    dprintf("fpsearch <address>          - Find a freed special pool allocation\n");
    dprintf("frag [flags]                - Kernel mode pool fragmentation\n");
    dprintf("     flags:  1 - List all fragment information\n");
    dprintf("             2 - List allocation information\n");
    dprintf("             3 - both\n");
    dprintf("gentable <address> - dumps the given rtl_generic_table\n");
    dprintf("handle <addr> <flags> <TypeName> -  Dumps handle for a process\n");
    dprintf("       flags:  -2 Dump non-paged object\n");
    dprintf("heap <addr> [flags]         - Dumps heap for a process\n");
    dprintf("       flags:  -v Verbose\n");
    dprintf("               -f Free List entries\n");
    dprintf("               -a All entries\n");
    dprintf("               -s Summary\n");
    dprintf("               -x Force a dump even if the data is bad\n");
    dprintf("       address: desired heap to dump or 0 for all\n");
    dprintf("help                         - Displays this list\n" );
    dprintf("HidPpd <address> <flags>     - Dump Preparsed Data of HID device\n");
    dprintf("ib <port>                    - Read a byte from an I/O port\n");
    dprintf("id <port>                    - Read a double-word from an I/O port\n");
    dprintf("iw <port>                    - Read a word from an I/O port\n");
    dprintf("ioreslist <IO Resource List> - Dump IO resource requirements list\n" );
    dprintf("irp <address> <dumplevel>    - Dump Irp at specified address\n");
    dprintf("                 address == 0   Dump active IRPs (checked only)\n");
    dprintf("                 dumplevel: 0   Basic stack info\n");
    dprintf("                 dumplevel: 1   Full field dump\n");
    dprintf("                 dumplevel: 2   Include tracking information (checked only)\n");
    dprintf("irpfind [pooltype] [restart addr] [<irpsearch> <address>]- Search pool for active Irps\n");
    dprintf("     pooltype is 0 for nonpaged pool (default)\n");
    dprintf("     pooltype is 1 for paged pool\n");
    dprintf("     pooltype is 2 for special pool\n");
    dprintf("     restart addr - if specfied, scan will be restarted from \n");
    dprintf("                    this location in pool\n");
    dprintf("     <irpsearch> - specifies filter criteria to find a specific irp\n");
    dprintf("           'userevent' - finds IRPs where Irp.UserEvent == <address>\n");
    dprintf("           'device' - finds IRPs with a stack location where DeviceObject == <address>\n");
    dprintf("           'fileobject' - finds IRPs where Irp.Tail.Overlay.OriginalFileObject == <address>\n");
    dprintf("           'mdlprocess' - finds IRPs where Irp.MdlAddress.Process == <address>\n");
    dprintf("           'thread' - finds IRPs where Irp.Tail.Overlay.Thread == <address>\n");
    dprintf("           'arg' - finds IRPs with one of the args == <address>\n");
    dprintf("locks [-v] <address>         - Dump kernel mode resource locks\n");
    dprintf("memusage                    - Dumps the page frame database table\n");
    dprintf("ob <port>                   - Write a byte to an I/O port\n");
    dprintf("obja <TypeName>             - Dumps an object manager object's attributes\n");
    dprintf("object <-r | Path | address | 0 TypeName>  - Dumps an object manager object\n");
    dprintf("       -r   -  Force reload of cached object pointers\n");
    dprintf("od <port>                   - Write a double-word to an I/O port\n");
    dprintf("ow <port>                   - Write a word to an I/O port\n");
    dprintf("ohcd                        - Performs checks and dumps information on the OpenHCI state,\n");
    dprintf("                              including: dumping all endpoints in the schedule, check the\n");
    dprintf("                              bandwidth tables for internal consistency, display summary bandwidth\n");
    dprintf("                              statics.\n");
    dprintf("patch                       - Enable and disable various driver flags\n");
    dprintf("pfn                         - Dumps the page frame database entry for the physical page\n");
    dprintf("pnpevent <event entry> - Dump PNP events\n");
    dprintf("       event entry: 0       - list all queued events\n");
    dprintf("                    address - list specified event\n");
    dprintf("pool <address> [detail]     - Dump kernel mode heap\n");
    dprintf("        address: 0 or blank - Only the process heap\n");
    dprintf("                         -1 - All heaps in the process\n");
    dprintf("              Otherwise for the heap address listed\n");
    dprintf("     detail:  0 - Summary Information\n");
    dprintf("              1 - Above + location/size of regions\n");
    dprintf("              2 - Print information only for address\n");
    dprintf("              3 - Above + allocated/free blocks in committed regions\n");
    dprintf("              4 - Above + free lists\n");
    dprintf("poolfind Tag [pooltype] -   - Finds occurrences of the specified Tag\n");
    dprintf("     Tag is 4 character tag, * and ? are wild cards\n");
    dprintf("     pooltype is 0 for nonpaged pool (default)\n");
    dprintf("     pooltype is 1 for paged pool\n");
    dprintf("     pooltype is 2 for special pool\n");
    dprintf("   NOTE - this can take a long time!\n");
    dprintf("poolused [flags [TAG]]      - Dump usage by pool tag\n");
    dprintf("       flags:  1 Verbose\n");
    dprintf("       flags:  2 Sort by NonPagedPool Usage\n");
    dprintf("       flags:  4 Sort by PagedPool Usage\n");
    dprintf("process [flags] [image name] - Dumps process at specified address\n");
    dprintf("                (dumps only the process with specified image name, if given)\n");
    dprintf("        flags:         1       - don't stop after cid/image information\n");
    dprintf("                       2       - dump thread wait states\n");
    dprintf("                       4       - dump only thread states, combine with 2 to get stack\n");
    dprintf("ptov PhysicalPageNumber     - Dump all valid physical<->virtual mappings\n");
    dprintf("                              for the given page directory\n");
    dprintf("range <RtlRangeList>        - Dump RTL_RANGE_LIST\n");
    dprintf("ready                       - Dumps state of all READY system threads\n");
    dprintf("sd <Address> [flags]        - Displays the SECURITY_DESCRIPTOR\n" );
    dprintf("sid <Address> [flags]       - Displays the SID\n" );
    dprintf("srb <address>               - Dump Srb at specified address\n");
    dprintf("stacks <detail-level>       - Dump summary of current kernel stacks\n");
    dprintf("            detail-level: 0    Display stack summary\n");
    dprintf("            detail-level: 1    Display stacks, no parameters\n");
    dprintf("            detail-level: 2    Display stacks, full parameters\n");
    dprintf("sysptes                     - Dumps the system PTEs\n");
    dprintf("thread [flags]              - Dump current thread, or specified thread,\n");
    dprintf("                                  or with stack containing address\n");
    dprintf("        flags:         1       - not used\n");
    dprintf("                       2       - dump thread wait states\n");
    dprintf("                       4       - dump only thread states, combine with 2 to get stack\n");
    dprintf("threadfields                - Show offsets to all fields in a thread\n");
    dprintf("time                        - Reports PerformanceCounterRate and TimerDifference\n");
    dprintf("timer                       - Dumps timer tree\n");
    dprintf("trap <address>              - Dump a trap frame\n");
    dprintf("urb <address> <flags>       - Dump a USB Request Block\n");
    dprintf("usbstruc <address> <type>   - Print out an USB HC descriptor of <type>\n");
    dprintf("                   <type>   - {OHCIReg | HCCA | OHCIHcdED | OHCIHcdTD |\n"
            "                               OHCIEndpoint | DevData | UHCDReg  }\n");
    dprintf("vad                         - Dumps VADs\n");
    dprintf("version                     - Version of extension dll\n");
    dprintf("vm                          - Dumps virtual management values\n");
    dprintf("vpd <address>               - Dumps volume parameter block\n");
    dprintf("vtop DirBase address        - Dumps physical page for virtual address\n");
    dprintf("wdmaud <address> <flags>    - Dumps wdmaud data for structures\n");
    dprintf("       flags:  1 - Ioctl History Dump given WdmaIoctlHistoryListHead\n");
    dprintf("       flags:  2 - Pending Irps given WdmaPendingIrpListHead\n");
    dprintf("       flags:  4 - Allocated MDLs given WdmaAllocatedMdlListHead\n");
    dprintf("       flags:  8 - pContext Dump given WdmaContextListHead\n");
    dprintf("       flags:100 - Verbose\n");
    dprintf("xpptree                     - Dumps the Xbox Peripheral Port Tree\n");
    dprintf("zombies                     - Find all zombie processes\n");
}
