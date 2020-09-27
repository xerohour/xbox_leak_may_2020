MAJORCOMP=crt
MINORCOMP=heap

TARGETNAME=heap

!INCLUDE ..\..\crt32.inc

C_DEFINES = $(C_DEFINES) -DWINHEAP=1

# removed resetstk.c

SOURCES=\
    ..\calloc.c    \
    ..\align.c     \
    ..\delete.c    \
    ..\delete2.c   \
    ..\expand.c    \
    ..\free.c      \
    ..\handler.c   \
    ..\heapadd.c   \
    ..\heapchk.c   \
    ..\heapinit.c  \
    ..\heapmin.c   \
    ..\heapused.c  \
    ..\heapwalk.c  \
    ..\hpabort.c   \
    ..\malloc.c    \
    ..\msize.c     \
    ..\new.c       \
    ..\new2.c      \
    ..\new_mode.c  \
    ..\_newmode.c  \
    ..\realloc.c   \
    ..\sbheap.c    \
    ..\setnewh.c
