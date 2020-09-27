!include ..\..\..\..\sources.inc

TARGETNAME=Lighting
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_lightn.lib

SOURCES=\
    include.cpp
