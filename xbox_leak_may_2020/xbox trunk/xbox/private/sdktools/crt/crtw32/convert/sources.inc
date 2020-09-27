MAJORCOMP=crt
MINORCOMP=convert

TARGETNAME=convert

!INCLUDE ..\..\crt32.inc

SOURCES=\
        ..\_ctype.c   \
        ..\_fptostr.c \
        ..\_mbslen.c  \
        ..\_wctype.c  \
        ..\atof.c     \
        ..\atox.c     \
        ..\fcvt.c     \
        ..\gcvt.c     \
        ..\isctype.c  \
        ..\iswctype.c \
        ..\mblen.c    \
        ..\mbstowcs.c \
        ..\mbtowc.c   \
        ..\strtod.c   \
        ..\strtol.c   \
        ..\strtoq.c   \
        ..\swab.c     \
        ..\tolower.c  \
        ..\toupper.c  \
        ..\towlower.c \
        ..\towupper.c \
        ..\wchtodig.c \
        ..\wcstod.c   \
        ..\wcstol.c   \
        ..\wcstoq.c   \
        ..\wcstombs.c \
        ..\wctomb.c   \
        ..\wtof.c     \
        ..\wtox.c     \
        ..\xtoa.c     \
        ..\xtow.c
