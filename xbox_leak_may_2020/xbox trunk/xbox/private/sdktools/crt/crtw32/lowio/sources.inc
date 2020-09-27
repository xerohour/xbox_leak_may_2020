MAJORCOMP=crt
MINORCOMP=lowio

TARGETNAME=lowio

!INCLUDE ..\..\crt32.inc

SOURCES=\
        ..\chsize.c   \
        ..\close.c    \
        ..\commit.c   \
        ..\creat.c    \
        ..\dup.c      \
        ..\dup2.c     \
        ..\eof.c      \
        ..\flength.c  \
        ..\fstat.c    \
        ..\isatty.c   \
        ..\locking.c  \
        ..\lseek.c    \
        ..\mktemp.c   \
        ..\open.c     \
        ..\osfinfo.c  \
        ..\read.c     \
        ..\setmode.c  \
        ..\tell.c     \
        ..\txtmode.c  \
        ..\write.c    \
        ..\fleni64.c  \
        ..\fstat64.c  \
        ..\fstati64.c \
        ..\ioinit.c   \
        ..\lseeki64.c \
        ..\pipe.c     \
        ..\telli64.c  \
        ..\cgets.c    \
        ..\cgetws.c   \
        ..\cputs.c    \
        ..\getch.c    \
        ..\getwch.c   \
        ..\putch.c    \
        ..\putwch.c   \
        ..\wcreat.c   \
        ..\wmktemp.c  \
        ..\wopen.c

i386_SOURCES=\
        ..\inp.asm    \
        ..\outp.asm
