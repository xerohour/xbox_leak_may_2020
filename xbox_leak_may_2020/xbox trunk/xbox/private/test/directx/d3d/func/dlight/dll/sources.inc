!include ..\..\..\..\sources.inc

TARGETNAME=dlight
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_dlight.lib

SOURCES=\
    include.cpp
