/*++

Copyright (c) 2000 Microsoft Corporation

    string.cpp

Abstract:

    String functions, so we don't need to include msvcrt.

Revision History:

    07-11-00    vadimg      created

--*/

#include "precomp.h"

char* str_search(const char *str1, const char *str2)
{
    char *cp = (char*)str1;
    char *s1, *s2;

    while (*cp) {
        s1 = cp;
        s2 = (char*)str2;

        while (*s1 && *s2 && !(*s1-*s2)) {
            s1++, s2++;
        }

        if (!*s2) {
            return cp;
        }

        cp++;
    }

    return NULL;
}

char* str_add(char *s1, const char *s2)
{
    while (*s2) {
        *s1 = *s2;
        s1++, s2++;
    }

    *s1 = '\0';

    return s1;
}

