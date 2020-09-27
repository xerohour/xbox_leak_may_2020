!include ..\..\..\..\sources.inc

TARGETNAME=textr1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_textr1.lib

SOURCES=\
    include.cpp
