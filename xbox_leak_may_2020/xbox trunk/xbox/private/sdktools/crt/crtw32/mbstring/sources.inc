MAJORCOMP=crt
MINORCOMP=mbstring

TARGETNAME=mbstring

!INCLUDE ..\..\crt32.inc

INCLUDES = $(INCLUDES);..\..\string\i386

C_DEFINES=$(C_DEFINES) -D_POSIX_

SOURCES=\
        ..\ismbalnm.c  \
        ..\ismbalph.c  \
        ..\ismbbyte.c  \
        ..\ismbdgt.c   \
        ..\ismbgrph.c  \
        ..\ismbknj.c   \
        ..\ismblgl.c   \
        ..\ismblwr.c   \
        ..\ismbprn.c   \
        ..\ismbpunc.c  \
        ..\ismbsle.c   \
        ..\ismbspc.c   \
        ..\ismbstr.c   \
        ..\ismbupr.c   \
        ..\mbbtype.c   \
        ..\mbccpy.c    \
        ..\mbclen.c    \
        ..\mbclevel.c  \
        ..\mbctype.c   \
        ..\mbsbtype.c  \
        ..\mbschr.c    \
        ..\mbscmp.c    \
        ..\mbscoll.c   \
        ..\mbscspn.c   \
        ..\mbsdec.c    \
        ..\mbsicmp.c   \
        ..\mbsicoll.c  \
        ..\mbsinc.c    \
        ..\mbslen.c    \
        ..\mbslwr.c    \
        ..\mbsnbcat.c  \
        ..\mbsnbcmp.c  \
        ..\mbsnbcnt.c  \
        ..\mbsnbcol.c  \
        ..\mbsnbcpy.c  \
        ..\mbsnbicm.c  \
        ..\mbsnbico.c  \
        ..\mbsnbset.c  \
        ..\mbsncat.c   \
        ..\mbsnccnt.c  \
        ..\mbsncmp.c   \
        ..\mbsncoll.c  \
        ..\mbsncpy.c   \
        ..\mbsnextc.c  \
        ..\mbsnicmp.c  \
        ..\mbsnicol.c  \
        ..\mbsninc.c   \
        ..\mbsnset.c   \
        ..\mbspbrk.c   \
        ..\mbsrchr.c   \
        ..\mbsrev.c    \
        ..\mbsset.c    \
        ..\mbsspn.c    \
        ..\mbsspnp.c   \
        ..\mbsstr.c    \
        ..\mbstok.c    \
        ..\mbsupr.c    \
        ..\mbtohira.c  \
        ..\mbtokata.c  \
        ..\mbtolwr.c   \
        ..\mbtoupr.c   \
        ..\tojisjms.c  \
        ..\tombbmbc.c
