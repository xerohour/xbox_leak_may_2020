#include "precomp.hpp"

VOID HelpStats(VOID)
{
    dprintf("Stats - dumps various counters\n");
    dprintf("        No options dumps everything\n");
    dprintf("  -a: dump API counters\n");
    dprintf("  -r: dump render state counters\n");
    dprintf("  -t: dump texture stage state counters\n");
    dprintf("  -i: dump internal counters\n");
    dprintf("  -p: dump perf counters\n");
    dprintf("  -h: display help info\n");
    dprintf("  -?: display help info\n");
}

VOID HelpHelp(VOID)
{
    dprintf("Help - shows this help info\n");
}

VOID HelpDumper(VOID)
{
    dprintf("Dumper - dumps push-buffer / hardware state\n");
}

