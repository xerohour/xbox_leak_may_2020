MAJORCOMP=crt
MINORCOMP=string

TARGETNAME=string

!INCLUDE ..\..\crt32.inc

INCLUDES=$(INCLUDES);..

SOURCES=\
    ..\strcoll.c   \
    ..\stricoll.c  \
    ..\strlwr.c    \
    ..\strncoll.c  \
    ..\strnicol.c  \
    ..\strtok.c    \
    ..\strupr.c    \
    ..\strxfrm.c   \
    ..\wcscat.c    \
    ..\wcschr.c    \
    ..\wcscmp.c    \
    ..\wcscoll.c   \
    ..\wcscspn.c   \
    ..\wcsdup.c    \
    ..\wcsicmp.c   \
    ..\wcsicoll.c  \
    ..\wcslen.c    \
    ..\wcslwr.c    \
    ..\wcsncat.c   \
    ..\wcsncmp.c   \
    ..\wcsncoll.c  \
    ..\wcsncpy.c   \
    ..\wcsnicmp.c  \
    ..\wcsnicol.c  \
    ..\wcsnset.c   \
    ..\wcspbrk.c   \
    ..\wcsrchr.c   \
    ..\wcsrev.c    \
    ..\wcsset.c    \
    ..\wcsspn.c    \
    ..\wcsstr.c    \
    ..\wcstok.c    \
    ..\wcsupr.c    \
    ..\wcsxfrm.c
    
i386_SOURCES=\
    ..\memcpy.asm    \
    ..\memchr.asm    \
    ..\memcmp.asm    \
    ..\memccpy.asm   \
    ..\_memicmp.asm  \
    ..\memmove.asm   \
    ..\memset.asm    \
    ..\strcat.asm    \
    ..\strchr.asm    \
    ..\strcmp.asm    \
    ..\strcspn.asm   \
    ..\strdup.asm    \
    ..\_stricmp.asm  \
    ..\strlen.asm    \
    ..\strncat.asm   \
    ..\strncmp.asm   \
    ..\strncpy.asm   \
    ..\_strnicm.asm  \
    ..\strnset.asm   \
    ..\strpbrk.asm   \
    ..\strrchr.asm   \
    ..\strrev.asm    \
    ..\strset.asm    \
    ..\strspn.asm    \
    ..\strstr.asm    \
    ..\memicmp.c   \
    ..\stricmp.c   \
    ..\strnicmp.c
