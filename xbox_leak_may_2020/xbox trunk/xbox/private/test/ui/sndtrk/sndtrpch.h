
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
#include <ntos.h>
#include <stdio.h>
#include <xtl.h>
#include <stdio.h>
#define XDBG_COMPILE_LEVEL XDBG_TRACE
#include <xdbg.h>
#define XFONT_TRUETYPE
#include <xfont.h>



#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }

