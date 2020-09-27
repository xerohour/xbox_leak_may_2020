!include ..\..\..\..\sources.inc

TARGETNAME=rteapot
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_rtpot.lib

SOURCES=\
    include.cpp
