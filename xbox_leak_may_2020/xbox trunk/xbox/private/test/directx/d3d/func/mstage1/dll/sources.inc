!include ..\..\..\..\sources.inc

TARGETNAME=mstage1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mstag1.lib

SOURCES=\
    include.cpp
