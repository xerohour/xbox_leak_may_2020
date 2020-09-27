MAJORCOMP=crt
MINORCOMP=time

TARGETNAME=time

!INCLUDE ..\..\crt32.inc

INCLUDES=$(INCLUDES);..\..\extinc

SOURCES= \
    ..\asctime.c   \
    ..\clock.c     \
    ..\ctime.c     \
    ..\days.c      \
    ..\difftime.c  \
    ..\dtoxtime.c  \
    ..\ftime.c     \
    ..\gmtime.c    \
    ..\localtim.c  \
    ..\mktime.c    \
    ..\strftime.c  \
    ..\strdate.c   \
    ..\strtime.c   \
    ..\time.c      \
    ..\timeset.c   \
    ..\tzset.c     \
    ..\utime.c     \
    ..\ctime64.c   \
    ..\dtoxtm64.c  \
    ..\ftime64.c   \
    ..\gmtime64.c  \
    ..\loctim64.c  \
    ..\mktime64.c  \
    ..\time64.c    \
    ..\systime.c   \
    ..\utime64.c   \
    ..\wcsftime.c  \
    ..\wasctime.c  \
    ..\wctime.c    \
    ..\wctime64.c  \
    ..\wstrdate.c  \
    ..\wstrtime.c  \
    ..\wutime.c    \
    ..\wutime64.c
