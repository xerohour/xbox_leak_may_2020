!include ..\..\..\..\sources.inc

TARGETNAME=clockwrk
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_clwork.lib

SOURCES=\
    include.cpp
