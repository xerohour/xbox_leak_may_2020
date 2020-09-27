!include ..\..\..\..\sources.inc

TARGETNAME=mirror
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mirror.lib

SOURCES=\
    include.cpp
