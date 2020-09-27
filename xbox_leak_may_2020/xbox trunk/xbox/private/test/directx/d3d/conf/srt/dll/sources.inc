!include ..\..\..\..\sources.inc

TARGETNAME=srt
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_srt.lib

SOURCES=\
    include.cpp
