!include ..\..\..\..\sources.inc

TARGETNAME=texgenxt
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_tgenxt.lib

SOURCES=\
    include.cpp
