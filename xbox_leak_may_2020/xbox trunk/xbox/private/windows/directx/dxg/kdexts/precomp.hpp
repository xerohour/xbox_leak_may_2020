// public headers
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#define NOGDI
#include <windows.h>
#include <wdbgexts.h>
#include <ntverp.h>
#include <d3d8.h>

// DXG private headers
#include "nv32.h"
#include "stats.hpp"
#include "dumper.hpp"

BOOL IsOption(const char *s, char opt);

VOID HelpStats(VOID);
VOID HelpHelp(VOID);
VOID HelpDumper(VOID);

