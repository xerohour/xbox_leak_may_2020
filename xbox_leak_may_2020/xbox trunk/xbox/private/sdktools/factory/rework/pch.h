#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddscsi.h>
#include <stdio.h>
#include <xtl.h>
#include "ntos.h"
#include <stdio.h>
#include <xdbg.h>
#define XFONT_TRUETYPE
#include <xfont.h>

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }


