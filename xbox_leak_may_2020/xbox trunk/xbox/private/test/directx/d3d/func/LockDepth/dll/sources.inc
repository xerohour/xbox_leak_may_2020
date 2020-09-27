!include ..\..\..\..\sources.inc

TARGETNAME=LockDpth
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_lckdep.lib

SOURCES=\
    include.cpp
