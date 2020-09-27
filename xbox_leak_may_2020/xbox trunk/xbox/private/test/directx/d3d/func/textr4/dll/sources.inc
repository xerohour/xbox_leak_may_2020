!include ..\..\..\..\sources.inc

TARGETNAME=textr4
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_textr4.lib

SOURCES=\
    include.cpp
