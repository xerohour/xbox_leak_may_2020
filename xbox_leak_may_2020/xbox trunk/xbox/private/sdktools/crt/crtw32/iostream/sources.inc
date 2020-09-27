MAJORCOMP=crt
MINORCOMP=iostream

TARGETNAME=iostream

!include ..\..\crt32.inc

C_DEFINES=$(C_DEFINES) -D_CRT_LIBCI

SOURCES= \
    ..\_iostrea.cpp \
    ..\iostrini.cpp \
    ..\cerrinit.cpp \
    ..\cininit.cpp  \
    ..\cloginit.cpp \
    ..\filebuf.cpp  \
    ..\filebuf1.cpp \
    ..\fstream.cpp  \
    ..\ifstream.cpp \
    ..\_ios.cpp     \
    ..\istrchar.cpp \
    ..\istrdbl.cpp  \
    ..\istream.cpp  \
    ..\istream1.cpp \
    ..\istrflt.cpp  \
    ..\istrgdbl.cpp \
    ..\istrget.cpp  \
    ..\istrgetl.cpp \
    ..\istrgint.cpp \
    ..\istrint.cpp  \
    ..\istrldbl.cpp \
    ..\istrlong.cpp \
    ..\istrshrt.cpp \
    ..\istruint.cpp \
    ..\istrulng.cpp \
    ..\istrusht.cpp \
    ..\mtlock.cpp   \
    ..\ofstream.cpp \
    ..\ostrchar.cpp \
    ..\ostrdbl.cpp  \
    ..\ostream.cpp  \
    ..\ostream1.cpp \
    ..\ostrint.cpp  \
    ..\ostrldbl.cpp \
    ..\ostrlong.cpp \
    ..\ostrptr.cpp  \
    ..\ostrput.cpp  \
    ..\ostrshrt.cpp \
    ..\ostruint.cpp \
    ..\ostrulng.cpp \
    ..\ostrusht.cpp \
    ..\stdiostr.cpp \
    ..\streamb.cpp  \
    ..\streamb1.cpp \
    ..\strmbdbp.cpp \
    ..\_strstre.cpp
