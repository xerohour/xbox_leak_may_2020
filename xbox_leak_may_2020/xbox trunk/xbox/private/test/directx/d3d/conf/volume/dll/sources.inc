!include ..\..\..\..\sources.inc

TARGETNAME=volume
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_volume.lib

SOURCES=\
    include.cpp
