
#include <stdio.h>
#include <xtl.h>
#include <xdbg.h>

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#define CONSTANT_UNICODE_STRING(s)   { sizeof( s ) - sizeof( WCHAR ), sizeof( s ), s }

