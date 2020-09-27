MAJORCOMP=crt
MINORCOMP=rtc

TARGETNAME=rtc

!INCLUDE ..\..\crt32.inc

C_DEFINES=$(C_DEFINES) -D_RTC

#removed pdblkup.cpp
SOURCES=\
    ..\bintree.cpp  \
    ..\callsite.cpp \
    ..\chsyheap.cpp \
    ..\contain.cpp  \
    ..\convert.cpp  \
    ..\error.cpp    \
    ..\fileio.cpp   \
    ..\init.cpp     \
    ..\initsect.cpp \
    ..\memory.cpp   \
    ..\shadow.cpp   \
    ..\stack.cpp    \
    ..\userapi.cpp
