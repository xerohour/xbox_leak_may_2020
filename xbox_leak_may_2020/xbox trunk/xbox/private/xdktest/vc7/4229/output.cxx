#include "names.h"
#include "types.h"
#include "game.h"

void myprintf(const char *fmt, ...)
{
	static char buf[1024];
	va_list arg;
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);
	OutputDebugString(buf);
}

void PrintCustomFault(const char *fmt, ...)
{
	static char buf[1024];
	va_list arg;
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);
	OutputDebugString(buf);
	for(;;)
	{
		Sleep(1000);
	}
}
