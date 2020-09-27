MAJORCOMP=crt
MINORCOMP=dos

TARGETNAME=dos

!INCLUDE ..\..\crt32.inc

SOURCES=\
        ..\access.c    \
        ..\chmod.c     \
        ..\chdir.c     \
        ..\dosmap.c    \
        ..\drive.c     \
        ..\fullpath.c  \
        ..\getcwd.c    \
        ..\getpid.c    \
        ..\mkdir.c     \
        ..\rename.c    \
        ..\rmdir.c     \
        ..\stat.c      \
        ..\unlink.c    \
        ..\mterrno.c   \
        ..\stat64.c    \
        ..\stati64.c   \
        ..\waccess.c   \
        ..\wchdir.c    \
        ..\wchmod.c    \
        ..\wfullpat.c  \
        ..\wgetcwd.c   \
        ..\wmkdir.c    \
        ..\wrename.c   \
        ..\wrmdir.c    \
        ..\wstat.c     \
        ..\wstat64.c   \
        ..\wstati64.c  \
        ..\wunlink.c
