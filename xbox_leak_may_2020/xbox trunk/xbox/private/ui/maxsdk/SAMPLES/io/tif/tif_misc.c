//
// Tom's miscellaneous TIF support
//

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

void
TIFFError(char *module, char *fmt, ...)
	{
	va_list ap;
	char tmp_buffer[256];

	va_start(ap, fmt);
	vsprintf(tmp_buffer, fmt, ap);
	va_end(ap);
//	OutputDebugString(tmp_buffer);
}

void
TIFFWarning(char *module, char *fmt, ...)
	{
	va_list ap;
	char tmp_buffer[256];

	va_start(ap, fmt);
	vsprintf(tmp_buffer, fmt, ap);
	va_end(ap);
//	OutputDebugString(tmp_buffer);
}
