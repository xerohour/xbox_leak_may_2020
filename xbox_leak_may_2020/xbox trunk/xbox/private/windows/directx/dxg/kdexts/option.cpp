#include "precomp.hpp"

BOOL IsOption(const char *s, char opt)
{
    while (s != NULL && *s != '\0')
    {
        if (*s == '-')
        {
            while (*++s)
            {
                if (*s == opt)
                {
                    return TRUE;
                }
            }
        }
        else
        {
            s++;
        }
    }
    return FALSE;
}

