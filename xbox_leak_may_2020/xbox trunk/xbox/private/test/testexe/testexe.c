#include <xtl.h>

void __cdecl main()
{
    OutputDebugString(L"TestExe sleeping. Note that nothing useful\r\n"
        L"will happen after this, you might as well break in and reboot\r\n");
    Sleep(INFINITE);
}
