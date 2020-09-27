//
// SHolden - need lstrcmpiA.
//


#include <tchar.h>

int __cdecl stricmp (const char *str1, const char *str2)
{
    for (;(*str1 != '\0') && (*str2 != '\0'); str1++, str2++) {
        if (_totupper(*str1) != _totupper(*str2)) {
            return *str1 - *str2;
        }
    }
    // Check last character.
    return *str1 - *str2;
}


