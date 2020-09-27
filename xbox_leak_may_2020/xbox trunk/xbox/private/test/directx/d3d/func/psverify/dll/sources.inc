!include ..\..\..\..\sources.inc

TARGETNAME=psverify
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_psver.lib

SOURCES=\
    include.cpp
