!include ..\..\..\..\sources.inc

TARGETNAME=textr5
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_textr5.lib

SOURCES=\
    include.cpp
