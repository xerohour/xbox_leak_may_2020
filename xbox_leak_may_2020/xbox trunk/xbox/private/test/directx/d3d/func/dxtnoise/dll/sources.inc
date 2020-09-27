!include ..\..\..\..\sources.inc

TARGETNAME=dxtnoise
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_dnoise.lib

SOURCES=\
    include.cpp
