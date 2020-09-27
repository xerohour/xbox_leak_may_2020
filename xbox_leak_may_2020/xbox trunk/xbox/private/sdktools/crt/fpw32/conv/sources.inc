!INCLUDE ..\..\fp32.inc

MAJORCOMP=fp
MINORCOMP=conv

TARGETNAME=conv

INCLUDES=..\..\include;..\..\..\crtw32\h

SOURCES=\
        ..\cfin.c      \
        ..\cfout.c     \
        ..\constpow.c  \
        ..\cvt.c       \
        ..\fpinit.c    \
        ..\intrncvt.c  \
        ..\mantold.c   \
        ..\strgtold.c  \
        ..\tenpow.c    \
        ..\x10fout.c   \
        ..\fltinf.c
