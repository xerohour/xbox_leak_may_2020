!include ..\..\..\..\sources.inc

TARGETNAME=VBIB
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_vbib.lib

SOURCES=\
    include.cpp
