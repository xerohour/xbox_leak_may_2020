!include ..\..\..\..\sources.inc

TARGETNAME=misorbit
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_morbit.lib

SOURCES=\
    include.cpp
