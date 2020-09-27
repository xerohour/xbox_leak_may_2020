!include ..\..\..\..\sources.inc

TARGETNAME=Present
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_presnt.lib

SOURCES=\
    include.cpp
