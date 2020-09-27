!include ..\..\..\..\sources.inc

TARGETNAME=mstage5
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mstag5.lib

SOURCES=\
    include.cpp
