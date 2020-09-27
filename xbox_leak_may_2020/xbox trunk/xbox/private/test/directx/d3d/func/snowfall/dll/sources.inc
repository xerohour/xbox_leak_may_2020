!include ..\..\..\..\sources.inc

TARGETNAME=snowfall
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_snow.lib

SOURCES=\
    include.cpp
