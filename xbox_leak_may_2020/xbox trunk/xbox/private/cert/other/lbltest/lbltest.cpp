#include "precomp.h"

int __cdecl main(int argc, char **argv)
{
    CLabelPrinter *ppr;
    int n = 0;

    if(argc < 2)
        return 1;

    if(argc > 2) {
        if(argv[2][0] == '-') {
            sscanf(argv[2] + 1, "%d", &n);
            n = -n;
        } else
            sscanf(argv[2], "%d", &n);
    }
    ppr = CLabelPrinter::PprCreate(argv[1], n);
    if(ppr && ppr->FStartLabel()) {
        ppr->PrintLine("Line 1\nLine 2\nLine 3\nLine 4\nLine 5\nLine 6\n");
        ppr->EndLabel();
    }
        
    return 0;
}
