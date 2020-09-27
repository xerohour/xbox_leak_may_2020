!include ..\..\..\..\sources.inc

TARGETNAME=mstage3
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mstag3.lib

SOURCES=\
    include.cpp
