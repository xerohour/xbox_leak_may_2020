!include ..\..\..\..\sources.inc

TARGETNAME=bees
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_bees.lib

SOURCES=\
    include.cpp
