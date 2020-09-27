#include <xtl.h>

struct Die
{
    Die() {}
};

namespace doom
{
    void death();
}

void doom::death()
{
    Die msvc[2];
}

void __cdecl main()
{
    while (1)
        Sleep(500);
    return;
}

