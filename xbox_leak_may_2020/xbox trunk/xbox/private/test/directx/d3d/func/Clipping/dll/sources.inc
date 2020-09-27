!include ..\..\..\..\sources.inc

TARGETNAME=Clipping
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_clip.lib

SOURCES=\
    include.cpp
