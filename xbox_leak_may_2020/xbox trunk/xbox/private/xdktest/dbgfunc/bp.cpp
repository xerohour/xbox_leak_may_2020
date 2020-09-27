#include "dbgtest.h"

void Test_Breakpoints()
{
    int i = 0;
    int j = 1;

    // set a breakpoint inside this broken-up line
    // thanks to danrose for finding this bug

    i
    =
    i
    +
    1
    ;

    
    while (j)
        ;
}
