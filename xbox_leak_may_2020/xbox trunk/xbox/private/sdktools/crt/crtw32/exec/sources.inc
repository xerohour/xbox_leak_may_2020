MAJORCOMP=crt
MINORCOMP=exec

TARGETNAME=exec

!INCLUDE ..\..\crt32.inc

# removed loaddll.c cenvarg.c wcenvarg.c getproc.c

SOURCES=\
        ..\fileinfo.c  \
        ..\dospawn.c   \
        ..\execl.c     \
        ..\execle.c    \
        ..\execlp.c    \
        ..\execlpe.c   \
        ..\execv.c     \
        ..\execve.c    \
        ..\execvp.c    \
        ..\execvpe.c   \
        ..\spawnl.c    \
        ..\spawnle.c   \
        ..\spawnlp.c   \
        ..\spawnlpe.c  \
        ..\spawnv.c    \
        ..\spawnve.c   \
        ..\spawnvp.c   \
        ..\spawnvpe.c  \
        ..\system.c    \
        ..\wait.c      \
        ..\wdospawn.c  \
        ..\wexecl.c    \
        ..\wexecle.c   \
        ..\wexeclp.c   \
        ..\wexeclpe.c  \
        ..\wexecv.c    \
        ..\wexecve.c   \
        ..\wexecvp.c   \
        ..\wexecvpe.c  \
        ..\wspawnl.c   \
        ..\wspawnle.c  \
        ..\wspawnlp.c  \
        ..\wspwnlpe.c  \
        ..\wspawnv.c   \
        ..\wspawnve.c  \
        ..\wspawnvp.c  \
        ..\wspwnvpe.c  \
        ..\wsystem.c
