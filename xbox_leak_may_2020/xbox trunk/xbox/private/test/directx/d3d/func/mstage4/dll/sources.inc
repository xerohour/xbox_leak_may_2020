!include ..\..\..\..\sources.inc

TARGETNAME=mstage4
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mstag4.lib

SOURCES=\
    include.cpp
