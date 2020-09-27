!include ..\..\..\..\sources.inc

TARGETNAME=mstage2
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mstag2.lib

SOURCES=\
    include.cpp
