MAJORCOMP=crt
MINORCOMP=stdcpp

TARGETNAME=stdcpp

!INCLUDE ..\..\crt32.inc

INCLUDES=$(INCLUDES);$(CRTBASE)\crtw32\stdhpp

C_DEFINES=$(C_DEFINES) /D_BUILD_LIBCP /FIlibcpver.h

SOURCES= \
    ..\cerr.cpp      \
    ..\cin.cpp       \
    ..\clog.cpp      \
    ..\cout.cpp      \
    ..\delaop2.cpp   \
    ..\delop2.cpp    \
    ..\fiopen.cpp    \
    ..\iomanip.cpp   \
    ..\ios.cpp       \
    ..\iosptrs.cpp   \
    ..\iostream.cpp  \
    ..\locale.cpp    \
    ..\locale0.cpp   \
    ..\newaop.cpp    \
    ..\newaop2.cpp   \
    ..\newop.cpp     \
    ..\newop2.cpp    \
    ..\nomemory.cpp  \
    ..\nothrow.cpp   \
    ..\raisehan.cpp  \
    ..\stdhndlr.cpp  \
    ..\stdthrow.cpp  \
    ..\string.cpp    \
    ..\strstrea.cpp  \
    ..\uncaught.cpp  \
    ..\ushcerr.cpp   \
    ..\ushcout.cpp   \
    ..\ushcin.cpp    \
    ..\ushclog.cpp   \
    ..\ushiostr.cpp  \
    ..\wcerr.cpp     \
    ..\wcin.cpp      \
    ..\wclog.cpp     \
    ..\wcout.cpp     \
    ..\wctrans.c     \
    ..\wctype.c      \
    ..\wiostrea.cpp  \
    ..\wlocale.cpp   \
    ..\xcosh.c       \
    ..\xdateord.cpp  \
    ..\xdebug.cpp    \
    ..\xdnorm.c      \
    ..\xdscale.c     \
    ..\xdtest.c      \
    ..\xexp.c        \
    ..\xfcosh.c      \
    ..\xfdnorm.c     \
    ..\xfdscale.c    \
    ..\xfdtest.c     \
    ..\xferaise.c    \
    ..\xfexp.c       \
    ..\xfsinh.c      \
    ..\xfvalues.c    \
    ..\xgetwctype.c  \
    ..\xlcosh.c      \
    ..\xldnorm.c     \
    ..\xldscale.c    \
    ..\xldtest.c     \
    ..\xlexp.c       \
    ..\xlocale.cpp   \
    ..\xlock.cpp     \
    ..\xlpoly.c      \
    ..\xlsinh.c      \
    ..\xlvalues.c    \
    ..\xmbtowc.c     \
    ..\xmtx.c        \
    ..\xmutex.cpp    \
    ..\xpoly.c       \
    ..\xsinh.c       \
    ..\xstod.c       \
    ..\xstrcoll.c    \
    ..\xstrxfrm.c    \
    ..\xtowlower.c   \
    ..\xtowupper.c   \
    ..\xvalues.c     \
    ..\xwcscoll.c    \
    ..\xwcsxfrm.c    \
    ..\xwctomb.c     \
    ..\_tolower.c    \
    ..\_toupper.c
