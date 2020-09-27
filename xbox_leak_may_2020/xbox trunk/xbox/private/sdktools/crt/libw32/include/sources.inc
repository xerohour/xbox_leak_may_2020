!if 0

   Sources file

!endif

TARGETNAME=
TARGETPATH=
TARGETTYPE=NOTARGET


SOURCES=

CRT_FILES1 = \
    $(CRT_INC_PATH)\assert.h \
    $(CRT_INC_PATH)\conio.h \
    $(CRT_INC_PATH)\crtdbg.h \
    $(CRT_INC_PATH)\ctype.h \
    $(CRT_INC_PATH)\direct.h \
    $(CRT_INC_PATH)\dos.h \
    $(CRT_INC_PATH)\eh.h \
    $(CRT_INC_PATH)\errno.h \
    $(CRT_INC_PATH)\excpt.h \
    $(CRT_INC_PATH)\fcntl.h \
    $(CRT_INC_PATH)\float.h \
    $(CRT_INC_PATH)\fpieee.h \
    $(CRT_INC_PATH)\fstream.h \
    $(CRT_INC_PATH)\io.h \
    $(CRT_INC_PATH)\iomanip.h \
    $(CRT_INC_PATH)\ios.h \
    $(CRT_INC_PATH)\iostream.h \
    $(CRT_INC_PATH)\istream.h \
    $(CRT_INC_PATH)\limits.h \
    $(CRT_INC_PATH)\locale.h \
    $(CRT_INC_PATH)\malloc.h \
    $(CRT_INC_PATH)\math.h \
    $(CRT_INC_PATH)\mbctype.h \
    $(CRT_INC_PATH)\mbstring.h \
    $(CRT_INC_PATH)\memory.h \
    $(CRT_INC_PATH)\minmax.h \
    $(CRT_INC_PATH)\new.h \
    $(CRT_INC_PATH)\ostream.h \
    $(CRT_INC_PATH)\process.h

CRT_FILES2 = \
    $(CRT_INC_PATH)\search.h \
    $(CRT_INC_PATH)\setjmp.h \
    $(CRT_INC_PATH)\setjmpex.h \
    $(CRT_INC_PATH)\share.h \
    $(CRT_INC_PATH)\signal.h \
    $(CRT_INC_PATH)\stdarg.h \
    $(CRT_INC_PATH)\stddef.h \
    $(CRT_INC_PATH)\stdexcpt.h \
    $(CRT_INC_PATH)\stdio.h \
    $(CRT_INC_PATH)\stdiostr.h \
    $(CRT_INC_PATH)\stdlib.h \
    $(CRT_INC_PATH)\streamb.h \
    $(CRT_INC_PATH)\string.h \
    $(CRT_INC_PATH)\strstrea.h \
    $(CRT_INC_PATH)\tchar.h \
    $(CRT_INC_PATH)\time.h \
    $(CRT_INC_PATH)\typeinfo.h \
    $(CRT_INC_PATH)\useoldio.h \
    $(CRT_INC_PATH)\use_ansi.h \
    $(CRT_INC_PATH)\varargs.h \
    $(CRT_INC_PATH)\wchar.h \
    $(CRT_INC_PATH)\wctype.h \
    $(CRT_INC_PATH)\sys\locking.h \
    $(CRT_INC_PATH)\sys\stat.h \
    $(CRT_INC_PATH)\sys\timeb.h \
    $(CRT_INC_PATH)\sys\types.h \
    $(CRT_INC_PATH)\sys\utime.h

STDHPP_FILES1 = \
    $(CRT_INC_PATH)\algorithm \
    $(CRT_INC_PATH)\bitset \
    $(CRT_INC_PATH)\cassert \
    $(CRT_INC_PATH)\cctype \
    $(CRT_INC_PATH)\cerrno \
    $(CRT_INC_PATH)\cfloat \
    $(CRT_INC_PATH)\ciso646 \
    $(CRT_INC_PATH)\climits \
    $(CRT_INC_PATH)\clocale \
    $(CRT_INC_PATH)\cmath \
    $(CRT_INC_PATH)\complex \
    $(CRT_INC_PATH)\csetjmp \
    $(CRT_INC_PATH)\csignal \
    $(CRT_INC_PATH)\cstdarg \
    $(CRT_INC_PATH)\cstddef \
    $(CRT_INC_PATH)\cstdio \
    $(CRT_INC_PATH)\cstdlib \
    $(CRT_INC_PATH)\cstring \
    $(CRT_INC_PATH)\ctime \
    $(CRT_INC_PATH)\ctime \
    $(CRT_INC_PATH)\cwctype \
    $(CRT_INC_PATH)\deque \
    $(CRT_INC_PATH)\exception \
    $(CRT_INC_PATH)\fstream \
    $(CRT_INC_PATH)\functional \
    $(CRT_INC_PATH)\iomanip \
    $(CRT_INC_PATH)\ios \
    $(CRT_INC_PATH)\iosfwd \
    $(CRT_INC_PATH)\iostream \
    $(CRT_INC_PATH)\istream \
    $(CRT_INC_PATH)\iterator

STDHPP_FILES2= \
    $(CRT_INC_PATH)\limits \
    $(CRT_INC_PATH)\list \
    $(CRT_INC_PATH)\locale \
    $(CRT_INC_PATH)\map \
    $(CRT_INC_PATH)\memory \
    $(CRT_INC_PATH)\new \
    $(CRT_INC_PATH)\numeric \
    $(CRT_INC_PATH)\ostream \
    $(CRT_INC_PATH)\queue \
    $(CRT_INC_PATH)\set \
    $(CRT_INC_PATH)\sstream \
    $(CRT_INC_PATH)\stack \
    $(CRT_INC_PATH)\stdexcept \
    $(CRT_INC_PATH)\streambuf \
    $(CRT_INC_PATH)\string \
    $(CRT_INC_PATH)\strstream \
    $(CRT_INC_PATH)\typeinfo \
    $(CRT_INC_PATH)\utility \
    $(CRT_INC_PATH)\valarray \
    $(CRT_INC_PATH)\vector \
    $(CRT_INC_PATH)\xcomplex \
    $(CRT_INC_PATH)\xiosbase \
    $(CRT_INC_PATH)\xlocale \
    $(CRT_INC_PATH)\xlocinfo \
    $(CRT_INC_PATH)\xlocmon \
    $(CRT_INC_PATH)\xlocnum \
    $(CRT_INC_PATH)\xloctime \
    $(CRT_INC_PATH)\xmemory \
    $(CRT_INC_PATH)\xstddef \
    $(CRT_INC_PATH)\xstring \
    $(CRT_INC_PATH)\xtree \
    $(CRT_INC_PATH)\xutility

NTTARGETFILE0= $(CRT_FILES1) $(CRT_FILES2) $(STDHPP_FILES1) $(STDHPP_FILES2)
