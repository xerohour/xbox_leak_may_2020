!include ..\..\..\..\sources.inc

TARGETNAME=textr3
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_textr3.lib

SOURCES=\
    include.cpp
