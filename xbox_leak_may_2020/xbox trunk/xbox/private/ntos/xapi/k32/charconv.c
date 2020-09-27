#include "basedll.h"

LPSTR
WINAPI
CharUpperA(
    LPSTR psz
    )
{
    if (HIWORD(psz))
    {
        LPSTR pszCur = psz;
        while (*pszCur)
        {
            *pszCur = RtlUpperChar(*pszCur);
            pszCur++;
        }
        return psz;
    }
    else
    {
        return (LPSTR)RtlUpperChar((CHAR)psz);
    }
}

LPSTR
WINAPI
CharLowerA(
    LPSTR psz
    )
{
    if (HIWORD(psz))
    {
        LPSTR pszCur = psz;
        while (*pszCur)
        {
            *pszCur = RtlLowerChar(*pszCur);
            pszCur++;
        }
        return psz;
    }
    else
    {
        return (LPSTR)RtlLowerChar((CHAR)psz);
    }
}

LPWSTR
WINAPI
CharUpperW(
    LPWSTR psz
    )
{
    if (HIWORD(psz))
    {
        LPWSTR pszCur = psz;
        while (*pszCur)
        {
            *pszCur = RtlUpcaseUnicodeChar(*pszCur);
            pszCur++;
        }
        return psz;
    }
    else
    {
        return (LPWSTR)RtlUpcaseUnicodeChar((WCHAR)psz);
    }
}

LPWSTR
WINAPI
CharLowerW(
    LPWSTR psz
    )
{
    if (HIWORD(psz))
    {
        LPWSTR pszCur = psz;
        while (*pszCur)
        {
            *pszCur = RtlDowncaseUnicodeChar(*pszCur);
            pszCur++;
        }
        return psz;
    }
    else
    {
        return (LPWSTR)RtlDowncaseUnicodeChar((WCHAR)psz);
    }
}
