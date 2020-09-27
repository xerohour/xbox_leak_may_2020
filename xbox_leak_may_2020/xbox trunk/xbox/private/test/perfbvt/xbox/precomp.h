#include <stdio.h>
#include <stdlib.h>
#include <xtl.h>

// Not sure why such convoluted macro magic is needed.
// But I can't get #pragma message() to work directly.

#ifndef PERFBVT_SERVER
#define QUOTE0(a) #a
#define QUOTE1(a) QUOTE0(a)
#define MESSAGE(a) message(__FILE__ "(" QUOTE1(__LINE__) "): " a)
#define PERFBVT(s) MESSAGE("PERFBVT: " s)
#pragma PERFBVT("No server address defined. Skip network logging and tests.")
#pragma PERFBVT("To set server address, type this in your razzle build window:")
#pragma PERFBVT("   set PERFBVT_SERVER=aaa.bbb.ccc.ddd")
#endif

#include "perfbvt.h"
