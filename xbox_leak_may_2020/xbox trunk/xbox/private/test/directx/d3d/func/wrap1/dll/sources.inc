!include ..\..\..\..\sources.inc

TARGETNAME=wrap1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_wrap1.lib

SOURCES=\
    include.cpp
