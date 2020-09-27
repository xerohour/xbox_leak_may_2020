!include ..\..\..\..\sources.inc

TARGETNAME=textr2
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_textr2.lib

SOURCES=\
    include.cpp
