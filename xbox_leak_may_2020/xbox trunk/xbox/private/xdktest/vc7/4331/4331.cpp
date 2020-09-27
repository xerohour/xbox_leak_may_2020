#include <xtl.h>

template <class T> struct boom
{
    static const int x = 3;
    static const int y = x + 1;
};

void __cdecl main() { return; }
