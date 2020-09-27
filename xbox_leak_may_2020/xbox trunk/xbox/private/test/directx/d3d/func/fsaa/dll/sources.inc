!include ..\..\..\..\sources.inc

TARGETNAME=fsaa
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_fsaa.lib

SOURCES=\
    include.cpp
